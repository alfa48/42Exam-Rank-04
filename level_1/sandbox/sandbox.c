// sandbox.c
#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <stdbool.h>    // bool
#include <signal.h>     // struct sigaction, sigaction(), strsignal()
#include <unistd.h>     // fork, alarm, _exit
#include <sys/wait.h>   // waitpid
#include <errno.h>      // errno


static volatile sig_atomic_t g_timed_out = 0;

static void alarm_handler(int sig)
{
    (void)sig;
    g_timed_out = 1;
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    struct sigaction sa;
    pid_t pid;
    int status;

    g_timed_out = 0;
    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        return -1;

    pid = fork();
    if (pid < 0)
        return -1;

    if (pid == 0)
    {
        // Filho: executa f
        f();
        _exit(0);
    }

    alarm(timeout);
    if (waitpid(pid, &status, 0) == -1)
    {
        return -1;
    }
    alarm(0); // cancela alarme se terminou antes

    if (g_timed_out)
    {
        // matou por timeout
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0); // garantir que não vire zumbi
        if (verbose)
            printf("Bad function: timed out after %u seconds\n", timeout);
        return 0;
    }

    if (WIFEXITED(status))
    {
        int code = WEXITSTATUS(status);
        if (code == 0)
        {
            if (verbose)
                printf("Nice function!\n");
            return 1;
        }
        else
        {
            if (verbose)
                printf("Bad function: exited with code %d\n", code);
            return 0;
        }
    }
    else if (WIFSIGNALED(status))
    {
        int sig = WTERMSIG(status);
        if (verbose)
            printf("Bad function: %s\n", strsignal(sig));
        return 0;
    }
    return -1; // caso inesperado
}
