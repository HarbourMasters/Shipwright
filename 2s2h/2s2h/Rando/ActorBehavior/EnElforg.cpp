#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Elforg/z_en_elforg.h"

void EnElforg_SpawnSparkles(EnElforg* enElforg, PlayState* play, s32 life);
void EnElforg_FairyCollected(EnElforg* enElforg, PlayState* play);
}

#define CUSTOM_PARAM (enElforg->actor.home.rot.x)

void EnElforg_DrawCustom(Actor* thisx, PlayState* play) {
    EnElforg* enElforg = (EnElforg*)thisx;
    Matrix_Scale(20.0f, 20.0f, 20.0f, MTXMODE_APPLY);

    if (CUSTOM_PARAM == RC_UNKNOWN) {
        return;
    }

    auto randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_PARAM];

    EnElforg_SpawnSparkles(enElforg, play, 16);
    thisx->shape.rot.y = thisx->shape.rot.y + 960;

    if (enElforg->actionFunc == EnElforg_FairyCollected) {
        // If the fairy is being collected, we don't want to draw the item, it may have been converted by now
        return;
    }

    Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_PARAM),
                    (RandoCheckId)CUSTOM_PARAM, thisx);
}

void EnElforg_Setup(EnElforg* enElforg) {
    CUSTOM_PARAM = RC_UNKNOWN;

    if (STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_CLOCK_TOWN) {
        CUSTOM_PARAM = RC_CLOCK_TOWN_STRAY_FAIRY;
    } else if (STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_COLLECTIBLE) {
        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(
            FLAG_CYCL_SCENE_COLLECTIBLE, STRAY_FAIRY_GET_FLAG(&enElforg->actor), gPlayState->sceneId);
        CUSTOM_PARAM = randoStaticCheck.randoCheckId;
    } else if (STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_FREE_FLOATING ||
               STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_ENEMY ||
               STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_BUBBLE) {
        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(
            FLAG_CYCL_SCENE_SWITCH, STRAY_FAIRY_GET_FLAG(&enElforg->actor), gPlayState->sceneId);
        CUSTOM_PARAM = randoStaticCheck.randoCheckId;
    }

    if (CUSTOM_PARAM == RC_UNKNOWN) {
        return;
    }

    auto randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_PARAM];

    // Set up custom draw function
    enElforg->actor.draw = EnElforg_DrawCustom;

    // Override area, as it's used in the sparkle effect
    switch (randoSaveCheck.randoItemId) {
        case RI_WOODFALL_STRAY_FAIRY:
            enElforg->area = STRAY_FAIRY_AREA_WOODFALL;
            break;
        case RI_SNOWHEAD_STRAY_FAIRY:
            enElforg->area = STRAY_FAIRY_AREA_SNOWHEAD;
            break;
        case RI_GREAT_BAY_STRAY_FAIRY:
            enElforg->area = STRAY_FAIRY_AREA_GREAT_BAY;
            break;
        case RI_STONE_TOWER_STRAY_FAIRY:
            enElforg->area = STRAY_FAIRY_AREA_STONE_TOWER;
            break;
        default:
            enElforg->area = STRAY_FAIRY_AREA_CLOCK_TOWN;
            break;
    }
}

// This handles the Stray fairy checks, as well as overriding the draw function for the Stray Fairies
void Rando::ActorBehavior::InitEnElforgBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_ELFORG, IS_RANDO, [](Actor* actor) {
        bool invisible = actor->draw == NULL;
        EnElforg_Setup((EnElforg*)actor);
        if (invisible) {
            actor->draw = NULL;
        }
    });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_ELFORG, IS_RANDO, {
        *should = false;
        Actor* actor = va_arg(args, Actor*);

        if (STRAY_FAIRY_TYPE(actor) == STRAY_FAIRY_TYPE_CLOCK_TOWN) {
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_STRAY_FAIRY];
            randoSaveCheck.eligible = true;
        }
    });

    COND_VB_SHOULD(VB_KILL_CLOCK_TOWN_STRAY_FAIRY, IS_RANDO, {
        auto& randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_STRAY_FAIRY];
        *should = randoSaveCheck.cycleObtained;
    });

    // Stray fairies that are trapped by enemies have their draw func set later on, so we need to override that as well
    COND_VB_SHOULD(VB_SET_DRAW_FOR_SAVED_STRAY_FAIRY, IS_RANDO, {
        *should = false;
        Actor* actor = va_arg(args, Actor*);

        EnElforg_Setup((EnElforg*)actor);
    });
}
