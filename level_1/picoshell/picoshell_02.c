#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int picoshell(char **cmds[]){

	int i = -1;
	int pipes[2];
	int in_fd = 0;
	pid_t pid;
	int status;
	int rtn = 0;
	
	if (!cmds || !cmds[0])
		return (1);
	while (cmds[++i]){
	
		if (cmds[i + 1]){
			if (pipe(pipes) < 0)
				return (1);
		}
		else {
			pipes[0] = -1;
			pipes[1] = -1;
		}
		pid = fork();
		if (pid < 0){
			if (pipes[0] != -1)
				close(pipes[0]);
			if (pipes[1] != -1)
				close(pipes[1]);
			if (in_fd != 0)
				close(in_fd);
			return (1);
		}
		if (pid == 0){//filho
		
			if (pipes[1] != -1){//não é o ultimo cmd
				if (dup2(pipes[1], 1) < 0)
					exit (1);
				close(pipes[0]);// por que fechar aqui?
				close(pipes[1]);// R: certeza que eles existem
			}
			if (in_fd != 0){//não é o primeiro cmd
				if (dup2(in_fd, 0) < 0)//0 aponta in_fd
						//excvp vai ler do in_fd
					exit(1);
					//por que nao fechar o in_fd?
					//tem a ver com o 0?
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		//api
		if (in_fd != 0)
			close(in_fd);
		if (pipes[1] != -1)
			close(pipes[1]);
		in_fd = pipes[0];
	}
		
		while (wait(&status) > 0){
			if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
				rtn = 1;
			else if (!WIFEXITED(status))
				rtn = 1;
		}
		
		return (rtn);		
}
