/*
 * File: z_en_daiku_kakariko.c
 * Overlay: ovl_En_Daiku_Kakariko
 * Description: Kakariko Village Carpenters
 */

#include "z_en_daiku_kakariko.h"
#include "objects/object_daiku/object_daiku.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_WHILE_CULLED)

typedef enum {
    /* 0x0 */ CARPENTER_ICHIRO,  // Red and purple pants, normal hair
    /* 0x1 */ CARPENTER_SABOORO, // Light blue pants
    /* 0x2 */ CARPENTER_JIRO,    // Green pants
    /* 0x3 */ CARPENTER_SHIRO    // Pink and purple pants, two-spiked hair
} KakarikoCarpenterType;

void EnDaikuKakariko_Init(Actor* thisx, PlayState* play);
void EnDaikuKakariko_Destroy(Actor* thisx, PlayState* play);
void EnDaikuKakariko_Update(Actor* thisx, PlayState* play);
void EnDaikuKakariko_Draw(Actor* thisx, PlayState* play);

void EnDaikuKakariko_Wait(EnDaikuKakariko* this, PlayState* play);
void EnDaikuKakariko_Run(EnDaikuKakariko* this, PlayState* play);

const ActorInit En_Daiku_Kakariko_InitVars = {
    ACTOR_EN_DAIKU_KAKARIKO,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DAIKU,
    sizeof(EnDaikuKakariko),
    (ActorFunc)EnDaikuKakariko_Init,
    (ActorFunc)EnDaikuKakariko_Destroy,
    (ActorFunc)EnDaikuKakariko_Update,
    (ActorFunc)EnDaikuKakariko_Draw,
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

static CollisionCheckInfoInit2 sColChkInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

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
    /* 0 */ ENDAIKUKAKARIKO_ANIM_0,
    /* 1 */ ENDAIKUKAKARIKO_ANIM_1,
    /* 2 */ ENDAIKUKAKARIKO_ANIM_2,
    /* 3 */ ENDAIKUKAKARIKO_ANIM_3,
    /* 4 */ ENDAIKUKAKARIKO_ANIM_4,
    /* 5 */ ENDAIKUKAKARIKO_ANIM_5
} EnDaikuKakarikoAnimation;

static AnimationFrameCountInfo sAnimationInfo[] = {
    { &object_daiku_Anim_001AB0, 1.0f, 2, -7.0f }, { &object_daiku_Anim_007DE0, 1.0f, 0, -7.0f },
    { &object_daiku_Anim_00885C, 1.0f, 0, -7.0f }, { &object_daiku_Anim_000C44, 1.0f, 0, -7.0f },
    { &object_daiku_Anim_000600, 1.0f, 0, -7.0f }, { &object_daiku_Anim_008164, 1.0f, 0, -7.0f },
};

void EnDaikuKakariko_ChangeAnim(EnDaikuKakariko* this, s32 index, s32* currentIndex) {
    f32 morphFrames;

    if ((*currentIndex < 0) || (index == *currentIndex)) {
        morphFrames = 0.0f;
    } else {
        morphFrames = sAnimationInfo[index].morphFrames;
    }

    Animation_Change(&this->skelAnime, sAnimationInfo[index].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[index].animation), sAnimationInfo[index].mode, morphFrames);

    *currentIndex = index;
}

