#include "global.h"
#include "z64voice.h"
#include "libc/string.h"

// internal voice functions
char* func_801A5A1C(s8* words);

typedef struct {
    /* 0x00 */ OSVoiceDictionary* dict;
    /* 0x04 */ s8 mode;
    /* 0x08 */ OSVoiceData* data;
    /* 0x0C */ u16 distance;
    /* 0x0E */ u16 answerNum;
    /* 0x10 */ u16 warning;
    /* 0x12 */ u16 voiceLevel;
    /* 0x14 */ u16 voiceRelLevel;
} OSVoiceContext; // size = 0x18

typedef enum VoiceMode {
    /* 0 */ VOICE_MODE_0,
    /* 1 */ VOICE_MODE_1,
    /* 2 */ VOICE_MODE_2
} VoiceMode;

// BSS
OSVoiceContext sVoiceContext;
OSVoiceHandle gVoiceHandle;
OSVoiceData D_801FD5C8; // Intermediate Voice Data during processsing?
OSVoiceData D_801FD5E8; // Best Match Voice Data?
u8 sVoiceMaskPattern[8];
char D_801FD610[256];

// Data
s8 D_801D8E50[96][3] = {
    "aa", "AA", "ii", "II", "uu", "UU", "ee", "EE", "oo", "OO", "KA", "GA", "KI", "GI", "KU", "GU",
    "KE", "GE", "KO", "GO", "SA", "ZA", "SI", "ZI", "SU", "ZU", "SE", "ZE", "SO", "ZO", "TA", "DA",
    "TI", "DI", "tu", "TU", "DU", "TE", "DE", "TO", "DO", "NA", "NI", "NU", "NE", "NO", "HA", "BA",
    "PA", "HI", "BI", "PI", "HU", "BU", "PU", "HE", "BE", "PE", "HO", "BO", "PO", "MA", "MI", "MU",
    "ME", "MO", "ya", "YA", "yu", "YU", "yo", "YO", "RA", "RI", "RU", "RE", "RO", "wa", "WA", "WI",
    "WE", "WO", "NN", "VU", "ka", "ke", "",   "",   "",   "",   "",   "",   "",   "",   "",   "",
};

s8 D_801D8F70[96][3] = {
    "aa", "AA", "ii", "II", "uu", "UU", "ee", "EE", "oo", "OO", "KA", "GA", "KI", "GI", "KU", "GU",
    "KE", "GE", "KO", "GO", "SA", "ZA", "SI", "ZI", "SU", "ZU", "SE", "ZE", "SO", "ZO", "TA", "DA",
    "TI", "DI", "tu", "TU", "DU", "TE", "DE", "TO", "DO", "NA", "NI", "NU", "NE", "NO", "HA", "BA",
    "PA", "HI", "BI", "PI", "HU", "BU", "PU", "HE", "BE", "PE", "HO", "BO", "PO", "MA", "MI", "  ",
    "MU", "ME", "MO", "ya", "YA", "yu", "YU", "yo", "YO", "RA", "RI", "RU", "RE", "RO", "wa", "WA",
    "WI", "WE", "WO", "NN", "VU", "ka", "ke", "",   "",   "",   "",   "",   "",   "",   "",   "",
};

const char D_801E0EC0[] = "Error %d\n";
const char D_801E0ECC[] = "NAI_VRS:osVoiceClearDictionary %d\n";
const char D_801E0EF0[] = "NAI_VRS:dict error! (%d-%d %s)\n";
const char D_801E0F10[] = "NAI_VRS:Ina_SetVruGain Error!\n";
const char D_801E0F30[] = "NAI_VRS:mask on  %d\n";
const char D_801E0F48[] = "NAI_VRS:mask off %d\n";
const char D_801E0F60[] = "NAI_VRS:answer No.:%d Dist:%d Warn:%04X Level:%5d SN:%5d (Num:%d)\n";
const char D_801E0FA4[] = "NAI_VRS:error !! (ANS_MAX:%d DIST:%d WARNING:%04X LEVEL:%5d SN:%5d)\n";

s32 func_801A51F0(s32 errorCode) {
    switch (errorCode) {
        case CONT_ERR_NO_CONTROLLER:
        case CONT_ERR_CONTRFAIL:
        case CONT_ERR_INVALID:
        case CONT_ERR_DEVICE:
        case CONT_ERR_VOICE_MEMORY:
        case CONT_ERR_VOICE_WORD:
        case CONT_ERR_VOICE_NO_RESPONSE:
            return -1;

        default:
            return 0;
    }
}

