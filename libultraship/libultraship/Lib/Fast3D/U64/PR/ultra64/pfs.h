#ifndef ULTRA64_PFS_H
#define ULTRA64_PFS_H

#include "message.h"

/* File System size */
#define OS_PFS_VERSION      0x0200
#define OS_PFS_VERSION_HI   (OS_PFS_VERSION >> 8)
#define OS_PFS_VERSION_LO   (OS_PFS_VERSION & 255)

#define PFS_INODE_SIZE_PER_PAGE 128
#define PFS_FILE_NAME_LEN       16
#define PFS_FILE_EXT_LEN        4
#define BLOCKSIZE               32
#define PFS_ONE_PAGE            8
#define PFS_MAX_BANKS           62

/* File System flag */

#define PFS_READ                0
#define PFS_WRITE               1
#define PFS_CREATE              2

/* File System status */
#define PFS_INITIALIZED         0x1
#define PFS_CORRUPTED           0x2
#define PFS_ID_BROKEN           0x4
#define PFS_MOTOR_INITIALIZED   0x8
#define PFS_GBPAK_INITIALIZED   0x10

/* Definition for page usage */
#define PFS_EOF                 1
#define PFS_PAGE_NOT_EXIST      2
#define PFS_PAGE_NOT_USED       3

/* File System error number */

#define PFS_ERR_NOPACK          1   /* no memory card is plugged or */
#define PFS_ERR_NEW_PACK        2   /* ram pack has been changed to a  different one */
#define PFS_ERR_INCONSISTENT    3   /* need to run Pfschecker*/
#define PFS_ERR_CONTRFAIL       CONT_OVERRUN_ERROR              
#define PFS_ERR_INVALID         5   /* invalid parameter or file not exist*/
#define PFS_ERR_BAD_DATA        6   /* the data read from pack are bad*/
#define PFS_DATA_FULL           7   /* no free pages on ram pack*/
#define PFS_DIR_FULL            8   /* no free directories on ram pack*/
#define PFS_ERR_EXIST           9   /* file exists*/
#define PFS_ERR_ID_FATAL        10  /* dead ram pack */
#define PFS_ERR_DEVICE          11  /* wrong device type*/
#define PFS_ERR_NO_GBCART       12  /* no gb cartridge (64GB-PAK) */
#define PFS_ERR_NEW_GBCART      13  /* gb cartridge may be changed */

/* Definition for bank */
#define PFS_ID_BANK_256K    0
#define PFS_ID_BANK_1M      4
#define PFS_BANKS_256K      1

#define PFS_WRITTEN             2
#define DEF_DIR_PAGES           2

#define PFS_ID_0AREA            1
#define PFS_ID_1AREA            3
#define PFS_ID_2AREA            4
#define PFS_ID_3AREA            6
#define PFS_LABEL_AREA          7
#define PFS_ID_PAGE             PFS_ONE_PAGE * 0

#define PFS_BANK_LAPPED_BY  8   /* => u8 */
#define PFS_SECTOR_PER_BANK 32
#define PFS_INODE_DIST_MAP  (PFS_BANK_LAPPED_BY * PFS_SECTOR_PER_BANK)
#define PFS_SECTOR_SIZE     (PFS_INODE_SIZE_PER_PAGE/PFS_SECTOR_PER_BANK)

typedef struct {
    /* 0x00 */ s32 status;
    /* 0x04 */ OSMesgQueue* queue;
    /* 0x08 */ s32 channel;
    /* 0x0C */ u8 id[32];
    /* 0x2C */ u8 label[32];
    /* 0x4C */ s32 version;
    /* 0x50 */ s32 dir_size;
    /* 0x54 */ s32 inode_table;         /* block location */
    /* 0x58 */ s32 minode_table;        /* mirrioring inode_table */
    /* 0x5C */ s32 dir_table;           /* block location */
    /* 0x60 */ s32 inodeStartPage;    /* page # */
    /* 0x64 */ u8 banks;
    /* 0x65 */ u8 activebank;
} OSPfs; // size = 0x68

typedef struct {
    /* 0x00 */ u32 file_size;   /* bytes */
    /* 0x04 */ u32 game_code;
    /* 0x08 */ u16 company_code;
    /* 0x0C */ char ext_name[4];
    /* 0x10 */ char game_name[16];
} OSPfsState; // size = 0x20

typedef union {
    struct {
        /* 0x00 */ u8 bank;
        /* 0x01 */ u8 page;
    } inode_t;
    /* 0x00 */ u16 ipage;
} __OSInodeUnit; // size = 0x02

typedef struct {
    /* 0x00 */ __OSInodeUnit inodePage[128];
} __OSInode; // size = 0x100

typedef struct {
    /* 0x00 */ u32 game_code;
    /* 0x04 */ u16 company_code;
    /* 0x06 */ __OSInodeUnit start_page;
    /* 0x08 */ u8 status;
    /* 0x09 */ s8 reserved;
    /* 0x0A */ u16 data_sum;
    /* 0x0C */ u8 ext_name[PFS_FILE_EXT_LEN];
    /* 0x10 */ u8 game_name[PFS_FILE_NAME_LEN];
} __OSDir; // size = 0x20

typedef struct {
    /* 0x00 */ u32 repaired;
    /* 0x04 */ u32 random;
    /* 0x08 */ u64 serialMid;
    /* 0x10 */ u64 serialLow;
    /* 0x18 */ u16 deviceid;
    /* 0x1A */ u8 banks;
    /* 0x1B */ u8 version;
    /* 0x1C */ u16 checksum;
    /* 0x1E */ u16 invertedChecksum;
} __OSPackId; // size = 0x20

typedef struct {
    /* 0x000 */ __OSInode inode;
    /* 0x100 */ u8 bank;
    /* 0x101 */ u8 map[PFS_INODE_DIST_MAP];
} __OSInodeCache; // size = 0x202

#endif
