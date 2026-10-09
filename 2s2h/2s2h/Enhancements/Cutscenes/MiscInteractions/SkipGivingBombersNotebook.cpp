#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "macros.h"

#include "overlays/actors/ovl_En_Bom_Bowl_Man/z_en_bom_bowl_man.h"
void func_809C5310(EnBomBowlMan* enBomBowlMan, PlayState* play);
void func_809C53A4(EnBomBowlMan* enBomBowlMan);
void CutsceneManager_End();
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void EnBomBowlMan_WaitForPlayer(EnBomBowlMan* enBomBowlMan, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (GameInteractor_Should(VB_BE_ELIGIBLE_FOR_BOMBERS_NOTEBOOK,
                              CHECK_WEEKEVENTREG(WEEKEVENTREG_73_80) && !CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK),
                              enBomBowlMan)) {
        if (player->actor.world.pos.x < 1510.0f && player->transformation != PLAYER_FORM_DEKU &&
            enBomBowlMan->actor.xzDistToPlayer < 75.0f && Message_GetState(&gPlayState->msgCtx) == TEXT_STATE_NONE) {
            if (player->transformation != PLAYER_FORM_DEKU) {
                if (enBomBowlMan->actor.xzDistToPlayer < 75.0f) {
                    Player_SetCsActionWithHaltedActors(play, &enBomBowlMan->actor, PLAYER_CSACTION_WAIT);
                    func_809C53A4(enBomBowlMan);
                }
            }
        }
    }
}

/*
 * This speeds up the interaction in which the leader of the Bombers gives you the Bombers' Notebook
 *
 * Typically what this requires is for you to enter the code to get into the hideout, then exit back
 * into East Clock Town as a non-deku form. The leader will approach you and give you the notebook.
 *
 * The changes made here make the leader approach you as soon as eligible, skipping the need to enter
 * and exit the hideout. This also skips the dialog before and after the notebook is given, and gives
 * the player control as soon as the bomber leader starts running away.
 */
void RegisterSkipGivingBombersNotebook() {
    COND_VB_SHOULD(VB_SETUP_EAST_CLOCK_TOWN_BOM_BOWL_MAN, CVAR, {
        if (gPlayState->sceneId == SCENE_TOWN) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnActorInit, ACTOR_EN_BOM_BOWL_MAN, CVAR, [](Actor* actor) {
        EnBomBowlMan* enBomBowlMan = (EnBomBowlMan*)actor;
        if (enBomBowlMan->actionFunc == func_809C5310) {
            enBomBowlMan->actionFunc = EnBomBowlMan_WaitForPlayer;
        }
    });

    // "Hey, guy! You haven't passed my test..."
    COND_ID_HOOK(OnOpenText, 0x72F, CVAR, [](u16* textId, bool* loadFromMessageTable) {
        Player* player = GET_PLAYER(gPlayState);
        EnBomBowlMan* enBomBowlMan =
            (EnBomBowlMan*)Actor_FindNearby(gPlayState, &player->actor, ACTOR_EN_BOM_BOWL_MAN, ACTORCAT_NPC, 100.0f);
        if (enBomBowlMan != NULL) {
            enBomBowlMan->actor.textId = 0x715;
        }
        *textId = 0x715;
    });

    // "Rules of the Bombers..."
    COND_ID_HOOK(OnOpenText, 0x716, CVAR, [](u16* textId, bool* loadFromMessageTable) {
        Player* player = GET_PLAYER(gPlayState);
        EnBomBowlMan* enBomBowlMan =
            (EnBomBowlMan*)Actor_FindNearby(gPlayState, &player->actor, ACTOR_EN_BOM_BOWL_MAN, ACTORCAT_NPC, 100.0f);
        if (enBomBowlMan != NULL) {
            enBomBowlMan->actor.textId = 0x735;
        }
        *textId = 0x735;
    });

    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        // Cutscene 12 in East Clock Town is the bomber running away after giving notebook
        if (gPlayState->sceneId == SCENE_TOWN) {
            s16* csId = va_arg(args, s16*);
            if (*csId == 12) {
                *should = false;
                EnBomBowlMan* enBomBowlMan = va_arg(args, EnBomBowlMan*);
                Player_SetCsActionWithHaltedActors(gPlayState, &enBomBowlMan->actor, PLAYER_CSACTION_END);
                SET_WEEKEVENTREG(WEEKEVENTREG_84_80);
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipGivingBombersNotebook, { CVAR_NAME });
