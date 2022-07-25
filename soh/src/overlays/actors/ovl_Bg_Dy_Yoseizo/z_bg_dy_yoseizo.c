/*
 * File: z_bg_dy_yoseizo.c
 * Overlay: ovl_Bg_Dy_Yoseizo
 * Description: Great Fairy
 */

#include "z_bg_dy_yoseizo.h"
#include "objects/object_dy_obj/object_dy_obj.h"
#include "vt.h"
#include "overlays/actors/ovl_Demo_Effect/z_demo_effect.h"
#include "scenes/indoors/yousei_izumi_yoko/yousei_izumi_yoko_scene.h"
#include "scenes/indoors/daiyousei_izumi/daiyousei_izumi_scene.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_25)

typedef enum {
    /* 0 */ FAIRY_UPGRADE_MAGIC,
    /* 1 */ FAIRY_UPGRADE_DOUBLE_MAGIC,
    /* 2 */ FAIRY_UPGRADE_HALF_DAMAGE
} BgDyYoseizoRewardType;

typedef enum {
    /* 0 */ FAIRY_SPELL_FARORES_WIND,
    /* 1 */ FAIRY_SPELL_DINS_FIRE,
    /* 2 */ FAIRY_SPELL_NAYRUS_LOVE
} BgDyYoseizoSpellType;

