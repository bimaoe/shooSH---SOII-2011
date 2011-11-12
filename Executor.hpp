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
		Description:
			Executes a process calling fork and exec
		Parameters:
			Process	process to be executed
			bool	flag of foreground or background
		Return:
			Nothing.
		Throws:
			-1		error in call of fork
	*/
	
	void execute (Job* job) {
	 	pid_t pid;
		int status, err;
		pid = fork();
		if (pid < 0)	throw -1;
		else if (pid == 0) { // executor
			if (job->hasPipe()) {
				Piper piper;
				piper.execPipe (job);
			} else {
				pid = getpid();
				job->setPGID(pid);
				job->getProcess(0).setPID(pid);
				if (!job->inBG())
					tcsetpgrp (STDIN_FILENO, job->getPGID());
				if (job->getProcess(0).hasRedirection()) {
					Redirector r;
					r.init (job->getProcess(0).getFilenames(), job->getProcess(0).getRedirFlags());
				}
				execvp(job->getProcess(0).getCommand()[0], job->getProcess(0).getCommand());
			}
		} else { // shell
			if (!job->inBG()) {
				printf ("nao estah em bg. estou esperando %d\n", SIGTSTP);
				if (waitpid(pid, &status, WUNTRACED) == -1){
					printf ("error on wuntraced\n");
//					err = errno;
//					if (err == ECHILD)
//						printf ("process does not exist or is not my child\n");
//					else if (err == EINTR) 
//						printf ("SIGCHLD was caught\n");
//					else
//						printf ("EINVAL\n");
				} else {
					if (WIFEXITED(status))	std::cout << "Child " << pid << " terminated normally with return value " << WEXITSTATUS(status) << std::endl;
					else if (WIFSIGNALED(status))	std::cout << "Child " << pid << " was terminated by signal " << WTERMSIG(status) << std::endl;
					else if (WIFSTOPPED(status))	std::cout << "Child " << pid << " was stopped by signal " << WSTOPSIG(status) << std::endl;
				}
			} else {
				waitpid (pid, &status, WNOHANG);
			}
			tcgetattr(STDIN_FILENO, job->getTermios());
		}
	}
};

#endif
