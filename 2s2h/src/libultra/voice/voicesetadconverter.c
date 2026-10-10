/**
 * File: voicesetadconverter.c
 */

#include "ultra64.h"
#include "PR/controller_voice.h"
#include "PR/voiceinternal.h"
#include "PR/controller.h"

#define SWRITEFORMAT(ptr) ((__OSVoiceSWriteFormat*)(ptr))

s32 __osVoiceSetADConverter(OSMesgQueue* mq, s32 channel, u8 data) {
    s32 errorCode;
    s32 i;
    u8* ptr;
    u8 status;
    s32 retryCount = 2;

    __osSiGetAccess();

    do {
        ptr = (u8*)&__osPfsPifRam.ramarray;

        if ((__osContLastPoll != CONT_CMD_SWRITE_VOICE) || (channel != __osPfsLastChannel)) {
            __osContLastPoll = CONT_CMD_SWRITE_VOICE;
            __osPfsLastChannel = channel;

            for (i = 0; i < channel; i++, *ptr++ = 0) {}

            __osPfsPifRam.status = CONT_CMD_EXE;

            SWRITEFORMAT(ptr)->txsize = CONT_CMD_SWRITE_VOICE_TX;
            SWRITEFORMAT(ptr)->rxsize = CONT_CMD_SWRITE_VOICE_RX;
            SWRITEFORMAT(ptr)->cmd = CONT_CMD_SWRITE_VOICE;
            SWRITEFORMAT(ptr)->datacrc = 0;

            ptr[sizeof(__OSVoiceSWriteFormat)] = CONT_CMD_END;
        } else {
            ptr += channel;
        }

        SWRITEFORMAT(ptr)->data = data;
        SWRITEFORMAT(ptr)->scrc = __osContAddressCrc(data << 3);

        __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        __osSiRawStartDma(OS_READ, &__osPfsPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);

        errorCode = CHNL_ERR(*SWRITEFORMAT(ptr));

        if (errorCode == 0) {
            if (SWRITEFORMAT(ptr)->datacrc & 1) {
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
