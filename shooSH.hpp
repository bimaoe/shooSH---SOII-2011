#ifndef SHOOSH_H
#define SHOOSH_H

#include <cstdio>
#include <list>
#include <unistd.h>
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
int currID;

void shooSH_run();
			
/*
 * Ctrl Z
 */
void hantstp (int signum, siginfo_t *siginfo, void* useless) {

	std::list <Job*>::iterator curr, end;

	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->inBG(); curr++);
	if (curr != end)	kill ((*curr)->getPID(), SIGTSTP);
	fprintf (stderr, "TSTP no filho %d\n", (*curr)->getPID());
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
	std::list<Job*>::iterator curr, end;
	int status;
	fprintf (stderr, "Aconteceu algo com o child ");
	fprintf (stderr, "%d pelo sinal %d com status %d\n", siginfo->si_pid, 
			siginfo->si_code, siginfo->si_status);
	if (siginfo->si_code == CLD_KILLED || siginfo->si_code == CLD_EXITED) {
		for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->getPID() != siginfo->si_pid; curr++);
		if (curr != end)	(*curr)->destroy();
		jobList.erase (curr);
		waitpid (siginfo->si_pid, &status, 0);
	}
}

void jobs (void) {
	std::list<Job*>::iterator curr,end;
	for (curr = jobList.begin(), end = jobList.end(); curr != end; curr++) {
		(*curr)->print();
	}
}

void changeDirectory (const char* dir) {
	chdir (dir);
}

void showDirectoryName (void) {
	char buf[256];
	if (buf != getcwd(buf, 256))	throw -1; //buffer overflow
	printf ("%s\n", buf);
}

void foreground (int id) {
	std::list<Job*>::iterator curr, end;
	int status;

	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->getID() != id; curr++);

	if (curr == end) {
		throw -id;
	} else {
		(*curr)->setBG(false);
		kill ((*curr)->getPID(), SIGCONT); /*descobrir como se volta o processo para fg*/
		tcsetpgrp (STDIN_FILENO, (*curr)->getPID());
		waitpid (-(*curr)->getPGID(), &status, WUNTRACED);
		if (WIFEXITED(status)) {
			printf ("saiu com %d\n", WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			printf ("foi terminado por um sinal %d\n", WTERMSIG(status));
		} else if (WIFSTOPPED(status)) {
			printf ("estava stopped ainda %d\n", WSTOPSIG(status));
		} else if (WIFCONTINUED(status)) {
			printf ("continuou\n");
		}
	}
}

void background (int id) {
	std::list<Job*>::iterator curr, end;
	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->getID() != id; curr++);
	if (curr == end) {
		throw -id;
	} else {
		(*curr)->setBG(true);
		kill ((*curr)->getPID(), SIGCONT);
	}
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

	currID = 1;
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
			job->setID (currID++);
			jobList.push_back (job);
			if (job->getCommand()[0] == 'f') {
				foreground (1);		
			} else {
				tcsetpgrp (STDIN_FILENO, job->getPID());
				executor.execute (job);
			}
		} else {
			exited = job->hasExited();
		}
		job = NULL;
	}
}

#endif
