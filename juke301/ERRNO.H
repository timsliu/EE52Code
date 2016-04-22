/* errno.h - Error number definitions. */
/* Copyright 1988 Intel Corporation */
extern int	errno;

#define	EXDEV		0x0102		/* Cross volume */
#define	EACCESS		0x0026		/* Access error */
#define	EEXIST		0x0020		/* File exists, and shouldn't */
#define	ENOENT		0x0021		/* The file does not exist */
#define	ENOMEM		0x0002		/* Not enough memory */
#define	EINVAL		0x0108		/* Illegal parameter */
#define	EFAULT		0x0109		/* Illegal pointer */
#define	EBUSY		0x0028		/* Illegal operation on shared thing */
#define	EMFILE		0x010A		/* Six files open */
#define	ENOSPC		0x0029		/* The directory is full */
#define	ENAMETOOLONG	0x0081		/* String too long */
#define	ENOTDIR		0x010C		/* Path name syntax */
#define	EDOM		0x0021		/* Domain error */
#define	ERANGE		0x0022		/* Result too large */
