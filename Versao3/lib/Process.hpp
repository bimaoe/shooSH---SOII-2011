#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include "Redirector.hpp"

/*!
 *	Classe de processo
 */
class Process{
//@{
	private:
		char**		command; //matriz de comandos
		int     	cmdSize; //tamanho da matriz de comandos
		pid_t   	pid; //PID do pricesso
		std::string	filename[3]; //nome dos arquivos de redirecionamento
		int     	redirflag[3]; //flag de redirecionamento

	public:

		Process(void);
		/*!
		 *	\brief Desaloca a matriz de comandos
		 */
		void			destroy (void);
		
		bool			hasRedirection(void);
		void			setCommand(char**, int);
		void			setFilename(std::string, int);
		void			setPID (pid_t);
		
		char**			getCommand (void);
		std::string*	getFilenames (void);
		pid_t			getPID (void);
		int*			getRedirFlags(void);
		int				size (void);
		
		void			print (void);
//@}
};

#endif
