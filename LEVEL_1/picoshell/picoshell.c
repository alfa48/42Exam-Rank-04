/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: man-tung <948manuel@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 13:21:29 by man-tung          #+#    #+#             */
/*   Updated: 2025/09/17 13:21:38 by man-tung         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int picoshell(char **cmds[]){

    if (!cmds || !cmds[0])
        return (1);
    int count = 0;
    while (cmds[count])
        count++;
    if (count == 1){
        pid_t pid = fork();
        if (pid < 0)
            return (1);
        if (pid == 0){//filho
            execvp(cmds[0][0], cmds[0]);
            exit (1);
        }
        //pai
        int status;
        wait(&status);
        return (0);
    }
    int pipes[count -1][2];
    pid_t pids[count];
    for(int i = 0; i < count - 1; i ++){//criar os pipes 
        if (pipe(pipes[i]) < 0){
            for(int j = 0; j < i; j++){
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return (1);
        }
    }
    for(int i = 0; i < count; i ++){//forkar os cmds
        pids[i] = fork();
        if (pids[i] < 0){
            for(int j = 0; j < count - 1; j++){
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return (0);
        }
        if (pids[i] == 0){//FILHO
            if (i > 0)
                dup2(pipes[i - 1][0], 0);
            if (i != count - 1)
                dup2(pipes[i][1], 1);
            for(int j = 0; j < count - 1; j++){
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(cmds[i][0], cmds[i]);
            exit (1);
        }
    }
    //PAI
    for(int i = 0; i < count - 1; i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    for(int i = 0; i < count; i++){
        int status;
        wait(&status);
    }
    return (0);
}

int main(void)
{
    // pipeline: echo "hello" | sed "s/e/E/g"
    char *cmd1[] = {"echo", "hello", NULL};
    char *cmd2[] = {"sed", "s/e/E/g", NULL};
    char **cmds[] = {cmd1, cmd2, NULL};

    return picoshell((char ***)cmds);
}
