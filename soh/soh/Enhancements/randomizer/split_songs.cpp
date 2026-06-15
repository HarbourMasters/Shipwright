// split_songs.cpp — Split Ocarina Songs (randomizer)
// Created by RaccoonCloud.

#include "split_songs.h"

#include "SeedContext.h"
#include "logic.h"
#include "static_data.h"

#include "global.h"
#include "macros.h"

namespace Rando {

static constexpr uint32_t kSplitSongPartFlags[SPLIT_SONG_MAX] = {
    RAND_INF_SPLIT_ZL_PART1,      RAND_INF_SPLIT_EPONA_PART1,    RAND_INF_SPLIT_SARIA_PART1,
    RAND_INF_SPLIT_SUN_PART1,     RAND_INF_SPLIT_TIME_PART1,     RAND_INF_SPLIT_STORMS_PART1,
    RAND_INF_SPLIT_MINUET_PART1,  RAND_INF_SPLIT_BOLERO_PART1,   RAND_INF_SPLIT_SERENADE_PART1,
    RAND_INF_SPLIT_REQUIEM_PART1, RAND_INF_SPLIT_NOCTURNE_PART1, RAND_INF_SPLIT_PRELUDE_PART1,
};

static constexpr SplitSongDef kSplitSongs[SPLIT_SONG_MAX] = {
    { SPLIT_SONG_ZELDAS_LULLABY, RG_PROGRESSIVE_ZELDAS_LULLABY, RG_ZELDAS_LULLABY },
    { SPLIT_SONG_EPONAS_SONG, RG_PROGRESSIVE_EPONAS_SONG, RG_EPONAS_SONG },
    { SPLIT_SONG_SARIAS_SONG, RG_PROGRESSIVE_SARIAS_SONG, RG_SARIAS_SONG },
    { SPLIT_SONG_SUNS_SONG, RG_PROGRESSIVE_SUNS_SONG, RG_SUNS_SONG },
    { SPLIT_SONG_SONG_OF_TIME, RG_PROGRESSIVE_SONG_OF_TIME, RG_SONG_OF_TIME },
    { SPLIT_SONG_SONG_OF_STORMS, RG_PROGRESSIVE_SONG_OF_STORMS, RG_SONG_OF_STORMS },
    { SPLIT_SONG_MINUET_OF_FOREST, RG_PROGRESSIVE_MINUET_OF_FOREST, RG_MINUET_OF_FOREST },
    { SPLIT_SONG_BOLERO_OF_FIRE, RG_PROGRESSIVE_BOLERO_OF_FIRE, RG_BOLERO_OF_FIRE },
    { SPLIT_SONG_SERENADE_OF_WATER, RG_PROGRESSIVE_SERENADE_OF_WATER, RG_SERENADE_OF_WATER },
    { SPLIT_SONG_REQUIEM_OF_SPIRIT, RG_PROGRESSIVE_REQUIEM_OF_SPIRIT, RG_REQUIEM_OF_SPIRIT },
    { SPLIT_SONG_NOCTURNE_OF_SHADOW, RG_PROGRESSIVE_NOCTURNE_OF_SHADOW, RG_NOCTURNE_OF_SHADOW },
    { SPLIT_SONG_PRELUDE_OF_LIGHT, RG_PROGRESSIVE_PRELUDE_OF_LIGHT, RG_PRELUDE_OF_LIGHT },
};

static bool IsValidSplitSongId(SplitSongId id) {
    return id >= 0 && id < SPLIT_SONG_MAX;
}

static bool UsingLogicSimulationBuffer(Logic* logic) {
    return logic != nullptr && logic->mSaveContext != nullptr && logic->mSaveContext != &gSaveContext;
}

const SplitSongDef* SplitSongs::GetSongDef(SplitSongId id) {
    if (!IsValidSplitSongId(id)) {
        return nullptr;
    }
    return &kSplitSongs[id];
}

const SplitSongDef* SplitSongs::GetSongDefFromProgressive(RandomizerGet rg) {
    for (const auto& def : kSplitSongs) {
        if (def.progressive == rg) {
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

bool SplitSongs::IsProgressiveSong(RandomizerGet rg) {
    return GetSongDefFromProgressive(rg) != nullptr;
}

bool SplitSongs::HasSplitPart(SplitSongId id) {
    if (!IsValidSplitSongId(id)) {
        return false;
    }
    const RandomizerInf flag = static_cast<RandomizerInf>(kSplitSongPartFlags[id]);
    auto* logic = Context::GetInstance()->GetLogic().get();
    if (UsingLogicSimulationBuffer(logic)) {
        return logic->CheckRandoInf(flag);
    }
    return Flags_GetRandomizerInf(flag) != 0;
}

void SplitSongs::SetSplitPart(SplitSongId id, bool state) {
    if (!IsValidSplitSongId(id)) {
        return;
    }
    RandomizerInf flag = static_cast<RandomizerInf>(kSplitSongPartFlags[id]);
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

void SplitSongs::OnProgressiveSongReceived(RandomizerGet rg) {
    const SplitSongDef* def = GetSongDefFromProgressive(rg);
    if (def == nullptr || HasFullSong(def->id) || HasSplitPart(def->id)) {
        return;
    }
    SetSplitPart(def->id, true);
}

ItemObtainability SplitSongs::GetProgressiveSongObtainability(RandomizerGet progressiveRg) {
    const SplitSongDef* def = GetSongDefFromProgressive(progressiveRg);
    if (def == nullptr) {
        return CAN_OBTAIN;
    }
    return HasFullSong(def->id) ? CANT_OBTAIN_ALREADY_HAVE : CAN_OBTAIN;
}

void SplitSongs::ApplyProgressiveEffectToLogicScratch(Logic* logic, RandomizerGet rg, bool state) {
    if (!UsingLogicSimulationBuffer(logic)) {
        return;
    }
    const SplitSongDef* def = GetSongDefFromProgressive(rg);
    if (def == nullptr) {
        return;
    }

    const uint32_t partFlag = kSplitSongPartFlags[def->id];
    const auto qiIt = Logic::RandoGetToQuestItem.find(static_cast<uint32_t>(def->fullSong));
    if (qiIt == Logic::RandoGetToQuestItem.end()) {
        return;
    }

    if (state) {
        if (!logic->CheckRandoInf(static_cast<RandomizerInf>(partFlag))) {
            logic->SetRandoInf(partFlag, true);
        } else {
            logic->SetQuestItem(qiIt->second, true);
        }
    } else if (logic->CheckQuestItem(qiIt->second)) {
        logic->SetQuestItem(qiIt->second, false);
    } else if (logic->CheckRandoInf(static_cast<RandomizerInf>(partFlag))) {
        logic->SetRandoInf(partFlag, false);
    }
}

RandomizerGet SplitSongs::ResolveProgressiveSongStage(Logic* logic, RandomizerGet rg) {
    const SplitSongDef* def = GetSongDefFromProgressive(rg);
    if (def == nullptr) {
        return RG_NONE;
    }

    if (logic != nullptr && UsingLogicSimulationBuffer(logic)) {
        const auto qiIt = Logic::RandoGetToQuestItem.find(static_cast<uint32_t>(def->fullSong));
        if (qiIt != Logic::RandoGetToQuestItem.end() && logic->CheckQuestItem(qiIt->second)) {
            return def->fullSong;
        }
        if (logic->CheckRandoInf(static_cast<RandomizerInf>(kSplitSongPartFlags[def->id]))) {
            return def->fullSong;
        }
        return def->progressive;
    }

    if (HasFullSong(def->id) || HasSplitPart(def->id)) {
        return def->fullSong;
    }
    return def->progressive;
}

RandomizerGet SplitSongs::ResolveProgressiveSongStage(RandomizerGet rg) {
    return ResolveProgressiveSongStage(Context::GetInstance()->GetLogic().get(), rg);
}

} // namespace Rando
