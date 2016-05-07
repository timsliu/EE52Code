/****************************************************************************/
/*                                                                          */
/*                                 PLAYMP3                                  */
/*                              Play Functions                              */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS  52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the key processing and update functions for the Play
   operations of the MP3 Jukebox Project.  These functions take care of
   processing an input key (from the keypad) and updates for the Play
   operation.  They are called by the main loop of the MP3 Jukebox.  The
   functions included are:
      begin_Play         - start playing from fast forward or reverse (key
                           processing function)
      begin_RptPlay      - start repeatedly playing from fast forward or
                           reverse (key processing function)
      cont_RptPlay       - switch to repeat play from standard play (key
                           processing function)
      start_Play         - begin playing the current track (key processing
                           function)
      start_RptPlay      - begin repeatedly playing the current track (key
                           processing function)
      stop_Play          - stop when playing (key processing function)
      update_Play        - update function for play and repeat play (update
                           function)

   The local functions included are:
      init_Play          - actually start playing a track

   The locally global variable definitions included are:
      buffers        - buffers for playing
      empty_buffer   - buffer used for audio I/O when have no data available
      current_buffer - which buffer is currently being played
      play_time      - current time of play operation
      rpt_play       - flag indicating doing repeat play instead of play


   Revision History
      6/5/00   Glen George       Initial revision (from 3/6/99 version of
                                 playrec.c for the Digital Audio Recorder
                                 Project).
      6/7/00   Glen George       Fixed calls to get_track_position() in
                                 init_Play and update_Play to call
                                 get_track_block_position() instead and
                                 fixed the size of some variables.
      6/7/00   Glen George       Call elapsed_time() in init_Play() to reset
                                 the play timing.
      6/14/00  Glen George       "Fix" arithmetic in init_Play() so it
                                 actually does long int calculations.
      6/14/00  Glen George       Fix buffer wrapping error in update_Play().
      6/14/00  Glen George       Updated type casts on buffers[].p in
                                 init_Play() and changed empty_buffer type to
                                 match that of buffers[].p.
      6/2/02   Glen George       Removed calls to ffrev_halt() in begin_Play()
                                 and begin_RptPlay() since they are no longer
                                 needed and the function no longer exists.
      6/2/02   Glen George       Output the absolute value of the time in
                                 update_Play() so tracks with unknown length
                                 will automatically count up in time.  Also
                                 requires that stdlib.h be included.
      6/2/02   Glen George       Use MAKE_FARPTR macro instead of inline code
                                 to create far pointers.
      6/2/02   Glen George       Use size_t instead of int for array indices.
      6/2/02   Glen George       Updated comments.
      6/10/02  Glen George       Added use of SECTOR_ADJUST constant for
                                 dealing with hard drives with different
                                 geometries.
      6/5/03   Glen George       Updated start_Play and start_RptPlay to
                                 handle FAT directory structures by checking
                                 if the current track is a directory or a
                                 song (file).
      6/5/03   Glen George       Added #include of fatutil.h for function
                                 declarations needed by above change.
      6/5/03   Glen George       Updated function headers.
      6/5/03   Glen George       Added support for the PC simulation version
                                 in init_Play, stop_Play, and update_Play
                                 (allocate/deallocate memory instead of
                                 assuming DRAM).
      6/9/05   Glen George       Fixed a minor bug in update_Play that caused
                                 repeat play to "stutter" at the start of the
                                 track when repeating.
      4/29/06  Glen George       Removed inclusion of stdlib.h - it's handled
                                 by mp3defs.h now.
      4/29/06  Glen George       Changed buffers to be short ints instead of
                                 chars.
      6/4/06   Glen George       Fixed allocated buffer size to match the fact
                                 that buffers are short ints, not chars.
      6/6/06   Glen George       Made allocation buffer sizes more portable
                                 and fixed more byte/word problems.
      6/6/06   Glen George       Unrolled part of a loop to fix a possible
                                 compiler issue with gcc.
      6/6/06   Glen George       Added a temporary variable to fix a possible
                                 compiler issue with gcc.
      6/6/06   Glen George       Updated comments.
      6/19/08  Glen George       Now using get_partition_start() to get the
                                 start of the partition for accessing the hard
                                 drive.
      3/15/13  Glen George       Changed to using get_file_blocks() instead of
                                 get_blocks() to support fragmented files.
