/*
 * File: z_en_kusa.c
 * Overlay: ovl_En_Kusa
 * Description: Grass / Bush
 */

#include "prevent_bss_reordering.h"
#include "z_en_kusa.h"
#include "objects/object_kusa/object_kusa.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "overlays/actors/ovl_En_Insect/z_en_insect.h"

#include "2s2h/ShipUtils.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_THROW_ONLY)

void EnKusa_Init(Actor* thisx, PlayState* play);
void EnKusa_Destroy(Actor* thisx, PlayState* play);
void EnKusa_Update(Actor* thisx, PlayState* play2);

s32 EnKusa_SnapToFloor(EnKusa* this, PlayState* play, f32 yOffset);
void EnKusa_DropCollectible(EnKusa* this, PlayState* play);
void EnKusa_UpdateVelY(EnKusa* this);
void EnKusa_RandScaleVecToZero(Vec3f* vec, f32 scaleFactor);
void EnKusa_SetScaleSmall(EnKusa* this);
s32 EnKusa_IsUnderwater(EnKusa* this, PlayState* play);
void EnKusa_SetupWaitObject(EnKusa* this);
void EnKusa_WaitObject(EnKusa* this, PlayState* play);
void EnKusa_WaitForInteract(EnKusa* this, PlayState* play);
void EnKusa_SetupLiftedUp(EnKusa* this);
void EnKusa_LiftedUp(EnKusa* this, PlayState* play);
void EnKusa_WaitObject(EnKusa* this, PlayState* play);
void EnKusa_SetupInteract(EnKusa* this);
void EnKusa_SetupFall(EnKusa* this);
void EnKusa_Fall(EnKusa* this, PlayState* play);
void EnKusa_SetupCut(EnKusa* this);
void EnKusa_CutWaitRegrow(EnKusa* this, PlayState* play);
void EnKusa_DoNothing(EnKusa* this, PlayState* play);
void EnKusa_SetupUprootedWaitRegrow(EnKusa* this);
void EnKusa_UprootedWaitRegrow(EnKusa* this, PlayState* play);
void EnKusa_SetupRegrow(EnKusa* this);
void EnKusa_Regrow(EnKusa* this, PlayState* play);
void EnKusa_DrawBush(Actor* thisx, PlayState* play2);
void EnKusa_DrawGrass(Actor* thisx, PlayState* play);

s16 rotSpeedXtarget = 0;
s16 rotSpeedX = 0;
s16 rotSpeedYtarget = 0;
s16 rotSpeedY = 0;
s16 D_809366B0 = 0;
u8 D_809366B4 = true;

u32 kusaGameplayFrames;
MtxF D_80936AD8[8];
s16 D_80936CD8;
s16 D_80936CDA;
s16 D_80936CDC;
s16 D_80936CDE;
s16 D_80936CE0;

ActorProfile En_Kusa_Profile = {
    /**/ ACTOR_EN_KUSA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnKusa),
    /**/ EnKusa_Init,
    /**/ EnKusa_Destroy,
    /**/ EnKusa_Update,
    /**/ NULL,
};

static s16 sObjectIds[] = { GAMEPLAY_FIELD_KEEP, OBJECT_KUSA, OBJECT_KUSA, OBJECT_KUSA };

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00400000, 0x00, 0x02 },
        { 0x0580C71C, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 6, 44, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 30, MASS_IMMOVABLE };

static Vec3f sUnitDirections[] = {
    { 0.0f, 0.7071f, 0.7071f },
    { 0.7071f, 0.7071f, 0.0f },
    { 0.0f, 0.7071f, -0.7071f },
    { -0.7071f, 0.7071f, 0.0f },
};

static s16 sFragmentScales[] = { 108, 102, 96, 84, 66, 55, 42, 38 };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3200, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(terminalVelocity, -17000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 200, ICHAIN_STOP),
};

/**
 * @brief Applies a "swaying" motion to the provided matrix
 *
 */
void EnKusa_ApplySway(MtxF* matrix) {
    MtxF* mtxState = Matrix_GetCurrent();
    f32* tmp = &mtxState->mf[0][0];
    f32* tmp2 = &matrix->mf[0][0];
    s32 i;

    for (i = 0; i < 16; i++) {
        *tmp++ += *tmp2++;
    }
}

