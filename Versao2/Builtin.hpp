#ifndef BUILTIN_H
#define BUILTIN_H

#include <stdio.h>
#include "shooSHlib.hpp"
#include "Initializer.hpp"
	
std::list<std::string>	history;

/*
 *	Mostra todos os jobs da lista
 */
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

/*
 *	Muda o diretorio atual
 *	
 *	\Param
 *		Diretorio para o qual ocorrera a mudanca
 */
void changeDirectory (const char* dir) {

	if (chdir (dir) == -1)
		std::cout << "cd: Diretorio \"" << dir << "\" invalido" << std::endl;
}

/*
 *	Mostra o diretorio atual
 *	
 *	\Exception
 *		-1	Diretorio maior que 256 caracteres
 */
void showDirectoryName (void) {

	char buf[256];
	if (buf != getcwd(buf, 256))	throw -1; //buffer overflow
	printf ("%s\n", buf);
}

/*
 *	Coloca o job em execucao
 *
 *	\Param
 *		ID do job
 *	\Exception
 *		-ID	Job nao encontrado
 */
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
		
		kill (-(*curr)->getPGID(), SIGCONT); /*TODO descobrir como se volta o processo para fg*/
		while ((pid = waitpid (WAIT_ANY, &status, WUNTRACED)) == -1 && (err = errno) == EINTR);
		if (pid == -1) {
			fprintf (stderr, "Erro: fg: ");
			if (err == ECHILD)	fprintf (stderr, "processo (%d) nao existe ou nao eh meu filho\n", (*curr)->getPGID());
			else if (err == EINTR)	fprintf (stderr, "SIGCHLD provavelmente ferrou tudo\n");
			else	fprintf (stderr, "EINVAL\n");
		} else {
			tcgetattr (STDIN_FILENO, (*curr)->getTermios());
			tcsetattr(STDIN_FILENO, TCSADRAIN, &shooSHTermios);
			tcsetpgrp (STDIN_FILENO, shooSHPGID);
			
			/*if (WIFEXITED(status)) {
				fprintf (stderr, "Filho %d saiu com %d\n", pid, WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				fprintf (stderr, "Filho %d foi terminado por um sinal %d\n", (*curr)->getPGID(),  WTERMSIG(status)==SIGTSTP);
			} else if (WIFSTOPPED(status)) {
				fprintf (stderr, "Filho %d estava stopped ainda %d\n", (*curr)->getPGID(), WSTOPSIG(status));
			} else if (WIFCONTINUED(status)) {
				fprintf (stderr, "Filho %d continuou\n", (*curr)->getPGID());
			}*/
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


/*
 *	Executa comandos builtin
 *
 *	\Param
 *		Processo a ser executado
 *	\Return
 *		-1	Comando nao eh builtin
 *		1	Faltam parametros
 *		2	Erro nos parametros
 *		0	Sucesso
 */
int executeBuiltin (Process *p) {
	char** cmd = p->getCommand();
	int size = p->size();
	int id;
	
	if (!strcmp (cmd[0], "fg")) {
		if (size < 2) {
			std::cout << "fg: ID esperado" << std::endl;
			return 1;
		}
		sscanf (cmd[1], "%d", &id);
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
		sscanf (cmd[1], "%d", &id);
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
	} else if (!strcmp (cmd[0], "fg")) {
	} else if (!strcmp (cmd[0], "fg")) {
	} else if (!strcmp (cmd[0], "fg")) {
	} else	return -1;
	return 0;
}

#endif
