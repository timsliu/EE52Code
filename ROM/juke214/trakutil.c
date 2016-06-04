/****************************************************************************/
/*                                                                          */
/*                                TRAKUTIL                                  */
/*                         Track Utility Functions                          */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS 52                                  */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the utility functions for dealing with tracks used by
   the background routines of the MP3 Jukebox Project.  The current track
   header and buffer are also defined in this file (locally).  The functions
   included are:
      get_track_artist           - return the artist for the current track
      get_track_length           - get number of bytes in the current track
      get_track_position         - get the current position on the track
      get_track_block_position   - get the current block position on the track
      get_track_remaining_length - get number of bytes left on current track
      get_track_time             - return the current time for a track
      get_track_total_time       - return the total time for a track
      get_track_title            - return the title of the current track
      init_track                 - initialize to the start of the track
      init_tracks                - initialize the track information
      setup_cur_track_info       - setup info buffer for current track
      setup_error_track_info     - setup info buffer for an error track
      update_track_position      - update the position on the track

   The local functions included are:
      none

   The locally global variable definitions included are:
      track_info        - information on the current track
      track_info_buffer - buffer holding the current track string information


   Revision History
      6/6/00   Glen George       Initial revision (from the 3/6/99 version of
                                 updatfnc.h for the Digital Audio Recorder
                                 Project).
      6/7/00   Glen George       Added local get_track_info() function.
      6/7/00   Glen George       Fixed overflow problem in get_track_time().
      6/7/00   Glen George       Added get_track_block_position() function.
      6/14/00  Glen George       Made get_track_info() function static so it
                                 really is local.
      6/14/00  Glen George       Correctly updated get_track_time().
      6/14/00  Glen George       Fixed the offset for the title in function
                                 get_track_info().
      6/14/00  Glen George       Changed track_info_buffer[] to unsigned char
                                 to fix some type conversion warnings.
      6/14/00  Glen George       Added casts to functions get_track_title()
                                 and get_track_artist() to fix some type
                                 conversion warnings.
      6/2/02   Glen George       Added get_track_total_time() function.
      6/2/02   Glen George       Updated comments.
      6/10/02  Glen George       Added use of SECTOR_ADJUST constant for
                                 dealing with hard drives with different
                                 geometries.
      5/15/03  Glen George       Made get_track_info() more compiler
                                 independent by masking data appropriately and
                                 eliminating indexing which relies on the size
                                 of built-in types.
      5/15/03  Glen George       Made system output "Error" as song title and
                                 set times and lengths to reasonable values if
                                 there is an error reading the track
                                 information.
      6/5/03   Glen George       Removed functions and variables supporting
                                 track numbers (update_track_no(),
                                 get_track_no(), and track_number).
      6/5/03   Glen George       Split get_track_info() into two functions:
                                 setup_cur_track_info() to handle "normal"
                                 tracks and setup_error_track_info() to handle
                                 tracks with errors, both are compatible with
                                 FAT file systems.
      6/5/03   Glen George       Added #include of fatutil.h to support the
                                 above changes.
      6/5/03   Glen George       Modified get_track_time() to handle TIME_NONE
                                 and zero length tracks.
      6/5/03   Glen George       Updated function headers.
      5/13/05  Glen George       Modified get_track_time() to just use actual
                                 track time at the start of the track to
                                 eliminate rounding errors in the computation.
      6/4/06   Glen George       Fixed a bug in get_track_block_position to
                                 account for IDE_BLOCK_SIZE being in words
                                 instead of bytes now.
*/



/* library include files */
  /* none */

/* local include files */
#include  "interfac.h"
#include  "mp3defs.h"
#include  "trakutil.h"
#include  "fatutil.h"
#include  "vfat.h"




/* locally global variables */
static struct track_header  track_info;     /* current track information */
static unsigned char        track_info_buffer[MAX_LFN_LEN + 1]; /* buffer holding the current information */




/* local function declarations */
void  get_track_info(void);      /* read the track information from disk */




/*
   init_track

   Description:      This function initializes the current track to the start
                     of the track.  The current position (curpos element) is
                     set to the start of the track.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: track_info - the curpos element is reset.

   Author:           Glen George
   Last Modified:    June 6, 2000

*/

void  init_track()
{
    /* variables */
      /* none */



    /* initialize the current position to the start of the track */
    track_info.curpos = 0;


    /* all done, return */
    return;

}




/*
   get_track_artist

   Description:      This function returns the artist for the track.

   Arguments:        None.
   Return Value:     (const char *) - pointer to the string containing the
                     artist for the current track.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: track_info - the artist element is returned.

   Author:           Glen George
   Last Modified:    June 14, 2000

*/