void BgDyYoseizo_Init(Actor* thisx, GlobalContext* globalCtx);
void BgDyYoseizo_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgDyYoseizo_Update(Actor* thisx, GlobalContext* globalCtx);
void BgDyYoseizo_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgDyYoseizo_CheckMagicAcquired(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_ChooseType(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_SetupSpinGrow_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_SpinGrow_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_CompleteSpinGrow_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_SetupGreetPlayer_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_GreetPlayer_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_SetupHealPlayer_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_HealPlayer_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_SayFarewell_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_SetupSpinShrink(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_SpinShrink(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_Vanish(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_SetupSpinGrow_Reward(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_SpinGrowSetupGive_Reward(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_Give_Reward(BgDyYoseizo* this, GlobalContext* globalCtx);

void BgDyYoseizo_ParticleInit(BgDyYoseizo* this, Vec3f* initPos, Vec3f* initVelocity, Vec3f* accel,
                              Color_RGB8* primColor, Color_RGB8* envColor, f32 scale, s16 life, s16 type);
void BgDyYoseizo_ParticleUpdate(BgDyYoseizo* this, GlobalContext* globalCtx);
void BgDyYoseizo_ParticleDraw(BgDyYoseizo* this, GlobalContext* globalCtx);

static s32 sUnusedGetItemIds[] = { GI_FARORES_WIND, GI_NAYRUS_LOVE, GI_DINS_FIRE };

const ActorInit Bg_Dy_Yoseizo_InitVars = {
    ACTOR_BG_DY_YOSEIZO,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_DY_OBJ,
    sizeof(BgDyYoseizo),
    (ActorFunc)BgDyYoseizo_Init,
    (ActorFunc)BgDyYoseizo_Destroy,
    (ActorFunc)BgDyYoseizo_Update,
    NULL,
    NULL,
};

void GivePlayerRandoRewardGreatFairy(BgDyYoseizo* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    GetItemID getItemId = Randomizer_GetRandomizedItemId(GI_NONE, this->actor.id, this->fountainType + 1, globalCtx->sceneNum);

    if (this->actor.parent == GET_PLAYER(globalCtx) && !Flags_GetTreasure(globalCtx, this->fountainType + 1) &&
        !Player_InBlockingCsMode(globalCtx, GET_PLAYER(globalCtx))) {
        Flags_SetTreasure(globalCtx, this->fountainType + 1);
        Actor_Kill(&this->actor);
    } else if (!Flags_GetTreasure(globalCtx, this->fountainType + 1)) {
        func_8002F434(&this->actor, globalCtx, getItemId, 10000.0f, 100.0f);
    }
}

void BgDyYoseizo_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BgDyYoseizo* this = (BgDyYoseizo*)thisx;

    this->fountainType = globalCtx->curSpawn;

    if (this->fountainType < 0) {
        this->fountainType = 0;
    }

    this->vanishHeight = this->actor.world.pos.y;
    this->grownHeight = this->vanishHeight + 40.0f;
    this->actor.focus.pos = this->actor.world.pos;

    if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
        // "Great Fairy Fountain"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 大妖精の泉 ☆☆☆☆☆ %d\n" VT_RST, globalCtx->curSpawn);
        SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gGreatFairySkel, &gGreatFairySittingTransitionAnim,
                           this->jointTable, this->morphTable, 28);
    } else {
        // "Stone/Jewel Fairy Fountain"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 石妖精の泉 ☆☆☆☆☆ %d\n" VT_RST, globalCtx->curSpawn);
        SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gGreatFairySkel, &gGreatFairyLayingDownTransitionAnim,
                           this->jointTable, this->morphTable, 28);
    }
    this->actionFunc = BgDyYoseizo_CheckMagicAcquired;
}

void BgDyYoseizo_Destroy(Actor* this, GlobalContext* globalCtx) {
}

static Color_RGB8 sParticlePrimColors[] = {
    { 255, 255, 255 }, { 255, 255, 100 }, { 100, 255, 100 }, { 255, 100, 100 }, { 255, 255, 170 },
    { 255, 255, 100 }, { 100, 255, 100 }, { 255, 100, 100 }, { 255, 255, 170 },
};

static Color_RGB8 sParticleEnvColors[] = {
    { 155, 255, 255 }, { 255, 255, 100 }, { 100, 255, 100 }, { 255, 100, 100 }, { 255, 100, 255 },
    { 255, 255, 100 }, { 100, 255, 100 }, { 255, 100, 100 }, { 100, 255, 255 },
};

void BgDyYoseizo_SpawnParticles(BgDyYoseizo* this, GlobalContext* globalCtx, s16 type) {
    Vec3f particleInitVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f particleAccel;
    Vec3f particleInitPos;
    Color_RGB8 particlePrimColor;
    Color_RGB8 particleEnvColor;
    f32 spawnPosVariation;
    s32 particleType;
    f32 particleScale;
    s32 i;
    s16 particleLife;

    if (!(this->scale < 0.01f)) {
        spawnPosVariation = this->scale * 3500.0f;
        particleAccel.x = Rand_ZeroOne() - 0.5f;
        particleAccel.y = Rand_ZeroOne() - 0.5f;
        particleAccel.z = Rand_ZeroOne() - 0.5f;
        for (i = 0; i < 2; i++) {
            if (type == 0) {
                particleType = 0;
                particleScale = 0.4f;
                particleLife = 90;
                particleInitPos.x = this->actor.world.pos.x;
                particleInitPos.y = this->actor.world.pos.y + spawnPosVariation +
                                    ((Rand_ZeroOne() - 0.5f) * (spawnPosVariation * 0.5f));
                particleInitPos.z = this->actor.world.pos.z + 30.0f;
            } else {
                particleLife = 50;
                particleType = type;
                particleScale = 0.2f;
                particleInitPos.x = this->actor.world.pos.x + Rand_CenteredFloat(10.0f);

                if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
                    particleInitPos.y = this->actor.world.pos.y + spawnPosVariation + 50.0f +
                                        ((Rand_ZeroOne() - 0.5f) * (spawnPosVariation * 0.1f));
                    particleInitPos.z = this->actor.world.pos.z + 30.0f;
                } else {
                    particleInitPos.y = this->actor.world.pos.y + spawnPosVariation - 30.0f +
                                        ((Rand_ZeroOne() - 0.5f) * (spawnPosVariation * 0.1f));
                    particleInitPos.z = this->actor.world.pos.z + 60.0f;
                }

                if (LINK_IS_ADULT) {
                    particleInitPos.y += 20.0f;
                }
            }

            particlePrimColor.r = sParticlePrimColors[particleType].r;
            particlePrimColor.g = sParticlePrimColors[particleType].g;
            particlePrimColor.b = sParticlePrimColors[particleType].b;
            particleEnvColor.r = sParticleEnvColors[particleType].r;
            particleEnvColor.g = sParticleEnvColors[particleType].g;
            particleEnvColor.b = sParticleEnvColors[particleType].b;
            BgDyYoseizo_ParticleInit(this, &particleInitPos, &particleInitVelocity, &particleAccel, &particlePrimColor,
                                     &particleEnvColor, particleScale, particleLife, particleType);
        }
    }
}

void BgDyYoseizo_Bob(BgDyYoseizo* this, GlobalContext* globalCtx) {
    this->targetHeight = this->grownHeight + this->bobOffset;
    Math_ApproachF(&this->actor.world.pos.y, this->targetHeight, 0.1f, 10.0f);
    Math_ApproachF(&this->bobOffset, 10.0f, 0.1f, 0.5f);

    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        this->actor.velocity.y = Math_SinS(this->bobTimer);
    } else {
        this->actor.velocity.y = Math_SinS(this->bobTimer) * 0.4f;
    }
}

void BgDyYoseizo_CheckMagicAcquired(BgDyYoseizo* this, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, 0x38)) {
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;

        if(gSaveContext.n64ddFlag) {
            gSaveContext.healthAccumulator = 0x140;
            Magic_Fill(globalCtx);
            if(Flags_GetTreasure(globalCtx, this->fountainType + 1)) {
                Actor_Kill(&this->actor);
            } else {
                GivePlayerRandoRewardGreatFairy(this, globalCtx);
            }
            return;
        } 

        if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
            if (!gSaveContext.magicAcquired && (this->fountainType != FAIRY_UPGRADE_MAGIC)) {
                Actor_Kill(&this->actor);
                return;
            }
        } else {
            if (!gSaveContext.magicAcquired) {
                Actor_Kill(&this->actor);
                return;
            }
        }
        func_8002DF54(globalCtx, &this->actor, 1);
        this->actionFunc = BgDyYoseizo_ChooseType;
    }
}

