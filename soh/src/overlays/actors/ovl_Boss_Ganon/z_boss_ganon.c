#include "z_boss_ganon.h"
#include "overlays/ovl_Boss_Ganon/ovl_Boss_Ganon.h"
#include "overlays/actors/ovl_En_Ganon_Mant/z_en_ganon_mant.h"
#include "overlays/actors/ovl_En_Zl3/z_en_zl3.h"
#include "overlays/actors/ovl_Bg_Ganon_Otyuka/z_bg_ganon_otyuka.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "assets/objects/object_ganon/object_ganon.h"
#include "assets/objects/object_ganon_anime1/object_ganon_anime1.h"
#include "assets/objects/object_ganon_anime2/object_ganon_anime2.h"
#include "assets/scenes/dungeons/ganon_boss/ganon_boss_scene.h"

#include "soh/frame_interpolation.h"

#include <string.h>

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BossGanon_Init(Actor* thisx, PlayState* play);
void BossGanon_Destroy(Actor* thisx, PlayState* play);
void BossGanon_Update(Actor* thisx, PlayState* play);
void BossGanon_Draw(Actor* thisx, PlayState* play);
void func_808E1EB4(Actor* thisx, PlayState* play); // update
void func_808E2544(Actor* thisx, PlayState* play); // update
void BossGanon_LightBall_Update(Actor* thisx, PlayState* play);
void func_808E229C(Actor* thisx, PlayState* play); // draw
void func_808E324C(Actor* thisx, PlayState* play); // draw
void BossGanon_LightBall_Draw(Actor* thisx, PlayState* play);
void BossGanon_Reset(void);


void BossGanon_SetupIntroCutscene(BossGanon* this, PlayState* play);
void BossGanon_SetupTowerCutscene(BossGanon* this, PlayState* play);
void BossGanon_IntroCutscene(BossGanon* this, PlayState* play);
void BossGanon_DeathAndTowerCutscene(BossGanon* this, PlayState* play);
void BossGanon_Wait(BossGanon* this, PlayState* play);
void BossGanon_ChargeLightBall(BossGanon* this, PlayState* play);
void BossGanon_PlayTennis(BossGanon* this, PlayState* play);
void BossGanon_PoundFloor(BossGanon* this, PlayState* play);
void BossGanon_ChargeBigMagic(BossGanon* this, PlayState* play);
void BossGanon_Block(BossGanon* this, PlayState* play);
void BossGanon_HitByLightBall(BossGanon* this, PlayState* play);
void BossGanon_Vulnerable(BossGanon* this, PlayState* play);
void BossGanon_Damaged(BossGanon* this, PlayState* play);

void BossGanon_SetupWait(BossGanon* this, PlayState* play);
void BossGanon_SetupChargeLightBall(BossGanon* this, PlayState* play);
void BossGanon_SetupPlayTennis(BossGanon* this, PlayState* play);

void BossGanon_DrawEffects(PlayState* play);
void BossGanon_UpdateEffects(PlayState* play);

s32 BossGanon_CheckFallingPlatforms(BossGanon* this, PlayState* play, Vec3f* checkPos);

const ActorInit Boss_Ganon_InitVars = {
    ACTOR_BOSS_GANON,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_GANON,
    sizeof(BossGanon),
    (ActorFunc)BossGanon_Init,
    (ActorFunc)BossGanon_Destroy,
    (ActorFunc)BossGanon_Update,
    (ActorFunc)BossGanon_Draw,
    (ActorResetFunc)BossGanon_Reset,
};

static ColliderCylinderInit sDorfCylinderInit = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0xFFCFFFFE, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 80, -50, { 0, 0, 0 } },
};

static ColliderCylinderInit sLightBallCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK6,
        { 0x00100700, 0x00, 0x08 },
        { 0x0D900740, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 30, -15, { 0, 0, 0 } },
};

static u8 D_808E4C58[] = { 0, 12, 10, 12, 14, 16, 12, 14, 16, 12, 14, 16, 12, 14, 16, 10, 16, 14 };
static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

EnGanonMant* sBossGanonCape;

s32 sBossGanonSeed1;
s32 sBossGanonSeed3;
s32 sBossGanonSeed2;

BossGanon* sBossGanonGanondorf;

EnZl3* sBossGanonZelda;

GanondorfEffect sBossGanonEffectBuf[200];

void BossGanonEff_SpawnWindowShard(PlayState* play, Vec3f* pos, Vec3f* velocity, f32 scale) {
    static Color_RGB8 shardColors[] = { { 255, 175, 85 }, { 155, 205, 155 }, { 155, 125, 55 } };
    s16 i;
    GanondorfEffect* eff = play->specialEffects;
    Color_RGB8* color;

    for (i = 0; i < 200; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_WINDOW_SHARD;
            eff->pos = *pos;
            eff->velocity = *velocity;
            eff->accel = sZeroVec;
            eff->scale = scale;
            eff->accel.y = -1.5f;
            eff->unk_44 = Rand_ZeroFloat(6.28f);
            eff->unk_48 = Rand_ZeroFloat(6.28f);
            color = &shardColors[(s16)Rand_ZeroFloat(2.99f)];
            eff->color.r = color->r;
            eff->color.g = color->g;
            eff->color.b = color->b;
            eff->timer = (s16)Rand_ZeroFloat(20.0f);
            eff->epoch++;
            break;
        }
    }
}

void BossGanonEff_SpawnSparkle(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale,
                               s16 arg6) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_SPARKLE;
            eff->pos = *pos;
            eff->velocity = *velocity;
            eff->accel = *accel;
            eff->scale = scale / 1000.0f;
            eff->unk_2E = (s16)Rand_ZeroFloat(100.0f) + 0xC8;
            eff->unk_30 = arg6;
            eff->timer = (s16)Rand_ZeroFloat(10.0f);
            eff->epoch++;
            break;
        }
    }
}

void BossGanonEff_SpawnLightRay(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale,
                                f32 arg5, s16 arg6) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_LIGHT_RAY;
            eff->pos = *pos;
            eff->velocity = *velocity;
            eff->accel = *accel;
            eff->scale = scale / 1000.0f;
            eff->unk_38 = 1.0f;
            eff->unk_40 = arg5;
            eff->unk_2E = (s16)Rand_ZeroFloat(100.0f) + 0xC8;
            eff->unk_30 = arg6;
            eff->timer = (s16)Rand_ZeroFloat(10.0f);
            eff->unk_48 = Math_Atan2F(eff->velocity.z, eff->velocity.x);
            eff->unk_44 = -Math_Atan2F(sqrtf(SQXZ(eff->velocity)), eff->velocity.y);
            eff->epoch++;
            break;
        }
    }
}

void BossGanonEff_SpawnShock(PlayState* play, f32 scale, s16 shockType) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 75; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_SHOCK;
            eff->pos = sZeroVec;
            eff->pos.y = -2000.0f;
            eff->velocity = sZeroVec;
            eff->accel = sZeroVec;
            eff->scale = scale / 1000.0f;
            eff->unk_2E = shockType;
            eff->timer = 0;
            eff->epoch++;
            break;
        }
    }
}

void BossGanonEff_SpawnLightning(PlayState* play, f32 scale, f32 arg2, f32 arg3) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_LIGHTNING;
            eff->velocity = sZeroVec;
            eff->accel = sZeroVec;
            eff->unk_2E = 0;
            eff->scale = scale;
            eff->unk_48 = arg2;
            eff->unk_3C = arg3;
            eff->timer = 0;
            eff->epoch++;
            break;
        }
    }
}

void BossGanonEff_SpawnDustDark(PlayState* play, Vec3f* pos, f32 scale, f32 arg3) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_IMPACT_DUST_DARK;
            eff->pos = *pos;
            eff->velocity = sZeroVec;
            eff->accel = sZeroVec;
            eff->scale = scale;
            eff->unk_40 = 1.0f;
            eff->unk_38 = arg3;
            eff->unk_30 = (s16)Rand_ZeroFloat(100.0f);
            eff->unk_2E = eff->timer = eff->alpha = 0;
            eff->epoch++;
            break;
        }
    }
}

void BossGanonEff_SpawnDustLight(PlayState* play, Vec3f* pos, f32 scale, f32 arg3, s16 bufIndex) {
    GanondorfEffect* effArr = play->specialEffects;

    effArr[bufIndex].type = GDF_EFF_IMPACT_DUST_LIGHT;
    effArr[bufIndex].pos = *pos;
    effArr[bufIndex].velocity = sZeroVec;
    effArr[bufIndex].accel = sZeroVec;
    effArr[bufIndex].unk_40 = 1.0f;
    effArr[bufIndex].scale = scale;
    effArr[bufIndex].unk_38 = arg3;
    effArr[bufIndex].unk_30 = Rand_ZeroFloat(100.0f);
    effArr[bufIndex].unk_2E = effArr[bufIndex].timer = effArr[bufIndex].alpha = 0;
    effArr[bufIndex].epoch++;
}

void BossGanonEff_SpawnShockwave(PlayState* play, Vec3f* pos, f32 scale, f32 arg3) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_SHOCKWAVE;
            eff->pos = *pos;
            eff->velocity = sZeroVec;
            eff->accel = sZeroVec;
            eff->alpha = 255;
            eff->unk_40 = 0.6f;
            eff->scale = scale;
            eff->unk_38 = arg3;
            eff->unk_30 = (s16)Rand_ZeroFloat(100.0f);
            eff->unk_2E = eff->timer = 0;
            eff->epoch++;
            break;
        }
    }
}

void BossGanonEff_SpawnBlackDot(PlayState* play, Vec3f* pos, f32 scale) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_BLACK_DOT;
            eff->pos = *pos;
            eff->velocity = sZeroVec;
            eff->accel = sZeroVec;
            eff->unk_38 = 0.0f;
            eff->scale = scale / 1000.0f;
            eff->timer = 0;
            eff->alpha = 0;
            eff->unk_2E = 0;
            eff->epoch++;
            break;
        }
    }
}

void BossGanon_SetColliderPos(Vec3f* pos, ColliderCylinder* collider) {
    collider->dim.pos.x = pos->x;
    collider->dim.pos.y = pos->y;
    collider->dim.pos.z = pos->z;
}

void BossGanon_SetAnimationObject(BossGanon* this, PlayState* play, s32 objectId) {
    this->animBankIndex = Object_GetIndex(&play->objectCtx, objectId);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->animBankIndex].segment);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x3D, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

void BossGanon_Init(Actor* thisx, PlayState* play2) {
    s16 i;
    PlayState* play = play2;
    BossGanon* this = (BossGanon*)thisx;
    s32 cond;
    f32 xDistFromPlayer;
    f32 yDistFromPlayer;
    f32 zDistFromPlayer;
    Player* player = GET_PLAYER(play);

    if (thisx->params < 0x64) {
        Flags_SetSwitch(play, 0x14);
        play->specialEffects = sBossGanonEffectBuf;

        for (i = 0; i < ARRAY_COUNT(sBossGanonEffectBuf); i++) {
            sBossGanonEffectBuf[i].type = GDF_EFF_NONE;
        }

        sBossGanonGanondorf = this;
        thisx->colChkInfo.health = 40;
        Actor_ProcessInitChain(thisx, sInitChain);
        ActorShape_Init(&thisx->shape, 0, NULL, 0);
        Actor_SetScale(thisx, 0.01f);
        SkelAnime_InitFlex(play, &this->skelAnime, &gGanondorfSkel, NULL, NULL, NULL, 0);
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, thisx, &sDorfCylinderInit);

        if (thisx->params != 1) {
            BossGanon_SetupIntroCutscene(this, play);
            this->organAlpha = 255;
        } else {
            cond = Flags_GetSwitch(play, 0x37) &&
                   ((play->sceneNum == SCENE_GANON_DEMO) || (play->sceneNum == SCENE_GANON_FINAL) ||
                    (play->sceneNum == SCENE_GANON_SONOGO) || (play->sceneNum == SCENE_GANONTIKA_SONOGO));

            if (!cond) {
                BossGanon_SetupTowerCutscene(this, play);
            } else {
                Actor_Kill(thisx);
                return;
            }

            BossGanon_SetupTowerCutscene(this, play);
        }

        sBossGanonCape = (EnGanonMant*)Actor_SpawnAsChild(&play->actorCtx, thisx, play, ACTOR_EN_GANON_MANT, 0.0f,
                                                 0.0f, 0.0f, 0, 0, 0, 1);
        Actor_ChangeCategory(play, &play->actorCtx, thisx, ACTORCAT_BOSS);
    } else {
        thisx->flags &= ~ACTOR_FLAG_0;
        this->fwork[GDF_FWORK_1] = 255.0f;

        if (thisx->params >= 0xC8) {
            if (thisx->params == 0x12C) {
                thisx->update = BossGanon_LightBall_Update;
                thisx->draw = BossGanon_LightBall_Draw;
                this->unk_1A8 = 2;
            } else if (thisx->params == 0x190) {
                thisx->update = BossGanon_LightBall_Update;
                thisx->draw = BossGanon_LightBall_Draw;
                this->unk_1A8 = 1;
            } else if (thisx->params >= 0x104) {
                // big magic light ball thrown
                thisx->update = func_808E2544;
                thisx->draw = func_808E324C;
                this->unk_1C2 = 10;
                this->unk_1A2 = 520 + (-thisx->params * 2);

                for (i = 0; i < 15; i++) {
                    this->unk_2EC[i] = thisx->world.pos;
                }

                this->timers[1] = 3;
                Collider_InitCylinder(play, &this->collider);
                Collider_SetCylinder(play, &this->collider, thisx, &sLightBallCylinderInit);
            } else if (thisx->params >= 0xFA) {
                // big magic light ball charge
                thisx->update = func_808E2544;
                thisx->draw = func_808E324C;
                this->unk_1A2 = Rand_ZeroFloat(10000.0f);

                for (i = 0; i < 15; i++) {
                    this->unk_2EC[i] = thisx->world.pos;
                }

                this->fwork[GDF_FWORK_1] = 0;
            } else {
                thisx->update = func_808E1EB4;
                thisx->draw = func_808E229C;
                thisx->speedXZ = 11.0f;

                if (thisx->params == 0xC8) {
                    this->timers[0] = 7;
                } else {
                    this->timers[0] = (s16)Rand_ZeroFloat(3.0f) + 3;
                }

                for (i = 0; i < 15; i++) {
                    this->unk_2EC[i].y = 5000.0f;
                }
            }
        } else {
            // light ball (anything from 0x64 - 0xC7)
            thisx->update = BossGanon_LightBall_Update;
            thisx->draw = BossGanon_LightBall_Draw;
            thisx->speedXZ = 12.0f;

            xDistFromPlayer = player->actor.world.pos.x - thisx->world.pos.x;
            yDistFromPlayer = (player->actor.world.pos.y + 30.0f) - thisx->world.pos.y;
            zDistFromPlayer = player->actor.world.pos.z - thisx->world.pos.z;

            thisx->world.rot.y = Math_Atan2S(zDistFromPlayer, xDistFromPlayer);
            thisx->world.rot.x = Math_Atan2S(sqrtf(SQ(xDistFromPlayer) + SQ(zDistFromPlayer)), yDistFromPlayer);

            if (Rand_ZeroOne() < 0) {
                thisx->world.rot.y += (s16)Rand_CenteredFloat(5000.0f);
                thisx->world.rot.x += (s16)Rand_CenteredFloat(5000.0f);
            }

            this->timers[1] = 3;
            Collider_InitCylinder(play, &this->collider);
            Collider_SetCylinder(play, &this->collider, thisx, &sLightBallCylinderInit);
        }
    }
}

void BossGanon_Destroy(Actor* thisx, PlayState* play) {
    BossGanon* this = (BossGanon*)thisx;

    if ((this->actor.params < 0xC8) || (this->actor.params >= 0x104)) {
        Collider_DestroyCylinder(play, &this->collider);
    }

    if (this->actor.params < 0x64) {
        SkelAnime_Free(&this->skelAnime, play);
    }
}

void BossGanon_SetupIntroCutscene(BossGanon* this, PlayState* play) {
    s32 pad;
    s32 animBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_GANON_ANIME2);

    if (animBankIndex < 0) {
        Actor_Kill(&this->actor);
        return;
    }

    if (Object_IsLoaded(&play->objectCtx, animBankIndex)) {
        this->actionFunc = BossGanon_IntroCutscene;
        this->unk_198 = 1;
        this->animBankIndex = animBankIndex;
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[animBankIndex].segment);
        Animation_MorphToLoop(&this->skelAnime, &gGanondorfPlayOrganAnim, 0.0f);
    } else {
        this->actionFunc = BossGanon_SetupIntroCutscene;
    }
}

typedef struct {
    /* 0x00 */ Vec3s eye;
    /* 0x06 */ Vec3s at;
} CutsceneCameraPosition; // size = 0x12

static CutsceneCameraPosition sIntroCsCameraPositions[] = {
    { { 0, 40, 0 }, { 0, 50, 430 } },
    { { -20, 30, 400 }, { 10, 55, 440 } },
    { { 0, 60, 300 }, { 0, 273, -150 } },
    { { 0, 180, -260 }, { 0, 155, -300 } },
    { { -30, 60, 440 }, { 20, 25, 390 } },
    { { -50, 140, -360 }, { 50, 92, -390 } },
    { { -10, 264, -121 }, { 5, 266, -160 } },
    { { -13, 200, -310 }, { 0, 125, -410 } },
    { { 0, 40, -50 }, { 0, 35, 230 } },
    { { 0, 140, -250 }, { 0, 115, -570 } },
    { { -410, 150, -130 }, { 50, 155, -170 } },
    { { 0, 130, -230 }, { 0, 125, -2000 } },
    { { -2, 147, -293 }, { -200, 345, -2000 } },
};

void BossGanon_SetIntroCsCamera(BossGanon* this, u8 camPosIndex) {
    CutsceneCameraPosition* camPos = &sIntroCsCameraPositions[camPosIndex];

    this->csCamEye.x = camPos->eye.x;
    this->csCamEye.y = camPos->eye.y;
    this->csCamEye.z = camPos->eye.z;

    this->csCamAt.x = camPos->at.x;
    this->csCamAt.y = camPos->at.y;
    this->csCamAt.z = camPos->at.z;
}

