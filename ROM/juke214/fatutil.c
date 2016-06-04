/****************************************************************************/
/*                                                                          */
/*                                 FATUTIL                                  */
/*              Utility Functions for Reading a FAT16 Hard Drive            */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS 52                                  */
/*                                                                          */
/****************************************************************************/

/*
   This file contains utility functions for reading a FAT16 hard drive.  The
   current directory information and the path are kept locally in this file.
   The functions included are:
      cur_isDir              - is the current file a directory
      cur_isParentDir        - is the current file the parent directory (..)
      get_cur_file_attr      - get the attributes of the current file
      get_cur_file_name      - get the name of the current file
      get_cur_file_sector    - get the starting sector of the current file
      get_cur_file_size      - get the size in bytes of the current file
      get_cur_file_time      - get the time of the current file
      get_first_dir_entry    - get the first file in the current directory
      get_next_dir_entry     - get next file in the current directory
      get_partition_start    - get the start of the current partition
      get_previous_dir_entry - get previous file in the current directory
      init_FAT_system        - initialize the FAT file system

   The local functions included are:
      init_dir_stack         - initialize the directory name stack
      new_directory          - entering a new directory, update the stack
      get_dir_tos_name       - get name on the top of the stack
      get_dir_tos_sector     - get starting sector of directory at tos

   The locally global variable definitions included are:
      cur_dir                - current file entry in dir_sector[]
      cur_sector             - starting sector of current directory entry
      dir_offset             - current sector offset in the current directory
      dir_sector             - array of directory entries in a sector
      dirname                - name of current directory
      dirnames               - names of directories on stack as a char []
      dirnamestack           - stack of name positions in dirnames[]
      dirsectorstack         - stack of starting directory sector numbers
      dirstack_ptr           - the stack pointer into directory info stacks
      filename               - filename of current directory entry
      first_file_sector      - sector of the first file on the hard drive
      partition_start        - starting sector of the first partition
      sectors_per_cluster    - number of sectors per cluster
      start_sector           - starting sector of current directory


   Revision History
      6/5/03   Glen George       Initial revision.
      6/10/03  Glen George       Updated get_cur_file_time() to use macros
                                 to extract the hours/minutes/seconds from a
                                 word in the directory entry, rather than bit
                                 fields which aren't convenient in Intel C.
      6/10/03  Glen George       Changed code to not display volume label
                                 entries and instead use them as the name of
                                 the root directory.
      6/10/03  Glen George       Updated comments.
      6/11/03  Glen George       Changed code to use new packed int time and
                                 date structure for directory entries.
      6/11/03  Glen George       Fixed minor problem with unterminated
                                 comment.
      4/29/06  Glen George       Removed inclusion of string.h, that's handled
                                 by mp3defs.h now.
      4/29/06  Glen George       Changed all calls to get_blocks to use words
                                 (short int) instead of bytes (char).
      4/29/06  Glen George       Switched to using unions and macros for
                                 accessing hard drive data for portability.
      5/3/06   Glen George       Updated to use the modified macros in vfat.h
                                 for portability.
      5/3/06   Glen George       Updated to use the modified macros in vfat.h
                                 for portability.
      5/3/06   Glen George       Updated to use the modified macros in vfat.h
                                 for portability.
      5/3/06   Glen George       Updated to use the modified macros in vfat.h
                                 for portability.
      6/5/08   Glen George       Modified to also read the partition table to
                                 get the start of the first partition.  Mainly
                                 adds the shared variable partition_start
				 which is added to all hard drive sector
				 numbers.
      6/12/08  Glen George       Fixed calculation of start of the partition -
                                 there was a parenthesis error and a
                                 portability problem.
      6/19/08  Glen George       Added accessor method get_partition_start()
                                 for getting the start of the partition.
*/




/* library include files */
  /* none */

/* local include files */
#include  "mp3defs.h"
#include  "interfac.h"
#include  "vfat.h"
#include  "fatutil.h"




/* local definitions */
  /* none */




/* local function declarations */
void                init_dir_stack(void);       /* initialize stack of directory names */
void                new_directory(void);        /* entering a new directory, update stack */
const char         *get_dir_tos_name(void);     /* get name of directory at top of stack */
unsigned long int   get_dir_tos_sector(void);   /* get starting sector of directory at top of stack */




