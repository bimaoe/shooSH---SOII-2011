#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>
#include "Job.hpp"
#include "Process.hpp"
#include "Redirector.hpp"
#include "shooSHlib.hpp"

#define NUM_WORDS		100
#define MAX_LENGTH		1024
#define MAX_FILENAME	128

// Estados da maquina de estados
#define PARSCMD			0
#define PARSNEXT		1
#define PARSPARAM		2
#define PARSRIN			3
#define PARSROUT		4
#define PARSTWO			5
#define PARSRERR		6
#define PARSFILENAME	7
#define PARSSUCCESS		8
#define PARSFAIL		9

class Parser {
	private:
		int		currState;
		Job*	job;
		char**	cmdList;
		int		cmdListSize;
		int		currChar;
		char	line[MAX_LENGTH];
		char	filename[MAX_FILENAME];

		void initialize (void);
		void newWord (void);
		void endWord (void);
		void newProcess (void);
		void endProcess (void);

	public:
		Parser (void);
		/* 
		 * 	Pega uma linha de comando
		 *	
		 * 	Retorno
		 *		Ponteiro para o job gerado
		 */
		Job* parseLine (void);
};

void Parser::initialize (void) {
	cmdListSize = -1;
	currChar = 0;
	cmdList = (char**) malloc (sizeof(char*)*NUM_WORDS);
}
void Parser::newWord (void) {
	cmdList[++cmdListSize] = (char*)malloc(sizeof(char)*MAX_LENGTH);
	currChar = 0;
}
void Parser::endWord (void) {
	cmdList[cmdListSize][currChar] = '\0';
}
void Parser::newProcess (void) {
	initialize();
	job->createProcess();
}
void Parser::endProcess (void) {
	cmdList[++cmdListSize] = (char*)NULL;
	job->setProcessCommand (cmdList, cmdListSize);
}

Parser::Parser (void) {
}

Job* Parser::parseLine (void) {
	job = new Job();
	currState = PARSCMD;
	int i, length, curr;
	int rflag;
	
	// Limpa o buffer de entrada para evitar loop infinito
	strcpy(line, "\0");
	std::cin.clear();
	
	std::cin.getline (line, MAX_LENGTH);
	
	i = 0;
	while (i < length && line[i] == ' ')	i++;
	length = strlen(line+i);
	// check for blank line
	if (length == 0) {
		job->addFlag (shooSH_NOP);
		return job;
	} else {
		while (line[--length] == ' '); // pega os espacos do fim
		length++;
		if (strcmp (line+i, "exit") == 0) { //builtin
			job->addFlag (shooSH_EXIT);
			return job;
		}
	}
	if ((line+i)[length-1] == '&')
		job->setCommand (std::string (line+i, length-1));
	else
		job->setCommand (std::string (line+i, length));
		
	while (currState != PARSSUCCESS && currState != PARSFAIL) {
		if (currState == PARSCMD) {
			newProcess();
			newWord();
			rflag = 0;
			// pega espacos
			while (i < length && line[i] == ' ')	i++;
			// esperava um comando, mas nao veio nada (usado para pipe)
			if (i == length) {
				currState = PARSFAIL;
			} else {
				// le ate encontrar um espaco
				while (i < length && line[i] != ' ')	cmdList[cmdListSize][currChar++] = line[i++];
				currState = PARSNEXT;
			}
			endWord();
		} else if (currState == PARSNEXT) {
			// le espaco
			while (i < length && line[i] == ' ')	i++;
	
			if (i == length) { //chegou ao fim da linha
				currState = PARSSUCCESS;
				endProcess();
			} else {
				// procura token
				switch (line[i]) {
					case '<':
						i++;
						currState = PARSRIN;
						break;
					case '>':
						i++;
						currState = PARSROUT;
						break;
					case '2': //pode ser argumento ou redirecionamento de erro
						i++;
						currState = PARSTWO;
						break;
					case '|':
						i++;
						job->addFlag (shooSH_PIPE);
						endProcess();
						currState = PARSCMD;
						break;
					case '&':
						i++;
						job->addFlag (shooSH_BG);
						endProcess();
						currState = PARSSUCCESS;
						break;
					default:
						currState = PARSPARAM;
				}
			}
		} else if (currState == PARSPARAM) {
			newWord();
			while (i < length && line[i] != ' ' && line[i] != '\"' && line[i] != '\'')	cmdList[cmdListSize][currChar++] = line[i++];
			if (line[i] == '\"') {
				i++;
				while (i < length && line[i] != '\"')	cmdList[cmdListSize][currChar++] = line[i++];
				if (i == length) {
					endWord();
					currState = PARSFAIL;
				} else {
					i++;
					endWord();
					currState = PARSNEXT;
				}
			} else if (line[i] == '\'') {
				i++;
				while (i < length && line[i] != '\'')	cmdList[cmdListSize][currChar++] = line[i++];
				if (i == length) {
					endWord();
					currState = PARSFAIL;
				} else {
					i++;
					endWord();
					currState = PARSNEXT;
				}
			} else {
				endWord();
				currState = PARSNEXT;
			}
		} else if (currState == PARSTWO) {
			if (line[i] == '>') {
				i++;
				endWord();
				currState = PARSRERR;
			} else {
				i--;
				currState = PARSPARAM;
			}
			endWord();
		} else if (currState == PARSRERR) {
			if (line[i] == '>') { // append
				i++;
				rflag = REDERRA;
			} else { // trunc
				rflag = REDERRT;
			}
			currState = PARSFILENAME;
		} else if (currState == PARSRIN) {
			rflag = REDIN;
			currState = PARSFILENAME;
		} else if (currState == PARSROUT) {
			if (line[i] == '>') { // append
				i++;
				rflag = REDOUTA;
			} else { // trunc
				rflag = REDOUTT;
			}
			currState = PARSFILENAME;
		} else if (currState == PARSFILENAME) {
			while (i < length && line[i] == ' ')	i++;
			if (i == length)	currState = PARSFAIL; // TODO admitting the filename will be always valid
			else {
				curr = 0;
				if (line[i] == '"') { // get filenames like "file name"
					i++;
					while (i < length && line[i] != '"')	filename[curr++] = line[i++];
					if (i == length)	currState = PARSFAIL;
					else	i++;
				} else if (line[i] == '\'') { // get filenames like 'file name'
					i++;
					while (i < length && line[i] != '\'')	filename[curr++] = line[i++];
					if (i == length)	currState = PARSFAIL;
					else	i++;
				} else {
					while (i < length && line[i] != ' ')	filename[curr++] = line[i++];
				}
				filename[curr] = '\0';
				job->setProcessFile (filename, rflag);
				currState = PARSNEXT;
			}
		}
	}
	if (currState == PARSFAIL)	job->addFlag (shooSH_FAIL);
	return job;
}


#endif