/**
 * @brief Updates the matrix controlling movement of the leaves of grass to simulate a swaying motion from the wind
 * blowing.
 */
void EnKusa_Sway(void) {
    s32 i;
    s32 pad;
    f32 sin_6CDA;
    f32* ptr;
    f32 sin_6CDE;
    f32 sin_6CE0;
    f32 tempf1;
    f32 tempf2;
    f32 tempf3;
    f32 tempf4;
    f32 tempf5;
    f32 sp7C[8];
    f32 cos_6CE0;
    f32 cos_6CDA;
    f32 cos_6CDC;
    f32 cos_6CDE;
    f32 cos_6CD8;
    f32 sin_6CD8;
    f32 sin_6CDC;

    D_80936CD8 += 0x46;
    D_80936CDA += 0x12C;
    D_80936CDC += 0x2BC;
    D_80936CDE += 0x514;
    D_80936CE0 += 0x22C4;

    sin_6CD8 = Math_SinS(D_80936CD8);
    sin_6CDA = Math_SinS(D_80936CDA);
    sin_6CDC = Math_SinS(D_80936CDC);
    sin_6CDE = Math_SinS(D_80936CDE) * 1.2f;
    sin_6CE0 = Math_SinS(D_80936CE0) * 1.5f;

    cos_6CD8 = Math_CosS(D_80936CD8);
    cos_6CDA = Math_CosS(D_80936CDA);
    cos_6CDC = Math_CosS(D_80936CDC);
    cos_6CDE = Math_CosS(D_80936CDE) * 1.3f;
    cos_6CE0 = Math_CosS(D_80936CE0) * 1.7f;

    sp7C[0] = (sin_6CD8 - cos_6CDA) * sin_6CDC * cos_6CD8 * sin_6CD8 * 0.0015f;
    sp7C[1] = (sin_6CDA - cos_6CDC) * sin_6CDE * cos_6CDA * sin_6CD8 * 0.0015f;
    sp7C[2] = (sin_6CDC - cos_6CDE) * cos_6CDC * sin_6CD8 * cos_6CD8 * 0.0015f;
    sp7C[3] = (sin_6CDE - cos_6CDA) * cos_6CDE * sin_6CDA * cos_6CD8 * 0.0015f;
    sp7C[4] = (sin_6CD8 - cos_6CDC) * sin_6CD8 * sin_6CDA * sin_6CE0 * 0.0015f;
    sp7C[5] = (sin_6CDA - cos_6CDE) * sin_6CDC * sin_6CDE * sin_6CE0 * 0.0015f;
    sp7C[6] = (sin_6CDC - cos_6CD8) * cos_6CD8 * cos_6CDA * cos_6CE0 * 0.0015f;
    sp7C[7] = (sin_6CDE - cos_6CDA) * cos_6CDC * cos_6CDE * cos_6CE0 * 0.0015f;

    for (i = 0; i < ARRAY_COUNT(D_80936AD8); i++) {
        ptr = &D_80936AD8[i].mf[0][0];

        tempf1 = sp7C[i & 7];
        tempf2 = sp7C[(i + 1) & 7];
        tempf3 = sp7C[(i + 2) & 7];
        tempf4 = sp7C[(i + 3) & 7];
        tempf5 = sp7C[(i + 4) & 7];

        ptr[0] = sp7C[1] * 0.2f;
        ptr[1] = tempf1;
        ptr[2] = tempf2;
        ptr[3] = 0.0f;

        ptr[4] = tempf3;
        ptr[5] = sp7C[0];
        ptr[6] = tempf3;
        ptr[7] = 0.0f;

        ptr[8] = tempf4;
        ptr[9] = tempf5;
        ptr[10] = sp7C[3] * 0.2f;
        ptr[11] = 0.0f;

        ptr[12] = 0.0f;
        ptr[13] = 0.0f;
        ptr[14] = 0.0f;
        ptr[15] = 0.0f;
    }
}

