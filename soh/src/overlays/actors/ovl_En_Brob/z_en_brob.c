/*
 * File: z_en_brob.c
 * Overlay: ovl_En_Brob
 * Description: Flobbery Muscle Block (Jabu-Jabu's Belly)
 */

#include "z_en_brob.h"
#include "objects/object_brob/object_brob.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE)

void EnBrob_Init(Actor* thisx, PlayState* play);
void EnBrob_Destroy(Actor* thisx, PlayState* play);
void EnBrob_Update(Actor* thisx, PlayState* play);
void EnBrob_Draw(Actor* thisx, PlayState* play);

void func_809CADDC(EnBrob* this, PlayState* play);
void func_809CB054(EnBrob* this, PlayState* play);
void func_809CB114(EnBrob* this, PlayState* play);
void func_809CB218(EnBrob* this, PlayState* play);
void func_809CB2B8(EnBrob* this, PlayState* play);
void func_809CB354(EnBrob* this, PlayState* play);
void func_809CB458(EnBrob* this, PlayState* play);

const ActorInit En_Brob_InitVars = {
    ACTOR_EN_BROB,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BROB,
    sizeof(EnBrob),
    (ActorFunc)EnBrob_Init,
    (ActorFunc)EnBrob_Destroy,
    (ActorFunc)EnBrob_Update,
    (ActorFunc)EnBrob_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0xFFCFFFFF, 0x03, 0x08 },
        { 0xFFCFFFFF, 0x01, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 8000, 11000, -5000, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 60, 120, MASS_IMMOVABLE };

void EnBrob_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBrob* this = (EnBrob*)thisx;
    CollisionHeader* colHeader = NULL;

    SkelAnime_InitFlex(play, &this->skelAnime, &object_brob_Skel_0015D8, &object_brob_Anim_001750,
                       this->jointTable, this->morphTable, 10);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&object_brob_Col_001A70, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);
    Collider_InitCylinder(play, &this->colliders[0]);
    Collider_SetCylinder(play, &this->colliders[0], &this->dyna.actor, &sCylinderInit);
    Collider_InitCylinder(play, &this->colliders[1]);
    Collider_SetCylinder(play, &this->colliders[1], &this->dyna.actor, &sCylinderInit);
    CollisionCheck_SetInfo(&thisx->colChkInfo, NULL, &sColChkInfoInit);
    if (((thisx->params >> 8) & 0xFF) == 0) {
        Actor_SetScale(&this->dyna.actor, 0.01f);
        thisx->params &= 0xFF;
        if (thisx->params != 0xFF) {
            thisx->scale.y *= (thisx->params & 0xFF) * (1.0f / 30.0f);
        }
    } else {
        Actor_SetScale(&this->dyna.actor, 0.005f);
        thisx->params &= 0xFF;
        if (thisx->params != 0xFF) {
            thisx->scale.y *= (thisx->params & 0xFF) * (2.0f / 30.0f);
        }
    }
    this->colliders[0].dim.radius *= thisx->scale.x;
    this->colliders[0].dim.height = thisx->scale.y * 12000.0f;
    this->colliders[0].dim.yShift = 0;
    this->colliders[1].dim.radius *= thisx->scale.x;
    this->colliders[1].dim.height *= thisx->scale.y;
    this->colliders[1].dim.yShift *= thisx->scale.y;
    this->actionFunc = NULL;
    thisx->flags &= ~ACTOR_FLAG_TARGETABLE;
    func_809CADDC(this, play);
}