*/



/* library include files */
#ifdef  PCVERSION
    #include  <alloc.h>
#endif

/* local include files */
#include  "mp3defs.h"
#include  "keyproc.h"
#include  "updatfnc.h"
#include  "trakutil.h"
#include  "fatutil.h"




/* local definitions */
  /* none */




/* local function declarations */
enum status  init_Play(enum status);            /* initialize playing */




/* locally global variables */
static struct audio_buf          buffers[NO_BUFFERS];/* buffers to play */
static unsigned short int  far  *empty_buffer;       /* empty (no data) buffer */
static int                       current_buffer;     /* buffer currently playing */

static long int                  play_time;          /* time for play operation */
static int                       rpt_play;           /* doing repeat play */




/*
   start_Play

   Description:      This function handles the <Play> key when nothing is
                     happening in the system.  If the current entry is a
                     directory, that directory is entered.  If it is a song,
                     it starts playing the track at the current position.  If
                     there is no time remaining on the track (it is at the
                     end) of if the current entry is a directory, the function
                     returns with the passed status as the current state,
                     otherwise it returns the play state as the current state.

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new system status: STAT_PLAY if there
                     is something to play on the track, the passed status
                     otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: rpt_play - set to FALSE.

   Author:           Glen George
   Last Modified:    March 15, 2013

*/

enum status  start_Play(enum status cur_status)
{
    /* variables */
      /* none */



    /* check if we are on a directory or a song */
    if (cur_isDir())  {

        /* is a directory - move into it */
        if (!get_first_dir_entry())
            /* successfully got the new entry, load its data */
            setup_cur_track_info();
        else
            /* there was an error - load error track information */
            setup_error_track_info();

        /* display the track information for this track */
        display_time(get_track_time());
        display_title(get_track_title());
        display_artist(get_track_artist());
    }
    else  {

        /* it's a song so set global flag to normal play (not repeat play) */
        rpt_play = FALSE;

        /* and start playing and update the status */
        cur_status = init_Play(cur_status);
    }


    /* return the new status */
    return  cur_status;

}




/*
   start_RptPlay

   Description:      This function handles the <Repeat Play> key when nothing
                     is happening in the system.  If the current track is a
                     song it starts playing the track at the current position.
                     If there is no time remaining on the track (for example,
                     it was fast forwarded) the track is started from the
                     beginning.  If the track is empty or a directory, nothing
                     is played and the function returns the passed status.

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new status: STAT_PLAY if there is
                     something on the track, the passed status otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: rpt_play - set to TRUE.

   Author:           Glen George
   Last Modified:    June 5, 2003

*/

enum status  start_RptPlay(enum status cur_status)
{
    /* variables */
      /* none */



    /* check if this is a directory or a song */
    if (!cur_isDir())  {

        /* not a directory, must be a song, so play it */

        /* set global flags to repeat play */
        rpt_play = TRUE;

        /* now start playing and get the status */
        cur_status = init_Play(cur_status);
    }


    /* return with the possibly new status */
    return  cur_status;

}




/*
   cont_RptPlay

   Description:      This function handles the <Repeat Play> key when already
                     playing a track.  It just changes the locally global
                     variable rpt_play (to TRUE indicating doing repeat play).
                     The update function takes care of restarting the track at
                     the end of the track.

   Arguments:        cur_status (enum status) - the current system status (not
                                                used).
   Return Value:     (enum status) - the new system status (STAT_PLAY).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: rpt_play - set to TRUE (doing repeat play).

   Author:           Glen George
   Last Modified:    Mar. 15, 1995

*/