/**
 * @brief Detects if a bush is able to snap to the floor. BgCheck_EntityRaycastFloor5 will give the intersect point
 *        if no poit is found, a false value is returned.
 *
 * @param this
 * @param play
 * @param yOffset offset of Y coordinate, can be positive or negative.
 * @return true/false if the bush is able to snap to the floor and is above BGCHECK_Y_MIN
 */
s32 EnKusa_SnapToFloor(EnKusa* this, PlayState* play, f32 yOffset) {
    s32 pad;
    CollisionPoly* poly;
    Vec3f pos;
    s32 bgId;
    f32 floorY;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + 30.0f;
    pos.z = this->actor.world.pos.z;

    floorY = BgCheck_EntityRaycastFloor5(&play->colCtx, &poly, &bgId, &this->actor, &pos);
    if (floorY > BGCHECK_Y_MIN) {
        this->actor.world.pos.y = floorY + yOffset;
        Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
        return true;
    } else {
        return false;
    }
}

void EnKusa_DropCollectible(EnKusa* this, PlayState* play) {
    s32 collectible;
    s32 collectableParams;

    if ((KUSA_GET_TYPE(&this->actor) == ENKUSA_TYPE_GRASS) || (KUSA_GET_TYPE(&this->actor) == ENKUSA_TYPE_BUSH)) {
        if (!KUSA_GET_PARAM_0C(&this->actor)) {
            if (GameInteractor_Should(VB_GRASS_DROP_COLLECTIBLE, true, ACTOR_EN_KUSA, this)) {
                Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos,
                                           KUSA_GET_RAND_COLLECTIBLE_ID(&this->actor) * 0x10);
            }
        }
    } else if (KUSA_GET_TYPE(&this->actor) == ENKUSA_TYPE_REGROWING_GRASS) {
        Item_DropCollectible(play, &this->actor.world.pos, 3);
    } else { // ENKUSA_TYPE_GRASS_2
        collectible = func_800A8150(KUSA_GET_PARAM_FC(&this->actor));
        if (GameInteractor_Should(VB_GRASS_DROP_COLLECTIBLE, collectible >= 0, ACTOR_EN_KUSA, this)) {
            collectableParams = KUSA_GET_COLLECTIBLE_ID(&this->actor);
            Item_DropCollectible(play, &this->actor.world.pos, (collectableParams << 8) | collectible);
        }
    }
}

void EnKusa_UpdateVelY(EnKusa* this) {
    this->actor.velocity.y += this->actor.gravity;
    if (this->actor.velocity.y < this->actor.terminalVelocity) {
        this->actor.velocity.y = this->actor.terminalVelocity;
    }
}

/**
 * @brief Scales a vector down by provided scale factor
 *
 * @param vec vector to be scaled
 * @param scaleFactor scale factor to be applied to vector
 */
void EnKusa_RandScaleVecToZero(Vec3f* vec, f32 scaleFactor) {
    scaleFactor += ((Rand_ZeroOne() * 0.2f) - 0.1f) * scaleFactor;
    vec->x -= vec->x * scaleFactor;
    vec->y -= vec->y * scaleFactor;
    vec->z -= vec->z * scaleFactor;
}

void EnKusa_SetScaleSmall(EnKusa* this) {
    this->actor.scale.y = 160.0f * 0.001f;
    this->actor.scale.x = 120.0f * 0.001f;
    this->actor.scale.z = 120.0f * 0.001f;
}