const char *  get_track_artist()
{
    /* variables */
      /* none */



    /* return the artist name */
    return  (const char *) track_info.artist;

}




/*
   get_track_title

   Description:      This function returns the title for the track.

   Arguments:        None.
   Return Value:     (const char *) - pointer to the string containing the
                     title of the current track.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: track_info - the title element is returned.

   Author:           Glen George
   Last Modified:    June 14, 2000

*/

const char *  get_track_title()
{
    /* variables */
      /* none */



    /* return the artist name */
    return  (const char *) track_info.title;

}




/*
   get_track_length

   Description:      This function returns the length of the track in bytes.

   Arguments:        None.
   Return Value:     (long int) - the length of the track in bytes.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: track_info - the length element is returned.

   Author:           Glen George
   Last Modified:    June 5, 2000

*/

long int  get_track_length()
{
    /* variables */
      /* none */



    /* return the track length */
    return  track_info.length;

}




/*
   get_track_position

   Description:      This function returns the current position of the track
                     in bytes from the start of the track.

   Arguments:        None.
   Return Value:     (long int) - the current position (offset from start) on
                     the track.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: track_info - the curpos element is returned.

   Author:           Glen George
   Last Modified:    June 5, 2000

*/

long int  get_track_position()
{
    /* variables */
      /* none */



    /* return the current position */
    return  track_info.curpos;

}




/*
   get_track_block_position

   Description:      This function returns the current position of the track.
                     This is the position on the hard drive (the block
                     number).

   Arguments:        None.
   Return Value:     (long int) - the current position (block number on hard
                     drive) on the track.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: track_info - the curpos and start elements are used.

   Author:           Glen George
   Last Modified:    June 4, 2006

*/

long int  get_track_block_position()
{
    /* variables */
      /* none */



    /* return the current position (on the hard drive, in blocks) */
    /* this assumes that only complete blocks are read, thus curpos */
    /*    is evenly divisible by IDE_BLOCK_SIZE                     */
    /* remember that IDE_BLOCK_SIZE is in words, but curpos is in bytes */
    return  track_info.start_block + (track_info.curpos / (2 * IDE_BLOCK_SIZE));

}




/*
   get_track_remaining_length

   Description:      This function returns the number of bytes left on the
                     track.  This is the total length less the current
                     position.

   Arguments:        None.
   Return Value:     (long int) - the bytes left on the track.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: track_info - the curpos and length elements are used.

   Author:           Glen George
   Last Modified:    June 5, 2000

*/

long int  get_track_remaining_length()
{
    /* variables */
    long int  bytes_left;



    /* compute the number of bytes left */
    bytes_left = track_info.length - track_info.curpos;

    /* make sure it isn't negative */
    if (bytes_left < 0)
        bytes_left = 0;


    /* return the bytes remaining */
    return  bytes_left;

}




/*
   get_track_time

   Description:      This function returns the current time (time remaining)
                     for the passed track.  If the track has a total time
                     defined for it, the current time is computed by taking
                     the ratio of the current position to the total track
                     length and multiplying that by the total time.

   Arguments:        None.
   Return Value:     (int) - the remaining time for the passed track (in
                     tenths of seconds) or TIME_NONE if there is no time
                     information for the track.

   Input:            None.
   Output:           None.

   Error Handling:   There is no checking for overflow.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: track_info - the current time is computed from the time,
                                  curpos, and length elements.

   Author:           Glen George
   Last Modified:    May 13, 2005

*/

int  get_track_time()
{
    /* variables */
      /* none */



    /* just compute and return the time remaining on the track */
    /* check if a time calculation even makes sense */
    if ((track_info.time != TIME_NONE) && (track_info.time != 0) && (track_info.length > track_info.time))  {
        /* have time on the track, compute time remaining */
        /* check if at start of the track or in middle */
        if (track_info.curpos == 0)
            /* at start, just return total time */
            return  track_info.time;
        else
            /* in middle, compute time remaining */
            return  (track_info.length - track_info.curpos) / (track_info.length / track_info.time);
    }
    else  {
        /* else, no time or length information on track - return nothing */
        return  TIME_NONE;
    }

}




/*
   get_track_total_time

   Description:      This function returns the total time for the passed
                     track in tenths of seconds.

   Arguments:        None.
   Return Value:     (int) - the total time for the passed track (in tenths of
                     seconds).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: track_info - the time element is accessed and returned.

   Author:           Glen George
   Last Modified:    June 1, 2002

*/

