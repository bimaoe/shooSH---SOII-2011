#include "shooSH.hpp"

Process::Process() {
	command = NULL;
	cmdSize = 0;
	redirflag[0] = redirflag[1] = redirflag[2] = -1;
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
	if (command != NULL) {
		for(int i=0; i<cmdSize; i++){
			free(command[i]);
		}
		free(command);
	}
	command = NULL;
	cmdSize = 0;
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

