#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include <variables.h>
#include <overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h>
}

#define CVAR_NAME_RUPEES "gEnhancements.Cycle.DoNotResetRupees"
#define CVAR_RUPEES CVarGetInteger(CVAR_NAME_RUPEES, 0)
#define CVAR_NAME_CONSUME "gEnhancements.Cycle.DoNotResetConsumables"
#define CVAR_CONSUME CVarGetInteger(CVAR_NAME_CONSUME, 0)
#define CVAR_NAME_BOTTLE "gEnhancements.Cycle.DoNotResetBottleContent"
#define CVAR_BOTTLE CVarGetInteger(CVAR_NAME_BOTTLE, 0)
#define CVAR_NAME_SWORD "gEnhancements.Cycle.DoNotResetRazorSword"
#define CVAR_SWORD CVarGetInteger(CVAR_NAME_SWORD, 0)
#define CVAR_NAME_TIME "gEnhancements.Cycle.DoNotResetTimeSpeed"
#define CVAR_TIME CVarGetInteger(CVAR_NAME_TIME, 0)
#define CVAR_NAME_CHATEAU "gEnhancements.Cycle.DoNotResetChateau"
#define CVAR_CHATEAU CVarGetInteger(CVAR_NAME_CHATEAU, 0)
#define CVAR_NAME_SCARECROW "gEnhancements.Cycle.DoNotResetScarecrowSong"
#define CVAR_SCARECROW CVarGetInteger(CVAR_NAME_SCARECROW, 0)

SaveInfo saveInfoCopy;
ShipSaveInfo shipSaveInfoCopy;
s32 timeSpeedOffsetCopy = 0;

static void RegisterBeforeEndOfCycleSaveHook() {
    COND_HOOK(BeforeEndOfCycleSave, true, []() {
        memcpy(&saveInfoCopy, &gSaveContext.save.saveInfo, sizeof(SaveInfo));
        memcpy(&shipSaveInfoCopy, &gSaveContext.save.shipSaveInfo, sizeof(ShipSaveInfo));
        timeSpeedOffsetCopy = gSaveContext.save.timeSpeedOffset;
    });
}

static RegisterShipInitFunc initFunc_Before(RegisterBeforeEndOfCycleSaveHook);

static void RegisterEndOfCycleSaveHook_Rupees() {
    COND_HOOK(AfterEndOfCycleSave, CVAR_RUPEES, []() {
        gSaveContext.save.saveInfo.playerData.rupees = saveInfoCopy.playerData.rupees;
        CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_RUPEES);
    });
}

static RegisterShipInitFunc initFunc_Rupees(RegisterEndOfCycleSaveHook_Rupees, { CVAR_NAME_RUPEES });

static void RegisterEndOfCycleSaveHook_Consumables() {
    COND_HOOK(AfterEndOfCycleSave, CVAR_CONSUME, []() {
        if (INV_CONTENT(ITEM_BOMB) == ITEM_BOMB) {
            INV_CONTENT(ITEM_BOMB) = saveInfoCopy.inventory.items[ITEM_BOMB];
            AMMO(ITEM_BOMB) = saveInfoCopy.inventory.ammo[ITEM_BOMB];
        }

        if (INV_CONTENT(ITEM_DEKU_NUT) == ITEM_DEKU_NUT) {
            INV_CONTENT(ITEM_DEKU_NUT) = saveInfoCopy.inventory.items[ITEM_DEKU_NUT];
            AMMO(ITEM_DEKU_NUT) = saveInfoCopy.inventory.ammo[ITEM_DEKU_NUT];
        }

        if (INV_CONTENT(ITEM_DEKU_STICK) == ITEM_DEKU_STICK) {
            INV_CONTENT(ITEM_DEKU_STICK) = saveInfoCopy.inventory.items[ITEM_DEKU_STICK];
            AMMO(ITEM_DEKU_STICK) = saveInfoCopy.inventory.ammo[ITEM_DEKU_STICK];
        }

        if (INV_CONTENT(ITEM_BOW) == ITEM_BOW) {
            INV_CONTENT(ITEM_BOW) = saveInfoCopy.inventory.items[ITEM_BOW];
            AMMO(ITEM_BOW) = saveInfoCopy.inventory.ammo[ITEM_BOW];
        }

        for (int i = 0; i < ITEM_NUM_SLOTS; i++) {
            if (gAmmoItems[i] != ITEM_NONE) {
                if ((gSaveContext.save.saveInfo.inventory.items[i] != ITEM_NONE) && (i != SLOT_PICTOGRAPH_BOX)) {
                    gSaveContext.save.saveInfo.inventory.items[i] = saveInfoCopy.inventory.items[i];
                    gSaveContext.save.saveInfo.inventory.ammo[i] = saveInfoCopy.inventory.ammo[i];
                }
            }
        }

        CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_BOMB_AMMO);
        CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_NUT_AMMO);
        CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_STICK_AMMO);
        CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_ARROW_AMMO);
    });
}

static RegisterShipInitFunc initFunc_Consumables(RegisterEndOfCycleSaveHook_Consumables, { CVAR_NAME_CONSUME });

