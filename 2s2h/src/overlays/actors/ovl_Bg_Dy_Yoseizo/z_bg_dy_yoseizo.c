/*
 * File: z_bg_dy_yoseizo.c
 * Overlay: ovl_Bg_Dy_Yoseizo
 * Description: Great Fairy
 */

#include "z_bg_dy_yoseizo.h"
#include "overlays/actors/ovl_Demo_Effect/z_demo_effect.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void BgDyYoseizo_Init(Actor* thisx, PlayState* play);
void BgDyYoseizo_Destroy(Actor* thisx, PlayState* play);
void BgDyYoseizo_Update(Actor* thisx, PlayState* play);
void BgDyYoseizo_Draw(Actor* thisx, PlayState* play);

void func_80A0B184(BgDyYoseizo* this, PlayState* play);
void func_80A0BB08(BgDyYoseizo* this, PlayState* play);

/* Effects functions */
void BgDyYoseizo_SpawnEffect(BgDyYoseizo* this, Vec3f* initPos, Vec3f* initVelocity, Vec3f* accel,
                             Color_RGB8* primColor, Color_RGB8* envColor, f32 scale, s16 life, s16 type);
void BgDyYoseizo_UpdateEffects(BgDyYoseizo* this, PlayState* play);
void BgDyYoseizo_DrawEffects(BgDyYoseizo* this, PlayState* play);

ActorProfile Bg_Dy_Yoseizo_Profile = {
    /**/ ACTOR_BG_DY_YOSEIZO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_DY_OBJ,
    /**/ sizeof(BgDyYoseizo),
    /**/ BgDyYoseizo_Init,
    /**/ BgDyYoseizo_Destroy,
    /**/ BgDyYoseizo_Update,
    /**/ NULL,
};

typedef enum GreatFairyAnimation {
    /* 0 */ GREATFAIRY_ANIM_START_GIVING_UPGRADE,
    /* 1 */ GREATFAIRY_ANIM_GIVING_UPGRADE,
    /* 2 */ GREATFAIRY_ANIM_SPIN_LAY_DOWN,
    /* 3 */ GREATFAIRY_ANIM_LAY_DOWN_TRANSITION,
    /* 4 */ GREATFAIRY_ANIM_LAYING_DOWN,
    /* 5 */ GREATFAIRY_ANIM_SHOWING_ITEM,
    /* 6 */ GREATFAIRY_ANIM_ARMS_FOLDED,
    /* 7 */ GREATFAIRY_ANIM_CLAPPING,
    /* 8 */ GREATFAIRY_ANIM_TEACH_SPIN_ATTACK,
    /* 9 */ GREATFAIRY_ANIM_MAX
} GreatFairyAnimation;

static AnimationHeader* sAnimations[GREATFAIRY_ANIM_MAX] = {
    &gGreatFairyStartGivingUpgradeAnim, // GREATFAIRY_ANIM_START_GIVING_UPGRADE
    &gGreatFairyGivingUpgradeAnim,      // GREATFAIRY_ANIM_GIVING_UPGRADE
    &gGreatFairySpinLayDownAnim,        // GREATFAIRY_ANIM_SPIN_LAY_DOWN
    &gGreatFairyLayDownTransitionAnim,  // GREATFAIRY_ANIM_LAY_DOWN_TRANSITION
    &gGreatFairyLayingDownAnim,         // GREATFAIRY_ANIM_LAYING_DOWN
    &gGreatFairyShowingItemAnim,        // GREATFAIRY_ANIM_SHOWING_ITEM
    &gGreatFairyArmsFoldedAnim,         // GREATFAIRY_ANIM_ARMS_FOLDED
    &gGreatFairyClappingAnim,           // GREATFAIRY_ANIM_CLAPPING
    &gGreatFairyTeachSpinAttackAnim,    // GREATFAIRY_ANIM_TEACH_SPIN_ATTACK
};

