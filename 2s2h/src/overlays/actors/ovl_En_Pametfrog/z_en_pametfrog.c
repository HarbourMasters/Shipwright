/*
 * File: z_en_pametfrog.c
 * Overlay: ovl_En_Pametfrog
 * Description: Gekko & Snapper Miniboss: Gekko
 */

#include "z_en_pametfrog.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "overlays/actors/ovl_En_Bigpamet/z_en_bigpamet.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnPametfrog_Init(Actor* thisx, PlayState* play);
void EnPametfrog_Destroy(Actor* thisx, PlayState* play);
void EnPametfrog_Update(Actor* thisx, PlayState* play);
void EnPametfrog_Draw(Actor* thisx, PlayState* play);

void EnPametfrog_JumpWaterEffects(EnPametfrog* this, PlayState* play);
void EnPametfrog_RearOnSnapper(EnPametfrog* this, PlayState* play);
void EnPametfrog_RearOnSnapperWave(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupRearOnSnapperWave(EnPametfrog* this);
void EnPametfrog_RearOnSnapperWave(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupRearOnSnapperRise(EnPametfrog* this);
void EnPametfrog_RearOnSnapperRise(EnPametfrog* this, PlayState* play);
void EnPametfrog_FallOffSnapper(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupJumpToWall(EnPametfrog* this);
void EnPametfrog_JumpToWall(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupWallCrawl(EnPametfrog* this);
void EnPametfrog_WallCrawl(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupWallPause(EnPametfrog* this);
void EnPametfrog_WallPause(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupClimbDownWall(EnPametfrog* this);
void EnPametfrog_ClimbDownWall(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupRunToSnapper(EnPametfrog* this);
void EnPametfrog_RunToSnapper(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupJumpOnSnapper(EnPametfrog* this);
void EnPametfrog_JumpOnSnapper(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupLandOnSnapper(EnPametfrog* this);
void EnPametfrog_LandOnSnapper(EnPametfrog* this, PlayState* play);
void EnPametfrog_FallInAir(EnPametfrog* this, PlayState* play);
void EnPametfrog_FallOnGround(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupFallOnGround(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupDefeatGekko(EnPametfrog* this, PlayState* play);
void EnPametfrog_DefeatGekko(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupDefeatSnapper(EnPametfrog* this, PlayState* play);
void EnPametfrog_DefeatSnapper(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupSpawnFrog(EnPametfrog* this, PlayState* play);
void EnPametfrog_SpawnFrog(EnPametfrog* this, PlayState* play);
void EnPametfrog_PlayCutscene(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupLookAround(EnPametfrog* this);
void EnPametfrog_LookAround(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupJumpToLink(EnPametfrog* this);
void EnPametfrog_JumpToLink(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupMeleeAttack(EnPametfrog* this);
void EnPametfrog_MeleeAttack(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupCutscene(EnPametfrog* this);
void EnPametfrog_Damage(EnPametfrog* this, PlayState* play);
void EnPametfrog_Stun(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupCallSnapper(EnPametfrog* this, PlayState* play);
void EnPametfrog_CallSnapper(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupSnapperSpawn(EnPametfrog* this, PlayState* play);
void EnPametfrog_SnapperSpawn(EnPametfrog* this, PlayState* play);
void EnPametfrog_SetupTransitionGekkoSnapper(EnPametfrog* this, PlayState* play);
void EnPametfrog_TransitionGekkoSnapper(EnPametfrog* this, PlayState* play);

ActorProfile En_Pametfrog_Profile = {
    /**/ ACTOR_EN_PAMETFROG,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_BIGSLIME,
    /**/ sizeof(EnPametfrog),
    /**/ EnPametfrog_Init,
    /**/ EnPametfrog_Destroy,
    /**/ EnPametfrog_Update,
    /**/ EnPametfrog_Draw,
};

typedef enum {
    /* 0 */ GEKKO_DMGEFF_NONE,
    /* 1 */ GEKKO_DMGEFF_STUN,
    /* 2 */ GEKKO_DMGEFF_FIRE,
    /* 3 */ GEKKO_DMGEFF_ICE,
    /* 4 */ GEKKO_DMGEFF_LIGHT,
    /* 5 */ GEKKO_DMGEFF_ZORA_BARRIER
} EnPametfrogDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, GEKKO_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, GEKKO_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, GEKKO_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, GEKKO_DMGEFF_STUN),
    /* Goron punch    */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(2, GEKKO_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, GEKKO_DMGEFF_ICE),
    /* Light arrow    */ DMG_ENTRY(2, GEKKO_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(0, GEKKO_DMGEFF_STUN),
    /* Deku bubble    */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, GEKKO_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, GEKKO_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, GEKKO_DMGEFF_ZORA_BARRIER),
    /* Normal shield  */ DMG_ENTRY(0, GEKKO_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, GEKKO_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, GEKKO_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, GEKKO_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, GEKKO_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, GEKKO_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, GEKKO_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, GEKKO_DMGEFF_NONE),
};

static ColliderJntSphElementInit sJntSphElementsInit[2] = {
    {
        {
            ELEM_MATERIAL_UNK1,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 32 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_WOOD,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 1, { { 0, 0, 0 }, 25 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT6,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInit = { 3, 30, 60, 50 };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_GEKKO_SNAPPER, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(lockOnArrowOffset, -13221, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 7, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_10, ICHAIN_STOP),
};

static s32 sIsFrogReturnedFlags[] = {
    WEEKEVENTREG_32_40,
    WEEKEVENTREG_32_80,
    WEEKEVENTREG_33_01,
    WEEKEVENTREG_33_02,
};

void EnPametfrog_Init(Actor* thisx, PlayState* play) {
    EnPametfrog* this = (EnPametfrog*)thisx;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 55.0f);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInit);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGekkoSkel, &gGekkoBoxingStanceAnim, this->jointTable, this->morphTable,
                       GEKKO_LIMB_MAX);
    Collider_InitAndSetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colElement);
    this->params = CLAMP(this->actor.params, 1, 4);
    if (Flags_GetClear(play, play->roomCtx.curRoom.num)) {
        Actor_Kill(&this->actor);
        if (!CHECK_WEEKEVENTREG(sIsFrogReturnedFlags[this->actor.params - 1])) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_MINIFROG, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, this->params);
        }
    } else {
        for (i = 0; i < 2; i++) {
            this->collider.elements[i].dim.worldSphere.radius = this->collider.elements[i].dim.modelSphere.radius;
        }

        if (Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_BIGPAMET, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0) == NULL) {
            Actor_Kill(&this->actor);
            return;
        }

        this->actor.params = GEKKO_PRE_SNAPPER;
        EnPametfrog_SetupLookAround(this);
    }
}

void EnPametfrog_Destroy(Actor* thisx, PlayState* play) {
    EnPametfrog* this = (EnPametfrog*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

u8 EnPametfrog_Vec3fNormalize(Vec3f* vec) {
    f32 magnitude = Math3D_Vec3fMagnitude(vec);

    if (magnitude < 0.0001f) {
        return false;
    } else {
        Math_Vec3f_Scale(vec, 1.0f / magnitude);
        return true;
    }
}

void EnPametfrog_Freeze(EnPametfrog* this) {
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->collider.base.colMaterial = COL_MATERIAL_HIT3;
    this->collider.elements[0].base.elemMaterial = ELEM_MATERIAL_UNK0;
    this->drawDmgEffScale = 0.75f;
    this->drawDmgEffFrozenSteamScale = 1.125f;
    this->drawDmgEffAlpha = 1.0f;
}

void EnPametfrog_Thaw(EnPametfrog* this, PlayState* play) {
    this->freezeTimer = 0;
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->collider.base.colMaterial = COL_MATERIAL_HIT6;
        this->collider.elements[0].base.elemMaterial = ELEM_MATERIAL_UNK1;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, GEKKO_BODYPART_MAX, 2, 0.3f, 0.2f);
    }
}

void EnPametfrog_JumpWaterEffects(EnPametfrog* this, PlayState* play) {
    Vec3f pos;

    if (this->actor.depthInWater > 0.0f) {
        pos.x = this->actor.world.pos.x;
        pos.z = this->actor.world.pos.z;
        pos.y = this->actor.world.pos.y + this->actor.depthInWater;
        EffectSsGRipple_Spawn(play, &pos, 150, 550, 0);
        pos.y += 8.0f;
        EffectSsGSplash_Spawn(play, &pos, NULL, NULL, 0, 550);
    }
}

void EnPametfrog_IdleWaterEffects(EnPametfrog* this, PlayState* play) {
    Vec3f pos;

    if ((this->actor.depthInWater > 0.0f) && ((play->gameplayFrames % 14) == 0)) {
        pos.x = this->actor.world.pos.x;
        pos.z = this->actor.world.pos.z;
        pos.y = this->actor.world.pos.y + this->actor.depthInWater;
        EffectSsGRipple_Spawn(play, &pos, 150, 550, 0);
    }
}

void func_8086A238(EnPametfrog* this) {
    MtxF unkMtx;

    unkMtx.xx = this->unk_2E8.x;
    unkMtx.yx = this->unk_2E8.y;
    unkMtx.zx = this->unk_2E8.z;
    unkMtx.xy = this->unk_2DC.x;
    unkMtx.yy = this->unk_2DC.y;
    unkMtx.zy = this->unk_2DC.z;
    unkMtx.xz = this->unk_2D0.x;
    unkMtx.yz = this->unk_2D0.y;
    unkMtx.zz = this->unk_2D0.z;

    Matrix_MtxFToYXZRot(&unkMtx, &this->actor.shape.rot, false);
    this->actor.world.rot.x = -this->actor.shape.rot.x;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.world.rot.z = this->actor.shape.rot.z;
}

s32 func_8086A2CC(EnPametfrog* this, CollisionPoly* floorPoly) {
    Vec3f floorNorm;
    Vec3f vec2;
    f32 rotation;
    f32 arg0;

    this->actor.floorPoly = floorPoly;
    floorNorm.x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
    floorNorm.y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
    floorNorm.z = COLPOLY_GET_NORMAL(floorPoly->normal.z);
    arg0 = DOTXYZ(floorNorm, this->unk_2DC);
    if (fabsf(arg0) >= 1.0f) {
        return false;
    }

    rotation = Math_FAcosF(arg0);
    if (rotation < 0.0001f) {
        return false;
    }

    Math3D_Vec3f_Cross(&this->unk_2DC, &floorNorm, &vec2);
    EnPametfrog_Vec3fNormalize(&vec2);
    Matrix_RotateAxisF(rotation, &vec2, MTXMODE_NEW);
    Matrix_MultVec3f(&this->unk_2E8, &vec2);
    Math_Vec3f_Copy(&this->unk_2E8, &vec2);
    Math3D_Vec3f_Cross(&this->unk_2E8, &floorNorm, &this->unk_2D0);
    EnPametfrog_Vec3fNormalize(&this->unk_2D0);
    Math_Vec3f_Copy(&this->unk_2DC, &floorNorm);
    return true;
}

void EnPametfrog_ShakeCamera(EnPametfrog* this, PlayState* play, f32 magShakeXZ, f32 magShakeY) {
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    s16 subCamYaw;
    Vec3f subCamEye;

    subCamYaw = BINANG_ROT180(Camera_GetCamDirYaw(subCam));
    subCamEye.x = (Math_SinS(subCamYaw) * magShakeXZ) + subCam->at.x;
    subCamEye.y = subCam->at.y + magShakeY;
    subCamEye.z = (Math_CosS(subCamYaw) * magShakeXZ) + subCam->at.z;
    Play_SetCameraAtEye(play, this->subCamId, &subCam->at, &subCamEye);
}

void EnPametfrog_StopCutscene(EnPametfrog* this, PlayState* play) {
    Camera* subCam;

    if (this->subCamId != SUB_CAM_ID_DONE) {
        subCam = Play_GetCamera(play, this->subCamId);
        Play_SetCameraAtEye(play, CAM_ID_MAIN, &subCam->at, &subCam->eye);
        this->subCamId = SUB_CAM_ID_DONE;
        CutsceneManager_Stop(this->csId);
        Player_SetCsAction(play, &this->actor, PLAYER_CSACTION_END);
    }
}

void EnPametfrog_PlaceSnapper(EnPametfrog* this, PlayState* play) {
    CollisionPoly* poly;
    s32 bgId;
    Vec3f vec1;
    Vec3f vec2;
    Vec3f vec3;
    Actor* child;

    poly = NULL;
    this->actor.child->world.pos.x = (Math_SinS(this->actor.shape.rot.y) * 300.0f) + this->actor.world.pos.x;
    this->actor.child->world.pos.z = (Math_CosS(this->actor.shape.rot.y) * 300.0f) + this->actor.world.pos.z;
    this->actor.child->shape.rot.y = BINANG_ROT180(this->actor.shape.rot.y);
    vec2.x = this->actor.child->world.pos.x;
    vec2.y = this->actor.child->world.pos.y + 50.0f;
    vec2.z = this->actor.child->world.pos.z;
    vec3.x = this->actor.child->world.pos.x;
    vec3.y = this->actor.child->world.pos.y - 150.0f;
    vec3.z = this->actor.child->world.pos.z;
    if (BgCheck_EntityLineTest1(&play->colCtx, &vec2, &vec3, &vec1, &poly, false, true, false, true, &bgId)) {
        this->actor.child->floorHeight = vec1.y;
    } else {
        this->actor.child->floorHeight = this->actor.home.pos.y;
    }

    child = this->actor.child;
    child->world.pos.y = child->floorHeight - 60.0f;
}

void EnPametfrog_JumpOnGround(EnPametfrog* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_JUMP);
    } else if (Animation_OnFrame(&this->skelAnime, 11.0f)) {
        EnPametfrog_JumpWaterEffects(this, play);
        Actor_PlaySfx(&this->actor, NA_SE_EV_WALK_WATER);
    }
}

void EnPametfrog_ApplyMagicArrowEffects(EnPametfrog* this, PlayState* play) {
    if (this->actor.colChkInfo.damageEffect == GEKKO_DMGEFF_FIRE) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffAlpha = 3.0f;
        this->drawDmgEffScale = 0.75f;
    } else if (this->actor.colChkInfo.damageEffect == GEKKO_DMGEFF_LIGHT) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
        this->drawDmgEffScale = 0.75f;
        this->drawDmgEffAlpha = 3.0f;
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->collider.elements[0].base.acDmgInfo.hitPos.x,
                    this->collider.elements[0].base.acDmgInfo.hitPos.y,
                    this->collider.elements[0].base.acDmgInfo.hitPos.z, 0, 0, 0,
                    CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
    } else if (this->actor.colChkInfo.damageEffect == GEKKO_DMGEFF_ICE) {
        EnPametfrog_Freeze(this);
    }
}

void EnPametfrog_ApplyElectricStun(EnPametfrog* this) {
    this->freezeTimer = 40;
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
    this->drawDmgEffScale = 0.75f;
    this->drawDmgEffAlpha = 2.0f;
}

void EnPametfrog_ApplyStun(EnPametfrog* this) {
    this->freezeTimer = 40;
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
}

void EnPametfrog_SetupRearOnSnapper(EnPametfrog* this) {
    if (this->actionFunc == EnPametfrog_RearOnSnapperRise) {
        Animation_PlayOnce(&this->skelAnime, &gGekkoWiggleAnim);
    } else if (this->actionFunc == EnPametfrog_RearOnSnapperWave) {
        Animation_PlayOnce(&this->skelAnime, &gGekkoLandOnSnapperAnim);
    } else {
        Animation_PlayOnce(&this->skelAnime, &gGekkoStandingIdleAnim);
    }

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.params = GEKKO_ON_SNAPPER;
    this->actionFunc = EnPametfrog_RearOnSnapper;
}

void EnPametfrog_RearOnSnapper(EnPametfrog* this, PlayState* play) {
    Actor* actor;
    Vec3f rearingPoint;
    s32 pad;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (Rand_ZeroOne() < 0.5f) {
            Animation_PlayOnce(&this->skelAnime, &gGekkoQuickFistPumpAnim);
        } else {
            Animation_PlayOnce(&this->skelAnime, &gGekkoStandingIdleAnim);
        }
    }

    actor = func_800BC270(play, &this->actor, 60.0f, 0x138B0);
    if (actor != NULL) {
        rearingPoint.x = this->actor.world.pos.x;
        rearingPoint.y = this->actor.world.pos.y + 10.0f;
        rearingPoint.z = this->actor.world.pos.z;
        if (actor->world.rot.x < Actor_WorldPitchTowardPoint(actor, &rearingPoint)) {
            EnPametfrog_SetupRearOnSnapperWave(this);
        } else {
            EnPametfrog_SetupRearOnSnapperRise(this);
        }
    }
}

void EnPametfrog_SetupRearOnSnapperWave(EnPametfrog* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoJumpOnSnapperAnim);
    this->timer = 15;
    this->actionFunc = EnPametfrog_RearOnSnapperWave;
}

void EnPametfrog_RearOnSnapperWave(EnPametfrog* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) && (this->timer > 0)) {
        this->timer--;
        if (this->timer == 0) {
            EnPametfrog_SetupRearOnSnapper(this);
        }
    }
}

void EnPametfrog_SetupRearOnSnapperRise(EnPametfrog* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoSqueezeAnim);
    this->timer = 10;
    this->actor.params = GEKKO_REAR_ON_SNAPPER;
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    this->actor.shape.rot.y = this->actor.child->world.rot.y;
    this->actionFunc = EnPametfrog_RearOnSnapperRise;
}

void EnPametfrog_RearOnSnapperRise(EnPametfrog* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->timer--;
    if (this->timer == 0) {
        EnPametfrog_SetupRearOnSnapper(this);
    } else {
        this->actor.world.pos.y =
            Math_SinS(this->timer * 0xCCC) * 100.0f + (((EnBigpamet*)this->actor.child)->unk_2AC + 46.0f);
    }
}

void EnPametfrog_SetupFallOffSnapper(EnPametfrog* this, PlayState* play) {
    Vec3f subCamEye;
    s16 yaw;

    Animation_PlayOnce(&this->skelAnime, &gGekkoFallInAirAnim);
    this->actor.params = GEKKO_FALL_OFF_SNAPPER;
    this->actor.speed = 7.0f;
    this->actor.velocity.y = 15.0f;
    this->actor.world.rot.y = BINANG_ROT180(this->actor.child->world.rot.y);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->timer = 30;
    this->collider.base.ocFlags1 |= OC1_ON;
    yaw = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    subCamEye.x = (Math_SinS(yaw) * 300.0f) + this->actor.focus.pos.x;
    subCamEye.y = this->actor.focus.pos.y + 100.0f;
    subCamEye.z = (Math_CosS(yaw) * 300.0f) + this->actor.focus.pos.z;
    Play_SetCameraAtEye(play, this->subCamId, &this->actor.focus.pos, &subCamEye);
    Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_DAMAGE);
    this->actionFunc = EnPametfrog_FallOffSnapper;
}

void EnPametfrog_FallOffSnapper(EnPametfrog* this, PlayState* play) {
    f32 sin;

    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.x += 0x800;
    this->actor.shape.rot.z += 0x1000;
    if (this->timer != 0) {
        this->timer--;
    }

    sin = Math_SinF(this->timer * (M_PIf / 3)) * ((0.02f * (this->timer * (1.0f / 6.0f))) + 0.005f) + 1.0f;
    EnPametfrog_ShakeCamera(this, play, 300.0f * sin, 100.0f * sin);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        EnPametfrog_StopCutscene(this, play);
        EnPametfrog_SetupJumpToWall(this);
    }
}

void EnPametfrog_SetupJumpToWall(EnPametfrog* this) {
    Animation_Change(&this->skelAnime, &gGekkoJumpForwardAnim, 2.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -2.0f);
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
    Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_VOICE2);
    this->actionFunc = EnPametfrog_JumpToWall;
}

void EnPametfrog_JumpToWall(EnPametfrog* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    EnPametfrog_JumpOnGround(this, play);
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
        (this->actor.wallBgId == BGCHECK_SCENE) && (COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.y) < 0.5f)) {
        EnPametfrog_SetupWallCrawl(this);
    } else if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
               ((this->skelAnime.curFrame > 1.0f) && (this->skelAnime.curFrame < 12.0f))) {
        this->actor.speed = 12.0f;
    } else {
        this->actor.speed = 0.0f;
    }
}

