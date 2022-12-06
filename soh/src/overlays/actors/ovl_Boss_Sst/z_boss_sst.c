/*
 * File: z_boss_sst.c
 * Overlay: ovl_Boss_Sst
 * Description: Bongo Bongo
 */

#include "z_boss_sst.h"
#include "objects/object_sst/object_sst.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Bg_Sst_Floor/z_bg_sst_floor.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "soh/frame_interpolation.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_10)

#define vParity actionVar
#define vVanish actionVar

#define LEFT 0
#define RIGHT 1
#define OTHER_HAND(hand) ((BossSst*)hand->actor.child)
#define HAND_STATE(hand) sHandState[hand->actor.params]

#define ROOM_CENTER_X -50.0f
#define ROOM_CENTER_Y 0.0f
#define ROOM_CENTER_Z 0.0f

typedef enum {
    /*  0 */ HAND_WAIT,
    /*  1 */ HAND_BEAT,
    /*  2 */ HAND_RETREAT,
    /*  3 */ HAND_SLAM,
    /*  4 */ HAND_SWEEP,
    /*  5 */ HAND_PUNCH,
    /*  6 */ HAND_CLAP,
    /*  7 */ HAND_GRAB,
    /*  8 */ HAND_DAMAGED,
    /*  9 */ HAND_FROZEN,
    /* 10 */ HAND_BREAK_ICE,
    /* 11 */ HAND_DEATH
} BossSstHandState;

typedef enum {
    /* 0 */ BONGO_NULL,
    /* 1 */ BONGO_ICE,
    /* 2 */ BONGO_SHOCKWAVE,
    /* 3 */ BONGO_SHADOW
} BossSstEffectMode;

void BossSst_Init(Actor* thisx, PlayState* play);
void BossSst_Destroy(Actor* thisx, PlayState* play);
void BossSst_UpdateHand(Actor* thisx, PlayState* play);
void BossSst_UpdateHead(Actor* thisx, PlayState* play);
void BossSst_DrawHand(Actor* thisx, PlayState* play);
void BossSst_DrawHead(Actor* thisx, PlayState* play);
void BossSst_UpdateEffect(Actor* thisx, PlayState* play);
void BossSst_DrawEffect(Actor* thisx, PlayState* play);
void BossSst_Reset(void);


void BossSst_HeadSfx(BossSst* this, u16 sfxId);

void BossSst_HeadSetupLurk(BossSst* this);
void BossSst_HeadLurk(BossSst* this, PlayState* play);
void BossSst_HeadSetupIntro(BossSst* this, PlayState* play);
void BossSst_HeadIntro(BossSst* this, PlayState* play);
void BossSst_HeadSetupNeutral(BossSst* this);
void BossSst_HeadNeutral(BossSst* this, PlayState* play);
void BossSst_HeadWait(BossSst* this, PlayState* play);

void BossSst_HeadSetupDamagedHand(BossSst* this, s32 bothHands);
void BossSst_HeadDamagedHand(BossSst* this, PlayState* play);
void BossSst_HeadSetupReadyCharge(BossSst* this);
void BossSst_HeadReadyCharge(BossSst* this, PlayState* play);
void BossSst_HeadSetupCharge(BossSst* this);
void BossSst_HeadCharge(BossSst* this, PlayState* play);
void BossSst_HeadSetupEndCharge(BossSst* this);
void BossSst_HeadEndCharge(BossSst* this, PlayState* play);

void BossSst_HeadSetupFrozenHand(BossSst* this);
void BossSst_HeadFrozenHand(BossSst* this, PlayState* play);
void BossSst_HeadSetupUnfreezeHand(BossSst* this);
void BossSst_HeadUnfreezeHand(BossSst* this, PlayState* play);

void BossSst_HeadStunned(BossSst* this, PlayState* play);
void BossSst_HeadSetupVulnerable(BossSst* this);
void BossSst_HeadVulnerable(BossSst* this, PlayState* play);
void BossSst_HeadDamage(BossSst* this, PlayState* play);
void BossSst_HeadSetupRecover(BossSst* this);
void BossSst_HeadRecover(BossSst* this, PlayState* play);

void BossSst_HeadDeath(BossSst* this, PlayState* play);
void BossSst_HeadSetupThrash(BossSst* this);
void BossSst_HeadThrash(BossSst* this, PlayState* play);
void BossSst_HeadSetupDarken(BossSst* this);
void BossSst_HeadDarken(BossSst* this, PlayState* play);
void BossSst_HeadSetupFall(BossSst* this);
void BossSst_HeadFall(BossSst* this, PlayState* play);
void BossSst_HeadSetupMelt(BossSst* this);
void BossSst_HeadMelt(BossSst* this, PlayState* play);
void BossSst_HeadSetupFinish(BossSst* this);
void BossSst_HeadFinish(BossSst* this, PlayState* play);

void BossSst_HandGrabPlayer(BossSst* this, PlayState* play);
void BossSst_HandReleasePlayer(BossSst* this, PlayState* play, s32 dropPlayer);
void BossSst_HandSelectAttack(BossSst* this);
void BossSst_HandSetDamage(BossSst* this, s32 damage);
void BossSst_HandSetInvulnerable(BossSst* this, s32 isInv);

void BossSst_HandSetupWait(BossSst* this);
void BossSst_HandWait(BossSst* this, PlayState* play);
void BossSst_HandSetupDownbeat(BossSst* this);
void BossSst_HandDownbeat(BossSst* this, PlayState* play);
void BossSst_HandSetupOffbeat(BossSst* this);
void BossSst_HandOffbeat(BossSst* this, PlayState* play);
void BossSst_HandSetupDownbeatEnd(BossSst* this);
void BossSst_HandDownbeatEnd(BossSst* this, PlayState* play);
void BossSst_HandSetupOffbeatEnd(BossSst* this);
void BossSst_HandOffbeatEnd(BossSst* this, PlayState* play);

void BossSst_HandReadySlam(BossSst* this, PlayState* play);
void BossSst_HandSetupSlam(BossSst* this);
void BossSst_HandSlam(BossSst* this, PlayState* play);
void BossSst_HandEndSlam(BossSst* this, PlayState* play);

void BossSst_HandReadySweep(BossSst* this, PlayState* play);
void BossSst_HandSetupSweep(BossSst* this);
void BossSst_HandSweep(BossSst* this, PlayState* play);

void BossSst_HandReadyPunch(BossSst* this, PlayState* play);
void BossSst_HandSetupPunch(BossSst* this);
void BossSst_HandPunch(BossSst* this, PlayState* play);

void BossSst_HandReadyClap(BossSst* this, PlayState* play);
void BossSst_HandSetupClap(BossSst* this);
void BossSst_HandClap(BossSst* this, PlayState* play);
void BossSst_HandSetupEndClap(BossSst* this);
void BossSst_HandEndClap(BossSst* this, PlayState* play);

void BossSst_HandReadyGrab(BossSst* this, PlayState* play);
void BossSst_HandSetupGrab(BossSst* this);
void BossSst_HandGrab(BossSst* this, PlayState* play);
void BossSst_HandSetupCrush(BossSst* this);
void BossSst_HandCrush(BossSst* this, PlayState* play);
void BossSst_HandSetupEndCrush(BossSst* this);
void BossSst_HandEndCrush(BossSst* this, PlayState* play);
void BossSst_HandSetupSwing(BossSst* this);
void BossSst_HandSwing(BossSst* this, PlayState* play);

void BossSst_HandSetupRetreat(BossSst* this);
void BossSst_HandRetreat(BossSst* this, PlayState* play);

void BossSst_HandSetupReel(BossSst* this);
void BossSst_HandReel(BossSst* this, PlayState* play);
void BossSst_HandSetupReadyShake(BossSst* this);
void BossSst_HandReadyShake(BossSst* this, PlayState* play);
void BossSst_HandSetupShake(BossSst* this);
void BossSst_HandShake(BossSst* this, PlayState* play);
void BossSst_HandSetupReadyCharge(BossSst* this);
void BossSst_HandReadyCharge(BossSst* this, PlayState* play);

void BossSst_HandSetupFrozen(BossSst* this);
void BossSst_HandFrozen(BossSst* this, PlayState* play);
void BossSst_HandSetupReadyBreakIce(BossSst* this);
void BossSst_HandReadyBreakIce(BossSst* this, PlayState* play);
void BossSst_HandSetupBreakIce(BossSst* this);
void BossSst_HandBreakIce(BossSst* this, PlayState* play);

void BossSst_HandStunned(BossSst* this, PlayState* play);
void BossSst_HandDamage(BossSst* this, PlayState* play);
void BossSst_HandSetupRecover(BossSst* this);
void BossSst_HandRecover(BossSst* this, PlayState* play);

void BossSst_HandSetupThrash(BossSst* this);
void BossSst_HandThrash(BossSst* this, PlayState* play);
void BossSst_HandSetupDarken(BossSst* this);
void BossSst_HandDarken(BossSst* this, PlayState* play);
void BossSst_HandSetupFall(BossSst* this);
void BossSst_HandFall(BossSst* this, PlayState* play);
void BossSst_HandSetupMelt(BossSst* this);
void BossSst_HandMelt(BossSst* this, PlayState* play);
void BossSst_HandSetupFinish(BossSst* this);
void BossSst_HandFinish(BossSst* this, PlayState* play);

void BossSst_SpawnHeadShadow(BossSst* this);
void BossSst_SpawnHandShadow(BossSst* this);
void BossSst_SpawnShockwave(BossSst* this);
void BossSst_SpawnIceCrystal(BossSst* this, s32 index);
void BossSst_SpawnIceShard(BossSst* this);
void BossSst_IceShatter(BossSst* this);

#include "overlays/ovl_Boss_Sst/ovl_Boss_Sst.h"

static BossSst* sHead;
static BossSst* sHands[2];
static BgSstFloor* sFloor;

static Vec3f sRoomCenter = { ROOM_CENTER_X, ROOM_CENTER_Y, ROOM_CENTER_Z };
static Vec3f sHandOffsets[2];
static s16 sHandYawOffsets[2];

static s16 sCutsceneCamera;
static Vec3f sCameraAt = { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 0.0f, ROOM_CENTER_Z + 0.0f };
static Vec3f sCameraEye = { ROOM_CENTER_X + 150.0f, ROOM_CENTER_Y + 100.0f, ROOM_CENTER_Z + 0.0f };
static Vec3f sCameraAtVel = { 0.0f, 0.0f, 0.0f };
static Vec3f sCameraEyeVel = { 0.0f, 0.0f, 0.0f };

static Vec3f sCameraAtPoints[] = {
    { ROOM_CENTER_X - 50.0f, ROOM_CENTER_Y + 300.0f, ROOM_CENTER_Z + 0.0f },
    { ROOM_CENTER_X + 150.0f, ROOM_CENTER_Y + 300.0f, ROOM_CENTER_Z + 100.0f },
    { ROOM_CENTER_X + 0.0f, ROOM_CENTER_Y + 600.0f, ROOM_CENTER_Z + 100.0f },
    { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 400.0f, ROOM_CENTER_Z + 200.0f },
    { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 200.0f },
    { ROOM_CENTER_X - 50.0f, ROOM_CENTER_Y + 0.0f, ROOM_CENTER_Z + 200.0f },
    { ROOM_CENTER_X - 150.0f, ROOM_CENTER_Y + 0.0f, ROOM_CENTER_Z + 100.0f },
    { ROOM_CENTER_X - 60.0f, ROOM_CENTER_Y + 180.0f, ROOM_CENTER_Z + 730.0f },
};

static Vec3f sCameraEyePoints[] = {
    { ROOM_CENTER_X + 250.0f, ROOM_CENTER_Y + 800.0f, ROOM_CENTER_Z + 800.0f },
    { ROOM_CENTER_X - 150.0f, ROOM_CENTER_Y + 700.0f, ROOM_CENTER_Z + 1400.0f },
    { ROOM_CENTER_X + 250.0f, ROOM_CENTER_Y + 100.0f, ROOM_CENTER_Z + 750.0f },
    { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 900.0f },
    { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 900.0f },
    { ROOM_CENTER_X + 350.0f, ROOM_CENTER_Y + 400.0f, ROOM_CENTER_Z + 1200.0f },
    { ROOM_CENTER_X - 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 800.0f },
    { ROOM_CENTER_X - 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 800.0f },
};

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };
static u32 sBodyStatic = false;

// Unreferenced. Maybe two zero vectors?
static u32 sUnkValues[] = { 0, 0, 0, 0, 0, 0 };

static Color_RGBA8 sBodyColor = { 255, 255, 255, 255 };
static Color_RGBA8 sStaticColor = { 0, 0, 0, 255 };
static s32 sHandState[] = { HAND_WAIT, HAND_WAIT };

const ActorInit Boss_Sst_InitVars = {
    ACTOR_BOSS_SST,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_SST,
    sizeof(BossSst),
    (ActorFunc)BossSst_Init,
    (ActorFunc)BossSst_Destroy,
    (ActorFunc)BossSst_UpdateHand,
    (ActorFunc)BossSst_DrawHand,
    NULL,
};

#include "z_boss_sst_colchk.c"

static AnimationHeader* sHandIdleAnims[] = { &gBongoLeftHandIdleAnim, &gBongoRightHandIdleAnim };
static AnimationHeader* sHandFlatPoses[] = { &gBongoLeftHandFlatPoseAnim, &gBongoRightHandFlatPoseAnim };
static AnimationHeader* sHandOpenPoses[] = { &gBongoLeftHandOpenPoseAnim, &gBongoRightHandOpenPoseAnim };
static AnimationHeader* sHandFistPoses[] = { &gBongoLeftHandFistPoseAnim, &gBongoRightHandFistPoseAnim };
static AnimationHeader* sHandClenchAnims[] = { &gBongoLeftHandClenchAnim, &gBongoRightHandClenchAnim };
static AnimationHeader* sHandDamagePoses[] = { &gBongoLeftHandDamagePoseAnim, &gBongoRightHandDamagePoseAnim };
static AnimationHeader* sHandPushoffPoses[] = { &gBongoLeftHandPushoffPoseAnim, &gBongoRightHandPushoffPoseAnim };
static AnimationHeader* sHandHangPoses[] = { &gBongoLeftHandHangPoseAnim, &gBongoRightHandHangPoseAnim };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x29, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 20, ICHAIN_STOP),
};

void BossSst_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BossSst* this = (BossSst*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitCylinder(play, &this->colliderCyl);
    Collider_InitJntSph(play, &this->colliderJntSph);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Flags_SetSwitch(play, 0x14);
    if (this->actor.params == BONGO_HEAD) {
        sFloor = (BgSstFloor*)Actor_Spawn(&play->actorCtx, play, ACTOR_BG_SST_FLOOR, sRoomCenter.x,
                                          sRoomCenter.y, sRoomCenter.z, 0, 0, 0, BONGOFLOOR_REST, true);
        SkelAnime_InitFlex(play, &this->skelAnime, &gBongoHeadSkel, &gBongoHeadEyeOpenIdleAnim, this->jointTable,
                           this->morphTable, 45);
        ActorShape_Init(&this->actor.shape, 70000.0f, ActorShadow_DrawCircle, 95.0f);
        Collider_SetJntSph(play, &this->colliderJntSph, &this->actor, &sJntSphInitHead, this->colliderItems);
        Collider_SetCylinder(play, &this->colliderCyl, &this->actor, &sCylinderInitHead);
        sHead = this;
        this->actor.world.pos.x = ROOM_CENTER_X + 50.0f;
        this->actor.world.pos.y = ROOM_CENTER_Y + 0.0f;
        this->actor.world.pos.z = ROOM_CENTER_Z - 650.0f;
        this->actor.home.pos = this->actor.world.pos;
        this->actor.shape.rot.y = 0;
        if (Flags_GetClear(play, play->roomCtx.curRoom.num)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, ROOM_CENTER_X, ROOM_CENTER_Y,
                        ROOM_CENTER_Z + 400.0f, 0, 0, 0, WARP_DUNGEON_ADULT, true);
            Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, ROOM_CENTER_X, ROOM_CENTER_Y,
                        ROOM_CENTER_Z - 200.0f, 0, 0, 0, 0, true);
            Actor_Kill(&this->actor);
        } else {
            sHands[LEFT] =
                (BossSst*)Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_SST, this->actor.world.pos.x + 200.0f,
                                      this->actor.world.pos.y, this->actor.world.pos.z + 400.0f, 0,
                                      this->actor.shape.rot.y, 0, BONGO_LEFT_HAND, true);
            sHands[RIGHT] = (BossSst*)Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_SST,
                                                  this->actor.world.pos.x + (-200.0f), this->actor.world.pos.y,
                                                  this->actor.world.pos.z + 400.0f, 0, this->actor.shape.rot.y, 0,
                                                  BONGO_RIGHT_HAND, true);
            sHands[LEFT]->actor.child = &sHands[RIGHT]->actor;
            sHands[RIGHT]->actor.child = &sHands[LEFT]->actor;

            this->actor.flags &= ~ACTOR_FLAG_0;
            this->actor.update = BossSst_UpdateHead;
            this->actor.draw = BossSst_DrawHead;
            this->radius = -650.0f;
            this->actor.targetArrowOffset = 4000.0f;
            BossSst_HeadSetupLurk(this);
            Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_BOSS);
        }
    } else {
        Collider_SetJntSph(play, &this->colliderJntSph, &this->actor, &sJntSphInitHand, this->colliderItems);
        Collider_SetCylinder(play, &this->colliderCyl, &this->actor, &sCylinderInitHand);
        if (this->actor.params == BONGO_LEFT_HAND) {
            SkelAnime_InitFlex(play, &this->skelAnime, &gBongoLeftHandSkel, &gBongoLeftHandIdleAnim,
                               this->jointTable, this->morphTable, 27);
            this->vParity = -1;
            this->colliderJntSph.elements[0].dim.modelSphere.center.z *= -1;
        } else {
            SkelAnime_InitFlex(play, &this->skelAnime, &gBongoRightHandSkel, &gBongoRightHandIdleAnim,
                               this->jointTable, this->morphTable, 27);
            this->vParity = 1;
        }

        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 95.0f);
        this->handZPosMod = -3500;
        this->actor.targetArrowOffset = 5000.0f;
        this->actor.flags &= ~ACTOR_FLAG_0;
        BossSst_HandSetupWait(this);
    }
}