void EnKusa_SpawnFragments(EnKusa* this, PlayState* play) {
    Vec3f velocity;
    Vec3f pos;
    s32 i;
    s32 scaleIndex;
    Vec3f* directon;
    s32 pad;

    for (i = 0; i < ARRAY_COUNT(sUnitDirections); i++) {
        directon = &sUnitDirections[i];

        pos.x = this->actor.world.pos.x + (directon->x * this->actor.scale.x * 20.0f);
        pos.y = this->actor.world.pos.y + (directon->y * this->actor.scale.y * 20.0f) + 10.0f;
        pos.z = this->actor.world.pos.z + (directon->z * this->actor.scale.z * 20.0f);
        velocity.x = (Rand_ZeroOne() - 0.5f) * 8.0f;
        velocity.y = Rand_ZeroOne() * 10.0f;
        velocity.z = (Rand_ZeroOne() - 0.5f) * 8.0f;

        scaleIndex = (s32)(Rand_ZeroOne() * 111.1f) & 7;

        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -100, 64, 40, 3, 0, sFragmentScales[scaleIndex], 0, 0, 0x50,
                             -1, 1, gKakeraLeafMiddleDL);

        pos.x = this->actor.world.pos.x + (directon->x * this->actor.scale.x * 40.0f);
        pos.y = this->actor.world.pos.y + (directon->y * this->actor.scale.y * 40.0f) + 10.0f;
        pos.z = this->actor.world.pos.z + (directon->z * this->actor.scale.z * 40.0f);
        velocity.x = (Rand_ZeroOne() - 0.5f) * 6.0f;
        velocity.y = Rand_ZeroOne() * 10.0f;
        velocity.z = (Rand_ZeroOne() - 0.5f) * 6.0f;

        scaleIndex = (s32)(Rand_ZeroOne() * 111.1f) % 7;

        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -100, 64, 40, 3, 0, sFragmentScales[scaleIndex], 0, 0, 0x50,
                             -1, 1, gKakeraLeafTipDL);
    }
}

void EnKusa_SpawnBugs(EnKusa* this, PlayState* play) {
    u32 numBugs;

    for (numBugs = 0; numBugs < 3; numBugs++) {
        Actor* bug = Actor_SpawnAsChildAndCutscene(
            &play->actorCtx, play, ACTOR_EN_INSECT, this->actor.world.pos.x, this->actor.world.pos.y,
            this->actor.world.pos.z, 0, 0, 0, ENINSECT_PARAMS(true), this->actor.csId, this->actor.halfDaysBits, 0);

        if (bug == NULL) {
            break;
        }
    }
}

s32 EnKusa_IsUnderwater(EnKusa* this, PlayState* play) {
    s32 pad;
    WaterBox* waterBox;
    f32 waterSurface;
    s32 bgId;

    if (WaterBox_GetSurfaceImpl(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &waterSurface,
                                &waterBox, &bgId) &&
        (this->actor.world.pos.y < waterSurface)) {
        return true;
    }
    return false;
}