/* locally global variables */

/* local variables shared by directory functions */
static  union  VFAT_dir_entry  dir_sector[ENTRIES_PER_SECTOR];  /* directory entries in a sector */
static  int                    cur_dir;             /* current entry in dir_sector[] */

static  unsigned long int      partition_start;     /* starting sector number of the first partition */

static  long int               sectors_per_cluster; /* number of sectors per cluster */
static  unsigned long int      first_file_sector;   /* sector of the first file on the hard drive */

static  unsigned long int      start_sector;        /* starting sector of directory */
static  int                    dir_offset;          /* sector offset in directory */

static  unsigned long int      cur_sector;          /* starting sector of current entry */

static  char  dirname[MAX_LFN_LEN];                 /* name of current directory */
static  char  filename[MAX_LFN_LEN];                /* filename of current entry */




/*
   init_FAT_system()

   Description:      This function initializes FAT file system.
	   
   Operation:        The function reads the partition table and boot record to
	             set up the directory parameters: the starting sector
                     number for files on the drive, and the number of sectors
                     per cluster.  It also initializes the directory stack and
                     the directory name and filename.

   Arguments:        None.
   Return Value:     (long int) - starting sector of the root directory, zero
                     if there is an error.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   If there is an error reading the drive or interpretting
                     the data, zero (0) is returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: first_file_sector   - set to the computed sector number.
	             partition_start     - starting sector number of the
			                   partition.
                     sectors_per_cluster - set to the read sectors per
                                           cluster.

   Author:           Glen George
   Last Modified:    June 12, 2008

*/

long int  init_FAT_system()
{
    /* variables */
    union  first_sector    s;               /* the boot sector */

    long int               root_dir_start;  /* start of the root directory */

    char                   error;           /* drive reading error flag */



    /* read the first sector from the harddrive to get the partition table */
    error = (get_blocks(0, 1, (unsigned short int far *) &s) != 1);

    /* compute and store the starting sector of the partition */
    partition_start = ((unsigned long int) s.words[PARTITION_START_LO] & 0xFFFFUL) +
                      (((unsigned long int) s.words[PARTITION_START_HI] & 0xFFFFUL) << 16);


    /* now read the first sector of the partition from the harddrive */
    /* retrieves the BIOS Parameter Block (assuming no errors) */
    error = error || (get_blocks(partition_start, 1, (unsigned short int far *) &s) != 1);


    /* compute the start of the root directory (in sectors) */
    root_dir_start = RESERVED_SECTORS(s) + (NUMFATS(s) * FAT_SECTORS(s));


    /* set the drive variables */
    if (!error)  {
        /* if no error set the parameters from the boot sector value */
        first_file_sector = root_dir_start + (ROOT_ENTRIES(s) / ENTRIES_PER_SECTOR);
        sectors_per_cluster = ALLOC_SECTORS(s);
    }
    else  {
        /* there was an error - set the parameters to default values */
        first_file_sector = 1;
        sectors_per_cluster = 64;
    }


    /* initialize the directory name stack */
    init_dir_stack();


    /* initialize the directory and file names */
    dirname[0] = '\0';
    filename[0] = '\0';


    /* and return the start of the root directory or an error indicator */
    if (error)
        /* error - return 0 */
        return  0;
    else
        /* no error - return the start of the root directory */
        return  root_dir_start;

}




/*
   get_partition_start

   Description:      This function returns the starting sector number of the
                     current partition.

   Arguments:        None.
   Return Value:     (unsigned long int) - starting sector number of the
                     current partition.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: partition_start - accessed by this function.

   Author:           Glen George
   Last Modified:    June 19, 2008

*/

unsigned long int  get_partition_start()
{
    /* variables */
      /* none */



    /* just return the current start of the parition */
    return  partition_start;

}




/*
   get_cur_file_name

   Description:      This function returns a pointer to the name of the
                     current directory entry (a filename).

   Arguments:        None.
   Return Value:     (const char *) - pointer to the name of the current
                     directory entry.  If the entry has a long filename that
                     filename is returned, otherwise the 8.3 filename is
                     returned.  If there is no current directory, due to an
                     error, and empty string is returned.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: filename - accessed by this function.

   Author:           Glen George
   Last Modified:    June 1, 2003

*/

