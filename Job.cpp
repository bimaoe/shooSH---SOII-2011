#include "Job.hpp"

Job::Job(void){
	flag = 0;
}

struct termios *Job::getTermios(){
	return &modes;
}

void Job::destroy(void) {
	int i;
	for (i = 0; i < process.size(); i++) {
		process[i].destroy();
	}
}

void Job::print (void) {
	int i, size;
	std::cout << '[' << id << "] " << cmd << std::endl;
	/**std::cout << "Line: " <<  cmd << std::endl;
	for (i = 0, size = process.size(); i < size; i++) {
		printf ("p%d\n", i);
		process[i].print();
	}/**/
}

void Job::createProcess(void){
	Process p;
    process.push_back(p);
}

Process Job::getProcess(int i){
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

void Job::setProcessCommand(char** cmd, int size){
    process[process.size()-1].setCommand(cmd, size);
}

void Job::setProcessFile(std::string filename, int flag){
    process[process.size()-1].setFilename(filename, flag);
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

void Job::setPID (pid_t p) {
	pid = p;
}

pid_t Job::getPID (void) {
	return pid;
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
