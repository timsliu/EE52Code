/* stddef.h - This file contains "standard definitions". */

#ifndef _STDDEFH
#define _STDDEFH

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef int ptrdiff_t;		/* result of subtracting two pointers */

#ifndef _size_t
#define _size_t
typedef unsigned size_t;	/* result of sizeof operator */
#endif

#define offsetof(s_type, memb)	((size_t)(char *)&((s_type *)0)->memb)
#endif