void EnPametfrog_SetupWallCrawl(EnPametfrog* this) {
    if (this->actionFunc == EnPametfrog_JumpToWall) {
        Animation_PlayLoop(&this->skelAnime, &gGekkoCrawlAnim);
        this->collider.base.acFlags |= AC_ON;
        this->unk_2D0.x = 0.0f;
        this->unk_2D0.z = 0.0f;
        this->actor.gravity = 0.0f;
        this->actor.world.pos.y = this->actor.focus.pos.y;
        this->unk_2D0.y = 1.0f;
        Math_Vec3f_Copy(&this->actor.colChkInfo.displacement, &gZeroVec3f);
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->unk_2DC.x = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.x);
        this->unk_2DC.y = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.y);
        this->unk_2DC.z = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.z);
        Math3D_Vec3f_Cross(&this->unk_2DC, &this->unk_2D0, &this->unk_2E8);
        EnPametfrog_Vec3fNormalize(&this->unk_2E8);
        Math3D_Vec3f_Cross(&this->unk_2E8, &this->unk_2DC, &this->unk_2D0);
        EnPametfrog_Vec3fNormalize(&this->unk_2D0);
        func_8086A238(this);
        this->actor.floorPoly = this->actor.wallPoly;
        this->wallPauseTimer = 10;
    } else {
        this->skelAnime.playSpeed = 1.0f;
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_RUNAWAY);
    this->actor.speed = 8.0f;
    this->timer = Rand_S16Offset(35, 15);
    this->actionFunc = EnPametfrog_WallCrawl;
}

