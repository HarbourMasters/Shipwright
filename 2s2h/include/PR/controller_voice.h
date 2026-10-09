#ifndef PR_CONTROLLER_VOICE_H
#define PR_CONTROLLER_VOICE_H

#include "ultratypes.h"
#include "os_voice.h"
#include "os_message.h"
#include "libc/stddef.h"

typedef struct {
    /* 0x0 */ u8 dummy;
    /* 0x1 */ u8 txsize;
    /* 0x2 */ u8 rxsize;
    /* 0x3 */ u8 cmd;
    /* 0x4 */ u8 addrh;
    /* 0x5 */ u8 addrl;
    /* 0x6 */ u8 data[2];
    /* 0x8 */ u8 datacrc;
} __OSVoiceRead2Format; // size = 0x9

typedef struct {
    /* 0x00 */ u8 dummy;
    /* 0x01 */ u8 txsize;
    /* 0x02 */ u8 rxsize;
    /* 0x03 */ u8 cmd;
    /* 0x04 */ u8 addrh;
    /* 0x05 */ u8 addrl;
    /* 0x06 */ u8 data[36];
    /* 0x2A */ u8 datacrc;
} __OSVoiceRead36Format; // size = 0x2B

typedef struct {
    /* 0x0 */ u8 dummy;
    /* 0x1 */ u8 txsize;
    /* 0x2 */ u8 rxsize;
    /* 0x3 */ u8 cmd;
    /* 0x4 */ u8 addrh;
    /* 0x5 */ u8 addrl;
    /* 0x6 */ u8 data[4];
    /* 0xA */ u8 datacrc;
} __OSVoiceWrite4Format; // size = 0xB

typedef struct {
    /* 0x00 */ u8 dummy;
    /* 0x01 */ u8 txsize;
    /* 0x02 */ u8 rxsize;
    /* 0x03 */ u8 cmd;
    /* 0x04 */ u8 addrh;
    /* 0x05 */ u8 addrl;
    /* 0x06 */ u8 data[20];
    /* 0x1A */ u8 datacrc;
} __OSVoiceWrite20Format; // size = 0x1B

typedef struct {
    /* 0x0 */ u8 txsize;
    /* 0x1 */ u8 rxsize;
    /* 0x2 */ u8 cmd;
    /* 0x3 */ u8 data;
    /* 0x4 */ u8 scrc;
    /* 0x5 */ u8 datacrc;
} __OSVoiceSWriteFormat; // size = 0x6

s32 __osVoiceContRead2(OSMesgQueue* mq, s32 channel, u16 address, u8 dst[2]);
s32 __osVoiceContRead36(OSMesgQueue* mq, s32 channel, u16 address, u8 dst[36]);
s32 __osVoiceContWrite4(OSMesgQueue* mq, s32 channel, u16 address, u8 dst[4]);
s32 __osVoiceContWrite20(OSMesgQueue* mq, s32 channel, u16 address, u8 dst[20]);
s32 __osVoiceCheckResult(OSVoiceHandle* hd, u8* status);
s32 __osVoiceGetStatus(OSMesgQueue* mq, s32 channel, u8* status);
s32 __osVoiceSetADConverter(OSMesgQueue* mq, s32 channel, u8 data);
u8 __osVoiceContDataCrc(u8* data, size_t numBytes);

#endif