void BossGanon_IntroCutscene(BossGanon* this, PlayState* play) {
    u8 moveCam = false;
    Player* player = GET_PLAYER(play);
    s32 pad;
    f32 sin;
    f32 cos;
    Camera* mainCam;

    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->animBankIndex].segment);

    sBossGanonCape->backPush = -2.0f;
    sBossGanonCape->backSwayMagnitude = 0.25f;
    sBossGanonCape->sideSwayMagnitude = -1.0f;
    sBossGanonCape->minDist = 0.0f;

    this->csTimer++;

    SkelAnime_Update(&this->skelAnime);

    switch (this->csState) {
        case 0:
            player->actor.world.pos.x = 0.0f;
            player->actor.world.pos.y = 0.0f;
            player->actor.world.pos.z = 430.0f;

            this->actor.world.pos.x = 0.0f;
            this->actor.world.pos.y = 112.0f;
            this->actor.world.pos.z = -333.0f;

            this->actor.shape.yOffset = -7000.0f;
            this->actor.shape.rot.y = 0;

            func_80064520(play, &play->csCtx);
            func_8002DF54(play, &this->actor, 8);
            this->csCamIndex = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, this->csCamIndex, CAM_STAT_ACTIVE);
            this->csCamFov = 60.0f;

            if (gSaveContext.eventChkInf[7] & 0x100 || gSaveContext.n64ddFlag) {
                // watched cutscene already, skip most of it
                this->csState = 17;
                this->csTimer = 0;
                player->actor.world.pos.z = 20.0f;
                this->useOpenHand = false;
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfStandBackwardsAnim, -5.0f);
                this->fwork[GDF_FWORK_1] = 1000.0f;
                BossGanon_SetIntroCsCamera(this, 11);
                this->unk_198 = 2;
                this->timers[2] = 110;
                gSaveContext.healthAccumulator = 0x140;
                Audio_QueueSeqCmd(NA_BGM_STOP);
            } else {
                this->useOpenHand = true;
                BossGanon_SetIntroCsCamera(this, 0);
                this->csState = 1;
                sBossGanonZelda = (EnZl3*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_ZL3, 0.0f,
                                                    220.0f, -150.0f, 0, 0, 0, 0x2000);
            }

            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_GANON_ORGAN, 0.0f, 0.0f, 0.0f, 0,
                               0, 0, 1);
            sBossGanonCape->minY = 57.0f;
            // fallthrough
        case 1:
            this->envLightMode = 3;
            if (this->csTimer == 70) {
                this->csState = 2;
                this->csTimer = 0;
            }
            break;

        case 2:
            BossGanon_SetIntroCsCamera(this, 1);

            if (this->csTimer == 10) {
                func_8002DF54(play, &this->actor, 5);
            }

            if (this->csTimer == 13) {
                func_8002F7DC(&player->actor, player->ageProperties->unk_92 + NA_SE_VO_LI_SURPRISE);
            }

            if (this->csTimer != 35) {
                break;
            }

            this->csState = 3;
            this->csTimer = 0;

            this->csCamEye.x = 0.0f;
            this->csCamEye.y = 60.0f;
            this->csCamEye.z = 300.0f;

            this->csCamAt.x = 0.0f;
            this->unk_704 = 1.2566371f;
            // fallthrough
        case 3:
            this->envLightMode = 0;
            play->envCtx.unk_D8 = 0.0f;
            this->csCamAt.y = (sinf(this->unk_704) * 300.0f) + this->csCamEye.y;
            this->csCamAt.z = (cosf(this->unk_704) * -300.0f) + this->csCamEye.z;
            Math_ApproachF(&this->unk_704, 0.25f, 0.05f, this->csCamAtMaxStep.y);
            Math_ApproachF(&this->csCamAtMaxStep.y, 0.01f, 1.0f, 0.0001f);

            if (this->csTimer != 200) {
                break;
            }

            func_8002DF54(play, &this->actor, 8);
            this->csState = 4;
            BossGanon_SetIntroCsCamera(this, 2);
            this->csTimer = 0;
            // fallthrough
        case 4:
            if ((this->csTimer == 0) || (this->csTimer == 10) || (this->csTimer == 20)) {
                this->csCamEye.y += 68.0f;
                this->csCamEye.z -= 142.0f;
            }

            if (this->csTimer >= 20) {
                this->envLightMode = 4;
            } else {
                this->envLightMode = 35;
            }

            if (this->csTimer == 60) {
                BossGanon_SetIntroCsCamera(this, 1);
                this->csState = 5;
                this->csTimer = 0;
            }
            break;

        case 5:
            this->envLightMode = 5;

            if (this->csTimer < 50) {
                play->envCtx.unk_D8 = 1.0f;
            }

            if (this->csTimer == 10) {
                func_8002DF54(play, &this->actor, 0x4B);
            }

            if (this->csTimer == 70) {
                BossGanon_SetIntroCsCamera(this, 3);
                this->csState = 6;
                this->csTimer = 0;
                this->envLightMode = 3;
            }
            break;

        case 6:
            this->envLightMode = 3;

            if (this->csTimer != 30) {
                break;
            }

            this->csState = 7;
            this->csTimer = 0;
            BossGanon_SetIntroCsCamera(this, 4);
            this->triforceType = GDF_TRIFORCE_PLAYER;
            this->fwork[GDF_TRIFORCE_SCALE] = 10.0f;
            this->fwork[GDF_TRIFORCE_PRIM_A] = 0.0f;
            this->fwork[GDF_TRIFORCE_PRIM_B] = 255.0f;
            this->fwork[GDF_TRIFORCE_ENV_G] = 100.0f;
            func_80078884(NA_SE_EV_TRIFORCE_MARK);
            play->envCtx.unk_D8 = 0.0f;
            // fallthrough
        case 7:
            this->envLightMode = 6;
            // fade in links triforce
            Math_ApproachF(&this->fwork[GDF_TRIFORCE_PRIM_A], 255.0f, 1.0f, 10.0f);
            Math_ApproachF(&this->fwork[GDF_TRIFORCE_SCALE], 0.4f, 1.0f, 0.3f);
            Math_ApproachF(&this->fwork[GDF_TRIFORCE_PRIM_B], 170.0f, 1.0f, 2.55f);
            Math_ApproachF(&this->fwork[GDF_TRIFORCE_ENV_G], 200.0f, 1.0f, 3.0f);

            if (this->csTimer >= 30) {
                this->envLightMode = 65;
            }

            if (this->csTimer == 30) {
                play->envCtx.unk_D8 = 1.0f;
            }

            BossGanon_SetIntroCsCamera(this, 4);
            this->csCamEye.x += 5.0f;
            this->csCamEye.z += -10.0f;
            this->csCamAt.x += 18.0f;

            if (this->csTimer == 60) {
                this->csState = 8;
                this->csTimer = 0;
            }
            break;

        case 8:
            this->envLightMode = 3;
            BossGanon_SetIntroCsCamera(this, 5);

            if (this->csTimer != 30) {
                break;
            }

            this->csState = 9;
            this->csTimer = 0;
            func_8002DF54(play, &this->actor, 8);
            sBossGanonZelda->unk_3C8 = 0;
            this->triforceType = GDF_TRIFORCE_ZELDA;
            this->fwork[GDF_TRIFORCE_SCALE] = 10.0f;
            this->fwork[GDF_TRIFORCE_PRIM_A] = 0.0f;
            this->fwork[GDF_TRIFORCE_PRIM_B] = 255.0f;
            this->fwork[GDF_TRIFORCE_ENV_G] = 100.0f;
            func_80078884(NA_SE_EV_TRIFORCE_MARK);
            play->envCtx.unk_D8 = 0.0f;
            // fallthrough
        case 9:
            this->envLightMode = 7;
            BossGanon_SetIntroCsCamera(this, 6);
            // fade in zeldas triforce
            Math_ApproachF(&this->fwork[GDF_TRIFORCE_PRIM_A], 255.0f, 1.0f, 10.0f);
            Math_ApproachF(&this->fwork[GDF_TRIFORCE_SCALE], 0.4f, 1.0f, 0.3f);
            Math_ApproachF(&this->fwork[GDF_TRIFORCE_PRIM_B], 170.0f, 1.0f, 2.55f);
            Math_ApproachF(&this->fwork[GDF_TRIFORCE_ENV_G], 200.0f, 1.0f, 3.0f);

            if (this->csTimer == 30) {
                sBossGanonZelda->unk_3C8 = 1;
            }

            if (this->csTimer >= 32) {
                this->envLightMode = 75;
            }

            if (this->csTimer == 32) {
                play->envCtx.unk_D8 = 1.0f;
            }

            if (this->csTimer == 50) {
                this->csState = 10;
                this->csTimer = 0;
            }
            break;

        case 10: // top view of playing the organ
            this->envLightMode = 3;
            BossGanon_SetIntroCsCamera(this, 7);

            if (this->csTimer == 40) {
                this->csState = 11;
                this->csTimer = 0;
                this->fwork[GDF_TRIFORCE_PRIM_A] = 0.0f;
            }
            break;

        case 11: // link is healed
            this->envLightMode = 3;
            BossGanon_SetIntroCsCamera(this, 8);
            player->actor.world.pos.z = 20.0f;

            if (this->csTimer == 20) {
                func_8002DF54(play, &this->actor, 0x17);
                Interface_ChangeAlpha(11); // show hearts only
            }

            if (this->csTimer == 25) {
                gSaveContext.healthAccumulator = 0x140;
            }

            if (this->csTimer == 100) {
                Interface_ChangeAlpha(1);
            }

            if (this->csTimer == 120) {
                this->csState = 12;
                this->csTimer = 0;
            }
            break;

        case 12: // first dialogue, ganondorf facing away from link
            this->envLightMode = 3;
            BossGanon_SetIntroCsCamera(this, 9);

            if (this->csTimer == 30) {
                Audio_QueueSeqCmd(0x100100FF);
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfStopPlayingOrganAnim);
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfStopPlayingOrganAnim, -5.0f);
            }

            if ((this->csTimer > 30) && Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfLeanOnOrganAnim, 0.0f);
                this->fwork[GDF_FWORK_1] = 1000.0f;
            }

            if (this->csTimer == 80) {
                Message_StartTextbox(play, 0x70C8, NULL);
            }

            if ((this->csTimer > 180) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 15;
                this->csTimer = 0;
                this->useOpenHand = false;
            }
            break;

        case 15: // side view of all 3 of them
            this->envLightMode = 0;
            play->envCtx.unk_D8 = 0.0f;
            BossGanon_SetIntroCsCamera(this, 10);

            if (this->csTimer == 30) {
                Message_StartTextbox(play, 0x70C9, NULL);
            }

            if ((this->csTimer > 100) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 16;
                this->csTimer = 0;
                BossGanon_SetIntroCsCamera(this, 11);
                this->unk_198 = 2;
                sBossGanonZelda->unk_3C8 = 2;
                this->timers[2] = 110;
                this->envLightMode = 3;
            }
            break;

        case 16:
            this->envLightMode = 3;

            if (this->csTimer <= 20) {
                if (this->csTimer == 20) {
                    Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfStandUpFromOrganAnim, -5.0f);
                    this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfStandUpFromOrganAnim);
                }
            } else if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                Message_StartTextbox(play, 0x70CA, NULL);
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfStandBackwardsAnim, -5.0f);
                this->fwork[GDF_FWORK_1] = 1000.0f;
            }

            if ((this->csTimer > 100) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 17;
                this->csTimer = 0;
            }
            break;

        case 17: // turns around
            this->envLightMode = 3;

            if (this->csTimer == 20) {
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfTurnAroundAnim, -5.0f);
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfTurnAroundAnim);
            }

            if (this->csTimer > 10) {
                if (this->csTimer == 62) {
                    sBossGanonCape->attachRightArmTimer = 20.0f;
                }

                if (this->csTimer == 57) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EV_GANON_MANTLE);
                }

                Math_ApproachF(&this->csCamFov, 110.0f, 0.1f, this->csCamMaxStepScale * 2.0f);
                Math_ApproachF(&this->csCamEye.z, -290.0f, 0.1f, this->csCamMaxStepScale * 2.4f);
                Math_ApproachF(&this->csCamMaxStepScale, 0.75f, 1.0f, 0.05f);

                if (this->csTimer == 70) {
                    this->csState = 18;
                    this->csTimer = 0;
                    this->csCamFov = 60.0f;
                    BossGanon_SetIntroCsCamera(this, 12);
                    if (!gSaveContext.n64ddFlag) {
                        Message_StartTextbox(play, 0x70CB, NULL);
                    }
                }
            }
            break;

        case 18: // last dialog before triforce
            this->envLightMode = 3;
            BossGanon_SetIntroCsCamera(this, 12);
            this->csCamEye.y += -6.0f;
            this->csCamEye.z += 6.0f;

            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1] - 5.0f)) {
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfStandIdleAnim, -5.0f);
                this->fwork[GDF_FWORK_1] = 1000.0f;
            }

            if ((this->csTimer <= 50) || (Message_GetState(&play->msgCtx) != TEXT_STATE_NONE)) {
                break;
            }

            this->csState = 19;
            this->csTimer = 0;
            Message_StartTextbox(play, 0x70CC, NULL);
            Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfRaiseHandStartAnim, -5.0f);
            this->triforceType = GDF_TRIFORCE_DORF;
            this->fwork[GDF_TRIFORCE_SCALE] = 10.0f;
            this->fwork[GDF_TRIFORCE_PRIM_A] = 0.0f;
            this->fwork[GDF_TRIFORCE_PRIM_B] = 255.0f;
            this->fwork[GDF_TRIFORCE_ENV_G] = 100.0f;
            play->envCtx.unk_D8 = 0.0f;
            // fallthrough
        case 19: // show triforce
            this->envLightMode = 8;

            if (this->csTimer >= 60) {
                this->envLightMode = 9;

                if (this->csTimer == 60) {
                    play->envCtx.unk_D8 = 1.0f;
                }
            }

            BossGanon_SetIntroCsCamera(this, 12);
            this->csCamEye.y += -6.0f;
            this->csCamEye.z += 6.0f;

            if (this->csTimer >= 30) {
                if (this->csTimer == 30) {
                    func_80078884(NA_SE_EV_TRIFORCE_MARK);
                }

                // fade in ganondorf's triforce
                Math_ApproachF(&this->fwork[GDF_TRIFORCE_PRIM_A], 255.0f, 1.0f, 10.0f);
                Math_ApproachF(&this->fwork[GDF_TRIFORCE_SCALE], 0.6f, 1.0f, 0.3f);
                Math_ApproachF(&this->fwork[GDF_TRIFORCE_PRIM_B], 170.0f, 1.0f, 2.55f);
                Math_ApproachF(&this->fwork[GDF_TRIFORCE_ENV_G], 200.0f, 1.0f, 3.0f);
            }

            if (this->csTimer == 17) {
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfRaiseHandLoopAnim, -5.0f);
            }

            if ((this->csTimer > 80) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 20;
                this->csTimer = 0;

                this->csCamTargetEye.x = this->csCamEye.x - 50.0f;
                this->csCamTargetEye.y = this->csCamEye.y - 100.0f;
                this->csCamTargetEye.z = this->csCamEye.z + 400.0f;

                this->csCamEyeMaxStep.x = 50.0f;
                this->csCamEyeMaxStep.y = 100.0f;
                this->csCamEyeMaxStep.z = 400.0f;

                this->csCamAtMaxStep.x = 400.0f;
                this->csCamMaxStepScale = 0.0f;

                this->csCamTargetAt.x = this->csCamAt.x + 400.0f;
                this->csCamTargetAt.y = this->csCamAt.y;
                this->csCamTargetAt.z = this->csCamAt.z;

                this->csCamMovementScale = 0.2f;

                this->fwork[GDF_VORTEX_ALPHA] = 0.0f;
                this->fwork[GDF_VORTEX_SCALE] = 0.1f;

                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_DARKWAVE);
            }
            break;

        case 20: // zoom cam out
            this->envLightMode = 10;
            moveCam = true;
            Math_ApproachF(&this->csCamMaxStepScale, 0.15f, 1.0f, 0.015f);

            if (this->csTimer <= 40) {
                Math_ApproachF(&this->fwork[GDF_VORTEX_ALPHA], 255.0f, 1.0f, 6.5f);
                Math_ApproachF(&this->fwork[GDF_VORTEX_SCALE], 0.2f, 1.0f, 0.025f);
            }

            if (this->csTimer > 20) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_DARKWAVE_M - SFX_FLAG);
            }

            if (this->csTimer > 20) {
                BossGanonEff_SpawnShock(play, 700.0f, GDF_SHOCK_PLAYER_PURPLE);
                BossGanonEff_SpawnShock(play, 700.0f, GDF_SHOCK_PLAYER_PURPLE);
            }

            if (this->csTimer == 30) {
                func_8002DF54(play, &this->actor, 0x4A);
            }

            if (this->csTimer <= 50) {
                break;
            }

            this->csState = 21;
            this->csTimer = 0;
            this->fwork[GDF_TRIFORCE_PRIM_A] = 0.0f;
            this->fwork[GDF_VORTEX_SCALE] = 0.16f;
            goto skip_sound_and_fx;

        case 21: // purple vortex
            this->envLightMode = 11;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_DARKWAVE_M - SFX_FLAG);
            BossGanonEff_SpawnShock(play, 700.0f, GDF_SHOCK_PLAYER_PURPLE);
            BossGanonEff_SpawnShock(play, 700.0f, GDF_SHOCK_PLAYER_PURPLE);

        skip_sound_and_fx:
            this->csCamEye.x = -30.0f;
            this->csCamEye.y = 37.0f;
            this->csCamEye.z = -30.0f;

            this->csCamAt.x = -10.0f;
            this->csCamAt.y = 45.0f;
            this->csCamAt.z = 0.0f;

            if (this->csTimer == 13) {
                Message_StartTextbox(play, 0x70CD, NULL);
            }

            if ((this->csTimer <= 120) || (Message_GetState(&play->msgCtx) != TEXT_STATE_NONE)) {
                break;
            }

            this->csState = 22;
            this->csTimer = 0;
            this->timers[2] = 30;
            this->organAlpha = 254;
            this->csCamAt.x = this->unk_1FC.x - 10.0f;
            this->csCamAt.y = this->unk_1FC.y + 30.0f;
            this->csCamAt.z = this->unk_1FC.z;
            this->fwork[GDF_VORTEX_ALPHA] = 255.0f;
            this->fwork[GDF_VORTEX_SCALE] = 0.2f;
            // fallthrough
        case 22: // start floating, show title card, start fight
            if (this->csTimer > 30) {
                this->envLightMode = 0;
            } else {
                this->envLightMode = 12;
            }

            Math_ApproachZeroF(&this->fwork[GDF_VORTEX_ALPHA], 1.0f, 10.0f);

            this->csCamEye.x = -30.0f;
            this->csCamEye.y = 137.0f;
            this->csCamEye.z = -110.0f;

            Math_ApproachF(&this->csCamAt.y, this->unk_1FC.y + 30.0f, 0.1f, 20.0f);
            Math_ApproachF(&this->csCamAt.x, this->unk_1FC.x - 10.0f, 0.1f, 5.0f);

            if (this->csTimer == 20) {
                BossGanon_SetAnimationObject(this, play, OBJECT_GANON_ANIME1);
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfGetUp3Anim, 0.0f);
                SkelAnime_Update(&this->skelAnime);
                this->actor.shape.yOffset = 0.0f;
                sBossGanonCape->attachShouldersTimer = 18.0f;
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_GANON_MANTLE);
                this->unk_198 = 0;
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_GANONDORF_BOSS);
            }

            if (this->csTimer == 50) {
                gSegments[6] = VIRTUAL_TO_PHYSICAL(
                    play->objectCtx.status[Object_GetIndex(&play->objectCtx, OBJECT_GANON)].segment);

                if (!(gSaveContext.eventChkInf[7] & 0x100)) {
                    TitleCard_InitBossName(play, &play->actorCtx.titleCtx,
                                           SEGMENTED_TO_VIRTUAL(gGanondorfTitleCardTex), 160, 180, 128, 40, false);
                }

                gSaveContext.eventChkInf[7] |= 0x100;
            }

            if (this->csTimer >= 20) {
                this->legSwayEnabled = true;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_FLOAT - SFX_FLAG);

                Math_ApproachF(&this->actor.world.pos.y, 228.0f, 0.05f, 2.0f);
                Math_ApproachF(&this->actor.world.pos.z, -230.0f, 0.05f, 4.0f);

                sBossGanonCape->backPush = -3.0f;
                sBossGanonCape->backSwayMagnitude = 0.25f;
                sBossGanonCape->sideSwayMagnitude = -3.0f;

                sin = Math_SinS(this->csTimer * 1500);
                this->actor.velocity.y = this->fwork[GDF_FWORK_0] * sin * 0.04f;
                this->actor.world.pos.y += this->actor.velocity.y;

                cos = Math_CosS(this->csTimer * 1800);
                this->actor.world.pos.x = this->fwork[GDF_FWORK_0] * cos * 0.5f;
                this->actor.velocity.x = this->actor.world.pos.x - this->actor.prevPos.x;

                Math_ApproachF(&this->fwork[GDF_FWORK_0], 50.0f, 1.0f, 1.0f);
            }

            if (this->csTimer > 30) {
                this->organAlpha -= 5;

                if (this->organAlpha < 0) {
                    this->organAlpha = 0;
                }
            }

            if (this->csTimer == 120) {
                mainCam = Play_GetCamera(play, MAIN_CAM);
                mainCam->eye = this->csCamEye;
                mainCam->eyeNext = this->csCamEye;
                mainCam->at = this->csCamAt;
                func_800C08AC(play, this->csCamIndex, 0);
                this->csState = this->csCamIndex = 0;
                func_80064534(play, &play->csCtx);
                func_8002DF54(play, &this->actor, 7);
                BossGanon_SetupWait(this, play);
            }

            if (sBossGanonZelda != NULL) {
                sBossGanonZelda->actor.world.pos.x = 0.0f;
                sBossGanonZelda->actor.world.pos.y = 350.0f;
                sBossGanonZelda->actor.world.pos.z = 0.0f;
            }
    }

    if (this->csCamIndex != 0) {
        if (moveCam) {
            Math_ApproachF(&this->csCamEye.x, this->csCamTargetEye.x, this->csCamMovementScale,
                           this->csCamEyeMaxStep.x * this->csCamMaxStepScale);
            Math_ApproachF(&this->csCamEye.y, this->csCamTargetEye.y, this->csCamMovementScale,
                           this->csCamEyeMaxStep.y * this->csCamMaxStepScale);
            Math_ApproachF(&this->csCamEye.z, this->csCamTargetEye.z, this->csCamMovementScale,
                           this->csCamEyeMaxStep.z * this->csCamMaxStepScale);

            Math_ApproachF(&this->csCamAt.x, this->csCamTargetAt.x, this->csCamMovementScale,
                           this->csCamAtMaxStep.x * this->csCamMaxStepScale);
            Math_ApproachF(&this->csCamAt.y, this->csCamTargetAt.y, this->csCamMovementScale,
                           this->csCamAtMaxStep.y * this->csCamMaxStepScale);
            Math_ApproachF(&this->csCamAt.z, this->csCamTargetAt.z, this->csCamMovementScale,
                           this->csCamAtMaxStep.z * this->csCamMaxStepScale);
        }

        Play_CameraSetAtEye(play, this->csCamIndex, &this->csCamAt, &this->csCamEye);
        Play_CameraSetFov(play, this->csCamIndex, this->csCamFov);
    }
}

void BossGanon_SetupDeathCutscene(BossGanon* this, PlayState* play) {
    s32 pad;
    s32 animBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_GANON_ANIME2);

    if (Object_IsLoaded(&play->objectCtx, animBankIndex)) {
        this->actionFunc = BossGanon_DeathAndTowerCutscene;
        this->csTimer = this->csState = 0;
        this->unk_198 = 1;
        this->animBankIndex = animBankIndex;
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[animBankIndex].segment);
        Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfDefeatedStartAnim, 0.0f);
        this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfDefeatedStartAnim);
        this->unk_508 = 0.0f;
    }
}

void BossGanon_SetupTowerCutscene(BossGanon* this, PlayState* play) {
    s32 pad;
    s32 animBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_GANON_ANIME2);

    if (Object_IsLoaded(&play->objectCtx, animBankIndex)) {
        this->animBankIndex = animBankIndex;
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[animBankIndex].segment);
        Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfDefeatedStartAnim, 0.0f);
        this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfDefeatedStartAnim);
        this->actionFunc = BossGanon_DeathAndTowerCutscene;
        this->csTimer = 0;
        this->csState = 100;
        this->unk_198 = 1;
        gSaveContext.magic = gSaveContext.magicCapacity;
        gSaveContext.health = gSaveContext.healthCapacity;
    } else {
        this->actionFunc = BossGanon_SetupTowerCutscene;
    }
}

void BossGanon_ShatterWindows(u8 windowShatterState) {
    s16 i;
    u8* tex1 = ResourceMgr_LoadTexByName(SEGMENTED_TO_VIRTUAL(ganon_boss_sceneTex_006C18));
    u8* tex2 = ResourceMgr_LoadTexByName(SEGMENTED_TO_VIRTUAL(ganon_boss_sceneTex_007418));
    u8* templateTex = ResourceMgr_LoadTexByName(SEGMENTED_TO_VIRTUAL(gGanondorfWindowShatterTemplateTex));

    for (i = 0; i < 2048; i++) {
        if ((tex1[i] != 0) && (Rand_ZeroOne() < 0.03f)) {
            if ((templateTex[i] == 0) || (windowShatterState == GDF_WINDOW_SHATTER_FULL)) {
                tex1[i] = tex2[i] = 1;
            }
        }
    }
}

