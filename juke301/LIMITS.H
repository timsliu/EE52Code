/* limits.h - Compiler limits */

#define CHAR_BIT	8		/* number of bits in a byte */
#define SCHAR_MIN	-128		/* minimum value for a signed char */
#define SCHAR_MAX	127		/* maximum value for a signed char */
#define UCHAR_MAX	255u		/* maximum value of a unsigned char */
#define CHAR_MIN	SCHAR_MIN	/* minimum value of a 'plain' char */
#define CHAR_MAX	SCHAR_MAX	/* maximum value of a 'plain' char */

#define SHRT_MAX	32767		/* maximum value of a short int */
#define SHRT_MIN	-32768		/* minimum value for a short int */
#define USHRT_MAX	65535u		/* maximum value of an unsigned short */

#define _INT_BIT	(2 * CHAR_BIT)	/* number of bits in an int */

#if _INT_BIT == 16	/* for 86 or 286 architectures */
#define i86OR286
#define INT_MAX		32767		/* maximum value of a int */
#define INT_MIN		-32768		/* minimum value of a int */
#define UINT_MAX	65535u		/* maximum value of an unsigned int */
#else			/* for 386 architectures */
#define i386
#define INT_MAX		2147483647	/* maximum value of a int */
#define INT_MIN		-2147483648	/* minimum value of a int */
#define UINT_MAX	4294967295u	/* maximum value of an unsigned int */
#endif

#define LONG_MAX	2147483647	/* maximum value of a long int */
#define LONG_MIN	-2147483648	/* minimum value of a long int */
#define ULONG_MAX	4294967295u	/* maximum value of a unsigned long */
