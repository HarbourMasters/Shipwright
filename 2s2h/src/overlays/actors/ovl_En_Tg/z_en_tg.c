/*
 * File: z_en_tg.c
 * Overlay: ovl_En_Tg
 * Description: Target Game (Honey & Darling) - End Credits Scene
 */

#include "z_en_tg.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnTg_Init(Actor* thisx, PlayState* play);
void EnTg_Destroy(Actor* thisx, PlayState* play);
void EnTg_Update(Actor* thisx, PlayState* play);
void EnTg_Draw(Actor* thisx, PlayState* play);

void EnTg_Idle(EnTg* this, PlayState* play);
void EnTg_UpdateHearts(PlayState* play, EnTgHeartEffect* effect, s32 numEffects);
void EnTg_DrawHearts(PlayState* play, EnTgHeartEffect* effect, s32 numEffects);
void EnTg_SpawnHeart(EnTg* this, EnTgHeartEffect* effect, Vec3f* heartStartPos, s32 numEffects);

ActorProfile En_Tg_Profile = {
    /**/ ACTOR_EN_TG,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MU,
    /**/ sizeof(EnTg),
    /**/ EnTg_Init,
    /**/ EnTg_Destroy,
    /**/ EnTg_Update,
    /**/ EnTg_Draw,
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

typedef enum EnTgAnimation {
    /* 0 */ ENTG_ANIM_IDLE,
    /* 1 */ ENTG_ANIM_MAX
} EnTgAnimation;

static AnimationInfoS sAnimationInfo[ENTG_ANIM_MAX] = {
    { &gHoneyAndDarlingIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // ENTG_ANIM_IDLE
};

void EnTg_ChangeAnim(SkelAnime* skelAnime, AnimationInfoS* animationInfo, s16 animIndex) {
    f32 endFrame;

    animationInfo += animIndex;

    if (animationInfo->frameCount < 0) {
        endFrame = Animation_GetLastFrame(animationInfo->animation);
    } else {
        endFrame = animationInfo->frameCount;
    }
    Animation_Change(skelAnime, animationInfo->animation, animationInfo->playSpeed, animationInfo->startFrame, endFrame,
                     animationInfo->mode, animationInfo->morphFrames);
}

void EnTg_UpdateCollider(EnTg* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnTg_UpdateSkelAnime(EnTg* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnTg_Init(Actor* thisx, PlayState* play) {
    EnTg* this = (EnTg*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gHoneyAndDarlingSkel, NULL, this->jointTable, this->morphTable,
                       HONEY_AND_DARLING_LIMB_MAX);
    EnTg_ChangeAnim(&this->skelAnime, sAnimationInfo, ENTG_ANIM_IDLE);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = EnTg_Idle;
    this->actor.gravity = -4.0f;
}

void EnTg_Destroy(Actor* thisx, PlayState* play) {
    EnTg* this = (EnTg*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

/**
 * The actor spins, and a heart is spawned above the actor every 12 frames.
 */
void EnTg_Idle(EnTg* this, PlayState* play) {
    Vec3f heartStartPos;

    this->actor.shape.rot.y += sREG(0) + 0x258;
    this->actor.world.rot = this->actor.shape.rot;

    if (DECR(this->spawnHeartTimer) == 0) {
        this->spawnHeartTimer = 12;
        heartStartPos = this->actor.world.pos;
        heartStartPos.y += 62.0f;
        EnTg_SpawnHeart(this, this->effects, &heartStartPos, ARRAY_COUNT(this->effects));
    }
}

void EnTg_Update(Actor* thisx, PlayState* play) {
    EnTg* this = (EnTg*)thisx;

    this->actionFunc(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    EnTg_UpdateSkelAnime(this, play);
    EnTg_UpdateHearts(play, this->effects, ARRAY_COUNT(this->effects));
    EnTg_UpdateCollider(this, play);
}

s32 EnTg_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return false;
}

void EnTg_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnTg* this = (EnTg*)thisx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == HONEY_AND_DARLING_LIMB_MAN_HEAD) {
        Matrix_MultVec3f(&zeroVec, &this->actor.focus.pos);
    }
}

void EnTg_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTg* this = (EnTg*)thisx;

    Matrix_Push();
    EnTg_DrawHearts(play, this->effects, ARRAY_COUNT(this->effects));
    Matrix_Pop();

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gSPSegment(POLY_OPA_DISP++, 0x08, Gfx_EnvColor(play->state.gfxCtx, 0, 50, 160, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, Gfx_EnvColor(play->state.gfxCtx, 255, 255, 255, 0));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnTg_OverrideLimbDraw, EnTg_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Spawns a heart at the first effects array index that's not enabled.
 * Because of the frame counts, only two hearts are ever spawned at a time.
 */
void EnTg_SpawnHeart(EnTg* this, EnTgHeartEffect* effect, Vec3f* heartStartPos, s32 numEffects) {
    Vec3f heartVelocity = { 0.0f, 1.5f, 0.0f };
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    s32 i;

    for (i = 0; i < numEffects && effect->isEnabled; i++, effect++) {}

    if (i < numEffects) {
        effect->isEnabled = true;
        effect->pos = *heartStartPos;
        effect->velocity = heartVelocity;
        effect->unusedZeroVec = zeroVec;
        effect->scale = 0.01f;
        effect->pos.x += 4.0f * Math_SinS(this->actor.shape.rot.y);
        effect->pos.z += 4.0f * Math_CosS(this->actor.shape.rot.y);
        effect->timer = 16;
    }
}

/**
 * The heart path is curvy as it floats up because of the use of Math_SinS and Math_CosS.
 */
void EnTg_UpdateHearts(PlayState* play, EnTgHeartEffect* effect, s32 numEffects) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    s16 yaw = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));
    s32 i;

    for (i = 0; i < numEffects; i++, effect++) {
        if (effect->isEnabled == true) {
            if (DECR(effect->timer) == 0) {
                effect->isEnabled = false;
            }
            effect->pos.y += effect->velocity.y;
            effect->pos.x += 2.0f * Math_SinS(effect->angle);
            effect->pos.z += 2.0f * Math_CosS(effect->angle);

            Matrix_Push();
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateYS(yaw, MTXMODE_APPLY);
            Matrix_MultVec3f(&zeroVec, &effect->pos);
            Matrix_Pop();

            effect->angle += 0x1770;
        }
    }
}

void EnTg_DrawHearts(PlayState* play, EnTgHeartEffect* effect, s32 numEffects) {
    s32 i;
    s32 isMaterialApplied = false;

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    POLY_OPA_DISP = Gfx_SetupDL66(POLY_OPA_DISP);

    for (i = 0; i < numEffects; i++, effect++) {
        if (effect->isEnabled == true) {
            if (!isMaterialApplied) {
                gSPDisplayList(POLY_OPA_DISP++, gHoneyAndDarlingHeartMaterialDL);
                isMaterialApplied = true;
            }
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);

            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(gDropRecoveryHeartTex));
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gHoneyAndDarlingHeartModelDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
