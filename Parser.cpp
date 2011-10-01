#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>
#include "Executor.hpp"
#define NUM_WORDS 100
#define MAX_LENGTH 1024

class Parser {
private:
	char** cmdList;
	int cmdListSize;
	Executor executor;
	std::map <std::string, int> keywordList;

	void loadKeyword (void) {
		int size;
		std::string keys;
		std::ifstream fin (".keyword.key", std::ifstream::in);
		while (!fin.eof()) {
			fin >> keys;
			size = keywordList.size();
			keywordList[keys] = size;
		}
		if (fin.is_open())	fin.close();
	}
	
	void printKeywordList (void) {
		std::map <std::string, int>::iterator curr, end;
		std::cout << "Printing keyword list" << std::endl;
		for (curr = keywordList.begin(), end = keywordList.end(); curr != end; curr++) {
			std::cout << curr->first << " " << curr->second << std::endl;
		}
	}
	
	void printCmdList (void) {
		int i;
		std::cout << "Printing command list" << std::endl;
		for (i = 0; i < cmdListSize; i++) {
			std::cout << cmdList[i] << std::endl;
		}
	}

	void clearCmdList (void) {
		while (cmdListSize)	free (cmdList[cmdListSize--]);
	}
	
public:
	Parser (void) {
		cmdListSize = 0;
		cmdList = (char**) malloc (sizeof(char*)*NUM_WORDS);
		cmdList[0] = (char*) malloc (sizeof(char)*MAX_LENGTH);
	}
	

	/*
	 * Leitura de cada linha de comando.
	 * Retorna true se nao houve exit. 
	 * Retorna false caso contrario.
	 */
	/* TODO:
		fazer tratamento de keyword
	*/
	bool parseLine (void) {
	
		char line[MAX_LENGTH];
		int i, length, currChar;
		bool bg;

		std::cout << "shooSH> ";
		std::cin.getline (line, MAX_LENGTH);
		
		clearCmdList();

		for (i = currChar = 0, length = strlen(line); i < length; i++) {
			 // insert the character into the word until a space is found
			if (line[i] == ' ') {
				if (currChar > 0) {
					//TODO: check if the current word is a keyword
					//TODO: check if the current command is valid
					cmdList[cmdListSize][currChar] = '\0';
					//if (strcmp (cmdList[cmdListSize], ">")) 
					currChar = 0;
					cmdList[++cmdListSize] = (char*)malloc(sizeof(char)*MAX_LENGTH);
				}
			} else {
				cmdList[cmdListSize][currChar++] = line[i];
			}
		}
		// check if the command must run in background
		cmdList[cmdListSize][currChar] = '\0';
		if (strcmp (cmdList[cmdListSize], "&") == 0) {
			bg = true;
			free (cmdList[cmdListSize]);
		} else {
			cmdListSize++;
			bg = false;
		}
		cmdList[cmdListSize] = (char*)NULL;
		if (!((cmdListSize == 1) && (!strcmp(cmdList[0], "exit"))))	executor.execute (cmdList, bg);
		return !((cmdListSize == 1) && (!strcmp(cmdList[0], "exit")));
	}
	
	char** getCmdList (void) {
		return cmdList;
	}
	void parse (void) {
		while (parseLine()) {
			printCmdList();
		}
	}
};

int main (void) {
	Parser p;
	p.parse();
	
	return 0;
}
