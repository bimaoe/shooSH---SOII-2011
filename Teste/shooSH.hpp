#ifndef SHOOSH_H
#define SHOOSH_H

#include <string>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include "Redirector.hpp"

/****************************
 *	DEFINICOES DE SINAIS	*
 ****************************/
#define shooSH_EXIT		1
#define shooSH_PIPE		2
#define shooSH_REDIR	4
#define shooSH_NOP		8
#define shooSH_FAIL		16

/********************************
 *	DEFINICOES DE ESTRUTURAS	*
 ********************************/

class Process{					//fila

	private:
		char    	**command;		//matriz de comandos
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
		void    setCommand(char**, int);    //OK
		void    setFilename(std::string, int);   //OK
		void	print (void);
		void* get (void) {return (void*) &command;}
};

/**
 *	Job Qualquer
 *
 */
class Job {	//fila

	private:
        std::vector<Process>	process;
		pid_t 					pgid;			//pra dar controle habilitado para jobs
		char* 					info;			//Qualquer notificacao existente
		std::string 			cmd;			//nome do comando
	public:

		Job(void);
		void		destroy (void);
		std::string	getCommand(void);                   //OK
		void 		setCommand(std::string command);//OK
		void		createProcess(void);                //OK
		Process 	getProcess(int i);              //OK

		void    	setProcessCommand(char**, int); //OK
		void    	setProcessFile(std::string, int);//OK
		void		print (void);

};

/**
 *	Job List
 *
 *	Lista com a cabeca e a cauda da lista de jobs
 */

class JobList{
	private:
		Job	*first,
			*last;

	public:
		JobList();
		~JobList();

		void addJob();


};

/********************
 *	IMPLEMENTACOES	*
 ********************/


/**
 *	CLASS PROCESS
 */

Process::Process() {
//	printf ("proc constructor\n");
//	printf ("%p\n", &command);
	command = NULL;
	cmdSize = 0;
	redirflag[0] = redirflag[1] = redirflag[2] = -1;
//	printf ("endpc\n");
}

void Process::print(void) {
	int i;
	printf ("Commands:\n");
	for (i = 0; i < cmdSize; i++) {
		printf ("%s\n", command[i]);
	}
	printf ("Redirection:\n");
	for (i = 0; i < 3; i++)
		if (redirflag[i] != -1)	std::cout << i << " " << filename[i] << std::endl;
}

void Process::destroy(void) {
//	printf ("process destructor %p\n", &command);
	if (command != NULL) {
//		printf ("\tcmdSize = %d -- %p\n", cmdSize, command);
		for(int i=0; i<cmdSize; i++){
//			printf ("%d - %s\n", i, command[i]);
			free(command[i]);
		}
		free(command);
//		printf("\tpopo\n");
	}
	command = NULL;
	cmdSize = 0;
//	printf ("endpd\n");
}

void Process::setCommand(char** cmd, int size){

    command = cmd;
    cmdSize = size;
}

void Process::setFilename(std::string file, int flag){
	if (flag == REDIN) {
		filename[0] = file;
		redirflag[0] = flag;
	} else if (flag == REDOUTT || flag == REDOUTA) {
		filename[1] = file;
		redirflag[1] = flag;
	} else if (flag == REDERRT || flag == REDERRA) {
		filename[2] = file;
		redirflag[2] = flag;
	}
}

/**
 *	CLASS JOB
 */

Job::Job(void){

}

void Job::destroy(void) {
	int i;
	for (i = 0; i < process.size(); i++) {
		process[i].destroy();
	}
}

void Job::print (void) {
	int i, size;
	std::cout << "Line: " <<  cmd << std::endl;
	for (i = 0, size = process.size(); i < size; i++) {
		printf ("p%d\n", i);
		process[i].print();
	}
}

void Job::createProcess(){
	Process p;
//	printf ("\tcriando processo\n");
//	printf ("\t%p\n", p.get());
    process.push_back(p);
//	printf ("\t%d %p\n", process.size(), process[process.size()-1].get());
//	printf ("\tendcp\n");
}

Process Job::getProcess(int i){
    return process[i];
}

void Job::setCommand(std::string command){
    cmd = command;
}

std::string Job::getCommand(){
    return cmd;
}
/*
 * coloca a matriz de comandos no ultimo processo inserido
 */
void Job::setProcessCommand(char** cmd, int size){
    process[process.size()-1].setCommand(cmd, size);
}

void Job::setProcessFile(std::string filename, int flag){
    process[process.size()-1].setFilename(filename, flag);
}

/**
 *	CLASS JOB LIST
 */




#endif
