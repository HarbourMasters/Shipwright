#include "soh/Enhancements/HeartPieces/HeartPieceFlagEditor.h"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace HeartPieceViewer;

static auto Piece() {
    return heartFlags[0];
}
static auto Container() {
    return bossHeartFlags[0];
}
static auto& QuestBits(SaveContext& save) {
    return save.inventory.questItems;
}
static auto& Capacity(SaveContext& save) {
    return save.healthCapacity;
}
static auto& Health(SaveContext& save) {
    return save.health;
}
static void Init(SaveContext& save, int pieces = 0, int capacity = 64) {
    QuestBits(save) = 0x00001234u | (uint32_t(pieces) << 28);
    Capacity(save) = capacity;
    Health(save) = capacity - 7;
    save.ship.stats.heartPieces = 12;
    save.ship.stats.heartContainers = 4;
}
static const char* Edit(SaveContext& save, bool container, bool collected, bool sync = true) {
    return EditReward(container ? Container() : Piece(), save, nullptr, collected, container, sync);
}
int main() {
    for (bool container : { false, true }) {
        for (int pieces = 0; pieces < 4; ++pieces) {
            for (bool collected : { false, true }) {
                SaveContext save{};
                Init(save, pieces);
                WriteFlag(container ? Container() : Piece(), save, nullptr, !collected);
                assert(Edit(save, container, collected) == nullptr);
                const int delta = collected ? 1 : -1;
                const int expectedPieces = container ? pieces : (pieces + delta + 4) % 4;
                const int heartDelta = container ? delta : collected ? (pieces == 3) : -(pieces == 0);
                assert((QuestBits(save) >> 28) == expectedPieces);
                assert((QuestBits(save) & 0x0FFFFFFFu) == 0x1234u);
                assert(Capacity(save) == 64 + heartDelta * 16);
                assert(Health(save) == std::min(57 + std::max(heartDelta, 0) * 16, int(Capacity(save))));
                assert(ReadFlag(container ? Container() : Piece(), save, nullptr) == collected);
                assert(save.ship.stats.heartPieces == 12 + (container ? 0 : delta));
                assert(save.ship.stats.heartContainers == 4 + (container ? delta : 0));

                const auto once = save;
                assert(Edit(save, container, collected) == nullptr);
                assert(std::memcmp(&once, &save, sizeof(save)) == 0); // no duplicate credit
            }
        }
    }
    // Flag-only mode must leave health, counters and statistics untouched.
    {
        SaveContext actual{}, expected{};
        Init(actual, 3);
        expected = actual;
        WriteFlag(Piece(), expected, nullptr, true);
        assert(Edit(actual, false, true, false) == nullptr);
        assert(std::memcmp(&actual, &expected, sizeof(actual)) == 0);
    }
    // A piece that does not complete a heart leaves current health alone,
    // including deliberate over-capacity values from other editor controls.
    {
        SaveContext save{};
        Init(save, 1);
        Health(save) = 90;
        assert(Edit(save, false, true) == nullptr);
        assert(Health(save) == 90);
    }
    // Reject at health boundaries and during a pending fourth-piece conversion.
    for (int capacity : { 16, 320 }) {
        SaveContext save{};
        Init(save, 0, capacity);
        const bool collected = capacity == 320;
        WriteFlag(Container(), save, nullptr, !collected);
        const auto before = save;
        assert(Edit(save, true, collected) != nullptr);
        assert(std::memcmp(&before, &save, sizeof(save)) == 0);
    }
    {
        SaveContext save{};
        Init(save, 4);
        const auto before = save;
        assert(Edit(save, false, true) != nullptr);
        assert(std::memcmp(&before, &save, sizeof(save)) == 0);
    }
    // Sync must use the current scene's flag, and update both copies atomically.
    {
        SaveContext save{};
        Init(save);
        PlayState play{};
        const auto entry = Container();
        play.sceneNum = entry.scene;
        WriteFlag(entry, save, nullptr, true); // stale saved copy
        assert(EditReward(entry, save, &play, true, true) == nullptr);
        assert(Capacity(save) == 80);
        assert(ReadFlag(entry, save, &play) == true);
        assert(ReadFlag(entry, save, nullptr) == true);
    }
    // Hurt Container reverses the health delta without reversing collection credit.
    {
        SaveContext save{};
        Init(save, 3);
        assert(EditReward(Piece(), save, nullptr, true, false, true, true) == nullptr);
        assert(Capacity(save) == 48 && (QuestBits(save) >> 28) == 0);
        assert(save.ship.stats.heartPieces == 13);
        assert(EditReward(Piece(), save, nullptr, false, false, true, true) == nullptr);
        assert(Capacity(save) == 64 && (QuestBits(save) >> 28) == 3);
        assert(save.ship.stats.heartPieces == 12);
    }
    for (int stat : { 0, 255 }) {
        SaveContext save{};
        Init(save, 1);
        save.ship.stats.heartPieces = stat;
        const bool collected = stat == 255;
        WriteFlag(Piece(), save, nullptr, !collected);
        const auto before = save;
        assert(Edit(save, false, collected) != nullptr);
        assert(std::memcmp(&before, &save, sizeof(save)) == 0);
    }

    std::puts("PASS: reward sync rollover, borrow, boss hearts, health clamp, no-op, flag-only and atomic rejection");
}
