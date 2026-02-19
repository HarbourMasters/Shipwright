#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
#include "macros.h"
#include "functions.h"
}

#define CVAR_BOW_RETICLE_NAME CVAR_ENHANCEMENT("BowReticle")
#define CVAR_BOW_RETICLE_DEFAULT 0
#define CVAR_BOW_RETICLE_VALUE CVarGetInteger(CVAR_BOW_RETICLE_NAME, CVAR_BOW_RETICLE_DEFAULT)

#define CVAR_BOOMERANG_RETICLE_NAME CVAR_ENHANCEMENT("BoomerangReticle")
#define CVAR_BOOMERANG_RETICLE_DEFAULT 0
#define CVAR_BOOMERANG_RETICLE_VALUE CVarGetInteger(CVAR_BOOMERANG_RETICLE_NAME, CVAR_BOOMERANG_RETICLE_DEFAULT)

// OTRTODO: Figure out why this value works/what this value should be
// This was originally obtained by working down from FLT_MAX until the math
// started working out properly
#define RETICLE_MAX 3.402823466e+12f

const Vec3s BoomerangViewAdult = { -31200, -9200, 17000 };
const Vec3s BoomerangViewChild = { -31200, -8700, 17000 };

s32 Player_HoldsBoomerang(Player* player) {
    return player->heldItemAction == PLAYER_IA_BOOMERANG;
}

s32 Player_AimsBoomerang(Player* player) {
    return Player_HoldsBoomerang(player) && (player->unk_834 != 0);
}

void RegisterAdditionalReticles() {
    bool shouldRegister = CVAR_BOW_RETICLE_VALUE || CVAR_BOOMERANG_RETICLE_VALUE;

    COND_VB_SHOULD(VB_DRAW_ADDITIONAL_RETICLES, shouldRegister, {
        Player* player = GET_PLAYER(gPlayState);
        Actor* heldActor = player->heldActor;
        if (CVAR_BOW_RETICLE_VALUE &&
            ((player->heldItemAction >= PLAYER_IA_BOW && player->heldItemAction <= PLAYER_IA_BOW_LIGHT) ||
             player->heldItemAction == PLAYER_IA_SLINGSHOT)) {
            if (heldActor != NULL) {
                MtxF sp44;
                s32 pad;

                Matrix_RotateZYX(0, -15216, -17496, MTXMODE_APPLY);
                Matrix_Get(&sp44);

                if (func_8002DD78(player) != 0) {
                    Matrix_Translate(500.0f, 300.0f, 0.0f, MTXMODE_APPLY);
                    Player_DrawHookshotReticle(gPlayState, player, RETICLE_MAX);
                }
            }
        } else if (CVAR_BOOMERANG_RETICLE_VALUE && player->heldItemAction == PLAYER_IA_BOOMERANG) {
            if (Player_HoldsBoomerang(player)) {
                if (LINK_IS_ADULT) {
                    Matrix_RotateZYX(BoomerangViewAdult.x, BoomerangViewAdult.y, BoomerangViewAdult.z, MTXMODE_APPLY);
                } else {
                    Matrix_RotateZYX(BoomerangViewChild.x, BoomerangViewChild.y, BoomerangViewChild.z, MTXMODE_APPLY);
                }

                if (Player_AimsBoomerang(player)) {
                    Matrix_Translate(500.0f, 300.0f, 0.0f, MTXMODE_APPLY);
                    Player_DrawHookshotReticle(gPlayState, player, RETICLE_MAX);
                }
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterAdditionalReticles,
                                     { CVAR_ENHANCEMENT("BowReticle"), CVAR_ENHANCEMENT("BoomerangReticle") });