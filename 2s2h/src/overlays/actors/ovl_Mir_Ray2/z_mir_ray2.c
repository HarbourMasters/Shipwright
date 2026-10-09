/*
 * File: z_mir_ray2.c
 * Overlay: ovl_Mir_Ray2
 * Description: Reflectable light ray (static beam)
 */

#include "z_mir_ray2.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void MirRay2_Init(Actor* thisx, PlayState* play);
void MirRay2_Destroy(Actor* thisx, PlayState* play);
void MirRay2_Update(Actor* thisx, PlayState* play);
void MirRay2_Draw(Actor* thisx, PlayState* play);

ActorProfile Mir_Ray2_Profile = {
    /**/ ACTOR_MIR_RAY2,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_MIR_RAY,
    /**/ sizeof(MirRay2),
    /**/ MirRay2_Init,
    /**/ MirRay2_Destroy,
    /**/ MirRay2_Update,
    /**/ MirRay2_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00200000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 50 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_OTHER,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

void func_80AF3F70(MirRay2* this) {
    this->collider.elements->dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.elements->dim.worldSphere.center.y = this->actor.world.pos.y;
    this->collider.elements->dim.worldSphere.center.z = this->actor.world.pos.z;
    this->collider.elements->dim.worldSphere.radius = this->range * this->collider.elements->dim.scale;
}

void func_80AF3FE0(MirRay2* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < this->range) {
        Math_StepToS(&this->radius, 150, 50);
    } else {
        Math_StepToS(&this->radius, 0, 50);
    }
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y + 100.0f,
                              this->actor.world.pos.z, 255, 255, 255, this->radius);
}

void MirRay2_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    MirRay2* this = (MirRay2*)thisx;

    if (this->actor.home.rot.x <= 0) {
        this->range = 100.0f;
    } else {
        this->range = this->actor.home.rot.x * 4.0f;
    }
    Actor_SetScale(&this->actor, 1.0f);
    if (MIRRAY2_GET_F(&this->actor) != 1) {
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawWhiteCircle, this->range * 0.02f);
    }
    func_80AF3FE0(this, play);
    this->light = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, &this->elements);
    func_80AF3F70(this);
    this->actor.shape.rot.x = 0;
    this->actor.world.rot.x = this->actor.shape.rot.x;
    if (MIRRAY2_GET_F(thisx) != 1) {
        if ((MIRRAY2_GET_SWITCH_FLAG(thisx) != 0x7F) && !Flags_GetSwitch(play, MIRRAY2_GET_SWITCH_FLAG(thisx))) {
            this->unk1A4 |= 1;
        }
    }
}

void MirRay2_Destroy(Actor* thisx, PlayState* play) {
    MirRay2* this = (MirRay2*)thisx;

    LightContext_RemoveLight(play, &play->lightCtx, this->light);
    Collider_DestroyJntSph(play, &this->collider);
}

void MirRay2_Update(Actor* thisx, PlayState* play) {
    MirRay2* this = (MirRay2*)thisx;

    if (this->unk1A4 & 1) {
        if (Flags_GetSwitch(play, MIRRAY2_GET_SWITCH_FLAG(thisx))) {
            this->unk1A4 &= ~1;
        }
    } else {
        func_80AF3FE0(this, play);
        if (MIRRAY2_GET_F(thisx) != 1) {
            Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 10.0f, UPDBGCHECKINFO_FLAG_4);
            this->actor.shape.shadowAlpha = 0x50;
        } else {
            func_80AF3F70(this);
        }
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }
}

void MirRay2_Draw(Actor* thisx, PlayState* play) {
}
