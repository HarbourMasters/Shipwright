/*
 * File: z_boss_fd.c
 * Overlay: ovl_Boss_Fd
 * Description: Volvagia, flying form
 */

#include "z_boss_fd.h"
#include "objects/object_fd/object_fd.h"
#include "overlays/actors/ovl_En_Vb_Ball/z_en_vb_ball.h"
#include "overlays/actors/ovl_Bg_Vb_Sima/z_bg_vb_sima.h"
#include "overlays/actors/ovl_Boss_Fd2/z_boss_fd2.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#include "soh/frame_interpolation.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

typedef enum {
    /* 0 */ INTRO_FLY_EMERGE,
    /* 1 */ INTRO_FLY_HOLE,
    /* 2 */ INTRO_FLY_CAMERA,
    /* 3 */ INTRO_FLY_RETRAT
} BossFdIntroFlyState;

typedef enum {
    /* 0 */ MANE_CENTER,
    /* 1 */ MANE_RIGHT,
    /* 2 */ MANE_LEFT
} BossFdManeIndex;

typedef enum {
    /* 0 */ EYE_OPEN,
    /* 1 */ EYE_HALF,
    /* 2 */ EYE_CLOSED
} BossFdEyeState;

void BossFd_Init(Actor* thisx, PlayState* play);
void BossFd_Destroy(Actor* thisx, PlayState* play);
void BossFd_Update(Actor* thisx, PlayState* play);
void BossFd_Draw(Actor* thisx, PlayState* play);

void BossFd_SetupFly(BossFd* this, PlayState* play);
void BossFd_Fly(BossFd* this, PlayState* play);
void BossFd_Wait(BossFd* this, PlayState* play);
void BossFd_UpdateEffects(BossFd* this, PlayState* play);
void BossFd_DrawBody(PlayState* play, BossFd* this);

const ActorInit Boss_Fd_InitVars = {
    ACTOR_BOSS_FD,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_FD,
    sizeof(BossFd),
    (ActorFunc)BossFd_Init,
    (ActorFunc)BossFd_Destroy,
    (ActorFunc)BossFd_Update,
    (ActorFunc)BossFd_Draw,
    NULL,
};

#include "z_boss_fd_colchk.c"

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x21, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

void BossFd_SpawnEmber(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 150; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_EMBER;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->alpha = 255;
            effect->timer1 = (s16)Rand_ZeroFloat(10.0f);
            effect->epoch++;
            break;
        }
    }
}

void BossFd_SpawnDebris(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 150; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_DEBRIS;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->vFdFxRotX = Rand_ZeroFloat(100.0f);
            effect->vFdFxRotY = Rand_ZeroFloat(100.0f);
            effect->epoch++;
            break;
        }
    }
}

void BossFd_SpawnDust(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 150; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_DUST;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->timer2 = 0;
            effect->scale = scale / 400.0f;
            effect->epoch++;
            break;
        }
    }
}

void BossFd_SpawnFireBreath(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale,
                            s16 alpha, s16 kbAngle) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_FIRE_BREATH;
            effect->timer1 = 0;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->pos.x -= effect->velocity.x;
            effect->pos.y -= effect->velocity.y;
            effect->pos.z -= effect->velocity.z;
            effect->vFdFxScaleMod = 0.0f;
            effect->alpha = alpha;
            effect->vFdFxYStop = Rand_ZeroFloat(10.0f);
            effect->timer2 = 0;
            effect->scale = scale / 400.0f;
            effect->kbAngle = kbAngle;
            effect->epoch++;
            break;
        }
    }
}

void BossFd_SetCameraSpeed(BossFd* this, f32 speedMod) {
    this->camData.eyeVel.x = fabsf(this->camData.eye.x - this->camData.nextEye.x) * speedMod;
    this->camData.eyeVel.y = fabsf(this->camData.eye.y - this->camData.nextEye.y) * speedMod;
    this->camData.eyeVel.z = fabsf(this->camData.eye.z - this->camData.nextEye.z) * speedMod;
    this->camData.atVel.x = fabsf(this->camData.at.x - this->camData.nextAt.x) * speedMod;
    this->camData.atVel.y = fabsf(this->camData.at.y - this->camData.nextAt.y) * speedMod;
    this->camData.atVel.z = fabsf(this->camData.at.z - this->camData.nextAt.z) * speedMod;
}

void BossFd_UpdateCamera(BossFd* this, PlayState* play) {
    if (this->introCamera != SUBCAM_FREE) {
        Math_ApproachF(&this->camData.eye.x, this->camData.nextEye.x, this->camData.eyeMaxVel.x,
                       this->camData.eyeVel.x * this->camData.speedMod);
        Math_ApproachF(&this->camData.eye.y, this->camData.nextEye.y, this->camData.eyeMaxVel.y,
                       this->camData.eyeVel.y * this->camData.speedMod);
        Math_ApproachF(&this->camData.eye.z, this->camData.nextEye.z, this->camData.eyeMaxVel.z,
                       this->camData.eyeVel.z * this->camData.speedMod);
        Math_ApproachF(&this->camData.at.x, this->camData.nextAt.x, this->camData.atMaxVel.x,
                       this->camData.atVel.x * this->camData.speedMod);
        Math_ApproachF(&this->camData.at.y, this->camData.nextAt.y, this->camData.atMaxVel.y,
                       this->camData.atVel.y * this->camData.speedMod);
        Math_ApproachF(&this->camData.at.z, this->camData.nextAt.z, this->camData.atMaxVel.z,
                       this->camData.atVel.z * this->camData.speedMod);
        Math_ApproachF(&this->camData.speedMod, 1.0f, 1.0f, this->camData.accel);
        this->camData.at.y += this->camData.yMod;
        Play_CameraSetAtEye(play, this->introCamera, &this->camData.at, &this->camData.eye);
        Math_ApproachZeroF(&this->camData.yMod, 1.0f, 0.1f);
    }
}

void BossFd_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BossFd* this = (BossFd*)thisx;
    s16 i;

    Flags_SetSwitch(play, 0x14);
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BG_VB_SIMA, 680.0f, -100.0f, 0.0f, 0, 0, 0,
                       100);
    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    Actor_SetScale(&this->actor, 0.05f);
    SkelAnime_Init(play, &this->skelAnimeHead, &gVolvagiaHeadSkel, &gVolvagiaHeadEmergeAnim, NULL, NULL, 0);
    SkelAnime_Init(play, &this->skelAnimeRightArm, &gVolvagiaRightArmSkel, &gVolvagiaRightArmEmergeAnim, NULL,
                   NULL, 0);
    SkelAnime_Init(play, &this->skelAnimeLeftArm, &gVolvagiaLeftArmSkel, &gVolvagiaLeftArmEmergeAnim, NULL, NULL,
                   0);
    this->introState = BFD_CS_WAIT;
    if (this->introState == BFD_CS_NONE) {
        Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_FIRE_BOSS);
    }

    this->actor.world.pos.x = this->actor.world.pos.z = 0.0f;
    this->actor.world.pos.y = -200.0f;
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->elements);

    for (i = 0; i < 100; i++) {
        this->bodySegsPos[i].x = this->actor.world.pos.x;
        this->bodySegsPos[i].y = this->actor.world.pos.y;
        this->bodySegsPos[i].z = this->actor.world.pos.z;
        if (i < 30) {
            this->centerMane.pos[i].x = this->actor.world.pos.x;
            this->centerMane.pos[i].y = this->actor.world.pos.y;
            this->centerMane.pos[i].z = this->actor.world.pos.z;
        }
    }

    this->actor.colChkInfo.health = 24;
    this->skinSegments = 18;
    if (this->introState == BFD_CS_NONE) {
        this->actionFunc = BossFd_Wait;
    } else {
        BossFd_SetupFly(this, play);
    }

    if (Flags_GetClear(play, play->roomCtx.curRoom.num)) {
        Actor_Kill(&this->actor);
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, 0.0f, 100.0f, 0.0f, 0, 0, 0,
                           WARP_DUNGEON_ADULT);
        Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, 0.0f, 100.0f, 200.0f, 0, 0, 0, 0);
    } else {
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BOSS_FD2, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, this->introState);
    }
}

void BossFd_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    BossFd* this = (BossFd*)thisx;

    SkelAnime_Free(&this->skelAnimeHead, play);
    SkelAnime_Free(&this->skelAnimeRightArm, play);
    SkelAnime_Free(&this->skelAnimeLeftArm, play);
    Collider_DestroyJntSph(play, &this->collider);
}

s32 BossFd_IsFacingLink(BossFd* this) {
    return ABS((s16)(this->actor.yawTowardsPlayer - this->actor.world.rot.y)) < 0x2000;
}

void BossFd_SetupFly(BossFd* this, PlayState* play) {
    Animation_PlayOnce(&this->skelAnimeHead, &gVolvagiaHeadEmergeAnim);
    Animation_PlayOnce(&this->skelAnimeRightArm, &gVolvagiaRightArmEmergeAnim);
    Animation_PlayOnce(&this->skelAnimeLeftArm, &gVolvagiaLeftArmEmergeAnim);
    this->actionFunc = BossFd_Fly;
    this->fwork[BFD_TURN_RATE_MAX] = 1000.0f;
}

static Vec3f sHoleLocations[] = {
    { 0.0f, 90.0f, -243.0f },    { 0.0f, 90.0f, 0.0f },    { 0.0f, 90.0f, 243.0f },
    { -243.0f, 90.0f, -243.0f }, { -243.0f, 90.0f, 0.0f }, { -243.0f, 90.0f, 243.0f },
    { 243.0f, 90.0f, -243.0f },  { 243.0f, 90.0f, 0.0f },  { 243.0f, 90.0f, 243.0f },
};

static Vec3f sCeilingTargets[] = {
    { 0.0f, 900.0f, -243.0f }, { 243.0, 900.0f, -100.0f },  { 243.0f, 900.0f, 100.0f },
    { 0.0f, 900.0f, 243.0f },  { -243.0f, 900.0f, 100.0f }, { -243.0, 900.0f, -100.0f },
};

