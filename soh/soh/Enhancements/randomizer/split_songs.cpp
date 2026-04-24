// split_songs.cpp — Split Ocarina Songs (randomizer)
// Created by RaccoonCloud.

#include "split_songs.h"

#include "SeedContext.h"
#include "logic.h"
#include "static_data.h"

#include "functions.h"
#include "global.h"
#include "macros.h"
#include "z64player.h"
#include "variables.h"

#include <vector>

namespace Rando {

static constexpr uint32_t kSplitSongPart1Flags[SPLIT_SONG_MAX] = {
    RAND_INF_SPLIT_ZL_PART1,
    RAND_INF_SPLIT_EPONA_PART1,
    RAND_INF_SPLIT_SARIA_PART1,
    RAND_INF_SPLIT_SUN_PART1,
    RAND_INF_SPLIT_TIME_PART1,
    RAND_INF_SPLIT_STORMS_PART1,
    RAND_INF_SPLIT_MINUET_PART1,
    RAND_INF_SPLIT_BOLERO_PART1,
    RAND_INF_SPLIT_SERENADE_PART1,
    RAND_INF_SPLIT_REQUIEM_PART1,
    RAND_INF_SPLIT_NOCTURNE_PART1,
    RAND_INF_SPLIT_PRELUDE_PART1,
};

static constexpr uint32_t kSplitSongPart2Flags[SPLIT_SONG_MAX] = {
    RAND_INF_SPLIT_ZL_PART2,
    RAND_INF_SPLIT_EPONA_PART2,
    RAND_INF_SPLIT_SARIA_PART2,
    RAND_INF_SPLIT_SUN_PART2,
    RAND_INF_SPLIT_TIME_PART2,
    RAND_INF_SPLIT_STORMS_PART2,
    RAND_INF_SPLIT_MINUET_PART2,
    RAND_INF_SPLIT_BOLERO_PART2,
    RAND_INF_SPLIT_SERENADE_PART2,
    RAND_INF_SPLIT_REQUIEM_PART2,
    RAND_INF_SPLIT_NOCTURNE_PART2,
    RAND_INF_SPLIT_PRELUDE_PART2,
};

static constexpr SplitSongDef kSplitSongs[SPLIT_SONG_MAX] = {
    { SPLIT_SONG_ZELDAS_LULLABY, RG_ZELDAS_LULLABY_PART1, RG_ZELDAS_LULLABY_PART2, RG_ZELDAS_LULLABY },
    { SPLIT_SONG_EPONAS_SONG, RG_EPONAS_SONG_PART1, RG_EPONAS_SONG_PART2, RG_EPONAS_SONG },
    { SPLIT_SONG_SARIAS_SONG, RG_SARIAS_SONG_PART1, RG_SARIAS_SONG_PART2, RG_SARIAS_SONG },
    { SPLIT_SONG_SUNS_SONG, RG_SUNS_SONG_PART1, RG_SUNS_SONG_PART2, RG_SUNS_SONG },
    { SPLIT_SONG_SONG_OF_TIME, RG_SONG_OF_TIME_PART1, RG_SONG_OF_TIME_PART2, RG_SONG_OF_TIME },
    { SPLIT_SONG_SONG_OF_STORMS, RG_SONG_OF_STORMS_PART1, RG_SONG_OF_STORMS_PART2, RG_SONG_OF_STORMS },
    { SPLIT_SONG_MINUET_OF_FOREST, RG_MINUET_OF_FOREST_PART1, RG_MINUET_OF_FOREST_PART2, RG_MINUET_OF_FOREST },
    { SPLIT_SONG_BOLERO_OF_FIRE, RG_BOLERO_OF_FIRE_PART1, RG_BOLERO_OF_FIRE_PART2, RG_BOLERO_OF_FIRE },
    { SPLIT_SONG_SERENADE_OF_WATER, RG_SERENADE_OF_WATER_PART1, RG_SERENADE_OF_WATER_PART2, RG_SERENADE_OF_WATER },
    { SPLIT_SONG_REQUIEM_OF_SPIRIT, RG_REQUIEM_OF_SPIRIT_PART1, RG_REQUIEM_OF_SPIRIT_PART2, RG_REQUIEM_OF_SPIRIT },
    { SPLIT_SONG_NOCTURNE_OF_SHADOW, RG_NOCTURNE_OF_SHADOW_PART1, RG_NOCTURNE_OF_SHADOW_PART2, RG_NOCTURNE_OF_SHADOW },
    { SPLIT_SONG_PRELUDE_OF_LIGHT, RG_PRELUDE_OF_LIGHT_PART1, RG_PRELUDE_OF_LIGHT_PART2, RG_PRELUDE_OF_LIGHT },
};

static bool IsValidSplitSongId(SplitSongId id) {
    return id >= 0 && id < SPLIT_SONG_MAX;
}

static std::vector<SplitSongId> sPendingFullSongGrants;
static bool sSongGrantQueued[SPLIT_SONG_MAX] = {};

/** Granting the quest song updates save while get-item / blocking CS is active; defer to next safe player update. */
static bool ShouldDeferFullSongQuestGrant() {
    if (gPlayState == nullptr) {
        return false;
    }
    Player* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return false;
    }
    return Player_InBlockingCsMode(gPlayState, player) || (player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS) ||
           (player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) || (player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR);
}

