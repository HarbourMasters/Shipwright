/*
 * File: z_en_ru.c
 * Overlay: ovl_En_Ru
 * Description: OoT's Adult Ruto (unused)
 */

#include "z_en_ru.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnRu_Init(Actor* thisx, PlayState* play);
void EnRu_Destroy(Actor* thisx, PlayState* play);
void EnRu_Update(Actor* thisx, PlayState* play);
void EnRu_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Ru_Profile = {
    /**/ ACTOR_EN_RU,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_RU2,
    /**/ sizeof(EnRu),
    /**/ EnRu_Init,
    /**/ EnRu_Destroy,
    /**/ EnRu_Update,
    /**/ EnRu_Draw,
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
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0x0),
    /* Goron punch    */ DMG_ENTRY(0, 0x0),
    /* Sword          */ DMG_ENTRY(0, 0x0),
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(0, 0x0),
    /* Goron spikes   */ DMG_ENTRY(0, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0x0),
    /* Zora punch     */ DMG_ENTRY(0, 0x0),
    /* Spin attack    */ DMG_ENTRY(0, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

typedef enum AdultRutoAnimation {
    /* -1 */ ADULT_RUTO_ANIM_NONE = -1,
    /*  0 */ ADULT_RUTO_ANIM_IDLE,
    /*  1 */ ADULT_RUTO_ANIM_IDLE_MORPH,
    /*  2 */ ADULT_RUTO_ANIM_RAISING_ARMS_UP,
    /*  3 */ ADULT_RUTO_ANIM_CROSSING_ARMS,
    /*  4 */ ADULT_RUTO_ANIM_LOOKING_DOWN_LEFT,
    /*  5 */ ADULT_RUTO_ANIM_HANDS_ON_HIPS,
    /*  6 */ ADULT_RUTO_ANIM_TURN_DOWN_LEFT,
    /*  7 */ ADULT_RUTO_ANIM_SWIMMING_UP,
    /*  8 */ ADULT_RUTO_ANIM_MAX
} AdultRutoAnimation;

static AnimationInfoS sAnimationInfo[ADULT_RUTO_ANIM_MAX] = {
    { &gAdultRutoIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },              // ADULT_RUTO_ANIM_IDLE
    { &gAdultRutoIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },             // ADULT_RUTO_ANIM_IDLE_MORPH
    { &gAdultRutoRaisingArmsUpAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },    // ADULT_RUTO_ANIM_RAISING_ARMS_UP
    { &gAdultRutoCrossingArmsAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },     // ADULT_RUTO_ANIM_CROSSING_ARMS
    { &gAdultRutoLookingDownLeftAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },  // ADULT_RUTO_ANIM_LOOKING_DOWN_LEFT
    { &gAdultRutoIdleHandsOnHipsAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },  // ADULT_RUTO_ANIM_HANDS_ON_HIPS
    { &gAdultRutoHeadTurnDownLeftAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ADULT_RUTO_ANIM_TURN_DOWN_LEFT
    { &gAdultRutoSwimmingUpAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },       // ADULT_RUTO_ANIM_SWIMMING_UP
};

static s8 sLimbToBodyParts[RU2_LIMB_MAX] = {
    BODYPART_NONE,  // RU2_LIMB_NONE
    BODYPART_NONE,  // RU2_LIMB_ROOT
    RU_BODYPART_12, // RU2_LIMB_LEFT_THIGH
    RU_BODYPART_13, // RU2_LIMB_LEFT_LEG
    RU_BODYPART_14, // RU2_LIMB_LEFT_FOOT
    BODYPART_NONE,  // RU2_LIMB_LEFT_TAIL
    RU_BODYPART_9,  // RU2_LIMB_RIGHT_THIGH
    RU_BODYPART_10, // RU2_LIMB_RIGHT_LEG
    RU_BODYPART_11, // RU2_LIMB_RIGHT_FOOT
    BODYPART_NONE,  // RU2_LIMB_RIGHT_TAIL
    RU_BODYPART_0,  // RU2_LIMB_TORSO
    RU_BODYPART_6,  // RU2_LIMB_LEFT_UPPER_ARM
    BODYPART_NONE,  // RU2_LIMB_LEFT_UPPER_WING
    BODYPART_NONE,  // RU2_LIMB_LEFT_LOWER_WING
    RU_BODYPART_7,  // RU2_LIMB_LEFT_FOREARM
    RU_BODYPART_8,  // RU2_LIMB_LEFT_HAND
    RU_BODYPART_2,  // RU2_LIMB_RIGHT_UPPER_ARM
    BODYPART_NONE,  // RU2_LIMB_RIGHT_UPPER_WING
    BODYPART_NONE,  // RU2_LIMB_RIGHT_LOWER_WING
    RU_BODYPART_3,  // RU2_LIMB_RIGHT_FOREARM
    RU_BODYPART_4,  // RU2_LIMB_RIGHT_HAND
    RU_BODYPART_2,  // RU2_LIMB_HEAD
    RU_BODYPART_1,  // RU2_LIMB_WAIST
};

static s8 sParentShadowBodyParts[RU_BODYPART_MAX] = {
    RU_BODYPART_0,  // RU_BODYPART_0
    RU_BODYPART_0,  // RU_BODYPART_1
    RU_BODYPART_0,  // RU_BODYPART_2
    RU_BODYPART_0,  // RU_BODYPART_3
    RU_BODYPART_3,  // RU_BODYPART_4
    RU_BODYPART_4,  // RU_BODYPART_5
    RU_BODYPART_0,  // RU_BODYPART_6
    RU_BODYPART_6,  // RU_BODYPART_7
    RU_BODYPART_7,  // RU_BODYPART_8
    RU_BODYPART_0,  // RU_BODYPART_9
    RU_BODYPART_9,  // RU_BODYPART_10
    RU_BODYPART_10, // RU_BODYPART_11
    RU_BODYPART_0,  // RU_BODYPART_12
    RU_BODYPART_12, // RU_BODYPART_13
    RU_BODYPART_13, // RU_BODYPART_14
};

static u8 sShadowSizes[RU_BODYPART_MAX] = {
    0, // RU_BODYPART_0
    0, // RU_BODYPART_1
    0, // RU_BODYPART_2
    0, // RU_BODYPART_3
    0, // RU_BODYPART_4
    0, // RU_BODYPART_5
    0, // RU_BODYPART_6
    0, // RU_BODYPART_7
    0, // RU_BODYPART_8
    0, // RU_BODYPART_9
    0, // RU_BODYPART_10
    0, // RU_BODYPART_11
    0, // RU_BODYPART_12
    0, // RU_BODYPART_13
    0, // RU_BODYPART_14
};

s32 EnRu_ChangeAnim(SkelAnime* skelAnime, s16 animIndex) {
    s16 endFrame;
    s32 didAnimChange = false;

    if ((animIndex > ADULT_RUTO_ANIM_NONE) && (animIndex < ADULT_RUTO_ANIM_MAX)) {
        endFrame = sAnimationInfo[animIndex].frameCount;

        didAnimChange = true;
        if (endFrame < 0) {
            endFrame = Animation_GetLastFrame(sAnimationInfo[animIndex].animation);
        }

        Animation_Change(skelAnime, sAnimationInfo[animIndex].animation, sAnimationInfo[animIndex].playSpeed,
                         sAnimationInfo[animIndex].startFrame, endFrame, sAnimationInfo[animIndex].mode,
                         sAnimationInfo[animIndex].morphFrames);
    }

    return didAnimChange;
}

// En_Zo has a copy of this function
s32 EnRu_PlayWalkingSound(EnRu* this, PlayState* play) {
    u8 leftWasGrounded;
    u8 rightWasGrounded;
    SurfaceSfxOffset surfaceSfxOffset;
    s16 sfxId;
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

    return false;
}

// This function is only ever called with eyeStateMax == 3
void EnRu_UpdateEyes(EnRu* this, s32 eyeStateMax) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeState++;
        if (this->eyeState >= eyeStateMax) {
            this->eyeState = 0;
            this->blinkTimer = Rand_S16Offset(30, 30);
        }
    }
}