void BossFd_Fly(BossFd* this, PlayState* play) {
    u8 sp1CF = false;
    u8 temp_rand;
    s16 i1;
    s16 i2;
    s16 i3;
    f32 dx;
    f32 dy;
    f32 dz;
    Player* player = GET_PLAYER(play);
    f32 angleToTarget;
    f32 pitchToTarget;
    Vec3f* holePosition1;
    f32 temp_y;
    f32 temp_x;
    f32 temp_z;
    f32 temp;

    SkelAnime_Update(&this->skelAnimeHead);
    SkelAnime_Update(&this->skelAnimeRightArm);
    SkelAnime_Update(&this->skelAnimeLeftArm);
    dx = this->targetPosition.x - this->actor.world.pos.x;
    dy = this->targetPosition.y - this->actor.world.pos.y;
    dz = this->targetPosition.z - this->actor.world.pos.z;
    dx += Math_SinS((2096.0f + this->fwork[BFD_FLY_WOBBLE_RATE]) * this->work[BFD_MOVE_TIMER]) *
          this->fwork[BFD_FLY_WOBBLE_AMP];
    dy += Math_SinS((1096.0f + this->fwork[BFD_FLY_WOBBLE_RATE]) * this->work[BFD_MOVE_TIMER]) *
          this->fwork[BFD_FLY_WOBBLE_AMP];
    dz += Math_SinS((1796.0f + this->fwork[BFD_FLY_WOBBLE_RATE]) * this->work[BFD_MOVE_TIMER]) *
          this->fwork[BFD_FLY_WOBBLE_AMP];
    angleToTarget = (s16)(Math_FAtan2F(dx, dz) * (0x8000 / M_PI));
    pitchToTarget = (s16)(Math_FAtan2F(dy, sqrtf(SQ(dx) + SQ(dz))) * (0x8000 / M_PI));

    osSyncPrintf("MODE %d\n", this->work[BFD_ACTION_STATE]);

    Math_ApproachF(&this->fwork[BFD_BODY_PULSE], 0.1f, 1.0f, 0.02);

    //                                        Boss Intro Cutscene

    if (this->introState != BFD_CS_NONE) {
        Player* player2 = GET_PLAYER(play);
        Camera* mainCam = Play_GetCamera(play, MAIN_CAM);

        switch (this->introState) {
            case BFD_CS_WAIT:
                this->fogMode = 3;
                this->targetPosition.x = 0.0f;
                this->targetPosition.y = -110.0f;
                this->targetPosition.z = 0.0;
                this->fwork[BFD_TURN_RATE_MAX] = 10000.0f;
                this->work[BFD_ACTION_STATE] = BOSSFD_WAIT_INTRO;
                if ((fabsf(player2->actor.world.pos.z) < 80.0f) &&
                    (fabsf(player2->actor.world.pos.x - 340.0f) < 60.0f)) {

                    this->introState = BFD_CS_START;
                    func_80064520(play, &play->csCtx);
                    func_8002DF54(play, &this->actor, 8);
                    this->introCamera = Play_CreateSubCamera(play);
                    Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
                    Play_ChangeCameraStatus(play, this->introCamera, CAM_STAT_ACTIVE);
                    player2->actor.world.pos.x = 380.0f;
                    player2->actor.world.pos.y = 100.0f;
                    player2->actor.world.pos.z = 0.0f;
                    player2->actor.shape.rot.y = player2->actor.world.rot.y = -0x4000;
                    player2->actor.speedXZ = 0.0f;
                    this->camData.eye.x = player2->actor.world.pos.x - 70.0f;
                    this->camData.eye.y = player2->actor.world.pos.y + 40.0f;
                    this->camData.eye.z = player2->actor.world.pos.z + 70.0f;
                    this->camData.at.x = player2->actor.world.pos.x;
                    this->camData.at.y = player2->actor.world.pos.y + 30.0f;
                    this->camData.at.z = player2->actor.world.pos.z;
                    this->camData.nextEye.x = player2->actor.world.pos.x - 50.0f + 18.0f;
                    this->camData.nextEye.y = player2->actor.world.pos.y + 40;
                    this->camData.nextEye.z = player2->actor.world.pos.z + 50.0f - 18.0f;
                    this->camData.nextAt.x = player2->actor.world.pos.x;
                    this->camData.nextAt.y = player2->actor.world.pos.y + 50.0f;
                    this->camData.nextAt.z = player2->actor.world.pos.z;
                    BossFd_SetCameraSpeed(this, 1.0f);
                    this->camData.atMaxVel.x = this->camData.atMaxVel.y = this->camData.atMaxVel.z = 0.05f;
                    this->camData.eyeMaxVel.x = this->camData.eyeMaxVel.y = this->camData.eyeMaxVel.z = 0.05f;
                    this->timers[0] = 0;
                    this->camData.speedMod = 0.0f;
                    this->camData.accel = 0.0f;
                    if (gSaveContext.eventChkInf[7] & 8) {
                        this->introState = BFD_CS_EMERGE;
                        this->camData.nextEye.x = player2->actor.world.pos.x + 100.0f + 300.0f - 600.0f;
                        this->camData.nextEye.y = player2->actor.world.pos.y + 100.0f - 50.0f;
                        this->camData.nextEye.z = player2->actor.world.pos.z + 200.0f - 150.0f;
                        this->camData.nextAt.x = 0.0f;
                        this->camData.nextAt.y = 120.0f;
                        this->camData.nextAt.z = 0.0f;
                        BossFd_SetCameraSpeed(this, 0.5f);
                        this->camData.eyeMaxVel.x = this->camData.eyeMaxVel.y = this->camData.eyeMaxVel.z = 0.1f;
                        this->camData.atMaxVel.x = this->camData.atMaxVel.y = this->camData.atMaxVel.z = 0.1f;
                        this->camData.accel = 0.005f;
                        this->timers[0] = 0;
                        this->holeIndex = 1;
                        this->targetPosition.x = sHoleLocations[this->holeIndex].x;
                        this->targetPosition.y = sHoleLocations[this->holeIndex].y - 200.0f;
                        this->targetPosition.z = sHoleLocations[this->holeIndex].z;
                        this->timers[0] = 50;
                        this->work[BFD_ACTION_STATE] = BOSSFD_EMERGE;
                        this->actor.world.rot.x = 0x4000;
                        this->work[BFD_MOVE_TIMER] = 0;
                        this->timers[3] = 250;
                        this->timers[2] = 470;
                        this->fwork[BFD_FLY_SPEED] = 5.0f;
                    }
                }
                break;
            case BFD_CS_START:
                if (this->timers[0] == 0) {
                    this->camData.accel = 0.0010000002f;
                    this->timers[0] = 100;
                    this->introState = BFD_CS_LOOK_LINK;
                }
            case BFD_CS_LOOK_LINK:
                player2->actor.world.pos.x = 380.0f;
                player2->actor.world.pos.y = 100.0f;
                player2->actor.world.pos.z = 0.0f;
                player2->actor.speedXZ = 0.0f;
                player2->actor.shape.rot.y = player2->actor.world.rot.y = -0x4000;
                if (this->timers[0] == 50) {
                    this->fogMode = 1;
                }
                if (this->timers[0] < 50) {
                    Audio_PlaySoundGeneral(NA_SE_EN_DODO_K_ROLL - SFX_FLAG, &this->actor.projectedPos, 4, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                    this->camData.yMod = Math_CosS(this->work[BFD_MOVE_TIMER] * 0x8000) * this->camData.shake;
                    Math_ApproachF(&this->camData.shake, 2.0f, 1.0f, 0.8 * 0.01f);
                }
                if (this->timers[0] == 40) {
                    func_8002DF54(play, &this->actor, 0x13);
                }
                if (this->timers[0] == 0) {
                    this->introState = BFD_CS_LOOK_GROUND;
                    this->camData.nextAt.y = player2->actor.world.pos.y + 10.0f;
                    this->camData.atMaxVel.y = 0.2f;
                    this->camData.speedMod = 0.0f;
                    this->camData.accel = 0.02f;
                    this->timers[0] = 70;
                    this->work[BFD_MOVE_TIMER] = 0;
                }
                break;
            case BFD_CS_LOOK_GROUND:
                this->camData.yMod = Math_CosS(this->work[BFD_MOVE_TIMER] * 0x8000) * this->camData.shake;
                Math_ApproachF(&this->camData.shake, 2.0f, 1.0f, 0.8 * 0.01f);
                Audio_PlaySoundGeneral(NA_SE_EN_DODO_K_ROLL - SFX_FLAG, &this->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);
                if (this->timers[0] == 0) {
                    this->introState = BFD_CS_COLLAPSE;
                    this->camData.nextEye.x = player2->actor.world.pos.x + 100.0f + 300.0f;
                    this->camData.nextEye.y = player2->actor.world.pos.y + 100.0f;
                    this->camData.nextEye.z = player2->actor.world.pos.z + 200.0f;
                    this->camData.nextAt.x = player2->actor.world.pos.x;
                    this->camData.nextAt.y = player2->actor.world.pos.y - 150.0f;
                    this->camData.nextAt.z = player2->actor.world.pos.z - 50.0f;
                    BossFd_SetCameraSpeed(this, 0.1f);
                    this->timers[0] = 170;
                    this->camData.speedMod = 0.0f;
                    this->camData.accel = 0.0f;
                    func_8002DF54(play, &this->actor, 0x14);
                }
                break;
            case BFD_CS_COLLAPSE:
                this->camData.accel = 0.005f;
                this->camData.yMod = Math_CosS(this->work[BFD_MOVE_TIMER] * 0x8000) * this->camData.shake;
                Math_ApproachF(&this->camData.shake, 2.0f, 1.0f, 0.8 * 0.01f);
                Audio_PlaySoundGeneral(NA_SE_EN_DODO_K_ROLL - SFX_FLAG, &this->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);
                if (this->timers[0] == 100) {
                    this->platformSignal = VBSIMA_COLLAPSE;
                }
                if (this->timers[0] == 0) {
                    this->introState = BFD_CS_EMERGE;
                    this->camData.speedMod = 0.0f;
                    this->camData.nextEye.x = player2->actor.world.pos.x + 100.0f + 300.0f - 600.0f;
                    this->camData.nextEye.y = player2->actor.world.pos.y + 100.0f - 50.0f;
                    this->camData.nextEye.z = player2->actor.world.pos.z + 200.0f - 150.0f;
                    this->camData.nextAt.x = 0.0f;
                    this->camData.nextAt.y = 120.0f;
                    this->camData.nextAt.z = 0.0f;
                    BossFd_SetCameraSpeed(this, 0.5f);
                    this->camData.atMaxVel.x = this->camData.atMaxVel.y = this->camData.atMaxVel.z = 0.1f;
                    this->camData.eyeMaxVel.x = this->camData.eyeMaxVel.y = this->camData.eyeMaxVel.z = 0.1f;
                    this->camData.accel = 0.005f;
                    this->timers[0] = 0;
                    this->holeIndex = 1;
                    this->targetPosition.x = sHoleLocations[this->holeIndex].x;
                    this->targetPosition.y = sHoleLocations[this->holeIndex].y - 200.0f;
                    this->targetPosition.z = sHoleLocations[this->holeIndex].z;
                    this->timers[0] = 50;
                    this->work[BFD_ACTION_STATE] = BOSSFD_EMERGE;
                    this->actor.world.rot.x = 0x4000;
                    this->work[BFD_MOVE_TIMER] = 0;
                    this->timers[3] = 250;
                    this->timers[2] = 470;
                    this->fwork[BFD_FLY_SPEED] = 5.0f;
                }
                break;
            case BFD_CS_EMERGE:
                osSyncPrintf("WAY_SPD X = %f\n", this->camData.atVel.x);
                osSyncPrintf("WAY_SPD Y = %f\n", this->camData.atVel.y);
                osSyncPrintf("WAY_SPD Z = %f\n", this->camData.atVel.z);
                if ((this->timers[3] > 190) && !(gSaveContext.eventChkInf[7] & 8)) {
                    Audio_PlaySoundGeneral(NA_SE_EN_DODO_K_ROLL - SFX_FLAG, &this->actor.projectedPos, 4, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                }
                if (this->timers[3] == 190) {
                    this->camData.atMaxVel.x = this->camData.atMaxVel.y = this->camData.atMaxVel.z = 0.05f;
                    this->platformSignal = VBSIMA_KILL;
                    func_8002DF54(play, &this->actor, 1);
                }
                if (this->actor.world.pos.y > 120.0f) {
                    this->camData.nextAt = this->actor.world.pos;
                    this->camData.atVel.x = 190.0f;
                    this->camData.atVel.y = 85.56f;
                    this->camData.atVel.z = 25.0f;
                } else {
                    // the following `temp` stuff is probably fake but is required to match
                    // it's optimized to 1.0f because sp1CF is false at this point, but the 0.1f ends up in rodata
                    temp = 0.1f;
                    if (!sp1CF) {
                        temp = 1.0f;
                    }
                    Math_ApproachF(&this->camData.shake, 2.0f, temp, 0.1 * 0.08f);
                    this->camData.yMod = Math_CosS(this->work[BFD_MOVE_TIMER] * 0x8000) * this->camData.shake;
                }
                if (this->timers[3] == 160) {
                    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_FIRE_BOSS);
                }
                if ((this->timers[3] == 130) && !(gSaveContext.eventChkInf[7] & 8)) {
                    TitleCard_InitBossName(play, &play->actorCtx.titleCtx,
                                           SEGMENTED_TO_VIRTUAL(gVolvagiaBossTitleCardTex), 160, 180, 128, 40, true);
                }
                if (this->timers[3] <= 100) {
                    this->camData.eyeVel.x = this->camData.eyeVel.y = this->camData.eyeVel.z = 2.0f;
                    this->camData.nextEye.x = player2->actor.world.pos.x + 50.0f;
                    this->camData.nextEye.y = player2->actor.world.pos.y + 50.0f;
                    this->camData.nextEye.z = player2->actor.world.pos.z + 50.0f;
                }
                if (this->work[BFD_ACTION_STATE] == BOSSFD_FLY_HOLE) {
                    switch (this->introFlyState) {
                        case INTRO_FLY_EMERGE:
                            this->timers[5] = 100;
                            this->introFlyState = INTRO_FLY_HOLE;
                        case INTRO_FLY_HOLE:
                            if (this->timers[5] == 0) {
                                this->introFlyState = INTRO_FLY_CAMERA;
                                this->timers[5] = 75;
                            }
                            break;
                        case INTRO_FLY_CAMERA:
                            this->targetPosition = this->camData.eye;
                            if (this->timers[5] == 0) {
                                this->timers[0] = 0;
                                this->holeIndex = 7;
                                this->targetPosition.x = sHoleLocations[this->holeIndex].x;
                                this->targetPosition.y = sHoleLocations[this->holeIndex].y + 200.0f + 50.0f;
                                this->targetPosition.z = sHoleLocations[this->holeIndex].z;
                                this->introFlyState = INTRO_FLY_RETRAT;
                            }
                            if (this->timers[5] == 30) {
                                this->work[BFD_ROAR_TIMER] = 40;
                                this->fireBreathTimer = 20;
                            }
                        case INTRO_FLY_RETRAT:
                            break;
                    }
                }
                osSyncPrintf("this->timer[2] = %d\n", this->timers[2]);
                osSyncPrintf("this->timer[5] = %d\n", this->timers[5]);
                if (this->timers[2] == 0) {
                    mainCam->eye = this->camData.eye;
                    mainCam->eyeNext = this->camData.eye;
                    mainCam->at = this->camData.at;
                    func_800C08AC(play, this->introCamera, 0);
                    this->introState = this->introFlyState = this->introCamera = BFD_CS_NONE;
                    func_80064534(play, &play->csCtx);
                    func_8002DF54(play, &this->actor, 7);
                    this->actionFunc = BossFd_Wait;
                    this->handoffSignal = FD2_SIGNAL_GROUND;
                    gSaveContext.eventChkInf[7] |= 8;
                }
                break;
        }
        BossFd_UpdateCamera(this, play);
    } else {
        this->fwork[BFD_FLY_SPEED] = 5.0f;
    }

    //                             Attacks and Death Cutscene

    switch (this->work[BFD_ACTION_STATE]) {
        case BOSSFD_FLY_MAIN:
            sp1CF = true;
            if (this->timers[0] == 0) {
                if (this->actor.colChkInfo.health == 0) {
                    this->work[BFD_ACTION_STATE] = BOSSFD_DEATH_START;
                    this->timers[0] = 0;
                    this->timers[1] = 100;
                } else {
                    if (this->introState != BFD_CS_NONE) {
                        this->holeIndex = 6;
                    } else {
                        do {
                            temp_rand = Rand_ZeroFloat(8.9f);
                        } while (temp_rand == this->holeIndex);
                        this->holeIndex = temp_rand;
                    }
                    this->targetPosition.x = sHoleLocations[this->holeIndex].x;
                    this->targetPosition.y = sHoleLocations[this->holeIndex].y + 200.0f + 50.0f;
                    this->targetPosition.z = sHoleLocations[this->holeIndex].z;
                    this->fwork[BFD_TURN_RATE] = 0.0f;
                    this->fwork[BFD_TURN_RATE_MAX] = 1000.0f;
                    if (this->introState != BFD_CS_NONE) {
                        this->timers[0] = 10050;
                    } else {
                        this->timers[0] = 20;
                    }
                    this->fwork[BFD_FLY_WOBBLE_AMP] = 100.0f;
                    this->work[BFD_ACTION_STATE] = BOSSFD_FLY_HOLE;

                    if (this->work[BFD_START_ATTACK]) {
                        this->work[BFD_START_ATTACK] = false;
                        this->work[BFD_FLY_COUNT]++;
                        if (this->work[BFD_FLY_COUNT] & 1) {
                            this->work[BFD_ACTION_STATE] = BOSSFD_FLY_CHASE;
                            this->timers[0] = 300;
                            this->fwork[BFD_TURN_RATE_MAX] = 900.0f;
                            this->fwork[BFD_TARGET_Y_OFFSET] = 300.0f;
                            this->work[BFD_UNK_234] = this->work[BFD_UNK_236] = 0;
                        } else {
                            this->work[BFD_ACTION_STATE] = BOSSFD_FLY_CEILING;
                        }
                    }
                }
            }
            break;
        case BOSSFD_FLY_HOLE:
            if ((this->timers[0] == 0) && (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 100.0f)) {
                this->work[BFD_ACTION_STATE] = BOSSFD_BURROW;
                this->targetPosition.y = sHoleLocations[this->holeIndex].y - 70.0f;
                this->fwork[BFD_TURN_RATE_MAX] = 10000.0f;
                this->fwork[BFD_FLY_WOBBLE_AMP] = 0.0f;
                this->timers[0] = 150;
                this->work[BFD_ROAR_TIMER] = 40;
                this->holePosition.x = this->targetPosition.x;
                this->holePosition.z = this->targetPosition.z;
            }
            break;
        case BOSSFD_BURROW:
            sp1CF = true;
            if (this->timers[0] == 0) {
                this->actionFunc = BossFd_Wait;
                this->handoffSignal = FD2_SIGNAL_GROUND;
            }
            break;
        case BOSSFD_EMERGE:
            if ((this->timers[0] == 0) && (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 100.0f)) {
                this->actor.world.pos = this->targetPosition;
                this->work[BFD_ACTION_STATE] = BOSSFD_FLY_MAIN;
                this->actor.world.rot.x = 0x4000;
                this->targetPosition.y = sHoleLocations[this->holeIndex].y + 200.0f;
                this->timers[4] = 80;
                this->fwork[BFD_TURN_RATE_MAX] = 1000.0f;
                this->fwork[BFD_FLY_WOBBLE_AMP] = 0.0f;
                this->holePosition.x = this->targetPosition.x;
                this->holePosition.z = this->targetPosition.z;

                func_80033E1C(play, 1, 0x50, 0x5000);
                if (this->introState != BFD_CS_NONE) {
                    this->timers[0] = 50;
                } else {
                    this->timers[0] = 50;
                }
            }
            break;
        case BOSSFD_FLY_CEILING:
            this->fwork[BFD_FLY_SPEED] = 8;
            this->targetPosition.x = 0.0f;
            this->targetPosition.y = 700.0f;
            this->targetPosition.z = -300.0f;
            this->fwork[BFD_FLY_WOBBLE_AMP] = 200.0f;
            this->fwork[BFD_TURN_RATE_MAX] = 3000.0f;
            if (this->actor.world.pos.y > 700.0f) {
                this->work[BFD_ACTION_STATE] = BOSSFD_DROP_ROCKS;
                this->timers[0] = 25;
                this->timers[2] = 150;
                this->work[BFD_CEILING_TARGET] = 0;
            }
            break;
        case BOSSFD_DROP_ROCKS:
            this->fwork[BFD_FLY_SPEED] = 8;
            this->fwork[BFD_FLY_WOBBLE_AMP] = 200.0f;
            this->fwork[BFD_TURN_RATE_MAX] = 10000.0f;
            this->targetPosition.x = sCeilingTargets[this->work[BFD_CEILING_TARGET]].x;
            this->targetPosition.y = sCeilingTargets[this->work[BFD_CEILING_TARGET]].y + 900.0f;
            this->targetPosition.z = sCeilingTargets[this->work[BFD_CEILING_TARGET]].z;
            if (this->timers[0] == 0) {
                this->timers[0] = 25;
                this->work[BFD_CEILING_TARGET]++;
                if (this->work[BFD_CEILING_TARGET] >= 6) {
                    this->work[BFD_CEILING_TARGET] = 0;
                }
            }
            Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 50.0f, 100.0f, 2);
            if (this->timers[1] == 0) {
                osSyncPrintf("BGCHECKKKKKKKKKKKKKKKKKKKKKKK\n");
                if (this->actor.bgCheckFlags & 0x10) {
                    this->fwork[BFD_CEILING_BOUNCE] = -18384.0f;
                    this->timers[1] = 10;
                    Audio_PlaySoundGeneral(NA_SE_EV_EXPLOSION, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    func_80033E1C(play, 3, 0xA, 0x7530);
                    this->work[BFD_ROCK_TIMER] = 300;
                }
            } else {
                pitchToTarget = this->fwork[BFD_CEILING_BOUNCE];
                Math_ApproachZeroF(&this->fwork[BFD_CEILING_BOUNCE], 1.0f, 1000.0f);
            }
            if (this->timers[2] == 0) {
                this->work[BFD_ACTION_STATE] = BOSSFD_FLY_MAIN;
                this->timers[0] = 0;
                this->work[BFD_START_ATTACK] = false;
            }
            break;
        case BOSSFD_FLY_CHASE:
            this->actor.flags |= ACTOR_FLAG_24;
            temp_y = Math_SinS(this->work[BFD_MOVE_TIMER] * 2396.0f) * 30.0f + this->fwork[BFD_TARGET_Y_OFFSET];
            this->targetPosition.x = player->actor.world.pos.x;
            this->targetPosition.y = player->actor.world.pos.y + temp_y + 30.0f;
            this->targetPosition.z = player->actor.world.pos.z;
            this->fwork[BFD_FLY_WOBBLE_AMP] = 0.0f;
            if (((this->timers[0] % 64) == 0) && (this->timers[0] < 450)) {
                this->work[BFD_ROAR_TIMER] = 40;
                if (BossFd_IsFacingLink(this)) {
                    this->fireBreathTimer = 20;
                }
            }
            if ((this->work[BFD_DAMAGE_FLASH_TIMER] != 0) || (this->timers[0] == 0) ||
                (player->actor.world.pos.y < 70.0f)) {
                this->work[BFD_ACTION_STATE] = BOSSFD_FLY_MAIN;
                this->timers[0] = 0;
                this->work[BFD_START_ATTACK] = false;
            } else {
                Math_ApproachF(&this->fwork[BFD_TARGET_Y_OFFSET], 50.0, 1.0f, 2.0f);
            }
            break;
        case BOSSFD_DEATH_START:
            if (sqrtf(SQ(dx) + SQ(dz)) < 50.0f) {
                this->timers[0] = 0;
            }
            if (this->timers[0] == 0) {
                this->timers[0] = (s16)Rand_ZeroFloat(10.0f) + 10;
                do {
                    this->targetPosition.x = Rand_CenteredFloat(200.0f);
                    this->targetPosition.y = 390.0f;
                    this->targetPosition.z = Rand_CenteredFloat(200.0f);
                    temp_x = this->targetPosition.x - this->actor.world.pos.x;
                    temp_z = this->targetPosition.z - this->actor.world.pos.z;
                } while (!(sqrtf(SQ(temp_x) + SQ(temp_z)) > 100.0f));
            }
            this->fwork[BFD_FLY_WOBBLE_AMP] = 200.0f;
            this->fwork[BFD_FLY_WOBBLE_RATE] = 1000.0f;
            this->fwork[BFD_TURN_RATE_MAX] = 10000.0f;
            Math_ApproachF(&this->fwork[BFD_BODY_PULSE], 0.3f, 1.0f, 0.05f);
            if (this->timers[1] == 0) {
                this->work[BFD_ACTION_STATE] = BOSSFD_SKIN_BURN;
                this->timers[0] = 30;
            }
            break;
        case BOSSFD_SKIN_BURN:
            this->targetPosition.x = 0.0f;
            this->targetPosition.y = 390.0f;
            this->targetPosition.z = 0.0f;
            this->fwork[BFD_FLY_WOBBLE_AMP] = 200.0f;
            this->fwork[BFD_FLY_WOBBLE_RATE] = 1000.0f;
            this->fwork[BFD_TURN_RATE_MAX] = 2000.0f;
            Math_ApproachF(&this->fwork[BFD_BODY_PULSE], 0.3f, 1.0f, 0.05f);
            if ((this->timers[0] == 0) && ((this->work[BFD_MOVE_TIMER] % 4) == 0)) {
                if (this->skinSegments != 0) {
                    this->skinSegments--;
                    if (this->skinSegments == 0) {
                        Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS_CLEAR);
                    }
                } else {
                    this->work[BFD_ACTION_STATE] = BOSSFD_BONES_FALL;
                    this->timers[0] = 30;
                }
            }
            if ((this->work[BFD_MOVE_TIMER] % 32) == 0) {
                this->work[BFD_ROAR_TIMER] = 40;
            }

            if (this->skinSegments != 0) {
                Vec3f sp188;
                Vec3f sp17C = { 0.0f, 0.0f, 0.0f };
                Vec3f sp170;
                Vec3f sp164 = { 0.0f, 0.03f, 0.0f };
                Vec3f sp158;
                f32 pad154;
                s16 temp_rand2;
                s16 sp150;

                if (this->fogMode == 0) {
                    play->envCtx.unk_D8 = 0;
                }
                this->fogMode = 0xA;

                sp150 = 1;
                if (this->work[BFD_MOVE_TIMER] & 0x1C) {
                    Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_BURN - SFX_FLAG, &this->actor.projectedPos, 4, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                }
                for (i1 = 0; i1 < sp150; i1++) {
                    if (sp150) { // Needed for matching
                        temp_rand2 = Rand_ZeroFloat(99.9f);

                        sp188.x = this->bodySegsPos[temp_rand2].x;
                        sp188.y = this->bodySegsPos[temp_rand2].y - 10.0f;
                        sp188.z = this->bodySegsPos[temp_rand2].z;

                        sp164.y = 0.03f;

                        EffectSsKFire_Spawn(play, &sp188, &sp17C, &sp164, (s16)Rand_ZeroFloat(20.0f) + 40, 0x64);

                        for (i2 = 0; i2 < 15; i2++) {
                            sp170.x = Rand_CenteredFloat(20.0f);
                            sp170.y = Rand_CenteredFloat(20.0f);
                            sp170.z = Rand_CenteredFloat(20.0f);

                            sp158.y = 0.4f;
                            sp158.x = Rand_CenteredFloat(0.5f);
                            sp158.z = Rand_CenteredFloat(0.5f);

                            BossFd_SpawnEmber(this->effects, &sp188, &sp170, &sp158, (s16)Rand_ZeroFloat(3.0f) + 8);
                        }
                    }
                }
            }
            break;
        case BOSSFD_BONES_FALL:
            this->work[BFD_STOP_FLAG] = true;
            this->fogMode = 3;
            if (this->timers[0] < 18) {
                this->bodyFallApart[this->timers[0]] = 1;
            }
            if (this->timers[0] == 0) {
                this->work[BFD_ACTION_STATE] = BOSSFD_SKULL_PAUSE;
                this->timers[0] = 15;
                this->work[BFD_CEILING_TARGET] = 0;
                player->actor.world.pos.y = 90.0f;
                player->actor.world.pos.x = 40.0f;
                player->actor.world.pos.z = 150.0f;
            }
            break;
        case BOSSFD_SKULL_PAUSE:
            if (this->timers[0] == 0) {
                this->work[BFD_ACTION_STATE] = BOSSFD_SKULL_FALL;
                this->timers[0] = 20;
                this->work[BFD_STOP_FLAG] = false;
            }
            break;
        case BOSSFD_SKULL_FALL:
            this->fwork[BFD_TURN_RATE] = this->fwork[BFD_TURN_RATE_MAX] = this->actor.speedXZ =
                this->fwork[BFD_FLY_SPEED] = 0;

            if (this->timers[0] == 1) {
                this->actor.world.pos.x = 0;
                this->actor.world.pos.y = 900.0f;
                this->actor.world.pos.z = 150.0f;
                this->actor.world.rot.x = this->actor.world.rot.y = 0;
                this->actor.shape.rot.z = 0x1200;
                this->actor.velocity.x = 0;
                this->actor.velocity.z = 0;
            }
            if (this->timers[0] == 0) {
                if (this->actor.world.pos.y <= 110.0f) {
                    this->actor.world.pos.y = 110.0f;
                    this->actor.velocity.y = 0;
                    if (this->work[BFD_CEILING_TARGET] == 0) {
                        this->work[BFD_CEILING_TARGET]++;
                        this->timers[1] = 60;
                        this->work[BFD_CAM_SHAKE_TIMER] = 20;
                        Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_LAND2, &this->actor.projectedPos, 4, &D_801333E0,
                                               &D_801333E0, &D_801333E8);
                        func_8002DF54(play, &this->actor, 5);
                        for (i1 = 0; i1 < 15; i1++) {
                            Vec3f sp144 = { 0.0f, 0.0f, 0.0f };
                            Vec3f sp138 = { 0.0f, 0.0f, 0.0f };
                            Vec3f sp12C;

                            sp144.x = Rand_CenteredFloat(8.0f);
                            sp144.y = Rand_ZeroFloat(1.0f);
                            sp144.z = Rand_CenteredFloat(8.0f);

                            sp138.y = 0.3f;

                            sp12C.x = Rand_CenteredFloat(10.0f) + this->actor.world.pos.x;
                            sp12C.y = Rand_CenteredFloat(10.0f) + this->actor.world.pos.y;
                            sp12C.z = Rand_CenteredFloat(10.0f) + this->actor.world.pos.z;
                            BossFd_SpawnDust(this->effects, &sp12C, &sp144, &sp138, Rand_ZeroFloat(100.0f) + 300);
                        }
                    }
                } else {
                    this->actor.velocity.y -= 1.0f;
                }
            } else {
                this->actor.velocity.y = 0;
            }
            if (this->timers[1] == 1) {
                this->work[BFD_ACTION_STATE] = BOSSFD_SKULL_BURN;
                this->timers[0] = 70;
            }
            break;
        case BOSSFD_SKULL_BURN:
            this->actor.velocity.y = 0.0f;
            this->actor.world.pos.y = 110.0f;
            this->fwork[BFD_TURN_RATE] = this->fwork[BFD_TURN_RATE_MAX] = this->actor.speedXZ =
                this->fwork[BFD_FLY_SPEED] = 0.0f;

            if ((50 > this->timers[0]) && (this->timers[0] > 0)) {
                Vec3f sp120;
                Vec3f sp114 = { 0.0f, 0.0f, 0.0f };
                Vec3f sp108 = { 0.0f, 0.03f, 0.0f };

                Audio_PlaySoundGeneral(NA_SE_EN_GOMA_LAST - SFX_FLAG, &this->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);

                sp120.x = Rand_CenteredFloat(40.0f) + this->actor.world.pos.x;
                sp120.y = (Rand_CenteredFloat(10.0f) + this->actor.world.pos.y) - 10.0f;
                sp120.z = (Rand_CenteredFloat(40.0f) + this->actor.world.pos.z) + 5.0f;

                sp108.y = 0.03f;

                EffectSsKFire_Spawn(play, &sp120, &sp114, &sp108, (s16)Rand_ZeroFloat(15.0f) + 30, 0);
            }
            if (this->timers[0] < 20) {
                Math_ApproachZeroF(&this->actor.scale.x, 1.0f, 0.0025f);
                Actor_SetScale(&this->actor, this->actor.scale.x);
            }
            if (this->timers[0] == 0) {
                this->actionFunc = BossFd_Wait;
                this->actor.world.pos.y -= 1000.0f;
            }
            if (this->timers[0] == 7) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, this->actor.world.pos.x,
                            this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
            }
            break;
        case BOSSFD_WAIT_INTRO:
            break;
    }

    //                                 Update body segments and mane

    if (!this->work[BFD_STOP_FLAG]) {
        s16 i4;
        Vec3f spE0[3];
        Vec3f spBC[3];
        f32 phi_f20;
        f32 padB4;
        f32 padB0;
        f32 padAC;

        Math_ApproachS(&this->actor.world.rot.y, angleToTarget, 0xA, this->fwork[BFD_TURN_RATE]);

        if (((this->work[BFD_ACTION_STATE] == BOSSFD_FLY_CHASE) ||
             (this->work[BFD_ACTION_STATE] == BOSSFD_FLY_UNUSED)) &&
            (this->actor.world.pos.y < 110.0f) && (pitchToTarget < 0)) {
            pitchToTarget = 0;
            Math_ApproachF(&this->actor.world.pos.y, 110.0f, 1.0f, 5.0f);
        }

        Math_ApproachS(&this->actor.world.rot.x, pitchToTarget, 0xA, this->fwork[BFD_TURN_RATE]);
        Math_ApproachF(&this->fwork[BFD_TURN_RATE], this->fwork[BFD_TURN_RATE_MAX], 1.0f, 20000.0f);
        Math_ApproachF(&this->actor.speedXZ, this->fwork[BFD_FLY_SPEED], 1.0f, 0.1f);
        if (this->work[BFD_ACTION_STATE] < BOSSFD_SKULL_FALL) {
            func_8002D908(&this->actor);
        }
        func_8002D7EC(&this->actor);

        this->work[BFD_LEAD_BODY_SEG]++;
        if (this->work[BFD_LEAD_BODY_SEG] >= 100) {
            this->work[BFD_LEAD_BODY_SEG] = 0;
        }
        i4 = this->work[BFD_LEAD_BODY_SEG];
        this->bodySegsPos[i4].x = this->actor.world.pos.x;
        this->bodySegsPos[i4].y = this->actor.world.pos.y;
        this->bodySegsPos[i4].z = this->actor.world.pos.z;
        this->bodySegsRot[i4].x = (this->actor.world.rot.x / (f32)0x8000) * M_PI;
        this->bodySegsRot[i4].y = (this->actor.world.rot.y / (f32)0x8000) * M_PI;
        this->bodySegsRot[i4].z = (this->actor.world.rot.z / (f32)0x8000) * M_PI;

        this->work[BFD_LEAD_MANE_SEG]++;
        if (this->work[BFD_LEAD_MANE_SEG] >= 30) {
            this->work[BFD_LEAD_MANE_SEG] = 0;
        }
        i4 = this->work[BFD_LEAD_MANE_SEG];
        this->centerMane.scale[i4] = (Math_SinS(this->work[BFD_MOVE_TIMER] * 5596.0f) * 0.3f) + 1.0f;
        this->rightMane.scale[i4] = (Math_SinS(this->work[BFD_MOVE_TIMER] * 5496.0f) * 0.3f) + 1.0f;
        this->leftMane.scale[i4] = (Math_CosS(this->work[BFD_MOVE_TIMER] * 5696.0f) * 0.3f) + 1.0f;
        this->centerMane.pos[i4] = this->centerMane.head;
        this->fireManeRot[i4].x = (this->actor.world.rot.x / (f32)0x8000) * M_PI;
        this->fireManeRot[i4].y = (this->actor.world.rot.y / (f32)0x8000) * M_PI;
        this->fireManeRot[i4].z = (this->actor.world.rot.z / (f32)0x8000) * M_PI;
        this->rightMane.pos[i4] = this->rightMane.head;
        this->leftMane.pos[i4] = this->leftMane.head;

        if ((0x3000 > this->actor.world.rot.x) && (this->actor.world.rot.x > -0x3000)) {
            Math_ApproachF(&this->flattenMane, 1.0f, 1.0f, 0.05f);
        } else {
            Math_ApproachF(&this->flattenMane, 0.5f, 1.0f, 0.05f);
        }

        if (this->work[BFD_ACTION_STATE] < BOSSFD_SKULL_FALL) {
            if ((this->actor.prevPos.y < 90.0f) && (90.0f <= this->actor.world.pos.y)) {
                this->timers[4] = 80;
                func_80033E1C(play, 1, 80, 0x5000);
                this->work[BFD_ROAR_TIMER] = 40;
                this->work[BFD_MANE_EMBERS_TIMER] = 30;
                this->work[BFD_SPLASH_TIMER] = 10;
            }
            if ((this->actor.prevPos.y > 90.0f) && (90.0f >= this->actor.world.pos.y)) {
                this->timers[4] = 80;
                func_80033E1C(play, 1, 80, 0x5000);
                this->work[BFD_MANE_EMBERS_TIMER] = 30;
                this->work[BFD_SPLASH_TIMER] = 10;
            }
        }

        if (!sp1CF) {
            spE0[0].x = spE0[0].y = Math_SinS(this->work[BFD_MOVE_TIMER] * 1500.0f) * 3000.0f;
            spE0[1].x = Math_SinS(this->work[BFD_MOVE_TIMER] * 2000.0f) * 4000.0f;
            spE0[1].y = Math_SinS(this->work[BFD_MOVE_TIMER] * 2200.0f) * 4000.0f;
            spE0[2].x = Math_SinS(this->work[BFD_MOVE_TIMER] * 1700.0f) * 2000.0f;
            spE0[2].y = Math_SinS(this->work[BFD_MOVE_TIMER] * 1900.0f) * 2000.0f;
            spBC[0].x = spBC[0].y = Math_SinS(this->work[BFD_MOVE_TIMER] * 1500.0f) * -3000.0f;
            spBC[1].x = Math_SinS(this->work[BFD_MOVE_TIMER] * 2200.0f) * -4000.0f;
            spBC[1].y = Math_SinS(this->work[BFD_MOVE_TIMER] * 2000.0f) * -4000.0f;
            spBC[2].x = Math_SinS(this->work[BFD_MOVE_TIMER] * 1900.0f) * -2000.0f;
            spBC[2].y = Math_SinS(this->work[BFD_MOVE_TIMER] * 1700.0f) * -2000.0f;

            for (i3 = 0; i3 < 3; i3++) {
                Math_ApproachF(&this->rightArmRot[i3].x, spE0[i3].x, 1.0f, 1000.0f);
                Math_ApproachF(&this->rightArmRot[i3].y, spE0[i3].y, 1.0f, 1000.0f);
                Math_ApproachF(&this->leftArmRot[i3].x, spBC[i3].x, 1.0f, 1000.0f);
                Math_ApproachF(&this->leftArmRot[i3].y, spBC[i3].y, 1.0f, 1000.0f);
            }
        } else {
            for (i2 = 0; i2 < 3; i2++) {
                phi_f20 = 0.0f;
                Math_ApproachZeroF(&this->rightArmRot[i2].y, 0.1f, 100.0f);
                Math_ApproachZeroF(&this->leftArmRot[i2].y, 0.1f, 100.0f);
                if (i2 == 0) {
                    phi_f20 = -3000.0f;
                }
                Math_ApproachF(&this->rightArmRot[i2].x, phi_f20, 0.1f, 100.0f);
                Math_ApproachF(&this->leftArmRot[i2].x, -phi_f20, 0.1f, 100.0f);
            }
        }
    }
}

