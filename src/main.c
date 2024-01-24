#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>
#include "list.h"
#include "lexemes.h"
#include "tree.h"
#include "exec.h"

extern cmd_inf* tree;
extern Node* root;
char* homePath;

int run = 1;

void waitchild() {
    wait(NULL);
}

int main(){
    signal(SIGCHLD, waitchild);
    homePath = malloc(PATH_MAX + 1);
    getcwd(homePath, PATH_MAX);
    while (run) {
        printf("> ");
        buildLexemes();
        printList();
        if (run) {
            printMap();
            //buildTree();
            //printCommand(tree);
            //execute_cmd(tree);
            deleteList();
            //freeCmdInf(tree);
        }
    }
    free(homePath);
    return 0;
}