#include "z_en_skj.h"
#include "overlays/actors/ovl_En_Skjneedle/z_en_skjneedle.h"
#include "objects/object_skj/object_skj.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_NO_FREEZE_OCARINA)

void EnSkj_Init(Actor* thisx, PlayState* play);
void EnSkj_Destroy(Actor* thisx, PlayState* play);
void EnSkj_Update(Actor* thisx, PlayState* play);
void EnSkj_Draw(Actor* thisx, PlayState* play);

void EnSkj_SariasSongShortStumpUpdate(Actor* thisx, PlayState* play);
void EnSkj_OcarinaMinigameShortStumpUpdate(Actor* thisx, PlayState* play);

void func_80AFF2A0(EnSkj* this);
void func_80AFF334(EnSkj* this);

void EnSkj_CalculateCenter(EnSkj* this);
void EnSkj_OcarinaGameSetupWaitForPlayer(EnSkj* this);
void EnSkj_SetupResetFight(EnSkj* this);
void EnSkj_SetupLeaveOcarinaGame(EnSkj* this);
void EnSkj_SetupPlayOcarinaGame(EnSkj* this);
void EnSkj_Backflip(EnSkj* this);
void EnSkj_SetupNeedleRecover(EnSkj* this);
void EnSkj_SetupSpawnDeathEffect(EnSkj* this);
void EnSkj_SetupStand(EnSkj* this);
void EnSkj_SetupWaitForSong(EnSkj* this);
void EnSkj_SetupTalk(EnSkj* this);
void EnSkj_SetupMaskTrade(EnSkj* this);
void EnSkj_SetupWrongSong(EnSkj* this);
void EnSkj_SetupAfterSong(EnSkj* this);
void func_80AFFE24(EnSkj* this);
void EnSkj_SetupPostSariasSong(EnSkj* this);
void EnSkj_JumpFromStump(EnSkj* this);
void EnSkj_SetupWaitForLandAnimFinish(EnSkj* this);
void EnSkj_SetupWalkToPlayer(EnSkj* this);
void EnSkj_SetupWaitForMaskTextClear(EnSkj* this);
void EnSkj_SetupWaitForTextClear(EnSkj* this);
void EnSkj_SetupDie(EnSkj* this);
void func_80AFF1F0(EnSkj* this);
void EnSkj_OfferNextRound(EnSkj* this, PlayState* play);
void EnSkj_SetupAskForMask(EnSkj* this, PlayState* play);
f32 EnSkj_GetItemXzRange(EnSkj* this);
s32 EnSkj_CollisionCheck(EnSkj* this, PlayState* play);
void EnSkj_SetupTakeMask(EnSkj* this, PlayState* play);
void EnSkj_TurnPlayer(EnSkj* this, Player* player);

void EnSkj_SetupWaitForOcarina(EnSkj* this, PlayState* play);
void EnSkj_StartOcarinaMinigame(EnSkj* this, PlayState* play);
void EnSkj_WaitForOcarina(EnSkj* this, PlayState* play);
void EnSkj_WaitForPlayback(EnSkj* this, PlayState* play);
void EnSkj_FailedMiniGame(EnSkj* this, PlayState* play);
void EnSkj_WonOcarinaMiniGame(EnSkj* this, PlayState* play);
void EnSkj_WaitToGiveReward(EnSkj* this, PlayState* play);
void EnSkj_GiveOcarinaGameReward(EnSkj* this, PlayState* play);
void EnSkj_FinishOcarinaGameRound(EnSkj* this, PlayState* play);
void EnSkj_WaitForNextRound(EnSkj* this, PlayState* play);
void EnSkj_WaitForOfferResponse(EnSkj* this, PlayState* play);
void EnSkj_SetupWaitForOcarina(EnSkj* this, PlayState* play);
void EnSkj_CleanupOcarinaGame(EnSkj* this, PlayState* play);

void EnSkj_Fade(EnSkj* this, PlayState* play);
void EnSkj_WaitToShootNeedle(EnSkj* this, PlayState* play);
void EnSkj_SariasSongKidIdle(EnSkj* this, PlayState* play);
void EnSkj_WaitForDeathAnim(EnSkj* this, PlayState* play);
void EnSkj_PickNextFightAction(EnSkj* this, PlayState* play);
void EnSkj_WaitForLandAnim(EnSkj* this, PlayState* play);
void EnSkj_ResetFight(EnSkj* this, PlayState* play);
void EnSkj_Fight(EnSkj* this, PlayState* play);
void EnSkj_NeedleRecover(EnSkj* this, PlayState* play);
void EnSkj_SpawnDeathEffect(EnSkj* this, PlayState* play);
void EnSkj_WaitInRange(EnSkj* this, PlayState* play);
void EnSkj_WaitForSong(EnSkj* this, PlayState* play);
void EnSkj_AfterSong(EnSkj* this, PlayState* play);
void EnSkj_SariaSongTalk(EnSkj* this, PlayState* play);
void func_80AFFE44(EnSkj* this, PlayState* play);
void EnSkj_ChangeModeAfterSong(EnSkj* this, PlayState* play);
void EnSkj_StartMaskTrade(EnSkj* this, PlayState* play);
void EnSkj_WaitForLanding(EnSkj* this, PlayState* play);
void EnSkj_WaitForLandAnimFinish(EnSkj* this, PlayState* play);
void EnSkj_WalkToPlayer(EnSkj* this, PlayState* play);
void EnSkj_AskForMask(EnSkj* this, PlayState* play);
void EnSkj_TakeMask(EnSkj* this, PlayState* play);
void EnSkj_WaitForMaskTextClear(EnSkj* this, PlayState* play);
void EnSkj_WrongSong(EnSkj* this, PlayState* play);
void EnSkj_SariasSongWaitForTextClear(EnSkj* this, PlayState* play);
void EnSkj_OcarinaGameWaitForPlayer(EnSkj* this, PlayState* play);
void EnSkj_OcarinaGameIdle(EnSkj* this, PlayState* play);
void EnSkj_PlayOcarinaGame(EnSkj* this, PlayState* play);
void EnSkj_LeaveOcarinaGame(EnSkj* this, PlayState* play);

void EnSkj_SpawnBlood(PlayState* play, Vec3f* pos);

void EnSkj_SetupWaitInRange(EnSkj* this);

#define songFailTimer multiuseTimer
#define battleExitTimer multiuseTimer

typedef enum {
    /* 0 */ SKJ_ANIM_BACKFLIP,
    /* 1 */ SKJ_ANIM_SHOOT_NEEDLE,
    /* 2 */ SKJ_ANIM_PLAY_FLUTE,
    /* 3 */ SKJ_ANIM_DIE,
    /* 4 */ SKJ_ANIM_HIT,
    /* 5 */ SKJ_ANIM_LAND,
    /* 6 */ SKJ_ANIM_LOOK_LEFT_RIGHT,
    /* 7 */ SKJ_ANIM_FIGHTING_STANCE,
    /* 8 */ SKJ_ANIM_WALK_TO_PLAYER,
    /* 9 */ SKJ_ANIM_WAIT
} SkullKidAnim;

typedef enum {
    /* 0 */ SKULL_KID_LEFT,
    /* 1 */ SKULL_KID_RIGHT
} SkullKidStumpSide;

typedef enum {
    /* 0 */ SKULL_KID_OCARINA_WAIT,
    /* 1 */ SKULL_KID_OCARINA_PLAY_NOTES,
    /* 2 */ SKULL_KID_OCARINA_LEAVE_GAME
} SkullKidOcarinaGameState;

