#pragma once

#include "APCpp/Archipelago.h"
#include "z64.h"
#include <libultraship/libultraship.h>

extern bool OoTAP_ENABLED;
extern bool OoTAP_STARTED;

extern void OoTAP_Enable(const char* ip, int port, const char* slotName, const char* password);
extern void OoTAP_Disable();
extern void OoTAP_Update();
extern void OoTAP_DrawWindow();
extern void OoTAP_SendItem(RandomizerCheck check);
extern void OoTAP_DefeatGanon();
extern void OoTAP_Start();
extern void OoTAP_SendChatMsg(std::string msg);

void OoTAP_DrawStatus();
void OoTAP_ItemClear();
void OoTAP_ItemReceive(int64_t id, bool notify);
void OoTAP_ConnectError(std::string msg);
void OoTAP_LocationChecked(int64_t id);
void OoTAP_UpdateReceivedItems();
bool OoTAP_IsRandoCheckChecked(RandomizerCheck check);
void OoTAP_LocationInfo(std::vector<AP_NetworkItem> locations);

class OoTAP_Window : public LUS::GuiWindow {
public:
    using GuiWindow::GuiWindow;

protected:
    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};

struct AdditionalMessage {
    std::string text;
    int current_duration;
};

extern "C" {
uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);
}

#define CUR_UPG_VALUE(upg) ((s32)(gSaveContext.inventory.upgrades & gUpgradeMasks[upg]) >> gUpgradeShifts[upg])
#define CAPACITY(upg, value) gUpgradeCapacities[upg][value]
#define CUR_CAPACITY(upg) CAPACITY(upg, CUR_UPG_VALUE(upg))
#define FOREST_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_FOREST_TEMPLE) ? 6 : 5)
#define FIRE_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_FIRE_TEMPLE) ? 5 : 8)
#define WATER_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_WATER_TEMPLE) ? 2 : 6)
#define SPIRIT_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_SPIRIT_TEMPLE) ? 7 : 5)
#define SHADOW_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_SHADOW_TEMPLE) ? 6 : 5)
#define BOTTOM_OF_THE_WELL_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_BOTTOM_OF_THE_WELL) ? 2 : 3)
#define GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_GERUDO_TRAINING_GROUND) ? 3 : 9)
#define GERUDO_FORTRESS_SMALL_KEY_MAX 4
#define GANONS_CASTLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_INSIDE_GANONS_CASTLE) ? 3 : 2)
#define TREASURE_GAME_SMALL_KEY_MAX 6
#define SLOT(item) gItemSlots[item]
#define INV_CONTENT(item) gSaveContext.inventory.items[SLOT(item)]