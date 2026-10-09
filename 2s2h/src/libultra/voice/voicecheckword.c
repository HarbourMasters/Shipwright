/**
 * File: voicecheckwork.c
 *
 * Checks whether or not a word can be registered in the dictionary
 */
#include "PR/controller.h"

// TODO: SJIS support
s32 osVoiceCheckWord(u8* word) {
    s32 i;
    s32 errorCode = 0;
    u16 curSyllable;
    u16 prevSyllable = 0;

    for (i = 0; word[i] != 0; i += 2) {
        curSyllable = (word[i] << 8) + word[i + 1];
        if (((curSyllable != 0x815B) && (curSyllable < 0x829F)) ||
            ((curSyllable >= 0x82F2) && (curSyllable < 0x8340)) || (curSyllable == 0x837F) || (curSyllable >= 0x8397)) {
            errorCode = CONT_ERR_VOICE_WORD;
        } else if ((i == 0) && ((curSyllable == 0x815B) || (curSyllable == 0x82F1) || (curSyllable == 0x82C1) ||
                                (curSyllable == 0x82EC) || (curSyllable == 0x8393) || (curSyllable == 0x8362) ||
                                (curSyllable == 0x838E) || (curSyllable == 0x8395) || (curSyllable == 0x8396))) {
            errorCode = CONT_ERR_VOICE_WORD;
        } else if (((curSyllable == 0x829F) || (curSyllable == 0x8340)) && (prevSyllable != 0x82D3) &&
                   (prevSyllable != 0x8374) && (prevSyllable != 0x8394)) {
            errorCode = CONT_ERR_VOICE_WORD;
        } else if (((curSyllable == 0x82A1) || (curSyllable == 0x8342)) && (prevSyllable != 0x82A4) &&
                   (prevSyllable != 0x82C4) && (prevSyllable != 0x82C5) && (prevSyllable != 0x82D3) &&
                   (prevSyllable != 0x8345) && (prevSyllable != 0x8365) && (prevSyllable != 0x8366) &&
                   (prevSyllable != 0x8374) && (prevSyllable != 0x8394)) {
            errorCode = CONT_ERR_VOICE_WORD;
        } else if (((curSyllable == 0x82A3) || (curSyllable == 0x8344)) && (prevSyllable != 0x82C6) &&
                   (prevSyllable != 0x82C7) && (prevSyllable != 0x82D3) && (prevSyllable != 0x8367) &&
                   (prevSyllable != 0x8368) && (prevSyllable != 0x8374) && (prevSyllable != 0x8394)) {
            errorCode = CONT_ERR_VOICE_WORD;
        } else if (((curSyllable == 0x82A5) || (curSyllable == 0x8346)) && (prevSyllable != 0x82A4) &&
                   (prevSyllable != 0x82B5) && (prevSyllable != 0x82B6) && (prevSyllable != 0x82BF) &&
                   (prevSyllable != 0x82C0) && (prevSyllable != 0x82C2) && (prevSyllable != 0x82D3) &&
                   (prevSyllable != 0x8345) && (prevSyllable != 0x8356) && (prevSyllable != 0x8357) &&
                   (prevSyllable != 0x8360) && (prevSyllable != 0x8361) && (prevSyllable != 0x8363) &&
                   (prevSyllable != 0x8374) && (prevSyllable != 0x8394)) {
            errorCode = CONT_ERR_VOICE_WORD;
        } else if (((curSyllable == 0x82A7) || (curSyllable == 0x8348)) && (prevSyllable != 0x82A4) &&
                   (prevSyllable != 0x82D3) && (prevSyllable != 0x8345) && (prevSyllable != 0x8374) &&
                   (prevSyllable != 0x8394)) {
            errorCode = CONT_ERR_VOICE_WORD;
        } else if (((curSyllable == 0x82E1) || (curSyllable == 0x82E3) || (curSyllable == 0x82E5) ||
                    (curSyllable == 0x8383) || (curSyllable == 0x8385) || (curSyllable == 0x8387)) &&
                   (prevSyllable != 0x82AB) && (prevSyllable != 0x82B5) && (prevSyllable != 0x82BF) &&
                   (prevSyllable != 0x82C9) && (prevSyllable != 0x82D0) && (prevSyllable != 0x82DD) &&
                   (prevSyllable != 0x82E8) && (prevSyllable != 0x82AC) && (prevSyllable != 0x82B6) &&
                   (prevSyllable != 0x82C0) && (prevSyllable != 0x82D1) && (prevSyllable != 0x82D2) &&
                   (prevSyllable != 0x834C) && (prevSyllable != 0x8356) && (prevSyllable != 0x8360) &&
                   (prevSyllable != 0x836A) && (prevSyllable != 0x8371) && (prevSyllable != 0x837E) &&
                   (prevSyllable != 0x838A) && (prevSyllable != 0x834D) && (prevSyllable != 0x8357) &&
                   (prevSyllable != 0x8361) && (prevSyllable != 0x8372) && (prevSyllable != 0x8373) &&
                   (prevSyllable != 0x8394)) {
            errorCode = CONT_ERR_VOICE_WORD;
        } else if ((curSyllable == 0x815B) && ((prevSyllable == 0x82F1) || (prevSyllable == 0x82C1) ||
                                               (prevSyllable == 0x8393) || (prevSyllable == 0x8362))) {
            errorCode = CONT_ERR_VOICE_WORD;
        } else if (((curSyllable == 0x82F1) || (curSyllable == 0x8393)) &&
                   ((prevSyllable == 0x82F1) || (prevSyllable == 0x8393))) {
            errorCode = CONT_ERR_VOICE_WORD;
        } else if (((prevSyllable == 0x82C1) || (prevSyllable == 0x8362)) &&
                   ((curSyllable == 0x815B) || (curSyllable == 0x82A0) || (curSyllable == 0x82A2) ||
                    (curSyllable == 0x82A4) || (curSyllable == 0x82A6) || (curSyllable == 0x82A8) ||
                    (curSyllable == 0x82C8) || (curSyllable == 0x82C9) || (curSyllable == 0x82CA) ||
                    (curSyllable == 0x82CB) || (curSyllable == 0x82CC) || (curSyllable == 0x82DC) ||
                    (curSyllable == 0x82DD) || (curSyllable == 0x82DE) || (curSyllable == 0x82DF) ||
                    (curSyllable == 0x82E0) || (curSyllable == 0x82E2) || (curSyllable == 0x82E4) ||
                    (curSyllable == 0x82E6) || (curSyllable == 0x82E7) || (curSyllable == 0x82E8) ||
                    (curSyllable == 0x82E9) || (curSyllable == 0x82EA) || (curSyllable == 0x82EB) ||
                    (curSyllable == 0x82ED) || (curSyllable == 0x82EE) || (curSyllable == 0x82EF) ||
                    (curSyllable == 0x82F0) || (curSyllable == 0x82F1) || (curSyllable == 0x829F) ||
                    (curSyllable == 0x82A1) || (curSyllable == 0x82A3) || (curSyllable == 0x82A5) ||
                    (curSyllable == 0x82A7) || (curSyllable == 0x82E1) || (curSyllable == 0x82E3) ||
                    (curSyllable == 0x82E5) || (curSyllable == 0x82C1) || (curSyllable == 0x82EC) ||
                    (curSyllable == 0x8341) || (curSyllable == 0x8343) || (curSyllable == 0x8345) ||
                    (curSyllable == 0x8347) || (curSyllable == 0x8349) || (curSyllable == 0x8369) ||
                    (curSyllable == 0x836A) || (curSyllable == 0x836B) || (curSyllable == 0x836C) ||
                    (curSyllable == 0x836D) || (curSyllable == 0x837D) || (curSyllable == 0x837E) ||
                    (curSyllable == 0x8380) || (curSyllable == 0x8381) || (curSyllable == 0x8382) ||
                    (curSyllable == 0x8384) || (curSyllable == 0x8386) || (curSyllable == 0x8388) ||
                    (curSyllable == 0x8389) || (curSyllable == 0x838A) || (curSyllable == 0x838B) ||
                    (curSyllable == 0x838C) || (curSyllable == 0x838D) || (curSyllable == 0x838F) ||
                    (curSyllable == 0x8390) || (curSyllable == 0x8391) || (curSyllable == 0x8392) ||
                    (curSyllable == 0x8393) || (curSyllable == 0x8340) || (curSyllable == 0x8342) ||
                    (curSyllable == 0x8344) || (curSyllable == 0x8346) || (curSyllable == 0x8348) ||
                    (curSyllable == 0x8383) || (curSyllable == 0x8385) || (curSyllable == 0x8387) ||
                    (curSyllable == 0x8362) || (curSyllable == 0x838E))) {
            errorCode = CONT_ERR_VOICE_WORD;
        }

        if (errorCode != 0) {
            return errorCode;
        }

        prevSyllable = curSyllable;
    }

    if ((curSyllable == 0x82C1) || (curSyllable == 0x8362)) {
        errorCode = CONT_ERR_VOICE_WORD;
    }

    return errorCode;
}
