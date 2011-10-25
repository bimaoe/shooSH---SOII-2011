#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "Executor.hpp"
#include "Job.hpp"
#include "Process.hpp"
class Piper {
	Piper (void) {}
	//TODO: ver o que retornar
	void executePipe (Job job, bool bg) {
		pid_t pid;
		int status;
		pid = fork();
		if (pid < 0)	throw -1;
		else if (pid > 0) {
			if (bg)	waitpid(pid, &status, WNOHANG);
			else {
				if (WIFEXITED(status))  std::cout << "Child " << pid << " te    rminated normally with return value " << WEXITSTATUS(status) << std::endl;
				else if (WIFSIGNALED(status))   std::cout << "Child " << pid     << " was terminated by signal " << WTERMSIG(status) << std::endl;
				else if (WIFSTOPPED(status))    std::cout << "Child " << pid     << " was stopped by signal " << WSTOPSIG(status) << std::endl;
			}
		} else {
			int size = job.size();
			int fd[size][2];
			int status, i;
			Executor exec;
			pipe (fd[0]);
			pid = fork();
			if (pid < 0)	throw -1;
			else if (pid > 0) {
				waitpid (pid, &status, WUNTRACED);
				close (fd[0][1]);
				dup2 (fd[0][0], 0);
				exec.execute (job.getProcess(size-1));
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
						exec.execute (job.getProcess(size-1-i));
					}
				}
				if (pid < 0)	throw -1;
				else if (pid == 0) {
					close (fd[size-2][0]);
					dup2 (fd[size-2][1],1);
					exec.execute (job.getProcess (0));
				}
			}
		}
	}
};
