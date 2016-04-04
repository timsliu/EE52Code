/****************************************************************************/
/*                                                                          */
/*                                  FFREV                                   */
/*                      Fast Forward/Reverse Functions                      */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS  52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the key processing and update functions for the Fast
   Forward and Reverse operations of the MP3 Jukebox Project.  These functions
   take care of processing an input key (from the keypad) and updates for Fast
   Forward and Reverse operations.  They are called by the main loop of the
   MP3 Jukebox.  The functions included are:
      begin_FastFwd   - switch to fast forward from reverse (key processing
                        function)
      begin_Reverse   - switch to reverse from fast forward (key processing
                        function)
      dec_FFRev_rate  - decrease the fast forward/reverse rate
      inc_FFRev_rate  - increase the fast forward/reverse rate
      start_FastFwd   - start going fast forward (key processing function)
      start_Reverse   - start going reverse (key processing function)
      stop_FFRev      - stop when doing fast forward or reverse (key
                        processing function)
      switch_FastFwd  - switch to fast forward from play (key processing
                        function)
      switch_Reverse  - switch to reverse from play (key processing function)
      update_FastFwd  - fast forwarding, update the time (update function)
      update_Reverse  - reversing, update the time (update function)

   The local functions included are:
      none

   The locally global variable definitions included are:
      FFRev_rate - rate at which to run fast forward/reverse
      time_FFRev - leftover (after rounding) time for fast forward/reverse


   Revision History
      6/4/00   Glen George       Initial revision (from 3/6/99 version of
                                 ffrev.c from the Digital Audio Recorder
                                 Project).
      6/2/02   Glen George       Changed update_FastFwd() and update_Reverse()
                                 to use the elapsed_time() function to do the
                                 fast forward and reverse operations, rather
                                 than the user update function.
      6/2/02   Glen George       Added time_FFRev global variable to support
                                 fast forward and reverse operations.
      6/2/02   Glen George       Rewrote start_FastFwd(), start_Reverse(),
                                 begin_FastFwd(), begin_Reverse(), and
                                 stop_FFRev() to implement the new method for
                                 doing fast forward and reverse operations.
      6/2/02   Glen George       Updated comments.
      6/5/03   Glen George       Updated start_FastFwd and start_Reverse to
                                 not do anything if the current track is a
                                 directory (supports FAT file systems).
      6/5/03   Glen George       Added #include of fatutil.h for function
                                 declarations needed by above change.
      6/5/03   Glen George       Updated function headers.
      6/5/08   Glen George       Added functions dec_FFRev_rate and
                                 inc_FFRev_rate along with the shared variable
				 FFRev_rate to support variable rate fast
				 forward and reverse.
*/



/* library include files */
  /* none */

/* local include files */
#include  "mp3defs.h"
#include  "keyproc.h"
#include  "updatfnc.h"
#include  "trakutil.h"
#include  "fatutil.h"



/* locally global variables */

static int  FFRev_rate;         /* rate at which to increment/decrement fast forward/reverse */

static int  time_FFRev;         /* leftover time (after rounding) for fast forward/reverse */




/*
   start_FastFwd

   Description:      This function handles the <Fast Forward> key when nothing
                     is happening in the system.
			     
   Operation:        It starts the fast forward operation if there is time
                     remaining on the current track to fast forward thru and
                     the track is not a directory and does nothing otherwise.

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new system status: STAT_FF if there
                     is something on the track to fast forward thru, the
                     passed current status otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: FFRev_rate - reset to MIN_FFREV_RATE.
                     time_FFRev - reset to 0.

   Author:           Glen George
   Last Modified:    June 5, 2008

*/

enum status  start_FastFwd(enum status cur_status)
{
    /* variables */
      /* none */



    /* check if something is left on the track and it isn't a directory */
    if (!cur_isDir() && (get_track_remaining_length() != 0))  {

        /* not a directory and something is left on the track - fast forward it */

        /* clear out the timer for the fast forward operation */
        (void) elapsed_time();
        /* also clear leftover time */
        time_FFRev = 0;

	/* start at slowest fast forward rate */
	FFRev_rate = MIN_FFREV_RATE;

        /* set status to fast forward */
        cur_status = STAT_FF;
    }


