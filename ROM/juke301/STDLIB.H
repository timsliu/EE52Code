/* stdlib.h - General utility declarations and macro definitions. */
/* Copyright 1988 Intel Corporation */

#if !defined(_STDLIB)
#define	_STDLIB

#ifndef _size_t
#define _size_t
typedef unsigned size_t;	/* result of sizeof operator */
#endif

/* iC-86 V4.0 interface macros & #pragma's */
#define abs	abs_
#define atof	atof_
#define atoi	atoi_
#define atol	atol_
#define calloc	calloc_
#define exit	exit_
#define free	free_
#define malloc	malloc_
#define qsort	qsort_
#define rand	rand_
#define realloc	realloc_
#define shellsort	shellsort_
#define srand	srand_

#pragma VARPARAMS(abs_)
#pragma VARPARAMS(atof_)
#pragma VARPARAMS(atoi_)
#pragma VARPARAMS(atol_)
#pragma VARPARAMS(calloc_)
#pragma VARPARAMS(exit_)
#pragma VARPARAMS(free_)
#pragma VARPARAMS(malloc_)
#pragma VARPARAMS(qsort_)
#pragma VARPARAMS(rand_)
#pragma VARPARAMS(realloc_)
#pragma VARPARAMS(shellsort_)
#pragma VARPARAMS(srand_)

/* Function definitions */
int	abs(int);
double	atof(const char *);
int	atoi(const char *);
long	atol(const char *);
void	*calloc(size_t, size_t);
void	exit(int);
void	free(void *);
void	*malloc(size_t);
void	qsort(void *, size_t, size_t, int (*p)(const void *, const void *));
int	rand(void);
void	*realloc(void *, size_t);
void	shellsort(void *, int, int, int (*p)(const void *, const void *));
void	srand(unsigned int);

#define	_exit(s)	dqexit((s))
#endif