void BgDyYoseizo_ChooseType(BgDyYoseizo* this, GlobalContext* globalCtx) {
    s32 givingReward;

    func_8002DF54(globalCtx, &this->actor, 1);
    // "Mode"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ もうど ☆☆☆☆☆ %d\n" VT_RST, globalCtx->msgCtx.ocarinaMode);
    givingReward = false;

    if (globalCtx->sceneNum != SCENE_DAIYOUSEI_IZUMI) {
        switch (this->fountainType) {
            case FAIRY_SPELL_FARORES_WIND:
                if (!(gSaveContext.itemGetInf[1] & 0x100)) {
                    givingReward = true;
                }
                break;
            case FAIRY_SPELL_DINS_FIRE:
                if (!(gSaveContext.itemGetInf[1] & 0x200)) {
                    givingReward = true;
                }
                break;
            case FAIRY_SPELL_NAYRUS_LOVE:
                if (!(gSaveContext.itemGetInf[1] & 0x400)) {
                    givingReward = true;
                }
                break;
        }
    } else {
        switch (this->fountainType) {
            case FAIRY_UPGRADE_MAGIC:
                if (!gSaveContext.magicAcquired || BREG(2)) {
                    // "Spin Attack speed UP"
                    osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 回転切り速度ＵＰ ☆☆☆☆☆ \n" VT_RST);
                    this->givingSpell = true;
                    givingReward = true;
                }
                break;
            case FAIRY_UPGRADE_DOUBLE_MAGIC:
                if (!gSaveContext.doubleMagic) {
                    // "Magic Meter doubled"
                    osSyncPrintf(VT_FGCOL(YELLOW) " ☆☆☆☆☆ 魔法ゲージメーター倍増 ☆☆☆☆☆ \n" VT_RST);
                    this->givingSpell = true;
                    givingReward = true;
                }
                break;
            case FAIRY_UPGRADE_HALF_DAMAGE:
                if (!gSaveContext.doubleDefense) {
                    // "Damage halved"
                    osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆☆☆☆ ダメージ半減 ☆☆☆☆☆ \n" VT_RST);
                    this->givingSpell = true;
                    givingReward = true;
                }
                break;
        }
    }

    if (givingReward) {
        if (gSaveContext.sceneSetupIndex < 4) {
            if (globalCtx->sceneNum != SCENE_DAIYOUSEI_IZUMI) {
                switch (this->fountainType) {
                    case FAIRY_SPELL_FARORES_WIND:
                        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGreatFairyFaroresWindCs);
                        gSaveContext.cutsceneTrigger = 1;
                        break;
                    case FAIRY_SPELL_DINS_FIRE:
                        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGreatFairyDinsFireCs);
                        gSaveContext.cutsceneTrigger = 1;
                        break;
                    case FAIRY_SPELL_NAYRUS_LOVE:
                        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGreatFairyNayrusLoveCs);
                        gSaveContext.cutsceneTrigger = 1;
                        break;
                }
            } else {
                switch (this->fountainType) {
                    case FAIRY_UPGRADE_MAGIC:
                        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGreatFairyMagicCs);
                        gSaveContext.cutsceneTrigger = 1;
                        break;
                    case FAIRY_UPGRADE_DOUBLE_MAGIC:
                        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGreatFairyDoubleMagicCs);
                        gSaveContext.cutsceneTrigger = 1;
                        break;
                    case FAIRY_UPGRADE_HALF_DAMAGE:
                        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGreatFairyDoubleDefenceCs);
                        gSaveContext.cutsceneTrigger = 1;
                        break;
                }
            }
        }
        this->actionFunc = BgDyYoseizo_SetupSpinGrow_Reward;
        return;
    }

    globalCtx->envCtx.unk_BF = 2;

    if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
        OnePointCutscene_Init(globalCtx, 8603, -99, NULL, MAIN_CAM);
    } else {
        OnePointCutscene_Init(globalCtx, 8604, -99, NULL, MAIN_CAM);
    };

    Audio_PlayActorSound2(&this->actor, NA_SE_EV_GREAT_FAIRY_APPEAR);
    this->actor.draw = BgDyYoseizo_Draw;
    this->actionFunc = BgDyYoseizo_SetupSpinGrow_NoReward;
}

// Sets animations for spingrow
void BgDyYoseizo_SetupSpinGrow_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx) {
    if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
        this->frameCount = Animation_GetLastFrame(&gGreatFairySittingTransitionAnim);
        Animation_Change(&this->skelAnime, &gGreatFairySittingTransitionAnim, 1.0f, 0.0f, this->frameCount,
                         ANIMMODE_ONCE, -10.0f);
    } else {
        this->frameCount = Animation_GetLastFrame(&gGreatFairyLayingDownTransitionAnim);
        Animation_Change(&this->skelAnime, &gGreatFairyLayingDownTransitionAnim, 1.0f, 0.0f, this->frameCount,
                         ANIMMODE_ONCE, -10.0f);
    }

    Audio_PlayActorSound2(&this->actor, NA_SE_VO_FR_LAUGH_0);
    func_8002DF54(globalCtx, &this->actor, 1);
    this->actionFunc = BgDyYoseizo_SpinGrow_NoReward;
}

void BgDyYoseizo_SpinGrow_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx) {
    func_8002DF54(globalCtx, &this->actor, 1);
    Math_ApproachF(&this->actor.world.pos.y, this->grownHeight, this->heightFraction, 100.0f);
    Math_ApproachF(&this->scale, 0.035f, this->scaleFraction, 0.005f);
    Math_ApproachF(&this->heightFraction, 0.8f, 0.1f, 0.02f);
    Math_ApproachF(&this->scaleFraction, 0.2f, 0.03f, 0.05f);
    // Finished growing
    if (this->scale >= 0.034f) {
        if ((this->actor.shape.rot.y > -8000) && (this->actor.shape.rot.y < 1000)) {
            SkelAnime_Update(&this->skelAnime);
            // Turn to front
            Math_SmoothStepToS(&this->actor.shape.rot.y, 0, 5, 1000, 0);
            if (fabsf(this->actor.shape.rot.y) < 50.0f) {
                this->actionFunc = BgDyYoseizo_CompleteSpinGrow_NoReward;
            }
        } else {
            this->actor.shape.rot.y += 3000;
        }
    } else {
        this->actor.shape.rot.y += 3000;
    }
    BgDyYoseizo_SpawnParticles(this, globalCtx, 0);
}

