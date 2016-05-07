/****************************************************************************/
/*                                                                          */
/*                                KEYPROC.H                                 */
/*                         Key Processing Functions                         */
/*                               Include File                               */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS  52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants and function prototypes for the key
   processing functions defined in ffrev.c, keyupdat.c, and playmp3.c.


   Revision History:
      6/4/00   Glen George       Initial revision (from the 3/6/99 version of
                                 keyproc.h for the Digital Audio Recorder
                                 Project).
      6/5/08   Glen George       Added declarations for dec_FFRev_rate() and
                                 inc_FFRev_rate() functions.
*/



#ifndef  I__KEYPROC_H__
    #define  I__KEYPROC_H__


/* library include files */
  /* none */

/* local include files */
#include  "mp3defs.h"




/* constants */
    /* none */




/* structures, unions, and typedefs */
    /* none */




/* function declarations */

enum status  no_action(enum status);      /* nothing to do */
enum status  stop_idle(enum status);      /* <Stop> when doing nothing */

enum status  do_TrackUp(enum status);     /* go to the next track */
enum status  do_TrackDown(enum status);   /* go to the previous track */

enum status  start_Play(enum status);     /* begin playing the current track */
enum status  begin_Play(enum status);     /* start playing from fast forward or reverse */
enum status  stop_Play(enum status);      /* stop playing */

enum status  start_RptPlay(enum status);  /* begin repeatedly playing the current track */
enum status  cont_RptPlay(enum status);   /* switch to repeat play from standard play */
enum status  begin_RptPlay(enum status);  /* start repeatedly playing from fast forward or reverse */

enum status  start_FastFwd(enum status);  /* start going fast forward */
enum status  switch_FastFwd(enum status); /* switch to fast forward from play */
enum status  begin_FastFwd(enum status);  /* switch to fast forward from reverse */

enum status  start_Reverse(enum status);  /* start going reverse */
enum status  switch_Reverse(enum status); /* switch to reverse from play */
enum status  begin_Reverse(enum status);  /* switch to reverse from fast forward */

enum status  stop_FFRev(enum status);     /* stop fast forward or reverse */

void         dec_FFRev_rate(void);        /* decrease fast forward/reverse speed */
void         inc_FFRev_rate(void);        /* increase fast forward/reverse speed */


#endif
