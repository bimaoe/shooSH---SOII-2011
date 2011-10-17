#include <stdio.h>
#include <unistd>
#include <vector>
class Piper {
	Piper (void) {}
	//TODO: ver o que retornar
	void pipe (Job job) {
		int size = job.size;
		int fd[size][2];
		int status;
		pipe (fd[0]);
		pid = fork();
		if (pid < 0)	throw -1;
		else if (pid > 0) {
			waitpid (pid, &status, WUNTRACED);
			close (fd[0][1]);
			dup2 (fd[0][0], 0);
			execute (job.getProcess(size-1));
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
					execute (job.getProcess(size-1-i));
				}
			}
			if (pid < 0)	throw -1;
			else if (pid == 0) {
				close (fd[size-2][0]);
				dup2 (fd[size-2][1],1);
				execute (job.getProcess (0));
			}
		}
	}
};