    /* return with the possibly new system status */
    return  cur_status;

}




/*
   start_Reverse

   Description:      This function handles the <Reverse> key when nothing is
                     happening in the system.

   Operation:        The function starts the reverse operation if there is
                     data to be reversed thru on the current track and the
                     track is not a directory and does nothing otherwise.

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new system status: STAT_REV if there
                     is something left on the track to reverse thru, the
                     passed current status otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: FFRev_rate - reset to MIN_FFREV_RATE.
                     time_FFRev - reset to 0.

   Author:           Glen George
   Last Modified:    June 5, 2008

*/

enum status  start_Reverse(enum status cur_status)
{
    /* variables */
      /* none */



    /* check if entry is not a directory and something is left on the track */
    if (!cur_isDir() && (get_track_remaining_length() != get_track_length()))  {

        /* something is on the track & not a directory, can do reverse */

        /* clear out the timer for the reverse operation */
        (void) elapsed_time();
        /* also clear leftover time */
        time_FFRev = 0;

	/* start at slowest reverse rate */
	FFRev_rate = MIN_FFREV_RATE;

        /* set status to reverse */
        cur_status = STAT_REV;
    }


    /* return the possibly new status */
    return  cur_status;

}




/*
   switch_FastFwd

   Description:      This function handles the <Fast Forward> key when playing
                     a track.  It turns off the audio output and then starts
                     the fast forward operation.

   Arguments:        cur_status (enum status) - the current system status (not
                                                used).
   Return Value:     (enum status) - the new system status is returned (by
                     start_FastFwd actually).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    Mar. 11, 1995

*/

enum status  switch_FastFwd(enum status cur_status)
{
    /* variables */
      /* none */



    /* first turn off the audio output */
    audio_halt();


    /* now start the fast forward operation (returning it's status) */
    /* note: currently doing nothing so in Idle state */
    return  start_FastFwd(STAT_IDLE);

}




/*
   switch_Reverse

   Description:      This function handles the <Reverse> key when playing a
                     track.  It turns off the audio output and then starts the
                     reverse operation.

   Arguments:        cur_status (enum status) - the current system status (not
                                                used).
   Return Value:     (enum status) - the new system status is returned (by
                     start_Reverse actually).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    Mar. 11, 1995

*/

enum status  switch_Reverse(enum status cur_status)
{
    /* variables */
      /* none */



    /* first turn off the audio output */
    audio_halt();


    /* now start up reverse, returning it's status */
    /* note: currently doing nothing so in Idle state */
    return  start_Reverse(STAT_IDLE);

}




/*
   begin_FastFwd

   Description:      This function handles the <Fast Forward> key when
                     currently going in reverse.

   Operation:        The function resets the time for timing the fast forward
                     operation and the fast forward rate and returns a new
                     status.

   Arguments:        cur_status (enum status) - the current system status (not
                                                used).
   Return Value:     (enum status) - the new status (STAT_FF) is returned.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: FFRev_rate - reset to MIN_FFREV_RATE.
                     time_FFRev - reset to 0.

   Author:           Glen George
   Last Modified:    June 5, 2008

*/

enum status  begin_FastFwd(enum status cur_status)
{
    /* variables */
      /* none */



    /* clear out the timer for the fast forward operation */
    (void) elapsed_time();
    /* also clear leftover time */
    time_FFRev = 0;

    /* start at slowest fast forward rate */
    FFRev_rate = MIN_FFREV_RATE;

    /* and return the new status */
    return  STAT_FF;

}




