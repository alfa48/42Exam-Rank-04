#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>


int picoshell(char **cmds[]){
	if (!cmds || !cmds[0])
		return (1);
	int count = 0;
	while (cmds[count])
		count++;
	if (count == 1){
		pid_t pid = fork();
		if (pid < 0)
			return (1);
		if (pid == 0){
			execvp(cmds[0][0], cmds[0]);
			exit (1);
		}
		//pai
		int status;
		wait(&status);
		return (0);
	}
	pid_t pids[count];
	int pipes[count - 1][2];
	for (int i = 0; i < count - 1; i++){//pipes
		if (pipe(pipes[i]) < 0){
			for(int j = 0; j < i; j++){
				close(pipes[j][0]);
				close(pipes[j][1]);
			}
			return (1);
		}
	}
	for(int i = 0; i < count; i++){// forks
	
		pids[i] = fork();
		if (pids[i] < 0){
			for(int j = 0; j < count - 1; j++){
				close(pipes[j][0]);
				close(pipes[j][1]);
			}
			return (1);
		}
		if (pids[i] == 0){//filho
			if (i > 0){
				dup2(pipes[i - 1][0], 0);
			}
			if (i != count - 1){
				dup2(pipes[i][1], 1);
			}
			//cade o for para fechar os pipes(fd's)
			execvp(cmds[i][0], cmds[i]);
			exit (1);		
		}
	}
	for(int j = 0; j < count - 1; j++){
		close(pipes[j][0]);
		close(pipes[j][1]);
	}
	//pai
	int status;
	wait(&status);
	return (0);
}


/* Assignment name:    picoshell
Expected files:        picoshell.c
Allowed functions:    close, fork, wait, exit, execvp, dup2, pipe
___

Write the following function:

int    picoshell(char *cmds[]);

The goal of this function is to execute a pipeline. It must execute each
commands [sic] of cmds and connect the output of one to the input of the
next command (just like a shell).

Cmds contains a null-terminated list of valid commands. Each rows [sic]
of cmds are an argv array directly usable for a call to execvp. The first
arguments [sic] of each command is the command name or path and can be passed
directly as the first argument of execvp.

If any error occur [sic], The function must return 1 (you must of course
close all the open fds before). otherwise the function must wait all child
processes and return 0. You will find in this directory a file main.c which
contain [sic] something to help you test your function.


Examples: 
./picoshell /bin/ls "|" /usr/bin/grep picoshell
picoshell
./picoshell echo 'squalala' "|" cat "|" sed 's/a/b/g'
squblblb/
*/

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int picoshell(char **cmds[])
{
    int i = 0;
    int fd[2];
    int in_fd = 0;   // stdin par défaut (0)
    int ret = 0;
    pid_t pid;
    int status;
    
    
	if (!cmds || !cmds[0])
		return 1;

    while (cmds[i])
    {
        if (cmds[i + 1]) // Si ce n’est pas la dernière commande
        {
            if (pipe(fd) == -1){
                perror("pipe");
                return 1;
            }
        }
        else
        {
            fd[0] = -1;
            fd[1] = -1;
        }

        pid = fork();
        if (pid < 0)
        {
            if (fd[0] != -1)
                close(fd[0]);
            if (fd[1] != -1)
                close(fd[1]);
            if (in_fd != 0)
                close(in_fd);
            perror("fork");
            return 1;
        }
        if (pid == 0) // Enfant
        {
            if (in_fd != 0)
            {
                if (dup2(in_fd, 0) == -1)
                    exit(1);
                close(in_fd);
            }
            if (fd[1] != -1) // si ce n'est pas le dernier
            {
                if (dup2(fd[1], 1) == -1)
                    exit(1);
                close(fd[1]);
                close(fd[0]);
            }
            execvp(cmds[i][0], cmds[i]);
            exit(1); // execvp échoué
        }
        else // Parent
        {
            if (in_fd != 0)
                close(in_fd);
            if (fd[1] != -1)
                close(fd[1]);
            in_fd = fd[0];
            i++;
        }
    }
	
    while (wait(&status) > 0)
    {
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            ret = 1;
        else if (!WIFEXITED(status))
            ret = 1;
    }

    return ret;
    
}


























static int count_cmds(int argc, char **argv)
{
    int count = 1;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
            count++;
    }
    return count;
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return (fprintf(stderr, "Usage: %s cmd1 [args] | cmd2 [args] ...\n", argv[0]), 1);

    int cmd_count = count_cmds(argc, argv);
    char ***cmds = calloc(cmd_count + 1, sizeof(char **));
    if (!cmds)
        return (perror("calloc"), 1);

    int i = 1, j = 0;
    while (i < argc)
    {
        int len = 0;
        while (i + len < argc && strcmp(argv[i + len], "|") != 0)
            len++;

        cmds[j] = calloc(len + 1, sizeof(char *));
        if (!cmds[j])
            return (perror("calloc"), 1);

        for (int k = 0; k < len; k++)
            cmds[j][k] = argv[i + k];
        cmds[j][len] = NULL;

        i += len;
        if (i < argc && strcmp(argv[i], "|") == 0)
            i++; // pular o "|"
        j++;
    }
    cmds[cmd_count] = NULL;

    int ret = picoshell(cmds);

    // liberar memória
    for (int i = 0; cmds[i]; i++)
        free(cmds[i]);
    free(cmds);

    return ret;
}





















/*

ulimit -u 1024 ./a.out ls "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e "|" cat -e

*/
