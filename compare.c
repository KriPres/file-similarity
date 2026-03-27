#include <stdio.h>
#include <stdlib.h>
#include "fileutil.h"

#define SUFFIX ".txt"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file|dir> ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    FileList files;
    filelist_init(&files);

    collect_files(argc, argv, &files, SUFFIX);

    if (files.count < 2) {
        fprintf(stderr, "Error: need at least 2 files to compare\n");
        filelist_free(&files);
        return EXIT_FAILURE;
    }

    // TODO: build WFDs (wfd.c)
    // TODO: compute JSDs and sort (jsd.c)
    // TODO: print results

    filelist_free(&files);
    return EXIT_SUCCESS;
}