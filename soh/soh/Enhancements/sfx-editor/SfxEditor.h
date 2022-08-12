#pragma once

void InitSfxEditor();

enum SeqType {
    SEQ_NOSHUFFLE  = 0,
    SEQ_BGM_WORLD  = 1 << 0,
    SEQ_BGM_EVENT  = 1 << 1,
    SEQ_BGM_BATTLE = 1 << 2,
    SEQ_OCARINA    = 1 << 3,
    SEQ_FANFARE    = 1 << 4,
    SEQ_BGM_ERROR  = 1 << 5,
};