/* ctype.h - Character type classification routines. 
** Copyright (c) 1982, 1983, 1984, 1985 by Mark Williams Company, Chicago.
** All rights reserved. May not be copied or disclosed without permission.
*/

extern const char _ctype_[];

/* Bits classifications */
#define	_U	01		/* Upper case alphabetic */
#define	_L	02		/* Lower case alphabetic */
#define	_A	(_U|_L)		/* Alphabetic */
#define	_D	010		/* Digit */
#define	_S	020		/* White space character */
#define	_P	040		/* Punctuation character */
#define	_C	0100		/* Control character */
#define	_X	0200		/* Printable but nothing else */

/* Macro definitions */
#define	isalpha(c)	((_ctype_+1)[(c)]&_A)
#define	isupper(c)	((_ctype_+1)[(c)]&_U)
#define	islower(c)	((_ctype_+1)[(c)]&_L)
#define	isdigit(c)	((_ctype_+1)[(c)]&_D)
#define	isalnum(c)	((_ctype_+1)[(c)]&(_A|_D))
#define	isspace(c)	((_ctype_+1)[(c)]&_S)
#define	ispunct(c)	((_ctype_+1)[(c)]&_P)
#define	isprint(c)	((_ctype_+1)[(c)]&(_P|_X|_A|_D))
#define	iscntrl(c)	((_ctype_+1)[(c)]&_C)
#define	isascii(c)	(((c)&~0177)==0)
#define	tolower(c)	((c)|('a'-'A'))
#define	toupper(c)	((c)&~('a'-'A'))
