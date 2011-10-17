#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include "Redirector.hpp"

class Process{

	private:
		char    	**command;
		int     	cmdSize;
		pid_t   	pid;
		char    	finish;
		char    	stop;
		int     	status;
		std::string	filename[3];
		int     	redirflag[3];

	public:

		Process(void);
		void	destroy (void);
		void    setCommand(char**, int);
		void    setFilename(std::string, int);
		void	print (void);
		void*	get (void) {return (void*) &command;}
};

#endif