void BossFd_Wait(BossFd* this, PlayState* play) {
    if (this->handoffSignal == FD2_SIGNAL_FLY) { // Set by BossFd2
        u8 temp_rand;

        this->handoffSignal = FD2_SIGNAL_NONE;
        BossFd_SetupFly(this, play);
        do {
            temp_rand = Rand_ZeroFloat(8.9f);
        } while (temp_rand == this->holeIndex);
        this->holeIndex = temp_rand;
        this->targetPosition.x = sHoleLocations[this->holeIndex].x;
        this->targetPosition.y = sHoleLocations[this->holeIndex].y - 200.0f;
        this->targetPosition.z = sHoleLocations[this->holeIndex].z;
        this->actor.world.pos = this->targetPosition;

        this->timers[0] = 10;
        this->work[BFD_ACTION_STATE] = BOSSFD_EMERGE;
        this->work[BFD_START_ATTACK] = true;
    }
    if (this->handoffSignal == FD2_SIGNAL_DEATH) {
        this->handoffSignal = FD2_SIGNAL_NONE;
        BossFd_SetupFly(this, play);
        this->holeIndex = 1;
        this->targetPosition.x = sHoleLocations[1].x;
        this->targetPosition.y = sHoleLocations[1].y - 200.0f;
        this->targetPosition.z = sHoleLocations[1].z;
        this->actor.world.pos = this->targetPosition;
        this->timers[0] = 10;
        this->work[BFD_ACTION_STATE] = BOSSFD_EMERGE;
    }
}

