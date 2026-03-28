#ifndef WFD_H
#define WFD_H

typedef struct WordNode {
    char *word;
    int count;
    double frequency;
    struct WordNode *next;
} WordNode;

typedef struct {
    WordNode *head;
    int total_words;
} WFD;

WFD *wfd_create();
void wfd_insert(WFD *wfd, const char *word);
void wfd_normalize(WFD *wfd);
WFD *wfd_build(const char *path);
void wfd_free(WFD *wfd);
void wfd_print(WFD *wfd);

#endif
