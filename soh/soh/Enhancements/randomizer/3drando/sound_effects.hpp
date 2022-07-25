#pragma once

#include <array>
#include <vector>
#include <stdint.h>

#define SFX_BASE 0x1000001
#define SFX_COUNT 1388
#define SFX_COUNT_TRIMMED (1388 - 233)

namespace SFX {
typedef enum {
    // Movement
    SEQ_WALK,
    SEQ_WALK_LOUD,
    SEQ_JUMP,
    SEQ_LAND,
    SEQ_SLIP,
    SEQ_SLIP_LOOP,
    SEQ_BOUND,
    SEQ_CRAWL,
    SEQ_MOVE_LOOP,
    // Combat
    SEQ_WEAPON_SWING,
    SEQ_WEAPON_HIT,
    SEQ_THROW_LOOP,
    SEQ_PROJECTILE_SHOT,
    SEQ_MAGIC_CHARGE_LOOP,
    SEQ_EXPLOSION,
    // Monsters
    SEQ_MONSTER_CRY_SHORT,
    SEQ_MONSTER_CRY_MEDIUM,
    SEQ_MONSTER_CRY_LONG,
    SEQ_MONSTER_DAMAGED,
    SEQ_MONSTER_DEAD,
    SEQ_MONSTER_ATTACK,
    // Voice
    SEQ_VOICE_SHORT,
    SEQ_VOICE_MEDIUM,
    SEQ_VOICE_LONG,
    // Etc
    SEQ_DOOR_OPEN,
    SEQ_DOOR_CLOSE,
    SEQ_AMBIENCE,
    // Meta
    SEQTYPE_COUNT,
    SEQ_NOCAT = 0xFE,     // For sound effects that doesn't fit into any category
    SEQ_NOSHUFFLE = 0xFF, // For DUMMYs and YOBIs that are either blank or duplicates, and for system sound effects
} SeqType;

typedef struct {
    /// Contains the amount of sound effects in each group, excluding SEQ_NOCAT and SEQ_NOSHUFFLE.
    uint16_t rSeqMaxes[SEQTYPE_COUNT];

    /// Contains the original list of SeqTypes.
    /// Can be used to check which type a sound effect is.
    SeqType rSeqTypesSFX[SFX_COUNT];

    /// Contains all sound effects.
    uint32_t rSFXOverrides_All[SFX_COUNT];

    /// Contains all sound effects excluding SEQ_NOSHUFFLE.
    uint32_t rSFXOverrides_AllTrimmed[SFX_COUNT_TRIMMED];

    /// Contains all sound effects grouped into their SeqTypes.
    /// The size of the second dimension should be at least the amount in the largest group.
    uint32_t rSFXOverrides_Types[SEQTYPE_COUNT][100];
} SFXData;

extern const std::array<SeqType, SFX_COUNT> seqTypesSFX;

const SFXData& GetSFXData();

void InitSFXRandomizer();
void ShuffleSequences(bool shuffleCategorically);
} // namespace SFX