const char  *get_cur_file_name()
{
    /* variables */
      /* none */



    /* just return a pointer to the current filename */
    return  filename;

}




/*
   get_cur_file_attr

   Description:      This function returns the attribute byte of the current
                     directory entry.

   Arguments:        None.
   Return Value:     (unsigned char) - attribute byte of the current directory
                     entry (a file).  If there is no current directory entry
                     due to an error, zero is returned.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: dir_sector - accessed to determine attribute.
                     cur_dir    - accessed to determine current attribute.

   Author:           Glen George
   Last Modified:    April 29, 2006

*/

unsigned char  get_cur_file_attr()
{
    /* variables */
      /* none */



    /* just return the attribute of the current directory entry */
    return  ATTR(dir_sector[cur_dir]);

}




/*
   cur_isDir

   Description:      This function returns whether or not the current file
                     entry is a subdirectory.

   Arguments:        None.
   Return Value:     (char) - TRUE if the current entry is a subdirectory,
                     FALSE if it is not.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: dir_sector - accessed to determine directory status.
                     cur_dir    - accessed to determine directory status.

   Author:           Glen George
   Last Modified:    June 27, 2002

*/

char  cur_isDir()
{
    /* variables */
      /* none */



    /* just return whether or not current entry is a directory */
    return  ((get_cur_file_attr() & ATTRIB_DIR) != 0);

}




/*
   cur_isParentDir

   Description:      This function returns whether or not the current file
                     entry is the parent directory.

   Arguments:        None.
   Return Value:     (char) - TRUE if the current entry is the parent
                     directory (".."), FALSE if it is not.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: dir_sector - accessed to determine directory status.
                     cur_dir    - accessed to determine directory status.

   Author:           Glen George
   Last Modified:    April 29, 2006

*/

char  cur_isParentDir()
{
    /* variables */
      /* none */



    /* just return whether or not current entry is the parent directory */
    /* it's the parent if the name starts with '.' */
    return  (cur_isDir() && (FILENAME(dir_sector[cur_dir], 0) == '.'));

}




/*
   get_cur_file_time

   Description:      This function returns the time (in seconds) of the
                     current directory entry (a file).

   Arguments:        None.
   Return Value:     (unsigned int) - the time stamp for the current
                     directory entry in seconds.  If there is no current
                     directory entry due to an error, zero is returned.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: cur_sector - accessed by this function.

   Author:           Glen George
   Last Modified:    April 29, 2006

*/

unsigned int  get_cur_file_time()
{
    /* variables */
    unsigned int  t;            /* the file time (in seconds) */



    /* first get the seconds (kept in units of 2 seconds) */
    t = 2 * DIR_SECONDS(FTIME(dir_sector[cur_dir]));
    /* then add in the minutes and hours */
    t += 60 * DIR_MINUTES(FTIME(dir_sector[cur_dir]));
    t += 60 * 60 * DIR_HOURS(FTIME(dir_sector[cur_dir]));


    /* and return the resulting time in seconds */
    return  t;

}




/*
   get_cur_file_size

   Description:      This function returns the size of the current directory
                     entry (a file) in bytes.

   Arguments:        None.
   Return Value:     (unsigned long int) - size (in bytes) of the current
                     directory entry.  If there is no current directory entry
                     due to an error, zero is returned.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: cur_sector - accessed by this function.

   Author:           Glen George
   Last Modified:    April 29, 2006

*/

long int  get_cur_file_size()
{
    /* variables */
      /* none */



    /* return the length in bytes of the current directory entry */
    return  FSIZE(dir_sector[cur_dir]);

}




/*
   get_cur_file_sector

   Description:      This function returns the starting sector of the current
                     directory entry (a file).

   Arguments:        None.
   Return Value:     (unsigned long int) - starting sector of the current
                     directory entry.  If there is no current directory entry
                     due to an error, zero is returned.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: cur_sector - accessed by this function.

   Author:           Glen George
   Last Modified:    June 1, 2003

*/

unsigned long int  get_cur_file_sector()
{
    /* variables */
      /* none */



    /* return the starting sector of the current directory entry */
    return  cur_sector;

}




