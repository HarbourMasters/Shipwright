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

#define ITEM_TRACKER_ITEM(id, data, drawFunc)     \
    {                                             \
        id, #id, #id "_Faded", data, drawFunc     \
    }

typedef struct ItemTrackerDungeon {
    uint32_t id;
    std::vector<uint32_t> items;
} ItemTrackerDungeon;

class ItemTrackerSettingsWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

  protected:
    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};
};

class ItemTrackerWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

protected:
    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};