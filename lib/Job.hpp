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
#include "shooSHlib.hpp"


class Job {

	private:
		std::vector<Process>	process;
		pid_t 					pgid;
		pid_t					pid;
		char* 					info;
		std::string 			cmd;
		int						flag;

	public:

		Job(void);
		int			size(void);
		void		destroy (void);
		pid_t		getPID (void);
		std::string	getCommand (void);
		void 		setCommand (std::string command);
		void		createProcess (void);
		Process 	getProcess (int i);
		void    	setProcessCommand (char**, int);
		void    	setProcessFile (std::string, int);
		void		setPID (pid_t);
		void		print (void);
		void		addFlag (int);
		bool		inBg (void);
		bool		hasPipe (void);
		bool		hasExited (void);
		bool		isNop (void);
		bool		hasFailed (void);
};

#endif