void BossGanon_DeathAndTowerCutscene(BossGanon* this, PlayState* play) {
    static Color_RGBA8 bloodPrimColor = { 0, 120, 0, 255 };
    static Color_RGBA8 bloodEnvColor = { 0, 120, 0, 255 };

    if(CVar_GetS32("gRedGanonBlood", 0)) {
        bloodPrimColor.r = 120;
        bloodPrimColor.g = 0;

        bloodEnvColor.r = 120;
        bloodEnvColor.g = 0;
    }

    s16 i;
    u8 moveCam = false;
    Player* player = GET_PLAYER(play);
    s16 pad;
    Vec3f sp98;
    Vec3f sp8C;
    Vec3f sp80;
    Vec3f sp74;
    Camera* mainCam;
    Vec3f sp64;

    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->animBankIndex].segment);

    this->csTimer++;
    SkelAnime_Update(&this->skelAnime);

    switch (this->csState) {
        case 0:
            func_80064520(play, &play->csCtx);
            func_8002DF54(play, &this->actor, 8);
            this->csCamIndex = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, this->csCamIndex, CAM_STAT_ACTIVE);

            this->actor.world.pos.x = 0.0f;
            this->actor.world.pos.y = 70.0f;
            this->actor.world.pos.z = -80.0f;

            this->actor.shape.yOffset = -7000.0f;

            this->actor.shape.rot.y = 0;
            this->csState = 1;
            this->csTimer = 0;
            this->useOpenHand = true;
            // fallthrough
        case 1:
            player->actor.shape.rot.y = -0x8000;

            player->actor.world.pos.x = -10.0f;
            player->actor.world.pos.y = 0.0f;
            player->actor.world.pos.z = 115.0f;

            this->envLightMode = 13;

            if (this->csTimer < 30) {
                play->envCtx.unk_D8 = 0.0f;
            }

            if (this->csTimer >= 2) {
                play->envCtx.fillScreen = false;
            }

            this->csCamEye.x = -50.0f;
            this->csCamEye.z = -50.0f;
            this->csCamEye.y = 50.0f;

            this->csCamAt.x = this->unk_1FC.x;
            this->csCamAt.y = this->unk_1FC.y + 30.0f;
            this->csCamAt.z = this->unk_1FC.z;

            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfDefeatedLoopAnim, 0.0f);
                this->csState = 2;
                this->csTimer = 0;
            }
            break;

        case 2:
            this->csCamEye.x = -100.0f;
            this->csCamEye.y = 20.0f;
            this->csCamEye.z = -130.0f;

            this->envLightMode = 13;

            this->csCamAt.x = this->unk_1FC.x;
            this->csCamAt.y = this->unk_1FC.y;
            this->csCamAt.z = this->unk_1FC.z + 40.0f;

            if (this->csTimer >= 30) {
                this->csState = 3;
                this->csTimer = 0;
                Message_StartTextbox(play, 0x70CE, NULL);
                this->fwork[GDF_FWORK_1] = 1000.0f;
            }

            if ((this->unk_1A2 % 32) == 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_BREATH);
            }
            break;

        case 3:
            this->envLightMode = 14;

            if ((this->fwork[GDF_FWORK_1] > 100.0f) && ((this->unk_1A2 % 32) == 0)) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_BREATH);
            }

            this->csCamEye.x = 7.0f;
            this->csCamEye.y = 52.0f;
            this->csCamEye.z = -15.0f;

            this->csCamAt.x = this->unk_1FC.x - 5.0f;
            this->csCamAt.y = this->unk_1FC.y + 30.0f - 10.0f;
            this->csCamAt.z = this->unk_1FC.z;

            if ((this->fwork[GDF_FWORK_1] > 100.0f) && (this->csTimer > 100) &&
                (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfVomitStartAnim, 0.0f);
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfVomitStartAnim);
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_TOKETU);
            } else {
                if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1] - 16.0f)) {
                    for (i = 0; i < 40; i++) {
                        sp98.x = Rand_CenteredFloat(5.0f);
                        sp98.y = Rand_CenteredFloat(1.5f) + 1.0f;
                        sp98.z = Rand_ZeroFloat(5.0f) + 2.0f;

                        sp8C.x = 0.0f;
                        sp8C.y = -1.0f;
                        sp8C.z = 0.0f;

                        sp80.x = this->unk_208.x;
                        sp80.y = this->unk_208.y - 10.0f;
                        sp80.z = this->unk_208.z;

                        func_8002836C(play, &sp80, &sp98, &sp8C, &bloodPrimColor, &bloodEnvColor,
                                      (s16)Rand_ZeroFloat(50.0f) + 50, 0, 17);
                    }
                }

                if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                    Animation_MorphToLoop(&this->skelAnime, &gGanondorfVomitLoopAnim, 0.0f);
                    this->csState = 4;
                    this->csTimer = 0;
                }
            }
            break;

        case 4:
            this->envLightMode = 14;

            if (this->csTimer == 30) {
                Message_StartTextbox(play, 0x70CF, NULL);
                this->csState = 5;
                this->csTimer = 0;
            }
            break;

        case 5:
            this->envLightMode = 14;

            if ((this->csTimer > 70) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 6;
                this->csTimer = 0;
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfYellStartAnim, 0.0f);
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfYellStartAnim);

                this->csCamMovementScale = 0.05f;
                this->csCamMaxStepScale = 0.0f;

                this->csCamTargetEye.x = 7.0f;
                this->csCamTargetEye.y = 12.0f;
                this->csCamTargetEye.z = 70.0f;

                this->csCamTargetAt.x = this->unk_1FC.x - 5.0f;
                this->csCamTargetAt.y = (this->unk_1FC.y + 30.0f) - 10.0f;
                this->csCamTargetAt.z = this->unk_1FC.z;

                this->csCamEyeMaxStep.x = fabsf(this->csCamEye.x - this->csCamTargetEye.x);
                this->csCamEyeMaxStep.y = fabsf(this->csCamEye.y - this->csCamTargetEye.y);
                this->csCamEyeMaxStep.z = fabsf(this->csCamEye.z - this->csCamTargetEye.z);

                this->csCamAtMaxStep.x = fabsf(this->csCamAt.x - this->csCamTargetAt.x);
                this->csCamAtMaxStep.y = fabsf(this->csCamAt.y - this->csCamTargetAt.y);
                this->csCamAtMaxStep.z = fabsf(this->csCamAt.z - this->csCamTargetAt.z);

                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_CASBREAK);
            }
            break;

        case 6:
            this->envLightMode = 14;
            moveCam = true;
            Math_ApproachF(&this->csCamMaxStepScale, 0.2f, 1.0f, 0.01f);

            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfYellLoopAnim, 0.0f);
                this->csState = 7;
                this->csTimer = 0;
                this->unk_2E8 = 0;
                this->envLightMode = 15;
                this->unk_508 = 0.0f;
                this->fwork[GDF_FWORK_1] = 1000.0f;
                play->envCtx.unk_D8 = 0.0f;
            }
            break;

        case 7:
            if (this->csTimer < 10) {
                play->envCtx.unk_D8 = 0.0f;
            }

            if (this->csTimer == 30) {
                this->csState = 8;
                this->csTimer = 0;
                this->unk_70C = 0.0f;
            }
            goto skip_cam_and_quake;

        case 8:
            this->csCamEye.x = -60.0f;
            this->csCamEye.y = 80.0f;
            this->csCamEye.z = -130.0f;

            this->csCamAt.x = 0.0f;
            this->csCamAt.y = 0.0f;
            this->csCamAt.z = 70.0f;

            this->unk_70C = Math_SinS(this->csTimer * 0x6300) * 0.2f;

            func_80078884(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

        skip_cam_and_quake:
            this->envLightMode = 15;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_BODY_SPARK - SFX_FLAG);

            for (i = 1; i < 15; i++) {
                this->unk_4E4[i] = 0xA;
            }

            this->unk_2E6 = 20000;
            Math_ApproachF(&this->unk_508, 5.0f, 0.05f, 0.1f);

            if (this->csTimer == 30) {
                this->csState = 9;
                this->csTimer = 0;

                this->csCamEye.x = -30.0f;
                this->csCamEye.y = 40.0f;
                this->csCamEye.z = 60.0f;

                this->csCamAt.x = 492.0f;
                this->csCamAt.y = 43.0f;
                this->csCamAt.z = 580.0f;

                this->csCamMaxStepScale = 0.0f;
                this->unk_710 = 10.0f;
            }
            break;

        case 9:
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_BODY_SPARK - SFX_FLAG);

            if (this->csTimer == 2) {
                func_8002DF54(play, &this->actor, 0x39);
            }

            if (this->csTimer > 50) {
                Math_ApproachZeroF(&this->unk_710, 1.0f, 0.2f);
                Math_ApproachF(&this->csCamEye.x, 270.0f, 0.05f, this->csCamMaxStepScale * 30.0f);
                Math_ApproachF(&this->csCamEye.z, 260.0f, 0.05f, this->csCamMaxStepScale * 20.0f);
                Math_ApproachF(&this->csCamAt.y, 103.0f, 0.05f, this->csCamMaxStepScale * 6.0f);
                Math_ApproachF(&this->csCamAt.z, 280.0f, 0.05f, this->csCamMaxStepScale * 20.0f);
                Math_ApproachF(&this->csCamMaxStepScale, 1.0f, 1.0f, 0.01f);
            }

            this->unk_70C = Math_SinS(this->csTimer * 0x6300) * this->unk_710;
            func_80078884(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            if (this->csTimer < 100) {
                this->windowShatterState = GDF_WINDOW_SHATTER_PARTIAL;
                this->envLightMode = 15;
            } else {
                this->envLightMode = 16;
                this->windowShatterState = GDF_WINDOW_SHATTER_FULL;
            }

            if (this->csTimer >= 130) {
                Math_ApproachF(&this->whiteFillAlpha, 255.0f, 1.0f, 5.0f);
            }

            if (this->csTimer == 180) {
                play->sceneLoadFlag = 0x14;
                if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SKIP_TOWER_ESCAPE)) {
                    Flags_SetEventChkInf(0xC7);
                    play->nextEntranceIndex = 0x517;
                }
                else {
                    play->nextEntranceIndex = 0x43F;
                }
                play->fadeTransition = 5;
            }
            break;

        case 100:
            func_80064520(play, &play->csCtx);
            func_8002DF54(play, &this->actor, 8);
            this->csCamIndex = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, this->csCamIndex, CAM_STAT_ACTIVE);
            Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfCollapseAnim, 0.0f);
            this->fwork[1] = Animation_GetLastFrame(&gGanondorfDefeatedStartAnim);
            this->csState = 101;
            this->skelAnime.playSpeed = 0.0f;
            sBossGanonZelda = (EnZl3*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_ZL3, 0.0f,
                                                6000.0f, 0.0f, 0, 0, 0, 0x2000);

            player->actor.world.pos.x = -472.0f;
            player->actor.world.pos.y = 4102.0f;
            player->actor.world.pos.z = -130.0f;

            player->actor.shape.rot.y = -0x8000;

            this->actor.world.pos.x = -472.0f;
            this->actor.world.pos.y = 4172.0f;
            this->actor.world.pos.z = -400.0f;

            this->actor.shape.yOffset = -7000.0f;
            this->actor.shape.rot.y = 0;

            this->csCamEye.x = this->csCamAt.x = -472.0f;
            this->csCamEye.y = this->csCamAt.y = 4152.0f;
            this->csCamEye.z = -160.0f;

            this->csCamAt.z = -100.0f;

            sBossGanonCape->backPush = -2.0f;
            sBossGanonCape->backSwayMagnitude = 0.25f;
            sBossGanonCape->sideSwayMagnitude = -1.0f;
            sBossGanonCape->minDist = 0.0f;
            sBossGanonCape->minY = 4104.0f;
            sBossGanonCape->tearTimer = 20;

            this->whiteFillAlpha = 255.0f;
            play->envCtx.unk_D8 = 1.0f;
            // fallthrough
        case 101:
            player->actor.world.pos.y = 4102.0f;
            Math_ApproachZeroF(&this->whiteFillAlpha, 1.0f, 5.0f);

            if (this->csTimer > 40) {
                Math_ApproachF(&this->csCamEye.z, -520.0f, 0.1f, this->csCamMaxStepScale);
                Math_ApproachF(&this->csCamMaxStepScale, 5.0f, 1.0f, 0.1f);

                if (this->csTimer == 150) {
                    this->skelAnime.playSpeed = 1.0f;
                }

                if (this->csTimer == 160) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_PL_BOUND_NOWEAPON);
                }

                if (this->csTimer == 187) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_PL_BODY_HIT);
                }

                if (this->csTimer == 180) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EV_GANON_MANTLE);
                }

                if (this->csTimer == 190) {
                    sp74 = this->actor.world.pos;
                    sp74.y = 4102.0f;
                    BossGanonEff_SpawnDustDark(play, &sp74, 0.2f, 0.7f);
                }

                if (this->csTimer == 230) {
                    this->csState = 102;
                    this->csTimer = 0;
                }
            }
            break;

        case 102:
            player->actor.world.pos.y = 4102.0f;

            this->csCamEye.x = -442.0f;
            this->csCamEye.y = 4152.0f;
            this->csCamEye.z = -135.0f;

            this->csCamAt.x = -472.0f;
            this->csCamAt.y = 4152.0f;
            this->csCamAt.z = -135.0f;

            if (this->csTimer == 5) {
                func_8002DF54(play, &this->actor, 0x4C);
            }

            if (this->csTimer == 70) {
                func_8002DF54(play, &this->actor, 0x4D);
            }

            if (this->csTimer == 90) {
                this->csState = 103;
                this->csTimer = 0;
                sBossGanonZelda->actor.world.pos.x = -472.0f;
                sBossGanonZelda->actor.world.pos.y = 4352.0f;
                sBossGanonZelda->actor.world.pos.z = -200.0f;
                sBossGanonZelda->unk_3C8 = 3;
            }
            break;

        case 103:
            Audio_PlayActorSound2(&sBossGanonZelda->actor, NA_SE_EV_DOWN_TO_GROUND - SFX_FLAG);
            Math_ApproachF(&sBossGanonZelda->actor.world.pos.y, 4102.0f, 0.05f, 1.5f);

            this->csCamEye.x = -242.0f;
            this->csCamEye.y = 4122.0f;
            this->csCamEye.z = -190.0f;

            this->csCamAt.x = sBossGanonZelda->actor.world.pos.x;
            this->csCamAt.y = sBossGanonZelda->actor.world.pos.y + 40.0f + 5.0f;
            this->csCamAt.z = sBossGanonZelda->actor.world.pos.z;

            if (this->csTimer == 200) {
                sBossGanonZelda->actor.world.pos.y = 4102.0f;
                this->csState = 104;
                this->csTimer = 0;
            } else {
                break;
            }
            // fallthrough
        case 104:
            this->csCamEye.x = -432.0f;
            this->csCamEye.y = 4147.0f;
            this->csCamEye.z = -200.0f;

            this->csCamAt.x = sBossGanonZelda->actor.world.pos.x;
            this->csCamAt.y = sBossGanonZelda->actor.world.pos.y + 40.0f + 5.0f;
            this->csCamAt.z = sBossGanonZelda->actor.world.pos.z;

            if (this->csTimer >= 10) {
                Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.05f);
            }

            if (this->csTimer == 10) {
                sBossGanonZelda->unk_3C8 = 8;
            }

            if (this->csTimer == 50) {
                sBossGanonZelda->unk_3C8 = 4;
            }

            if (this->csTimer == 100) {
                this->csState = 105;
                this->csTimer = 0;
            }
            break;

        case 105:
            this->csCamEye.x = -450.0f;
            this->csCamEye.y = 4154.0f;
            this->csCamEye.z = -182.0f;

            this->csCamAt.x = sBossGanonZelda->actor.world.pos.x - 5.0f;
            this->csCamAt.y = sBossGanonZelda->actor.world.pos.y + 40.0f + 5.0f;
            this->csCamAt.z = sBossGanonZelda->actor.world.pos.z - 25.0f;

            if (this->csTimer == 10) {
                Message_StartTextbox(play, 0x70D0, NULL);
            }

            if ((this->csTimer > 100) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 1055;
                this->csTimer = 0;
            }
            break;

        case 1055:
            this->unk_70C = Math_SinS(this->csTimer * 0x6300) * 0.3f;
            func_80078884(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            if (this->csTimer == 20) {
                sBossGanonZelda->unk_3C8 = 5;
                func_8002DF54(play, &this->actor, 0x39);
            }

            if (this->csTimer == 40) {
                this->csState = 1056;
                this->csTimer = 0;
            }
            break;

        case 1056:
            this->unk_70C = Math_SinS(this->csTimer * 0x6300) * 0.3f;
            func_80078884(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            this->csCamEye.x = -503.0f;
            this->csCamEye.y = 4128.0f;
            this->csCamEye.z = -162.0f;

            this->csCamAt.x = -416.0f;
            this->csCamAt.y = 4181.0f;
            this->csCamAt.z = -75.0f;

            if (this->csTimer > 40) {
                this->csState = 1057;
                this->csTimer = 0;
            }
            break;

        case 1057:
            this->unk_70C = Math_SinS(this->csTimer * 0x6300) * (50.0f * this->csCamMovementScale);
            func_80078884(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            Math_ApproachF(&this->csCamEye.x, -1200.0f, 0.1f, this->csCamMovementScale * 697.0f);
            Math_ApproachF(&this->csCamEye.y, 4241.0f, 0.1f, this->csCamMovementScale * 113.0f);
            Math_ApproachF(&this->csCamEye.z, -1048.0f, 0.1f, this->csCamMovementScale * 886.0f);

            Math_ApproachF(&this->csCamMovementScale, 0.05f, 1.0f, 0.001f);

            if (this->csTimer > 80) {
                this->csState = 106;
                this->csTimer = 60;
            }
            break;

        case 106:
            this->csCamEye.x = -450.0f;
            this->csCamEye.y = 4154.0f;
            this->csCamEye.z = -182.0f;

            this->csCamAt.x = sBossGanonZelda->actor.world.pos.x - 5.0f;
            this->csCamAt.y = sBossGanonZelda->actor.world.pos.y + 40.0f + 5.0f;
            this->csCamAt.z = sBossGanonZelda->actor.world.pos.z - 25.0f;

            this->unk_70C = Math_SinS(this->csTimer * 0x6300) * 0.3f;
            func_80078884(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            if (this->csTimer == 70) {
                sBossGanonZelda->unk_3C8 = 6;
            }

            if (this->csTimer == 90) {
                Message_StartTextbox(play, 0x70D1, NULL);
            }

            if ((this->csTimer > 150) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 107;
                this->csTimer = 0;
                Message_StartTextbox(play, 0x70D2, NULL);
                func_8002DF54(play, &this->actor, 0x39);
            }
            break;

        case 107:
            this->unk_70C = Math_SinS(this->csTimer * 0x6300) * 0.8f;
            func_80078884(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            this->csCamEye.x = -380.0f;
            this->csCamEye.y = 4154.0f;
            this->csCamEye.z = -242.0f;

            this->csCamAt.x = (sBossGanonZelda->actor.world.pos.x - 5.0f) - 30.0f;
            this->csCamAt.y = (sBossGanonZelda->actor.world.pos.y + 40.0f + 5.0f) - 20.0f;
            this->csCamAt.z = (sBossGanonZelda->actor.world.pos.z - 25.0f) + 80.0f;

            if ((this->csTimer > 50) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                sBossGanonZelda->unk_3C8 = 7;
                this->csState = 108;
                this->csTimer = 0;
            }
            break;

        case 108:
            this->unk_70C = Math_SinS(this->csTimer * 0x6300) * 0.8f;
            func_80078884(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            this->csCamAt.x = (sBossGanonZelda->actor.world.pos.x - 5.0f) - 30.0f;
            this->csCamAt.y = (sBossGanonZelda->actor.world.pos.y + 40.0f + 5.0f) - 20.0f;
            this->csCamAt.z = (sBossGanonZelda->actor.world.pos.z - 25.0f) + 80.0f;

            if (this->csTimer > 50) {
                mainCam = Play_GetCamera(play, MAIN_CAM);

                mainCam->eye = this->csCamEye;
                mainCam->eyeNext = this->csCamEye;
                mainCam->at = this->csCamAt;

                func_800C08AC(play, this->csCamIndex, 0);
                this->csState = 109;
                this->csCamIndex = 0;
                func_80064534(play, &play->csCtx);
                func_8002DF54(play, &this->actor, 7);
                Flags_SetSwitch(play, 0x37);
            }
            break;

        case 109:
            func_80078884(NA_SE_EV_EARTHQUAKE - SFX_FLAG);
            break;
    }

    if (this->csState >= 100) {
        this->envLightMode = 20;
    }

    if (this->csCamIndex != 0) {
        if (moveCam) {
            Math_ApproachF(&this->csCamEye.x, this->csCamTargetEye.x, this->csCamMovementScale,
                           this->csCamEyeMaxStep.x * this->csCamMaxStepScale);
            Math_ApproachF(&this->csCamEye.y, this->csCamTargetEye.y, this->csCamMovementScale,
                           this->csCamEyeMaxStep.y * this->csCamMaxStepScale);
            Math_ApproachF(&this->csCamEye.z, this->csCamTargetEye.z, this->csCamMovementScale,
                           this->csCamEyeMaxStep.z * this->csCamMaxStepScale);
            Math_ApproachF(&this->csCamAt.x, this->csCamTargetAt.x, this->csCamMovementScale,
                           this->csCamAtMaxStep.x * this->csCamMaxStepScale);
            Math_ApproachF(&this->csCamAt.y, this->csCamTargetAt.y, this->csCamMovementScale,
                           this->csCamAtMaxStep.y * this->csCamMaxStepScale);
            Math_ApproachF(&this->csCamAt.z, this->csCamTargetAt.z, this->csCamMovementScale,
                           this->csCamAtMaxStep.z * this->csCamMaxStepScale);
        }

        sp64 = this->csCamAt;
        sp64.y += this->unk_70C;
        Play_CameraSetAtEye(play, this->csCamIndex, &sp64, &this->csCamEye);
    }
}

void BossGanon_SetupPoundFloor(BossGanon* this, PlayState* play) {
    this->unk_1C2 = 0;
    this->timers[0] = 40;
    this->actionFunc = BossGanon_PoundFloor;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->fwork[GDF_CENTER_POS] = 100.0f;
}

void BossGanon_PoundFloor(BossGanon* this, PlayState* play) {
    s16 i;
    f32 heightTarget;
    f32 targetPosX;
    f32 targetPosZ;
    Vec3f sp6C;
    Vec3f sp60;
    Vec3f sp54;
    Vec3f sp48;

    SkelAnime_Update(&this->skelAnime);

    switch (this->unk_1C2) {
        case 0:
            targetPosX = Math_SinS(this->unk_1A2 * 1280);
            targetPosX = targetPosX * this->fwork[GDF_CENTER_POS];
            targetPosZ = Math_CosS(this->unk_1A2 * 1792);
            targetPosZ = targetPosZ * this->fwork[GDF_CENTER_POS];

            Math_ApproachF(&this->actor.world.pos.x, targetPosX, 0.05f, this->fwork[GDF_FWORK_0]);
            Math_ApproachF(&this->actor.world.pos.z, targetPosZ, 0.05f, this->fwork[GDF_FWORK_0]);
            Math_ApproachF(&this->fwork[GDF_CENTER_POS], 0.0f, 1, 1.5f);

            if (this->timers[0] == 5) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_HIT_GND);
            }

            if (this->timers[0] < 14) {
                heightTarget = 250.0f;
                this->unk_258 += (Rand_ZeroFloat(M_PI / 2) + (M_PI / 2));
                Math_ApproachF(&this->handLightBallScale, 7.0f, 0.5f, 1.0f);
                this->envLightMode = 1;
            } else {
                heightTarget = 200.0f;
            }

            Math_ApproachF(&this->actor.world.pos.y, heightTarget, 0.1f, this->actor.velocity.y);
            Math_ApproachF(&this->actor.velocity.y, 20.0f, 1.0f, 1.0f);

            if (this->timers[0] == 14) {
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfPoundAnim);
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfPoundAnim, 0.0f);
                this->actor.velocity.y = 0.0f;
            }

            if (this->timers[0] == 0) {
                this->unk_1C2 = 1;
                this->actor.velocity.y = 0.0f;
            }
            break;

        case 1:
            sBossGanonCape->gravity = -1.0f;
            this->envLightMode = 1;
            Math_ApproachF(&this->actor.velocity.y, -50.0f, 1.0f, 10.0f);
            this->actor.world.pos.y += this->actor.velocity.y;

            if (this->actor.world.pos.y < 60.0f) {
                this->actor.world.pos.y = 60.0f;
                this->unk_1C2 = 2;
                this->timers[0] = 10;
                func_80033E88(&this->actor, play, 0xA, 0x14); // rumble
                this->unk_19C = 35;
                this->unk_19E = 0;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_HIT_GND_IMP);
                this->handLightBallScale = 0.0f;
                sp60 = this->unk_260;
                sp60.y = 0.0f;

                for (i = 0; i < 80; i++) {
                    sp6C.x = Rand_CenteredFloat(25.0f);
                    sp6C.y = Rand_ZeroFloat(17.0f);
                    sp6C.z = Rand_CenteredFloat(25.0f);
                    BossGanonEff_SpawnLightRay(play, &sp60, &sp6C, &sZeroVec, Rand_ZeroFloat(300.0f) + 500.0f,
                                               13.0f, 0x1E);
                }
            }
            break;

        case 2:
            this->envLightMode = 1;

            if (this->timers[0] == 0) {
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfPoundEndAnim);
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfPoundEndAnim, 0.0f);
                this->unk_1C2 = 3;
                this->unk_19F = 1;
                this->actor.velocity.y = 0.0f;
            }
            break;

        case 3:
            Math_ApproachF(&this->actor.world.pos.y, 150.0f, 0.1f, this->actor.velocity.y);
            Math_ApproachF(&this->actor.velocity.y, 20.0f, 1.0f, 1.0f);

            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfGetUp3Anim);
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfGetUp3Anim, 0.0f);
                SkelAnime_Update(&this->skelAnime);
                sBossGanonCape->attachShouldersTimer = 18.0f;
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_GANON_MANTLE);
                this->unk_1C2 = 4;
            }
            break;

        case 4:
            Math_ApproachF(&this->actor.world.pos.y, 150.0f, 0.1f, this->actor.velocity.y);
            Math_ApproachF(&this->actor.velocity.y, 20.0f, 1.0f, 1.0f);

            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                BossGanon_SetupWait(this, play);
            }
            break;
    }

    if ((this->unk_19C == 35) || (this->unk_19C == 30) || (this->unk_19C == 25)) {
        sp54 = this->actor.world.pos;
        sp54.y = 0.0f;
        BossGanonEff_SpawnDustLight(play, &sp54, 0, 3.0f, this->unk_19C - 25);
    }

    if (this->unk_19C == 35) {
        sp48 = this->actor.world.pos;
        sp48.y = 0.0f;
        BossGanonEff_SpawnShockwave(play, &sp48, 0, 3.0f);
    }
}