/*
   get_first_dir_entry

   Description:      This function gets the first valid directory entry in
                     the directory whose starting sector number is passed.
                     The long filename of this directory entry is also read
                     and the filename variable is set to this long filename
                     if it exists or the 8.3 filename if there is no long
                     filename.  Finally the starting sector number of this
                     entry is also saved.  If there is an error reading the
                     directory entry the filename is set to the empty string,
                     the starting sector number is set to 0, the directory
                     information is properly initialized, and TRUE is
                     returned.  The function get_next_dir_entry is used to
                     actually get the first directory entry.

   Arguments:        first_sector (unsigned long int) - starting sector number
                                                        of the directory.
   Return Value:     (char) - TRUE if there is an error reading the directory
                     information, FALSE otherwise.

   Inputs:           Data is read from the disk drive.
   Outputs:          None.

   Error Handling:   If there is an error reading the directory, the saved
                     information is set to reasonable values and TRUE is
                     returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: dir_sector   - filled with a sector of directory entries.
                     dir_offset   - set to zero (0), 1st sector of directory.
                     start_sector - set to the passed sector number.
                     dirname      - set to the old value of filename.
                     filename     - set to the filename of the current entry.
                     cur_dir      - set to the current file entry.
                     cur_sector   - set to the starting sector of the entry.

   Author:           Glen George
   Last Modified:    June 1, 2003

*/

char  get_first_dir_entry(unsigned long int first_sector)
{
    /* variables */
    char  error = FALSE;        /* read error flag */



    /* first save the current directory information */
    new_directory();

    /* now entering a directory, so save it as the directory name */
    strcpy(dirname, filename);

    /* and set the starting sector for the directory */
    start_sector = first_sector;


    /* setup the file variables for the get_next_dir_entry function */
    /* have to point at entry "before" first entry */
    cur_dir = ENTRIES_PER_SECTOR - 1;   /* think at end of previous sector */
    dir_offset = -1;                    /* will be updated to 0 */


    /* now can just use the get_next_dir_entry function to get first file */
    error = get_next_dir_entry();


    /* done, return with the error status */
    return  error;

}




/*
   get_next_dir_entry

   Description:      This function gets the next valid directory entry in
                     the directory.  As necessary it reads sectors of 
                     directory entries from the hard drive.  The long filename
                     of the next directory entry is also read and the filename
                     variable is set to this long filename if it exists or the
                     8.3 filename if it does not exist.  The current sector
                     number and directory entry number are also updated.  If
                     there is an error reading the directory entry the
                     filename is set to the empty string, the starting sector
                     number is set to 0, the directory information is properly
                     initialized, and TRUE is returned.

   Arguments:        None.
   Return Value:     (char) - TRUE if there is an error reading the directory
                     information, FALSE otherwise.

   Inputs:           Data is read from the disk drive.
   Outputs:          None.

   Error Handling:   If there is an error reading the directory, the saved
                     information is set to reasonable values and TRUE is
                     returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: dir_sector          - accessed and possibly filled with a
                                           sector of directory entries.
                     dir_offset          - accessed and possibly updated to
                                           the sector offset of the directory
                                           entries.
                     start_sector        - accessed to get the starting sector
                                           of the current directory.
                     sectors_per_cluster - accessed to compute starting sector
                                           of an entry.
                     first_file_sector   - accessed to compute starting sector
                                           of an entry.
                     filename            - set to the filename of the current
                                           entry.
                     cur_dir             - accessed and updated to the current
                                           entry.
                     cur_sector          - set to the starting sector of the
                                           entry.

   Author:           Glen George
   Last Modified:    April 29, 2006

*/

