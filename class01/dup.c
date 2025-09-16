#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

// Função auxiliar para mostrar descritores ativos
void show_fd(const char *msg, int fd1, int fd2) {
    printf("%s -> fd1=%d, fd2=%d\n", msg, fd1, fd2);
}

int main(void) {
    // ---------- TESTE 1: STDOUT DUPLICADO ----------
    printf("=== TESTE 1: duplicando stdout (1) ===\n");
    int copia = dup(1); // duplica o stdout
    show_fd("stdout duplicado", 1, copia);

    // Agora tanto 1 (stdout) quanto copia escrevem na tela
    write(1, "Escrevendo com fd=1\n", 21);
    write(copia, "Escrevendo com copia de stdout\n", 31);

    close(copia); // fecha o duplicado


    // ---------- TESTE 2: DUPLICANDO ARQUIVO ----------
    printf("\n=== TESTE 2: escrevendo em arquivo com dup ===\n");
    int fd = open("saida.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); exit(1); }

    int fd2 = dup(fd); // duplica o descritor
    show_fd("arquivo aberto", fd, fd2);

    // Ambos apontam para o mesmo arquivo e compartilham o mesmo cursor
    write(fd, "Ola", 3);       // escreve "Ola"
    write(fd2, " Mundo\n", 7); // continua no mesmo ponto -> "Ola Mundo"

    close(fd);
    close(fd2);
    printf("Veja o conteúdo de 'saida.txt'\n");


    // ---------- TESTE 3: MOSTRANDO O CURSOR COMPARTILHADO ----------
    printf("\n=== TESTE 3: cursor de leitura compartilhado ===\n");
    fd = open("saida.txt", O_RDONLY);
    fd2 = dup(fd);

    char buf[10];

    read(fd, buf, 3);  // lê "Ola"
    buf[3] = '\0';
    printf("fd leu: %s\n", buf);

    read(fd2, buf, 6); // continua de onde o outro parou
    buf[6] = '\0';
    printf("fd2 leu: %s\n", buf);

    close(fd);
    close(fd2);


    // ---------- TESTE 4: REUTILIZANDO MENOR DESCRITOR LIVRE ----------
    printf("\n=== TESTE 4: menor descritor livre ===\n");
    fd = open("saida.txt", O_RDONLY);
    printf("fd = %d\n", fd); // deve ser 3
    close(fd);               // libero o 3

    fd = open("saida.txt", O_RDONLY);
    printf("fd reaberto = %d (kernel reutilizou menor livre)\n", fd);

    close(fd);

    return 0;
}

