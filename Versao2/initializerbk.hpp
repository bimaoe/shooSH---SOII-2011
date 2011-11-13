#ifndef INITIALIZER_HPP
#define INITIALIZER_HPP

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <signal.h>

class Initializer{

	private:	
		pid_t pgid;
		struct termios modes;
		
	public:
		Initializer();
		void init();
		pid_t getPGID(void);
		struct termios *getTermios();

};

Initializer::Initializer(){
}

pid_t Initializer::getPGID (void) {
	return pgid;
}

struct termios *Initializer::getTermios(){
	return &modes;
}

void Initializer::init(){
	
	int term = STDIN_FILENO;
	pgid = getpgrp(); // Pega o meu group ID
		
	if( isatty(term) ){
		/*Enquanto nao tiver em foreground*/
		while( tcgetpgrp(term) != pgid ){ //Enquanto eu nao estiver em foreground
			kill(-tcgetpgrp(term), SIGTTIN ); //Tenta depor a shell
			pgid = getpgrp();
		}
		
		/*Ignorando sinais*/ 
		//signal (SIGINT, SIG_IGN);
		signal (SIGQUIT, SIG_IGN);
		
		pgid = getpid(); // Pega o meu pid
		if( setpgid(pgid, pgid) < 0){ // Seta o meu group ID como o meu PID
			fprintf(stderr, "Initializer::Initializer() - Nao criou program group\n");
			exit(1);
		}
		
		tcsetpgrp( term, pgid ); //Seta o meu group ID como o dono do terminal
		tcgetattr( term, &modes ); //Pega meus atributos
	}
};

#endif
