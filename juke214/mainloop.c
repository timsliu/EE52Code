/****************************************************************************/
/*                                                                          */
/*                                 MAINLOOP                                 */
/*                             Main Program Loop                            */
/*                            MP3 Jukebox Project                           */
/*                                 EE/CS 52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the main processing loop (background) for the MP3
   Jukebox Project.  The only global function included is:
      main - background processing loop

   The local functions included are:
      key_lookup - get a key and look up its keycode

   The locally global variable definitions included are:
      none


   Revision History
      6/5/00   Glen George       Initial revision (from 3/6/99 version of
                                 mainloop.c for the Digital Audio Recorder
                                 Project).
      6/2/02   Glen George       Updated comments.
      5/15/03  Glen George       Moved static declarations to first keyword
                                 since the lame NIOS compiler requires it.
      5/15/03  Glen George       Changed type on some variables to size_t
                                 for better portability and more accurate
				 typing.
      5/15/03  Glen George       Added #include of stddef.h to get some
                                 standard definitions.
      6/5/03   Glen George       Removed references to track number, it is no
	                         longer used.
      6/5/03   Glen George       Added initialization to FAT directory system.
      6/5/03   Glen George       Updated function headers.
*/



/* library include files */
#include  <stddef.h>

/* local include files */
#include  "interfac.h"
#include  "mp3defs.h"
#include  "keyproc.h"
#include  "updatfnc.h"
#include  "trakutil.h"
#include  "fatutil.h"




/* local function declarations */
enum keycode  key_lookup(void);      /* translate key values into keycodes */




/*
   main

   Description:      This procedure is the main program loop for the MP3
                     Jukebox.  It loops getting keys from the keypad,
                     processing those keys as is appropriate.  It also handles
                     updating the display and setting up the buffers for MP3
                     playback.

   Arguments:        None.
   Return Value:     (int) - return code, always 0 (never returns).

   Input:            Keys from the keypad.
   Output:           Status information to the display.

   Error Handling:   Invalid input is ignored.

   Algorithms:       The function is table-driven.  The processing routines
                     for each input are given in tables which are selected
                     based on the context (state) in which the program is
                     operating.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 5, 2003

*/

int  main()
{
    /* variables */
    enum keycode  key;                      /* an input key */

    enum status   cur_status = STAT_IDLE;   /* current program status */
    enum status   prev_status = STAT_IDLE;  /* previous program status */

    long int      root_dir_start;	    /* start of the root directory */

    /* array of status type translations (from enum status to #defines) */
    /* note: the array must match the enum definition order exactly */
    static const unsigned int  xlat_stat[] =
        {  STATUS_IDLE,      /* system idle */
           STATUS_PLAY,      /* playing (or repeat playing) a track */
           STATUS_FASTFWD,   /* fast forwarding a track */
           STATUS_REVERSE    /* reversing a track */
        };

    /* update functions (one for each system status type) */
    static enum status  (* const update_fnc[NUM_STATUS])(enum status) =
        /*                        Current System Status                           */
        /*    idle        play      fast forward      reverse      */
        {  no_update, update_Play, update_FastFwd, update_Reverse  };

    /* key processing functions (one for each system status type and key) */
    static enum status  (* const process_key[NUM_KEYCODES][NUM_STATUS])(enum status) =
        /*                            Current System Status                                                */
        /* idle           play            fast forward   reverse                  key         */
      { {  do_TrackUp,    no_action,      no_action,     no_action     },   /* <Track Up>     */
        {  do_TrackDown,  no_action,      no_action,     no_action     },   /* <Track Down>   */
        {  start_Play,    no_action,      begin_Play,    begin_Play    },   /* <Play>         */
        {  start_RptPlay, cont_RptPlay,   begin_RptPlay, begin_RptPlay },   /* <Repeat Play>  */
        {  start_FastFwd, switch_FastFwd, stop_FFRev,    begin_FastFwd },   /* <Fast Forward> */
        {  start_Reverse, switch_Reverse, begin_Reverse, stop_FFRev    },   /* <Reverse>      */
        {  stop_idle,     stop_Play,      stop_FFRev,    stop_FFRev    },   /* <Stop>         */
        {  no_action,     no_action,      no_action,     no_action     } }; /* illegal key    */



    /* first initialize everything */
    /* initalize FAT directory functions */
    root_dir_start = init_FAT_system();

    /* get the first directory entry (file/song) */
    if (root_dir_start != 0)  {
	/* have a valid starting sector - get the first directory entry */
	get_first_dir_entry(root_dir_start);
	/* and setup the information for the track/file */
	setup_cur_track_info();
    }
    else  {
        /* had an error - fill with error track information */
	setup_error_track_info();
    }


    /* display track information */
    display_time(get_track_time());
    display_title(get_track_title());
    display_artist(get_track_artist());

    display_status(xlat_stat[cur_status]);  /* display status */


    /* infinite loop processing input */
    while(TRUE)  {

        /* handle updates */
        cur_status = update_fnc[cur_status](cur_status);


        /* now check for keypad input */
        if (key_available())  {

            /* have keypad input - get the key */
            key = key_lookup();

            /* execute processing routine for that key */
            cur_status = process_key[key][cur_status](cur_status);
        }


        /* finally, if the status has changed - display the new status */
        if (cur_status != prev_status)  {

            /* status has changed - update the status display */
            display_status(xlat_stat[cur_status]);
        }

        /* always remember the current status for next loop iteration */
        prev_status = cur_status;
    }


    /* done with main (never should get here), return 0 */
    return  0;

}




/*
   key_lookup

   Description:      This function gets a key from the keypad and translates
                     the raw keycode to an enumerated keycode for the main
                     loop.

   Arguments:        None.
   Return Value:     (enum keycode) - type of the key input on keypad.

   Input:            Keys from the keypad.
   Output:           None.

   Error Handling:   Invalid keys are returned as KEYCODE_ILLEGAL.

   Algorithms:       The function uses an array to lookup the key types.
   Data Structures:  Array of key types versus key codes.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    May 15, 2003

*/

static  enum keycode  key_lookup()
{
    /* variables */

    static const enum keycode  keycodes[] = /* array of keycodes */
        {                                   /* order must match keys array exactly */
           KEYCODE_TRACKUP,    /* <Track Up>     */    /* also needs to have extra */
           KEYCODE_TRACKDOWN,  /* <Track Down>   */    /* entry for illegal codes */
           KEYCODE_PLAY,       /* <Play>         */
           KEYCODE_RPTPLAY,    /* <Repeat Play>  */
           KEYCODE_FASTFWD,    /* <Fast Forward> */
           KEYCODE_REVERSE,    /* <Reverse>      */
           KEYCODE_STOP,       /* <Stop>         */
           KEYCODE_ILLEGAL     /* other keys     */
        }; 

    static const int  keys[] =   /* array of key values */
        {                        /* order must match keycodes array exactly */
           KEY_TRACKUP,    /* <Track Up>     */
           KEY_TRACKDOWN,  /* <Track Down>   */
           KEY_PLAY,       /* <Play>         */
           KEY_RPTPLAY,    /* <Repeat Play>  */
           KEY_FASTFWD,    /* <Fast Forward> */
           KEY_REVERSE,    /* <Reverse>      */
           KEY_STOP        /* <Stop>         */
        }; 

    int     key;           /* an input key */

    size_t  i;             /* general loop index */



    /* get a key */
    key = getkey();


    /* lookup key in keys array */
    for (i = 0; ((i < (sizeof(keys)/sizeof(int))) && (key != keys[i])); i++);


    /* return the appropriate key type */
    return  keycodes[i];

}
