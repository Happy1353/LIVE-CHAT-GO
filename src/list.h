#include <stddef.h>
#ifndef LIST_H
#define LIST_H

typedef struct Node {
    struct Node* next;
    char* word;
}Node;

void addNode(char *word);

void printList();

void printMap();

void deleteList();

#endif
