/****************************************************************************/
/*                                                                          */
/*                                INTERFAC.H                                */
/*                           Interface Definitions                          */
/*                               Include File                               */
/*                            MP3 Jukebox Project                           */
/*                                 EE/CS 52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants for interfacing between the C code and
   the assembly code/hardware.  This is a sample interface file to allow test
   compilation and linking of the code.


   Revision History:
      6/3/00   Glen George       Initial revision.
      4/2/01   Glen George       Removed definitions of DRAM_SIZE and
	                         IDE_SIZE, they are no longer used.
      6/5/03   Glen George       Added constant definitions of TIME_NONE,
	                         PARENT_DIR_CHAR, and SUBDIR_CHAR.
      4/29/06  Glen George       Updated value of IDE_BLOCK_SIZE to be in
	                         units of words, not bytes.
*/



#ifndef  I__INTERFAC_H__
    #define  I__INTERFAC_H__


/* library include files */
  /* none */

/* local include files */
  /* none */




#define  DRAM_STARTSEG   0x4000

#define  KEY_TRACKUP     0
#define  KEY_TRACKDOWN   1
#define  KEY_PLAY        2
#define  KEY_RPTPLAY     3
#define  KEY_FASTFWD     4
#define  KEY_REVERSE     5
#define  KEY_STOP        6
#define  KEY_ILLEGAL     7

#define  TIME_NONE       65535

#define  PARENT_DIR_CHAR '<'
#define  SUBDIR_CHAR     '>'

#define  STATUS_PLAY     0
#define  STATUS_FASTFWD  1
#define  STATUS_REVERSE  2
#define  STATUS_IDLE     3
#define  STATUS_ILLEGAL  4

#define  IDE_BLOCK_SIZE  256		/* 256 words/block */


#endif
