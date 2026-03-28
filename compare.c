#include <stdio.h>
#include <stdlib.h>
#include "fileutil.h"
#include "wfd.h"
#include "jsd.h"

#define SUFFIX ".txt"
#define DEBUG 1   // set to 0 to disable debug outputs

#if DEBUG
    #define dprintf(...) fprintf(stderr, "[DEBUG] " __VA_ARGS__)
#else
    #define dprintf(...)
#endif

typedef struct {
    char *path;
    WFD  *wfd;
} FileEntry;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file|dir> ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    // collection phase of files and directories
    FileList files;
    filelist_init(&files);
    collect_files(argc, argv, &files, SUFFIX);

    if (files.count < 2) {
        fprintf(stderr, "Error: need at least 2 files to compare\n");
        filelist_free(&files);
        return EXIT_FAILURE;
    }

    // debug: print appropriate files which were considered
    #if DEBUG
        dprintf("collected %d files:\n", files.count);
        for (int i = 0; i < files.count; i++) {
            dprintf("  [%d] %s\n", i, files.paths[i]);
        }
    #endif

    // calculated WFDs for each file
    FileEntry *entries = malloc(files.count * sizeof(FileEntry));
    for (int i = 0; i < files.count; i++) {
        entries[i].path = files.paths[i];
        entries[i].wfd  = wfd_build(files.paths[i]);
    }

    // Finding all nC2 pairs
    int n = files.count;
    int num_pairs = n * (n - 1) / 2;

    // debug: printing all nC2 pairs
    #if DEBUG
        dprintf("\n%d files → %d pairs (nC2 = %d*%d/2):\n",
                n, num_pairs, n, n - 1);
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                dprintf("  (%d,%d) %s  vs  %s\n",
                        i, j, entries[i].path, entries[j].path);
            }
        }
        dprintf("\n");
    #endif

    Comparison *comparisons = malloc(num_pairs * sizeof(Comparison));
    int idx = 0;

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            comparisons[idx].path1 = entries[i].path;
            comparisons[idx].path2 = entries[j].path;
            comparisons[idx].combined_words = entries[i].wfd->total_words + entries[j].wfd->total_words;
            comparisons[idx].jsd = compute_jsd(entries[i].wfd, entries[j].wfd);
            #if DEBUG
                dprintf("  pair (%s, %s) => combined=%d jsd=%.5f\n", entries[i].path, entries[j].path, comparisons[idx].combined_words, comparisons[idx].jsd);
            #endif
            idx++;
        }
    }

    // sorting by combined word count (descending)
    qsort(comparisons, num_pairs, sizeof(Comparison), compare_fn);

    // printing results
    for (int i = 0; i < num_pairs; i++) {
        printf("%.5f %s %s\n",
               comparisons[i].jsd,
               comparisons[i].path1,
               comparisons[i].path2);
    }

    // cleanup before exit
    for (int i = 0; i < n; i++) {
        wfd_free(entries[i].wfd);
    }
    free(entries);
    free(comparisons);
    filelist_free(&files);

    return EXIT_SUCCESS;
}
