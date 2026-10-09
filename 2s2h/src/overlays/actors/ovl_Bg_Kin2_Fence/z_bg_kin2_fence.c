/*
 * File: z_bg_kin2_fence.c
 * Overlay: ovl_Bg_Kin2_Fence
 * Description: Ocean Spider House - Fireplace Grate
 */

#include "z_bg_kin2_fence.h"
#include "objects/object_kin2_obj/object_kin2_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgKin2Fence_Init(Actor* thisx, PlayState* play);
void BgKin2Fence_Destroy(Actor* thisx, PlayState* play);
void BgKin2Fence_Update(Actor* thisx, PlayState* play);
void BgKin2Fence_Draw(Actor* thisx, PlayState* play);

void BgKin2Fence_SetupHandleMaskCode(BgKin2Fence* this);
void BgKin2Fence_HandleMaskCode(BgKin2Fence* this, PlayState* play);
void BgKin2Fence_SetupPlayOpenCutscene(BgKin2Fence* this);
void BgKin2Fence_PlayOpenCutscene(BgKin2Fence* this, PlayState* play);
void BgKin2Fence_SetupWaitBeforeOpen(BgKin2Fence* this);
void BgKin2Fence_WaitBeforeOpen(BgKin2Fence* this, PlayState* play);
void BgKin2Fence_SetupRaiseFence(BgKin2Fence* this);
void BgKin2Fence_RaiseFence(BgKin2Fence* this, PlayState* play);
void BgKin2Fence_SetupDoNothing(BgKin2Fence* this);
void BgKin2Fence_DoNothing(BgKin2Fence* this, PlayState* play);

ActorProfile Bg_Kin2_Fence_Profile = {
    /**/ ACTOR_BG_KIN2_FENCE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_KIN2_OBJ,
    /**/ sizeof(BgKin2Fence),
    /**/ BgKin2Fence_Init,
    /**/ BgKin2Fence_Destroy,
    /**/ BgKin2Fence_Update,
    /**/ BgKin2Fence_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[4] = {
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00003820, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { -2040, 1400, 350 }, 28 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00003820, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { -1140, 1400, 350 }, 28 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00003820, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { 1140, 1400, 350 }, 28 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00003820, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { 2040, 1400, 350 }, 28 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

s32 BgKin2Fence_CheckHitMask(BgKin2Fence* this) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->colliderElements); i++) {
        if (this->collider.elements[i].base.acElemFlags & ACELEM_HIT) {
            return i;
        }
    }
    return -1;
}

static Vec3f sEyeSparkleSpawnPositions[][2] = {
    { { -215.0f, 139.0f, 50.0f }, { -193.0f, 139.0f, 50.0f } },
    { { -125.0f, 139.0f, 50.0f }, { -103.0f, 139.0f, 50.0f } },
    { { 103.0f, 139.0f, 50.0f }, { 125.0f, 139.0f, 50.0f } },
    { { 193.0f, 139.0f, 50.0f }, { 215.0f, 139.0f, 50.0f } },
};

void BgKin2Fence_SpawnEyeSparkles(BgKin2Fence* this, PlayState* play, s32 mask) {
    static Color_RGBA8 sPrimColor = { 255, 255, 255, 0 };
    static Color_RGBA8 sEnvColor = { 0, 128, 128, 0 };
    s32 i;
    Vec3f sp58;
    s32 pad[2];

    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);

    for (i = 0; i < 2; i++) {
        Matrix_MultVec3f(&sEyeSparkleSpawnPositions[mask][i], &sp58);
        EffectSsKirakira_SpawnDispersed(play, &sp58, &gZeroVec3f, &gZeroVec3f, &sPrimColor, &sEnvColor, 6000, -10);
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgKin2Fence_Init(Actor* thisx, PlayState* play) {
    BgKin2Fence* this = (BgKin2Fence*)thisx;
    s32 i = 0;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gOceanSpiderHouseFireplaceGrateCol);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->dyna.actor, &sJntSphInit, this->colliderElements);
    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
    Matrix_Scale(this->dyna.actor.scale.x, this->dyna.actor.scale.y, this->dyna.actor.scale.z, MTXMODE_APPLY);

    for (i = 0; i < 4; i++) {
        Collider_UpdateSpheres(i, &this->collider);
    }

    if (Flags_GetSwitch(play, BG_KIN2_FENCE_GET_SWITCH_FLAG(&this->dyna.actor))) {
        BgKin2Fence_SetupDoNothing(this);
        return;
    }
    BgKin2Fence_SetupHandleMaskCode(this);
}