typedef enum {
    /* 00 */ SKJ_ACTION_FADE,
    /* 01 */ SKJ_ACTION_WAIT_TO_SHOOT_NEEDLE,
    /* 02 */ SKJ_ACTION_SARIA_SONG_IDLE,
    /* 03 */ SKJ_ACTION_WAIT_FOR_DEATH_ANIM,
    /* 04 */ SKJ_ACTION_PICK_NEXT_FIHGT_ACTION,
    /* 05 */ SKJ_ACTION_WAIT_FOR_LAND_ANIM,
    /* 06 */ SKJ_ACTION_RESET_FIGHT,
    /* 07 */ SKJ_ACTION_FIGHT,
    /* 08 */ SKJ_ACTION_NEEDLE_RECOVER,
    /* 09 */ SKJ_ACTION_SPAWN_DEATH_EFFECT,
    /* 10 */ SKJ_ACTION_SARIA_SONG_WAIT_IN_RANGE,
    /* 11 */ SKJ_ACTION_SARIA_SONG_WAIT_FOR_SONG,
    /* 12 */ SKJ_ACTION_SARIA_SONG_AFTER_SONG,
    /* 13 */ SKJ_ACTION_SARIA_TALK,
    /* 14 */ SKJ_ACTION_UNK14,
    /* 15 */ SKJ_ACTION_SARIA_SONG_CHANGE_MODE,
    /* 16 */ SKJ_ACTION_SARIA_SONG_START_TRADE,
    /* 17 */ SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING,
    /* 18 */ SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING_ANIM,
    /* 19 */ SKJ_ACTION_SARIA_SONG_WALK_TO_PLAYER,
    /* 20 */ SKJ_ACTION_SARIA_SONG_ASK_FOR_MASK,
    /* 21 */ SKJ_ACTION_SARIA_SONG_TAKE_MASK,
    /* 22 */ SKJ_ACTION_SARIA_SONG_WAIT_MASK_TEXT,
    /* 23 */ SKJ_ACTION_SARIA_SONG_WRONG_SONG,
    /* 24 */ SKJ_ACTION_SARIA_SONG_WAIT_FOR_TEXT,
    /* 25 */ SKJ_ACTION_OCARINA_GAME_WAIT_FOR_PLAYER,
    /* 26 */ SKJ_ACTION_OCARINA_GAME_IDLE,
    /* 27 */ SKJ_ACTION_OCARINA_GAME_PLAY,
    /* 28 */ SKJ_ACTION_OCARINA_GAME_LEAVE
} SkullKidAction;

typedef struct {
    u8 unk_0;
    EnSkj* skullkid;
} EnSkjUnkStruct;

static EnSkjUnkStruct sSmallStumpSkullKid = { 0, NULL };
static EnSkjUnkStruct sOcarinaMinigameSkullKids[] = { { 0, NULL }, { 0, NULL } };

const ActorInit En_Skj_InitVars = {
    ACTOR_EN_SKJ,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_SKJ,
    sizeof(EnSkj),
    (ActorFunc)EnSkj_Init,
    (ActorFunc)EnSkj_Destroy,
    (ActorFunc)EnSkj_Update,
    (ActorFunc)EnSkj_Draw,
    NULL,
};

static ColliderCylinderInitType1 D_80B01678 = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x0, 0x08 },
        { 0xFFCFFFFF, 0x0, 0x0 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 8, 48, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xF),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static s32 sOcarinaGameRewards[] = {
    GI_RUPEE_GREEN,
    GI_RUPEE_BLUE,
    GI_HEART_PIECE,
    GI_RUPEE_RED,
};

static AnimationMinimalInfo sAnimationInfo[] = {
    { &gSkullKidBackflipAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidShootNeedleAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidPlayFluteAnim, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidDieAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidHitAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidLandAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidLookLeftAndRightAnim, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidFightingStanceAnim, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidWalkToPlayerAnim, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidWaitAnim, ANIMMODE_LOOP, 0.0f },
};

static EnSkjActionFunc sActionFuncs[] = {
    EnSkj_Fade,
    EnSkj_WaitToShootNeedle,
    EnSkj_SariasSongKidIdle,
    EnSkj_WaitForDeathAnim,
    EnSkj_PickNextFightAction,
    EnSkj_WaitForLandAnim,
    EnSkj_ResetFight,
    EnSkj_Fight,
    EnSkj_NeedleRecover,
    EnSkj_SpawnDeathEffect,
    EnSkj_WaitInRange,
    EnSkj_WaitForSong,
    EnSkj_AfterSong,
    EnSkj_SariaSongTalk,
    func_80AFFE44,
    EnSkj_ChangeModeAfterSong,
    EnSkj_StartMaskTrade,
    EnSkj_WaitForLanding,
    EnSkj_WaitForLandAnimFinish,
    EnSkj_WalkToPlayer,
    EnSkj_AskForMask,
    EnSkj_TakeMask,
    EnSkj_WaitForMaskTextClear,
    EnSkj_WrongSong,
    EnSkj_SariasSongWaitForTextClear,
    EnSkj_OcarinaGameWaitForPlayer,
    EnSkj_OcarinaGameIdle,
    EnSkj_PlayOcarinaGame,
    EnSkj_LeaveOcarinaGame,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

static s32 D_80B01EA0; // gets set if ACTOR_FLAG_PLAYER_TALKED_TO is set

void EnSkj_ChangeAnim(EnSkj* this, u8 index) {
    f32 endFrame = Animation_GetLastFrame(sAnimationInfo[index].animation);

    this->animIndex = index;
    Animation_Change(&this->skelAnime, sAnimationInfo[index].animation, 1.0f, 0.0f, endFrame,
                     sAnimationInfo[index].mode, sAnimationInfo[index].morphFrames);
}

void EnSkj_SetupAction(EnSkj* this, u8 action) {
    this->action = action;
    this->actionFunc = sActionFuncs[action];

    switch (action) {
        case SKJ_ACTION_FADE:
        case SKJ_ACTION_WAIT_FOR_DEATH_ANIM:
        case SKJ_ACTION_PICK_NEXT_FIHGT_ACTION:
        case SKJ_ACTION_SPAWN_DEATH_EFFECT:
        case SKJ_ACTION_SARIA_SONG_START_TRADE:
        case SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING:
        case SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING_ANIM:
        case SKJ_ACTION_SARIA_SONG_WALK_TO_PLAYER:
        case SKJ_ACTION_SARIA_SONG_ASK_FOR_MASK:
        case SKJ_ACTION_SARIA_SONG_TAKE_MASK:
        case SKJ_ACTION_SARIA_SONG_WAIT_MASK_TEXT:
        case SKJ_ACTION_SARIA_SONG_WRONG_SONG:
        case SKJ_ACTION_SARIA_SONG_WAIT_FOR_TEXT:
        case SKJ_ACTION_OCARINA_GAME_WAIT_FOR_PLAYER:
        case SKJ_ACTION_OCARINA_GAME_IDLE:
        case SKJ_ACTION_OCARINA_GAME_PLAY:
        case SKJ_ACTION_OCARINA_GAME_LEAVE:
            this->unk_2D3 = 0;
            break;
        default:
            this->unk_2D3 = 1;
            break;
    }
}

void EnSkj_CalculateCenter(EnSkj* this) {
    Vec3f mult;

    mult.x = 0.0f;
    mult.y = 0.0f;
    mult.z = 120.0f;

    Matrix_RotateY((this->actor.shape.rot.y / 32768.0f) * M_PI, MTXMODE_NEW);
    Matrix_MultVec3f(&mult, &this->center);

    this->center.x += this->actor.world.pos.x;
    this->center.z += this->actor.world.pos.z;
}

void EnSkj_SetNaviId(EnSkj* this) {
    switch (this->actor.params) {
        case 0:
            if (Flags_GetItemGetInf(ITEMGETINF_39)) {
                this->actor.naviEnemyId = 0x41; // Skull kid with skull mask
            } else if (Flags_GetItemGetInf(ITEMGETINF_16)) {
                this->actor.naviEnemyId = 0x40; // Skull kid after Saria's song but no mask
            } else {
                this->actor.naviEnemyId = 0x3F; // No Sarias song no skull mask
            }
            break;

        case 1:
        case 2:
            this->actor.naviEnemyId = 0x3F;
            break;

        default:
            this->actor.naviEnemyId = 0x36; // Skull kid as adult
            break;
    }
}

void EnSkj_Init(Actor* thisx, PlayState* play2) {
    s16 type = (thisx->params >> 0xA) & 0x3F;
    EnSkj* this = (EnSkj*)thisx;
    PlayState* play = play2;
    s32 pad;
    Player* player;

    Actor_ProcessInitChain(thisx, sInitChain);
    switch (type) {
        case 5: // Invisible on the small stump (sarias song))
            sSmallStumpSkullKid.unk_0 = 1;
            sSmallStumpSkullKid.skullkid = (EnSkj*)thisx;
            this->actor.destroy = NULL;
            this->actor.draw = NULL;
            this->actor.update = EnSkj_SariasSongShortStumpUpdate;
            this->actor.flags &= ~(ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE);
            this->actor.flags |= 0;
            Actor_ChangeCategory(play, &play->actorCtx, thisx, ACTORCAT_PROP);
            break;

        case 6: // Invisible on the short stump (ocarina game)
            sSmallStumpSkullKid.unk_0 = 1;
            sSmallStumpSkullKid.skullkid = (EnSkj*)thisx;
            this->actor.destroy = NULL;
            this->actor.draw = NULL;
            this->actor.update = EnSkj_OcarinaMinigameShortStumpUpdate;
            this->actor.flags &= ~(ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE);
            this->actor.flags |= 0;
            Actor_ChangeCategory(play, &play->actorCtx, thisx, ACTORCAT_PROP);
            this->actor.focus.pos.x = 1230.0f;
            this->actor.focus.pos.y = -90.0f;
            this->actor.focus.pos.z = 450.0f;
            this->actionFunc = EnSkj_SetupWaitForOcarina;
            break;

        default:
            this->actor.params = type;
            if (((this->actor.params != 0) && (this->actor.params != 1)) && (this->actor.params != 2)) {
                if (INV_CONTENT(ITEM_TRADE_ADULT) < ITEM_SAW) {
                    Actor_Kill(&this->actor);
                    return;
                }
            }

            EnSkj_SetNaviId(this);
            SkelAnime_InitFlex(play, &this->skelAnime, &gSkullKidSkel, &gSkullKidPlayFluteAnim, this->jointTable,
                               this->morphTable, 19);
            if ((type >= 0) && (type < 3)) {
                this->actor.flags &= ~(ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE);
                this->actor.flags |= ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY;
                Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_NPC);
            }

            if ((type < 0) || (type >= 7)) {
                this->actor.flags &= ~ACTOR_FLAG_NO_FREEZE_OCARINA;
            }

            if ((type > 0) && (type < 3)) {
                this->actor.targetMode = 7;
                this->posCopy = this->actor.world.pos;
                sOcarinaMinigameSkullKids[type - 1].unk_0 = 1;
                sOcarinaMinigameSkullKids[type - 1].skullkid = this;
                this->minigameState = 0;
                this->alpha = 0;
                EnSkj_OcarinaGameSetupWaitForPlayer(this);
            } else {
                this->alpha = 255;
                EnSkj_SetupResetFight(this);
            }

            this->actor.colChkInfo.damageTable = &sDamageTable;
            this->actor.colChkInfo.health = 10;
            Collider_InitCylinder(play, &this->collider);
            Collider_SetCylinderType1(play, &this->collider, &this->actor, &D_80B01678);
            ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);
            Actor_SetScale(thisx, 0.01f);
            this->actor.textId = this->textId = 0;
            this->multiuseTimer = 0;
            this->backflipFlag = 0;
            this->needlesToShoot = 3;
            this->hitsUntilDodge = 3;
            this->actor.speedXZ = 0.0f;
            this->actor.velocity.y = 0.0f;
            this->actor.gravity = -1.0f;
            EnSkj_CalculateCenter(this);

            player = GET_PLAYER(play);
            osSyncPrintf("Player_X : %f\n", player->actor.world.pos.x);
            osSyncPrintf("Player_Z : %f\n", player->actor.world.pos.z);
            osSyncPrintf("World_X  : %f\n", this->actor.world.pos.x);
            osSyncPrintf("World_Z  : %f\n", this->actor.world.pos.z);
            osSyncPrintf("Center_X : %f\n", this->center.x);
            osSyncPrintf("Center_Z : %f\n\n", this->center.z);

            break;
    }
}

