#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

extern "C" {
#include "src/overlays/actors/ovl_En_Kz/z_en_kz.h"
#include "objects/object_kz/object_kz.h"

s32 EnKz_FollowPath(EnKz* enKz, PlayState* play);
s32 EnKz_SetMovedPos(EnKz* enKz, PlayState* play);
void EnKz_SetupMweep(EnKz* enKz, PlayState* play);
void EnKz_Wait(EnKz* enKz, PlayState* play);
}

#define CVAR_STORY_NAME CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story")
#define CVAR_STORY CVarGetInteger(CVAR_STORY_NAME, IS_RANDO)

#define CVAR_SPEED_NAME CVAR_ENHANCEMENT("MweepSpeed")
#define CVAR_SPEED CVarGetFloat(CVAR_SPEED_NAME, 1.0f)

static void EnKz_MweepNoCutscene(EnKz* enKz, PlayState* play) {
    if ((EnKz_FollowPath(enKz, play) == 1) && (enKz->waypoint == 0)) {
        Animation_Change(&enKz->skelanime, (AnimationHeader*)&gKzIdleAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame((void*)&gKzIdleAnim), ANIMMODE_LOOP, -10.0f);
        EnKz_SetMovedPos(enKz, play);
        enKz->actor.speedXZ = 0.0;
        enKz->actionFunc = EnKz_Wait;
    } else {
        enKz->actor.speedXZ = 0.1f * CVAR_SPEED;
    }
    if (enKz->skelanime.curFrame == 13.0f) {
        Audio_PlayActorSound2(&enKz->actor, NA_SE_VO_KZ_MOVE);
    }
}

static void EnKz_InterceptMweep(void* actorPtr) {
    EnKz* enKz = (EnKz*)actorPtr;
    if (enKz->actionFunc != EnKz_SetupMweep) {
        return;
    }

    enKz->actor.speedXZ = 0.1f * CVAR_SPEED;
    Inventory_ReplaceItem(gPlayState, ITEM_LETTER_RUTO, ITEM_BOTTLE);
    Flags_SetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED);
    enKz->actionFunc = EnKz_MweepNoCutscene;
}

static void RegisterSkipKingZoraMoveOver() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_KZ, CVAR_STORY, EnKz_InterceptMweep);
}

static RegisterShipInitFunc initFunc(RegisterSkipKingZoraMoveOver, { CVAR_STORY_NAME, "IS_RANDO" });
