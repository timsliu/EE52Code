/****************************************************************************/
/*                                                                          */
/*                                 VFAT.H                                   */
/*                     Definitions for FAT Hard Drives                      */
/*                              Include File                                */
/*                           MP3 Jukebox Project                            */
/*                                EE/CS 52                                  */
/*                                                                          */
/****************************************************************************/

/*
   This file contains constants and data structures needed to read DOS hard
   drives, including long filename support.


   Revision History
      5/18/02  Glen George      Initial revision.
      6/27/02  Glen George      Added definitions for DOS_FILENAME_LEN,
                                DOS_EXTENSION_LEN, MAX_PATH_CHARS, and
                                MAX_NUM_PATHS (the latter two probably need
                                to be checked).
      6/5/03   Glen George      Added definition for ENTRIES_PER_SECTOR.
      6/5/03   Glen George      Reorganized file and updated comments.
      6/10/03  Glen George      Added macros for extracting times and dates
                                from directory entries and added a union to
                                get at the packed time and date information
                                (Intel C doesn't implement bit fields in the
                                necessary manner).
      6/11/03  Glen George      Fixed up structures for doing times and dates,
                                can't use a union because it still makes it
                                the wrong size.
      6/8/05   Glen George      Fixed up ints in structures so code is more
                                portable (in particular made them short ints).
      4/29/06  Glen George      Changed all structures to be unions with
                                arrays of words (short ints) and added macros
                                for element access.
      5/3/06   Glen George      Changed string access macros for directories
                                to be character based so it will work on more
                                architectures.
      5/30/06  Glen George      Changed unions to not include structs if using
                                arrays so the code will work with more types
                                of architectures.
      6/28/07  Glen George      Changed short int elements to mask off bits
                                past bit 15 in case short int's are larger
                                than 16 bits (for portability).
      6/5/08   Glen George      Added definitions for PARTITION_START_HI and
                                PARTITION_START_LO, the position of the high
                                and low word of the first partition's starting
                                sector number in the partition table sector.
      6/12/08  Glen George      Fixed definitions for PARTITION_START_HI and
                                PARTITION_START_LO (they were byte indices
                                instead of word indices).
      3/15/13  Glen George      Added constants, macros, and structures to
                                support FAT32.
*/



#ifndef  I__VFAT_H__
    #define  I__VFAT_H__


/* library include files */
  /* none */

/* local include files */
#include  "mp3defs.h"
#include  "interfac.h"




/* constants */

/* Master Boot Record constants */

/* offset of high and low word of the first partition's starting sector */
#define  PARTITION_START_LO  0xE3
#define  PARTITION_START_HI  0xE4

/* offset of partition type */
#define  PARTITION_TYPE     0xE1

/* values for partition type */
#define  PARTITION_FAT12    0x01
#define  PARTITION_FAT16S   0x04
#define  PARTITION_FAT16    0x06
#define  PARTITION_FAT32    0x0B
#define  PARTITION_FAT32_1  0x0C
#define  PARTITION_FAT32_2  0x0E
#define  PARTITION_FAT32_3  0x0F


/* boot record constants */

/* size of the volume label */
#define  VOL_LABEL_LEN      11


/* FAT constants */

/* bad cluster marker for FAT16 and FAT32 */
#define  FAT16_BAD  0xFFF7
#define  FAT32_BAD  0xFFFFFFF7


/* directory constants */

/* words in a directory entry */
#define  DIR_ENTRY_SIZE     16

/* directory entries per sector */
#define  ENTRIES_PER_SECTOR (IDE_BLOCK_SIZE / DIR_ENTRY_SIZE)


/* length of filenames of extensions in DOS (standard 8.3 filename) */
#define  DOS_FILENAME_LEN   8
#define  DOS_EXTENSION_LEN  3

/* number of characters in long filename entries */
#define  LFN1_CHARS     5               /* 5 characters in first part */
#define  LFN2_CHARS     6               /* 6 characters in second part */
#define  LFN3_CHARS     2               /* 2 characters in third/final part */
#define  LFN_CHARS      (LFN1_CHARS + LFN2_CHARS + LFN3_CHARS)

/* constants for long filenames */
#define  LFN_SEQ_MASK    0x1F           /* mask for LFN sequence number (mask off flags) */
#define  LAST_LFN_ENTRY  0x40           /* this is the last LFN entry */
#define  MAX_LFN_LEN     256            /* maximum length of a long filename */

