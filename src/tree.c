#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "tree.h"
#include "lexemes.h"

cmd_inf* tree = NULL;
extern Node* root;

Node* addArg(Node* curLexeme, cmd_inf* node){
    int k = 0;
    Node* temp = curLexeme;
    while (temp != NULL && strstr("; & || && > < << >> ( )", temp->word) == NULL) {
        k++;
        temp = temp->next;
    }

    if(k > 0){
         node->argv = malloc((k + 1) * sizeof(char*));

        for (int i = 0; i < k; i++) {
            if(strcmp(curLexeme->word, "$") == 0){
                char *env = getenv(curLexeme->next->word);
                node->argv[i] = env;
                curLexeme = curLexeme->next->next;
                k--;
            }else{
                node->argv[i] = curLexeme->word;
                curLexeme = curLexeme->next;
            }
        }
        node->argv[k] = NULL;
        node->next = NULL;
        node->pipe = NULL;
        node->infile = NULL;
        node->outfile = NULL;
        node->backgrnd = 0;
        node->copIn = 0;
    }
   

    return curLexeme;
}

void freeCmdInf(cmd_inf *node) {
    if (node == NULL) {
        return;
    }

    if (node->argv != NULL) {
        free(node->argv);
    }

    if (node->outfile != NULL) {
        free(node->outfile);
    }

    if (node->infile != NULL) {
        free(node->infile);
    }

    freeCmdInf(node->pipe);
    freeCmdInf(node->next);

    free(node);
}

void printCommand(cmd_inf *cmd) {
    if (cmd == NULL) {
        return;
    }

    for (int i = 0; cmd->argv[i] != NULL; i++) {
        printf("Command: %s \n", cmd->argv[i]);
    }

    if (cmd->infile != NULL) {
        printf("Input file: %s\n", cmd->infile);
    }

    if (cmd->outfile != NULL) {
        printf("Output file: %s\n", cmd->outfile);
    }

    if (cmd->backgrnd) {
        printf("Background command\n");
    }

    printCommand(cmd->next);
    printCommand(cmd->pipe);
}


void buildTree() {
    if(root == NULL){
        return;
    }
    cmd_inf *rootTree = malloc(sizeof(cmd_inf));
    tree = rootTree;
    Node *curLexeme = root;
    rootTree->next = NULL;
    rootTree->pipe = NULL;
    rootTree->infile = NULL;
    rootTree->outfile = NULL;
    rootTree->backgrnd = 0;
    rootTree->copIn = 0;
    while(curLexeme != NULL){
        if(rootTree->backgrnd == 1){
            cmd_inf *nextNode = malloc(sizeof(cmd_inf));
            rootTree->next = nextNode;
            rootTree = nextNode;
        }
        curLexeme = addArg(curLexeme, rootTree);
        if(curLexeme != NULL){
            if(strcmp(curLexeme->word, ";") == 0){
                cmd_inf *nextNode = malloc(sizeof(cmd_inf));
                nextNode->backgrnd = 0;
                rootTree->next = nextNode;
                rootTree = nextNode;
            }
            if(strcmp(curLexeme->word, "&") == 0){
                rootTree->backgrnd = 1;
            }
            if(strcmp(curLexeme->word, "|") == 0){
                cmd_inf *nextNode = malloc(sizeof(cmd_inf));
                nextNode->backgrnd = 0;
                rootTree->pipe = nextNode;
                rootTree = nextNode;
            }
            if(strcmp(curLexeme->word, ">") == 0 || strcmp(curLexeme->word, ">>") == 0){
                if(strcmp(curLexeme->word, ">>") == 0){
                    rootTree->copIn = 1;
                }
                rootTree->outfile = strdup(curLexeme->next->word);
                curLexeme = curLexeme->next;
            }else if(strcmp(curLexeme->word, "<") == 0 || strcmp(curLexeme->word, "<<") == 0){
                rootTree->infile = strdup(curLexeme->next->word);
                curLexeme = curLexeme->next;
            }
            if(curLexeme != NULL){
                curLexeme = curLexeme->next;
            }
        }
    }
}