void BossSst_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    BossSst* this = (BossSst*)thisx;

    Collider_DestroyJntSph(play, &this->colliderJntSph);
    Collider_DestroyCylinder(play, &this->colliderCyl);
    Audio_StopSfxByPos(&this->center);
}

void BossSst_HeadSetupLurk(BossSst* this) {
    this->actor.draw = NULL;
    sHands[LEFT]->actor.draw = NULL;
    sHands[RIGHT]->actor.draw = NULL;
    this->vVanish = false;
    this->actionFunc = BossSst_HeadLurk;
}

void BossSst_HeadLurk(BossSst* this, PlayState* play) {
    if (this->actor.yDistToPlayer < 1000.0f) {
        BossSst_HeadSetupIntro(this, play);
    }
}

void BossSst_HeadSetupIntro(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->timer = 611;
    this->ready = false;
    player->actor.world.pos.x = sRoomCenter.x;
    player->actor.world.pos.y = ROOM_CENTER_Y + 1000.0f;
    player->actor.world.pos.z = sRoomCenter.z;
    player->linearVelocity = player->actor.velocity.y = 0.0f;
    player->actor.shape.rot.y = -0x8000;
    player->targetYaw = -0x8000;
    player->currentYaw = -0x8000;
    player->fallStartHeight = 0;
    player->stateFlags1 |= 0x20;

    func_80064520(play, &play->csCtx);
    func_8002DF54(play, &this->actor, 8);
    sCutsceneCamera = Play_CreateSubCamera(play);
    Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
    Play_ChangeCameraStatus(play, sCutsceneCamera, CAM_STAT_ACTIVE);
    Math_Vec3f_Copy(&sCameraAt, &player->actor.world.pos);
    if (gSaveContext.eventChkInf[7] & 0x80) {
        sCameraEye.z = ROOM_CENTER_Z - 100.0f;
    }

    Play_CameraSetAtEye(play, sCutsceneCamera, &sCameraAt, &sCameraEye);
    Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
    this->actionFunc = BossSst_HeadIntro;
}

void BossSst_HeadIntro(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 tempo;
    s32 introStateTimer;
    s32 revealStateTimer;

    if (this->timer != 0) {
        this->timer--;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_MorphToLoop(&this->skelAnime, &gBongoHeadEyeCloseIdleAnim, -3.0f);
    }

    if (this->timer == 0) {
        sHands[RIGHT]->actor.flags |= ACTOR_FLAG_0;
        sHands[LEFT]->actor.flags |= ACTOR_FLAG_0;
        player->stateFlags1 &= ~0x20;
        func_80064534(play, &play->csCtx);
        func_8002DF54(play, &this->actor, 7);
        sCameraAt.y += 30.0f;
        sCameraAt.z += 300.0f;
        Play_CameraSetAtEye(play, sCutsceneCamera, &sCameraAt, &sCameraEye);
        Play_CopyCamera(play, MAIN_CAM, sCutsceneCamera);
        Play_ChangeCameraStatus(play, sCutsceneCamera, CAM_STAT_WAIT);
        Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_ACTIVE);
        Play_ClearCamera(play, sCutsceneCamera);
        gSaveContext.eventChkInf[7] |= 0x80;
        BossSst_HeadSetupNeutral(this);
        this->colliderJntSph.base.ocFlags1 |= OC1_ON;
        sHands[LEFT]->colliderJntSph.base.ocFlags1 |= OC1_ON;
        sHands[RIGHT]->colliderJntSph.base.ocFlags1 |= OC1_ON;
        this->timer = 112;
    } else if (this->timer >= 546) {
        if (player->actor.world.pos.y > 100.0f) {
            player->actor.world.pos.x = sRoomCenter.x;
            player->actor.world.pos.z = sRoomCenter.z;
            player->linearVelocity = 0;
            player->actor.shape.rot.y = -0x8000;
            player->targetYaw = -0x8000;
            player->currentYaw = -0x8000;
        }

        Math_Vec3f_Copy(&sCameraAt, &player->actor.world.pos);
        if (player->actor.bgCheckFlags & 2) {
            if (!this->ready) {
                sFloor->dyna.actor.params = BONGOFLOOR_HIT;
                this->ready = true;
                func_800AA000(this->actor.xyzDistToPlayerSq, 0xFF, 0x14, 0x96);
                Audio_PlayActorSound2(&sFloor->dyna.actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
            } else if (gSaveContext.eventChkInf[7] & 0x80) {
                sHands[RIGHT]->actor.draw = BossSst_DrawHand;
                sHands[LEFT]->actor.draw = BossSst_DrawHand;
                this->actor.draw = BossSst_DrawHead;
                this->timer = 178;
                sCameraAt.x = ROOM_CENTER_X - 23.0f;
                sCameraAt.y = ROOM_CENTER_Y + 0.0f;
                sCameraAt.z = ROOM_CENTER_Z + 0.0f;
            } else {
                this->timer = 546;
            }
        }
    } else if (this->timer >= 478) {
        sCameraEye.x += 10.0f;
        sCameraEye.y += 10.0f;
        sCameraEye.z -= 10.0f;
    } else if (this->timer >= 448) {
        if (this->timer == 460) {
            sHands[RIGHT]->actor.draw = BossSst_DrawHand;
            sHands[LEFT]->actor.draw = BossSst_DrawHand;
            this->actor.draw = BossSst_DrawHead;
            player->actor.world.pos.x = sRoomCenter.x;
            player->actor.world.pos.z = sRoomCenter.z;
            BossSst_HandSetupDownbeat(sHands[RIGHT]);
        }
        if (this->timer > 460) {
            sCameraEye.x -= 40.0f;
            sCameraEye.y -= 40.0f;
            sCameraEye.z += 20.0f;
        } else if (this->timer == 460) {
            sCameraAt.x = sHands[RIGHT]->actor.home.pos.x + 0.0f;
            sCameraAt.y = sHands[RIGHT]->actor.home.pos.y - 20.0f;
            sCameraAt.z = sHands[RIGHT]->actor.home.pos.z + 10.0f;
            sCameraEye.x = sHands[RIGHT]->actor.home.pos.x + 150.0f;
            sCameraEye.y = sHands[RIGHT]->actor.home.pos.y + 100.0f;
            sCameraEye.z = sHands[RIGHT]->actor.home.pos.z + 80.0f;
        }
    } else {
        if (this->timer >= 372) {
            introStateTimer = this->timer - 372;
            tempo = 6;
            if (this->timer == 447) {
                sCameraAt = player->actor.world.pos;
                sCameraEye.x = ROOM_CENTER_X - 200.0f;
                sCameraEye.y = ROOM_CENTER_Y + 160.0f;
                sCameraEye.z = ROOM_CENTER_Z - 190.0f;
            } else if (introStateTimer == 11) {
                sCameraAt.x = sHands[RIGHT]->actor.home.pos.x + 30.0f;
                sCameraAt.y = sHands[RIGHT]->actor.home.pos.y + 0.0f;
                sCameraAt.z = sHands[RIGHT]->actor.home.pos.z + 20.0f;
                sCameraEye.x = sHands[RIGHT]->actor.home.pos.x + 100.0f;
                sCameraEye.y = sHands[RIGHT]->actor.home.pos.y + 10.0f;
                sCameraEye.z = sHands[RIGHT]->actor.home.pos.z - 210.0f;
            } else if (introStateTimer == 62) {
                sCameraAt.x = sHands[LEFT]->actor.home.pos.x + 0.0f;
                sCameraAt.y = sHands[LEFT]->actor.home.pos.y + 50.0f;
                sCameraAt.z = sHands[LEFT]->actor.home.pos.z + 100.0f;
                sCameraEye.x = sHands[LEFT]->actor.home.pos.x + 110.0f;
                sCameraEye.y = sHands[LEFT]->actor.home.pos.y + 180.0f;
                sCameraEye.z = sHands[LEFT]->actor.home.pos.z - 70.0f;
            }
        } else if (this->timer >= 304) {
            introStateTimer = this->timer - 304;
            tempo = 5;
            if (introStateTimer == 11) {
                sCameraAt.x = sHands[RIGHT]->actor.home.pos.x + 40.0f;
                sCameraAt.y = sHands[RIGHT]->actor.home.pos.y - 90.0f;
                sCameraAt.z = sHands[RIGHT]->actor.home.pos.z - 40.0f;
                sCameraEye.x = sHands[RIGHT]->actor.home.pos.x - 20.0f;
                sCameraEye.y = sHands[RIGHT]->actor.home.pos.y + 210.0f;
                sCameraEye.z = sHands[RIGHT]->actor.home.pos.z + 170.0f;
            } else if (this->timer == 368) {
                sCameraAt.x = sHands[LEFT]->actor.home.pos.x - 20.0f;
                sCameraAt.y = sHands[LEFT]->actor.home.pos.y + 0.0f;
                sCameraAt.z = sHands[LEFT]->actor.home.pos.z + 0.0f;
                sCameraEye.x = sHands[LEFT]->actor.home.pos.x - 70.0f;
                sCameraEye.y = sHands[LEFT]->actor.home.pos.y + 170.0f;
                sCameraEye.z = sHands[LEFT]->actor.home.pos.z + 150.0f;
            }
        } else if (this->timer >= 244) {
            introStateTimer = this->timer - 244;
            tempo = 4;
            if (introStateTimer == 11) {
                sCameraAt.x = sHands[RIGHT]->actor.home.pos.x + 30.0f;
                sCameraAt.y = sHands[RIGHT]->actor.home.pos.y + 70.0f;
                sCameraAt.z = sHands[RIGHT]->actor.home.pos.z + 40.0f;
                sCameraEye.x = sHands[RIGHT]->actor.home.pos.x + 110.0f;
                sCameraEye.y = sHands[RIGHT]->actor.home.pos.y - 140.0f;
                sCameraEye.z = sHands[RIGHT]->actor.home.pos.z - 10.0f;
            } else if (this->timer == 300) {
                sCameraAt.x = sHands[LEFT]->actor.home.pos.x - 20.0f;
                sCameraAt.y = sHands[LEFT]->actor.home.pos.y - 80.0f;
                sCameraAt.z = sHands[LEFT]->actor.home.pos.z + 320.0f;
                sCameraEye.x = sHands[LEFT]->actor.home.pos.x - 130.0f;
                sCameraEye.y = sHands[LEFT]->actor.home.pos.y + 130.0f;
                sCameraEye.z = sHands[LEFT]->actor.home.pos.z - 150.0f;
            }
        } else if (this->timer >= 192) {
            introStateTimer = this->timer - 192;
            tempo = 3;
            if (this->timer == 240) {
                sCameraAt.x = sHands[LEFT]->actor.home.pos.x - 190.0f;
                sCameraAt.y = sHands[LEFT]->actor.home.pos.y - 110.0f;
                sCameraAt.z = sHands[LEFT]->actor.home.pos.z + 40.0f;
                sCameraEye.x = sHands[LEFT]->actor.home.pos.x + 120.0f;
                sCameraEye.y = sHands[LEFT]->actor.home.pos.y + 130.0f;
                sCameraEye.z = sHands[LEFT]->actor.home.pos.z + 50.0f;
            } else if (introStateTimer == 12) {
                sCameraAt.x = sRoomCenter.x + 50.0f;
                sCameraAt.y = sRoomCenter.y - 90.0f;
                sCameraAt.z = sRoomCenter.z - 200.0f;
                sCameraEye.x = sRoomCenter.x + 50.0f;
                sCameraEye.y = sRoomCenter.y + 350.0f;
                sCameraEye.z = sRoomCenter.z + 150.0f;
            }
        } else if (this->timer >= 148) {
            introStateTimer = this->timer - 148;
            tempo = 2;
        } else if (this->timer >= 112) {
            introStateTimer = this->timer - 112;
            tempo = 1;
        } else {
            introStateTimer = this->timer % 28;
            tempo = 0;
        }
        if (this->timer <= 198) {
            revealStateTimer = 198 - this->timer;
            if ((gSaveContext.eventChkInf[7] & 0x80) && (revealStateTimer <= 44)) {
                sCameraAt.x += 492.0f * 0.01f;
                sCameraAt.y += 200.0f * 0.01f;
                sCameraEye.x -= 80.0f * 0.01f;
                sCameraEye.y -= 360.0f * 0.01f;
                sCameraEye.z += 1000.0f * 0.01f;
            } else if (this->timer <= 20) {
                sCameraAt.y -= 700.0f * 0.01f;
                sCameraAt.z += 900.0f * 0.01f;
                sCameraEye.x += 650.0f * 0.01f;
                sCameraEye.y += 400.0f * 0.01f;
                sCameraEye.z += 1550.0f * 0.01f;
                this->vVanish = true;
                this->actor.flags |= ACTOR_FLAG_7;
            } else if (revealStateTimer < 40) {
                sCameraAt.x += 125.0f * 0.01f;
                sCameraAt.y += 350.0f * 0.01f;
                sCameraAt.z += 500.0f * 0.01f;
                sCameraEye.x += 200.0f * 0.01f;
                sCameraEye.y -= 850.0f * 0.01f;
            } else if (revealStateTimer >= 45) {
                if (revealStateTimer < 85) {
                    sCameraAt.x -= 250.0f * 0.01f;
                    sCameraAt.y += 425.0f * 0.01f;
                    sCameraAt.z -= 1200.0f * 0.01f;
                    sCameraEye.x -= 650.0f * 0.01f;
                    sCameraEye.y += 125.0f * 0.01f;
                    sCameraEye.z -= 350.0f * 0.01f;
                } else if (revealStateTimer == 85) {
                    if (!(gSaveContext.eventChkInf[7] & 0x80)) {
                        TitleCard_InitBossName(play, &play->actorCtx.titleCtx,
                                               SEGMENTED_TO_VIRTUAL(gBongoTitleCardTex), 160, 180, 128, 40, true);
                    }
                    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS);
                    Animation_MorphToPlayOnce(&this->skelAnime, &gBongoHeadEyeCloseAnim, -5.0f);
                    BossSst_HeadSfx(this, NA_SE_EN_SHADEST_DISAPPEAR);
                }
            }
        }
        if (introStateTimer == 12) {
            BossSst_HandSetupDownbeat(sHands[RIGHT]);
        }
        if ((introStateTimer != 5) && ((introStateTimer % ((tempo * 2) + 7)) == 5)) {
            BossSst_HandSetupOffbeat(sHands[LEFT]);
        }
    }

    if (this->actionFunc != BossSst_HeadNeutral) {
        Play_CameraSetAtEye(play, sCutsceneCamera, &sCameraAt, &sCameraEye);
    }
}

void BossSst_HeadSetupWait(BossSst* this) {
    if (this->skelAnime.animation != &gBongoHeadEyeCloseIdleAnim) {
        Animation_MorphToLoop(&this->skelAnime, &gBongoHeadEyeCloseIdleAnim, -5.0f);
    }
    this->actionFunc = BossSst_HeadWait;
}

void BossSst_HeadWait(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((HAND_STATE(sHands[LEFT]) == HAND_WAIT) && (HAND_STATE(sHands[RIGHT]) == HAND_WAIT)) {
        BossSst_HeadSetupNeutral(this);
    }
}

void BossSst_HeadSetupNeutral(BossSst* this) {
    this->timer = 127;
    this->ready = false;
    this->actionFunc = BossSst_HeadNeutral;
}

void BossSst_HeadNeutral(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (!this->ready && ((HAND_STATE(sHands[LEFT]) == HAND_BEAT) || (HAND_STATE(sHands[LEFT]) == HAND_WAIT)) &&
        ((HAND_STATE(sHands[RIGHT]) == HAND_BEAT) || (HAND_STATE(sHands[RIGHT]) == HAND_WAIT))) {
        this->ready = true;
    }

    if (this->ready) {
        if (this->timer != 0) {
            this->timer--;
        }
    }

    if (this->timer == 0) {
        if ((GET_PLAYER(play)->actor.world.pos.y > -50.0f) && !(GET_PLAYER(play)->stateFlags1 & 0x6080)) {
            sHands[Rand_ZeroOne() <= 0.5f]->ready = true;
            BossSst_HeadSetupWait(this);
        } else {
            this->timer = 28;
        }
    } else {
        Math_ApproachS(&this->actor.shape.rot.y,
                       Actor_WorldYawTowardPoint(&GET_PLAYER(play)->actor, &sRoomCenter) + 0x8000, 4, 0x400);
        if ((this->timer == 28) || (this->timer == 84)) {
            BossSst_HeadSfx(this, NA_SE_EN_SHADEST_PRAY);
        }
    }
}

