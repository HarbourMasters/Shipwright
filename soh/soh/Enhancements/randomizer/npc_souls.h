#pragma once

#include <string>
#include <vector>

#include "randomizerTypes.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/custom-message/text.h"

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
    const char* german;  // item name with @ for the noun: "Talons @", "@ der Zoras"
    const char* french;  // follows the noun: "de Talon", "des Zoras"
};

// Every NPC soul, in enum order.
const std::vector<NpcSoul>& GetNpcSouls();

// The soul entry for an RG, or nullptr if the RG isn't an NPC soul.
const NpcSoul* GetNpcSoul(RandomizerGet rg);

bool IsNpcSoul(RandomizerGet rg);

// True when the Shuffle NPC Souls setting puts this category's souls in the pool:
// Shops & Games covers Merchants and Minigames, Shops, Games & Story adds Story, All adds
// Gatekeepers and Townsfolk.
bool IsNpcSoulCategoryShuffled(NpcSoulCategory category, uint8_t shuffleNpcSouls);
bool IsNpcSoulCategoryShuffled(NpcSoulCategory category);

// True when sages hold their dungeon rewards: the switch is on and story souls are shuffled.
bool DoSagesHoldRewards();

// "Talon's Soul", "Soul of the Zoras". `noun` replaces "Soul" (ice trap names: "Talon's Sole").
std::string GetNpcSoulItemName(const NpcSoul& soul, const char* noun = "Soul");

// The item name in every language: "Talon's Soul", "Talons Seele", "Âme de Talon". The nouns replace
// "Soul", "Seele" and "Âme" (ice trap names: "Talon's Sole", "Talons Sohle", "Lame de Talon").
Text GetNpcSoulItemText(const NpcSoul& soul, const char* english = "Soul", const char* german = "Seele",
                        const char* french = "Âme");

// Hint text: "the soul of Talon", "Talons Seele", "l'âme de Talon"; "the soul of the Frogs",
// "die Seele der Frösche", "l'âme des grenouilles"
CustomMessage GetNpcSoulHintMessage(const NpcSoul& soul);

// True when the seed's settings put this soul in the item pool (its category is on and
// the player doesn't start with it). A soul that isn't shuffled is owned from the start.
bool IsNpcSoulShuffled(const NpcSoul& soul);

// In game: true when this soul is shuffled and hasn't been found yet (its NPC is a statue).
bool IsNpcSoulMissing(RandomizerGet rg);

// True when any NPC soul category is shuffled in the current seed.
bool AreNpcSoulsEnabled();

// "Sages Hold Dungeon Rewards": the sage who hands over a dungeon reward, and the reward location.
struct SageReward {
    RandomizerCheck rc;
    RandomizerGet soul;
};
const std::vector<SageReward>& GetSageRewards();

// The sage's soul for a reward location, or RG_NONE when no sage holds that reward in this seed
// (the switch is off, or the location isn't a sage's).
RandomizerGet GetSageRewardSoul(RandomizerCheck rc);

} // namespace Rando
