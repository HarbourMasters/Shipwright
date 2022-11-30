#include "z_en_daiku.h"
#include "overlays/actors/ovl_En_GeldB/z_en_geldb.h"
#include "objects/object_daiku/object_daiku.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

typedef struct {
    Vec3f eyePosDeltaLocal;
    s32 maxFramesActive;
} EnDaikuEscapeSubCamParam;

// state flags

// probably related to animating torso and head to look towards the player
#define ENDAIKU_STATEFLAG_1 (1 << 1)
// same
#define ENDAIKU_STATEFLAG_2 (1 << 2)
// the gerudo guard appeared (after talking to the carpenter)
#define ENDAIKU_STATEFLAG_GERUDOFIGHTING (1 << 3)
// the gerudo guard was defeated
#define ENDAIKU_STATEFLAG_GERUDODEFEATED (1 << 4)

typedef enum {
    /* 0 */ ENDAIKU_STATE_CAN_TALK,
    /* 2 */ ENDAIKU_STATE_TALKING = 2,
    /* 3 */ ENDAIKU_STATE_NO_TALK
} EnDaikuTalkState;

void EnDaiku_Init(Actor* thisx, PlayState* play);
void EnDaiku_Destroy(Actor* thisx, PlayState* play);
void EnDaiku_Update(Actor* thisx, PlayState* play);
void EnDaiku_Draw(Actor* thisx, PlayState* play);