void BossGanon_SetupChargeBigMagic(BossGanon* this, PlayState* play) {
    this->unk_1C2 = 0;
    this->timers[0] = 30;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->fwork[GDF_CENTER_POS] = 100.0f;
    this->unk_1AA = Rand_ZeroFloat(20000.0f);
    this->unk_1AC = 0;
    this->actionFunc = BossGanon_ChargeBigMagic;
}

void BossGanon_ChargeBigMagic(BossGanon* this, PlayState* play) {
    s32 pad;
    f32 targetPosX;
    f32 targetPosZ;
    Vec3f sp80;
    Vec3f sp74;
    Vec3f sp68;
    s16 i;

    SkelAnime_Update(&this->skelAnime);

    targetPosX = Math_SinS(this->unk_1A2 * 1280);
    targetPosX = targetPosX * this->fwork[GDF_CENTER_POS];

    targetPosZ = Math_CosS(this->unk_1A2 * 1792);
    targetPosZ = targetPosZ * this->fwork[GDF_CENTER_POS];

    Math_ApproachF(&this->actor.world.pos.x, targetPosX, 0.05f, this->fwork[GDF_FWORK_0]);
    Math_ApproachF(&this->actor.world.pos.z, targetPosZ, 0.05, this->fwork[GDF_FWORK_0]);

    Math_ApproachF(&this->fwork[GDF_CENTER_POS], 0.0f, 1.0f, 1.5f);
    Math_ApproachF(&this->actor.world.pos.y, 200.0f, 0.05f, this->actor.velocity.y);
    Math_ApproachF(&this->actor.velocity.y, 20.0f, 1.0f, 1.0f);

    switch (this->unk_1C2) {
        case 0:
            if (this->timers[0] == 0) {
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfBigMagicChargeStartAnim);
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfBigMagicChargeStartAnim, 0.0f);
                this->unk_1C2 = 1;
            }
            break;

        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfBigMagicChargeHoldAnim);
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfBigMagicChargeHoldAnim, 0.0f);
                this->unk_1C2 = 2;
                this->timers[0] = 100;
            }
            break;

        case 2:
            this->envLightMode = 2;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_CHARGE_MASIC - SFX_FLAG);
            this->unk_278.x = this->unk_2EC[0].x;
            this->unk_278.y = this->unk_2EC[0].y + 50.0f + 30.0f;
            this->unk_278.z = this->unk_2EC[0].z;

            Math_ApproachF(&this->unk_284, 0.25f, 0.1f, 0.006f);
            Math_ApproachF(&this->unk_288, 255.0f, 1.0f, 255.0f);
            Math_ApproachF(&this->unk_28C, 0.25f, 0.1f, 0.006f);

            if ((this->timers[0] > 20) && (this->timers[0] < 60)) {
                Math_ApproachF(&this->unk_290, 255.0f, 1.0f, 15.0f);
            }

            if (this->timers[0] == 0) {
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfBigMagicWindupAnim);
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfBigMagicWindupAnim, 0.0f);
                this->unk_1C2 = 3;
                this->timers[0] = 6;
                this->timers[1] = 15;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_DARKWAVE);
                break;
            }

            Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x3E8);

            if (this->timers[0] < -4) {
                for (i = 0; i < ARRAY_COUNT(this->unk_294); i++) {
                    Math_ApproachF(&this->unk_294[i], 0.0f, 1.0f, 40.0f);
                }
            } else if ((this->timers[0] >= 7) && (this->timers[0] < 26)) {
                if (this->unk_1AC < ARRAY_COUNT(this->unk_294)) {
                    this->unk_1AC++;
                }

                for (i = 0; i < this->unk_1AC; i++) {
                    Math_ApproachF(&this->unk_294[i], 200.0f, 1.0f, 40.0f);
                }
            }

            if (this->timers[0] <= 30) {
                Math_ApproachF(&this->unk_284, 0.4f, 0.5f, 0.017f);
                this->unk_28C = this->unk_284;
            }

            if (this->timers[0] <= 30) {
                Math_ApproachF(&this->unk_2D0, 45.0f, 0.1f, 10.0f);
                this->lensFlareTimer = 1;
                this->lensFlareMode = 2;
                gCustomLensFlarePos = this->unk_278;
            }

            if (this->timers[0] == 47) {
                this->unk_274 = 1;
            }

            if (this->timers[0] == 46) {
                this->unk_274 = 2;
            }

            if (this->timers[0] == 45) {
                this->unk_274 = 3;
            }

            if (this->timers[0] == 44) {
                this->unk_274 = 4;
            }

            if (this->timers[0] == 43) {
                this->unk_274 = 5;
            }

            if (this->timers[0] == 42) {
                this->unk_274 = 6;
            }

            if (this->timers[0] > 30) {
                sp74.x = 0.0f;
                sp74.y = Rand_ZeroFloat(10.0f) + 150.0f;
                sp74.z = 0.0f;

                Matrix_RotateY(BINANG_TO_RAD(this->actor.yawTowardsPlayer), MTXMODE_NEW);
                Matrix_RotateZ(Rand_ZeroFloat(65536.0f), MTXMODE_APPLY);
                Matrix_MultVec3f(&sp74, &sp68);

                sp80.x = this->unk_278.x + sp68.x;
                sp80.y = this->unk_278.y + sp68.y;
                sp80.z = this->unk_278.z + sp68.z;

                BossGanonEff_SpawnBlackDot(play, &sp80, 20.0f);
            }
            break;

        case 3:
            this->envLightMode = 2;

            for (i = 0; i < ARRAY_COUNT(this->unk_294); i++) {
                Math_ApproachF(&this->unk_294[i], 0.0f, 1.0f, 40.0f);
            }

            if (this->timers[0] == 1) {
                sBossGanonCape->attachLeftArmTimer = 15.0f;
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_GANON_MANTLE);
            }

            if (this->timers[0] == 0) {
                Math_ApproachZeroF(&this->unk_284, 1.0f, 0.08f);
                this->unk_28C = this->unk_284;
                Math_ApproachZeroF(&this->unk_2D0, 1.0f, 10.0f);
                Math_ApproachF(&this->unk_278.x, this->unk_1FC.x, 0.5f, 30.0f);
                Math_ApproachF(&this->unk_278.y, this->unk_1FC.y, 0.5f, 30.0f);
                Math_ApproachF(&this->unk_278.z, this->unk_1FC.z, 0.5f, 30.0f);
            }

            if (this->timers[1] == 0) {
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfBigMagicThrowAnim);
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfBigMagicThrowAnim, 0.0f);
                this->unk_1C2 = 4;
                this->unk_288 = 0.0f;
                this->unk_290 = 0.0f;
                this->unk_284 = 0.0f;
                this->unk_28C = 0.0f;
            }
            break;

        case 4:
            this->envLightMode = 2;

            if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
                for (i = 0; i < 5; i++) {
                    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BOSS_GANON, this->unk_1FC.x,
                                       this->unk_1FC.y, this->unk_1FC.z, 0, this->actor.yawTowardsPlayer, 0, 0x104 + i);
                }

                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_BIGMASIC);
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_THROW_BIG);
            }

            if (Animation_OnFrame(&this->skelAnime, 3.0f)) {
                sBossGanonCape->attachShouldersTimer = 26.0f;
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_GANON_MANTLE);
            }

            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfBigMagicThrowEndAnim);
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfBigMagicThrowEndAnim, 0.0f);
                this->unk_1C2 = 5;
            }
            break;

        case 5:
            this->envLightMode = 2;

            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                BossGanon_SetupWait(this, play);
            }
            break;
    }
}

void BossGanon_SetupWait(BossGanon* this, PlayState* play) {
    BossGanon_SetAnimationObject(this, play, OBJECT_GANON_ANIME1);
    Animation_MorphToLoop(&this->skelAnime, &gGanondorfFloatAnim, -10.0f);
    this->actionFunc = BossGanon_Wait;
    this->fwork[GDF_FWORK_0] = 0.0f;
    this->timers[0] = (s16)Rand_ZeroFloat(64.0f) + 30;
    this->unk_1C2 = 0;
    sBossGanonCape->minY = 2.0f;
}

void BossGanon_Wait(BossGanon* this, PlayState* play) {
    f32 sin;
    s32 pad;
    f32 cos;
    Player* player = GET_PLAYER(play);

    this->legSwayEnabled = true;

    sBossGanonCape->backPush = -3.0f;
    sBossGanonCape->backSwayMagnitude = 0.25f;
    sBossGanonCape->sideSwayMagnitude = -3.0f;
    sBossGanonCape->minDist = 20.0f;

    SkelAnime_Update(&this->skelAnime);

    if ((this->unk_1C2 == 0) && !(player->actor.world.pos.y < 0.0f)) {
        if (!(player->stateFlags1 & 0x2000) && (fabsf(player->actor.world.pos.x) < 110.0f) &&
            (fabsf(player->actor.world.pos.z) < 110.0f)) {
            BossGanon_SetupPoundFloor(this, play);
        } else if ((this->timers[0] == 0) && !(player->stateFlags1 & 0x2000)) {
            this->timers[0] = (s16)Rand_ZeroFloat(30.0f) + 30;

            if ((s8)this->actor.colChkInfo.health >= 20) {
                BossGanon_SetupChargeLightBall(this, play);
            } else if (Rand_ZeroOne() >= 0.5f) {
                if ((Rand_ZeroOne() >= 0.5f) || (this->actor.xzDistToPlayer > 350.0f)) {
                    BossGanon_SetupChargeBigMagic(this, play);
                } else {
                    BossGanon_SetupPoundFloor(this, play);
                }
            } else {
                BossGanon_SetupChargeLightBall(this, play);
            }
        }
    }

    sin = Math_SinS(this->unk_1A2 * 1280) * 100.0f;
    cos = Math_CosS(this->unk_1A2 * 1792) * 100.0f;

    Math_ApproachF(&this->actor.world.pos.x, sin, 0.05f, this->fwork[GDF_FWORK_0]);
    Math_ApproachF(&this->actor.world.pos.y, 150.0f, 0.05f, this->fwork[GDF_FWORK_0] * 0.2f);
    Math_ApproachF(&this->actor.world.pos.z, cos, 0.05f, this->fwork[GDF_FWORK_0]);
    Math_ApproachF(&this->fwork[GDF_FWORK_0], 50.0f, 1.0f, 0.5f);

    this->actor.velocity.x = this->actor.world.pos.x - this->actor.prevPos.x;
    this->actor.velocity.z = this->actor.world.pos.z - this->actor.prevPos.z;

    sin = Math_SinS(this->unk_1A2 * 1500);
    this->actor.velocity.y = this->fwork[GDF_FWORK_0] * sin * 0.04f;
    this->actor.world.pos.y += this->actor.velocity.y;

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0xBB8);
    func_80078914(&this->actor.projectedPos, NA_SE_EN_FANTOM_FLOAT - SFX_FLAG);
}

void BossGanon_SetupChargeLightBall(BossGanon* this, PlayState* play) {
    BossGanon_SetAnimationObject(this, play, OBJECT_GANON_ANIME1);
    this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfChargeLightBallAnim);
    Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfChargeLightBallAnim, -3.0f);
    this->actionFunc = BossGanon_ChargeLightBall;
    this->timers[0] = 25;
}

void BossGanon_ChargeLightBall(BossGanon* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    sBossGanonCape->backPush = -3.0f;
    sBossGanonCape->backSwayMagnitude = 1.25f;
    sBossGanonCape->sideSwayMagnitude = -2.0f;
    sBossGanonCape->minDist = 10.0f;

    if (this->timers[0] < 17) {
        this->envLightMode = 1;
    }

    if (this->timers[0] == 17) {
        this->unk_26C = 10;
        this->unk_270 = Rand_ZeroFloat(M_PI);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_SPARK);
    }

    if (this->timers[0] < 10) {
        this->unk_258 += (Rand_ZeroFloat(M_PI / 2) + (M_PI / 2));
        Math_ApproachF(&this->handLightBallScale, 10.0f, 0.5f, 1.25f);

        if (this->timers[0] == 0) {
            BossGanon_SetupPlayTennis(this, play);
        }
    }

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x7D0);

    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.z += this->actor.velocity.z;

    Math_ApproachZeroF(&this->actor.velocity.x, 1.0f, 0.5f);
    Math_ApproachZeroF(&this->actor.velocity.z, 1.0f, 0.5f);

    this->actor.velocity.y = Math_SinS(this->unk_1A2 * 1500) * 2.0f;
    this->actor.world.pos.y += this->actor.velocity.y;
}

void BossGanon_SetupPlayTennis(BossGanon* this, PlayState* play) {
    BossGanon_SetAnimationObject(this, play, OBJECT_GANON_ANIME1);
    this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfThrowAnim);
    Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfThrowAnim, 0.0f);
    this->actionFunc = BossGanon_PlayTennis;
}

void BossGanon_PlayTennis(BossGanon* this, PlayState* play) {
    static AnimationHeader* volleyAnims[] = { &gGanondorfVolleyLeftAnim, &gGanondorfVolleyRightAnim };
    static s16 capeRightArmDurations[] = { 26, 20 };
    s16 rand;

    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->handLightBallScale, 1.0f, 0.2f);

    switch (this->unk_1C2) {
        case 0:
            this->envLightMode = 1;

            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->unk_1C2 = 1;
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfFloatAnim, 0.0f);
            }

            if (this->skelAnime.curFrame <= 12.0f) {
                this->lensFlareTimer = 2;
                this->lensFlareMode = 2;
                gCustomLensFlarePos = this->unk_260;
            }

            if (Animation_OnFrame(&this->skelAnime, 12.0f)) {
                this->handLightBallScale = 0.0f;
            }

            if (Animation_OnFrame(&this->skelAnime, 11.0f)) {
                this->unk_25C = 1;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_THROW);
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_THROW_MASIC);
                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BOSS_GANON, this->unk_260.x,
                                   this->unk_260.y, this->unk_260.z, 0, 0, 0, 0x64);
            }
            break;

        case 1:
            if (this->startVolley) {
                rand = Rand_ZeroOne() * 1.99f;
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(volleyAnims[rand]);
                Animation_MorphToPlayOnce(&this->skelAnime, volleyAnims[rand], 0.0f);
                sBossGanonCape->attachRightArmTimer = capeRightArmDurations[rand];
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_GANON_MANTLE);
                this->startVolley = false;
            }
            break;
    }

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x7D0);

    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.z += this->actor.velocity.z;

    Math_SmoothStepToF(&this->actor.velocity.x, 0.0f, 1.0f, 0.5f, 0.0f);
    Math_SmoothStepToF(&this->actor.velocity.z, 0.0f, 1.0f, 0.5f, 0.0f);

    this->actor.velocity.y = Math_SinS(this->unk_1A2 * 1500) * 2.0f;
    this->actor.world.pos.y += this->actor.velocity.y;
}

void BossGanon_SetupBlock(BossGanon* this, PlayState* play) {
    if ((this->actionFunc != BossGanon_Block) || (this->unk_1C2 != 0)) {
        BossGanon_SetAnimationObject(this, play, OBJECT_GANON_ANIME1);
        this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfBlockAnim);
        Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfBlockAnim, 0.0f);
        this->actionFunc = BossGanon_Block;
    }

    this->unk_1C2 = 0;
    sBossGanonCape->attachLeftArmTimer = this->timers[0] = 10;
    Audio_PlayActorSound2(&this->actor, NA_SE_EV_GANON_MANTLE);
    this->handLightBallScale = 0.0f;
}

void BossGanon_Block(BossGanon* this, PlayState* play) {
    this->collider.base.colType = 9;
    SkelAnime_Update(&this->skelAnime);
    sBossGanonCape->backPush = -9.0f;
    sBossGanonCape->backSwayMagnitude = 0.25f;
    sBossGanonCape->sideSwayMagnitude = -2.0f;
    sBossGanonCape->minDist = 13.0f;

    if (this->unk_1C2 == 0) {
        if (this->timers[0] == 0) {
            this->unk_1C2 = 1;
            Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfBlockReleaseAnim, 0.0f);
            this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfBlockReleaseAnim);
            SkelAnime_Update(&this->skelAnime);
            sBossGanonCape->attachShouldersTimer = 15.0f;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_GANON_MANTLE);
        }
    } else {
        sBossGanonCape->sideSwayMagnitude = -13.0f;

        if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
            BossGanon_SetupWait(this, play);
        }
    }

    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.y += this->actor.velocity.y;
    this->actor.world.pos.z += this->actor.velocity.z;

    Math_ApproachZeroF(&this->actor.velocity.x, 1.0f, 0.5f);
    Math_ApproachZeroF(&this->actor.velocity.y, 1.0f, 0.5f);
    Math_ApproachZeroF(&this->actor.velocity.z, 1.0f, 0.5f);
}

void BossGanon_SetupHitByLightBall(BossGanon* this, PlayState* play) {
    s16 i;

    BossGanon_SetAnimationObject(this, play, OBJECT_GANON_ANIME1);
    this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfBigMagicHitAnim);
    Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfBigMagicHitAnim, 0);
    this->timers[0] = 70;
    sBossGanonCape->attachRightArmTimer = sBossGanonCape->attachLeftArmTimer = 0;

    for (i = 1; i < 15; i++) {
        this->unk_4E4[i] = D_808E4C58[i];
    }

    this->unk_2E6 = 80;
    this->unk_2E8 = 0;
    this->actionFunc = BossGanon_HitByLightBall;
    this->actor.velocity.x = this->actor.velocity.z = 0.0f;
    this->unk_1C2 = 0;
    this->unk_1A6 = 15;
    this->unk_508 = 6.0f;
}

void BossGanon_HitByLightBall(BossGanon* this, PlayState* play) {
    s16 i;
    Vec3f sp50;

    SkelAnime_Update(&this->skelAnime);

    if (this->unk_1C2 == 0) {
        BossGanonEff_SpawnShock(play, 1500.0f, GDF_SHOCK_DORF_YELLOW);

        if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
            this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfLightArrowWaitAnim);
            Animation_MorphToLoop(&this->skelAnime, &gGanondorfLightArrowWaitAnim, 0.0f);
            this->unk_1C2 = 1;
        }
    } else if (this->unk_1C2 == 1) {
        BossGanonEff_SpawnShock(play, 1000.0f, GDF_SHOCK_DORF_YELLOW);

        if (this->timers[0] == 0) {
            this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfGetUp3Anim);
            Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfGetUp3Anim, 0.0f);
            this->unk_1C2 = 2;
            SkelAnime_Update(&this->skelAnime);
            sBossGanonCape->attachShouldersTimer = 18.0f;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_GANON_MANTLE);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_RESTORE);
            this->timers[2] = 130;
        }
    } else {
        if (Animation_OnFrame(&this->skelAnime, 7.0f)) {
            for (i = 0; i < 100; i++) {
                sp50.x = Rand_CenteredFloat(25.0f);
                sp50.y = Rand_CenteredFloat(25.0f);
                sp50.z = Rand_CenteredFloat(25.0f);

                BossGanonEff_SpawnSparkle(play, &this->unk_1FC, &sp50, &sZeroVec, Rand_ZeroFloat(200.0f) + 500.0f,
                                          0x14);
            }
            Audio_PlayActorSound2(&this->actor, NA_SE_PL_WALK_WATER2);
        }

        if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
            BossGanon_SetupWait(this, play);
        }
    }

    this->actor.velocity.y = Math_SinS(this->unk_1A2 * 1500) * 2.0f;
    this->actor.world.pos.y += this->actor.velocity.y;
}

void BossGanon_SetupVulnerable(BossGanon* this, PlayState* play) {
    s16 i;

    if (this->actionFunc != BossGanon_Vulnerable) {
        BossGanon_SetAnimationObject(this, play, OBJECT_GANON_ANIME1);
        this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfLightArrowHitAnim);
        Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfLightArrowHitAnim, 0.0f);
        sBossGanonCape->attachRightArmTimer = sBossGanonCape->attachLeftArmTimer = 0;
        this->actionFunc = BossGanon_Vulnerable;

        this->actor.velocity.x = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.velocity.z = 0.0f;

        this->unk_1C2 = 0;
        sBossGanonCape->backPush = -4.0f;
        sBossGanonCape->backSwayMagnitude = 0.75f;
        sBossGanonCape->sideSwayMagnitude = -3.0f;
        sBossGanonCape->minDist = 20.0f;

        for (i = 0; i < 10; i++) {
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BOSS_GANON, this->unk_1FC.x,
                               this->unk_1FC.y, this->unk_1FC.z, Rand_CenteredFloat(0x8000),
                               (s16)Rand_CenteredFloat(0x8000) + this->actor.yawTowardsPlayer, 0, 0xC8 + i);
        }

        this->unk_1A4 = 0;
        this->unk_288 = 0.0f;
        this->unk_290 = 0.0f;
        this->unk_284 = 0.0f;
        this->unk_28C = 0.0f;
    }
}

