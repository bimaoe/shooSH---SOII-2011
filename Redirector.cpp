#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define REDINPUT 0
#define REDTRUNC 1
#define REDAPPEND 2

class Redirector {
	private:
		const int flags[] = {	O_RDONLY, 
								O_RDWR|O_CREAT|O_TRUNC,
								O_RDWR|O_CREAT|O_APPEND }
		std::pair <int, bool> fd[2]; //Pode ter mais de um arquivo aberto ao mesmo tempo
		//TODO verificar multiplos redirecionamentos
		
	public:
		Redirector (void) {}
		
		void init(char* name, int flag){
			
			if(!fd[0].second){
				fd[0].first = open (filename, flags[flag], 0666); //TODO tirar essa coisa (0666)
				fd[0].second = true;
				dup2 (fd[0].first, flag!=0);
			} else {
				fd[1].first = open (filename, flags[flag], 0666); //TODO tirar essa coisa (0666)
				fd[1].second = true;
				dup2 (fd[1].first, flag!=0);
			}
		}
		
		void close(){
		
			if(fd[0].second) close(fd[0].first);
			if(fd[1].second) close(fd[1].first);
			
			fd[0].second = false;
			fd[1].second = false;
		}
};
