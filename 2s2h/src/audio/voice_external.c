#include "z64voice.h"
#include "padmgr.h"

OSVoiceDictionary sVoiceDictionary = {
    {
        // "アトナンジカン" - "atonanjikan" -  "How many hours"
        { 0x8341, 0x8367, 0x8369, 0x8393, 0x8357, 0x834A, 0x8393 },

        // "ハイチーズ" - "haichīzu" - "say cheese"
        { 0x836E, 0x8343, 0x8360, 0x815B, 0x8359 },

        // "オキロー" - "okirō" - "wake up"
        { 0x8349, 0x834C, 0x838D, 0x815B },

        // "オスワリ" - "osuwari" - "sit"
        { 0x8349, 0x8358, 0x838F, 0x838A },

        // "ミルク" - "miruku" - "milk"
        { 0x837E, 0x838B, 0x834E },

        // "ハイヤー" - "haiyā" - "hiya!"
        { 0x836E, 0x8343, 0x8384, 0x815B },
    },

    VOICE_WORD_ID_MAX, // number of words
};

u8 D_801D8E3C = 0;
OSVoiceData* sVoiceData = NULL;
u16 sTopScoreWordId = VOICE_WORD_ID_NONE;
u8 D_801D8E48 = 0;

// Relation to voice initialization
void AudioVoice_Noop(void) {
}

void func_801A4EB8(void) {
#if 0
    u8* voiceMaskPattern;
    OSMesgQueue* serialEventQueue;
    s32 index;
    u8 sp38[1];
    u8 i;

    if (D_801D8E3C != 0) {
        serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
        osVoiceStopReadData(&gVoiceHandle);
        PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);
    }

    voiceMaskPattern = AudioVoice_GetVoiceMaskPattern();

    for (i = 0; i < 1; i++) {
        sp38[i] = voiceMaskPattern[i];
    }

    if (func_801A5228(&sVoiceDictionary) == 0) {
        for (i = 0; i < VOICE_WORD_ID_MAX; i++) {
            index = i / 8;
            if (((sp38[index] >> (i % 8)) & 1) == 1) {
                AudioVoice_InitWordImplAlt(i);
            }
        }

        func_801A53E8(800, 2, VOICE_WARN_TOO_SMALL, 500, 2000);
        D_801D8E3C = 1;
    }
#endif
}

// Used externally in code_8019AF00
void AudioVoice_ResetWord(void) {
#if 0
    s32 errorCode;
    OSMesgQueue* serialEventQueue;

    AudioVoice_InitWordImplAlt(VOICE_WORD_ID_NONE);

    if (D_801D8E3C != 0) {
        serialEventQueue = PadMgr_VoiceAcquireSerialEventQueue();
        osVoiceStopReadData(&gVoiceHandle);
        PadMgr_VoiceReleaseSerialEventQueue(serialEventQueue);

        errorCode = func_801A5228(&sVoiceDictionary);
        AudioVoice_InitWordImplAlt(VOICE_WORD_ID_NONE);
        if (errorCode == 0) {
            func_801A53E8(800, 2, VOICE_WARN_TOO_SMALL, 500, 2000);
            D_801D8E3C = 1;
        }

        AudioVoice_InitWord(VOICE_WORD_ID_HIYA);
        AudioVoice_InitWord(VOICE_WORD_ID_CHEESE);
    }
#endif
}

void AudioVoice_InitWord(u16 wordId) {
    if ((D_801D8E3C != 0) && (wordId < VOICE_WORD_ID_MAX)) {
        AudioVoice_InitWordImpl(wordId);
    }
}

// Unused
void AudioVoice_InitWordAlt(u16 wordId) {
    if ((D_801D8E3C != 0) && (wordId < VOICE_WORD_ID_MAX)) {
        AudioVoice_InitWordImplAlt(wordId);
    }
}

// Used externally in many files
u16 AudioVoice_GetWord(void) {
    return sTopScoreWordId;
}

// Unused
u8 func_801A510C(void) {
    return D_801D8E3C;
}

// Used externally in Audio_Update (code_8019AF00)
void AudioVoice_Update(void) {
    if (D_801D8E3C & 2) {
        D_801D8E3C &= 1;
        func_801A4EB8();
    }

    if (D_801D8E3C != 0) {
        if (func_801A5808() != 0) {
            D_801D8E48++;
            if (D_801D8E48 == 10) {
                D_801D8E3C = 0;
                sTopScoreWordId = VOICE_WORD_ID_NONE;
                return;
            }
        } else {
            D_801D8E48 = 0;
        }

        sVoiceData = func_801A5390();
        if (sVoiceData != 0) {
            sTopScoreWordId = sVoiceData->answer[0];
        } else {
            sTopScoreWordId = VOICE_WORD_ID_NONE;
        }
    }
}
