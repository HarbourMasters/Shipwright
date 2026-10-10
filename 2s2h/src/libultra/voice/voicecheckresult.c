/**
 * File: voicecheckresult.c
 */

#include "PR/controller_voice.h"
#include "PR/controller.h"
#include "PR/os_voice.h"

s32 __osVoiceCheckResult(OSVoiceHandle* hd, u8* status) {
    s32 errorCode = __osVoiceGetStatus(hd->mq, hd->channel, status);
    u8 data[2];

    if (errorCode == 0) {
        if (*status & 1) {
            errorCode = CONT_ERR_VOICE_NO_RESPONSE;
        } else {
            errorCode = __osVoiceContRead2(hd->mq, hd->channel, 0, data);
            if (errorCode == 0) {
                hd->status = data[0] & 7;

                if (data[0] & 0x40) {
                    errorCode = CONT_ERR_VOICE_NO_RESPONSE;
                } else {
                    errorCode = data[1] << 8;
                }
            }
        }
    }
    return errorCode;
}
