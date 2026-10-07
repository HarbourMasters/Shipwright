#include "soh/Enhancements/HeartPieces/HeartPieceFlagEditor.h"
// Standalone regression test: uses the real game types and production reader.
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace HeartPieceViewer;
int main() {
    // Editing one flag must preserve all other reward bits and all unrelated
    // save data. A set/clear round trip restores the exact original structures.
    for (const auto& entry : HeartPieceViewer::heartFlags) {
        SaveContext save{};
        PlayState play{};
        play.sceneNum = entry.scene;
        for (const auto& other : HeartPieceViewer::heartFlags)
            assert(WriteFlag(other, save, &play, true));
        const auto all = save;
        const auto allPlay = play;
        assert(WriteFlag(entry, save, &play, false));
        assert(ReadFlag(entry, save, &play) == false);
        assert(ReadFlag(entry, save, nullptr) == false);
        size_t remaining = 0;
        for (const auto& other : HeartPieceViewer::heartFlags)
            remaining += ReadFlag(other, save, &play) == true;
        assert(remaining == HeartPieceViewer::heartFlags.size() - 1);
        assert(WriteFlag(entry, save, &play, true));
        assert(std::memcmp(&all, &save, sizeof(save)) == 0);
        assert(std::memcmp(&allPlay, &play, sizeof(play)) == 0);
    }
    for (const auto& entry : bossHeartFlags) {
        SaveContext save{};
        PlayState play{};
        play.sceneNum = entry.scene;
        for (const auto& other : bossHeartFlags)
            assert(WriteFlag(other, save, &play, true));
        const auto all = save;
        const auto allPlay = play;
        assert(WriteFlag(entry, save, &play, false));
        assert(ReadFlag(entry, save, &play) == false);
        assert(ReadFlag(entry, save, nullptr) == false);
        size_t remaining = 0;
        for (const auto& other : bossHeartFlags)
            remaining += ReadFlag(other, save, &play) == true;
        assert(remaining == bossHeartFlags.size() - 1);
        assert(WriteFlag(entry, save, &play, true));
        assert(std::memcmp(&all, &save, sizeof(save)) == 0);
        assert(std::memcmp(&allPlay, &play, sizeof(play)) == 0);
    }
    // Scene edits reconcile only the selected bit, not the entire flag word.
    {
        const auto& entry = bossHeartFlags[0];
        SaveContext save{};
        PlayState play{};
        play.sceneNum = entry.scene;
        save.sceneFlags[entry.scene].collect = 1u << 3;
        play.actorCtx.flags.collect = 1u << 4;
        assert(WriteFlag(entry, save, &play, true));
        assert(WriteFlag(entry, save, &play, false));
        assert(save.sceneFlags[entry.scene].collect == (1u << 3));
        assert(play.actorCtx.flags.collect == (1u << 4));
    }
    std::puts("PASS: editor set/clear, unrelated bits, live/saved synchronization");

    static_assert(bossHeartFlags.size() == 8);
    for (const auto& entry : bossHeartFlags) {
        assert(entry.type == HeartFlagType::Collectible && entry.flag == 0x1F);
        SaveContext save{};
        save.sceneFlags[entry.scene].collect = 1u << 31;
        assert(ReadFlag(entry, save, nullptr) == true);
        for (const auto& piece : HeartPieceViewer::heartFlags)
            assert(ReadFlag(piece, save, nullptr) == false);
    }
    std::puts("PASS: 8 boss drops use collectible 0x1F and never count as pieces");

    size_t pieces = 0;
    for (const auto& entry : HeartPieceViewer::heartFlags) {
        SaveContext save{};
        assert(ReadFlag(entry, save, nullptr) == false);

        switch (entry.type) {
            case HeartFlagType::Collectible:
                save.sceneFlags[entry.scene].collect |= 1u << entry.flag;
                break;
            case HeartFlagType::Chest:
                save.sceneFlags[entry.scene].chest |= 1u << entry.flag;
                break;
            case HeartFlagType::ItemGet:
                save.itemGetInf[entry.flag >> 4] |= 1u << (entry.flag & 15);
                break;
            case HeartFlagType::Event:
                save.eventChkInf[entry.flag >> 4] |= 1u << (entry.flag & 15);
                break;
            case HeartFlagType::Info:
                save.infTable[entry.flag >> 4] |= 1u << (entry.flag & 15);
                break;
            case HeartFlagType::Fishing:
                save.highScores[HS_FISHING] |= entry.flag;
                break;
        }

        auto before = save;
        size_t hits = 0;
        for (const auto& other : HeartPieceViewer::heartFlags)
            hits += ReadFlag(other, save, nullptr).value();
        assert(hits == 1); // Each reward has its own flag; no duplicates or false positives.
        assert(ReadFlag(entry, save, nullptr) == true);
        assert(std::memcmp(&before, &save, sizeof(save)) == 0);
        // The expected flag was set independently above, without the writer.
        assert(WriteFlag(entry, save, nullptr, false));
        const SaveContext cleared{};
        assert(std::memcmp(&cleared, &save, sizeof(save)) == 0);
        assert(WriteFlag(entry, save, nullptr, true));
        assert(std::memcmp(&before, &save, sizeof(save)) == 0);

        if (entry.type == HeartFlagType::Collectible || entry.type == HeartFlagType::Chest) {
            PlayState play{};
            play.sceneNum = entry.scene;
            // Current scene is authoritative even when its saved copy disagrees.
            assert(ReadFlag(entry, save, &play) == false);
            if (entry.type == HeartFlagType::Collectible)
                play.actorCtx.flags.collect = 1u << entry.flag;
            else
                play.actorCtx.flags.chest = 1u << entry.flag;
            SaveContext empty{};
            assert(ReadFlag(entry, empty, &play) == true);
            play.sceneNum = -1;
            assert(ReadFlag(entry, empty, &play) == false);
        }

        pieces += 1;
    }
    assert(pieces == 36);
    auto invalid = HeartPieceViewer::heartFlags[0];
    invalid.flag = 0xFFFF;
    SaveContext empty{};
    assert(!ReadFlag(invalid, empty, nullptr).has_value());
    const auto beforeInvalid = empty;
    assert(!WriteFlag(invalid, empty, nullptr, true));
    assert(std::memcmp(&beforeInvalid, &empty, sizeof(empty)) == 0);
    std::printf("PASS: %zu pieces, unique flags, empty saves, current-scene "
                "precedence, invalid flags, read-only state\n",
                pieces);
}