void EnBrob_Destroy(Actor* thisx, PlayState* play) {
    EnBrob* this = (EnBrob*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(play, &this->colliders[0]);
    Collider_DestroyCylinder(play, &this->colliders[1]);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void func_809CADDC(EnBrob* this, PlayState* play) {
    func_8003EC50(play, &play->colCtx.dyna, this->dyna.bgId);
    this->timer = this->actionFunc == func_809CB2B8 ? 200 : 0;
    this->unk_1AE = 0;
    this->actionFunc = func_809CB054;
}

void func_809CAE44(EnBrob* this, PlayState* play) {
    Animation_PlayOnce(&this->skelAnime, &object_brob_Anim_001750);
    func_8003EBF8(play, &play->colCtx.dyna, this->dyna.bgId);
    this->unk_1AE = 1000;
    this->actionFunc = func_809CB114;
}

void func_809CAEA0(EnBrob* this) {
    Animation_MorphToLoop(&this->skelAnime, &object_brob_Anim_001958, -5.0f);
    this->unk_1AE = 8000;
    this->timer = 1200;
    this->actionFunc = func_809CB218;
}

void func_809CAEF4(EnBrob* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_brob_Anim_000290, -5.0f);
    this->unk_1AE -= 125.0f;
    Actor_SetColorFilter(&this->dyna.actor, 0, 0xFF, 0, 0x50);
    Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->actionFunc = func_809CB2B8;
}

void func_809CAF88(EnBrob* this) {
    Animation_Change(&this->skelAnime, &object_brob_Anim_001750, -1.0f,
                     Animation_GetLastFrame(&object_brob_Anim_001750), 0.0f, ANIMMODE_ONCE, -5.0f);
    this->unk_1AE = 8250;
    this->actionFunc = func_809CB354;
}

void func_809CB008(EnBrob* this) {
    Animation_MorphToLoop(&this->skelAnime, &object_brob_Anim_001678, -5.0f);
    this->timer = 10;
    this->actionFunc = func_809CB458;
}

void func_809CB054(EnBrob* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        if (func_8004356C(&this->dyna) != 0) {
            func_8002F71C(play, &this->dyna.actor, 5.0f, this->dyna.actor.yawTowardsPlayer, 1.0f);
            func_809CAE44(this, play);
        } else if (this->dyna.actor.xzDistToPlayer < 300.0f) {
            func_809CAE44(this, play);
        }
    } else if (this->timer >= 81) {
        this->dyna.actor.colorFilterTimer = 80;
    }
}

void func_809CB114(EnBrob* this, PlayState* play) {
    f32 curFrame;

    if (SkelAnime_Update(&this->skelAnime)) {
        func_809CAEA0(this);
    } else {
        curFrame = this->skelAnime.curFrame;
        if (curFrame < 8.0f) {
            this->unk_1AE += 1000.0f;
        } else if (curFrame < 12.0f) {
            this->unk_1AE += 250.0f;
        } else {
            this->unk_1AE -= 250.0f;
        }
    }
}

void func_809CB218(EnBrob* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 6.0f) || Animation_OnFrame(&this->skelAnime, 15.0f)) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EN_BROB_WAVE);
    }
    if (this->timer != 0) {
        this->timer--;
    }
    if ((this->timer == 0) && (this->dyna.actor.xzDistToPlayer > 500.0f)) {
        func_809CAF88(this);
    }
}

void func_809CB2B8(EnBrob* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        func_809CADDC(this, play);
    } else if (this->skelAnime.curFrame < 8.0f) {
        this->unk_1AE -= 1250.0f;
    }
    this->dyna.actor.colorFilterTimer = 0x50;
}

void func_809CB354(EnBrob* this, PlayState* play) {
    f32 curFrame;

    if (SkelAnime_Update(&this->skelAnime)) {
        func_809CADDC(this, play);
    } else {
        curFrame = this->skelAnime.curFrame;
        if (curFrame < 8.0f) {
            this->unk_1AE -= 1000.0f;
        } else if (curFrame < 12.0f) {
            this->unk_1AE -= 250.0f;
        } else {
            this->unk_1AE += 250.0f;
        }
    }
}

