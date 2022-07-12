#pragma once

#include <array>
#include <vector>
#include <stdint.h>

namespace Music {
    const uint32_t BGM_BASE  = 0x1000585;
    const int SEQ_COUNT = 85;

    enum SeqType {
        SEQ_NOSHUFFLE  = 0,
        SEQ_BGM_WORLD  = 1 << 0,
        SEQ_BGM_EVENT  = 1 << 1,
        SEQ_BGM_BATTLE = 1 << 2,
        SEQ_OCARINA    = 1 << 3,
        SEQ_FANFARE    = 1 << 4,
        // A soundtrack in this category has the issue where if another soundtrack that isn't
        // in this category overrides it, it will keep playing when it should be stopped.
        // For example when beating a mini-boss or finishing the zora diving game.
        SEQ_BGM_ERROR  = 1 << 5,
    };

    extern const std::array<SeqType, SEQ_COUNT> seqTypesMusic;
    extern std::array<uint32_t, SEQ_COUNT> seqOverridesMusic;

    void InitMusicRandomizer();
    void ShuffleSequences(int type);
} // namespace Music
