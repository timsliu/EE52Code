/****************************************************************************/
/*                                                                          */
/*                               TRAKUTIL.H                                 */
/*                         Track Utility Functions                          */
/*                              Include File                                */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS 52                                  */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants and function prototypes for the track
   utility functions defined in trakutil.c.


   Revision History
      6/6/00   Glen George       Initial revision (from the 3/6/99 version of
                                 updatfnc.h for the Digital Audio Recorder
                                 Project).
      6/7/00   Glen George       Added function prototype for
                                 get_track_block_position().
      6/2/02   Glen George       Added function prototype for
                                 get_track_total_time().
      5/15/03  Glen George       Added constants needed for indexing into a
                                 track information block read from the hard
                                 drive.
      6/5/03   Glen George       Removed function declarations supporting
                                 track numbers (update_track_no() and
                                 get_track_no()).
      6/5/03   Glen George       Added function declarations for new functions
                                 setup_cur_track_info() and
                                 setup_error_track_info() and removed
                                 constants associated with old index file
                                 scheme for getting song information.
*/




#ifndef  I__TRAKUTIL_H__
    #define  I__TRAKUTIL_H__


/* library include files */
  /* none */

/* local include files */
#include  "mp3defs.h"




/* constants */

/* marker to separate song name from artist in filename */
#define  END_TITLE_CHAR  0x7F




/* structures, unions, and typedefs */
    /* none */




/* function declarations */

/* initialization functions */
void  init_track(void);         /* initialize to the start of the track */

/* track running functions */
void  update_track_position(long int);  /* update the current position of the track */

/* track accessor functions */
long int     get_track_position(void);          /* get the current position of the track (relative to start in bytes) */
long int     get_track_block_position(void);    /* get the current position of the track (in blocks on hard drive) */
long int     get_track_length(void);            /* get the length of the track */
long int     get_track_remaining_length(void);  /* get the remaining length of the track */
const char  *get_track_title(void);             /* get the title of the track */
const char  *get_track_artist(void);            /* get the artist for the track */
int          get_track_time(void);              /* get the current time for the track */
int          get_track_total_time(void);        /* get the total time for the track */

/* setup functions */
void   setup_cur_track_info(void);              /* setup information for current track/file */
void   setup_error_track_info(void);            /* setup information to report an error */


#endif
