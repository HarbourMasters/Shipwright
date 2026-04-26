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
    RandomizerGet part1;
    RandomizerGet part2;
    RandomizerGet fullSong;
};

class SplitSongs {
  public:
    static bool IsSongPart(RandomizerGet rg);
    static SplitSongId GetSongIdFromPart(RandomizerGet rg);
    static const SplitSongDef* GetSongDef(SplitSongId id);
    static const SplitSongDef* GetSongDefFromPart(RandomizerGet rg);
    static const SplitSongDef* GetSongDefFromProgressive(RandomizerGet rg);
    /** Full-song RandomizerGet (RG_ZELDAS_LULLABY, …), not part items. */
    static const SplitSongDef* GetSongDefFromFullSong(RandomizerGet fullSongRg);

    static bool HasPart1(SplitSongId id);
    static bool HasPart2(SplitSongId id);
    static bool HasBothParts(SplitSongId id);
    static bool IsProgressiveSong(RandomizerGet rg);

    static void SetPart1(SplitSongId id, bool state);
    static void SetPart2(SplitSongId id, bool state);

    static bool HasFullSong(SplitSongId id);
    static void GrantFullSong(SplitSongId id);
    static void TryCompleteSong(SplitSongId id);

    /** After get-item / cutscene-safe: applies deferred quest song when both parts were obtained mid get-item. */
    static void ProcessPendingFullSongGrants();
    /** Reset deferred grants (e.g. save load). */
    static void ClearPendingFullSongGrants();

    static void OnItemReceived(RandomizerGet rg);
    static void OnProgressiveSongReceived(RandomizerGet rg);

    // Item pool / ice-trap models: 12 full songs vs 24 part items (same 12 logical songs).
    static void AppendShuffledSongPoolItems(std::vector<RandomizerGet>& pool, bool split);
    static void AppendSongIceTrapModels(std::vector<RandomizerGet>& models, bool split);

    /** In-world obtainability for RG_*_PART1/PART2 (duplicate parts / song already complete). */
    static ItemObtainability GetPartObtainability(RandomizerGet partRg);
    static ItemObtainability GetProgressiveSongObtainability(RandomizerGet progressiveRg);

    /** Dev/testing: grant every song via Part 1 + Part 2 through the same path as chests/NPCs (Randomizer_Item_Give).
     */
    static void DebugGiveAllSongParts(PlayState* play);

    /**
     * During 3drando fill, Logic uses a scratch SaveContext. Song-part flags must live there (not gSaveContext).
     * No-op when Logic is tied to the real gSaveContext (in-game).
     */
    static void ApplyPartEffectToLogicScratch(Logic* logic, RandomizerGet rg, bool state);
    static void ApplyProgressiveEffectToLogicScratch(Logic* logic, RandomizerGet rg, bool state);

    /** Returns the concrete part item represented by this progressive pickup at current state. */
    static RandomizerGet ResolveProgressiveSongStage(RandomizerGet rg);
};

} // namespace Rando
