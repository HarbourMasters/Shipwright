/*
 * File: z_bg_kin2_picture.c
 * Overlay: ovl_Bg_Kin2_Picture
 * Description: Ocean Spider House - Skullkid Painting
 */

#include "z_bg_kin2_picture.h"
#include "assets/objects/object_kin2_obj/object_kin2_obj.h"

#define FLAGS 0x00000000

void BgKin2Picture_Init(Actor* thisx, PlayState* play);
void BgKin2Picture_Destroy(Actor* thisx, PlayState* play);
void BgKin2Picture_Update(Actor* thisx, PlayState* play);
void BgKin2Picture_Draw(Actor* thisx, PlayState* play);

void BgKin2Picture_SetupWait(BgKin2Picture* this);
void BgKin2Picture_Wait(BgKin2Picture* this, PlayState* play);
void BgKin2Picture_SetupPlayCutscene(BgKin2Picture* this);
void BgKin2Picture_PlayCutscene(BgKin2Picture* this, PlayState* play);
void BgKin2Picture_SetupShiver(BgKin2Picture* this);
void BgKin2Picture_Shiver(BgKin2Picture* this, PlayState* play);
void BgKin2Picture_SetupFall(BgKin2Picture* this);
void BgKin2Picture_Fall(BgKin2Picture* this, PlayState* play);
void BgKin2Picture_SetupDoNothing(BgKin2Picture* this);
void BgKin2Picture_DoNothing(BgKin2Picture* this, PlayState* play);

ActorProfile Bg_Kin2_Picture_Profile = {
    /**/ ACTOR_BG_KIN2_PICTURE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_KIN2_OBJ,
    /**/ sizeof(BgKin2Picture),
    /**/ BgKin2Picture_Init,
    /**/ BgKin2Picture_Destroy,
    /**/ BgKin2Picture_Update,
    /**/ BgKin2Picture_Draw,
};

static ColliderTrisElementInit sTrisElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x000138B0, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -20.0f, 53.3f, 9.0f }, { -20.0f, 3.0f, 9.0f }, { 20.0f, 3.0f, 9.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x000138B0, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -20.0f, 53.3f, 9.0f }, { 20.0f, 3.0f, 9.0f }, { 20.0f, 53.3f, 9.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit),
    sTrisElementsInit,
};