enum status  cont_RptPlay(enum status cur_status)
{
    /* variables */
      /* none */



    /* now doing repeat play */
    rpt_play = TRUE;


    /* done setting up for repeat play - return the status (STAT_PLAY) */
    return  STAT_PLAY;

}




/*
   begin_Play

   Description:      This function handles the <Play> key when fast forwarding
                     or reversing.  It turns off the fast forward or reverse
                     operation and then starts playing the track at the
                     current position.

   Arguments:        cur_status (enum status) - the current system status (not
                                                used).
   Return Value:     (enum status) - the new system status (actually returned
                     by start_Play, either STAT_IDLE if at the end of the
                     track, or STAT_PLAY otherwise).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 2, 2002

*/

enum status  begin_Play(enum status cur_status)
{
    /* variables */
      /* none */



    /* start playing, returning the appropriate status */
    /* note: fast forward or reverse is turned off by change of state */
    /* note: want to return to idle state if at the end of the track */
    return  start_Play(STAT_IDLE);

}




/*
   begin_RptPlay

   Description:      This function handles the <Repeat Play> key when fast
                     forwarding or reversing.  It turns off the fast forward
                     or reverse operation and then starts playing the track at
                     the current position (time).

   Arguments:        cur_status (enum status) - the current system status (not
                                                used).
   Return Value:     (enum status) - the new status (actually returned by
                     start_RptPlay, either STAT_IDLE if the track is empty, or
                     STAT_PLAY otherwise).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 2, 2002

*/

enum status  begin_RptPlay(enum status cur_status)
{
    /* variables */
      /* none */



    /* now start repeat playing, returning the appropriate status */
    /* note: fast forward or reverse is turned off by change of state */
    /* note: want to return to idle state if nothing on the track */
    return  start_RptPlay(STAT_IDLE);

}




/*
   stop_Play

   Description:      This function handles the <Stop> key when playing.  It
                     halts the audio system, resets the track to the start of
                     the track, and changes the current status to idle.

   Arguments:        cur_status (enum status) - the current system status (not
                                                used).
   Return Value:     (enum status) - the new status (STAT_IDLE).

   Input:            None.
   Output:           The new track time (the track length) is output.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 5, 2003

*/

enum status  stop_Play(enum status cur_status)
{
    /* variables */
#ifdef  PCVERSION
    int  i;             /* loop index */
#else
    /* none */
#endif



    /* first halt the audio output */
    audio_halt();

    /* if the PC version need to free memory */
#ifdef PCVERSION
    for (i = 0; i < NO_BUFFERS; i++)  {
        /* free the buffer */
        farfree(buffers[i].p);
    }
    farfree(empty_buffer);
#endif

    /* reset to the start of the current track */
    init_track();

    /* display the new track time */
    display_time(get_track_time());


    /* return with the new status */
    return  STAT_IDLE;

}




/*
   init_Play

   Description:      This function handles starting a track playing for the
                     <Play> and <Repeat Play> keys.  It starts playing the
                     track at the current position.  If there is no time
                     remaining on the track (for example, it is at the end)
                     the function returns with the current status, otherwise
                     it returns with the status set to STAT_PLAY.

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new system status: STAT_PLAY if there
                     is something to play on the track, the passed status
                     otherwise.

   Input:            None.
   Output:           The new time for the track is output to the display.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: buffers        - initialized with data.
                     empty_buffer   - filled with NO_MP3_DATA signal.
                     current_buffer - set to first buffer (0).
                     play_time      - set to the current track time.
                     rpt_play       - used to determine normal or repeat play.

   Author:           Glen George
   Last Modified:    June 6, 2006

*/

static  enum status  init_Play(enum status cur_status)
{
    /* variables */
    int           blocks_to_read;       /* number of blocks to read */
    int           blocks_read;          /* blocks actually read from disk */
    int           tot_blocks_read = 0;  /* total number of blocks read */

    long int      bytes_left;           /* bytes left in the track */

