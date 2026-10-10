/*
 * File: z_en_zo.c
 * Overlay: ovl_En_Zo
 * Description: Zoras (unused)
 */

#include "z_en_zo.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnZo_Init(Actor* thisx, PlayState* play);
void EnZo_Destroy(Actor* thisx, PlayState* play);
void EnZo_Update(Actor* thisx, PlayState* play);
void EnZo_Draw(Actor* thisx, PlayState* play);

void EnZo_FollowPath(EnZo* this, PlayState* play);
void EnZo_TreadWater(EnZo* this, PlayState* play);
void EnZo_DoNothing(EnZo* this, PlayState* play);

ActorProfile En_Zo_Profile = {
    /**/ ACTOR_EN_ZO,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZO,
    /**/ sizeof(EnZo),
    /**/ EnZo_Init,
    /**/ EnZo_Destroy,
    /**/ EnZo_Update,
    /**/ EnZo_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 64, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0),
    /* Deku Stick     */ DMG_ENTRY(0, 0),
    /* Horse trample  */ DMG_ENTRY(0, 0),
    /* Explosives     */ DMG_ENTRY(0, 0),
    /* Zora boomerang */ DMG_ENTRY(0, 0),
    /* Normal arrow   */ DMG_ENTRY(0, 0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0),
    /* Hookshot       */ DMG_ENTRY(0, 0),
    /* Goron punch    */ DMG_ENTRY(0, 0),
    /* Sword          */ DMG_ENTRY(0, 0),
    /* Goron pound    */ DMG_ENTRY(0, 0),
    /* Fire arrow     */ DMG_ENTRY(0, 0),
    /* Ice arrow      */ DMG_ENTRY(0, 0),
    /* Light arrow    */ DMG_ENTRY(0, 0),
    /* Goron spikes   */ DMG_ENTRY(0, 0),
    /* Deku spin      */ DMG_ENTRY(0, 0),
    /* Deku bubble    */ DMG_ENTRY(0, 0),
    /* Deku launch    */ DMG_ENTRY(0, 0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0),
    /* Zora barrier   */ DMG_ENTRY(0, 0),
    /* Normal shield  */ DMG_ENTRY(0, 0),
    /* Light ray      */ DMG_ENTRY(0, 0),
    /* Thrown object  */ DMG_ENTRY(0, 0),
    /* Zora punch     */ DMG_ENTRY(0, 0),
    /* Spin attack    */ DMG_ENTRY(0, 0),
    /* Sword beam     */ DMG_ENTRY(0, 0),
    /* Normal Roll    */ DMG_ENTRY(0, 0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0),
    /* Unblockable    */ DMG_ENTRY(0, 0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0),
    /* Powder Keg     */ DMG_ENTRY(0, 0),
};

typedef enum EnZoAnimation {
    /* -1 */ ENZO_ANIM_NONE = -1,
    /*  0 */ ENZO_ANIM_IDLE,
    /*  1 */ ENZO_ANIM_MORPH,
    /*  2 */ ENZO_ANIM_SURFACING,
    /*  3 */ ENZO_ANIM_HANDS_ON_HIP_TAPPING_FOOT,
    /*  4 */ ENZO_ANIM_ARMS_OPEN,
    /*  5 */ ENZO_ANIM_THROW_RUPEE,
    /*  6 */ ENZO_ANIM_WALK,
    /*  7 */ ENZO_ANIM_MAX
} EnZoAnimation;

