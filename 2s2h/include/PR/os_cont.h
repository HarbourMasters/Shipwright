#ifndef PR_OS_CONT_H
#define PR_OS_CONT_H
#include <libultraship/libultra/controller.h>
#if 0
#include "ultratypes.h"
#include "os_message.h"


typedef struct {
    /* 0x0 */ u16 type;
    /* 0x2 */ u8 status;
    /* 0x3 */ u8 err_no;
} OSContStatus; // size = 0x4

typedef struct {
    /* 0x0 */ u16 button;
    /* 0x2 */ s8 stick_x;
    /* 0x3 */ s8 stick_y;
    /* 0x4 */ u8 err_no;
} OSContPad; // size = 0x6

#define MAXCONTROLLERS 4

/* controller errors */
#define CONT_NO_RESPONSE_ERROR 0x8
#define CONT_OVERRUN_ERROR 0x4

/* Controller type */
#define CONT_ABSOLUTE 0x0001
#define CONT_RELATIVE 0x0002
#define CONT_JOYPORT 0x0004
#define CONT_EEPROM 0x8000
#define CONT_EEP16K 0x4000
#define CONT_TYPE_MASK 0x1F07
#define CONT_TYPE_NORMAL 0x0005
#define CONT_TYPE_MOUSE 0x0002
#define CONT_TYPE_VOICE 0x0100

/* Controller status */
#define CONT_CARD_ON 0x01
#define CONT_CARD_PULL 0x02
#define CONT_ADDR_CRC_ER 0x04
#define CONT_EEPROM_BUSY 0x80

#define R_CBUTTONS      0x0001
#define L_CBUTTONS      0x0002
#define D_CBUTTONS      0x0004
#define U_CBUTTONS      0x0008
#define R_TRIG          0x0010
#define L_TRIG          0x0020
#define R_JPAD          0x0100
#define L_JPAD          0x0200
#define D_JPAD          0x0400
#define U_JPAD          0x0800
#define START_BUTTON    0x1000
#define Z_TRIG          0x2000
#define B_BUTTON        0x4000
#define A_BUTTON        0x8000

#define CONT_ERR_NO_CONTROLLER      PFS_ERR_NOPACK      /* 1 */
#define CONT_ERR_CONTRFAIL          CONT_OVERRUN_ERROR  /* 4 */
#define CONT_ERR_INVALID            PFS_ERR_INVALID     /* 5 */
#define CONT_ERR_DEVICE             PFS_ERR_DEVICE      /* 11 */
#define CONT_ERR_NOT_READY          12
#define CONT_ERR_VOICE_MEMORY       13
#define CONT_ERR_VOICE_WORD         14
#define CONT_ERR_VOICE_NO_RESPONSE  15


s32 osContInit(OSMesgQueue* mq, u8* bitpattern, OSContStatus* data);
s32 osContStartQuery(OSMesgQueue* mq);
s32 osContStartReadData(OSMesgQueue* mq);
s32 osContSetCh(u8 ch);
void osContGetQuery(OSContStatus* data);
void osContGetReadData(OSContPad* data);
#endif

#endif
