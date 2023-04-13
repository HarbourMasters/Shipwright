#pragma once

#include <string>
#include <vector>

void InitItemTracker();
void DrawItemTracker(bool& open);
void DrawItemAmmo(int itemId);

typedef struct ItemTrackerItem {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    uint32_t data;
    void (*drawFunc)(ItemTrackerItem);
} ItemTrackerItem;

std::vector<ItemTrackerItem> dungeonRewardStones;
std::vector<ItemTrackerItem> dungeonRewardMedallions;
std::vector<ItemTrackerItem> songItems;

bool HasSong(ItemTrackerItem);
bool HasQuestItem(ItemTrackerItem);

#define ITEM_TRACKER_ITEM(id, data, drawFunc)     \
    {                                             \
        id, #id, #id "_Faded", data, drawFunc     \
    }

typedef struct ItemTrackerDungeon {
    uint32_t id;
    std::vector<uint32_t> items;
} ItemTrackerDungeon;