void EnPametfrog_WallCrawl(EnPametfrog* this, PlayState* play) {
    CollisionPoly* poly1 = NULL;
    CollisionPoly* poly2 = NULL;
    Vec3f vec1;
    Vec3f vec2;
    Vec3f worldPos1;
    Vec3f worldPos2;
    f32 doubleSpeedXZ;
    s32 bgId1;
    s32 bgId2;
    s32 isSuccess = false;

    if (this->freezeTimer > 0) {
        this->freezeTimer--;
    } else {
        SkelAnime_Update(&this->skelAnime);
        this->timer--;
        this->actor.speed = 8.0f;
        doubleSpeedXZ = this->actor.speed * 2.0f;
        vec1.x = this->actor.world.pos.x + this->unk_2DC.x * 2.0f;
        vec1.y = this->actor.world.pos.y + this->unk_2DC.y * 2.0f;
        vec1.z = this->actor.world.pos.z + this->unk_2DC.z * 2.0f;
        vec2.x = this->actor.world.pos.x - this->unk_2DC.x * 25.0f;
        vec2.y = this->actor.world.pos.y - this->unk_2DC.y * 25.0f;
        vec2.z = this->actor.world.pos.z - this->unk_2DC.z * 25.0f;
        if (BgCheck_EntityLineTest1(&play->colCtx, &vec1, &vec2, &worldPos2, &poly2, true, true, true, true, &bgId2)) {
            vec2.x = this->unk_2D0.x * doubleSpeedXZ + vec1.x;
            vec2.y = this->unk_2D0.y * doubleSpeedXZ + vec1.y;
            vec2.z = this->unk_2D0.z * doubleSpeedXZ + vec1.z;
            if (BgCheck_EntityLineTest1(&play->colCtx, &vec1, &vec2, &worldPos1, &poly1, true, true, true, true,
                                        &bgId1)) {
                isSuccess = func_8086A2CC(this, poly1);
                Math_Vec3f_Copy(&this->actor.world.pos, &worldPos1);
                this->actor.floorBgId = bgId1;
                this->actor.speed = 0.0f;
            } else {
                if (this->actor.floorPoly != poly2) {
                    isSuccess = func_8086A2CC(this, poly2);
                }
                Math_Vec3f_Copy(&this->actor.world.pos, &worldPos2);
                this->actor.floorBgId = bgId2;
            }
        } else {
            EnPametfrog_SetupClimbDownWall(this);
        }

        if (isSuccess) {
            func_8086A238(this);
        }

        if (Animation_OnFrame(&this->skelAnime, 15.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_BOMCHU_WALK);
        }

        if (((play->gameplayFrames % 60) == 0) && (Rand_ZeroOne() < 0.8f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_REAL);
        }

        if ((this->timer == 0) ||
            ((this->actor.world.pos.y < (this->actor.home.pos.y + 200.0f)) && (this->unk_2D0.y <= 0.0f))) {
            this->wallPauseTimer--;
            if (this->wallPauseTimer == 0) {
                EnPametfrog_SetupClimbDownWall(this);
            } else {
                EnPametfrog_SetupWallPause(this);
            }
        }
    }
}

