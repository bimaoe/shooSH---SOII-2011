#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <string.h>

class Parser {
private:
	std::vector <std::string> cmdList;
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
		for (i = 0; i < cmdList.size(); i++) {
			std::cout << cmdList[i] << std::endl;
		}
	}
	
public:
	Parser (void) {
		loadKeyword();
		printKeywordList();
	}
	
	/*
	 * Leitura de cada linha de comando.
	 * Retorna true se nao houve exit. 
	 * Retorna false caso contrario.
	 */
	bool parseLine (void) {
	
		std::string cmd, line;
		int i;
		
		std::cout << "shooSH> ";
		getline (std::cin, line);
		
		cmdList.clear();
		
		for (i = 0; i < line.length(); i++) {
			if (line[i] == ' ') { //se for espaco, termina o comando atual e insere
				cmd += '\0';
				it = keywordList.find (cmd); //checa se eh uma keyword
				if (it != keywordList.end())	cmd[0] = '\0';
				cmdList.push_back(cmd);
				cmd.clear();
			} else {
				cmd += line[i];
			}
		}
		cmd += '\0';
		cmdList.push_back(cmd);
		cmdList.push_back(NULL);
		
		printCmdList();
		
		return !((cmdList.size() == 2) && (!strcmp(cmdList[0].c_str(), "exit")));
	}
	
	void parse (void) {
		while (parseLine());
	}
};

int main (void) {
	Parser p;
	p.parse();
	
	return 0;
}