void EnSkj_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSkj* this = (EnSkj*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

s32 EnSkj_RangeCheck(Player* player, EnSkj* this) {
    f32 xDiff = player->actor.world.pos.x - this->actor.world.pos.x;
    f32 zDiff = player->actor.world.pos.z - this->actor.world.pos.z;
    f32 yDiff = player->actor.world.pos.y - this->actor.world.pos.y;

    return (SQ(xDiff) + SQ(zDiff) <= 676.0f) && (yDiff >= 0.0f);
}

f32 EnSkj_GetItemXzRange(EnSkj* this) {
    EnSkj* temp_v0;
    f32 zDiff;
    f32 xDiff;

    temp_v0 = sSmallStumpSkullKid.skullkid;
    xDiff = temp_v0->actor.world.pos.x - this->actor.world.pos.x;
    zDiff = temp_v0->actor.world.pos.z - this->actor.world.pos.z;
    return sqrtf(SQ(xDiff) + SQ(zDiff)) + 26.0f;
}

f32 EnSkj_GetItemYRange(EnSkj* this) {
    return fabsf(sSmallStumpSkullKid.skullkid->actor.world.pos.y - this->actor.world.pos.y) + 10.0f;
}

s32 EnSkj_ShootNeedle(EnSkj* this, PlayState* play) {
    s32 pad;
    Vec3f pos;
    Vec3f pos2;
    EnSkjneedle* needle;

    pos.x = 1.5f;
    pos.y = 0.0f;
    pos.z = 40.0f;

    Matrix_RotateY((this->actor.shape.rot.y / 32768.0f) * M_PI, MTXMODE_NEW);
    Matrix_MultVec3f(&pos, &pos2);

    pos2.x += this->actor.world.pos.x;
    pos2.z += this->actor.world.pos.z;
    pos2.y = this->actor.world.pos.y + 27.0f;

    needle = (EnSkjneedle*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_SKJNEEDLE, pos2.x, pos2.y, pos2.z,
                                       this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z, 0, true);
    if (needle != NULL) {
        needle->killTimer = 100;
        needle->actor.speedXZ = 24.0f;
        return 1;
    }
    return 0;
}

void EnSkj_SpawnBlood(PlayState* play, Vec3f* pos) {
    EffectSparkInit effect;
    s32 sp20;

    effect.position.x = pos->x;
    effect.position.y = pos->y;
    effect.position.z = pos->z;
    effect.uDiv = 5;
    effect.vDiv = 5;

    effect.colorStart[0].r = 0;
    effect.colorStart[0].g = 0;
    effect.colorStart[0].b = 128;
    effect.colorStart[0].a = 255;

    effect.colorStart[1].r = 0;
    effect.colorStart[1].g = 0;
    effect.colorStart[1].b = 128;
    effect.colorStart[1].a = 255;

    effect.colorStart[2].r = 0;
    effect.colorStart[2].g = 0;
    effect.colorStart[2].b = 128;
    effect.colorStart[2].a = 255;

    effect.colorStart[3].r = 0;
    effect.colorStart[3].g = 0;
    effect.colorStart[3].b = 128;
    effect.colorStart[3].a = 255;

    effect.colorEnd[0].r = 0;
    effect.colorEnd[0].g = 0;
    effect.colorEnd[0].b = 32;
    effect.colorEnd[0].a = 0;

    effect.colorEnd[1].r = 0;
    effect.colorEnd[1].g = 0;
    effect.colorEnd[1].b = 32;
    effect.colorEnd[1].a = 0;

    effect.colorEnd[2].r = 0;
    effect.colorEnd[2].g = 0;
    effect.colorEnd[2].b = 64;
    effect.colorEnd[2].a = 0;

    effect.colorEnd[3].r = 0;
    effect.colorEnd[3].g = 0;
    effect.colorEnd[3].b = 64;
    effect.colorEnd[3].a = 0;

    effect.speed = 8.0f;
    effect.gravity = -1.0f;

    effect.timer = 0;
    effect.duration = 8;

    Effect_Add(play, &sp20, EFFECT_SPARK, 0, 1, &effect);
}

s32 EnSkj_CollisionCheck(EnSkj* this, PlayState* play) {
    s16 yawDiff;
    Vec3f effectPos;

    if (!((this->unk_2D3 == 0) || (D_80B01EA0 != 0) || !(this->collider.base.acFlags & AC_HIT))) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->actor.colChkInfo.damageEffect != 0) {
            if (this->actor.colChkInfo.damageEffect == 0xF) {
                effectPos.x = this->collider.info.bumper.hitPos.x;
                effectPos.y = this->collider.info.bumper.hitPos.y;
                effectPos.z = this->collider.info.bumper.hitPos.z;

                EnSkj_SpawnBlood(play, &effectPos);
                EffectSsHitMark_SpawnFixedScale(play, 1, &effectPos);

                yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
                if ((this->action == 2) || (this->action == 6)) {
                    if ((yawDiff > 0x6000) || (yawDiff < -0x6000)) {
                        Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 8);
                        EnSkj_SetupDie(this);
                        return 1;
                    }
                }

                Actor_ApplyDamage(&this->actor);
                Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 8);

                if (this->actor.colChkInfo.health != 0) {
                    if (this->hitsUntilDodge != 0) {
                        this->hitsUntilDodge--;
                    }
                    if (this->dodgeResetTimer == 0) {
                        this->dodgeResetTimer = 60;
                    }
                    func_80AFF1F0(this);
                    return 1;
                }
                EnSkj_SetupDie(this);
                return 1;
            }
        } else {
            this->backflipFlag = 1;
            EnSkj_Backflip(this);
            return 1;
        }
    }
    return 0;
}

