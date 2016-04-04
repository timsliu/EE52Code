/****************************************************************************/
/*                                                                          */
/*                                STUBFNCS                                  */
/*                          Audio Stub Functions                            */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS 52                                  */
/*                                                                          */
/****************************************************************************/

/*
   This file contains stub functions for the hardware interfacing code.  The
   file is meant to allow linking of the main code without necessarily having
   all the low-level functions.  The functions included are:
      update         - check if ready for an update
      elapsed_time   - get the time since the last call to this function
      key_available  - check if a key is available
      getkey         - get a key
      display_time   - display the passed time
      display_track  - display the passed track number
      display_status - display the passed status
      display_title  - display the passed track title
      display_artist - display the passed track artist
      get_blocks     - get data from the hard drive
      audio_play     - start audio output
      audio_halt     - halt audio input or output

   The local functions included are:
      none

   The locally global variable definitions included are:
      none


   Revision History
      6/6/00   Glen George       Initial revision (from the 3/6/99 version of
                                 stubfncs.c for the Digital Audio Recorder
                                 Project).
      6/2/02   Glen George       Removed ffrev_start() and ffrev_halt(), they
                                 are no longer part of the user-written code.
      6/5/03   Glen George       Removed display_track(), is is no longer part
                                 of the user-written code.
      4/29/06  Glen George       Updated definitions of get_blocks(),
                                 update(), and audio_play() to use words
				 instead of bytes.
*/



/* library include files */
  /* none */

/* local include files */
#include  "mp3defs.h"




/* update function */

unsigned char  update(unsigned short int far *p, int n)
{
    return  FALSE;
}



/* timing function */

int  elapsed_time()
{
    return  0;
}



/* keypad functions */

unsigned char  key_available()
{
    return  FALSE;
}

int  getkey()
{
    return  KEY_ILLEGAL;
}



/* display functions  */

void  display_time(unsigned int t)
{
    return;
}

void  display_status(unsigned int s)
{
    return;
}

void  display_title(const char far *t)
{
    return;
}

void  display_artist(const char far *a)
{
    return;
}



/* IDE interface function */

int  get_blocks(unsigned long int b, int n, unsigned short int far *p)
{
    return  n;
}



/* audio functions */

void  audio_play(unsigned short int far *p, int n)
{
    return;
}

void  audio_halt()
{
    return;
}

