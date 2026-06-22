// split_songs.h — Split Ocarina Songs (randomizer)
// Created by RaccoonCloud.

#pragma once

#include "randomizerTypes.h"

namespace Rando {

class Logic;

struct SplitSongData {
    RandomizerGet realSong;
    RandomizerGet part;
    RandomizerInf randInf;
    QuestItem quest;
};

  static const SplitSongData* GetSongDef(RandomizerGet rg);
  static const SplitSongData* GetSongDefFromProgressive(RandomizerGet rg);

  // static bool HasSplitPart(SplitSongId id);
  // static void SetSplitPart(SplitSongId id, bool state);
  // static bool HasFullSong(SplitSongId id);

  static bool IsProgressiveSong(RandomizerGet rg);
  static void OnProgressiveSongReceived(RandomizerGet rg);

  static RandomizerGet ResolveProgressiveSongStage(RandomizerGet rg);
  static RandomizerGet ResolveProgressiveSongStage(Logic* logic, RandomizerGet rg);

} // namespace Rando
