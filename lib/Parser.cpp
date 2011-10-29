#include "Parser.hpp"
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
	currState = PARSCMD;
}

Job* Parser::parseLine (void) {
	job = new Job();
	currState = PARSCMD;
	int i, length, curr;
	int rflag;

	std::cin.getline (line, MAX_LENGTH);
	std::cout << line << std::endl;
	i = 0;
	while (i < length && line[i] == ' ')	i++;
	length = strlen(line+i);
	// check for blank line
	if (length == 0) {
		job->addFlag (shooSH_NOP);
		return job;
	} else {
		while (line[--length] == ' '); // get trailing spaces
		length++;
		if (strcmp (line+i, "exit") == 0) {
			job->addFlag (shooSH_EXIT);
			return job;
		}
	}
	job->setCommand (std::string (line+i, length));
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
				currState = PARSSUCCESS;
				endProcess();
			} else {
				// search for token
				switch (line[i]) {
					case '<':
						i++;
						currState = PARSRIN;
						break;
					case '>':
						i++;
						currState = PARSROUT;
						break;
					case '2':
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
						currState = PARSSUCCESS; //TODO: ainda nao trata cmd1 & cmd2
						break;
					default:
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
					filename[curr++] = line[i++];
					while (i < length && line[i] != '"')	filename[curr++] = line[i++];
					if (i == length)	currState = PARSFAIL;
					else	filename[curr++] = line[i++];
				} else if (line[i] == '\'') { // get filenames like 'file name'
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
	if (currState == PARSFAIL)	job->addFlag (shooSH_FAIL);
	return job;
}	

