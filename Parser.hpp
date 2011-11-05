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
#include "shooSH.hpp"

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
	 *	Description
	 * 		Parse a command line
	 *	Parameters
	 *		Pointer to integer that keeps the generated flags
	 * 	Return
	 *		if blank line or exit
	 *			NULL
	 *		else
	 *			Pointer to job of parsed line (have to be deleted after using)
	 */
	Job* parseLine (void);
};

#endif