bool SplitSongs::IsSongPart(RandomizerGet rg) {
    return GetSongDefFromPart(rg) != nullptr;
}

SplitSongId SplitSongs::GetSongIdFromPart(RandomizerGet rg) {
    const SplitSongDef* def = GetSongDefFromPart(rg);
    return def != nullptr ? def->id : SPLIT_SONG_MAX;
}

const SplitSongDef* SplitSongs::GetSongDef(SplitSongId id) {
    if (!IsValidSplitSongId(id)) {
        return nullptr;
    }
    return &kSplitSongs[id];
}

const SplitSongDef* SplitSongs::GetSongDefFromPart(RandomizerGet rg) {
    for (const auto& def : kSplitSongs) {
        if (def.part1 == rg || def.part2 == rg) {
            return &def;
        }
    }
    return nullptr;
}

const SplitSongDef* SplitSongs::GetSongDefFromFullSong(RandomizerGet fullSongRg) {
    for (const auto& def : kSplitSongs) {
        if (def.fullSong == fullSongRg) {
            return &def;
        }
    }
    return nullptr;
}

static bool UsingLogicSimulationBuffer(Logic* logic) {
    return logic != nullptr && logic->mSaveContext != nullptr && logic->mSaveContext != &gSaveContext;
}

bool SplitSongs::HasPart1(SplitSongId id) {
    if (!IsValidSplitSongId(id)) {
        return false;
    }
    const RandomizerInf flag = static_cast<RandomizerInf>(kSplitSongPart1Flags[id]);
    auto* logic = Context::GetInstance()->GetLogic().get();
    if (UsingLogicSimulationBuffer(logic)) {
        return logic->CheckRandoInf(flag);
    }
    return Flags_GetRandomizerInf(flag) != 0;
}

bool SplitSongs::HasPart2(SplitSongId id) {
    if (!IsValidSplitSongId(id)) {
        return false;
    }
    const RandomizerInf flag = static_cast<RandomizerInf>(kSplitSongPart2Flags[id]);
    auto* logic = Context::GetInstance()->GetLogic().get();
    if (UsingLogicSimulationBuffer(logic)) {
        return logic->CheckRandoInf(flag);
    }
    return Flags_GetRandomizerInf(flag) != 0;
}

bool SplitSongs::HasBothParts(SplitSongId id) {
    return HasPart1(id) && HasPart2(id);
}

void SplitSongs::SetPart1(SplitSongId id, bool state) {
    if (!IsValidSplitSongId(id)) {
        return;
    }
    RandomizerInf flag = static_cast<RandomizerInf>(kSplitSongPart1Flags[id]);
    if (state) {
        Flags_SetRandomizerInf(flag);
    } else {
        Flags_UnsetRandomizerInf(flag);
    }
}

void SplitSongs::SetPart2(SplitSongId id, bool state) {
    if (!IsValidSplitSongId(id)) {
        return;
    }
    RandomizerInf flag = static_cast<RandomizerInf>(kSplitSongPart2Flags[id]);
    if (state) {
        Flags_SetRandomizerInf(flag);
    } else {
        Flags_UnsetRandomizerInf(flag);
    }
}

bool SplitSongs::HasFullSong(SplitSongId id) {
    const SplitSongDef* def = GetSongDef(id);
    if (def == nullptr) {
        return false;
    }
    const auto qiIt = Logic::RandoGetToQuestItem.find(static_cast<uint32_t>(def->fullSong));
    if (qiIt == Logic::RandoGetToQuestItem.end()) {
        return false;
    }
    auto* logic = Context::GetInstance()->GetLogic().get();
    if (UsingLogicSimulationBuffer(logic)) {
        return logic->CheckQuestItem(qiIt->second);
    }
    return CHECK_QUEST_ITEM(qiIt->second);
}

