#ifndef INITIALIZER_HPP
#define INITIALIZER_HPP

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <signal.h>
#include "Job.hpp"
//@{
std::list<Job*>			jobList; //!< Lista de Jobs da shell.
struct termios			shooSHTermios; //!< Atributos da shell.
pid_t					shooSHPGID; //!< Group ID da shell.

struct sigaction sa_chld; //!< Estrutura para o sinal SIGCHLD.
struct sigaction sa_ttin; //!< Estrutura para o sinal SIGTTIN.
struct sigaction sa_ttou; //!< Estrutura para o sinal SIGTTOU.

/*!
 * \brief Handler para sinais de processos filhos.
 */
void hanchld (int signum, siginfo_t *siginfo, void* context) ;
/*!
 *	\brief Handler para o sinal SIGTTIN
 */
void hanttin(int signum, siginfo_t *siginfo, void* context);

/*!
 *	\brief Handler para o sinal SIGTTOU
 */
void hanttou (int signum);

/*!
 *	\brief Inicializa sinais e entrega o terminal para shell
 */
void init_tty (void);
//@}



void hanchld (int signum, siginfo_t *siginfo, void* context) {

	std::list<Job*>::iterator curr, end;
	int status;
	int err;
	
	// procura o job
	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->getPGID() != siginfo->si_pid; curr++);

	if (siginfo->si_code == CLD_KILLED || siginfo->si_code == CLD_EXITED) {
		if (curr != end) {
			std::cout << '[' << (*curr)->getID() << "]\tFinalizado\t" << (*curr)->getCommand() << std::endl;
			if (!(*curr)->inBG()) { // se o job estava em foreground, retorna o terminal para a shell
				tcsetattr (STDIN_FILENO, TCSADRAIN, &shooSHTermios);
				tcsetpgrp (STDIN_FILENO, shooSHPGID);
			}
			(*curr)->destroy(); // desaloca a memoria do job finalizado
			jobList.erase (curr); // retira o job da lista
		}
	} else if (siginfo->si_code == CLD_STOPPED) {
		if (curr != end) {
			tcgetattr (STDIN_FILENO, (*curr)->getTermios());
			if (!(*curr)->inBG()) {
				tcsetattr (STDIN_FILENO, TCSADRAIN, &shooSHTermios);
				tcsetpgrp (STDIN_FILENO, shooSHPGID);
			}
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


void hanttin(int signum, siginfo_t *siginfo, void* context){
	fprintf(stderr, "TTIN no processo %d\n", getpid());
}


void hanttou (int signum){
	fprintf(stderr, "TTOU no processo %d\n", getpid());
}


void init_tty (void){
	
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


		/*Tratamento de sinais*/
		sigaction (SIGCHLD, &sa_chld, NULL);
		sigaction (SIGTTIN, &sa_ttin, NULL);
		sigaction (SIGTTOU, &sa_ttou, NULL);
		signal (SIGTSTP, SIG_IGN);
		
		
		shooSHPGID = getpid(); // Pega o meu pid
		if( setpgid(shooSHPGID, shooSHPGID) < 0){ // Seta o meu group ID como o meu PID
			fprintf(stderr, "Erro: nao foi possivel setar o group ID\n");
			exit(1);
		}
		
		tcsetpgrp( term, shooSHPGID ); //Seta o meu group ID como o dono do terminal
		tcgetattr( term, &shooSHTermios ); //Pega meus atributos
	}
}
#endif
