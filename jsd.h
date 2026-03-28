#ifndef JSD_H
#define JSD_H

#include "wfd.h"

typedef struct {
    char *path1;
    char *path2;
    int combined_words;
    double jsd;
} Comparison;

double compute_jsd(WFD *wfd1, WFD *wfd2);
int compare_fn(const void *a, const void *b);

#endif
