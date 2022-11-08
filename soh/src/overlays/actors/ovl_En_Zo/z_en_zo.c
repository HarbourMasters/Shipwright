/*
 * File: z_en_zo.c
 * Overlay: ovl_En_Zo
 * Description: Zora
 */

#include "z_en_zo.h"
#include "objects/object_zo/object_zo.h"

#include "soh/frame_interpolation.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

typedef enum {
    /* 0 */ ENZO_EFFECT_NONE,
    /* 1 */ ENZO_EFFECT_RIPPLE,
    /* 2 */ ENZO_EFFECT_SPLASH,
    /* 3 */ ENZO_EFFECT_BUBBLE
} EnZoEffectType;

void EnZo_Init(Actor* thisx, PlayState* play);
void EnZo_Destroy(Actor* thisx, PlayState* play);
void EnZo_Update(Actor* thisx, PlayState* play);
void EnZo_Draw(Actor* thisx, PlayState* play);

// Actions
void EnZo_Standing(EnZo* this, PlayState* play);
void EnZo_Submerged(EnZo* this, PlayState* play);
void EnZo_Surface(EnZo* this, PlayState* play);
void EnZo_TreadWater(EnZo* this, PlayState* play);
void EnZo_Dive(EnZo* this, PlayState* play);

void EnZo_Ripple(EnZo* this, Vec3f* pos, f32 scale, f32 targetScale, u8 alpha) {
    EnZoEffect* effect;
    Vec3f vec = { 0.0f, 0.0f, 0.0f };
    s16 i;

    effect = this->effects;
    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        if (effect->type == ENZO_EFFECT_NONE) {
            effect->type = ENZO_EFFECT_RIPPLE;
            effect->pos = *pos;
            effect->scale = scale;
            effect->targetScale = targetScale;
            effect->color.a = alpha;
            effect->epoch++;
            break;
        }
        effect++;
    }
}

void EnZo_Bubble(EnZo* this, Vec3f* pos) {
    EnZoEffect* effect;
    Vec3f vec = { 0.0f, 0.0f, 0.0f };
    Vec3f vel = { 0.0f, 1.0f, 0.0f };
    s16 i;
    f32 waterSurface;

    effect = this->effects;
    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        if (effect->type == ENZO_EFFECT_NONE) {
            waterSurface = this->actor.world.pos.y + this->actor.yDistToWater;
            if (!(waterSurface <= pos->y)) {
                effect->type = ENZO_EFFECT_BUBBLE;
                effect->pos = *pos;
                effect->vec = *pos;
                effect->vel = vel;
                effect->scale = ((Rand_ZeroOne() - 0.5f) * 0.02f) + 0.12f;
                effect->epoch++;
                break;
            }
        }
        effect++;
    }
}

void EnZo_Splash(EnZo* this, Vec3f* pos, Vec3f* vel, f32 scale) {
    EnZoEffect* effect;
    Vec3f accel = { 0.0f, -1.0f, 0.0f };
    s16 i;

    effect = this->effects;
    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        if (effect->type != ENZO_EFFECT_SPLASH) {
            effect->type = ENZO_EFFECT_SPLASH;
            effect->pos = *pos;
            effect->vec = accel;
            effect->vel = *vel;
            effect->color.a = (Rand_ZeroOne() * 100.0f) + 100.0f;
            effect->scale = scale;
            effect->epoch++;
            break;
        }
        effect++;
    }
}

void EnZo_UpdateRipples(EnZo* this) {
    EnZoEffect* effect = this->effects;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        if (effect->type == ENZO_EFFECT_RIPPLE) {
            Math_ApproachF(&effect->scale, effect->targetScale, 0.2f, 0.8f);
            if (effect->color.a > 20) {
                effect->color.a -= 20;
            } else {
                effect->color.a = 0;
            }

            if (effect->color.a == 0) {
                effect->type = ENZO_EFFECT_NONE;
            }
        }
        effect++;
    }
}

