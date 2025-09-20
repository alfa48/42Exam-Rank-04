# Exam Rank 04
- manandre

## ft_popen

### O que o exercício pede

O objectivo deste exercício é contruir uma função chamada ft_popen, aqui tens seu prototipo:

```c
int ft_popen(const char *file, char *const argv[], char type);
```

 Como podem ver a função recebe como argumentos uma string um array de string e um char.
 
### argumentos da função ft_popen

```c
const char *file
```

O file seria um comando(ex: ls) que a função terá de executar.

```bash
int ft_popen(const char *file, char *const argv[], char type);
```

 O argv um array começando com o comando e a lista de argumentos(ex: (char *const[]){"cat", "-e", NULL}). Terminando sempre em NULL
 
 
 
```c
char type
```
O type é um char que pode assumir dois valores: 'w' ou 'r'. Influenciando o retorno da função.

### retorno da função
 
A função deve retornar um descritor de arquivo (fd) para ler, se type = 'r', a saida da execussão do comando ou escrever se type for 'w' na entrada da execusão do comando. Nota o subject obriga a usares a função execvp para executares o comando (file).

Se der algum erro a função deve retornar -1;
 
### Implementação

Primeiro validei a entrada:
```c
	if (!file || !argv || !argv[0] || (type != 'w' && type != 'r'))
		return (-1);
```
Como o execvp depois de ser executado mata o processo que o chamou, temos que criar um processo dentro da nossa função. Não queremos que o processo que está usando a nossa função morra né. na verdade não queremos que ninguem morra certo? certo?

então teremos que usar a forka ou melhor o fork(); Mas nota que precisamos retornar a saida da execusão 
do file ou entregar algo para a entrada da execusão do file. Ou seja, o processo principal precisa de communicar com o processo filho que vamos criar. e pra isso a arma que o subject nos permite usar é o pipe(). Se você não sabe o que isso é não é problema meu. Brincadera, cadera também preceberam se estiverem a ler isso em portugues. vai pesquisar.

Por isso o proximo parsso foi criar pipes ainda no processo principal

```c
	int pipes[2];
	if (pipe(pipes) < 0)
		return (-1);
```
depois criar o processo filho
```bash
	pid_t pid = fork();
```
#### Pontos relevantes da implementação

No processo filho verifica-se o valor de type. E se for 'r' é fechado o fd do pipe de leitura e o descritor padrão stdout aponta para o mesmo objecto que o de escrita do pipe. Permitindo o execvp escrever no pipe para a função devolver o fd de leitura isto no processo principal (pai). 


```c
	if (type == 'r'){
		if (dup2(pipes[1], 1) < 0){
			close(pipes[0]);
			close(pipes[1]);
			exit (-1);
		}
		close(pipes[0]);
		close(pipes[1]);
	}
```

e ao coontrario se type 'w'. Tó cansado de escrever var ver o códio: level_1/ft_popen/ft_popen.c
E nota, o principal processo não se precisa aguardar o filho morrer. 

## picoshell

### Implementação inicial

#### Problema

- **Too many open files**

Minha implementação usava laços para criar vários *file descriptors* (FDs) simultaneamente.  
Parece que um dos testes do exame da 42 para esta função (`picoshell`) primeiro usa o comando no terminal:

```bash
ulimit -n 1024
```
Forçando o sistema a permitir no máximo 1024 FDs abertos. E o trecho:
```c
for (int i = 0; i < count - 1; i++){ // pipes
    if (pipe(pipes[i]) < 0){
        // fechar os anteriores
        return 1;
    }
}
```
Em casos onde havia um número superior a 1024 de pipes no comando, a função pipe() retorna -1 e o picoshell retorna 1. O perror() no main do tester exibe:

```bash
Too many open files
```
## Ideia 099 fazer um picoshell com ft_popen
