#ifndef SHOOSH_H
#define SHOOSH_H

#include <cstdio>
#include <list>
#include <unistd.h>
#include "Process.hpp"
#include "Parser.hpp"
#include "Redirector.hpp"
#include "Job.hpp"
#include "Executor.hpp"
#include "Initializer.hpp"
#include "shooSHlib.hpp"

int currID; //!< ID do ultimo Job criado.

/*!
 *	\brief Funcao de inicializacao da Shell
 */
void shooSH_init (void) { 
	init_tty();	
	currID = 0;
}

/*!
 *	\brief Funcao para liberar os recursos alocados pela Shell
 */
void shooSH_clean(void) {
	std::list<Job*>::iterator curr, end;
	for (curr =jobList.begin(), end = jobList.end(); curr != end; curr++) {
		(*curr)->destroy();
	}
	jobList.clear();
}

/*!
 *	\brief Funcao que executa a Shell.
 */
void shooSH_run (void) {
	Job* job;
	Parser p;
	Executor executor;
	bool exited = false;
	
	while (!exited) {
		std::cout << "shooSH> ";
		job = p.parseLine ();
		history.push_back (job->getCommand());
		if (job->hasFailed ()) {
			std::cout << "Erro de sintaxe" << std::endl;
		} else {
			if (!(job->isNop()||job->hasExited())) {
				if (!job->hasPipe()) {
					if (executeBuiltin (job->getProcess(0)) == -1) {
						job->setID (++currID);
						jobList.push_back (job);
						executor.execute (job);
					}
				} else {
					job->setID (++currID);
					jobList.push_back (job);
					executor.execute (job);
				}
			} else {
				exited = job->hasExited();
			}
		}
		job = NULL;
	}
	shooSH_clean();
}

#endif
