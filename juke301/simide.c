/****************************************************************************/
/*                                                                          */
/*                                  SIMIDE                                  */
/*                         IDE Simulation Functions                         */
/*                           MP3 Jukebox Project                            */
/*                                 EE/CS 52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains a function for simulation an IDE hard drive for the MP3
   Jukebox project.  This function can be used to test the software without a
   physical hard drive being connected.  The functions included are:
      get_blocks - retrieve blocks of data from the simulated hard drive.

   The local functions included are:
      none

   The locally global variable definitions included are:
      none


   Revision History
      6/7/00   Glen George       Initial revision.
      6/10/00  Glen George       Changed hex character constants to octal so
                                 will compile under Intel C.
      6/2/02   Glen George       Fixed format for simulated index file, it was
                                 inconsistent with current code.
      6/2/02   Glen George       Updated comments.
      5/11/03  Glen George       Fixed const-ness for character arrays so will
                                 compile using rom model.
      6/5/03   Glen George       Updated function headers.
      5/13/05  Glen George       Updated the function to allow simulation for
                                 version 2 of the code.
      4/29/06  Glen George       Made arrays of faked data be unsigned char.
      4/29/06  Glen George       Updated the function to match the new
                                 specification for get_update() to use words
                                 instead of bytes.
      3/20/13  Glen George       Updated the function to match the new code
                                 that uses ID3 tags and FAT entries.
*/



/* library include files */
  /* none */

/* local include files */
#include  "mp3defs.h"
#include  "interfac.h"



/* useful definitions */
#define  MAX_READ_SIZE   3      /* maximum number of blocks to read */

#define  NO_ENTRIES      3      /* number of song entries */

#define  BLOCK_LENGTH    8      /* number of words in the block number */
#define  CHAR_COUNT      26     /* number of characters to put in a block */




/*
   get_blocks

   Description:      This function simulates the reading of blocks from an IDE
                     hard drive.  The data "read" is written to the memory
                     pointed to by the third argument.  The number of blocks
                     requested is given as the second argument.  If this value
                     is greater than the MAX_READ_SIZE, only MAX_READ_SIZE
                     blocks are read.  The starting block number to read is
                     passed as the first argument.  If this is between 0 and
                     NO_ENTRIES a fake track information block is returned.
                     Otherwise the block returned has the hex string for the
                     block number followed by a thru z and then all 0 values.
                     The function returns the number of blocks "read" (the
                     smaller of MAX_READ_SIZE and the second argument).

   Arguments:        block (unsigned long int)       - block number at which
                                                       to start the read.
                     length (int)                    - number of blocks to be
                                                       read.
                     dest (unsigned short int far *) - pointer to the memory
                                                       where the read data is
                                                       to be written.
   Return Value:     The number of blocks actually written (for this code it
                     is always either the passed length or MAX_READ_SIZE).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 20, 2013

*/

