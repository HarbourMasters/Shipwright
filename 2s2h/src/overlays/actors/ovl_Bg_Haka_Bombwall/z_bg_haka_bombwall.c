/*
 * File: z_bg_haka_bombwall.c
 * Overlay: ovl_Bg_Haka_Bombwall
 * Description: Beneath the Grave - Bombable Wall
 */

#include "z_bg_haka_bombwall.h"
#include "objects/object_haka_obj/object_haka_obj.h"

#define FLAGS 0x00000000

void BgHakaBombwall_Init(Actor* thisx, PlayState* play);
void BgHakaBombwall_Destroy(Actor* thisx, PlayState* play);
void BgHakaBombwall_Update(Actor* thisx, PlayState* play);
void BgHakaBombwall_Draw(Actor* thisx, PlayState* play);

void func_80BD5E6C(BgHakaBombwall* this, PlayState* play);
void func_80BD6260(BgHakaBombwall* this);
void func_80BD6274(BgHakaBombwall* this, PlayState* play);
void BgHakaBombwall_SetupPlayCutscene(BgHakaBombwall* this);
void BgHakaBombwall_PlayCutscene(BgHakaBombwall* this, PlayState* play);
void BgHakaBombwall_SetupEndCutscene(BgHakaBombwall* this);
void BgHakaBombwall_EndCutscene(BgHakaBombwall* this, PlayState* play);

ActorProfile Bg_Haka_Bombwall_Profile = {
    /**/ ACTOR_BG_HAKA_BOMBWALL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HAKA_OBJ,
    /**/ sizeof(BgHakaBombwall),
    /**/ BgHakaBombwall_Init,
    /**/ BgHakaBombwall_Destroy,
    /**/ BgHakaBombwall_Update,
    /**/ BgHakaBombwall_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 80, 80, 0, { 0, 0, 0 } },
};

static s16 sRockScales[4] = { 24, 15, 10, 5 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 500, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

s32 func_80BD5E00(BgHakaBombwall* this) {
    if (this->collider.base.acFlags & AC_HIT) {
        if ((this->collider.base.ac != NULL) &&
            (Math3D_Vec3fDistSq(&this->dyna.actor.world.pos, &this->collider.base.ac->world.pos) < SQ(80.0f))) {
            return true;
        }
    }
    return false;
}

void func_80BD5E6C(BgHakaBombwall* this, PlayState* play) {
    u32 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f posOffset;
    Vec3f velocityOffset;
    f32 offsetPosX;
    f32 offsetPosY;
    s16 scale;
    s16 phi_s0;
    s16 phi_t0;
    s16 gravity;

    Matrix_Push();
    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);

    offsetPosX = 0.0f;
    offsetPosY = 0.0f;

    for (i = 0; i < 30; i++) {
        offsetPosX += 60.0f + (Rand_ZeroOne() * 20.0f);

        if (offsetPosX > 75.0f) {
            offsetPosX -= 150.0f;
        }

        offsetPosY += 5;

        posOffset.x = offsetPosX;
        posOffset.y = offsetPosY;
        posOffset.z = (Rand_ZeroOne() * 20.0f) - 10.0f;

        velocityOffset.x = ((Rand_ZeroOne() - 0.5f) * 5.0f) + (offsetPosX * (4.0f / 75.0f));
        velocityOffset.y = (Rand_ZeroOne() * 7.0f) - 2.0f;
        velocityOffset.z = (Rand_ZeroOne() * 4.0f) - 2.0f;

        Matrix_MultVec3f(&posOffset, &pos);
        Matrix_MultVec3f(&velocityOffset, &velocity);

        pos.x += this->dyna.actor.world.pos.x;
        pos.y += this->dyna.actor.world.pos.y;
        pos.z += this->dyna.actor.world.pos.z;

        //! FAKE
        if (1) {}

        if ((i & 3) == 0) {
            phi_s0 = 32;
            func_800BBFB0(play, &pos, 60.0f, 2, 100, 120, 1);
        } else {
            phi_s0 = 64;
        }

        if ((i % 2) != 0) {
            phi_s0 |= 1;
            phi_t0 = 1;
        } else {
            phi_t0 = 0;
        }

        scale = sRockScales[i & (ARRAY_COUNT(sRockScales) - 1)];

        if (scale >= 16) {
            gravity = -550;
        } else {
            gravity = -450;
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, gravity, phi_s0, 30, 0, 0, scale, phi_t0, 0, 50, -1,
                             OBJECT_HAKA_OBJ, object_haka_obj_DL_001680);
    }

    Matrix_Pop();
}

void BgHakaBombwall_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHakaBombwall* this = (BgHakaBombwall*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    Collider_InitCylinder(play, &this->collider);
    if (Flags_GetSwitch(play, BGHAKABOMBWALL_GET_SWITCH_FLAG(thisx))) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    DynaPolyActor_LoadMesh(play, &this->dyna, &object_haka_obj_Colheader_000148);
    Collider_SetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
    func_80BD6260(this);
}

void BgHakaBombwall_Destroy(Actor* thisx, PlayState* play) {
    BgHakaBombwall* this = (BgHakaBombwall*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(play, &this->collider);
}

void func_80BD6260(BgHakaBombwall* this) {
    this->actionFunc = func_80BD6274;
}

void func_80BD6274(BgHakaBombwall* this, PlayState* play) {
    if (func_80BD5E00(this)) {
        BgHakaBombwall_SetupPlayCutscene(this);
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void BgHakaBombwall_SetupPlayCutscene(BgHakaBombwall* this) {
    this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    CutsceneManager_Queue(this->dyna.actor.csId);
    this->actionFunc = BgHakaBombwall_PlayCutscene;
}

void BgHakaBombwall_PlayCutscene(BgHakaBombwall* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        func_80BD5E6C(this, play);
        this->dyna.actor.draw = NULL;
        Flags_SetSwitch(play, BGHAKABOMBWALL_GET_SWITCH_FLAG(&this->dyna.actor));
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 0x3C, NA_SE_EV_WALL_BROKEN);
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        BgHakaBombwall_SetupEndCutscene(this);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void BgHakaBombwall_SetupEndCutscene(BgHakaBombwall* this) {
    this->csTimer = 30;
    this->actionFunc = BgHakaBombwall_EndCutscene;
}

void BgHakaBombwall_EndCutscene(BgHakaBombwall* this, PlayState* play) {
    this->csTimer--;
    if (this->csTimer <= 0) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        Actor_Kill(&this->dyna.actor);
    }
}

void BgHakaBombwall_Update(Actor* thisx, PlayState* play) {
    BgHakaBombwall* this = (BgHakaBombwall*)thisx;

    this->actionFunc(this, play);
}

void BgHakaBombwall_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_haka_obj_DL_000040);
}
