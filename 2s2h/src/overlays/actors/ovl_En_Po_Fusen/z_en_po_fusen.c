/*
 * File: z_en_po_fusen.c
 * Overlay: ovl_En_Po_Fusen
 * Description: Romani Ranch - Poe Balloon
 */

#include "z_en_po_fusen.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_En_Ma4/z_en_ma4.h"

#define FLAGS                                                                                              \
    (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_FREEZE_EXCEPTION | \
     ACTOR_FLAG_MINIMAP_ICON_ENABLED)

void EnPoFusen_Init(Actor* thisx, PlayState* play);
void EnPoFusen_Destroy(Actor* thisx, PlayState* play);
void EnPoFusen_Update(Actor* thisx, PlayState* play);
void EnPoFusen_Draw(Actor* thisx, PlayState* play);

s32 EnPoFusen_CheckParent(EnPoFusen* this, PlayState* play);
void EnPoFusen_InitNoFuse(EnPoFusen* this);
void EnPoFusen_InitFuse(EnPoFusen* this);
void EnPoFusen_Pop(EnPoFusen* this, PlayState* play);
void EnPoFusen_Idle(EnPoFusen* this, PlayState* play);
void EnPoFusen_IdleFuse(EnPoFusen* this, PlayState* play);

ActorProfile En_Po_Fusen_Profile = {
    /**/ ACTOR_EN_PO_FUSEN,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_PO_FUSEN,
    /**/ sizeof(EnPoFusen),
    /**/ EnPoFusen_Init,
    /**/ EnPoFusen_Destroy,
    /**/ EnPoFusen_Update,
    /**/ EnPoFusen_Draw,
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 200 }, 100 },
};

typedef enum {
    /* 0x0 */ POE_BALLOON_DMGEFF_NONE,
    /* 0xF */ POE_BALLOON_DMGEFF_POP = 0xF
} PoeBalloonDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Deku Stick     */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, POE_BALLOON_DMGEFF_POP),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Goron punch    */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(1, POE_BALLOON_DMGEFF_POP),
    /* Ice arrow      */ DMG_ENTRY(1, POE_BALLOON_DMGEFF_POP),
    /* Light arrow    */ DMG_ENTRY(1, POE_BALLOON_DMGEFF_POP),
    /* Goron spikes   */ DMG_ENTRY(1, POE_BALLOON_DMGEFF_POP),
    /* Deku spin      */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Zora barrier   */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Normal shield  */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(0, POE_BALLOON_DMGEFF_NONE),
};

void EnPoFusen_Init(Actor* thisx, PlayState* play) {
    EnPoFusen* this = (EnPoFusen*)thisx;
    f32 flyingHeightMin;

    this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 0.007f;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.colChkInfo.damageTable = &sDamageTable;

    Collider_InitSphere(play, &this->collider);
    Collider_SetSphere(play, &this->collider, &this->actor, &sSphereInit);

    this->collider.dim.worldSphere.radius = 40;
    SkelAnime_InitFlex(play, &this->anime, &gPoeBalloonSkel, &gPoeBalloonEmptyAnim, this->jointTable, this->morphTable,
                       POE_BALLOON_LIMB_MAX);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    if (!EnPoFusen_CheckParent(this, play)) {
        Actor_Kill(&this->actor);
    }

    flyingHeightMin = this->actor.floorHeight + 90.0f;
    if (this->actor.home.pos.y < flyingHeightMin) {
        this->actor.home.pos.y = flyingHeightMin;
    }

    this->randScaleChange = (Rand_Next() % 0xFFFE) - 0x7FFF;
    this->randYRotChange = (Rand_Next() % 0x4B0) - 0x258;
    this->avgBaseRotation = 0x10000 / 12;
    this->limbRotYRightUpperArm = 0;
    this->limb46Rot = 0;
    this->limb57Rot = 0;
    this->limbRotLeftHand = 0;
    this->limbRotChainAndLantern = 0x71C;
    this->randBaseRotChange = 0;

    if (POE_BALLOON_IS_FUSE_TYPE(&this->actor)) {
        EnPoFusen_InitFuse(this);
    } else {
        EnPoFusen_InitNoFuse(this);
    }
}

