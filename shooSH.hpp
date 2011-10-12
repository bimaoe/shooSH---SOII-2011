#ifndef SHOOSH_H
#define SHOOSH_H

#include <string>
#include <unistd.h>
#include <cstdlib>
#include <vector>

/****************************
 *	DEFINICOES DE SINAIS	*
 ****************************/
#define shooSH_EXIT		1
#define shooSH_PIPE		2
#define shooSH_REDIR	4
#define shooSH_NOP		8

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
		std::string	filename;
		int     	flag;

	public:

		Process();
		~Process();

		void    setCommand(char**, int);    //OK
		void    setFilename(std::string, int);   //OK

};

/**
 *	Job Qualquer
 *
 */
class Job{	//fila

	private:
        std::vector<Process>	process;
		pid_t 					pgid;			//pra dar controle habilitado para jobs
		char* 					info;			//Qualquer notificacao existente
		std::string 			cmd;			//nome do comando
	public:

		Job();

		std::string	getCommand();                   //OK
		void 		setCommand(std::string command);//OK
		void		createProcess();                //OK
		Process 	getProcess(int i);              //OK

		void    	setProcessCommand(char**, int); //OK
		void    	setProcessFile(std::string, int);//OK

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
	command = NULL;
	cmdSize = 0;
}

Process::~Process(){
	if (command != NULL) {
		printf ("%d\n", cmdSize);
		for(int i=0; i<cmdSize; i++){
			free(command[i]);
		}
		free(command);
		printf("popo\n");
	}
	command = NULL;
	cmdSize = 0;
}

void Process::setCommand(char** cmd, int size){

    command = cmd;
    cmdSize = size;
}

void Process::setFilename(std::string filename, int flag){
    this->filename = filename;
    this->flag = flag;
}

/**
 *	CLASS JOB
 */

Job::Job(){

}

void Job::createProcess(){
    process.push_back(Process());
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