s32 func_80AFEDF8(EnSkj* this, PlayState* play) {
    s16 yawDiff;

    if (this->actor.xzDistToPlayer < this->unk_2EC) {
        this = this;
        if (func_8002DDE4(play) != 0) {
            return 1;
        }
    }

    yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;

    if ((yawDiff < this->unk_2C8) && (-this->unk_2C8 < yawDiff)) {
        return 1;
    }

    return 0;
}

void EnSkj_Backflip(EnSkj* this) {
    this->actor.velocity.y = 8.0f;
    this->actor.speedXZ = -8.0f;

    EnSkj_ChangeAnim(this, SKJ_ANIM_BACKFLIP);
    EnSkj_SetupAction(this, SKJ_ACTION_FADE);
}

void EnSkj_Fade(EnSkj* this, PlayState* play) {
    u32 alpha = this->alpha;

    if (this->unk_2D6 == 2) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_00;
        this->unk_2D6 = 0;
    }

    alpha -= 20;

    if (this->backflipFlag != 0) {
        if (alpha > 255) {
            alpha = 0;
        }

        this->alpha = alpha;
        this->actor.shape.shadowAlpha = alpha;
    }

    if (this->actor.velocity.y <= 0.0f) {
        if (this->actor.bgCheckFlags & 2) {
            this->actor.bgCheckFlags &= ~2;
            func_80AFF2A0(this);
        }
    }
}

void EnSkj_SetupWaitToShootNeedle(EnSkj* this) {
    this->needlesToShoot = 3;
    this->needleShootTimer = 0;
    EnSkj_ChangeAnim(this, SKJ_ANIM_SHOOT_NEEDLE);
    EnSkj_SetupAction(this, SKJ_ACTION_WAIT_TO_SHOOT_NEEDLE);
}

void EnSkj_WaitToShootNeedle(EnSkj* this, PlayState* play) {
    u8 val;
    s16 lastFrame = Animation_GetLastFrame(&gSkullKidShootNeedleAnim);

    if ((this->skelAnime.curFrame == lastFrame) && (this->needleShootTimer == 0)) {
        val = this->needlesToShoot;
        if (this->needlesToShoot != 0) {
            EnSkj_ShootNeedle(this, play);
            this->needleShootTimer = 4;
            val--;
            this->needlesToShoot = val;

        } else {
            EnSkj_SetupNeedleRecover(this);
        }
    }
}

void EnSkj_SetupResetFight(EnSkj* this) {
    this->unk_2C8 = 0xAAA;
    this->unk_2EC = 200.0f;
    EnSkj_ChangeAnim(this, SKJ_ANIM_PLAY_FLUTE);
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_IDLE);
}

void EnSkj_SariasSongKidIdle(EnSkj* this, PlayState* play) {
    if (this->actor.params == 0) {
        if (!Flags_GetItemGetInf(ITEMGETINF_16) && (this->actor.xzDistToPlayer < 200.0f)) {
            this->backflipFlag = 1;
            EnSkj_Backflip(this);
        } else if (sSmallStumpSkullKid.unk_0 != 0) {
            Player* player = GET_PLAYER(play);
            if (EnSkj_RangeCheck(player, sSmallStumpSkullKid.skullkid)) {
                EnSkj_SetupWaitInRange(this);
                player->stateFlags2 |= 0x800000;
                player->unk_6A8 = &sSmallStumpSkullKid.skullkid->actor;
            }
        }
    } else {
        if (func_80AFEDF8(this, play) != 0) {
            func_80AFF334(this);
        }
    }
}

void EnSkj_SetupDie(EnSkj* this) {
    EnSkj_ChangeAnim(this, SKJ_ANIM_DIE);
    EnSkj_SetupAction(this, SKJ_ACTION_WAIT_FOR_DEATH_ANIM);
    GameInteractor_ExecuteOnEnemyDefeat(&this->actor);
}

void EnSkj_WaitForDeathAnim(EnSkj* this, PlayState* play) {
    s16 lastFrame = Animation_GetLastFrame(&gSkullKidDieAnim);

    if (this->skelAnime.curFrame == lastFrame) {
        EnSkj_SetupSpawnDeathEffect(this);
    }
}

void func_80AFF1F0(EnSkj* this) {
    EnSkj_ChangeAnim(this, SKJ_ANIM_HIT);
    EnSkj_SetupAction(this, SKJ_ACTION_PICK_NEXT_FIHGT_ACTION);
}

void EnSkj_PickNextFightAction(EnSkj* this, PlayState* play) {
    s16 lastFrame = Animation_GetLastFrame(&gSkullKidHitAnim);

    if (this->skelAnime.curFrame == lastFrame) {
        if (this->hitsUntilDodge == 0) {
            this->hitsUntilDodge = 3;
            EnSkj_Backflip(this);
        } else {
            EnSkj_SetupStand(this);
        }
    }
}

void func_80AFF2A0(EnSkj* this) {
    EnSkj_CalculateCenter(this);
    this->actor.speedXZ = 0.0f;
    EnSkj_ChangeAnim(this, SKJ_ANIM_LAND);
    EnSkj_SetupAction(this, SKJ_ACTION_WAIT_FOR_LAND_ANIM);
}

void EnSkj_WaitForLandAnim(EnSkj* this, PlayState* play) {
    s16 lastFrame = Animation_GetLastFrame(&gSkullKidLandAnim);

    if (this->skelAnime.curFrame == lastFrame) {
        EnSkj_SetupStand(this);
    }
}

void func_80AFF334(EnSkj* this) {
    this->unk_2C8 = 0x2000;
    this->battleExitTimer = 400;
    this->unk_2EC = 600.0f;
    EnSkj_ChangeAnim(this, SKJ_ANIM_LOOK_LEFT_RIGHT);
    EnSkj_SetupAction(this, SKJ_ACTION_RESET_FIGHT);
}

void EnSkj_ResetFight(EnSkj* this, PlayState* play) {
    if (this->battleExitTimer == 0) {
        EnSkj_SetupResetFight(this);
    } else if (func_80AFEDF8(this, play) != 0) {
        this->battleExitTimer = 600;
        EnSkj_SetupStand(this);
    }
}

void EnSkj_SetupStand(EnSkj* this) {
    this->needleShootTimer = 60;
    this->unk_2C8 = 0x2000;
    this->unk_2F0 = 0.0f;
    this->unk_2EC = 600.0f;
    EnSkj_ChangeAnim(this, SKJ_ANIM_FIGHTING_STANCE);
    EnSkj_SetupAction(this, SKJ_ACTION_FIGHT);
}

