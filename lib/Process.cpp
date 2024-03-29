#include "Process.hpp"

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

char** Process::getCommand (void) {
	return command;
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

bool Process::hasRedirection (void) {
	return redirflag[0] != -1 || redirflag[1] != -1 || redirflag[2] != -1;
}

std::string* Process::getFilenames (void) {
	return filename;
}

int* Process::getRedirFlags (void) {
	return redirflag;
}

void Process::setPID (pid_t p) {
	pid = p;
}

pid_t Process::getPID (void) {
	return pid;
}
