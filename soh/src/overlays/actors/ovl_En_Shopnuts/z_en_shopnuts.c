#include "z_en_shopnuts.h"
#include "objects/object_shopnuts/object_shopnuts.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnShopnuts_Init(Actor* thisx, PlayState* play);
void EnShopnuts_Destroy(Actor* thisx, PlayState* play);
void EnShopnuts_Update(Actor* thisx, PlayState* play);
void EnShopnuts_Draw(Actor* thisx, PlayState* play);

void EnShopnuts_SetupWait(EnShopnuts* this);
void EnShopnuts_Wait(EnShopnuts* this, PlayState* play);
void EnShopnuts_LookAround(EnShopnuts* this, PlayState* play);
void EnShopnuts_Stand(EnShopnuts* this, PlayState* play);
void EnShopnuts_ThrowNut(EnShopnuts* this, PlayState* play);
void EnShopnuts_Burrow(EnShopnuts* this, PlayState* play);
void EnShopnuts_SpawnSalesman(EnShopnuts* this, PlayState* play);

const ActorInit En_Shopnuts_InitVars = {
    ACTOR_EN_SHOPNUTS,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_SHOPNUTS,
    sizeof(EnShopnuts),
    (ActorFunc)EnShopnuts_Init,
    (ActorFunc)EnShopnuts_Destroy,
    (ActorFunc)EnShopnuts_Update,
    (ActorFunc)EnShopnuts_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 40, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 20, 40, 0xFE };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x4E, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2600, ICHAIN_STOP),
};

void EnShopnuts_Init(Actor* thisx, PlayState* play) {
    EnShopnuts* this = (EnShopnuts*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gBusinessScrubSkel, &gBusinessScrubAnim_4574, this->jointTable,
                       this->morphTable, 18);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    Collider_UpdateCylinder(&this->actor, &this->collider);

    if (gSaveContext.n64ddFlag) {
        s16 respawnData = gSaveContext.respawn[RESPAWN_MODE_RETURN].data & ((1 << 8) - 1);
        ScrubIdentity scrubIdentity = Randomizer_IdentifyScrub(play->sceneNum, this->actor.params, respawnData);

        if (scrubIdentity.isShuffled && Flags_GetRandomizerInf(scrubIdentity.randomizerInf)) {
            Actor_Kill(&this->actor);
        }
    }

    if (((this->actor.params == 0x0002) && (gSaveContext.itemGetInf[0] & 0x800)) ||
        ((this->actor.params == 0x0009) && (gSaveContext.infTable[25] & 4)) ||
        ((this->actor.params == 0x000A) && (gSaveContext.infTable[25] & 8))) {
        Actor_Kill(&this->actor);
    } else {
        EnShopnuts_SetupWait(this);
    }
}

void EnShopnuts_Destroy(Actor* thisx, PlayState* play) {
    EnShopnuts* this = (EnShopnuts*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnShopnuts_SetupWait(EnShopnuts* this) {
    Animation_PlayOnceSetSpeed(&this->skelAnime, &gBusinessScrubAnim_139C, 0.0f);
    this->animFlagAndTimer = Rand_S16Offset(100, 50);
    this->collider.dim.height = 5;
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnShopnuts_Wait;
}

void EnShopnuts_SetupLookAround(EnShopnuts* this) {
    Animation_PlayLoop(&this->skelAnime, &gBusinessScrubLookAroundAnim);
    this->animFlagAndTimer = 2;
    this->actionFunc = EnShopnuts_LookAround;
}

void EnShopnuts_SetupThrowNut(EnShopnuts* this) {
    Animation_PlayOnce(&this->skelAnime, &gBusinessScrubAnim_1EC);
    this->actionFunc = EnShopnuts_ThrowNut;
}

void EnShopnuts_SetupStand(EnShopnuts* this) {
    Animation_MorphToLoop(&this->skelAnime, &gBusinessScrubAnim_4574, -3.0f);
    if (this->actionFunc == EnShopnuts_ThrowNut) {
        this->animFlagAndTimer = 2 | 0x1000; // sets timer and flag
    } else {
        this->animFlagAndTimer = 1;
    }
    this->actionFunc = EnShopnuts_Stand;
}

void EnShopnuts_SetupBurrow(EnShopnuts* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gBusinessScrubAnim_39C, -5.0f);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_DOWN);
    this->actionFunc = EnShopnuts_Burrow;
}

void EnShopnuts_SetupSpawnSalesman(EnShopnuts* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gBusinessScrubRotateAnim, -3.0f);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_DAMAGE);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnShopnuts_SpawnSalesman;
}

