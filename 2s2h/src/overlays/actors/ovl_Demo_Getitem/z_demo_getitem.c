/*
 * File: z_demo_getitem.c
 * Overlay: ovl_Demo_Getitem
 * Description: Cutscene objects for Great Fairy's Mask and Great Fairy's Sword
 */

#include "z_demo_getitem.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DemoGetitem_Init(Actor* thisx, PlayState* play);
void DemoGetitem_Destroy(Actor* thisx, PlayState* play);
void DemoGetitem_Update(Actor* thisx, PlayState* play);

void DemoGetitem_Wait(DemoGetitem* this, PlayState* play);
void DemoGetitem_PerformCutsceneActions(DemoGetitem* this, PlayState* play);
void DemoGetitem_Draw(Actor* thisx, PlayState* play);

ActorProfile Demo_Getitem_Profile = {
    /**/ ACTOR_DEMO_GETITEM,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(DemoGetitem),
    /**/ DemoGetitem_Init,
    /**/ DemoGetitem_Destroy,
    /**/ DemoGetitem_Update,
    /**/ NULL,
};

static s16 sObjectIds[] = { OBJECT_GI_MASK14, OBJECT_GI_SWORD_4 };

static s16 sGetItemDraws[] = { GID_MASK_GREAT_FAIRY, GID_SWORD_GREAT_FAIRY };

static u16 sCueTypes[] = { CS_CMD_ACTOR_CUE_110, CS_CMD_ACTOR_CUE_566 };

typedef enum GreatFairyRewardItem {
    /* 0 */ DEMOGETITEM_ITEM_MASK_GREAT_FAIRY,
    /* 1 */ DEMOGETITEM_ITEM_SWORD_GREAT_FAIRY
} GreatFairyRewardItem;

void DemoGetitem_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    s32 objectSlot;
    s32 itemIndex;
    DemoGetitem* this = (DemoGetitem*)thisx;

    itemIndex = DEMOGETITEM_ITEM_MASK_GREAT_FAIRY;
    if (DEMOGETITEM_GET_F(&this->actor) == 1) {
        itemIndex = DEMOGETITEM_ITEM_SWORD_GREAT_FAIRY;
    }

    Actor_SetScale(&this->actor, 0.25f);
    this->actionFunc = DemoGetitem_Wait;
    this->getItemDrawId = sGetItemDraws[itemIndex];
    this->cueType = sCueTypes[itemIndex];

    objectSlot = Object_GetSlot(&play->objectCtx, sObjectIds[itemIndex]);
    if (objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    this->objectSlot = objectSlot;
}

void DemoGetitem_Destroy(Actor* thisx, PlayState* play) {
}

void DemoGetitem_Wait(DemoGetitem* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.draw = NULL;
        this->actor.objectSlot = this->objectSlot;
        this->actionFunc = DemoGetitem_PerformCutsceneActions;
    }
}

void DemoGetitem_PerformCutsceneActions(DemoGetitem* this, PlayState* play) {
    s32 pad;
    u16 bobPhase = (play->gameplayFrames * 1000) % 0x10000;

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        if (play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id != 4) {
            this->actor.shape.yOffset = 0.0f;
        }

        switch (play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id) {
            case 2:
                this->actor.draw = DemoGetitem_Draw;
                Cutscene_ActorTranslate(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));
                this->actor.shape.rot.y += 0x3E8;
                break;

            case 3:
                Actor_Kill(&this->actor);
                break;

            case 4:
                this->actor.draw = DemoGetitem_Draw;
                Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));
                this->actor.shape.yOffset = Math_SinS(bobPhase) * 15.0f;
                break;

            default:
                this->actor.draw = NULL;
                break;
        }
    } else {
        this->actor.draw = NULL;
    }
}

void DemoGetitem_Update(Actor* thisx, PlayState* play) {
    DemoGetitem* this = (DemoGetitem*)thisx;

    this->actionFunc(this, play);
}

void DemoGetitem_Draw(Actor* thisx, PlayState* play) {
    DemoGetitem* this = (DemoGetitem*)thisx;

    func_800B8050(&this->actor, play, 0);
    func_800B8118(&this->actor, play, 0);
    GetItem_Draw(play, this->getItemDrawId);
}