s32 func_801A5228(OSVoiceDictionary* dict) {
    return 0;
#if 0
    OSMesgQueue* serialEventQueue;
    s32 errorCode;
    u8 numWords;
    u8 i;

    sVoiceContext.mode = VOICE_MODE_0;
    sVoiceContext.data = NULL;
    sVoiceContext.distance = 1000;
    sVoiceContext.answerNum = 5;
    sVoiceContext.warning = 0;
    sVoiceContext.dict = dict;

    numWords = dict->numWords;

    serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
    errorCode = osVoiceClearDictionary(&gVoiceHandle, numWords);
    PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);

    if (errorCode != 0) {
        return errorCode;
    }

    for (i = 0; i < (((numWords - 1) / 8) + 1); i++) {
        sVoiceMaskPattern[i] = 0;
    }

    for (i = 0; i < numWords; i++) {
        serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
        errorCode = osVoiceSetWord(&gVoiceHandle, (u8*)&dict->words[i]);
        PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);

        if (func_801A51F0(errorCode) != 0) {
            func_801A5A1C((s8*)&dict->words[i]);
        }
    }

    return errorCode;
#endif
}

OSVoiceData* func_801A5390(void) {
    return NULL;
#if 0
    OSVoiceData* voiceData;
    OSMesgQueue* serialEventQueue;

    voiceData = sVoiceContext.data;
    sVoiceContext.data = NULL;

    serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
    osVoiceStartReadData(&gVoiceHandle);
    PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);

    return voiceData;
#endif
}

// Unused
OSVoiceDictionary* AudioVoice_GetVoiceDict(void) {
    return sVoiceContext.dict;
}

void func_801A53E8(u16 distance, u16 answerNum, u16 warning, u16 voiceLevel, u16 voiceRelLevel) {
    sVoiceContext.distance = distance;
    sVoiceContext.answerNum = answerNum;
    sVoiceContext.warning = warning;
    sVoiceContext.voiceLevel = voiceLevel;
    sVoiceContext.voiceRelLevel = voiceRelLevel;
}

// Unused
// Could have a return? or be void return?
void func_801A541C(s32 analog, s32 digital) {
#if 0
    s32 errorCode;
    OSMesgQueue* serialEventQueue;

    if (sVoiceContext.dict != NULL) {
        serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
        errorCode = osVoiceControlGain(&gVoiceHandle, analog, digital);
        PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);

        if (errorCode != 0) {
            func_801A51F0(errorCode);
        }
    }
#endif
}

// Unused
s32 func_801A5488(u8* word) {
    return 0;
#if 0
    s32 errorCode;
    OSMesgQueue* serialEventQueue;

    serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
    errorCode = osVoiceCheckWord(word);
    PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);

    return errorCode;
#endif
}

u8* AudioVoice_GetVoiceMaskPattern(void) {
    return sVoiceMaskPattern;
}

s32 AudioVoice_InitWordImplAlt(u16 wordId) {
    return 0;
#if 0
    s32 errorCode;
    u8 stopReadingData = true;
    u8 numWords;
    u8 i;
    OSMesgQueue* serialEventQueue;

    if (sVoiceContext.dict != NULL) {
        numWords = sVoiceContext.dict->numWords;
    } else {
        numWords = 20;
        stopReadingData = false;
    }

    if (wordId == VOICE_WORD_ID_NONE) {
        for (i = 0; i < numWords; i++) {
            sVoiceMaskPattern[i / 8] |= 1 << (i % 8);
        }
    } else {
        if (sVoiceMaskPattern[wordId / 8] & (1 << (wordId % 8))) {
            stopReadingData = false;
        } else {
            sVoiceMaskPattern[wordId / 8] |= (1 << (wordId % 8));
        }
    }

    if (stopReadingData) {
        serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
        errorCode = osVoiceStopReadData(&gVoiceHandle);
        PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);

        if ((errorCode == 0) || (sVoiceContext.mode == VOICE_MODE_0)) {
            serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
            errorCode = osVoiceMaskDictionary(&gVoiceHandle, sVoiceMaskPattern, ((numWords - 1) / 8) + 1);
            PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);
        }

        sVoiceContext.mode = VOICE_MODE_0;
    }

    return errorCode;
#endif
}

