#pragma once
#include "stdint.h"

void InitSfxEditor();
#ifndef __cplusplus
void SfxEditor_AddSequence(char *otrPath, uint16_t seqNum);
#endif

#define INSTRUMENT_OFFSET 0x81

enum SeqType {
    SEQ_NOSHUFFLE  = 0,
    SEQ_BGM_WORLD  = 1 << 0,
    SEQ_BGM_EVENT  = 1 << 1,
    SEQ_BGM_BATTLE = 1 << 2,
    SEQ_OCARINA    = 1 << 3,
    SEQ_FANFARE    = 1 << 4,
    SEQ_BGM_ERROR  = 1 << 5,
    SEQ_SFX        = 1 << 6,
    SEQ_INSTRUMENT = 1 << 7,
    SEQ_BGM_CUSTOM = SEQ_BGM_WORLD | SEQ_BGM_EVENT | SEQ_BGM_BATTLE,
};