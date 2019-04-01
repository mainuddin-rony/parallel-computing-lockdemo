#ifndef PORTABLE_FPUTS_UNLOCKED_H

#define PORTABLE_FPUTS_UNLOCKED_H




/** Writes a nul terminated string to stdout without first acquiring the
 *  lock for the specified stream.
 *  @param string a pointer to the nul-terminated sequence of chars to be written
 *  @param stream the stream to write to.
 */
//int portable_fputs_unlocked(const char * string, FILE *stream);

//#ifdef __APPLE__
int fputs_unlocked(const char *s, FILE *stream);
int putc_unlocked(int c, FILE *stream);
//#endif


#endif

