#include "npc_souls.h"

#include "SeedContext.h"
#include "randomizerEnums/NpcSoulList.h"

extern "C" {
#include "z64.h"
#include "functions.h"
}

namespace Rando {

static std::vector<NpcSoul> BuildNpcSouls() {
    std::vector<NpcSoul> souls;
#define NPC_SOUL_ENTRY(id, article, name, tracker, category, advancement)                              \
    souls.push_back({ RG_##id##_SOUL, RAND_INF_##id##_SOUL, RHT_##id##_SOUL, RSK_STARTING_##id##_SOUL, \
                      NpcSoulCategory::category, article, name, tracker, advancement });
    NPC_SOUL_LIST(NPC_SOUL_ENTRY)
#undef NPC_SOUL_ENTRY
    return souls;
}

const std::vector<NpcSoul>& GetNpcSouls() {
    static const std::vector<NpcSoul> souls = BuildNpcSouls();
    return souls;
}

const NpcSoul* GetNpcSoul(RandomizerGet rg) {
    const auto& souls = GetNpcSouls();
    // The RG values are contiguous and in list order.
    if (souls.empty() || rg < souls.front().rg || rg > souls.back().rg) {
        return nullptr;
    }
    return &souls[rg - souls.front().rg];
}

bool IsNpcSoul(RandomizerGet rg) {
    return GetNpcSoul(rg) != nullptr;
}

RandomizerSettingKey GetNpcSoulCategorySetting(NpcSoulCategory category) {
    switch (category) {
        case NpcSoulCategory::MERCHANTS:
            return RSK_SHUFFLE_NPC_SOULS_MERCHANTS;
        case NpcSoulCategory::MINIGAMES:
            return RSK_SHUFFLE_NPC_SOULS_MINIGAMES;
        case NpcSoulCategory::GATEKEEPERS:
            return RSK_SHUFFLE_NPC_SOULS_GATEKEEPERS;
        case NpcSoulCategory::STORY:
            return RSK_SHUFFLE_NPC_SOULS_STORY;
        case NpcSoulCategory::TOWNSFOLK:
            return RSK_SHUFFLE_NPC_SOULS_TOWNSFOLK;
    }
    return RSK_SHUFFLE_NPC_SOULS_MERCHANTS;
}

std::string GetNpcSoulItemName(const NpcSoul& soul, const char* noun) {
    std::string name = soul.name;
    if (soul.article[0] != '\0') {
        // Group souls: "Soul of the Zoras"
        return std::string(noun) + " of " + soul.article + name;
    }
    name += (!name.empty() && name.back() == 's') ? "' " : "'s ";
    return name + noun;
}

std::string GetNpcSoulHintName(const NpcSoul& soul) {
    return std::string("the soul of ") + soul.article + soul.name;
}

bool IsNpcSoulShuffled(const NpcSoul& soul) {
    auto ctx = Context::GetInstance();
    return ctx->GetOption(GetNpcSoulCategorySetting(soul.category)).Is(true) &&
           ctx->GetOption(soul.startingSetting).Is(false);
}

const std::vector<SageReward>& GetSageRewards() {
    // The Kokiri Emerald comes from the Deku Tree, who has no soul.
    static const std::vector<SageReward> rewards = {
        { RC_KING_DODONGO, RG_DARUNIA_SOUL }, { RC_BARINADE, RG_RUTO_SOUL },
        { RC_PHANTOM_GANON, RG_SARIA_SOUL },  { RC_VOLVAGIA, RG_DARUNIA_SOUL },
        { RC_MORPHA, RG_RUTO_SOUL },          { RC_BONGO_BONGO, RG_IMPA_SOUL },
        { RC_TWINROVA, RG_NABOORU_SOUL },     { RC_GIFT_FROM_RAURU, RG_RAURU_SOUL },
    };
    return rewards;
}

RandomizerGet GetSageRewardSoul(RandomizerCheck rc) {
    if (Context::GetInstance()->GetOption(RSK_NPC_SOULS_SAGES).Is(false)) {
        return RG_NONE;
    }
    for (const SageReward& reward : GetSageRewards()) {
        if (reward.rc == rc) {
            return reward.soul;
        }
    }
    return RG_NONE;
}

bool IsNpcSoulMissing(RandomizerGet rg) {
    const NpcSoul* soul = GetNpcSoul(rg);
    return soul != nullptr && IsNpcSoulShuffled(*soul) && !Flags_GetRandomizerInf(soul->randInf);
}

bool AreNpcSoulsEnabled() {
    auto ctx = Context::GetInstance();
    for (NpcSoulCategory category :
         { NpcSoulCategory::MERCHANTS, NpcSoulCategory::MINIGAMES, NpcSoulCategory::GATEKEEPERS, NpcSoulCategory::STORY,
           NpcSoulCategory::TOWNSFOLK }) {
        if (ctx->GetOption(GetNpcSoulCategorySetting(category)).Is(true)) {
            return true;
        }
    }
    return false;
}

} // namespace Rando
