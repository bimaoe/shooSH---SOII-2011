#include "Redirector.hpp"
Redirector::Redirector (void) {
	fd[0] = fd[1] = fd[2] = -1; 		// intialize the file descriptors
	flags[0] = O_RDONLY;				// initialize flags
	flags[1] = O_RDWR|O_CREAT|O_TRUNC;
	flags[2] = O_RDWR|O_CREAT|O_APPEND;
}

void Redirector::init (char* filename[3], int flag){
	if (flag & REDIN) {
		fd[0] = open (filename[0], flags[0], 666); //TODO tirar essa coisa
		dup2 (fd[0], 0);
	}
	if (flag & REDOUTT) {
		fd[1] = open (filename[1], flags[1], 666);
		dup2 (fd[1], 1);
	} else if (flag & REDOUTA) { // to prevent from opening twice
		fd[1] = open (filename[1], flags[2], 666);
		dup2 (fd[1], 1);
	}
	if (flag & REDERRT) {
		fd[2] = open (filename[2], flags[1], 666);
		dup2 (fd[2], 2);
	} else if (flag & REDERRA) {
		fd[2] = open (filename[2], flags[2], 666);
		dup2 (fd[2], 2);
	}
}

void Redirector::end(){
	if (fd[0] != -1)	close (fd[0]);
	if (fd[1] != -1)	close (fd[1]);
	if (fd[2] != -1)	close (fd[2]);
	
	fd[0] = fd[1] = fd[2] = -1;
}