void EnPametfrog_SetupWallPause(EnPametfrog* this) {
    s32 pad;
    f32 randFloat;

    this->actor.speed = 0.0f;
    this->skelAnime.playSpeed = 1.5f;
    if (this->timer != 0) {
        this->wallRotation = this->unk_2E8.y > 0.0f ? (M_PIf / 30) : (-M_PIf / 30);
    } else {
        randFloat = Rand_ZeroFloat(0x2000);
        this->wallRotation = ((Rand_ZeroOne() < 0.5f) ? -1 : 1) * (0x1000 + randFloat) * (M_PIf / (15 * 0x8000));
    }
    this->timer = 15;
    Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_RUNAWAY2);
    this->actionFunc = EnPametfrog_WallPause;
}

void EnPametfrog_WallPause(EnPametfrog* this, PlayState* play) {
    Vec3f vec;

    if (this->freezeTimer > 0) {
        this->freezeTimer--;
    } else {
        SkelAnime_Update(&this->skelAnime);
        this->timer--;
        Matrix_RotateAxisF(this->wallRotation, &this->unk_2DC, MTXMODE_NEW);
        Matrix_MultVec3f(&this->unk_2D0, &vec);
        Math_Vec3f_Copy(&this->unk_2D0, &vec);
        Math3D_Vec3f_Cross(&this->unk_2DC, &this->unk_2D0, &this->unk_2E8);
        func_8086A238(this);
        if (((play->gameplayFrames % 60) == 0) && (Rand_ZeroOne() < 0.8f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_REAL);
        }

        if (this->timer == 0) {
            EnPametfrog_SetupWallCrawl(this);
        }
    }
}

void EnPametfrog_SetupClimbDownWall(EnPametfrog* this) {
    s16 yaw;

    Animation_Change(&this->skelAnime, &gGekkoJumpForwardAnim, 0.0f, 0.0f,
                     Animation_GetLastFrame(&gGekkoJumpForwardAnim), ANIMMODE_ONCE, 0.0f);
    this->actor.shape.rot.y = Actor_WorldYawTowardActor(&this->actor, this->actor.child);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    this->actor.colChkInfo.mass = 50;
    this->actor.speed = 5.0f;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -1.0f;
    yaw = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    this->actor.world.pos.x += 30.0f * Math_SinS(yaw);
    this->actor.world.pos.z += 30.0f * Math_CosS(yaw);
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->actor.params = GEKKO_RETURN_TO_SNAPPER;
    this->actionFunc = EnPametfrog_ClimbDownWall;
}

void EnPametfrog_ClimbDownWall(EnPametfrog* this, PlayState* play) {
    s16 yaw;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        EnPametfrog_SetupRunToSnapper(this);
    } else if (this->actor.floorHeight == BGCHECK_Y_MIN) {
        yaw = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
        this->actor.world.pos.x += 5.0f * Math_SinS(yaw);
        this->actor.world.pos.z += 5.0f * Math_CosS(yaw);
        if (this->actor.world.pos.y < (this->actor.home.pos.y - 5.0f)) {
            this->actor.world.pos.y = this->actor.home.pos.y;
        }
    }
}

void EnPametfrog_SetupRunToSnapper(EnPametfrog* this) {
    Animation_Change(&this->skelAnime, &gGekkoJumpForwardAnim, 2.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -2.0f);
    this->actor.params = GEKKO_RETURN_TO_SNAPPER;
    this->actionFunc = EnPametfrog_RunToSnapper;
}

void EnPametfrog_RunToSnapper(EnPametfrog* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    EnPametfrog_JumpOnGround(this, play);
    this->actor.shape.rot.y = Actor_WorldYawTowardActor(&this->actor, this->actor.child);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
        ((this->skelAnime.curFrame > 1.0f) && (this->skelAnime.curFrame < 12.0f))) {
        this->actor.speed = 12.0f;
    } else {
        this->actor.speed = 0.0f;
    }

    if ((this->actor.child->params == 1) && (Actor_WorldDistXZToActor(&this->actor, this->actor.child) < 120.0f) &&
        Animation_OnFrame(&this->skelAnime, 0.0f)) {
        EnPametfrog_SetupJumpOnSnapper(this);
    }
}