void BgKin2Fence_Destroy(Actor* thisx, PlayState* play) {
    BgKin2Fence* this = (BgKin2Fence*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyJntSph(play, &this->collider);
}

void BgKin2Fence_SetupHandleMaskCode(BgKin2Fence* this) {
    this->masksHit = 0;
    this->actionFunc = BgKin2Fence_HandleMaskCode;
}

void BgKin2Fence_HandleMaskCode(BgKin2Fence* this, PlayState* play) {
    s32 hitMask;
    s32 nextMask;

    if (this->collider.base.acFlags & AC_HIT) {
        hitMask = BgKin2Fence_CheckHitMask(this);
        if (hitMask >= 0) {
            nextMask = (s8)gSaveContext.save.saveInfo.spiderHouseMaskOrder[this->masksHit];
            if (hitMask == nextMask) {
                Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
                this->masksHit += 1;
                BgKin2Fence_SpawnEyeSparkles(this, play, nextMask);
            } else {
                Audio_PlaySfx(NA_SE_SY_ERROR);
                this->masksHit = 0;
            }
        }
        this->collider.base.acFlags &= ~AC_HIT;
        this->cooldownTimer = 5;
        if (this->masksHit > 5) {
            BgKin2Fence_SetupPlayOpenCutscene(this);
            return;
        }
    } else {
        if (this->cooldownTimer > 0) {
            this->cooldownTimer -= 1;
            return;
        }
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void BgKin2Fence_SetupPlayOpenCutscene(BgKin2Fence* this) {
    this->actionFunc = BgKin2Fence_PlayOpenCutscene;
}

void BgKin2Fence_PlayOpenCutscene(BgKin2Fence* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        Flags_SetSwitch(play, BG_KIN2_FENCE_GET_SWITCH_FLAG(&this->dyna.actor));
        BgKin2Fence_SetupWaitBeforeOpen(this);
        return;
    }
    CutsceneManager_Queue(this->dyna.actor.csId);
}

void BgKin2Fence_SetupWaitBeforeOpen(BgKin2Fence* this) {
    this->waitBeforeOpenTimer = 14;
    this->actionFunc = BgKin2Fence_WaitBeforeOpen;
}

void BgKin2Fence_WaitBeforeOpen(BgKin2Fence* this, PlayState* play) {
    if (this->waitBeforeOpenTimer > 0) {
        this->waitBeforeOpenTimer -= 1;
    } else {
        BgKin2Fence_SetupRaiseFence(this);
    }
}

void BgKin2Fence_SetupRaiseFence(BgKin2Fence* this) {
    this->actionFunc = BgKin2Fence_RaiseFence;
}

void BgKin2Fence_RaiseFence(BgKin2Fence* this, PlayState* play) {
    if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 58.0f, 5.0f)) {
        BgKin2Fence_SetupDoNothing(this);
    }
}
void BgKin2Fence_SetupDoNothing(BgKin2Fence* this) {
    this->actionFunc = BgKin2Fence_DoNothing;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 58.0f;
}

void BgKin2Fence_DoNothing(BgKin2Fence* this, PlayState* play) {
}

void BgKin2Fence_Update(Actor* thisx, PlayState* play) {
    BgKin2Fence* this = (BgKin2Fence*)thisx;

    this->actionFunc(this, play);
}

void BgKin2Fence_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gOceanSpiderHouseFireplaceGrateDL);
}
