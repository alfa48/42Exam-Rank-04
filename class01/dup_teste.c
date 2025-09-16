#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

void show_fd(const char *sms, int fd1, int fd2){
	printf("%s :::> fd1=%d, fd2=%d", sms, fd1, fd2);
}

int main(void){

	
	// DUPLICANDO O FD 1
	int fd_copy = dup(1);
	show_fd("usado o int fd1 = dup(1)\n", 1, fd_copy);

	
	// ESCREVENDO ONDE O DUPLICADO E NO 1 APONTAM
	write(1, "escrevendo ende o fd=1 aponta\n", 31);
	write(fd_copy, "escrevendo onde o fd=copy aponta\n", 34);
	return (0);
}
