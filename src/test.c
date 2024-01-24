#include "../include/execute.h"

void addzombie(pid_t pid)
{
	zombie_t *znode = malloc(sizeof(zombie_t));
	znode->pid = pid;
	znode->next = zhead;
	zhead = znode;
}

void execute(cmdinf_t *c);

void make_process(cmdinf_t *t, bool background, pid_t pgid)
{
    if (background)
        signal(SIGINT, SIG_IGN);
    else
        signal(SIGINT, SIG_DFL);
    if (t->infile != NULL)
    {
        int f = open(t->infile, O_RDONLY);
        if (f == -1)
        {
            fprintf(stderr, "%s: no such file\n", t->infile);
            longjmp(repeat_jmp_buf, 1);
        }
        else
            dup2(f, 0);
        close(f);
    }
    if (t->outfile != NULL)
    {
        int f;
        if (t->append)
            f = open(t->outfile, O_WRONLY|O_APPEND|O_CREAT, 0664);
        else
            f = open(t->outfile, O_WRONLY|O_TRUNC|O_CREAT, 0664);
        if (f == -1)
        {
            fprintf(stderr, "%s: can't redirect output to file\n", t->outfile);
            longjmp(repeat_jmp_buf, 1);
        }
        else
            dup2(f, 1);
        close(f);
    }
    if (background)
    {
        setpgid(0, pgid);                       // PGID mustn't be the same with the shell
        int f = open("/dev/null", O_RDONLY);    // instant EOF guaranteed
        dup2(f, 0);
    }
    if (t->argv != NULL)        // handle built-ins
    {
        builtin_list_node_t *it;
        for (it = builtins.begin; it != NULL; it = it->next)
            if (!strcmp(t->argv[0], it->name))
                it->fptr(t->argc, t->argv);
    }
    if (t->argv == NULL)            // case of (...)
    {
        execute(t->psubcmd);
        exit(0);
    }
    else                            // simple command
        execvp(t->argv[0], t->argv);
    fprintf(stderr, "%s: command not found\n", t->argv[0]);
    exit(1);
}

int execute_pipe(cmdinf_t *exec_info)
{
    if (exec_info == NULL)
        return 0;
	cmdinf_t *now = exec_info;
	pid_t pid, pgid;
    int fd_temp, fd[2], laststat = 0;
    fd_temp = -1;
    bool is_fork = false;
    if (exec_info->background)
    {
        if (getpgrp() != getpid())      // determine if it's a forked process
        {
            setpgid(0, 0);              // "create" new process group
            is_fork = true;
        }
        else if ((pid = fork()) == 0)   // new process for background pipe
            execute_pipe(exec_info);
        else
            return 0;
    }
    pgid = getpgrp();
	while (now != NULL)
	{
        if (now->argv && !strcmp(now->argv[0], "exit"))     // exit built-in
            exit(0);
        if (now->argv && !strcmp(now->argv[0], "cd"))       // cd built-in
        {
            if (fd_temp != -1)
                close(fd_temp);
            if (now->pipe != NULL)
            {
                if (pipe(fd) < 0)
                    exit(1);
                fd_temp = fd[0];
                close(fd[1]);
            }
            return cd(now->argc, now->argv);
        }
		if (now == exec_info)       // first command in pipe
        {
            if (now->pipe != NULL)  // ...and not last
            {
                if (pipe(fd) < 0)
                    exit(1);
                if ((pid = fork()) == 0)
                {
                    dup2(fd[1], 1);
                    close(fd[0]);
                    close(fd[1]);
                    make_process(now, exec_info->background, pgid);
                }
                else if (exec_info->background)
                    addzombie(pid);
                fd_temp = fd[0];
                close(fd[1]);
            }
            else if ((pid = fork()) == 0)   // only one element in a pipe
                make_process(now, exec_info->background, pgid);
            else if (exec_info->background)
                addzombie(pid);
        }
        else if (now->pipe == NULL)         // last process
        {
            if ((pid = fork()) == 0)
            {
                dup2(fd_temp, 0);
                close(fd_temp);
                make_process(now, exec_info->background, pgid);
            }
            else if (exec_info->background)
                addzombie(pid);
            close(fd_temp);
        }
        else
        {
            if (pipe(fd) < 0)
                exit(1);
            if ((pid = fork()) == 0)
            {
                dup2(fd_temp, 0);
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
                close(fd_temp);
               	make_process(now, exec_info->background, pgid);
            }
            else if (exec_info->background)
                addzombie(pid);
            close(fd[1]);
            close(fd_temp);
            fd_temp = fd[0];
        }
		now = now->pipe;
	}
    if (!exec_info->background || is_fork)
    {
    	waitpid(pid, &laststat, 0);
    	while (wait(NULL) != -1);
    }
    if (is_fork)
        exit(laststat);
    return laststat;
}

void execute(cmdinf_t *exec_tree)
{
	int laststat = 0;
	while (exec_tree != NULL)
	{
		if (exec_tree->stat == 1)
        {
            if (laststat)
                laststat = execute_pipe(exec_tree);
        }
		else if (exec_tree->stat == 2)
        {
            if (!laststat)
                laststat = execute_pipe(exec_tree);
        }
		else
            laststat = execute_pipe(exec_tree);
		exec_tree = exec_tree->next;
	}
	return;
}

void kill_zombie(void)
{
	int status;
	while (zhead != NULL)
	{
		waitpid(zhead->pid, &status, 0);
		printf("[%ld] done\n", (long)zhead->pid);
		zombie_t * temp = zhead;
		zhead = zhead->next;
		free(temp);
	}
	return;
}
