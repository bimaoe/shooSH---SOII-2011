#include "Builtin.hpp"

void jobs (void) {

	std::list<Job*>::iterator curr,end;
	for (curr = jobList.begin(), end = jobList.end(); curr != end; curr++) {
		(*curr)->print();
	}
}

void showHistory (void) {

	std::list<std::string>::iterator curr, end;
	
	for (curr = history.begin(), end = history.end(); curr != end; curr++) {
		std::cout << *curr << std::endl;
	}
}

void sendsigterm (int id) {
	std::list<Job*>::iterator curr, end;
	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->getID() != id; curr++);
	if (curr == end)	throw -id;
	else
		kill (-(*curr)->getPGID(), SIGTERM);
}

void echo (int argc, char* args[]) {
	int i;
	for (i = 1; i < argc-1; i++)
		printf ("%s ", args[i]);
	printf ("%s\n", args[i]);
}

void changeDirectory (const char* dir) {

	if (chdir (dir) == -1)
		std::cout << "cd: Diretorio \"" << dir << "\" invalido" << std::endl;
}

void showDirectoryName (void) {

	char buf[256];
	if (buf != getcwd(buf, 256))	throw -1; //buffer overflow
	printf ("%s\n", buf);
}

void foreground (int id) {
	std::list<Job*>::iterator curr, end;
	int status;
	int err;
	pid_t pid;

	// procura o job
	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->getID() != id; curr++);

	if (curr == end) {
		throw -id;
	} else {
		(*curr)->setBG(false);
		tcsetattr(STDIN_FILENO, TCSADRAIN, (*curr)->getTermios());
		tcsetpgrp (STDIN_FILENO, (*curr)->getPGID());
		
		kill (-(*curr)->getPGID(), SIGCONT);
		
		while ((pid = waitpid ((*curr)->getPGID(), &status, WUNTRACED)) == -1 && (err = errno) == EINTR);
		if (pid == -1) {
			fprintf (stderr, "Erro: fg: ");
			if (err == ECHILD)	fprintf (stderr, "processo (%d) nao existe ou nao eh meu filho\n", (*curr)->getPGID());
			else	fprintf (stderr, "EINVAL\n");
		} else {
			tcgetattr (STDIN_FILENO, (*curr)->getTermios());
			tcsetattr(STDIN_FILENO, TCSADRAIN, &shooSHTermios);
			tcsetpgrp (STDIN_FILENO, shooSHPGID);
		}
	}
}

void background (int id) {
	std::list<Job*>::iterator curr, end;
	for (curr = jobList.begin(), end = jobList.end(); curr != end && (*curr)->getID() != id; curr++);
	if (curr == end) {
		throw -id;
	} else {
		(*curr)->setBG(true);
		// shooSH volta ao terminal
		tcsetattr (STDIN_FILENO, TCSADRAIN, &shooSHTermios);
		tcsetpgrp (STDIN_FILENO, shooSHPGID);
		kill (-(*curr)->getPGID(), SIGCONT);
		
	}
}

int executeBuiltin (Process *p) {
	char** cmd = p->getCommand();
	int size = p->size();
	int id;
	
	if (!strcmp (cmd[0], "fg")) {
		if (size < 2) {
			std::cout << "fg: ID esperado" << std::endl;
			return 1;
		}
		if (cmd[1][0] != '%') {
			std::cout << "fg: \% esperado" << std::endl;
			return 1;
		}
		sscanf (cmd[1], "%*c%d", &id);
		try {
			foreground (id);
		} catch (int err) {
			fprintf (stderr, "fg: %d inexistente\n", -err);
			return 2;
		}
	} else if (!strcmp (cmd[0], "bg")) {
		if (size < 2) {
			std::cout << "bg: ID esperado" << std::endl;
			return 1;
		}
		if (cmd[1][0] != '%') {
			std::cout << "bg: \% esperado" << std::endl;
			return 1;
		}
		sscanf (cmd[1], "%*c%d", &id);
		try {
			background (id);
		} catch (int err) {
			fprintf (stderr, "bg: %d inexistente\n", -err);
			return 2;
		}
	} else if (!strcmp (cmd[0], "jobs")) {
		jobs();
	} else if (!strcmp (cmd[0], "pwd")) {
		showDirectoryName();
	} else if (!strcmp (cmd[0], "cd")) {
		if (size < 2) {
			std::cout << "cd: Nome do diretorio esperado"  << std::endl;
			return 1;
		}
		changeDirectory (cmd[1]);
	} else if (!strcmp (cmd[0], "history")) {
		showHistory();
	} else if (!strcmp (cmd[0], "echo")) {
		echo (size, cmd);
	} else if (!strcmp (cmd[0], "kill")) {
		if (size < 2) {
			std::cout << "kill: ID esperado" << std::endl;
			return 1;
		}
		if (cmd[1][0] != '%')	return -1;
		sscanf (cmd[1], "%*c%d", &id);
		try {
			sendsigterm (id);
		} catch (int err) {
			fprintf (stderr, "kill: %d inexistente\n", -err);
			return 2;
		}
	} else	return -1;
	return 0;
}

