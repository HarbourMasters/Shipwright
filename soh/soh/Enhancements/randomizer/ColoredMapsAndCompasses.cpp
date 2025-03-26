#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/ShipInit.hpp"
#include "z64save.h"
#include "objects/object_gi_compass/object_gi_compass.h"
#include "objects/object_gi_map/object_gi_map.h"

extern "C" {
    extern SaveContext gSaveContext;
    #include "variables.h"
    #include "macros.h"
    u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);
}

#define CVAR_COLORED_MAPS_AND_COMPASSES_NAME CVAR_RANDOMIZER_ENHANCEMENT("ColoredMapsAndCompasses")
#define CVAR_COLORED_MAPS_AND_COMPASSES_DEFAULT 1
#define CVAR_COLORED_MAPS_AND_COMPASSES_VALUE CVarGetInteger(CVAR_COLORED_MAPS_AND_COMPASSES_NAME, CVAR_COLORED_MAPS_AND_COMPASSES_DEFAULT)

void OnLoadFileColoredMapsAndCompasses(int32_t _) {
    s8 mapsAndCompassesCanBeOutsideDungeon = IS_RANDO && DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_SHUFFLE_MAPANDCOMPASS);
    s8 isColoredMapsAndCompassesEnabled = mapsAndCompassesCanBeOutsideDungeon && CVAR_COLORED_MAPS_AND_COMPASSES_VALUE;
    if (isColoredMapsAndCompassesEnabled) {
        ResourceMgr_PatchGfxByName(gGiDungeonMapDL, "Map_PrimColor", 5, gsDPNoOp());
        ResourceMgr_PatchGfxByName(gGiDungeonMapDL, "Map_EnvColor", 6, gsDPNoOp());
        ResourceMgr_PatchGfxByName(gGiCompassDL, "Compass_PrimColor", 5, gsDPNoOp());
        ResourceMgr_PatchGfxByName(gGiCompassDL, "Compass_EnvColor", 6, gsDPNoOp());
    } else {
        ResourceMgr_UnpatchGfxByName(gGiDungeonMapDL, "Map_PrimColor");
        ResourceMgr_UnpatchGfxByName(gGiDungeonMapDL, "Map_EnvColor");
        ResourceMgr_UnpatchGfxByName(gGiCompassDL, "Compass_PrimColor");
        ResourceMgr_UnpatchGfxByName(gGiCompassDL, "Compass_EnvColor");
    }
}

void RegisterColoredMapsAndCompasses() {
    COND_HOOK(OnLoadFile, CVAR_COLORED_MAPS_AND_COMPASSES_VALUE, OnLoadFileColoredMapsAndCompasses)

    //Also need to call it directly to patch/unpatch on cvar change
    OnLoadFileColoredMapsAndCompasses(0);
}

static RegisterShipInitFunc initFunc(RegisterColoredMapsAndCompasses, { CVAR_COLORED_MAPS_AND_COMPASSES_NAME });
