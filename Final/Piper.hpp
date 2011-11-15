#ifndef PIPER_H
#define PIPER_H

#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "Executor.hpp"
#include "lib/Process.hpp"
#include "lib/Builtin.hpp"

/*!
 * Classe para execucao de comandos com pipe.
 */
class Piper {
public:
	Piper (void) {}

	/*!
	 *	\brief Executa um job com pipe.
	 *		Implementada como uma fila invertida, ou seja, o primeiro comando a ser executado sera o processo com maior PID.
	 *	\param job Ponteiro para o job a ser executado
	 *	\throw -1	Erro no fork
	 */
	void execPipe (Job* job) {
		pid_t pid;
		int size = job->size();
		int fd[size][2];
		int status, i;

		pipe (fd[0]);
		pid = fork();

		if (pid < 0)	throw -1;
		else if (pid > 0) { // executa o utimo comando
			job->getProcess (size-1)->setPID(getpid());

			if (waitpid (pid, &status, WUNTRACED) == -1) {
				fprintf (stderr, "Erro no waitpid (%d) do pipe\n", pid);
				exit (1);
			}
			
			// fecha descritor nao usado
			close (fd[0][1]);
			dup2 (fd[0][0], 0);

			if (job->getProcess(size-1)->hasRedirection()) {
				Redirector r;
				r.init (job->getProcess(size-1)->getFilenames(), job->getProcess(size-1)->getRedirFlags());
			}
			
			if (execvp (job->getProcess(size-1)->getCommand()[0], job->getProcess(size-1)->getCommand()) == -1) {
				std::cout << "Erro: comando inexistente" << std::endl;
				kill (-job->getPGID(), SIGKILL);
			}
		} else {
			for (i = 1; i < size-1; i++) {
				pipe (fd[i]);
				pid = fork();

				if (pid < 0)	throw -1;
				else if (pid > 0) {
					setpgid (getpid(), job->getPGID());
					job->getProcess (size-1-i)->setPID(getpid());

					if (waitpid (pid, &status, WUNTRACED) == -1) {
						fprintf (stderr, "Erro no waitpid (%d) do pipe\n", pid);
						exit (1);
					}
					
					// fecha descritores nao usados
					close (fd[i-1][0]);
					close (fd[i][1]);
					dup2 (fd[i][0],0);
					dup2 (fd[i-1][1], 1);

					if (job->getProcess(size-1-i)->hasRedirection()) {
						Redirector r;
						r.init (job->getProcess(size-1-i)->getFilenames(), job->getProcess(size-1-i)->getRedirFlags());
					}
					if (execvp (job->getProcess(size-1-i)->getCommand()[0], job->getProcess(size-1-i)->getCommand()) == -1) {
						std::cout << "Erro: comando inexistente" << std::endl;
						kill (-job->getPGID(), SIGKILL);
					}
				}
			}
			if (pid == 0) { //executa o primeiro comando
				close (fd[size-2][0]);
				dup2 (fd[size-2][1],1);

				setpgid (getpid(), job->getPGID());
				job->getProcess(0)->setPID(getpid());

				if (job->getProcess(0)->hasRedirection()) {
					Redirector r;
					r.init (job->getProcess(0)->getFilenames(), job->getProcess(0)->getRedirFlags());
				}
				if (execvp (job->getProcess (0)->getCommand()[0], job->getProcess(0)->getCommand()) == -1){
					std::cout << "Erro: comando inexistente" << std::endl;
					kill (-job->getPGID(), SIGKILL);
				}
			}
		}
	}
};
#endif