void EnDaikuKakariko_Init(Actor* thisx, PlayState* play) {
    static u16 initFlags[] = { 0x0080, 0x00B0, 0x0070, 0x0470 }; // List of inital values for this->flags
    EnDaikuKakariko* this = (EnDaikuKakariko*)thisx;
    s32 pad;

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        switch (play->sceneNum) {
            case SCENE_KAKARIKO_VILLAGE:
                if (IS_DAY) {
                    this->flags |= 1;
                    this->flags |= initFlags[this->actor.params & 3];
                }
                break;
            case SCENE_KAKARIKO_CENTER_GUEST_HOUSE:
                if (IS_NIGHT) {
                    this->flags |= 2;
                }
                break;
            case SCENE_POTION_SHOP_KAKARIKO:
                this->flags |= 4;
                break;
        }
    }

    if (!(this->flags & 7)) {
        Actor_Kill(&this->actor);
    }

    if (IS_NIGHT) {
        this->flags |= 8;
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);

    SkelAnime_InitFlex(play, &this->skelAnime, &object_daiku_Skel_007958, NULL, this->jointTable, this->morphTable,
                       17);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInit);

    Animation_Change(&this->skelAnime, sAnimationInfo[ENDAIKUKAKARIKO_ANIM_0].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[ENDAIKUKAKARIKO_ANIM_0].animation),
                     sAnimationInfo[ENDAIKUKAKARIKO_ANIM_0].mode, sAnimationInfo[ENDAIKUKAKARIKO_ANIM_0].morphFrames);

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);

    this->actor.gravity = 0.0f;
    this->runSpeed = 3.0f;
    this->actor.uncullZoneForward = 1200.0f;
    this->actor.targetMode = 6;
    this->currentAnimIndex = -1;

    if (this->flags & 0x40) {
        this->actor.gravity = -1.0f;
    }

    if (this->flags & 0x10) {
        EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_3, &this->currentAnimIndex);
        this->actionFunc = EnDaikuKakariko_Run;
    } else {
        if (this->flags & 8) {
            if (((this->actor.params & 3) == CARPENTER_SABOORO) || ((this->actor.params & 3) == CARPENTER_SHIRO)) {
                EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_5, &this->currentAnimIndex);
                this->flags |= 0x800;
            } else {
                EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_1, &this->currentAnimIndex);
            }

            this->skelAnime.curFrame = (s32)(Rand_ZeroOne() * this->skelAnime.endFrame);
        } else {
            EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_0, &this->currentAnimIndex);
            this->skelAnime.curFrame = (s32)(Rand_ZeroOne() * this->skelAnime.endFrame);
        }

        this->flags |= 0x100;
        this->actionFunc = EnDaikuKakariko_Wait;
    }
}

void EnDaikuKakariko_Destroy(Actor* thisx, PlayState* play) {
    EnDaikuKakariko* this = (EnDaikuKakariko*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

s32 EnDaikuKakariko_GetTalkState(EnDaikuKakariko* this, PlayState* play) {
    s32 talkState = 2;

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        switch (this->actor.textId) {
            case 0x6061:
                Flags_SetInfTable(INFTABLE_176);
                break;
            case 0x6064:
                Flags_SetInfTable(INFTABLE_178);
                break;
        }
        talkState = 0;
    }
    return talkState;
}

void EnDaikuKakariko_HandleTalking(EnDaikuKakariko* this, PlayState* play) {
    static s32 maskReactionSets[] = { 1, 2, 3, 4 };
    s16 sp26;
    s16 sp24;

    if (this->talkState == 2) {
        this->talkState = EnDaikuKakariko_GetTalkState(this, play);
    } else if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->talkState = 2;
    } else {
        Actor_GetScreenPos(play, &this->actor, &sp26, &sp24);

        if ((sp26 >= 0) && (sp26 <= 320) && (sp24 >= 0) && (sp24 <= 240) && (this->talkState == 0) &&
            (func_8002F2CC(&this->actor, play, 100.0f) == 1)) {
            this->actor.textId = Text_GetFaceReaction(play, maskReactionSets[this->actor.params & 3]);

            if (this->actor.textId == 0) {
                switch (this->actor.params & 3) {
                    case 0:
                        if (this->flags & 8) {
                            this->actor.textId = 0x5076;
                        } else {
                            this->actor.textId = 0x5075;
                        }
                        break;
                    case 1:
                        if (this->flags & 1) {
                            this->actor.textId = 0x502A;
                        } else {
                            this->actor.textId = 0x5074;
                        }
                        break;
                    case 2:
                        if (this->flags & 1) {
                            this->actor.textId = 0x506A;
                        } else {
                            this->actor.textId = 0x506B;
                        }
                        break;
                    case 3:
                        if (this->flags & 1) {
                            this->actor.textId = 0x5077;
                        } else {
                            this->actor.textId = 0x5078;
                        }
                        break;
                }
            }
        }
    }
}

void EnDaikuKakariko_Talk(EnDaikuKakariko* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_3, &this->currentAnimIndex);
    }

    EnDaikuKakariko_HandleTalking(this, play);

    if (this->talkState == 0) {
        if (this->flags & 0x10) {
            EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_3, &this->currentAnimIndex);
            this->flags &= ~0x0300;
            this->actionFunc = EnDaikuKakariko_Run;
            return;
        }

        if (!(this->flags & 8)) {
            EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_0, &this->currentAnimIndex);
        }

        if ((this->flags & 0x800) == 0) {
            this->flags &= ~0x0200;
            this->flags |= 0x100;
        }

        this->actionFunc = EnDaikuKakariko_Wait;
    }
}