void BossSst_HeadSetupDamagedHand(BossSst* this, s32 bothHands) {
    if (bothHands) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gBongoHeadEyeOpenAnim, -5.0f);
    } else {
        Animation_MorphToPlayOnce(&this->skelAnime, &gBongoHeadDamagedHandAnim, -5.0f);
    }
    this->actionFunc = BossSst_HeadDamagedHand;
}

void BossSst_HeadDamagedHand(BossSst* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if ((HAND_STATE(sHands[LEFT]) == HAND_DAMAGED) && (HAND_STATE(sHands[RIGHT]) == HAND_DAMAGED)) {
            BossSst_HeadSetupReadyCharge(this);
        } else if ((HAND_STATE(sHands[LEFT]) == HAND_FROZEN) || (HAND_STATE(sHands[RIGHT]) == HAND_FROZEN)) {
            BossSst_HeadSetupFrozenHand(this);
        } else if (this->skelAnime.animation == &gBongoHeadEyeOpenAnim) {
            BossSst_HeadSetupUnfreezeHand(this);
        } else {
            BossSst_HeadSetupWait(this);
        }
    }
}

void BossSst_HeadSetupReadyCharge(BossSst* this) {
    Animation_MorphToLoop(&this->skelAnime, &gBongoHeadEyeOpenIdleAnim, -5.0f);
    this->actor.speedXZ = 0.0f;
    this->colliderCyl.base.acFlags |= AC_ON;
    this->actionFunc = BossSst_HeadReadyCharge;
}

void BossSst_HeadReadyCharge(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (sHands[LEFT]->ready && (sHands[LEFT]->actionFunc == BossSst_HandReadyCharge) && sHands[RIGHT]->ready &&
        (sHands[RIGHT]->actionFunc == BossSst_HandReadyCharge)) {
        BossSst_HeadSetupCharge(this);
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0x800, 0x400);
    }
}

void BossSst_HeadSetupCharge(BossSst* this) {
    Animation_Change(&this->skelAnime, &gBongoHeadChargeAnim, 0.5f, 0.0f, Animation_GetLastFrame(&gBongoHeadChargeAnim),
                     ANIMMODE_ONCE_INTERP, -5.0f);
    BossSst_HandSetDamage(sHands[LEFT], 0x20);
    BossSst_HandSetDamage(sHands[RIGHT], 0x20);
    this->colliderJntSph.base.atFlags |= AT_ON;
    this->actor.speedXZ = 3.0f;
    this->radius = -650.0f;
    this->ready = false;
    this->actionFunc = BossSst_HeadCharge;
}

void BossSst_HeadCharge(BossSst* this, PlayState* play) {
    f32 chargeDist;
    s32 animFinish = SkelAnime_Update(&this->skelAnime);

    if (!this->ready && Animation_OnFrame(&this->skelAnime, 6.0f)) {
        this->ready = true;
        this->actor.speedXZ = 0.25f;
        this->skelAnime.playSpeed = 0.2f;
    }

    this->actor.speedXZ *= 1.25f;
    this->actor.speedXZ = CLAMP_MAX(this->actor.speedXZ, 45.0f);

    if (this->ready) {
        if (Math_SmoothStepToF(&this->radius, 650.0f, 0.4f, this->actor.speedXZ, 1.0f) < 10.0f) {
            this->radius = 650.0f;
            BossSst_HeadSetupEndCharge(this);
        } else {
            chargeDist = (650.0f - this->radius) * 3.0f;
            if (chargeDist > 180.0f) {
                chargeDist = 180.0f;
            }

            this->actor.world.pos.y = this->actor.home.pos.y - chargeDist;
        }

        if (!animFinish) {
            sHandOffsets[LEFT].z += 5.0f;
            sHandOffsets[RIGHT].z += 5.0f;
        }
    } else {
        Math_ApproachF(&this->radius, -700.0f, 0.4f, this->actor.speedXZ);
        Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y - 180.0f, 20.0f);
        sHandOffsets[LEFT].y += 5.0f;
        sHandOffsets[RIGHT].y += 5.0f;
    }

    if (this->colliderJntSph.base.atFlags & AT_HIT) {
        this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        sHands[LEFT]->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        sHands[RIGHT]->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        func_8002F71C(play, &this->actor, 10.0f, this->actor.shape.rot.y, 5.0f);
        func_8002F7DC(&GET_PLAYER(play)->actor, NA_SE_PL_BODY_HIT);
    }
}

void BossSst_HeadSetupEndCharge(BossSst* this) {
    Animation_MorphToLoop(&this->skelAnime, &gBongoHeadEyeCloseIdleAnim, -20.0f);
    this->targetYaw = Actor_WorldYawTowardPoint(&this->actor, &sRoomCenter);
    this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    this->colliderCyl.base.acFlags &= ~AC_ON;
    this->radius *= -1.0f;
    this->actionFunc = BossSst_HeadEndCharge;
}

void BossSst_HeadEndCharge(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Math_SmoothStepToS(&this->actor.shape.rot.y, this->targetYaw, 4, 0x800, 0x100) == 0) {
        BossSst_HandSetupRetreat(sHands[LEFT]);
        BossSst_HandSetupRetreat(sHands[RIGHT]);
        BossSst_HeadSetupNeutral(this);
    }
}

void BossSst_HeadSetupFrozenHand(BossSst* this) {
    Animation_MorphToLoop(&this->skelAnime, &gBongoHeadEyeOpenIdleAnim, -5.0f);
    this->ready = false;
    this->colliderCyl.base.acFlags |= AC_ON;
    this->actionFunc = BossSst_HeadFrozenHand;
}

void BossSst_HeadFrozenHand(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->ready) {
        BossSst_HeadSetupUnfreezeHand(this);
    }
}

void BossSst_HeadSetupUnfreezeHand(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gBongoHeadEyeCloseAnim, -5.0f);
    this->colliderCyl.base.acFlags &= ~AC_ON;
    this->actionFunc = BossSst_HeadUnfreezeHand;
}

void BossSst_HeadUnfreezeHand(BossSst* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        BossSst_HeadSetupWait(this);
    }
}

void BossSst_HeadSetupStunned(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gBongoHeadKnockoutAnim, -5.0f);
    Actor_SetColorFilter(&this->actor, 0, 0xFF, 0, Animation_GetLastFrame(&gBongoHeadKnockoutAnim));
    this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    this->colliderCyl.base.acFlags &= ~AC_ON;
    this->vVanish = false;
    this->actor.flags &= ~ACTOR_FLAG_7;
    BossSst_HeadSfx(this, NA_SE_EN_SHADEST_FREEZE);
    this->actionFunc = BossSst_HeadStunned;
}

void BossSst_HeadStunned(BossSst* this, PlayState* play) {
    f32 bounce;
    s32 animFinish;
    f32 currentFrame;

    Math_StepToF(&sHandOffsets[LEFT].z, 600.0f, 20.0f);
    Math_StepToF(&sHandOffsets[RIGHT].z, 600.0f, 20.0f);
    Math_StepToF(&sHandOffsets[LEFT].x, 200.0f, 20.0f);
    Math_StepToF(&sHandOffsets[RIGHT].x, -200.0f, 20.0f);
    this->actor.velocity.y += this->actor.gravity;
    animFinish = SkelAnime_Update(&this->skelAnime);
    currentFrame = this->skelAnime.curFrame;
    if (currentFrame <= 6.0f) {
        bounce = (sinf((M_PI / 11) * currentFrame) * 100.0f) + (this->actor.home.pos.y - 180.0f);
        if (this->actor.world.pos.y < bounce) {
            this->actor.world.pos.y = bounce;
        }
    } else if (currentFrame <= 11.0f) {
        this->actor.world.pos.y = (sinf((M_PI / 11) * currentFrame) * 170.0f) + (this->actor.home.pos.y - 250.0f);
    } else {
        this->actor.world.pos.y =
            (sinf((currentFrame - 11.0f) * (M_PI / 5)) * 50.0f) + (this->actor.home.pos.y - 250.0f);
    }

    if ((animFinish) || Animation_OnFrame(&this->skelAnime, 11.0f)) {
        BossSst_HeadSfx(this, NA_SE_EN_SHADEST_LAND);
    }

    if (this->radius < -500.0f) {
        Math_SmoothStepToF(&this->radius, -500.0f, 1.0f, 50.0f, 5.0f);
    } else {
        Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 0.5f, 15.0f, 3.0f);
        this->radius += this->actor.speedXZ;
    }

    this->radius = CLAMP_MAX(this->radius, 400.0f);

    this->actor.world.pos.y += this->actor.velocity.y;
    if (animFinish) {
        BossSst_HeadSetupVulnerable(this);
    }
}

void BossSst_HeadSetupVulnerable(BossSst* this) {
    Animation_MorphToLoop(&this->skelAnime, &gBongoHeadStunnedAnim, -5.0f);
    this->colliderCyl.base.acFlags |= AC_ON;
    this->colliderCyl.info.bumper.dmgFlags = 0x0FC00702; // Sword-type damage
    this->actor.speedXZ = 0.0f;
    this->colliderJntSph.elements[10].info.bumperFlags |= (BUMP_ON | BUMP_HOOKABLE);
    this->colliderJntSph.elements[0].info.bumperFlags &= ~BUMP_ON;
    if (this->actionFunc != BossSst_HeadDamage) {
        this->timer = 50;
    }

    this->actionFunc = BossSst_HeadVulnerable;
}

void BossSst_HeadVulnerable(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_StepToF(&sHandOffsets[LEFT].z, 600.0f, 20.0f);
    Math_StepToF(&sHandOffsets[RIGHT].z, 600.0f, 20.0f);
    Math_StepToF(&sHandOffsets[LEFT].x, 200.0f, 20.0f);
    Math_StepToF(&sHandOffsets[RIGHT].x, -200.0f, 20.0f);
    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_13)) {
        this->timer += 2;
        this->timer = CLAMP_MAX(this->timer, 50);
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            BossSst_HandSetupRecover(sHands[LEFT]);
            BossSst_HandSetupRecover(sHands[RIGHT]);
            BossSst_HeadSetupRecover(this);
        }
    }
}

void BossSst_HeadSetupDamage(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gBongoHeadDamageAnim, -3.0f);
    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, Animation_GetLastFrame(&gBongoHeadDamageAnim));
    Actor_SetColorFilter(&sHands[LEFT]->actor, 0x4000, 0xFF, 0, Animation_GetLastFrame(&gBongoHeadDamageAnim));
    Actor_SetColorFilter(&sHands[RIGHT]->actor, 0x4000, 0xFF, 0, Animation_GetLastFrame(&gBongoHeadDamageAnim));
    this->colliderCyl.base.acFlags &= ~AC_ON;
    BossSst_HeadSfx(this, NA_SE_EN_SHADEST_DAMAGE);
    this->actionFunc = BossSst_HeadDamage;
}

void BossSst_HeadDamage(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        BossSst_HeadSetupVulnerable(this);
    }
}

void BossSst_HeadSetupRecover(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gBongoHeadRecoverAnim, -5.0f);
    this->colliderCyl.base.acFlags &= ~AC_ON;
    this->colliderCyl.info.bumper.dmgFlags = 0xFFCFFFFF;
    this->colliderJntSph.elements[10].info.bumperFlags &= ~(BUMP_ON | BUMP_HOOKABLE);
    this->colliderJntSph.elements[0].info.bumperFlags |= BUMP_ON;
    this->vVanish = true;
    this->actor.speedXZ = 5.0f;
    this->actionFunc = BossSst_HeadRecover;
}

void BossSst_HeadRecover(BossSst* this, PlayState* play) {
    s32 animFinish;
    f32 currentFrame;
    f32 diff;

    animFinish = SkelAnime_Update(&this->skelAnime);
    currentFrame = this->skelAnime.curFrame;
    if (currentFrame < 10.0f) {
        this->actor.world.pos.y += 10.0f;
        sHandOffsets[LEFT].y -= 10.0f;
        sHandOffsets[RIGHT].y -= 10.0f;
        Math_SmoothStepToF(&this->radius, -750.0f, 1.0f, this->actor.speedXZ, 2.0f);
    } else {
        this->actor.speedXZ *= 1.25f;
        this->actor.speedXZ = CLAMP_MAX(this->actor.speedXZ, 50.0f);
        diff = Math_SmoothStepToF(&this->radius, -650.0f, 1.0f, this->actor.speedXZ, 2.0f);
        diff += Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 0.5f, 30.0f, 3.0f);
    }
    if (animFinish && (diff < 10.0f)) {
        this->actor.world.pos.y = this->actor.home.pos.y;
        this->radius = -650.0f;
        BossSst_HandSetupRetreat(sHands[LEFT]);
        BossSst_HandSetupRetreat(sHands[RIGHT]);
        BossSst_HeadSetupNeutral(this);
    }
}

void BossSst_SetCameraTargets(f32 cameraSpeedMod, s32 targetIndex) {
    Vec3f* nextAt = &sCameraAtPoints[targetIndex];
    Vec3f* nextEye = &sCameraEyePoints[targetIndex];

    if (targetIndex != 0) {
        Math_Vec3f_Copy(&sCameraAt, &sCameraAtPoints[targetIndex - 1]);
        Math_Vec3f_Copy(&sCameraEye, &sCameraEyePoints[targetIndex - 1]);
    }

    sCameraAtVel.x = (nextAt->x - sCameraAt.x) * cameraSpeedMod;
    sCameraAtVel.y = (nextAt->y - sCameraAt.y) * cameraSpeedMod;
    sCameraAtVel.z = (nextAt->z - sCameraAt.z) * cameraSpeedMod;

    sCameraEyeVel.x = (nextEye->x - sCameraEye.x) * cameraSpeedMod;
    sCameraEyeVel.y = (nextEye->y - sCameraEye.y) * cameraSpeedMod;
    sCameraEyeVel.z = (nextEye->z - sCameraEye.z) * cameraSpeedMod;
}

void BossSst_UpdateDeathCamera(BossSst* this, PlayState* play) {
    Vec3f cameraAt;
    Vec3f cameraEye;
    f32 sn;
    f32 cs;

    sCameraAt.x += sCameraAtVel.x;
    sCameraAt.y += sCameraAtVel.y;
    sCameraAt.z += sCameraAtVel.z;
    sCameraEye.x += sCameraEyeVel.x;
    sCameraEye.y += sCameraEyeVel.y;
    sCameraEye.z += sCameraEyeVel.z;

    sn = Math_SinS(this->actor.shape.rot.y);
    cs = Math_CosS(this->actor.shape.rot.y);
    cameraAt.x = this->actor.world.pos.x + (sCameraAt.z * sn) + (sCameraAt.x * cs);
    cameraAt.y = this->actor.home.pos.y - 140.0f + sCameraAt.y;
    cameraAt.z = this->actor.world.pos.z + (sCameraAt.z * cs) - (sCameraAt.x * sn);
    cameraEye.x = this->actor.world.pos.x + (sCameraEye.z * sn) + (sCameraEye.x * cs);
    cameraEye.y = this->actor.home.pos.y - 140.0f + sCameraEye.y;
    cameraEye.z = this->actor.world.pos.z + (sCameraEye.z * cs) - (sCameraEye.x * sn);
    Play_CameraSetAtEye(play, sCutsceneCamera, &cameraAt, &cameraEye);
}

void BossSst_HeadSetupDeath(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Animation_MorphToLoop(&this->skelAnime, &gBongoHeadEyeOpenIdleAnim, -5.0f);
    BossSst_HeadSfx(this, NA_SE_EN_SHADEST_DEAD);
    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 60);
    Actor_SetColorFilter(&sHands[LEFT]->actor, 0x4000, 0xFF, 0, 60);
    Actor_SetColorFilter(&sHands[RIGHT]->actor, 0x4000, 0xFF, 0, 60);
    this->timer = 60;
    this->colliderCyl.base.acFlags &= ~AC_ON;
    this->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
    sHands[LEFT]->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
    sHands[RIGHT]->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
    Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
    sCutsceneCamera = Play_CreateSubCamera(play);
    Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
    Play_ChangeCameraStatus(play, sCutsceneCamera, CAM_STAT_ACTIVE);
    Play_CopyCamera(play, sCutsceneCamera, MAIN_CAM);
    func_8002DF54(play, &player->actor, 8);
    func_80064520(play, &play->csCtx);
    Math_Vec3f_Copy(&sCameraEye, &GET_ACTIVE_CAM(play)->eye);
    this->actionFunc = BossSst_HeadDeath;
}

