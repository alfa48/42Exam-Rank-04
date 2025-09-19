# Exam Rank 04
- manandre

## picoshell


### Implementacão inicial

#### Problema

- Too many open files

Minha implementação usava laços para criar vários files descriptors(fd's) simultaneamente
e parece que o um dos testes do exame da 42  para esta função(picoshell) primero usa o commando no terminal 'ulimit -n 1024' forcando o sistema a permitir no maximo 1024 fd's abertos. E o:

'for (int i = 0; i < count - 1; i++){//pipes
	if (pipe(pipes[i]) < 0){
		//fechar os anteriores
		return (1);
	}
}'

em casos onde tinha um numero superior a 1024 de pipes no commando a função pipes retorna -1 e o picoshell retorna 1 e o perror() no main do tester exiba o Too many open files.

### Implementacão 01