void EnPoFusen_Destroy(Actor* thisx, PlayState* play) {
    EnPoFusen* this = (EnPoFusen*)thisx;

    Collider_DestroySphere(play, &this->collider);
}

/**
 * Search for Romani's actor, beacuse it's PoFusen's job to update her actor when the balloon is popped.
 */
s32 EnPoFusen_CheckParent(EnPoFusen* this, PlayState* play) {
    Actor* actorIter = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    if (POE_BALLOON_IS_FUSE_TYPE(&this->actor)) {
        return true;
    }

    while (actorIter != NULL) {
        if (actorIter->id == ACTOR_EN_MA4) {
            this->actor.parent = actorIter;
            return true;
        }
        actorIter = actorIter->next;
    }

    return false;
}

s32 EnPoFusen_CheckCollision(EnPoFusen* this, PlayState* play) {
    if (this->actionFunc == EnPoFusen_IdleFuse) {
        return false;
    }

    this->collider.dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.dim.worldSphere.center.y = this->actor.world.pos.y + 20.0f;
    this->collider.dim.worldSphere.center.z = this->actor.world.pos.z;

    if ((this->collider.base.acFlags & AC_HIT) && (this->actor.colChkInfo.damageEffect == POE_BALLOON_DMGEFF_POP)) {
        this->collider.base.acFlags &= ~AC_HIT;
        return true;
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

    return false;
}

void EnPoFusen_InitNoFuse(EnPoFusen* this) {
    this->actor.shape.rot.z = 0;
    this->actionFunc = EnPoFusen_Idle;
}

void EnPoFusen_Idle(EnPoFusen* this, PlayState* play) {
    f32 shadowScaleTmp;
    f32 shadowAlphaTmp;
    f32 heightOffset;

    this->actor.world.pos = this->actor.home.pos;
    this->randScaleChange += 0x190;
    this->randXZRotChange += 0x5DC;
    this->randBaseRotChange += 0x9C4;
    heightOffset = Math_SinS(this->randScaleChange);
    heightOffset = 50.0f * heightOffset;
    this->actor.shape.rot.y += this->randYRotChange;
    this->actor.world.pos.y += heightOffset;
    this->actor.shape.rot.z = (Math_SinS(this->randBaseRotChange) * 910.0f);

    if ((this->randScaleChange < 0x4000) && (this->randScaleChange > -0x4000)) {
        Math_SmoothStepToS(&this->limbRotChainAndLantern, 0x38E, 0x14, 0xBB8, 0x64);
    } else {
        Math_SmoothStepToS(&this->limbRotChainAndLantern, 0x71C, 0x8, 0xBB8, 0x64);
    }

    this->avgBaseRotation = this->limbRotChainAndLantern * 3;
    this->limbRotYRightUpperArm = (Math_SinS(this->randBaseRotChange + 0x38E3) * this->avgBaseRotation);
    this->limb46Rot = (Math_SinS(this->randBaseRotChange) * this->avgBaseRotation);
    this->limb57Rot = (Math_SinS(this->randBaseRotChange - 0x38E3) * this->avgBaseRotation);
    this->limbRotLeftHand = (Math_SinS(this->randBaseRotChange - 0x71C6) * this->avgBaseRotation);

    shadowScaleTmp = ((1.0f - Math_SinS(this->randScaleChange)) * 10.0f) + 50.0f;
    shadowAlphaTmp = ((1.0f - Math_SinS(this->randScaleChange)) * 75.0f) + 100.0f;
    this->actor.shape.shadowScale = shadowScaleTmp;
    this->actor.shape.shadowAlpha = CLAMP_MAX(shadowAlphaTmp, 255.0f);
}

void EnPoFusen_IncrementRomaniPop(EnPoFusen* this) {
    Actor* parent = this->actor.parent;

    if ((parent != NULL) && (parent->id == ACTOR_EN_MA4)) {
        EnMa4* romani = (EnMa4*)parent;

        romani->poppedBalloonCounter++;
    }

    this->actor.draw = NULL;
    this->actionFunc = EnPoFusen_Pop;
}

void EnPoFusen_Pop(EnPoFusen* this, PlayState* play) {
    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x, this->actor.world.pos.y + 20.0f,
                this->actor.world.pos.z, 255, 255, 200, CLEAR_TAG_PARAMS(CLEAR_TAG_POP));
    Actor_PlaySfx(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
    Actor_Kill(&this->actor);
}

void EnPoFusen_InitFuse(EnPoFusen* this) {
    s16 rotZ = this->actor.shape.rot.z;

    this->fuse = POE_BALLOON_GET_FUSE_LEN(&this->actor);
    this->actor.shape.rot.z = 0;
    this->randScaleChange = rotZ & 0xFFFF;
    this->actionFunc = EnPoFusen_IdleFuse;
}

void EnPoFusen_IdleFuse(EnPoFusen* this, PlayState* play) {
    EnPoFusen_Idle(this, play);
    if (this->fuse-- == 0) {
        EnPoFusen_IncrementRomaniPop(this);
    }
}

void EnPoFusen_Update(Actor* thisx, PlayState* play) {
    EnPoFusen* this = (EnPoFusen*)thisx;

    this->actionFunc(this, play);
    if (EnPoFusen_CheckCollision(this, play)) {
        EnPoFusen_IncrementRomaniPop(this);
    }
}

s32 EnPoFusen_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnPoFusen* this = (EnPoFusen*)thisx;

    if (limbIndex == POE_BALLOON_LIMB_BODY) {
        f32 zScale = (Math_CosS(this->randScaleChange) * 0.08f) + 1.0f;
        f32 yScale;
        f32 xScale = zScale;
        s16 pad;
        s16 zRot;
        s16 pad2;
        s16 xRot;

        yScale = (Math_SinS(this->randScaleChange) * 0.08f) + 1.0f;
        yScale = SQ(yScale);
        xRot = ((Math_SinS(this->randXZRotChange) * 2730.0f));
        zRot = ((Math_CosS(this->randXZRotChange) * 2730.0f));
        Matrix_RotateZYX(xRot, 0, zRot, MTXMODE_APPLY);
        Matrix_Scale(xScale, yScale, zScale, MTXMODE_APPLY);
        Matrix_RotateZS(-zRot, MTXMODE_APPLY);
        Matrix_RotateXS(-xRot, MTXMODE_APPLY);

    } else if (limbIndex == POE_BALLOON_RIGHT_UPPER_ARM) {
        rot->y += this->limbRotYRightUpperArm;
    } else if (limbIndex == POE_BALLOON_LEFT_UPPER_ARM) {
        rot->y += this->limb46Rot;
    } else if (limbIndex == POE_BALLOON_RIGHT_FOREARM) {
        rot->z += this->limb46Rot;
    } else if ((limbIndex == POE_BALLOON_RIGHT_HAND) || (limbIndex == POE_BALLOON_LEFT_FOREARM)) {
        rot->z += this->limb57Rot;
    } else if (limbIndex == POE_BALLOON_LEFT_HAND) {
        rot->z += this->limbRotLeftHand;
    } else if (limbIndex == POE_BALLOON_LIMB_CHAIN_AND_LANTERN) {
        rot->y += TRUNCF_BINANG(this->limbRotChainAndLantern * Math_SinS(this->randBaseRotChange));
        rot->z += TRUNCF_BINANG(this->limbRotChainAndLantern * Math_CosS(this->randBaseRotChange));
    }
    return false;
}

void EnPoFusen_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
}

void EnPoFusen_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
}

void EnPoFusen_Draw(Actor* thisx, PlayState* play) {
    EnPoFusen* this = (EnPoFusen*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawTransformFlexOpa(play, this->anime.skeleton, this->anime.jointTable, this->anime.dListCount,
                                   EnPoFusen_OverrideLimbDraw, EnPoFusen_PostLimbDraw, EnPoFusen_TransformLimbDraw,
                                   &this->actor);
}