void EnDaiku_TentIdle(EnDaiku* this, PlayState* play);
void EnDaiku_Jailed(EnDaiku* this, PlayState* play);
void EnDaiku_WaitFreedom(EnDaiku* this, PlayState* play);
void EnDaiku_InitEscape(EnDaiku* this, PlayState* play);
void EnDaiku_EscapeRotate(EnDaiku* this, PlayState* play);
void EnDaiku_InitSubCamera(EnDaiku* this, PlayState* play);
void EnDaiku_EscapeRun(EnDaiku* this, PlayState* play);
s32 EnDaiku_OverrideLimbDraw(PlayState* play, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
void EnDaiku_PostLimbDraw(PlayState* play, s32 limb, Gfx** dList, Vec3s* rot, void* thisx);

const ActorInit En_Daiku_InitVars = {
    ACTOR_EN_DAIKU,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DAIKU,
    sizeof(EnDaiku),
    (ActorFunc)EnDaiku_Init,
    (ActorFunc)EnDaiku_Destroy,
    (ActorFunc)EnDaiku_Update,
    (ActorFunc)EnDaiku_Draw,
    NULL,
};

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
    { 18, 66, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit2 = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(0, 0x0),
    /* Giant's Knife */ DMG_ENTRY(0, 0x0),
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
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

typedef enum {
    /* 0 */ ENDAIKU_ANIM_SHOUT,
    /* 1 */ ENDAIKU_ANIM_STAND,
    /* 2 */ ENDAIKU_ANIM_CELEBRATE,
    /* 3 */ ENDAIKU_ANIM_RUN,
    /* 4 */ ENDAIKU_ANIM_SIT
} EnDaikuAnimation;

static AnimationFrameCountInfo sAnimationInfo[] = {
    { &object_daiku_Anim_001AB0, 1.0f, 0, 0 }, { &object_daiku_Anim_007DE0, 1.0f, 0, 0 },
    { &object_daiku_Anim_00885C, 1.0f, 0, 0 }, { &object_daiku_Anim_000C44, 1.0f, 0, 0 },
    { &object_daiku_Anim_008164, 1.0f, 0, 0 },
};

static EnDaikuEscapeSubCamParam sEscapeSubCamParams[] = {
    { { 0, 130, 220 }, 100 },
    { { -20, 22, 280 }, 110 },
    { { 50, 180, 350 }, 100 },
    { { -40, 60, 60 }, 120 },
};

void EnDaiku_ChangeAnim(EnDaiku* this, s32 index, s32* currentIndex) {
    f32 morphFrames;

    if (*currentIndex < 0 || *currentIndex == index) {
        morphFrames = 0.0f;
    } else {
        morphFrames = sAnimationInfo[index].morphFrames;
    }

    Animation_Change(&this->skelAnime, sAnimationInfo[index].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[index].animation), sAnimationInfo[index].mode, morphFrames);

    *currentIndex = index;
}

void EnDaiku_Init(Actor* thisx, PlayState* play) {
    EnDaiku* this = (EnDaiku*)thisx;
    s32 pad;
    s32 noKill = true;
    s32 isFree = false;

    if ((this->actor.params & 3) == 0 && (gSaveContext.eventChkInf[9] & 1)) {
        isFree = true;
    } else if ((this->actor.params & 3) == 1 && (gSaveContext.eventChkInf[9] & 2)) {
        isFree = true;
    } else if ((this->actor.params & 3) == 2 && (gSaveContext.eventChkInf[9] & 4)) {
        isFree = true;
    } else if ((this->actor.params & 3) == 3 && (gSaveContext.eventChkInf[9] & 8)) {
        isFree = true;
    }

    if (isFree == true && play->sceneNum == SCENE_GERUDOWAY) {
        noKill = false;
    } else if (isFree == false && play->sceneNum == SCENE_TENT) {
        noKill = false;
    }

    this->startFightSwitchFlag = this->actor.shape.rot.z & 0x3F;
    this->actor.shape.rot.z = 0;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_daiku_Skel_007958, NULL, this->jointTable, this->morphTable,
                       17);

    if (!noKill) {
        Actor_Kill(&this->actor);
        return;
    }

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit2);

    Animation_Change(&this->skelAnime, sAnimationInfo[ENDAIKU_ANIM_SHOUT].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[ENDAIKU_ANIM_SHOUT].animation),
                     sAnimationInfo[ENDAIKU_ANIM_SHOUT].mode, sAnimationInfo[ENDAIKU_ANIM_SHOUT].morphFrames);

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);

    this->actor.targetMode = 6;
    this->currentAnimIndex = -1;
    this->runSpeed = 5.0f;
    this->initRot = this->actor.world.rot;
    this->initPos = this->actor.world.pos;

    if (play->sceneNum == SCENE_GERUDOWAY) {
        EnDaiku_ChangeAnim(this, ENDAIKU_ANIM_STAND, &this->currentAnimIndex);
        this->stateFlags |= ENDAIKU_STATEFLAG_1 | ENDAIKU_STATEFLAG_2;
        this->actionFunc = EnDaiku_Jailed;
    } else {
        if ((this->actor.params & 3) == 1 || (this->actor.params & 3) == 3) {
            EnDaiku_ChangeAnim(this, ENDAIKU_ANIM_SIT, &this->currentAnimIndex);
            this->stateFlags |= ENDAIKU_STATEFLAG_1;
        } else {
            EnDaiku_ChangeAnim(this, ENDAIKU_ANIM_SHOUT, &this->currentAnimIndex);
            this->stateFlags |= ENDAIKU_STATEFLAG_1 | ENDAIKU_STATEFLAG_2;
        }

        this->skelAnime.curFrame = (s32)(Rand_ZeroOne() * this->skelAnime.endFrame);
        this->actionFunc = EnDaiku_TentIdle;
    }
}

