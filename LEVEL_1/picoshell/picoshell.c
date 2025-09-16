/* picoshell.c
 *
 * Implementação didática e simples da função:
 * int picoshell(char *cmds[]);
 *
 * Obrigado: usar apenas as syscalls/funções permitidas para este exercício:
 * close, fork, wait, exit, execvp, dup2, pipe
 *
 * A função recebe um array NULL-terminated de "argv" (cada elemento é um
 * argv pronto para execvp). Internamente fazemos cast para char ***.
 */

#include <unistd.h>   // fork, pipe, dup2, execvp, close
#include <sys/types.h>
#include <sys/wait.h> // wait
#include <stdlib.h>   // exit
#include <stddef.h>   // NULL

#define MAX_CMDS 1024

int picoshell(char *cmds[])
{
    int i;
    char ***commands = (char ***)cmds; // cada commands[i] é um argv (NULL-terminated)
    int n = 0;

    /* Conta comandos (NULL-terminated). */
    while (n < MAX_CMDS && commands[n] != NULL)
        n++;

    if (n == 0)
        return 0; /* nada a executar -> sucesso */

    /* Se houver muitos comandos, tratar como erro (fechar nada) */
    if (n >= MAX_CMDS)
        return 1;

    /* Arrays em stack: pipes e pids */
    int pipes[MAX_CMDS - 1][2];
    pid_t pids[MAX_CMDS];

    /* Cria os pipes necessários (n-1). Vamos criar antes de cada fork
       só quando necessário para simplificar o fluxo abaixo. */

    int prev_rd = -1; /* descriptor de leitura do pipe anterior */

    for (i = 0; i < n; ++i)
    {
        /* Se não for o último comando, crie um pipe para conectar a saída deste
           comando à entrada do próximo. */
        int do_pipe = (i < n - 1);
        if (do_pipe)
        {
            if (pipe(pipes[i]) == -1)
            {
                /* erro ao criar pipe: fechar pipes já abertos */
                int j;
                for (j = 0; j < i; ++j)
                {
                    /* alguns pipes podem não ter sido inicializados; feche os que já existem */
                    if (j < n - 1)
                    {
                        /* close pode falhar, mas não podemos fazer mais nada */
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                    }
                }
                return 1;
            }
        }

        pid_t pid = fork();
        if (pid == -1)
        {
            /* Erro no fork: fechar todos os pipes já criados */
            int j;
            for (j = 0; j < i; ++j)
            {
                if (j < n - 1)
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            }
            return 1;
        }

        if (pid == 0)
        {
            /* ---------- Child ---------- */
            /* Se existe leitura do pipe anterior, duplicar para stdin */
            if (prev_rd != -1)
            {
                if (dup2(prev_rd, 0) == -1)
                    exit(1);
            }

            /* Se este comando tem pipe para próximo, duplicar escrita para stdout */
            if (do_pipe)
            {
                if (dup2(pipes[i][1], 1) == -1)
                    exit(1);
            }

            /* Fechar todos os descritores de pipe no filho (não mais necessários) */
            {
                int j;
                for (j = 0; j < n - 1; ++j)
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            }
            /* Também fechar prev_rd se ficou aberto (dup2 não fecha automaticamente) */
            if (prev_rd != -1)
                close(prev_rd);

            /* Executar o comando: commands[i] é argv já pronto para execvp */
            execvp(commands[i][0], commands[i]);

            /* Se execvp falhar, sair com status 1. */
            exit(1);
        }

        /* ---------- Parent ---------- */
        pids[i] = pid;

        /* No pai: fechar extremos que não vamos mais usar.
           - Se havia prev_rd (leitura do pipe anterior), já não precisa mais aqui. */
        if (prev_rd != -1)
        {
            close(prev_rd);
            prev_rd = -1;
        }

        /* Se criamos um pipe para este comando, fechar sua extremidade de escrita
           no pai (já que a escrita ficará com o filho). Mantemos a leitura
           para ligar ao próximo filho. */
        if (do_pipe)
        {
            close(pipes[i][1]);     /* fechar escrita no pai */
            prev_rd = pipes[i][0];  /* leitura permanece para o próximo comando */
        }
    }

    /* Após criar todos os filhos, no pai podemos fechar qualquer prev_rd que ficou aberto. */
    if (prev_rd != -1)
    {
        close(prev_rd);
        prev_rd = -1;
    }

    /* Esperar todos os filhos terminarem */
    for (i = 0; i < n; ++i)
    {
        /* wait() retorna pid do filho reaped ou -1; ignoramos o status */
        wait(NULL);
    }

    return 0;
}
