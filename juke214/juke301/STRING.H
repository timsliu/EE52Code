/* string.h - String function declarations. */
/* Copyright 1988 Intel Corporation */

#if !defined(_STRINGH)
#define	_STRINGH

/* iC-86 V4.0 interface macros & #pragma's */
#define strcat	strcat_
#define strncat	strncat_
#define strcmp	strcmp_
#define strncmp	strncmp_
#define strlen	strlen_
#define strchr	index_
#define strcpy	strcpy_
#define strncpy	strncpy_
#define strrchr	rindex_

#pragma VARPARAMS(strcat_)
#pragma VARPARAMS(strncat_)
#pragma VARPARAMS(strcmp_)
#pragma VARPARAMS(strncmp_)
#pragma VARPARAMS(strlen_)
#pragma VARPARAMS(strcpy_)
#pragma VARPARAMS(strncpy_)
#pragma VARPARAMS(index_)
#pragma VARPARAMS(rindex_)

#ifndef _size_t
#define _size_t
typedef unsigned size_t;	/* result of sizeof operator */
#endif

/* Function definitions */
char	*strcat(char *, const char *);
char	*strncat(char *, const char *, size_t);
int	    strcmp(const char *, const char *);
int	    strncmp(const char *, const char *, size_t);
int	    strlen(const char *);
char	*strcpy(char *, const char *);
char	*strchr(const char *, int);
char	*strncpy(char *, const char *, size_t);
char	*strrchr(const char *, int);
#endif
