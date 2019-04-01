#include <stdio.h>

#include "portable_fputs_unlocked.h"


/** Writes a nul-terminated string to stdout without first acquiring the
 *  lock for the specified stream. The nul terminator is not written.
 *  @param string a pointer to the nul-terminated sequence of chars to be written
 *  @param stream the stream to write to.
 *  @return the number of characters written
 */
int portable_fputs_unlocked(const char * string, FILE *stream){

	int status;
	const char * start = string;
	
	while(*string){
	
		status = putc_unlocked(*string, stream);
		string++;
	}
	
	if (status == EOF)
		return EOF;
	else
		return string - start;
	
}


#ifdef __APPLE__

int fputs_unlocked(const char *s, FILE *stream){

	return portable_fputs_unlocked(s, stream);
}
#endif 
