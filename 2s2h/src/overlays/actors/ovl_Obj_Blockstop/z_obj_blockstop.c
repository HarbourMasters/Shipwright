/*
 * File: z_obj_blockstop.c
 * Overlay: ovl_Obj_Blockstop
 * Description: Push Block Trigger - Blockstop switch triggered by pushblock (Snowhead)
 */

#include "z_obj_blockstop.h"
#include "overlays/actors/ovl_Obj_Oshihiki/z_obj_oshihiki.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjBlockstop_Init(Actor* thisx, PlayState* play);
void ObjBlockstop_Update(Actor* thisx, PlayState* play);

void ObjBlockstop_CheckForBlock(ObjBlockstop* this, PlayState* play);
void ObjBlockstop_TryPlayCutscene(ObjBlockstop* this, PlayState* play);

ActorProfile Obj_Blockstop_Profile = {
    /**/ ACTOR_OBJ_BLOCKSTOP,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjBlockstop),
    /**/ ObjBlockstop_Init,
    /**/ Actor_Noop,
    /**/ ObjBlockstop_Update,
    /**/ NULL,
};

void ObjBlockstop_Init(Actor* thisx, PlayState* play) {
    ObjBlockstop* this = (ObjBlockstop*)thisx;

    if (Flags_GetSwitch(play, OBJBLOCKSTOP_GET_SWITCH_FLAG(&this->actor))) {
        Actor_Kill(&this->actor);
    }
    this->actionFunc = ObjBlockstop_CheckForBlock;
}

void ObjBlockstop_CheckForBlock(ObjBlockstop* this, PlayState* play) {
    Actor* prop = play->actorCtx.actorLists[ACTORCAT_PROP].first;

    while (prop != NULL) {
        if ((prop->id == ACTOR_OBJ_OSHIHIKI) && (fabsf(prop->world.pos.x - this->actor.world.pos.x) < 20.0f) &&
            (fabsf(prop->world.pos.z - this->actor.world.pos.z) < 20.0f) &&
            (fabsf(prop->world.pos.y - this->actor.world.pos.y) < 20.0f)) {

            s32 params = OBJOSHIHIKI_GET_F(prop);
            if (params < 3) {
                CutsceneManager_Queue(this->actor.csId);
                this->actionFunc = ObjBlockstop_TryPlayCutscene;
            }
        }
        prop = prop->next;
    }
}

void ObjBlockstop_TryPlayCutscene(ObjBlockstop* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->actor.csId)) {
        Flags_SetSwitch(play, OBJBLOCKSTOP_GET_SWITCH_FLAG(&this->actor));
        if (CutsceneManager_GetLength(this->actor.csId) != -1) {
            CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        }
        Actor_Kill(&this->actor);
        return;
    }
    CutsceneManager_Queue(this->actor.csId);
}

void ObjBlockstop_Update(Actor* thisx, PlayState* play) {
    ObjBlockstop* this = (ObjBlockstop*)thisx;

    this->actionFunc(this, play);
}
