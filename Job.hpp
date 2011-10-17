#ifndef JOB_H
#define JOB_H

#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include "Redirector.hpp"
#include "Process.hpp"


class Job {

	private:
        std::vector<Process>	process;
		pid_t 					pgid;
		char* 					info;
		std::string 			cmd;
	public:

		Job(void);
		void		destroy (void);
		std::string	getCommand (void);
		void 		setCommand (std::string command);
		void		createProcess (void);
		Process 	getProcess (int i);

		void    	setProcessCommand (char**, int);
		void    	setProcessFile (std::string, int);
		void		print (void);

};

#endif
