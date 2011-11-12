#ifndef SHOOSH_H
#define SHOOSH_H

#include <cstdio>
#include <list>
#include <unistd.h>
#include <errno.h>
#include "Process.hpp"
#include "Parser.hpp"
#include "Redirector.hpp"
#include "Job.hpp"
#include "Executor.hpp"
#include "Initializer.hpp"

extern int errno;

std::list<Job*>			jobList;
std::list<std::string>	history;
struct sigaction sa_chld, sa_tstp, sa_cont, sa_ttin, sa_ttou;
int currID;
Initializer initializer;

void shooSH_run();
			
/*
 * Ctrl Z
 */
void hantstp (int signum, siginfo_t *siginfo, void* useless) {

	std::list <Job*>::iterator curr, end;

	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->inBG(); curr++);
	printf("TSTP no filho %d\n", (*curr)->getPGID());
	if (curr != end)	kill (-(*curr)->getPGID(), SIGTSTP);
}

/*
 * Resumo do processo parado
 */
void hancont (int signum) {

	fprintf (stderr, "Filho voltando a ativa\n");
}
void jobs (void) {
	std::list<Job*>::iterator curr,end;
	for (curr = jobList.begin(), end = jobList.end(); curr != end; curr++) {
		(*curr)->print();
	}
}

/*
 * Quando acontece alguma coisa com os childs
 */
void hanchld (int signum, siginfo_t *siginfo, void* useless) {
	std::list<Job*>::iterator curr, end;
	int status;
	int err;
	fprintf (stderr, "Aconteceu algo com o child ");
	fprintf (stderr, "%d pelo code %d com status %d\n", siginfo->si_pid, 
			siginfo->si_code, siginfo->si_status);
	// encontra o job se for o primeiro processo
	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->getPGID() != siginfo->si_pid; curr++);

	if (siginfo->si_code == CLD_KILLED || siginfo->si_code == CLD_EXITED) {
		printf ("killed ou exited");
		if (curr != end) {
			(*curr)->destroy();
			jobList.erase (curr);
		}
		jobs();
		// tratar processos defuntos

//		if (waitpid (siginfo->si_pid, &status, 0) == -1) {
//			err = errno;
//			printf ("error: ");
//			if (err == ECHILD)	printf ("process does not exist or is not my child\n");
//			else if (err == EINTR)	printf ("SIGCHLD was caught\n");
//			else	printf ("EINVAL\n");
//		} else {
			tcsetattr (STDIN_FILENO, TCSADRAIN, initializer.getTermios());
			tcsetpgrp (STDIN_FILENO, initializer.getPGID());
//		}
	} else if (siginfo->si_code == CLD_STOPPED) {
		printf ("entrou no stopped\n");
	//	if (curr != end)
	//		tcgetattr (STDIN_FILENO, (*curr)->getTermios()
		if (waitpid (siginfo->si_pid, &status, WNOHANG) == -1) {
			err = errno;
			printf ("error: ");
			if (err == ECHILD)	printf ("process does not exist or is not my child\n");
			else if (err == EINTR)	printf ("SIGCHLD was caught\n");
			else	printf ("EINVAL\n");
		} else {
			tcsetattr (STDIN_FILENO, TCSADRAIN, initializer.getTermios());
			tcsetpgrp (STDIN_FILENO, initializer.getPGID());
		}
	}

}

/*
 *	TTIN
 */
void hanttin(int signum, siginfo_t *siginfo, void* useless){
	printf("TTIN no %d %d\n", siginfo->si_pid, getpid());
	exit(0);
}


/*
 *	TTOU
 */
void hanttou (int signum){

	printf("TTou %d\n", getpid());
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
	int err;

	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->getID() != id; curr++);

	if (curr == end) {
		throw -id;
	} else {
		(*curr)->setBG(false);
		tcsetattr(STDIN_FILENO, TCSADRAIN, (*curr)->getTermios());
		tcsetpgrp (STDIN_FILENO, (*curr)->getPGID());
		printf ("aqui?\n");
		kill (-(*curr)->getPGID(), SIGCONT); /*descobrir como se volta o processo para fg*/
		if (waitpid ((*curr)->getProcess(0).getPID(), &status, WUNTRACED) == -1) {
			err = errno;
			printf ("error: ");
			if (err == ECHILD)	printf ("process does not exist or is not my child\n");
			else if (err == EINTR)	printf ("SIGCHLD was caught\n");
			else	printf ("EINVAL\n");
		} else {
			if (WIFEXITED(status)) {
				printf ("saiu com %d\n", WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				printf ("foi terminado por um sinal %d\n", WTERMSIG(status)==SIGTSTP);
			} else if (WIFSTOPPED(status)) {
				printf ("estava stopped ainda %d\n", WSTOPSIG(status));
			} else if (WIFCONTINUED(status)) {
				printf ("continuou\n");
			}
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
		kill (-(*curr)->getPGID(), SIGTSTP);
	}
}

		
void shooSH_init() { 

	initializer.init();	
	
	sa_chld.sa_sigaction = &hanchld;
	sa_chld.sa_flags = SA_SIGINFO;
	
	sa_tstp.sa_sigaction = &hantstp;
	sa_tstp.sa_flags = SA_SIGINFO;

	sa_cont.sa_handler = &hancont;
	sa_cont.sa_flags = 0;

	sa_ttin.sa_sigaction = &hanttin;
	sa_ttin.sa_flags = SA_SIGINFO;

	sa_ttou.sa_handler = &hanttou;
	sa_ttou.sa_flags = 0;

	/*Tratamento decente de sinais*/
	sigaction (SIGCHLD, &sa_chld, NULL);
	sigaction (SIGTSTP, &sa_tstp, NULL);
	sigaction (SIGCONT, &sa_cont, NULL);
	sigaction (SIGTTIN, &sa_ttin, NULL);
	sigaction (SIGTTOU, &sa_ttou, NULL);

	currID = 0;
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
			job->setID (++currID);
			job->print();
			jobList.push_back (job);

			if (job->getCommand()[0] == 'f') {
				foreground (currID-1);		
			} else {
				executor.execute (job);
			}
		} else {
			exited = job->hasExited();
		}
		job = NULL;
		// retorna o controle para a shell
		tcsetattr (STDIN_FILENO, TCSADRAIN, initializer.getTermios());
		tcsetpgrp (STDIN_FILENO, initializer.getPGID());
	}
}

#endif