void BgDyYoseizo_Init(Actor* thisx, PlayState* play) {
    BgDyYoseizo* this = (BgDyYoseizo*)thisx;

    this->unk2EC = this->actor.world.pos.y + 40.0f;
    this->actor.focus.pos = this->actor.world.pos;

    SkelAnime_InitFlex(play, &this->skelAnime, &gGreatFairySkel, &gGreatFairyShowingItemAnim, this->jointTable,
                       this->morphTable, GREAT_FAIRY_LIMB_MAX);

    this->actionFunc = func_80A0BB08;
    Actor_SetScale(&this->actor, 0.0f);
    this->eyeIndex = 0;
    this->mouthIndex = 0;
    this->blinkTimer = 0;
    this->unk2F8 = 0;
    GREAT_FAIRY_ROTZ(&this->actor) = 1;
    this->unk302 = 0;
}

void BgDyYoseizo_Destroy(Actor* thisx, PlayState* play) {
}

// Has no visible effect since no segment is set for manually-controllable eye textures
void BgDyYoseizo_UpdateEyes(BgDyYoseizo* this) {
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    }

    if (this->blinkTimer == 0) {
        this->eyeIndex++;
        if (this->eyeIndex >= 3) {
            this->eyeIndex = 0;
            this->blinkTimer = (s32)Rand_ZeroFloat(60.0f) + 20;
        }
    }
}

void BgDyYoseizo_Bob(BgDyYoseizo* this, PlayState* play) {
    this->actor.shape.yOffset = Math_SinS(play->gameplayFrames * 1000) * 15.0f;
}

typedef enum GreatFairyEffectTrajectory {
    /* 0 */ GREAT_FAIRY_EFFECT_TRAJECTORY_RADIANT, // Dispersing particles, in the Great Fairy's signature colour.
    /* 2 */ GREAT_FAIRY_EFFECT_TRAJECTORY_FAST_RADIANT = 2,      // As above, but initially move 10 times faster.
    /* 5 */ GREAT_FAIRY_EFFECT_TRAJECTORY_CONVERGE_ON_PLAYER = 5 // Similar to OoT's healing effect, fixed colour.
} GreatFairyEffectTrajectory;

/**
 * Spawn particle effects; see `GreatFairyEffectTrajectory` enum for details
 *
 * @param trajectoryType use the `GreatFairyEffectTrajectory` enum.
 * @param count number to spawn.
 */
