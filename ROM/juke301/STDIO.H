/* stdio.h - Typdefs, declarations & macro definitions for input & output. */
/* Copyright 1988 Intel Corporation */

#if !defined(_STDIOH)
#define _STDIOH

#include "stddef.h"

/* iC-86 V4.0 interface macros & #pragma's */
#define fclose	fclose_
#define fflush	fflush_
#define fgetc	fgetc_
#define fgets	fgets_
#define fopen	fopen_
#define fprintf	fprintf_
#define fread	fread_
#define fputc	fputc_
#define fputs	fputs_
#define freopen	freopen_
#define fscanf	fscanf_
#define fseek	fseek_
#define ftell	ftell_
#define fwrite	fwrite_
#define gets	gets_
#define getw	getw_
#define printf	printf_
#define puts	puts_
#define putw	putw_
#define rewind	rewind_
#define scanf	scanf_
#define setbuf	setbuf_
#define sprintf	sprintf_
#define sscanf	sscanf_
#define ungetc	ungetc_

#pragma VARPARAMS(fclose_)
#pragma VARPARAMS(fflush_)
#pragma VARPARAMS(fgetc_)
#pragma VARPARAMS(fgets_)
#pragma VARPARAMS(fopen_)
#pragma VARPARAMS(fprintf_)
#pragma VARPARAMS(fread_)
#pragma VARPARAMS(fputc_)
#pragma VARPARAMS(fputs_)
#pragma VARPARAMS(freopen_)
#pragma VARPARAMS(fscanf_)
#pragma VARPARAMS(fseek_)
#pragma VARPARAMS(ftell_)
#pragma VARPARAMS(fwrite_)
#pragma VARPARAMS(gets_)
#pragma VARPARAMS(getw_)
#pragma VARPARAMS(printf_)
#pragma VARPARAMS(puts_)
#pragma VARPARAMS(putw_)
#pragma VARPARAMS(rewind_)
#pragma VARPARAMS(scanf_)
#pragma VARPARAMS(setbuf_)
#pragma VARPARAMS(sprintf_)
#pragma VARPARAMS(sscanf_)
#pragma VARPARAMS(ungetc_)

#define	EOF	(-1)			/* End of file */
#define	BUFSIZ	128			/* Size of a buffer */
#define	_NFILE	10			/* # of FILE structures */

/*
** This is a FILE; there is a FILE for every open stream. An external array 
** of these files is used by the I/O package. If the flags are 0 the FILE is 
** not in use.
*/
typedef struct	 FILE {
	unsigned int _flag;		/* Flags */
	unsigned int _count;		/* Count remaining */
	int	     _ugetc;		/* Ungot character */
	unsigned char *_ptr;		/* Pointer */
	unsigned char *_buf;		/* Buffer */
	unsigned int _conn;		/* Connection */
	unsigned int _resp;		/* Response mailbox token for RMX */
}	FILE;

extern	FILE	_file_[_NFILE];		/* The file structures */
extern	int	_fcol;			/* Console column */

/* Flags */
#define	_FWR	0x01			/* Writing */
#define	_FWROK	0x02			/* Writing is ok */
#define	_FRDOK	0x04			/* Reading is ok */
#define	_FBIN	0x08			/* 8 bits */
#define	_FUBUF	0x10			/* Buffer from setbuf */
#define	_FERR	0x20			/* Error */
#define	_FEOF	0x40			/* End of file */
#define	_FRMXTH	0x80			/* File is RMX terminal handler */
#define	_FCI	0x200			/* File is :ci: */
#define	_FCO	0x400			/* File is :co: */
#define	_FSTR	0x800			/* String stream */

/* Function definitions */
int	fclose(FILE *);
int	fflush(FILE *);
int	fgetc(FILE *);
char	*fgets(char *, int, FILE *);
FILE	*fopen(const char *, const char *);
int	fprintf(FILE *, const char *, ...);
size_t	fread(void *, size_t, size_t, FILE *);
int	fputc(int, FILE *);
int	fputs(const char *, FILE *);
FILE	*freopen(const char *, const char *, FILE *);
int	fscanf(FILE *, const char *, ...);
int	fseek(FILE *, long int, int);
long	ftell(FILE *);
size_t	fwrite(const void *, size_t, size_t, FILE *);
char	*gets(char *);
int	getw(FILE *);		/* non-ANSI function */
int	printf(const char *, ...);
int	puts(const char *);
int	putw(int, FILE *);	/* non-ANSI function */		
void rewind(FILE *);
int	scanf(const char *, ...);
void	setbuf(FILE *, char *);
int	sprintf(char *, const char *, ...);
int	sscanf(const char *, const char *, ...);
int	ungetc(int, FILE *);

/* Macro definitions */
#define	clearerr(fp)	((fp)->_flag &= ~(_FERR|_FEOF))
#define	feof(fp)	((fp)->_flag&(_FEOF|_FERR))
#define	ferror(fp)	((fp)->_flag&_FERR)
#define	fileno(fp)	((fp)->_conn)
#define	getc(fp)	fgetc((fp))
#define	getchar()	fgetc(stdin)
#define	putc(c, fp)	fputc((c), (fp))
#define	putchar(c)	fputc((c), stdout)
#define	stderr		(&_file_[2])
#define	stdin		(&_file_[0])
#define	stdout		(&_file_[1])
#endif
