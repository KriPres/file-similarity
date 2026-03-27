#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "fileutil.h"

void filelist_init(FileList *list) {
    list->count = 0;
    list->capacity = 8;
    list->paths = malloc(list->capacity * sizeof(char *));
}

void filelist_add(FileList *list, const char *path) {
    if (list->count == list->capacity) {
        list->capacity *= 2;
        list->paths = realloc(list->paths, list->capacity * sizeof(char *));
    }
    list->paths[list->count++] = strdup(path);
}

void filelist_free(FileList *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->paths[i]);
    }
    free(list->paths);
    list->count = 0;
    list->capacity = 0;
}

int has_suffix(const char *name, const char *suffix) {
    int name_len = strlen(name);
    int suf_len = strlen(suffix);
    if (suf_len > name_len) return 0;
    return strcmp(name + name_len - suf_len, suffix) == 0;
}

void traverse_directory(char *path, const char *suffix, FileList *list) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror(path);
        return;
    }

    int path_len = strlen(path);
    struct dirent *de;

    while ((de = readdir(dir))) {
        // skips "." and ".." and any hidden files/dirs — prevents infinite recursion too
        if (de->d_name[0] == '.') continue;

        int name_len = strlen(de->d_name);
        char *file_path = malloc(path_len + name_len + 2);
        memcpy(file_path, path, path_len);
        file_path[path_len] = '/';
        memcpy(file_path + path_len + 1, de->d_name, name_len + 1);

        struct stat sb;
        if (stat(file_path, &sb) == -1) {
            perror(file_path);
            free(file_path);
            continue;
        }

        if (S_ISDIR(sb.st_mode)) {
            traverse_directory(file_path, suffix, list);
        } else if (S_ISREG(sb.st_mode)) {
            if (has_suffix(de->d_name, suffix)) {
                filelist_add(list, file_path);
            }
        }

        free(file_path);
    }

    closedir(dir);
}

void collect_files(int argc, char *argv[], FileList *list, const char *suffix) {
    for (int i = 1; i < argc; i++) {
        struct stat sb;
        if (stat(argv[i], &sb) == -1) {
            perror(argv[i]);
            continue;
        }

        if (S_ISDIR(sb.st_mode)) {
            traverse_directory(argv[i], suffix, list);
        } else if (S_ISREG(sb.st_mode)) {
            // explicitly given — no suffix check
            filelist_add(list, argv[i]);
        }
    }
}