/****************************************************************************/
/*                                                                          */
/*                                KEYUPDAT                                  */
/*            Miscellaneous Key Processing and Update Functions             */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS  52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the key processing and update functions for operations
   other than Play, Record, Fast Forward, and Reverse for the MP3 Jukebox
   Project.  These functions are called by the main loop of the MP3 Jukebox.
   The functions included are:
      do_TrackUp      - go to the next track (key processing function)
      do_TrackDown    - go to the previous track (key processing function)
      no_action       - nothing to do (key processing function)
      no_update       - nothing to do (update function)
      stop_idle       - stop when doing nothing (key processing function)

   The local functions included are:
      none

   The global variable definitions included are:
      none


   Revision History
      6/6/00   Glen George       Initial revision (from 3/6/99 version of
                                 keyupdat.c for the Digital Audio Recorder
                                 Project).
      6/2/02   Glen George       Updated comments.
      6/5/03   Glen George       Updated do_TrackUp and do_TrackDown to handle
                                 directory traversal in order to support FAT
                                 directory structures, they now go up and down
                                 in the current directory.
      6/5/03   Glen George       Added #include of fatutil.h for function
                                 declarations needed by above change.
      6/5/03   Glen George       Updated function headers.
*/



/* library include files */
  /* none */

/* local include files */
#include  "interfac.h"
#include  "mp3defs.h"
#include  "keyproc.h"
#include  "updatfnc.h"
#include  "trakutil.h"
#include  "fatutil.h"




/*
   no_action

   Description:      This function handles a key when there is nothing to be
                     done.  It just returns.

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new status (same as current status).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    Mar. 5, 1994

*/

enum status  no_action(enum status cur_status)
{
    /* variables */
      /* none */



    /* return the current status */
    return  cur_status;

}




/*
   do_TrackUp

   Description:      This function handles the <Track Up> key when nothing is
                     happening in the system.  It moves to the previous entry
                     in the directory and resets the track time and loads the
                     track information for the new track.

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new status (same as current status).

   Input:            None.
   Output:           The new track information is output.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 5, 2003

*/

enum status  do_TrackUp(enum status cur_status)
{
    /* variables */
      /* none */



    /* move to the previous directory entry, watching for errors */
    if (!get_previous_dir_entry())
        /* successfully got the new entry, load its data */
        setup_cur_track_info();
    else
        /* there was an error - load error track information */
        setup_error_track_info();


    /* display the track information for this track */
    display_time(get_track_time());
    display_title(get_track_title());
    display_artist(get_track_artist());


    /* done so return the current status */
    return  cur_status;

}




/*
   do_TrackDown

   Description:      This function handles the <Track Down> key when nothing
                     is happening in the system.  It moves to the next entry
                     in the directory and resets the track time and loads the
                     track information for the new track.

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new status (same as current status).

   Input:            None.
   Output:           The new track information is output.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 5, 2003

*/

enum status  do_TrackDown(enum status cur_status)
{
    /* variables */
      /* none */



    /* move to the next directory entry, watching for errors */
    if (!get_next_dir_entry())
        /* successfully got the new entry, load its data */
        setup_cur_track_info();
    else
        /* there was an error - load error track information */
        setup_error_track_info();


    /* display the track information for this track */
    display_time(get_track_time());
    display_title(get_track_title());
    display_artist(get_track_artist());


    /* done so return the current status */
    return  cur_status;

}




/*
   stop_idle

   Description:      This function handles the <Stop> key when nothing is
                     happening in the system.  It just resets the track time
                     and variables to indicate the start of the track.

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new status (same as current status).

   Input:            None.
   Output:           The new track time (the track length) is output.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 3, 2000

*/

enum status  stop_idle(enum status cur_status)
{
    /* variables */
      /* none */



    /* reset to the start of the current track */
    init_track();

    /* display the new time for the current track */
    display_time(get_track_time());


    /* return with the status unchanged */
    return  cur_status;

}




/*
   no_update

   Description:      This function handles updates when there is nothing to
                     do.  It just returns with the status unchanged.

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new status (same as current status).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    Mar. 5, 1994

*/

enum status  no_update(enum status cur_status)
{
    /* variables */
      /* none */



    /* nothing to do - return with the status unchanged */
    return  cur_status;

}