void BgDyYoseizo_CompleteSpinGrow_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx) {
    f32 curFrame = this->skelAnime.curFrame;

    func_8002DF54(globalCtx, &this->actor, 1);

    if ((this->frameCount * 1273.0f) <= this->bobTimer) {
        this->bobTimer = 0.0f;
    }

    SkelAnime_Update(&this->skelAnime);

    if ((this->frameCount <= curFrame) && !this->animationChanged) {
        this->actionFunc = BgDyYoseizo_SetupGreetPlayer_NoReward;
    }
}

void BgDyYoseizo_SetupGreetPlayer_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx) {
    func_8002DF54(globalCtx, &this->actor, 1);

    if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
        this->frameCount = Animation_GetLastFrame(&gGreatFairySittingAnim);
        Animation_Change(&this->skelAnime, &gGreatFairySittingAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP,
                         -10.0f);
    } else {
        this->frameCount = Animation_GetLastFrame(&gGreatFairyLayingSidewaysAnim);
        Animation_Change(&this->skelAnime, &gGreatFairyLayingSidewaysAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP,
                         -10.0f);
    }

    this->actor.textId = 0xDB;
    this->dialogState = TEXT_STATE_EVENT;
    Message_StartTextbox(globalCtx, this->actor.textId, NULL);
    BgDyYoseizo_SpawnParticles(this, globalCtx, 0);
    this->actionFunc = BgDyYoseizo_GreetPlayer_NoReward;
}

void BgDyYoseizo_GreetPlayer_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx) {
    func_8002DF54(globalCtx, &this->actor, 1);
    this->bobTimer = this->skelAnime.curFrame * 1273.0f;

    if ((this->frameCount * 1273.0f) <= this->bobTimer) {
        this->bobTimer = 0.0f;
    }

    SkelAnime_Update(&this->skelAnime);

    if ((this->dialogState == Message_GetState(&globalCtx->msgCtx)) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        Interface_ChangeAlpha(5);
        this->actionFunc = BgDyYoseizo_SetupHealPlayer_NoReward;
    }

    BgDyYoseizo_Bob(this, globalCtx);
    BgDyYoseizo_SpawnParticles(this, globalCtx, 0);
}

void BgDyYoseizo_SetupHealPlayer_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx) {
    if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
        this->frameCount = Animation_GetLastFrame(&gGreatFairyGivingUpgradeAnim);
        Animation_Change(&this->skelAnime, &gGreatFairyGivingUpgradeAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_ONCE,
                         -10.0f);
    } else {
        this->frameCount = Animation_GetLastFrame(&gGreatFairyAnim_005810);
        Animation_Change(&this->skelAnime, &gGreatFairyAnim_005810, 1.0f, 0.0f, this->frameCount, ANIMMODE_ONCE,
                         -10.0f);
    }

    Audio_PlayActorSound2(&this->actor, NA_SE_VO_FR_SMILE_0);
    this->mouthState = 1;
    this->actionFunc = BgDyYoseizo_HealPlayer_NoReward;
}

void BgDyYoseizo_HealPlayer_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f beamPos;
    s16 beamParams;

    if (this->animationChanged) {
        this->bobTimer = this->skelAnime.curFrame * 1300.0f;
        if ((this->frameCount * 1300.0f) <= this->bobTimer) {
            this->bobTimer = 0.0f;
        }
    }

    SkelAnime_Update(&this->skelAnime);
    if ((this->frameCount <= curFrame) && !(this->animationChanged)) {
        if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
            this->frameCount = Animation_GetLastFrame(&gGreatFairyAfterUpgradeAnim);
            Animation_Change(&this->skelAnime, &gGreatFairyAfterUpgradeAnim, 1.0f, 0.0f, this->frameCount,
                             ANIMMODE_LOOP, -10.0f);
        } else {
            this->frameCount = Animation_GetLastFrame(&gGreatFairyAfterSpellAnim);
            Animation_Change(&this->skelAnime, &gGreatFairyAfterSpellAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP,
                             -10.0f);
        }
        this->healingTimer = 150;
        this->animationChanged = true;
        if (!this->givingSpell) {
            beamPos.x = player->actor.world.pos.x;
            beamPos.y = player->actor.world.pos.y + 200.0f;
            beamPos.z = player->actor.world.pos.z;

            beamParams = ((globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) ? 0 : 1);

            this->beam =
                (EnDyExtra*)Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_EN_DY_EXTRA,
                                               beamPos.x, beamPos.y, beamPos.z, 0, 0, 0, beamParams);
        }
    }
    if (this->refillTimer > 1) {
        this->refillTimer--;
    }

    if (this->healingTimer >= 110) {
        this->healingTimer--;
    }

    if (this->healingTimer == 110) {
        gSaveContext.healthAccumulator = 0x140;
        Magic_Fill(globalCtx);
        this->refillTimer = 200;
    }

    if (((gSaveContext.healthCapacity == gSaveContext.health) && (gSaveContext.magic == gSaveContext.unk_13F4)) ||
        (this->refillTimer == 1)) {
        this->healingTimer--;
        if (this->healingTimer == 90) {
            if (!this->givingSpell) {
                this->beam->trigger = 1;
            }
            this->givingSpell = false;
        }
    }

    if (this->healingTimer == 1) {
        this->actor.textId = 0xDA;
        this->dialogState = TEXT_STATE_EVENT;
        Message_ContinueTextbox(globalCtx, this->actor.textId);
        this->actionFunc = BgDyYoseizo_SayFarewell_NoReward;
        return;
    }
    BgDyYoseizo_Bob(this, globalCtx);
}