static Vec3f sDustBasePos = { 0.0f, 23.0f, 0.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(terminalVelocity, -20000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

bool BgKin2Picture_IsSkulltulaCollected(PlayState* play, s32 skulltulaParams) {
    s32 flag = -1;

    if ((u8)skulltulaParams & 3) {
        flag = BG_KIN2_PICTURE_GET_3FC(skulltulaParams);
    }

    return (flag >= 0) && Flags_GetTreasure(play, flag);
}

void BgKin2Picture_SpawnSkulltula(BgKin2Picture* this, PlayState* play2) {
    PlayState* play = play2;
    s32 skulltulaSpawnParams;

    if (!BG_KIN2_PICTURE_SKULLTULA_COLLECTED(&this->dyna.actor)) { // Gold Skulltula is still here.
        skulltulaSpawnParams = BG_KIN2_PICTURE_SKULLTULA_SPAWN_PARAM(&this->dyna.actor);
        if (!BgKin2Picture_IsSkulltulaCollected(play, skulltulaSpawnParams) &&
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_SW, this->dyna.actor.home.pos.x,
                        this->dyna.actor.home.pos.y + 23.0f, this->dyna.actor.home.pos.z, 0,
                        this->dyna.actor.home.rot.y, 0, skulltulaSpawnParams)) {
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
        }
    }
}

#define DUST_COUNT 20

void BgKin2Picture_SpawnDust(BgKin2Picture* this, PlayState* play) {
    f32 offset;
    Vec3f basePos;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    s32 angle;
    s32 scale;
    s16 scaleStep;
    s32 baseAngle;
    s32 i;

    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x,
                                 this->dyna.actor.world.pos.y +
                                     (this->dyna.actor.shape.yOffset * this->dyna.actor.scale.y),
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
    Matrix_MultVec3f(&sDustBasePos, &basePos);
    pos.y = basePos.y - 7.0f;
    velocity.y = 0.0f;
    accel.y = 0.2f;

    for (i = 0, baseAngle = 0; i < DUST_COUNT; i++, baseAngle += (0x10000 / DUST_COUNT)) {
        angle = (s32)(Rand_ZeroOne() * (0x10000 / DUST_COUNT)) + baseAngle;
        offset = (Rand_ZeroOne() * 14.0f) + 4.0f;
        pos.x = Math_SinS(angle) * offset;
        pos.z = Math_CosS(angle) * offset;
        velocity.x = (Rand_ZeroOne() - 0.5f) + (pos.x * (1.0f / 6.0f));
        velocity.z = (Rand_ZeroOne() - 0.5f) + (pos.z * (1.0f / 6.0f));
        pos.x += basePos.x;
        pos.z += basePos.z;
        accel.x = velocity.x * -0.09f;
        accel.z = velocity.z * -0.09f;
        scale = (s32)(Rand_ZeroOne() * 10.0f) + 10;
        scaleStep = (s32)(Rand_ZeroOne() * 10.0f) + 15;
        func_800B1210(play, &pos, &velocity, &accel, scale, scaleStep);
    }
}

void BgKin2Picture_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgKin2Picture* this = (BgKin2Picture*)thisx;
    s32 skulltulaParams;
    Vec3f vertices[3];
    s32 i;
    s32 j;
    ColliderTrisElementInit* element;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gOceanSpiderHouseSkullkidPaintingCol);
    DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_InitTris(play, &this->colliderTris);
    Collider_SetTris(play, &this->colliderTris, &this->dyna.actor, &sTrisInit, this->colliderElement);
    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);

    for (i = 0; i < ARRAY_COUNT(sTrisElementsInit); i++) {
        for (j = 0; j < ARRAY_COUNT(vertices); j++) {
            element = &sTrisInit.elements[i];
            Matrix_MultVec3f(&element->dim.vtx[j], &vertices[j]);
        }
        Collider_SetTrisVertices(&this->colliderTris, i, &vertices[0], &vertices[1], &vertices[2]);
    }

    Actor_SetFocus(&this->dyna.actor, 23.0f);
    skulltulaParams = BG_KIN2_PICTURE_SKULLTULA_SPAWN_PARAM(&this->dyna.actor);

    if (BG_KIN2_PICTURE_SKULLTULA_COLLECTED(&this->dyna.actor) ||
        BgKin2Picture_IsSkulltulaCollected(play, skulltulaParams)) {
        this->skulltulaNoiseTimer = -1;
    }

    BgKin2Picture_SetupWait(this);
}

void BgKin2Picture_Destroy(Actor* thisx, PlayState* play) {
    BgKin2Picture* this = (BgKin2Picture*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyTris(play, &this->colliderTris);
}

void BgKin2Picture_SetupWait(BgKin2Picture* this) {
    this->actionFunc = BgKin2Picture_Wait;
}

void BgKin2Picture_Wait(BgKin2Picture* this, PlayState* play) {
    // hit by projectile
    if (this->colliderTris.base.acFlags & AC_HIT) {
        this->colliderTris.base.acFlags &= ~AC_HIT;
        CutsceneManager_Queue(this->dyna.actor.csId);
        BgKin2Picture_SetupPlayCutscene(this);
    } else { // Gold Skulltula can be heard behind Skullkid's painting.
        if (this->skulltulaNoiseTimer >= 0) {
            if (this->skulltulaNoiseTimer == 0) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_STALGOLD_ROLL);

                if (Rand_ZeroOne() < 0.1f) {
                    this->skulltulaNoiseTimer = Rand_S16Offset(40, 80);
                } else {
                    this->skulltulaNoiseTimer = 8;
                }
            } else {
                this->skulltulaNoiseTimer--;
            }
        }
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderTris.base);
    }
}

void BgKin2Picture_SetupPlayCutscene(BgKin2Picture* this) {
    this->actionFunc = BgKin2Picture_PlayCutscene;
}

