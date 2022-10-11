#pragma once

#ifndef ULTRA64_CONTROLLER_H
#define ULTRA64_CONTROLLER_H

#include "stdint.h"

#define SIAccessQueueSize 2
#define BLOCKSIZE 32
#define MAXCONTROLLERS  4
#define PFS_ONE_PAGE 8

#define PFS_PAGE_SIZE (BLOCKSIZE*PFS_ONE_PAGE)

#define CONT_CMD_REQUEST_STATUS 0
#define CONT_CMD_READ_BUTTON 1
#define CONT_CMD_READ_MEMPACK 2
#define CONT_CMD_WRITE_MEMPACK 3
#define CONT_CMD_READ_EEPROM 4
#define CONT_CMD_WRITE_EEPROM 5
#define CONT_CMD_RESET 0xFF

#define CONT_CMD_REQUEST_STATUS_TX 1
#define CONT_CMD_READ_BUTTON_TX 1
#define CONT_CMD_READ_MEMPACK_TX 3
#define CONT_CMD_WRITE_MEMPACK_TX 35
#define CONT_CMD_READ_EEPROM_TX 2
#define CONT_CMD_WRITE_EEPROM_TX 10
#define CONT_CMD_RESET_TX 1

#define CONT_CMD_REQUEST_STATUS_RX 3
#define CONT_CMD_READ_BUTTON_RX 4
#define CONT_CMD_READ_MEMPACK_RX 33
#define CONT_CMD_WRITE_MEMPACK_RX 1
#define CONT_CMD_READ_EEPROM_RX 8
#define CONT_CMD_WRITE_EEPROM_RX 1
#define CONT_CMD_RESET_RX 3

#define CONT_CMD_NOP 0xFF
#define CONT_CMD_END 0xFE // Indicates end of a command
#define CONT_CMD_EXE 1    // Set pif ram status byte to this to do a command

#define CONT_ERR_NO_CONTROLLER      PFS_ERR_NOPACK      /* 1 */
#define CONT_ERR_CONTRFAIL          CONT_OVERRUN_ERROR  /* 4 */
#define CONT_ERR_INVALID            PFS_ERR_INVALID     /* 5 */
#define CONT_ERR_DEVICE             PFS_ERR_DEVICE      /* 11 */
#define CONT_ERR_NOT_READY          12
#define CONT_ERR_VOICE_MEMORY       13
#define CONT_ERR_VOICE_WORD         14
#define CONT_ERR_VOICE_NO_RESPONSE  15


#define DIR_STATUS_EMPTY 0
#define DIR_STATUS_UNKNOWN 1
#define DIR_STATUS_OCCUPIED 2

#define PFS_FORCE 1
#define PFS_DELETE 1

#define PFS_LABEL_AREA 7

#define PFS_ERR_NOPACK 1

/* controller errors */
#define CONT_NO_RESPONSE_ERROR          0x8
#define CONT_OVERRUN_ERROR              0x4

/* Controller type */
#define CONT_ABSOLUTE           0x0001
#define CONT_RELATIVE           0x0002
#define CONT_JOYPORT            0x0004
#define CONT_EEPROM             0x8000
#define CONT_EEP16K             0x4000
#define CONT_TYPE_MASK          0x1F07
#define CONT_TYPE_NORMAL        0x0005
#define CONT_TYPE_MOUSE         0x0002
#define CONT_TYPE_VOICE         0x0100

/* Controller status */
#define CONT_CARD_ON            0x01
#define CONT_CARD_PULL          0x02
#define CONT_ADDR_CRC_ER        0x04
#define CONT_EEPROM_BUSY        0x80

