#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <libultraship/libultraship.h>

void DrawItemAmmo(int itemId);

typedef struct ItemTrackerItem {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    uint32_t data;
    void (*drawFunc)(ItemTrackerItem);
} ItemTrackerItem;

bool HasSong(ItemTrackerItem);
bool HasQuestItem(ItemTrackerItem);
bool HasEquipment(ItemTrackerItem);

#define ITEM_TRACKER_ITEM(id, data, drawFunc) \
    { id, #id, #id "_Faded", data, drawFunc }

#define ITEM_TRACKER_ITEM_CUSTOM(id, name, nameFaded, data, drawFunc) \
    { id, #name, #nameFaded "_Faded", data, drawFunc }

static std::vector<const char*> itemTrackerWindowIDs = { "Item Tracker",
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
                                                         "Fishing Pole Tracker",
                                                         "Personal Notes",
                                                         "Total Checks" };
void ItemTracker_LoadFromPreset(nlohmann::json trackerInfo);

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