void EnKusa_InitCollider(Actor* thisx, PlayState* play) {
    EnKusa* this = (EnKusa*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(thisx, &this->collider);
}

void EnKusa_Init(Actor* thisx, PlayState* play) {
    EnKusa* this = (EnKusa*)thisx;
    s32 pad;
    s32 kusaType = KUSA_GET_TYPE(&this->actor);

    Actor_ProcessInitChain(&this->actor, sInitChain);

    if (play->csCtx.state != CS_STATE_IDLE) {
        this->actor.cullingVolumeDistance += 1000.0f;
    }
    EnKusa_InitCollider(&this->actor, play);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);

    if (kusaType == ENKUSA_TYPE_BUSH) {
        this->actor.shape.shadowScale = 1.0f;
        this->actor.shape.shadowAlpha = 60;
    } else {
        this->actor.shape.shadowScale = 0.9f;
        this->actor.shape.shadowAlpha = 70;
    }

    if (this->actor.shape.rot.y == 0) {
        this->actor.shape.rot.y = (Rand_Next() >> 0x10);
        this->actor.home.rot.y = this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
    if (!EnKusa_SnapToFloor(this, play, 0.0f)) {
        Actor_Kill(&this->actor);
        return;
    }
    if (EnKusa_IsUnderwater(this, play)) {
        this->isInWater |= 1;
    }

    this->objectSlot = Object_GetSlot(&play->objectCtx, sObjectIds[(KUSA_GET_TYPE(&this->actor))]);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    EnKusa_SetupWaitObject(this);
    if (D_809366B4) {
        D_80936CD8 = Rand_Next() >> 0x10;
        D_80936CDA = Rand_Next() >> 0x10;
        D_80936CDC = Rand_Next() >> 0x10;
        D_80936CDE = Rand_Next() >> 0x10;
        D_80936CE0 = Rand_Next() >> 0x10;
        D_809366B4 = false;
        EnKusa_Sway();
        kusaGameplayFrames = play->gameplayFrames;
    }
    this->kusaMtxIdx = D_809366B0 & 7;
    D_809366B0++;
}

void EnKusa_Destroy(Actor* thisx, PlayState* play) {
    PlayState* play2 = play;
    EnKusa* this = (EnKusa*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnKusa_SetupWaitObject(EnKusa* this) {
    this->actionFunc = EnKusa_WaitObject;
}

void EnKusa_WaitObject(EnKusa* this, PlayState* play) {
    s32 pad;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        s32 kusaType = KUSA_GET_TYPE(&this->actor);

        if (this->isCut) {
            EnKusa_SetupCut(this);
        } else {
            EnKusa_SetupInteract(this);
        }
        if (GameInteractor_Should(VB_KUSA_DRAW_BE_OVERRIDDEN, true, this)) {
            if (kusaType == ENKUSA_TYPE_BUSH) {
                this->actor.draw = EnKusa_DrawBush;
            } else {
                this->actor.draw = EnKusa_DrawGrass;
            }
        }
        this->actor.objectSlot = this->objectSlot;
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }
}

void EnKusa_SetupInteract(EnKusa* this) {
    this->actionFunc = EnKusa_WaitForInteract;
    this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
}

void EnKusa_WaitForInteract(EnKusa* this, PlayState* play) {
    s32 pad;

    if (Actor_HasParent(&this->actor, play)) {
        EnKusa_SetupLiftedUp(this);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_PL_PULL_UP_PLANT);
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;

    } else if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        EnKusa_SpawnFragments(this, play);
        EnKusa_DropCollectible(this, play);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EV_PLANT_BROKEN);

        if (KUSA_SHOULD_SPAWN_BUGS(&this->actor)) {
            if (KUSA_GET_TYPE(&this->actor) != ENKUSA_TYPE_GRASS_2) {
                EnKusa_SpawnBugs(this, play);
            }
        }
        if (KUSA_GET_TYPE(&this->actor) == ENKUSA_TYPE_BUSH) {
            Actor_Kill(&this->actor);
            return;
        }

        EnKusa_SetupCut(this);
        this->isCut = true;
    } else {
        if (!(this->collider.base.ocFlags1 & OC1_TYPE_PLAYER) && (this->actor.xzDistToPlayer > 12.0f)) {
            this->collider.base.ocFlags1 |= OC1_TYPE_PLAYER;
        }

        if (this->actor.xzDistToPlayer < 600.0f) {
            Collider_UpdateCylinder(&this->actor, &this->collider);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

            if (this->actor.xzDistToPlayer < 400.0f) {
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
                if (this->actor.xzDistToPlayer < 100.0f) {
                    if (KUSA_GET_TYPE(&this->actor) != ENKUSA_TYPE_GRASS_2) {
                        Actor_OfferCarry(&this->actor, play);
                    }
                }
            }
        }
    }
}

void EnKusa_SetupLiftedUp(EnKusa* this) {
    this->actionFunc = EnKusa_LiftedUp;
    this->actor.room = -1;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
}

void EnKusa_LiftedUp(EnKusa* this, PlayState* play) {
    s32 pad;
    Vec3f pos;
    s32 bgId;

    if (Actor_HasNoParent(&this->actor, play)) {
        this->actor.room = play->roomCtx.curRoom.num;
        EnKusa_SetupFall(this);
        this->actor.velocity.x = this->actor.speed * Math_SinS(this->actor.world.rot.y);
        this->actor.velocity.z = this->actor.speed * Math_CosS(this->actor.world.rot.y);
        this->actor.colChkInfo.mass = 80;
        this->actor.gravity = -0.1f;
        EnKusa_UpdateVelY(this);
        EnKusa_RandScaleVecToZero(&this->actor.velocity, 0.005f);
        Actor_UpdatePos(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 7.5f, 35.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40 |
                                    UPDBGCHECKINFO_FLAG_80);
        this->actor.gravity = -3.2f;
    } else {
        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 20.0f;
        pos.z = this->actor.world.pos.z;
        this->actor.floorHeight =
            BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &pos);
    }
}