static Vec3f sFireAudioVec = { 0.0f, 0.0f, 50.0f };

void BossFd_Effects(BossFd* this, PlayState* play) {
    static Color_RGBA8 colorYellow = { 255, 255, 0, 255 };
    static Color_RGBA8 colorRed = { 255, 10, 0, 255 };
    s16 breathOpacity = 0;
    f32 jawAngle;
    f32 jawSpeed;
    f32 emberRate;
    f32 emberSpeed;
    s16 eyeStates[] = { EYE_OPEN, EYE_HALF, EYE_CLOSED, EYE_CLOSED, EYE_HALF };
    f32 temp_x;
    f32 temp_z;
    s16 i;

    if (this->fogMode == 0) {
        play->envCtx.unk_BF = 0;
        play->envCtx.unk_D8 = 0.5f + 0.5f * Math_SinS(this->work[BFD_VAR_TIMER] * 0x500);
        play->envCtx.unk_DC = 2;
        play->envCtx.unk_BD = 1;
        play->envCtx.unk_BE = 0;
    } else if (this->fogMode == 3) {
        play->envCtx.unk_BF = 0;
        play->envCtx.unk_DC = 2;
        play->envCtx.unk_BD = 2;
        play->envCtx.unk_BE = 0;
        Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.05f);
    } else if (this->fogMode == 2) {
        this->fogMode--;
        play->envCtx.unk_BF = 0;
        Math_ApproachF(&play->envCtx.unk_D8, 0.55f + 0.05f * Math_SinS(this->work[BFD_VAR_TIMER] * 0x3E00), 1.0f,
                       0.15f);
        play->envCtx.unk_DC = 2;
        play->envCtx.unk_BD = 3;
        play->envCtx.unk_BE = 0;
    } else if (this->fogMode == 10) {
        this->fogMode = 1;
        play->envCtx.unk_BF = 0;
        Math_ApproachF(&play->envCtx.unk_D8, 0.21f + 0.07f * Math_SinS(this->work[BFD_VAR_TIMER] * 0xC00), 1.0f,
                       0.05f);
        play->envCtx.unk_DC = 2;
        play->envCtx.unk_BD = 3;
        play->envCtx.unk_BE = 0;
    } else if (this->fogMode == 1) {
        Math_ApproachF(&play->envCtx.unk_D8, 0.0f, 1.0f, 0.03f);
        if (play->envCtx.unk_D8 <= 0.01f) {
            this->fogMode = 0;
        }
    }

    if (this->work[BFD_MANE_EMBERS_TIMER] != 0) {
        this->work[BFD_MANE_EMBERS_TIMER]--;
        emberSpeed = emberRate = 20.0f;
    } else {
        emberRate = 3.0f;
        emberSpeed = 5.0f;
    }
    Math_ApproachF(&this->fwork[BFD_MANE_EMBER_RATE], emberRate, 1.0f, 0.1f);
    Math_ApproachF(&this->fwork[BFD_MANE_EMBER_SPEED], emberSpeed, 1.0f, 0.5f);

    if (((this->work[BFD_VAR_TIMER] % 8) == 0) && (Rand_ZeroOne() < 0.3f)) {
        this->work[BFD_BLINK_TIMER] = 4;
    }
    this->eyeState = eyeStates[this->work[BFD_BLINK_TIMER]];

    if (this->work[BFD_BLINK_TIMER] != 0) {
        this->work[BFD_BLINK_TIMER]--;
    }

    if (this->work[BFD_ROAR_TIMER] != 0) {
        if (this->work[BFD_ROAR_TIMER] == 37) {
            Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_ROAR, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
        jawAngle = 6000.0f;
        jawSpeed = 1300.0f;
    } else {
        jawAngle = (this->work[BFD_VAR_TIMER] & 0x10) ? 0.0f : 1000.0f;
        jawSpeed = 500.0f;
    }
    Math_ApproachF(&this->jawOpening, jawAngle, 0.3f, jawSpeed);

    if (this->work[BFD_ROAR_TIMER] != 0) {
        this->work[BFD_ROAR_TIMER]--;
    }

    if (this->timers[4] != 0) {
        Vec3f spawnVel1;
        Vec3f spawnAccel1;
        Vec3f spawnPos1;
        s32 pad;

        Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_APPEAR - SFX_FLAG, &this->actor.projectedPos, 4, &D_801333E0,
                               &D_801333E0, &D_801333E8);
        if (this->work[BFD_SPLASH_TIMER] != 0) {
            this->work[BFD_SPLASH_TIMER]--;
            if ((this->actor.colChkInfo.health == 0) ||
                ((this->introState == BFD_CS_EMERGE) && (this->actor.world.rot.x > 0x3000))) {
                if ((u8)this->fogMode == 0) {
                    play->envCtx.unk_D8 = 0.0f;
                }
                this->fogMode = 2;
            }
            for (i = 0; i < 5; i++) {
                spawnVel1.x = Rand_CenteredFloat(20.0f);
                spawnVel1.y = Rand_ZeroFloat(5.0f) + 4.0f;
                spawnVel1.z = Rand_CenteredFloat(20.0f);

                spawnAccel1.x = spawnAccel1.z = 0.0f;
                spawnAccel1.y = -0.3f;

                temp_x = (spawnVel1.x * 20) / 10.0f;
                temp_z = (spawnVel1.z * 20) / 10.0f;
                spawnPos1.x = temp_x + this->holePosition.x;
                spawnPos1.y = 100.0f;
                spawnPos1.z = temp_z + this->holePosition.z;

                func_8002836C(play, &spawnPos1, &spawnVel1, &spawnAccel1, &colorYellow, &colorRed,
                              (s16)Rand_ZeroFloat(150.0f) + 800, 10, (s16)Rand_ZeroFloat(5.0f) + 17);
            }
        } else {
            for (i = 0; i < 2; i++) {
                spawnVel1.x = Rand_CenteredFloat(10.0f);
                spawnVel1.y = Rand_ZeroFloat(3.0f) + 3.0f;
                spawnVel1.z = Rand_CenteredFloat(10.0f);

                spawnAccel1.x = spawnAccel1.z = 0.0f;
                spawnAccel1.y = -0.3f;
                temp_x = (spawnVel1.x * 50) / 10.0f;
                temp_z = (spawnVel1.z * 50) / 10.0f;

                spawnPos1.x = temp_x + this->holePosition.x;
                spawnPos1.y = 100.0f;
                spawnPos1.z = temp_z + this->holePosition.z;

                func_8002836C(play, &spawnPos1, &spawnVel1, &spawnAccel1, &colorYellow, &colorRed, 500, 10, 20);
            }
        }

        for (i = 0; i < 8; i++) {
            spawnVel1.x = Rand_CenteredFloat(20.0f);
            spawnVel1.y = Rand_ZeroFloat(10.0f);
            spawnVel1.z = Rand_CenteredFloat(20.0f);

            spawnAccel1.y = 0.4f;
            spawnAccel1.x = Rand_CenteredFloat(0.5f);
            spawnAccel1.z = Rand_CenteredFloat(0.5f);

            spawnPos1.x = Rand_CenteredFloat(60.0) + this->holePosition.x;
            spawnPos1.y = Rand_ZeroFloat(40.0f) + 100.0f;
            spawnPos1.z = Rand_CenteredFloat(60.0) + this->holePosition.z;

            BossFd_SpawnEmber(this->effects, &spawnPos1, &spawnVel1, &spawnAccel1, (s16)Rand_ZeroFloat(1.5f) + 6);
        }
    }

    if ((this->fireBreathTimer != 0) && (this->fireBreathTimer < 17)) {
        breathOpacity = (this->fireBreathTimer >= 6) ? 255 : this->fireBreathTimer * 50;
    }
    if (breathOpacity != 0) {
        f32 spawnAngleX;
        f32 spawnAngleY;
        Vec3f spawnSpeed2 = { 0.0f, 0.0f, 0.0f };
        Vec3f spawnVel2;
        Vec3f spawnAccel2 = { 0.0f, 0.0f, 0.0f };
        Vec3f spawnPos2;

        this->fogMode = 2;
        spawnSpeed2.z = 30.0f;

        Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_FIRE - SFX_FLAG, &sFireAudioVec, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        spawnPos2 = this->headPos;

        spawnAngleY = (this->actor.world.rot.y / (f32)0x8000) * M_PI;
        spawnAngleX = (((-this->actor.world.rot.x) / (f32)0x8000) * M_PI) + 0.3f;
        Matrix_RotateY(spawnAngleY, MTXMODE_NEW);
        Matrix_RotateX(spawnAngleX, MTXMODE_APPLY);
        Matrix_MultVec3f(&spawnSpeed2, &spawnVel2);

        BossFd_SpawnFireBreath(this->effects, &spawnPos2, &spawnVel2, &spawnAccel2,
                               50.0f * Math_SinS(this->work[BFD_VAR_TIMER] * 0x2000) + 300.0f, breathOpacity,
                               this->actor.world.rot.y);

        spawnPos2.x += spawnVel2.x * 0.5f;
        spawnPos2.y += spawnVel2.y * 0.5f;
        spawnPos2.z += spawnVel2.z * 0.5f;

        BossFd_SpawnFireBreath(this->effects, &spawnPos2, &spawnVel2, &spawnAccel2,
                               50.0f * Math_SinS(this->work[BFD_VAR_TIMER] * 0x2000) + 300.0f, breathOpacity,
                               this->actor.world.rot.y);
        spawnSpeed2.x = 0.0f;
        spawnSpeed2.y = 17.0f;
        spawnSpeed2.z = 0.0f;

        for (i = 0; i < 6; i++) {
            spawnAngleY = Rand_ZeroFloat(2.0f * M_PI);
            spawnAngleX = Rand_ZeroFloat(2.0f * M_PI);
            Matrix_RotateY(spawnAngleY, MTXMODE_NEW);
            Matrix_RotateX(spawnAngleX, MTXMODE_APPLY);
            Matrix_MultVec3f(&spawnSpeed2, &spawnVel2);

            spawnAccel2.x = (spawnVel2.x * -10) / 100;
            spawnAccel2.y = (spawnVel2.y * -10) / 100;
            spawnAccel2.z = (spawnVel2.z * -10) / 100;

            BossFd_SpawnEmber(this->effects, &this->headPos, &spawnVel2, &spawnAccel2, (s16)Rand_ZeroFloat(2.0f) + 8);
        }
    }

    if ((this->actor.world.pos.y < 90.0f) || (700.0f < this->actor.world.pos.y) || (this->actionFunc == BossFd_Wait)) {
        this->actor.flags &= ~ACTOR_FLAG_0;
    } else {
        this->actor.flags |= ACTOR_FLAG_0;
    }
}