void EnRu_UpdateCollider(EnRu* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

static TrackOptionsSet sTrackOptions = {
    { 0xFA0, 4, 1, 3 }, { 0x1770, 4, 1, 6 }, { 0xFA0, 4, 1, 3 }, { 0x1770, 4, 1, 6 }
};

void EnRu_UpdateModel(EnRu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);

    // Head and Torso tracking to Player if in front of Ru.
    if (SubS_AngleDiffLessEqual(this->actor.shape.rot.y, 0x2710, this->actor.yawTowardsPlayer)) {
        Vec3f playerPos;

        playerPos.x = player->actor.world.pos.x;
        playerPos.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
        playerPos.z = player->actor.world.pos.z;
        SubS_TrackPoint(&playerPos, &this->actor.focus.pos, &this->actor.shape.rot, &this->trackTarget, &this->headRot,
                        &this->torsoRot, &sTrackOptions);

    } else { // smooth her back to facing forward
        Math_SmoothStepToS(&this->trackTarget.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->trackTarget.y, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->headRot.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->headRot.y, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->torsoRot.y, 0, 4, 0x3E8, 1);
    }

    EnRu_UpdateEyes(this, 3);
    EnRu_PlayWalkingSound(this, play);
    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, RU2_LIMB_MAX);
}

