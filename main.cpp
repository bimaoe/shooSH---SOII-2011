#include "Parser.hpp"
#include "Redirector.hpp"
#include "shooSH.hpp"
#include "Job.hpp"
#include "Process.hpp"
#include "Executor.hpp"
#include "Piper.hpp"
#include "Initializer.hpp"
#include <cstdio>

int main (void) {
	Job *job;
	Parser p;
	Piper piper;
	Executor executor;
	bool exited = false;
	Initializer initializer;
	//initializer.init();
	while (!exited) {
		printf ("shooSH>");
		job = p.parseLine ();
		if (!(job->isNop()||job->hasExited())) {
			job->print();
			printf ("Executando\n");
			executor.execute (job);
			printf ("Acabou a execução\n");
		} else {
			exited = job->hasExited();
		}
		delete job;
	}
	return 0;
}
