#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <vector>
#include <iostream>
#include "Process.hpp"
#include "Job.hpp"
#include "Piper.hpp"
#include "shooSHlib.hpp"
#include "Initializer.hpp"
#include "Builtin.hpp"

/*!
 *	Executor
 *	\brief Classe que executa os comandos
 */

class Executor {
public:
	Executor (void) {};
	
	/*!
	 *	\brief Executa um job.
	 *	\param job Ponteiro para o job a ser executado.
	 *	\exception -1 Ocorreu um erro no fork.
	 */
	void execute (Job* job) {
	 	pid_t pid;
		int status, err;
		pid_t w;
				
		pid = fork();
		
		if (pid < 0)	throw -1;
		else if (pid == 0) { // o filho executa o processo
			if (!job->inBG()) //se for foreground
				tcsetpgrp (STDIN_FILENO, job->getPGID()); //vira o dono do terminal
			// seta todos handlers para default
			signal (SIGINT, SIG_DFL);
			signal (SIGQUIT, SIG_DFL);
			signal (SIGTSTP, SIG_DFL);
			signal (SIGTTIN, SIG_DFL);
			signal (SIGTTOU, SIG_DFL);
			signal (SIGCHLD, SIG_DFL);
			
			job->setPGID(getpid());
			setpgid (getpid(), getpid());
			
			if (job->hasPipe()) { // se houver pipe, manda para o piper executar
			
				Piper piper;
				piper.execPipe (job);
			} else { // se nao houver, executa
				job->getProcess(0)->setPID(getpid());
				
				if (job->getProcess(0)->hasRedirection()) {
					Redirector r;
					r.init (job->getProcess(0)->getFilenames(), job->getProcess(0)->getRedirFlags());
				}
					
				if (execvp(job->getProcess(0)->getCommand()[0], job->getProcess(0)->getCommand()) == -1) {
					fprintf (stderr, "Erro no execvp\n");
					exit (1);
				}
				exit (0);
			}
		} else { // shell
			std::cout << '[' << job->getID() << "]\t" << pid << std::endl;
			job->getProcess(0)->setPID(pid);
			job->setPGID(pid);
			setpgid (pid, pid);
			if (!job->inBG()) {
				tcsetpgrp (STDIN_FILENO, pid);
				
				while ((w = waitpid(pid, &status, WUNTRACED)) == -1 && (err = errno) == EINTR);
				if (w == -1) { // se deu erro
					fprintf (stderr, "Erro: Executor: ");
					if (err == ECHILD)	fprintf (stderr, "processo (%d) nao existe ou nao eh meu filho\n", pid);
					else	fprintf (stderr, "EINVAL\n");
				} else {
					// devolve o terminal para a shell
					tcgetattr(STDIN_FILENO, job->getTermios());
					tcsetattr (STDIN_FILENO, TCSADRAIN, &shooSHTermios);
					tcsetpgrp (STDIN_FILENO, shooSHPGID);
				}
			} else
				tcgetattr(STDIN_FILENO, job->getTermios()); // Pega os atributos do job
		}
	}
};

#endif