void EnDaiku_Destroy(Actor* thisx, PlayState* play) {
    EnDaiku* this = (EnDaiku*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

s32 EnDaiku_UpdateTalking(EnDaiku* this, PlayState* play) {
    s32 newTalkState = ENDAIKU_STATE_TALKING;

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) {
        if (play->sceneNum == SCENE_GERUDOWAY) {
            if (Message_ShouldAdvance(play)) {
                if (this->actor.textId == 0x6007) {
                    Flags_SetSwitch(play, this->startFightSwitchFlag);
                    newTalkState = ENDAIKU_STATE_CAN_TALK;
                } else {
                    this->actionFunc = EnDaiku_InitEscape;
                    newTalkState = ENDAIKU_STATE_NO_TALK;
                }
            }
        } else if (play->sceneNum == SCENE_TENT) {
            if (Message_ShouldAdvance(play)) {
                switch (this->actor.textId) {
                    case 0x6061:
                        gSaveContext.infTable[23] |= 0x40;
                        break;
                    case 0x6064:
                        gSaveContext.infTable[23] |= 0x100;
                        break;
                }

                newTalkState = ENDAIKU_STATE_CAN_TALK;
            }
        }
    }

    return newTalkState;
}

void EnDaiku_UpdateText(EnDaiku* this, PlayState* play) {
    s32 carpenterType;
    s32 freedCount;
    s16 sp2E;
    s16 sp2C;

    if (this->talkState == ENDAIKU_STATE_TALKING) {
        this->talkState = EnDaiku_UpdateTalking(this, play);
    } else if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->talkState = ENDAIKU_STATE_TALKING;
    } else {
        Actor_GetScreenPos(play, &this->actor, &sp2E, &sp2C);
        if (sp2E >= 0 && sp2E <= 320 && sp2C >= 0 && sp2C <= 240 && this->talkState == ENDAIKU_STATE_CAN_TALK &&
            func_8002F2CC(&this->actor, play, 100.0f) == 1) {
            if (play->sceneNum == SCENE_GERUDOWAY) {
                if (this->stateFlags & ENDAIKU_STATEFLAG_GERUDODEFEATED) {
                    freedCount = 0;
                    for (carpenterType = 0; carpenterType < 4; carpenterType++) {
                        if (gSaveContext.eventChkInf[9] & (1 << carpenterType)) {
                            freedCount++;
                        }
                    }

                    switch (freedCount) {
                        case 0:
                            this->actor.textId = 0x605B;
                            break;
                        case 1:
                            this->actor.textId = 0x605C;
                            break;
                        case 2:
                            this->actor.textId = 0x605D;
                            break;
                        case 3:
                            this->actor.textId = 0x605E;
                            break;
                    }
                } else if (!(this->stateFlags &
                             (ENDAIKU_STATEFLAG_GERUDOFIGHTING | ENDAIKU_STATEFLAG_GERUDODEFEATED))) {
                    this->actor.textId = 0x6007;
                }
            } else if (play->sceneNum == SCENE_TENT) {
                switch (this->actor.params & 3) {
                    case 0:
                        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                            this->actor.textId = 0x6060;
                        } else {
                            this->actor.textId = 0x605F;
                        }
                        break;
                    case 1:
                        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                            this->actor.textId = 0x6063;
                        } else {
                            if (!(gSaveContext.infTable[23] & 0x40)) {
                                this->actor.textId = 0x6061;
                            } else {
                                this->actor.textId = 0x6062;
                            }
                        }
                        break;
                    case 2:
                        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                            this->actor.textId = 0x6066;
                        } else {
                            if (!(gSaveContext.infTable[23] & 0x100)) {
                                this->actor.textId = 0x6064;
                            } else {
                                this->actor.textId = 0x6065;
                            }
                        }
                        break;
                    case 3:
                        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                            this->actor.textId = 0x6068;
                        } else {
                            this->actor.textId = 0x6067;
                        }
                        break;
                }
            }
        }
    }
}

/**
 * The carpenter is idling in the tent.
 */
void EnDaiku_TentIdle(EnDaiku* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    EnDaiku_UpdateText(this, play);
}

/**
 * The carpenter is jailed in a Gerudo fortress cell, talking to him starts a fight against a gerudo guard
 */
