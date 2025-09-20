#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
 #include <sys/types.h>

void handler_sig(int sig){
	(void)sig;//nathing
}
int sandbox(void (*f)(void), unsigned int timeout, bool verbose){
	
	pid_t pid;
	int status;
	
	if (!f || timeout <= 0)
		return (-1);
	pid = fork();
	if (pid < 0)
		return (-1);
	if (pid == 0){//filho
		f();
		exit (0);
	}//pai
	//config signal
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handler_sig;
	sa.sa_flags = 0;
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
	*c = 43; 
}

void f_good(void){
	int a = 1;
}

int main(void){
	sandbox(f_bad, 2, true);
	perror("monitorando:");
	return (0);
}
