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
      get_file_blocks        - get data blocks from the current file
      get_first_dir_entry    - get the first file in the current directory
      get_ID3_tag            - get the possible ID3 tag for the current file
      get_next_dir_entry     - get next file in the current directory
      get_partition_start    - get the start of the current partition
      get_previous_dir_entry - get previous file in the current directory
      init_FAT_system        - initialize the FAT file system

   The local functions included are:
      get_block_info         - get file FAT information for a block
      get_contig_sectors     - get contiguous sectors of a file
      get_dir_tos_name       - get name on the top of the stack
      get_dir_tos_sector     - get starting sector of directory at tos
      get_disk_blocks        - get sectors of a file from the disk
      get_file_info          - fill in passed structure with file information
      init_dir_stack         - initialize the directory name stack
      new_directory          - entering a new directory, update the stack

   The locally global variable definitions included are:
      cur_dir                - current file entry in dir_sector[]
      cur_info               - file information of current directory entry
      dir_info               - file information of current directory
      dir_offset             - current sector offset in the current directory
      dir_sector             - array of directory entries in a sector
      dirclusterstack        - stack of starting directory cluster numbers
      dirname                - name of current directory
      dirnames               - names of directories on stack as a char []
      dirnamestack           - stack of name positions in dirnames[]
      dirstack_ptr           - the stack pointer into directory info stacks
      fat16                  - flag indicating FAT16 or FAT32 disk
      filename               - filename of current directory entry
      first_FAT_sector       - sector number of the start of the first FAT
      first_file_sector      - sector of the first file on the hard drive
      partition_start        - starting sector of the first partition
      root_dir_size          - size of the root directory in sectors (FAT16)
      root_start_sector      - starting sector of root directory (FAT16)
      sectors_per_cluster    - number of sectors per cluster


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
      3/16/13  Glen George       Fixed bug in get_previous_dir_entry()
                                 function where it would backup by two entries
                                 if files didn't have long filenames.
      3/17/13  Glen George       Changed interfaces for init_FAT_system() and
                                 get_first_dir_entry() functions.
      3/17/13  Glen George       Added get_ID3_tag() and get_file_blocks()
                                 functions to support reading ID3 tags and
                                 fragmented files.
      3/17/13  Glen George       Added numerous variables and local functions
                                 to support fragmented files and FAT32.
      3/21/13  Glen George       Fixed problem with 0 length FAT caches.
      3/24/13  Glen George       Fixed problem with not setting up the FAT16
                                 root directory information correctly when
                                 move back into the directory.
      4/3/13   Glen George       Fixed bug in get_previous_dir_entry()
                                 function.
      4/5/13   Glen George       Fixed bug in how the far pointer is formed,
                                 it was crossing a segment boundary.
*/




/* library include files */
#ifdef  PCVERSION
    #include  <alloc.h>
#endif

/* local include files */
#include  "mp3defs.h"
#include  "interfac.h"
#include  "vfat.h"
#include  "fatutil.h"




/* local definitions */
  /* none */




/* local function declarations */
void                get_block_info(struct block_info *, unsigned long int);     /* get file FAT information */
unsigned long int   get_contig_sectors(unsigned long int, struct cache_entry *);   /* get contiguous sectors of file */
int                 get_disk_blocks(struct block_info *, unsigned long int,
                                    int, unsigned short int far *);     /* get blocks from disk */
void                init_dir_stack(void);       /* initialize stack of directory names */
void                new_directory(void);        /* entering a new directory, update stack */
const char         *get_dir_tos_name(void);     /* get name of directory at top of stack */
unsigned long int   get_dir_tos_sector(void);   /* get starting sector of directory at top of stack */




/* locally global variables */

/* local variables shared by directory functions */

static  union  VFAT_dir_entry  dir_sector[ENTRIES_PER_SECTOR];  /* directory entries in a sector */
static  int                    cur_dir;             /* current entry in dir_sector[] */

static  struct  block_info     dir_info;            /* current directory information */
static  unsigned long int      dir_offset;          /* sector offset in directory */

static  char  dirname[MAX_LFN_LEN];                 /* name of current directory */


/* information about current file */

static  struct  block_info     cur_info;            /* current file information */

static  char  filename[MAX_LFN_LEN];                /* filename of current entry */


/* general drive variables */

static  char                   fat16;               /* whether FAT16 or FAT32 disk */

static  long int               sectors_per_cluster; /* number of sectors per cluster */
static  int                    clusters_per_sector; /* number of clusters per sector in FAT */

static  unsigned long int      partition_start;     /* starting sector number of the first partition */
static  unsigned long int      first_FAT_sector;    /* sector number of the start of the first FAT */
static  unsigned long int      first_file_sector;   /* sector of the first file on the hard drive */
static  unsigned long int      root_start_sector;   /* starting sector of root directory (FAT 16) */
static  long int               root_dir_size;       /* size of root directory (FAT16) */

static  struct cache_entry  far  *FAT_cache;        /* cache of FAT entries */