static void RegisterEndOfCycleSaveHook_Bottles() {
    COND_HOOK(AfterEndOfCycleSave, CVAR_BOTTLE, []() {
        int stolenBottles = (((saveInfoCopy.stolenItems & 0xFF000000) >> 0x18) == ITEM_BOTTLE) +
                            (((saveInfoCopy.stolenItems & 0x00FF0000) >> 0x10) == ITEM_BOTTLE);

        // Replace bottles back, accounting for any stolen bottles
        for (int i = SLOT_BOTTLE_1; i <= SLOT_BOTTLE_6; i++) {
            gSaveContext.save.saveInfo.inventory.items[i] = saveInfoCopy.inventory.items[i];

            if (stolenBottles > 0 && saveInfoCopy.inventory.items[i] == ITEM_NONE) {
                stolenBottles--;
                gSaveContext.save.saveInfo.inventory.items[i] = ITEM_BOTTLE;
            }
        }

        // Set back button equips to the correct bottle type
        for (int j = EQUIP_SLOT_C_LEFT; j <= EQUIP_SLOT_C_RIGHT; j++) {
            if (GET_CUR_FORM_BTN_ITEM(j) == ITEM_BOTTLE) {
                SET_CUR_FORM_BTN_ITEM(j, saveInfoCopy.equips.buttonItems[0][j]);
            }
        }
        for (int j = EQUIP_SLOT_D_RIGHT; j <= EQUIP_SLOT_D_UP; j++) {
            if (DPAD_GET_CUR_FORM_BTN_ITEM(j) == ITEM_BOTTLE) {
                DPAD_SET_CUR_FORM_BTN_ITEM(j, shipSaveInfoCopy.dpadEquips.dpadItems[0][j]);
            }
        }
    });
}

static RegisterShipInitFunc initFunc_Bottles(RegisterEndOfCycleSaveHook_Bottles, { CVAR_NAME_BOTTLE });

static void RegisterEndOfCycleSaveHook_Chateau() {
    COND_HOOK(AfterEndOfCycleSave, CVAR_CHATEAU, []() {
        if (saveInfoCopy.weekEventReg[((WEEKEVENTREG_DRANK_CHATEAU_ROMANI) >> 8)] &
            ((WEEKEVENTREG_DRANK_CHATEAU_ROMANI)&0xFF)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_DRANK_CHATEAU_ROMANI);
        }
    });
}

static RegisterShipInitFunc initFunc_Chateau(RegisterEndOfCycleSaveHook_Chateau, { CVAR_NAME_CHATEAU });

static void RegisterEndOfCycleSaveHook_Sword() {
    COND_HOOK(AfterEndOfCycleSave, CVAR_SWORD || IS_RANDO, []() {
        u8 curSword = (saveInfoCopy.equips.equipment & gEquipMasks[EQUIP_TYPE_SWORD]) >> gEquipShifts[EQUIP_TYPE_SWORD];

        // Check for razor sword equipped, stolen, or turned into the smithy
        if (curSword == EQUIP_VALUE_SWORD_RAZOR ||
            (curSword == EQUIP_VALUE_SWORD_NONE &&
             ((saveInfoCopy.permanentSceneFlags[SCENE_KAJIYA].unk_14 & 4) ||
              (((saveInfoCopy.stolenItems & 0xFF000000) >> 0x18) == ITEM_SWORD_RAZOR) ||
              (((saveInfoCopy.stolenItems & 0x00FF0000) >> 0x10) == ITEM_SWORD_RAZOR)))) {

            SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_RAZOR);
            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_SWORD_RAZOR;
        }
    });
}

static RegisterShipInitFunc initFunc_Sword(RegisterEndOfCycleSaveHook_Sword, { CVAR_NAME_SWORD, "IS_RANDO" });

static void RegisterEndOfCycleSaveHook_Time() {
    COND_HOOK(AfterEndOfCycleSave, CVAR_TIME, []() { gSaveContext.save.timeSpeedOffset = timeSpeedOffsetCopy; });
}

static RegisterShipInitFunc initFunc_Time(RegisterEndOfCycleSaveHook_Time, { CVAR_NAME_TIME });

static void RegisterEndOfCycleSaveHook_ScarecrowSong() {
    COND_HOOK(AfterEndOfCycleSave, CVAR_SCARECROW, []() {
        if (!saveInfoCopy.scarecrowSpawnSongSet) {
            return;
        }

        gSaveContext.save.saveInfo.scarecrowSpawnSongSet = true;
        memcpy(gSaveContext.save.saveInfo.scarecrowSpawnSong, saveInfoCopy.scarecrowSpawnSong,
               sizeof(gSaveContext.save.saveInfo.scarecrowSpawnSong));
        memcpy(gScarecrowSpawnSongPtr, saveInfoCopy.scarecrowSpawnSong,
               sizeof(gSaveContext.save.saveInfo.scarecrowSpawnSong));

        if (saveInfoCopy.weekEventReg[(WEEKEVENTREG_79_08 >> 8)] & (WEEKEVENTREG_79_08 & 0xFF)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_79_08);
        }
    });

    COND_HOOK(OnSaveLoad, CVAR_SCARECROW, [](s16 fileNum) {
        if (!gSaveContext.save.isOwlSave && gSaveContext.save.saveInfo.scarecrowSpawnSongSet) {
            memcpy(gScarecrowSpawnSongPtr, gSaveContext.save.saveInfo.scarecrowSpawnSong,
                   sizeof(gSaveContext.save.saveInfo.scarecrowSpawnSong));
        }
    });
}

static RegisterShipInitFunc initFunc_ScarecrowSong(RegisterEndOfCycleSaveHook_ScarecrowSong, { CVAR_NAME_SCARECROW });