void BgDyYoseizo_SpawnEffects(BgDyYoseizo* this, s16 trajectoryType, s32 count) {
    static Color_RGB8 sEffectPrimColors[] = {
        { 255, 235, 220 }, // Magic
        { 255, 220, 220 }, // Power
        { 220, 255, 220 }, // Wisdom
        { 220, 220, 255 }, // Courage
        { 255, 255, 200 }, // Kindness
        { 255, 255, 170 }, // GREAT_FAIRY_EFFECT_TRAJECTORY_CONVERGE_ON_PLAYER
    };
    static Color_RGB8 sEffectEnvColors[] = {
        { 255, 150, 0 },   // Magic
        { 255, 0, 0 },     // Power
        { 0, 255, 0 },     // Wisdom
        { 0, 0, 255 },     // Courage
        { 255, 255, 0 },   // Kindness
        { 255, 100, 255 }, // GREAT_FAIRY_EFFECT_TRAJECTORY_CONVERGE_ON_PLAYER
    };
    Vec3f velocity;
    Vec3f accel;
    Vec3f pos;
    Color_RGB8 primColor;
    Color_RGB8 envColor;
    f32 spawnHeightVariation;
    f32 scale;
    s32 effectType;
    s32 life;
    s32 i;

    if (this->actor.scale.y < 0.01f) {
        return;
    }

    spawnHeightVariation = this->actor.scale.y * 3500.0f;
    accel.x = Rand_ZeroOne() - 0.5f;
    accel.y = Rand_ZeroOne() - 0.5f;
    accel.z = Rand_ZeroOne() - 0.5f;
    velocity.x = accel.x * 10.0f;
    velocity.y = accel.y * 10.0f;
    velocity.z = accel.z * 10.0f;

    for (i = 0; i < count; i++) {
        switch (trajectoryType) {
            case GREAT_FAIRY_EFFECT_TRAJECTORY_FAST_RADIANT:
                scale = 1.0f;
                life = 90;

                velocity.x = accel.x * 100.0f;
                velocity.y = accel.y * 100.0f;
                velocity.z = accel.z * 100.0f;

                effectType = GREAT_FAIRY_GET_TYPE(&this->actor);

                pos.x = this->actor.world.pos.x;
                pos.y = this->actor.world.pos.y + spawnHeightVariation +
                        (Rand_ZeroOne() - 0.5f) * (spawnHeightVariation * 0.5f);
                pos.z = this->actor.world.pos.z + 30.0f;
                break;

            case GREAT_FAIRY_EFFECT_TRAJECTORY_RADIANT:
                scale = 1.0f;
                life = 90;

                effectType = GREAT_FAIRY_GET_TYPE(&this->actor);

                pos.x = this->actor.world.pos.x;
                pos.y = this->actor.world.pos.y + spawnHeightVariation +
                        (Rand_ZeroOne() - 0.5f) * (spawnHeightVariation * 0.5f);
                pos.z = this->actor.world.pos.z + 30.0f;
                break;

            default: // all become convergent type
                scale = 0.2f;
                life = 50;

                effectType = GREAT_FAIRY_EFFECT_TRAJECTORY_CONVERGE_ON_PLAYER;

                pos.x = Rand_CenteredFloat(10.0f) + this->actor.world.pos.x;
                pos.y = this->actor.world.pos.y + spawnHeightVariation + 50.0f +
                        (Rand_ZeroOne() - 0.5f) * (spawnHeightVariation * 0.1f);
                pos.z = this->actor.world.pos.z + 30.0f;
                break;
        }

        primColor.r = sEffectPrimColors[effectType].r;
        primColor.g = sEffectPrimColors[effectType].g;
        primColor.b = sEffectPrimColors[effectType].b;
        envColor.r = sEffectEnvColors[effectType].r;
        envColor.g = sEffectEnvColors[effectType].g;
        envColor.b = sEffectEnvColors[effectType].b;

        BgDyYoseizo_SpawnEffect(this, &pos, &velocity, &accel, &primColor, &envColor, scale, life, effectType);
    }
}

void func_80A0AD50(BgDyYoseizo* this) {
    f32 scale = this->actor.scale.x;
    f32 heightTarget = this->actor.home.pos.y + 40.0f;

    Math_ApproachF(&this->actor.world.pos.y, heightTarget, this->unk2F0, 100.0f);
    Math_ApproachF(&scale, 0.035f, this->unk2F4, 0.005f);
    Math_ApproachF(&this->unk2F0, 0.8f, 0.1f, 0.02f);
    Math_ApproachF(&this->unk2F4, 0.2f, 0.03f, 0.05f);
    BgDyYoseizo_SpawnEffects(this, GREAT_FAIRY_EFFECT_TRAJECTORY_RADIANT, 2);
    Actor_SetScale(&this->actor, scale);
}

