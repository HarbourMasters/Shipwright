#pragma once

#include <string>
#include <vector>

#include "randomizerTypes.h"

namespace Rando {

enum class NpcSoulCategory {
    MERCHANTS,
    MINIGAMES,
    GATEKEEPERS,
    STORY,
    TOWNSFOLK,
};

struct NpcSoul {
    RandomizerGet rg;
    RandomizerInf randInf;
    RandomizerHintTextKey hintKey;
    RandomizerSettingKey startingSetting;
    NpcSoulCategory category;
    const char* article; // "" or "the "
    const char* name;    // "Talon", "Frogs"
    const char* tracker; // short item tracker label, "TALON"
    bool advancement;    // false = red herring that gates nothing
};

// Every NPC soul, in enum order.
const std::vector<NpcSoul>& GetNpcSouls();

// The soul entry for an RG, or nullptr if the RG isn't an NPC soul.
const NpcSoul* GetNpcSoul(RandomizerGet rg);

bool IsNpcSoul(RandomizerGet rg);

// The RSK_SHUFFLE_NPC_SOULS_* setting that controls a category.
RandomizerSettingKey GetNpcSoulCategorySetting(NpcSoulCategory category);

// "Talon's Soul", "Soul of the Zoras". `noun` replaces "Soul" (ice trap names: "Talon's Sole").
std::string GetNpcSoulItemName(const NpcSoul& soul, const char* noun = "Soul");

// "the soul of Talon", "the soul of the Frogs"
std::string GetNpcSoulHintName(const NpcSoul& soul);

// True when the seed's settings put this soul in the item pool (its category is on and
// the player doesn't start with it). A soul that isn't shuffled is owned from the start.
bool IsNpcSoulShuffled(const NpcSoul& soul);

// True when any NPC soul category is shuffled in the current seed.
bool AreNpcSoulsEnabled();

// "Sages hold dungeon rewards": the sage who hands over a dungeon reward, and the reward location.
struct SageReward {
    RandomizerCheck rc;
    RandomizerGet soul;
};
const std::vector<SageReward>& GetSageRewards();

// The sage's soul for a reward location, or RG_NONE when no sage holds that reward in this seed
// (the switch is off, or the location isn't a sage's).
RandomizerGet GetSageRewardSoul(RandomizerCheck rc);

} // namespace Rando
