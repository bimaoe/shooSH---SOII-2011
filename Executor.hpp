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
		int status;
		pid = fork();
		if (pid < 0)	throw -1;
		else if (pid == 0) {
			if (job->hasPipe()) {
				Piper piper;
				piper.execPipe (job);
			} else {
				execvp(job->getProcess(0).getCommand()[0], job->getProcess(0).getCommand());
			}
		} else {
			if (job->inBg()) waitpid(pid, &status, WNOHANG);
			else {
				waitpid(pid, &status, WUNTRACED);
				if (WIFEXITED(status))	std::cout << "Child " << pid << " terminated normally with return value " << WEXITSTATUS(status) << std::endl;
				else if (WIFSIGNALED(status))	std::cout << "Child " << pid << " was terminated by signal " << WTERMSIG(status) << std::endl;
				else if (WIFSTOPPED(status))	std::cout << "Child " << pid << " was stopped by signal " << WSTOPSIG(status) << std::endl;
			}
		}
	}
};

#endif