void func_80A0AE1C(BgDyYoseizo* this, PlayState* play) {
    f32 scale = this->actor.scale.x;
    f32 heightTarget = this->actor.home.pos.y;

    if (scale < 0.003f) {
        this->actionFunc = func_80A0BB08;
        Actor_SetScale(&this->actor, 0.0f);
        this->unk2F0 = 0.0f;
        this->unk2F4 = 0.0f;
        this->unk2F8 = 0;

        if (GREAT_FAIRY_GET_TYPE(&this->actor) <= GREAT_FAIRY_TYPE_COURAGE) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_EFFECT, this->actor.world.pos.x,
                        this->actor.world.pos.y + 20.0f, this->actor.world.pos.z, 0, 0, 0,
                        GREAT_FAIRY_GET_TYPE(&this->actor) + DEMO_EFFECT_TYPE_LIGHT_BASE);
        } else {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_EFFECT, this->actor.world.pos.x,
                        this->actor.world.pos.y + 20.0f, this->actor.world.pos.z, 0, 0, 0,
                        DEMO_EFFECT_TYPE_LIGHT_DARK_YELLOW);
        }
        Audio_PlaySfx(NA_SE_SY_WHITE_OUT_T);
    } else {
        Math_ApproachF(&this->actor.world.pos.y, heightTarget, this->unk2F0, 100.0f);
        Math_ApproachZeroF(&scale, this->unk2F4, 0.005f);
        Math_ApproachF(&this->unk2F0, 0.8f, 0.1f, 0.02f);
        Math_ApproachF(&this->unk2F4, 0.2f, 0.03f, 0.05f);

        this->actor.shape.rot.y += this->unk2F8;
        if (this->unk2F8 < 0x1770) {
            this->unk2F8 += 0x12C;
        }

        BgDyYoseizo_SpawnEffects(this, GREAT_FAIRY_EFFECT_TRAJECTORY_RADIANT, 2);
        Actor_SetScale(&this->actor, scale);
    }
}

void func_80A0AFDC(BgDyYoseizo* this) {
    Animation_Change(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_SPIN_LAY_DOWN], 0.0f, 46.0f,
                     Animation_GetLastFrame(sAnimations[GREATFAIRY_ANIM_SPIN_LAY_DOWN]), ANIMMODE_ONCE, 0.0f);
    this->actionFunc = func_80A0AE1C;
    Actor_PlaySfx(&this->actor, NA_SE_VO_FR_LAUGH_0);
    Actor_PlaySfx(&this->actor, NA_SE_EV_GREAT_FAIRY_VANISH);
    this->unk2F8 = 0;
    this->actor.velocity.y = 0.0f;
    this->unk2F0 = 0.0f;
    this->unk2F4 = 0.0f;
    this->actor.shape.yOffset = 0.0f;
}

void func_80A0B078(BgDyYoseizo* this, PlayState* play) {
    BgDyYoseizo_Bob(this, play);
    SkelAnime_Update(&this->skelAnime);

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103)]->id == 7)) {
        Animation_Change(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_LAYING_DOWN], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimations[GREATFAIRY_ANIM_LAYING_DOWN]), ANIMMODE_LOOP, 0.0f);
        this->actionFunc = func_80A0B184;
    } else if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103) &&
               (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103)]->id == 6)) {
        func_80A0AFDC(this);
    }
}

void func_80A0B184(BgDyYoseizo* this, PlayState* play) {
    BgDyYoseizo_Bob(this, play);
    SkelAnime_Update(&this->skelAnime);

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103)]->id == 8)) {
        Animation_Change(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_SHOWING_ITEM], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimations[GREATFAIRY_ANIM_SHOWING_ITEM]), ANIMMODE_LOOP, 0.0f);
        this->actionFunc = func_80A0B078;
    } else if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103) &&
               (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103)]->id == 6)) {
        func_80A0AFDC(this);
    }
}

void func_80A0B290(BgDyYoseizo* this, PlayState* play) {
    BgDyYoseizo_Bob(this, play);
    SkelAnime_Update(&this->skelAnime);

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103)]->id == 7)) {
        Animation_Change(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_LAYING_DOWN], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimations[GREATFAIRY_ANIM_LAYING_DOWN]), ANIMMODE_LOOP, -10.0f);
        this->actionFunc = func_80A0B184;
        this->mouthIndex = 0;
    }
}