/* file attributes */
#define  ATTRIB_RO       0x01           /* file is read only */
#define  ATTRIB_HIDDEN   0x02           /* file is hidden */
#define  ATTRIB_SYSTEM   0x04           /* system file */
#define  ATTRIB_VOLUME   0x08           /* volume label */
#define  ATTRIB_DIR      0x10           /* file is a sub-directory */
#define  ATTRIB_ARCHIVE  0x20           /* file is archived */
#define  ATTRIB_LFN      0x0F           /* entry is a long filename */


/* maximum length of paths (number of characters and number of directories */
#define  MAX_PATH_CHARS     300
#define  MAX_NUM_SUBDIRS    150




/* macros */

/* macros to access the time elements in a packed word (directory info) */
#define  DIR_SECONDS(x)  ((x) & 0x1F)           /* seconds in bits 0-4 */
#define  DIR_MINUTES(x)  (((x) >> 5) & 0x3F)    /* minutes in bits 5-10 */
#define  DIR_HOURS(x)    (((x) >> 11) & 0x1F)   /* hours in bits 11-15 */

/* macros to access the date elements in a packed word (directory entry) */
#define  DIR_DAY(x)    ((x) & 0x1F)             /* day in bits 0-4 */
#define  DIR_MONTH(x)  (((x) >> 5) & 0x0F)      /* months in bits 5-8 */
#define  DIR_YEAR(x)   (((x) >> 9) & 0x7F)      /* year in bits 9-15 */




/* structures, unions, and typedefs */

/* the first sector (boot sector) of a FAT16 hard drive */
struct  boot_sector_16  {
    char           bootjmp[3];          /* jump to boot code */
    char           OEMName[8];          /* OEM name and version */
    short int      bytes_per_sector;    /* bytes in a sector */
    unsigned char  alloc_sectors;       /* sectors per allocation unit */
    short int      reserved_sectors;    /* number of reserved sectors */
    unsigned char  numFATs;             /* number of FAT copies */
    short int      root_entries;        /* number of root directory entries */
    short int      log_vol_sectors;     /* sectors in logical volume */
    unsigned char  media_type;          /* type of media */
    short int      FAT_sectors;         /* number of sectors per FAT */
    short int      track_sectors;       /* number of sectors per track */
    short int      num_heads;           /* number of heads */
    long int       hidden_sectors;      /* number of hidden sectors */
    long int       log_vol_4_sectors;   /* sectors in logical volume (v4.0) */
    unsigned char  drive_no;            /* drive number */
    unsigned char  resvd1;              /* reserved byte */
    unsigned char  boot_sig;            /* extended boot record signature */
    long int       volID;               /* volume ID number */
    char           vol_label[11];       /* volume label */
    char           FAT_name[8];         /* name of the FAT type */
    char           bootstrap[448];      /* bootstrap code */
    short int      signature;           /* boot signature (0xAA55) */
};


/* the first sector (boot sector) of a FAT32 hard drive */
struct  boot_sector_32  {
    char           bootjmp[3];          /* jump to boot code */
    char           OEMName[8];          /* OEM name and version */
    short int      bytes_per_sector;    /* bytes in a sector */
    unsigned char  alloc_sectors;       /* sectors per allocation unit */
    short int      reserved_sectors;    /* number of reserved sectors */
    unsigned char  numFATs;             /* number of FAT copies */
    short int      root_entries;        /* number of root directory entries */
    short int      log_vol_sectors;     /* sectors in logical volume */
    unsigned char  media_type;          /* type of media */
    short int      FAT16_sectors;       /* number of sectors per FAT16 */
    short int      track_sectors;       /* number of sectors per track */
    short int      num_heads;           /* number of heads */
    long int       hidden_sectors;      /* number of hidden sectors */
    long int       log_vol_4_sectors;   /* sectors in logical volume (v4.0) */
    long int       FAT_sectors;         /* number of sectors per FAT */
    short int      FAT_flags;           /* flags describing FAT */
    short int      FAT_version;         /* FAT version */
    long int       root_cluster;        /* first cluster of root directory */
    short int      FSIS_sector;         /* sector number of FSIS */
    short int      backup_boot_sector;  /* backup boot sector number */
    char           resvd2[12];          /* reserved bytes */
    unsigned char  drive_no;            /* drive number */
    unsigned char  resvd1;              /* reserved byte */
    unsigned char  boot_sig;            /* extended boot record signature */
    long int       volID;               /* volume ID number */
    char           vol_label[11];       /* volume label */
    char           FAT_name[8];         /* name of the FAT type */
    char           bootstrap[420];      /* bootstrap code */
    short int      signature;           /* boot signature (0xAA55) */
};


