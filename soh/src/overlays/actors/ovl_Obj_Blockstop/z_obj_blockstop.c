/*
 * File: z_obj_blockstop.c
 * Overlay: ovl_Obj_Blockstop
 * Description: Stops blocks and sets relevant flags when the block is in position.
 */

#include "z_obj_blockstop.h"
#include "overlays/actors/ovl_Obj_Oshihiki/z_obj_oshihiki.h"

#define FLAGS 0

void ObjBlockstop_Init(Actor* thisx, PlayState* play);
void ObjBlockstop_Destroy(Actor* thisx, PlayState* play);
void ObjBlockstop_Update(Actor* thisx, PlayState* play);

const ActorInit Obj_Blockstop_InitVars = {
    ACTOR_OBJ_BLOCKSTOP,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjBlockstop),
    (ActorFunc)ObjBlockstop_Init,
    (ActorFunc)ObjBlockstop_Destroy,
    (ActorFunc)ObjBlockstop_Update,
    NULL,
    NULL,
};

void ObjBlockstop_Init(Actor* thisx, PlayState* play) {
    ObjBlockstop* this = (ObjBlockstop*)thisx;

    if (Flags_GetSwitch(play, this->actor.params)) {
        Actor_Kill(&this->actor);
    } else {
        this->actor.world.pos.y++;
    }
}

void ObjBlockstop_Destroy(Actor* thisx, PlayState* play) {
}

void ObjBlockstop_Update(Actor* thisx, PlayState* play) {
    ObjBlockstop* this = (ObjBlockstop*)thisx;
    DynaPolyActor* dynaPolyActor;
    Vec3f sp4C;
    s32 bgId;
    s32 pad;

    if (BgCheck_EntityLineTest2(&play->colCtx, &this->actor.home.pos, &this->actor.world.pos, &sp4C,
                                &this->actor.floorPoly, false, false, true, true, &bgId, &this->actor)) {
        dynaPolyActor = DynaPoly_GetActor(&play->colCtx, bgId);

        if (dynaPolyActor != NULL && dynaPolyActor->actor.id == ACTOR_OBJ_OSHIHIKI) {
            if ((dynaPolyActor->actor.params & 0x000F) == PUSHBLOCK_HUGE_START_ON ||
                (dynaPolyActor->actor.params & 0x000F) == PUSHBLOCK_HUGE_START_OFF) {
                func_80078884(NA_SE_SY_CORRECT_CHIME);
            } else {
                func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
            }

            Flags_SetSwitch(play, this->actor.params);
            Actor_Kill(&this->actor);
        }
    }
}
