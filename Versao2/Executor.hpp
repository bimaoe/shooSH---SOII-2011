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

class Executor {
public:
	Executor (void) {};
	
	/*
	 *	Executa um job
	 *	@Param
	 *		Ponteiro para o job a ser executado
	 *	@Exception
	 *		-1		erro no fork
	 */
	
	void execute (Job* job) {
	 	pid_t pid;
		int status, err;
		pid_t w;
				
		pid = fork();
		
		if (pid < 0)	throw -1;
		else if (pid == 0) { // o filho executa o processo
			//TODO setar atributos(?)
			if (!job->inBG()) //se for foreground
				tcsetpgrp (STDIN_FILENO, job->getPGID()); //vira o dono do terminal
			signal (SIGINT, SIG_DFL);
			signal (SIGQUIT, SIG_DFL);
			signal (SIGTSTP, SIG_DFL);
			signal (SIGTTIN, SIG_DFL);
			signal (SIGTTOU, SIG_DFL);
			signal (SIGCHLD, SIG_DFL);
			
			fprintf (stderr, "Executando %d\n", getpid());
			job->setPGID(getpid());
			setpgid (getpid(), getpid());
			
			if (job->hasPipe()) {
			
				Piper piper;
				piper.execPipe (job);
			} else {
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
			job->getProcess(0)->setPID(pid);
			job->setPGID(pid);
			setpgid (pid, pid);
			if (!job->inBG()) {
				tcsetpgrp (STDIN_FILENO, pid);
				/*signal (SIGINT, SIG_DFL);
				signal (SIGQUIT, SIG_DFL);
				signal (SIGTSTP, SIG_DFL);
				signal (SIGTTIN, SIG_DFL);
				signal (SIGTTOU, SIG_DFLexit);
				signal (SIGCHLD, SIG_DFL);*/
				while ((w = waitpid(pid, &status, WUNTRACED)) == -1 && (err = errno) == EINTR);
				if (w == -1) {
					fprintf (stderr, "Erro: Executor: ");
					if (err == ECHILD)	fprintf (stderr, "processo (%d) nao existe ou nao eh meu filho\n", pid);
					else if (err == EINTR)	fprintf (stderr, "SIGCHLD provavelmente ferrou tudo\n");
					else	fprintf (stderr, "EINVAL\n");
				} else {
					tcgetattr(STDIN_FILENO, job->getTermios());
					tcsetattr (STDIN_FILENO, TCSADRAIN, &shooSHTermios);
					tcsetpgrp (STDIN_FILENO, shooSHPGID);
					if (WIFEXITED(status))	std::cout << "Filho " << pid << " morreu em paz " << WEXITSTATUS(status) << std::endl;
					else if (WIFSIGNALED(status))	std::cout << "Filho " << pid << " recebeu sinal " << WTERMSIG(status) << std::endl;
					else if (WIFSTOPPED(status))	std::cout << "Filho " << pid << " foi parado com sinal " << WSTOPSIG(status) << std::endl;
				}
			} else
				tcgetattr(STDIN_FILENO, job->getTermios()); // Pega os atributos do job
		}
	}
};

#endif