void func_80A0B35C(BgDyYoseizo* this, PlayState* play) {
    BgDyYoseizo_Bob(this, play);
    SkelAnime_Update(&this->skelAnime);

    if (this->timer == 60) {
        if (GameInteractor_Should(VB_GIVE_ITEM_FROM_GREAT_FAIRY,
                                  !Flags_GetSwitch(play, GREAT_FAIRY_GET_SWITCHFLAG(&this->actor)), this)) {
            switch (GREAT_FAIRY_GET_TYPE(&this->actor)) {
                case GREAT_FAIRY_TYPE_MAGIC:
                    if (gSaveContext.save.saveInfo.playerData.isMagicAcquired != true) {
                        gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
                        gSaveContext.magicFillTarget = MAGIC_NORMAL_METER;
                    }
                    break;

                case GREAT_FAIRY_TYPE_WISDOM:
                    if (gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired != true) {
                        gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = true;
                        gSaveContext.magicFillTarget = MAGIC_DOUBLE_METER;
                        gSaveContext.save.saveInfo.playerData.magicLevel = 0;
                    }
                    break;

                case GREAT_FAIRY_TYPE_COURAGE:
                    if (gSaveContext.save.saveInfo.playerData.doubleDefense != true) {
                        gSaveContext.save.saveInfo.playerData.doubleDefense = true;
                    }
                    break;

                default:
                    break;
            }
        }
        Interface_SetHudVisibility(9);
    }

    if (GameInteractor_Should(VB_GREAT_FAIRY_GIVE_DOUBLE_DEFENSE_HEARTS,
                              (this->timer < 50) && (GREAT_FAIRY_GET_TYPE(&this->actor) == GREAT_FAIRY_TYPE_COURAGE),
                              this)) {
        if (gSaveContext.save.saveInfo.inventory.defenseHearts < 20) {
            gSaveContext.save.saveInfo.inventory.defenseHearts++;
        }
    }

    if (this->timer == 50) {
        gSaveContext.healthAccumulator = 0x140;
        Magic_Add(play, MAGIC_FILL_TO_CAPACITY);
    }

    if (this->timer == 0) {
        this->beam->trigger = true;
        this->actionFunc = func_80A0B290;
    }
}

void func_80A0B500(BgDyYoseizo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    BgDyYoseizo_Bob(this, play);

    if (SkelAnime_Update(&this->skelAnime)) {
        Vec3f pos;

        Animation_Change(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_GIVING_UPGRADE], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimations[GREATFAIRY_ANIM_GIVING_UPGRADE]), ANIMMODE_LOOP, 0.0f);
        this->actionFunc = func_80A0B35C;
        pos.x = player->actor.world.pos.x;
        pos.y = player->actor.world.pos.y + 200.0f;
        pos.z = player->actor.world.pos.z;
        this->beam = (EnDyExtra*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_DY_EXTRA, pos.x,
                                                    pos.y, pos.z, 0, 0, 0, GREAT_FAIRY_GET_TYPE(&this->actor));
        this->timer = 120;
    }
}

void func_80A0B5F0(BgDyYoseizo* this, PlayState* play) {
    BgDyYoseizo_Bob(this, play);

    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_Change(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_LAYING_DOWN], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimations[GREATFAIRY_ANIM_LAYING_DOWN]), ANIMMODE_LOOP, 0.0f);
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103)]->id == 5)) {
        Animation_Change(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_START_GIVING_UPGRADE], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimations[GREATFAIRY_ANIM_START_GIVING_UPGRADE]), ANIMMODE_ONCE,
                         -5.0f);
        Actor_PlaySfx(&this->actor, NA_SE_VO_FR_SMILE_0);
        this->mouthIndex = 1;
        this->eyeIndex = 0;
        this->actionFunc = func_80A0B500;
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103)]->id == 6)) {
        func_80A0AFDC(this);
    }

    BgDyYoseizo_UpdateEyes(this);
}

