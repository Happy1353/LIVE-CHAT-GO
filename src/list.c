#include "list.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Node* root = NULL;

void addNode(char *word) {
    Node *newNode = malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    newNode->word = strdup(word);
    newNode->next = NULL;

    if (root == NULL) {
        root = newNode;
    } else {
        Node *current = root;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void printList() {
    Node *current = root;
    while (current != NULL) {
        if(strcmp(current->word, "#") == 0){
            break;
        }
        printf("%s\n", current->word);
        current = current->next;
    }
}

void printMap(){
    printf("------\n");
    int map[12];
    int k;
    Node *ptr = root;
    for(int i = 0; i < 12; i++){
        map[i] = 0;
    }
    while(ptr != NULL){
        k++;
        if(strcmp(ptr->word, "#") == 0){
            map[0]++;
            break;
        }
        else if(strcmp(ptr->word, "&") == 0){
            map[1]++;
        }else if(strcmp(ptr->word, "&&") == 0){
            map[2]++;
        }else if(strcmp(ptr->word, "|") == 0){
            map[3]++;
        }else if(strcmp(ptr->word, "||") == 0){
            map[4]++;
        }else if(strcmp(ptr->word, "<") == 0){
            map[5]++;
        }else if(strcmp(ptr->word, "<<") == 0){
            map[6]++;
        }else if(strcmp(ptr->word, ">") == 0){
            map[7]++;
        }else if(strcmp(ptr->word, ">>") == 0){
            map[8]++;
        }else if(strcmp(ptr->word, "(") == 0){
            map[9]++;
        }else if(strcmp(ptr->word, ")") == 0){
            map[10]++;
        }else if(strcmp(ptr->word, ";") == 0){
            map[11]++;
        }
        ptr = ptr->next;

    }

    int a1, a1Max, a2, a2Max, a3, a3Max, max = 0, p;
    for(int i = 0; i < 12; i++){
        if(map[i] > max){
            max = map[i];
            p = i;
        }
    }
    a1 = p;
    a1Max = max;
    max = 0;
    for(int i = 0; i < 12; i++){
        if(map[i] > max && i != a1){
            max = map[i];
            p = i;
        }
    }
    a2 = p;
    a2Max = max;
    max = 0;
    for(int i = 0; i < 12; i++){
        if(map[i] > max && i != a1 && i != a2){
            max = map[i];
            p = i;
        }
    }
    a3 = p;
    a3Max = max;
    max = 0;

    if(map[a1] != 0){
        printSym(a1);
        printf(": %d\n", map[a1]);
    }
    if(map[a2] != 0){
        printSym(a2);
        printf(": %d\n", map[a2]);
    }
    if(map[a3] != 0){
        printSym(a3);
        printf(": %d\n", map[a3]);
    }
}

void printSym(int k){
    if(k == 0){
        printf("#");
    }
    if(k == 1){
        printf("&");
    }
    if(k == 2){
        printf("&&");
    }
    if(k == 3){
        printf("|");
    }
    if(k == 4){
        printf("||");
    }
    if(k == 5){
        printf("<");
    }
    if(k == 6){
        printf("<<");
    }
    if(k == 7){
        printf(">");
    }
    if(k == 8){
        printf(">>");
    }
    if(k == 9){
        printf("(");
    }
    if(k == 10){
        printf(")");
    }
    if(k == 11){
        printf(";");
    }
}

void deleteList() {
    Node *current = root;
    Node *nextNode;

    while (current != NULL) {
        nextNode = current->next;
        free(current->word);
        free(current);
        current = nextNode;
    }

    root = NULL;
}