static AnimationInfoS sAnimationInfo[ENZO_ANIM_MAX] = {
    { &gZoraIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },                    // ENZO_ANIM_IDLE
    { &gZoraIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },                   // ENZO_ANIM_MORPH
    { &gZoraSurfacingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },              // ENZO_ANIM_SURFACING
    { &gZoraHandsOnHipsTappingFootAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENZO_ANIM_HANDS_ON_HIP_TAPPING_FOOT
    { &gZoraArmsOpenAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },               // ENZO_ANIM_ARMS_OPEN
    { &gZoraThrowRupeeAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },             // ENZO_ANIM_THROW_RUPEE
    { &gZoraWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },                   // ENZO_ANIM_WALK
};

static s8 sLimbToBodyParts[ZORA_LIMB_MAX] = {
    BODYPART_NONE,    // ZORA_LIMB_NONE
    ZORA_BODYPART_1,  // ZORA_LIMB_PELVIS
    ZORA_BODYPART_12, // ZORA_LIMB_LEFT_THIGH
    ZORA_BODYPART_13, // ZORA_LIMB_LEFT_SHIN
    ZORA_BODYPART_14, // ZORA_LIMB_LEFT_FOOT
    ZORA_BODYPART_9,  // ZORA_LIMB_RIGHT_THIGH
    ZORA_BODYPART_10, // ZORA_LIMB_RIGHT_SHIN
    ZORA_BODYPART_11, // ZORA_LIMB_RIGHT_FOOT
    ZORA_BODYPART_0,  // ZORA_LIMB_TORSO
    ZORA_BODYPART_6,  // ZORA_LIMB_LEFT_UPPER_ARM
    ZORA_BODYPART_7,  // ZORA_LIMB_LEFT_FOREARM
    ZORA_BODYPART_8,  // ZORA_LIMB_LEFT_HAND
    ZORA_BODYPART_3,  // ZORA_LIMB_RIGHT_UPPER_ARM
    ZORA_BODYPART_4,  // ZORA_LIMB_RIGHT_FOREARM
    ZORA_BODYPART_5,  // ZORA_LIMB_RIGHT_HAND
    ZORA_BODYPART_2,  // ZORA_LIMB_HEAD
    BODYPART_NONE,    // ZORA_LIMB_HEAD_TAIL_1
    BODYPART_NONE,    // ZORA_LIMB_HEAD_TAIL_2
    BODYPART_NONE,    // ZORA_LIMB_HEAD_TAIL_3
    BODYPART_NONE,    // ZORA_LIMB_HEAD_TAIL_FIN
};

static s8 sParentShadowBodyParts[ZORA_BODYPART_MAX] = {
    ZORA_BODYPART_0,  // ZORA_BODYPART_0
    ZORA_BODYPART_0,  // ZORA_BODYPART_1
    ZORA_BODYPART_0,  // ZORA_BODYPART_2
    ZORA_BODYPART_0,  // ZORA_BODYPART_3
    ZORA_BODYPART_3,  // ZORA_BODYPART_4
    ZORA_BODYPART_4,  // ZORA_BODYPART_5
    ZORA_BODYPART_0,  // ZORA_BODYPART_6
    ZORA_BODYPART_6,  // ZORA_BODYPART_7
    ZORA_BODYPART_7,  // ZORA_BODYPART_8
    ZORA_BODYPART_0,  // ZORA_BODYPART_9
    ZORA_BODYPART_9,  // ZORA_BODYPART_10
    ZORA_BODYPART_10, // ZORA_BODYPART_11
    ZORA_BODYPART_0,  // ZORA_BODYPART_12
    ZORA_BODYPART_12, // ZORA_BODYPART_13
    ZORA_BODYPART_13, // ZORA_BODYPART_14
};

static u8 sShadowSizes[ZORA_BODYPART_MAX] = {
    0, // ZORA_BODYPART_0
    0, // ZORA_BODYPART_1
    0, // ZORA_BODYPART_2
    0, // ZORA_BODYPART_3
    0, // ZORA_BODYPART_4
    0, // ZORA_BODYPART_5
    0, // ZORA_BODYPART_6
    0, // ZORA_BODYPART_7
    0, // ZORA_BODYPART_8
    0, // ZORA_BODYPART_9
    0, // ZORA_BODYPART_10
    0, // ZORA_BODYPART_11
    0, // ZORA_BODYPART_12
    0, // ZORA_BODYPART_13
    0, // ZORA_BODYPART_14
};

s32 EnZo_ChangeAnim(SkelAnime* skelAnime, s16 animIndex) {
    s16 endFrame;
    s32 didChange = false;

    if ((animIndex > ENZO_ANIM_NONE) && (animIndex < ENZO_ANIM_MAX)) {
        didChange = true;
        endFrame = sAnimationInfo[animIndex].frameCount;
        if (endFrame < 0) {
            endFrame = Animation_GetLastFrame(sAnimationInfo[animIndex].animation);
        }
        Animation_Change(skelAnime, sAnimationInfo[animIndex].animation, sAnimationInfo[animIndex].playSpeed,
                         sAnimationInfo[animIndex].startFrame, endFrame, sAnimationInfo[animIndex].mode,
                         sAnimationInfo[animIndex].morphFrames);
    }
    return didChange;
}

s32 EnZo_PlayWalkingSound(EnZo* this, PlayState* play) {
    u8 leftWasGrounded;
    u8 rightWasGrounded;
    SurfaceSfxOffset surfaceSfxOffset;
    u16 sfxId;
    u8 isFootGrounded;

    leftWasGrounded = this->isLeftFootGrounded;
    rightWasGrounded = this->isRightFootGrounded;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
        if (this->actor.depthInWater < 20.0f) {
            surfaceSfxOffset = SURFACE_SFX_OFFSET_WATER_SHALLOW;
        } else {
            surfaceSfxOffset = SURFACE_SFX_OFFSET_WATER_DEEP;
        }
        sfxId = NA_SE_PL_WALK_GROUND + surfaceSfxOffset;
    } else {
        sfxId = NA_SE_PL_WALK_GROUND +
                SurfaceType_GetSfxOffset(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
    }

    this->isLeftFootGrounded = isFootGrounded = SubS_IsFloorAbove(play, &this->leftFootPos, -6.0f);
    if (this->isLeftFootGrounded && !leftWasGrounded && isFootGrounded) {
        Actor_PlaySfx(&this->actor, sfxId);
    }

    this->isRightFootGrounded = isFootGrounded = SubS_IsFloorAbove(play, &this->rightFootPos, -6.0f);
    if (this->isRightFootGrounded && !rightWasGrounded && isFootGrounded) {
        Actor_PlaySfx(&this->actor, sfxId);
    }

    return 0;
}

void EnZo_Blink(EnZo* this, s32 maxEyeIndex) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeIndex++;
        if (this->eyeIndex >= maxEyeIndex) {
            this->eyeIndex = 0;
            this->blinkTimer = Rand_S16Offset(30, 30);
        }
    }
}