    int           have_buffer = FALSE;  /* have a buffer with data */
    int           end_track = FALSE;    /* at the end of the track */

    unsigned int  tmp;                  /* temporary variable for intermediate results */
                                        /*    (seems to fix a compiler bug) */

    int           i;                    /* loop index */



    /* first initialize the buffer pointers and buffer structure */
    for (i = 0; i < NO_BUFFERS; i++)  {
        /* nothing in the buffer, it isn't the end, and point to DRAM */
        buffers[i].size = 0;
        buffers[i].done = FALSE;
    }

    /* allocate the buffer - remember it is words (short ints) */
    /* unroll the loop to workaround a possible compiler problem */
#ifdef  PCVERSION
    /* in PC version, allocate the buffer */
    buffers[0].p    = (unsigned short int far *) farmalloc(BUFFER_SIZE * sizeof(short int));
    buffers[1].p    = (unsigned short int far *) farmalloc(BUFFER_SIZE * sizeof(short int));
    buffers[2].p    = (unsigned short int far *) farmalloc(BUFFER_SIZE * sizeof(short int));
#else
    buffers[0].p    = (unsigned short int far *) MAKE_FARPTR(DRAM_STARTSEG, 0UL);
    buffers[1].p    = (unsigned short int far *) MAKE_FARPTR(DRAM_STARTSEG, 1UL * BUFFER_SIZE * sizeof(short int));
    buffers[2].p    = (unsigned short int far *) MAKE_FARPTR(DRAM_STARTSEG, 2UL * BUFFER_SIZE * sizeof(short int));
#endif

    /* need to setup empty buffer too */
    /* first the pointer */
#ifdef  PCVERSION
    /* in the PC version have to allocate the buffer */
    empty_buffer = (unsigned short int far *) farmalloc(BUFFER_SIZE);
#else
    empty_buffer = (unsigned short int far *) MAKE_FARPTR(DRAM_STARTSEG, (unsigned long int) NO_BUFFERS * BUFFER_SIZE);
#endif
    /* now fill it */
    for (i = 0; i < BUFFER_SIZE; i++)
        empty_buffer[i] = NO_MP3_DATA;


    /* now setup the playing time */
    play_time = get_track_time() * TIME_SCALE;


    /* now get the two buffers for the track from the disk */
    for (i = 0 ; (i < 2); i++)  {

        /* first check if at end of track */
        if (get_track_remaining_length() == 0)  {
            /* at end of track - check if repeat playing */
            if (rpt_play)  {
                /* at end and repeat playing - restart at beginning */
                init_track();
                /* need to reset total number of blocks read for track too */
                tot_blocks_read = 0;
            }
            else  {
                /* at end, but not repeating, so set flag */
                end_track = TRUE;
            }
        }

        /* if not at end, read the blocks for this buffer */
        if (!end_track)  {

            /* compute the number of blocks to read (block size is in words) */
            bytes_left = get_track_remaining_length() - (2 * IDE_BLOCK_SIZE * tot_blocks_read);
            blocks_to_read = (bytes_left + (2 * IDE_BLOCK_SIZE - 1)) / (2 * IDE_BLOCK_SIZE);
            /* but only read up to BUFFER_BLOCKS blocks */
            if (blocks_to_read > BUFFER_BLOCKS)
                blocks_to_read = BUFFER_BLOCKS;

            /* now read the blocks */
            blocks_read = get_file_blocks(get_track_block_position() + tot_blocks_read, blocks_to_read, buffers[i].p);

            /* check if read anything */
            if (blocks_read > 0)  {
                /* did read something, store how much (in words, not bytes) */
                if (bytes_left >= (2 * IDE_BLOCK_SIZE * blocks_read))
                    /* all of the blocks are data */
                    tmp = blocks_read * IDE_BLOCK_SIZE;
                else
                    /* remember the buffer size is in words */
                    tmp = (bytes_left + 1) / 2;
                /* now store the buffer size */
                buffers[i].size = tmp;
                /* also set the flag that we read data */
                have_buffer = TRUE;
            }
            else  {
                /* couldn't read anything, it is the end of the track */
                end_track = TRUE;
            }

            /* update number of blocks read so far */
            tot_blocks_read += blocks_read;
        }

        /* if at the end of the track need to play the empty buffer */
        if (end_track)  {
            buffers[i].size = BUFFER_SIZE;
            buffers[i].done = TRUE;
            buffers[i].p = empty_buffer;
        }
    }