void BgDyYoseizo_SayFarewell_NoReward(BgDyYoseizo* this, GlobalContext* globalCtx) {
    this->bobTimer = this->skelAnime.curFrame * 1400.0f;

    if (this->bobTimer >= (this->frameCount * 1400.0f)) {
        this->bobTimer = 0.0f;
    }

    SkelAnime_Update(&this->skelAnime);

    if ((this->dialogState == Message_GetState(&globalCtx->msgCtx)) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        this->mouthState = 0;
        this->actionFunc = BgDyYoseizo_SetupSpinShrink;
        func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
    }

    BgDyYoseizo_Bob(this, globalCtx);
    BgDyYoseizo_SpawnParticles(this, globalCtx, 0);
}

void BgDyYoseizo_SetupSpinShrink(BgDyYoseizo* this, GlobalContext* globalCtx) {
    if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
        this->frameCount = Animation_GetLastFrame(&gGreatFairyJewelFountainSpinShrinkAnim);
        Animation_Change(&this->skelAnime, &gGreatFairyJewelFountainSpinShrinkAnim, 1.0f, 0.0f, this->frameCount,
                         ANIMMODE_ONCE, -10.0f);
    } else {
        this->frameCount = Animation_GetLastFrame(&gGreatFairySpellFountainSpinShrinkAnim);
        Animation_Change(&this->skelAnime, &gGreatFairySpellFountainSpinShrinkAnim, 1.0f, 0.0f, this->frameCount,
                         ANIMMODE_ONCE, -10.0f);
    }

    this->vanishTimer = 5;
    this->scaleFraction = 0.0f;
    this->heightFraction = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_VO_FR_LAUGH_0);
    Audio_PlayActorSound2(&this->actor, NA_SE_EV_GREAT_FAIRY_VANISH);
    this->actionFunc = BgDyYoseizo_SpinShrink;
}

void BgDyYoseizo_SpinShrink(BgDyYoseizo* this, GlobalContext* globalCtx) {
    SkelAnime_Update(&this->skelAnime);
    if (this->vanishTimer == 0) {
        if (this->scale < 0.003f) {
            this->vanishTimer = 30;
            this->actionFunc = BgDyYoseizo_Vanish;
        } else {
            Math_ApproachF(&this->actor.world.pos.y, this->vanishHeight, this->heightFraction, 100.0f);
            Math_ApproachZeroF(&this->scale, this->scaleFraction, 0.005f);
            Math_ApproachF(&this->heightFraction, 0.8f, 0.1f, 0.02f);
            Math_ApproachF(&this->scaleFraction, 0.2f, 0.03f, 0.05f);
            this->actor.shape.rot.y += 3000;
            BgDyYoseizo_SpawnParticles(this, globalCtx, 0);
        }
    }
}

void BgDyYoseizo_Vanish(BgDyYoseizo* this, GlobalContext* globalCtx) {
    Actor* findOcarinaSpot;

    if (this->vanishTimer == 0) {
        func_8002DF54(globalCtx, &this->actor, 7);
        globalCtx->envCtx.unk_BF = 0;
        findOcarinaSpot = globalCtx->actorCtx.actorLists[ACTORCAT_PROP].head;

        while (findOcarinaSpot != NULL) {
            if (findOcarinaSpot->id != ACTOR_EN_OKARINA_TAG) {
                findOcarinaSpot = findOcarinaSpot->next;
                continue;
            }
            Actor_Kill(findOcarinaSpot);
            break;
        }

        Flags_UnsetSwitch(globalCtx, 0x38);
        Actor_Kill(&this->actor);
    }
}

void BgDyYoseizo_SetupSpinGrow_Reward(BgDyYoseizo* this, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        if ((globalCtx->csCtx.npcActions[0] != NULL) && (globalCtx->csCtx.npcActions[0]->action == 2)) {
            this->actor.draw = BgDyYoseizo_Draw;
            func_8002DF54(globalCtx, &this->actor, 1);
            this->finishedSpinGrow = false;

            if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
                this->frameCount = Animation_GetLastFrame(&gGreatFairySittingTransitionAnim);
                Animation_Change(&this->skelAnime, &gGreatFairySittingTransitionAnim, 1.0f, 0.0f, this->frameCount,
                                 ANIMMODE_ONCE, -10.0f);
            } else {
                this->frameCount = Animation_GetLastFrame(&gGreatFairyLayingDownTransitionAnim);
                Animation_Change(&this->skelAnime, &gGreatFairyLayingDownTransitionAnim, 1.0f, 0.0f, this->frameCount,
                                 ANIMMODE_ONCE, -10.0f);
            }

            Audio_PlayActorSound2(&this->actor, NA_SE_EV_GREAT_FAIRY_APPEAR);
            this->actionFunc = BgDyYoseizo_SpinGrowSetupGive_Reward;
        }
    }
}

