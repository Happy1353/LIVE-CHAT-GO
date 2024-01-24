#ifndef TREE_H
#define TREE_H

#include "list.h"

typedef struct cmd_inf {
    char ** argv; // список из имени команды и аргументов
    char *infile; // переназначенный файл стандартного ввода
    char *outfile; // переназначенный файл стандартного вывода
    int backgrnd; // =1, если команда подлежит выполнению в фоновом режиме
    int copIn;
    struct cmd_inf* psubcmd; // команды для запуска в дочернем shell
    struct cmd_inf* pipe; // следующая команда после “|”
    struct cmd_inf* next; // следующая после “;” (или после “&”)
}cmd_inf;

void freeCmdInf(cmd_inf *node);

void buildTree();

void printCommand(cmd_inf *cmd);

#endif