    /* got a buffer, start the audio output if there is anything to output */
    if (have_buffer)  {
        /* have audio data - play it */
        audio_play(buffers[0].p, buffers[0].size);
        /* on the first buffer */
        current_buffer = 0;
        /* also update the time display */
        display_time(play_time / TIME_SCALE);
        /* and reset the elapsed time */
        elapsed_time();
    }


    /* finally, return with the proper status */
    if (have_buffer)
        /* have something to play - return with play status */
        return  STAT_PLAY;
    else
        /* empty track - return with status unchanged */
        return  cur_status;

}




/*
   update_Play

   Description:      This function handles updates when playing or repeat
                     playing.  It first checks if it is time for an update (by
                     calling the function update) and if so it gets the next
                     buffer to output and updates the time as is appropriate.
                     When it reaches the end of the track (when not in repeat
                     play mode) it uses the empty_buffer, which was previously
                     filled with NO_MP3_DATA signal, to fill out the track and
                     make sure all of the "good" signal has made it all the
                     way through the pipeline.

   Arguments:        cur_status (enum status) - the current system status.
   Return Value:     (enum status) - the new system status: STAT_IDLE if have
                     finished with the track, the passed status otherwise.

   Input:            None.
   Output:           The new time for the track is output to the display.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: buffers        - used for track data and filled.
                     empty_buffer   - output at the end of the track.
                     current_buffer - set to the buffer now being played.
                     play_time      - updated to the time the track has left
                                      to play.
                     rpt_play       - accessed to determine normal or repeat
                                      play mode.

   Author:           Glen George
   Last Modified:    March 15, 2013

*/

enum status  update_Play(enum status cur_status)
{
    /* variables */
    long int  old_play_time = play_time;    /* previous time value */

    int       next_buffer;                  /* next buffer to play */
    int       previous_buffer;              /* buffer that just finished */
    int       fill_buffer;                  /* next buffer to fill */

    long int  start_pos;                    /* starting position for read */
    int       blocks_to_read;               /* number of blocks to read */
    int       blocks_read;                  /* blocks actually read from disk */

    long int  bytes_left;                   /* bytes left in the track */
    long int  words_read;                   /* words read and waiting to play */

    int       end_play = FALSE;             /* done playing (out of data) */

#ifdef  PCVERSION
    int       i;                            /* loop index (needed in PC version only) */
#endif



    /* figure out the next buffer */
    next_buffer = current_buffer + 1;
    /* check if wrapping around the end of the buffers */
    if (next_buffer >= NO_BUFFERS)
        next_buffer -= NO_BUFFERS;