void EnZo_UpdateCollider(EnZo* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

static TrackOptionsSet sTrackOptions = {
    { 0xFA0, 4, 1, 3 },
    { 0x1770, 4, 1, 6 },
    { 0xFA0, 4, 1, 3 },
    { 0x1770, 4, 1, 6 },
};

void EnZo_LookAtPlayer(EnZo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f point;

    SkelAnime_Update(&this->skelAnime);
    if (SubS_AngleDiffLessEqual(this->actor.shape.rot.y, 0x2710, this->actor.yawTowardsPlayer)) {
        point.x = player->actor.world.pos.x;
        point.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
        point.z = player->actor.world.pos.z;
        SubS_TrackPoint(&point, &this->actor.focus.pos, &this->actor.shape.rot, &this->trackTarget, &this->headRot,
                        &this->upperBodyRot, &sTrackOptions);
    } else {
        Math_SmoothStepToS(&this->trackTarget.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->trackTarget.y, 0, 4, 0x3E8, 1);

        Math_SmoothStepToS(&this->headRot.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->headRot.y, 0, 4, 0x3E8, 1);

        Math_SmoothStepToS(&this->upperBodyRot.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->upperBodyRot.y, 0, 4, 0x3E8, 1);
    }

    EnZo_Blink(this, 3);
    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, ZORA_LIMB_MAX);
}

void EnZo_Walk(EnZo* this, PlayState* play) {
    if (ENZO_GET_PATH_INDEX(&this->actor) != ENZO_PATH_INDEX_NONE) {
        EnZo_ChangeAnim(&this->skelAnime, ENZO_ANIM_WALK);
    }

    if (ENZO_GET_PATH_INDEX(&this->actor) != ENZO_PATH_INDEX_NONE) {
        this->actionFunc = EnZo_FollowPath;
    } else {
        this->actionFunc = EnZo_DoNothing;
    }
}

void EnZo_FollowPath(EnZo* this, PlayState* play) {
    s16 speed;
    Vec3f pos;

    Math_SmoothStepToF(&this->actor.speed, 1.0f, 0.4f, 1000.0f, 0.0f);
    speed = this->actor.speed * 400.0f;
    if (SubS_CopyPointFromPath(this->path, this->waypoint, &pos) && SubS_MoveActorToPoint(&this->actor, &pos, speed)) {
        this->waypoint++;
        if (this->waypoint >= this->path->count) {
            this->waypoint = 0;
        }
    }

    if (this->actor.depthInWater > 60.0f) {
        EnZo_ChangeAnim(&this->skelAnime, ENZO_ANIM_MORPH);
        this->actionFunc = EnZo_TreadWater;
        this->actor.gravity = 0.0f;
        this->actor.speed = 0.0f;
    }
}

void EnZo_TreadWater(EnZo* this, PlayState* play) {
    f32 targetYVel;

    if (this->actor.depthInWater < (sREG(0) + 50.0f)) {
        targetYVel = -1.0f;
    } else {
        targetYVel = 1.0f;
    }
    Math_ApproachF(&this->actor.velocity.y, targetYVel, (sREG(1) + 18.0f) * 0.01f, (sREG(2) + 12.0f) * 0.01f);
}

