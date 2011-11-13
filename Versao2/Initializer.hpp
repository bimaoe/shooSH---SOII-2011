#ifndef INITIALIZER_HPP
#define INITIALIZER_HPP

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <signal.h>

std::list<Job*>			jobList;
struct termios			shooSHTermios;
pid_t					shooSHPGID;

struct sigaction sa_chld, sa_ttin, sa_ttou;

/*
 * Handler para sinais de processos filhos
 */
void hanchld (int signum, siginfo_t *siginfo, void* context) {

	std::list<Job*>::iterator curr, end;
	int status;
	int err;
	//fprintf (stderr, "hanchld - Filho %d ", siginfo->si_pid);
	//fprintf (stderr, "com code %d com status %d\n",
	//					siginfo->si_code, siginfo->si_status);
	//TODO mudar esse comentario e lembrar o que isso faz
	// encontra o job se for o primeiro processo
	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->getPGID() != siginfo->si_pid; curr++);

	if (siginfo->si_code == CLD_KILLED || siginfo->si_code == CLD_EXITED) {
		if (curr != end) {
			std::cout << "Finalizando\n\t";
			(*curr)->print();
			(*curr)->destroy();
			jobList.erase (curr);
		}
		tcsetattr (STDIN_FILENO, TCSADRAIN, &shooSHTermios);
		tcsetpgrp (STDIN_FILENO, shooSHPGID);
	} else if (siginfo->si_code == CLD_STOPPED) {
		if (curr != end) {
			tcgetattr (STDIN_FILENO, (*curr)->getTermios());
			
			tcsetattr (STDIN_FILENO, TCSADRAIN, &shooSHTermios);
			tcsetpgrp (STDIN_FILENO, shooSHPGID);

			(*curr)->stop();
			(*curr)->print();
		} else {
			tcsetattr (STDIN_FILENO, TCSADRAIN, &shooSHTermios);
			tcsetpgrp (STDIN_FILENO, shooSHPGID);
		}
	} else if (siginfo->si_code == CLD_CONTINUED) {
		(*curr)->cont();
		(*curr)->print();
	}
}

/*
 *	TTIN
 */
void hanttin(int signum, siginfo_t *siginfo, void* context){

	fprintf(stderr, "TTIN no processo %d\n", getpid());
	exit(0);
}


/*
 *	TTOU
 */
void hanttou (int signum){

	fprintf(stderr, "TTOU no processo %d\n", getpid());
}


class Initializer{
	public:
		Initializer();
		void init();
		pid_t getPGID(void);
		struct termios *getTermios();

};

Initializer::Initializer(){
}

void Initializer::init(){
	
	int term = STDIN_FILENO;
	shooSHPGID = getpgrp(); // Pega o meu group ID
		
	if( isatty(term) ){
		/*Enquanto nao tiver em foreground*/
		while( tcgetpgrp(term) != shooSHPGID ){ //Enquanto eu nao estiver em foreground
			kill(-tcgetpgrp(term), SIGTTIN ); //Tenta depor a shell
			shooSHPGID = getpgrp();
		}
		
		/*Ignorando sinais*/ 
		signal (SIGINT, SIG_IGN);
		signal (SIGQUIT, SIG_IGN);
		
		sa_chld.sa_sigaction = &hanchld;
		sa_chld.sa_flags = SA_SIGINFO;
		sigaddset (&(sa_chld.sa_mask), SIGINT);
		sigaddset (&(sa_chld.sa_mask), SIGQUIT);
		sigaddset (&(sa_chld.sa_mask), SIGTTIN);
		sigaddset (&(sa_chld.sa_mask), SIGTTOU);
		sigaddset (&(sa_chld.sa_mask), SIGCHLD);
		sigaddset (&(sa_chld.sa_mask), SIGCONT);
		sigaddset (&(sa_chld.sa_mask), SIGTSTP);

		sa_ttin.sa_sigaction = &hanttin;
		sa_ttin.sa_flags = SA_SIGINFO;

		sa_ttou.sa_handler = &hanttou;
		sa_ttou.sa_flags = 0;


		/*Tratamento decente de sinais*/
		sigaction (SIGCHLD, &sa_chld, NULL);
		sigaction (SIGTTIN, &sa_ttin, NULL);
		sigaction (SIGTTOU, &sa_ttou, NULL);
		signal (SIGTSTP, SIG_IGN);
		
		
		shooSHPGID = getpid(); // Pega o meu pid
		if( setpgid(shooSHPGID, shooSHPGID) < 0){ // Seta o meu group ID como o meu PID
			fprintf(stderr, "Initializer::Initializer() - Nao criou program group\n");
			exit(1);
		}
		
		tcsetpgrp( term, shooSHPGID ); //Seta o meu group ID como o dono do terminal
		tcgetattr( term, &shooSHTermios ); //Pega meus atributos
	}
};

#endif