void EnPametfrog_SetupJumpOnSnapper(EnPametfrog* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGekkoStandingIdleAnim, 6.0f);
    this->timer = 6;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.shape.rot.y = Actor_WorldYawTowardActor(&this->actor, this->actor.child);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.params = GEKKO_JUMP_ON_SNAPPER;
    Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_GREET);
    this->actionFunc = EnPametfrog_JumpOnSnapper;
}

void EnPametfrog_JumpOnSnapper(EnPametfrog* this, PlayState* play) {
    f32 lerp;
    EnBigpamet* bigpamet;

    SkelAnime_Update(&this->skelAnime);
    this->timer--;
    if (this->timer == 0) {
        EnPametfrog_SetupLandOnSnapper(this);
    } else {
        bigpamet = (EnBigpamet*)this->actor.child;
        lerp = 1.0f / this->timer;
        this->actor.world.pos.x -= (this->actor.world.pos.x - bigpamet->actor.world.pos.x) * lerp;
        this->actor.world.pos.z -= (this->actor.world.pos.z - bigpamet->actor.world.pos.z) * lerp;
        this->actor.world.pos.y =
            Math_SinS((-this->timer * 0x1000) + 0x6000) * 65.0f + ((EnBigpamet*)this->actor.child)->unk_2AC;
    }
}

void EnPametfrog_SetupLandOnSnapper(EnPametfrog* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoFullFistPumpAnim);
    this->actor.shape.rot.y = this->actor.child->shape.rot.y;
    this->actor.params = GEKKO_ON_SNAPPER;
    this->actionFunc = EnPametfrog_LandOnSnapper;
}

void EnPametfrog_LandOnSnapper(EnPametfrog* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        EnPametfrog_StopCutscene(this, play);
        EnPametfrog_SetupRearOnSnapper(this);
    }
}

void EnPametfrog_SetupFallInAir(EnPametfrog* this, PlayState* play) {
    s16 yaw;
    Vec3f subCamEye;
    f32 xzDist;

    Animation_PlayOnce(&this->skelAnime, &gGekkoFallInAirAnim);
    if (this->actor.colChkInfo.health > 0) {
        this->actor.params = GEKKO_RETURN_TO_SNAPPER;
    }

    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->collider.base.acFlags &= ~AC_ON;
    this->timer = 10;
    if (this->actor.colChkInfo.health == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_DEAD);
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_DAMAGE);
    }

    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 16);
    yaw = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    this->actor.world.pos.x += 30.0f * Math_SinS(yaw);
    this->actor.world.pos.z += 30.0f * Math_CosS(yaw);
    if (this->subCamId != SUB_CAM_ID_DONE) {
        xzDist = sqrtf(SQXZ(this->unk_2DC));
        if (xzDist > 0.001f) {
            xzDist = 200.0f / xzDist;
        } else {
            xzDist = 200.0f;
            this->unk_2DC.x = 1.0f;
            this->unk_2DC.z = 0.0f;
        }

        subCamEye.x = this->actor.world.pos.x + (xzDist * this->unk_2DC.x);
        subCamEye.y = (this->actor.world.pos.y + this->actor.home.pos.y) * 0.5f;
        subCamEye.z = this->actor.world.pos.z + (xzDist * this->unk_2DC.z);
        Play_SetCameraAtEye(play, this->subCamId, &this->actor.world.pos, &subCamEye);
    }

    this->actionFunc = EnPametfrog_FallInAir;
}

void EnPametfrog_FallInAir(EnPametfrog* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->actor.colorFilterTimer = 16;
    if (this->timer > 0) {
        this->timer--;
        if (this->timer == 0) {
            this->actor.gravity = -1.0f;
            this->actor.colChkInfo.mass = 50;
        }
    } else {
        this->spinYaw += 0xF00;
        if (this->subCamId != SUB_CAM_ID_DONE) {
            Play_SetCameraAtEye(play, this->subCamId, &this->actor.world.pos,
                                &Play_GetCamera(play, this->subCamId)->eye);
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            EnPametfrog_SetupFallOnGround(this, play);
        }
    }
}

void EnPametfrog_SetupFallOnGround(EnPametfrog* this, PlayState* play) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoFallOnGroundAnim);
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.y += this->spinYaw;
    this->actor.shape.rot.z = 0;
    this->spinYaw = 0;
    this->timer = 5;
    EnPametfrog_Thaw(this, play);
    EnPametfrog_JumpWaterEffects(this, play);
    Actor_PlaySfx(&this->actor, NA_SE_EV_WALK_WATER);
    this->actionFunc = EnPametfrog_FallOnGround;
}

void EnPametfrog_FallOnGround(EnPametfrog* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->skelAnime.animation == &gGekkoFallOnGroundAnim) {
            if (this->actor.colChkInfo.health == 0) {
                this->timer--;
                if (this->timer == 0) {
                    EnPametfrog_SetupDefeatGekko(this, play);
                }
            } else {
                Animation_PlayOnce(&this->skelAnime, &gGekkoRecoverAnim);
            }
        } else {
            EnPametfrog_SetupRunToSnapper(this);
        }
    }
}

void EnPametfrog_SetupDefeatGekko(EnPametfrog* this, PlayState* play) {
    Vec3f subCamEye;
    s16 yaw = Actor_WorldYawTowardPoint(this->actor.child, &this->actor.home.pos);
    s16 yawDiff = this->actor.yawTowardsPlayer - yaw;

    yaw = (yawDiff > 0) ? (yaw - 0x2000) : (yaw + 0x2000);
    subCamEye.x = this->actor.child->focus.pos.x + 150.0f * Math_SinS(yaw);
    subCamEye.y = this->actor.child->focus.pos.y + 20.0f;
    subCamEye.z = this->actor.child->focus.pos.z + 150.0f * Math_CosS(yaw);
    Play_SetCameraAtEye(play, this->subCamId, &this->actor.child->focus.pos, &subCamEye);
    this->actor.params = GEKKO_DEFEAT;
    this->timer = 38;
    this->actionFunc = EnPametfrog_DefeatGekko;
}

void EnPametfrog_DefeatGekko(EnPametfrog* this, PlayState* play) {
    this->actor.colorFilterTimer = 16;
    if (this->timer > 0) {
        this->timer--;
        if (this->timer == 0) {
            EnPametfrog_SetupDefeatSnapper(this, play);
        }
    }
}

void EnPametfrog_SetupDefeatSnapper(EnPametfrog* this, PlayState* play) {
    Vec3f subCamEye;
    s16 yaw = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    s16 yawDiff = this->actor.yawTowardsPlayer - yaw;

    yaw = (yawDiff > 0) ? (yaw - 0x2000) : (yaw + 0x2000);
    subCamEye.x = this->actor.world.pos.x + Math_SinS(yaw) * 150.0f;
    subCamEye.y = this->actor.world.pos.y + 20.0f;
    subCamEye.z = this->actor.world.pos.z + Math_CosS(yaw) * 150.0f;
    Play_SetCameraAtEye(play, this->subCamId, &this->actor.world.pos, &subCamEye);
    this->timer = 20;
    this->actionFunc = EnPametfrog_DefeatSnapper;
}