void BossGanon_Vulnerable(BossGanon* this, PlayState* play) {
    s16 i;
    Vec3f sp40;

    if (this->timers[3] == 0) {
        this->actor.flags |= ACTOR_FLAG_0;
    }

    SkelAnime_Update(&this->skelAnime);

    this->envLightMode = 1;
    this->actor.world.pos.y += this->actor.velocity.y;

    if (this->unk_1A4 < 0x28) {
        Math_ApproachF(&this->unk_508, 4.0f, 0.1f, 0.1f);
    }

    if ((this->unk_1A4 >= 0x28) && (this->unk_1A4 < 0x37)) {
        Math_ApproachF(&this->unk_508, 0.0f, 1.0f, 0.5f);
    }

    if (this->unk_1A4 >= 0x37) {
        Math_ApproachF(&this->unk_508, 5.0f, 0.1f, 0.15f);
        this->shockGlow = true;
    }

    switch (this->unk_1C2) {
        case 0:
            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->unk_1C2 = 1;
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfLightEnergyHitAnim);
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfLightEnergyHitAnim, 0.0f);
            }
            break;

        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->unk_1C2 = 2;
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfDownedAnim);
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfDownedAnim, 0.0f);
            }
            break;

        case 2:
            sBossGanonCape->minDist = 0.0f;
            this->actor.velocity.y = this->actor.velocity.y - 0.5f;

            if (this->actor.world.pos.y < 40.0f) {
                this->actor.world.pos.y = 40.0f;
                this->actor.velocity.y = 0.0f;
                this->unk_1C2 = 3;
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfLandAnim);
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfLandAnim, 0.0f);
                this->timers[0] = 70;
                this->actor.flags |= ACTOR_FLAG_10;
            }
            break;

        case 3:
            if (this->timers[0] == 68) {
                this->unk_19F = 1;
            }

            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->unk_1C2 = 4;
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfVulnerableAnim);
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfVulnerableAnim, 0.0f);
            }
            break;

        case 4:
            if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_DOWN);
            }

            if (this->timers[0] == 0) {
                this->unk_1C2 = 5;
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfGetUp1Anim);
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfGetUp1Anim, 0.0f);
                this->unk_2D4 = 80;

                for (i = 1; i < 15; i++) {
                    this->unk_4E4[i] = Rand_ZeroFloat(10.0f);
                }

                this->unk_2E6 = 80;
                this->unk_2E8 = 0;
                this->actor.flags &= ~ACTOR_FLAG_10;
            }
            break;

        case 5:
            BossGanonEff_SpawnShock(play, 1000.0f, GDF_SHOCK_DORF_YELLOW);

            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->unk_1C2 = 6;
                this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfGetUp2Anim);
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfGetUp2Anim, 0.0f);
                sBossGanonCape->minDist = 20.0f;
                this->unk_19F = 1;
            }
            break;

        case 6:
            this->envLightMode = 0;
            Math_ApproachF(&this->actor.world.pos.y, 200.0f, 0.1f, 1000.0f);

            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->unk_1C2 = 7;
            }
            break;

        case 7:
            this->envLightMode = 0;
            Math_ApproachF(&this->actor.world.pos.y, 150.0f, 0.05f, 30.0f);
            this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfGetUp3Anim);
            Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfGetUp3Anim, 0.0f);
            this->unk_1C2 = 8;
            SkelAnime_Update(&this->skelAnime);
            sBossGanonCape->attachShouldersTimer = 18.0f;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_GANON_MANTLE);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_RESTORE);
            break;

        case 8:
            this->envLightMode = 0;

            if (Animation_OnFrame(&this->skelAnime, 7.0f)) {
                for (i = 0; i < 100; i++) {
                    sp40.x = Rand_CenteredFloat(25.0f);
                    sp40.y = Rand_CenteredFloat(25.0f);
                    sp40.z = Rand_CenteredFloat(25.0f);
                    BossGanonEff_SpawnSparkle(play, &this->unk_1FC, &sp40, &sZeroVec,
                                              Rand_ZeroFloat(200.0f) + 500.0f, 0x14);
                }

                Audio_PlayActorSound2(&this->actor, NA_SE_PL_WALK_WATER2);
                this->timers[3] = 50;
            }

            if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                BossGanon_SetupWait(this, play);
            }
            break;
    }
}

void BossGanon_SetupDamaged(BossGanon* this, PlayState* play) {
    BossGanon_SetAnimationObject(this, play, OBJECT_GANON_ANIME1);
    this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfDamageAnim);
    Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfDamageAnim, 0.0f);
    this->actionFunc = BossGanon_Damaged;
}

void BossGanon_Damaged(BossGanon* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_0;

    SkelAnime_Update(&this->skelAnime);

    if (this->unk_1A4 >= 0x37) {
        Math_ApproachF(&this->unk_508, 5.0f, 0.1f, 0.15f);
        this->shockGlow = true;
    }

    if (Animation_OnFrame(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
        this->actionFunc = BossGanon_Vulnerable;
        this->unk_1C2 = 4;
        this->fwork[GDF_FWORK_1] = Animation_GetLastFrame(&gGanondorfVulnerableAnim);
        Animation_MorphToLoop(&this->skelAnime, &gGanondorfVulnerableAnim, 0.0f);
    }
}

void BossGanon_UpdateDamage(BossGanon* this, PlayState* play) {
    s16 i;
    s16 j;
    ColliderInfo* acHitInfo;

    if (this->collider.base.acFlags & 2) {
        this->unk_2D4 = 2;
        this->collider.base.acFlags &= ~2;
        acHitInfo = this->collider.info.acHitInfo;

        if ((this->actionFunc == BossGanon_HitByLightBall) || (this->actionFunc == BossGanon_ChargeBigMagic)) {
            if (acHitInfo->toucher.dmgFlags & 0x2000) {
                BossGanon_SetupVulnerable(this, play);
                this->timers[2] = 0;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_DAMAGE1);
                this->unk_1A6 = 15;
            }
        } else if ((this->actionFunc == BossGanon_Vulnerable) && (this->unk_1C2 >= 3)) {
            if (!(acHitInfo->toucher.dmgFlags & 0x80)) {
                u8 hitWithSword = false;
                u8 damage;
                Vec3f sp50;
                u32 flags;

                for (i = 0; i < 30; i++) {
                    sp50.x = Rand_CenteredFloat(20.0f);
                    sp50.y = Rand_CenteredFloat(20.0f);
                    sp50.z = Rand_CenteredFloat(20.0f);
                    BossGanonEff_SpawnSparkle(play, &this->unk_1FC, &sp50, &sZeroVec,
                                              Rand_ZeroFloat(200.0f) + 500.0f, 0x1E);
                }

                damage = flags = CollisionCheck_GetSwordDamage(acHitInfo->toucher.dmgFlags);

                if (flags == 0) {
                    damage = 2;
                } else {
                    hitWithSword = true;
                }

                if (((s8)this->actor.colChkInfo.health >= 3) || hitWithSword) {
                    this->actor.colChkInfo.health -= damage;
                }

                for (i = 0; i < ARRAY_COUNT(sBossGanonCape->strands); i++) {
                    for (j = 1; j < 12; j++) {
                        sBossGanonCape->strands[i].velocities[j].x = Rand_CenteredFloat(15.0f);
                        sBossGanonCape->strands[i].velocities[j].z = Rand_CenteredFloat(15.0f);
                    }
                }

                if ((s8)this->actor.colChkInfo.health <= 0) {
                    BossGanon_SetupDeathCutscene(this, play);
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_DEAD);
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_DD_THUNDER);
                    func_80078914(&sZeroVec, NA_SE_EN_LAST_DAMAGE);
                    Audio_QueueSeqCmd(0x100100FF);
                    this->screenFlashTimer = 4;
                    gSaveContext.sohStats.timestamp[TIMESTAMP_DEFEAT_GANONDORF] = GAMEPLAYSTAT_TOTAL_TIME;
                } else {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_DAMAGE2);
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_CUTBODY);
                    BossGanon_SetupDamaged(this, play);
                    this->unk_1A6 = 15;
                    sBossGanonCape->tearTimer = 1;
                }
            }
        } else if (acHitInfo->toucher.dmgFlags & 0x1F8A4) {
            Audio_PlayActorSound2(&this->actor, 0);

            for (i = 0; i < ARRAY_COUNT(sBossGanonCape->strands); i++) {
                for (j = 1; j < 12; j++) {
                    sBossGanonCape->strands[i].velocities[j].x = Rand_CenteredFloat(15.0f);
                    sBossGanonCape->strands[i].velocities[j].z = Rand_CenteredFloat(15.0f);
                }
            }
        }
    }
}

static f32 D_808E4D44[] = {
    1.0f, 3.0f, 0.0f, 7.0f, 13.0f, 4.0f, 6.0f, 11.0f, 5.0f, 2.0f, 8.0f, 14.0f, 10.0f, 12.0f, 9.0f,
};

void BossGanon_Update(Actor* thisx, PlayState* play2) {
    BossGanon* this = (BossGanon*)thisx;
    PlayState* play = play2;
    f32 legRotX;
    f32 legRotY;
    f32 legRotZ;
    Player* player = GET_PLAYER(play);
    s16 i;
    f32 sin;
    f32 cos;
    Vec3f shardPos;
    Vec3f shardVel;
    Vec3f spE8;
    s16 i2;
    s16 j;
    Vec3f spD8;
    Vec3f platformCheckPos;
    Actor* explosive;
    Vec3f spBC;
    Vec3f spB0;
    Vec3f platCheckPosBomb;
    Actor* prop;
    BgGanonOtyuka* platform;
    f32 targetLensFlareScale;
    f32 xOffset;
    f32 zOffset;

    if ((this->actionFunc != BossGanon_IntroCutscene) && (this->actionFunc != BossGanon_DeathAndTowerCutscene)) {
        BossGanon_SetAnimationObject(this, play, OBJECT_GANON_ANIME1);
    } else {
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->animBankIndex].segment);
    }

    if (this->windowShatterState != GDF_WINDOW_SHATTER_OFF) {
        BossGanon_ShatterWindows(this->windowShatterState);
        shardVel.y = 0.0f;

        for (i = 0; i < 10; i++) {
            shardPos.y = Rand_ZeroFloat(240.0f) + 20.0f;

            if (Rand_ZeroOne() < 0.5f) {
                shardPos.x = 463;
                shardPos.z = Rand_ZeroFloat(463.0f);

                shardVel.x = Rand_ZeroFloat(2.0f);
                shardVel.z = Rand_ZeroFloat(1.0f);
            } else {
                shardPos.z = 463;
                shardPos.x = Rand_ZeroFloat(463.0f);

                shardVel.z = Rand_ZeroFloat(2.0f);
                shardVel.x = Rand_ZeroFloat(1.0f);
            }

            BossGanonEff_SpawnWindowShard(play, &shardPos, &shardVel, Rand_ZeroFloat(0.075f) + 0.08f);
        }
    }

    this->collider.base.colType = 3;
    sBossGanonCape->gravity = -3.0f;
    this->shockGlow = false;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->unk_1A2++;
    this->unk_1A4++;

    // block players attack if hes shooting something
    if ((this->actionFunc == BossGanon_Wait) || (this->actionFunc == BossGanon_Block)) {
        if (player->unk_A73 != 0) {
            BossGanon_SetupBlock(this, play);
        }
    }

    this->actionFunc(this, play);

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }

    if (this->unk_1A6 != 0) {
        this->unk_1A6--;
    }

    if (this->unk_2D4 != 0) {
        this->unk_2D4--;
    }

    if (this->unk_2E8 != 0) {
        this->unk_2E8--;
    }

    if (this->unk_2E6 != 0) {
        this->unk_2E6--;
    }

    if (this->unk_19C != 0) {
        this->unk_19C--;
    }

    if (this->csState == 0) {
        BossGanon_UpdateDamage(this, play);
        BossGanon_SetColliderPos(&this->unk_1FC, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

        if (this->unk_2D4 == 0) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

            if ((this->actionFunc != BossGanon_HitByLightBall) && (this->actionFunc != BossGanon_Vulnerable) &&
                (this->actionFunc != BossGanon_Damaged)) {
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
            }
        }
    }

    if (this->legSwayEnabled) {
        sin = Math_SinS(-this->actor.shape.rot.y);
        cos = Math_CosS(-this->actor.shape.rot.y);

        legRotX = ((this->actor.velocity.z * sin) + (cos * this->actor.velocity.x)) * 300.0f;
        legRotY = ((-sin * this->actor.velocity.x) + (cos * this->actor.velocity.z)) * 300.0f;
        legRotZ = (Math_SinS(this->unk_1A2 * 2268) * -500.0f) - 500.0f;
    } else {
        legRotY = legRotX = legRotZ = 0.0f;
    }

    this->legSwayEnabled = false;

    Math_SmoothStepToF(&this->legRot.x, legRotX, 1.0f, 600.0f, 0.0f);
    Math_SmoothStepToF(&this->legRot.y, legRotY, 1.0f, 600.0f, 0.0f);
    Math_SmoothStepToF(&this->legRot.z, legRotZ, 1.0f, 100.0f, 0.0f);

    if (this->timers[2] == 1) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_LAUGH);
    }

    if (this->timers[2] == 100) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_ST_LAUGH);
        this->timers[2] = 0;
    }

    if ((this->unk_2E6 != 0) || (this->unk_2E8 != 0)) {
        for (i = 1; i < ARRAY_COUNT(this->unk_49C); i++) {
            if (this->unk_4E4[i] != 0) {
                this->unk_4E4[i]--;
                Math_ApproachF(&this->unk_49C[i], this->unk_508, 1.0f, 2.0f);
            } else {
                Math_ApproachZeroF(&this->unk_49C[i], 1.0f, 0.2f);
            }
        }

        // player hit, spawn shock and play sound
        if (this->unk_2E8 != 0) {
            func_80078914(&player->actor.projectedPos, NA_SE_PL_SPARK - SFX_FLAG);
            BossGanonEff_SpawnShock(play, 700.0f, GDF_SHOCK_PLAYER_YELLOW);
        }
    }

    if (this->unk_19F != 0) {
        this->unk_19F = 0;
        spE8 = this->actor.world.pos;
        spE8.y = 0.0f;
        BossGanonEff_SpawnDustDark(play, &spE8, 0.2, 0.7f);
        BossGanonEff_SpawnDustDark(play, &spE8, 0.3f, 0.8f);
    }

    if (this->unk_26C != 0) {
        this->unk_26C--;

        if (this->unk_26C == 0) {
            BossGanonEff_SpawnLightning(play, 1.0f, 0.0f, 0.0f);
        }

        BossGanonEff_SpawnLightning(play, 1.0f, D_808E4D44[this->unk_26C] * (M_PI / 5) + this->unk_270,
                                    Rand_CenteredFloat(M_PI / 5) + (M_PI / 2));
    }

    // see if light ball hit and should knock platform down?
    if ((this->unk_19C != 0) && (this->unk_19E < 4)) {
        if ((this->unk_19A == 0) && (this->unk_19C == 20)) {
            this->unk_19A = 1;
            platformCheckPos.x = -180.0f;
            platformCheckPos.y = 0.0f;

            for (i2 = 0; i2 < 4; i2++) {
                for (j = 0, platformCheckPos.z = -180.0f; j < 4; j++) {
                    BossGanon_CheckFallingPlatforms(this, play, &platformCheckPos);
                    platformCheckPos.z += 120.0f;
                }

                platformCheckPos.x += 120.0f;
            }
        } else if (this->unk_19C < 30) {
            spD8.x = 0.0f;
            spD8.y = 0.0f;
            spD8.z = 15.0f * (30.0f - this->unk_19C);

            Matrix_RotateY(Rand_ZeroFloat(6.2831855f), MTXMODE_NEW);
            Matrix_MultVec3f(&spD8, &platformCheckPos);

            this->unk_19E += BossGanon_CheckFallingPlatforms(this, play, &platformCheckPos);
        }
    }

    // see if a bomb exploded near a group of platforms and if they should fall
    explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;

    while (explosive != NULL) {
        if (explosive->params != BOMB_EXPLOSION) {
            explosive = explosive->next;
        } else {
            for (i = 0; i < 8; i++) {
                spBC.x = 0.0f;
                spBC.y = 0.0f;
                spBC.z = 60.0f;

                Matrix_RotateY(i * (M_PI / 4), MTXMODE_NEW);
                Matrix_MultVec3f(&spBC, &spB0);

                platCheckPosBomb.x = explosive->world.pos.x + spB0.x;
                platCheckPosBomb.y = explosive->world.pos.y;
                platCheckPosBomb.z = explosive->world.pos.z + spB0.z;

                BossGanon_CheckFallingPlatforms(this, play, &platCheckPosBomb);
            }

            explosive = explosive->next;
        }
    }

    BossGanon_UpdateEffects(play);

    prop = play->actorCtx.actorLists[ACTORCAT_PROP].head;

    // if a platform is lit up, change the room lighting
    while (prop != NULL) {
        if (prop->id != ACTOR_BG_GANON_OTYUKA) {
            prop = prop->next;
        } else {
            platform = (BgGanonOtyuka*)prop;

            if (platform->flashState != 0) {
                this->envLightMode = 1;
                break;
            }

            prop = prop->next;
        }
    }

    play->envCtx.unk_BF = 0;
    play->envCtx.unk_BE = 0;
    play->envCtx.unk_DC = 2;

    switch (this->envLightMode) {
        case -1:
            break;
        case 0:
            Math_ApproachF(&play->envCtx.unk_D8, 0.0f, 1.0f, 0.02f);
            break;
        case 1:
            play->envCtx.unk_BD = 1;
            Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.1f);
            break;
        case 2:
            play->envCtx.unk_BD = 1;
            Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.02f);
            break;
        case 3:
            play->envCtx.unk_BD = 3;
            play->envCtx.unk_D8 = 1.0f;
            break;
        case 35:
            play->envCtx.unk_BD = 0;
            play->envCtx.unk_D8 = 1.0f;
            break;
        case 4:
            play->envCtx.unk_BD = 4;
            play->envCtx.unk_D8 = 1.0f;
            break;
        case 5:
            play->envCtx.unk_BE = 5;
            play->envCtx.unk_BD = 3;
            Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.075f);
            break;
        case 6:
            play->envCtx.unk_BE = 5;
            play->envCtx.unk_D8 = 0.0f;
            break;
        case 65:
            play->envCtx.unk_BE = 3;
            play->envCtx.unk_BD = 6;
            Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.05f);
            break;
        case 7:
            play->envCtx.unk_BE = 7;
            play->envCtx.unk_D8 = 0.0f;
            break;
        case 75:
            play->envCtx.unk_BE = 4;
            play->envCtx.unk_BD = 8;
            Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.05f);
            break;
        case 8:
            play->envCtx.unk_BE = 3;
            play->envCtx.unk_BD = 9;
            Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.05f);
            break;
        case 9:
            play->envCtx.unk_BE = 3;
            play->envCtx.unk_BD = 0xA;
            Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.05f);
            break;
        case 10:
            play->envCtx.unk_BE = 3;
            play->envCtx.unk_BD = 0xB;
            Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.05f);
            this->unk_1A4 = 0;
            break;
        case 11:
            play->envCtx.unk_BE = 0xC;
            play->envCtx.unk_BD = 0xB;
            Math_ApproachF(&play->envCtx.unk_D8, (Math_CosS(this->unk_1A4 * 0x1800) * 0.5f) + 0.5f, 1.0f, 1.0f);
            break;
        case 12:
            play->envCtx.unk_BE = 0xC;
            play->envCtx.unk_BD = 3;
            Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.05f);
            break;
        case 13:
            play->envCtx.unk_BD = 0xD;
            Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.025f);
            break;
        case 14:
            play->envCtx.unk_BD = 0xE;
            play->envCtx.unk_D8 = 1.0f;
            break;
        case 15:
            play->envCtx.unk_BE = 0xE;
            play->envCtx.unk_BD = 0xF;
            Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.01f);
            break;
        case 16:
            play->envCtx.unk_BE = 0x10;
            play->envCtx.unk_BD = 0xF;
            Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.05f);
            break;
        case 20:
            play->envCtx.unk_BE = 2;
            play->envCtx.unk_BD = 1;
            break;
        default:
            break;
    }

    this->envLightMode = 0;

    if (this->whiteFillAlpha != 0) {
        play->envCtx.screenFillColor[3] = (s8)(u8)this->whiteFillAlpha;
        play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] =
            play->envCtx.screenFillColor[2] = 255;
        play->envCtx.fillScreen = true;
    } else if (this->screenFlashTimer != 0) {
        play->envCtx.fillScreen = true;
        play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] =
            play->envCtx.screenFillColor[2] = 255;

        play->envCtx.screenFillColor[3] = ((this->screenFlashTimer % 2) != 0) ? 100 : 0;

        this->screenFlashTimer--;
    } else {
        play->envCtx.fillScreen = play->envCtx.screenFillColor[3] = 0;
    }

    if (this->lensFlareTimer != 0) {
        this->lensFlareTimer--;

        if (this->lensFlareMode == 1) {
            targetLensFlareScale = 40.0f;
        } else if (this->lensFlareMode == 4) {
            targetLensFlareScale = 25.0f;
        } else {
            targetLensFlareScale = 10.0f;
        }

        Math_ApproachF(&this->lensFlareScale, targetLensFlareScale, 0.3f, 10.0f);
    } else {
        Math_ApproachZeroF(&this->lensFlareScale, 1.0f, 5.0f);

        if (this->lensFlareScale == 0.0f) {
            this->lensFlareMode = 0;
        }
    }

    if (this->lensFlareMode != 0) {
        gCustomLensFlareOn = true;

        if (this->lensFlareMode == 1) {
            gCustomLensFlarePos = this->actor.world.pos;
        }

        gLensFlareScale = this->lensFlareScale;
        gLensFlareColorIntensity = 10.0f;
        gLensFlareScreenFillAlpha = 0;
    } else {
        gCustomLensFlareOn = false;
    }

    if (this->unk_274 != 0) {
        i = this->unk_274 - 1;

        this->unk_278.x = this->unk_2EC[0].x;
        this->unk_278.y = this->unk_2EC[0].y + 50.0f + 30.0f;
        this->unk_278.z = this->unk_2EC[0].z;

        xOffset = (sinf(i * 1.2566371f) * 600.0f);
        zOffset = (cosf(i * 1.2566371f) * 600.0f);

        // 5 or 6 light balls that go into the charge. not the same as the ones that he throws
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BOSS_GANON, this->unk_1FC.x + xOffset,
                           this->unk_1FC.y, this->unk_1FC.z + zOffset, 0, (s16)(i * 13107.2f) + 0x6000, 0, 0xFA + i);
        this->unk_274 = 0;
    }
}