s32 AudioVoice_InitWordImpl(u16 wordId) {
    return 0;
#if 0
    s32 errorCode;
    u8 stopReadingData = true;
    u8 numWords;
    u8 i;
    OSMesgQueue* serialEventQueue;

    if (sVoiceContext.dict != NULL) {
        numWords = sVoiceContext.dict->numWords;
    } else {
        numWords = 20;
        stopReadingData = false;
    }

    if (wordId == VOICE_WORD_ID_NONE) {
        for (i = 0; i < (((numWords - 1) / 8) + 1); i++) {
            sVoiceMaskPattern[i] = 0;
        }
    } else {
        if (!(sVoiceMaskPattern[wordId / 8] & (1 << (wordId % 8)))) {
            stopReadingData = false;
        } else {
            sVoiceMaskPattern[wordId / 8] &= (1 << (wordId % 8)) ^ 0xFF;
        }
    }

    if (stopReadingData) {
        serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
        errorCode = osVoiceStopReadData(&gVoiceHandle);
        PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);

        if ((errorCode == 0) || (sVoiceContext.mode == VOICE_MODE_0)) {
            serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
            errorCode = osVoiceMaskDictionary(&gVoiceHandle, sVoiceMaskPattern, ((numWords - 1) / 8) + 1);
            PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);
        }

        sVoiceContext.mode = VOICE_MODE_0;
    }

    return errorCode;
#endif
}

s32 func_801A5808(void) {
    return 0;
#if 0
    s32 errorCode = 0;
    s32 pad;
    OSMesgQueue* serialEventQueue;

    switch (sVoiceContext.mode) {
        case VOICE_MODE_0:
            serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
            errorCode = osVoiceStartReadData(&gVoiceHandle);
            PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);

            sVoiceContext.mode = VOICE_MODE_1;
            break;

        case VOICE_MODE_1:
            serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
            errorCode = osVoiceGetReadData(&gVoiceHandle, &D_801FD5C8);
            PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);

            if (func_801A51F0(errorCode) == 0) {
                switch (gVoiceHandle.status) {
                    case VOICE_STATUS_READY:
                        sVoiceContext.mode = VOICE_MODE_2;
                        break;

                    case VOICE_STATUS_START:
                    case 2:
                    case VOICE_STATUS_CANCEL:
                    case 4:
                    case VOICE_STATUS_BUSY:
                    case 6:
                        break;

                    case VOICE_STATUS_END:
                        sVoiceContext.mode = VOICE_MODE_2;
                        break;

                    default:
                        break;
                }
            }
            break;

        case VOICE_MODE_2:
            if (((D_801FD5C8.warning & sVoiceContext.warning) == 0) &&
                (sVoiceContext.answerNum >= D_801FD5C8.answerNum) &&
                (sVoiceContext.distance >= D_801FD5C8.distance[0]) &&
                (D_801FD5C8.voiceLevel >= sVoiceContext.voiceLevel) &&
                (D_801FD5C8.voiceRelLevel >= sVoiceContext.voiceRelLevel)) {
                D_801FD5E8 = D_801FD5C8;
                sVoiceContext.data = &D_801FD5E8;
            }

            serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
            osVoiceStopReadData(&gVoiceHandle);
            PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);

            serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
            errorCode = osVoiceStartReadData(&gVoiceHandle);
            PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);

            sVoiceContext.mode = VOICE_MODE_1;
            break;

        default:
            break;
    }

    return func_801A51F0(errorCode);
#endif
}

// Unused
void AudioVoice_ResetData(void) {
    sVoiceContext.dict = NULL;
}

char* func_801A5A1C(s8* words) {
    u8 i;
    u8 j;
    u8 numSyllables = strlen((const char*)words); // technically twice the num of syllables
    u8 syllable[2];

    for (j = 0, i = 0; i < numSyllables; i += 2) {
        syllable[0] = words[i];
        syllable[1] = words[i + 1];

        if (syllable[0] == 0x83) {
            D_801FD610[j++] = D_801D8F70[syllable[1] - 0x40][0];
            D_801FD610[j++] = D_801D8F70[syllable[1] - 0x40][1];
        } else if (syllable[0] == 0x82) {
            D_801FD610[j++] = D_801D8E50[syllable[1] - 0x9F][0];
            D_801FD610[j++] = D_801D8E50[syllable[1] - 0x9F][1];
        } else if ((syllable[0] == 0x81) && (syllable[1] == 0x5B)) {
            D_801FD610[j++] = '-';
            D_801FD610[j++] = '-';
        } else {
            D_801FD610[j++] = ' ';
            D_801FD610[j++] = ' ';
        }
    }

    D_801FD610[i] = '\0';

    return D_801FD610;
}