void BossSst_HeadDeath(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y - 140.0f, 20.0f);
    if (this->timer == 0) {
        BossSst_HandSetupThrash(sHands[LEFT]);
        BossSst_HandSetupThrash(sHands[RIGHT]);
        BossSst_HeadSetupThrash(this);
    } else if (this->timer > 48) {
        Play_CameraSetAtEye(play, sCutsceneCamera, &this->actor.focus.pos, &sCameraEye);
        Math_StepToF(&this->radius, -350.0f, 10.0f);
    } else if (this->timer == 48) {
        Player* player = GET_PLAYER(play);

        player->actor.world.pos.x = sRoomCenter.x + (400.0f * Math_SinS(this->actor.shape.rot.y)) +
                                    (Math_CosS(this->actor.shape.rot.y) * -120.0f);
        player->actor.world.pos.z = sRoomCenter.z + (400.0f * Math_CosS(this->actor.shape.rot.y)) -
                                    (Math_SinS(this->actor.shape.rot.y) * -120.0f);
        player->actor.shape.rot.y = Actor_WorldYawTowardPoint(&player->actor, &sRoomCenter);
        func_8002DBD0(&this->actor, &sCameraEye, &GET_ACTIVE_CAM(play)->eye);
        func_8002DBD0(&this->actor, &sCameraAt, &GET_ACTIVE_CAM(play)->at);
        this->radius = -350.0f;
        this->actor.world.pos.x = sRoomCenter.x - (Math_SinS(this->actor.shape.rot.y) * 350.0f);
        this->actor.world.pos.z = sRoomCenter.z - (Math_CosS(this->actor.shape.rot.y) * 350.0f);
        BossSst_SetCameraTargets(1.0 / 48, 0);
        BossSst_UpdateDeathCamera(this, play);
    } else {
        BossSst_UpdateDeathCamera(this, play);
    }
}

void BossSst_HeadSetupThrash(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gBongoHeadEyeOpenIdleAnim, -5.0f);
    this->timer = 160;
    this->targetYaw = this->actor.shape.rot.y;
    BossSst_SetCameraTargets(1.0 / 80, 1);
    this->actionFunc = BossSst_HeadThrash;
}

void BossSst_HeadThrash(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    if ((this->timer == 0) && (this->actor.shape.rot.y == this->targetYaw)) {
        BossSst_HeadSetupDarken(this);
    } else if (this->timer >= 80) {
        BossSst_UpdateDeathCamera(this, play);
    }
}

void BossSst_HeadSetupDarken(BossSst* this) {
    this->timer = 160;
    BossSst_SetCameraTargets(1.0 / 80, 2);
    this->actionFunc = BossSst_HeadDarken;
}

void BossSst_HeadDarken(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer >= 80) {
        if (this->timer == 80) {
            sBodyStatic = true;
        }
        BossSst_UpdateDeathCamera(this, play);
        sBodyColor.r = sBodyColor.g = sBodyColor.b = (this->timer * 3) - 240;
        if (this->timer == 80) {
            BossSst_SetCameraTargets(1.0 / 80, 3);
        }
    } else {
        sBodyColor.b = (80 - this->timer) / 1.0f;
        sBodyColor.r = sBodyColor.g = sStaticColor.r = sStaticColor.g = sStaticColor.b = (80 - this->timer) / 8.0f;
        BossSst_UpdateDeathCamera(this, play);
        if (this->timer == 0) {
            BossSst_HeadSetupFall(this);
        }
    }
}

void BossSst_HeadSetupFall(BossSst* this) {
    this->actor.speedXZ = 1.0f;
    Math_Vec3f_Copy(&sCameraAt, &sCameraAtPoints[3]);
    Math_Vec3f_Copy(&sCameraEye, &sCameraEyePoints[3]);
    sCameraAtVel.x = 0.0f;
    sCameraAtVel.z = 0.0f;
    sCameraAtVel.y = -50.0f;
    Math_Vec3f_Copy(&sCameraEyeVel, &sZeroVec);
    this->actionFunc = BossSst_HeadFall;
}

void BossSst_HeadFall(BossSst* this, PlayState* play) {
    this->actor.speedXZ *= 1.5f;
    if (Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y - 230.0f, this->actor.speedXZ)) {
        BossSst_HeadSetupMelt(this);
    }

    if (sCameraAt.y > 200.0f) {
        BossSst_UpdateDeathCamera(this, play);
    }
}

void BossSst_HeadSetupMelt(BossSst* this) {
    BossSst_SpawnHeadShadow(this);
    this->timer = 80;
    BossSst_SetCameraTargets(1.0 / 60, 5);
    this->actionFunc = BossSst_HeadMelt;
}

void BossSst_HeadMelt(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.scale.y -= 0.00025f;
    this->actor.scale.x += 0.000075f;
    this->actor.scale.z += 0.000075f;
    this->actor.world.pos.y = this->actor.home.pos.y - 11500.0f * this->actor.scale.y;
    if (this->timer == 0) {
        BossSst_HeadSetupFinish(this);
    } else if (this->timer >= 20.0f) {
        BossSst_UpdateDeathCamera(this, play);
    }
}

void BossSst_HeadSetupFinish(BossSst* this) {
    this->actor.draw = BossSst_DrawEffect;
    this->timer = 40;
    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS_CLEAR);
    BossSst_SetCameraTargets(1.0 / 40, 6);
    this->actionFunc = BossSst_HeadFinish;
}

void BossSst_HeadFinish(BossSst* this, PlayState* play) {
    static Color_RGBA8 colorIndigo = { 80, 80, 150, 255 };
    static Color_RGBA8 colorDarkIndigo = { 40, 40, 80, 255 };
    static Color_RGBA8 colorUnused[2] = {
        { 0, 0, 0, 255 },
        { 100, 100, 100, 0 },
    };
    Vec3f spawnPos;
    s32 i;

    this->timer--;
    if (this->effectMode == BONGO_NULL) {
        if (this->timer < -170) {
            BossSst_UpdateDeathCamera(this, play);
            Play_CopyCamera(play, MAIN_CAM, sCutsceneCamera);
            Play_ChangeCameraStatus(play, sCutsceneCamera, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_ACTIVE);
            Play_ClearCamera(play, sCutsceneCamera);
            func_8002DF54(play, &GET_PLAYER(play)->actor, 7);
            func_80064534(play, &play->csCtx);
            Actor_Kill(&this->actor);
            Actor_Kill(&sHands[LEFT]->actor);
            Actor_Kill(&sHands[RIGHT]->actor);
            Flags_SetClear(play, play->roomCtx.curRoom.num);
        }
    } else if (this->effects[0].alpha == 0) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, ROOM_CENTER_X, ROOM_CENTER_Y, ROOM_CENTER_Z, 0,
                    0, 0, WARP_DUNGEON_ADULT, true);
        Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART,
                    (Math_SinS(this->actor.shape.rot.y) * 200.0f) + ROOM_CENTER_X, ROOM_CENTER_Y,
                    Math_CosS(this->actor.shape.rot.y) * 200.0f + ROOM_CENTER_Z, 0, 0, 0, 0, true);
        BossSst_SetCameraTargets(1.0f, 7);
        this->effectMode = BONGO_NULL;
    } else if (this->timer == 0) {
        this->effects[0].status = 0;
        this->effects[1].status = -1;
        this->effects[2].status = -1;
    } else if (this->timer > 0) {
        this->effects[0].status += 5;
        BossSst_UpdateDeathCamera(this, play);
    }

    colorIndigo.a = this->effects[0].alpha;
    colorDarkIndigo.a = this->effects[0].alpha;

    for (i = 0; i < 5; i++) {
        spawnPos.x = sRoomCenter.x + 0.0f + Rand_CenteredFloat(800.0f);
        spawnPos.y = sRoomCenter.y + (-28.0f) + (Rand_ZeroOne() * 5.0f);
        spawnPos.z = sRoomCenter.z + 0.0f + Rand_CenteredFloat(800.0f);
        EffectSsGSplash_Spawn(play, &spawnPos, &colorIndigo, &colorDarkIndigo, 0, 0x3E8);
    }
}

void BossSst_HandSetupWait(BossSst* this) {
    HAND_STATE(this) = HAND_WAIT;
    this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    Animation_MorphToLoop(&this->skelAnime, sHandIdleAnims[this->actor.params], 5.0f);
    this->ready = false;
    this->timer = 20;
    this->actionFunc = BossSst_HandWait;
}

void BossSst_HandWait(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight, 20.0f);
    Math_StepToF(&this->actor.world.pos.x, this->actor.home.pos.x, 1.0f);
    Math_StepToF(&this->actor.world.pos.z, this->actor.home.pos.z, 1.0f);
    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        Player* player = GET_PLAYER(play);

        if (this->timer != 0) {
            this->timer--;
        }

        if ((this->timer == 0) && (player->actor.world.pos.y > -50.0f) && !(player->stateFlags1 & 0x6080)) {
            BossSst_HandSelectAttack(this);
        }
    } else if (sHead->actionFunc == BossSst_HeadNeutral) {
        if ((this->actor.params == BONGO_RIGHT_HAND) && ((sHead->timer % 28) == 12)) {
            BossSst_HandSetupDownbeat(this);
        } else if ((this->actor.params == BONGO_LEFT_HAND) && ((sHead->timer % 7) == 5) && (sHead->timer < 112)) {
            BossSst_HandSetupOffbeat(this);
        }
    }
}

void BossSst_HandSetupDownbeat(BossSst* this) {
    HAND_STATE(this) = HAND_BEAT;
    Animation_MorphToPlayOnce(&this->skelAnime, sHandOpenPoses[this->actor.params], 5.0f);
    this->actor.shape.rot.x = 0;
    this->timer = 12;
    this->actionFunc = BossSst_HandDownbeat;
}

void BossSst_HandDownbeat(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        BossSst_HandSetupWait(this);
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer >= 3) {
            this->actor.shape.rot.x -= 0x100;
            Math_StepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 180.0f, 20.0f);
        } else {
            this->actor.shape.rot.x += 0x300;
            Math_StepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 0.0f, 60.0f);
        }

        if (this->timer == 0) {
            sFloor->dyna.actor.params = BONGOFLOOR_HIT;
            if (sHead->actionFunc == BossSst_HeadWait) {
                if (this->ready) {
                    BossSst_HandSelectAttack(this);
                } else {
                    BossSst_HandSetupWait(this);
                }
            } else {
                BossSst_HandSetupDownbeatEnd(this);
            }
            func_800AA000(this->actor.xyzDistToPlayerSq, 0xFF, 0x14, 0x96);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
        }
    }
}

void BossSst_HandSetupDownbeatEnd(BossSst* this) {
    sFloor->dyna.actor.params = BONGOFLOOR_HIT;
    Animation_PlayOnce(&this->skelAnime, sHandFlatPoses[this->actor.params]);
    this->actionFunc = BossSst_HandDownbeatEnd;
}

void BossSst_HandDownbeatEnd(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        BossSst_HandSetupWait(this);
    } else {
        Math_SmoothStepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 40.0f, 0.5f, 20.0f, 3.0f);
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0x800, 0x100);
        Math_StepToF(&this->actor.world.pos.x, this->actor.home.pos.x, 1.0f);
        Math_StepToF(&this->actor.world.pos.z, this->actor.home.pos.z, 1.0f);
        if ((sHead->actionFunc != BossSst_HeadIntro) && ((sHead->timer % 28) == 12)) {
            BossSst_HandSetupDownbeat(this);
        }
    }
}

void BossSst_HandSetupOffbeat(BossSst* this) {
    HAND_STATE(this) = HAND_BEAT;
    Animation_MorphToPlayOnce(&this->skelAnime, sHandOpenPoses[this->actor.params], 5.0f);
    this->actor.shape.rot.x = 0;
    this->timer = 5;
    this->actionFunc = BossSst_HandOffbeat;
}

void BossSst_HandOffbeat(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        BossSst_HandSetupWait(this);
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer != 0) {
            this->actor.shape.rot.x -= 0x140;
            Math_StepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 60.0f, 15.0f);
        } else {
            this->actor.shape.rot.x += 0x500;
            Math_StepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 0.0f, 60.0f);
        }

        if (this->timer == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_TAIKO_LOW);
            BossSst_HandSetupOffbeatEnd(this);
        }
    }
}

void BossSst_HandSetupOffbeatEnd(BossSst* this) {
    Animation_PlayOnce(&this->skelAnime, sHandFlatPoses[this->actor.params]);
    this->actionFunc = BossSst_HandOffbeatEnd;
}

void BossSst_HandOffbeatEnd(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        BossSst_HandSetupWait(this);
    } else {
        Math_SmoothStepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 40.0f, 0.5f, 20.0f, 3.0f);
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0x400, 0xA0);
        Math_StepToF(&this->actor.world.pos.x, this->actor.home.pos.x, 1.0f);
        Math_StepToF(&this->actor.world.pos.z, this->actor.home.pos.z, 1.0f);
        if (sHead->actionFunc == BossSst_HeadWait) {
            if (this->ready) {
                BossSst_HandSelectAttack(this);
            } else {
                BossSst_HandSetupWait(this);
            }
        } else if ((sHead->actionFunc != BossSst_HeadIntro) && ((sHead->timer % 7) == 5) &&
                   ((sHead->timer % 28) != 5)) {
            BossSst_HandSetupOffbeat(this);
        }
    }
}

void BossSst_HandSetupEndSlam(BossSst* this) {
    HAND_STATE(this) = HAND_RETREAT;
    this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    Animation_MorphToPlayOnce(&this->skelAnime, sHandPushoffPoses[this->actor.params], 6.0f);
    this->actionFunc = BossSst_HandEndSlam;
}

void BossSst_HandEndSlam(BossSst* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        BossSst_HandSetupRetreat(this);
    }
}

void BossSst_HandSetupRetreat(BossSst* this) {
    HAND_STATE(this) = HAND_RETREAT;
    Animation_MorphToPlayOnce(&this->skelAnime, sHandHangPoses[this->actor.params], 10.0f);
    this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    this->colliderJntSph.base.acFlags |= AC_ON;
    this->actor.flags |= ACTOR_FLAG_0;
    BossSst_HandSetInvulnerable(this, false);
    this->timer = 0;
    this->actionFunc = BossSst_HandRetreat;
    this->actor.speedXZ = 3.0f;
}

void BossSst_HandRetreat(BossSst* this, PlayState* play) {
    f32 diff;
    s32 inPosition;

    SkelAnime_Update(&this->skelAnime);
    this->actor.speedXZ = this->actor.speedXZ * 1.2f;
    this->actor.speedXZ = CLAMP_MAX(this->actor.speedXZ, 50.0f);

    diff = Math_SmoothStepToF(&this->actor.world.pos.x, this->actor.home.pos.x, 0.3f, this->actor.speedXZ, 1.0f);
    diff += Math_SmoothStepToF(&this->actor.world.pos.z, this->actor.home.pos.z, 0.3f, this->actor.speedXZ, 1.0f);
    if (this->timer != 0) {
        if (this->timer != 0) {
            this->timer--;
        }

        this->actor.world.pos.y = (sinf((this->timer * M_PI) / 16.0f) * 250.0f) + this->actor.home.pos.y;
        if (this->timer == 0) {
            BossSst_HandSetupWait(this);
        } else if (this->timer == 4) {
            Animation_MorphToLoop(&this->skelAnime, sHandIdleAnims[this->actor.params], 4.0f);
        }
    } else {
        inPosition = Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 0x200);
        inPosition &= Math_ScaledStepToS(&this->actor.shape.rot.z, this->actor.home.rot.z, 0x200);
        inPosition &= Math_ScaledStepToS(&this->handYRotMod, 0, 0x800);
        func_8002F974(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
        if ((Math_SmoothStepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 250.0f, 0.5f, 70.0f, 5.0f) < 1.0f) &&
            inPosition && (diff < 10.0f)) {
            this->timer = 8;
        }
    }
}

void BossSst_HandSetupReadySlam(BossSst* this) {
    HAND_STATE(this) = HAND_SLAM;
    this->timer = 0;
    Animation_MorphToPlayOnce(&this->skelAnime, sHandOpenPoses[this->actor.params], 10.0f);
    this->actionFunc = BossSst_HandReadySlam;
}

void BossSst_HandReadySlam(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            BossSst_HandSetupSlam(this);
        }
    } else {
        Player* player = GET_PLAYER(play);

        if (Math_StepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 300.0f, 30.0f) &&
            (this->actor.xzDistToPlayer < 140.0f)) {
            this->timer = 20;
        }
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0x1000, 0x100);
        Math_ApproachF(&this->actor.world.pos.x, player->actor.world.pos.x, 0.5f, 40.0f);
        Math_ApproachF(&this->actor.world.pos.z, player->actor.world.pos.z, 0.5f, 40.0f);
        func_8002F974(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
    }
}

void BossSst_HandSetupSlam(BossSst* this) {
    HAND_STATE(this) = HAND_SLAM;
    this->actor.velocity.y = 1.0f;
    Animation_MorphToPlayOnce(&this->skelAnime, sHandFlatPoses[this->actor.params], 10.0f);
    BossSst_HandSetDamage(this, 0x20);
    this->ready = false;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_FLY_ATTACK);
    this->actionFunc = BossSst_HandSlam;
}

void BossSst_HandSlam(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_StepToS(&this->handZPosMod, -0xDAC, 0x1F4);
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x1000);
    Math_ScaledStepToS(&this->handYRotMod, 0, 0x1000);
    if (this->timer != 0) {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            if (this->colliderJntSph.base.acFlags & AC_ON) {
                BossSst_HandSetupEndSlam(this);
            } else {
                this->colliderJntSph.base.acFlags |= AC_ON;
                BossSst_HandSetupWait(this);
            }
        }
    } else {
        if (this->ready) {
            this->timer = 30;
            this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        } else {
            this->actor.velocity.y *= 1.5f;
            if (Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight, this->actor.velocity.y)) {
                this->ready = true;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_TAIKO_LOW);
                BossSst_SpawnShockwave(this);
                this->colliderCyl.base.atFlags |= AT_ON;
                Collider_UpdateCylinder(&this->actor, &this->colliderCyl);
                this->colliderCyl.dim.radius = sCylinderInitHand.dim.radius;
            }
        }

        if (this->colliderJntSph.base.atFlags & AT_HIT) {
            Player* player = GET_PLAYER(play);

            player->actor.world.pos.x = (Math_SinS(this->actor.yawTowardsPlayer) * 100.0f) + this->actor.world.pos.x;
            player->actor.world.pos.z = (Math_CosS(this->actor.yawTowardsPlayer) * 100.0f) + this->actor.world.pos.z;

            this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            func_8002F71C(play, &this->actor, 5.0f, this->actor.yawTowardsPlayer, 0.0f);
        }

        Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x200);
    }
}

