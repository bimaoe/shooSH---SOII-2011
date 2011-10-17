#include "Job.hpp"

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
    process.push_back(p);
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
void Job::setProcessCommand(char** cmd, int size){
    process[process.size()-1].setCommand(cmd, size);
}

void Job::setProcessFile(std::string filename, int flag){
    process[process.size()-1].setFilename(filename, flag);
}
