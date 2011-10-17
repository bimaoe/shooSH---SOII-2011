#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>
#include "shooSH.hpp"
#include "Redirector.hpp"

#define NUM_WORDS		100
#define MAX_LENGTH		1024
#define MAX_FILENAME	128

// states of the state machine
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
	int currState;
	Job *job;
	char** cmdList;
	int cmdListSize, currChar;
	char line[MAX_LENGTH], filename[MAX_FILENAME];

	void initialize (void) {
		cmdListSize = -1;
		currChar = 0;
		cmdList = (char**) malloc (sizeof(char*)*NUM_WORDS);
//		printf ("initialize %p\n", cmdList);
	}
	void newWord (void) {
		cmdList[++cmdListSize] = (char*)malloc(sizeof(char)*MAX_LENGTH);
//		printf ("new word : %d\n", cmdListSize);
		currChar = 0;
	}
	void endWord (void) {
		cmdList[cmdListSize][currChar] = '\0';
	}
	void newProcess (void) {
		initialize();
//		printf ("creating process\n");
		job->createProcess();
//		printf ("endcp\n");
	}
	void endProcess (void) {
		cmdList[++cmdListSize] = (char*)NULL;
//		printf ("cmdlistsize: %d\n", cmdListSize);
		job->setProcessCommand (cmdList, cmdListSize);
	}

public:
	Parser (void) {
		currState = PARSCMD;
	}
	

	/* 
	 *	Description
	 * 		Parse a command line
	 *	Parameters
	 *		Pointer to integer to hold the generated flags
	 * 	Return
	 *		Job of the parsed line
	 */
	Job* parseLine (int* out_flags) {
		job = new Job();
		currState = PARSCMD;
		int i, length, curr;
		int rflag;
		*out_flags = 0;

		std::cin.getline (line, MAX_LENGTH);
		std::cout << line << std::endl;
		i = 0;
		while (i < length && line[i] == ' ')	i++;
		length = strlen(line+i);
		// check for blank line
		if (length == 0) {
//			printf ("saiu no nop\n");
			(*out_flags) |= shooSH_NOP;
			return job;
		} else {
			if (strcmp (line+i, "exit") == 0) {
				*out_flags |= shooSH_EXIT;
				return job;
			}
		}
		job->setCommand (std::string (line+i));
		while (currState != PARSSUCCESS && currState != PARSFAIL) {
			if (currState == PARSCMD) {
				newProcess();
				newWord();
				rflag = 0;
				// get blank spaces
				while (i < length && line[i] == ' ')	i++;
				// expected command but found nothing
				if (i == length) {
					currState = PARSFAIL;
//					printf ("s parscmd\n");
				} else {
					// get command until a blank space is found
					while (i < length && line[i] != ' ')	cmdList[cmdListSize][currChar++] = line[i++];
					currState = PARSNEXT;
				}
				endWord();
			} else if (currState == PARSNEXT) {
				// get blank spaces
				while (i < length && line[i] == ' ')	i++;
				// reached end of line successfully
				if (i == length) {
//					printf ("s parsnext\n");
					currState = PARSSUCCESS;
					endProcess();
				} else {
					// search for token
					switch (line[i]) {
						case '<':
							i++;
							printf ("--ri--\n");
							currState = PARSRIN;
							break;
						case '>':
							i++;
							printf ("--ro--\n");
							currState = PARSROUT;
							break;
						case '2':
							i++;
							printf ("--2--\n");
							currState = PARSTWO;
							break;
						case '|':
							i++;
							printf ("--pp--\n");
							*out_flags |= shooSH_PIPE;
							endProcess();
							currState = PARSCMD;
							break;
						default:
							printf ("--pr--\n");
							currState = PARSPARAM;
					}
				}
			} else if (currState == PARSPARAM) {
				newWord();
				while (i < length && line[i] != ' ')	cmdList[cmdListSize][currChar++] = line[i++];
				currState = PARSNEXT;
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
				// get blankspaces
				rflag = REDIN;
				currState = PARSFILENAME;
			} else if (currState == PARSROUT) {
				if (line[i] == '>') { // append;
					i++;
					rflag = REDOUTA;
				} else {
					rflag = REDOUTT;
				}
				currState = PARSFILENAME;
			} else if (currState == PARSFILENAME) {
				while (i < length && line[i] == ' ')	i++;
				if (i == length)	currState = PARSFAIL; // TODO admitting the filename will be always valid
				else {
					curr = 0;
					if (line[i] == '"') {
						filename[curr++] = line[i++];
						while (i < length && line[i] != '"')	filename[curr++] = line[i++];
						if (i == length)	currState = PARSFAIL;
						else	filename[curr++] = line[i++];
					} else if (line[i] == '\'') {
						filename[curr++] = line[i++];
						while (i < length && line[i] != '\'')	filename[curr++] = line[i++];
						if (i == length)	currState = PARSFAIL;
						else	filename[curr++] = line[i++];
					} else {
						while (i < length && line[i] != ' ')	filename[curr++] = line[i++];
					}
					filename[curr] = '\0';
					job->setProcessFile (filename, rflag);
					currState = PARSNEXT;
				}
			}
		}
		if (currState == PARSFAIL)	*out_flags |= shooSH_FAIL;
		return job;
	}	
};

int main (void) {
	Parser p;
	Job* j;
	int flag;
	while (!(flag & shooSH_EXIT)) {
		j = p.parseLine(&flag);
		j->print();
		if (flag & shooSH_FAIL)	printf ("FAIL\n");
		printf ("flags: %d\n", flag);
		delete j;
		
	}
	return 0;
}
