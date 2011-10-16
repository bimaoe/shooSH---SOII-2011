#include <unistd.h>

class builtIn{

	public:
		
		static void changeDirectory(const char* dir){
			chdir(dir);
		}
		
		static char* showDirectoryName(char* buf){
			return getcwd(buf, 256);		
		}

};

