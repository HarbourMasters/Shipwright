#include <libultraship/bridge/consolevariablebridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
}

#define CVAR_WIDE_SHUTTER_DOOR_RANGE CVAR_ENHANCEMENT("WideShutterDoorRange")
#define WIDE_SHUTTER_DOOR_RANGE_DEFAULT 0
#define CVAR_WIDE_SHUTTER_DOOR_RANGE_VALUE CVarGetInteger(CVAR_WIDE_SHUTTER_DOOR_RANGE, WIDE_SHUTTER_DOOR_RANGE_DEFAULT)

// The X range is 70 on NTSC 1.0 and 50 in later versions. The Y range is 15 in any version.
// https://github.com/zeldaret/oot/blob/6ecb84097c1a9a8426f3815c84aa6a5d49ad5804/src/overlays/actors/ovl_Door_Shutter/z_door_shutter.c#L248-L259
#define SHUTTER_DOOR_RANGE_X 70
#define SHUTTER_DOOR_RANGE_Y 15

void RegisterWideShutterDoorRange() {
    COND_VB_SHOULD(VB_BE_NEAR_DOOR_SHUTTER, CVAR_WIDE_SHUTTER_DOOR_RANGE_VALUE, {
        DoorShutter* doorShutter = va_arg(args, DoorShutter*);
        Vec3f relPlayerPos = *va_arg(args, Vec3f*);
        if (doorShutter->gfxType == SHUTTER_BACK_LOCKED || doorShutter->gfxType == SHUTTER_PG_BARS ||
            doorShutter->gfxType == SHUTTER_BOSS || doorShutter->gfxType == SHUTTER_GOHMA_BLOCK) {
            *should = (SHUTTER_DOOR_RANGE_X < fabsf(relPlayerPos.x) || SHUTTER_DOOR_RANGE_Y < fabsf(relPlayerPos.y));
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterWideShutterDoorRange, { CVAR_WIDE_SHUTTER_DOOR_RANGE });
