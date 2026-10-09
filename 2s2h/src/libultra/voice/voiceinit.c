/**
 * File: voiceinit.c
 *
 * Initializes Voice Recognition System control structure and hardware
 */

#include "ultra64.h"
#include "PR/controller_voice.h"
#include "PR/os_voice.h"
#include "PR/controller.h"
#include "macros.h"

static u8 sCmds[] = {
    0x1E, 0x6E, 0x08, 0x56, 0x03,
};

s32 osVoiceInit(OSMesgQueue* mq, OSVoiceHandle* hd, int channel) {
    s32 errorCode;
    s32 i;
    u8 status = 0;
    u8 data[4];

    hd->channel = channel;
    hd->mq = mq;
    hd->mode = VOICE_HANDLE_MODE_0;

    errorCode = __osVoiceGetStatus(mq, channel, &status);
    if (errorCode != 0) {
        return errorCode;
    }

    if (__osContChannelReset(mq, channel) != 0) {
        return CONT_ERR_CONTRFAIL;
    }

    for (i = 0; i < ARRAY_COUNT(sCmds); i++) {
        errorCode = __osVoiceSetADConverter(mq, channel, sCmds[i]);
        if (errorCode != 0) {
            return errorCode;
        }
    }

    errorCode = __osVoiceGetStatus(mq, channel, &status);
    if (errorCode != 0) {
        return errorCode;
    }
    if (status & 2) {
        return CONT_ERR_VOICE_NO_RESPONSE;
    }

    /**
     * data[0] = 0
     * data[1] = 0
     * data[2] = 1
     * data[3] = 0
     */
    *(u32*)data = 0x100;
    errorCode = __osVoiceContWrite4(mq, channel, 0, data);
    if (errorCode != 0) {
        return errorCode;
    }

    errorCode = __osVoiceCheckResult(hd, &status);
    if (errorCode & 0xFF00) {
        errorCode = CONT_ERR_INVALID;
    }

    return errorCode;
}