void func_80A0B75C(BgDyYoseizo* this, PlayState* play) {
    func_80A0AD50(this);
    SkelAnime_Update(&this->skelAnime);

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103)]->id == 4)) {
        this->actor.shape.rot.y = 0;
        this->actionFunc = func_80A0B5F0;
        Animation_Change(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_LAY_DOWN_TRANSITION], 1.0f, 2.0f,
                         Animation_GetLastFrame(sAnimations[GREATFAIRY_ANIM_LAY_DOWN_TRANSITION]), ANIMMODE_ONCE, 0.0f);
        Actor_PlaySfx(&this->actor, NA_SE_VO_FR_SMILE_0);
        this->unk2F8 = 0;
    }
}

void func_80A0B834(BgDyYoseizo* this) {
    this->actor.draw = BgDyYoseizo_Draw;
    Animation_Change(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_SPIN_LAY_DOWN], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimations[GREATFAIRY_ANIM_SPIN_LAY_DOWN]), ANIMMODE_ONCE, 0.0f);
    Actor_PlaySfx(&this->actor, NA_SE_VO_FR_LAUGH_0);
    Actor_PlaySfx(&this->actor, NA_SE_EV_GREAT_FAIRY_APPEAR);
    BgDyYoseizo_SpawnEffects(this, GREAT_FAIRY_EFFECT_TRAJECTORY_FAST_RADIANT, 30);
}

void BgDyYoseizo_TrainPlayer(BgDyYoseizo* this, PlayState* play) {
    s16 csId;
    s32 pad;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);

    csId = 0;
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103)) {
        csId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103)]->id;
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103));
    } else {
        if (GREAT_FAIRY_ROTZ(&this->actor) != 0) {
            this->actor.home.pos.x = player->actor.world.pos.x;
            this->actor.home.pos.z = player->actor.world.pos.z;
            GREAT_FAIRY_ROTZ(&this->actor) = 0;
        } else {
            player->actor.world.pos.x = this->actor.home.pos.x;
            player->actor.world.pos.z = this->actor.home.pos.z;
        }

        if (this->unk302 & 1) {
            if (this->timer == 0) {
                if (CutsceneManager_IsNext(this->actor.csId)) {
                    CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
                    this->unk302 &= ~1;
                } else {
                    CutsceneManager_Queue(this->actor.csId);
                }
            }
        } else if (!(this->unk302 & 2) && (player->meleeWeaponState != 0)) {
            if (player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) {
                if (player->unk_B08 >= 0.85f) {
                    this->unk302 |= 1;
                    this->unk302 |= 2;
                    if (play->msgCtx.currentTextId == 0x59A) {
                        Message_CloseTextbox(play);
                    }
                    this->timer = 20;
                    return;
                }
            }

            if (play->msgCtx.currentTextId != 0x59A) {
                // "Hold B and then release"
                Message_StartTextbox(play, 0x59A, &this->actor);
            }
        }
    }

    if (csId != this->csId) {
        switch (csId) {
            case 9:
                Animation_PlayLoop(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_ARMS_FOLDED]);
                break;

            case 10:
                Animation_PlayLoop(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_CLAPPING]);
                break;

            case 11:
                Animation_PlayOnce(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_TEACH_SPIN_ATTACK]);
                break;

            default:
                break;
        }

        this->csId = csId;
    }
}

// Choose behaviour?
void func_80A0BB08(BgDyYoseizo* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103)]->id == 2)) {
        func_80A0B834(this);
        this->actionFunc = func_80A0B75C;
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103)]->id == 7)) {
        this->actor.draw = BgDyYoseizo_Draw;
        Animation_PlayLoop(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_LAYING_DOWN]);
        this->actionFunc = func_80A0B184;
        this->mouthIndex = 0;
        this->actor.world.pos.y = this->actor.home.pos.y + 40.0f;
        Actor_SetScale(&this->actor, 0.035f);
        this->unk2F8 = 0;
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_103)]->id == 9)) {
        Actor_SetScale(&this->actor, 0.01f);
        Animation_PlayLoop(&this->skelAnime, sAnimations[GREATFAIRY_ANIM_ARMS_FOLDED]);
        this->csId = 9;
        this->actionFunc = BgDyYoseizo_TrainPlayer;
        this->actor.draw = BgDyYoseizo_Draw;
    }
}

