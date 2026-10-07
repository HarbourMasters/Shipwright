#pragma once

#include "HeartPieceFlags.h"
#include <algorithm>

// Save Editor mutations are separate from flag inspection.
namespace HeartPieceViewer {
inline bool WriteFlag(const HeartFlag& entry, SaveContext& save, PlayState* play, bool collected) {
    if (!ReadFlag(entry, save, play).has_value())
        return false;
    const auto update = [collected](auto& value, uint32_t mask) {
        value = collected ? (value | mask) : (value & ~mask);
    };
    const bool current = play != nullptr && play->sceneNum == entry.scene;
    switch (entry.type) {
        case HeartFlagType::Collectible:
            update(save.sceneFlags[entry.scene].collect, uint32_t{ 1 } << entry.flag);
            if (current)
                update(play->actorCtx.flags.collect, uint32_t{ 1 } << entry.flag);
            break;
        case HeartFlagType::Chest:
            update(save.sceneFlags[entry.scene].chest, uint32_t{ 1 } << entry.flag);
            if (current)
                update(play->actorCtx.flags.chest, uint32_t{ 1 } << entry.flag);
            break;
        case HeartFlagType::ItemGet:
            update(save.itemGetInf[entry.flag >> 4], 1u << (entry.flag & 15));
            break;
        case HeartFlagType::Event:
            update(save.eventChkInf[entry.flag >> 4], 1u << (entry.flag & 15));
            break;
        case HeartFlagType::Info:
            update(save.infTable[entry.flag >> 4], 1u << (entry.flag & 15));
            break;
        case HeartFlagType::Fishing:
            update(save.highScores[HS_FISHING], entry.flag);
            break;
    }
    return true;
}
// Apply one reward's delta, never reconstruct health from the full flag list.
// Validate every value before writing so a rejected edit leaves both copies intact.
// Null means success; an error is displayed by the Save Editor.
inline const char* EditReward(const HeartFlag& entry, SaveContext& save, PlayState* play, bool collected,
                              bool isContainer, bool sync = true, bool hurtContainers = false) {
    const auto previous = ReadFlag(entry, save, play);
    if (!previous)
        return "This reward's flag is unavailable.";
    if (!sync || *previous == collected) {
        WriteFlag(entry, save, play, collected);
        return nullptr;
    }
    const int direction = collected ? 1 : -1;
    const int pieces = (save.inventory.questItems >> 28) & 0xF;
    const int capacity = save.healthCapacity;
    if (pieces > 3 || capacity < 16 || capacity > 320 || capacity % 16 != 0)
        return "Health or piece count is outside its normal range. Finish collecting or correct it before syncing.";
    int nextPieces = pieces;
    int heartDelta = isContainer ? direction : 0;
    if (!isContainer) {
        nextPieces += direction;
        if (nextPieces == 4) {
            nextPieces = 0;
            heartDelta = 1;
        } else if (nextPieces == -1) {
            nextPieces = 3;
            heartDelta = -1;
        }
    }
    if (hurtContainers)
        heartDelta = -heartDelta;
    const int nextCapacity = capacity + heartDelta * 16;
    if (nextCapacity < 16 || nextCapacity > 320)
        return "This edit would exceed the 1-20 heart range. Adjust health or use flag-only editing.";
    auto& statistic = isContainer ? save.ship.stats.heartContainers : save.ship.stats.heartPieces;
    const int nextStatistic = statistic + direction;
    if (nextStatistic < 0 || nextStatistic > 255)
        return "Reward statistic is at its limit. Correct it or use flag-only editing.";
    WriteFlag(entry, save, play, collected);
    save.inventory.questItems = (save.inventory.questItems & 0x0FFFFFFFu) | (uint32_t(nextPieces) << 28);
    save.healthCapacity = nextCapacity;
    // Match the native health gain, but only clamp current health when capacity falls.
    if (heartDelta != 0)
        save.health = std::clamp(int(save.health) + std::max(heartDelta, 0) * 16, 0, nextCapacity);
    statistic = nextStatistic;
    return nullptr;
}
} // namespace HeartPieceViewer
