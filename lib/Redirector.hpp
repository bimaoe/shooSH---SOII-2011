#ifndef REDIRECTOR_H
#define REDIRECTOR_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// types of redirection
#define REDIN		1
#define REDOUTT		2
#define REDOUTA		4
#define REDERRT		8
#define REDERRA		16

class Redirector {
	private:
		int flags[3];
		int fd[3];	// fd[0] = input
					// fd[1] = output
					// fd[2] = error
		
	public:
		Redirector (void);
		void init (char* filename[3], int flag);
		void end();
};
#endif