void BossFd_CollisionCheck(BossFd* this, PlayState* play) {
    ColliderJntSphElement* headCollider = &this->collider.elements[0];
    ColliderInfo* hurtbox;

    if (headCollider->info.bumperFlags & BUMP_HIT) {
        headCollider->info.bumperFlags &= ~BUMP_HIT;
        hurtbox = headCollider->info.acHitInfo;
        this->actor.colChkInfo.health -= 2;
        if (hurtbox->toucher.dmgFlags & 0x1000) {
            this->actor.colChkInfo.health -= 2;
        }
        if ((s8)this->actor.colChkInfo.health <= 2) {
            this->actor.colChkInfo.health = 2;
        }
        this->work[BFD_DAMAGE_FLASH_TIMER] = 10;
        this->work[BFD_INVINC_TIMER] = 20;
        Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_DAMAGE1, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

void BossFd_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    BossFd* this = (BossFd*)thisx;
    f32 headGlow;
    f32 rManeGlow;
    f32 lManeGlow;
    s16 i;

    osSyncPrintf("FD MOVE START \n");
    this->work[BFD_VAR_TIMER]++;
    this->work[BFD_MOVE_TIMER]++;
    this->actionFunc(this, play);

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }
    if (this->fireBreathTimer != 0) {
        this->fireBreathTimer--;
    }
    if (this->work[BFD_DAMAGE_FLASH_TIMER] != 0) {
        this->work[BFD_DAMAGE_FLASH_TIMER]--;
    }
    if (this->work[BFD_INVINC_TIMER] != 0) {
        this->work[BFD_INVINC_TIMER]--;
    }
    if (this->work[BFD_ACTION_STATE] < BOSSFD_DEATH_START) {
        if (this->work[BFD_INVINC_TIMER] == 0) {
            BossFd_CollisionCheck(this, play);
        }
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }

    BossFd_Effects(this, play);
    this->fwork[BFD_TEX1_SCROLL_X] += 4.0f;
    this->fwork[BFD_TEX1_SCROLL_Y] = 120.0f;
    this->fwork[BFD_TEX2_SCROLL_X] += 3.0f;
    this->fwork[BFD_TEX2_SCROLL_Y] -= 2.0f;

    Math_ApproachF(&this->fwork[BFD_BODY_TEX2_ALPHA], (this->work[BFD_VAR_TIMER] & 0x10) ? 30.0f : 158.0f, 1.0f, 8.0f);
    if (this->skinSegments == 0) {
        this->fwork[BFD_HEAD_TEX2_ALPHA] = this->fwork[BFD_BODY_TEX2_ALPHA];
    } else {
        headGlow = (this->work[BFD_VAR_TIMER] & 4) ? 0.0f : 255.0f;
        Math_ApproachF(&this->fwork[BFD_HEAD_TEX2_ALPHA], headGlow, 1.0f, 64.0f);
    }

    headGlow = (this->work[BFD_VAR_TIMER] & 8) ? 128.0f : 255.0f;
    rManeGlow = ((this->work[BFD_VAR_TIMER] + 3) & 8) ? 128.0f : 255.0f;
    lManeGlow = ((this->work[BFD_VAR_TIMER] + 6) & 8) ? 128.0f : 255.0f;

    Math_ApproachF(&this->fwork[BFD_MANE_COLOR_CENTER], headGlow, 1.0f, 16.0f);
    Math_ApproachF(&this->fwork[BFD_MANE_COLOR_RIGHT], rManeGlow, 1.0f, 16.0f);
    Math_ApproachF(&this->fwork[BFD_MANE_COLOR_LEFT], lManeGlow, 1.0f, 16.0f);

    if (this->work[BFD_ROCK_TIMER] != 0) {
        this->work[BFD_ROCK_TIMER]--;
        if ((this->work[BFD_ROCK_TIMER] % 16) == 0) {
            EnVbBall* bossFdRock = (EnVbBall*)Actor_SpawnAsChild(
                &play->actorCtx, &this->actor, play, ACTOR_EN_VB_BALL, this->actor.world.pos.x, 1000.0f,
                this->actor.world.pos.z, 0, 0, (s16)Rand_ZeroFloat(50.0f) + 130, 100);

            if (bossFdRock != NULL) {
                for (i = 0; i < 10; i++) {
                    Vec3f debrisVel = { 0.0f, 0.0f, 0.0f };
                    Vec3f debrisAccel = { 0.0f, -1.0f, 0.0f };
                    Vec3f debrisPos;

                    debrisPos.x = Rand_CenteredFloat(300.0f) + bossFdRock->actor.world.pos.x;
                    debrisPos.y = Rand_CenteredFloat(300.0f) + bossFdRock->actor.world.pos.y;
                    debrisPos.z = Rand_CenteredFloat(300.0f) + bossFdRock->actor.world.pos.z;

                    BossFd_SpawnDebris(this->effects, &debrisPos, &debrisVel, &debrisAccel,
                                       (s16)Rand_ZeroFloat(15.0f) + 20);
                }
            }
        }
    }

    if (1) { // Needed for matching, and also to define new variables
        Vec3f emberVel = { 0.0f, 0.0f, 0.0f };
        Vec3f emberAccel = { 0.0f, 0.0f, 0.0f };
        Vec3f emberPos;
        s16 temp_rand;

        for (i = 0; i < 6; i++) {
            emberAccel.y = 0.4f;
            emberAccel.x = Rand_CenteredFloat(0.5f);
            emberAccel.z = Rand_CenteredFloat(0.5f);

            temp_rand = Rand_ZeroFloat(8.9f);

            emberPos.x = sHoleLocations[temp_rand].x + Rand_CenteredFloat(60.0f);
            emberPos.y = (sHoleLocations[temp_rand].y + 10.0f) + Rand_ZeroFloat(40.0f);
            emberPos.z = sHoleLocations[temp_rand].z + Rand_CenteredFloat(60.0f);

            BossFd_SpawnEmber(this->effects, &emberPos, &emberVel, &emberAccel, (s16)Rand_ZeroFloat(2.0f) + 6);
        }

        if (this->skinSegments != 0) {
            for (i = 0; i < (s16)this->fwork[BFD_MANE_EMBER_RATE]; i++) {
                temp_rand = Rand_ZeroFloat(29.9f);
                emberPos.y = this->centerMane.pos[temp_rand].y + Rand_CenteredFloat(20.0f);

                if (emberPos.y >= 90.0f) {
                    emberPos.x = this->centerMane.pos[temp_rand].x + Rand_CenteredFloat(20.0f);
                    emberPos.z = this->centerMane.pos[temp_rand].z + Rand_CenteredFloat(20.0f);

                    emberVel.x = Rand_CenteredFloat(this->fwork[BFD_MANE_EMBER_SPEED]);
                    emberVel.y = Rand_CenteredFloat(this->fwork[BFD_MANE_EMBER_SPEED]);
                    emberVel.z = Rand_CenteredFloat(this->fwork[BFD_MANE_EMBER_SPEED]);

                    emberAccel.y = 0.4f;
                    emberAccel.x = Rand_CenteredFloat(0.5f);
                    emberAccel.z = Rand_CenteredFloat(0.5f);

                    BossFd_SpawnEmber(this->effects, &emberPos, &emberVel, &emberAccel, (s16)Rand_ZeroFloat(2.0f) + 8);
                }
            }
        }
    }
    osSyncPrintf("FD MOVE END 1\n");
    BossFd_UpdateEffects(this, play);
    osSyncPrintf("FD MOVE END 2\n");
}

