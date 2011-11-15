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

//@name Defines
//@{
//! Definicoes de limites
#define NUM_WORDS		100
#define MAX_LENGTH		1024
#define MAX_FILENAME	128

//! Estados da maquina de estados
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
//@}

/*!
 *	\brief Parser de linha
 */
class Parser {
//@{
	private:
		int		currState; //Estado atual
		Job*	job; //Ponteiro para o job que sera devolvido
		char**	cmdList; //Matriz de comandos
		int		cmdListSize; //Tamanho da matriz de comandos
		int		currChar; //Indice do caracter atual
		char	line[MAX_LENGTH]; //Linha de comando
		char	filename[MAX_FILENAME]; //Nome de arquivo de redirecionamento

		void initialize (void); //Funcao que inicializa o parser
		void newWord (void); //Funcao que inicia uma nova palavra na matriz de comandos
		void endWord (void); //Funcao que termina uma palavra da matriz de comandos
		void newProcess (void); //Funcao que cria um novo processo no job
		void endProcess (void); //Funcao que termina a criacao do processo no job

	public:
		Parser (void);
		/*! 
		 * 	\brief Pega uma linha de comando.
		 *		Implementada como uma maquina de estados para reconhecer a linguagem gerada pela gramatica indicada na especificacao.
		 * 	\return	Ponteiro para o job gerado.
		 */
		Job* parseLine (void);
//@}
};

#endif