void EnZo_UpdateBubbles(EnZo* this) {
    EnZoEffect* effect;
    f32 waterSurface;
    s16 i;

    effect = this->effects;
    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        if (effect->type == ENZO_EFFECT_BUBBLE) {
            effect->pos.x = ((Rand_ZeroOne() * 0.5f) - 0.25f) + effect->vec.x;
            effect->pos.z = ((Rand_ZeroOne() * 0.5f) - 0.25f) + effect->vec.z;
            effect->pos.y += effect->vel.y;

            // Bubbles turn into ripples when they reach the surface
            waterSurface = this->actor.world.pos.y + this->actor.yDistToWater;
            if (waterSurface <= effect->pos.y) {
                effect->type = ENZO_EFFECT_NONE;
                effect->pos.y = waterSurface;
                EnZo_Ripple(this, &effect->pos, 0.06f, 0.12f, 200);
            }
        }
        effect++;
    }
}

void EnZo_UpdateSplashes(EnZo* this) {
    EnZoEffect* effect;
    f32 waterSurface;
    s16 i;

    effect = this->effects;
    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        if (effect->type == ENZO_EFFECT_SPLASH) {
            effect->pos.x += effect->vel.x;
            effect->pos.y += effect->vel.y;
            effect->pos.z += effect->vel.z;

            if (effect->vel.y >= -20.0f) {
                effect->vel.y += effect->vec.y;
            } else {
                effect->vel.y = -20.0f;
                effect->vec.y = 0.0f;
            }

            // Splash particles turn into ripples when they hit the surface
            waterSurface = this->actor.world.pos.y + this->actor.yDistToWater;
            if (effect->pos.y < waterSurface) {
                effect->type = ENZO_EFFECT_NONE;
                effect->pos.y = waterSurface;
                EnZo_Ripple(this, &effect->pos, 0.06f, 0.12f, 200);
            }
        }
        effect++;
    }
}

void EnZo_DrawRipples(EnZo* this, PlayState* play) {
    EnZoEffect* effect;
    s16 i;
    u8 setup;

    effect = this->effects;
    OPEN_DISPS(play->state.gfxCtx);
    setup = false;
    func_80093D84(play->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        if (effect->type == ENZO_EFFECT_RIPPLE) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!setup) {
                gDPPipeSync(POLY_XLU_DISP++);
                gSPDisplayList(POLY_XLU_DISP++, gZoraRipplesMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 155, 155, 155, 0);
                setup = true;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, effect->color.a);
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale, 1.0f, effect->scale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gZoraRipplesModelDL);
            FrameInterpolation_RecordCloseChild();
        }
        effect++;
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnZo_DrawBubbles(EnZo* this, PlayState* play) {
    EnZoEffect* effect = this->effects;
    s16 i;
    u8 setup;

    OPEN_DISPS(play->state.gfxCtx);
    setup = false;
    func_80093D84(play->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        if (effect->type == ENZO_EFFECT_BUBBLE) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!setup) {
                gSPDisplayList(POLY_XLU_DISP++, gZoraBubblesMaterialDL);
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 150, 150, 150, 0);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);

                setup = true;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gZoraBubblesModelDL);
            FrameInterpolation_RecordCloseChild();
        }
        effect++;
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnZo_DrawSplashes(EnZo* this, PlayState* play) {
    EnZoEffect* effect;
    s16 i;
    u8 setup;

    effect = this->effects;
    OPEN_DISPS(play->state.gfxCtx);
    setup = false;
    func_80093D84(play->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        if (effect->type == ENZO_EFFECT_SPLASH) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!setup) {
                gSPDisplayList(POLY_XLU_DISP++, gZoraSplashesMaterialDL);
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, 0);
                setup = true;
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 180, 180, 180, effect->color.a);

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            gSPDisplayList(POLY_XLU_DISP++, gZoraSplashesModelDL);
            FrameInterpolation_RecordCloseChild();
        }
        effect++;
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnZo_TreadWaterRipples(EnZo* this, f32 scale, f32 targetScale, u8 alpha) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + this->actor.yDistToWater;
    pos.z = this->actor.world.pos.z;
    EnZo_Ripple(this, &pos, scale, targetScale, alpha);
}

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 26, 64, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

