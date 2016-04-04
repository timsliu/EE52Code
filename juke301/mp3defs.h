/****************************************************************************/
/*                                                                          */
/*                                MP3DEFS.H                                 */
/*                           General Definitions                            */
/*                               Include File                               */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS  52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the general definitions for the MP3 Jukebox.  This
   includes constant, macro, and structure definitions along with the function
   declarations for the assembly language functions.


   Revision History:
      6/5/00   Glen George       Initial revision.
      6/7/00   Glen George       Changed type of TIME_SCALE constant.
      6/14/00  Glen George       Increased BUFFER_BLOCKS from 2 to 32 to make
                                 the output smoother.
      6/14/00  Glen George       Changed the size element of the audio_buf
                                 structure from int to unsigned int and the
                                 pointer element (p) from char far * to
                                 unsigned char far *.
      6/2/02   Glen George       Replaced FFREV_SIZE with FFREV_RATE and added
                                 MIN_FFREV_TIME to support the new method of
                                 doing fast forward and reverse.
      6/2/02   Glen George       Removed declarations for ffrev_start() and
                                 ffrev_halt(), they are no longer used.
      6/2/02   Glen George       Added macro for creating far pointers to ease
                                 portablility for non-segmented architectures.
      6/2/02   Glen George       Updated comments.
      6/10/02  Glen George       Added SECTOR_ADJUST constant for dealing with
                                 hard drives with different geometries.
      6/10/02  Glen George       Updated comments.
      5/15/03  Glen George       Removed definition of NULL, let stddef.h take
                                 care of it.
      5/15/03  Glen George       Added conditional definition of MAKE_FARPTR
                                 so it will work for both flat and segmented
                                 architectures (segmented is default) and
                                 removed far keyword in the flat memory model.
      6/5/03   Glen George       Made track time an unsigned int in the track
                                 information structure.
      4/29/06  Glen George       Added conditional definitions of macros for
                                 library functions and conditional inclusion
                                 of the library function headers depending on
                                 whether or not the library is being used.
      4/29/06  Glen George       Added support for meta-macros like DSP56K and
                                 NIOS which automatically set all the other
                                 compilation controlling macros.
      4/29/06  Glen George       Changed declarations for the get_blocks(),
                                 update(), and audio_play() functions and the
                                 audio_buf structure to use words instead of
                                 bytes.
      4/29/06  Glen George       Removed display_track() declaration and
                                 MAX_NO_TRACKS definition since they are no
                                 longer used.
      6/5/08   Glen George       Removed FFREV_RATE and added MIN_FFREV_RATE,
                                 MAX_FFREV_RATE, and DELTA_FFREV_RATE to
                                 handle variable rate fast forward and
                                 reverse.
      6/19/08  Glen George       Removed INDEX_START and SECTOR_ADJUST, they
                                 are no longer used.
      3/10/13  Glen George       Added metamacro for BLACKFIN.
      3/10/13  Glen George       Added macro definition for strncpy()
                                 function.
      3/15/13  Glen George       Added constants for the size of the FAT
                                 cache.
      3/15/13  Glen George       Changed track_header declaration to hold
                                 actual title and artist strings instead of
                                 pointers to them and added constants to set
                                 the size of those strings.  It also no longer
                                 needs to keep track of the starting position.
*/



#ifndef  I__MP3DEFS_H__
    #define  I__MP3DEFS_H__


/* library include files */
  /* none */

/* local include files */
#include  "interfac.h"
#include  "id3info.h"




/* constants */

/* general constants */
#define  FALSE       0
#define  TRUE        !FALSE


/* number of words and blocks in the FAT cache */
#define  FAT_CACHE_BLOCKS     64
#define  FAT_CACHE_SIZE       (FAT_CACHE_BLOCKS * IDE_BLOCK_SIZE)


/* song information parameters */

/* maximum length of a title (based on ID3 length) */
#define  MAX_TITLE_LEN        (ID3_TAG_TITLE_SIZE + 1)

/* maximum length of an artist name (based on ID3 length) */
#define  MAX_ARTIST_LEN       (ID3_TAG_ARTIST_SIZE + 1)


/* audio parameters */

/* value to use when there is no MP3 data */
#define  NO_MP3_DATA          0

/* number of buffers to use for buffering MP3 data */
#define  NO_BUFFERS           3

/* number of words and blocks in an MP3 buffer */
#define  BUFFER_BLOCKS        32
#define  BUFFER_SIZE          (BUFFER_BLOCKS * IDE_BLOCK_SIZE)