void BgDyYoseizo_Update(Actor* thisx, PlayState* play) {
    BgDyYoseizo* this = (BgDyYoseizo*)thisx;

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);

    DECR(this->timer);

    BgDyYoseizo_UpdateEffects(this, play);
}

s32 BgDyYoseizo_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    BgDyYoseizo* this = (BgDyYoseizo*)thisx;

    if (limbIndex == GREAT_FAIRY_LIMB_TORSO) {
        rot->x += this->torsoRot.y;
    }

    if (limbIndex == GREAT_FAIRY_LIMB_HEAD) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.z;
    }

    return false;
}

/* Colour and shape of eyebrows, hair colour. */
typedef enum GreatFairyAppearance {
    /* 0 */ GREAT_FAIRY_APPEARANCE_MAGIC,   // Orange
    /* 1 */ GREAT_FAIRY_APPEARANCE_WISDOM,  // Green
    /* 2 */ GREAT_FAIRY_APPEARANCE_POWER,   // Pink
    /* 3 */ GREAT_FAIRY_APPEARANCE_COURAGE, // Purple
    /* 4 */ GREAT_FAIRY_APPEARANCE_KINDNESS // Yellow
} GreatFairyAppearance;

void BgDyYoseizo_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sMouthTextures[] = {
        gGreatFairyMouthClosedTex,
        gGreatFairyMouthOpenTex,
    };
    BgDyYoseizo* this = (BgDyYoseizo*)thisx;
    GreatFairyAppearance appearance = GREAT_FAIRY_APPEARANCE_MAGIC;

    // The differing eyes and hair colours
    switch (GREAT_FAIRY_GET_TYPE(&this->actor)) {
        case GREAT_FAIRY_TYPE_POWER:
            appearance = GREAT_FAIRY_APPEARANCE_POWER;
            break;

        case GREAT_FAIRY_TYPE_WISDOM:
            appearance = GREAT_FAIRY_APPEARANCE_WISDOM;
            break;

        case GREAT_FAIRY_TYPE_COURAGE:
        case GREAT_FAIRY_TYPE_KINDNESS:
            appearance = GREAT_FAIRY_GET_TYPE(&this->actor);
            break;

        default: // GREAT_FAIRY_APPEARANCE_MAGIC
            break;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    // Set eyes and hair colour, not actually animated.
    AnimatedMat_DrawStepOpa(play, Lib_SegmentedToVirtual(gGreatFairyAppearenceTexAnim), appearance);

    // Draw mouth
    {
        Gfx* gfx = POLY_OPA_DISP;
        s16 index = this->mouthIndex;
        TexturePtr mouthTex = Lib_SegmentedToVirtual(sMouthTextures[index]);

        gSPSegment(&gfx[0], 0x09, mouthTex);
        POLY_OPA_DISP = &gfx[1];
    }

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          BgDyYoseizo_OverrideLimbDraw, NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);

    BgDyYoseizo_DrawEffects(this, play);
}

/* Effects functions */

void BgDyYoseizo_SpawnEffect(BgDyYoseizo* this, Vec3f* initPos, Vec3f* initVelocity, Vec3f* accel,
                             Color_RGB8* primColor, Color_RGB8* envColor, f32 scale, s16 life, s16 type) {
    BgDyYoseizoEffect* effect = this->effects;
    s16 i;

    for (i = 0; i < BG_DY_YOSEIZO_EFFECT_COUNT; i++, effect++) {
        if (!effect->alive) {
            effect->alive = true;
            effect->pos = *initPos;
            effect->velocity = *initVelocity;
            effect->accel = *accel;
            effect->primColor = *primColor;
            effect->alpha = 0;
            effect->envColor = *envColor;
            effect->scale = scale;
            effect->timer = life;
            effect->type = type;
            effect->pitch = 0;
            effect->yaw = TRUNCF_BINANG(Rand_CenteredFloat(30000.0f));
            effect->roll = 0;
            return;
        }
    }
}