void EnDaiku_Jailed(EnDaiku* this, PlayState* play) {
    EnGeldB* gerudo;
    s32 temp_t9;
    s32 temp_v1;

    if (!(this->stateFlags & ENDAIKU_STATEFLAG_GERUDOFIGHTING)) {
        EnDaiku_UpdateText(this, play);
    }
    SkelAnime_Update(&this->skelAnime);

    gerudo = (EnGeldB*)Actor_Find(&play->actorCtx, ACTOR_EN_GELDB, ACTORCAT_ENEMY);
    if (gerudo == NULL) {
        this->stateFlags |= ENDAIKU_STATEFLAG_GERUDODEFEATED;
        this->stateFlags &= ~ENDAIKU_STATEFLAG_GERUDOFIGHTING;
        EnDaiku_ChangeAnim(this, ENDAIKU_ANIM_CELEBRATE, &this->currentAnimIndex);
        this->actionFunc = EnDaiku_WaitFreedom;
    } else if (!(this->stateFlags & ENDAIKU_STATEFLAG_GERUDOFIGHTING) && !gerudo->invisible) {
        this->stateFlags |= ENDAIKU_STATEFLAG_GERUDOFIGHTING;
        this->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
    }
}

/**
 * The player defeated the gerudo guard and the carpenter is waiting for the cell door to be opened, and for the player
 * to then talk to him
 */
void EnDaiku_WaitFreedom(EnDaiku* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Flags_GetSwitch(play, this->actor.params >> 8 & 0x3F)) {
        this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        EnDaiku_UpdateText(this, play);
    }
}

/**
 * The carpenter is free, initializes his running away animation
 */
void EnDaiku_InitEscape(EnDaiku* this, PlayState* play) {
    Path* path;
    f32 dxz;
    f32 dx;
    f32 dz;
    Vec3s* pointPos;
    s32 exitLoop;

    Audio_PlayFanfare(NA_BGM_APPEAR);
    EnDaiku_ChangeAnim(this, ENDAIKU_ANIM_RUN, &this->currentAnimIndex);
    this->stateFlags &= ~(ENDAIKU_STATEFLAG_1 | ENDAIKU_STATEFLAG_2);

    gSaveContext.eventChkInf[9] |= 1 << (this->actor.params & 3);

    this->actor.gravity = -1.0f;
    this->escapeSubCamTimer = sEscapeSubCamParams[this->actor.params & 3].maxFramesActive;
    EnDaiku_InitSubCamera(this, play);

    exitLoop = false;
    path = &play->setupPathList[this->actor.params >> 4 & 0xF];
    while (!exitLoop) {
        pointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->waypoint;
        dx = pointPos->x - this->actor.world.pos.x;
        dz = pointPos->z - this->actor.world.pos.z;
        this->rotYtowardsPath = Math_FAtan2F(dx, dz) * (0x8000 / M_PI);
        dxz = sqrtf(SQ(dx) + SQ(dz));
        if (dxz > 10.0f) {
            exitLoop = true;
        } else {
            this->waypoint++;
        }
    }

    this->actionFunc = EnDaiku_EscapeRotate;
}

/**
 * The carpenter is rotating towards where he is going next
 */
void EnDaiku_EscapeRotate(EnDaiku* this, PlayState* play) {
    s16 diff;

    diff = Math_SmoothStepToS(&this->actor.shape.rot.y, this->rotYtowardsPath, 1, 0x1388, 0);
    SkelAnime_Update(&this->skelAnime);
    if (diff == 0) {
        this->actionFunc = EnDaiku_EscapeRun;
        this->actionFunc(this, play);
    }
}

