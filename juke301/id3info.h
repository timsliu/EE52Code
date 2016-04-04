/****************************************************************************/
/*                                                                          */
/*                               ID3INFO.H                                  */
/*                         ID3 V1 Tag Information                           */
/*                              Include File                                */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants, macros, and structures for the parsing
   ID3 tags.


   Revision History
      3/10/13  Glen George       Initial revision.
*/




#ifndef  I__ID3INFO_H__
    #define  I__ID3INFO_H__


/* library include files */
  /* none */

/* local include files */
  /* none */




/* constants */

/* size and offsets of the tag elements */

/* ID3 tag identifier - value and size */
#define  ID3_TAG_ID             "TAG"
#define  ID3_TAG_ID_SIZE        3

/* ID3 title offset and size */
#define  ID3_TAG_TITLE_OFFSET   ID3_TAG_ID_SIZE
#define  ID3_TAG_TITLE_SIZE     30

/* ID3 artist offset and size */
#define  ID3_TAG_ARTIST_OFFSET  (ID3_TAG_TITLE_OFFSET + ID3_TAG_TITLE_SIZE)
#define  ID3_TAG_ARTIST_SIZE    30

/* ID3 album offset and size */
#define  ID3_TAG_ALBUM_OFFSET   (ID3_TAG_ARTIST_OFFSET + ID3_TAG_ARTIST_SIZE)
#define  ID3_TAG_ALBUM_SIZE     30

/* ID3 year offset and size */
#define  ID3_TAG_YEAR_OFFSET    (ID3_TAG_ALBUM_OFFSET + ID3_TAG_ALBUM_SIZE)
#define  ID3_TAG_YEAR_SIZE      4

/* ID3 comment offset and size */
#define  ID3_TAG_CMNT_OFFSET    (ID3_TAG_YEAR_OFFSET + ID3_TAG_YEAR_SIZE)
#define  ID3_TAG_CMNT_SIZE      24

/* ID3 time (in comment for our mod) offset and size */
#define  ID3_TAG_TIME_OFFSET    (ID3_TAG_CMNT_OFFSET + ID3_TAG_CMNT_SIZE)
#define  ID3_TAG_TIME_SIZE      4

/* ID3 track number (in comment for v1.1) offset and size */
#define  ID3_TAG_TRACK_OFFSET   (ID3_TAG_TIME_OFFSET + ID3_TAG_TIME_SIZE)
#define  ID3_TAG_TRACK_SIZE     2

/* ID3 genre offset and size */
#define  ID3_TAG_GENRE_OFFSET   (ID3_TAG_TRACK_OFFSET + ID3_TAG_TRACK_SIZE)
#define  ID3_TAG_GENRE_SIZE     1

/* size of an ID3 tag */
#define  ID3_TAG_SIZE           128




/* structures, unions, and typedefs */
    /* none */




/* function declarations */
    /* none */


#endif
