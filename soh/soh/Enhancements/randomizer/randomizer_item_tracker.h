#pragma once

#include "ship/window/gui/GuiWindow.h"
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>
#include <stdint.h>
#include <array>

void DrawItemAmmo(int itemId);
void RefreshItemTrackerMainWindow();

typedef enum ItemKind {
    ITEM_KIND_ITEM,
    ITEM_KIND_QUEST,
    ITEM_KIND_RG,
    ITEM_KIND_SPECIAL,
    ITEM_KIND_DUMMY,
} ItemKind;

typedef struct ItemTrackerItem {
    uint32_t id;
    ItemKind kind;
    std::string displayName;
    std::string iconName;
    std::string fadedIconName;
    uint32_t data;
    void (*drawFunc)(ItemTrackerItem);
} ItemTrackerItem;

bool HasSong(ItemTrackerItem);
bool HasQuestItem(ItemTrackerItem);
bool HasEquipment(ItemTrackerItem);

#define ITEM_TRACKER_ITEM(id, displayName, data, drawFunc) \
    { id, ITEM_KIND_ITEM, displayName, #id, #id "_Faded", data, drawFunc }
#define ITEM_TRACKER_ITEM_CUSTOM(id, displayName, iconName, data, drawFunc) \
    { id, ITEM_KIND_ITEM, displayName, #iconName, #iconName "_Faded", data, drawFunc }

#define ITEM_TRACKER_QUEST(id, displayName, data, drawFunc) \
    { id, ITEM_KIND_QUEST, displayName, #id, #id "_Faded", data, drawFunc }
#define ITEM_TRACKER_QUEST_CUSTOM(id, displayName, iconName, data, drawFunc) \
    { id, ITEM_KIND_QUEST, displayName, #iconName, #iconName "_Faded", data, drawFunc }

#define ITEM_TRACKER_RG(id, displayName, data, drawFunc) \
    { id, ITEM_KIND_RG, displayName, #id, #id "_Faded", data, drawFunc }
#define ITEM_TRACKER_RG_CUSTOM(id, displayName, iconName, data, drawFunc) \
    { id, ITEM_KIND_RG, displayName, #iconName, #iconName "_Faded", data, drawFunc }

#define IM_COL_WHITE IM_COL32(255, 255, 255, 255)
#define IM_COL_RED IM_COL32(255, 0, 0, 255)
#define IM_COL_GREEN IM_COL32(0, 255, 0, 255)
#define IM_COL_GRAY IM_COL32(155, 155, 155, 255)
#define IM_COL_PURPLE IM_COL32(180, 90, 200, 255)
#define IM_COL_LIGHT_YELLOW IM_COL32(255, 255, 130, 255)

static std::array<const char*, 16> itemTrackerWindowIDs = { "Item Tracker",
                                                            "Inventory Items Tracker",
                                                            "Equipment Items Tracker",
                                                            "Misc Items Tracker",
                                                            "Dungeon Rewards Tracker",
                                                            "Songs Tracker",
                                                            "Dungeon Items Tracker",
                                                            "Greg Tracker",
                                                            "Triforce Piece Tracker",
                                                            "Boss Soul Tracker",
                                                            "Ocarina Button Tracker",
                                                            "Overworld Key Tracker",
                                                            "Silver Rupee Tracker",
                                                            "Fishing Pole Tracker",
                                                            "Personal Notes",
                                                            "Total Checks" };
void ItemTracker_LoadFromPreset(const nlohmann::json& trackerInfo);

typedef struct ItemTrackerDungeon {
    uint32_t id;
    std::vector<uint32_t> items;
} ItemTrackerDungeon;

class ItemTrackerSettingsWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

  protected:
    void InitElement() override{};
    void DrawElement() override;
    void UpdateElement() override{};
};

class ItemTrackerWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    void Draw() override;

  protected:
    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};