void BossSst_HandSetupReadySweep(BossSst* this) {
    HAND_STATE(this) = HAND_SWEEP;
    Animation_MorphToPlayOnce(&this->skelAnime, sHandOpenPoses[this->actor.params], 10.0f);
    this->radius = Actor_WorldDistXZToPoint(&this->actor, &sHead->actor.world.pos);
    this->actor.world.rot.y = Actor_WorldYawTowardPoint(&sHead->actor, &this->actor.world.pos);
    this->targetYaw = this->actor.home.rot.y + (this->vParity * 0x2000);
    this->actionFunc = BossSst_HandReadySweep;
}

void BossSst_HandReadySweep(BossSst* this, PlayState* play) {
    s32 inPosition;

    SkelAnime_Update(&this->skelAnime);
    inPosition = Math_StepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 50.0f, 4.0f);
    inPosition &= Math_ScaledStepToS(&this->actor.shape.rot.y, this->targetYaw, 0x200);
    inPosition &= Math_ScaledStepToS(&this->actor.world.rot.y, this->targetYaw, 0x400);
    inPosition &= (Math_SmoothStepToF(&this->radius, sHead->actor.xzDistToPlayer, 0.5f, 60.0f, 1.0f) < 10.0f);

    this->actor.world.pos.x = (Math_SinS(this->actor.world.rot.y) * this->radius) + sHead->actor.world.pos.x;
    this->actor.world.pos.z = (Math_CosS(this->actor.world.rot.y) * this->radius) + sHead->actor.world.pos.z;
    if (inPosition) {
        BossSst_HandSetupSweep(this);
    } else {
        func_8002F974(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
    }
}

void BossSst_HandSetupSweep(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, sHandFlatPoses[this->actor.params], 5.0f);
    BossSst_HandSetDamage(this, 0x10);
    this->targetYaw = this->actor.home.rot.y - (this->vParity * 0x2000);
    this->handMaxSpeed = 0x300;
    this->handAngSpeed = 0;
    this->ready = false;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_FLY_ATTACK);
    this->actionFunc = BossSst_HandSweep;
}

void BossSst_HandSweep(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 newTargetYaw;

    SkelAnime_Update(&this->skelAnime);
    this->handAngSpeed += 0x60;
    this->handAngSpeed = CLAMP_MAX(this->handAngSpeed, this->handMaxSpeed);

    if (!Math_SmoothStepToS(&this->actor.shape.rot.y, this->targetYaw, 4, this->handAngSpeed, 0x10)) {
        this->colliderJntSph.base.ocFlags1 &= ~OC1_NO_PUSH;
        BossSst_HandSetupRetreat(this);
    } else if (this->colliderJntSph.base.atFlags & AT_HIT) {
        this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        this->ready = true;
        func_8002F71C(play, &this->actor, 5.0f, this->actor.shape.rot.y - (this->vParity * 0x3800), 0.0f);
        func_8002F7DC(&player->actor, NA_SE_PL_BODY_HIT);
        newTargetYaw = this->actor.shape.rot.y - (this->vParity * 0x1400);
        if (((s16)(newTargetYaw - this->targetYaw) * this->vParity) > 0) {
            this->targetYaw = newTargetYaw;
        }
    }

    if (!this->ready && ((player->cylinder.dim.height > 40.0f) || (player->actor.world.pos.y > 1.0f))) {
        this->colliderJntSph.base.atFlags |= AT_ON;
        this->colliderJntSph.base.ocFlags1 &= ~OC1_NO_PUSH;
    } else {
        this->colliderJntSph.base.atFlags &= ~AT_ON;
        this->colliderJntSph.base.ocFlags1 |= OC1_NO_PUSH;
    }

    this->actor.world.pos.x = (Math_SinS(this->actor.shape.rot.y) * this->radius) + sHead->actor.world.pos.x;
    this->actor.world.pos.z = (Math_CosS(this->actor.shape.rot.y) * this->radius) + sHead->actor.world.pos.z;
}

void BossSst_HandSetupReadyPunch(BossSst* this) {
    HAND_STATE(this) = HAND_PUNCH;
    Animation_MorphToPlayOnce(&this->skelAnime, sHandPushoffPoses[this->actor.params], 10.0f);
    this->actionFunc = BossSst_HandReadyPunch;
}

void BossSst_HandReadyPunch(BossSst* this, PlayState* play) {
    s32 inPosition = Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x400);

    if (SkelAnime_Update(&this->skelAnime) && inPosition) {
        BossSst_HandSetupPunch(this);
    }
}

void BossSst_HandSetupPunch(BossSst* this) {
    this->actor.speedXZ = 0.5f;
    Animation_MorphToPlayOnce(&this->skelAnime, sHandFistPoses[this->actor.params], 5.0f);
    BossSst_HandSetInvulnerable(this, true);
    this->targetRoll = this->vParity * 0x3F00;
    BossSst_HandSetDamage(this, 0x10);
    this->actionFunc = BossSst_HandPunch;
}

void BossSst_HandPunch(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_StepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 80.0f, 20.0f);
    if (Math_ScaledStepToS(&this->actor.shape.rot.z, this->targetRoll, 0x400)) {
        this->targetRoll *= -1;
    }

    this->actor.speedXZ *= 1.25f;
    this->actor.speedXZ = CLAMP_MAX(this->actor.speedXZ, 50.0f);

    this->actor.world.pos.x += this->actor.speedXZ * Math_SinS(this->actor.shape.rot.y);
    this->actor.world.pos.z += this->actor.speedXZ * Math_CosS(this->actor.shape.rot.y);
    if (this->actor.bgCheckFlags & 8) {
        BossSst_HandSetupRetreat(this);
    } else if (this->colliderJntSph.base.atFlags & AT_HIT) {
        func_8002F7DC(&GET_PLAYER(play)->actor, NA_SE_PL_BODY_HIT);
        func_8002F71C(play, &this->actor, 10.0f, this->actor.shape.rot.y, 5.0f);
        BossSst_HandSetupRetreat(this);
    }

    func_8002F974(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
}

void BossSst_HandSetupReadyClap(BossSst* this) {
    HAND_STATE(this) = HAND_CLAP;
    if (HAND_STATE(OTHER_HAND(this)) != HAND_CLAP) {
        BossSst_HandSetupReadyClap(OTHER_HAND(this));
    }

    Animation_MorphToPlayOnce(&this->skelAnime, sHandOpenPoses[this->actor.params], 10.0f);
    this->radius = Actor_WorldDistXZToPoint(&this->actor, &sHead->actor.world.pos);
    this->actor.world.rot.y = Actor_WorldYawTowardPoint(&sHead->actor, &this->actor.world.pos);
    this->targetYaw = this->actor.home.rot.y - (this->vParity * 0x1800);
    this->targetRoll = this->vParity * 0x4000;
    this->timer = 0;
    this->ready = false;
    OTHER_HAND(this)->ready = false;
    this->actionFunc = BossSst_HandReadyClap;
}

void BossSst_HandReadyClap(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            BossSst_HandSetupClap(this);
            BossSst_HandSetupClap(OTHER_HAND(this));
            OTHER_HAND(this)->radius = this->radius;
        }
    } else if (!this->ready) {
        this->ready = SkelAnime_Update(&this->skelAnime);
        this->ready &= Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x600);
        this->ready &= Math_ScaledStepToS(&this->actor.shape.rot.z, this->targetRoll, 0x600);
        this->ready &= Math_ScaledStepToS(&this->actor.shape.rot.y, this->targetYaw, 0x200);
        this->ready &= Math_ScaledStepToS(&this->actor.world.rot.y, this->targetYaw, 0x400);
        this->ready &= Math_SmoothStepToF(&this->radius, sHead->actor.xzDistToPlayer, 0.5f, 50.0f, 1.0f) < 10.0f;
        this->ready &= Math_SmoothStepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 95.0f, 0.5f, 30.0f, 1.0f) < 1.0f;

        this->actor.world.pos.x = Math_SinS(this->actor.world.rot.y) * this->radius + sHead->actor.world.pos.x;
        this->actor.world.pos.z = Math_CosS(this->actor.world.rot.y) * this->radius + sHead->actor.world.pos.z;
    } else if (OTHER_HAND(this)->ready) {
        this->timer = 20;
    }
}

void BossSst_HandSetupClap(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, sHandFlatPoses[this->actor.params], 3.0f);
    this->timer = 0;
    this->handMaxSpeed = 0x240;
    this->handAngSpeed = 0;
    this->ready = false;
    BossSst_HandSetDamage(this, 0x20);
    this->actionFunc = BossSst_HandClap;
}

void BossSst_HandClap(BossSst* this, PlayState* play) {
    static s32 dropFlag = false;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            if (dropFlag) {
                Item_DropCollectible(play, &this->actor.world.pos,
                                     (Rand_ZeroOne() < 0.5f) ? ITEM00_ARROWS_SMALL : ITEM00_MAGIC_SMALL);
                dropFlag = false;
            }

            BossSst_HandReleasePlayer(this, play, true);
            BossSst_HandSetupEndClap(this);
        }
    } else {
        if (this->colliderJntSph.base.atFlags & AT_HIT) {
            this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            OTHER_HAND(this)->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            BossSst_HandGrabPlayer(this, play);
        }

        if (this->ready) {
            this->timer = 30;
            this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            if (!(player->stateFlags2 & 0x80)) {
                dropFlag = true;
            }
        } else {
            this->handAngSpeed += 0x40;
            this->handAngSpeed = CLAMP_MAX(this->handAngSpeed, this->handMaxSpeed);

            if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, this->handAngSpeed)) {
                if (this->actor.params == BONGO_LEFT_HAND) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_CLAP);
                }
                this->ready = true;
            } else {
                func_8002F974(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
            }

            this->actor.world.pos.x = (Math_SinS(this->actor.shape.rot.y) * this->radius) + sHead->actor.world.pos.x;
            this->actor.world.pos.z = (Math_CosS(this->actor.shape.rot.y) * this->radius) + sHead->actor.world.pos.z;
        }
    }

    if (player->actor.parent == &this->actor) {
        player->unk_850 = 0;
        player->actor.world.pos = this->actor.world.pos;
    }
}

void BossSst_HandSetupEndClap(BossSst* this) {
    this->targetYaw = this->actor.home.rot.y - (this->vParity * 0x1000);
    Animation_MorphToPlayOnce(&this->skelAnime, sHandOpenPoses[this->actor.params], 10.0f);
    this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    this->actionFunc = BossSst_HandEndClap;
}

void BossSst_HandEndClap(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->actor.shape.rot.z, 0, 0x200);
    if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->targetYaw, 0x100)) {
        BossSst_HandSetupRetreat(this);
    }
    this->actor.world.pos.x = (Math_SinS(this->actor.shape.rot.y) * this->radius) + sHead->actor.world.pos.x;
    this->actor.world.pos.z = (Math_CosS(this->actor.shape.rot.y) * this->radius) + sHead->actor.world.pos.z;
}

void BossSst_HandSetupReadyGrab(BossSst* this) {
    HAND_STATE(this) = HAND_GRAB;
    Animation_MorphToPlayOnce(&this->skelAnime, sHandOpenPoses[this->actor.params], 10.0f);
    this->targetYaw = this->vParity * -0x5000;
    this->targetRoll = this->vParity * 0x4000;
    this->actionFunc = BossSst_HandReadyGrab;
}

void BossSst_HandReadyGrab(BossSst* this, PlayState* play) {
    s32 inPosition;

    SkelAnime_Update(&this->skelAnime);
    inPosition = Math_SmoothStepToS(&this->actor.shape.rot.z, this->targetRoll, 4, 0x800, 0x100) == 0;
    inPosition &= Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer + this->targetYaw, 0xA00);
    Math_ApproachF(&this->actor.world.pos.y, ROOM_CENTER_Y + 95.0f, 0.5f, 20.0f);
    if (inPosition) {
        BossSst_HandSetupGrab(this);
    }
}

void BossSst_HandSetupGrab(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, sHandFistPoses[this->actor.params], 5.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y + (this->vParity * 0x4000);
    this->targetYaw = this->actor.world.rot.y;
    this->timer = 30;
    this->actor.speedXZ = 0.5f;
    BossSst_HandSetDamage(this, 0x20);
    this->actionFunc = BossSst_HandGrab;
}

void BossSst_HandGrab(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.world.rot.y =
        ((1.0f - sinf(this->timer * (M_PI / 60.0f))) * (this->vParity * 0x2000)) + this->targetYaw;
    this->actor.shape.rot.y = this->actor.world.rot.y - (this->vParity * 0x4000);
    if (this->timer < 5) {
        Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 0.5f, 25.0f, 5.0f);
        if (SkelAnime_Update(&this->skelAnime)) {
            this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            this->actor.speedXZ = 0.0f;
            if (player->stateFlags2 & 0x80) {
                if (Rand_ZeroOne() < 0.5f) {
                    BossSst_HandSetupCrush(this);
                } else {
                    BossSst_HandSetupSwing(this);
                }
            } else {
                Item_DropCollectible(play, &this->actor.world.pos,
                                     (Rand_ZeroOne() < 0.5f) ? ITEM00_ARROWS_SMALL : ITEM00_MAGIC_SMALL);
                BossSst_HandSetupRetreat(this);
            }
        }
    } else {
        this->actor.speedXZ *= 1.26f;
        this->actor.speedXZ = CLAMP_MAX(this->actor.speedXZ, 70.0f);
        func_8002F974(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
    }

    if (this->colliderJntSph.base.atFlags & AT_HIT) {
        this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_CATCH);
        BossSst_HandGrabPlayer(this, play);
        this->timer = CLAMP_MAX(this->timer, 5);
    }

    this->actor.world.pos.x += this->actor.speedXZ * Math_SinS(this->actor.world.rot.y);
    this->actor.world.pos.z += this->actor.speedXZ * Math_CosS(this->actor.world.rot.y);
    if (player->stateFlags2 & 0x80) {
        player->unk_850 = 0;
        player->actor.world.pos = this->actor.world.pos;
        player->actor.shape.rot.y = this->actor.shape.rot.y;
    }
}

void BossSst_HandSetupCrush(BossSst* this) {
    Animation_MorphToLoop(&this->skelAnime, sHandClenchAnims[this->actor.params], -10.0f);
    this->timer = 20;
    this->actionFunc = BossSst_HandCrush;
}

void BossSst_HandCrush(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    if (!(player->stateFlags2 & 0x80)) {
        BossSst_HandReleasePlayer(this, play, true);
        BossSst_HandSetupEndCrush(this);
    } else {
        player->actor.world.pos = this->actor.world.pos;
        if (this->timer == 0) {
            this->timer = 20;
            if (!LINK_IS_ADULT) {
                func_8002F7DC(&player->actor, NA_SE_VO_LI_DAMAGE_S_KID);
            } else {
                func_8002F7DC(&player->actor, NA_SE_VO_LI_DAMAGE_S);
            }

            play->damagePlayer(play, -8);
        }
        if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_CATCH);
        }
    }
}

void BossSst_HandSetupEndCrush(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, sHandFlatPoses[this->actor.params], 10.0f);
    this->actionFunc = BossSst_HandEndCrush;
}

void BossSst_HandEndCrush(BossSst* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        BossSst_HandSetupRetreat(this);
    }
}

void BossSst_HandSetupSwing(BossSst* this) {
    this->amplitude = -0x4000;
    this->timer = 1;
    this->center.x = this->actor.world.pos.x - (Math_SinS(this->actor.shape.rot.y) * 200.0f);
    this->center.y = this->actor.world.pos.y;
    this->center.z = this->actor.world.pos.z - (Math_CosS(this->actor.shape.rot.y) * 200.0f);
    this->actionFunc = BossSst_HandSwing;
}

