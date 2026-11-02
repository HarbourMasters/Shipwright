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
#define NPC_SOUL_ENTRY(id, article, name, tracker, category, advancement, german, french)              \
    souls.push_back({ RG_##id##_SOUL, RAND_INF_##id##_SOUL, RHT_##id##_SOUL, RSK_STARTING_##id##_SOUL, \
                      NpcSoulCategory::category, article, name, tracker, advancement, german, french });
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

bool IsNpcSoulCategoryShuffled(NpcSoulCategory category, uint8_t shuffleNpcSouls) {
    switch (category) {
        case NpcSoulCategory::MERCHANTS:
        case NpcSoulCategory::MINIGAMES:
            return shuffleNpcSouls >= RO_NPC_SOULS_SHOPS_AND_GAMES;
        case NpcSoulCategory::STORY:
            return shuffleNpcSouls >= RO_NPC_SOULS_PLUS_STORY;
        case NpcSoulCategory::GATEKEEPERS:
        case NpcSoulCategory::TOWNSFOLK:
            return shuffleNpcSouls >= RO_NPC_SOULS_ALL;
    }
    return false;
}

bool IsNpcSoulCategoryShuffled(NpcSoulCategory category) {
    return IsNpcSoulCategoryShuffled(category, Context::GetInstance()->GetOption(RSK_SHUFFLE_NPC_SOULS).Get());
}

bool DoSagesHoldRewards() {
    auto ctx = Context::GetInstance();
    return ctx->GetOption(RSK_NPC_SOULS_SAGES).Is(true) && IsNpcSoulCategoryShuffled(NpcSoulCategory::STORY);
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

// "Talons @" with "Seele" is "Talons Seele"
static std::string FillGermanNoun(const char* german, const std::string& noun) {
    std::string text = german;
    size_t at = text.find('@');
    return at == std::string::npos ? text : text.replace(at, 1, noun);
}

Text GetNpcSoulItemText(const NpcSoul& soul, const char* english, const char* german, const char* french) {
    return Text{ GetNpcSoulItemName(soul, english), std::string(french) + " " + soul.french,
                 FillGermanNoun(soul.german, german) };
}

CustomMessage GetNpcSoulHintMessage(const NpcSoul& soul) {
    // A German name that starts with the noun takes an article: "die Seele der Frösche"
    std::string german = FillGermanNoun(soul.german, soul.german[0] == '@' ? "die Seele" : "Seele");
    return CustomMessage(std::string("the soul of ") + soul.article + soul.name, german,
                         std::string("l'âme ") + soul.french);
}

bool IsNpcSoulShuffled(const NpcSoul& soul) {
    auto ctx = Context::GetInstance();
    return IsNpcSoulCategoryShuffled(soul.category) && ctx->GetOption(soul.startingSetting).Is(false);
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
    if (!DoSagesHoldRewards()) {
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
    return Context::GetInstance()->GetOption(RSK_SHUFFLE_NPC_SOULS).IsNot(RO_NPC_SOULS_OFF);
}

} // namespace Rando
