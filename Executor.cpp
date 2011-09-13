#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <vector>
#include <iostream>


class Executor {
public:
	Executor (void) {};
	int execute (char** cmd, bool bg) {
		pid_t pid;
		int status;
		pid = fork();
		if (pid < 0)	throw -1;
		else if (pid == 0)	execvp(cmd[0], cmd);
		else {
			if (bg) waitpid(pid, &status, WNOHANG);
			else {
				waitpid(pid, &status, WUNTRACED);
				if (WIFEXITED(status))	std::cout << "Child " << pid << " terminated normally with return value " << WEXITSTATUS(status) << std::endl;
				else if (WIFSIGNALED(status))	std::cout << "Child " << pid << " was terminated by signal " << WTERMSIG(status) << std::endl;
				else if (WIFSTOPPED(status))	std::cout << "Child " << pid << " was stopped by signal " << WSTOPSIG(status) << std::endl;
			}
		}
	}
};

int main(int argc, char* argv[]){

	char* comando[] = {"xeyes", NULL}; //dispensavel
	Executor ex;
	ex.execute(comando, argc > 1);
}