/*
   init_FAT_system()

   Description:      This function initializes FAT file system.
           
   Operation:        The function reads the partition table and boot record to
                     set up the directory parameters: the starting sector
                     number for files on the drive, and the number of sectors
                     per cluster.  It also initializes the directory stack and
                     the directory name and filename.

   Arguments:        None.
   Return Value:     (char) - TRUE (non-zero) if there is an error and FALSE
                     (zero) if not.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   If there is an error reading the drive or interpretting
                     the data, a non-zero value is returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: clusters_per_sector - set based on the FAT type.
                     cur_info            - set to the information for the
                                           root directory.
                     dirname             - set to the read volume label.
                     FAT_cache           - set to point at the cache.
                     fat16               - set to the read filesystem type.
                     filename            - set to the empty string.
                     first_FAT_sector    - set to computed sector number.
                     first_file_sector   - set to the computed sector number.
                     partition_start     - starting sector number of the
                                           partition.
                     root_dir_size       - set to the read size of the root
                                           directory (FAT16 only).
                     root_start_sector   - set to the starting sector of the
                                           root directory (FAT16 only).
                     sectors_per_cluster - set to the read sectors per
                                           cluster.

   Author:           Glen George
   Last Modified:    April 5, 2013

*/

char  init_FAT_system()
{
    /* variables */
    union  first_sector   s;            /* the boot sector */

    char                 *vid;          /* volume ID string */

    char                  error;        /* drive reading error flag */

    int                   i;            /* general loop index */



    /* setup the FAT cache - just make it point at memory for it */
    /* allocate the buffer (different on PC vs. embedded system) */
#ifdef  PCVERSION
    /* in PC version, allocate the cache */
    FAT_cache = (struct cache_entry far *) farmalloc(FAT_CACHE_SIZE * sizeof(short int));
#else
    /* embedded version, FAT cache comes after the audio buffers */
    /*    note that this can cause a segment boundary to be crossed so need */
    /*    to do the calculation in the segment number just in case */
    FAT_cache = (struct cache_entry far *) MAKE_FARPTR(DRAM_STARTSEG +
                   ((NO_BUFFERS + 1L) * BUFFER_SIZE * sizeof(short int)) / 16L, 0);
#endif


    /* read the first sector from the harddrive to get the partition table */
    error = (get_blocks(0, 1, (unsigned short int far *) &s) != 1);

    /* compute and store the starting sector of the first partition */
    partition_start = ((unsigned long int) s.words[PARTITION_START_LO] & 0xFFFFUL) +
                      (((unsigned long int) s.words[PARTITION_START_HI] & 0xFFFFUL) << 16);

    /* determine partition type (in low byte) - just check if FAT16 */
    fat16 = ((s.words[PARTITION_TYPE] & 0xFF) == PARTITION_FAT16);


    /* now read the first sector of the partition from the harddrive */
    /* retrieves the BIOS Parameter Block (assuming no errors) */
    error = error || (get_blocks(partition_start, 1, (unsigned short int far *) &s) != 1);


    /* set parameters that are independent of FAT type */

    /* get the starting sector for the first FAT */
    first_FAT_sector = partition_start + RESERVED_SECTORS(s);
    /* get the sectors per cluster */
    sectors_per_cluster = ALLOC_SECTORS(s);


    /* compute the root directory information (depends on FAT type) */
    if (fat16)  {

        /* get the start of the root directory (sector number) for FAT16 */
        root_start_sector = partition_start + RESERVED_SECTORS(s) +
                            (NUMFATS(s) * FAT_SECTORS_16(s));
        /* get the root directory size as well */
        root_dir_size = (ROOT_ENTRIES(s) / ENTRIES_PER_SECTOR);

        /* get the starting sector for files */
        first_file_sector = root_start_sector + root_dir_size;

        /* get the start of the volume label */
        vid = VOLUME_ID_16(s);

        /* set clusters per sector in the FAT (1 word per entry) */
        clusters_per_sector = IDE_BLOCK_SIZE;

        /* set first cluster number to 0 to indicate fixed root directory */
        cur_info.cluster1 = 0;
    }
    else  {

        /* for FAT32 there is no fixed root directory */
        root_start_sector = 0;
        root_dir_size = 0;

        /* get the starting sector for files */
        first_file_sector = partition_start + RESERVED_SECTORS(s) +
                            (NUMFATS(s) * FAT_SECTORS_32(s));

        /* get the start of the volume label */
        vid = VOLUME_ID_32(s);

        /* set clusters per sector in the FAT (2 words per entry) */
        clusters_per_sector = IDE_BLOCK_SIZE / 2;

        /* for FAT32 have to get the first root cluster from boot record */
        cur_info.cluster1 = ROOT_CLUSTER(s);
    }


    /* not using FAT cache yet */
    cur_info.cache_idx = -1;
    /* and point to end of file so will reset to beginning */
    cur_info.offset = 0xFFFFFFFF;
    /* get the information for the root directory */
    get_block_info(&cur_info, 0);


    /* set the directory name to the volume label and the filename to blank */
    for (i = 0; i < VOL_LABEL_LEN; i++)
        dirname[i] = vid[i];
    /* make sure directory name is <null> terminated */
    dirname[VOL_LABEL_LEN] = '\0';
    /* and blank the filename */
    filename[0] = '\0';


    /* verify there are 512 bytes per sector (all code assumes this) */
    error = error || (SECTOR_SIZE(s) != 512);


    /* if there was an error set everything to default values */
    if (error)  {

        /* there was an error - set some parameters to default values */
        first_file_sector = 1;
        first_FAT_sector = 1;
        sectors_per_cluster = 64;
    }


    /* initialize the directory name stack */
    init_dir_stack();


    /* and return the error status */
    return  error;

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

   Shared Variables: dir_sector - accessed by this function.
                     cur_dir    - accessed by this function.

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

   Shared Variables: dir_sector - accessed by this function.
                     cur_dir    - accessed by this function.

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

   Shared Variables: cur_info            - accessed by this function.
                     first_file_sector   - accessed by this function.
                     sectors_per_cluster - accessed by this function.

   Author:           Glen George
   Last Modified:    March 17, 2013

*/

unsigned long int  get_cur_file_sector()
{
    /* variables */
      /* none */



    /* return the starting sector of the current directory entry */
    /* watch for FAT16 root directory */
    if (cur_info.cluster1 == 0)
        /* FAT16 root directory, return its starting sector */
        return  root_start_sector;
    else
        /* not FAT16 root directory, compute and return starting sector */
        return  first_file_sector + (cur_info.cluster1 - 2) * sectors_per_cluster;

}




/*
   get_ID3_tag

   Description:      This function reads the ID3 tag into the passed buffer.
                     It just reads the last ID3_TAG_SIZE bytes of the current
                     file into the passed buffer.

   Arguments:        buffer (char *) - buffer into which the the ID3 tag is to
                                       be read.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 17, 2013

*/

void  get_ID3_tag(char *buffer)
{
    /* variables */
    char  s[IDE_BLOCK_SIZE * 2];        /* sector from the hard drive */

    int   sector;                       /* sector where ID3 tag starts */
    int   offset;                       /* offset within sector for ID3 tag */

    char  error;                        /* error reading the file */

    int   i;                            /* general loop index */



    /* get the sector number of the start of the ID3 tag and its offset */
    /* the ID3 tag is at the end of the file (doing byte calculations) */
    sector = (get_cur_file_size() - ID3_TAG_SIZE) / (2 * IDE_BLOCK_SIZE);
    offset = (get_cur_file_size() - ID3_TAG_SIZE) % (2 * IDE_BLOCK_SIZE);


    /* try to read a sector from the harddrive to get the ID3 tag */
    error = (get_file_blocks(sector, 1, (unsigned short int far *) s) != 1);

    /* now fill the tag with the data read watching for errors */
    for (i = 0; (!error && (i < ID3_TAG_SIZE)); i++, offset++)  {

        /* check if past the end of the sector (working with bytes, not words) */
        if (offset >= (2 * IDE_BLOCK_SIZE))  {
            /* past the end of this sector, need to read next sector */
            error = (get_file_blocks(++sector, 1, (unsigned short int far *) s) != 1);
            /* and at the start of this new sector */
            offset = 0;
        }

        /* can always copy a byte, even if there was an error */
        buffer[i] = s[offset];
    }


    /* if there was an error reading the tag, clear out the tag */
    if (error)  {
        /* there was an error, fill tag with <null> */
        for (i = 0; i < ID3_TAG_SIZE; i++)
            buffer[i] = '\0';
    }


    /* all done reading the ID3 tag, return */
    return;

}




/*
   get_file_blocks

   Description:      This function reads blocks from the current file.  The
                     data "read" is written to the memory pointed to by the
                     third argument.  The number of blocks requested is given
                     as the second argument.  The starting block number in the
                     file to read is passed as the first argument.  The
                     function just calls get_disk_blocks() with the current
                     file block information to read the actual hard drive.

   Arguments:        block (unsigned long int)       - sector number (relative
                                                       to the start of the
                                                       file) at which to start
                                                       reading.
                     length (int)                    - number of blocks to be
                                                       read.
                     dest (unsigned short int far *) - pointer to the memory
                                                       where the read data is
                                                       to be written.
   Return Value:     The number of blocks actually read.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 17, 2013

*/

int  get_file_blocks(unsigned long int block, int length, unsigned short int far *dest)
{
    /* variables */
      /* none */



    /* just call the get_disk_blocks function and return its result */
    return  get_disk_blocks(&cur_info, block, length, dest);

}




/*
   get_first_dir_entry

   Description:      This function gets the first valid directory entry in
                     the directory whose starting sector number is that of the
                     current file.  The long filename of this directory entry
                     is also read and the filename variable is set to this
                     long filename if it exists or the 8.3 filename if there
                     is no long filename.  Finally the starting sector number
                     of this entry is also saved and that FAT cache is filled
                     with information for this file.  If there is an error
                     reading the directory entry the filename is set to the
                     empty string, the starting sector number is set to 0, the
                     directory information is properly initialized, and TRUE
                     is returned.  The function get_next_dir_entry is used to
                     actually get the first directory entry.

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

   Shared Variables: cur_dir    - set to the current file entry.
                     cur_info   - set to the info for current file entry.
                     dir_info   - set to the current value of cur_info.
                     dir_offset - set to zero (0), 1st sector of directory.
                     dir_sector - filled with a sector of directory entries.
                     dirname    - set to the old value of filename.
                     filename   - set to the filename of the current entry.

   Author:           Glen George
   Last Modified:    March 17, 2013

*/

char  get_first_dir_entry()
{
    /* variables */
    char  error = FALSE;        /* read error flag */



    /* first save the current directory information */
    new_directory();

    /* now entering a directory, so save it as the directory name */
    strcpy(dirname, filename);

    /* and set the block information for the directory */
    dir_info.sector   = cur_info.sector;
    dir_info.size     = cur_info.size;
    dir_info.next     = cur_info.next;
    dir_info.offset   = cur_info.offset;
    dir_info.cluster1 = cur_info.cluster1;
    /* directories never cache their FAT information */
    dir_info.cache_idx = -1;


    /* setup the directory variables for the get_next_dir_entry function */
    /* have to point at entry "before" first entry */
    cur_dir = ENTRIES_PER_SECTOR - 1;   /* point at end of previous sector */
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
                     number and directory entry number are also updated.  The
                     FAT cache is also filled with the information for this
                     file.  If there is an error reading the directory entry
                     the filename is set to the empty string, the starting
                     sector number is set to 0, the directory information is
                     properly initialized, and TRUE is returned.

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

   Shared Variables: cur_dir             - accessed and updated to the current
                                           entry.
                     cur_info            - set to the FAT information for the
                                           current directory entry.
                     dir_info            - accessed to get the starting
                                           cluster of the current directory.
                     dir_offset          - accessed and possibly updated to
                                           the sector offset of the directory
                                           entries.
                     dir_sector          - accessed and possibly filled with a
                                           sector of directory entries.
                     FAT_cache           - filled with the FAT chain for the
                                           current directory entry.
                     filename            - set to the filename of the current
                                           entry.
                     first_file_sector   - accessed to compute starting sector
                                           of an entry.
                     sectors_per_cluster - accessed to compute starting sector
                                           of an entry.

   Author:           Glen George
   Last Modified:    March 24, 2013

*/

char  get_next_dir_entry()
{
    /* variables */
    char  longfilename[MAX_LFN_LEN];    /* long filename of current entry */
    int   lfn_seq;                      /* sequence number for LFN */
    int   chksum;                       /* long filename checksum */

    struct  cache_entry  e;             /* a FAT cache entry */
    unsigned long int    next;          /* pointer to next FAT cluster */

    unsigned long int  old_dir_offset;  /* previous directory offset */
    int                old_cur_dir;     /* old file entry in directory */

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
            error = (get_disk_blocks(&dir_info, dir_offset, 1,
                               (unsigned short int far *) dir_sector) != 1);
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
                        error = (get_disk_blocks(&dir_info, old_dir_offset, 1,
                                     (unsigned short int far *) dir_sector) != 1);
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
                        /* so get starting cluster number and parent name */
                        cur_info.cluster1 = get_dir_tos_sector();
                        strcpy(filename, get_dir_tos_name());

                        /* also need to fill in the rest of the block info */
                        /* check whether this is the FAT16 root directory */
                        if (cur_info.cluster1 == 0)  {
                            /* FAT16 root directory, handle it specially */
                            cur_info.sector = root_start_sector;
                            cur_info.size = root_dir_size;
                            cur_info.next = CHAIN_END;
                        }
                        else  {
                            /* not FAT16 root, get the directory information */
                            cur_info.next = get_contig_sectors(cur_info.cluster1, &e);
                            cur_info.sector = (e.cluster - 2) * sectors_per_cluster + first_file_sector;
                            cur_info.size = e.size;
                        }

                        /* always at start of the directory */
                        cur_info.offset = 0;
                        /* and never use a cache for directories */
                        cur_info.cache_idx = -1;

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

                    /* need to set the file information, FAT cache, and filename */
                    /* get the starting cluster based on FAT type */
                    if (fat16)
                        next = START_CLUSTER(dir_sector[cur_dir]);
                    else
                        next = START_CLUSTER32(dir_sector[cur_dir]);

                    /* fill the FAT cache for this file */
                    for (i = 0;
                         ((i + 1) < (FAT_CACHE_SIZE / sizeof(struct cache_entry))) && (next != CHAIN_END);
                         i++)  {

                        /* get the contiguous sectors at current position */
                        next = get_contig_sectors(next, &e);

                        /* add this entry to the cache */
                        FAT_cache[i].cluster = e.cluster;
                        FAT_cache[i].size = e.size;
                    }

                    /* fill in last cache entry with the last next pointer */
                    /*    but only if there is room in the FAT cache */
                    if (i < (FAT_CACHE_SIZE / sizeof(struct cache_entry)))  {
                        FAT_cache[i].cluster = next;
                        FAT_cache[i].size = 0;
                    }

                    /* now set up the block information */
                    /* get the first cluster from the directory information */
                    if (fat16)
                        cur_info.cluster1 = START_CLUSTER(dir_sector[cur_dir]);
                    else
                        cur_info.cluster1 = START_CLUSTER32(dir_sector[cur_dir]);
                    /* at start of file */
                    cur_info.offset = 0;

                    /* get info from first two cache entries if they exist */
                    if ((FAT_CACHE_SIZE / sizeof(struct cache_entry)) > 1)  {

                        /* the FAT cache exists - get info there */
                        cur_info.sector = (FAT_cache[0].cluster - 2) * sectors_per_cluster + first_file_sector;
                        cur_info.size = FAT_cache[0].size;
                        cur_info.next = FAT_cache[1].cluster;
                        /* at start of FAT cache */
                        cur_info.cache_idx = 0;
                    }
                    else  {

                        /* no FAT cache, so get information from hard drive */
                        /* get the contiguous sectors at start of file position */
                        cur_info.next = get_contig_sectors(cur_info.cluster1, &e);
                        /* add this information to the file information */
                        cur_info.sector = (e.cluster - 2) * sectors_per_cluster + first_file_sector;
                        cur_info.size = e.size;
                        /* no cache so no index */
                        cur_info.cache_idx = -1;
                    }

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
        /* set the file info to the first sector */
        cur_info.sector    = first_file_sector;
        cur_info.size      = sectors_per_cluster;
        cur_info.next      = CHAIN_END;
        cur_info.offset    = 0;
        cur_info.cluster1  = 2;
        cur_info.cache_idx = -1;
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

   Shared Variables: cur_dir       - accessed and updated to the current
                                     entry.
                     cur_info      - set to the block information for the
                                     current entry.
                     dir_offset    - accessed and possibly updated to the
                                     sector offset of the directory entries.
                     dir_sector    - accessed and possibly filled with a
                                     sector of directory entries.
                     filename      - set to the filename of the current entry.

   Author:           Glen George
   Last Modified:    April 3, 2013

*/

char  get_previous_dir_entry()
{
    /* variables */
    unsigned long int  new_offset;      /* new directory sector offset */
    int                new_entry;       /* new directory entry */

    char  error = FALSE;        /* read error flag */
    char  done = FALSE;         /* done getting the previous directory info */
    char  have_entry = FALSE;   /* have the entry (but maybe not the filename) */



    /* find the previous entry in this directory */
    /* loop until find the entry or get an error */
    while (!error && !done)  {

        /* check if need to read a new sector's worth of entries */
        if (cur_dir == 0)  {

            /* need to read in the previous sector of directory entries */
            /* check if out of directory entries */
            if (dir_offset == 0)  {
                /* out of directory entries - reset to the start */
                new_offset = 0;
                new_entry = 0;
                /* and done */
                done = TRUE;
            }
            else  {
                /* have a previous directory entry to check, read the */
                /*    sector of directory entries, watching for an error */
                error = (get_disk_blocks(&dir_info, --dir_offset, 1,
                                 (unsigned short int far *) dir_sector) != 1);
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

            /* check if have already found a previous entry */
            if (have_entry)  {

                /* already found a previous entry, but need to skip its */
                /* potentional long filename too */
                if (ATTR(dir_sector[cur_dir]) != ATTRIB_LFN)  {
                    /* not a long filename, must be done */
                    done = TRUE;
                }
                else  {
                    /* still part of a long filename for the entry */
                    /* so remember that this is potentially where new entry will start */
                    new_offset = dir_offset;
                    new_entry = cur_dir;
                }
            }
            else  {

                /* have not found a previous entry yet, is this one */
                /* ignore empty entries, deleted entries, long filenames, */
                /*    volume labels, and '.' directory */
                if ((FILENAME(dir_sector[cur_dir], 0) != '\0')  &&
                     (FILENAME(dir_sector[cur_dir], 0) != '\xE5')  &&
                     (ATTR(dir_sector[cur_dir]) != ATTRIB_LFN) &&
                     (ATTR(dir_sector[cur_dir]) != ATTRIB_VOLUME) &&
                     ((FILENAME(dir_sector[cur_dir], 0) != '.')  ||
                      (FILENAME(dir_sector[cur_dir], 1) == '.')))  {

                    /* have the previous directory entry, remember that */
                    have_entry = TRUE;
                    /* and keep track of where the entry starts */
                    new_offset = dir_offset;
                    new_entry = cur_dir;
                }
                else  {

                    /* not a previous entry, ignore it and keep looking */
                    ;
                }
            }
        }
    }

    /* if no error, update to the new sector, offset, and directory entry */
    if (!error)  {

        /* first read in the new sector if had moved past it */
        if (new_offset != dir_offset)
            error = (get_disk_blocks(&dir_info, new_offset, 1,
                                 (unsigned short int far *) dir_sector) != 1);
        /* now update the sector offset and directory entry */
        dir_offset = new_offset;
        cur_dir = new_entry - 1;    /* get_next_dir_entry() will inc this */
    }


    /* if finished without an error then get the actual previous filename */
    if (done & !error)  {

        /* get the entry watching for errors */
        /* since we've backed up past the previous entry, this will now */
        /*    find the previous entry */
        error = get_next_dir_entry();
    }


    /* check if there was an error */
    /*    note that this is redundant if error is from get_next_dir_entry() */
    /*    but it's needed if error is from get_disk_blocks() */
    if (error)  {
        /* had an error - clear out the data */
        /* clear the filename */
        filename[0] = '\0';
        /* set the file info to the first sector */
        cur_info.sector    = first_file_sector;
        cur_info.size      = sectors_per_cluster;
        cur_info.next      = CHAIN_END;
        cur_info.offset    = 0;
        cur_info.cluster1  = 2;
        cur_info.cache_idx = 0;
    }


    /* finally done, return with the error status */
    return  error;

}




/* local functions to support FAT16/FAT32 fragmented files */


/*
   get_disk_blocks

   Description:      This function reads blocks from the file whose
                     information block is passed as the first argument.  The
                     data "read" is written to the memory pointed to by the
                     fourth argument.  The number of blocks requested is given
                     as the third argument.  The starting block number in the
                     file to read is passed as the second argument.  The
                     number of sectors (blocks) actually read is returned.

   Arguments:        info (struct block_info *)      - block information to be
                                                       used and possibly
                                                       updated by this
                                                       function to get file
                                                       data.
                     block (unsigned long int)       - sector number (relative
                                                       to the start of the
                                                       file) at which to start
                                                       reading.
                     length (int)                    - number of blocks to be
                                                       read.
                     dest (unsigned short int far *) - pointer to the memory
                                                       where the read data is
                                                       to be written.
   Return Value:     The number of blocks actually read.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 17, 2013

*/

int  get_disk_blocks(struct block_info *info, unsigned long int block,
                     int length, unsigned short int far *dest)
{
    /* variables */
    int   sectors_read = 0;     /* total number of sectors actually read */

    int   xfer_cnt;             /* number of blocks to try to read */
    int   blk_cnt;              /* number of blocks read by get_blocks */

    char  error = FALSE;        /* error reading the disk */



    /* read contiguous groups of blocks until error or all are read */
    while (!error && (sectors_read < length))  {

        /* see if can get sectors from current file block */
        if ((block < info->offset) || (block >= (info->offset + info->size)))
            /* the sector isn't in current block, get new block */
            get_block_info(info, block);

        /* should now be able to find/read the desired sectors */
        /* see how many sectors are contiguous */
        if ((info->offset + info->size - block) >= (length - sectors_read))
            /* all the sectors we need are contiguous in this block */
            xfer_cnt = length - sectors_read;
        else
            /* can only get contiguous sectors up to the end of the block */
            xfer_cnt = info->offset + info->size - block;

        /* now call the get_blocks function to actually read sectors */
        blk_cnt = get_blocks(info->sector + block - info->offset, xfer_cnt, dest);

        /* update the state of the transfer */
        block += blk_cnt;                   /* update next block to be read */
        sectors_read += blk_cnt;            /* update total sectors read */
        dest += blk_cnt * IDE_BLOCK_SIZE;   /* update buffer position */

        /* check for an error */
        if (blk_cnt < xfer_cnt)
            /* couldn't read all the sectors so an error must have occurred */
            error = TRUE;
    }


    /* return the number of sectors actually read */
    return  sectors_read;

}




/*
   get_block_info

   Description:      This function fills in the passed block information
                     structure with information from the FAT on the hard
                     drive.  The passed sector within the file along with the
                     current value of the block information are used to figure
                     out which block to load the structure with.  The cluster1
                     element of the structure is not changed.

   Arguments:        info (struct block_info *) - block information to be used
                                                  and updated by this
                                                  function.
                     sector (unsigned long int) - sector number within a file
                                                  for which the block
                                                  information is to be found.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: FAT_cache - accessed to get block information.

   Author:           Glen George
   Last Modified:    March 17, 2013

*/

static  void  get_block_info(struct block_info *info, unsigned long int sector)
{
    /* variables */
    struct  cache_entry  e;                     /* information on clusters */

    char                 have_block = FALSE;    /* have the requested block */



    /* if don't have the block yet, check if the sector is in a later cache entry */
    while (!have_block && (info->cache_idx != -1) &&
           ((info->cache_idx + 1) < (FAT_CACHE_SIZE / sizeof(struct cache_entry))) &&
           (info->next != CHAIN_END) &&
           (sector >= (info->offset + info->size)))  {

        /* move to the next FAT cache entry (they are in file order) */
        info->offset += info->size;
        info->cache_idx++;
        info->sector = (FAT_cache[info->cache_idx].cluster - 2) * sectors_per_cluster +
                       first_file_sector;
        info->size = FAT_cache[info->cache_idx].size;
        info->next = FAT_cache[info->cache_idx + 1].cluster;
    }

    /* check if have the sector now */
    if ((sector >= info->offset) && (sector < (info->offset + info->size)))
        /* have the sector in this new block */
        have_block = TRUE;


    /* if don't have the block yet, check if the sector is in an earlier cache entry */
    while (!have_block && (info->cache_idx > 0) &&
           (info->next != CHAIN_END) && (sector < info->offset))  {

        /* move to the previous FAT cache entry (they are in file order) */
        info->cache_idx--;
        info->offset -= FAT_cache[info->cache_idx].size;
        info->sector = (FAT_cache[info->cache_idx].cluster - 2) * sectors_per_cluster +
                       first_file_sector;
        info->size = FAT_cache[info->cache_idx].size;
        info->next = FAT_cache[info->cache_idx + 1].cluster;
    }

    /* check if have the sector now */
    if ((sector >= info->offset) && (sector < (info->offset + info->size)))
        /* have the sector in this new block */
        have_block = TRUE;


    /* if still don't have the sector will need to search the FAT on the */
    /*    hard drive for it - either start at beginning or next cluster */
    if (!have_block)  {

        /* still don't have it, check where to start searching */
        if (sector < info->offset)  {
            /* moving backward in file, have to start at beginning */
            info->next = info->cluster1;
            /* reset block information to zero sized block at beginning of file */
            info->size = 0;
            info->offset = 0;
        }
    }

    /* now search through the hard drive FAT for the sector */
    while (!have_block && (info->next != CHAIN_END) &&
           (sector >= (info->offset + info->size)))  {

        /* get information on the contiguous sectors starting at current cluster */
        info->next = get_contig_sectors(info->next, &e);

        /* now fill in the block information based on this */
        /* watch for special case of FAT16 root directory */
        if (info->cluster1 == 0)
            /* FAT16 root use FAT16 starting sector, not returned cluster */
            info->sector = root_start_sector;
        else
            /* not FAT16 root, use returned cluster to get sector */
            info->sector = (e.cluster - 2) * sectors_per_cluster + first_file_sector;

        /* get the rest of the information */
        info->offset += info->size;
        info->size = e.size;

        /* getting information directly from hard drive so no cache index */
        info->cache_idx = -1;
    }


    /* we've looked everywhere at this point so we'd better have the sector */
    /* no way to report errors if we don't, so just return now */
    return;

}




/*
   get_contig_sectors

   Description:      This function reads the FAT information on the hard drive
                     to fill in the passed structure with information for the
                     passed cluster.  The returned information gives the
                     number of contiguous sectors starting at the passed
                     cluster number.  The cluster number of the first
                     non-contiguous cluster is returned.

   Operation:        The function first checks for the special cases of
                     cluster 0 (FAT16 root directory) and END_CHAIN (returns
                     a zero length entry).  If it is neither special case the
                     first FAT on the hard drive is read for the passed
                     cluster number and the information for that entry is
                     entered in the structure.  The function continues reading
                     the FAT as long as the clusters are contiguous, updating
                     the size of the contiguous block as it goes.

   Arguments:        cluster (unsigned long int)  - starting cluster number 
                                                    at which the number of
                                                    contiguous sectors is to
                                                    be found.
                     entry (struct cache_entry *) - cache entry (general FAT)
                                                    information to be filled
                                                    in by this function.
   Return Value:     (unsigned long int) - cluster number of the first
                     non-contiguous cluster following the passed cluster
                     number, CHAIN_END if the end of the file is reached
                     before a non-contiguous cluster is found.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: clusters_per_sector - accessed.
                     fat16               - accessed to determine FAT type.
                     first_FAT_sector    - accessed.
                     first_file_sector   - accessed.
                     root_dir_size       - accessed if cluster is FAT16 root.
                     root_start_sector   - accessed if cluster is FAT16 root.
                     sectors_per_cluster - accessed.

   Author:           Glen George
   Last Modified:    March 17, 2013

*/

static  unsigned long int  get_contig_sectors(unsigned long int cluster,
                                              struct cache_entry *entry)
{
    /* variables */
    unsigned long int   s;                      /* sector number of FAT entry */
    unsigned long int   c;                      /* cluster offset of FAT entry */

    unsigned long int   next;                   /* next cluster from FAT */

    unsigned short int  sector[IDE_BLOCK_SIZE]; /* sector from the hard drive */

    char                contig = TRUE;          /* clusters are contiguous */
    char                error = FALSE;          /* error flag */



    /* first check for special cluster values */
    if (cluster == 0)  {

        /* zero cluster is illegal, so this indicates want FAT16 root */
        entry->cluster = root_start_sector;     /* use root values */
        entry->size = root_dir_size;
        next = CHAIN_END;                       /* no next cluster */
    }
    else if (cluster == CHAIN_END)  {

        /* end of chain indicator, return a zero length cluster */
        entry->cluster = 0;
        entry->size = 0;
        next = CHAIN_END;                       /* no next cluster */
    }
    else  {

        /* normal cluster number */
        /* first sector for entry is based on cluster number */
        entry->cluster = cluster;
        /* nothing in it yet */
        entry->size = 0;

        /* now try to get info from FAT */
        /* first get sector number for the cluster entry */
        s = cluster / clusters_per_sector + first_FAT_sector;
        /* get cluster entry within the sector */
        c = cluster % clusters_per_sector;
        /* and read the sector from the hard drive */
        error = (get_blocks(s, 1, (unsigned short int far *) sector) != 1);

        /* while there are contiguous clusters, get the FAT information */
        while (contig && !error)  {

            /* check if cluster number is still in this sector */
            if (c >= clusters_per_sector)  {
                /* finished entries in this sector, move to next */
                s++;
                /* at the start of this sector */
                c = 0;
                /* and read the sector from the hard drive */
                error = (get_blocks(s, 1, (unsigned short int far *) sector) != 1);
            }

            /* get the next cluster number (based on FAT type) */
            if (fat16)
                /* FAT16 so each entry is one word */
                next = sector[c];
            else
                /* FAT32, each entry is two words */
                next = ((unsigned long int *) sector)[c];

            /* check if the cluster contiguous */
            /* note that end of chain markers will not be contiguous */
            contig = (next == (cluster + 1));

            /* this cluster was already found to be contiguous, so update size */
            entry->size += sectors_per_cluster;

            /* can update the cluster number assuming contiguous */
            cluster++;
            /* and move to next cluster in this FAT sector */
            c++;
        }

        /* set the next cluster pointer in chain to CHAIN_END if there was */
        /* an error or hit the end of the cluster chain in the FAT */
        if (error || (fat16 && (next >= FAT16_BAD)) ||
            (!fat16 && (next >= FAT32_BAD)))  {
            /* have an error or a bad cluster or end of chain marker */
            /* next pointer is end of chain */
            next = CHAIN_END;
        }
    }


    /* done getting the cluster information, return */
    return  next;

}




/* locally global variables for the stack routines */

/* stack of directory information */
static  char               dirnames[MAX_PATH_CHARS];            /* names */
static  unsigned long int  dirclusterstack[MAX_NUM_SUBDIRS];    /* starting clusters */
static  int                dirnamestack[MAX_NUM_SUBDIRS];       /* name positions in dirnames */
static  int                dirstack_ptr;                        /* the stack pointer */




/*
   init_dir_stack

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

   Shared Variables: dirclusterstack - first element is set to 0.
                     dirnames        - first character is set to '\0'.
                     dirnamestack    - first element is set to 0.
                     dirstack_ptr    - initialized to -1.

   Author:           Glen George
   Last Modified:    March 17, 2013

*/

static  void  init_dir_stack()
{
    /* variables */
      /* none */



    /* set the string of names to the empty string */
    dirnames[0] = '\0';

    /* initialize the first directory entries to 0 */
    dirclusterstack[0] = 0;
    dirnamestack[0] = 0;

    /* finally, set the stack pointer to empty stack */
    dirstack_ptr = -1;


    /* all done with the initialization - return */
    return;

}




/*
   new_directory

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

   Shared Variables: cur_info        - accessed to check if the top of the
                                       stack matches the current
                                       file/directory.
                     dir_info        - accessed for the directory starting
                                       cluster number.
                     dirclusterstack - may be updated to add a directory
                                       starting cluster location.
                     dirname         - accessed to get the current directory
                                       name.
                     dirnames        - updated to add or remove directory
                                       names.
                     dirnamestack    - may be updated to add the starting
                                       character number in dirnames[] for this
                                       directory name.
                     dirstack_ptr    - updated to adjust the stack.

   Author:           Glen George
   Last Modified:    March 17, 2013

*/

static  void  new_directory()
{
    /* variables */
      /* none */



    /* check if the current entry matches the top of the stack */
    if ((dirstack_ptr >= 0) && (dirclusterstack[dirstack_ptr] == cur_info.cluster1))  {

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
            /* save the starting cluster */
            dirclusterstack[dirstack_ptr] = dir_info.cluster1;
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
   get_dir_tos_name

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
   get_dir_tos_cluster

   Description:      This function returns the starting cluster number of the
                     directory on the top of the directory stack.  If the
                     stack is empty, zero (0) is returned.

   Arguments:        None.
   Return Value:     (unsigned long int) - the starting cluster of the
                     directory on the top of the directory stack or zero (0)
                     if there is nothing on the stack.

   Input:            None.
   Output:           None.

   Error Handling:   If nothing is on the stack, zero (0) is returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: dirclusterstack - accessed to get cluster number.
                     dirstack_ptr    - accessed to get cluster number.

   Author:           Glen George
   Last Modified:    March 17, 2013

*/

static  unsigned long int  get_dir_tos_sector()
{
    /* variables */
    unsigned long int  c;       /* cluster number to return */



    /* check if there is something in the directory stack */
    if (dirstack_ptr >= 0)  {

        /* there is something on the stack, return the cluster number */
        c = dirclusterstack[dirstack_ptr];
    }
    else  {

        /* nothing on the stack, return 0 */
        c = 0;
    }


    /* all done - return with the starting cluster number */
    return  c;

}