void EnKusa_SetupFall(EnKusa* this) {
    this->actionFunc = EnKusa_Fall;
    rotSpeedXtarget = -0xBB8;
    rotSpeedYtarget = (Rand_ZeroOne() - 0.5f) * 1600.0f;
    rotSpeedX = 0;
    rotSpeedY = 0;
    this->timer = 0;
}

void EnKusa_Fall(EnKusa* this, PlayState* play) {
    s32 pad;
    s32 wasHit;
    Vec3f contactPos;
    s32 i;
    s16 angleOffset;

    wasHit = (this->collider.base.atFlags & AT_HIT) != 0;

    if (wasHit) {
        this->collider.base.atFlags &= ~AT_HIT;
    }
    this->timer++;
    if ((this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_WALL)) || wasHit ||
        (this->timer >= 100)) {
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EV_PLANT_BROKEN);
        }
        EnKusa_SpawnFragments(this, play);
        EnKusa_DropCollectible(this, play);
        switch (KUSA_GET_TYPE(&this->actor)) {
            case ENKUSA_TYPE_BUSH:
            case ENKUSA_TYPE_GRASS:
                Actor_Kill(&this->actor);
                break;

            case ENKUSA_TYPE_REGROWING_GRASS:
                EnKusa_SetupUprootedWaitRegrow(this);
                this->actor.shape.shadowDraw = NULL;
                break;

            default:
                break;
        }

    } else {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
            contactPos.y = this->actor.world.pos.y + this->actor.depthInWater;
            for (angleOffset = 0, i = 0; i < 4; i++, angleOffset += 0x4000) {
                contactPos.x =
                    this->actor.world.pos.x + (Math_SinS((s32)(Rand_ZeroOne() * 7200.0f) + angleOffset) * 15.0f);
                contactPos.z =
                    this->actor.world.pos.z + (Math_CosS((s32)(Rand_ZeroOne() * 7200.0f) + angleOffset) * 15.0f);
                EffectSsGSplash_Spawn(play, &contactPos, NULL, NULL, 0, 190);
            }
            contactPos.x = this->actor.world.pos.x;
            contactPos.z = this->actor.world.pos.z;
            EffectSsGSplash_Spawn(play, &contactPos, NULL, NULL, 0, 280);
            EffectSsGRipple_Spawn(play, &contactPos, 300, 700, 0);
            this->actor.terminalVelocity = -3.0f;
            this->actor.velocity.x *= 0.1f;
            this->actor.velocity.y *= 0.4f;
            this->actor.velocity.z *= 0.1f;
            this->actor.gravity *= 0.5f;
            rotSpeedX >>= 1;
            rotSpeedXtarget >>= 1;
            rotSpeedY >>= 1;
            rotSpeedYtarget >>= 1;
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER_TOUCH;
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EV_DIVE_INTO_WATER_L);
        }
        EnKusa_UpdateVelY(this);
        Math_StepToS(&rotSpeedX, rotSpeedXtarget, 500);
        Math_StepToS(&rotSpeedY, rotSpeedYtarget, 170);
        this->actor.shape.rot.x += rotSpeedX;
        this->actor.shape.rot.y += rotSpeedY;
        EnKusa_RandScaleVecToZero(&this->actor.velocity, 0.05f);
        Actor_UpdatePos(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 7.5f, 35.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40 |
                                    UPDBGCHECKINFO_FLAG_80);
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnKusa_SetupCut(EnKusa* this) {
    switch (KUSA_GET_TYPE(&this->actor)) {
        case ENKUSA_TYPE_GRASS:
        case ENKUSA_TYPE_GRASS_2:
            this->actionFunc = EnKusa_DoNothing;
            break;

        case ENKUSA_TYPE_REGROWING_GRASS:
            this->actionFunc = EnKusa_CutWaitRegrow;
            break;

        default:
            break;
    }
    this->timer = 0;
}

void EnKusa_CutWaitRegrow(EnKusa* this, PlayState* play) {
    this->timer++;
    if (this->timer >= 120) {
        EnKusa_SetupRegrow(this);
    }
}

