/****************************************************************************/
/*                                                                          */
/*                                  LIB.H                                   */
/*                      Replacement Library Functions                       */
/*                               Include File                               */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS  52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants and function prototypes for the library
   functions defined in lib188.asm.  These functions are needed because the
   standard library functions don't work due to the memory model being used.


   Revision History:
      4/29/06  Glen George       Initial revision.
*/



#ifndef  I__LIB_H__
    #define  I__LIB_H__


/* library include files */
  /* none */

/* local include files */
  /* none */




/* constants */
    /* none */




/* structures, unions, and typedefs */
    /* none */




/* function declarations */

int        abs_(int);				    /* find the absolute value */

char far  *strcat_(char far *, const char far *);   /* concatenate strings */
char far  *strcpy_(char far *, const char far *);   /* copy strings */
int        strlen_(const char far *);		    /* find the string length */


#endif