void BossSst_HandSwing(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 offXZ;

    if (Math_ScaledStepToS(&this->actor.shape.rot.x, this->amplitude, this->timer * 0xE4 + 0x1C8)) {
        if (this->amplitude != 0) {
            this->amplitude = 0;
            if (this->timer == 4) {
                Animation_MorphToPlayOnce(&this->skelAnime, sHandFlatPoses[this->actor.params], 4.0f);
            }
        } else {
            if (this->timer == 4) {
                player->actor.shape.rot.x = 0;
                player->actor.shape.rot.z = 0;
                BossSst_HandSetupRetreat(this);
                return;
            }
            this->amplitude = (this->timer == 3) ? -0x6000 : -0x4000;
            this->timer++;
        }
    }

    this->actor.world.pos.y = (Math_CosS(this->actor.shape.rot.x + 0x4000) * 200.0f) + this->center.y;
    offXZ = Math_SinS(this->actor.shape.rot.x + 0x4000) * 200.0f;
    this->actor.world.pos.x = (Math_SinS(this->actor.shape.rot.y) * offXZ) + this->center.x;
    this->actor.world.pos.z = (Math_CosS(this->actor.shape.rot.y) * offXZ) + this->center.z;
    if (this->timer != 4) {
        this->actor.shape.rot.z = (this->actor.shape.rot.x + 0x4000) * this->vParity;
    } else {
        Math_ScaledStepToS(&this->actor.shape.rot.z, 0, 0x800);
    }

    if (player->stateFlags2 & 0x80) {
        player->unk_850 = 0;
        Math_Vec3f_Copy(&player->actor.world.pos, &this->actor.world.pos);
        player->actor.shape.rot.x = this->actor.shape.rot.x;
        player->actor.shape.rot.z = (this->vParity * -0x4000) + this->actor.shape.rot.z;
    } else {
        Math_ScaledStepToS(&player->actor.shape.rot.x, 0, 0x600);
        Math_ScaledStepToS(&player->actor.shape.rot.z, 0, 0x600);
        player->actor.world.pos.x += 20.0f * Math_SinS(this->actor.shape.rot.y);
        player->actor.world.pos.z += 20.0f * Math_CosS(this->actor.shape.rot.y);
    }

    if ((this->timer == 4) && (this->amplitude == 0) && SkelAnime_Update(&this->skelAnime) &&
        (player->stateFlags2 & 0x80)) {
        BossSst_HandReleasePlayer(this, play, false);
        player->actor.world.pos.x += 70.0f * Math_SinS(this->actor.shape.rot.y);
        player->actor.world.pos.z += 70.0f * Math_CosS(this->actor.shape.rot.y);
        func_8002F71C(play, &this->actor, 15.0f, this->actor.shape.rot.y, 2.0f);
        func_8002F7DC(&player->actor, NA_SE_PL_BODY_HIT);
    }

    func_8002F974(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
}

void BossSst_HandSetupReel(BossSst* this) {
    HAND_STATE(this) = HAND_DAMAGED;
    Animation_MorphToPlayOnce(&this->skelAnime, sHandFlatPoses[this->actor.params], 4.0f);
    this->timer = 36;
    Math_Vec3f_Copy(&this->center, &this->actor.world.pos);
    Actor_SetColorFilter(&this->actor, 0, 0xFF, 0, 200);
    this->actionFunc = BossSst_HandReel;
}

void BossSst_HandReel(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    if (!(this->timer % 4)) {
        if (this->timer % 8) {
            Animation_MorphToPlayOnce(&this->skelAnime, sHandFlatPoses[this->actor.params], 4.0f);
        } else {
            Animation_MorphToPlayOnce(&this->skelAnime, sHandFistPoses[this->actor.params], 6.0f);
        }
    }

    this->actor.colorFilterTimer = 200;
    this->actor.world.pos.x += Rand_CenteredFloat(20.0f);
    this->actor.world.pos.y += Rand_CenteredFloat(20.0f);
    this->actor.world.pos.z += Rand_CenteredFloat(20.0f);

    if (this->actor.world.pos.y < (this->actor.floorHeight + 100.0f)) {
        Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight + 100.0f, 20.0f);
    }

    if (this->timer == 0) {
        BossSst_HandSetupReadyShake(this);
    }
}

void BossSst_HandSetupReadyShake(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, sHandDamagePoses[this->actor.params], 8.0f);
    this->actionFunc = BossSst_HandReadyShake;
}

void BossSst_HandReadyShake(BossSst* this, PlayState* play) {
    f32 diff;
    s32 inPosition;

    diff = Math_SmoothStepToF(&this->actor.world.pos.x, this->actor.home.pos.x, 0.5f, 25.0f, 1.0f);
    diff += Math_SmoothStepToF(&this->actor.world.pos.z, this->actor.home.pos.z, 0.5f, 25.0f, 1.0f);
    diff += Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.home.pos.y + 200.0f, 0.2f, 30.0f, 1.0f);
    inPosition = Math_ScaledStepToS(&this->actor.shape.rot.x, 0x4000, 0x400);
    inPosition &= Math_ScaledStepToS(&this->actor.shape.rot.z, 0, 0x1000);
    inPosition &= Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 0x800);
    inPosition &= Math_StepToS(&this->handZPosMod, -0x5DC, 0x1F4);
    inPosition &= Math_ScaledStepToS(&this->handYRotMod, this->vParity * -0x2000, 0x800);
    this->actor.colorFilterTimer = 200;
    if ((diff < 30.0f) && inPosition) {
        BossSst_HandSetupShake(this);
    } else {
        func_8002F974(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
    }
}

void BossSst_HandSetupShake(BossSst* this) {
    this->timer = 200;
    this->actionFunc = BossSst_HandShake;
}

void BossSst_HandShake(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.shape.rot.x = 0x4000 + (sinf(this->timer * (M_PI / 5)) * 0x2000);
    this->handYRotMod = (this->vParity * -0x2000) + (sinf(this->timer * (M_PI / 4)) * 0x2800);

    if (!(this->timer % 8)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_SHAKEHAND);
    }

    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        if ((OTHER_HAND(this)->actionFunc == BossSst_HandShake) ||
            (OTHER_HAND(this)->actionFunc == BossSst_HandReadyCharge)) {
            BossSst_HandSetupReadyCharge(this);
        } else if (this->timer == 0) {
            this->timer = 80;
        }
    } else if (this->timer == 0) {
        this->actor.flags |= ACTOR_FLAG_0;
        BossSst_HandSetupSlam(this);
    }
}

void BossSst_HandSetupReadyCharge(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, sHandFistPoses[this->actor.params], 10.0f);
    this->ready = false;
    this->actionFunc = BossSst_HandReadyCharge;
}

void BossSst_HandReadyCharge(BossSst* this, PlayState* play) {
    if (!this->ready) {
        this->ready = SkelAnime_Update(&this->skelAnime);
        this->ready &= Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x800);
        this->ready &=
            Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y + (this->vParity * 0x1000), 0x800);
        this->ready &= Math_ScaledStepToS(&this->handYRotMod, 0, 0x800);
        this->ready &= Math_ScaledStepToS(&this->actor.shape.rot.z, this->vParity * 0x2800, 0x800);
        this->ready &= Math_StepToS(&this->handZPosMod, -0xDAC, 0x1F4);
        if (this->ready) {
            this->actor.colorFilterTimer = 0;
        }
    } else if (this->colliderJntSph.base.atFlags & AT_HIT) {
        this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        OTHER_HAND(this)->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        sHead->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        func_8002F71C(play, &this->actor, 10.0f, this->actor.shape.rot.y, 5.0f);
        func_8002F7DC(&GET_PLAYER(play)->actor, NA_SE_PL_BODY_HIT);
    }
}

void BossSst_HandSetupStunned(BossSst* hand) {
    Animation_MorphToPlayOnce(&hand->skelAnime, sHandIdleAnims[hand->actor.params], 10.0f);
    if (hand->actionFunc != BossSst_HandDamage) {
        hand->ready = false;
    }

    hand->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    hand->colliderJntSph.base.acFlags |= AC_ON;
    BossSst_HandSetInvulnerable(hand, true);
    Actor_SetColorFilter(&hand->actor, 0, 0xFF, 0, Animation_GetLastFrame(&gBongoHeadKnockoutAnim));
    hand->actionFunc = BossSst_HandStunned;
}

void BossSst_HandStunned(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachF(&this->actor.world.pos.z, (Math_CosS(sHead->actor.shape.rot.y) * 200.0f) + this->actor.home.pos.z,
                   0.5f, 25.0f);
    Math_ApproachF(&this->actor.world.pos.x, (Math_SinS(sHead->actor.shape.rot.y) * 200.0f) + this->actor.home.pos.x,
                   0.5f, 25.0f);
    if (!this->ready) {
        Math_ScaledStepToS(&this->handYRotMod, 0, 0x800);
        Math_StepToS(&this->handZPosMod, -0xDAC, 0x1F4);
        Math_ScaledStepToS(&this->actor.shape.rot.x, this->actor.home.rot.x, 0x800);
        Math_ScaledStepToS(&this->actor.shape.rot.z, this->actor.home.rot.z, 0x800);
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 0x800);
        if (sHead->actionFunc == BossSst_HeadVulnerable) {
            this->ready = true;
            Animation_MorphToPlayOnce(&this->skelAnime, sHandDamagePoses[this->actor.params], 10.0f);
        }
    } else {
        Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight, 30.0f);
    }
}

void BossSst_HandSetupDamage(BossSst* hand) {
    hand->actor.shape.rot.x = 0;
    Animation_MorphToPlayOnce(&hand->skelAnime, sHandOpenPoses[hand->actor.params], 3.0f);
    hand->timer = 6;
    hand->actionFunc = BossSst_HandDamage;
}

void BossSst_HandDamage(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    SkelAnime_Update(&this->skelAnime);
    if (this->timer >= 2) {
        this->actor.shape.rot.x -= 0x200;
        Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight + 200.0f, 50.0f);
    } else {
        this->actor.shape.rot.x += 0x400;
        Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight, 100.0f);
    }

    if (this->timer == 0) {
        if (this->actor.floorHeight >= 0.0f) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
        }
        BossSst_HandSetupStunned(this);
    }
}

void BossSst_HandSetupThrash(BossSst* this) {
    HAND_STATE(this) = HAND_DEATH;
    Animation_MorphToPlayOnce(&this->skelAnime, sHandOpenPoses[this->actor.params], 2.0f);
    this->actor.shape.rot.x = 0;
    this->timer = 160;
    if (this->actor.params == BONGO_LEFT_HAND) {
        this->amplitude = -0x800;
    } else {
        this->amplitude = 0;
        this->actor.shape.rot.x = -0x800;
    }

    this->handAngSpeed = 0x180;
    this->actionFunc = BossSst_HandThrash;
}

void BossSst_HandThrash(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    SkelAnime_Update(&this->skelAnime);
    Math_ApproachF(&this->actor.world.pos.z, (Math_CosS(sHead->actor.shape.rot.y) * 200.0f) + this->actor.home.pos.z,
                   0.5f, 25.0f);
    Math_ApproachF(&this->actor.world.pos.x, (Math_SinS(sHead->actor.shape.rot.y) * 200.0f) + this->actor.home.pos.x,
                   0.5f, 25.0f);
    if (Math_ScaledStepToS(&this->actor.shape.rot.x, this->amplitude, this->handAngSpeed)) {
        if (this->amplitude != 0) {
            this->amplitude = 0;
            Animation_MorphToPlayOnce(&this->skelAnime, sHandFlatPoses[this->actor.params], 5.0f);
        } else {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
            this->amplitude = -0x800;
            Animation_MorphToPlayOnce(&this->skelAnime, sHandOpenPoses[this->actor.params], 5.0f);
        }

        if (this->timer < 80.0f) {
            this->handAngSpeed -= 0x40;
            this->handAngSpeed = CLAMP_MIN(this->handAngSpeed, 0x40);
        }
    }

    this->actor.world.pos.y =
        (((this->handAngSpeed / 256.0f) + 0.5f) * 150.0f) * (-1.0f / 0x800) * this->actor.shape.rot.x;
    if (this->timer == 0) {
        BossSst_HandSetupDarken(this);
    }
}

void BossSst_HandSetupDarken(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, sHandFlatPoses[this->actor.params], 5.0f);
    this->actionFunc = BossSst_HandDarken;
}

void BossSst_HandDarken(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->actor.shape.rot.x, -0x800, this->handAngSpeed);
    Math_StepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 90.0f, 5.0f);
    if (sHead->actionFunc == BossSst_HeadFall) {
        BossSst_HandSetupFall(this);
    }
}

void BossSst_HandSetupFall(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, sHandFlatPoses[this->actor.params], 3.0f);
    this->actionFunc = BossSst_HandFall;
}

void BossSst_HandFall(BossSst* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x400);
    this->actor.world.pos.y = sHead->actor.world.pos.y + 230.0f;
    if (sHead->actionFunc == BossSst_HeadMelt) {
        BossSst_HandSetupMelt(this);
    }
}

void BossSst_HandSetupMelt(BossSst* this) {
    BossSst_SpawnHandShadow(this);
    this->actor.shape.shadowDraw = NULL;
    this->timer = 80;
    this->actionFunc = BossSst_HandMelt;
}

void BossSst_HandMelt(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.scale.y -= 0.00025f;
    this->actor.scale.x += 0.000025f;
    this->actor.scale.z += 0.000025f;
    this->actor.world.pos.y = ROOM_CENTER_Y + 0.0f;
    if (this->timer == 0) {
        BossSst_HandSetupFinish(this);
    }
}

void BossSst_HandSetupFinish(BossSst* this) {
    this->actor.draw = BossSst_DrawEffect;
    this->timer = 20;
    this->effects[0].status = 0;
    this->actionFunc = BossSst_HandFinish;
}

void BossSst_HandFinish(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        this->effectMode = BONGO_NULL;
    }
}

void BossSst_HandSetupRecover(BossSst* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, sHandPushoffPoses[this->actor.params], 10.0f);
    this->ready = false;
    this->actionFunc = BossSst_HandRecover;
}

void BossSst_HandRecover(BossSst* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.world.pos.y, ROOM_CENTER_Y + 250.0f, 0.5f, 70.0f, 5.0f);
    if (SkelAnime_Update(&this->skelAnime)) {
        if (!this->ready) {
            Animation_MorphToPlayOnce(&this->skelAnime, sHandHangPoses[this->actor.params], 10.0f);
            this->ready = true;
        }
    }
    func_8002F974(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
}

void BossSst_HandSetupFrozen(BossSst* this) {
    s32 i;

    HAND_STATE(this) = HAND_FROZEN;
    Math_Vec3f_Copy(&this->center, &this->actor.world.pos);
    BossSst_HandSetupReadyBreakIce(OTHER_HAND(this));
    this->ready = false;
    this->effectMode = BONGO_ICE;
    this->timer = 35;
    for (i = 0; i < 18; i++) {
        this->effects[i].move = false;
    }

    BossSst_SpawnIceCrystal(this, 0);
    Actor_SetColorFilter(&this->actor, 0, 0xFF, 0, 0xA);
    this->handAngSpeed = 0;
    this->actionFunc = BossSst_HandFrozen;
}

void BossSst_HandFrozen(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if ((this->timer % 2) != 0) {
        BossSst_SpawnIceCrystal(this, (this->timer >> 1) + 1);
    }

    if (this->ready) {
        BossSst_IceShatter(this);
        BossSst_HandSetupRetreat(this);
        sHead->ready = true;
    } else {
        this->actor.colorFilterTimer = 10;
        if (this->handAngSpeed != 0) {
            f32 offY = Math_SinS(OTHER_HAND(this)->actor.shape.rot.x) * 5.0f;
            f32 offXZ = Math_CosS(OTHER_HAND(this)->actor.shape.rot.x) * 5.0f;

            if ((this->handAngSpeed % 2) != 0) {
                offY *= -1.0f;
                offXZ *= -1.0f;
            }

            this->actor.world.pos.x = this->center.x + (Math_CosS(OTHER_HAND(this)->actor.shape.rot.y) * offXZ);
            this->actor.world.pos.y = this->center.y + offY;
            this->actor.world.pos.z = this->center.z + (Math_SinS(OTHER_HAND(this)->actor.shape.rot.y) * offXZ);
            this->handAngSpeed--;
        }
    }
}

void BossSst_HandSetupReadyBreakIce(BossSst* this) {
    HAND_STATE(this) = HAND_BREAK_ICE;
    Animation_MorphToPlayOnce(&this->skelAnime, sHandFistPoses[this->actor.params], 5.0f);
    this->ready = false;
    this->actor.colorFilterTimer = 0;
    if (this->effectMode == BONGO_ICE) {
        this->effectMode = BONGO_NULL;
    }

    this->radius = Actor_WorldDistXZToPoint(&this->actor, &OTHER_HAND(this)->center);
    this->targetYaw = Actor_WorldYawTowardPoint(&this->actor, &OTHER_HAND(this)->center);
    BossSst_HandSetInvulnerable(this, true);
    this->actionFunc = BossSst_HandReadyBreakIce;
}

void BossSst_HandReadyBreakIce(BossSst* this, PlayState* play) {
    s32 inPosition;

    inPosition = Math_ScaledStepToS(&this->actor.shape.rot.y, this->targetYaw, 0x400);
    inPosition &= Math_ScaledStepToS(&this->actor.shape.rot.x, 0x1000, 0x400);
    inPosition &= Math_ScaledStepToS(&this->actor.shape.rot.z, 0, 0x800);
    inPosition &= Math_ScaledStepToS(&this->handYRotMod, 0, 0x400);
    inPosition &= Math_StepToF(&this->actor.world.pos.y, OTHER_HAND(this)->center.y + 200.0f, 50.0f);
    inPosition &= Math_StepToF(&this->radius, 400.0f, 60.0f);
    this->actor.world.pos.x = OTHER_HAND(this)->center.x - (Math_SinS(this->targetYaw) * this->radius);
    this->actor.world.pos.z = OTHER_HAND(this)->center.z - (Math_CosS(this->targetYaw) * this->radius);
    if (SkelAnime_Update(&this->skelAnime) && inPosition) {
        BossSst_HandSetupBreakIce(this);
    }
}

void BossSst_HandSetupBreakIce(BossSst* this) {
    this->timer = 9;
    this->actionFunc = BossSst_HandBreakIce;
    this->actor.speedXZ = 0.5f;
}