void SplitSongs::GrantFullSong(SplitSongId id) {
    const SplitSongDef* def = GetSongDef(id);
    if (def == nullptr) {
        return;
    }

    auto logic = Context::GetInstance()->GetLogic();
    logic->SetSaveContext(&gSaveContext);
    const auto qiIt = Logic::RandoGetToQuestItem.find(static_cast<uint32_t>(def->fullSong));
    if (qiIt != Logic::RandoGetToQuestItem.end() && CHECK_QUEST_ITEM(qiIt->second)) {
        return;
    }

    auto& fullSongItem = StaticData::RetrieveItem(def->fullSong);
    logic->ApplyItemEffect(fullSongItem, true);
}

void SplitSongs::TryCompleteSong(SplitSongId id) {
    if (!HasBothParts(id) || HasFullSong(id)) {
        return;
    }
    if (gPlayState == nullptr) {
        GrantFullSong(id);
        return;
    }
    if (!IsValidSplitSongId(id) || sSongGrantQueued[id]) {
        return;
    }
    sSongGrantQueued[id] = true;
    sPendingFullSongGrants.push_back(id);
}

void SplitSongs::ProcessPendingFullSongGrants() {
    if (sPendingFullSongGrants.empty() || gPlayState == nullptr) {
        return;
    }
    Player* player = GET_PLAYER(gPlayState);
    if (player == nullptr || ShouldDeferFullSongQuestGrant()) {
        return;
    }

    std::vector<SplitSongId> batch = std::move(sPendingFullSongGrants);
    sPendingFullSongGrants.clear();

    for (SplitSongId id : batch) {
        if (IsValidSplitSongId(id)) {
            sSongGrantQueued[id] = false;
        }
    }
    for (SplitSongId id : batch) {
        if (!IsValidSplitSongId(id)) {
            continue;
        }
        if (HasBothParts(id) && !HasFullSong(id)) {
            GrantFullSong(id);
        }
    }
}

void SplitSongs::ClearPendingFullSongGrants() {
    sPendingFullSongGrants.clear();
    for (size_t i = 0; i < SPLIT_SONG_MAX; i++) {
        sSongGrantQueued[i] = false;
    }
}

void SplitSongs::OnItemReceived(RandomizerGet rg) {
    const SplitSongDef* def = GetSongDefFromPart(rg);
    if (def == nullptr) {
        return;
    }

    if (rg == def->part1) {
        SetPart1(def->id, true);
    } else if (rg == def->part2) {
        SetPart2(def->id, true);
    }

    TryCompleteSong(def->id);
}

ItemObtainability SplitSongs::GetPartObtainability(RandomizerGet partRg) {
    const SplitSongDef* def = GetSongDefFromPart(partRg);
    if (def == nullptr) {
        return CAN_OBTAIN;
    }
    const auto qiIt = Logic::RandoGetToQuestItem.find(static_cast<uint32_t>(def->fullSong));
    if (qiIt != Logic::RandoGetToQuestItem.end() && CHECK_QUEST_ITEM(qiIt->second)) {
        return CANT_OBTAIN_ALREADY_HAVE;
    }
    if (partRg == def->part1 && HasPart1(def->id)) {
        return CANT_OBTAIN_ALREADY_HAVE;
    }
    if (partRg == def->part2 && HasPart2(def->id)) {
        return CANT_OBTAIN_ALREADY_HAVE;
    }
    return CAN_OBTAIN;
}

void SplitSongs::AppendShuffledSongPoolItems(std::vector<RandomizerGet>& pool, bool split) {
    for (const auto& def : kSplitSongs) {
        if (split) {
            pool.push_back(def.part1);
            pool.push_back(def.part2);
        } else {
            pool.push_back(def.fullSong);
        }
    }
}

void SplitSongs::AppendSongIceTrapModels(std::vector<RandomizerGet>& models, bool split) {
    AppendShuffledSongPoolItems(models, split);
}

void SplitSongs::DebugGiveAllSongParts(PlayState* play) {
    if (play == nullptr) {
        return;
    }
    for (const auto& def : kSplitSongs) {
        Randomizer_Item_Give(play, StaticData::RetrieveItem(def.part1).GetGIEntry_Copy());
        Randomizer_Item_Give(play, StaticData::RetrieveItem(def.part2).GetGIEntry_Copy());
    }
}

void SplitSongs::ApplyPartEffectToLogicScratch(Logic* logic, RandomizerGet rg, bool state) {
    if (!UsingLogicSimulationBuffer(logic)) {
        return;
    }
    const SplitSongDef* def = GetSongDefFromPart(rg);
    if (def == nullptr) {
        return;
    }
    const uint32_t flag = (rg == def->part1) ? kSplitSongPart1Flags[def->id] : kSplitSongPart2Flags[def->id];
    logic->SetRandoInf(flag, state);
}

} // namespace Rando