/*
   begin_Reverse

   Description:      This function handles the <Reverse> key when currently
                     operating in fast forward.

   Operation:        The function resets the timer used to time the reverse
                     operation and the reverse rate and then returns STAT_REV
                     as the status.

   Arguments:        cur_status (enum status) - the current system status (not
                                                used).
   Return Value:     (enum status) - the new status (STAT_REV) is returned.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: FFRev_rate - reset to MIN_FFREV_RATE.
                     time_FFRev - reset to 0.

   Author:           Glen George
   Last Modified:    June 5, 2008

*/

enum status  begin_Reverse(enum status cur_status)
{
    /* variables */
      /* none */



    /* clear out the timer for the reverse operation */
    (void) elapsed_time();
    /* also clear leftover time */
    time_FFRev = 0;

    /* start at slowest reverse rate */
    FFRev_rate = MIN_FFREV_RATE;

    /* and return STAT_REV as the new status */
    return  STAT_REV;

}




/*
   stop_FFRev

   Description:      This function handles the <Stop> key when fast forwarding
                     or reversing.  It just changes to the idle status.  Note
                     that the time is left unaffected.

   Arguments:        cur_status (enum status) - the current system status (not
                                                used).
   Return Value:     (enum status) - the new status (STAT_IDLE) is returned.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 1, 2002

*/

enum status  stop_FFRev(enum status cur_status)
{
    /* variables */
      /* none */



    /* just return the idle status */
    return  STAT_IDLE;

}




/*
   update_FastFwd

   Description:      This function handles updates when fast forwarding.  The
                     function gets the elapsed time, scales it appropriately,
                     and updates the track time and buffer pointer for the new
                     position.  When the end of the track is reached the
                     status is returned to idle (the time is left at 0).

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new status: passed current status if
                     not at the end of the track and STAT_IDLE if at the end.

   Input:            None.
   Output:           The new track time (if any) is output to the display.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: time_FFRev - updated.

   Author:           Glen George
   Last Modified:    June 1, 2002

*/

enum status  update_FastFwd(enum status cur_status)
{
    /* variables */
    long int  etime;            /* the elapsed time since the last call */

    long int  buffer_fwd;       /* amount to move forward on track */



    /* is there anything left in the track to fast forward through */
    if (get_track_remaining_length() != 0)  {


        /* something on track - get the elapsed time for fast forward operation */
        /* it needs to be scaled and have any leftover time added in */
        etime = FFRev_rate * elapsed_time() + time_FFRev;

        /* has enough time elapsed for fast forwarding */
        if (etime > MIN_FFREV_TIME)  {

            /* can and should move forward - compute how many bytes */
            buffer_fwd = (get_track_length() * etime) / (100L * get_track_total_time());

            /* truncate it to the nearest number of blocks */
            buffer_fwd = (buffer_fwd / IDE_BLOCK_SIZE);
            /* compute the leftover time and save it for next time */
            time_FFRev = etime - (100L * get_track_total_time() * buffer_fwd * IDE_BLOCK_SIZE) / get_track_length();
            /* make sure there isn't a minor math error */
            if (time_FFRev < 0)
                /* leftover amount shouldn't be negative */
                time_FFRev = 0;

            /* if there are buffers to move forward, do so */
            if (buffer_fwd > 0)  {
                update_track_position(buffer_fwd * IDE_BLOCK_SIZE);

                /* also display the new time */
                display_time(get_track_time());
            }
        }
        else  {

            /* not enough time yet for fast forwarding - save the accumulated time */
            time_FFRev = etime;
        }
    }
    else  {


        /* done with this track - switch to the idle state */
        cur_status = STAT_IDLE;
    }


    /* done with update, return the new status */
    return  cur_status;

}




/*
   update_Reverse

   Description:      This function handles updates when reversing.  The
                     function gets the elapsed time, scales it appropriately,
                     and updates the track time and buffer pointer for the new
                     position.  When the start of the track is reached the
                     status is returned to idle (the time is left at the
                     start).

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new status: the passed current status
                     if not at the start of the track and STAT_IDLE if rewound
                     to the start of the track.

   Input:            None.
   Output:           New track time (if any) is output to the display.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: time_FFRev - updated.

   Author:           Glen George
   Last Modified:    June 1, 2002

*/

