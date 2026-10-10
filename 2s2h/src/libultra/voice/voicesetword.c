/**
 * File: voicesetword.c
 *
 * Registers words to the Voice Recognition System dictionary
 */

#include "ultra64.h"
#include "PR/controller_voice.h"
#include "PR/os_voice.h"
#include "PR/controller.h"
#include "macros.h"

s32 osVoiceSetWord(OSVoiceHandle* hd, u8* word) {
    s32 i;
    s32 k;
    s32 errorCode;
    u8 status;
    u8 data[40];

    errorCode = __osVoiceGetStatus(hd->mq, hd->channel, &status);
    if (errorCode != 0) {
        return errorCode;
    }

    if (status & 2) {
        return CONT_ERR_VOICE_NO_RESPONSE;
    }

    k = 0;

    while (word[k] != 0) {
        k += 2;
    }

    bzero(data, ARRAY_COUNT(data));

    for (i = 0; i < k; i += 2) {
        data[ARRAY_COUNT(data) - 1 - k + i] = word[i];
        data[ARRAY_COUNT(data) - 1 - k + i - 1] = word[i + 1];
    }

    data[ARRAY_COUNT(data) - 1 - i - 5] = 3;

    if (k >= 15) {
        errorCode = __osVoiceContWrite20(hd->mq, hd->channel, 0, &data[0]);
        if (errorCode != 0) {
            return errorCode;
        }
    }

    errorCode = __osVoiceContWrite20(hd->mq, hd->channel, 0, &data[20]);
    if (errorCode != 0) {
        return errorCode;
    }

    errorCode = __osVoiceCheckResult(hd, &status);

    if (errorCode != 0) {
        if (errorCode & 0x100) {
            errorCode = CONT_ERR_VOICE_MEMORY;
        } else if (errorCode & 0x200) {
            errorCode = CONT_ERR_VOICE_WORD;
        } else if (errorCode & 0xFF00) {
            errorCode = CONT_ERR_INVALID;
        }
    }

    return errorCode;
}