/* rates at which fast forward and reverse are to run */
#define  MIN_FFREV_RATE        3    /* minimum fast forward/reverse rate */
#define  MAX_FFREV_RATE       10    /* maximum fast forward/reverse rate */
#define  DELTA_FFREV_RATE      2    /* amount to change fast forward/reverse rate */

/* minimum amount of time (in ms) to move by when in fast forward or reverse */
#define  MIN_FFREV_TIME       500


/* timing parameters */

/* difference between elapsed_time() and display_time() times */
#define  TIME_SCALE           100L




/* macros */

/* add the definitions necessary for the Analog Devices Blackfin chip */
#ifdef  BLACKFIN
  #define  FLAT_MEMORY          /* use the flat memory model */
  #define  USE_LIBRARY          /* use the standard library functions */
  #define  USE_ARRAY            /* use arrays to access disk data, not structures */
#endif

/* add the definitions necessary for the Freescale DSP56K chip */
#ifdef  DSP56K
  #define  FLAT_MEMORY          /* use the flat memory model */
  #define  USE_LIBRARY          /* use the standard library functions */
  #define  USE_ARRAY            /* use arrays to access disk data, not structures */
#endif

/* add the definitions necessary for the Altera NIOS chip */
#ifdef  NIOS
  #define  FLAT_MEMORY          /* use the flat memory model */
  #define  USE_LIBRARY          /* use the standard library functions */
#endif


/* macro to make a far pointer given a segment and offset */
#ifdef  FLAT_MEMORY
  #define  MAKE_FARPTR(seg, off)  ((void *) ((0x10UL * (seg)) + (unsigned long int) (off)))
#else
  #define  MAKE_FARPTR(seg, off)  ((void far *) ((0x10000UL * (seg)) + (unsigned long int) (off)))
#endif


/* if a flat memory model don't need far pointers */
#ifdef  FLAT_MEMORY
  #define  far
#endif


/* if using standard libraries, include them */
#ifdef  USE_LIBRARY
  #include  <string.h>
  #include  <stdlib.h>
#else
  /* need to redefine the library functions to use locally defined functions */
  #define  abs(x)               abs_(x)
  #define  strlen(str)          strlen_(str)
  #define  strcpy(s1, s2)       strcpy_((s1), (s2))
  #define  strncpy(s1, s2, n)   strncpy_((s1), (s2), (n))
  #define  strcat(s1, s2)       strcat_((s1), (s2))
  /* also declare the functions */
  #include  "lib.h"
#endif



/* structures, unions, and typedefs */

/* audio buffer structure */
struct  audio_buf  {
                      unsigned short int far  *p;    /* pointer to actual buffer data */
                      unsigned int             size; /* size of the buffer in words */
                      int                      done; /* out of data flag */
                   };

/* track header structure */
struct  track_header  {
                         char          title[MAX_TITLE_LEN];    /* title of the track */
                         char          artist[MAX_ARTIST_LEN];  /* track artist */
                         unsigned int  time;                    /* time length of track */
                         long int      length;                  /* length in bytes */
                         long int      curpos;                  /* current position (offset in bytes) */
                      };

/* status types */
enum status  {  STAT_IDLE,              /* system idle */
                STAT_PLAY,              /* playing (or repeat playing) a track */
                STAT_FF,                /* fast forwarding a track */
                STAT_REV,               /* reversing a track */
                NUM_STATUS              /* number of status types */
             };

/* key codes */
enum keycode  {  KEYCODE_TRACKUP,    /* <Track Up>     */
                 KEYCODE_TRACKDOWN,  /* <Track Down>   */
                 KEYCODE_PLAY,       /* <Play>         */
                 KEYCODE_RPTPLAY,    /* <Repeat Play>  */
                 KEYCODE_FASTFWD,    /* <Fast Forward> */
                 KEYCODE_REVERSE,    /* <Reverse>      */
                 KEYCODE_STOP,       /* <Stop>         */
                 KEYCODE_ILLEGAL,    /* other keys     */
                 NUM_KEYCODES        /* number of key codes */
              }; 




/* function declarations */

/* update needed function */
unsigned char  update(unsigned short int far *, int);

/* how much time has elapsed */
int  elapsed_time(void);

/* keypad functions */
unsigned char  key_available(void);     /* key is available */
int            getkey(void);            /* get a key */

/* display functions  */
void  display_time(unsigned int);       /* display the track time */
void  display_status(unsigned int);     /* display the system status */
void  display_title(const char far *);  /* display the track title */
void  display_artist(const char far *); /* display the track artist */

/* IDE interface functions */
int  get_blocks(unsigned long int, int, unsigned short int far *);   /* get data */

/* audio functions */
void  audio_play(unsigned short int far *, int);  /* start playing */
void  audio_halt(void);                           /* halt play or record */


#endif
