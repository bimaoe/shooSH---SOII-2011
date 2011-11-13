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

//TODO replace useless
		
/*
 * Handler para o sinal de TSTP (Ctrl+Z)
 */
void hantstp (int signum, siginfo_t *siginfo, void* context) {

	std::list <Job*>::iterator curr, end;

	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->inBG(); curr++);
	
	if (curr != end) {
		kill (-(*curr)->getPGID(), SIGTSTP);
		fprintf(stderr, "hantstp - TSTP no filho %d - %s\n", (*curr)->getProcess(0)->getPID(), (*curr)->getCommand().c_str());
	}
}

/*
 * Retomada do processo parado
 */
void hancont (int signum) {

	fprintf (stderr, "hancont - Filho voltando a ativa\n");
}

/*
 *	Mostra todos os jobs da lista
 */
void jobs (void) {

	std::list<Job*>::iterator curr,end;
	for (curr = jobList.begin(), end = jobList.end(); curr != end; curr++) {
		(*curr)->print();
	}
}

/*
 * Handler para sinais de processos filhos
 */
void hanchld (int signum, siginfo_t *siginfo, void* useless) {

	std::list<Job*>::iterator curr, end;
	int status;
	int err;
	fprintf (stderr, "hanchld - Filho %d ", siginfo->si_pid);
	fprintf (stderr, "com code %d com status %d\n",
						siginfo->si_code, siginfo->si_status);
	//TODO mudar esse comentario e lembrar o que isso faz
	// encontra o job se for o primeiro processo
	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->getPGID() != siginfo->si_pid; curr++);

	if (siginfo->si_code == CLD_KILLED || siginfo->si_code == CLD_EXITED) {
		fprintf (stderr, "filho morreu\n");
		if (curr != end) {
			fprintf (stderr, "Done\n");
			(*curr)->print();
			(*curr)->destroy();
			jobList.erase (curr);
		}
		// tratar processos defuntos

//		if (waitpid (siginfo->si_pid, &status, 0) == -1) {
//			err = errno;
//			fprintf (stderr, "error: ");
//			if (err == ECHILD)	fprintf (stderr, "process does not exist or is not my child\n");
//			else if (err == EINTR)	fprintf (stderr, "SIGCHLD was caught\n");
//			else	fprintf (stderr, "EINVAL\n");
//		} else {
//			tcsetattr (STDIN_FILENO, TCSADRAIN, initializer.getTermios());
//			tcsetpgrp (STDIN_FILENO, initializer.getPGID());
//		}
	} else if (siginfo->si_code == CLD_STOPPED) {
		fprintf (stderr, "filho parou\n");
	//	if (curr != end)
	//		tcgetattr (STDIN_FILENO, (*curr)->getTermios())
		/*if (waitpid (siginfo->si_pid, &status, WNOHANG) == -1) {
			err = errno;
			fprintf (stderr, stderr, "Erro: ");
			if (err == ECHILD)	fprintf (stderr, "processo nao existe ou nao e meu\n");
			else if (err == EINTR)	fprintf (stderr, "SIGCHLD possivelmente ferrou isso\n");
			else	fprintf (stderr, "EINVAL\n");
		} else {
			fprintf (stderr, "hanchld - nao foi morte nem parada\n");
		}*/
	}
}

/*
 *	TTIN
 */
void hanttin(int signum, siginfo_t *siginfo, void* useless){

	fprintf(stderr, "TTIN no processo %d\n", getpid());
}


/*
 *	TTOU
 */
void hanttou (int signum){

	fprintf(stderr, "TTOU no processo %d\n", getpid());
}

/*
 *	Muda o diretorio atual
 *	
 *	@Param
 *		Diretorio para o qual ocorrera a mudanca
 */
void changeDirectory (const char* dir) {

	chdir (dir);
}

/*
 *	Mostra o diretorio atual
 *	
 *	@Exception
 *		-1	Diretorio maior que 256 caracteres
 */
void showDirectoryName (void) {

	char buf[256];
	if (buf != getcwd(buf, 256))	throw -1; //buffer overflow
	printf ("%s\n", buf);
}

/*
 *	Coloca o job em execucao
 *
 *	@Param
 *		ID do job
 *	@Exception
 *		-ID	Job nao encontrado
 */
void foreground (int id) {
	std::list<Job*>::iterator curr, end;
	int status;
	int err;

	// procura o job
	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->getID() != id; curr++);

	if (curr == end) {
		throw -id;
	} else {
		(*curr)->setBG(false);
		
//		tcsetattr(STDIN_FILENO, TCSADRAIN, (*curr)->getTermios());
//		tcsetpgrp (STDIN_FILENO, (*curr)->getPGID());
		
		kill (-(*curr)->getPGID(), SIGCONT); /*TODO descobrir como se volta o processo para fg*/
		if (waitpid ((*curr)->getPGID(), &status, WUNTRACED) == -1) {
			err = errno;
			fprintf (stderr, "Erro: ");
			if (err == ECHILD)	fprintf (stderr, "processo nao existe ou nao eh meu filho\n");
			else if (err == EINTR)	fprintf (stderr, "SIGCHLD provavelmente ferrou tudo\n");
			else	fprintf (stderr, "EINVAL\n");
		} else {
			if (WIFEXITED(status)) {
				fprintf (stderr, "Filho %d saiu com %d\n", (*curr)->getPGID(), WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				fprintf (stderr, "Filho %d foi terminado por um sinal %d\n", (*curr)->getPGID(),  WTERMSIG(status)==SIGTSTP);
			} else if (WIFSTOPPED(status)) {
				fprintf (stderr, "Filho %d estava stopped ainda %d\n", (*curr)->getPGID(), WSTOPSIG(status));
			} else if (WIFCONTINUED(status)) {
				fprintf (stderr, "Filho %d continuou\n", (*curr)->getPGID());
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
		// shooSH volta ao terminal
		tcsetattr (STDIN_FILENO, TCSADRAIN, initializer.getTermios());
		tcsetpgrp (STDIN_FILENO, initializer.getPGID());
		
		kill (-(*curr)->getPGID(), SIGCONT);
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
			jobList.push_back (job);

			if (job->getCommand()[0] == 'f') {
				foreground (currID-1);
			} else {
				executor.execute (jobList.back());
				jobs();
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
