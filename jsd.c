#include <math.h>
#include <stdlib.h>
#include "jsd.h"
#include <string.h>

// sort comparisons by combined word count (descending)
int compare_fn(const void *a, const void *b) {
    Comparison *ca = (Comparison *)a;
    Comparison *cb = (Comparison *)b;
    return cb->combined_words - ca->combined_words;
}

double compute_jsd(WFD *wfd1, WFD *wfd2) {
    double kld1 = 0.0;
    double kld2 = 0.0;

    WordNode *n1 = wfd1->head;
    WordNode *n2 = wfd2->head;

    // simultaneous iteration over both sorted lists
    while (n1 != NULL || n2 != NULL) {
        double f1 = 0.0, f2 = 0.0;

        int cmp;
        
        // f1 exhausted
        if (n1 == NULL) {
            cmp =  1;
        }

        // f2 exhausted
        else if (n2 == NULL) {
            cmp = -1;
        }

        else {
            cmp = strcmp(n1->word, n2->word);
        }

        if (cmp == 0) {
            // word in both files
            f1 = n1->frequency;
            f2 = n2->frequency;
            n1 = n1->next;
            n2 = n2->next;
        } else if (cmp < 0) {
            // word only in file 1
            f1 = n1->frequency;
            f2 = 0.0;
            n1 = n1->next;
        } else {
            // word only in file 2
            f1 = 0.0;
            f2 = n2->frequency;
            n2 = n2->next;
        }

        // mean frequency
        double mean = (f1 + f2) / 2.0;

        // add to KLD totals (only when frequency > 0)
        if (f1 > 0.0) kld1 += f1 * log2(f1 / mean);
        if (f2 > 0.0) kld2 += f2 * log2(f2 / mean);
    }

    return sqrt((0.5 * kld1) + (0.5 * kld2));
}
