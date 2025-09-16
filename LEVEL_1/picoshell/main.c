// main.c (apenas para testes locais)
#include <stdio.h>

int picoshell(char *cmds[]);

int main(void)
{
    // pipeline: echo "hello" | sed "s/e/E/g"
    char *cmd1[] = {"echo", "hello", NULL};
    char *cmd2[] = {"sed", "s/e/E/g", NULL};
    char **cmds[] = {cmd1, cmd2, NULL};

    return picoshell((char **)cmds);
}
