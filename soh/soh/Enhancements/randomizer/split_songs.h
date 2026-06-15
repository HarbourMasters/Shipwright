// split_songs.h — Split Ocarina Songs (randomizer)
// Created by RaccoonCloud.

#pragma once

#include "randomizerTypes.h"

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

    static bool HasSplitPart(SplitSongId id);
    static void SetSplitPart(SplitSongId id, bool state);
    static bool HasFullSong(SplitSongId id);

    static bool IsProgressiveSong(RandomizerGet rg);
    static void OnProgressiveSongReceived(RandomizerGet rg);

    static ItemObtainability GetProgressiveSongObtainability(RandomizerGet progressiveRg);
    static void ApplyProgressiveEffectToLogicScratch(Logic* logic, RandomizerGet rg, bool state);
    static RandomizerGet ResolveProgressiveSongStage(RandomizerGet rg);
    static RandomizerGet ResolveProgressiveSongStage(Logic* logic, RandomizerGet rg);
};

} // namespace Rando
