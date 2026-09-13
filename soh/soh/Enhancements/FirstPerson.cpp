#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

#include <math.h>

extern "C" {
#include "global.h"
void func_8084029C(Player* player, f32 arg1);
}

#define CVAR_MOVE_IN_FIRST_PERSON_NAME CVAR_SETTING("MoveInFirstPerson")
#define CVAR_RIGHT_STICK_AIM_NAME CVAR_SETTING("Controls.RightStickAim")

static void MoveInFirstPerson(Player* player, Input* input) {
    float relX = (input->rel.stick_x * (CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0) ? 1 : -1)) / 10.0f;
    float relY = input->rel.stick_y / 10.0f;

    // Normalize so that diagonal movement isn't faster
    float relMag = sqrtf((relX * relX) + (relY * relY));
    if (relMag > 1.0f) {
        relX /= relMag;
        relY /= relMag;
    }

    // Determine what left and right mean based on camera angle
    int16_t focusYaw = player->actor.focus.rot.y;
    float relX2 = relX * Math_CosS(focusYaw) + relY * Math_SinS(focusYaw);
    float relY2 = relY * Math_CosS(focusYaw) - relX * Math_SinS(focusYaw);
    int16_t moveYaw = Math_Atan2S(relY2, relX2);

    // Use the same speed cap as normal movement, so boots, swimming, quicksand, walls and slopes still slow Link
    // down. Walls and slopes are checked along the movement direction, since Link may not face where he moves.
    f32 movementSpeed = R_RUN_SPEED_LIMIT / 100.0f;
    if ((player->stateFlags1 & PLAYER_STATE1_IN_WATER) && (player->currentBoots != PLAYER_BOOTS_IRON)) {
        movementSpeed *= 0.8f;
        GameInteractor_Should(VB_PLAYER_MODIFY_SWIM_SPEED, true, player, &movementSpeed, true);
    } else {
        if ((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (player->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
            int16_t wallYawDiff = moveYaw - (int16_t)(player->actor.wallYaw + 0x8000);
            float wallScale = ABS(wallYawDiff) * 0.00008f;
            if (wallScale < 1.0f) {
                movementSpeed = CLAMP_MIN(movementSpeed * wallScale, 0.1f);
            }
        }
        if (player->unk_6C4 != 0.0f) {
            movementSpeed = CLAMP_MIN(movementSpeed - player->unk_6C4 * 0.008f, 2.0f);
        }
        if ((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (player->actor.floorPoly != NULL)) {
            CollisionPoly* floorPoly = player->actor.floorPoly;
            float slope = (-(COLPOLY_GET_NORMAL(floorPoly->normal.x) * Math_SinS(moveYaw)) -
                           (COLPOLY_GET_NORMAL(floorPoly->normal.z) * Math_CosS(moveYaw))) /
                          COLPOLY_GET_NORMAL(floorPoly->normal.y);
            float uphill = CLAMP(Math_SinS(Math_Atan2S(1.0f, slope)), 0.0f, 0.6f);
            movementSpeed = CLAMP_MIN(movementSpeed - (8.0f * uphill * uphill), 0.0f);
        }
        GameInteractor_Should(VB_PLAYER_MODIFY_FIRST_PERSON_SPEED, true, player, &movementSpeed);
    }
    // Speed to distance per update, same as Actor_UpdatePos
    movementSpeed *= R_UPDATE_RATE * 0.5f;

    // Calculate distance for footstep sound
    float distance = sqrtf((relX2 * relX2) + (relY2 * relY2)) * movementSpeed;
    func_8084029C(player, distance / 4.5f);

    // No collision push here, Actor_UpdatePos already added it this frame
    player->actor.world.pos.x += relX2 * movementSpeed;
    player->actor.world.pos.z += relY2 * movementSpeed;
}

static void RegisterFirstPerson() {
    bool moveInFirstPerson =
        CVarGetInteger(CVAR_MOVE_IN_FIRST_PERSON_NAME, 0) && CVarGetInteger(CVAR_RIGHT_STICK_AIM_NAME, 0);

    // Left stick moves instead of aiming, the right stick aims
    COND_VB_SHOULD(VB_PLAYER_AIM_WITH_LEFT_STICK, moveInFirstPerson, { *should = false; });

    COND_VB_SHOULD(VB_PLAYER_MOVE_IN_FIRST_PERSON, moveInFirstPerson, {
        Player* player = va_arg(args, Player*);
        Input* input = va_arg(args, Input*);
        MoveInFirstPerson(player, input);
    });
}

static RegisterShipInitFunc initFunc(RegisterFirstPerson,
                                     { CVAR_MOVE_IN_FIRST_PERSON_NAME, CVAR_RIGHT_STICK_AIM_NAME });
