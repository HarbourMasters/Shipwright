/**
 * File: voicestartreaddata.c
 *
 * Start voice recognition by the Voice Recognition System
 */

#include "PR/controller_voice.h"
#include "PR/os_voice.h"
#include "PR/controller.h"

s32 osVoiceStartReadData(OSVoiceHandle* hd) {
    s32 errorCode;
    u8 status;
    u8 data[4];

    errorCode = __osVoiceGetStatus(hd->mq, hd->channel, &status);
    if (errorCode != 0) {
        return errorCode;
    }

    if (status & 2) {
        return CONT_ERR_VOICE_NO_RESPONSE;
    }

    if (hd->mode != VOICE_HANDLE_MODE_0) {
        return CONT_ERR_INVALID;
    }

    /**
     * data[0] = 5
     * data[1] = 0
     * data[2] = 0
     * data[3] = 0
     */
    *(u32*)data = 0x5000000;
    errorCode = __osVoiceContWrite4(hd->mq, hd->channel, 0, data);

    if (errorCode == 0) {
        errorCode = __osVoiceCheckResult(hd, &status);

        if (errorCode & 0xFF00) {
            errorCode = CONT_ERR_INVALID;
        } else {
            hd->mode = VOICE_HANDLE_MODE_1;
        }
    }

    return errorCode;
}
