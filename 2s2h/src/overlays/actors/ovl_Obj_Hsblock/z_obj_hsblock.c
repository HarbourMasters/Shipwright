/*
 * File: z_obj_hsblock.c
 * Overlay: ovl_Obj_Hsblock
 * Description: Hookshot Block
 */

#include "z_obj_hsblock.h"
#include "objects/object_d_hsblock/object_d_hsblock.h"
#include "overlays/actors/ovl_Obj_Ice_Poly/z_obj_ice_poly.h"

#define FLAGS 0x00000000

void ObjHsblock_Init(Actor* thisx, PlayState* play);
void ObjHsblock_Destroy(Actor* thisx, PlayState* play);
void ObjHsblock_Update(Actor* thisx, PlayState* play);
void ObjHsblock_Draw(Actor* thisx, PlayState* play);

void func_8093E0A0(ObjHsblock* this, PlayState* play);

void func_8093E03C(ObjHsblock* this);
void func_8093E05C(ObjHsblock* this);
void func_8093E0E8(ObjHsblock* this);
void func_8093E10C(ObjHsblock* this, PlayState* play);

ActorProfile Obj_Hsblock_Profile = {
    /**/ ACTOR_OBJ_HSBLOCK,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_D_HSBLOCK,
    /**/ sizeof(ObjHsblock),
    /**/ ObjHsblock_Init,
    /**/ ObjHsblock_Destroy,
    /**/ ObjHsblock_Update,
    /**/ ObjHsblock_Draw,
};

static f32 sFocusHeights[] = { 85.0f, 85.0f, 0.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 200, ICHAIN_STOP),
};

static CollisionHeader* sColHeaders[] = {
    &gHookshotPostCol,
    &gHookshotPostCol,
    &gHookshotTargetCol,
};

static Gfx* sDisplayLists[] = { gHookshotPostDL, gHookshotPostDL, gHookshotTargetDL };

void ObjHsblock_SetupAction(ObjHsblock* this, ObjHsblockActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void func_8093DEAC(ObjHsblock* this, PlayState* play) {
    if (OBJHSBLOCK_GET_5(&this->dyna.actor) != 0) {
        Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_OBJ_ICE_POLY, this->dyna.actor.world.pos.x,
                           this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, this->dyna.actor.world.rot.x,
                           this->dyna.actor.world.rot.y, this->dyna.actor.world.rot.z,
                           OBJICEPOLY_PARAMS(100, OBJICEPOLY_SWITCH_FLAG_NONE));
    }
}

void ObjHsblock_Init(Actor* thisx, PlayState* play) {
    ObjHsblock* this = (ObjHsblock*)thisx;

    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, sColHeaders[OBJHSBLOCK_GET_3(thisx)]);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    func_8093DEAC(this, play);

    switch (OBJHSBLOCK_GET_3(&this->dyna.actor)) {
        case 0:
        case 2:
            func_8093E03C(this);
            break;
        case 1:
            if (Flags_GetSwitch(play, OBJHSBLOCK_GET_SWITCH_FLAG(thisx))) {
                func_8093E03C(this);
            } else {
                func_8093E05C(this);
            }
            break;
        default:
            break;
    }
}

void ObjHsblock_Destroy(Actor* thisx, PlayState* play) {
    ObjHsblock* this = (ObjHsblock*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_8093E03C(ObjHsblock* this) {
    ObjHsblock_SetupAction(this, NULL);
}

void func_8093E05C(ObjHsblock* this) {
    this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 105.0f;
    ObjHsblock_SetupAction(this, func_8093E0A0);
}

void func_8093E0A0(ObjHsblock* this, PlayState* play) {
    if (Flags_GetSwitch(play, OBJHSBLOCK_GET_SWITCH_FLAG(&this->dyna.actor))) {
        func_8093E0E8(this);
    }
}

void func_8093E0E8(ObjHsblock* this) {
    ObjHsblock_SetupAction(this, func_8093E10C);
}

void func_8093E10C(ObjHsblock* this, PlayState* play) {
    Math_SmoothStepToF(&this->dyna.actor.velocity.y, 16.0f, 0.1f, 0.8f, 0.0f);
    if (fabsf(Math_SmoothStepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 0.3f,
                                 this->dyna.actor.velocity.y, 0.3f)) < 0.001f) {
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
        func_8093E03C(this);
        this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }
}

void ObjHsblock_Update(Actor* thisx, PlayState* play) {
    ObjHsblock* this = (ObjHsblock*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
    Actor_SetFocus(&this->dyna.actor, sFocusHeights[OBJHSBLOCK_GET_3(thisx)]);
}

void ObjHsblock_Draw(Actor* thisx, PlayState* play) {
    static Color_RGB8 sEnvColors[] = {
        { 60, 60, 120 },
        { 120, 100, 70 },
        { 100, 150, 120 },
        { 255, 255, 255 },
    };
    Color_RGB8* envColor = &sEnvColors[OBJHSBLOCK_GET_6(thisx)];

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, envColor->r, envColor->g, envColor->b, 255);
    gSPDisplayList(POLY_OPA_DISP++, sDisplayLists[OBJHSBLOCK_GET_3(thisx)]);

    CLOSE_DISPS(play->state.gfxCtx);
}