const ActorInit En_Zo_InitVars = {
    ACTOR_EN_ZO,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ZO,
    sizeof(EnZo),
    (ActorFunc)EnZo_Init,
    (ActorFunc)EnZo_Destroy,
    (ActorFunc)EnZo_Update,
    (ActorFunc)EnZo_Draw,
    NULL,
};

typedef enum {
    /* 0 */ ENZO_ANIM_0,
    /* 1 */ ENZO_ANIM_1,
    /* 2 */ ENZO_ANIM_2,
    /* 3 */ ENZO_ANIM_3,
    /* 4 */ ENZO_ANIM_4,
    /* 5 */ ENZO_ANIM_5,
    /* 6 */ ENZO_ANIM_6,
    /* 7 */ ENZO_ANIM_7
} EnZoAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gZoraIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gZoraSurfaceAnim, 0.0f, 1.0f, 1.0f, ANIMMODE_ONCE, 0.0f },
    { &gZoraSurfaceAnim, 1.0f, 1.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraSurfaceAnim, 1.0f, 8.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraThrowRupeesAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraHandsOnHipsTappingFootAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraOpenArmsAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
};

void EnZo_SpawnSplashes(EnZo* this) {
    Vec3f pos;
    Vec3f vel;
    s32 i;

    // Convert 20 particles into splashes (all of them since there are only 15)
    for (i = 0; i < 20; i++) {
        f32 speed = Rand_ZeroOne() * 1.5f + 0.5f;
        f32 angle = Rand_ZeroOne() * 6.28f; // ~pi * 2

        vel.y = Rand_ZeroOne() * 3.0f + 3.0f;

        vel.x = sinf(angle) * speed;
        vel.z = cosf(angle) * speed;

        pos = this->actor.world.pos;
        pos.x += vel.x * 6.0f;
        pos.z += vel.z * 6.0f;
        pos.y += this->actor.yDistToWater;
        EnZo_Splash(this, &pos, &vel, 0.08f);
    }
}

u16 func_80B61024(PlayState* play, Actor* thisx) {
    u16 textId;

    textId = Text_GetFaceReaction(play, 29);
    if (textId != 0) {
        return textId;
    }

    switch (thisx->params & 0x3F) {
        case 8:
            if (gSaveContext.eventChkInf[3] & 1) {
                return 0x402A;
            }
            break;

        case 6:
            return 0x4020;

        case 7:
            return 0x4021;

        case 0:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402D;
            }
            if (gSaveContext.eventChkInf[3] & 1) {
                return 0x4007;
            }
            break;

        case 1:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402E;
            }

            if (gSaveContext.eventChkInf[3] & 1) {
                return (gSaveContext.infTable[18] & 0x10) ? 0x4009 : 0x4008;
            }
            break;

        case 2:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402D;
            }
            if (gSaveContext.eventChkInf[3] & 2) {
                return (gSaveContext.infTable[18] & 0x200) ? 0x400B : 0x402F;
            }
            if (gSaveContext.eventChkInf[3] & 1) {
                return 0x400A;
            }
            break;

        case 3:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402E;
            }
            if (gSaveContext.eventChkInf[3] & 1) {
                return 0x400C;
            }
            break;

        case 4:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402D;
            }

            if (gSaveContext.eventChkInf[3] & 8) {
                return 0x4010;
            }
            if (gSaveContext.eventChkInf[3] & 1) {
                return 0x400F;
            }
            break;

        case 5:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402E;
            }
            if (gSaveContext.eventChkInf[3] & 1) {
                return 0x4011;
            }
            break;
    }
    return 0x4006;
}