void BossSst_HandBreakIce(BossSst* this, PlayState* play) {
    if ((this->timer % 2) != 0) {
        this->actor.speedXZ *= 1.5f;
        this->actor.speedXZ = CLAMP_MAX(this->actor.speedXZ, 60.0f);

        if (Math_StepToF(&this->radius, 100.0f, this->actor.speedXZ)) {
            BossSst_SpawnIceShard(this);
            if (this->timer != 0) {
                this->timer--;
            }

            if (this->timer != 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_ICE_BROKEN);
            }

            OTHER_HAND(this)->handAngSpeed = 5;
        }
    } else {
        this->actor.speedXZ *= 0.8f;
        Math_StepToF(&this->radius, 500.0f, this->actor.speedXZ);
        if (this->actor.speedXZ < 2.0f) {
            if (this->timer != 0) {
                this->timer--;
            }
        }
    }

    this->actor.world.pos.x = OTHER_HAND(this)->center.x - (Math_SinS(this->targetYaw) * this->radius);
    this->actor.world.pos.z = OTHER_HAND(this)->center.z - (Math_CosS(this->targetYaw) * this->radius);
    this->actor.world.pos.y = OTHER_HAND(this)->center.y + (this->radius * 0.4f);
    if (this->timer == 0) {
        OTHER_HAND(this)->ready = true;
        BossSst_HandSetupRetreat(this);
    }

    func_8002F974(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
}

void BossSst_HandGrabPlayer(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->grabPlayer(play, player)) {
        player->actor.parent = &this->actor;
        if (player->actor.colChkInfo.health > 0) {
            this->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
            if (HAND_STATE(this) == HAND_CLAP) {
                OTHER_HAND(this)->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
            }
        }
    }
}

void BossSst_HandReleasePlayer(BossSst* this, PlayState* play, s32 dropPlayer) {
    Player* player = GET_PLAYER(play);

    if (player->actor.parent == &this->actor) {
        player->actor.parent = NULL;
        player->unk_850 = 100;
        this->colliderJntSph.base.ocFlags1 |= OC1_ON;
        OTHER_HAND(this)->colliderJntSph.base.ocFlags1 |= OC1_ON;
        if (dropPlayer) {
            func_8002F71C(play, &this->actor, 0.0f, this->actor.shape.rot.y, 0.0f);
        }
    }
}

void BossSst_MoveAround(BossSst* this) {
    BossSst* hand;
    Vec3f* vec;
    f32 sn;
    f32 cs;
    s32 i;

    sn = Math_SinS(this->actor.shape.rot.y);
    cs = Math_CosS(this->actor.shape.rot.y);
    if (this->actionFunc != BossSst_HeadEndCharge) {
        this->actor.world.pos.x = sRoomCenter.x + (this->radius * sn);
        this->actor.world.pos.z = sRoomCenter.z + (this->radius * cs);
    }

    for (i = 0; i < 2; i++) {
        hand = sHands[i];
        vec = &sHandOffsets[i];

        hand->actor.world.pos.x = this->actor.world.pos.x + (vec->z * sn) + (vec->x * cs);
        hand->actor.world.pos.y = this->actor.world.pos.y + vec->y;
        hand->actor.world.pos.z = this->actor.world.pos.z + (vec->z * cs) - (vec->x * sn);

        hand->actor.home.pos.x = this->actor.world.pos.x + (400.0f * sn) + (-200.0f * hand->vParity * cs);
        hand->actor.home.pos.y = this->actor.world.pos.y;
        hand->actor.home.pos.z = this->actor.world.pos.z + (400.0f * cs) - (-200.0f * hand->vParity * sn);

        hand->actor.home.rot.y = this->actor.shape.rot.y;
        hand->actor.shape.rot.y = sHandYawOffsets[i] + this->actor.shape.rot.y;

        if (hand->actor.world.pos.y < hand->actor.floorHeight) {
            hand->actor.world.pos.y = hand->actor.floorHeight;
        }
    }
}

void BossSst_HandSelectAttack(BossSst* this) {
    f32 rand = Rand_ZeroOne() * 6.0f;
    s32 randInt;

    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        rand *= 5.0f / 6;
        if (rand > 4.0f) {
            rand = 4.0f;
        }
    }

    randInt = rand;
    if (randInt == 0) {
        BossSst_HandSetupReadySlam(this);
    } else if (randInt == 1) {
        BossSst_HandSetupReadySweep(this);
    } else if (randInt == 2) {
        BossSst_HandSetupReadyPunch(this);
    } else if (randInt == 5) {
        BossSst_HandSetupReadyClap(this);
    } else { // randInt == 3 || randInt == 4
        BossSst_HandSetupReadyGrab(this);
    }
}

void BossSst_HandSetDamage(BossSst* this, s32 damage) {
    s32 i;

    this->colliderJntSph.base.atFlags |= AT_ON;
    for (i = 0; i < 11; i++) {
        this->colliderJntSph.elements[i].info.toucher.damage = damage;
    }
}

void BossSst_HandSetInvulnerable(BossSst* this, s32 isInv) {
    this->colliderJntSph.base.acFlags &= ~AC_HIT;
    if (isInv) {
        this->colliderJntSph.base.colType = COLTYPE_HARD;
        this->colliderJntSph.base.acFlags |= AC_HARD;
    } else {
        this->colliderJntSph.base.colType = COLTYPE_HIT0;
        this->colliderJntSph.base.acFlags &= ~AC_HARD;
    }
}

void BossSst_HeadSfx(BossSst* this, u16 sfxId) {
    func_80078914(&this->center, sfxId);
}

void BossSst_HandCollisionCheck(BossSst* this, PlayState* play) {
    if ((this->colliderJntSph.base.acFlags & AC_HIT) && (this->colliderJntSph.base.colType != COLTYPE_HARD)) {
        s32 bothHands = true;

        this->colliderJntSph.base.acFlags &= ~AC_HIT;
        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            this->colliderJntSph.base.acFlags &= ~AC_ON;
            this->colliderJntSph.base.ocFlags1 &= ~OC1_NO_PUSH;
            BossSst_HandReleasePlayer(this, play, true);
            if (HAND_STATE(OTHER_HAND(this)) == HAND_CLAP) {
                BossSst_HandReleasePlayer(OTHER_HAND(this), play, true);
                BossSst_HandSetupRetreat(OTHER_HAND(this));
            }

            this->actor.flags &= ~ACTOR_FLAG_0;
            if (this->actor.colChkInfo.damageEffect == 3) {
                BossSst_HandSetupFrozen(this);
            } else {
                BossSst_HandSetupReel(this);
                if (HAND_STATE(OTHER_HAND(this)) != HAND_DAMAGED) {
                    bothHands = false;
                }
            }

            BossSst_HeadSetupDamagedHand(sHead, bothHands);
            Item_DropCollectible(play, &this->actor.world.pos,
                                 (Rand_ZeroOne() < 0.5f) ? ITEM00_ARROWS_SMALL : ITEM00_MAGIC_SMALL);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_DAMAGE_HAND);
        }
    }
}

void BossSst_HeadCollisionCheck(BossSst* this, PlayState* play) {
    if (this->colliderCyl.base.acFlags & AC_HIT) {
        this->colliderCyl.base.acFlags &= ~AC_HIT;
        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            if (this->actionFunc == BossSst_HeadVulnerable) {
                if (Actor_ApplyDamage(&this->actor) == 0) {
                    Enemy_StartFinishingBlow(play, &this->actor);
                    BossSst_HeadSetupDeath(this, play);
                    gSaveContext.sohStats.timestamp[TIMESTAMP_DEFEAT_BONGO_BONGO] = GAMEPLAYSTAT_TOTAL_TIME;
                } else {
                    BossSst_HeadSetupDamage(this);
                }

                BossSst_HandSetupDamage(sHands[LEFT]);
                BossSst_HandSetupDamage(sHands[RIGHT]);
            } else {
                BossSst_HeadSetupStunned(this);
                if (HAND_STATE(sHands[RIGHT]) == HAND_FROZEN) {
                    BossSst_IceShatter(sHands[RIGHT]);
                } else if (HAND_STATE(sHands[LEFT]) == HAND_FROZEN) {
                    BossSst_IceShatter(sHands[LEFT]);
                }

                BossSst_HandSetupStunned(sHands[RIGHT]);
                BossSst_HandSetupStunned(sHands[LEFT]);
            }
        }
    }
}

void BossSst_UpdateHand(Actor* thisx, PlayState* play) {
    s32 pad;
    BossSst* this = (BossSst*)thisx;
    BossSstHandTrail* trail;

    if (this->colliderCyl.base.atFlags & AT_ON) {
        if ((this->effects[0].move < 5) ||
            (this->actor.xzDistToPlayer < ((this->effects[2].scale * 0.01f) * sCylinderInitHand.dim.radius)) ||
            (this->colliderCyl.base.atFlags & AT_HIT)) {
            this->colliderCyl.base.atFlags &= ~(AT_ON | AT_HIT);
        } else {
            this->colliderCyl.dim.radius = (this->effects[0].scale * 0.01f) * sCylinderInitHand.dim.radius;
        }
    }

    BossSst_HandCollisionCheck(this, play);
    this->actionFunc(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 130.0f, 0.0f, 5);
    Actor_SetFocus(&this->actor, 0.0f);
    if (this->colliderJntSph.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderJntSph.base);
    }

    if ((sHead->actionFunc != BossSst_HeadLurk) && (sHead->actionFunc != BossSst_HeadIntro) &&
        (this->colliderJntSph.base.acFlags & AC_ON)) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderJntSph.base);
    }

    if (this->colliderJntSph.base.ocFlags1 & OC1_ON) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderJntSph.base);
    }

    if (this->colliderCyl.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderCyl.base);
    }

    if ((HAND_STATE(this) != HAND_DEATH) && (HAND_STATE(this) != HAND_WAIT) && (HAND_STATE(this) != HAND_BEAT) &&
        (HAND_STATE(this) != HAND_FROZEN)) {
        this->trailCount++;
        this->trailCount = CLAMP_MAX(this->trailCount, 7);
    } else {
        this->trailCount--;
        this->trailCount = CLAMP_MIN(this->trailCount, 0);
    }

    trail = &this->handTrails[this->trailIndex];
    Math_Vec3f_Copy(&trail->world.pos, &this->actor.world.pos);
    trail->world.rot = this->actor.shape.rot;
    trail->zPosMod = this->handZPosMod;
    trail->yRotMod = this->handYRotMod;

    this->trailIndex = (this->trailIndex + 1) % 7;
    BossSst_UpdateEffect(&this->actor, play);
}

void BossSst_UpdateHead(Actor* thisx, PlayState* play) {
    s32 pad;
    BossSst* this = (BossSst*)thisx;

    func_8002DBD0(&this->actor, &sHandOffsets[RIGHT], &sHands[RIGHT]->actor.world.pos);
    func_8002DBD0(&this->actor, &sHandOffsets[LEFT], &sHands[LEFT]->actor.world.pos);

    sHandYawOffsets[LEFT] = sHands[LEFT]->actor.shape.rot.y - thisx->shape.rot.y;
    sHandYawOffsets[RIGHT] = sHands[RIGHT]->actor.shape.rot.y - thisx->shape.rot.y;

    BossSst_HeadCollisionCheck(this, play);
    this->actionFunc(this, play);
    if (this->vVanish) {
        if (!play->actorCtx.lensActive || (thisx->colorFilterTimer != 0)) {
            this->actor.flags &= ~ACTOR_FLAG_7;
        } else {
            this->actor.flags |= ACTOR_FLAG_7;
        }
    }

    if (this->colliderJntSph.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderJntSph.base);
    }

    if ((this->actionFunc != BossSst_HeadLurk) && (this->actionFunc != BossSst_HeadIntro)) {
        if (this->colliderCyl.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderCyl.base);
        }
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderJntSph.base);
    }

    if (this->colliderJntSph.base.ocFlags1 & OC1_ON) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderJntSph.base);
    }

    BossSst_MoveAround(this);
    if ((!this->vVanish || CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_7)) &&
        ((this->actionFunc == BossSst_HeadReadyCharge) || (this->actionFunc == BossSst_HeadCharge) ||
         (this->actionFunc == BossSst_HeadFrozenHand) || (this->actionFunc == BossSst_HeadStunned) ||
         (this->actionFunc == BossSst_HeadVulnerable) || (this->actionFunc == BossSst_HeadDamage))) {
        this->actor.flags |= ACTOR_FLAG_0;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_0;
    }

    if (this->actionFunc == BossSst_HeadCharge) {
        BossSst_HeadSfx(this, NA_SE_EN_SHADEST_MOVE - SFX_FLAG);
    }

    BossSst_UpdateEffect(&this->actor, play);
}

s32 BossSst_OverrideHandDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                             void* thisx) {
    BossSst* this = (BossSst*)thisx;

    if (limbIndex == 1) {
        pos->z += this->handZPosMod;
        rot->y += this->handYRotMod;
    }
    return false;
}

void BossSst_PostHandDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    BossSst* this = (BossSst*)thisx;

    Collider_UpdateSpheres(limbIndex, &this->colliderJntSph);
}

s32 BossSst_OverrideHandTrailDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* data, Gfx** gfx) {
    BossSstHandTrail* trail = (BossSstHandTrail*)data;

    if (limbIndex == 1) {
        pos->z += trail->zPosMod;
        rot->y += trail->yRotMod;
    }
    return false;
}

void BossSst_DrawHand(Actor* thisx, PlayState* play) {
    BossSst* this = (BossSst*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x80, sBodyColor.r, sBodyColor.g, sBodyColor.b, 255);

    if (!sBodyStatic) {
        gSPSegment(POLY_OPA_DISP++, 0x08, &D_80116280[2]);
    } else {
        gDPSetEnvColor(POLY_OPA_DISP++, sStaticColor.r, sStaticColor.g, sStaticColor.b, 0);
        gSPSegment(POLY_OPA_DISP++, 0x08, sBodyStaticDList);
    }

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          BossSst_OverrideHandDraw, BossSst_PostHandDraw, this);
    if (this->trailCount >= 2) {
        BossSstHandTrail* trail;
        BossSstHandTrail* trail2;
        s32 i;
        s32 idx;
        s32 end;
        s32 pad;

        Gfx_SetupDL_25Xlu(play->state.gfxCtx);

        end = this->trailCount >> 1;
        idx = (this->trailIndex + 4) % 7;
        trail = &this->handTrails[idx];
        trail2 = &this->handTrails[(idx + 2) % 7];

        for (i = 0; i < end; i++) {
            if (Math3D_Vec3fDistSq(&trail2->world.pos, &trail->world.pos) > 900.0f) {
                FrameInterpolation_RecordOpenChild(trail, i);

                Matrix_SetTranslateRotateYXZ(trail->world.pos.x, trail->world.pos.y, trail->world.pos.z,
                                             &trail->world.rot);
                Matrix_Scale(0.02f, 0.02f, 0.02f, MTXMODE_APPLY);

                gSPSegment(POLY_XLU_DISP++, 0x08, sHandTrailDList);
                gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, ((3 - i) * 10) + 20, 0, ((3 - i) * 20) + 50,
                                ((3 - i) * 30) + 70);

                POLY_XLU_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                                   this->skelAnime.dListCount, BossSst_OverrideHandTrailDraw, NULL,
                                                   trail, POLY_XLU_DISP);
                
                FrameInterpolation_RecordCloseChild();
            }
            idx = (idx + 5) % 7;
            trail2 = trail;
            trail = &this->handTrails[idx];
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);

    BossSst_DrawEffect(&this->actor, play);
}

