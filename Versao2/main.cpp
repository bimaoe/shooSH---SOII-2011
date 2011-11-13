#include "shooSH.hpp"
#include <mcheck.h>

int main (void) {
	mtrace();
	shooSH_init();
	shooSH_run();
	muntrace();
	return 0;
}
