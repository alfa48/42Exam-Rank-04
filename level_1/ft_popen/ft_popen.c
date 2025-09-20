#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>


int ft_popen(const char *file, char *const argv[], char type){
	int pipes[2];
	pid_t pid;
	int status;
	
	if (!file || !argv || !argv[0]|| (type != 'r' && type != 'w'))
		return (-1);
	if (pipe(pipes) < 0)
		return (-1);
	pid = fork();
	if (pid < 0)
		return (-1);
	if (pid == 0){// filho
		if (type == 'r'){
			if (dup2(pipes[1], 1) < 0){
				close(pipes[0]);
				close(pipes[1]);			
				exit (-1);
			}
			close(pipes[0]);
			close(pipes[1]);
		}else{
			if (dup2(pipes[0], 0) < 0){
				close(pipes[0]);
				close(pipes[1]);			
				exit (-1);
			}
			close(pipes[0]);
			close(pipes[1]);
		}
		execvp (file, argv);
		exit (-1);
	}
	//pai
	//wait(&status);	
	if (type == 'r'){
		close(pipes[1]);
		return (pipes[0]);
		
	}
	close(pipes[0]);
	return (pipes[1]);
}










































/*escrever*/


int main(void)
{
    int fd = ft_popen("cat", (char *const[]){"cat", "-e", NULL}, 'w');
    if (fd < 0) {
        perror("ft_popen");
        return 1;
    }

    write(fd, "hello world\n", 12);

    close(fd);
    return 0;
}



/*ler*/
/*
int main(int argc, char *argv[], char *envp[])
{
    int fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');

    char ch[1];
    while (read(fd, &ch, 1) > 0)
        write(0, &ch, 1);
}
*/
