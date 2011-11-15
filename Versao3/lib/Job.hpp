#ifndef JOB_H
#define JOB_H

#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <termios.h>
#include "Redirector.hpp"
#include "Process.hpp"
#include "shooSHlib.hpp"

/*!
 *	\brief Classe de Job
 */
class Job {
//@{
	private:
		std::vector<Process*>	process; //!< Vetor de ponteiros dos processos do job.
		pid_t 					pgid; //!< Group ID do Job.
		std::string 			cmd; //!< Linha de comando.
		int						flag; //!< Flag de bg, pipe, exit, nop.
		int 					id; //!< ID interno.
		struct termios 			modes; //!< Atributos.
		bool					stopped; //!< Flag de parado.

	public:
		Job(void);
		/*!
		 *	\brief Cria um processo e o insere no vetor de processos.
		 */
		void			createProcess (void);
		/*!
		 *	\brief Desaloca todos os recursos.
		 */
		void			destroy (void);
		
		std::string		getCommand (void);
		int				getID (void);
		pid_t			getPGID (void);
		/*!
		 *	\brief Retorna um processo do vetor de processos.
		 *	\param i Indice do processo no vetor.
		 */
		Process*		getProcess (int i);
		struct termios*	getTermios(void);
		bool			hasPipe (void);
		bool			hasExited (void);
		bool			hasFailed (void);
		bool			inBG (void);
		bool			isNop (void);
		bool			isStopped (void);
		int				size(void);

		void			addFlag (int);
		void			cont (void);
		void			setBG (bool);
		void			setCommand (std::string command);
		void			setID (int);
		void			setPGID (pid_t);
		/*!
		 *	\brief Seta o comando do ultimo processo inserido.
		 *	\param command Matriz de caracteres cuja ultima linha deve ser (char*)NULL.
		 *	\param size Numero de linhas da matriz sem contar a nula.
		 */
		void			setProcessCommand (char** command, int size);
		/*!
		 *	\brief Seta o nome do arquivo relacionado a um tipo de redirecionamento.
		 *	\param filename Nome do arquivo.
		 *	\param flag Flag indicando o tipo de redirecionamento
		 */
		
		void			setProcessFile (std::string filename, int flag);
		void			stop (void);

		void			print (void);
//@}
};

#endif