void BossFd_UpdateEffects(BossFd* this, PlayState* play) {
    BossFdEffect* effect = this->effects;
    Player* player = GET_PLAYER(play);
    Color_RGB8 colors[4] = { { 255, 128, 0 }, { 255, 0, 0 }, { 255, 255, 0 }, { 255, 0, 0 } };
    Vec3f diff;
    s16 i1;
    s16 i2;

    for (i1 = 0; i1 < 180; i1++, effect++) {
        if (effect->type != BFD_FX_NONE) {
            effect->timer1++;

            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;

            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;
            if (effect->type == BFD_FX_EMBER) {
                s16 cInd = effect->timer1 % 4;

                effect->color.r = colors[cInd].r;
                effect->color.g = colors[cInd].g;
                effect->color.b = colors[cInd].b;
                effect->alpha -= 20;
                if (effect->alpha <= 0) {
                    effect->alpha = 0;
                    effect->type = 0;
                }
            } else if ((effect->type == BFD_FX_DEBRIS) || (effect->type == BFD_FX_SKULL_PIECE)) {
                effect->vFdFxRotX += 0.55f;
                effect->vFdFxRotY += 0.1f;
                if (effect->pos.y <= 100.0f) {
                    effect->type = 0;
                }
            } else if (effect->type == BFD_FX_DUST) {
                if (effect->timer2 >= 8) {
                    effect->timer2 = 8;
                    effect->type = 0;
                } else if (((effect->timer1 % 2) != 0) || (Rand_ZeroOne() < 0.3f)) {
                    effect->timer2++;
                }
            } else if (effect->type == BFD_FX_FIRE_BREATH) {
                diff.x = player->actor.world.pos.x - effect->pos.x;
                diff.y = player->actor.world.pos.y + 30.0f - effect->pos.y;
                diff.z = player->actor.world.pos.z - effect->pos.z;
                if ((this->timers[3] == 0) && (sqrtf(SQ(diff.x) + SQ(diff.y) + SQ(diff.z)) < 20.0f)) {
                    this->timers[3] = 50;
                    func_8002F6D4(play, NULL, 5.0f, effect->kbAngle, 0.0f, 0x30);
                    if (player->isBurning == false) {
                        for (i2 = 0; i2 < ARRAY_COUNT(player->flameTimers); i2++) {
                            player->flameTimers[i2] = Rand_S16Offset(0, 200);
                        }
                        player->isBurning = true;
                    }
                }
                if (effect->timer2 == 0) {
                    if (effect->scale < 2.5f) {
                        effect->scale += effect->vFdFxScaleMod;
                        effect->vFdFxScaleMod += 0.08f;
                    }
                    if ((effect->pos.y <= (effect->vFdFxYStop + 130.0f)) || (effect->timer1 >= 10)) {
                        effect->accel.y = 5.0f;
                        effect->timer2++;
                        effect->velocity.y = 0.0f;
                        effect->accel.x = (effect->velocity.x * -25.0f) / 100.0f;
                        effect->accel.z = (effect->velocity.z * -25.0f) / 100.0f;
                    }
                } else {
                    if (effect->scale < 2.5f) {
                        Math_ApproachF(&effect->scale, 2.5f, 0.5f, 0.5f);
                    }
                    effect->timer2++;
                    if (effect->timer2 >= 9) {
                        effect->type = 0;
                    }
                }
            }
        }
    }
}

