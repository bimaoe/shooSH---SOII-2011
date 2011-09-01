#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>

int main(int argc, char* argv[]){

	char* comando[] = {"ls", "-l", NULL}; //dispensavel
	pid_t pid;
	pid = fork();	//retorna 0 para o filho e o numero do id do filho para o pai 	

	if(pid < 0)
		throw "ERRO";
	else if(pid == 0){
		execvp(comando[0], comando);
	} else {
		//Espera o pid do filho
		//wait(pid);
	}
}