void EnDaiku_InitSubCamera(EnDaiku* this, PlayState* play) {
    s32 pad;
    Vec3f eyePosDeltaLocal;
    Vec3f eyePosDeltaWorld;

    this->subCamActive = true;
    this->escapeSubCamTimer = sEscapeSubCamParams[this->actor.params & 3].maxFramesActive;

    eyePosDeltaLocal.x = sEscapeSubCamParams[this->actor.params & 3].eyePosDeltaLocal.x;
    eyePosDeltaLocal.y = sEscapeSubCamParams[this->actor.params & 3].eyePosDeltaLocal.y;
    eyePosDeltaLocal.z = sEscapeSubCamParams[this->actor.params & 3].eyePosDeltaLocal.z;
    Matrix_RotateY(this->actor.world.rot.y * (M_PI / 0x8000), MTXMODE_NEW);
    Matrix_MultVec3f(&eyePosDeltaLocal, &eyePosDeltaWorld);

    this->subCamEyeInit.x = this->subCamEye.x = this->actor.world.pos.x + eyePosDeltaWorld.x;
    this->subCamEyeInit.y = this->subCamEye.y = this->actor.world.pos.y + eyePosDeltaWorld.y;
    this->subCamEyeInit.z = this->subCamEye.z = this->actor.world.pos.z + eyePosDeltaWorld.z;

    this->subCamAtTarget.x = this->subCamAt.x = this->actor.world.pos.x;
    this->subCamAtTarget.y = this->subCamAt.y = this->actor.world.pos.y + 60.0f;
    this->subCamAtTarget.z = this->subCamAt.z = this->actor.world.pos.z;

    this->subCamId = Play_CreateSubCamera(play);
    Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
    Play_ChangeCameraStatus(play, this->subCamId, CAM_STAT_ACTIVE);

    Play_CameraSetAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    Play_CameraSetFov(play, this->subCamId, play->mainCamera.fov);
    func_8002DF54(play, &this->actor, 1);
}

void EnDaiku_UpdateSubCamera(EnDaiku* this, PlayState* play) {
    s32 pad;

    this->subCamAtTarget.x = this->actor.world.pos.x;
    this->subCamAtTarget.y = this->actor.world.pos.y + 60.0f;
    this->subCamAtTarget.z = this->actor.world.pos.z;

    Math_SmoothStepToF(&this->subCamAt.x, this->subCamAtTarget.x, 1.0f, 1000.0f, 0.0f);
    Math_SmoothStepToF(&this->subCamAt.y, this->subCamAtTarget.y, 1.0f, 1000.0f, 0.0f);
    Math_SmoothStepToF(&this->subCamAt.z, this->subCamAtTarget.z, 1.0f, 1000.0f, 0.0f);

    Play_CameraSetAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
}

void EnDaiku_EscapeSuccess(EnDaiku* this, PlayState* play) {
    static Vec3f D_809E4148 = { 0.0f, 0.0f, 120.0f };
    Actor* gerudoGuard;
    Vec3f vec;

    Play_ClearCamera(play, this->subCamId);
    Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_ACTIVE);
    this->subCamActive = false;

    if ((gSaveContext.eventChkInf[9] & 0xF) == 0xF) {
        Matrix_RotateY(this->initRot.y * (M_PI / 0x8000), MTXMODE_NEW);
        Matrix_MultVec3f(&D_809E4148, &vec);
        gerudoGuard =
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_GE3, this->initPos.x + vec.x, this->initPos.y + vec.y,
                        this->initPos.z + vec.z, 0, Math_FAtan2F(-vec.x, -vec.z) * (0x8000 / M_PI), 0, 2);

        if (gerudoGuard == NULL) {
            Actor_Kill(&this->actor);
        }
    } else {
        func_8002DF54(play, &this->actor, 7);
    }
}

/**
 * The carpenter is running away
 */