s32 BossSst_OverrideHeadDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                             Gfx** gfx) {
    BossSst* this = (BossSst*)thisx;
    s32 shakeAmp;
    s32 pad;
    s32 timer12;
    f32 shakeMod;

    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_7) && this->vVanish) {
        *dList = NULL;
    } else if (this->actionFunc == BossSst_HeadThrash) { // Animation modifications for death cutscene
        shakeAmp = (this->timer / 10) + 1;
        if ((limbIndex == 3) || (limbIndex == 39) || (limbIndex == 42)) {

            shakeMod = sinf(this->timer * (M_PI / 5));
            rot->x += ((0x500 * Rand_ZeroOne() + 0xA00) / 0x10) * shakeAmp * shakeMod;

            shakeMod = sinf((this->timer % 5) * (M_PI / 5));
            rot->z -= ((0x800 * Rand_ZeroOne() + 0x1000) / 0x10) * shakeAmp * shakeMod + 0x1000;

            if (limbIndex == 3) {

                shakeMod = sinf(this->timer * (M_PI / 5));
                rot->y += ((0x500 * Rand_ZeroOne() + 0xA00) / 0x10) * shakeAmp * shakeMod;
            }
        } else if ((limbIndex == 5) || (limbIndex == 6)) {

            shakeMod = sinf((this->timer % 5) * (M_PI / 5));
            rot->z -= ((0x280 * Rand_ZeroOne() + 0x500) / 0x10) * shakeAmp * shakeMod + 0x500;

            if (limbIndex == 5) {

                shakeMod = sinf(this->timer * (M_PI / 5));
                rot->x += ((0x500 * Rand_ZeroOne() + 0xA00) / 0x10) * shakeAmp * shakeMod;

                shakeMod = sinf(this->timer * (M_PI / 5));
                rot->y += ((0x500 * Rand_ZeroOne() + 0xA00) / 0x10) * shakeAmp * shakeMod;
            }
        } else if (limbIndex == 2) {
            shakeMod = sinf(this->timer * (M_PI / 5));
            rot->x += ((0x200 * Rand_ZeroOne() + 0x400) / 0x10) * shakeAmp * shakeMod;

            shakeMod = sinf(this->timer * (M_PI / 5));
            rot->y += ((0x200 * Rand_ZeroOne() + 0x400) / 0x10) * shakeAmp * shakeMod;

            shakeMod = sinf((this->timer % 5) * (M_PI / 5));
            rot->z -= ((0x100 * Rand_ZeroOne() + 0x200) / 0x10) * shakeAmp * shakeMod + 0x200;
        }
    } else if (this->actionFunc == BossSst_HeadDeath) {
        if (this->timer > 48) {
            timer12 = this->timer - 36;
        } else {
            pad = ((this->timer > 6) ? 6 : this->timer);
            timer12 = pad * 2;
        }

        if ((limbIndex == 3) || (limbIndex == 39) || (limbIndex == 42)) {
            rot->z -= 0x2000 * sinf(timer12 * (M_PI / 24));
        } else if ((limbIndex == 5) || (limbIndex == 6)) {
            rot->z -= 0xA00 * sinf(timer12 * (M_PI / 24));
        } else if (limbIndex == 2) {
            rot->z -= 0x400 * sinf(timer12 * (M_PI / 24));
        }
    } else if ((this->actionFunc == BossSst_HeadDarken) || (this->actionFunc == BossSst_HeadFall) ||
               (this->actionFunc == BossSst_HeadMelt)) {
        if ((limbIndex == 3) || (limbIndex == 39) || (limbIndex == 42)) {
            rot->z -= 0x1000;
        } else if ((limbIndex == 5) || (limbIndex == 6)) {
            rot->z -= 0x500;
        } else if (limbIndex == 2) {
            rot->z -= 0x200;
        }
    }
    return false;
}

void BossSst_PostHeadDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    static Vec3f headVec = { 1000.0f, 0.0f, 0.0f };
    BossSst* this = (BossSst*)thisx;
    Vec3f headPos;

    if (limbIndex == 8) {
        Matrix_MultVec3f(&zeroVec, &this->actor.focus.pos);
        Matrix_MultVec3f(&headVec, &headPos);
        this->colliderCyl.dim.pos.x = headPos.x;
        this->colliderCyl.dim.pos.y = headPos.y;
        this->colliderCyl.dim.pos.z = headPos.z;
    }

    Collider_UpdateSpheres(limbIndex, &this->colliderJntSph);
}

void BossSst_DrawHead(Actor* thisx, PlayState* play) {
    s32 pad;
    BossSst* this = (BossSst*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_7)) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x80, sBodyColor.r, sBodyColor.g, sBodyColor.b, 255);
        if (!sBodyStatic) {
            gSPSegment(POLY_OPA_DISP++, 0x08, &D_80116280[2]);
        } else {
            gDPSetEnvColor(POLY_OPA_DISP++, sStaticColor.r, sStaticColor.g, sStaticColor.b, 0);
            gSPSegment(POLY_OPA_DISP++, 0x08, sBodyStaticDList);
        }
    } else {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 255, 255, 255);
        gSPSegment(POLY_XLU_DISP++, 0x08, &D_80116280[2]);
    }

    if (this->actionFunc == BossSst_HeadThrash) {
        f32 randPitch = Rand_ZeroOne() * (2 * M_PI);
        f32 randYaw = Rand_ZeroOne() * (2 * M_PI);

        Matrix_RotateY(randYaw, MTXMODE_APPLY);
        Matrix_RotateX(randPitch, MTXMODE_APPLY);
        Matrix_Scale((this->timer * 0.000375f) + 1.0f, 1.0f - (this->timer * 0.00075f),
                     (this->timer * 0.000375f) + 1.0f, MTXMODE_APPLY);
        Matrix_RotateX(-randPitch, MTXMODE_APPLY);
        Matrix_RotateY(-randYaw, MTXMODE_APPLY);
    }

    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_7)) {
        POLY_OPA_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, BossSst_OverrideHeadDraw, BossSst_PostHeadDraw,
                                           this, POLY_OPA_DISP);
    } else {
        POLY_XLU_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, BossSst_OverrideHeadDraw, BossSst_PostHeadDraw,
                                           this, POLY_XLU_DISP);
    }

    if ((this->actionFunc == BossSst_HeadIntro) && (113 >= this->timer) && (this->timer > 20)) {
        s32 yOffset;
        Vec3f vanishMaskPos;
        Vec3f vanishMaskOffset;

        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, 0, 0, 18, 255);

        yOffset = 113 * 8 - this->timer * 8;
        vanishMaskPos.x = ROOM_CENTER_X + 85.0f;
        vanishMaskPos.y = ROOM_CENTER_Y - 250.0f + yOffset;
        vanishMaskPos.z = ROOM_CENTER_Z + 190.0f;
        if (vanishMaskPos.y > 450.0f) {
            vanishMaskPos.y = 450.0f;
        }

        Matrix_MultVec3fExt(&vanishMaskPos, &vanishMaskOffset, &play->billboardMtxF);
        Matrix_Translate(this->actor.world.pos.x + vanishMaskOffset.x, this->actor.world.pos.y + vanishMaskOffset.y,
                         this->actor.world.pos.z + vanishMaskOffset.z, MTXMODE_NEW);
        Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, sIntroVanishDList);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &this->actor.focus.pos, &this->center);
    BossSst_DrawEffect(&this->actor, play);
}

void BossSst_SpawnHeadShadow(BossSst* this) {
    static Vec3f shadowOffset[] = {
        { 0.0f, 0.0f, 340.0f },
        { -160.0f, 0.0f, 250.0f },
        { 160.0f, 0.0f, 250.0f },
    };
    s32 pad;
    s32 i;
    f32 sn;
    f32 cs;

    this->effectMode = BONGO_SHADOW;
    sn = Math_SinS(this->actor.shape.rot.y);
    cs = Math_CosS(this->actor.shape.rot.y);

    for (i = 0; i < 3; i++) {
        BossSstEffect* shadow = &this->effects[i];
        Vec3f* offset = &shadowOffset[i];

        shadow->pos.x = this->actor.world.pos.x + (sn * offset->z) + (cs * offset->x);
        shadow->pos.y = 0.0f;
        shadow->pos.z = this->actor.world.pos.z + (cs * offset->z) - (sn * offset->x);

        shadow->scale = 1450;
        shadow->alpha = 254;
        shadow->status = 65;

        shadow->epoch++;
    }

    this->effects[3].status = -1;
}

void BossSst_SpawnHandShadow(BossSst* this) {
    this->effectMode = BONGO_SHADOW;
    this->effects[0].pos.x = this->actor.world.pos.x + (Math_CosS(this->actor.shape.rot.y) * 30.0f * this->vParity);
    this->effects[0].pos.z = this->actor.world.pos.z - (Math_SinS(this->actor.shape.rot.y) * 30.0f * this->vParity);
    this->effects[0].pos.y = this->actor.world.pos.y;
    this->effects[0].scale = 2300;
    this->effects[0].alpha = 254;
    this->effects[0].status = 5;
    this->effects[0].epoch++;
    this->effects[1].status = -1;
}

void BossSst_SpawnShockwave(BossSst* this) {
    s32 i;
    s32 scale = 120;
    s32 alpha = 250;

    Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHADEST_HAND_WAVE);
    this->effectMode = BONGO_SHOCKWAVE;

    for (i = 0; i < 3; i++) {
        BossSstEffect* shockwave = &this->effects[i];
        shockwave->epoch++;

        Math_Vec3f_Copy(&shockwave->pos, &this->actor.world.pos);
        shockwave->move = (i + 9) * 2;
        shockwave->scale = scale;
        shockwave->alpha = alpha / shockwave->move;
        scale -= 50;
        alpha -= 25;
    }
}

void BossSst_SpawnIceCrystal(BossSst* this, s32 index) {
    BossSstEffect* ice = &this->effects[index];
    Sphere16* sphere;

    if (index < 11) {
        sphere = &this->colliderJntSph.elements[index].dim.worldSphere;

        ice->pos.x = sphere->center.x;
        ice->pos.y = sphere->center.y;
        ice->pos.z = sphere->center.z;
        if (index == 0) {
            ice->pos.x -= 25.0f;
            ice->pos.y -= 25.0f;
            ice->pos.z -= 25.0f;
        }
    } else {
        sphere = &this->colliderJntSph.elements[0].dim.worldSphere;

        ice->pos.x = ((((index - 11) & 1) ? 1 : -1) * 25.0f) + sphere->center.x;
        ice->pos.y = ((((index - 11) & 2) ? 1 : -1) * 25.0f) + sphere->center.y;
        ice->pos.z = ((((index - 11) & 4) ? 1 : -1) * 25.0f) + sphere->center.z;
    }

    ice->pos.x -= this->actor.world.pos.x;
    ice->pos.y -= this->actor.world.pos.y;
    ice->pos.z -= this->actor.world.pos.z;

    ice->status = 0;

    ice->rot.x = Rand_ZeroOne() * 0x10000;
    ice->rot.y = Rand_ZeroOne() * 0x10000;
    ice->rot.z = Rand_ZeroOne() * 0x10000;

    ice->alpha = 120;
    ice->move = true;

    ice->vel.x = (Rand_ZeroOne() * 0.06f + 0.12f) * ice->pos.x;
    ice->vel.y = (Rand_ZeroOne() * 15.0f + 5.0f);
    ice->vel.z = (Rand_ZeroOne() * 0.06f + 0.12f) * ice->pos.z;
    ice->scale = 4000;

    if ((index % 2) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_PL_FREEZE_S);
    }

    ice->epoch++;
}

void BossSst_SpawnIceShard(BossSst* this) {
    s32 i;
    Vec3f spawnPos;
    f32 offXZ;

    this->effectMode = BONGO_ICE;
    offXZ = Math_CosS(this->actor.shape.rot.x) * 50.0f;
    spawnPos.x = Math_CosS(this->actor.shape.rot.y) * offXZ + this->actor.world.pos.x;
    spawnPos.y = Math_SinS(this->actor.shape.rot.x) * 50.0f + this->actor.world.pos.y - 10.0f;
    spawnPos.z = Math_SinS(this->actor.shape.rot.y) * offXZ + this->actor.world.pos.z;

    for (i = 0; i < 18; i++) {
        BossSstEffect* ice = &this->effects[i];
        ice->epoch++;

        Math_Vec3f_Copy(&ice->pos, &spawnPos);
        ice->status = 1;
        ice->rot.x = Rand_ZeroOne() * 0x10000;
        ice->rot.y = Rand_ZeroOne() * 0x10000;
        ice->rot.z = Rand_ZeroOne() * 0x10000;

        ice->alpha = 120;
        ice->move = true;

        ice->vel.x = Rand_CenteredFloat(20.0f);
        ice->vel.y = Rand_ZeroOne() * 10.0f + 3.0f;
        ice->vel.z = Rand_CenteredFloat(20.0f);

        ice->scale = Rand_ZeroOne() * 200.0f + 400.0f;
    }
}

void BossSst_IceShatter(BossSst* this) {
    s32 i;

    this->effects[0].status = 1;
    Audio_PlayActorSound2(&this->actor, NA_SE_PL_ICE_BROKEN);

    for (i = 0; i < 18; i++) {
        BossSstEffect* ice = &this->effects[i];

        if (ice->move) {
            ice->pos.x += this->actor.world.pos.x;
            ice->pos.y += this->actor.world.pos.y;
            ice->pos.z += this->actor.world.pos.z;
        }
    }
}

void BossSst_UpdateEffect(Actor* thisx, PlayState* play) {
    BossSst* this = (BossSst*)thisx;
    BossSstEffect* effect;
    s32 i;

    if (this->effectMode != BONGO_NULL) {
        if (this->effectMode == BONGO_ICE) {
            if (this->effects[0].status) {
                for (i = 0; i < 18; i++) {
                    effect = &this->effects[i];

                    if (effect->move) {
                        effect->pos.x += effect->vel.x;
                        effect->pos.y += effect->vel.y;
                        effect->pos.z += effect->vel.z;
                        effect->alpha -= 3;
                        effect->vel.y -= 1.0f;
                        effect->rot.x += 0xD00;
                        effect->rot.y += 0x1100;
                        effect->rot.z += 0x1500;
                    }
                }
            }
            if (this->effects[0].alpha == 0) {
                this->effectMode = BONGO_NULL;
            }
        } else if (this->effectMode == BONGO_SHOCKWAVE) {
            for (i = 0; i < 3; i++) {
                BossSstEffect* effect2 = &this->effects[i];
                s32 scale = effect2->move * 2;

                effect2->scale += CLAMP_MAX(scale, 20) + i;
                if (effect2->move != 0) {
                    effect2->move--;
                }
            }

            if (this->effects[0].move == 0) {
                this->effectMode = BONGO_NULL;
            }
        } else if (this->effectMode == BONGO_SHADOW) {
            effect = &this->effects[0];

            if (this->actor.params == BONGO_HEAD) {
                SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &this->actor.focus.pos, &this->center);
                BossSst_HeadSfx(this, NA_SE_EN_SHADEST_LAST - SFX_FLAG);
            }
            while (effect->status != -1) {
                if (effect->status == 0) {
                    effect->alpha -= 2;
                } else {
                    effect->scale += effect->status;
                }

                effect->scale = CLAMP_MAX(effect->scale, 10000);
                effect++;
            }
        }
    }
}

void BossSst_DrawEffect(Actor* thisx, PlayState* play) {
    s32 pad;
    BossSst* this = (BossSst*)thisx;
    s32 i;
    BossSstEffect* effect;

    if (this->effectMode != BONGO_NULL) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        if (this->effectMode == BONGO_ICE) {
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, play->gameplayFrames % 256, 0x20, 0x10, 1,
                                        0, (play->gameplayFrames * 2) % 256, 0x40, 0x20));
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, this->effects[0].alpha);
            gSPDisplayList(POLY_XLU_DISP++, gBongoIceCrystalDL);

            for (i = 0; i < 18; i++) {
                effect = &this->effects[i];
                FrameInterpolation_RecordOpenChild(effect, effect->epoch);

                if (effect->move) {
                    func_8003435C(&effect->pos, play);
                    if (this->effects[0].status != 0) {
                        Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
                    } else {
                        Matrix_Translate(effect->pos.x + this->actor.world.pos.x,
                                         effect->pos.y + this->actor.world.pos.y,
                                         effect->pos.z + this->actor.world.pos.z, MTXMODE_NEW);
                    }

                    Matrix_RotateZYX(effect->rot.x, effect->rot.y, effect->rot.z, MTXMODE_APPLY);
                    Matrix_Scale(effect->scale * 0.001f, effect->scale * 0.001f, effect->scale * 0.001f, MTXMODE_APPLY);

                    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPDisplayList(POLY_XLU_DISP++, gBongoIceShardDL);
                }

                FrameInterpolation_RecordCloseChild();
            }
        } else if (this->effectMode == BONGO_SHOCKWAVE) {
            f32 scaleY = 0.005f;

            gDPPipeSync(POLY_XLU_DISP++);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, play->gameplayFrames % 128, 0, 0x20, 0x40, 1,
                                        0, (play->gameplayFrames * -15) % 256, 0x20, 0x40));

            for (i = 0; i < 3; i++, scaleY -= 0.001f) {
                effect = &this->effects[i];
                FrameInterpolation_RecordOpenChild(effect, effect->epoch);

                if (effect->move != 0) {
                    Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
                    Matrix_Scale(effect->scale * 0.001f, scaleY, effect->scale * 0.001f, MTXMODE_APPLY);

                    gDPPipeSync(POLY_XLU_DISP++);
                    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 30, 0, 30, effect->alpha * effect->move);
                    gDPSetEnvColor(POLY_XLU_DISP++, 30, 0, 30, 0);
                    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPDisplayList(POLY_XLU_DISP++, gEffFireCircleDL);
                }

                FrameInterpolation_RecordCloseChild();
            }
        } else if (this->effectMode == BONGO_SHADOW) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 10, 10, 80, 0);
            gDPSetEnvColor(POLY_XLU_DISP++, 10, 10, 10, this->effects[0].alpha);

            effect = &this->effects[0];
            while (effect->status != -1) {
                FrameInterpolation_RecordOpenChild(effect, effect->epoch);

                Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
                Matrix_Scale(effect->scale * 0.001f, 1.0f, effect->scale * 0.001f, MTXMODE_APPLY);

                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, sShadowDList);
                
                FrameInterpolation_RecordCloseChild();
                effect++;
            }
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void BossSst_Reset(void) {
    sHead = NULL;
    sHands[0] = NULL;
    sHands[1] = NULL;
    sFloor = NULL;
    sHandOffsets[0].x = 0.0f;
    sHandOffsets[0].y = 0.0f;
    sHandOffsets[0].z = 0.0f;
    
    sHandOffsets[1].x = 0.0f;
    sHandOffsets[1].y = 0.0f;
    sHandOffsets[1].z = 0.0f;

    sHandYawOffsets[0] = 0;
    sHandYawOffsets[1] = 0;

    sCutsceneCamera= 0;
    sBodyStatic = false;
}