s32 BossGanon_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                               void* thisx) {
    BossGanon* this = (BossGanon*)thisx;

    switch (limbIndex) {
        case 10:
            if (this->useOpenHand) {
                *dList = gGanondorfRightHandOpenDL;
            }
            break;

        case 20:
            rot->y += this->legRot.x + this->legRot.z;
            rot->z += this->legRot.y;
            break;

        case 21:
            if (this->legRot.y > 0.0f) {
                rot->z += this->legRot.y;
            }
            break;

        case 22:
            rot->y += this->legRot.x + this->legRot.z;
            rot->z += this->legRot.y;
            break;

        case 23:
            rot->y += this->legRot.x - this->legRot.z;
            rot->z += this->legRot.y;
            break;

        case 24:
            if (this->legRot.y > 0.0f) {
                rot->z += this->legRot.y;
            }
            break;

        case 25:
            rot->y += this->legRot.x - this->legRot.z;
            rot->z += this->legRot.y;
            break;

        default:
            break;
    }

    return 0;
}

void BossGanon_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static s8 bodyPartLimbMap[] = {
        -1, -1, 1, -1, 3, 4, 5, -1, 6, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, 2, 12, 13, 14, 9, 10, 11, -1, -1, -1, -1,
    };
    static Vec3f D_808E4DA0 = { -500.0f, 200.0f, -300.0f };
    static Vec3f D_808E4DAC = { -500.0f, 200.0f, 300.0f };
    static Vec3f D_808E4DB8 = { 0.0f, 0.0f, 0.0f };
    static Vec3f D_808E4DC4 = { 0.0f, 0.0f, 0.0f };
    static Vec3f D_808E4DD0 = { 0.0f, 0.0f, 0.0f };
    static Vec3f D_808E4DDC = { 1300.0f, 0.0f, 0.0f };
    static Vec3f D_808E4DE8 = { 600.0f, 420.0f, 100.0f };
    s8 bodyPart;
    BossGanon* this = (BossGanon*)thisx;

    bodyPart = bodyPartLimbMap[limbIndex];
    if (bodyPart >= 0) {
        Matrix_MultVec3f(&D_808E4DB8, &this->unk_2EC[bodyPart]);
    }

    if (limbIndex == 2) {
        Matrix_MultVec3f(&D_808E4DB8, &this->unk_1FC);
    } else if (limbIndex == 19) {
        Matrix_MultVec3f(&D_808E4DB8, &this->actor.focus.pos);
    } else if (limbIndex == 11) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_MultVec3f(&D_808E4DB8, &this->unk_208);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gGanondorfEyesDL));

        CLOSE_DISPS(play->state.gfxCtx);
    } else if (limbIndex == 6) {
        Matrix_MultVec3f(&D_808E4DC4, &this->unk_238);
    } else if (limbIndex == 10) {
        Matrix_MultVec3f(&D_808E4DD0, &this->unk_22C);

        if (this->unk_25C == 0) {
            Matrix_MultVec3f(&D_808E4DDC, &this->unk_260);
        }

        this->unk_25C = 0;

        if (this->triforceType == GDF_TRIFORCE_DORF) {
            Matrix_MultVec3f(&D_808E4DE8, &this->triforcePos);
        }
    } else if (limbIndex == 4) {
        Vec3f sp28 = D_808E4DA0;

        if (this->unk_198 == 1) {
            sp28.x += -300.0f;
            sp28.y += -300.0f;
            sp28.z += 700.0f;
        } else if (this->unk_198 == 2) {
            sp28.x += -300.0f;
            sp28.z += 700.0f;
        }

        Matrix_MultVec3f(&sp28, &this->unk_220);
    } else if (limbIndex == 8) {
        Vec3f sp1C = D_808E4DAC;

        if (this->unk_198 == 1) {
            sp1C.x += -300.0f;
            sp1C.y += -300.0f;
            sp1C.z += -700.0f;
        } else if (this->unk_198 == 2) {
            sp1C.x += -300.0f;
            sp1C.y += 100.0f;
            sp1C.z += -700.0f;
        }

        Matrix_MultVec3f(&sp1C, &this->unk_214);
    }
}

void BossGanon_InitRand(s32 seedInit0, s32 seedInit1, s32 seedInit2) {
    sBossGanonSeed1 = seedInit0;
    sBossGanonSeed2 = seedInit1;
    sBossGanonSeed3 = seedInit2;
}

f32 BossGanon_RandZeroOne(void) {
    // Wichmann-Hill algorithm
    f32 randFloat;

    sBossGanonSeed1 = (sBossGanonSeed1 * 171) % 30269;
    sBossGanonSeed2 = (sBossGanonSeed2 * 172) % 30307;
    sBossGanonSeed3 = (sBossGanonSeed3 * 170) % 30323;

    randFloat = (sBossGanonSeed1 / 30269.0f) + (sBossGanonSeed2 / 30307.0f) + (sBossGanonSeed3 / 30323.0f);

    while (randFloat >= 1.0f) {
        randFloat -= 1.0f;
    }

    return fabsf(randFloat);
}

void BossGanon_DrawShock(BossGanon* this, PlayState* play) {
    s32 pad;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 i;

    OPEN_DISPS(gfxCtx);

    if ((this->unk_2E8 != 0) || (this->unk_2E6 != 0)) {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
        gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightBallMaterialDL);

        if (this->unk_2E8 != 0) {
            Player* player = GET_PLAYER(play);

            for (i = 0; i < ARRAY_COUNT(player->bodyPartsPos); i++) {
                FrameInterpolation_RecordOpenChild("Ganondorf Shock 0", i);

                Matrix_Translate(player->bodyPartsPos[i].x, player->bodyPartsPos[i].y, player->bodyPartsPos[i].z,
                                 MTXMODE_NEW);
                Matrix_ReplaceRotation(&play->billboardMtxF);
                Matrix_Scale(this->unk_49C[i], this->unk_49C[i], this->unk_49C[i], MTXMODE_APPLY);
                Matrix_RotateZ(Rand_CenteredFloat(M_PI), MTXMODE_APPLY);
                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, gGanondorfSquareDL);
                
                FrameInterpolation_RecordCloseChild();
            }
        } else {
            for (i = 1; i < 15; i++) {
                FrameInterpolation_RecordOpenChild("Ganondorf Shock 1", i);

                Matrix_Translate(this->unk_2EC[i].x, this->unk_2EC[i].y, this->unk_2EC[i].z, MTXMODE_NEW);
                Matrix_ReplaceRotation(&play->billboardMtxF);
                Matrix_Scale(this->unk_49C[i], this->unk_49C[i], this->unk_49C[i], MTXMODE_APPLY);

                if (!this->shockGlow) {
                    Matrix_RotateZ(Rand_CenteredFloat(M_PI), MTXMODE_APPLY);
                }

                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

                if (this->shockGlow) {
                    gSPSegment(POLY_XLU_DISP++, 0x08,
                               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                                (this->unk_1A2 + i) * -15, 32, 64));
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 255, 170, 255);
                    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 128);
                    gSPDisplayList(POLY_XLU_DISP++, gGanondorfShockGlowDL);
                } else {
                    gSPDisplayList(POLY_XLU_DISP++, gGanondorfSquareDL);
                }

                FrameInterpolation_RecordCloseChild();
            }
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void BossGanon_DrawHandLightBall(BossGanon* this, PlayState* play) {
    s32 pad;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 alpha;

    OPEN_DISPS(gfxCtx);

    if (this->handLightBallScale > 0.0f) {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);

        if ((this->unk_1A2 % 2) != 0) {
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, 100, 255, 0, 0);
        }

        gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightBallMaterialDL);

        Matrix_Translate(this->unk_260.x, this->unk_260.y, this->unk_260.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(this->handLightBallScale, this->handLightBallScale, this->handLightBallScale, MTXMODE_APPLY);
        Matrix_RotateZ(this->unk_258, 1);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gGanondorfSquareDL);

        alpha = ((this->unk_1A2 % 2) != 0) ? 100 : 80;
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 155, alpha);
        Matrix_Translate(this->unk_260.x, 0.0f, this->unk_260.z, MTXMODE_NEW);
        Matrix_Scale(this->handLightBallScale * 0.75f, 1.0f, this->handLightBallScale * 0.75f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightCoreDL);
    }

    CLOSE_DISPS(gfxCtx);
}

void BossGanon_DrawBigMagicCharge(BossGanon* this, PlayState* play) {
    s32 pad;
    f32 yRot;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 i;

    OPEN_DISPS(gfxCtx);

    if (this->unk_284 > 0.0f) {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);

        // light flecks
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, (s8)this->unk_290);
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 255, 0, 128);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, this->unk_1A2 * -2, 0, 0x40, 0x40, 1, 0,
                                    this->unk_1A2 * 0xA, 0x40, 0x40));
        Matrix_Translate(this->unk_278.x, this->unk_278.y, this->unk_278.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(this->unk_28C, this->unk_28C, this->unk_28C, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightFlecksDL);

        // background circle texture
        Matrix_Translate(this->unk_278.x, this->unk_278.y, this->unk_278.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(this->unk_284, this->unk_284, this->unk_284, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 0, 100, (s8)this->unk_288);
        gSPSegment(
            POLY_XLU_DISP++, 0x09,
            Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x20, 1, 0, this->unk_1A2 * -4, 0x20, 0x20));
        gSPDisplayList(POLY_XLU_DISP++, gGanondorfBigMagicBGCircleDL);

        // yellow background dot
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 150, 170, 0, (s8)this->unk_288);
        gSPSegment(POLY_XLU_DISP++, 0x0A,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x20, 1, this->unk_1A2 * 2,
                                    this->unk_1A2 * -0x14, 0x40, 0x40));
        gSPDisplayList(POLY_XLU_DISP++, gGanondorfDotDL);

        // light ball material
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 100, 0);
        gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightBallMaterialDL);

        // light ball geometry
        Matrix_Translate(this->unk_278.x, this->unk_278.y, this->unk_278.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(this->unk_2D0, this->unk_2D0, this->unk_2D0, MTXMODE_APPLY);
        Matrix_RotateZ((this->unk_1A2 * 10.0f) / 1000.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gGanondorfSquareDL);

        BossGanon_InitRand(this->unk_1AA + 1, 0x71AC, 0x263A);
        Matrix_Translate(this->unk_278.x, this->unk_278.y, this->unk_278.z, MTXMODE_NEW);
        Matrix_RotateY((this->unk_1A2 * 10.0f) / 1000.0f, MTXMODE_APPLY);
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 255, 0, 0);

        yRot = BINANG_TO_RAD(this->actor.yawTowardsPlayer);

        for (i = 0; i < this->unk_1AC; i++) {
            FrameInterpolation_RecordOpenChild("Ganondorf Big Magic", i);

            f32 xzRot = (BossGanon_RandZeroOne() - 0.5f) * M_PI * 1.5f;

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s8)this->unk_294[i]);
            Matrix_Push();
            Matrix_RotateY(xzRot + yRot, MTXMODE_APPLY);
            Matrix_RotateX((BossGanon_RandZeroOne() - 0.5f) * M_PI, MTXMODE_APPLY);
            Matrix_RotateZ(xzRot, MTXMODE_APPLY);
            Matrix_Translate(0.0f, 0.0f, 50.0f, MTXMODE_APPLY);
            Matrix_Scale(4.0f, 4.0f, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightRayTriDL);

            Matrix_Pop();

            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void BossGanon_DrawTriforce(BossGanon* this, PlayState* play) {
    s32 pad;

    if (this->fwork[GDF_TRIFORCE_PRIM_A] > 0.0f) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();

        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, (u8)this->fwork[GDF_TRIFORCE_PRIM_B],
                        (s8)this->fwork[GDF_TRIFORCE_PRIM_A]);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, (u8)this->fwork[GDF_TRIFORCE_ENV_G], 0, 128);

        if (this->triforceType == GDF_TRIFORCE_PLAYER) {
            Player* player = GET_PLAYER(play);

            this->triforcePos = player->bodyPartsPos[12];

            this->triforcePos.x += -0.6f;
            this->triforcePos.y += 3.0f;
            this->triforcePos.z += -2.0f;
        } else if (this->triforceType == GDF_TRIFORCE_ZELDA) {
            this->triforcePos = sBossGanonZelda->unk_31C;

            this->triforcePos.y += 1.8f;
            this->triforcePos.z += 4.0f;
        }

        Matrix_Translate(this->triforcePos.x, this->triforcePos.y, this->triforcePos.z, MTXMODE_NEW);

        if (this->triforceType == GDF_TRIFORCE_PLAYER) {
            Matrix_RotateX(-1.4f, MTXMODE_APPLY);
            Matrix_RotateZ(4.0f, MTXMODE_APPLY);
        } else if (this->triforceType == GDF_TRIFORCE_ZELDA) {
            Matrix_RotateY(1.5f, 1);
            Matrix_RotateX(1.1f, 1);
            Matrix_RotateZ(-0.99999994f, MTXMODE_APPLY);
        } else {
            Matrix_ReplaceRotation(&play->billboardMtxF);
        }

        Matrix_Scale(this->fwork[GDF_TRIFORCE_SCALE], this->fwork[GDF_TRIFORCE_SCALE], 1.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gGanondorfTriforceDL));

        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void BossGanon_DrawDarkVortex(BossGanon* this, PlayState* play) {
    s32 pad;

    if (this->fwork[GDF_VORTEX_ALPHA] > 0.0f) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        gDPPipeSync(POLY_XLU_DISP++);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, this->unk_1A2 * -8, 0, 0x20, 0x40, 1,
                                    this->unk_1A2 * -4, this->unk_1A2 * -8, 0x20, 0x20));
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 0, 200, (s8)this->fwork[GDF_VORTEX_ALPHA]);
        gDPSetEnvColor(POLY_XLU_DISP++, 130, 0, 0, 128);

        if (this->csState != 21) {
            Matrix_Translate(0.0f, 105.0f, -400.0f, MTXMODE_NEW);
            Matrix_RotateX(M_PI / 2, MTXMODE_APPLY);
        } else {
            Matrix_Translate(-50.0f, 50.0f, -150.0f, MTXMODE_NEW);
            Matrix_RotateY(M_PI / 10, MTXMODE_APPLY);
            Matrix_RotateX(M_PI / 2, MTXMODE_APPLY);
        }

        Matrix_Scale(this->fwork[GDF_VORTEX_SCALE], this->fwork[GDF_VORTEX_SCALE], this->fwork[GDF_VORTEX_SCALE],
                     MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gGanondorfVortexDL));

        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void func_808E0254(BossGanon* this, u8* tex, f32 arg2) {
    static s16 D_808E4DF4[] = { 1, 2, 3, 3, 2, 1 };
    static s16 D_808E4E00[] = { 2, 3, 4, 4, 4, 3, 2 };
    static s16 D_808E4E10[] = { 2, 3, 4, 4, 4, 4, 3, 2 };
    static s16 D_808E4E20[] = { 2, 4, 5, 5, 6, 6, 6, 6, 5, 5, 4, 2 };
    static s16 D_808E4E38[] = { 1, -1, 1, 1, 3, 4, 1, 6, 7, 2, 9, 10, 2, 12, 13 };
    static u8 D_808E4E58[] = { 3, 2, 2, 1, 3, 3, 1, 3, 3, 1, 0, 3, 1, 0, 3 };
    s16 baseX;
    s16 index;
    s16 i;
    s16 baseY;
    s16 x;
    s16 addY;
    f32 lerpX;
    s16 y;
    f32 lerpY;
    f32 lerpZ;
    Vec3f sp68;
    Vec3f sp5C;

    for (i = 0; i < 15; i++) {
        if (arg2 == 0.0f || (y = D_808E4E38[i]) >= 0) {
            if (arg2 > 0.0f) {
                lerpX = this->unk_2EC[i].x + (this->unk_2EC[y].x - this->unk_2EC[i].x) * arg2;
                lerpY = this->unk_2EC[i].y + (this->unk_2EC[y].y - this->unk_2EC[i].y) * arg2;
                lerpZ = this->unk_2EC[i].z + (this->unk_2EC[y].z - this->unk_2EC[i].z) * arg2;

                sp68.x = lerpX - this->actor.world.pos.x;
                sp68.y = lerpY - this->actor.world.pos.y + 76 + 30 + 30;
                sp68.z = lerpZ - this->actor.world.pos.z;
            } else {

                sp68.x = this->unk_2EC[i].x - this->actor.world.pos.x;
                sp68.y = this->unk_2EC[i].y - this->actor.world.pos.y + 76 + 30 + 30;
                sp68.z = this->unk_2EC[i].z - this->actor.world.pos.z;
            }
            Matrix_MultVec3f(&sp68, &sp5C);

            sp5C.x *= 0.4f;
            sp5C.y *= 0.4f;

            baseX = (s16)(sp5C.x + 32.0f);
            baseY = (s16)sp5C.y * 64;

            if (D_808E4E58[i] == 2) {
                for (y = 0, addY = -0x180; y < 12; y++, addY += 0x40) {
                    for (x = -D_808E4E20[y]; x < D_808E4E20[y]; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < 0x1000)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else if (D_808E4E58[i] == 1) {
                for (y = 0, addY = -0x100; y < 8; y++, addY += 0x40) {
                    for (x = -D_808E4E10[y]; x < D_808E4E10[y]; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < 0x1000)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else if (D_808E4E58[i] == 0) {
                for (y = 0, addY = -0xC0; y < 7; y++, addY += 0x40) {
                    for (x = -D_808E4E00[y]; x < D_808E4E00[y] - 1; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < 0x1000)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else {
                for (y = 0, addY = -0x80; y < 6; y++, addY += 0x40) {
                    for (x = -D_808E4DF4[y]; x < D_808E4DF4[y] - 1; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < 0x1000)) {
                            tex[index] = 255;
                        }
                    }
                }
            }
        }
    }
}

void BossGanon_GenShadowTexture(u8* tex, BossGanon* this, PlayState* play) {
    s16 addY;
    s16 baseX;
    s16 baseY;
    s16 i;
    s16 j;
    s16 y;
    s16 x;
    s16 index;
    Vec3f sp7C;
    Vec3f sp70;
    s32* ptr = (s32*)tex;

    for (i = 0; i < 64 * 64 / 4; i++, ptr++) {
        *ptr = 0;
    }

    Matrix_RotateX(1.0f, MTXMODE_NEW);

    for (i = 0; i <= 5; i++) {
        func_808E0254(this, tex, i / 5.0f);
    }

    for (i = 0; i < 12; i++) {
        for (j = 0; j < 12; j++) {
            sp7C.x = sBossGanonCape->strands[i].joints[j].x - this->actor.world.pos.x;
            sp7C.y = sBossGanonCape->strands[i].joints[j].y - this->actor.world.pos.y + 76.0f + 100.0f + 30.0f;
            sp7C.z = sBossGanonCape->strands[i].joints[j].z - this->actor.world.pos.z;

            Matrix_MultVec3f(&sp7C, &sp70);

            sp70.x = sp70.x * 0.28f;
            sp70.y = sp70.y * 0.28f;

            baseX = (s32)(sp70.x + 32.0f);
            baseY = (s16)sp70.y * 0x40;

            if (!sBossGanonCape->strands[i].torn[j]) {
                for (y = -1, addY = -0x40; y <= 1; y++, addY += 0x40) {
                    for (x = -3; x <= 3; x++) {
                        index = baseX + x + baseY + addY;
                        if (0 <= index && index < 0x1000) {
                            tex[index] = 255;
                        }
                    }
                }
            } else {
                for (y = -1, addY = -0x40; y <= 1; y++, addY += 0x40) {
                    for (x = -1; x <= 1; x++) {
                        index = baseX + x + baseY + addY;
                        if (0 <= index && index < 0x1000) {
                            tex[index] = 255;
                        }
                    }
                }
            }
        }
    }
}

void BossGanon_DrawShadowTexture(void* tex, BossGanon* this, PlayState* play) {
    s32 pad;
    f32 zOffset;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, 50);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    if (this->csState < 100) {
        zOffset = (((((this->actor.world.pos.y - 10) + 70.0f) * -5.0f) / 10.0f) + 10.0f);
        Matrix_Translate(this->actor.world.pos.x, 0.0f, this->actor.world.pos.z + zOffset, MTXMODE_NEW);
    } else {
        Matrix_Translate(this->actor.world.pos.x, 4102.0f, this->actor.world.pos.z - 20.0f, MTXMODE_NEW);
    }

    Matrix_Scale(0.95000005f, 1.0f, 0.95000005f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gGanondorfShadowSetupDL);
    gDPLoadTextureBlock(POLY_OPA_DISP++, tex, G_IM_FMT_I, G_IM_SIZ_8b, 64, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPDisplayList(POLY_OPA_DISP++, gGanondorfShadowModelDL);
    gSPInvalidateTexCache(POLY_OPA_DISP++, tex); // Shadow texture will change every frame, no use keeping it around

    CLOSE_DISPS(gfxCtx);
}

void BossGanon_Draw(Actor* thisx, PlayState* play) {
    s32 i;
    BossGanon* this = (BossGanon*)thisx;
    void* shadowTex;

    shadowTex = Graph_Alloc(play->state.gfxCtx, 64 * 64);

    OPEN_DISPS(play->state.gfxCtx);

    // Invalidate textures if they have changed
    if (this->windowShatterState != GDF_WINDOW_SHATTER_OFF) {
        gSPInvalidateTexCache(POLY_OPA_DISP++, ganon_boss_sceneTex_006C18);
        gSPInvalidateTexCache(POLY_OPA_DISP++, ganon_boss_sceneTex_007418);
    }

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    if ((this->unk_1A6 & 2) != 0) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 50, 0, 0, 900, 1099);
    }

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gGanondorfNormalEyeTex));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          BossGanon_OverrideLimbDraw, BossGanon_PostLimbDraw, &this->actor);

    this->unk_2EC[0].x = this->unk_2EC[1].x;
    this->unk_2EC[0].y = this->unk_2EC[1].y + 30.0f;
    this->unk_2EC[0].z = this->unk_2EC[1].z;

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    BossGanon_DrawEffects(play);

    sBossGanonCape->actor.world.pos = this->actor.world.pos;

    sBossGanonCape->rightForearmPos = this->unk_214;
    sBossGanonCape->leftForearmPos = this->unk_220;

    sBossGanonCape->rightShoulderPos = this->unk_22C;
    sBossGanonCape->leftShoulderPos = this->unk_238;

    BossGanon_DrawShock(this, play);
    BossGanon_DrawHandLightBall(this, play);
    BossGanon_DrawBigMagicCharge(this, play);
    BossGanon_DrawTriforce(this, play);
    BossGanon_DrawDarkVortex(this, play);

    BossGanon_GenShadowTexture(shadowTex, this, play);
    BossGanon_DrawShadowTexture(shadowTex, this, play);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 BossGanon_CheckFallingPlatforms(BossGanon* this, PlayState* play, Vec3f* checkPos) {
    Actor* prop = play->actorCtx.actorLists[ACTORCAT_PROP].head;

    while (prop != NULL) {
        if (((BossGanon*)prop == this) || (prop->id != ACTOR_BG_GANON_OTYUKA)) {
            prop = prop->next;
        } else {
            BgGanonOtyuka* platform = (BgGanonOtyuka*)prop;
            f32 xDiff = platform->dyna.actor.world.pos.x - checkPos->x;
            f32 yDiff = platform->dyna.actor.world.pos.y - checkPos->y;
            f32 zDiff = platform->dyna.actor.world.pos.z - checkPos->z;

            if ((fabsf(xDiff) < 60.0f) && (yDiff < 20.0f) && (yDiff > -20.0f) && (fabsf(zDiff) < 60.0f)) {
                platform->isFalling = true;
                platform->visibleSides = OTYUKA_SIDE_ALL;

                return 1;
            } else {
                prop = prop->next;
            }
        }
    }

    return 0;
}