void EnPametfrog_DefeatSnapper(EnPametfrog* this, PlayState* play) {
    this->timer--;
    Actor_SetScale(&this->actor, this->timer * 0.00035000002f);
    this->actor.colorFilterTimer = 16;
    EnPametfrog_ShakeCamera(this, play, (this->timer * 3.75f) + 75.0f, (this->timer * 0.5f) + 10.0f);
    if (this->timer == 0) {
        EnPametfrog_SetupSpawnFrog(this, play);
    }
}

void EnPametfrog_SetupSpawnFrog(EnPametfrog* this, PlayState* play) {
    static Vec3f sAccel = { 0.0f, -0.5f, 0.0f };
    static Color_RGBA8 sPrimColor = { 250, 250, 250, 255 };
    static Color_RGBA8 sEnvColor = { 180, 180, 180, 255 };
    s16 yaw = BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)));
    Vec3f vec1;
    Vec3f velocity;
    s32 i;

    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_MINIFROG, this->actor.world.pos.x, this->actor.world.pos.y,
                this->actor.world.pos.z, 0, yaw, 0, this->params);
    vec1.x = (Math_SinS(yaw) * 20.0f) + this->actor.world.pos.x;
    vec1.y = this->actor.world.pos.y + 25.0f;
    vec1.z = (Math_CosS(yaw) * 20.0f) + this->actor.world.pos.z;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    func_800B0DE0(play, &vec1, &gZeroVec3f, &gZeroVec3f, &sPrimColor, &sEnvColor, 800, 50);
    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_NPC_APPEAR);
    Flags_SetClearTemp(play, play->roomCtx.curRoom.num);

    for (i = 0; i < 25; i++) {
        velocity.x = Rand_CenteredFloat(5.0f);
        velocity.y = Rand_ZeroFloat(3.0f) + 4.0f;
        velocity.z = Rand_CenteredFloat(5.0f);
        EffectSsHahen_Spawn(play, &this->actor.world.pos, &velocity, &sAccel, 0, Rand_S16Offset(12, 3),
                            HAHEN_OBJECT_DEFAULT, 10, 0);
    }

    this->timer = 40;
    this->actionFunc = EnPametfrog_SpawnFrog;
}

void EnPametfrog_SpawnFrog(EnPametfrog* this, PlayState* play) {
    f32 magShake;

    this->timer--;
    magShake = (Math_SinF(this->timer * (M_PIf / 5)) * ((0.04f * (this->timer * 0.1f)) + 0.02f)) + 1.0f;
    EnPametfrog_ShakeCamera(this, play, 75.0f * magShake, 10.0f * magShake);
    if (this->timer == 0) {
        EnPametfrog_StopCutscene(this, play);
        Actor_Kill(&this->actor);
    }
}

void EnPametfrog_SetupCutscene(EnPametfrog* this) {
    if (this->actor.colChkInfo.health == 0) {
        this->csId = this->actor.csId;
    } else {
        this->csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);
    }

    CutsceneManager_Queue(this->csId);
    this->actionFunc = EnPametfrog_PlayCutscene;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
}

void EnPametfrog_PlayCutscene(EnPametfrog* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_Start(this->csId, &this->actor);
        this->subCamId = CutsceneManager_GetCurrentSubCamId(this->csId);
        Player_SetCsAction(play, &this->actor, PLAYER_CSACTION_WAIT);
        if (this->actor.colChkInfo.health == 0) {
            if (this->actor.params == GEKKO_PRE_SNAPPER) {
                EnPametfrog_SetupCallSnapper(this, play);
            } else {
                EnPametfrog_SetupFallInAir(this, play);
            }
        } else {
            EnPametfrog_SetupFallOffSnapper(this, play);
        }
    } else {
        CutsceneManager_Queue(this->csId);
    }
}

void EnPametfrog_SetupLookAround(EnPametfrog* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoLookAroundAnim);
    this->collider.base.atFlags &= ~AT_ON;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnPametfrog_LookAround;
}

void EnPametfrog_LookAround(EnPametfrog* this, PlayState* play) {
    EnPametfrog_IdleWaterEffects(this, play);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 5, 0x400, 0x80);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (SkelAnime_Update(&this->skelAnime) && !Play_InCsMode(play)) {
        if (!this->unk_2AE) {
            Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
            this->unk_2AE = true;
        }
        EnPametfrog_SetupJumpToLink(this);
    }
}

void EnPametfrog_SetupJumpToLink(EnPametfrog* this) {
    Animation_PlayLoop(&this->skelAnime, &gGekkoJumpForwardAnim);
    this->collider.base.acFlags |= AC_ON;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnPametfrog_JumpToLink;
}

void EnPametfrog_JumpToLink(EnPametfrog* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x80);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    SkelAnime_Update(&this->skelAnime);
    EnPametfrog_JumpOnGround(this, play);
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
        ((this->skelAnime.curFrame > 1.0f) && (this->skelAnime.curFrame < 12.0f))) {
        this->actor.speed = 8.0f;
    } else {
        this->actor.speed = 0.0f;
    }

    if ((this->collider.base.ocFlags1 & OC1_HIT) && (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) &&
        Actor_IsFacingPlayer(&this->actor, 0x3000) &&
        ((this->skelAnime.curFrame <= 2.0f) || (this->skelAnime.curFrame >= 11.0f))) {
        EnPametfrog_SetupMeleeAttack(this);
    }
}

void EnPametfrog_SetupMeleeAttack(EnPametfrog* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoBoxingStanceAnim);
    this->timer = 7;
    this->actor.speed = 0.0f;
    this->actionFunc = EnPametfrog_MeleeAttack;
}

static AnimationHeader* sAttackAnimations[] = {
    &gGekkoJabPunchAnim,
    &gGekkoHookPunchAnim,
    &gGekkoKickAnim,
    &gGekkoWindupPunchAnim,
};

void EnPametfrog_MeleeAttack(EnPametfrog* this, PlayState* play) {
    EnPametfrog_IdleWaterEffects(this, play);
    if (SkelAnime_Update(&this->skelAnime)) {
        this->timer--;
        if (this->timer == 0) {
            EnPametfrog_SetupLookAround(this);
        } else if (this->timer == 6) {
            Animation_PlayOnce(&this->skelAnime, &gGekkoBoxingStanceAnim);
        } else {
            Animation_PlayOnce(&this->skelAnime, sAttackAnimations[(s32)Rand_ZeroFloat(4.0f) % 4]);
        }
    }

    if (((this->skelAnime.animation == &gGekkoJabPunchAnim) && Animation_OnFrame(&this->skelAnime, 2.0f)) ||
        ((this->skelAnime.animation == &gGekkoHookPunchAnim) && Animation_OnFrame(&this->skelAnime, 9.0f)) ||
        ((this->skelAnime.animation == &gGekkoKickAnim) && Animation_OnFrame(&this->skelAnime, 2.0f)) ||
        ((this->skelAnime.animation == &gGekkoWindupPunchAnim) && Animation_OnFrame(&this->skelAnime, 27.0f))) {
        this->collider.base.atFlags |= AT_ON;
        if (this->skelAnime.animation == &gGekkoKickAnim) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_KICK);
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_PUNCH1);
        }
    } else {
        this->collider.base.atFlags &= ~AT_ON;
    }
}