void EnDaikuKakariko_Wait(EnDaikuKakariko* this, PlayState* play) {
    EnDaikuKakariko_HandleTalking(this, play);

    if (SkelAnime_Update(&this->skelAnime)) {
        EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_0, &this->currentAnimIndex);
    }

    if (this->talkState != 0) {
        if (!(this->flags & 8)) {
            EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_4, &this->currentAnimIndex);
        }

        if (!(this->flags & 0x800)) {
            this->flags |= 0x200;
            this->flags &= ~0x0100;
        }

        this->actionFunc = EnDaikuKakariko_Talk;
    }
}

void EnDaikuKakariko_StopRunning(EnDaikuKakariko* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->timer--;

        if (this->timer <= 0) {
            EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_3, &this->currentAnimIndex);
            this->actionFunc = EnDaikuKakariko_Run;
        } else {
            this->skelAnime.curFrame = this->skelAnime.startFrame;
        }
    }

    EnDaikuKakariko_HandleTalking(this, play);

    if (this->talkState != 0) {
        this->flags |= 0x200;
        EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_4, &this->currentAnimIndex);
        this->actionFunc = EnDaikuKakariko_Talk;
    }
}

void EnDaikuKakariko_Run(EnDaikuKakariko* this, PlayState* play) {
    s32 pad;
    Path* path;
    Vec3s* pathPos;
    f32 xDist;
    f32 zDist;
    s16 runAngle;
    f32 runDist;
    s16 angleStepDiff;
    s32 run;

    do {
        path = &play->setupPathList[(this->actor.params >> 8) & 0xFF];
        pathPos = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[this->waypoint];
        xDist = pathPos->x - this->actor.world.pos.x;
        zDist = pathPos->z - this->actor.world.pos.z;
        runAngle = Math_FAtan2F(xDist, zDist) * (32768.0f / M_PI);
        runDist = sqrtf((xDist * xDist) + (zDist * zDist));

        run = false;

        if (runDist <= 10.0f) {
            if (this->pathContinue == false) {
                this->waypoint++;

                if (this->waypoint >= path->count) {
                    if (this->flags & 0x20) {
                        this->waypoint = path->count - 2;
                        this->pathContinue = true;
                        this->run = run = false;

                        if (this->flags & 0x400) {
                            this->timer = 2;
                            EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_0, &this->currentAnimIndex);
                            this->actionFunc = EnDaikuKakariko_StopRunning;
                            return;
                        }
                    } else {
                        this->waypoint = 0;
                        run = true;
                    }
                } else {
                    this->run = run = true;
                }
            } else {
                this->waypoint--;

                if (this->waypoint < 0) {
                    this->waypoint = 1;
                    this->pathContinue = false;
                    this->run = run = false;

                    if (this->flags & 0x400) {
                        this->timer = 2;
                        EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_0, &this->currentAnimIndex);
                        this->actionFunc = EnDaikuKakariko_StopRunning;
                        return;
                    }
                } else {
                    run = true;
                }
            }
        }
    } while (run);

    angleStepDiff = Math_SmoothStepToS(&this->actor.shape.rot.y, runAngle, 1, 5000, 0);

    this->actor.world.rot.y = this->actor.shape.rot.y;

    if (this->run == false) {
        if (angleStepDiff == 0) {
            this->run = true;
        } else {
            this->actor.speedXZ = 0.0f;
        }
    }

    if (this->run == true) {
        Math_SmoothStepToF(&this->actor.speedXZ, this->runSpeed, 0.8f, runDist, 0.0f);
    }

    Actor_MoveForward(&this->actor);

    if (this->flags & 0x40) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    } else if (this->flags & 0x80) {
        this->runFlag |= 1;
        this->flags &= ~0x0080;
    } else if (this->runFlag & 1) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
        this->runFlag &= ~1;
    }

    SkelAnime_Update(&this->skelAnime);
    EnDaikuKakariko_HandleTalking(this, play);

    if (this->talkState != 0) {
        this->flags |= 0x200;
        EnDaikuKakariko_ChangeAnim(this, ENDAIKUKAKARIKO_ANIM_4, &this->currentAnimIndex);
        this->actionFunc = EnDaikuKakariko_Talk;
    }
}

