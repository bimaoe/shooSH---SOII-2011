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
		
};

Initializer::Initializer(){
}

void Initializer::init(){
	
	int term = STDIN_FILENO;
	pgid = getpgrp();
	
	if( isatty(term) ){
		/*Enquanto nao tiver em foreground*/
		while( tcgetpgrp(term) != pgid ){
			kill( -pgid, SIGTTIN );
			pgid = getpgrp();
		}
		
		/*Ignorando sinais*/
		signal (SIGINT, SIG_IGN);
		//signal (SIGQUIT, SIG_IGN);
		signal (SIGTSTP, SIG_IGN);
		signal (SIGTTIN, SIG_IGN);
		signal (SIGTTOU, SIG_IGN);
		signal (SIGCHLD, SIG_IGN);
		
		pgid = getpid();
		if( setpgid(pgid, pgid) < 0){
			fprintf(stderr, "Initializer::Initializer() - Nao criou program group\n");
			exit(1);
		}
		
		tcsetpgrp( term, pgid );
		tcgetattr( term, &modes );
	}	
};

#endif