char  get_next_dir_entry()
{
    /* variables */
    char  longfilename[MAX_LFN_LEN];    /* long filename of current entry */
    int   lfn_seq;                      /* sequence number for LFN */
    int   chksum;                       /* long filename checksum */

    int   old_dir_offset;               /* previous directory offset */
    int   old_cur_dir;                  /* old file entry in directory */

    char  error = FALSE;                /* read error flag */
    char  done = FALSE;                 /* done reading the directory info */

    int   i;                            /* general loop indices */
    int   k;



    /* reset the filename to setup for this file */
    longfilename[0] = '\0';

    /* keep track of the old values (in case can't find a next entry */
    old_dir_offset = dir_offset;
    old_cur_dir = cur_dir;


    /* now find the next entry in this directory */
    while (!error && !done)  {

        /* check if need to read a new sector's worth of entries */
        if (cur_dir >= (ENTRIES_PER_SECTOR - 1))  {

            /* need to read in a new sector of directory entries */
            /* update the directory sector number */
            dir_offset++;
            /* read a sector of directory entries, watching for an error */
            error = (get_blocks((start_sector + dir_offset + partition_start),
                                1, (unsigned short int far *) dir_sector) != 1);
            /* reset the pointer into the sector of entries */
            /* set to -1 so will be properly incremented in a couple lines */
            cur_dir = -1;
        }


        /* is this the first entry or the end of the directory */
        if ((cur_dir == -1) || (FILENAME(dir_sector[cur_dir], 0) != '\0'))
            /* not end of the directory - update the entry number */
            cur_dir++;


        /* try to find the next directory entry */
        while (!error && !done && (cur_dir < ENTRIES_PER_SECTOR))  {

            /* check if this is a long filename or a normal entry */
            if (ATTR(dir_sector[cur_dir]) == ATTRIB_LFN)  {

                /* this is a long filename - collect characters */
                /* assume ASCII instead of Unicode */

                /* get the sequence number for this part of the filename */
                /* make it zero-based */
                lfn_seq = (L_SEQ_NUM(dir_sector[cur_dir]) & LFN_SEQ_MASK) - 1;

                /* collect the pieces of the long filename */
                for (k = 0; k < LFN_CHARS; k++)  {
                    /* figure out where the LFN characters are */
                    if (k < LFN1_CHARS)
                        longfilename[LFN_CHARS * lfn_seq + k] = L_LFN1(dir_sector[cur_dir], 2 * k);
                    else if (k < (LFN1_CHARS + LFN2_CHARS))
                        longfilename[LFN_CHARS * lfn_seq + k] = L_LFN2(dir_sector[cur_dir], 2 * (k - LFN1_CHARS));
                    else
                        longfilename[LFN_CHARS * lfn_seq + k] = L_LFN3(dir_sector[cur_dir], 2 * (k - LFN1_CHARS - LFN2_CHARS));
                }

                /* check if this is the last entry */
                if ((L_SEQ_NUM(dir_sector[cur_dir]) & LAST_LFN_ENTRY) != 0)  {
                    /* last entry so remember the checksum */
                    chksum = CHECKSUM(dir_sector[cur_dir]);
                    /* also terminate the filename */
                    longfilename[LFN_CHARS * (lfn_seq + 1)] = '\0';
                }
                
                /* go to the next directory entry */
                cur_dir++;
            }
            else  {

                /* this is a normal entry */
                /* first check if this entry really exists */
                if (FILENAME(dir_sector[cur_dir], 0) == '\xE5')  {

                    /* deleted entry */
                    /* not a valid file, clear the long filename */
                    longfilename[0] = '\0';
                    /* and move to the next entry */
                    cur_dir++;
                }

                /* is it the end of directory marker */
                else if (FILENAME(dir_sector[cur_dir], 0) == '\0')  {

                    /* end of directory marker */
                    /* not a valid file, clear the filename */
                    longfilename[0] = '\0';
                    /* need to restore the old file state (on last file) */
                    cur_dir = old_cur_dir;
                    /* check if need to restore the directory sector */
                    if (dir_offset != old_dir_offset)  {
                        /* need to restore the old directory sector */
                        error = (get_blocks((start_sector + old_dir_offset + partition_start),
                                            1, (unsigned short int far *) dir_sector) != 1);
                        /* also restore the actual offset */
                        dir_offset = old_dir_offset;
                    }
                    /* restored state, now we're done */
                    done = TRUE;
                }

                /* is it . or .. */
                else if (FILENAME(dir_sector[cur_dir], 0) == '.')  {

                    /* is it pointer to this directory or parent directory */
                    if (FILENAME(dir_sector[cur_dir], 1) == '.')  {

                        /* pointer to parent directory */
                        /* so get sector number and parent name */
                        cur_sector = get_dir_tos_sector();
                        strcpy(filename, get_dir_tos_name());
                        /* and we are done */
                        done = TRUE;
                    }
                    else  {

                        /* pointer to current directory - skip it */
                        /* not a valid file, clear the long filename */
                        longfilename[0] = '\0';
                        /* and move to the next entry */
                        cur_dir++;
                    }
                }

                /* is it a volume label */
                else if ((ATTR(dir_sector[cur_dir]) & ATTRIB_VOLUME) != 0)  {

                    /* it is a volume label, see if already have a directory name */
                    if (dirname[0] == '\0')  {

                        /* no directory name currently, use volume label */
                        /* now check if there is a long filename (shouldn't be) */
                        /* first compute the checksum for this entry */
                        /* if checksum OK and there is a filename - keep it */
                        if (longfilename[0] == '\0')  {

                            /* no long filename, set the directory name from 8.3 name */
                            /* copy the filename and extension (but no .) */
                            /* start at first character of filename */
                            k = 0;
                            /* copy the full filename */
                            for (i = 0; (i < DOS_FILENAME_LEN); i++)
                                dirname[k++] = FILENAME(dir_sector[cur_dir], i);

                            /* now append the extension */
                            for (i = 0; (i < DOS_EXTENSION_LEN); i++)
                                dirname[k++] = EXTENSION(dir_sector[cur_dir], i);

                            /* finally, null terminate the string */
                            dirname[k] = '\0';
                        }
                        else  {

                            /* have a long filename - save it as the  directory name */
                            strcpy(dirname, longfilename);
                        }
                    }
                    else  {

                        /* already have a directory name so ignore volume label */
                        /* means we cursored back up to the directory */
                        ;
                    }

                    /* in any case, erase any long file name */
                    longfilename[0] = '\0';
                    /* and move to the next entry (ignore volume label) */
                    cur_dir++;
                }

                /* none of the above, so must actually be a file */
                else  {

                    /* need to set the starting sector and filename */
                    cur_sector = (START_CLUSTER(dir_sector[cur_dir]) - 2) * sectors_per_cluster + first_file_sector;

                    /* now check if there is a long filename */
                    /* first compute the checksum for this entry */
                    /* if checksum OK and there is a filename - keep it */
                    if (longfilename[0] == '\0')  {

                        /* no long filename, set the filename from 8.3 name */
                        /* copy the filename and extension */
                        /* start at first character of filename */
                        k = 0;
                        /* copy the filename without the trailing spaces */
                        for (i = 0; ((i < DOS_FILENAME_LEN) && (FILENAME(dir_sector[cur_dir], i) != ' ')); i++)
                            filename[k++] = FILENAME(dir_sector[cur_dir], i);

                        /* add the '.' separating name and extension */
                        filename[k++] = '.';

                        /* now add the extension, skipping trailing spaces */
                        for (i = 0; ((i < DOS_EXTENSION_LEN) && (EXTENSION(dir_sector[cur_dir], i) != ' ')); i++)
                            filename[k++] = EXTENSION(dir_sector[cur_dir], i);

                        /* finally, null terminate the string */
                        filename[k] = '\0';
                    }
                    else  {

                        /* have a long filename - save it as the filename */
                        strcpy(filename, longfilename);
                    }

                    /* got to the next entry so set the done flag */
                    done = TRUE;
                }
            }
        }
    }


    /* check if there was an error */
    if (error)  {
        /* had an error - clear out the data */
        /* clear the filename */
        filename[0] = '\0';
        /* set the sector to 0 */
        cur_sector = 0;
    }


    /* finally done, return with the error status */
    return  error;

}