void EnRu_DoNothing(EnRu* this, PlayState* play) {
}

void EnRu_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRu* this = (EnRu*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gAdultRutoSkel, NULL, this->jointTable, this->morphTable, RU2_LIMB_MAX);
    EnRu_ChangeAnim(&this->skelAnime, ADULT_RUTO_ANIM_IDLE);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    this->path = SubS_GetPathByIndex(play, RU_GET_PATH_INDEX(thisx), RU_PATH_INDEX_NONE);
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = EnRu_DoNothing;
    this->actor.gravity = -4.0f;
}

void EnRu_Destroy(Actor* thisx, PlayState* play) {
    EnRu* this = (EnRu*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnRu_Update(Actor* thisx, PlayState* play) {
    EnRu* this = (EnRu*)thisx;

    this->actionFunc(this, play);

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    EnRu_UpdateModel(this, play);
    EnRu_UpdateCollider(this, play);
}

s32 EnRu_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                          Gfx** gfx) {
    EnRu* this = (EnRu*)thisx;

    if (limbIndex == RU2_LIMB_HEAD) {
        Matrix_Translate(1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(this->headRot.x, MTXMODE_APPLY);
        Matrix_Translate(-1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == RU2_LIMB_TORSO) {
        Matrix_RotateYS(this->torsoRot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->torsoRot.x, MTXMODE_APPLY);
    }

    if ((limbIndex == RU2_LIMB_TORSO) || (limbIndex == RU2_LIMB_LEFT_UPPER_ARM) ||
        (limbIndex == RU2_LIMB_RIGHT_UPPER_ARM)) {
        rot->y += TRUNCF_BINANG(Math_SinS(this->fidgetTableY[limbIndex]) * 200.0f);
        rot->z += TRUNCF_BINANG(Math_CosS(this->fidgetTableZ[limbIndex]) * 200.0f);
    }

    return false;
}

void EnRu_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnRu* this = (EnRu*)thisx;
    Vec3f headFocus = { 800.0f, 0, 0 };
    Vec3f bodyPartPos = { 0, 0, 0 };

    if (sLimbToBodyParts[limbIndex] > BODYPART_NONE) {
        Matrix_MultVec3f(&bodyPartPos, &this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
    }
    if (limbIndex == RU2_LIMB_HEAD) {
        Matrix_MultVec3f(&headFocus, &thisx->focus.pos);
    }

    if (limbIndex == RU2_LIMB_LEFT_FOOT) {
        Matrix_MultVec3f(&bodyPartPos, &this->leftFootPos);
    }
    if (limbIndex == RU2_LIMB_RIGHT_FOOT) {
        Matrix_MultVec3f(&bodyPartPos, &this->rightFootPos);
    }
}

// This is a copy of displaylist found in En_Zo
static Gfx sTransparencyDlist[] = {
    gsDPSetRenderMode(AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                          G_RM_FOG_SHADE_A,
                      AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                          GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)),
    gsDPSetAlphaCompare(G_AC_THRESHOLD),
    gsSPEndDisplayList(),
};

void EnRu_Draw(Actor* thisx, PlayState* play) {
    EnRu* this = (EnRu*)thisx;
    u8* shadowTex = GRAPH_ALLOC(play->state.gfxCtx, SUBS_SHADOW_TEX_SIZE);
    u8* shadowTexIter;
    s32 i;
    TexturePtr eyeTextures[] = { gAdultRutoEyeOpenTex, gAdultRutoEyeHalfTex, gAdultRutoEyeClosedTex };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    gDPPipeSync(POLY_OPA_DISP++);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(eyeTextures[this->eyeState]));

    // only runs the last command of the display list, which is gsSPEndDisplayList
    gSPSegment(POLY_OPA_DISP++, 0x0C, &sTransparencyDlist[2]);

    POLY_OPA_DISP =
        SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                           EnRu_OverrideLimbDraw, EnRu_PostLimbDraw, &this->actor, POLY_OPA_DISP);

    Matrix_RotateXS(0, MTXMODE_NEW);

    for (i = 0, shadowTexIter = shadowTex; i < SUBS_SHADOW_TEX_SIZE; i++) {
        *shadowTexIter++ = 0;
    }

    for (i = 0; i < 5; i++) {
        SubS_GenShadowTex(this->bodyPartsPos, &this->actor.world.pos, shadowTex, i / 5.0f, RU_BODYPART_MAX,
                          sShadowSizes, sParentShadowBodyParts);
    }

    SubS_DrawShadowTex(&this->actor, &play->state, shadowTex);

    CLOSE_DISPS(play->state.gfxCtx);
}
