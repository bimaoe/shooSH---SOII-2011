#include "Parser.hpp"
#include "Redirector.hpp"
#include "shooSH.hpp"
#include <cstdio>

int main (void) {
	Job *job;
	Parser p;
	int flags;
	while (1) {
		job = p.parseLine (&flags);
		if (job != NULL) {
			job->print();
			delete job;
		}
	}
	return 0;
}
