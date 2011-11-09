#ifndef SHOOSH_H
#define SHOOSH_H

#include <cstdio>
#include <list>
#include "Parser.hpp"
#include "Redirector.hpp"
#include "shooSH.hpp"
#include "Job.hpp"
#include "Process.hpp"
#include "Executor.hpp"
#include "Initializer.hpp"

class shooSH {
private:
	std::list<Job*>			jobList;
	std::list<std::string>	history;

public:
	shooSH (void) {}
	void run (void) {
		Job* job;
		Parser p;
		Executor executor;
		bool exited = false;
		Initializer initializer;

		while (!exited) {
			std::cout << "shooSH> ";
			job = p.parseLine ();
			history.push_back (job->getCommand());
			if (!(job->isNop()||job->hasExited())) {
				job->print();
				jobList.push_back (job);
				executor.execute (job);
			} else {
				exited = job->hasExited();
			}
			job = NULL;
		}
	}
};

#endif
