#include <unistd.h>

int ft_popen(const char *file, char *const argv[], char type){
	pid_t pid;
	int pipes[2];
	int status;
	
	if (!file || !argv || (argv && !argv[0]))
		return (-1);
	if (pipe(pipes[2]) < 0)
		return (-1);
	pid = fork();
	if (pid < 0){
		close(pipes[0]);
		close(pipes[1]);
		return (-1);
	}
	if (pid == 0){//filho
		if (type == 'r'){
			close(pipes[0]);
			if (dup2(pipes[1], 1) < 0){
				close(pipes[1]);
				exit (-1);
			}
			close(pipes[1]);
		}else{
			close(pipes[1]);
			if (dup2(pipes[0], 0) < 0){
				close(pipes[0]);
				exit (-1);
			}
			close(pipes[0]);
		}
		execvp(file, argv);
		exit(-1);
	}//pai
	if (type == 'w'){
		close(pipes[0]);
		return (pipes[1]);
	}
	//(type == 'r')
	wait(&status);
	close(pipes[1]);
	return (pipes[0]);
}
