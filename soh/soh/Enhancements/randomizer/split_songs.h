// split_songs.h — Split Ocarina Songs (randomizer)
// Created by RaccoonCloud.

#pragma once

#include "randomizerTypes.h"
#include <vector>

struct PlayState;

namespace Rando {

class Logic;

enum SplitSongId {
    SPLIT_SONG_ZELDAS_LULLABY = 0,
    SPLIT_SONG_EPONAS_SONG,
    SPLIT_SONG_SARIAS_SONG,
    SPLIT_SONG_SUNS_SONG,
    SPLIT_SONG_SONG_OF_TIME,
    SPLIT_SONG_SONG_OF_STORMS,
    SPLIT_SONG_MINUET_OF_FOREST,
    SPLIT_SONG_BOLERO_OF_FIRE,
    SPLIT_SONG_SERENADE_OF_WATER,
    SPLIT_SONG_REQUIEM_OF_SPIRIT,
    SPLIT_SONG_NOCTURNE_OF_SHADOW,
    SPLIT_SONG_PRELUDE_OF_LIGHT,
    SPLIT_SONG_MAX
};

struct SplitSongDef {
    SplitSongId id;
    RandomizerGet progressive;
    RandomizerGet fullSong;
};

class SplitSongs {
  public:
    static const SplitSongDef* GetSongDef(SplitSongId id);
    static const SplitSongDef* GetSongDefFromProgressive(RandomizerGet rg);
    static const SplitSongDef* GetSongDefFromFullSong(RandomizerGet fullSongRg);

    /** RandInf set after the first progressive pickup for this song. */
    static bool HasSplitPart(SplitSongId id);
    static void SetSplitPart(SplitSongId id, bool state);

    static bool HasFullSong(SplitSongId id);
    static void GrantFullSong(SplitSongId id);
    static void TryCompleteSong(SplitSongId id);

    static bool IsProgressiveSong(RandomizerGet rg);

    static void ProcessPendingFullSongGrants();
    static void ClearPendingFullSongGrants();

    static void OnProgressiveSongReceived(RandomizerGet rg);

    static void AppendShuffledSongPoolItems(std::vector<RandomizerGet>& pool, bool split);
    static void AppendSongIceTrapModels(std::vector<RandomizerGet>& models, bool split);

    static ItemObtainability GetProgressiveSongObtainability(RandomizerGet progressiveRg);

    static void DebugGiveAllSongParts(PlayState* play);

    static void ApplyProgressiveEffectToLogicScratch(Logic* logic, RandomizerGet rg, bool state);

    /** Item to show on pickup / GI resolve: progressive first, full song on second. */
    static RandomizerGet ResolveProgressiveSongStage(RandomizerGet rg);
};

} // namespace Rando
