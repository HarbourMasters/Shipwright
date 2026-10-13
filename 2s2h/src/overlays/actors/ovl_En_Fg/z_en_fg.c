/*
 * File: z_en_fg.c
 * Overlay: ovl_En_Fg
 * Description: Likely beta frogs, a version where they were all enemies
 */

#include "z_en_fg.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                               \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR | \
     ACTOR_FLAG_CAN_ATTACH_TO_ARROW)

void EnFg_Init(Actor* thisx, PlayState* play);
void EnFg_Destroy(Actor* thisx, PlayState* play);
void EnFg_Update(Actor* thisx, PlayState* play);
void EnFg_Draw(Actor* thisx, PlayState* play);

void EnFg_Jump(EnFg* this, PlayState* play);
void EnFg_DoNothing(EnFg* this, PlayState* play);
void EnFg_Knockback(EnFg* this, PlayState* play);
void EnFg_AddDust(BetaFrogEffectDust* dustEffect, Vec3f* pos);
void EnFg_UpdateDust(BetaFrogEffectDust* dustEffect);
void EnFg_DrawDust(PlayState* play, BetaFrogEffectDust* dustEffect);

typedef enum {
    /* 0 */ BETAFROG_DMGEFFECT_NONE,
    /* 1 */ BETAFROG_DMGEFFECT_EXPLOSION, // Bomb or bombchu, not powderkeg
    /* 2 */ BETAFROG_DMGEFFECT_DEKUSTICK,
    /* 3 */ BETAFROG_DMGEFFECT_HOOKSHOT,
    /* 4 */ BETAFROG_DMGEFFECT_ARROW,
    /* 5 */ BETAFROG_DMGEFFECT_ICEARROW
} BetaFrogDamageEffect;

