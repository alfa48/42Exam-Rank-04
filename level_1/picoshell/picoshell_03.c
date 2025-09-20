#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>



int picoshell(char **cmds[]){
	int rtn = 0;
	int i = -1;
	int pipes[2];
	pid_t pid;
	int in_fd = 0;
	int status;
	
	if (!cmds || !cmds[0])
		return (1);
	while (cmds[++i])
	{
		if (cmds[i + 1]){
			if (pipe(pipes) < 0){
				if (in_fd != 0)
					close(in_fd);
				return (1);
			}
		}else{
			pipes[0] = -1;
			pipes[1] = -1;
		}
		pid = fork();
		if (pid < 0){
			if (pipes[0] != -1)
				close (pipes[0]);
			if (pipes[1] != -1)
				close(pipes[1]);
			if (in_fd != 0)
				close(in_fd);
			return (1);
		}
		if (pid == 0){//filho
			if (pipes[1] != -1){// mais cmds afrente, escreve algo pra ele
				if (dup2(pipes[1], 1) < 0){
					close(pipes[1]);
					close(pipes[0]);
					exit (1);
				}
				close(pipes[0]);
				close(pipes[1]);
			}
			if (in_fd != 0){// cmd atras deixou algo pra leres
				if (dup2(in_fd, 0) < 0){
					close(in_fd);
					exit (1);
				}
				//close(in_fd);
			}
			execvp(cmds[i][0], cmds[i]);
			exit (1);
		}// pai
		if (pipes[1] != -1)
			close(pipes[1]);
		if (in_fd != 0)
			close(in_fd);
		in_fd = pipes[0];
	}
	
	while (wait(&status) > 0){
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			rtn = 1;
		else if (!WIFEXITED(status))
			return (1);
	}
	return (rtn);
}









































/*MAIN TESTES*/


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
    
    perror("controlo");

    // liberar memória
    for (int i = 0; cmds[i]; i++)
        free(cmds[i]);
    free(cmds);

    return ret;
}
