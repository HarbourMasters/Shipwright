/**
 * File: voicecontrolgain.c
 *
 * Adjusts the input gain of the Voice Recognition System
 */

#include "PR/controller_voice.h"
#include "PR/os_voice.h"
#include "PR/controller.h"
#include "macros.h"

u8 sDigitalTable[] = { 0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0 };

s32 osVoiceControlGain(OSVoiceHandle* hd, s32 analog, s32 digital) {
    s32 errorCode;
    u8 cmd;

    if (analog == 0) {
        cmd = 0x18;
    } else {
        cmd = 0x98;
    }

    errorCode = __osVoiceSetADConverter(hd->mq, hd->channel, cmd);
    if (errorCode != 0) {
        return errorCode;
    }

    if ((digital < ARRAY_COUNT(sDigitalTable)) && (digital >= 0)) {
        cmd = sDigitalTable[digital] + 2;
    } else {
        return CONT_ERR_INVALID;
    }

    errorCode = __osVoiceSetADConverter(hd->mq, hd->channel, cmd);
    if (errorCode != 0) {
        return errorCode;
    }

    return errorCode;
}
