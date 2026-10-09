/**
 * File: voicemaskdictionary.c
 *
 * Mask words registered in the voice recognition system
 */

#include "ultra64.h"
#include "PR/controller_voice.h"
#include "PR/os_voice.h"
#include "PR/controller.h"
#include "macros.h"

s32 osVoiceMaskDictionary(OSVoiceHandle* hd, u8* maskPattern, int size) {
    s32 errorCode;
    s32 i;
    s32 j;
    u8 status;
    u8 data[20];

    errorCode = __osVoiceGetStatus(hd->mq, hd->channel, &status);
    if (errorCode != 0) {
        return errorCode;
    }

    if (status & 2) {
        return CONT_ERR_VOICE_NO_RESPONSE;
    }

    if (size & 1) {
        j = size + 1;
    } else {
        j = size;
    }

    bzero(&data, ARRAY_COUNT(data));

    data[ARRAY_COUNT(data) - 2 - j] = 4;

    for (i = 0; i < j; i += 2) {
        data[i + ARRAY_COUNT(data) - j] = maskPattern[i];
        data[i + ARRAY_COUNT(data) - j + 1] = maskPattern[i + 1];
    }

    if (size & 1) {
        data[ARRAY_COUNT(data) - 1] = 0;
    }

    errorCode = __osVoiceContWrite20(hd->mq, hd->channel, 0, data);
    if (errorCode == 0) {
        errorCode = __osVoiceCheckResult(hd, &status);
        if (errorCode & 0xFF00) {
            errorCode = CONT_ERR_INVALID;
        }
    }

    return errorCode;
}
