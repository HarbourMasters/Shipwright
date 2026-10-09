/*
 * File: z_bg_ikninside.c
 * Overlay: ovl_Bg_Ikninside
 * Description: Ancient Castle of Ikana Objects
 */

#include "z_bg_ikninside.h"
#include "objects/object_ikninside_obj/object_ikninside_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgIkninside_Init(Actor* thisx, PlayState* play);
void BgIkninside_Destroy(Actor* thisx, PlayState* play);
void BgIkninside_Update(Actor* thisx, PlayState* play);
void BgIkninside_Draw(Actor* thisx, PlayState* play);

void func_80C072D0(BgIkninside* this, PlayState* play);

ActorProfile Bg_Ikninside_Profile = {
    /**/ ACTOR_BG_IKNINSIDE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_IKNINSIDE_OBJ,
    /**/ sizeof(BgIkninside),
    /**/ BgIkninside_Init,
    /**/ BgIkninside_Destroy,
    /**/ BgIkninside_Update,
    /**/ BgIkninside_Draw,
};

static Gfx* D_80C076A0[] = { object_ikninside_obj_DL_00A748, object_ikninside_obj_DL_00A5A8 };

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x80000008, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 32, 32, 0, { 0, 0, 0 } },
};

void BgIkninside_Init(Actor* thisx, PlayState* play) {
    BgIkninside* this = (BgIkninside*)thisx;
    CollisionHeader* colHeader = NULL;
    s32 pad;

    Actor_SetScale(&this->dyna.actor, 0.1f);
    this->actionFunc = func_80C072D0;
    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&object_ikninside_obj_Colheader_00DE48, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    Collider_InitAndSetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
    if (Flags_GetSwitch(play, DMIKNINSIDE_GET_SWITCH_FLAG(thisx))) {
        Actor_Kill(&this->dyna.actor);
    }
}

void BgIkninside_Destroy(Actor* thisx, PlayState* play) {
    BgIkninside* this = (BgIkninside*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80C07220(BgIkninside* this, PlayState* play) {
}

void func_80C07230(BgIkninside* this, PlayState* play) {
    if (this->dyna.actor.csId == CS_ID_NONE) {
        this->actionFunc = func_80C07220;
    } else if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(this->dyna.actor.csId);
    } else if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        this->actionFunc = func_80C07220;
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

static Vec3f D_80C076D4 = { 0.0f, -1.0f, 0.0f };

void func_80C072D0(BgIkninside* this, PlayState* play) {
    s16 altitude;
    s16 azimuth;
    Vec3f pos;
    Vec3f velocity;
    f32 speed;
    s32 i;

    if (this->collider.base.acFlags & AC_HIT) {
        if ((this->collider.elem.acHitElem != NULL) &&
            (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x80000000)) {
            for (i = 0; i < 20; i++) {
                altitude = Rand_S16Offset(0x1800, 0x2800);
                azimuth = Rand_Next() >> 0x10;
                speed = Rand_ZeroFloat(3.0f) + 8.0f;
                velocity.x = speed * Math_CosS(altitude) * Math_SinS(azimuth);
                velocity.y = speed * Math_SinS(altitude) + Rand_ZeroFloat(5.0f);
                velocity.z = speed * Math_CosS(altitude) * Math_CosS(azimuth);
                pos.x = Rand_ZeroFloat(10.0f) * velocity.x + this->dyna.actor.world.pos.x;
                pos.y = Rand_ZeroFloat(1.0f) * velocity.y + this->dyna.actor.world.pos.y;
                pos.z = Rand_ZeroFloat(10.0f) * velocity.z + this->dyna.actor.world.pos.z;
                EffectSsHahen_Spawn(play, &pos, &velocity, &D_80C076D4, 0, 30, OBJECT_IKNINSIDE_OBJ, 25,
                                    D_80C076A0[i & 1]);
            }
            Flags_SetSwitch(play, DMIKNINSIDE_GET_SWITCH_FLAG(&this->dyna.actor));
            this->actionFunc = func_80C07230;
            this->dyna.actor.draw = NULL;
            DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        } else {
            this->timer = 20;
        }
    }
    if (this->timer > 0) {
        if ((this->timer % 2) != 0) {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 1.0f;
        } else {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
        }
        this->timer--;
    } else {
        this->timer = 0;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
    }
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void BgIkninside_Update(Actor* thisx, PlayState* play) {
    BgIkninside* this = (BgIkninside*)thisx;

    this->actionFunc(this, play);
}

void BgIkninside_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, object_ikninside_obj_DL_00CC78);

    CLOSE_DISPS(play->state.gfxCtx);
}
