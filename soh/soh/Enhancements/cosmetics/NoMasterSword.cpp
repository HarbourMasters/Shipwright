#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"

extern PlayState* gPlayState;
extern void Player_StartMode_Idle(PlayState*, Player*);
extern u8 Randomizer_GetSettingValue(RandomizerSettingKey);
}

// copied from z_player (right above Player_StartMode_TimeTravel)
static Vec3f D_808546F4 = { -1.0f, 69.0f, 20.0f };

void RegisterNoMasterSword() {
    // skip post pedestal animation when we don't have a master sword
    REGISTER_VB_SHOULD(VB_EXECUTE_PLAYER_STARTMODE_FUNC, {
        int32_t startMode = va_arg(args, int32_t);
        Player* player = GET_PLAYER(gPlayState);

        if (startMode == PLAYER_START_MODE_TIME_TRAVEL && IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_MASTER_SWORD) &&
            !CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER)) {
            // don't run the vanilla startMode func
            *should = false;
            
            // position link correctly
            Math_Vec3f_Copy(&player->actor.world.pos, &D_808546F4);
            player->yaw = player->actor.shape.rot.y = -0x8000;
            
            // execute the idle startMode func
            Player_StartMode_Idle(gPlayState, player);
        }
    });
}

static RegisterShipInitFunc initFuncAlways(RegisterNoMasterSword);