void BgDyYoseizo_SpinGrowSetupGive_Reward(BgDyYoseizo* this, GlobalContext* globalCtx) {
    f32 curFrame = this->skelAnime.curFrame;

    if (!this->finishedSpinGrow) {
        Math_ApproachF(&this->actor.world.pos.y, this->grownHeight, this->heightFraction, 100.0f);
        Math_ApproachF(&this->scale, 0.035f, this->scaleFraction, 0.005f);
        Math_ApproachF(&this->heightFraction, 0.8f, 0.1f, 0.02f);
        Math_ApproachF(&this->scaleFraction, 0.2f, 0.03f, 0.05f);
        // Finished growing
        if (this->scale >= 0.034f) {
            if ((this->actor.shape.rot.y > -8000) && (this->actor.shape.rot.y < 1000)) {
                SkelAnime_Update(&this->skelAnime);
                // Spin until facing front
                Math_ApproachS(&this->actor.shape.rot.y, 0, 5, 1000);
                if (fabsf(this->actor.shape.rot.y) < 50.0f) {
                    this->finishedSpinGrow = true;
                }
            } else {
                this->actor.shape.rot.y += 3000;
            }
        } else {
            this->actor.shape.rot.y += 3000;
        }
    } else {
        SkelAnime_Update(&this->skelAnime);

        if ((this->frameCount <= curFrame) && !this->animationChanged) {
            if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
                this->frameCount = Animation_GetLastFrame(&gGreatFairySittingAnim);
                Animation_Change(&this->skelAnime, &gGreatFairySittingAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP,
                                 -10.0f);
            } else {
                this->frameCount = Animation_GetLastFrame(&gGreatFairyLayingSidewaysAnim);
                Animation_Change(&this->skelAnime, &gGreatFairyLayingSidewaysAnim, 1.0f, 0.0f, this->frameCount,
                                 ANIMMODE_LOOP, -10.0f);
            }
            this->animationChanged = true;
        }

        if ((globalCtx->csCtx.state != CS_STATE_IDLE) &&
            ((globalCtx->csCtx.npcActions[0] != NULL) && (globalCtx->csCtx.npcActions[0]->action == 3))) {
            this->finishedSpinGrow = this->animationChanged = false;
            if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
                this->frameCount = Animation_GetLastFrame(&gGreatFairyGivingUpgradeAnim);
                Animation_Change(&this->skelAnime, &gGreatFairyGivingUpgradeAnim, 1.0f, 0.0f, this->frameCount,
                                 ANIMMODE_ONCE, -10.0f);
            } else {
                this->frameCount = Animation_GetLastFrame(&gGreatFairyAnim_005810);
                Animation_Change(&this->skelAnime, &gGreatFairyAnim_005810, 1.0f, 0.0f, this->frameCount, ANIMMODE_ONCE,
                                 -10.0f);
            }
            this->mouthState = 1;
            this->actionFunc = BgDyYoseizo_Give_Reward;
        }
    }
    BgDyYoseizo_SpawnParticles(this, globalCtx, 0);
}

static s16 sDemoEffectLightColors[] = { DEMO_EFFECT_LIGHT_GREEN, DEMO_EFFECT_LIGHT_RED, DEMO_EFFECT_LIGHT_BLUE };

static s16 sExItemTypes[] = { EXITEM_MAGIC_WIND, EXITEM_MAGIC_FIRE, EXITEM_MAGIC_DARK };

static s16 sItemGetFlags[] = { 0x100, 0x200, 0x400 };

static u8 sItemIds[] = { ITEM_FARORES_WIND, ITEM_DINS_FIRE, ITEM_NAYRUS_LOVE };

