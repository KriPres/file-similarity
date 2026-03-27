#ifndef FILEUTIL_H
#define FILEUTIL_H

typedef struct {
    char **paths;   
    int count;
    int capacity;
} FileList;

void filelist_init(FileList *list);
void filelist_add(FileList *list, const char *path);
void filelist_free(FileList *list);

int has_suffix(const char *name, const char *suffix);
void traverse_directory(char *path, const char *suffix, FileList *list);
void collect_files(int argc, char *argv[], FileList *list, const char *suffix);

#endif