s16 func_80B61298(PlayState* play, Actor* thisx) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            return 1;

        case TEXT_STATE_CLOSING:
            switch (thisx->textId) {
                case 0x4020:
                case 0x4021:
                    return 0;
                case 0x4008:
                    gSaveContext.infTable[18] |= 0x10;
                    break;
                case 0x402F:
                    gSaveContext.infTable[18] |= 0x200;
                    break;
            }
            gSaveContext.eventChkInf[3] |= 1;
            return 0;

        case TEXT_STATE_CHOICE:
            switch (Message_ShouldAdvance(play)) {
                case 0:
                    return 1;
                default:
                    if (thisx->textId == 0x400C) {
                        thisx->textId = (play->msgCtx.choiceIndex == 0) ? 0x400D : 0x400E;
                        Message_ContinueTextbox(play, thisx->textId);
                    }
                    break;
            }
            return 1;

        case TEXT_STATE_EVENT:
            switch (Message_ShouldAdvance(play)) {
                case 0:
                    return 1;
                default:
                    return 2;
            }
    }

    return 1;
}

void EnZo_Blink(EnZo* this) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeTexture++;
        if (this->eyeTexture >= 3) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeTexture = 0;
        }
    }
}

void EnZo_Dialog(EnZo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->unk_194.unk_18 = player->actor.world.pos;
    if (this->actionFunc == EnZo_Standing) {
        // Look down at link if young, look up if old
        this->unk_194.unk_14 = !LINK_IS_ADULT ? 10.0f : -10.0f;
    } else {
        this->unk_194.unk_18.y = this->actor.world.pos.y;
    }
    func_80034A14(&this->actor, &this->unk_194, 11, this->unk_64C);
    if (this->canSpeak == true) {
        func_800343CC(play, &this->actor, &this->unk_194.unk_00, this->dialogRadius, func_80B61024, func_80B61298);
    }
}

s32 EnZo_PlayerInProximity(EnZo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f surfacePos;
    f32 yDist;
    f32 hDist;

    surfacePos.x = this->actor.world.pos.x;
    surfacePos.y = this->actor.world.pos.y + this->actor.yDistToWater;
    surfacePos.z = this->actor.world.pos.z;

    hDist = Math_Vec3f_DistXZ(&surfacePos, &player->actor.world.pos);
    yDist = fabsf(player->actor.world.pos.y - surfacePos.y);

    if (hDist < 240.0f && yDist < 80.0f) {
        return 1;
    }
    return 0;
}

void EnZo_SetAnimation(EnZo* this) {
    s32 animId = ARRAY_COUNT(sAnimationInfo);

    if (this->skelAnime.animation == &gZoraHandsOnHipsTappingFootAnim ||
        this->skelAnime.animation == &gZoraOpenArmsAnim) {
        if (this->unk_194.unk_00 == 0) {
            if (this->actionFunc == EnZo_Standing) {
                animId = ENZO_ANIM_0;
            } else {
                animId = ENZO_ANIM_3;
            }
        }
    }

    if (this->unk_194.unk_00 != 0 && this->actor.textId == 0x4006 &&
        this->skelAnime.animation != &gZoraHandsOnHipsTappingFootAnim) {
        animId = ENZO_ANIM_6;
    }

    if (this->unk_194.unk_00 != 0 && this->actor.textId == 0x4007 && this->skelAnime.animation != &gZoraOpenArmsAnim) {
        animId = ENZO_ANIM_7;
    }

    if (animId != ARRAY_COUNT(sAnimationInfo)) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, animId);
        if (animId == ENZO_ANIM_3) {
            this->skelAnime.curFrame = this->skelAnime.endFrame;
            this->skelAnime.playSpeed = 0.0f;
        }
    }
}