/*
   get_previous_dir_entry

   Description:      This function gets the previous valid directory entry in
                     the directory.  As necessary it reads sectors of 
                     directory entries from the hard drive.  The function
                     backs up through the directory to the first standard
                     directory entry prior the current one and then back
                     before any long filename for that entry.  The long
                     filename of the previous directory entry is also read and
                     the filename variable is set to this long filename if it
                     exists or the 8.3 filename if there is no long filename.
                     The current sector number and directory entry number are
                     also updated.  If there is an error reading the directory
                     entry the filename is set to the empty string, the
                     starting sector number is set to 0, the directory
                     information is properly initialized, and TRUE is
                     returned.

   Arguments:        None.
   Return Value:     (char) - TRUE if there is an error reading the directory
                     information, FALSE otherwise.

   Inputs:           Data is read from the disk drive.
   Outputs:          None.

   Error Handling:   If there is an error reading the directory, the saved
                     information is set to reasonable values and TRUE is
                     returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: dir_sector   - accessed and possibly filled with a sector
                                    of directory entries.
                     dir_offset   - accessed and possibly updated to the
                                    sector offset of the directory entries.
                     start_sector - accessed to get the starting sector of the
                                    current directory.
                     filename     - set to the filename of the current entry.
                     cur_dir      - accessed and updated to the current entry.
                     cur_sector   - set to the starting sector of the entry.

   Author:           Glen George
   Last Modified:    April 29, 2006

*/