ActorProfile En_Fg_Profile = {
    /**/ ACTOR_EN_FG,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_FR,
    /**/ sizeof(EnFg),
    /**/ EnFg_Init,
    /**/ EnFg_Destroy,
    /**/ EnFg_Update,
    /**/ EnFg_Draw,
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
        { 0x000010AA, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 8, 10, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit2 = {
    1, 0, 0, 0, MASS_IMMOVABLE,
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x1),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0xE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0x1),
    /* Hookshot       */ DMG_ENTRY(0, 0xF),
    /* Goron punch    */ DMG_ENTRY(0, 0x0),
    /* Sword          */ DMG_ENTRY(0, 0x0),
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(1, 0x3),
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

typedef enum {
    /* -1 */ BETAFROG_ANIM_NONE = -1,
    /*  0 */ BETAFROG_ANIM_IDLE,
    /*  1 */ BETAFROG_ANIM_IDLE_MORPH,
    /*  2 */ BETAFROG_ANIM_DANCE,
    /*  3 */ BETAFROG_ANIM_JUMP,
    /*  4 */ BETAFROG_ANIM_MAX
} BetaFrogAnimation;

static AnimationInfoS sAnimationInfo[BETAFROG_ANIM_MAX] = {
    { &gFrogIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },   // BETAFROG_ANIM_IDLE
    { &gFrogIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },  // BETAFROG_ANIM_IDLE_MORPH
    { &gFrogDanceAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // BETAFROG_ANIM_DANCE
    { &gFrogJumpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },  // BETAFROG_ANIM_JUMP
};

s32 EnFg_ChangeAnim(SkelAnime* skelAnime, s16 animIndex) {
    s16 endFrame;
    s32 didAnimChange = false;

    if ((animIndex > BETAFROG_ANIM_NONE) && (animIndex < BETAFROG_ANIM_MAX)) {
        didAnimChange = true;
        endFrame = sAnimationInfo[animIndex].frameCount;
        if (endFrame < 0) {
            endFrame = Animation_GetLastFrame(sAnimationInfo[animIndex].animation);
        }
        Animation_Change(skelAnime, sAnimationInfo[animIndex].animation, sAnimationInfo[animIndex].playSpeed,
                         sAnimationInfo[animIndex].startFrame, endFrame, sAnimationInfo[animIndex].mode,
                         sAnimationInfo[animIndex].morphFrames);
    }
    return didAnimChange;
}

void func_80A2D348(EnFg* this, PlayState* play) {
    if (this->actor.colChkInfo.health != 0) {
        this->collider.dim.pos.x = this->actor.world.pos.x;
        this->collider.dim.pos.y = this->actor.world.pos.y;
        this->collider.dim.pos.z = this->actor.world.pos.z;
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnFg_UpdateSkelAnime(EnFg* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

u8 EnFg_UpdateHealth(EnFg* this) {
    if (this->actor.colChkInfo.damage >= this->actor.colChkInfo.health) {
        this->actor.colChkInfo.health = 0;
    } else {
        this->actor.colChkInfo.health -= this->actor.colChkInfo.damage;
    }
    return this->actor.colChkInfo.health;
}

s32 EnFg_GetDamageEffect(EnFg* this) {
    s32 ret = BETAFROG_DMGEFFECT_NONE;

    if (this->collider.base.acFlags & AC_HIT) {
        switch (this->actor.colChkInfo.damageEffect) {
            case 1:
                ret = BETAFROG_DMGEFFECT_DEKUSTICK;
                break;

            case 15:
                ret = BETAFROG_DMGEFFECT_HOOKSHOT;
                break;

            case 14:
                ret = BETAFROG_DMGEFFECT_ARROW;
                break;

            case 3:
                ret = BETAFROG_DMGEFFECT_ICEARROW;
                break;

            default:
                ret = BETAFROG_DMGEFFECT_EXPLOSION;
                break;
        }
        this->collider.base.acFlags &= ~AC_HIT;
        EnFg_UpdateHealth(this);
    }
    return ret;
}

void EnFg_Idle(EnFg* this, PlayState* play) {
    Actor* ac;
    s16 rotY;
    s16 rotX;

    switch (EnFg_GetDamageEffect(this)) {
        case BETAFROG_DMGEFFECT_DEKUSTICK:
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            Actor_PlaySfx(&this->actor, NA_SE_EV_FROG_CRY_1);
            this->skelAnime.playSpeed = 0.0f;
            this->actor.shape.shadowDraw = NULL;
            this->actor.scale.x *= 1.5f;
            this->actor.scale.z *= 1.5f;
            this->actor.scale.y = 0.001f;
            this->actor.world.pos.y = this->actor.floorHeight + 2.0f;
            this->actionFunc = EnFg_DoNothing;
            break;

        case BETAFROG_DMGEFFECT_HOOKSHOT:
            break;

        case BETAFROG_DMGEFFECT_ARROW:
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->skelAnime.playSpeed = 0.0f;
            rotY = this->collider.base.ac->world.rot.y;
            rotX = this->collider.base.ac->world.rot.x;
            this->actor.scale.x = fabsf(0.01f * Math_CosS(rotY));
            this->actor.scale.y = fabsf(0.01f * Math_CosS(rotX));
            this->actor.scale.z = fabsf(0.01f * Math_SinS(rotY));
            this->actor.scale.x = CLAMP_MIN(this->actor.scale.x, 0.001f);
            this->actor.scale.y = CLAMP_MIN(this->actor.scale.y, 0.001f);
            this->actor.scale.z = CLAMP_MIN(this->actor.scale.z, 0.001f);
            this->actionFunc = EnFg_DoNothing;
            break;

        case BETAFROG_DMGEFFECT_EXPLOSION:
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            Actor_PlaySfx(&this->actor, NA_SE_EV_FROG_CRY_0);
            this->actor.params = BETAFROG_BLACK;
            this->skelAnime.playSpeed = 0.0f;
            ac = this->collider.base.ac;
            this->actor.world.rot.y = Math_Vec3f_Yaw(&ac->world.pos, &this->actor.world.pos);
            this->actor.shape.rot = this->actor.world.rot;
            this->actor.velocity.y = 10.0f;
            this->actor.speed = 3.0f;
            this->actor.gravity = -0.8f;
            this->bounceCounter = 1;
            this->timer = 0;
            this->actionFunc = EnFg_Knockback;
            break;

        default:
            if (DECR(this->timer) == 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_FROG_JUMP);
                EnFg_ChangeAnim(&this->skelAnime, BETAFROG_ANIM_JUMP);
                this->actor.velocity.y = 10.0f;
                this->timer = Rand_S16Offset(30, 30);
                this->actionFunc = EnFg_Jump;
            }
    }
    Actor_MoveWithGravity(&this->actor);
}

void EnFg_Jump(EnFg* this, PlayState* play) {
    Actor* ac;
    s32 pad;
    s16 rotY;
    s16 rotX;

    switch (EnFg_GetDamageEffect(this)) {
        case BETAFROG_DMGEFFECT_ARROW:
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->skelAnime.playSpeed = 0.0f;
            ac = this->collider.base.ac;
            rotY = ac->world.rot.y;
            rotX = ac->world.rot.x;
            this->actor.scale.x = fabsf(Math_CosS(rotY) * 0.01f);
            this->actor.scale.y = fabsf(Math_CosS(rotX) * 0.01f);
            this->actor.scale.z = fabsf(Math_SinS(rotY) * 0.01f);
            this->actor.scale.x = CLAMP_MIN(this->actor.scale.x, 0.001f);
            this->actor.scale.y = CLAMP_MIN(this->actor.scale.y, 0.001f);
            this->actor.scale.z = CLAMP_MIN(this->actor.scale.z, 0.001f);
            this->actionFunc = EnFg_DoNothing;
            break;

        case BETAFROG_DMGEFFECT_HOOKSHOT:
            break;

        case BETAFROG_DMGEFFECT_EXPLOSION:
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            Actor_PlaySfx(&this->actor, NA_SE_EV_FROG_CRY_0);
            EnFg_ChangeAnim(&this->skelAnime, BETAFROG_ANIM_IDLE);
            this->actor.params = BETAFROG_BLACK;
            this->skelAnime.playSpeed = 0.0f;
            ac = this->collider.base.ac;
            this->actor.world.rot.y = Math_Vec3f_Yaw(&ac->world.pos, &this->actor.world.pos);
            this->actor.shape.rot = this->actor.world.rot;
            this->actor.velocity.y = 10.0f;
            this->actor.speed = 3.0f;
            this->actor.gravity = -0.8f;
            this->bounceCounter = 1;
            this->timer = 0;
            this->actionFunc = EnFg_Knockback;
            break;

        default:
            if (Animation_OnFrame(&this->skelAnime, 8.0f)) {
                this->skelAnime.curFrame = 8.0f;
                this->skelAnime.playSpeed = 0.0f;
            }

            if ((this->actor.velocity.y <= 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                EnFg_ChangeAnim(&this->skelAnime, BETAFROG_ANIM_IDLE);
                this->actionFunc = EnFg_Idle;
                this->actor.velocity.y = 0.0f;
            } else {
                Actor_MoveWithGravity(&this->actor);
            }
    }
}

void EnFg_DoNothing(EnFg* this, PlayState* play) {
}

void EnFg_Knockback(EnFg* this, PlayState* play) {
    if ((this->actor.velocity.y <= 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->bounceCounter++;
        if (this->bounceCounter < 4) {
            this->actor.shape.rot.x += 0x1000;
            this->actor.velocity.y = 10.0f / this->bounceCounter;
        } else {
            this->actionFunc = EnFg_DoNothing;
        }
    } else {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            this->actor.world.rot.y = this->actor.wallYaw;
            this->actor.shape.rot = this->actor.world.rot;
        }

        if (DECR(this->timer) == 0) {
            EnFg_AddDust(&this->dustEffect[0], &this->actor.world.pos);
        }
        this->actor.shape.rot.x += 0x1000;
        Actor_MoveWithGravity(&this->actor);
    }
}

void EnFg_Init(Actor* thisx, PlayState* play) {
    EnFg* this = (EnFg*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 10.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gFrogSkel, NULL, this->jointTable, this->morphTable, FROG_LIMB_MAX);
    EnFg_ChangeAnim(&this->skelAnime, BETAFROG_ANIM_IDLE);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit2);
    this->actor.flags |= ACTOR_FLAG_CAN_ATTACH_TO_ARROW;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.gravity = -1.6f;
    this->actionFunc = EnFg_Idle;
}

void EnFg_Destroy(Actor* thisx, PlayState* play) {
    EnFg* this = (EnFg*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnFg_Update(Actor* thisx, PlayState* play) {
    EnFg* this = (EnFg*)thisx;

    if ((CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED) == 0) &&
        (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_ATTACHED_TO_ARROW) == 0)) {
        this->actionFunc(this, play);
        Actor_UpdateBgCheckInfo(play, &this->actor, sREG(0), sREG(1), 0.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    }

    EnFg_UpdateSkelAnime(this, play);
    EnFg_UpdateDust(&this->dustEffect[0]);
    func_80A2D348(this, play);
}

s32 EnFg_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnFg* this = (EnFg*)thisx;

    if ((limbIndex == FROG_LIMB_RIGHT_EYE) || (limbIndex == FROG_LIMB_LEFT_EYE)) {
        *dList = NULL;
    }

    if (this->actor.colChkInfo.health == 0) {
        if ((limbIndex == FROG_LIMB_RIGHT_IRIS) || (limbIndex == FROG_LIMB_LEFT_IRIS)) {
            *dList = NULL;
        }
    }
    return false;
}

void EnFg_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnFg* this = (EnFg*)thisx;
    s16 pad;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if ((limbIndex == FROG_LIMB_RIGHT_EYE) || (limbIndex == FROG_LIMB_LEFT_EYE)) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        Matrix_ReplaceRotation(&play->billboardMtxF);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if (limbIndex == FROG_LIMB_HEAD) {
        Matrix_MultVec3f(&zeroVec, &this->actor.focus.pos);
    }
}

void EnFg_Draw(Actor* thisx, PlayState* play) {
    EnFg* this = (EnFg*)thisx;
    s32 pad;
    Color_RGBA8 envColor[] = {
        { 200, 170, 0, 255 },   // BETAFROG_YELLOW
        { 0, 170, 200, 255 },   // BETAFROG_CYAN
        { 210, 120, 100, 255 }, // BETAFROG_PINK
        { 120, 130, 230, 255 }, // BETAFROG_BLUE
        { 190, 190, 190, 255 }, // BETAFROG_WHITE
        { 0, 0, 0, 255 },       // BETAFROG_BLACK
    };

    Matrix_Push();
    EnFg_DrawDust(play, &this->dustEffect[0]);
    Matrix_Pop();

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, envColor[this->actor.params].r, envColor[this->actor.params].g,
                   envColor[this->actor.params].b, envColor[this->actor.params].a);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(gFrogIrisOpenTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(gFrogIrisOpenTex));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnFg_OverrideLimbDraw, EnFg_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFg_AddDust(BetaFrogEffectDust* dustEffect, Vec3f* pos) {
    Vec3f velocity = { 0.0f, 3.0f, 0.0f };
    Vec3f unk_20 = { 0.0f, 0.0f, 0.0f };
    s32 i;

    for (i = 0; i < 10; i++, dustEffect++) {
        if (!dustEffect->type) {
            dustEffect->type = true;
            dustEffect->timer = 16;
            dustEffect->pos = *pos;
            dustEffect->velocity = velocity;
            dustEffect->unk_20 = unk_20;
            dustEffect->xyScale = 0.4f;
            break;
        }
    }
}

void EnFg_UpdateDust(BetaFrogEffectDust* dustEffect) {
    s32 i;

    for (i = 0; i < 10; i++, dustEffect++) {
        if (dustEffect->type == true) {
            if (DECR(dustEffect->timer) == 0) {
                dustEffect->type = false;
            }
            dustEffect->pos.y += dustEffect->velocity.y;
        }
    }
}

static TexturePtr sDustTextures[] = {
    gEffDust8Tex, gEffDust7Tex, gEffDust6Tex, gEffDust5Tex, gEffDust4Tex, gEffDust3Tex, gEffDust2Tex, gEffDust1Tex,
};

void EnFg_DrawDust(PlayState* play, BetaFrogEffectDust* dustEffect) {
    s16 i;
    s16 alpha;
    s16 index;
    s16 firstDone = false;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0; i < 10; i++, dustEffect++) {
        if (dustEffect->type == 0) {
            continue;
        }

        if (!firstDone) {
            POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_0);
            gSPDisplayList(POLY_XLU_DISP++, gFrogDustMaterialDL);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 0);
            firstDone = true;
        }

        alpha = (255.0f / 16.0f) * dustEffect->timer;
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, alpha);
        gDPPipeSync(POLY_XLU_DISP++);
        Matrix_Translate(dustEffect->pos.x, dustEffect->pos.y, dustEffect->pos.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(dustEffect->xyScale, dustEffect->xyScale, 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        index = 0.5f * dustEffect->timer;
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(sDustTextures[index]));
        gSPDisplayList(POLY_XLU_DISP++, gFrogDustModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