void EnPametfrog_SetupDamage(EnPametfrog* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGekkoKnockbackAnim, -3.0f);
    this->timer = 20;
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.speed = 10.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_DAMAGE);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 20);
    func_800BE5CC(&this->actor, &this->collider, 0);
    this->actor.shape.rot.y = BINANG_ROT180(this->actor.world.rot.y);
    this->actionFunc = EnPametfrog_Damage;
}

void EnPametfrog_Damage(EnPametfrog* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->timer--;
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    if (this->timer == 0) {
        if (this->actor.colChkInfo.health > 0) {
            EnPametfrog_SetupJumpToLink(this);
        } else {
            EnPametfrog_SetupCutscene(this);
        }
    }
}

void EnPametfrog_SetupStun(EnPametfrog* this) {
    if (this->skelAnime.animation == &gGekkoJumpForwardAnim) {
        this->skelAnime.curFrame = 0.0f;
        SkelAnime_Update(&this->skelAnime);
    }

    this->collider.base.atFlags &= ~AT_ON;
    this->actor.speed = 0.0f;

    if (this->actor.colChkInfo.health == 0) {
        this->collider.base.acFlags &= ~AC_ON;
    }

    this->actionFunc = EnPametfrog_Stun;
}

void EnPametfrog_Stun(EnPametfrog* this, PlayState* play) {
    this->freezeTimer--;
    if (this->freezeTimer == 0) {
        EnPametfrog_Thaw(this, play);
        EnPametfrog_SetupJumpToLink(this);
    } else if (this->freezeTimer == 78) {
        EnPametfrog_Thaw(this, play);
        this->actor.colorFilterTimer = 0;
        EnPametfrog_SetupCutscene(this);
    }
}

void EnPametfrog_SetupCallSnapper(EnPametfrog* this, PlayState* play) {
    Vec3f subCamEye;
    Vec3f subCamAt;
    s16 yawDiff;

    Animation_MorphToPlayOnce(&this->skelAnime, &gGekkoCallAnim, 3.0f);
    Actor_PlaySfx(&this->actor, NA_SE_EN_FROG_GREET);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.colChkInfo.health = 6;
    this->actor.world.rot.y = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
    if (yawDiff > 0) {
        this->actor.world.rot.y -= 0x2000;
    } else {
        this->actor.world.rot.y += 0x2000;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    subCamAt.x = this->actor.world.pos.x;
    subCamAt.z = this->actor.world.pos.z;
    subCamAt.y = this->actor.world.pos.y + 45.0f;
    subCamEye.x = subCamAt.x + (Math_SinS(this->actor.shape.rot.y) * 90.0f);
    subCamEye.z = subCamAt.z + (Math_CosS(this->actor.shape.rot.y) * 90.0f);
    subCamEye.y = subCamAt.y + 4.0f;

    // Zooms in on Gekko
    Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCamEye);
    this->timer = 0;
    this->actor.hintId = TATL_HINT_ID_GEKKO_GIANT_SLIME;
    this->actionFunc = EnPametfrog_CallSnapper;
}

void EnPametfrog_CallSnapper(EnPametfrog* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        EnPametfrog_SetupSnapperSpawn(this, play);
    }
}

void EnPametfrog_SetupSnapperSpawn(EnPametfrog* this, PlayState* play) {
    Vec3f subCamAt;
    Vec3f subCamEye;
    s16 yaw;

    EnPametfrog_PlaceSnapper(this, play);
    subCamAt.x = this->actor.child->world.pos.x;
    subCamAt.z = this->actor.child->world.pos.z;
    subCamAt.y = this->actor.child->floorHeight + 50.0f;
    if ((s16)(Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos) - this->actor.shape.rot.y) > 0) {
        yaw = this->actor.child->shape.rot.y - 0x1000;
    } else {
        yaw = this->actor.child->shape.rot.y + 0x1000;
    }

    subCamEye.x = (Math_SinS(yaw) * 500.0f) + subCamAt.x;
    subCamEye.y = subCamAt.y + 55.0f;
    subCamEye.z = (Math_CosS(yaw) * 500.0f) + subCamAt.z;

    // Zooms in on Snapper spawn point
    Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCamEye);

    this->quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_6);
    Quake_SetSpeed(this->quakeIndex, 18000);
    Quake_SetPerturbations(this->quakeIndex, 2, 0, 0, 0);
    Quake_SetDuration(this->quakeIndex, 15);

    Rumble_Request(this->actor.xyzDistToPlayerSq, 120, 20, 10);

    this->timer = 40;
    this->actionFunc = EnPametfrog_SnapperSpawn;
}

void EnPametfrog_SnapperSpawn(EnPametfrog* this, PlayState* play) {
    this->timer--;
    EnPametfrog_ShakeCamera(this, play, (f32)(this->timer * 7.5f) + 200.0f,
                            ((f32)(this->timer * 2) * (15.0f / 16.0f)) + -20.0f);
    if (this->timer != 0) {
        Rumble_Request(this->actor.xyzDistToPlayerSq, 120, 20, 10);
    } else {
        EnPametfrog_SetupTransitionGekkoSnapper(this, play);
    }
}

void EnPametfrog_SetupTransitionGekkoSnapper(EnPametfrog* this, PlayState* play) {
    this->actor.params = GEKKO_GET_SNAPPER;

    Quake_RemoveRequest(this->quakeIndex);

    this->quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
    Quake_SetSpeed(this->quakeIndex, 20000);
    Quake_SetPerturbations(this->quakeIndex, 17, 0, 0, 0);
    Quake_SetDuration(this->quakeIndex, 12);

    Rumble_Request(this->actor.xyzDistToPlayerSq, 255, 20, 150);

    this->actionFunc = EnPametfrog_TransitionGekkoSnapper;
}

void EnPametfrog_TransitionGekkoSnapper(EnPametfrog* this, PlayState* play) {
    if (this->actor.params == GEKKO_INIT_SNAPPER) {
        Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
        EnPametfrog_SetupRunToSnapper(this);
    }
}