/* Buttons */
#define BTN_CRIGHT      0x00001
#define BTN_CLEFT       0x00002
#define BTN_CDOWN       0x00004
#define BTN_CUP         0x00008
#define BTN_R           0x00010
#define BTN_L           0x00020
#define BTN_MODIFIER1   0x00040
#define BTN_MODIFIER2   0x00080
#define BTN_DRIGHT      0x00100
#define BTN_DLEFT       0x00200
#define BTN_DDOWN       0x00400
#define BTN_DUP         0x00800
#define BTN_START       0x01000
#define BTN_Z           0x02000
#define BTN_B           0x04000
#define BTN_A           0x08000
#define BTN_STICKLEFT   0x10000
#define BTN_STICKRIGHT  0x20000
#define BTN_STICKDOWN   0x40000
#define BTN_STICKUP     0x80000
#define BTN_VSTICKUP    0x100000
#define BTN_VSTICKDOWN  0x200000
#define BTN_VSTICKLEFT  0x400000
#define BTN_VSTICKRIGHT 0x800000

typedef struct {
    /* 0x00 */ int32_t ram[15];
    /* 0x3C */ int32_t status;
} OSPifRam; // size = 0x40

typedef struct {
    /* 0x00 */ uint16_t type;
    /* 0x02 */ uint8_t status;
    /* 0x03 */ uint8_t err_no;
} OSContStatus; // size = 0x04

typedef struct {
    /* 0x00 */ uint16_t button;
    /* 0x02 */ int8_t stick_x;
    /* 0x03 */ int8_t stick_y;
    /* 0x04 */ uint8_t err_no;
    /* 0x05 */ float gyro_x;
    /* 0x09 */ float gyro_y;
    /* 0x1C */ int8_t right_stick_x;
    /* 0x20 */ int8_t right_stick_y;
} OSContPad; // size = 0x24

typedef struct {
    /* 0x00 */ uint8_t rumble;
    /* 0x01 */ uint8_t ledColor;
} ControllerCallback; // size = 0x02

typedef struct {
    /* 0x00 */ void* address;
    /* 0x04 */ uint8_t databuffer[32];
    /* 0x24 */ uint8_t addressCrc;
    /* 0x25 */ uint8_t dataCrc;
    /* 0x26 */ uint8_t err_no;
} OSContRamIo; // size = 0x28

// Original name: __OSContRequesFormat
typedef struct {
    /* 0x00 */ uint8_t align;
    /* 0x01 */ uint8_t txsize;
    /* 0x02 */ uint8_t rxsize;
    /* 0x03 */ uint8_t poll;
    /* 0x04 */ uint8_t typeh;
    /* 0x05 */ uint8_t typel;
    /* 0x06 */ uint8_t status;
    /* 0x07 */ uint8_t align1;
} __OSContRequestHeader; // size = 0x8

// Original name: __OSContRequesHeaderFormatShort
typedef struct {
    /* 0x00 */ uint8_t txsize;
    /* 0x01 */ uint8_t rxsize;
    /* 0x02 */ uint8_t poll;
    /* 0x03 */ uint8_t typeh;
    /* 0x04 */ uint8_t typel;
    /* 0x05 */ uint8_t status;
} __OSContRequestHeaderAligned; // size = 0x6

// Original Name: __OSContRamReadFormat
typedef struct {
    /* 0x00 */ uint8_t unk_00;
    /* 0x01 */ uint8_t txsize;
    /* 0x02 */ uint8_t rxsize;
    /* 0x03 */ uint8_t poll;
    /* 0x04 */ uint8_t hi;
    /* 0x05 */ uint8_t lo;
    /* 0x06 */ uint8_t data[BLOCKSIZE];
    /* 0x26 */ uint8_t datacrc;
} __OSContRamHeader; // size = 0x27

// Original name: __OSContReadFormat
typedef struct {
    /* 0x00 */ uint8_t align;
    /* 0x01 */ uint8_t txsize;
    /* 0x02 */ uint8_t rxsize;
    /* 0x03 */ uint8_t poll;
    /* 0x04 */ uint16_t button;
    /* 0x06 */ int8_t joyX;
    /* 0x07 */ int8_t joyY;
} __OSContReadHeader; // size = 0x8

#endif
