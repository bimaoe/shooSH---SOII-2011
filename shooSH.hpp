#include <string>
#include <unistd.h>
#include <cstdlib>
#include <vector>

#ifndef SHOOSH_H
#define SHOOSH_H


/****************************
 *	DEFINICOES DE SINAIS	*
 ****************************/
#define shooSH_EXIT		1
#define shooSH_PIPE		2
#define shooSH_REDIR	4

/********************************
 *	DEFINICOES DE ESTRUTURAS	*
 ********************************/

using namespace std;

class Process{					//fila

	private:
		char    **command;		//matriz de comandos
		int     cmdSize;
		pid_t   pid;
		char    finish;
		char    stop;
		int     status;
		string  filename;
		int     flag;

	public:

		Process();
		~Process();

		bool    processSearch();            //
		void    setCommand(char**, int);    //OK
		void    setFilename(string, int);   //OK

};

/**
 *	Job Qualquer
 *
 */
class Job{	//fila

	private:
        vector<Process>
                process;
		pid_t 	pgid;			//pra dar controle habilitado para jobs
		char* 	info;			//Qualquer notificacao existente
		string 	cmd;			//nome do comando
	public:

		Job();
		~Job();

		string	getCommand();                   //OK
		void 	setCommand( string );           //OK
		void	createProcess();                //OK
		Process getProcess(int i);              //OK

		void    setProcessCommand(char**, int); //OK
		void    setProcessFile(string, int);    //OK

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

void Process::setCommand(char** cmd, int size){

    this->command = cmd;
    cmdSize = size;
}

void Process::~Process(){
    for(int i=0; i<cmdSize; i++){
        free(command[i]);
    }
}


void setFilename(string filename, int flag){
    this->filename = filename;
    this->flag = flag;
}

/**
 *	CLASS JOB
 */

Job::Job(){

}

Job::~ Job(){

    p = processBegin;
    pn = p->getNext();

    //No problem, porque da break no meio. O ob
    while(!vector.empty()){

        delete process;
    }

}

void Job::createProcess(){

    Process p;

    process.push_back(p);
}

Process Job::getProcess(int i){
    return process[i];
}

void Job::setCommand(string cmd){
    this->cmd = cmd;
}

string Job::getCommand(){
    return cmd;
}
/*
 * coloca a matriz de comandos no ultimo processo inserido
 */
void Job::setProcessCommand(char** cmd, int size){
    process[process.size()-1].setCommand(cmd, size);
}

void Job::setProcessFile(string filename, int flag){
    process[process.size()-1].setFilename(filename, flag);
}

/**
 *	CLASS JOB LIST
 */




#endif