void EnSkj_Fight(EnSkj* this, PlayState* play) {
    Vec3f pos1;
    Vec3f pos2;
    s32 pad[3];
    f32 prevPosX;
    f32 prevPosZ;
    f32 phi_f14;
    s16 yawDistToPlayer;

    if (this->needleShootTimer == 0) {
        EnSkj_SetupWaitToShootNeedle(this);
    } else if (this->battleExitTimer != 0) {
        yawDistToPlayer =
            Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, this->unk_2F0, 0);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        Math_ApproachF(&this->unk_2F0, 2000.0f, 1.0f, 200.0f);

        pos1.x = 0.0f;
        pos1.y = 0.0f;
        pos1.z = -120.0f;

        Matrix_RotateY((this->actor.shape.rot.y / 32768.0f) * M_PI, MTXMODE_NEW);
        Matrix_MultVec3f(&pos1, &pos2);
        prevPosX = this->actor.world.pos.x;
        prevPosZ = this->actor.world.pos.z;
        this->actor.world.pos.x = this->center.x + pos2.x;
        this->actor.world.pos.z = this->center.z + pos2.z;

        phi_f14 = sqrtf(SQ(this->actor.world.pos.x - prevPosX) + SQ(this->actor.world.pos.z - prevPosZ));
        phi_f14 = CLAMP_MAX(phi_f14, 10.0f);
        phi_f14 /= 10.0f;

        this->skelAnime.playSpeed = (yawDistToPlayer < 0) ? -(1.0f + phi_f14) : (1.0f + phi_f14);

    } else if (func_80AFEDF8(this, play) != 0) {
        this->backflipFlag = 1;
        EnSkj_Backflip(this);
    } else {
        EnSkj_SetupResetFight(this);
    }
}

void EnSkj_SetupNeedleRecover(EnSkj* this) {
    Animation_Reverse(&this->skelAnime);
    EnSkj_SetupAction(this, SKJ_ACTION_NEEDLE_RECOVER);
}

void EnSkj_NeedleRecover(EnSkj* this, PlayState* play) {
    if (this->skelAnime.curFrame == 0.0f) {
        EnSkj_SetupStand(this);
    }
}

void EnSkj_SetupSpawnDeathEffect(EnSkj* this) {
    this->backflipFlag = 1;
    EnSkj_SetupAction(this, SKJ_ACTION_SPAWN_DEATH_EFFECT);
}

void EnSkj_SpawnDeathEffect(EnSkj* this, PlayState* play) {
    Vec3f effectPos;
    Vec3f effectVel;
    Vec3f effectAccel;
    u32 phi_v0;

    phi_v0 = this->alpha - 4;

    if (phi_v0 > 255) {
        phi_v0 = 0;
    }
    this->alpha = phi_v0;
    this->actor.shape.shadowAlpha = phi_v0;

    effectPos.x = Rand_CenteredFloat(30.0f) + this->actor.world.pos.x;
    effectPos.y = Rand_CenteredFloat(30.0f) + this->actor.world.pos.y;
    effectPos.z = Rand_CenteredFloat(30.0f) + this->actor.world.pos.z;

    effectAccel.z = 0.0f;
    effectAccel.y = 0.0f;
    effectAccel.x = 0.0f;

    effectVel.z = 0.0f;
    effectVel.y = 0.0f;
    effectVel.x = 0.0f;

    EffectSsDeadDb_Spawn(play, &effectPos, &effectVel, &effectAccel, 100, 10, 255, 255, 255, 255, 0, 0, 255, 1, 9,
                         1);
}

void EnSkj_SetupWaitInRange(EnSkj* this) {
    this->textId = 0x10BC;

    EnSkj_ChangeAnim(this, SKJ_ANIM_WAIT);
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_WAIT_IN_RANGE);
}

void EnSkj_WaitInRange(EnSkj* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    // When link pulls out the Ocarina center him on the stump
    // Link was probably supposed to be pointed towards skull kid as well
    if (player->stateFlags2 & 0x1000000) {
        player->stateFlags2 |= 0x2000000;
        player->unk_6A8 = &sSmallStumpSkullKid.skullkid->actor;
        player->actor.world.pos.x = sSmallStumpSkullKid.skullkid->actor.world.pos.x;
        player->actor.world.pos.y = sSmallStumpSkullKid.skullkid->actor.world.pos.y;
        player->actor.world.pos.z = sSmallStumpSkullKid.skullkid->actor.world.pos.z;
        EnSkj_TurnPlayer(sSmallStumpSkullKid.skullkid, player);
        func_8010BD88(play, OCARINA_ACTION_CHECK_SARIA);
        EnSkj_SetupWaitForSong(this);
    } else if (D_80B01EA0 != 0) {
        player->actor.world.pos.x = sSmallStumpSkullKid.skullkid->actor.world.pos.x;
        player->actor.world.pos.y = sSmallStumpSkullKid.skullkid->actor.world.pos.y;
        player->actor.world.pos.z = sSmallStumpSkullKid.skullkid->actor.world.pos.z;
        if ((Player_GetMask(play) == PLAYER_MASK_SKULL) && !Flags_GetItemGetInf(ITEMGETINF_39)) {
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
            EnSkj_SetupMaskTrade(this);
        } else {
            EnSkj_SetupTalk(this);
        }
    } else if (!EnSkj_RangeCheck(player, sSmallStumpSkullKid.skullkid)) {
        EnSkj_SetupResetFight(this);
    } else {
        player->stateFlags2 |= 0x800000;
        if (Flags_GetItemGetInf(ITEMGETINF_16)) {
            if (Flags_GetItemGetInf(ITEMGETINF_39)) {
                this->textId = Text_GetFaceReaction(play, 0x15);
                if (this->textId == 0) {
                    this->textId = 0x1020;
                }
            } else if (Player_GetMask(play) == PLAYER_MASK_NONE) {
                this->textId = 0x10BC;
            } else if (Player_GetMask(play) == PLAYER_MASK_SKULL) {
                this->textId = 0x101B;
            } else {
                this->textId = Text_GetFaceReaction(play, 0x15);
            }
            func_8002F2CC(&this->actor, play, EnSkj_GetItemXzRange(this));
        }
    }
}

void EnSkj_SetupWaitForSong(EnSkj* this) {
    this->unk_2D6 = 0;
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_WAIT_FOR_SONG);
}

void EnSkj_WaitForSong(EnSkj* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    // Played a song thats not Saria's song
    if (!Flags_GetItemGetInf(ITEMGETINF_16) && ((play->msgCtx.msgMode == MSGMODE_OCARINA_FAIL) ||
                                                 (play->msgCtx.msgMode == MSGMODE_OCARINA_FAIL_NO_TEXT))) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        Message_CloseTextbox(play);
        player->unk_6A8 = &this->actor;
        func_8002F2CC(&this->actor, play, EnSkj_GetItemXzRange(this));
        EnSkj_SetupWrongSong(this);
    } else {
        if ((play->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK) && (this->unk_2D6 == 0)) {
            this->unk_2D6 = 1;
            EnSkj_ChangeAnim(this, SKJ_ANIM_PLAY_FLUTE);
        } else if ((this->unk_2D6 != 0) && (play->msgCtx.msgMode == MSGMODE_SONG_DEMONSTRATION_DONE)) {
            this->unk_2D6 = 0;
            EnSkj_ChangeAnim(this, SKJ_ANIM_WAIT);
        }
        if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
            play->msgCtx.ocarinaMode = OCARINA_MODE_00;
            this->unk_2D6 = 0;
            EnSkj_ChangeAnim(this, SKJ_ANIM_WAIT);
            EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_WAIT_IN_RANGE);
        } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) {
            if (!Flags_GetItemGetInf(ITEMGETINF_16)) {
                // Saria's song has been played for the first titme
                play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                func_80078884(NA_SE_SY_CORRECT_CHIME);
                player->unk_6A8 = &this->actor;
                func_8002F2CC(&this->actor, play, EnSkj_GetItemXzRange(this));
                this->textId = 0x10BB;
                EnSkj_SetupAfterSong(this);
            } else {
                play->msgCtx.ocarinaMode = OCARINA_MODE_05;
            }
        } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_02) {
            player->stateFlags2 &= ~0x1000000;
            Actor_Kill(&this->actor);
        } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
            player->stateFlags2 |= 0x800000;
        } else {
            if (play->msgCtx.ocarinaMode >= OCARINA_MODE_05) {
                gSaveContext.sunsSongState = 0;
                if (Flags_GetItemGetInf(ITEMGETINF_16)) {
                    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                    player->unk_6A8 = &this->actor;
                    func_8002F2CC(&this->actor, play, EnSkj_GetItemXzRange(this));
                    this->textId = 0x10BD;
                    EnSkj_SetupAfterSong(this);
                } else {
                    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                    player->unk_6A8 = &this->actor;
                    func_8002F2CC(&this->actor, play, EnSkj_GetItemXzRange(this));
                    EnSkj_SetupWrongSong(this);
                }
            }
        }
    }
}

