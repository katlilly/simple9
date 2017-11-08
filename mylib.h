#ifndef MYLIB_H_
#define MYLIB_H_

/* this .h file is a specification file, the .c file is called the
   implementation file */


#include <stddef.h>

/* extern means that these function definitions apply beyond this file
   (as opposed to static, which restricts something to only being used
   in the file in which it is written*/
extern void *emalloc(size_t);
extern void *erealloc(void *, size_t);

#endif