void BgKin2Picture_PlayCutscene(BgKin2Picture* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        this->cutsceneStarted = true;
        BgKin2Picture_SetupShiver(this);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void BgKin2Picture_SetupShiver(BgKin2Picture* this) {
    this->paintingTimer = 13;
    this->xOffsetAngle = 0;
    this->yOffsetAngle = 0;
    this->actionFunc = BgKin2Picture_Shiver;
}

void BgKin2Picture_Shiver(BgKin2Picture* this, PlayState* play) {
    s32 pad;
    Vec3f basePosOffset;
    Vec3f posOffset;

    this->paintingTimer--;
    if (this->paintingTimer <= 0) {
        Math_Vec3f_Copy(&this->dyna.actor.world.pos, &this->dyna.actor.home.pos);
        BgKin2Picture_SetupFall(this);
    } else {
        this->xOffsetAngle += 0x7BAC;
        this->yOffsetAngle += 0x4E20;
        basePosOffset.x = Math_CosS(this->xOffsetAngle);
        basePosOffset.y = Math_CosS(this->yOffsetAngle) * 0.2f;
        basePosOffset.z = 0.0f;
        Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);
        Matrix_MultVec3f(&basePosOffset, &posOffset);
        Math_Vec3f_Sum(&this->dyna.actor.home.pos, &posOffset, &this->dyna.actor.world.pos);
    }
}

void BgKin2Picture_SetupFall(BgKin2Picture* this) {
    this->landTimer = 0;
    this->step = 0;
    this->paintingTimer = 4;
    this->actionFunc = BgKin2Picture_Fall;
}

void BgKin2Picture_Fall(BgKin2Picture* this, PlayState* play) {
    if (this->paintingTimer > 0) {
        this->paintingTimer--;

        if (this->paintingTimer == 0) {
            BgKin2Picture_SpawnSkulltula(this, play);
        }
    }

    Actor_MoveWithGravity(&this->dyna.actor);
    Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Math_StepToS(&this->step, 0x7D0, 0x78);

        if (this->landTimer < 3) {
            this->landTimer++;

            if (this->dyna.actor.velocity.y <= 0.01f) {
                this->dyna.actor.velocity.y *= -0.7f;

                if (this->dyna.actor.velocity.y > 6.0f) {
                    this->dyna.actor.velocity.y = 6.0f;
                }

                if (this->landTimer < 3) {
                    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_SMALL_WOODPLATE_BOUND_0);
                }
            }
        }
    }

    Actor_SetFocus(&this->dyna.actor, 23.0f);

    if (!this->hasSpawnedDust && (this->dyna.actor.shape.rot.x > 0x3300)) {
        BgKin2Picture_SpawnDust(this, play);
        this->hasSpawnedDust = true;
    }

    if (Math_ScaledStepToS(&this->dyna.actor.shape.rot.x, 0x4000, this->step)) { // facing the floor
        this->dyna.actor.shape.yOffset = 40.0f;

        if (this->cutsceneStarted) {
            CutsceneManager_Stop(this->dyna.actor.csId);
        }

        DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WOODPLATE_BROKEN);
        BgKin2Picture_SetupDoNothing(this);
    } else {
        this->dyna.actor.shape.yOffset = Math_SinS(this->dyna.actor.shape.rot.x) * 40.0f;
    }
}

void BgKin2Picture_SetupDoNothing(BgKin2Picture* this) {
    this->actionFunc = BgKin2Picture_DoNothing;
}

void BgKin2Picture_DoNothing(BgKin2Picture* this, PlayState* play) {
}

void BgKin2Picture_Update(Actor* thisx, PlayState* play) {
    BgKin2Picture* this = (BgKin2Picture*)thisx;

    this->actionFunc(this, play);
}

void BgKin2Picture_Draw(Actor* thisx, PlayState* play) {
    BgKin2Picture* this = (BgKin2Picture*)thisx;

    Gfx_DrawDListOpa(play, gOceanSpiderHouseSkullkidPaintingDL);
}
