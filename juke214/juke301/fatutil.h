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
      3/14/13  Glen George       Added constants and file_info and cache_entry
                                 structures to support FAT usage.
      3/14/13  Glen George       Added declaration for get_file_blocks() and
                                 get_ID3_tag() functions and updated
                                 declarations for init_FAT_system() and
                                 get_first_dir_entry().
*/




#ifndef  I__FATUTIL_H__
    #define  I__FATUTIL_H__


/* library include files */
  /* none */

/* local include files */
#include  "mp3defs.h"




/* constants */

#define  CHAIN_END      0xFFFFFFFF      /* end of a cluster chain */




/* structures, unions, and typedefs */

/* cache entry structure */
struct  cache_entry  {
                        unsigned long int  cluster; /* starting cluster number of cache entry */
                        unsigned long int  size;    /* number of sectors in cache entry */
                     };

/* block information structure for holding the current cluster state */
struct  block_info  {
                       unsigned long int  sector;   /* starting sector number of cluster */
                       unsigned long int  size;     /* number of contiguous sectors */
                       unsigned long int  next;     /* next cluster number in chain */
                       unsigned long int  offset;   /* sector offset within file */
                       unsigned long int  cluster1; /* first cluster in file */
                       int                cache_idx;/* cache index for this cluster */
                   };




/* function declarations */

/* initialization functions */
char                init_FAT_system(void);      /* initialize directory system */

/* accessor functions */
unsigned long int   get_partition_start(void);  /* get partition starting sector */
const char         *get_cur_file_name(void);    /* get current file name */
unsigned char       get_cur_file_attr(void);    /* get current file attributes */
unsigned int        get_cur_file_time(void);    /* get current file length in seconds */
long int            get_cur_file_size(void);    /* get current file length in bytes */
unsigned long int   get_cur_file_sector(void);  /* get current file starting sector */

/* status functions */
char                cur_isDir(void);            /* current file is a directory */
char                cur_isParentDir(void);      /* current file is ".." */

/* directory traversal functions */
char                get_first_dir_entry();      /* get first directory entry */
char                get_next_dir_entry(void);   /* get next directory entry */
char                get_previous_dir_entry(void);   /* get previous directory entry */

/* file access functions */
int                 get_file_blocks(unsigned long int, int, unsigned short int far *);   /* get data from a file */
void                get_ID3_tag(char *);        /* get ID3 tag data from file */


#endif