void EnSkj_SetupAfterSong(EnSkj* this) {
    this->unk_2D6 = 0;
    EnSkj_ChangeAnim(this, SKJ_ANIM_WAIT);
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_AFTER_SONG);
}

void EnSkj_AfterSong(EnSkj* this, PlayState* play) {
    if (D_80B01EA0 != 0) {
        EnSkj_SetupTalk(this);
    } else {
        func_8002F2CC(&this->actor, play, EnSkj_GetItemXzRange(this));
    }
}

void EnSkj_SetupTalk(EnSkj* this) {
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_TALK);
}

void EnSkj_SariaSongTalk(EnSkj* this, PlayState* play) {
    s32 pad;

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        if (Flags_GetItemGetInf(ITEMGETINF_16)) {
            EnSkj_SetupWaitInRange(this);
        } else {
            func_80AFFE24(this);
            if (!IS_RANDO) {
                func_8002F434(&this->actor, play, GI_HEART_PIECE, EnSkj_GetItemXzRange(this), EnSkj_GetItemYRange(this));
            } else {
                GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LW_SKULL_KID, GI_HEART_PIECE);
                GiveItemEntryFromActor(&this->actor, play, getItemEntry, EnSkj_GetItemXzRange(this), EnSkj_GetItemYRange(this));
            }
        }
    }
}

void func_80AFFE24(EnSkj* this) {
    EnSkj_SetupAction(this, SKJ_ACTION_UNK14);
}

void func_80AFFE44(EnSkj* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        EnSkj_SetupPostSariasSong(this);
    } else {
        if (!IS_RANDO) {
            func_8002F434(&this->actor, play, GI_HEART_PIECE, EnSkj_GetItemXzRange(this), EnSkj_GetItemYRange(this));
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LW_SKULL_KID, GI_HEART_PIECE);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, EnSkj_GetItemXzRange(this), EnSkj_GetItemYRange(this));
        }
    }
}

void EnSkj_SetupPostSariasSong(EnSkj* this) {
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_CHANGE_MODE);
}

void EnSkj_ChangeModeAfterSong(EnSkj* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        Flags_SetItemGetInf(ITEMGETINF_16);
        EnSkj_SetNaviId(this);
        EnSkj_SetupWaitInRange(this);
    }
}

void EnSkj_SetupMaskTrade(EnSkj* this) {
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_START_TRADE);
}

void EnSkj_StartMaskTrade(EnSkj* this, PlayState* play) {
    u8 sp1F = Message_GetState(&play->msgCtx);

    func_8002DF54(play, &this->actor, 1);
    if ((sp1F == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        EnSkj_JumpFromStump(this);
    }
}

void EnSkj_JumpFromStump(EnSkj* this) {
    this->actor.velocity.y = 8.0f;
    this->actor.speedXZ = 2.0f;
    EnSkj_ChangeAnim(this, SKJ_ANIM_BACKFLIP);
    Animation_Reverse(&this->skelAnime);
    this->skelAnime.curFrame = this->skelAnime.startFrame;
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING);
}

void EnSkj_WaitForLanding(EnSkj* this, PlayState* play) {
    if (this->actor.velocity.y <= 0.0f) {
        if (this->actor.bgCheckFlags & 2) {
            this->actor.bgCheckFlags &= ~2;
            this->actor.speedXZ = 0.0f;
            EnSkj_SetupWaitForLandAnimFinish(this);
        }
    }
}

void EnSkj_SetupWaitForLandAnimFinish(EnSkj* this) {
    EnSkj_ChangeAnim(this, SKJ_ANIM_LAND);
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING_ANIM);
}

void EnSkj_WaitForLandAnimFinish(EnSkj* this, PlayState* play) {
    s16 lastFrame = Animation_GetLastFrame(&gSkullKidLandAnim);

    if (this->skelAnime.curFrame == lastFrame) {
        EnSkj_SetupWalkToPlayer(this);
    }
}

void EnSkj_SetupWalkToPlayer(EnSkj* this) {
    this->unk_2F0 = 0.0f;
    this->actor.speedXZ = 2.0f;
    EnSkj_ChangeAnim(this, SKJ_ANIM_WALK_TO_PLAYER);
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_WALK_TO_PLAYER);
}

void EnSkj_WalkToPlayer(EnSkj* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, this->unk_2F0, 0);
    Math_ApproachF(&this->unk_2F0, 2000.0f, 1.0f, 100.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (this->actor.xzDistToPlayer < 120.0f) {
        this->actor.speedXZ = 0.0f;
        EnSkj_SetupAskForMask(this, play);
    }
}

void EnSkj_SetupAskForMask(EnSkj* this, PlayState* play) {
    Message_StartTextbox(play, 0x101C, &this->actor);
    EnSkj_ChangeAnim(this, SKJ_ANIM_WAIT);
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_ASK_FOR_MASK);
}

void EnSkj_AskForMask(EnSkj* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // Yes
                EnSkj_SetupTakeMask(this, play);
                break;
            case 1: // No
                Message_ContinueTextbox(play, 0x101D);
                EnSkj_SetupWaitForMaskTextClear(this);
                break;
        }
    }
}

void EnSkj_SetupTakeMask(EnSkj* this, PlayState* play) {
    Message_ContinueTextbox(play, 0x101E);
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_TAKE_MASK);
}

void EnSkj_TakeMask(EnSkj* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        Rupees_ChangeBy(10);
        Flags_SetItemGetInf(ITEMGETINF_39);
        EnSkj_SetNaviId(this);
        Player_UnsetMask(play);
        Item_Give(play, ITEM_SOLD_OUT);
        Message_ContinueTextbox(play, 0x101F);
        EnSkj_SetupWaitForMaskTextClear(this);
    }
}

void EnSkj_SetupWaitForMaskTextClear(EnSkj* this) {
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_WAIT_MASK_TEXT);
}

void EnSkj_WaitForMaskTextClear(EnSkj* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        func_8002DF54(play, &this->actor, 7);
        this->backflipFlag = 1;
        EnSkj_Backflip(this);
    }
}

void EnSkj_SetupWrongSong(EnSkj* this) {
    this->textId = 0x1041;
    EnSkj_ChangeAnim(this, SKJ_ANIM_WAIT);
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_WRONG_SONG);
}

void EnSkj_WrongSong(EnSkj* this, PlayState* play) {
    if (D_80B01EA0 != 0) {
        EnSkj_SetupWaitForTextClear(this);
    } else {
        func_8002F2CC(&this->actor, play, EnSkj_GetItemXzRange(this));
    }
}

void EnSkj_SetupWaitForTextClear(EnSkj* this) {
    EnSkj_SetupAction(this, SKJ_ACTION_SARIA_SONG_WAIT_FOR_TEXT);
}

void EnSkj_SariasSongWaitForTextClear(EnSkj* this, PlayState* play) {
    u8 state = Message_GetState(&play->msgCtx);
    Player* player = GET_PLAYER(play);

    if (state == TEXT_STATE_DONE && Message_ShouldAdvance(play)) {
        EnSkj_SetupWaitInRange(this);
        player->stateFlags2 |= 0x800000;
        player->unk_6A8 = (Actor*)sSmallStumpSkullKid.skullkid;
    }
}