void BossGanon_LightBall_Update(Actor* thisx, PlayState* play2) {
    u8 hitWithBottle;
    s16 i;
    s16 spBA = 0;
    Vec3f spAC;
    Vec3f spA0;
    Vec3f sp94;
    BossGanon* this = (BossGanon*)thisx;
    PlayState* play = play2;
    f32 xDistFromLink;
    f32 yDistFromLink;
    f32 zDistFromLink;
    f32 minReflectDist;
    f32 xDistFromGanondorf;
    f32 yDistFromGanondorf;
    f32 zDistFromGanondorf;
    Player* player = GET_PLAYER(play);
    s32 pad;
    BossGanon* ganondorf = (BossGanon*)this->actor.parent;
    s32 pad1;

    this->unk_1A2++;
    ganondorf->envLightMode = 1;

    if (this->unk_1A8 != 0) {
        if (this->unk_1A8 == 2) {
            Math_ApproachZeroF(&this->fwork[GDF_FWORK_1], 1.0f, 10.0f);
            Math_ApproachF(&this->actor.scale.x, 30.0f, 0.5f, 100.0f);
        } else {
            this->actor.shape.rot.y += 0x1000;
            ganondorf->lensFlareTimer = 1;
            gCustomLensFlarePos = this->actor.world.pos;
            Math_ApproachZeroF(&this->fwork[GDF_FWORK_1], 1.0f, 30.0f);
            Math_ApproachF(&this->actor.scale.x, 20.0f, 0.5f, 100.0f);
            this->fwork[GDF_FWORK_0] += ((M_PI / 2) + Rand_ZeroFloat(M_PI / 4));
        }

        Actor_SetScale(&this->actor, this->actor.scale.x);

        if (this->fwork[GDF_FWORK_1] == 0.0f) {
            Actor_Kill(&this->actor);
        }
    } else {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_FIRE - SFX_FLAG);

        if ((this->unk_1A2 % 2) != 0) {
            Actor_SetScale(&this->actor, 6.0f);
        } else {
            Actor_SetScale(&this->actor, 5.25f);
        }

        this->actor.shape.rot.z += (s16)(Rand_ZeroOne() * 20000.0f) + 0x4000;

        for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
            if (this->timers[i] != 0) {
                this->timers[i]--;
            }
        }

        xDistFromGanondorf = ganondorf->unk_1FC.x - this->actor.world.pos.x;
        yDistFromGanondorf = ganondorf->unk_1FC.y - this->actor.world.pos.y;
        zDistFromGanondorf = ganondorf->unk_1FC.z - this->actor.world.pos.z;

        xDistFromLink = player->actor.world.pos.x - this->actor.world.pos.x;
        yDistFromLink = (player->actor.world.pos.y + 40.0f) - this->actor.world.pos.y;
        zDistFromLink = player->actor.world.pos.z - this->actor.world.pos.z;

        func_8002D908(&this->actor);
        func_8002D7EC(&this->actor);

        switch (this->unk_1C2) {
            case 0:
                if ((player->stateFlags1 & PLAYER_STATE1_SWINGING_BOTTLE) &&
                    (ABS((s16)(player->actor.shape.rot.y - (s16)(ganondorf->actor.yawTowardsPlayer + 0x8000))) <
                     0x2000) &&
                    (sqrtf(SQ(xDistFromLink) + SQ(yDistFromLink) + SQ(zDistFromLink)) <= 25.0f)) {
                    hitWithBottle = true;
                } else {
                    hitWithBottle = false;
                }

                if ((this->collider.base.acFlags & 2) || hitWithBottle) {
                    ColliderInfo* acHitInfo = this->collider.info.acHitInfo;

                    this->collider.base.acFlags &= ~2;

                    if ((hitWithBottle == false) && (acHitInfo->toucher.dmgFlags & 0x100000)) {
                        spBA = 2;
                        Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_REFLECT_MG, &player->actor.projectedPos, 4, &D_801333E0,
                                               &D_801333E0, &D_801333E8);
                        func_800AA000(this->actor.xyzDistToPlayerSq, 0xFF, 0x14, 0x96);
                    } else {
                        spBA = 1;
                        this->actor.world.rot.y = Math_Atan2S(zDistFromGanondorf, xDistFromGanondorf);
                        this->actor.world.rot.x =
                            Math_Atan2S(sqrtf(SQ(xDistFromGanondorf) + SQ(zDistFromGanondorf)), yDistFromGanondorf);
                        this->unk_1A4++;
                        this->timers[1] = 2;
                        Audio_PlaySoundGeneral(NA_SE_IT_SWORD_REFLECT_MG, &player->actor.projectedPos, 4, &D_801333E0,
                                               &D_801333E0, &D_801333E8);
                        func_800AA000(this->actor.xyzDistToPlayerSq, 0xB4, 0x14, 0x64);

                        if (hitWithBottle == false) {
                            // if ganondorf is 250 units away from link, at least 3 volleys are required
                            if ((ganondorf->actor.xyzDistToPlayerSq > 62500.0f) && (this->unk_1A4 < 3)) {
                                this->unk_1C2 = 1;
                            } else if (Rand_ZeroOne() < 0.7f) {
                                this->unk_1C2 = 1;
                            } else {
                                this->unk_1C2 = 3;
                            }

                            // if a spin attack is used
                            if (player->meleeWeaponAnimation >= 0x18) {
                                this->actor.speedXZ = 20.0f;
                            }
                            break;
                        } else {
                            if (Rand_ZeroOne() < 0.9f) {
                                this->unk_1C2 = 1;
                            } else {
                                this->unk_1C2 = 3;
                            }
                        }
                    }
                } else {
                    if (sqrtf(SQ(xDistFromLink) + SQ(yDistFromLink) + SQ(zDistFromLink)) <= 25.0f) {
                        spBA = 5;
                        func_8002F6D4(play, &this->actor, 3.0f, this->actor.world.rot.y, 0.0f, 0x30);
                        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40,
                                                           NA_SE_EN_GANON_HIT_THUNDER);
                        ganondorf->timers[2] = 20;

                        for (i = 0; i < ARRAY_COUNT(ganondorf->unk_4E4); i++) {
                            ganondorf->unk_4E4[i] = D_808E4C58[i];
                        }

                        ganondorf->unk_2E6 = 0;
                        ganondorf->unk_2E8 = 60;
                        ganondorf->unk_508 = 4.0f;
                    }
                }
                break;

            case 1:
                if ((ganondorf->actionFunc == BossGanon_PlayTennis) && (ganondorf->unk_1C2 == 1)) {
                    minReflectDist = (this->actor.speedXZ >= 19.0f) ? 250.0f : 170.0f;

                    if (sqrtf(SQ(xDistFromGanondorf) + SQ(yDistFromGanondorf) + SQ(zDistFromGanondorf)) <
                        minReflectDist) {
                        ganondorf->startVolley = true;
                        this->timers[0] = 8;
                        this->unk_1C2 = 2;
                    }
                }
                break;

            case 2:
                if (this->timers[0] == 1) {
                    spBA = 1;
                    this->actor.world.rot.y = Math_Atan2S(zDistFromLink, xDistFromLink);
                    this->actor.world.rot.x = Math_Atan2S(sqrtf(SQ(xDistFromLink) + SQ(zDistFromLink)), yDistFromLink);
                    this->timers[1] = 2;
                    Audio_PlayActorSound2(&this->actor, NA_SE_IT_SWORD_REFLECT_MG);
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_AT_RETURN);
                    this->unk_1C2 = 0;
                    break;
                }
                // fallthrough
            case 4:
                if (sqrtf(SQ(xDistFromGanondorf) + SQ(yDistFromGanondorf) + SQ(zDistFromGanondorf)) < 30.0f) {
                    spBA = 3;
                    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_GANON_DAMAGE1);
                    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40,
                                                       NA_SE_EN_GANON_HIT_THUNDER);
                }
                break;

            case 3:
                if (sqrtf(SQ(xDistFromGanondorf) + SQ(yDistFromGanondorf) + SQ(zDistFromGanondorf)) < 100.0f) {
                    ganondorf->startVolley = true;
                    this->unk_1C2 = 4;
                }
                break;
        }

        Collider_UpdateCylinder(&this->actor, &this->collider);

        if (this->timers[1] == 0) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }

        for (i = 0; i < 2; i++) {
            spA0.x = spA0.z = 0.0f;
            spA0.y = 0.2f;

            spAC.x = spAC.y = spAC.z = 0.0f;

            sp94.x = Rand_CenteredFloat(30.0f) + this->actor.world.pos.x;
            sp94.y = Rand_CenteredFloat(30.0f) + this->actor.world.pos.y;
            sp94.z = Rand_CenteredFloat(30.0f) + this->actor.world.pos.z;

            BossGanonEff_SpawnSparkle(play, &sp94, &spAC, &spA0, Rand_ZeroFloat(500.0f) + 700.0f, 0x1E);
        }

        if (this->actor.world.pos.y < 10.0f) {
            Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 20.0f, 20.0f, 4);
        }

        if ((fabsf(this->actor.world.pos.x) > 465.0f) || (this->actor.world.pos.y > 500.0f) ||
            (fabsf(this->actor.world.pos.z) > 465.0f)) {
            spBA = 4;
        }

        if ((spBA != 0) || (this->actor.bgCheckFlags & 1)) {
            f32 sp58;
            f32 sp54;
            f32 phi_f20;
            s16 sp4E;

            if (spBA == 1) {
                sp58 = Rand_ZeroFloat(100.0f) + 300.0f;
                sp54 = 10.0f;
                phi_f20 = 25.0f;
                sp4E = 40;
            } else {
                sp58 = Rand_ZeroFloat(200.0f) + 500.0f;
                sp54 = 15.0f;
                phi_f20 = 30.0f;
                sp4E = 70;
                SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 80, NA_SE_EN_GANON_HIT_THUNDER);
            }

            for (i = 0; i < sp4E; i++) {
                if (spBA != 0) {
                    spAC.x = Rand_CenteredFloat(phi_f20);
                    spAC.y = Rand_CenteredFloat(phi_f20);
                    spAC.z = Rand_CenteredFloat(phi_f20);
                } else {
                    spAC.x = Rand_CenteredFloat(phi_f20);
                    spAC.y = Rand_ZeroFloat(25.0f);
                    spAC.z = Rand_CenteredFloat(phi_f20);
                }

                BossGanonEff_SpawnLightRay(play, &this->actor.world.pos, &spAC, &sZeroVec, sp58, sp54, 0x1E);
            }

            if (spBA != 1) {
                this->unk_1A8 = 1;

                if (spBA == 0) {
                    BossGanon_CheckFallingPlatforms(this, play, &this->actor.world.pos);
                }

                if (spBA == 3) {
                    BossGanon_SetupHitByLightBall(ganondorf, play);
                } else if (ganondorf->actionFunc == BossGanon_PlayTennis) {
                    BossGanon_SetupWait(ganondorf, play);

                    if (spBA == 5) {
                        ganondorf->timers[0] = 125;
                    }
                }
            }
        }
    }
}

void BossGanon_LightBall_Draw(Actor* thisx, PlayState* play) {
    BossGanon* this = (BossGanon*)thisx;
    s16 i;
    f32 alpha;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    alpha = ((this->unk_1A2 % 2) != 0) ? this->fwork[GDF_FWORK_1] * 0.4f : this->fwork[GDF_FWORK_1] * 0.35f;

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 155, (s8)alpha);
    Matrix_Push();
    Matrix_Translate(this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_Scale(this->actor.scale.x * 0.75f, 1.0f, this->actor.scale.z * 0.75f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightCoreDL);

    Matrix_Pop();
    gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightBallMaterialDL);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s8)this->fwork[GDF_FWORK_1]);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);

    if (this->unk_1A8 == 1) {
        for (i = 0; i < 8; i++) {
            FrameInterpolation_RecordOpenChild("Ganondorf Light Ball 0", i);

            Matrix_Push();
            Matrix_RotateY(i * (M_PI / 8), MTXMODE_APPLY);
            Matrix_RotateZ(this->fwork[GDF_FWORK_0], MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            gSPDisplayList(POLY_XLU_DISP++, gGanondorfSquareDL);
            Matrix_Pop();

            FrameInterpolation_RecordCloseChild();
        }
    } else if (this->unk_1A8 == 0) {
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_RotateZ((this->actor.shape.rot.z / 32768.0f) * 3.1416f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gGanondorfSquareDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_808E1EB4(Actor* thisx, PlayState* play2) {
    s16 i;
    BossGanon* this = (BossGanon*)thisx;
    PlayState* play = play2;
    BossGanon* dorf = (BossGanon*)this->actor.parent;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 xzDist;
    s16 xRotTarget;
    s16 yRotTarget;
    Vec3f vel;
    Vec3f accel;

    this->unk_1A2++;
    dorf->envLightMode = 1;
    Actor_SetScale(&this->actor, 6.0f);
    this->actor.shape.rot.z += ((s16)(Rand_ZeroOne() * 20000.0f) + 0x4000);

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }

    func_8002D908(&this->actor);
    func_8002D7EC(&this->actor);

    this->unk_1A6++;

    if (this->unk_1A6 >= 15) {
        this->unk_1A6 = 0;
    }

    this->unk_2EC[this->unk_1A6] = this->actor.world.pos;

    if (this->unk_1C2 == 0) {
        if (this->timers[0] == 0) {
            this->unk_1C2 = 1;
        }
    } else if (this->unk_1C2 == 1) {
        xDiff = dorf->unk_1FC.x - this->actor.world.pos.x;
        yDiff = dorf->unk_1FC.y - this->actor.world.pos.y;
        zDiff = dorf->unk_1FC.z - this->actor.world.pos.z;

        yRotTarget = RADF_TO_BINANG(Math_FAtan2F(xDiff, zDiff));
        xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));
        xRotTarget = RADF_TO_BINANG(Math_FAtan2F(yDiff, xzDist));

        Math_ApproachS(&this->actor.world.rot.x, xRotTarget, 1, 0x1000);
        Math_ApproachS(&this->actor.world.rot.y, yRotTarget, 1, 0x1000);

        if (sqrtf(SQ(xDiff) + SQ(zDiff) + SQ(yDiff)) < 40.0f) {
            this->unk_1C2 = 2;
            this->timers[0] = 30;
            this->actor.speedXZ = 0.0f;

            if (this->actor.params == 0xC8) {
                func_80078884(NA_SE_EN_GANON_DAMAGE2);
                func_80078884(NA_SE_EN_GANON_DD_THUNDER);

                for (i = 0; i < 150; i++) {

                    vel.x = Rand_CenteredFloat(25.0f);
                    vel.y = Rand_CenteredFloat(25.0f);
                    vel.z = Rand_CenteredFloat(25.0f);

                    accel.x = vel.x * -0.03f;
                    accel.y = vel.y * -0.03f;
                    accel.z = vel.z * -0.03f;

                    BossGanonEff_SpawnLightRay(play, &dorf->unk_1FC, &vel, &accel,
                                               Rand_ZeroFloat(500.0f) + 1000.0f, 15.0f, 0x14);
                }

                for (i = 1; i < 15; i++) {
                    dorf->unk_4E4[i] = 1000;
                }

                dorf->unk_2E6 = 1000;
                dorf->unk_2E8 = 0;
                dorf->screenFlashTimer = 4;
                dorf->lensFlareTimer = 10;
                dorf->lensFlareMode = 1;
                dorf->unk_508 = 10.0f;

                Actor_SpawnAsChild(&play->actorCtx, &dorf->actor, play, ACTOR_BOSS_GANON, dorf->unk_1FC.x,
                                   dorf->unk_1FC.y, dorf->unk_1FC.z, 0, 0, 0, 0x12C);
            }

            this->actor.world.pos.y = 5000.0f;
        }
    } else if (this->timers[0] == 0) {
        Actor_Kill(&this->actor);
    }
}

void func_808E229C(Actor* thisx, PlayState* play2) {
    BossGanon* this = (BossGanon*)thisx;
    PlayState* play = play2;
    s16 i;
    s32 temp;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
    gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightBallMaterialDL);

    for (i = 9; i >= 0; i--) {
        temp = (s16)(((this->unk_1A6 - i) + 0xF) % 15);
        Matrix_Translate(this->unk_2EC[temp].x, this->unk_2EC[temp].y, this->unk_2EC[temp].z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x * (1.0f - (i * 0.07000001f)), this->actor.scale.y * (1.0f - (i * 0.07000001f)),
                     this->actor.scale.z * (1.0f - (i * 0.07000001f)), MTXMODE_APPLY);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_RotateZ(((2.0f * (i * M_PI)) / 10.0f) + BINANG_TO_RAD(this->actor.shape.rot.z), MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gGanondorfSquareDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_808E2544(Actor* thisx, PlayState* play) {
    u8 numEffects = 0;
    s16 xRot;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 xzDist;
    f32 new_var;
    f32 sp84;
    s16 i;
    s16 sp80;
    BossGanon* this = (BossGanon*)thisx;
    BossGanon* dorf = (BossGanon*)this->actor.parent;
    s32 pad;
    Player* player = GET_PLAYER(play);
    ColliderInfo* acHitInfo;
    Vec3f sp60;

    this->unk_1A2++;
    Actor_SetScale(&this->actor, 0.01f);

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }

    func_8002D908(&this->actor);
    func_8002D7EC(&this->actor);

    this->unk_1A6++;

    if (this->unk_1A6 >= 15) {
        this->unk_1A6 = 0;
    }

    this->unk_2EC[this->unk_1A6] = this->actor.world.pos;
    this->unk_3C4[this->unk_1A6].x = BINANG_TO_RAD(this->actor.world.rot.x);
    this->unk_3C4[this->unk_1A6].y = BINANG_TO_RAD(this->actor.world.rot.y);

    switch (this->unk_1C2) {
        case 0:
            this->actor.speedXZ = 40.0f;
            Math_ApproachF(&this->fwork[1], 255.0f, 1.0f, 40.0f);
            xDiff = dorf->unk_278.x - this->actor.world.pos.x;
            yDiff = dorf->unk_278.y - this->actor.world.pos.y;
            zDiff = dorf->unk_278.z - this->actor.world.pos.z;
            sp80 = RADF_TO_BINANG(Math_FAtan2F(xDiff, zDiff));
            xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));

            xRot = RADF_TO_BINANG(Math_FAtan2F(yDiff, xzDist));
            sp84 = (xzDist * 700.0f) / 10.0f;
            if (sp84 > 6144.0f) {
                sp84 = 6144.0f;
            }

            xRot += (Math_CosS(this->unk_1A2 * 0x2200) * sp84);
            this->actor.world.rot.x = xRot;
            Math_ApproachS(&this->actor.shape.rot.y, sp80, 1, this->csCamMaxStepScale);
            Math_ApproachF(&this->csCamMaxStepScale, 4096.0f, 1.0f, 256.0f);
            this->actor.world.rot.y = (Math_SinS(this->unk_1A2 * 0x1A00) * sp84) + this->actor.shape.rot.y;

            if (sqrtf(SQ(xDiff) + SQ(zDiff) + SQ(yDiff)) < 45.0f) {
                this->unk_1C2 = 1;
                this->actor.speedXZ = 0.0f;
            }
            break;

        case 1:
            Math_ApproachZeroF(&this->fwork[1], 1.0f, 40.0f);

            if (this->fwork[1] == 0.0f) {
                Actor_Kill(&this->actor);
            }
            break;

        case 10:
            this->unk_1C2 = 0xB;
            this->timers[0] = 14;

            this->collider.dim.radius = 15;
            this->collider.dim.height = 20;
            this->collider.dim.yShift = -10;

            this->actor.speedXZ = 20.0f;
            this->fwork[1] = 255.0f;
            this->unk_1F0 = player->actor.world.pos;
            new_var = this->unk_1F0.x - this->actor.world.pos.x;
            this->actor.shape.rot.y = RADF_TO_BINANG(Math_FAtan2F(new_var, this->unk_1F0.z - this->actor.world.pos.z)) +
                                      (this->actor.params << 0xD) - 0x20C000;
            // fallthrough
        case 11:
            if (this->timers[0] != 0) {
                this->unk_1F0 = player->actor.world.pos;
                xDiff = this->unk_1F0.x - this->actor.world.pos.x;
                yDiff = (this->unk_1F0.y + 30.0f) - this->actor.world.pos.y;
                zDiff = this->unk_1F0.z - this->actor.world.pos.z;

                sp80 = RADF_TO_BINANG(Math_FAtan2F(xDiff, zDiff));
                this->actor.shape.rot.x = RADF_TO_BINANG(Math_FAtan2F(yDiff, sqrtf(SQ(xDiff) + SQ(zDiff))));
                Math_ApproachS(&this->actor.shape.rot.y, sp80, 1, this->csCamMaxStepScale);
                Math_ApproachF(&this->csCamMaxStepScale, 4096.0f, 1.0f, 256.0f);
            }

            sp84 = (sqrtf(this->actor.xyzDistToPlayerSq) * 200.0f) / 10.0f;
            if (sp84 > 13824.0f) {
                sp84 = 13824.0f;
            }

            this->actor.world.rot.x = (Math_CosS(this->unk_1A2 * 0x3400) * sp84 * 0.1f) + this->actor.shape.rot.x;
            this->actor.world.rot.y = (Math_SinS(this->unk_1A2 * 0x1A00) * sp84) + this->actor.shape.rot.y;

            if ((player->swordState != 0) && (player->meleeWeaponAnimation >= 0x18) && (this->actor.xzDistToPlayer < 80.0f)) {
                this->unk_1C2 = 0xC;
                this->actor.speedXZ = -30.0f;
                func_8002D908(&this->actor);
                func_8002D7EC(&this->actor);
                this->unk_1F0 = dorf->unk_1FC;
                numEffects = 10;
                break;
            }

            if (this->collider.base.acFlags & 2) {
                acHitInfo = this->collider.info.acHitInfo;

                this->collider.base.acFlags &= ~2;

                if (!(acHitInfo->toucher.dmgFlags & 0x100000) || Player_HasMirrorShieldEquipped(play)) {
                    func_800AA000(this->actor.xyzDistToPlayerSq, 0xB4, 0x14, 0x64);
                    this->unk_1C2 = 0xC;
                    this->actor.speedXZ = -30.0f;

                    func_8002D908(&this->actor);
                    func_8002D7EC(&this->actor);

                    this->unk_1F0.x = Rand_CenteredFloat(700.0f) + dorf->unk_1FC.x;
                    this->unk_1F0.y = Rand_CenteredFloat(200.0f) + dorf->unk_1FC.y;
                    this->unk_1F0.z = Rand_CenteredFloat(700.0f) + dorf->unk_1FC.z;

                    this->unk_1F0.x = this->unk_1F0.x + ((this->unk_1F0.x - this->actor.world.pos.x) * 100.0f);
                    this->unk_1F0.y = this->unk_1F0.y + ((this->unk_1F0.y - this->actor.world.pos.y) * 100.0f);
                    this->unk_1F0.z = this->unk_1F0.z + ((this->unk_1F0.z - this->actor.world.pos.z) * 100.0f);

                    numEffects = 10;
                    break;
                }
            }

            Collider_UpdateCylinder(&this->actor, &this->collider);

            if (this->timers[1] == 0) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
            }

            xDiff = player->actor.world.pos.x - this->actor.world.pos.x;
            yDiff = (player->actor.world.pos.y + 30.0f) - this->actor.world.pos.y;
            zDiff = player->actor.world.pos.z - this->actor.world.pos.z;

            if (sqrtf(SQ(xDiff) + SQ(zDiff) + SQ(yDiff)) < 30.0f) {
                this->unk_1C2 = 1;
                this->actor.speedXZ = 0.0f;

                if (dorf->timers[2] == 0) {
                    func_8002F6D4(play, &this->actor, 3.0f, this->actor.world.rot.y, 0.0f, 0x50);
                    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40,
                                                       NA_SE_EN_GANON_HIT_THUNDER);
                    dorf->timers[2] = 20;

                    for (i = 0; i < ARRAY_COUNT(this->unk_4E4); i++) {
                        dorf->unk_4E4[i] = D_808E4C58[i];
                    }

                    dorf->unk_2E6 = 0;
                    dorf->unk_2E8 = 60;
                    dorf->unk_508 = 4.0f;
                    numEffects = 40;
                }
            }
            break;

        case 12:
            this->actor.speedXZ = 20.0f;

            xDiff = this->unk_1F0.x - this->actor.world.pos.x;
            yDiff = this->unk_1F0.y - this->actor.world.pos.y;
            zDiff = this->unk_1F0.z - this->actor.world.pos.z;

            sp80 = RADF_TO_BINANG(Math_FAtan2F(xDiff, zDiff));
            xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));
            xRot = RADF_TO_BINANG(Math_FAtan2F(yDiff, xzDist));
            sp84 = (xzDist * 700.0f) / 10.0f;

            if (sp84 > 6144.0f) {
                sp84 = 6144.0f;
            }

            sp80 += Math_SinS(this->unk_1A2 * 0x2200) * sp84;

            xRot += Math_CosS(this->unk_1A2 * 0x1800) * sp84;

            this->actor.world.rot.x = xRot;
            this->actor.world.rot.y = sp80;

            xDiff = dorf->unk_1FC.x - this->actor.world.pos.x;
            yDiff = dorf->unk_1FC.y - this->actor.world.pos.y;
            zDiff = dorf->unk_1FC.z - this->actor.world.pos.z;

            if (sqrtf(SQ(xDiff) + SQ(zDiff) + SQ(yDiff)) < 45.0f) {
                BossGanon_SetupHitByLightBall(dorf, play);
                this->timers[0] = 150;
                numEffects = 40;
                this->unk_1C2 = 1;
                this->actor.speedXZ = 0.0f;
            }
            break;
    }

    if (this->unk_1C2 >= 0xB) {
        xzDist = (this->unk_1C2 == 0xC) ? -65.0f : 0.0f;

        if ((fabsf(this->actor.world.pos.x) > (465.0f + xzDist)) ||
            (fabsf(this->actor.world.pos.z) > (465.0f + xzDist)) || ((this->actor.world.pos.y < 0.0f)) ||
            (this->actor.world.pos.y > 450.0f)) {
            this->unk_1C2 = 1;
            this->actor.speedXZ = 0.0f;
            numEffects = 10;
            BossGanon_CheckFallingPlatforms(this, play, &this->actor.world.pos);
            Actor_SpawnAsChild(&play->actorCtx, &dorf->actor, play, ACTOR_BOSS_GANON, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0x190);
        }
    }

    if (numEffects) {
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 80, NA_SE_EN_FANTOM_THUNDER);

        for (i = 0; i < numEffects; i++) {
            sp60.x = Rand_CenteredFloat(30.0f);
            sp60.y = Rand_CenteredFloat(30.0f);
            sp60.z = Rand_CenteredFloat(30.0);

            BossGanonEff_SpawnLightRay(play, &this->actor.world.pos, &sp60, &sZeroVec,
                                       Rand_ZeroFloat(200.0f) + 500.0f, 15.0f, 0x1E);
        }
    }
}

