/*
 * File: z_bg_dkjail_ivy.c
 * Overlay: ovl_Bg_Dkjail_Ivy
 * Description: Cuttable Ivy wall (beneath Woodfall Temple, Swamp Spider House)
 */

#include "z_bg_dkjail_ivy.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_dkjail_obj/object_dkjail_obj.h"

#define FLAGS 0x00000000

void BgDkjailIvy_Init(Actor* thisx, PlayState* play);
void BgDkjailIvy_Destroy(Actor* thisx, PlayState* play);
void BgDkjailIvy_Update(Actor* thisx, PlayState* play);
void BgDkjailIvy_Draw(Actor* thisx, PlayState* play);

void BgDkjailIvy_SetupWaitForCut(BgDkjailIvy* this);
void BgDkjailIvy_WaitForCut(BgDkjailIvy* this, PlayState* play);
void BgDkjailIvy_SetupCutscene(BgDkjailIvy* this);
void BgDkjailIvy_BeginCutscene(BgDkjailIvy* this, PlayState* play);
void BgDkjailIvy_SetupFadeOut(BgDkjailIvy* this);
void BgDkjailIvy_FadeOut(BgDkjailIvy* this, PlayState* play);

ActorProfile Bg_Dkjail_Ivy_Profile = {
    /**/ ACTOR_BG_DKJAIL_IVY,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_DKJAIL_OBJ,
    /**/ sizeof(BgDkjailIvy),
    /**/ BgDkjailIvy_Init,
    /**/ BgDkjailIvy_Destroy,
    /**/ BgDkjailIvy_Update,
    /**/ BgDkjailIvy_Draw,
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
        { 0x01000200, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 20, 80, 0, { 0, 0, 0 } },
};

void BgDkjailIvy_IvyCutEffects(BgDkjailIvy* this, PlayState* play) {
    static Gfx* sLeafDlists[] = { gKakeraLeafMiddleDL, gKakeraLeafTipDL };
    static s16 sLeafScales[] = { 110, 80, 60, 40 };
    f32 phi_fs0;
    s32 i;
    Vec3f spD4;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    s16 angle;

    Matrix_RotateYS(this->dyna.actor.home.rot.y, MTXMODE_NEW);

    for (i = 0, angle = 0; i < 50; i++, angle += 0x4E20) {
        spD4.y = (i * (90.0f / 49.0f)) + 2.0f;
        if (spD4.y < 45.0f) {
            phi_fs0 = ((1.0f / 180.0f) * spD4.y) + 0.75f;
        } else if (spD4.y < 90.0f) {
            phi_fs0 = ((-1.0f / 180.0f) * spD4.y) + 1.25f;
        } else {
            phi_fs0 = ((-1.0f / 30.0f) * spD4.y) + 3.75f;
        }

        spD4.x = Math_SinS(angle) * 40.0f * phi_fs0;
        spD4.z = (Rand_ZeroOne() * 6.0f) - 3.0f;

        Matrix_MultVec3f(&spD4, &pos);

        velocity.x = (Rand_ZeroOne() - 0.5f) + (pos.x * 0.075f);
        velocity.y = 2.0f * Rand_ZeroOne();
        velocity.z = (Rand_ZeroOne() - 0.5f) + (pos.z * 0.075f);

        pos.x += this->dyna.actor.world.pos.x;
        pos.y += this->dyna.actor.world.pos.y;
        pos.z += this->dyna.actor.world.pos.z;

        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -0x82, 0x40, 0x28, 0, 0, sLeafScales[i & 3], 0, 0, 44, -1,
                             GAMEPLAY_KEEP, sLeafDlists[(s32)Rand_Next() > 0]);

        if ((i > 20) && ((i % 2) != 0)) {
            accel.x = (Rand_ZeroOne() - 0.5f) * 0.2f;
            accel.y = (Rand_ZeroOne() * 0.02f) - 0.1f;
            accel.z = (Rand_ZeroOne() - 0.5f) * 0.2f;

            func_800B12F0(play, &pos, &gZeroVec3f, &accel, ((Rand_Next() >> 26) + 10), (Rand_Next() >> 28), 50);
        }
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 200, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgDkjailIvy_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgDkjailIvy* this = (BgDkjailIvy*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    Collider_InitCylinder(play, &this->collider);

    if (Flags_GetSwitch(play, BG_DKJAIL_GET_SWITCH_FLAG(thisx))) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    DynaPolyActor_LoadMesh(play, &this->dyna, &gDkjailCol);
    Collider_SetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
    this->alpha = 255;
    BgDkjailIvy_SetupWaitForCut(this);
}

void BgDkjailIvy_Destroy(Actor* thisx, PlayState* play) {
    BgDkjailIvy* this = (BgDkjailIvy*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(play, &this->collider);
}

void BgDkjailIvy_SetupWaitForCut(BgDkjailIvy* this) {
    this->actionFunc = BgDkjailIvy_WaitForCut;
}

void BgDkjailIvy_WaitForCut(BgDkjailIvy* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        CutsceneManager_Queue(this->dyna.actor.csId);
        BgDkjailIvy_SetupCutscene(this);
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void BgDkjailIvy_SetupCutscene(BgDkjailIvy* this) {
    this->actionFunc = BgDkjailIvy_BeginCutscene;
}

void BgDkjailIvy_BeginCutscene(BgDkjailIvy* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        this->fadeOutTimer = 50;
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        Flags_SetSwitch(play, BG_DKJAIL_GET_SWITCH_FLAG(&this->dyna.actor));
        BgDkjailIvy_IvyCutEffects(this, play);
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_GRASS_WALL_BROKEN);
        BgDkjailIvy_SetupFadeOut(this);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void BgDkjailIvy_SetupFadeOut(BgDkjailIvy* this) {
    this->actionFunc = BgDkjailIvy_FadeOut;
}

void BgDkjailIvy_FadeOut(BgDkjailIvy* this, PlayState* play) {
    if (this->alpha > 8) {
        this->alpha -= 8;
    } else {
        this->alpha = 0;
        this->dyna.actor.draw = NULL;
    }

    this->fadeOutTimer--;
    if (this->fadeOutTimer <= 0) {
        Actor_Kill(&this->dyna.actor);
    }
}

void BgDkjailIvy_Update(Actor* thisx, PlayState* play) {
    BgDkjailIvy* this = (BgDkjailIvy*)thisx;

    this->actionFunc(this, play);
}

void BgDkjailIvy_Draw(Actor* thisx, PlayState* play) {
    BgDkjailIvy* this = (BgDkjailIvy*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0xFF, 255, 255, 255, this->alpha);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, &gDkjailIvyDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
