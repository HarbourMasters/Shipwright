// split_songs.cpp — Split Ocarina Songs (randomizer)
// Created by RaccoonCloud.

#include "split_songs.h"

#include "SeedContext.h"
#include "logic.h"
#include "static_data.h"
#include <unordered_map>

#include "global.h"
#include "macros.h"

namespace Rando {


static const std::unordered_map<RandomizerGet, SplitSongData> kSplitSongs = {
    { RG_PROGRESSIVE_ZELDAS_LULLABY,     { RG_ZELDAS_LULLABY, RG_PART_OF_ZELDAS_LULLABY ,RAND_INF_SPLIT_ZL_PART, QUEST_SONG_LULLABY } },
    { RG_PROGRESSIVE_EPONAS_SONG,        { RG_EPONAS_SONG, RG_PART_OF_EPONAS_SONG, RAND_INF_SPLIT_EPONA_PART, QUEST_SONG_EPONA } },
    { RG_PROGRESSIVE_SARIAS_SONG,        { RG_SARIAS_SONG, RG_PART_OF_SARIAS_SONG, RAND_INF_SPLIT_SARIA_PART, QUEST_SONG_SARIA } },
    { RG_PROGRESSIVE_SUNS_SONG,          { RG_SUNS_SONG, RG_PART_OF_SUNS_SONG, RAND_INF_SPLIT_SUN_PART, QUEST_SONG_SUN } },
    { RG_PROGRESSIVE_SONG_OF_TIME,       { RG_SONG_OF_TIME, RG_PART_OF_SONG_OF_TIME, RAND_INF_SPLIT_TIME_PART, QUEST_SONG_TIME } },
    { RG_PROGRESSIVE_SONG_OF_STORMS,     { RG_SONG_OF_STORMS, RG_PART_OF_SONG_OF_STORMS, RAND_INF_SPLIT_STORMS_PART, QUEST_SONG_STORMS } },
    { RG_PROGRESSIVE_MINUET_OF_FOREST,   { RG_MINUET_OF_FOREST, RG_PART_OF_MINUET_OF_FOREST, RAND_INF_SPLIT_MINUET_PART, QUEST_SONG_MINUET } },
    { RG_PROGRESSIVE_BOLERO_OF_FIRE,     { RG_BOLERO_OF_FIRE, RG_PART_OF_BOLERO_OF_FIRE, RAND_INF_SPLIT_BOLERO_PART, QUEST_SONG_BOLERO } },
    { RG_PROGRESSIVE_SERENADE_OF_WATER,  { RG_SERENADE_OF_WATER, RG_PART_OF_SERENADE_OF_WATER, RAND_INF_SPLIT_SERENADE_PART, QUEST_SONG_SERENADE } },
    { RG_PROGRESSIVE_REQUIEM_OF_SPIRIT,  { RG_REQUIEM_OF_SPIRIT, RG_PART_OF_REQUIEM_OF_SPIRIT, RAND_INF_SPLIT_REQUIEM_PART, QUEST_SONG_REQUIEM } },
    { RG_PROGRESSIVE_NOCTURNE_OF_SHADOW, { RG_NOCTURNE_OF_SHADOW, RG_PART_OF_NOCTURNE_OF_SHADOW, RAND_INF_SPLIT_NOCTURNE_PART, QUEST_SONG_NOCTURNE } },
    { RG_PROGRESSIVE_PRELUDE_OF_LIGHT,   { RG_PRELUDE_OF_LIGHT, RG_PART_OF_PRELUDE_OF_LIGHT, RAND_INF_SPLIT_PRELUDE_PART, QUEST_SONG_PRELUDE } },
};

static bool UsingLogicSimulationBuffer(Logic* logic) {
    return logic != nullptr && logic->mSaveContext != nullptr && logic->mSaveContext != &gSaveContext;
}

const SplitSongData* GetSongDef(RandomizerGet rg) {
    return &kSplitSongs.at(rg);
}

bool IsProgressiveSong(RandomizerGet rg) {
    return rg >= RG_PROGRESSIVE_ZELDAS_LULLABY && rg <= RG_PROGRESSIVE_PRELUDE_OF_LIGHT;
}

bool HasSplitPart(SplitSongId id) {
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

void SetSplitPart(SplitSongId id, bool state) {
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

bool HasFullSong(SplitSongId id) {
    const SplitSongData* def = GetSongDef(id);
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

/*void OnProgressiveSongReceived(RandomizerGet rg) {
    const SplitSongData* def = GetSongDefFromProgressive(rg);
    if (def == nullptr || HasFullSong(def->id) || HasSplitPart(def->id)) {
        return;
    }
    SetSplitPart(def->id, true);
}*/

RandomizerGet ResolveProgressiveSongStage(Logic* logic, RandomizerGet rg) {
    const SplitSongData* def = GetSongDefFromProgressive(rg);
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

RandomizerGet ResolveProgressiveSongStage(RandomizerGet rg) {
    return ResolveProgressiveSongStage(Context::GetInstance()->GetLogic().get(), rg);
}

} // namespace Rando
