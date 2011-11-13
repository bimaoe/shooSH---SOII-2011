#ifndef JOB_H
#define JOB_H

#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <termios.h>
#include "Redirector.hpp"
#include "Process.hpp"
#include "shooSHlib.hpp"


class Job {

	private:
		std::vector<Process*>	process; //vetor de ponteiros dos processos do job
		pid_t 					pgid; //group id
		char* 					info; //nao usado
		std::string 			cmd; //linha de comando
		int						flag; //flag de bg, pipe, exit, nop
		int 					id; //internal id
		struct termios 			modes; //atributos
		bool					stopped;

	public:
		Job(void);
		void			createProcess (void);
		/*
		 *	Desaloca todos os recursos
		 */
		void			destroy (void);

		std::string		getCommand (void);
		int				getID (void);
		pid_t			getPGID (void);
		Process*		getProcess (int i);
		struct termios*	getTermios();
		bool			hasPipe (void);
		bool			hasExited (void);
		bool			hasFailed (void);
		bool			inBG (void);
		bool			isNop (void);
		bool			isStopped (void);
		int				size(void);

		void			addFlag (int);
		void			cont (void);
		void			setBG (bool);
		void			setCommand (std::string command);
		void			setID (int);
		void			setPGID (pid_t);
		void			setProcessCommand (char**, int);
		void			setProcessFile (std::string, int);
		void			stop (void);

		void			print (void);
};

Job::Job(void){
	flag = 0;
}

struct termios *Job::getTermios(){
	return &modes;
}

void Job::cont (void) {
	stopped = false;
}

void Job::stop (void) {
	stopped = true;
}

bool Job::isStopped (void) {
	return stopped;
}

void Job::destroy(void) {
	int i;
	for (i = 0; i < process.size(); i++) {
		process[i]->destroy();
		delete process[i];
	}
	process.clear();
}

void Job::print (void) {
	int i, size;
	std::cout << '[' << id << "]\t" << cmd << std::endl;
	/**std::cout << "Line: " <<  cmd << std::endl;
	for (i = 0, size = process.size(); i < size; i++) {
		printf ("p%d\n", i);
		process[i]->print();
	}/**/
}

void Job::createProcess(void){
    process.push_back(new Process());
}

Process* Job::getProcess(int i){
    return process[i];
}

void Job::setCommand(std::string command){
    cmd = command;
}

std::string Job::getCommand(void){
    return cmd;
}

int Job::size (void) {
	return process.size();
}

/*
 *	cmd = matriz de comandos terminada em NULL
 *	size = numero de posicoes da matriz usada sem contar o NULL
 */
void Job::setProcessCommand(char** cmd, int size){
    process[process.size()-1]->setCommand(cmd, size);
}

void Job::setProcessFile(std::string filename, int flag){
    process[process.size()-1]->setFilename(filename, flag);
}

void Job::addFlag (int newflag) {
	flag |= newflag;
}

bool Job::inBG(void) {
	return (flag & shooSH_BG) != 0;
}

bool Job::hasPipe (void) {
	return (flag & shooSH_PIPE) != 0;
}

bool Job::isNop (void) {
	return (flag & shooSH_NOP) != 0;
}

bool Job::hasFailed (void) {
	return (flag & shooSH_FAIL) != 0;
}

bool Job::hasExited (void) {
	return (flag & shooSH_EXIT) != 0;
}

void Job::setBG (bool isBG) {
	flag |= (isBG & shooSH_BG);
}

void Job::setID (int jid) {
	id = jid;
}

int Job::getID (void) {
	return id;
}

pid_t Job::getPGID (void) {
	return pgid;
}

void Job::setPGID (pid_t gid) {
	pgid = gid;
}
#endif