static Gfx* sBigMagicLightStreakDLists[] = {
    gGanondorfLightStreak12DL, gGanondorfLightStreak11DL, gGanondorfLightStreak10DL, gGanondorfLightStreak9DL,
    gGanondorfLightStreak8DL,  gGanondorfLightStreak7DL,  gGanondorfLightStreak6DL,  gGanondorfLightStreak5DL,
    gGanondorfLightStreak4DL,  gGanondorfLightStreak3DL,  gGanondorfLightStreak2DL,  gGanondorfLightStreak1DL,
};

void func_808E324C(Actor* thisx, PlayState* play) {
    BossGanon* this = (BossGanon*)thisx;
    Mtx* mtx;
    s16 i;
    s32 temp;

    mtx = Graph_Alloc(play->state.gfxCtx, 12 * sizeof(Mtx));

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, (s8)this->fwork[GDF_FWORK_1]);
    gDPSetEnvColor(POLY_XLU_DISP++, 150, 255, 0, 128);
    gSPSegment(POLY_XLU_DISP++, 0x0D, mtx);

    for (i = 0; i < 12; i++) {
        temp = (s16)(((this->unk_1A6 - i) + 0xF) % 15);
        Matrix_Translate(this->unk_2EC[temp].x, this->unk_2EC[temp].y, this->unk_2EC[temp].z, MTXMODE_NEW);
        Matrix_RotateY(this->unk_3C4[temp].y, MTXMODE_APPLY);
        Matrix_RotateX(-this->unk_3C4[temp].x, MTXMODE_APPLY);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateY(M_PI / 2, MTXMODE_APPLY);
        MATRIX_TOMTX(mtx);
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, sBigMagicLightStreakDLists[i]);
        mtx++;
    };

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_Scale(10.0f, 10.0f, 10.0f, MTXMODE_APPLY);
    Matrix_RotateZ(Rand_CenteredFloat(M_PI), MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightBallMaterialDL);

    gSPDisplayList(POLY_XLU_DISP++, gGanondorfSquareDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void BossGanon_UpdateEffects(PlayState* play) {
    Player* player = GET_PLAYER(play);
    GanondorfEffect* eff = play->specialEffects;
    s16 i;
    s32 pad;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 yRot;
    f32 xRot;
    Vec3f spA0;
    s16 bodyPart;
    f32 distToPlayer;
    s32 pad2;
    s32 pad3;

    spA0.x = 0.0f;
    spA0.y = 0.0f;

    for (i = 0; i < ARRAY_COUNT(sBossGanonEffectBuf); i++, eff++) {
        if (eff->type != GDF_EFF_NONE) {
            eff->pos.x += eff->velocity.x;
            eff->pos.y += eff->velocity.y;
            eff->pos.z += eff->velocity.z;

            eff->timer++;

            eff->velocity.x += eff->accel.x;
            eff->velocity.y += eff->accel.y;
            eff->velocity.z += eff->accel.z;

            if (eff->type == GDF_EFF_WINDOW_SHARD) {
                eff->unk_44 += 0.3f;
                eff->unk_48 += 0.5f;

                if (eff->pos.y < 0.0f) {
                    eff->type = GDF_EFF_NONE;
                }
            } else if (eff->type == GDF_EFF_SPARKLE) {
                eff->unk_3C += Rand_ZeroFloat(M_PI / 2) + M_PI / 2;
                eff->unk_2E -= eff->unk_30;

                if (eff->unk_2E <= 0) {
                    eff->unk_2E = 0;
                    eff->type = GDF_EFF_NONE;
                }

                eff->alpha = eff->unk_2E;

                if (eff->alpha > 255) {
                    eff->alpha = 255;
                }
            } else if (eff->type == GDF_EFF_BLACK_DOT) {
                xDiff = sBossGanonGanondorf->unk_278.x - eff->pos.x;
                yDiff = sBossGanonGanondorf->unk_278.y - eff->pos.y;
                zDiff = sBossGanonGanondorf->unk_278.z - eff->pos.z;

                yRot = Math_FAtan2F(xDiff, zDiff);

                xRot = -Math_FAtan2F(yDiff, sqrtf(SQ(xDiff) + SQ(zDiff)));
                spA0.z = eff->unk_38;
                Matrix_RotateY(yRot, MTXMODE_NEW);
                Matrix_RotateX(xRot, MTXMODE_APPLY);
                Matrix_MultVec3f(&spA0, &eff->velocity);
                Math_ApproachF(&eff->unk_38, 10.0f, 1.0f, 0.5f);

                eff->alpha += 10;

                if (eff->alpha > 255) {
                    eff->alpha = 255;
                }

                if ((sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff)) < 20.0f) || (eff->timer > 70)) {
                    eff->type = GDF_EFF_NONE;
                }
            } else if (eff->type == GDF_EFF_LIGHT_RAY) {
                eff->unk_3C += Rand_ZeroFloat(M_PI / 2) + M_PI / 2;
                eff->unk_2E -= eff->unk_30;

                if (eff->unk_2E <= 0) {
                    eff->unk_2E = 0;
                    eff->type = GDF_EFF_NONE;
                }

                eff->alpha = eff->unk_2E;

                if (eff->alpha > 255) {
                    eff->alpha = 255;
                }

                Math_ApproachF(&eff->unk_38, eff->unk_40, 1.0f, (eff->unk_40 / 15.0f) * 4.0f);
            } else if (eff->type == GDF_EFF_SHOCK) {
                if (eff->unk_2E == GDF_SHOCK_DORF_YELLOW) {
                    bodyPart = (s16)Rand_ZeroFloat(13.9f) + 1;

                    eff->pos.x = sBossGanonGanondorf->unk_2EC[bodyPart].x + Rand_CenteredFloat(20.0f);
                    eff->pos.y = sBossGanonGanondorf->unk_2EC[bodyPart].y + Rand_CenteredFloat(20.0f);
                    eff->pos.z = sBossGanonGanondorf->unk_2EC[bodyPart].z + Rand_CenteredFloat(20.0f);
                } else {
                    bodyPart = (s16)Rand_ZeroFloat(17.9f);

                    eff->pos.x = player->bodyPartsPos[bodyPart].x + Rand_CenteredFloat(10.0f);
                    eff->pos.y = player->bodyPartsPos[bodyPart].y + Rand_CenteredFloat(15.0f);
                    eff->pos.z = player->bodyPartsPos[bodyPart].z + Rand_CenteredFloat(10.0f);
                }

                eff->unk_3C += (Rand_ZeroFloat(M_PI / 2) + M_PI / 2);

                if (eff->timer > 20) {
                    eff->type = GDF_EFF_NONE;
                }
            } else if (eff->type == GDF_EFF_LIGHTNING) {
                if (eff->unk_3C == 0.0f) {
                    eff->unk_44 = BINANG_TO_RAD(Camera_GetInputDirYaw(Play_GetCamera(play, MAIN_CAM)));
                } else {
                    eff->unk_44 = M_PI / 2;
                }

                if (eff->timer > 12) {
                    eff->type = GDF_EFF_NONE;
                }
            } else if (eff->type == GDF_EFF_IMPACT_DUST_DARK) {
                eff->unk_30++; // unused

                if (eff->unk_2E == 0) {
                    eff->alpha += 26;

                    if (eff->alpha > 255) {
                        eff->alpha = 255;
                        eff->unk_2E = 1;
                    }
                } else if (eff->unk_2E == 1) {
                    eff->unk_2E = 2;
                } else if (eff->unk_2E == 2) {
                    eff->alpha -= 26;

                    if (eff->alpha < 0) {
                        eff->alpha = 0;
                        eff->type = GDF_EFF_NONE;
                    }
                }

                Math_ApproachF(&eff->scale, eff->unk_38, 1.0f, 0.01f);
                Math_ApproachF(&eff->unk_40, 4.0f, 1.0f, 0.15f);
            } else if (eff->type == GDF_EFF_IMPACT_DUST_LIGHT) {
                if (i == 0) {
                    func_80078884(NA_SE_EN_GANON_WAVE_GND - SFX_FLAG);
                }

                eff->unk_30++; // unused

                if (eff->unk_2E == 0) {
                    eff->alpha += 100;

                    if (eff->alpha > 255) {
                        eff->alpha = 255;
                        eff->unk_2E = 1;
                    }
                } else if (eff->unk_2E == 1) {
                    if (eff->timer >= 20) {
                        eff->unk_2E = 2;
                    }
                } else if (eff->unk_2E == 2) {
                    eff->alpha -= 30;

                    if (eff->alpha < 0) {
                        eff->alpha = 0;
                        eff->type = GDF_EFF_NONE;
                    }
                }

                Math_ApproachF(&eff->scale, eff->unk_38, 1.0f, 0.1f);
                Math_ApproachF(&eff->unk_40, 1.0f, 1.0f, 0.15f);
            } else if (eff->type == GDF_EFF_SHOCKWAVE) {
                eff->unk_30++; // unused
                eff->alpha -= 30;

                if (eff->alpha < 0) {
                    eff->alpha = 0;
                    eff->type = GDF_EFF_NONE;
                }

                Math_ApproachF(&eff->scale, eff->unk_38, 1.0f, 0.13f);

                if ((eff->timer < 150) && (fabsf(player->actor.world.pos.y) < 5.0f)) {
                    distToPlayer =
                        sqrtf(SQ(eff->pos.x - player->actor.world.pos.x) + SQ(eff->pos.z - player->actor.world.pos.z));

                    if (((eff->scale * 150.0f) < distToPlayer) && (distToPlayer < (eff->scale * 300.0f))) {
                        eff->timer = 150;
                        func_8002F6D4(play, &sBossGanonGanondorf->actor, 7.0f, sBossGanonGanondorf->actor.yawTowardsPlayer, 0.0f,
                                      0x20);
                    }
                }
            }
        }
    }
}

static void* sLightningTextures[] = {
    gGanondorfLightning1Tex,  gGanondorfLightning1Tex,  gGanondorfLightning2Tex,  gGanondorfLightning3Tex, gGanondorfLightning4Tex,
    gGanondorfLightning5Tex,  gGanondorfLightning6Tex,  gGanondorfLightning7Tex,  gGanondorfLightning8Tex, gGanondorfLightning9Tex,
    gGanondorfLightning10Tex, gGanondorfLightning11Tex, gGanondorfLightning12Tex,
};

static u8 sLightningPrimColors[] = {
    0,   0,   0,   255, 255, 255, 231, 250, 231, 208, 245, 208, 185, 240, 185, 162, 235, 162, 139, 230,
    139, 115, 225, 115, 92,  220, 92,  69,  215, 69,  46,  210, 46,  23,  205, 23,  0,   200, 0,
};

static u8 sLightningEnvColors[] = {
    0,   0,   0,   255, 255, 0,   240, 231, 23,  226, 208, 46,  212, 185, 69,  198, 162, 92,
    184, 139, 115, 170, 115, 139, 156, 92,  162, 142, 69,  185, 128, 46,  208, 114, 23,  231,
    100, 0,   255, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

void BossGanon_DrawEffects(PlayState* play) {
    u8 flag = 0;
    s16 i;
    s32 pad;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    GanondorfEffect* eff = play->specialEffects;
    GanondorfEffect* effFirst = eff;

    OPEN_DISPS(gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    for (i = 0; i < 200; i++, eff++) {
        if (eff->type == GDF_EFF_WINDOW_SHARD) {
            FrameInterpolation_RecordOpenChild(eff, eff->epoch);
            gDPPipeSync(POLY_OPA_DISP++);
            if (flag == 0) {
                gSPDisplayList(POLY_OPA_DISP++, gGanondorfWindowShardMaterialDL);
                flag++;
            }
            if ((eff->timer & 7) != 0) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, eff->color.r, eff->color.g, eff->color.b, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
            }
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_Scale(eff->scale, eff->scale, eff->scale, MTXMODE_APPLY);
            Matrix_RotateY(eff->unk_48, MTXMODE_APPLY);
            Matrix_RotateX(eff->unk_44, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gGanondorfWindowShardModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    eff = effFirst;
    flag = 0;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_SPARKLE) {
            FrameInterpolation_RecordOpenChild(eff, eff->epoch);
            gDPPipeSync(POLY_XLU_DISP++);
            if (flag == 0) {
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
                gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightBallMaterialDL);
                flag++;
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, eff->alpha);
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
            Matrix_RotateZ(eff->unk_3C, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gGanondorfSquareDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    eff = effFirst;
    flag = 0;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_LIGHT_RAY) {
            FrameInterpolation_RecordOpenChild(eff, eff->epoch);
            gDPPipeSync(POLY_XLU_DISP++);
            if (flag == 0) {
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
                gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightBallMaterialDL);
                flag++;
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, eff->alpha);
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_RotateY(eff->unk_48, MTXMODE_APPLY);
            Matrix_RotateX(eff->unk_44, MTXMODE_APPLY);
            Matrix_RotateZ(eff->unk_3C, MTXMODE_APPLY);
            Matrix_Scale(eff->scale, eff->scale, eff->unk_38 * eff->scale, MTXMODE_APPLY);
            Matrix_RotateX(M_PI / 2, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gGanondorfSquareDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    eff = effFirst;
    flag = 0;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_SHOCK) {
            FrameInterpolation_RecordOpenChild(eff, eff->epoch);
            if (flag == 0) {
                gDPPipeSync(POLY_XLU_DISP++);
                if (eff->unk_2E == GDF_SHOCK_PLAYER_PURPLE) {
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 0, 200, 255);
                    gDPSetEnvColor(POLY_XLU_DISP++, 130, 0, 0, 0);
                } else { // GDF_SHOCK_DORF_YELLOW or GDF_SHOCK_PLAYER_YELLOW
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
                    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
                }
                flag++;
            }
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_Scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
            Matrix_RotateX(eff->unk_3C * 1.3f, MTXMODE_APPLY);
            Matrix_RotateZ(eff->unk_3C, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gGanondorfShockDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    eff = effFirst;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_LIGHTNING) {
            FrameInterpolation_RecordOpenChild(eff, eff->epoch);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, sLightningPrimColors[(eff->timer * 3) + 0],
                            sLightningPrimColors[(eff->timer * 3) + 1], sLightningPrimColors[(eff->timer * 3) + 2],
                            255);
            gDPSetEnvColor(POLY_XLU_DISP++, sLightningEnvColors[(eff->timer * 3) + 0],
                           sLightningEnvColors[(eff->timer * 3) + 1], sLightningEnvColors[(eff->timer * 3) + 2], 0);
            Matrix_Translate(sBossGanonGanondorf->unk_260.x, sBossGanonGanondorf->unk_260.y, sBossGanonGanondorf->unk_260.z, MTXMODE_NEW);
            Matrix_RotateY(eff->unk_48, MTXMODE_APPLY);
            Matrix_RotateZ(eff->unk_3C, MTXMODE_APPLY);
            Matrix_Scale(eff->scale, eff->scale, eff->scale, MTXMODE_APPLY);
            Matrix_RotateY(eff->unk_44, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sLightningTextures[eff->timer]));
            gSPDisplayList(POLY_XLU_DISP++, gGanondorfLightningDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    eff = effFirst;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_IMPACT_DUST_DARK) {
            FrameInterpolation_RecordOpenChild(eff, eff->epoch);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, eff->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 100, 70, 0, 128);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, eff->timer * 4, 0, 32, 64, 1, eff->timer * 2,
                                        eff->timer * -20, 32, 32));
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_Scale(eff->scale, eff->unk_40 * eff->scale, eff->scale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gGanondorfImpactDarkDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    eff = effFirst;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_IMPACT_DUST_LIGHT) {
            FrameInterpolation_RecordOpenChild(eff, eff->epoch);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, eff->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 200, 100, 0, 128);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, eff->timer * 4, 0, 32, 64, 1, eff->timer * 2,
                                        eff->timer * -20, 32, 32));
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_Scale(eff->scale, eff->unk_40 * eff->scale, eff->scale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gGanondorfImpactLightDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    eff = effFirst;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_SHOCKWAVE) {
            FrameInterpolation_RecordOpenChild(eff, eff->epoch);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, eff->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 150, 255, 0, 128);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, (eff->timer * 100), 0, 64, 32, 1,
                                        (eff->timer * 100), 0, 64, 32));
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_Scale((eff->scale * 200.0f) / 1500.0f, (eff->unk_40 * 200.0f) / 1500.0f,
                         (eff->scale * 200.0f) / 1500.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gGanondorfShockwaveDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    eff = effFirst;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_BLACK_DOT) {
            FrameInterpolation_RecordOpenChild(eff, eff->epoch);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 150, 170, 0, eff->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);
            gSPSegment(POLY_XLU_DISP++, 0x0A,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 32, 32, 1, eff->timer * 2, eff->timer * -20,
                                        64, 64));
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gGanondorfDotDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(gfxCtx);
}

#include "overlays/ovl_Boss_Ganon/ovl_Boss_Ganon.h"

void BossGanon_Reset(void) {
    sBossGanonSeed1 = 0;
    sBossGanonSeed2 = 0;
    sBossGanonSeed3 = 0;
    sBossGanonGanondorf = NULL;
    sBossGanonZelda = NULL;
    sBossGanonCape = NULL;
    memset(sBossGanonEffectBuf, 0, sizeof(sBossGanonEffectBuf));
}