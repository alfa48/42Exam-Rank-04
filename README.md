# Exam Rank 04
- manandre

## picoshell

### Implementação inicial

#### Problema

- **Too many open files**

Minha implementação usava laços para criar vários *file descriptors* (FDs) simultaneamente.  
Parece que um dos testes do exame da 42 para esta função (`picoshell`) primeiro usa o comando no terminal:

```bash
ulimit -n 1024
forçando o sistema a permitir no máximo 1024 FDs abertos. E o trecho:

for (int i = 0; i < count - 1; i++){ // pipes
    if (pipe(pipes[i]) < 0){
        // fechar os anteriores
        return 1;
    }
}
done

em casos onde havia um número superior a 1024 de pipes no comando, a função pipe() retorna -1 e o picoshell retorna 1. O perror() no main do tester exibe:

```bash
Too many open files
done
