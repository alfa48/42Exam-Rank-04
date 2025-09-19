#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/*
int ft_popen(const char *file, char *const argv[], char type)
{
    if (!file || !argv || (type != 'r' && type != 'w'))
        return -1;

    int fd[2];
    if (pipe(fd) < 0)
        return -1;

    pid_t pid = fork();
    if (pid < 0)
    {
        close(fd[0]);
        close(fd[1]);
        return -1;
    }

    if (pid == 0)
    {
        if (type == 'r')
        {
            close(fd[0]);
            if (dup2(fd[1], STDOUT_FILENO) < 0)
                exit(-1);
        }
        else
        {
            close(fd[1]);
            if (dup2(fd[0], STDIN_FILENO) < 0)
                exit(-1);
        }
        close(fd[0]);
        close(fd[1]);
        execvp(file, (char *const *)argv);
        exit(-1);
    }

    if (type == 'r')
    {
        close(fd[1]);
        return fd[0];
    }
    close(fd[0]);
    return fd[1];
}
*/

int ft_popen(const char *file, char *const argv[], char type){
    if (!file || !argv || (type != 'r' && type != 'w'))
        return (-1);
    int pipes[2];
    if (pipe(pipes) < 0)
        return (-1);
    pid_t pid = fork();
    if (pid < 0)
        return (-1);
    if (pid == 0){//filho
        if (type == 'r'){
            close(pipes[0]);
            if (dup2(pipes[1], 1) < 0)
                exit (-1);
        }
        else{
            close(pipes[1]);
            if (dup2(pipes[0], 0) < 0)
                exit (-1);
        }
        execvp(file, argv);
        exit (-1);
    }
    //pai
    if (type == 'r'){
        close(pipes[1]);
        return (pipes[0]);
    }
        close(pipes[0]);
        return (pipes[1]);
}


int main(int argc, char *argv[], char *envp[])
{
    int fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');

    char ch[1];
    while (read(fd, &ch, 1) > 0)
        write(0, &ch, 1);
}