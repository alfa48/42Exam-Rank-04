#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>



void signal_hendler(int sig){
	(void)sig;//so ignora
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose){

	pid_t pid;
	int status;

	if (!f || timeout <= 0)
		return (-1);
	pid = fork();
	if (pid < 0){
		return (-1);
	}
	if (pid == 0){//filho
		f();
		exit (0);
	}//pai
	//config
	struct sigaction sa;
	sa.sa_handler = signal_hendler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGALRM, &sa, NULL) < 0)
		return (-1);
	alarm(timeout);
	if (waitpid(pid, &status, 0) == -1){
		if (errno == EINTR){
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			if (verbose)
				printf("Bad function: timed out after %d seconds\n", timeout);
				return (0);
		}
		return (-1);
	}
	if (WIFEXITED(status)){
		if (WEXITSTATUS(status) == 0){
			if (verbose)
				printf("Nice function!\n");			
			return (1);
		}
		if (verbose)
			printf("Bad function: exited with code %d\n", WEXITSTATUS(status));
		return (0);
	}
	
	if (WIFSIGNALED(status)){
		if (verbose)
			printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
		return (0);
	}
	return (-1);

}


void f_bad(void){
	int *c = NULL;
	*c = 14; 
}

void f_good(void){
	int a = 1;
}

int main(void){
	sandbox(f_bad, 2, true);
	perror("monitorando:");
	return (0);
}
