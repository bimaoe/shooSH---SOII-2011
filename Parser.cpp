#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>
#define NUM_WORDS 100
#define MAX_LENGTH 1024

class Parser {
private:
	char** cmdList;
	int cmdListSize;
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
		
		std::cout << "shooSH> ";
		std::cin.getline (line, MAX_LENGTH);
		
		clearCmdList();

		for (i = currChar = 0, length = strlen(line); i < length; i++) {
			if (line[i] == ' ' && currChar > 0) { //se for espaco, termina o comando atual e insere
				cmdList[cmdListSize][currChar] = '\0';
				currChar = 0;
				cmdList[++cmdListSize] = (char*)malloc(sizeof(char)*MAX_LENGTH);
			} else {
				cmdList[cmdListSize][currChar++] = line[i];
			}
		}
		cmdList[cmdListSize++][currChar] = '\0';
		cmdList[cmdListSize] = (char*)NULL;
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
