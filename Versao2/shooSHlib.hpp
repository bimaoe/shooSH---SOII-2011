#ifndef SHOOSHLIB_H
#define SHOOSHLIB_H

#include <errno.h>

#define shooSH_EXIT		1
#define shooSH_PIPE		2
#define shooSH_REDIR	4
#define shooSH_NOP		8
#define shooSH_FAIL		16
#define shooSH_BG		32

extern int errno;

#endif
