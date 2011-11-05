#ifndef PIPER_H
#define PIPER_H

#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "Executor.hpp"
#include "Process.hpp"
class Piper {
public:
	Piper (void) {}
	//TODO: ver o que retornar
	void execPipe (Job* job) {
		pid_t pid;
		int size = job->size();
		int fd[size][2];
		int status, i;
		pipe (fd[0]);
		pid = fork();
		if (pid < 0)	throw -1;
		else if (pid > 0) {
			waitpid (pid, &status, WUNTRACED);
			close (fd[0][1]);
			dup2 (fd[0][0], 0);
			if (job->getProcess(size-1).hasRedirection()) {
				Redirector r;
				r.init (job->getProcess(size-1).getFilenames(), job->getProcess(size-1).getRedirFlags());
			}
			execvp (job->getProcess(size-1).getCommand()[0], job->getProcess(size-1).getCommand());
		} else {
			for (i = 1; i < size-1; i++) {
				pipe (fd[i]);
				pid = fork();
				if (pid < 0)	throw -1;
				else if (pid > 0) {
					waitpid (pid, &status, WUNTRACED);
					close (fd[i-1][0]);
					close (fd[i][1]);
					dup2 (fd[i][0],0);
					dup2 (fd[i-1][1], 1);
					if (job->getProcess(size-1-i).hasRedirection()) {
						Redirector r;
						r.init (job->getProcess(size-1-i).getFilenames(), job->getProcess(size-1-i).getRedirFlags());
					}
					execvp (job->getProcess(size-1-i).getCommand()[0], job->getProcess(size-1-i).getCommand());
				}
			}
			if (pid < 0)	throw -1;
			else if (pid == 0) {
				close (fd[size-2][0]);
				dup2 (fd[size-2][1],1);
				if (job->getProcess(0).hasRedirection()) {
					Redirector r;
					r.init (job->getProcess(0).getFilenames(), job->getProcess(0).getRedirFlags());
				}
				execvp (job->getProcess (0).getCommand()[0], job->getProcess(0).getCommand());
			}
		}
	}
};
#endif