void EnZo_DoNothing(EnZo* this, PlayState* play) {
}

void EnZo_Init(Actor* thisx, PlayState* play) {
    EnZo* this = (EnZo*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gZoraSkel, NULL, this->jointTable, this->morphTable, ZORA_LIMB_MAX);
    EnZo_ChangeAnim(&this->skelAnime, ENZO_ANIM_IDLE);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    this->path = SubS_GetPathByIndex(play, ENZO_GET_PATH_INDEX(&this->actor), ENZO_PATH_INDEX_NONE);
    Actor_SetScale(&this->actor, 0.01f);

    this->actionFunc = EnZo_Walk;
    this->actor.gravity = -4.0f;
}

void EnZo_Destroy(Actor* thisx, PlayState* play) {
    EnZo* this = (EnZo*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnZo_Update(Actor* thisx, PlayState* play) {
    EnZo* this = (EnZo*)thisx;

    this->actionFunc(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    EnZo_LookAtPlayer(this, play);
    EnZo_PlayWalkingSound(this, play);
    EnZo_UpdateCollider(this, play);
}

s32 EnZo_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                          Gfx** gfx) {
    EnZo* this = (EnZo*)thisx;

    if (limbIndex == ZORA_LIMB_HEAD) {
        Matrix_Translate(1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(-this->headRot.x, MTXMODE_APPLY);
        Matrix_Translate(-1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == ZORA_LIMB_TORSO) {
        Matrix_RotateXS(-this->upperBodyRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(-this->upperBodyRot.x, MTXMODE_APPLY);
    }

    if ((limbIndex == ZORA_LIMB_TORSO) || (limbIndex == ZORA_LIMB_LEFT_UPPER_ARM) ||
        (limbIndex == ZORA_LIMB_RIGHT_UPPER_ARM)) {
        rot->y += TRUNCF_BINANG(Math_SinS(this->fidgetTableY[limbIndex]) * 200.0f);
        rot->z += TRUNCF_BINANG(Math_CosS(this->fidgetTableZ[limbIndex]) * 200.0f);
    }
    return false;
}

void EnZo_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnZo* this = (EnZo*)thisx;
    Vec3f sp30 = { 400.0f, 0.0f, 0.0f };
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (sLimbToBodyParts[limbIndex] > BODYPART_NONE) {
        Matrix_MultVec3f(&zeroVec, &this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
    }

    if (limbIndex == ZORA_LIMB_HEAD) {
        Matrix_MultVec3f(&sp30, &this->actor.focus.pos);
    }

    if (limbIndex == ZORA_LIMB_LEFT_FOOT) {
        Matrix_MultVec3f(&zeroVec, &this->leftFootPos);
    }

    if (limbIndex == ZORA_LIMB_RIGHT_FOOT) {
        Matrix_MultVec3f(&zeroVec, &this->rightFootPos);
    }
}

static Gfx sTransparencyDlist[] = {
    gsDPSetRenderMode(AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                          G_RM_FOG_SHADE_A,
                      AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                          GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)),
    gsDPSetAlphaCompare(G_AC_THRESHOLD),
    gsSPEndDisplayList(),
};

void EnZo_Draw(Actor* thisx, PlayState* play) {
    EnZo* this = (EnZo*)thisx;
    s32 i;
    u8* shadowTex = GRAPH_ALLOC(play->state.gfxCtx, SUBS_SHADOW_TEX_SIZE);
    u8* shadowTexIter;
    TexturePtr eyeTextures[] = { gZoraEyeOpenTex, gZoraEyeHalfTex, gZoraEyeClosedTex };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gDPPipeSync(POLY_OPA_DISP++);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(eyeTextures[this->eyeIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x0C, &sTransparencyDlist[2]);

    POLY_OPA_DISP =
        SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                           EnZo_OverrideLimbDraw, EnZo_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    Matrix_RotateXS(0, MTXMODE_NEW);

    for (i = 0, shadowTexIter = shadowTex; i < SUBS_SHADOW_TEX_SIZE; i++) {
        *shadowTexIter = 0;
        shadowTexIter++;
    }
    for (i = 0; i < 5; i++) {
        SubS_GenShadowTex(this->bodyPartsPos, &this->actor.world.pos, shadowTex, i / 5.0f, ZORA_BODYPART_MAX,
                          sShadowSizes, sParentShadowBodyParts);
    }

    SubS_DrawShadowTex(&this->actor, &play->state, shadowTex);

    CLOSE_DISPS(play->state.gfxCtx);
}
