#ifndef BUILTIN_H
#define BUILTIN_H

#include <stdio.h>
#include <list>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "Job.hpp"
#include "shooSHlib.hpp"
#include "Initializer.hpp"

/*!
 *
 *	\defgroup Builtin Builtin
 *	@{
 *	Funcoes integradas
 *	}@
 */

/*!
 *	\addtogroup Builtin
 *	@{
 */

std::list<std::string>	history; //!< Historico de comandos

/*!
 *	\brief Mostra todos os jobs da lista.
 */
void jobs (void);

/*!
 *	\brief Mostra comandos digitados ate o momento.
 */
void showHistory (void);

/*!
 *	\brief Envia um sinal SIGTERM para o job dado por id.
 *	\param id ID do job.
 *	\exception -id	Job inexistente.
 */
void sendsigterm (int id);

/*!
 *	\brief Escreve os argumentos com 1 espaco entre eles.
 *	\param argc Numero de palavras a serem escritas + 1.
 *	\param args Palavras a serem escritas, comeca com "echo".
 */
void echo (int argc, char* args[]);

/*!
 *	\brief Muda o diretorio atual.
 *	\param dir Diretorio para o qual ocorrera a mudanca.
 */
void changeDirectory (const char* dir);

/*!
 *	\brief Mostra o diretorio atual.
 *	\exception -1	Diretorio maior que 256 caracteres.
 */
void showDirectoryName (void);

/*!
 *	\brief Coloca o job em execucao em foreground.
 *	\param id ID do job.
 *	\exception -ID	Job nao encontrado.
 */
void foreground (int id);

/*!
 *	\brief Coloca o job em execucao em background.
 *	\param id ID do job.
 *	\exception -ID	Job nao encontrado.
 */
void background (int id);


/*!
 *	\brief Executa comandos builtin.
 *
 *	\param p Processo a ser executado.
 *	\return
 *		0 	Comando nao eh builtin
 *		1	Faltam parametros
 *		2	Erro nos parametros
 *		0	Sucesso
 */
int executeBuiltin (Process *p) ;

/*!
 *	@}
 */

#endif
