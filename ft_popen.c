/* Allowed functions: pipe, fork, dup2, execvp, close, exit

write the following function:

    int    ft_popen(const char file, charconst argv[], char type)

The function must launch the executable file with the arguments argv (using execvp).
If the type is 'r' the function must return a file descriptor connected to the output of the command.
If the type is 'w' the function must return a file descriptor connected to the input of the command.
In case of error or invalid parameter the function must return -1.

example:
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include  <sys/types.h>

int ft_popen(const char *file, char *const av[], int type)
{
    if(!file || !av || (type != 'r' && type !='w' ))
        return -1;

    int fd[2];
    if (pipe(fd) < 0)
        return -1;
    pid_t pid = fork();
    if (pid < 0) {
        close(fd[1]);
        close(fd[0]);
        return -1;
    }
    if(pid == 0) {
        if(type == 'r') {
            close(fd[0]);
            if(dup2(fd[1], STDOUT_FILENO) < 0)
                exit (-1);
        } else {
            close(fd[1]);
            if(dup2(fd[0], STDIN_FILENO) < 0)
                exit (-1);
        }
        close(fd[0]);
        close(fd[1]);
        execvp(file, av);
        exit (-1);
    }
    if (type == 'r') {
        close(fd[1]);
        return (fd[0]);
    } else {
        close(fd[0]);
        return (fd[1]);
    }
}

int main() {
    int fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');

    char *line;
    while(line = get_next_line(fd))
        ft_putstr(line);
}