void EnKusa_DoNothing(EnKusa* this, PlayState* play) {
}

void EnKusa_SetupUprootedWaitRegrow(EnKusa* this) {
    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.y = this->actor.home.pos.y - 9.0f;
    this->actor.world.pos.z = this->actor.home.pos.z;
    EnKusa_SetScaleSmall(this);
    this->timer = 0;
    this->actor.shape.rot = this->actor.home.rot;
    this->actionFunc = EnKusa_UprootedWaitRegrow;
}

void EnKusa_UprootedWaitRegrow(EnKusa* this, PlayState* play) {
    this->timer++;
    if (this->timer > 120) {
        if (Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 0.6f) && (this->timer >= 170)) {
            EnKusa_SetupRegrow(this);
        }
    }
}

void EnKusa_SetupRegrow(EnKusa* this) {
    this->actionFunc = EnKusa_Regrow;
    EnKusa_SetScaleSmall(this);
    this->isCut = false;
    this->actor.shape.rot = this->actor.home.rot;
}

void EnKusa_Regrow(EnKusa* this, PlayState* play) {
    s32 isFullyGrown = 1;

    isFullyGrown &= Math_StepToF(&this->actor.scale.y, 0.4f, 0.014f);
    isFullyGrown &= Math_StepToF(&this->actor.scale.x, 0.4f, 0.011f);
    this->actor.scale.z = this->actor.scale.x;
    if (isFullyGrown) {
        Actor_SetScale(&this->actor, 0.4f);
        EnKusa_SetupInteract(this);
        this->collider.base.ocFlags1 &= ~OC1_TYPE_PLAYER;
    }
}

void EnKusa_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnKusa* this = (EnKusa*)thisx;

    this->actionFunc(this, play);

    if (this->isCut) {
        this->actor.shape.yOffset = -6.25f;
    } else {
        this->actor.shape.yOffset = 0.0f;
    }
    if ((kusaGameplayFrames != play->gameplayFrames) && (play->roomCtx.curRoom.type == ROOM_TYPE_NORMAL)) {
        EnKusa_Sway();
        kusaGameplayFrames = play->gameplayFrames;
    }
}

void EnKusa_DrawBush(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnKusa* this = (EnKusa*)thisx;

    Ship_ExtendedCullingActorAdjustProjectedZ(&this->actor);

    if ((this->actor.projectedPos.z <= 1200.0f) || ((this->isInWater & 1) && (this->actor.projectedPos.z < 1300.0f))) {

        if ((play->roomCtx.curRoom.type == ROOM_TYPE_NORMAL) && (this->actionFunc == EnKusa_WaitForInteract) &&
            (this->actor.projectedPos.z > -150.0f) && (this->actor.projectedPos.z < 400.0f)) {
            EnKusa_ApplySway(&D_80936AD8[this->kusaMtxIdx]);
        }

        Gfx_DrawDListOpa(play, gKusaBushType1DL);

    } else if (this->actor.projectedPos.z < 1300.0f) {
        s32 alpha;

        OPEN_DISPS(play->state.gfxCtx);

        alpha = (1300.0f - this->actor.projectedPos.z) * 2.55f;
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, alpha);
        gSPDisplayList(POLY_XLU_DISP++, gKusaBushType2DL);

        CLOSE_DISPS(play->state.gfxCtx);
    }

    Ship_ExtendedCullingActorRestoreProjectedPos(play, &this->actor);
}

void EnKusa_DrawGrass(Actor* thisx, PlayState* play) {
    EnKusa* this = (EnKusa*)thisx;

    if (this->isCut) {
        Gfx_DrawDListOpa(play, gKusaStumpDL);
    } else {
        if ((play->roomCtx.curRoom.type == ROOM_TYPE_NORMAL) && (this->actionFunc == EnKusa_WaitForInteract)) {
            if ((this->actor.projectedPos.z > -150.0f) && (this->actor.projectedPos.z < 400.0f)) {
                EnKusa_ApplySway(&D_80936AD8[this->kusaMtxIdx]);
            }
        }
        Gfx_DrawDListOpa(play, gKusaSproutDL);
    }
}
