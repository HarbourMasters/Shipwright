/**
 * File: voicecontread2.c
 */

#include "ultra64.h"
#include "PR/controller_voice.h"
#include "PR/voiceinternal.h"
#include "PR/controller.h"
#include "macros.h"

#define READ2FORMAT(ptr) ((__OSVoiceRead2Format*)(ptr))

s32 __osVoiceContRead2(OSMesgQueue* mq, s32 channel, u16 address, u8 dst[2]) {
    s32 errorCode;
    u8 status;
    u8* ptr;
    s32 retryCount = 2;
    s32 i;

    __osSiGetAccess();

    do {
        ptr = (u8*)&__osPfsPifRam.ramarray;

        if ((__osContLastPoll != CONT_CMD_READ2_VOICE) || (channel != __osPfsLastChannel)) {
            __osContLastPoll = CONT_CMD_READ2_VOICE;
            __osPfsLastChannel = channel;

            for (i = 0; i < channel; i++, *ptr++ = 0) {}

            __osPfsPifRam.status = CONT_CMD_EXE;

            READ2FORMAT(ptr)->dummy = CONT_CMD_NOP;
            READ2FORMAT(ptr)->txsize = CONT_CMD_READ2_VOICE_TX;
            READ2FORMAT(ptr)->rxsize = CONT_CMD_READ2_VOICE_RX;
            READ2FORMAT(ptr)->cmd = CONT_CMD_READ2_VOICE;
            READ2FORMAT(ptr)->datacrc = 0xFF;

            ptr[sizeof(__OSVoiceRead2Format)] = CONT_CMD_END;
        } else {
            ptr += channel;
        }

        READ2FORMAT(ptr)->addrh = address >> 3;
        READ2FORMAT(ptr)->addrl = (address << 5) | __osContAddressCrc(address);

        __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        __osSiRawStartDma(OS_READ, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);

        errorCode = CHNL_ERR(*READ2FORMAT(ptr));

        if (errorCode == 0) {
            if (__osVoiceContDataCrc(READ2FORMAT(ptr)->data, ARRAY_COUNT(READ2FORMAT(ptr)->data)) !=
                READ2FORMAT(ptr)->datacrc) {
                errorCode = __osVoiceGetStatus(mq, channel, &status);
                if (errorCode != 0) {
                    break;
                }

                errorCode = CONT_ERR_CONTRFAIL;
            } else {
                bcopy(READ2FORMAT(ptr)->data, dst, ARRAY_COUNT(READ2FORMAT(ptr)->data));
            }
        } else {
            errorCode = CONT_ERR_NO_CONTROLLER;
        }

    } while ((errorCode == CONT_ERR_CONTRFAIL) && (retryCount-- >= 0));

    __osSiRelAccess();

    return errorCode;
}