void EnShopnuts_Wait(EnShopnuts* this, PlayState* play) {
    s32 hasSlowPlaybackSpeed = false;

    if (this->skelAnime.playSpeed < 0.5f) {
        hasSlowPlaybackSpeed = true;
    }
    if (hasSlowPlaybackSpeed && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if (Animation_OnFrame(&this->skelAnime, 9.0f)) {
        this->collider.base.acFlags |= AC_ON;
    } else if (Animation_OnFrame(&this->skelAnime, 8.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_UP);
    }

    this->collider.dim.height = ((CLAMP(this->skelAnime.curFrame, 9.0f, 13.0f) - 9.0f) * 9.0f) + 5.0f;
    if (!hasSlowPlaybackSpeed && (this->actor.xzDistToPlayer < 120.0f)) {
        EnShopnuts_SetupBurrow(this);
    } else if (SkelAnime_Update(&this->skelAnime)) {
        if (this->actor.xzDistToPlayer < 120.0f) {
            EnShopnuts_SetupBurrow(this);
        } else if ((this->animFlagAndTimer == 0) && (this->actor.xzDistToPlayer > 320.0f)) {
            EnShopnuts_SetupLookAround(this);
        } else {
            EnShopnuts_SetupStand(this);
        }
    }
    if (hasSlowPlaybackSpeed &&
        ((this->actor.xzDistToPlayer > 160.0f) && (fabsf(this->actor.yDistToPlayer) < 120.0f)) &&
        ((this->animFlagAndTimer == 0) || (this->actor.xzDistToPlayer < 480.0f))) {
        this->skelAnime.playSpeed = 1.0f;
    }
}

void EnShopnuts_LookAround(EnShopnuts* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if ((this->actor.xzDistToPlayer < 120.0f) || (this->animFlagAndTimer == 0)) {
        EnShopnuts_SetupBurrow(this);
    }
}

void EnShopnuts_Stand(EnShopnuts* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if (!(this->animFlagAndTimer & 0x1000)) {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    }
    if ((this->actor.xzDistToPlayer < 120.0f) || (this->animFlagAndTimer == 0x1000)) {
        EnShopnuts_SetupBurrow(this);
    } else if (this->animFlagAndTimer == 0) {
        EnShopnuts_SetupThrowNut(this);
    }
}

void EnShopnuts_ThrowNut(EnShopnuts* this, PlayState* play) {
    Vec3f spawnPos;

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    if (this->actor.xzDistToPlayer < 120.0f) {
        EnShopnuts_SetupBurrow(this);
    } else if (SkelAnime_Update(&this->skelAnime)) {
        EnShopnuts_SetupStand(this);
    } else if (Animation_OnFrame(&this->skelAnime, 6.0f)) {
        spawnPos.x = this->actor.world.pos.x + (Math_SinS(this->actor.shape.rot.y) * 23.0f);
        spawnPos.y = this->actor.world.pos.y + 12.0f;
        spawnPos.z = this->actor.world.pos.z + (Math_CosS(this->actor.shape.rot.y) * 23.0f);
        if (Actor_Spawn(&play->actorCtx, play, ACTOR_EN_NUTSBALL, spawnPos.x, spawnPos.y, spawnPos.z,
                        this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z, 2) != NULL) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_THROW);
        }
    }
}

void EnShopnuts_Burrow(EnShopnuts* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        EnShopnuts_SetupWait(this);
    } else {
        this->collider.dim.height = ((4.0f - CLAMP_MAX(this->skelAnime.curFrame, 4.0f)) * 10.0f) + 5.0f;
    }
    if (Animation_OnFrame(&this->skelAnime, 4.0f)) {
        this->collider.base.acFlags &= ~AC_ON;
    }
}

void EnShopnuts_SpawnSalesman(EnShopnuts* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_DNS, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z,
                    this->actor.params);
        Actor_Kill(&this->actor);
    } else {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    }
}

void EnShopnuts_ColliderCheck(EnShopnuts* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.info, 1);
        EnShopnuts_SetupSpawnSalesman(this);
    } else if (play->actorCtx.unk_02 != 0) {
        EnShopnuts_SetupSpawnSalesman(this);
    }
}

void EnShopnuts_Update(Actor* thisx, PlayState* play) {
    EnShopnuts* this = (EnShopnuts*)thisx;

    EnShopnuts_ColliderCheck(this, play);
    this->actionFunc(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, this->collider.dim.radius, this->collider.dim.height, 4);
    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    if (this->actionFunc == EnShopnuts_Wait) {
        Actor_SetFocus(&this->actor, this->skelAnime.curFrame);
    } else if (this->actionFunc == EnShopnuts_Burrow) {
        Actor_SetFocus(&this->actor,
                       20.0f - ((this->skelAnime.curFrame * 20.0f) / Animation_GetLastFrame(&gBusinessScrubAnim_39C)));
    } else {
        Actor_SetFocus(&this->actor, 20.0f);
    }
}

s32 EnShopnuts_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    EnShopnuts* this = (EnShopnuts*)thisx;

    if ((limbIndex == 9) && (this->actionFunc == EnShopnuts_ThrowNut)) {
        *dList = NULL;
    }
    return 0;
}

void EnShopnuts_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnShopnuts* this = (EnShopnuts*)thisx;

    f32 curFrame;
    f32 x;
    f32 y;
    f32 z;

    if ((limbIndex == 9) && (this->actionFunc == EnShopnuts_ThrowNut)) {
        OPEN_DISPS(play->state.gfxCtx);
        curFrame = this->skelAnime.curFrame;
        if (curFrame <= 6.0f) {
            y = 1.0f - (curFrame * 0.0833f);
            x = z = (curFrame * 0.1167f) + 1.0f;
        } else if (curFrame <= 7.0f) {
            curFrame -= 6.0f;
            y = 0.5f + curFrame;
            x = z = 1.7f - (curFrame * 0.7f);
        } else if (curFrame <= 10.0f) {
            y = 1.5f - ((curFrame - 7.0f) * 0.1667f);
            x = z = 1.0f;
        } else {
            x = y = z = 1.0f;
        }

        Matrix_Scale(x, y, z, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gBusinessScrubNoseDL);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnShopnuts_Draw(Actor* thisx, PlayState* play) {
    EnShopnuts* this = (EnShopnuts*)thisx;

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnShopnuts_OverrideLimbDraw, EnShopnuts_PostLimbDraw, this);
}
