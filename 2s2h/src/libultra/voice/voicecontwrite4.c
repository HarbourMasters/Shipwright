/**
 * File: voicecontwrite4.c
 */

#include "ultra64.h"
#include "PR/controller_voice.h"
#include "PR/voiceinternal.h"
#include "PR/controller.h"
#include "macros.h"

#define WRITE4FORMAT(ptr) ((__OSVoiceWrite4Format*)(ptr))

s32 __osVoiceContWrite4(OSMesgQueue* mq, s32 channel, u16 address, u8 dst[4]) {
    s32 errorCode;
    u8 status;
    u8* ptr;
    s32 retryCount = 2;
    s32 i;
    u8 crc;

    __osSiGetAccess();

    do {
        ptr = (u8*)&__osPfsPifRam.ramarray;

        if ((__osContLastPoll != CONT_CMD_WRITE4_VOICE) || (channel != __osPfsLastChannel)) {
            __osContLastPoll = CONT_CMD_WRITE4_VOICE;
            __osPfsLastChannel = channel;

            for (i = 0; i < channel; i++, *ptr++ = 0) {}

            __osPfsPifRam.status = CONT_CMD_EXE;

            WRITE4FORMAT(ptr)->dummy = CONT_CMD_NOP;
            WRITE4FORMAT(ptr)->txsize = CONT_CMD_WRITE4_VOICE_TX;
            WRITE4FORMAT(ptr)->rxsize = CONT_CMD_WRITE4_VOICE_RX;
            WRITE4FORMAT(ptr)->cmd = CONT_CMD_WRITE4_VOICE;
            WRITE4FORMAT(ptr)->datacrc = 0xFF;

            ptr[sizeof(__OSVoiceWrite4Format)] = CONT_CMD_END;
        } else {
            ptr += channel;
        }

        WRITE4FORMAT(ptr)->addrh = address >> 3;
        WRITE4FORMAT(ptr)->addrl = (address << 5) | __osContAddressCrc(address);

        bcopy(dst, WRITE4FORMAT(ptr)->data, ARRAY_COUNT(WRITE4FORMAT(ptr)->data));

        __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
        crc = __osVoiceContDataCrc(dst, ARRAY_COUNT(WRITE4FORMAT(ptr)->data));
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        __osSiRawStartDma(OS_READ, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);

        errorCode = CHNL_ERR(*WRITE4FORMAT(ptr));

        if (errorCode == 0) {
            if (crc != WRITE4FORMAT(ptr)->datacrc) {
                errorCode = __osVoiceGetStatus(mq, channel, &status);
                if (errorCode != 0) {
                    break;
                }

                errorCode = CONT_ERR_CONTRFAIL;
            }
        } else {
            errorCode = CONT_ERR_NO_CONTROLLER;
        }

    } while ((errorCode == CONT_ERR_CONTRFAIL) && (retryCount-- >= 0));

    __osSiRelAccess();

    return errorCode;
}