void EnSkj_OcarinaGameSetupWaitForPlayer(EnSkj* this) {
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    EnSkj_ChangeAnim(this, SKJ_ANIM_WAIT);
    EnSkj_SetupAction(this, SKJ_ACTION_OCARINA_GAME_WAIT_FOR_PLAYER);
}

void EnSkj_OcarinaGameWaitForPlayer(EnSkj* this, PlayState* play) {
    if (this->playerInRange) {
        this->actor.flags |= ACTOR_FLAG_TARGETABLE;
        EnSkj_SetupAction(this, SKJ_ACTION_OCARINA_GAME_IDLE);
    }
}

s32 EnSkj_IsLeavingGame(EnSkj* this) {
    s32 paramDecr = this->actor.params - 1;

    if (sOcarinaMinigameSkullKids[paramDecr].unk_0 == 2) {
        EnSkj_SetupLeaveOcarinaGame(this);
        return true;
    }
    return false;
}

void EnSkj_SetupIdle(EnSkj* this) {
    EnSkj_ChangeAnim(this, SKJ_ANIM_WAIT);
    EnSkj_SetupAction(this, SKJ_ACTION_OCARINA_GAME_IDLE);
}

void EnSkj_Appear(EnSkj* this) {
    if (this->alpha != 255) {
        this->alpha += 20;

        if (this->alpha > 255) {
            this->alpha = 255;
        }
    }
}

void EnSkj_OcarinaGameIdle(EnSkj* this, PlayState* play) {
    EnSkj_Appear(this);

    if ((EnSkj_IsLeavingGame(this) == false) && (this->minigameState != 0)) {
        EnSkj_SetupPlayOcarinaGame(this);
    }
}

void EnSkj_SetupPlayOcarinaGame(EnSkj* this) {
    EnSkj_ChangeAnim(this, SKJ_ANIM_PLAY_FLUTE);
    EnSkj_SetupAction(this, SKJ_ACTION_OCARINA_GAME_PLAY);
}

void EnSkj_PlayOcarinaGame(EnSkj* this, PlayState* play) {
    EnSkj_Appear(this);

    if (!EnSkj_IsLeavingGame(this) && (this->minigameState == 0)) {
        EnSkj_SetupIdle(this);
    }
}

void EnSkj_SetupLeaveOcarinaGame(EnSkj* this) {
    this->actor.velocity.y = 8.0f;
    this->actor.speedXZ = -8.0f;
    EnSkj_ChangeAnim(this, SKJ_ANIM_BACKFLIP);
    EnSkj_SetupAction(this, SKJ_ACTION_OCARINA_GAME_LEAVE);
}

void EnSkj_LeaveOcarinaGame(EnSkj* this, PlayState* play) {
    s32 paramsDecr = this->actor.params - 1;

    sOcarinaMinigameSkullKids[paramsDecr].unk_0 = 0;
    sOcarinaMinigameSkullKids[paramsDecr].skullkid = NULL;
    this->backflipFlag = 1;
    EnSkj_Backflip(this);
}

void EnSkj_Update(Actor* thisx, PlayState* play) {
    Vec3f dropPos;
    s32 pad;
    EnSkj* this = (EnSkj*)thisx;

    D_80B01EA0 = Actor_ProcessTalkRequest(&this->actor, play);

    this->timer++;

    if (this->multiuseTimer != 0) {
        this->multiuseTimer--;
    }

    if (this->needleShootTimer != 0) {
        this->needleShootTimer--;
    }

    if (this->dodgeResetTimer != 0) {
        this->dodgeResetTimer--;
    }

    if (this->dodgeResetTimer == 0) {
        this->hitsUntilDodge = 3;
    }

    if ((this->backflipFlag != 0) && (this->alpha == 0)) {
        if (this->action == 9) {
            dropPos.x = this->actor.world.pos.x;
            dropPos.y = this->actor.world.pos.y;
            dropPos.z = this->actor.world.pos.z;

            Item_DropCollectible(play, &dropPos, ITEM00_RUPEE_ORANGE);
        }
        Actor_Kill(&this->actor);
        return;
    }

    Actor_SetFocus(&this->actor, 30.0f);
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc(this, play);
    this->actor.textId = this->textId;
    EnSkj_CollisionCheck(this, play);
    Collider_UpdateCylinder(&this->actor, &this->collider);

    if ((this->unk_2D3 != 0) && (D_80B01EA0 == 0)) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);

        if (this->actor.colorFilterTimer == 0) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    SkelAnime_Update(&this->skelAnime);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 20.0f, 7);
}

void EnSkj_SariasSongShortStumpUpdate(Actor* thisx, PlayState* play) {
    EnSkj* this = (EnSkj*)thisx;

    D_80B01EA0 = Actor_ProcessTalkRequest(&this->actor, play);

    if (BREG(0) != 0) {
        DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }
}

void EnSkj_TurnPlayer(EnSkj* this, Player* player) {
    Math_SmoothStepToS(&player->actor.shape.rot.y, this->actor.world.rot.y, 5, 2000, 0);
    player->actor.world.rot.y = player->actor.shape.rot.y;
    player->currentYaw = player->actor.shape.rot.y;
}

void EnSkj_SetupWaitForOcarina(EnSkj* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (EnSkj_RangeCheck(player, this)) {
        sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid->playerInRange = true;
        sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].skullkid->playerInRange = true;

        if (player->stateFlags2 & 0x1000000) {
            player->stateFlags2 |= 0x2000000;
            func_800F5BF0(NATURE_ID_KOKIRI_REGION);
            EnSkj_TurnPlayer(this, player);
            player->unk_6A8 = &this->actor;
            Message_StartTextbox(play, 0x10BE, &this->actor);
            this->actionFunc = EnSkj_StartOcarinaMinigame;
        } else {
            this->actionFunc = EnSkj_WaitForOcarina;
        }
    }
}

void EnSkj_WaitForOcarina(EnSkj* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags2 & 0x1000000) {
        player->stateFlags2 |= 0x2000000;
        func_800F5BF0(NATURE_ID_KOKIRI_REGION);
        EnSkj_TurnPlayer(this, player);
        player->unk_6A8 = &this->actor;
        Message_StartTextbox(play, 0x10BE, &this->actor);
        this->actionFunc = EnSkj_StartOcarinaMinigame;
    } else if (EnSkj_RangeCheck(player, this)) {
        player->stateFlags2 |= 0x800000;
    }
}

void EnSkj_StartOcarinaMinigame(EnSkj* this, PlayState* play) {
    u8 dialogState = Message_GetState(&play->msgCtx);
    Player* player = GET_PLAYER(play);

    EnSkj_TurnPlayer(this, player);

    if (dialogState == TEXT_STATE_CLOSING) {
        func_8010BD58(play, OCARINA_ACTION_MEMORY_GAME);
        if (sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid != NULL) {
            sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid->minigameState = SKULL_KID_OCARINA_PLAY_NOTES;
        }
        this->songFailTimer = 160;
        this->actionFunc = EnSkj_WaitForPlayback;
    }
}