void BossFd_DrawEffects(BossFdEffect* effect, PlayState* play) {
    static void* dustTex[] = {
        gDust1Tex, gDust1Tex, gDust2Tex, gDust3Tex, gDust4Tex, gDust5Tex, gDust6Tex, gDust7Tex, gDust8Tex,
    };
    u8 flag = false;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 i;
    BossFdEffect* firstEffect = effect;

    OPEN_DISPS(gfxCtx);

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_EMBER) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                func_80093D84(play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gVolvagiaEmberMaterialDL);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, effect->color.r, effect->color.g, effect->color.b, effect->alpha);
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gVolvagiaEmberModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = firstEffect;
    flag = false;
    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_DEBRIS) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                func_80093D18(play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gVolvagiaDebrisMaterialDL);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateY(effect->vFdFxRotY, MTXMODE_APPLY);
            Matrix_RotateX(effect->vFdFxRotX, MTXMODE_APPLY);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gVolvagiaDebrisModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = firstEffect;
    flag = false;
    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_DUST) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0);
                gSPDisplayList(POLY_XLU_DISP++, gVolvagiaDustMaterialDL);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 90, 30, 0, 255);
                gDPSetEnvColor(POLY_XLU_DISP++, 90, 30, 0, 0);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&play->billboardMtxF);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(dustTex[effect->timer2]));
            gSPDisplayList(POLY_XLU_DISP++, gVolvagiaDustModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = firstEffect;
    flag = false;
    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_FIRE_BREATH) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0);
                gSPDisplayList(POLY_XLU_DISP++, gVolvagiaDustMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 10, 0, 255);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 0, effect->alpha);
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&play->billboardMtxF);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(dustTex[effect->timer2]));
            gSPDisplayList(POLY_XLU_DISP++, gVolvagiaDustModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = firstEffect;
    flag = false;
    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_SKULL_PIECE) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                func_80093D84(play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gVolvagiaSkullPieceMaterialDL);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateY(effect->vFdFxRotY, MTXMODE_APPLY);
            Matrix_RotateX(effect->vFdFxRotX, MTXMODE_APPLY);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gVolvagiaSkullPieceModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void BossFd_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BossFd* this = (BossFd*)thisx;

    osSyncPrintf("FD DRAW START\n");
    if (this->actionFunc != BossFd_Wait) {
        OPEN_DISPS(play->state.gfxCtx);
        func_80093D18(play->state.gfxCtx);
        if (this->work[BFD_DAMAGE_FLASH_TIMER] & 2) {
            POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 255, 255, 0, 900, 1099);
        }

        BossFd_DrawBody(play, this);
        POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
        CLOSE_DISPS(play->state.gfxCtx);
    }

    osSyncPrintf("FD DRAW END\n");
    BossFd_DrawEffects(this->effects, play);
    osSyncPrintf("FD DRAW END2\n");
}

s32 BossFd_OverrideRightArmDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    BossFd* this = (BossFd*)thisx;

    switch (limbIndex) {
        case 1:
            rot->y += 4000.0f + this->rightArmRot[0].x;
            break;
        case 2:
            rot->y += this->rightArmRot[1].x;
            rot->z += this->rightArmRot[1].y;
            break;
        case 3:
            rot->y += this->rightArmRot[2].x;
            rot->z += this->rightArmRot[2].y;
            break;
    }
    if (this->skinSegments < limbIndex) {
        *dList = NULL;
    }
    return false;
}

s32 BossFd_OverrideLeftArmDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                               void* thisx) {
    BossFd* this = (BossFd*)thisx;

    switch (limbIndex) {
        case 1:
            rot->y += -4000.0f + this->leftArmRot[0].x;
            break;
        case 2:
            rot->y += this->leftArmRot[1].x;
            rot->z += this->leftArmRot[1].y;
            break;
        case 3:
            rot->y += this->leftArmRot[2].x;
            rot->z += this->leftArmRot[2].y;
            break;
    }
    if (this->skinSegments < limbIndex) {
        *dList = NULL;
    }
    return false;
}

static s16 sBodyIndex[] = { 0, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5 };
static s16 sManeIndex[] = { 0, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10 }; // Unused