int  get_blocks(unsigned long int block, int length, unsigned short int far *dest)
{
    /* variables */

    /* array containing the "fake" data for the boot sector */
    static const unsigned char  boot_sector[] = {
        0, 0, 0,                /* boot jump */
        'F', 'a', 'k', 'e',     /* OEM name */
        'B', 'o', 'o', 't',
        0, 2,                   /* 512 bytes per sector */
        64,                     /* sectors per allocation unit */
        1, 0,                   /* 1 reserved sector */
        2,                      /* 2 FATs */
        0, 4,                   /* 1024 root directory entries */
        0, 0,                   /* 0 sectors per logical volume */
        0xF8,                   /* media type */
        0, 1,                   /* 256 FAT sectors */
        63, 0,                  /* 63 sectors per track */
        255, 0,                 /* 255 heads */
        63, 0, 0, 0,            /* 63 hidden sectors */
        0x86, 0xFA, 0x3F, 0,    /* 4192902 sectors in the volume */
        0x80,                   /* drive number 0x80 */
        0,                      /* reserved byte */
        0x29,                   /* extended boot record signature */
        1, 2, 3, 4              /* volume ID */
    };

    /* array containing the "fake" data for the first FAT sector */
    static const unsigned char  FAT_sector[] = {
        0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, /* cluster 0 - 3 */
        0x05, 0x00, 0x06, 0x00, 0x07, 0x00, 0x08, 0x00,
        0x09, 0x00, 0x0A, 0x00, 0x0B, 0x00, 0x0C, 0x00,
        0x0D, 0x00, 0x0E, 0x00, 0x0F, 0x00, 0x10, 0x00,
        0x11, 0x00, 0x12, 0x00, 0x13, 0x00, 0x14, 0x00, /* cluster 16 - 19 */
        0x15, 0x00, 0x16, 0x00, 0x17, 0x00, 0x18, 0x00,
        0x19, 0x00, 0x1A, 0x00, 0x1B, 0x00, 0x1C, 0x00,
        0x1D, 0x00, 0x1E, 0x00, 0x1F, 0x00, 0x20, 0x00,
        0x21, 0x00, 0x22, 0x00, 0xFF, 0xFF, 0x00, 0x00, /* cluster 32 - 35 */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x29, 0x00, 0x30, 0x00, 0x00, 0x00, 0x2C, 0x00,
        0x2D, 0x00, 0x2E, 0x00, 0x2F, 0x00, 0x68, 0x00,
        0x31, 0x00, 0x32, 0x00, 0x34, 0x00, 0x00, 0x00, /* cluster 48 - 51 */
        0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x39, 0x00, 0x3A, 0x00, 0x3B, 0x00, 0x3C, 0x00,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x41, 0x00, 0x42, 0x00, 0x43, 0x00, 0x44, 0x00, /* cluster 64 - 67 */
        0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x49, 0x00, 0x4A, 0x00, 0x4B, 0x00, 0x4C, 0x00,
        0x4D, 0x00, 0x4E, 0x00, 0x71, 0x00, 0x00, 0x00,
        0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* cluster 80 - 83 */
        0x55, 0x00, 0x56, 0x00, 0x57, 0x00, 0x58, 0x00,
        0x59, 0x00, 0x5A, 0x00, 0x48, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x61, 0x00, 0x62, 0x00, 0xFF, 0xFF, 0x00, 0x00, /* cluster 96 - 99 */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x69, 0x00, 0x6A, 0x00, 0x6B, 0x00, 0x6D, 0x00,
        0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
        0x54, 0x00, 0x72, 0x00, 0x73, 0x00, 0x74, 0x00, /* cluster 112 - 115 */
        0x75, 0x00, 0x2B, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    /* array containing the "fake" data for the first directory sector */
    static const unsigned char  dir_sector[] = {
        0x42, 0x65, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x20,     /* first song */
        0x00, 0x44, 0x00, 0x0F, 0x00, 0xA2, 0x61, 0x00,
        0x79, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
        0x01, 0x50, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x69,
        0x00, 0x63, 0x00, 0x0F, 0x00, 0xA2, 0x20, 0x00,
        0x53, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x67, 0x00,
        0x7F, 0x00, 0x00, 0x00, 0x47, 0x00, 0x72, 0x00,
        0x50, 0x41, 0x4E, 0x49, 0x43, 0x53, 0x4F, 0x4E,
        0x20, 0x20, 0x20, 0x20, 0x00, 0x64, 0x62, 0x00,
        0xAD, 0x32, 0xAD, 0x32, 0x00, 0x00, 0x62, 0x00,
        0xAD, 0x32, 0x02, 0x00, 0x62, 0x00, 0x10, 0x00,
        0x42, 0x20, 0x00, 0x44, 0x00, 0x6F, 0x00, 0x75,     /* second song */
        0x00, 0x62, 0x00, 0x0F, 0x00, 0xCB, 0x74, 0x00,
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
        0x01, 0x53, 0x00, 0x70, 0x00, 0x69, 0x00, 0x64,
        0x00, 0x65, 0x00, 0x0F, 0x00, 0xCB, 0x72, 0x00,
        0x77, 0x00, 0x65, 0x00, 0x62, 0x00, 0x73, 0x00,
        0x7F, 0x00, 0x00, 0x00, 0x4E, 0x00, 0x6F, 0x00,
        0x53, 0x50, 0x49, 0x44, 0x45, 0x52, 0x57, 0x45,
        0x20, 0x20, 0x20, 0x20, 0x00, 0x64, 0x8D, 0x00,
        0xAD, 0x32, 0xAD, 0x32, 0x00, 0x00, 0x8D, 0x00,
        0xAD, 0x32, 0x28, 0x00, 0x00, 0x00, 0x0A, 0x00,
        0x43, 0x6B, 0x00, 0x20, 0x00, 0x31, 0x00, 0x38,     /* third song */
        0x00, 0x32, 0x00, 0x0F, 0x00, 0x38, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
        0x02, 0x20, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6C,
        0x00, 0x6C, 0x00, 0x0F, 0x00, 0x38, 0x65, 0x00,
        0x67, 0x00, 0x65, 0x00, 0x7F, 0x00, 0x42, 0x00,
        0x6C, 0x00, 0x00, 0x00, 0x69, 0x00, 0x6E, 0x00,
        0x01, 0x47, 0x00, 0x6F, 0x00, 0x69, 0x00, 0x6E,
        0x00, 0x67, 0x00, 0x0F, 0x00, 0x38, 0x20, 0x00,
        0x41, 0x00, 0x77, 0x00, 0x61, 0x00, 0x79, 0x00,
        0x20, 0x00, 0x00, 0x00, 0x74, 0x00, 0x6F, 0x00,
        0x47, 0x4F, 0x49, 0x4E, 0x47, 0x41, 0x57, 0x41,
        0x20, 0x20, 0x20, 0x20, 0x00, 0x64, 0xA3, 0x00,
        0xAD, 0x32, 0xAD, 0x32, 0x00, 0x00, 0xA3, 0x00,
        0xAD, 0x32, 0x70, 0x00, 0x7F, 0x80, 0x0E, 0x00
    };


    /* arrays containing the "fake" ID3 tag information for each song */
    static const unsigned char  ID3_tags[][128] = {
        "TAG"                                           /* song 1 */
        "Panic Song\0                   "
        "Green Day\0                    "
        "                              "
        "    "
        "                        \0\x40\x42\x79\0\0\0",
        "TAG"                                           /* song 2 */
        "Spiderwebs\0                   "
        "No Doubt\0                     "
        "                              "
        "    "
        "                        \0\x40\x44\x4C\0\0\0",
        "TAG"                                           /* song 3 */
        "Going Away to College\0        "
        "Blink-182\0                    "
        "                              "
        "    "
        "                        \0\x40\x44\x73\0\0\0"
    };


    int  no_blocks;             /* number of blocks to "read" */

    int  digit;                 /* a hex digit to output */

    int  i;                     /* general loop indices */
    int  j;
    int  k;



    /* first figure how many blocks to transfer */
    if (length > MAX_READ_SIZE)
        /* too many requested, only "read" MAX_READ_SIZE */
        no_blocks = MAX_READ_SIZE;
    else
        /* can "read" all the requested blocks */
        no_blocks = length;


    /* fill the blocks */
    for (i = 0; i < no_blocks; i++)  {

        /* check the block number to figure out what to transfer */
        if ((block + i) == 0)  {

            /* trying to read the master boot record, mostly it is 0 */
            for (j = 0; j < IDE_BLOCK_SIZE; j++)
                dest[j] = 0;

            /* set the start of the first partition */
            dest[227] = 64;
            /* set the partition type */
            dest[225] = 6;
        }
        else if ((block + i) == 64)  {

            /* trying to read the boot sector, transfer fake boot sector */
            for (j = 0; j < IDE_BLOCK_SIZE; j++)  {
                /* information or boot code */
                if (j < (sizeof(boot_sector) / 2))
                    /* still boot sector information - copy it */
                    *dest++ = boot_sector[2 * j] | (boot_sector[2 * j + 1] << 8);
                else
                    /* now in the boot code part of the sector - just zero it */
                    *dest++ = 0;
            }
        }
        else if ((block + i) == 65)  {

            /* trying to read first sector of FAT - fill the block */
            /* transfer the fake FAT sector */
            for (j = 0; j < IDE_BLOCK_SIZE; j++)  {
                /* does the fake data exist? */
                if (j < (sizeof(FAT_sector) / 2))
                    /* still FAT sector information - copy it */
                    *dest++ = FAT_sector[2 * j] | (FAT_sector[2 * j + 1] << 8);
                else
                    /* unused part of FAT sector - zero it */
                    *dest++ = 0;
            }
        }
        else if ((block + i) == 577)  {

            /* trying to read directory information - fill the block */
            /* transfer the fake directory sector */
            for (j = 0; j < IDE_BLOCK_SIZE; j++)  {
                /* does the fake data exist? */
                if (j < (sizeof(dir_sector) / 2))
                    /* still directory sector information - copy it */
                    *dest++ = dir_sector[2 * j] | (dir_sector[2 * j + 1] << 8);
                else
                    /* unused part of directory sector - zero it */
                    *dest++ = 0;
            }
        }
        else if ((block + i) == (34 - 2) * 64 - 1 + 641)  {

            /* beginning of ID3 tag for song 1 */
            /* transfer the fake sector */
            for (j = 0, k = 0; j < IDE_BLOCK_SIZE; j++)  {
                /* does the fake data exist? */
                if (j < (IDE_BLOCK_SIZE - 15))  {
                    /* not in ID3 tag - zero it */
                    dest[j] = 0;
                }
                else  {
                    /* in the ID3 tag - copy it */
                    dest[j] = ID3_tags[0][k] | (ID3_tags[0][k + 1] << 8);
                    /* move forward two bytes */
                    k += 2;
                }
            }
        }
        else if ((block + i) == (34 - 2) * 64 + 641)  {

            /* ending of ID3 tag for song 1 */
            /* transfer the fake sector */
            for (j = 0, k = 30; j < IDE_BLOCK_SIZE; j++, k += 2)  {
                /* does the fake data exist? */
                if (k >= 128)
                    /* not in ID3 tag - zero it */
                    dest[j] = 0;
                else
                    /* in the ID3 tag - copy it */
                    dest[j] = ID3_tags[0][k] | (ID3_tags[0][k + 1] << 8);
            }
        }
        else if ((block + i) == (98 - 2) * 64 + 63 + 641)  {

            /* complete ID3 tag for song 2 */
            /* transfer the fake sector */
            for (j = 0; j < IDE_BLOCK_SIZE; j++)  {
                /* does the fake data exist? */
                if (j < (IDE_BLOCK_SIZE - 64))
                    /* not in ID3 tag - zero it */
                    dest[j] = 0;
                else
                    /* in the ID3 tag - copy it */
                    dest[j] = ID3_tags[1][2 * (j - IDE_BLOCK_SIZE + 64)] |
                              (ID3_tags[1][2 * (j - IDE_BLOCK_SIZE + 64) + 1] << 8);
            }
        }
        else if ((block + i) == (107 - 2) * 64 + 63 + 641)  {

            /* beginning of ID3 tag for song 3 */
            /* transfer the fake sector */
            for (j = 0; j < IDE_BLOCK_SIZE; j++)
                /* fill sector with 0's */
                dest[j] = 0;

            /* now set the last byte to start of the ID3 tag */
            dest[IDE_BLOCK_SIZE - 1] = ID3_tags[2][0] << 8;
        }
        else if ((block + i) == (109 - 2) * 64 + 641)  {

            /* end of ID3 tag for song 3 */
            /* transfer the fake sector */
            for (j = 0, k = 1; j < IDE_BLOCK_SIZE; j++, k+= 2)  {
                /* check if want ID3 tag or past it */
                if (k == 127)
                    /* fill sector with ID3 tag */
                    dest[j] = ID3_tags[2][k];
                else if (k < 128)
                    /* fill sector with ID3 tag */
                    dest[j] = ID3_tags[2][k] | (ID3_tags[2][k + 1] << 8);
                else
                    /* fill rest if sector with 0's */
                    dest[j] = 0;
            }
        }
        else  {

            /* trying to read general blocks - fill the blocks */
            for (j = 0; j < IDE_BLOCK_SIZE; j++)  {

                /* figure out what to output */
                if (j < BLOCK_LENGTH)  {

                    /* output the block number in the first words */
                    /* get this digit of the block number */
                    digit = (((block + i) >> (4 * i)) & 0x00000000FL);

                    /* and output it while converting to a hex digit */
                    if (digit < 10)
                        *dest++ = digit + '0';
                    else
                        *dest++ = digit + 'A';
                }
                else if (j < (BLOCK_LENGTH + CHAR_COUNT))  {

                    /* output a-z in the next bytes */
                    *dest++ = 'a' + (j - BLOCK_LENGTH);
                }
                else  {

                    /* rest of the block is 0 */
                    *dest++ = 0;
                }
            }
        }
    }


    /* all done - return the number of blocks actually transferred */
    return  no_blocks;

}