void BgDyYoseizo_Give_Reward(BgDyYoseizo* this, GlobalContext* globalCtx) {
    f32 curFrame = this->skelAnime.curFrame;
    Player* player = GET_PLAYER(globalCtx);
    s16 actionIndex;
    s16 demoEffectParams;
    Vec3f itemPos;

    if (this->animationChanged) {
        this->bobTimer = this->skelAnime.curFrame * 1400.0f;
        if ((this->frameCount * 1400.0f) <= this->bobTimer) {
            this->bobTimer = 0.0f;
        }
    }
    SkelAnime_Update(&this->skelAnime);

    if ((this->frameCount <= curFrame) && !this->animationChanged) {
        if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
            this->frameCount = Animation_GetLastFrame(&gGreatFairyAfterUpgradeAnim);
            Animation_Change(&this->skelAnime, &gGreatFairyAfterUpgradeAnim, 1.0f, 0.0f, this->frameCount,
                             ANIMMODE_LOOP, -10.0f);
        } else {
            this->frameCount = Animation_GetLastFrame(&gGreatFairyAfterSpellAnim);
            Animation_Change(&this->skelAnime, &gGreatFairyAfterSpellAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP,
                             -10.0f);
        }
        this->animationChanged = true;
    }

    if (globalCtx->csCtx.npcActions[0]->action == 13) {
        this->actionFunc = BgDyYoseizo_SetupSpinShrink;
        return;
    }

    if ((globalCtx->csCtx.npcActions[0]->action >= 4) && (globalCtx->csCtx.npcActions[0]->action < 7)) {
        actionIndex = globalCtx->csCtx.npcActions[0]->action - 4;
        if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
            actionIndex++;
            BgDyYoseizo_SpawnParticles(this, globalCtx, actionIndex);

        } else if (!this->lightBallSpawned) {
            demoEffectParams = ((s16)(sDemoEffectLightColors[actionIndex] << 0xC) | DEMO_EFFECT_LIGHT);
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DEMO_EFFECT, this->actor.world.pos.x,
                        this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, (s32)demoEffectParams);
            this->lightBallSpawned = true;
        }
    } else {
        BgDyYoseizo_SpawnParticles(this, globalCtx, 0);
    }

    if ((globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) && (globalCtx->csCtx.npcActions[0]->action >= 10) &&
        (globalCtx->csCtx.npcActions[0]->action < 13)) {
        actionIndex = globalCtx->csCtx.npcActions[0]->action - 10;

        switch (actionIndex) {
            case FAIRY_UPGRADE_MAGIC:
                gSaveContext.magicAcquired = true;
                gSaveContext.unk_13F6 = 0x30;
                Interface_ChangeAlpha(9);
                break;
            case FAIRY_UPGRADE_DOUBLE_MAGIC:
                if (!gSaveContext.magicAcquired) {
                    gSaveContext.magicAcquired = true;
                }
                gSaveContext.doubleMagic = true;
                gSaveContext.unk_13F6 = 0x60;
                gSaveContext.magicLevel = 0;
                Interface_ChangeAlpha(9);
                break;
            case FAIRY_UPGRADE_HALF_DAMAGE:
                gSaveContext.doubleDefense = true;
                Interface_ChangeAlpha(9);
                break;
        }

        if (!this->healing) {
            gSaveContext.healthAccumulator = 0x140;
            this->healing = true;
            if (actionIndex == 2) {
                Magic_Fill(globalCtx);
            }
        }
    }

    if ((globalCtx->sceneNum != SCENE_DAIYOUSEI_IZUMI) && (globalCtx->csCtx.npcActions[0]->action >= 14) &&
        (globalCtx->csCtx.npcActions[0]->action < 17)) {
        actionIndex = globalCtx->csCtx.npcActions[0]->action - 14;

        if (!this->itemSpawned) {
            itemPos.x = player->actor.world.pos.x;
            itemPos.y = (LINK_IS_ADULT ? player->actor.world.pos.y + 73.0f : player->actor.world.pos.y + 53.0f);
            itemPos.z = player->actor.world.pos.z;

            this->item =
                (EnExItem*)Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_EN_EX_ITEM,
                                              itemPos.x, itemPos.y, itemPos.z, 0, 0, 0, sExItemTypes[actionIndex]);

            if (this->item != NULL) {
                if (gSaveContext.magicAcquired == 0) {
                    gSaveContext.magicAcquired = 1;
                } else {
                    Magic_Fill(globalCtx);
                }

                this->itemSpawned = true;
                gSaveContext.healthAccumulator = 0x140;
                Interface_ChangeAlpha(9);
                gSaveContext.itemGetInf[1] |= sItemGetFlags[actionIndex];
                Item_Give(globalCtx, sItemIds[actionIndex]);
            }
        } else {
            this->item->actor.world.pos.x = player->actor.world.pos.x;
            this->item->actor.world.pos.y =
                (LINK_IS_ADULT ? player->actor.world.pos.y + 73.0f : player->actor.world.pos.y + 53.0f);
            this->item->actor.world.pos.z = player->actor.world.pos.z;
            this->item->scale = 0.3f;
        }
    }

    if ((globalCtx->sceneNum != SCENE_DAIYOUSEI_IZUMI) && (globalCtx->csCtx.npcActions[0]->action == 17) &&
        (this->item != NULL)) {
        Actor_Kill(&this->item->actor);
        this->item = NULL;
    }

    if ((globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) && (globalCtx->csCtx.npcActions[0]->action == 18)) {
        this->giveDefenseHearts = true;
    }

    if (this->giveDefenseHearts) {
        if (gSaveContext.inventory.defenseHearts < 20) {
            gSaveContext.inventory.defenseHearts++;
        }
    }

    if ((globalCtx->csCtx.npcActions[0]->action >= 19) && (globalCtx->csCtx.npcActions[0]->action < 22) &&
        !this->warpEffectSpawned) {
        actionIndex = globalCtx->csCtx.npcActions[0]->action - 11;
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DOOR_WARP1, player->actor.world.pos.x,
                    player->actor.world.pos.y, player->actor.world.pos.z, 0, 0, 0, actionIndex);
        this->warpEffectSpawned = true;
    }
    BgDyYoseizo_Bob(this, globalCtx);
}

void BgDyYoseizo_Update(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BgDyYoseizo* this = (BgDyYoseizo*)thisx;
    s32 phi_v1;

    this->absoluteTimer++;

    if (this->vanishTimer != 0) {
        this->vanishTimer--;
    }
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    }
    if (this->unusedTimer != 0) {
        this->unusedTimer--;
    }

    this->actionFunc(this, globalCtx);

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        phi_v1 = 0;
        if (globalCtx->sceneNum == SCENE_DAIYOUSEI_IZUMI) {
            if ((globalCtx->csCtx.frames == 32) || (globalCtx->csCtx.frames == 291) ||
                (globalCtx->csCtx.frames == 426) || (globalCtx->csCtx.frames == 851)) {
                phi_v1 = 1;
            }
            if (globalCtx->csCtx.frames == 101) {
                phi_v1 = 2;
            }
        } else {
            if ((globalCtx->csCtx.frames == 35) || (globalCtx->csCtx.frames == 181) ||
                (globalCtx->csCtx.frames == 462) || (globalCtx->csCtx.frames == 795)) {
                phi_v1 = 1;
            }
            if (globalCtx->csCtx.frames == 90) {
                phi_v1 = 2;
            }
        }

        if (phi_v1 == 1) {
            Audio_PlayActorSound2(&this->actor, NA_SE_VO_FR_SMILE_0);
        }
        if (phi_v1 == 2) {
            Audio_PlayActorSound2(&this->actor, NA_SE_VO_FR_LAUGH_0);
        }
    }

    if ((this->blinkTimer == 0) && (this->actionFunc != BgDyYoseizo_HealPlayer_NoReward)) {
        this->eyeState++;
        this->eyeState2++;
        if (this->eyeState >= 3) {
            this->eyeState = this->eyeState2 = 0;
            this->blinkTimer = (s16)Rand_ZeroFloat(60.0f) + 20;
        }
    }

    Actor_MoveForward(&this->actor);
    this->heightOffset = this->scale * 7500.0f;
    Actor_SetFocus(&this->actor, this->heightOffset);
    this->actor.focus.pos.y = this->heightOffset;
    func_80038290(globalCtx, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    BgDyYoseizo_ParticleUpdate(this, globalCtx);
    Actor_SetScale(&this->actor, this->scale);
}