/* the first sector of the hard drive */
/* if using arrays only, don't need/want the struct in the union */
union  first_sector  {
#ifndef  USE_ARRAY
    struct boot_sector_16  b16;                 /* either FAT16 boot sector */
    struct boot_sector_32  b32;                 /* or FAT32 boot sector */
#endif
    unsigned short int  words[IDE_BLOCK_SIZE];  /* or view as raw bytes */
};


/* macros to access elements of the boot sector for portability */
#ifdef  USE_ARRAY
  /* use the raw bytes to access the boot sector */
  #define  SECTOR_SIZE(s)       ((((s).words[5] >> 8) & 0xFF) | (((s).words[6] & 0xFF) << 8))
  #define  ALLOC_SECTORS(s)     (((s).words[6] >> 8) & 0xFF)
  #define  RESERVED_SECTORS(s)  (((s).words[7]) & 0xFFFF)
  #define  NUMFATS(s)           ((s).words[8] & 0xFF)
  #define  ROOT_ENTRIES(s)      ((((s).words[8] >> 8) & 0xFF) | (((s).words[9] & 0xFF) << 8))
  #define  FAT_SECTORS_16(s)    (((s).words[11]) & 0xFFFF)
  #define  FAT_SECTORS_32(s)    (((unsigned long int) ((s).words[18]) & 0xFFFF) | (((unsigned long int) ((s).words[19]) & 0xFFFF) << 16))
  #define  ROOT_CLUSTER(s)      (((unsigned long int) ((s).words[22]) & 0xFFFF) | (((unsigned long int) ((s).words[23]) & 0xFFFF) << 16))
  #define  VOLUME_ID_16(s)      (((char *) &(s)) + 43)
  #define  VOLUME_ID_32(s)      (((char *) &(s)) + 71)
#else
  /* use the structure to access the boot sector */
  #define  SECTOR_SIZE(s)       ((s).b16.bytes_per_sector)
  #define  ALLOC_SECTORS(s)     ((s).b16.alloc_sectors)
  #define  RESERVED_SECTORS(s)  ((s).b16.reserved_sectors)
  #define  NUMFATS(s)           ((s).b16.numFATs)
  #define  ROOT_ENTRIES(s)      ((s).b16.root_entries)
  #define  FAT_SECTORS_16(s)    ((s).b16.FAT_sectors)
  #define  FAT_SECTORS_32(s)    ((s).b32.FAT_sectors)
  #define  ROOT_CLUSTER(s)      ((s).b32.root_cluster)
  #define  VOLUME_ID_16(s)      ((s).b16.vol_label)
  #define  VOLUME_ID_32(s)      ((s).b32.vol_label)
#endif


/* the timestamp on the file */

/*
   there are two versions of the timestamp - the first uses bitfields and
   can be used for compilers that support packed bit fields, the second
   uses ints and macros to extract the bit fields for other compilers.
*/

/* bit field version of the timestamp */
struct  ftime_bitfield  {                   /* time and date as bitfields */
        unsigned int  seconds : 5;              /* seconds / 2 (0-29) */
        unsigned int  minutes : 6;              /* minutes (0-59) */
        unsigned int  hours   : 5;              /* hours (0-23) */
        unsigned int  day     : 5;              /* day of the month (1-31) */
        unsigned int  month   : 4;              /* month (1-12) */
        unsigned int  year    : 7;              /* year - 1980 */
};

/* packed int version of the timestamp */
struct  ftime_packed  {                     /* time and date as words */
        unsigned short int  time;               /* time packed into a word */
        unsigned short int  date;               /* date packed into a word */
};


/* a normal DOS (8.3) directory entry (FAT16) */
struct  DOS83_entry  {
    char           filename[DOS_FILENAME_LEN];      /* the name of the file */
    char           extension[DOS_EXTENSION_LEN];    /* the file extension */
    unsigned char         attr;             /* file attributes */
    char                  reserved[10];     /* reserved bytes */
    struct ftime_packed   tstamp;           /* the file time and date */
    unsigned short int    start_cluster;    /* starting cluster of the file */
    long int              size;             /* size of the file */
};