void BgDyYoseizo_UpdateEffects(BgDyYoseizo* this, PlayState* play) {
    BgDyYoseizoEffect* effect = this->effects;
    Player* player = GET_PLAYER(play);
    Vec3f sp94;
    Vec3f sp88;
    s32 pad[2];
    f32 targetPitch;
    f32 targetYaw;
    f32 floatAngle;
    s16 i = 0;

    for (i = 0; i < BG_DY_YOSEIZO_EFFECT_COUNT; i++, effect++) {
        if (effect->alive) {
            effect->roll += 3000;

            if (effect->type < GREAT_FAIRY_EFFECT_TRAJECTORY_CONVERGE_ON_PLAYER) {
                effect->pos.x += effect->velocity.x;
                effect->pos.y += effect->velocity.y;
                effect->pos.z += effect->velocity.z;
                effect->velocity.x += effect->accel.x;
                effect->velocity.y += effect->accel.y;
                effect->velocity.z += effect->accel.z;
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EV_HEALING - SFX_FLAG);

                sp94 = player->actor.world.pos;
                sp94.y = player->actor.world.pos.y - 150.0f;
                sp94.z = player->actor.world.pos.z - 50.0f;

                targetPitch = Math_Vec3f_Pitch(&effect->pos, &sp94);
                targetYaw = Math_Vec3f_Yaw(&effect->pos, &sp94);

                floatAngle = effect->pitch;
                Math_ApproachF(&floatAngle, targetPitch, 0.9f, 5000.0f);
                effect->pitch = floatAngle;

                floatAngle = effect->yaw;
                Math_ApproachF(&floatAngle, targetYaw, 0.9f, 5000.0f);
                effect->yaw = floatAngle;

                Matrix_Push();
                Matrix_RotateYS(effect->yaw, MTXMODE_NEW);
                Matrix_RotateXS(effect->pitch, MTXMODE_APPLY);

                sp94.x = sp94.y = sp94.z = 3.0f;

                Matrix_MultVec3f(&sp94, &sp88);
                Matrix_Pop();
                effect->pos.x += sp88.x;
                effect->pos.y += sp88.y;
                effect->pos.z += sp88.z;
            }
        }

        // fade up, fade down, vanish and reset
        if (effect->timer != 0) {
            effect->timer--;
            effect->alpha += 30;

            if (effect->alpha > 255) {
                effect->alpha = 255;
            }
        } else {
            effect->alpha -= 30;

            if (effect->alpha <= 0) {
                effect->alpha = 0;
                effect->alive = false;
            }
        }
    }
}

void BgDyYoseizo_DrawEffects(BgDyYoseizo* this, PlayState* play) {
    static f32 sStretchFactors[] = {
        1.0f, 1.1f, 1.15f, 1.1f, 1.0f, 0.9f, 0.85f, 0.9f,
    };
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    u8 setup = 0;
    BgDyYoseizoEffect* effect = this->effects;
    f32 stretchFactor = sStretchFactors[play->gameplayFrames % ARRAY_COUNT(sStretchFactors)];
    s16 i;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0; i < BG_DY_YOSEIZO_EFFECT_COUNT; i++, effect++) {
        if (effect->alive == true) {
            if (setup == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gGreatFairyParticleSetupDL);
                gDPPipeSync(POLY_XLU_DISP++);
                setup++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, effect->primColor.r, effect->primColor.g, effect->primColor.b,
                            effect->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, effect->envColor.r, effect->envColor.g, effect->envColor.b, 0);

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);

            Matrix_Scale(effect->scale, effect->scale * stretchFactor, 1.0f, MTXMODE_APPLY);
            Matrix_RotateZS(effect->roll, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gGreatFairyParticleDL);
        }
    }

    CLOSE_DISPS(gfxCtx);
}