s32 BgDyYoseizo_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                 void* thisx) {
    BgDyYoseizo* this = (BgDyYoseizo*)thisx;

    if (limbIndex == 8) { // Torso
        rot->x += this->torsoRot.y;
    }
    if (limbIndex == 15) { // Head
        rot->x += this->headRot.y;
        rot->z += this->headRot.z;
    }
    return 0;
}

static void* sEyeTextures[] = {
    gGreatFairyEyeOpenTex,   // Open
    gGreatFairyEyeHalfTex,   // Half
    gGreatFairyEyeClosedTex, // Closed
};

static void* sMouthTextures[] = {
    gGreatFairyMouthClosedTex, // Closed
    gGreatFairyMouthOpenTex,   // Open
};

void BgDyYoseizo_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgDyYoseizo* this = (BgDyYoseizo*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    if (this->actionFunc != BgDyYoseizo_Vanish) {
        func_80093D18(globalCtx->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[this->eyeState]));

        // This was probably intended to allow this actor to wink, but segment 09 is not used in the dList for the head,
        // so it can only blink
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sEyeTextures[this->eyeState2]));

        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(sMouthTextures[this->mouthState]));

        SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable,
                              this->skelAnime.dListCount, BgDyYoseizo_OverrideLimbDraw, NULL, this);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx);
    BgDyYoseizo_ParticleDraw(this, globalCtx);
}

void BgDyYoseizo_ParticleInit(BgDyYoseizo* this, Vec3f* initPos, Vec3f* initVelocity, Vec3f* accel,
                              Color_RGB8* primColor, Color_RGB8* envColor, f32 scale, s16 life, s16 type) {
    BgDyYoseizoParticle* particle;
    s16 i;

    particle = this->particles;

    for (i = 0; i < 200; i++, particle++) {
        if (particle->alive == 0) {
            particle->alive = 1;
            particle->pos = *initPos;
            particle->velocity = *initVelocity;
            particle->accel = *accel;
            particle->primColor = *primColor;
            particle->alpha = 0;
            particle->envColor = *envColor;
            particle->scale = scale;
            particle->timer = life;
            particle->type = type;
            particle->pitch = 0.0f;
            particle->yaw = Rand_CenteredFloat(30000.0f);
            particle->roll = 0.0f;
            return;
        }
    }
}

void BgDyYoseizo_ParticleUpdate(BgDyYoseizo* this, GlobalContext* globalCtx) {
    BgDyYoseizoParticle* particle = this->particles;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f sp94;
    Vec3f sp88;
    f32 goalPitch;
    f32 goalYaw;
    s16 i = 0;

    for (i = 0; i < 200; i++, particle++) {
        if (particle->alive != 0) {
            particle->roll += 3000.0f;

            if (particle->type == 0) {
                particle->pos.x += particle->velocity.x;
                particle->pos.y += particle->velocity.y;
                particle->pos.z += particle->velocity.z;
                particle->velocity.x += particle->accel.x;
                particle->velocity.y += particle->accel.y;
                particle->velocity.z += particle->accel.z;
            } else {
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_HEALING - SFX_FLAG);

                sp94 = player->actor.world.pos;
                sp94.y = player->actor.world.pos.y - 150.0f;
                sp94.z = player->actor.world.pos.z - 50.0f;

                goalPitch = Math_Vec3f_Pitch(&particle->pos, &sp94);
                goalYaw = Math_Vec3f_Yaw(&particle->pos, &sp94);

                Math_ApproachF(&particle->pitch, goalPitch, 0.9f, 5000.0f);
                Math_ApproachF(&particle->yaw, goalYaw, 0.9f, 5000.0f);
                Matrix_Push();
                Matrix_RotateY(BINANG_TO_RAD(particle->yaw), MTXMODE_NEW);
                Matrix_RotateX(BINANG_TO_RAD(particle->pitch), MTXMODE_APPLY);

                sp94.x = sp94.y = sp94.z = 3.0f;

                Matrix_MultVec3f(&sp94, &sp88);
                Matrix_Pop();
                particle->pos.x += sp88.x;
                particle->pos.y += sp88.y;
                particle->pos.z += sp88.z;
            }
        }

        // fade up, fade down, vanish and reset
        if (particle->timer != 0) {
            particle->timer--;
            particle->alpha += 30;

            if (particle->alpha > 255) {
                particle->alpha = 255;
            }
        } else {
            particle->alpha -= 30;

            if (particle->alpha <= 0) {
                particle->alpha = particle->alive = 0;
            }
        }
    }
}

void BgDyYoseizo_ParticleDraw(BgDyYoseizo* this, GlobalContext* globalCtx) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    u8 phi_s3 = 0;
    BgDyYoseizoParticle* particle = this->particles;
    s16 i;

    OPEN_DISPS(gfxCtx);
    func_80093D84(globalCtx->state.gfxCtx);

    for (i = 0; i < 200; i++, particle++) {
        if (particle->alive == 1) {
            if (phi_s3 == 0) {
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gGreatFairyParticleAppearDL));
                gDPPipeSync(POLY_XLU_DISP++);

                phi_s3++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, particle->primColor.r, particle->primColor.g, particle->primColor.b,
                            particle->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, particle->envColor.r, particle->envColor.g, particle->envColor.b, 0);

            Matrix_Translate(particle->pos.x, particle->pos.y, particle->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(particle->scale, particle->scale, 1.0f, MTXMODE_APPLY);
            Matrix_RotateZ(particle->roll, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gGreatFairyParticleAliveDL));
        }
    }

    CLOSE_DISPS(gfxCtx);
}
