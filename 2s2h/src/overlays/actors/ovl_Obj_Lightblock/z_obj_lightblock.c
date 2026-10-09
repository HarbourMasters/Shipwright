/*
 * File: z_obj_lightblock.c
 * Overlay: ovl_Obj_Lightblock
 * Description: Sun Block
 */

#include "z_obj_lightblock.h"
#include "overlays/actors/ovl_Demo_Effect/z_demo_effect.h"
#include "objects/object_lightblock/object_lightblock.h"
#include "code/actor/actor.h"

#define FLAGS 0x00000000

void ObjLightblock_Init(Actor* thisx, PlayState* play);
void ObjLightblock_Destroy(Actor* thisx, PlayState* play);
void ObjLightblock_Update(Actor* thisx, PlayState* play);
void ObjLightblock_Draw(Actor* thisx, PlayState* play);
void ObjLightblock_SetupWait(ObjLightblock* this);
void ObjLightblock_Wait(ObjLightblock* this, PlayState* play);
void ObjLightblock_SetupPlayCutscene(ObjLightblock* this);
void ObjLightblock_PlayCutscene(ObjLightblock* this, PlayState* play);
void ObjLightblock_SetupFadeAway(ObjLightblock* this);
void ObjLightblock_FadeAway(ObjLightblock* this, PlayState* play);

ActorProfile Obj_Lightblock_Profile = {
    /**/ ACTOR_OBJ_LIGHTBLOCK,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_LIGHTBLOCK,
    /**/ sizeof(ObjLightblock),
    /**/ ObjLightblock_Init,
    /**/ ObjLightblock_Destroy,
    /**/ ObjLightblock_Update,
    /**/ ObjLightblock_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00202000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 84, 120, 0, { 0, 0, 0 } },
};

typedef struct LightblockTypeVars {
    /* 0x0 */ f32 scale;
    /* 0x4 */ s16 radius;
    /* 0x6 */ s16 height;
    /* 0x8 */ s16 yShift;
    /* 0xC */ s32 effectParams;
} LightblockTypeVars; // size = 0x10

static LightblockTypeVars sLightblockTypeVars[] = {
    { 0.1f, 76, 80, 19, DEMO_EFFECT_TIMEWARP_LIGHTBLOCK_LARGE },
    { (1.0f / 6.0f), 126, 144, 19, DEMO_EFFECT_TIMEWARP_LIGHTBLOCK_VERY_LARGE },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 500, ICHAIN_STOP),
};

// extern Gfx D_801AEF88[];
// extern Gfx D_801AEFA0[];

void ObjLightblock_SpawnEffect(ObjLightblock* this, PlayState* play) {
    LightblockTypeVars* typeVars = &sLightblockTypeVars[LIGHTBLOCK_TYPE(&this->dyna.actor)];

    Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_EFFECT, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                this->dyna.actor.world.pos.z, 0, 0, 0, typeVars->effectParams);
}

void ObjLightblock_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjLightblock* this = (ObjLightblock*)thisx;
    LightblockTypeVars* typeVars = &sLightblockTypeVars[LIGHTBLOCK_TYPE(&this->dyna.actor)];

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    Actor_SetScale(&this->dyna.actor, typeVars->scale);
    DynaPolyActor_Init(&this->dyna, 0);
    Collider_InitCylinder(play, &this->collider);
    if (Flags_GetSwitch(play, LIGHTBLOCK_GET_DESTROYED_SWITCH_FLAG(&this->dyna.actor))) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    DynaPolyActor_LoadMesh(play, &this->dyna, &gSunBlockCol);
    Collider_SetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
    this->collider.dim.radius = typeVars->radius;
    this->collider.dim.height = typeVars->height;
    this->collider.dim.yShift = typeVars->yShift;
    this->alpha = 255;
    ObjLightblock_SetupWait(this);
}

void ObjLightblock_Destroy(Actor* thisx, PlayState* play) {
    ObjLightblock* this = (ObjLightblock*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(play, &this->collider);
}

void ObjLightblock_SetupWait(ObjLightblock* this) {
    this->actionFunc = ObjLightblock_Wait;
}

/**
 * Wait for a single collision from a Light Arrow or 8 frames of Mirror Shield ray collision.
 */
void ObjLightblock_Wait(ObjLightblock* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        // light arrows
        if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & (1 << 13)) {
            this->collisionCounter = 8;
        }
        // light ray
        else {
            this->collisionCounter++;
        }
    } else {
        this->collisionCounter = 0;
    }

    if (this->collisionCounter >= 8) {
        CutsceneManager_Queue(this->dyna.actor.csId);
        ObjLightblock_SetupPlayCutscene(this);
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void ObjLightblock_SetupPlayCutscene(ObjLightblock* this) {
    this->actionFunc = ObjLightblock_PlayCutscene;
}

void ObjLightblock_PlayCutscene(ObjLightblock* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        Flags_SetSwitch(play, LIGHTBLOCK_GET_DESTROYED_SWITCH_FLAG(&this->dyna.actor));
        ObjLightblock_SpawnEffect(this, play);
        ObjLightblock_SetupFadeAway(this);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void ObjLightblock_SetupFadeAway(ObjLightblock* this) {
    this->timer = 80;
    this->actionFunc = ObjLightblock_FadeAway;
}

void ObjLightblock_FadeAway(ObjLightblock* this, PlayState* play) {
    this->timer--;
    if (this->timer <= 0) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        Actor_Kill(&this->dyna.actor);
        return;
    }

    if (this->timer <= 60) {
        if (this->alpha > 40) {
            this->alpha -= 40;
        } else {
            this->alpha = 0;
            this->dyna.actor.draw = NULL;
            DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        }
    }
}

void ObjLightblock_Update(Actor* thisx, PlayState* play) {
    ObjLightblock* this = (ObjLightblock*)thisx;

    this->actionFunc(this, play);
}

void ObjLightblock_Draw(Actor* thisx, PlayState* play) {
    ObjLightblock* this = (ObjLightblock*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->alpha < 255) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, D_801AEF88);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, this->alpha);
        gSPDisplayList(POLY_XLU_DISP++, gSunBlockDL);
    } else {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, D_801AEFA0);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, 255, 255, 255, 255);
        gSPDisplayList(POLY_OPA_DISP++, gSunBlockDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