void BossFd_DrawMane(PlayState* play, BossFd* this, Vec3f* manePos, Vec3f* maneRot, f32* maneScale, u8 mode) {
    f32 sp140[] = { 0.0f, 10.0f, 17.0f, 20.0f, 19.5f, 18.0f, 17.0f, 15.0f, 15.0f, 15.0f };
    f32 sp118[] = { 0.0f, 10.0f, 17.0f, 20.0f, 21.0f, 21.0f, 21.0f, 21.0f, 21.0f, 21.0f };
    f32 spF0[] = { 0.4636457f, 0.3366129f, 0.14879614f, 0.04995025f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // arctan of {0.5, 0.35, 0.15, 0.05, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
    f32 spC8[] = { -0.4636457f, -0.3366129f, -0.14879614f, 0.024927188f, 0.07478157f,
                   0.04995025f, 0.09961288f, 0.0f,         0.0f,         0.0f };
    // arctan of {-0.5, -0.35, -0.15, 0.025, 0.075, 0.05, 0.1, 0.0, 0.0}
    s16 maneIndex;
    s16 i;
    s16 maneLength;
    Vec3f spB4;
    Vec3f spA8;
    f32 phi_f20;
    f32 phi_f22;

    OPEN_DISPS(play->state.gfxCtx);

    maneLength = this->skinSegments;
    maneLength = CLAMP_MAX(maneLength, 10);

    for (i = 0; i < maneLength; i++) {
        maneIndex = (this->work[BFD_LEAD_MANE_SEG] - (i * 2) + 30) % 30;

        if (mode == 0) {
            spB4.x = spB4.z = 0.0f;
            spB4.y = ((sp140[i] * 0.1f) * 10.0f) * this->flattenMane;
            phi_f20 = 0.0f;
            phi_f22 = spC8[i] * this->flattenMane;
        } else if (mode == 1) {
            phi_f22 = (spC8[i] * this->flattenMane) * 0.7f;
            phi_f20 = spF0[i] * this->flattenMane;

            spB4.y = (sp140[i] * this->flattenMane) * 0.7f;
            spB4.x = -sp118[i] * this->flattenMane;
            spB4.z = 0.0f;
        } else {
            phi_f22 = (spC8[i] * this->flattenMane) * 0.7f;
            phi_f20 = -spF0[i] * this->flattenMane;

            spB4.y = (sp140[i] * this->flattenMane) * 0.7f;
            spB4.x = sp118[i] * this->flattenMane;
            spB4.z = 0.0f;
        }

        Matrix_RotateY((maneRot + maneIndex)->y, MTXMODE_NEW);
        Matrix_RotateX(-(maneRot + maneIndex)->x, MTXMODE_APPLY);

        Matrix_MultVec3f(&spB4, &spA8);

        Matrix_Translate((manePos + maneIndex)->x + spA8.x, (manePos + maneIndex)->y + spA8.y,
                         (manePos + maneIndex)->z + spA8.z, MTXMODE_NEW);
        Matrix_RotateY((maneRot + maneIndex)->y + phi_f20, MTXMODE_APPLY);
        Matrix_RotateX(-((maneRot + maneIndex)->x + phi_f22), MTXMODE_APPLY);
        Matrix_Scale(maneScale[maneIndex] * (0.01f - (i * 0.0008f)), maneScale[maneIndex] * (0.01f - (i * 0.0008f)),
                     0.01f, MTXMODE_APPLY);
        Matrix_RotateX(-M_PI / 2.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gVolvagiaManeModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 BossFd_OverrideHeadDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossFd* this = (BossFd*)thisx;

    switch (limbIndex) {
        case 5:
        case 6:
            rot->z -= this->jawOpening * 0.1f;
            break;
        case 2:
            rot->z += this->jawOpening;
            break;
    }
    if ((this->faceExposed == true) && (limbIndex == 5)) {
        *dList = gVolvagiaBrokenFaceDL;
    }
    if (this->skinSegments == 0) {
        if (limbIndex == 6) {
            *dList = gVolvagiaSkullDL;
        } else if (limbIndex == 2) {
            *dList = gVolvagiaJawboneDL;
        } else {
            *dList = NULL;
        }
    }
    return false;
}

void BossFd_PostHeadDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f targetMod = { 4500.0f, 0.0f, 0.0f };
    static Vec3f headMod = { 4000.0f, 0.0f, 0.0f };
    BossFd* this = (BossFd*)thisx;

    if (limbIndex == 5) {
        Matrix_MultVec3f(&targetMod, &this->actor.focus.pos);
        Matrix_MultVec3f(&headMod, &this->headPos);
    }
}

static void* sEyeTextures[] = {
    gVolvagiaEyeOpenTex,
    gVolvagiaEyeHalfTex,
    gVolvagiaEyeClosedTex,
};

static Gfx* sBodyDLists[] = {
    gVolvagiaBodySeg1DL,  gVolvagiaBodySeg2DL,  gVolvagiaBodySeg3DL,  gVolvagiaBodySeg4DL,  gVolvagiaBodySeg5DL,
    gVolvagiaBodySeg6DL,  gVolvagiaBodySeg7DL,  gVolvagiaBodySeg8DL,  gVolvagiaBodySeg9DL,  gVolvagiaBodySeg10DL,
    gVolvagiaBodySeg11DL, gVolvagiaBodySeg12DL, gVolvagiaBodySeg13DL, gVolvagiaBodySeg14DL, gVolvagiaBodySeg15DL,
    gVolvagiaBodySeg16DL, gVolvagiaBodySeg17DL, gVolvagiaBodySeg18DL,
};

void BossFd_DrawBody(PlayState* play, BossFd* this) {
    s16 segIndex;
    s16 i;
    f32 temp_float;
    Mtx* tempMat = Graph_Alloc(play->state.gfxCtx, 18 * sizeof(Mtx));

    OPEN_DISPS(play->state.gfxCtx);
    if (this->skinSegments != 0) {
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sEyeTextures[this->eyeState]));
    }
    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, (s16)this->fwork[BFD_TEX1_SCROLL_X],
                                (s16)this->fwork[BFD_TEX1_SCROLL_Y], 0x20, 0x20, 1, (s16)this->fwork[BFD_TEX2_SCROLL_X],
                                (s16)this->fwork[BFD_TEX2_SCROLL_Y], 0x20, 0x20));
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, (s8)this->fwork[BFD_BODY_TEX2_ALPHA]);

    osSyncPrintf("LH\n");
    Matrix_Push();
    segIndex = (this->work[BFD_LEAD_BODY_SEG] + sBodyIndex[2]) % 100;
    Matrix_Translate(this->bodySegsPos[segIndex].x, this->bodySegsPos[segIndex].y, this->bodySegsPos[segIndex].z,
                     MTXMODE_NEW);
    Matrix_RotateY(this->bodySegsRot[segIndex].y, MTXMODE_APPLY);
    Matrix_RotateX(-this->bodySegsRot[segIndex].x, MTXMODE_APPLY);
    Matrix_Translate(-13.0f, -5.0f, 13.0f, MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x * 0.1f, this->actor.scale.y * 0.1f, this->actor.scale.z * 0.1f, MTXMODE_APPLY);
    SkelAnime_DrawOpa(play, this->skelAnimeRightArm.skeleton, this->skelAnimeRightArm.jointTable,
                      BossFd_OverrideRightArmDraw, NULL, this);
    Matrix_Pop();
    osSyncPrintf("RH\n");
    Matrix_Push();
    segIndex = (this->work[BFD_LEAD_BODY_SEG] + sBodyIndex[2]) % 100;
    Matrix_Translate(this->bodySegsPos[segIndex].x, this->bodySegsPos[segIndex].y, this->bodySegsPos[segIndex].z,
                     MTXMODE_NEW);
    Matrix_RotateY(this->bodySegsRot[segIndex].y, MTXMODE_APPLY);
    Matrix_RotateX(-this->bodySegsRot[segIndex].x, MTXMODE_APPLY);
    Matrix_Translate(13.0f, -5.0f, 13.0f, MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x * 0.1f, this->actor.scale.y * 0.1f, this->actor.scale.z * 0.1f, MTXMODE_APPLY);
    SkelAnime_DrawOpa(play, this->skelAnimeLeftArm.skeleton, this->skelAnimeLeftArm.jointTable,
                      BossFd_OverrideLeftArmDraw, NULL, this);
    Matrix_Pop();
    osSyncPrintf("BD\n");
    gSPSegment(POLY_OPA_DISP++, 0x0D, tempMat);

    Matrix_Push();
    for (i = 0; i < 18; i++, tempMat++) {
        segIndex = (this->work[BFD_LEAD_BODY_SEG] + sBodyIndex[i + 1]) % 100;
        Matrix_Translate(this->bodySegsPos[segIndex].x, this->bodySegsPos[segIndex].y, this->bodySegsPos[segIndex].z,
                         MTXMODE_NEW);
        Matrix_RotateY(this->bodySegsRot[segIndex].y, MTXMODE_APPLY);
        Matrix_RotateX(-this->bodySegsRot[segIndex].x, MTXMODE_APPLY);
        Matrix_Translate(0.0f, 0.0f, 35.0f, MTXMODE_APPLY);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        if (i < this->skinSegments) {
            Matrix_Scale(1.0f + (Math_SinS((this->work[BFD_LEAD_BODY_SEG] * 5000.0f) + (i * 7000.0f)) *
                                 this->fwork[BFD_BODY_PULSE]),
                         1.0f + (Math_SinS((this->work[BFD_LEAD_BODY_SEG] * 5000.0f) + (i * 7000.0f)) *
                                 this->fwork[BFD_BODY_PULSE]),
                         1.0f, MTXMODE_APPLY);
            Matrix_RotateY(M_PI / 2.0f, MTXMODE_APPLY);
            MATRIX_TOMTX(tempMat);
            gSPMatrix(POLY_OPA_DISP++, tempMat, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, sBodyDLists[i]);
        } else {
            MtxF spFC;
            Vec3f spF0 = { 0.0f, 0.0f, 0.0f };
            Vec3f spE4;
            Vec3s spDC;
            f32 padD8;

            if (this->bodyFallApart[i] < 2) {
                FrameInterpolation_RecordOpenChild(tempMat, i);

                f32 spD4 = 0.1f;

                temp_float = 0.1f;
                Matrix_Translate(0.0f, 0.0f, -1100.0f, MTXMODE_APPLY);
                Matrix_RotateY(-M_PI, MTXMODE_APPLY);
                if (i >= 14) {
                    f32 sp84 = 1.0f - ((i - 14) * 0.2f);

                    Matrix_Scale(sp84, sp84, 1.0f, MTXMODE_APPLY);
                    spD4 = 0.1f * sp84;
                    temp_float = 0.1f * sp84;
                }
                Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
                gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_OPA_DISP++, gVolvagiaRibsDL);

                if (this->bodyFallApart[i] == 1) {
                    EnVbBall* bones;

                    this->bodyFallApart[i] = 2;
                    Matrix_MultVec3f(&spF0, &spE4);
                    Matrix_Get(&spFC);
                    Matrix_MtxFToYXZRotS(&spFC, &spDC, 0);
                    bones =
                        (EnVbBall*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_VB_BALL,
                                                      spE4.x, spE4.y, spE4.z, spDC.x, spDC.y, spDC.z, i + 200);

                    bones->actor.scale.x = this->actor.scale.x * temp_float;
                    bones->actor.scale.y = this->actor.scale.y * spD4;
                    bones->actor.scale.z = this->actor.scale.z * 0.1f;
                }

                FrameInterpolation_RecordCloseChild();
            }
        }
        if (i > 0) {
            Collider_UpdateSpheres(i + 1, &this->collider);
        }
    }
    Matrix_Pop();
    osSyncPrintf("BH\n");

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, (s8)this->fwork[BFD_HEAD_TEX2_ALPHA]);
    Matrix_Push();
    temp_float =
        (this->work[BFD_ACTION_STATE] >= BOSSFD_SKULL_FALL) ? -20.0f : -10.0f - ((this->actor.speedXZ - 5.0f) * 10.0f);
    segIndex = (this->work[BFD_LEAD_BODY_SEG] + sBodyIndex[0]) % 100;
    Matrix_Translate(this->bodySegsPos[segIndex].x, this->bodySegsPos[segIndex].y, this->bodySegsPos[segIndex].z,
                     MTXMODE_NEW);
    Matrix_RotateY(this->bodySegsRot[segIndex].y, MTXMODE_APPLY);
    Matrix_RotateX(-this->bodySegsRot[segIndex].x, MTXMODE_APPLY);
    Matrix_RotateZ((this->actor.shape.rot.z / (f32)0x8000) * M_PI, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, temp_float, MTXMODE_APPLY);
    Matrix_Push();
    Matrix_Translate(0.0f, 0.0f, 25.0f, MTXMODE_APPLY);
    osSyncPrintf("BHC\n");
    Collider_UpdateSpheres(0, &this->collider);
    Matrix_Pop();
    osSyncPrintf("BHCE\n");
    Matrix_Scale(this->actor.scale.x * 0.1f, this->actor.scale.y * 0.1f, this->actor.scale.z * 0.1f, MTXMODE_APPLY);
    SkelAnime_DrawOpa(play, this->skelAnimeHead.skeleton, this->skelAnimeHead.jointTable, BossFd_OverrideHeadDraw,
                      BossFd_PostHeadDraw, &this->actor);
    osSyncPrintf("SK\n");
    {
        Vec3f spB0 = { 0.0f, 1700.0f, 7000.0f };
        Vec3f spA4 = { -1000.0f, 700.0f, 7000.0f };

        func_80093D84(play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gVolvagiaManeMaterialDL);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, this->fwork[BFD_MANE_COLOR_CENTER], 0, 255);
        Matrix_Push();
        Matrix_MultVec3f(&spB0, &this->centerMane.head);
        BossFd_DrawMane(play, this, this->centerMane.pos, this->fireManeRot, this->centerMane.scale, MANE_CENTER);
        Matrix_Pop();
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, this->fwork[BFD_MANE_COLOR_RIGHT], 0, 255);
        Matrix_Push();
        Matrix_MultVec3f(&spA4, &this->rightMane.head);
        BossFd_DrawMane(play, this, this->rightMane.pos, this->fireManeRot, this->rightMane.scale, MANE_RIGHT);
        Matrix_Pop();
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, this->fwork[BFD_MANE_COLOR_LEFT], 0, 255);
        Matrix_Push();
        spA4.x *= -1.0f;
        Matrix_MultVec3f(&spA4, &this->leftMane.head);
        BossFd_DrawMane(play, this, this->leftMane.pos, this->fireManeRot, this->leftMane.scale, MANE_LEFT);
        Matrix_Pop();
    }

    Matrix_Pop();
    osSyncPrintf("END\n");
    CLOSE_DISPS(play->state.gfxCtx);
}
