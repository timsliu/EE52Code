/****************************************************************************/
/*                                                                          */
/*                                UPDATFNC.H                                */
/*                             Update Functions                             */
/*                               Include File                               */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS  52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants and function prototypes for the update
   functions defined in ffrev.c, keyupdat.c, and playmp3.c.


   Revision History:
      6/4/00   Glen George       Initial revision (from the 3/6/99 version of
                                 updatfnc.h for the Digital Audio Recorder
                                 Project).
*/



#ifndef  I__UPDATFNC_H__
    #define  I__UPDATFNC_H__


/* library include files */
  /* none */

/* local include files */
#include  "mp3defs.h"




/* constants */
    /* none */




/* structures, unions, and typedefs */
    /* none */




/* function declarations */

enum status  no_update(enum status);       /* no update to do */
enum status  update_Play(enum status);     /* update play, fill another buffer */
enum status  update_FastFwd(enum status);  /* update fast forward, decrement the time */
enum status  update_Reverse(enum status);  /* update reverse, increment the time */


#endif