int  get_track_total_time()
{
    /* variables */
      /* none */



    /* just return the total time */
    return  track_info.time;

}




/*
   update_track_position

   Description:      This function updates the current position on the track
                     by the passed amount.

   Arguments:        delta (long int) - amount by which to update the current
                                        position.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: track_info - the curpos element is updated.

   Author:           Glen George
   Last Modified:    June 6, 2000

*/

void  update_track_position(long int delta)
{
    /* variables */
      /* none */



    /* update the current position */
    track_info.curpos += delta;

    /* make sure not out of range */
    if (track_info.curpos < 0)
        track_info.curpos = 0;
    if (track_info.curpos > track_info.length)
        track_info.curpos = track_info.length;


    /* now return */
    return;

}




/*
   setup_cur_track_info

   Description:      This function loads the information for the current
                     track/file from the hard drive and initializes the track
                     information data structure.  The track is positioned to
                     the start of the track.

   Arguments:        None.
   Return Value:     None.

   Input:            The new track information is read from the hard drive.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: track_info        - updated.
                     track_info_buffer - updated.

   Author:           Glen George
   Last Modified:    June 5, 2003

*/

void  setup_cur_track_info()
{
    /* variables */
    const char  *s;             /* general string pointer */
    int          i;             /* loop index */



    /* get the track/file information from the disk directory information */

    /* fill in the numeric values from directory information */
    /* length (in bytes) of the song/file */
    track_info.length = get_cur_file_size();
    /* starting sector number of the song/file */
    track_info.start_block = get_cur_file_sector();
    /* length (in time) of the song/file - watch out for directories */
    if (cur_isDir())
        /* currently on a directory, not a song - time is special */
        track_info.time = TIME_NONE;
    else
        /* on a song/file - get the length (in tenths of seconds) */
        track_info.time = 10 * get_cur_file_time();


    /* now setup the title and artist */

    /* first need to copy the filename into the buffer */
    s = get_cur_file_name();

    /* check if needs a directory symbol in front of the name */
    if (cur_isDir())  {
        /* is a directory, see which kind */
        if (cur_isParentDir())
            /* parent directory, put in "up directory character" */
            track_info_buffer[0] = PARENT_DIR_CHAR;
        else
            /* sub-directory, put in approipriate character */
            track_info_buffer[0] = SUBDIR_CHAR;
        /* and start filling at second character */
        i = 1;
    }
    else  {
        /* no directory character, start at beginning of the buffer */
        i = 0;
    }

    /* do the copy */
    do  {
        track_info_buffer[i++] = *s;
    }  while (*s++ != '\0');

    /* now figure out the artist and title position within the filename */
    /* title is always at the start of the buffer */
    track_info.title = &(track_info_buffer[0]);

    /* search for the artist */
    for (i = 0; ((track_info_buffer[i] != '\0') && (track_info_buffer[i] != END_TITLE_CHAR)); i++);

    /* check if found the end of the title or the end of the filename */
    if (track_info_buffer[i] == END_TITLE_CHAR)  {

        /* found the end of the title, terminate the title string */
        track_info_buffer[i] ='\0';
        /* and the artist starts on the next character */
        track_info.artist = &(track_info_buffer[i + 1]);
    }
    else  {

        /* hit end of string - there is no artist */
        /* so the pointer should point to the end of string */
        track_info.artist = &(track_info_buffer[i]);
    }


    /* always reset to the start of the track */
    track_info.curpos = 0;


    /* finally done so return */
    return;

}




/*
   setup_error_track_info

   Description:      This function loads the information for the current
                     track/file with error information.  This is means no
                     time and a title and artist name of "Error".  The track
                     is positioned the start of the track.

   Arguments:        None.
   Return Value:     None.

   Input:            The new track information is preset to error information.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: track_info        - updated.
                     track_info_buffer - updated.

   Author:           Glen George
   Last Modified:    June 5, 2003

*/

void  setup_error_track_info()
{
    /* variables */
      /* none */    



    /* first fill the buffer with the Error string so can use it for title and artist */
    track_info_buffer[0] = 'E';
    track_info_buffer[1] = 'r';
    track_info_buffer[2] = 'r';
    track_info_buffer[3] = 'o';
    track_info_buffer[4] = 'r';
    track_info_buffer[5] = '\0';

    /* now fill in the track information buffer */
    track_info.start_block = 0;
    track_info.length = 9999;
    track_info.time = TIME_NONE;
    track_info.artist = &(track_info_buffer[0]);
    track_info.title = &(track_info_buffer[0]);


    /* always reset to the start of the track */
    track_info.curpos = 0;


    /* done filling with error data so return */
    return;

}