enum status  update_Reverse(enum status cur_status)
{
    /* variables */
    long int  etime;            /* the elapsed time since the last call */

    long int  buffer_rev;       /* amount to move backward on the track */



    /* check if already at the start of the track */
    if (get_track_remaining_length() != get_track_length())  {


        /* something on track - get the elapsed time for reverse operation */
        /* it needs to be scaled and have any leftover time added in */
        etime = FFRev_rate * elapsed_time() + time_FFRev;

        /* has enough time elapsed for reversing */
        if (etime > MIN_FFREV_TIME)  {

            /* can and should move backward - compute how many bytes */
            buffer_rev = (get_track_length() * etime) / (100L * get_track_total_time());

            /* truncate it to the nearest number of blocks */
            buffer_rev = (buffer_rev / IDE_BLOCK_SIZE);
            /* compute the leftover time and save it for next time */
            time_FFRev = etime - (100L * get_track_total_time() * buffer_rev * IDE_BLOCK_SIZE) / get_track_length();
            /* make sure there isn't a minor math error */
            if (time_FFRev < 0)
                /* leftover amount shouldn't be negative */
                time_FFRev = 0;

            /* if there are buffers to move back, do so */
            if (buffer_rev > 0)  {
                update_track_position(-buffer_rev * IDE_BLOCK_SIZE);

                /* also display the new time */
                display_time(get_track_time());
            }
        }
        else  {

            /* not enough time yet for reversing - save the accumulated time */
            time_FFRev = etime;
        }
    }
    else  {


        /* hit the start of the track - need to reload the pointers */
        init_track();

        /* display the new time */
        display_time(get_track_time());

        /* and switch back to idle state */
        cur_status = STAT_IDLE;
    }


    /* all done, return the possibly new status */
    return  cur_status;

}




/*
   dec_FFRev_rate

   Description:      This function decrements the fast forward/reverse rate by
                     DELTA_FFREV_RATE.  It can only be decremented down to
                     MIN_FFREV_RATE, it cannot be decremented below that.

   Operation:        DELTA_FFREV_RATE is subtracted from the shared variable
                     FFRev_rate and if it is less than MIN_FFREV_RATE it is
		     set to that value.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: FFRev_rate - decremented by DELTA_FFREV_RATE.

   Author:           Glen George
   Last Modified:    June 5, 2008

*/

void  dec_FFRev_rate(void)
{
    /* variables */
      /* none */



    /* decrement the fast forward/reverse rate */
    FFRev_rate -= DELTA_FFREV_RATE;

    /* make sure the rate isn't too low */
    if (FFRev_rate < MIN_FFREV_RATE)
        /* limit FFRev_rate to [MIN_FFREV_RATE, MAX_FFREV_RATE] */
	FFRev_rate = MIN_FFREV_RATE;


    /* done computing the new fast forward/reverse rate */
    return;

}




/*
   inc_FFRev_rate

   Description:      This function increments the fast forward/reverse rate by
                     DELTA_FFREV_RATE.  It can only be incremented to
                     MAX_FFREV_RATE, it cannot be incremented above that.

   Operation:        DELTA_FFREV_RATE is added to the shared variable
                     FFRev_rate and if it is greater than MAX_FFREV_RATE it
		     is set to that value.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: FFRev_rate - incremented by DELTA_FFREV_RATE.

   Author:           Glen George
   Last Modified:    June 5, 2008

*/

void  inc_FFRev_rate(void)
{
    /* variables */
      /* none */



    /* increment the fast forward/reverse rate */
    FFRev_rate += DELTA_FFREV_RATE;

    /* make sure the rate isn't too high */
    if (FFRev_rate > MAX_FFREV_RATE)
        /* limit FFRev_rate to [MIN_FFREV_RATE, MAX_FFREV_RATE] */
	FFRev_rate = MAX_FFREV_RATE;


    /* done computing the new fast forward/reverse rate */
    return;

}
