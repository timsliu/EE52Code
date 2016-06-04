/* stdarg.h - Macros and types for variable argument list to a function */

#ifndef _STDARGH
#define _STDARGH

#ifndef _va_list
#define _va_list
typedef char *va_list;
#endif

#define va_start(ap,paramn) (ap=((char *)&(paramn)+sizeof(paramn)))
#define va_arg(ap,type)     (*((type *)(ap+=sizeof(type))-1)) 
#define va_end(ap)

#endif