/* a normal DOS (8.3) FAT32 directory entry */
struct  FAT32_entry  {
    char           filename[DOS_FILENAME_LEN];      /* the name of the file */
    char           extension[DOS_EXTENSION_LEN];    /* the file extension */
    unsigned char         attr;             /* file attributes */
    char                  reserved[8];      /* reserved bytes */
    unsigned short int    start_cluster_hi; /* high word of starting cluster */
    struct ftime_packed   tstamp;           /* the file time and date */
    unsigned short int    start_cluster_lo; /* low word of starting cluster */
    long int              size;             /* size of the file */
};


/* a long filename directory entry */
struct  LFN_entry  {
    unsigned char       seq_num;             /* sequence number of long filename entry */
    char                LFN1[2 * LFN1_CHARS];/* first part of Unicode filename */
    unsigned char       attr;                /* file attributes (should be 0x0F) */
    unsigned char       type;                /* file type (should be 0) */
    unsigned char       checksum;            /* checksum for directory entry */
    char                LFN2[2 * LFN2_CHARS];/* second part of Unicode filename */
    unsigned short int  start_cluster;       /* starting cluster (should be 0) */
    char                LFN3[2 * LFN3_CHARS];/* last part of Unicode filename */
};


/* a directory entry */
/* if using the array only, don't need/want structs */
union  VFAT_dir_entry  {
#ifndef  USE_ARRAY
    struct DOS83_entry  d;                      /* either a DOS 8.3 entry */
    struct FAT32_entry  f;                      /* or a FAT32 entry */
    struct LFN_entry    l;                      /* or a long filename entry */
#endif
    unsigned short int  words[DIR_ENTRY_SIZE];  /* or view as raw bytes */
};


/* macros to access elements of the directory entry for portability */
#ifdef  USE_ARRAY
  /* use the raw bytes to access the directory entry */
  /* note that these are hard coded as raw offsets since using symbols only */
  /*    masks potential problems with word boundaries if the symbols are */
  /*    changed */
  #define  FILENAME(e, i)      ((((e).words[(i) / 2]) >> (8 * ((i) % 2))) & 0xFF)
  #define  EXTENSION(e, i)     ((((e).words[4 + ((i) / 2)]) >> (8 * ((i) % 2))) & 0xFF)
  #define  ATTR(e)             (((e).words[5] >> 8) & 0xFF)
  #define  FTIME(e)            (((e).words[11]) & 0xFFFF)
  #define  FSIZE(e)            (((long int) (((e).words[14]) & 0xFFFF)) | (((long int) (((e).words[15]) & 0xFFFF)) << 16))
  #define  START_CLUSTER(e)    (((e).words[13]) & 0xFFFF)
  #define  START_CLUSTER32(e)  (((long int) (((e).words[13]) & 0xFFFF)) | (((long int) (((e).words[10]) & 0xFFFF)) << 16))
  #define  L_SEQ_NUM(e)        ((e).words[0] & 0xFF)
  #define  L_LFN1(e, i)        ((((e).words[((i) + 1) / 2]) >> (8 * (((i) + 1) % 2))) & 0xFF)
  #define  L_LFN2(e, i)        ((((e).words[7 + ((i) / 2)]) >> (8 * ((i) % 2))) & 0xFF)
  #define  L_LFN3(e, i)        ((((e).words[14 + ((i) / 2)]) >> (8 * ((i) % 2))) & 0xFF)
  #define  CHECKSUM(e)         (((e).words[6] >> 8) & 0xFF)
#else
  /* use the structures to access the directory entry */
  #define  FILENAME(e, i)      ((e).d.filename[i])
  #define  EXTENSION(e, i)     ((e).d.extension[i])
  #define  ATTR(e)             ((e).d.attr)
  #define  FTIME(e)            ((e).d.tstamp.time)
  #define  FSIZE(e)            ((e).d.size)
  #define  START_CLUSTER(e)    ((e).d.start_cluster)
  #define  START_CLUSTER32(e)  (((long int) ((e).f.start_cluster_lo)) | ((long int) ((e).f.start_cluster_hi) << 16))
  #define  L_SEQ_NUM(e)        ((e).l.seq_num)
  #define  L_LFN1(e, i)        ((e).l.LFN1[i])
  #define  L_LFN2(e, i)        ((e).l.LFN2[i])
  #define  L_LFN3(e, i)        ((e).l.LFN3[i])
  #define  CHECKSUM(e)         ((e).l.checksum)
#endif




/* function declarations */
    /* none */


#endif
