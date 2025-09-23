#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>



void handle_sig(int sig){
	(void)sig;
	//do nathing
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
	}
	struct sigaction sa;
	sa.sa_handler = handle_sig;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGALRM,&sa, NULL);
	alarm(timeout);
	if (waitpid(pid, &status, 0) == -1){
		if (errno == EINTR){
			kill(pid, SIGKILL);
			if (verbose)
				printf("Bad function: timed out after %d seconds\n", timeout);
			return (0);
		}
		return (-1);
	}
	alarm(0);
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
	return(-1);
}

