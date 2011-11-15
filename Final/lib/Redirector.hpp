#ifndef REDIRECTOR_H
#define REDIRECTOR_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

//!< Definicoes de flags de redirecionamento
#define REDIN		1
#define REDOUTT		2
#define REDOUTA		4
#define REDERRT		8
#define REDERRA		16

/*!
 *	Classe com funcoes de redirecionamento
 */
class Redirector {
	private:
		int flags[3];
		int fd[3];	// fd[0] = input
					// fd[1] = output
					// fd[2] = error
		
	public:
		Redirector (void);
		/*!
		 *	\brief Abre os arquivos necessarios e duplica descritores.
		 *	\param filename Nomes dos arquivos.
		 *	\param flag Flags indicando os tipos de redirecionamento
		 */
		void init (std::string filename[3], int flag[3]);
		/*!
		 *	\brief Fecha os descritores de arquivos
		 */
		void end (void);
};
#endif
