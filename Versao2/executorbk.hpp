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
		
		pid = fork();
		
		if (pid < 0)	throw -1;
		else if (pid == 0) { // o filho executa o processo
			if (job->hasPipe()) {
				Piper piper;
				piper.execPipe (job);
			} else {
				fprintf (stderr, "Executando %d\n", getpid());
				job->setPGID(getpid());
				job->getProcess(0)->setPID(getpid());
				setpgid (getpid(), getpid());
				fprintf (stderr, "novo valor do pgid %d\n", job->getPGID());
				
				//TODO setar atributos(?)
				if (!job->inBG()) //se for foreground
					tcsetpgrp (STDIN_FILENO, job->getPGID()); //vira o dono do terminal
				signal (SIGINT, SIG_DFL);
				signal (SIGQUIT, SIG_DFL);
				signal (SIGTSTP, SIG_DFL);
				signal (SIGTTIN, SIG_DFL);
				signal (SIGTTOU, SIG_DFL);
				signal (SIGCHLD, SIG_DFL);
				if (job->getProcess(0)->hasRedirection()) {
					Redirector r;
					r.init (job->getProcess(0)->getFilenames(), job->getProcess(0)->getRedirFlags());
				}
				
				execvp(job->getProcess(0)->getCommand()[0], job->getProcess(0)->getCommand());
			}
		} else { // shell
			job->getProcess(0)->setPID(getpid());
			job->setPGID(getpid());
			setpgid (pid, pid);
			if (!job->inBG()) {
				
				fprintf (stderr, "%d em foreground\n", pid);
				if (waitpid(pid, &status, WUNTRACED) == -1){
					fprintf (stderr, "erro no waitpid\n");
				} else {
					if (WIFEXITED(status))	std::cout << "Filho " << pid << " morreu em paz " << WEXITSTATUS(status) << std::endl;
					else if (WIFSIGNALED(status))	std::cout << "Filho " << pid << " recebeu sinal " << WTERMSIG(status) << std::endl;
					else if (WIFSTOPPED(status))	std::cout << "Filho " << pid << " foi parado com sinal " << WSTOPSIG(status) << std::endl;
				}
			}
			// Pega os atributos do job
			tcgetattr(STDIN_FILENO, job->getTermios());
			fprintf (stderr, "fim da funcao %d\n", job->getPGID());
		}
	}
};

#endif