void func_809CB458(EnBrob* this, PlayState* play) {
    Vec3f pos;
    f32 dist1;
    f32 dist2;
    s32 i;

    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0) && (this->timer != 0)) {
        this->timer--;
    }

    dist1 = play->gameplayFrames % 2 ? 0.0f : this->dyna.actor.scale.x * 5500.0f;
    dist2 = this->dyna.actor.scale.x * 5500.0f;

    for (i = 0; i < 4; i++) {
        static Color_RGBA8 primColor = { 255, 255, 255, 255 };
        static Color_RGBA8 envColor = { 200, 255, 255, 255 };

        if (i % 2) {
            pos.x = this->dyna.actor.world.pos.x + dist1;
            pos.z = this->dyna.actor.world.pos.z + dist2;
        } else {
            pos.x = this->dyna.actor.world.pos.x + dist2;
            pos.z = this->dyna.actor.world.pos.z + dist1;
            dist1 = -dist1;
            dist2 = -dist2;
        }
        pos.y = (((Rand_ZeroOne() * 15000.0f) + 1000.0f) * this->dyna.actor.scale.y) + this->dyna.actor.world.pos.y;
        EffectSsLightning_Spawn(play, &pos, &primColor, &envColor, this->dyna.actor.scale.y * 8000.0f,
                                Rand_ZeroOne() * 65536.0f, 4, 1);
    }

    if (this->timer == 0) {
        func_809CAEA0(this);
    }
}

void EnBrob_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBrob* this = (EnBrob*)thisx;
    s32 i;
    s32 acHits[2];

    acHits[0] = (this->colliders[0].base.acFlags & AC_HIT) != 0;
    acHits[1] = (this->colliders[1].base.acFlags & AC_HIT) != 0;
    if ((acHits[0] && (this->colliders[0].info.acHitInfo->toucher.dmgFlags & 0x10)) ||
        (acHits[1] && (this->colliders[1].info.acHitInfo->toucher.dmgFlags & 0x10))) {

        for (i = 0; i < 2; i++) {
            this->colliders[i].base.atFlags &= ~(AT_HIT | AT_BOUNCED);
            this->colliders[i].base.acFlags &= ~AC_HIT;
        }

        func_809CAEF4(this);
    } else if ((this->colliders[0].base.atFlags & AT_HIT) || (this->colliders[1].base.atFlags & AT_HIT) ||
               (acHits[0] && (this->colliders[0].info.acHitInfo->toucher.dmgFlags & 0x100)) ||
               (acHits[1] && (this->colliders[1].info.acHitInfo->toucher.dmgFlags & 0x100))) {

        if (this->actionFunc == func_809CB114 && !(this->colliders[0].base.atFlags & AT_BOUNCED) &&
            !(this->colliders[1].base.atFlags & AT_BOUNCED)) {
            func_8002F71C(play, &this->dyna.actor, 5.0f, this->dyna.actor.yawTowardsPlayer, 1.0f);
        } else if (this->actionFunc != func_809CB114) {
            func_809CB008(this);
        }

        for (i = 0; i < 2; i++) {
            this->colliders[i].base.atFlags &= ~(AT_HIT | AT_BOUNCED);
            this->colliders[i].base.acFlags &= ~AC_HIT;
        }
    }
    this->actionFunc(this, play);
    if (this->actionFunc != func_809CB054 && this->actionFunc != func_809CB354) {
        if (this->actionFunc != func_809CB2B8) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliders[0].base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliders[1].base);
            if (this->actionFunc != func_809CB114) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliders[0].base);
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliders[1].base);
            }
        }
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliders[0].base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliders[1].base);
    }
}

void EnBrob_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnBrob* this = (EnBrob*)thisx;
    MtxF mtx;

    Matrix_Get(&mtx);
    if (limbIndex == 3) {
        this->colliders[0].dim.pos.x = mtx.xw;
        this->colliders[0].dim.pos.y = mtx.yw;
        this->colliders[0].dim.pos.z = mtx.zw;
    } else if (limbIndex == 8) {
        this->colliders[1].dim.pos.x = mtx.xw;
        this->colliders[1].dim.pos.y = (mtx.yw + 7.0f);
        this->colliders[1].dim.pos.z = mtx.zw;
    }
}

void EnBrob_Draw(Actor* thisx, PlayState* play) {
    EnBrob* this = (EnBrob*)thisx;

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Matrix_Translate(0.0f, this->unk_1AE, 0.0f, MTXMODE_APPLY);
    SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, NULL, EnBrob_PostLimbDraw, this);
}
