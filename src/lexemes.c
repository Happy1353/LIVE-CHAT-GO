#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#define SIZE 16
#define N 10

extern int run;
char* buf;
char str[11];
char c;
int curbuf = 0;
int sizebuf = 0;
int curstr = 0;
int deleteWord = 0;

void nullbuf(){
    buf=NULL;
    sizebuf=0;
    curbuf=0;
}

void addsym(){
    if (curbuf>sizebuf-1){
        buf=realloc(buf, sizebuf+=SIZE);
    }
    buf[curbuf++]=c;
}

int symset(int c){
    return c != '\n' &&
    c != ' ' &&
    c != '\t' &&
    c != '$' &&
    c != '>' &&
    c != ';' &&
    c != '<' &&
    c != '|' &&
    c != '&' &&
    c != '(' &&
    c != ')' &&
    c != '\0' &&
    c != '#' &&
    c != EOF ;
}

void addword(){
    if (curbuf>sizebuf-1){
        buf=realloc(buf, sizebuf+=1);
    }
    buf[curbuf++]='\0';
    buf = realloc(buf , sizebuf=curbuf);
    addNode(buf);
    free(buf);
}

void buildLexemes(){
    Node *node = NULL;
    while(1){
        if (c == '\n') {
            c = '\0';
            nullbuf();
            break;
        } else if (c == '!') {
            nullbuf();
            run = 0;
            break;
        }else if(c == ' ' || c == '\t'){
            c = getchar();
        }else if(c == '\0'){
            c = getchar();
        }else if(c == '<'){
            nullbuf();
            addsym();
            c = getchar();
            if(c != '<'){
                addword();
                continue;
            }else{
                addsym();
                addword();
                c = getchar();
                continue;
            }
        }else if(c == ';'){
            nullbuf();
            addsym();
            addword();
            c = getchar();
            continue;
        }else if(c == '$'){
            nullbuf();
            addsym();
            addword();
            c = getchar();
            continue;
        }
        else if(c == '#'){
            nullbuf();
            addsym();
            addword();
            c = getchar();
            continue;
        }else if(c == '('){
            nullbuf();
            addsym();
            addword();
            c = getchar();
            continue;
        }else if(c == ')'){
            nullbuf();
            addsym();
            addword();
            c = getchar();
            continue;
        }else if(c == '|'){
            nullbuf();
            addsym();
            c = getchar();
            if(c != '|'){
                addword();
                continue;
            }else{
                addsym();
                addword();
                c = getchar();
                continue;
            }
        }else if(c == '>'){
            nullbuf();
            addsym();
            c = getchar();
            if(c != '>'){
                addword();
                continue;
            }else{
                addsym();
                addword();
                c = getchar();
                continue;
            }
        }else if(c == '&'){
            nullbuf();
            addsym();
            c = getchar();
            if(c != '&'){
                addword();
                continue;
            }else{
                addsym();
                addword();
                c = getchar();
                continue;
            }
        }else{
            nullbuf();
            addsym();
            c = getchar();
            while(symset(c)){
                addsym();
                c = getchar();
            }
            addword();
            continue;
        }
    }
    if (c == EOF) {
        run = 0;
    }
}
