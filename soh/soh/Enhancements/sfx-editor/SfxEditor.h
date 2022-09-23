#pragma once

void InitSfxEditor();

enum SeqType {
    SEQ_NOSHUFFLE = 0,
    SEQ_BGM_WORLD = 1 << 0,
    SEQ_BGM_EVENT = 1 << 1,
    SEQ_BGM_BATTLE = 1 << 2,
    SEQ_OCARINA = 1 << 3,
    SEQ_FANFARE = 1 << 4,
    SEQ_BGM_ERROR = 1 << 5,
    SFX_SOUND = 1 << 6,
    SFX_SOUND_PL = 1 << 7,
    SFX_SOUND_ITEM = 1 << 8,
    SFX_SOUND_ENV = 1 << 9, 
    SFX_SOUND_EN = 1 << 10,
    SFX_SOUND_SY = 1 << 11,
    SFX_SOUND_V = 1 << 12, 
    SFX_OCARINA = 1 << 13,
};