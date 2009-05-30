#ifndef DEBUG_H
#define DEBUG_H


#include <stdio.h>
#include <wchar.h>

#ifdef DEBUG

#define dbgprintf(format, args...) \
do{ \
	if(fwide(stderr, 0) < 0) \
	{ \
		fprintf(stderr, "%s:%d:%s: " format, __FILE__, __LINE__, __func__ , ##args); \
	}\
	else \
	{ \
		fwprintf(stderr, L"%s:%d:%s: " format, __FILE__, __LINE__, __func__ , ##args); \
	} \
	fflush(stderr);\
} while(0);
#else

#define dbgprintf(format, args...)

#endif


#endif

