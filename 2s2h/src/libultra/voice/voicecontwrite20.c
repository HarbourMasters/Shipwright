/**
 * File: voicecontwrite20.c
 */

#include "ultra64.h"
#include "PR/controller_voice.h"
#include "PR/voiceinternal.h"
#include "PR/controller.h"
#include "macros.h"

#define WRITE20FORMAT(ptr) ((__OSVoiceWrite20Format*)(ptr))

s32 __osVoiceContWrite20(OSMesgQueue* mq, s32 channel, u16 address, u8 dst[20]) {
    s32 errorCode;
    u8 status;
    u8* ptr;
    s32 retryCount = 2;
    s32 i;
    u8 crc;

    __osSiGetAccess();

    do {
        ptr = (u8*)&__osPfsPifRam.ramarray;

        if ((__osContLastPoll != CONT_CMD_WRITE20_VOICE) || (channel != __osPfsLastChannel)) {
            __osContLastPoll = CONT_CMD_WRITE20_VOICE;
            __osPfsLastChannel = channel;

            for (i = 0; i < channel; i++, *ptr++ = 0) {}

            __osPfsPifRam.status = CONT_CMD_EXE;

            WRITE20FORMAT(ptr)->dummy = CONT_CMD_NOP;
            WRITE20FORMAT(ptr)->txsize = CONT_CMD_WRITE20_VOICE_TX;
            WRITE20FORMAT(ptr)->rxsize = CONT_CMD_WRITE20_VOICE_RX;
            WRITE20FORMAT(ptr)->cmd = CONT_CMD_WRITE20_VOICE;
            WRITE20FORMAT(ptr)->datacrc = 0xFF;

            ptr[sizeof(__OSVoiceWrite20Format)] = CONT_CMD_END;
        } else {
            ptr += channel;
        }

        WRITE20FORMAT(ptr)->addrh = address >> 3;
        WRITE20FORMAT(ptr)->addrl = (address << 5) | __osContAddressCrc(address);

        bcopy(dst, WRITE20FORMAT(ptr)->data, ARRAY_COUNT(WRITE20FORMAT(ptr)->data));

        __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
        crc = __osVoiceContDataCrc(dst, ARRAY_COUNT(WRITE20FORMAT(ptr)->data));
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        __osSiRawStartDma(OS_READ, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);

        errorCode = CHNL_ERR(*WRITE20FORMAT(ptr));

        if (errorCode == 0) {
            if (crc != WRITE20FORMAT(ptr)->datacrc) {
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
