#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "global.h"
extern PlayState* gPlayState;
void func_8083DF68(Player* player, f32 arg1, s16 arg2);
void func_8084029C(Player* player, f32 arg1);
s32 Player_GetMovementSpeedAndYaw(Player* player, f32* outSpeedTarget, s16* outYawTarget, f32 speedMode,
                                  PlayState* play);
}

#define CVAR_MOVE_IN_FIRST_PERSON_NAME CVAR_SETTING("MoveInFirstPerson")
#define CVAR_RIGHT_STICK_AIM_NAME CVAR_SETTING("Controls.RightStickAim")

// from z_player.c
#define SPEED_MODE_CURVED 0.018f

static void MoveInFirstPerson(Player* player) {
    f32 speedTarget;
    s16 yawTarget;

    // Normal movement ignores the stick while unk_6AD says Link is in first person
    uint8_t firstPersonMode = player->unk_6AD;
    player->unk_6AD = 0;
    bool moving = Player_GetMovementSpeedAndYaw(player, &speedTarget, &yawTarget, SPEED_MODE_CURVED, gPlayState);
    player->unk_6AD = firstPersonMode;

    if ((player->stateFlags1 & PLAYER_STATE1_IN_WATER) && (player->currentBoots != PLAYER_BOOTS_IRON)) {
        // Swim speed cap from func_8084AEEC. Its speed step only runs on swim stroke frames, so it isn't used here.
        f32 swimSpeed = R_RUN_SPEED_LIMIT / 100.0f * 0.8f;
        GameInteractor_Should(VB_PLAYER_MODIFY_SWIM_SPEED, true, player, &swimSpeed, true);
        speedTarget = CLAMP_MAX(speedTarget, swimSpeed);
    } else {
        GameInteractor_Should(VB_PLAYER_MODIFY_FIRST_PERSON_SPEED, true, player, &speedTarget);
    }

    // Turn instantly so strafing feels direct
    if (moving) {
        player->yaw = yawTarget;
    }
    func_8083DF68(player, speedTarget, player->yaw);

    if (!(player->stateFlags1 & PLAYER_STATE1_IN_WATER) && (player->linearVelocity > 0.0f)) {
        func_8084029C(player, (REG(30) / 1000.0f) + ((REG(32) / 1000.0f) * player->linearVelocity));
    }
}

static void RegisterFirstPerson() {
    bool moveInFirstPerson =
        CVarGetInteger(CVAR_MOVE_IN_FIRST_PERSON_NAME, 0) && CVarGetInteger(CVAR_RIGHT_STICK_AIM_NAME, 0);

    // Left stick moves instead of aiming; right stick aims
    COND_VB_SHOULD(VB_PLAYER_AIM_WITH_LEFT_STICK, moveInFirstPerson, { *should = false; });

    COND_VB_SHOULD(VB_PLAYER_FIRST_PERSON_DECELERATE, moveInFirstPerson, {
        Player* player = va_arg(args, Player*);
        MoveInFirstPerson(player);
        *should = false;
    });

    // Keep moving where stick points rather than where Link faces. Once stopped, face forward as normal.
    COND_VB_SHOULD(VB_PLAYER_FIRST_PERSON_ALIGN_YAW, moveInFirstPerson, {
        Player* player = va_arg(args, Player*);
        if (player->linearVelocity != 0.0f) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFirstPerson,
                                     { CVAR_MOVE_IN_FIRST_PERSON_NAME, CVAR_RIGHT_STICK_AIM_NAME });
