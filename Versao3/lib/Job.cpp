#include "Job.hpp"

Job::Job(void){
	flag = 0;
	stopped = false;
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
	std::cout << '[' << id << "]\t";
	if (stopped)
		std::cout << "Parado";
	else
		std::cout << "Ativo";
	std::cout <<  "\t\t" << cmd << ' ' << (inBG()?'&':' ');
	std::cout << std::endl;
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
	if (isBG)
		flag |= (-1 & shooSH_BG);
	else
		flag &= (0 & shooSH_BG);
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