void EnZo_Init(Actor* thisx, PlayState* play) {
    EnZo* this = (EnZo*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gZoraSkel, NULL, this->jointTable, this->morphTable, 20);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &sColChkInit);

    if (LINK_IS_ADULT && ((this->actor.params & 0x3F) == 8)) {
        Actor_Kill(&this->actor);
        return;
    }

    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENZO_ANIM_2);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.targetMode = 6;
    this->dialogRadius = this->collider.dim.radius + 30.0f;
    this->unk_64C = 1;
    this->canSpeak = false;
    this->unk_194.unk_00 = 0;
    Actor_UpdateBgCheckInfo(play, &this->actor, this->collider.dim.height * 0.5f, this->collider.dim.radius, 0.0f,
                            5);

    if (this->actor.yDistToWater < 54.0f || (this->actor.params & 0x3F) == 8) {
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->actor.shape.shadowScale = 24.0f;
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENZO_ANIM_1);
        this->canSpeak = true;
        this->alpha = 255.0f;
        this->actionFunc = EnZo_Standing;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_0;
        this->actionFunc = EnZo_Submerged;
    }
}

void EnZo_Destroy(Actor* thisx, PlayState* play) {
}

void EnZo_Standing(EnZo* this, PlayState* play) {
    s16 angle;

    func_80034F54(play, this->unk_656, this->unk_67E, 20);
    EnZo_SetAnimation(this);
    if (this->unk_194.unk_00 != 0) {
        this->unk_64C = 4;
        return;
    }

    angle = ABS((s16)((f32)this->actor.yawTowardsPlayer - (f32)this->actor.shape.rot.y));
    if (angle < 0x4718) {
        if (EnZo_PlayerInProximity(this, play)) {
            this->unk_64C = 2;
        } else {
            this->unk_64C = 1;
        }
    } else {
        this->unk_64C = 1;
    }
}

void EnZo_Submerged(EnZo* this, PlayState* play) {
    if (EnZo_PlayerInProximity(this, play)) {
        this->actionFunc = EnZo_Surface;
        this->actor.velocity.y = 4.0f;
    }
}

void EnZo_Surface(EnZo* this, PlayState* play) {
    if (this->actor.yDistToWater < 54.0f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_OUT_OF_WATER);
        EnZo_SpawnSplashes(this);
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENZO_ANIM_3);
        this->actor.flags |= ACTOR_FLAG_0;
        this->actionFunc = EnZo_TreadWater;
        this->actor.velocity.y = 0.0f;
        this->alpha = 255.0f;
    } else if (this->actor.yDistToWater < 80.0f) {
        Math_ApproachF(&this->actor.velocity.y, 2.0f, 0.4f, 0.6f);
        Math_ApproachF(&this->alpha, 255.0f, 0.3f, 10.0f);
    }
}

void EnZo_TreadWater(EnZo* this, PlayState* play) {
    func_80034F54(play, this->unk_656, this->unk_67E, 20);
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->canSpeak = true;
        this->unk_64C = 4;
        this->skelAnime.playSpeed = 0.0f;
    }
    EnZo_SetAnimation(this);

    Math_ApproachF(&this->actor.velocity.y, this->actor.yDistToWater < 54.0f ? -0.6f : 0.6f, 0.3f, 0.2f);
    if (this->rippleTimer != 0) {
        this->rippleTimer--;
        if ((this->rippleTimer == 3) || (this->rippleTimer == 6)) {
            EnZo_TreadWaterRipples(this, 0.2f, 1.0f, 200);
        }
    } else {
        EnZo_TreadWaterRipples(this, 0.2f, 1.0f, 200);
        this->rippleTimer = 12;
    }

    if (EnZo_PlayerInProximity(this, play) != 0) {
        this->timeToDive = Rand_S16Offset(40, 40);
    } else if (DECR(this->timeToDive) == 0) {
        f32 startFrame;
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENZO_ANIM_4);
        this->canSpeak = false;
        this->unk_64C = 1;
        this->actionFunc = EnZo_Dive;
        startFrame = this->skelAnime.startFrame;
        this->skelAnime.startFrame = this->skelAnime.endFrame;
        this->skelAnime.curFrame = this->skelAnime.endFrame;
        this->skelAnime.endFrame = startFrame;
        this->skelAnime.playSpeed = -1.0f;
    }
}