void EnDaiku_EscapeRun(EnDaiku* this, PlayState* play) {
    s32 pad1;
    Path* path;
    s16 ry;
    f32 dx;
    f32 dz;
    s32 pad2;
    f32 dxz;
    Vec3s* pointPos;

    path = &play->setupPathList[this->actor.params >> 4 & 0xF];
    pointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->waypoint;
    dx = pointPos->x - this->actor.world.pos.x;
    dz = pointPos->z - this->actor.world.pos.z;
    ry = Math_FAtan2F(dx, dz) * (0x8000 / M_PI);
    dxz = sqrtf(SQ(dx) + SQ(dz));
    if (dxz <= 20.88f) {
        this->waypoint++;
        if (this->waypoint >= path->count) {
            if (this->subCamActive) {
                EnDaiku_EscapeSuccess(this, play);
            }
            Actor_Kill(&this->actor);
            return;
        }
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, ry, 1, 0xFA0, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    Math_SmoothStepToF(&this->actor.speedXZ, this->runSpeed, 0.6f, dxz, 0.0f);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);

    if (this->subCamActive) {
        EnDaiku_UpdateSubCamera(this, play);
        if (this->escapeSubCamTimer-- <= 0) {
            EnDaiku_EscapeSuccess(this, play);
        }
    }

    SkelAnime_Update(&this->skelAnime);
}

void EnDaiku_Update(Actor* thisx, PlayState* play) {
    EnDaiku* this = (EnDaiku*)thisx;
    s32 curFrame;
    Player* player = GET_PLAYER(play);

    if (this->currentAnimIndex == ENDAIKU_ANIM_RUN) {
        curFrame = this->skelAnime.curFrame;
        if (curFrame == 6 || curFrame == 15) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_WALK);
        }
    }

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

    this->actionFunc(this, play);

    if (this->stateFlags & ENDAIKU_STATEFLAG_1) {
        this->unk_244.unk_18.x = player->actor.focus.pos.x;
        this->unk_244.unk_18.y = player->actor.focus.pos.y;
        this->unk_244.unk_18.z = player->actor.focus.pos.z;

        if (this->stateFlags & ENDAIKU_STATEFLAG_2) {
            func_80034A14(&this->actor, &this->unk_244, 0, 4);
        } else {
            func_80034A14(&this->actor, &this->unk_244, 0, 2);
        }
    }
}

void EnDaiku_Draw(Actor* thisx, PlayState* play) {
    EnDaiku* this = (EnDaiku*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    if ((thisx->params & 3) == 0) {
        gDPSetEnvColor(POLY_OPA_DISP++, 170, 10, 70, 255);
    } else if ((thisx->params & 3) == 1) {
        gDPSetEnvColor(POLY_OPA_DISP++, 170, 200, 255, 255);
    } else if ((thisx->params & 3) == 2) {
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 230, 70, 255);
    } else if ((thisx->params & 3) == 3) {
        gDPSetEnvColor(POLY_OPA_DISP++, 200, 0, 150, 255);
    }

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnDaiku_OverrideLimbDraw, EnDaiku_PostLimbDraw, this);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnDaiku_OverrideLimbDraw(PlayState* play, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDaiku* this = (EnDaiku*)thisx;

    switch (limb) {
        case 8: // torso
            rot->x += this->unk_244.unk_0E.y;
            rot->y -= this->unk_244.unk_0E.x;
            break;
        case 15: // head
            rot->x += this->unk_244.unk_08.y;
            rot->z += this->unk_244.unk_08.x;
            break;
    }

    return false;
}

void EnDaiku_PostLimbDraw(PlayState* play, s32 limb, Gfx** dList, Vec3s* rot, void* thisx) {
    static Gfx* hairDLists[] = { object_daiku_DL_005BD0, object_daiku_DL_005AC0, object_daiku_DL_005990,
                                 object_daiku_DL_005880 };
    static Vec3f targetPosHeadLocal = { 700, 1100, 0 };
    EnDaiku* this = (EnDaiku*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (limb == 15) { // head
        Matrix_MultVec3f(&targetPosHeadLocal, &this->actor.focus.pos);
        gSPDisplayList(POLY_OPA_DISP++, hairDLists[this->actor.params & 3]);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