char  get_previous_dir_entry()
{
    /* variables */
    char  error = FALSE;        /* read error flag */
    char  done = FALSE;         /* done getting the previous directory info */
    char  have_entry = FALSE;   /* have the entry (but maybe not the filename) */



    /* find the previous entry in this directory */
    /* loop until find the entry or get an error */
    while (!error && !done)  {

        /* check if need to read a new sector's worth of entries */
        if (cur_dir == 0)  {

            /* need to read in the previous sector of directory entries */
            dir_offset--;
            /* check if out of directory entries */
            if (dir_offset < 0)  {
                /* out of directory entries - reset to the start */
                dir_offset = 0;
                /* need to be sure "next" operation finds first entry */
                cur_dir = -1;
                /* and done */
                done = TRUE;
            }
            else  {
                /* have a directory entry to check, read the sector of */
                /*    directory entries, watching for an error */
                error = (get_blocks((start_sector + dir_offset + partition_start),
                                    1, (unsigned short int far *) dir_sector) != 1);
                /* and reset to the last file entry in the directory */
                cur_dir = ENTRIES_PER_SECTOR - 1;
            }
        }
        else  {

            /* still more files in this directory, just check the previous entry */
            cur_dir--;
        }


        /* if not an error or done, keep processing */
        if (!done && !error)  {

            /* if don't have the entry yet, check if this is it */
            if (!have_entry)  {

                /* check if this is a file entry in the directory */
                have_entry = ((FILENAME(dir_sector[cur_dir], 0) != '\0')  &&
                              (FILENAME(dir_sector[cur_dir], 0) != '\xE5')  &&
                              (ATTR(dir_sector[cur_dir]) != ATTRIB_LFN));
            }
            else  {

                /* already have an entry, but need to skip its */
                /* potentional long filename too */
                if (ATTR(dir_sector[cur_dir]) != ATTRIB_LFN)  {
                    /* not a long filename, must be done */
                    done = TRUE;
                }
                else  {
                    /* still part of a long filename for the entry */
                    /* nothing to do, just keep going */
                    ;
                }
            }
        }
    }


    /* if finished without an error then get the actual previous filename */
    if (done & !error)  {

        /* get the entry watching for errors */
        /* since we've backed up past the previous entry, this will now */
        /*    find the previous entry */
        error = get_next_dir_entry();
    }


    /* check if there was an error */
    if (error)  {
        /* had an error - clear out the data */
        /* clear the filename */
        filename[0] = '\0';
        /* set the sector to 0 */
        cur_sector = 0;
    }


    /* finally done, return with the error status */
    return  error;

}




/* locally global variables for the stack routines */

/* stack of directory information */
static  char               dirnames[MAX_PATH_CHARS];            /* names */
static  unsigned long int  dirsectorstack[MAX_NUM_SUBDIRS];     /* starting sectors */
static  int                dirnamestack[MAX_NUM_SUBDIRS];       /* name positions in dirnames */
static  int                dirstack_ptr;                        /* the stack pointer */




/*
   init_dir_stack()

   Description:      This function initializes the directory stack.  It clears
                     the directory names, zeros the first stack elements, and
                     initializes the stack pointer.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: dirnames       - first character is set to '\0'.
                     dirsectorstack - first element is set to 0.
                     dirnamestack   - first element is set to 0.
                     dirstack_ptr   - initialized to -1.

   Author:           Glen George
   Last Modified:    June 27, 2002

*/

static  void  init_dir_stack()
{
    /* variables */
      /* none */



    /* set the string of names to the empty string */
    dirnames[0] = '\0';

    /* initialize the first directory entries to 0 */
    dirsectorstack[0] = 0;
    dirnamestack[0] = 0;

    /* finally, set the stack pointer to empty stack */
    dirstack_ptr = -1;


    /* all done with the initialization - return */
    return;

}




