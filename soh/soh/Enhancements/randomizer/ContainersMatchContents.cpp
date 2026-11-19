#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/item-tables/ItemTableManager.h"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/ObjectExtension/ObjectExtension.h"
#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/ShipInit.hpp"
#include "item_category_adj.h"

extern "C" {
#include "soh_assets.h"
#include "objects/object_box/object_box.h"
#include "src/overlays/actors/ovl_En_Box/z_en_box.h"
extern PlayState* gPlayState;
}

static constexpr const char* CVAR_CMC = CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents");
static constexpr const char* CVAR_CMC_AGONY = CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony");

struct ChestContents {
    s16 sceneNum;
    s16 params;
    GetItemEntry getItemEntry;
};
static ObjectExtension::Register<ChestContents> ChestContentsRegister;

static GetItemEntry EnBox_GetContents(EnBox* chest) {
    Actor* actor = &chest->dyna.actor;
    const ChestContents* contents = ObjectExtension::GetInstance().Get<ChestContents>(actor);
    if (contents != nullptr && contents->sceneNum == gPlayState->sceneNum && contents->params == actor->params) {
        return contents->getItemEntry;
    }

    GetItemID vanillaGetItemId = static_cast<GetItemID>(actor->params >> 5 & 0x7F);
    GetItemEntry getItemEntry = ItemTableManager::Instance->RetrieveItemEntry(MOD_NONE, vanillaGetItemId);
    RandomizerCheck rc =
        OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actor->id, gPlayState->sceneNum, actor->params);
    if (rc != RC_UNKNOWN_CHECK) {
        getItemEntry = OTRGlobals::Instance->gRandomizer->GetItemFromKnownCheck(rc, vanillaGetItemId);
    }

    ObjectExtension::GetInstance().Set<ChestContents>(
        actor, ChestContents{ gPlayState->sceneNum, actor->params, getItemEntry });
    return getItemEntry;
}

static void EnBox_DrawContentsDL(EnBox* chest, s32 limbIndex, Gfx** gfx, bool* should) {
    if (CVarGetInteger(CVAR_CMC_AGONY, 0) && !CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY)) {
        return;
    }

    // Exclude treasure game chests except for the final room, unless the chest game is shuffled
    if (gPlayState->sceneNum == SCENE_TREASURE_BOX_SHOP && chest->dyna.actor.room != 6 &&
        !RAND_GET_OPTION(RSK_SHUFFLE_CHEST_MINIGAME)) {
        return;
    }

    const char* bodyDL;
    const char* lidDL;
    switch (Randomizer_AdjustItemCategory(EnBox_GetContents(chest))) {
        case ITEM_CATEGORY_MAJOR:
            bodyDL = gChestBodyMajorDL;
            lidDL = gChestLidMajorDL;
            break;
        case ITEM_CATEGORY_SKULLTULA_TOKEN:
            bodyDL = gChestBodyTokenDL;
            lidDL = gChestLidTokenDL;
            break;
        case ITEM_CATEGORY_SMALL_KEY:
            bodyDL = gChestBodySmallKeyDL;
            lidDL = gChestLidSmallKeyDL;
            break;
        case ITEM_CATEGORY_BOSS_KEY:
            bodyDL = gTreasureChestBossKeyChestFrontDL;
            lidDL = gTreasureChestBossKeyChestSideAndTopDL;
            break;
        case ITEM_CATEGORY_HEALTH:
            bodyDL = gChestBodyHeartDL;
            lidDL = gChestLidHeartDL;
            break;
        case ITEM_CATEGORY_LESSER:
            bodyDL = gChestBodyMinorDL;
            lidDL = gChestLidMinorDL;
            break;
        case ITEM_CATEGORY_JUNK:
        default:
            bodyDL = gChestBodyJunkDL;
            lidDL = gChestLidJunkDL;
            break;
    }

    Gfx* dl = ResourceMgr_LoadGfxByName(limbIndex == 1 ? bodyDL : lidDL);
    if (dl == nullptr) {
        return;
    }

    gSPDisplayList((*gfx)++, dl);
    *should = false;
}

static void RegisterContainersMatchContentsChests() {
    COND_VB_SHOULD(VB_DRAW_CHEST_BODY_AND_LID, IS_RANDO && CVarGetInteger(CVAR_CMC, 0), {
        EnBox* chest = va_arg(args, EnBox*);
        s32 limbIndex = va_arg(args, s32);
        Gfx** gfx = va_arg(args, Gfx**);
        EnBox_DrawContentsDL(chest, limbIndex, gfx, should);
    });
}

static RegisterShipInitFunc initFunc(RegisterContainersMatchContentsChests, { CVAR_CMC, "IS_RANDO" });