    /* check if it is time to do an update */
    if (update(buffers[next_buffer].p, buffers[next_buffer].size))  {

        /* system was ready for the buffer - need to do an update */

        /* update the track position */
        /* get the buffer that just finished */
        previous_buffer = current_buffer - 1;
        /* take care of wrapping around start of array */
        if (previous_buffer < 0)
            previous_buffer += NO_BUFFERS;
        /* now update the position if not starting over on a repeat play */
        /* this is necessary because previous_buffer and/or current_buffer */
        /* could be from the last iteration of the repeat play */
        if (!(buffers[previous_buffer].done || buffers[current_buffer].done))
            /* not finishing an iteration - update the position */
            /* remember that buffer size is in words and track position is */
            /* maintained in bytes */
            update_track_position(2 * buffers[previous_buffer].size);

        /* check if at the end of the track (if now outputting done buffer */
        /* this guarantees last buffer with data has been output) */
        if (buffers[current_buffer].done && !rpt_play)  {

            /* done with this track - turn off audio output */
            audio_halt();

            /* if the PC version need to free memory */
#ifdef  PCVERSION
            for (i = 0; i < NO_BUFFERS; i++)  {
                /* free the buffer */
                farfree(buffers[i].p);
            }
            farfree(empty_buffer);
#endif

            /* reset to start of track */
            init_track();

            /* set status back to idle */
            cur_status = STAT_IDLE;
        }
        else  {

            /* not done playing */

            /* get the next buffer to fill */
            fill_buffer = current_buffer + 2;
            /* watch out for wrapping */
            if (fill_buffer >= NO_BUFFERS)
                fill_buffer -= NO_BUFFERS;

            /* attempt to get another buffer */

            /* first figure out where the buffer is and how big it is */
            /* the next buffer has to be part of the newest iteration */
            words_read = buffers[next_buffer].size;
            /* only include the current buffer if it wasn't the end of the previous iteration */
            if (!(buffers[current_buffer].done))
                words_read += buffers[current_buffer].size;
            /* now compute the number of bytes left and starting position */
            bytes_left = get_track_remaining_length() - (2 * words_read);
            start_pos = get_track_block_position() + (words_read / IDE_BLOCK_SIZE);

            /* check if out of data */
            if (bytes_left <= 0)  {
                /* nothing left to play, check if repeating */
                if (rpt_play)  {
                    /* repeating, so can reinitialize the track */
                    init_track();
                    /* and recompute the number of bytes left */
                    bytes_left = get_track_remaining_length();
                    /* and the starting position */
                    start_pos = get_track_block_position();
                    /* the block just sent was last one of the iteration */
                    buffers[next_buffer].done = TRUE;
                }
                else  {
                    /* not repeating, we're done */
                    end_play = TRUE;
                }
            }

            /* if still playing, can get the data */
            if (!end_play)  {

                /* compute the number of blocks to read */
                blocks_to_read = (bytes_left + (2 * IDE_BLOCK_SIZE - 1)) / (2 * IDE_BLOCK_SIZE);
                /* but only read up to BUFFER_BLOCKS blocks */
                if (blocks_to_read > BUFFER_BLOCKS)
                    blocks_to_read = BUFFER_BLOCKS;

                /* now read the blocks */
                blocks_read = get_file_blocks(start_pos, blocks_to_read, buffers[fill_buffer].p);

                /* check if read anything */
                if (blocks_read > 0)  {
                    /* did read something, store how much (words, not bytes) */
                    if (bytes_left >= (2 * IDE_BLOCK_SIZE * blocks_read))
                        /* all of the blocks are data */
                        buffers[fill_buffer].size = blocks_read * IDE_BLOCK_SIZE;
                    else
                        /* only play the real data */
                        /* remember that buffer sizes are in words, not bytes */
                        buffers[fill_buffer].size = (bytes_left + 1) / 2;
                    /* this block is not the last one */
                    buffers[fill_buffer].done = FALSE;
                }
                else  {
                    /* couldn't read anything, it is the end of the track */
                    end_play = TRUE;
                }
            }

            /* if at the end of play, need to play the empty buffer */
            if (end_play)  {
                buffers[fill_buffer].p = empty_buffer;
                buffers[fill_buffer].size = BUFFER_SIZE;
                buffers[fill_buffer].done = TRUE;
            }


            /* finally, update the current buffer */
            current_buffer = next_buffer;
        }
    }


    /* always update the displayed time */

    /* get the elapsed time */
    play_time -= elapsed_time();
    /* see if we need to update the display */
    if ((play_time / TIME_SCALE) != (old_play_time / TIME_SCALE))
        /* the time has changed - update the display */
        display_time(abs(play_time / TIME_SCALE));


    /* done with update, return possibly new status */
    return  cur_status;

}