/*
   new_directory()

   Description:      This function handles a new directory.  It may be either
                     a subdirectory or a parent directory.  If a subdirectory
                     the directory information (name and starting sector) is
                     added to the directory stack.  If a parent directory, it
                     is removed from the directory stack.  It is assumed that
                     the directory in question is the current entry.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: dirnames       - update to add or remove directory names.
                     dirsectorstack - may be updated to add a directory
                                      starting sector location.
                     dirnamestack   - may be updated to add the starting
                                      character number in dirnames[] for this
                                      directory name.
                     dirstack_ptr   - updated to adjust the stack.
                     dirname        - accessed for the current directory name.
                     start_sector   - accessed for the directory starting
                                      sector number.

   Author:           Glen George
   Last Modified:    June 1, 2003

*/

static  void  new_directory()
{
    /* variables */
      /* none */



    /* check if the current entry matches the top of the stack */
    if ((dirstack_ptr >= 0) && (dirsectorstack[dirstack_ptr] == cur_sector))  {

        /* new directory is on stack - need to pop it off of the stack */
        /* first get rid of the name */
        dirnames[dirnamestack[dirstack_ptr]] = '\0';
        /* now just decrement the stack pointer */
        dirstack_ptr--;
    }
    else  {

        /* does not match top of the stack, need to push new value */
        /* note - push the info for the current directory, not entry */
        /* make sure not out of space */
        if ((dirstack_ptr < (MAX_NUM_SUBDIRS - 1)) &&
            ((strlen(dirnames) + strlen(dirname)) < MAX_PATH_CHARS))  {

            /* there is room - update the stack pointer */
            dirstack_ptr++;
            /* save the sector */
            dirsectorstack[dirstack_ptr] = start_sector;
            /* save the name pointer and the name */
            dirnamestack[dirstack_ptr] = strlen(dirnames);
            strcat(dirnames, dirname);
        }
        else  {

            /* out of room in the stack - this shouldn't happen */
            ;
        }
    }


    /* all done - return */
    return;

}




/*
   get_dir_tos_name()

   Description:      This function returns the name of the directory on the
                     top of the directory stack.  If the stack is empty, the
                     empty string is returned.

   Arguments:        None.
   Return Value:     (const char *) - pointer to the name of the directory on
                     the top of the directory stack or the pointer to an empty
                     string if there is nothing on the stack.

   Input:            None.
   Output:           None.

   Error Handling:   If nothing is on the stack an empty string is returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: dirnames     - accessed to get the directory name.
                     dirnamestack - accessed to find position of name.
                     dirstack_ptr - accessed to find stack entry.

   Author:           Glen George
   Last Modified:    June 27, 2002

*/

static  const char  *get_dir_tos_name()
{
    /* variables */
    const char  *name;



    /* check if there is something in the directory stack */
    if (dirstack_ptr >= 0)  {

        /* there is something on the stack, get the pointer to the name */
        name = &(dirnames[dirnamestack[dirstack_ptr]]);
    }
    else  {

        /* nothing on the stack */
        /* make sure the directory names list is empty */
        dirnames[0] = '\0';
        /* and return a pointer to that empty string */
        name = dirnames;
    }


    /* all done - return with the name pointer */
    return  name;

}




/*
   get_dir_tos_sector()

   Description:      This function returns the starting sector of the
                     directory on the top of the directory stack.  If the
                     stack is empty, zero (0) is returned.

   Arguments:        None.
   Return Value:     (unsigned long int) - the starting sector of the
                     directory on the top of the directory stack or zero (0)
                     if there is nothing on the stack.

   Input:            None.
   Output:           None.

   Error Handling:   If nothing is on the stack, zero (0) is returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: dirsectorstack - accessed to get sector number.
                     dirstack_ptr   - accessed to get sector number.

   Author:           Glen George
   Last Modified:    June 27, 2002

*/

static  unsigned long int  get_dir_tos_sector()
{
    /* variables */
    unsigned long int  sect;



    /* check if there is something in the directory stack */
    if (dirstack_ptr >= 0)  {

        /* there is something on the stack, return the sector */
        sect = dirsectorstack[dirstack_ptr];
    }
    else  {

        /* nothing on the stack, return 0 */
        sect = 0;
    }


    /* all done - return with the starting sector */
    return  sect;

}
