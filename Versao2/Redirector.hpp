#ifndef REDIRECTOR_H
#define REDIRECTOR_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

// tipos de redirecionamento
#define REDIN		1
#define REDOUTT		2
#define REDOUTA		4
#define REDERRT		8
#define REDERRA		16

class Redirector {
	private:
		int flags[3];
		int fd[3];	// fd[0] = input
					// fd[1] = output
					// fd[2] = error
		
	public:
		Redirector (void);
		void init (std::string filename[3], int flag[3]);
		void end();
};

Redirector::Redirector (void) {
	fd[0] = fd[1] = fd[2] = -1; 		// incializa os descritores de arquivo
	flags[0] = O_RDONLY;				// inicializa flags
	flags[1] = O_RDWR|O_CREAT|O_TRUNC;
	flags[2] = O_RDWR|O_CREAT|O_APPEND;
}

void Redirector::init (std::string filename[3], int flag[3]){
	if (flag[0] == REDIN) {
		fd[0] = open (filename[0].c_str(), flags[0], 666); //TODO tirar essa coisa
		dup2 (fd[0], 0);
	}
	if (flag[1] == REDOUTT) {
		fd[1] = open (filename[1].c_str(), flags[1], 666);
		dup2 (fd[1], 1);
	}
	if (flag[1] == REDOUTA) { 
		fd[1] = open (filename[1].c_str(), flags[2], 666);
		dup2 (fd[1], 1);
	}
	if (flag[2] == REDERRT) {
		fd[2] = open (filename[2].c_str(), flags[1], 666);
		dup2 (fd[2], 2);
	} 
	if (flag[2] == REDERRA) {
		fd[2] = open (filename[2].c_str(), flags[2], 666);
		dup2 (fd[2], 2);
	}
}

void Redirector::end(){
	if (fd[0] != -1)	close (fd[0]);
	if (fd[1] != -1)	close (fd[1]);
	if (fd[2] != -1)	close (fd[2]);
	
	fd[0] = fd[1] = fd[2] = -1;
}
#endif