void EnSkj_WaitForPlayback(EnSkj* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    EnSkj_TurnPlayer(this, player);

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) { // failed the game
        Message_CloseTextbox(play);
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        player->unk_6A8 = &this->actor;
        func_8002F2CC(&this->actor, play, 26.0f);
        this->textId = 0x102D;
        this->actionFunc = EnSkj_FailedMiniGame;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_0F) { // completed the game
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        Message_CloseTextbox(play);
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        player->unk_6A8 = &this->actor;
        func_8002F2CC(&this->actor, play, 26.0f);
        this->textId = 0x10BF;
        this->actionFunc = EnSkj_WonOcarinaMiniGame;
    } else { // playing the game
        switch (play->msgCtx.msgMode) {
            case MSGMODE_MEMORY_GAME_LEFT_SKULLKID_WAIT:
                if (sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid != NULL) {
                    sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid->minigameState = SKULL_KID_OCARINA_WAIT;
                }
                if (!Audio_IsSfxPlaying(NA_SE_SY_METRONOME)) {
                    if (sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].skullkid != NULL) {
                        sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].skullkid->minigameState =
                            SKULL_KID_OCARINA_PLAY_NOTES;
                    }
                    Message_UpdateOcarinaGame(play);
                }
                break;
            case MSGMODE_MEMORY_GAME_RIGHT_SKULLKID_WAIT:
                if (sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].skullkid != NULL) {
                    sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].skullkid->minigameState = SKULL_KID_OCARINA_WAIT;
                }
                if (!Audio_IsSfxPlaying(NA_SE_SY_METRONOME)) {
                    Message_UpdateOcarinaGame(play);
                    this->songFailTimer = 160;
                }
                break;
            case MSGMODE_MEMORY_GAME_PLAYER_PLAYING:
                if (this->songFailTimer != 0) {
                    this->songFailTimer--;
                } else { // took too long, game failed
                    func_80078884(NA_SE_SY_OCARINA_ERROR);
                    Message_CloseTextbox(play);
                    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                    player->unk_6A8 = &this->actor;
                    func_8002F2CC(&this->actor, play, 26.0f);
                    this->textId = 0x102D;
                    this->actionFunc = EnSkj_FailedMiniGame;
                }
                break;
            case MSGMODE_MEMORY_GAME_START_NEXT_ROUND:
                if (!Audio_IsSfxPlaying(NA_SE_SY_METRONOME)) {
                    if (sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid != NULL) {
                        sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid->minigameState =
                            SKULL_KID_OCARINA_PLAY_NOTES;
                    }
                    this->songFailTimer = 160;
                    Audio_OcaSetInstrument(6); // related instrument sound (flute?)
                    Audio_OcaSetSongPlayback(OCARINA_SONG_MEMORY_GAME + 1, 1);
                    play->msgCtx.msgMode = MSGMODE_MEMORY_GAME_LEFT_SKULLKID_PLAYING;
                    play->msgCtx.stateTimer = 2;
                }
                break;
        }
    }
}

void EnSkj_FailedMiniGame(EnSkj* this, PlayState* play) {
    if (D_80B01EA0) {
        this->actionFunc = EnSkj_WaitForNextRound;
    } else {
        func_8002F2CC(&this->actor, play, 26.0f);
    }
}

void EnSkj_WaitForNextRound(EnSkj* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_DONE && Message_ShouldAdvance(play)) {
        EnSkj_OfferNextRound(this, play);
    }
}

void EnSkj_OfferNextRound(EnSkj* this, PlayState* play) {
    Message_ContinueTextbox(play, 0x102E);
    this->actionFunc = EnSkj_WaitForOfferResponse;
}

void EnSkj_WaitForOfferResponse(EnSkj* this, PlayState* play) {
    Player* player;

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // yes
                player = GET_PLAYER(play);
                player->stateFlags3 |= 0x20; // makes player take ocarina out right away after closing box
                this->actionFunc = EnSkj_SetupWaitForOcarina;
                break;
            case 1: // no
                this->actionFunc = EnSkj_CleanupOcarinaGame;
                break;
        }
    }
}

void EnSkj_WonOcarinaMiniGame(EnSkj* this, PlayState* play) {
    if (D_80B01EA0) {
        this->actionFunc = EnSkj_WaitToGiveReward;
    } else {
        func_8002F2CC(&this->actor, play, 26.0f);
    }
}

void EnSkj_WaitToGiveReward(EnSkj* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        if (IS_RANDO && gSaveContext.ocarinaGameRoundNum != 3) {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LW_OCARINA_MEMORY_GAME, GI_HEART_PIECE);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 26.0f, 26.0f);
        } else {
            func_8002F434(&this->actor, play, sOcarinaGameRewards[gSaveContext.ocarinaGameRoundNum], 26.0f, 26.0f);
        }

        this->actionFunc = EnSkj_GiveOcarinaGameReward;
    }
}

void EnSkj_GiveOcarinaGameReward(EnSkj* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = EnSkj_FinishOcarinaGameRound;
    } else {
        if (IS_RANDO && gSaveContext.ocarinaGameRoundNum != 3) {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LW_OCARINA_MEMORY_GAME, GI_HEART_PIECE);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 26.0f, 26.0f);
        } else {
            func_8002F434(&this->actor, play, sOcarinaGameRewards[gSaveContext.ocarinaGameRoundNum], 26.0f, 26.0f);
        }
    }
}

void EnSkj_FinishOcarinaGameRound(EnSkj* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        s32 ocarinaGameRoundNum = gSaveContext.ocarinaGameRoundNum;

        if (gSaveContext.ocarinaGameRoundNum < 3) {
            gSaveContext.ocarinaGameRoundNum++;
        }

        if (IS_RANDO) {
            gSaveContext.ocarinaGameRoundNum = 3;
        }

        if (ocarinaGameRoundNum == 2 || IS_RANDO) {
            Flags_SetItemGetInf(ITEMGETINF_17);
            this->actionFunc = EnSkj_CleanupOcarinaGame;
        } else {
            EnSkj_OfferNextRound(this, play);
        }
    }
}

void EnSkj_CleanupOcarinaGame(EnSkj* this, PlayState* play) {
    if (sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid != NULL) {
        sOcarinaMinigameSkullKids[SKULL_KID_LEFT].unk_0 = 2;
    }

    if (sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].skullkid != NULL) {
        sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].unk_0 = 2;
    }

    if ((sOcarinaMinigameSkullKids[SKULL_KID_LEFT].unk_0 == 2) &&
        (sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].unk_0 == 2)) {
        func_800F5C2C();
        Actor_Kill(&this->actor);
    }
}

void EnSkj_OcarinaMinigameShortStumpUpdate(Actor* thisx, PlayState* play) {
    EnSkj* this = (EnSkj*)thisx;

    D_80B01EA0 = Actor_ProcessTalkRequest(&this->actor, play);
    this->timer++;

    this->actor.focus.pos.x = 1230.0f;
    this->actor.focus.pos.y = -90.0f;
    this->actor.focus.pos.z = 450.0f;

    if (BREG(0) != 0) {
        DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }

    this->actionFunc(this, play);

    this->actor.textId = this->textId;
    this->actor.xzDistToPlayer = 50.0;
}

s32 EnSkj_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    return 0;
}

void EnSkj_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    OPEN_DISPS(play->state.gfxCtx);

    if ((limbIndex == 11) && (Flags_GetItemGetInf(ITEMGETINF_39))) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        Matrix_Push();
        Matrix_RotateZYX(-0x4000, 0, 0, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gSkullKidSkullMaskDL);
        Matrix_Pop();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

Gfx* EnSkj_TranslucentDL(GraphicsContext* gfxCtx, u32 alpha) {
    Gfx* dList;
    Gfx* dListHead;

    //! @bug This only allocates space for 1 command but uses 3
    dList = dListHead = Graph_Alloc(gfxCtx, sizeof(Gfx) * 3);
    gDPSetRenderMode(dListHead++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
    gDPSetEnvColor(dListHead++, 0, 0, 0, alpha);
    gSPEndDisplayList(dListHead++);

    return dList;
}

Gfx* EnSkj_OpaqueDL(GraphicsContext* gfxCtx, u32 alpha) {
    Gfx* dList;
    Gfx* dListHead;

    //! @bug This only allocates space for 1 command but uses 2
    dList = dListHead = Graph_Alloc(gfxCtx, sizeof(Gfx));
    gDPSetEnvColor(dListHead++, 0, 0, 0, alpha);
    gSPEndDisplayList(dListHead++);

    return dList;
}

void EnSkj_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSkj* this = (EnSkj*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    if (this->alpha < 255) {
        gSPSegment(POLY_OPA_DISP++, 0x0C, EnSkj_TranslucentDL(play->state.gfxCtx, this->alpha));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x0C, EnSkj_OpaqueDL(play->state.gfxCtx, this->alpha));
    }

    SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, EnSkj_OverrideLimbDraw, EnSkj_PostLimbDraw, this);

    CLOSE_DISPS(play->state.gfxCtx);
}
