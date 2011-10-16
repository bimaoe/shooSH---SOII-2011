#ifndef SHOOSH_H
#define SHOOSH_H

#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include "Redirector.hpp"

#define shooSH_EXIT		1
#define shooSH_PIPE		2
#define shooSH_REDIR	4
#define shooSH_NOP		8
#define shooSH_FAIL		16

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
		void* get (void) {return (void*) &command;}
};


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


class JobList{
	private:
		Job	*first,
			*last;

	public:
		JobList();
		~JobList();

		void addJob();


};

#endif
