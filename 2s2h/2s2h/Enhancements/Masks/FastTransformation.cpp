#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"

void Player_PlaySfx(Player* player, u16 sfxId);
void PlayerCall_Init(Actor* thisx, PlayState* play);
void PlayerCall_Update(Actor* thisx, PlayState* play);
void PlayerCall_Draw(Actor* thisx, PlayState* play);
void TransitionFade_SetColor(void* thisx, u32 color);
}

#define CVAR_NAME "gEnhancements.Masks.FastTransformation"
#define CVAR CVarGetInteger(CVAR_NAME, FAST_TRANSFORM_OFF)

// Copy of `D_8085D908` in `z_player.c`
static u16 sFormCheckFlags[] = {
    WEEKEVENTREG_30_80, // PLAYER_FORM_FIERCE_DEITY
    WEEKEVENTREG_30_20, // PLAYER_FORM_GORON
    WEEKEVENTREG_30_40, // PLAYER_FORM_ZORA
    WEEKEVENTREG_30_10, // PLAYER_FORM_DEKU
};

// Externed so Easy Mask Equip can also use it
extern bool ShouldFastTransform() {
    switch (CVAR) {
        case FAST_TRANSFORM_OFF:
            return false;

        case FAST_TRANSFORM_ON:
            return true;

        case FAST_TRANSFORM_AFTER_FIRST:
            Player* player = GET_PLAYER(gPlayState);
            return (player->transformation < PLAYER_FORM_HUMAN) || CHECK_WEEKEVENTREG(sFormCheckFlags[GET_PLAYER_FORM]);
    }

    return false;
}

static void RegisterFastTransformation() {
    COND_VB_SHOULD(VB_PREVENT_MASK_TRANSFORMATION_CS, CVAR, {
        if (ShouldFastTransform()) {
            *should = true;
            Player* player = GET_PLAYER(gPlayState);

            // This was mostly copied directly from func_8012301C within z_player_lib.c
            s16 objectId = gPlayerFormObjectIds[GET_PLAYER_FORM];

            gActorOverlayTable[ACTOR_PLAYER].profile->objectId = objectId;
            func_8012F73C(&gPlayState->objectCtx, player->actor.objectSlot, objectId);
            player->actor.objectSlot = Object_GetSlot(&gPlayState->objectCtx, GAMEPLAY_KEEP);

            s32 objectSlot = Object_GetSlot(&gPlayState->objectCtx, gActorOverlayTable[ACTOR_PLAYER].profile->objectId);
            player->actor.objectSlot = objectSlot;
            player->actor.shape.rot.z = GET_PLAYER_FORM + 1;
            player->actor.init = PlayerCall_Init;
            player->actor.update = PlayerCall_Update;
            player->actor.draw = PlayerCall_Draw;
            gSaveContext.save.equippedMask = PLAYER_MASK_NONE;

            TransitionFade_SetColor(&gPlayState->unk_18E48, 0x000000);
            R_TRANS_FADE_FLASH_ALPHA_STEP = -1;
            Player_PlaySfx(GET_PLAYER(gPlayState), NA_SE_SY_TRANSFORM_MASK_FLASH);

            // Clear previous mask to prevent crashing with masks being drawn while we switch transformations
            if (player->transformation == PLAYER_FORM_HUMAN) {
                player->prevMask = PLAYER_MASK_NONE;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFastTransformation, { CVAR_NAME });
