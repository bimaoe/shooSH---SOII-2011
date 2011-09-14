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
	public:
		Redirector (void) {}
		void redirect (char* filename, int flag) {
			fd = open (filename, flags[flag], 0666); //TODO tirar essa coisa (0666)
			dup2 (fd, flag!=0);
			//useful code here
			close (fd);
		}
};