void EnPametfrog_ApplyDamageEffect(EnPametfrog* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        if ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
            !(this->collider.elements[0].base.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3)) {
            if (this->actor.params == GEKKO_PRE_SNAPPER) {
                if (Actor_ApplyDamage(&this->actor) == 0) {
                    Audio_RestorePrevBgm();
                }

                if (this->actor.colChkInfo.damageEffect == GEKKO_DMGEFF_ZORA_BARRIER) {
                    EnPametfrog_ApplyElectricStun(this);
                    EnPametfrog_SetupStun(this);
                } else if (this->actor.colChkInfo.damageEffect == GEKKO_DMGEFF_STUN) {
                    EnPametfrog_ApplyStun(this);
                    EnPametfrog_SetupStun(this);
                } else if (this->actor.colChkInfo.damageEffect == GEKKO_DMGEFF_ICE) {
                    EnPametfrog_Freeze(this);
                    this->freezeTimer = 80;
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
                    EnPametfrog_SetupStun(this);
                } else {
                    EnPametfrog_Thaw(this, play);
                    if (this->actor.colChkInfo.damageEffect == GEKKO_DMGEFF_FIRE) {
                        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                        this->drawDmgEffScale = 0.75f;
                        this->drawDmgEffAlpha = 4.0f;
                    } else if (this->actor.colChkInfo.damageEffect == GEKKO_DMGEFF_LIGHT) {
                        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                        this->drawDmgEffScale = 0.75f;
                        this->drawDmgEffAlpha = 4.0f;
                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG,
                                    this->collider.elements[0].base.acDmgInfo.hitPos.x,
                                    this->collider.elements[0].base.acDmgInfo.hitPos.y,
                                    this->collider.elements[0].base.acDmgInfo.hitPos.z, 0, 0, 0,
                                    CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                    }
                    EnPametfrog_SetupDamage(this);
                }
            } else if (Actor_ApplyDamage(&this->actor) == 0) {
                this->collider.base.acFlags &= ~AC_ON;
                EnPametfrog_ApplyMagicArrowEffects(this, play);
                Enemy_StartFinishingBlow(play, &this->actor);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                Audio_RestorePrevBgm();
                EnPametfrog_SetupCutscene(this);
            } else if (this->actor.colChkInfo.damageEffect == GEKKO_DMGEFF_ZORA_BARRIER) {
                EnPametfrog_ApplyElectricStun(this);
            } else if (this->actor.colChkInfo.damageEffect == GEKKO_DMGEFF_STUN) {
                EnPametfrog_ApplyStun(this);
            } else {
                EnPametfrog_ApplyMagicArrowEffects(this, play);
                EnPametfrog_SetupFallInAir(this, play);
            }
        }
    }
}

void EnPametfrog_Update(Actor* thisx, PlayState* play) {
    EnPametfrog* this = (EnPametfrog*)thisx;
    f32 unk2C4;
    f32 wallCheckRadius;

    if (this->actor.params == GEKKO_CUTSCENE) {
        EnPametfrog_SetupCutscene(this);
    } else if (this->actionFunc != EnPametfrog_PlayCutscene) {
        EnPametfrog_ApplyDamageEffect(this, play);
    } else {
        this->collider.base.acFlags &= ~AC_HIT;
    }

    this->actionFunc(this, play);

    if ((this->actionFunc != EnPametfrog_JumpOnSnapper) && (this->actionFunc != EnPametfrog_RearOnSnapperRise)) {
        if (this->actor.gravity < -0.1f) {
            Actor_MoveWithGravity(&this->actor);
            wallCheckRadius = (this->actionFunc == EnPametfrog_FallInAir) ? 3.0f : 15.0f;
            Actor_UpdateBgCheckInfo(play, &this->actor, 25.0f, wallCheckRadius, 3.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                        UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
        } else if (this->freezeTimer == 0) {
            Actor_MoveWithoutGravity(&this->actor);
            this->actor.floorHeight = this->actor.world.pos.y;
        }
    }

    if (this->collider.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->collider.base.ocFlags1 & OC1_ON) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->drawDmgEffAlpha > 0.0f) {
        if ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) &&
            (this->actionFunc != EnPametfrog_PlayCutscene)) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            unk2C4 = ((this->drawDmgEffAlpha + 1.0f) * 0.375f);
            this->drawDmgEffScale = unk2C4;
            this->drawDmgEffScale = unk2C4 > 0.75f ? 0.75f : this->drawDmgEffScale;
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.75f, (3.0f / 160.0f))) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

static s8 sLimbToBodyParts[GEKKO_LIMB_MAX] = {
    BODYPART_NONE,                  // GEKKO_LIMB_NONE
    BODYPART_NONE,                  // GEKKO_LIMB_ROOT
    GEKKO_BODYPART_WAIST,           // GEKKO_LIMB_WAIST
    BODYPART_NONE,                  // GEKKO_LIMB_LEFT_THIGH
    GEKKO_BODYPART_LEFT_SHIN,       // GEKKO_LIMB_LEFT_SHIN
    BODYPART_NONE,                  // GEKKO_LIMB_LEFT_ANKLE
    GEKKO_BODYPART_LEFT_FOOT,       // GEKKO_LIMB_LEFT_FOOT
    BODYPART_NONE,                  // GEKKO_LIMB_RIGHT_THIGH
    GEKKO_BODYPART_RIGHT_SHIN,      // GEKKO_LIMB_RIGHT_SHIN
    BODYPART_NONE,                  // GEKKO_LIMB_RIGHT_ANKLE
    GEKKO_BODYPART_RIGHT_FOOT,      // GEKKO_LIMB_RIGHT_FOOT
    BODYPART_NONE,                  // GEKKO_LIMB_TORSO
    GEKKO_BODYPART_LEFT_UPPER_ARM,  // GEKKO_LIMB_LEFT_UPPER_ARM
    GEKKO_BODYPART_LEFT_FOREARM,    // GEKKO_LIMB_LEFT_FOREARM
    BODYPART_NONE,                  // GEKKO_LIMB_LEFT_WRIST
    GEKKO_BODYPART_LEFT_HAND,       // GEKKO_LIMB_LEFT_HAND
    GEKKO_BODYPART_RIGHT_UPPER_ARM, // GEKKO_LIMB_RIGHT_UPPER_ARM
    GEKKO_BODYPART_RIGHT_FOREARM,   // GEKKO_LIMB_RIGHT_FOREARM
    BODYPART_NONE,                  // GEKKO_LIMB_RIGHT_WRIST
    GEKKO_BODYPART_RIGHT_HAND,      // GEKKO_LIMB_RIGHT_HAND
    BODYPART_NONE,                  // GEKKO_LIMB_HEAD
    GEKKO_BODYPART_JAW,             // GEKKO_LIMB_JAW
    BODYPART_NONE,                  // GEKKO_LIMB_LEFT_EYE
    BODYPART_NONE,                  // GEKKO_LIMB_RIGHT_EYE
};

void EnPametfrog_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnPametfrog* this = (EnPametfrog*)thisx;
    Vec3f vec;
    Vec3s* center;
    s8 bodyPartIndex;

    if (limbIndex == GEKKO_LIMB_HEAD) {
        Matrix_MultZero(&this->actor.focus.pos);
        this->actor.focus.rot.y = this->actor.shape.rot.y;
        Matrix_MultVecY(2500.0f, &vec);
        center = &this->collider.elements[0].dim.worldSphere.center;
        center->x = vec.x;
        center->y = vec.y;
        center->z = vec.z;
        center = &this->collider.elements[1].dim.worldSphere.center;
        center->x = this->actor.focus.pos.x + (Math_SinS(this->actor.shape.rot.y) * 35.0f);
        center->y = this->actor.focus.pos.y - 10.0f;
        center->z = this->actor.focus.pos.z + (Math_CosS(this->actor.shape.rot.y) * 35.0f);
    }

    bodyPartIndex = sLimbToBodyParts[limbIndex];
    if (bodyPartIndex != BODYPART_NONE) {
        Matrix_MultZero(&this->bodyPartsPos[bodyPartIndex]);
    }
}

void EnPametfrog_Draw(Actor* thisx, PlayState* play) {
    EnPametfrog* this = (EnPametfrog*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Matrix_RotateYS(this->spinYaw, MTXMODE_APPLY);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          EnPametfrog_PostLimbDraw, &this->actor);
    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, GEKKO_BODYPART_MAX, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);
}