void EnZo_Dive(EnZo* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_DIVE_WATER);
        EnZo_SpawnSplashes(this);
        this->actor.flags &= ~ACTOR_FLAG_0;
        this->actor.velocity.y = -4.0f;
        this->skelAnime.playSpeed = 0.0f;
    }

    if (this->skelAnime.playSpeed > 0.0f) {
        return;
    }

    if (this->actor.yDistToWater > 80.0f || this->actor.bgCheckFlags & 1) {
        Math_ApproachF(&this->actor.velocity.y, -1.0f, 0.4f, 0.6f);
        Math_ApproachF(&this->alpha, 0.0f, 0.3f, 10.0f);
    }

    if ((s16)this->alpha == 0) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENZO_ANIM_2);
        this->actor.world.pos = this->actor.home.pos;
        this->alpha = 0.0f;
        this->actionFunc = EnZo_Submerged;
    }
}

void EnZo_Update(Actor* thisx, PlayState* play) {
    EnZo* this = (EnZo*)thisx;
    u32 pad;
    Vec3f pos;

    if ((s32)this->alpha != 0) {
        SkelAnime_Update(&this->skelAnime);
        EnZo_Blink(this);
    }

    Actor_MoveForward(thisx);
    Actor_UpdateBgCheckInfo(play, thisx, this->collider.dim.radius, this->collider.dim.height * 0.25f, 0.0f, 5);
    this->actionFunc(this, play);
    EnZo_Dialog(this, play);

    // Spawn air bubbles
    if (play->state.frames & 8) {
        pos = this->actor.world.pos;

        pos.y += (Rand_ZeroOne() - 0.5f) * 10.0f + 18.0f;
        pos.x += (Rand_ZeroOne() - 0.5f) * 28.0f;
        pos.z += (Rand_ZeroOne() - 0.5f) * 28.0f;
        EnZo_Bubble(this, &pos);
    }

    if ((s32)this->alpha != 0) {
        Collider_UpdateCylinder(thisx, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }

    EnZo_UpdateRipples(this);
    EnZo_UpdateBubbles(this);
    EnZo_UpdateSplashes(this);
}

s32 EnZo_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnZo* this = (EnZo*)thisx;
    Vec3s vec;

    if (limbIndex == 15) {
        Matrix_Translate(1800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        vec = this->unk_194.unk_08;
        Matrix_RotateX((vec.y / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_RotateZ((vec.x / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_Translate(-1800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == 8) {
        vec = this->unk_194.unk_0E;
        Matrix_RotateX((-vec.y / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_RotateZ((vec.x / 32768.0f) * M_PI, MTXMODE_APPLY);
    }

    if ((limbIndex == 8) || (limbIndex == 9) || (limbIndex == 12)) {
        rot->y += (Math_SinS(this->unk_656[limbIndex]) * 200.0f);
        rot->z += (Math_CosS(this->unk_67E[limbIndex]) * 200.0f);
    }

    return 0;
}

void EnZo_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnZo* this = (EnZo*)thisx;
    Vec3f vec = { 0.0f, 600.0f, 0.0f };

    if (limbIndex == 15) {
        Matrix_MultVec3f(&vec, &this->actor.focus.pos);
    }
}

void EnZo_Draw(Actor* thisx, PlayState* play) {
    EnZo* this = (EnZo*)thisx;
    void* eyeTextures[] = { gZoraEyeOpenTex, gZoraEyeHalfTex, gZoraEyeClosedTex };

    Matrix_Push();
    EnZo_DrawRipples(this, play);
    EnZo_DrawBubbles(this, play);
    EnZo_DrawSplashes(this, play);
    Matrix_Pop();

    if ((s32)this->alpha != 0) {
        OPEN_DISPS(play->state.gfxCtx);

        if (this->alpha == 255.0f) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTexture]));
            func_80034BA0(play, &this->skelAnime, EnZo_OverrideLimbDraw, EnZo_PostLimbDraw, thisx, this->alpha);
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTexture]));
            func_80034CC4(play, &this->skelAnime, EnZo_OverrideLimbDraw, EnZo_PostLimbDraw, thisx, this->alpha);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
