#ifndef PR_OS_INTERNAL_FLASH_H
#define PR_OS_INTERNAL_FLASH_H

#include "os_flash.h"

#define FLASH_BLOCK_SIZE 128

#define FLASH_CMD_REG 0x10000 // Located at 0x08010000 on the Cart

/* set whole chip erase mode */
#define FLASH_CMD_CHIP_ERASE    0x3C000000
/* set sector erase mode */
#define FLASH_CMD_SECTOR_ERASE  0x4B000000
/* do erasure */
#define FLASH_CMD_EXECUTE_ERASE 0x78000000
/* program selected page */
#define FLASH_CMD_PROGRAM_PAGE  0xA5000000
/* set page program mode */
#define FLASH_CMD_PAGE_PROGRAM  0xB4000000
/* set status mode */
#define FLASH_CMD_STATUS        0xD2000000
/* set silicon id mode */
#define FLASH_CMD_ID            0xE1000000
/* set read mode */
#define FLASH_CMD_READ_ARRAY    0xF0000000

#endif
