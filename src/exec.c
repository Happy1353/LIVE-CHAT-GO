#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include "exec.h"

extern char* homePath;

void outfile(cmd_inf *cmd){
    int file;
    if(cmd->copIn){
        file = open(cmd->outfile, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    }else{
        file = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }
    
    if (file == -1) {
        perror("open");
        exit(1);
    }

    if (dup2(file, STDOUT_FILENO) == -1) {
        perror("dup2");
        close(file);
        exit(1);
    }

    close(file);
}

void infile(cmd_inf *cmd){
   int file = open(cmd->infile, O_RDONLY);
    if (file == -1) {
        perror("open");
        exit(1);
    }

    if (dup2(file, STDIN_FILENO) == -1) {
        perror("dup2");
        close(file);
        exit(1);
    }

    close(file);
}
int cd(int argc, char **argv) {
    if (argc == 1) {
        if (chdir(homePath) != 0) {
            perror("chdir");
            return 3;
        }
    } else if (argc > 2) {
        fprintf(stderr, "cd command accepts only 1 argument\n");
        return 2;
    } else if (chdir(argv[1]) != 0) {
        perror("chdir");
        return 3;
    }
    return 0;
}

void pipe_cmd(cmd_inf *cmd) {
    int fd[2], in, out, next_in, i;
    int pid_arr[100];
    pipe(fd);
    out = fd[1];
    next_in = fd[0];
    if ((pid_arr[0] = fork()) == 0) {
        close(next_in);
        dup2(out, STDOUT_FILENO);
        close(out);
        if (cmd->infile) {
            int file = open(cmd->infile, O_RDONLY);
            if (file == -1) {
                perror("open");
                exit(1);
            }
            dup2(file, STDIN_FILENO);
            close(file);
        }
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        exit(-1);
    }

    in = next_in;

    for (i = 2; cmd->pipe != NULL; i++) {
        close(out);
        pipe(fd);
        out = fd[1];
        next_in = fd[0];
        if ((pid_arr[i - 1] = fork()) == 0) {
            close(next_in);
            dup2(in, STDIN_FILENO);
            close(in);
            int file;
            if (cmd->outfile) {
                int file = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (file == -1) {
                    perror("open");
                    exit(1);
                }
                dup2(file, STDOUT_FILENO);
                close(file);
            } else {
                dup2(out, STDOUT_FILENO);
            }
            close(out);
            execvp(cmd->pipe->argv[0], cmd->pipe->argv);
            perror("execvp");
            exit(-1);
        }
        close(in);
        in = next_in;
        cmd = cmd->pipe;
    }

    close(out);

    if ((pid_arr[i - 1] = fork()) == 0) {
        dup2(in, STDIN_FILENO);
        close(in);
        if (cmd->outfile) {
            int file;
            if(cmd->copIn){
                file = open(cmd->outfile, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
            }else{
                file = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            }
            if (file == -1) {
                perror("open");
                exit(1);
            }
            dup2(file, STDOUT_FILENO);
            close(file);
        }
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        exit(-1);
    }

    close(in);

    for (int k = 0; k < i; k++) {
        waitpid(pid_arr[k], NULL, 0);
    }
}


void next(cmd_inf *cmd) {
    if (fork() == 0) {
        if(cmd->outfile){
            outfile(cmd);
        }
        if(cmd->infile){
            infile(cmd);
        }
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        exit(0);
    } else {
        wait(NULL);
    }
    execute_cmd(cmd->next);
}

void next_background(cmd_inf *cmd) {
    pid_t pid;
    if ((pid = fork()) == 0) {
        if(cmd->outfile){
            outfile(cmd);
        }
        if(cmd->infile){
            infile(cmd);
        }
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        exit(1);
    } else {
        waitpid(pid, NULL, 0);
        printf("PID %d завершился\n", pid);
        exit(0);
    }
}

void execute_cmd(cmd_inf *cmd) {
    if (cmd == NULL) {
        return;
    }

    if (cmd->backgrnd)
        signal(SIGINT, SIG_IGN);
    else
        signal(SIGINT, SIG_DFL);

    pid_t pid;


    if (strcmp(cmd->argv[0], "cd") == 0) {
        int count = 0;
        while (cmd->argv[count] != NULL) {
            count++;
        }
        cd(count, cmd->argv);
        execute_cmd(cmd->next);
    }else{
        if (cmd->backgrnd == 1) {
            if ((pid = fork()) == 0) {
                
                if (cmd->pipe) {
                    pipe_cmd(cmd);
                } else {
                    next_background(cmd);
                } 
            } else {
                execute_cmd(cmd->next);
            }
        } else {
            if (cmd->pipe) {
                pipe_cmd(cmd);
            } else {
                next(cmd);
            }
        }
    }

}
