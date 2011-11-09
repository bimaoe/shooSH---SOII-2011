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


class shooSH {
	private:
		std::list<Job*>			jobList;
		std::list<std::string>	history;
		Initializer initializer;
		struct sigaction sa_chld, sa_tstp, sa_cont;

	public:
		shooSH (void);
		
		static void hanchld (int, siginfo_t*, void*);
		static void hantstp (int);
		static void hancont (int);
	
		void run();
};

shooSH::shooSH() { 

	initializer.init();	
	
	sa_chld.sa_sigaction = &hanchld;
	sa_chld.sa_flags = SA_SIGINFO;
	
	sa_tstp.sa_handler = &hantstp;
	sa_tstp.sa_flags = 0;

	sa_tstp.sa_handler = &hancont;
	sa_tstp.sa_flags = 0;

	/*Tratamento decente de sinais*/
	sigaction (SIGCHLD, &sa_chld, NULL);
	sigaction (SIGTSTP, &sa_tstp, NULL);
	sigaction (SIGCONT, &sa_cont, NULL);
}

/*
 * Ctrl Z
 */
void shooSH::hantstp (int signum) {
	
	kill (jobList.back().getPID(), SIGTSTP);
	printf ("Pai: shoosh filhinho '-'\n");
}

/*
 * Resumo do processo parado
 */
void shooSH::hancont (int signum) {
//	kill (pid, SIGTSTP);
	printf ("Mae: diga filhinho ~ \n");
}

/*
 * Quando acontece alguma coisa com os childs
 */
void shooSH::hanchld (int signum, siginfo_t *siginfo, void* useless) {
	printf ("Mae: filhinhooo  TToTT\n");
	printf ("%d %d com %d\n", siginfo->si_pid, siginfo->si_code, 
								siginfo->si_status);
}


/**
 *	Executor da Shell
 */
void shooSH::run (void) {
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