void EnDaikuKakariko_Update(Actor* thisx, PlayState* play) {
    EnDaikuKakariko* this = (EnDaikuKakariko*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 pad2;

    if (this->currentAnimIndex == 3) {
        if (((s32)this->skelAnime.curFrame == 6) || ((s32)this->skelAnime.curFrame == 15)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_WALK);
        }
    }

    Collider_UpdateCylinder(&this->actor, &this->collider);

    if (this->flags & 4) {
        this->collider.dim.pos.x -= 27;
        this->collider.dim.pos.z -= 27;
        this->collider.dim.radius = 63;
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

    this->actionFunc(this, play);

    this->interactInfo.trackPos.x = player->actor.focus.pos.x;
    this->interactInfo.trackPos.y = player->actor.focus.pos.y;
    this->interactInfo.trackPos.z = player->actor.focus.pos.z;

    if (this->flags & 0x100) {
        this->neckAngleTarget.x = 5900;
        this->flags |= 0x1000;
        Npc_TrackPoint(&this->actor, &this->interactInfo, 0, NPC_TRACKING_HEAD_AND_TORSO);
    } else if (this->flags & 0x200) {
        this->neckAngleTarget.x = 5900;
        this->flags |= 0x1000;
        Npc_TrackPoint(&this->actor, &this->interactInfo, 0, NPC_TRACKING_FULL_BODY);
    }

    Math_SmoothStepToS(&this->neckAngle.x, this->neckAngleTarget.x, 1, 1820, 0);
}

s32 EnDaikuKakariko_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                     void* thisx) {
    EnDaikuKakariko* this = (EnDaikuKakariko*)thisx;
    Vec3s angle;

    switch (limbIndex) {
        case 8:
            angle = this->interactInfo.torsoRot;
            Matrix_RotateX(-(angle.y * (M_PI / 32768.0f)), MTXMODE_APPLY);
            Matrix_RotateZ(-(angle.x * (M_PI / 32768.0f)), MTXMODE_APPLY);
            break;
        case 15:
            Matrix_Translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            angle = this->interactInfo.headRot;

            if (this->flags & 0x1000) {
                osSyncPrintf("<%d>\n", this->neckAngle.x);
                Matrix_RotateX((angle.y + this->neckAngle.y) * (M_PI / 32768.0f), MTXMODE_APPLY);
                Matrix_RotateZ((angle.x + this->neckAngle.x) * (M_PI / 32768.0f), MTXMODE_APPLY);
            } else {
                Matrix_RotateX(angle.y * (M_PI / 32768.0f), MTXMODE_APPLY);
                Matrix_RotateZ(angle.x * (M_PI / 32768.0f), MTXMODE_APPLY);
            }

            Matrix_Translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            break;
    }

    return 0;
}

void EnDaikuKakariko_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Gfx* carpenterHeadDLists[] = { object_daiku_DL_005BD0, object_daiku_DL_005AC0, object_daiku_DL_005990,
                                          object_daiku_DL_005880 };
    static Vec3f unkVec = { 700.0f, 1100.0f, 0.0f };
    EnDaikuKakariko* this = (EnDaikuKakariko*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == 15) {
        Matrix_MultVec3f(&unkVec, &this->actor.focus.pos);
        gSPDisplayList(POLY_OPA_DISP++, carpenterHeadDLists[this->actor.params & 3]);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDaikuKakariko_Draw(Actor* thisx, PlayState* play) {
    EnDaikuKakariko* this = (EnDaikuKakariko*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    if ((thisx->params & 3) == CARPENTER_ICHIRO) {
        gDPSetEnvColor(POLY_OPA_DISP++, 170, 10, 70, 255);
    } else if ((thisx->params & 3) == CARPENTER_SABOORO) {
        gDPSetEnvColor(POLY_OPA_DISP++, 170, 200, 255, 255);
    } else if ((thisx->params & 3) == CARPENTER_JIRO) {
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 230, 70, 255);
    } else if ((thisx->params & 3) == CARPENTER_SHIRO) {
        gDPSetEnvColor(POLY_OPA_DISP++, 200, 0, 150, 255);
    }

    SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, EnDaikuKakariko_OverrideLimbDraw, EnDaikuKakariko_PostLimbDraw, thisx);

    CLOSE_DISPS(play->state.gfxCtx);
}
