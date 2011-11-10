#ifndef SHOOSH_H
#define SHOOSH_H

#include <cstdio>
#include <list>
#include "Parser.hpp"
#include "Redirector.hpp"
#include "shooSH.hpp"
#include "Job.hpp"
#include "Process.hpp"
#include "Executor.hpp"
#include "Initializer.hpp"

std::list<Job*>			jobList;
std::list<std::string>	history;
struct sigaction sa_chld, sa_tstp, sa_cont;

void shooSH_run();
			
/*
 * Ctrl Z
 */
void hantstp (int signum, siginfo_t *siginfo, void* useless) {

	kill (jobList.back()->getPID(), SIGTSTP);
	fprintf (stderr, "TSTP no filho %d\n", jobList.back()->getPID());
}

/*
 * Resumo do processo parado
 */
void hancont (int signum) {

	fprintf (stderr, "Filho voltando a ativa\n");
}

/*
 * Quando acontece alguma coisa com os childs
 */
void hanchld (int signum, siginfo_t *siginfo, void* useless) {
	fprintf (stderr, "Aconteceu algo com o child ");
	fprintf (stderr, "%d pelo sinal %d com status %d\n", siginfo->si_pid, 
			siginfo->si_signo, siginfo->si_status);
}

		
void shooSH_init() { 

	Initializer initializer;
	initializer.init();	
	
	sa_chld.sa_sigaction = &hanchld;
	sa_chld.sa_flags = SA_SIGINFO;
	
	sa_tstp.sa_sigaction = &hantstp;
	sa_tstp.sa_flags = SA_SIGINFO;

	sa_cont.sa_handler = &hancont;
	sa_cont.sa_flags = 0;

	/*Tratamento decente de sinais*/
	sigaction (SIGCHLD, &sa_chld, NULL);
	sigaction (SIGTSTP, &sa_tstp, NULL);
	sigaction (SIGCONT, &sa_cont, NULL);
}


/**
 *	Executor da Shell
 */
void shooSH_run (void) {
	Job* job;
	Parser p;
	Executor executor;
	bool exited = false;
	
	while (!exited) {
		std::cout << "shooSH> ";
		job = p.parseLine ();
		history.push_back (job->getCommand());
		if (!(job->isNop()||job->hasExited())) {
			job->print();
			jobList.push_back (job);
			executor.execute (job);
		} else {
			exited = job->hasExited();
		}
		job = NULL;
	}
}

#endif
