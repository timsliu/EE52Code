/* setjmp.h - Declares functions for bypassing normal call and return. */
/* Copyright 1988 Intel Corporation */

#if !defined(_SETJMPH)
#define _SETJMPH

/* iC-86 V4.0 interface macros & #pragma's */
#define setjmp	setjmp_
#define longjmp	longjmp_

#pragma VARPARAMS(setjmp_)
#pragma VARPARAMS(longjmp_)

/*
** Environment for setjmp/longjmp.
** Four words.
** Saves SP, BP and return address.
** Only first 3 words used in SMALL model.
*/
typedef	int	jmp_buf[4];

/* Function definitions */
int	setjmp(jmp_buf);
void	longjmp(jmp_buf, int);
#endif
