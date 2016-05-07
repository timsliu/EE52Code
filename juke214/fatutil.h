/****************************************************************************/
/*                                                                          */
/*                                FATUTIL.H                                 */
/*              Utility Functions for Reading a FAT16 Hard Drive            */
/*                              Include File                                */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS 52                                  */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants and function prototypes for the FAT hard
   drive access utility functions defined in fatutil.c.


   Revision History
      6/5/03   Glen George       Initial revision.
      6/19/08  Glen George       Added declaration for the accessor method
                                 get_partition_start() used to get the start
			         of the partition.
*/




#ifndef  I__FATUTIL_H__
    #define  I__FATUTIL_H__


/* library include files */
  /* none */

/* local include files */
#include  "mp3defs.h"




/* constants */
    /* none */




/* structures, unions, and typedefs */
    /* none */




/* function declarations */

/* initialization functions */
long int            init_FAT_system(void);	/* initialize directory system */

/* accessor functions */
unsigned long int   get_partition_start(void);  /* get partition starting sector */
const char         *get_cur_file_name(void);	/* get current file name */
unsigned char       get_cur_file_attr(void);    /* get current file attributes */
unsigned int        get_cur_file_time(void);	/* get current file length in seconds */
long int	    get_cur_file_size(void);	/* get current file length in bytes */
unsigned long int   get_cur_file_sector(void);	/* get current file starting sector */

/* status functions */
char                cur_isDir(void);		/* current file is a directory */
char                cur_isParentDir(void);	/* current file is ".." */

/* directory traversal functions */
char                get_first_dir_entry(unsigned long int);	/* get first directory entry */
char                get_next_dir_entry(void);			/* get next directory entry */
char                get_previous_dir_entry(void);		/* get previous directory entry */


#endif
