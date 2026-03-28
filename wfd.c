#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include "wfd.h"

#define DEBUG 0

#if DEBUG
    #define dprintf(...) fprintf(stderr, "[DEBUG] " __VA_ARGS__)
#else
    #define dprintf(...)
#endif

// creating empty WFD linked list

WFD *wfd_create() {
    WFD *wfd = malloc(sizeof(WFD));
    wfd->head = NULL;
    wfd->total_words = 0;
    return wfd;
}

// inserting a word to a sorted linked list (lexicographically)

void wfd_insert(WFD *wfd, const char *word) {
    wfd->total_words++;

    // search for existing node or find insertion point
    WordNode *prev = NULL;
    WordNode *curr = wfd->head;

    while (curr != NULL) {
        int cmp = strcmp(curr->word, word);

        if (cmp == 0) {
            // word already exists, so just increment
            curr->count++;
            dprintf("incremented '%s' to %d\n", word, curr->count);
            return;
        }

        if (cmp > 0) {
            // curr->word comes after word alphabetically
            // insert new node before curr
            break;
        }

        prev = curr;
        curr = curr->next;
    }

    // create new node
    WordNode *node = malloc(sizeof(WordNode));
    node->word = strdup(word);
    node->count = 1;
    node->frequency = 0.0;
    node->next = curr;

    if (prev == NULL) {
        wfd->head = node;  // insert at head
    } else {
        prev->next = node; // insert after prev
    }

    dprintf("inserted '%s'\n", word);
}

// get frequencies = (word count)/(total words)

void wfd_normalize(WFD *wfd) {
    WordNode *curr = wfd->head;
    while (curr != NULL) {
        curr->frequency = (double)curr->count / wfd->total_words;
        dprintf("'%s' freq = %d / %d = %.4f\n",
                curr->word, curr->count, wfd->total_words, curr->frequency);
        curr = curr->next;
    }
}

// have a buffer (of considerable size) and insert all words in them

static void process_buffer(WFD *wfd, char *buf, int len) {
    // word characters: letters, digits, dashes
    // we build words character by character
    char word[4096];  // temp buffer
    int wlen = 0;

    for (int i = 0; i < len; i++) {
        char c = buf[i];

        if (isalpha((unsigned char)c) || isdigit((unsigned char)c) || c == '-') {
            // convert to lowercase
            word[wlen++] = tolower((unsigned char)c);
        } else {
            // if a word is built, then insert it
            if (wlen > 0) {
                word[wlen] = '\0';
                wfd_insert(wfd, word);
                wlen = 0;
            }
        }
    }

    // handle word at end of buffer with no trailing separator
    if (wlen > 0) {
        word[wlen] = '\0';
        wfd_insert(wfd, word);
    }
}

// building a WFD from a file

WFD *wfd_build(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror(path);
        return NULL;
    }

    dprintf("building WFD for '%s'\n", path);

    WFD *wfd = wfd_create();

    char buf[4096];
    int bytes_read;

    while ((bytes_read = read(fd, buf, sizeof(buf))) > 0) {
        process_buffer(wfd, buf, bytes_read);
    }

    if (bytes_read == -1) {
        perror(path);
    }

    close(fd);

    dprintf("total words in '%s': %d\n", path, wfd->total_words);

    wfd_normalize(wfd);

    return wfd;
}

// free all memory

void wfd_free(WFD *wfd) {
    WordNode *curr = wfd->head;
    while (curr != NULL) {
        WordNode *next = curr->next;
        free(curr->word);
        free(curr);
        curr = next;
    }
    free(wfd);
}

// debug: printing WFD

void wfd_print(WFD *wfd) {
    WordNode *curr = wfd->head;
    while (curr != NULL) {
        printf("  %-20s count: %3d  freq: %.4f\n",
               curr->word, curr->count, curr->frequency);
        curr = curr->next;
    }
}
