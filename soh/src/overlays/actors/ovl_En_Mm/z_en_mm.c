/*
 * File: z_en_mm.c
 * Overlay: ovl_En_Mm
 * Description: Running Man (child)
 */

#include "z_en_mm.h"
#include "objects/object_mm/object_mm.h"
#include "objects/object_link_child/object_link_child.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

typedef enum {
    /* 0 */ RM_ANIM_RUN,
    /* 1 */ RM_ANIM_SIT,
    /* 2 */ RM_ANIM_SIT_WAIT,
    /* 3 */ RM_ANIM_STAND,
    /* 4 */ RM_ANIM_SPRINT,
    /* 5 */ RM_ANIM_EXCITED, // plays when talking to him with bunny hood on
    /* 6 */ RM_ANIM_HAPPY    // plays when you sell him the bunny hood
} RunningManAnimIndex;

typedef enum {
    /* 0 */ RM_MOUTH_CLOSED,
    /* 1 */ RM_MOUTH_OPEN
} RunningManMouthTex;

void EnMm_Init(Actor* thisx, PlayState* play);
void EnMm_Destroy(Actor* thisx, PlayState* play);
void EnMm_Update(Actor* thisx, PlayState* play);
void EnMm_Draw(Actor* thisx, PlayState* play);

void func_80AAE598(EnMm* this, PlayState* play);
void func_80AAE294(EnMm* this, PlayState* play);
void func_80AAE50C(EnMm* this, PlayState* play);
void func_80AAE224(EnMm* this, PlayState* play);
s32 func_80AADA70(void);

s32 EnMm_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
void EnMm_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void*);

const ActorInit En_Mm_InitVars = {
    ACTOR_EN_MM,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MM,
    sizeof(EnMm),
    (ActorFunc)EnMm_Init,
    (ActorFunc)EnMm_Destroy,
    (ActorFunc)EnMm_Update,
    (ActorFunc)EnMm_Draw,
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
    { 18, 63, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit[] = { 0, 0, 0, 0, MASS_IMMOVABLE };

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

static AnimationSpeedInfo sAnimationInfo[] = {
    { &gRunningManRunAnim, 1.0f, ANIMMODE_LOOP, -7.0f },     { &gRunningManSitStandAnim, -1.0f, ANIMMODE_ONCE, -7.0f },
    { &gRunningManSitWaitAnim, 1.0f, ANIMMODE_LOOP, -7.0f }, { &gRunningManSitStandAnim, 1.0f, ANIMMODE_ONCE, -7.0f },
    { &gRunningManSprintAnim, 1.0f, ANIMMODE_LOOP, -7.0f },  { &gRunningManExcitedAnim, 1.0f, ANIMMODE_LOOP, -12.0f },
    { &gRunningManHappyAnim, 1.0f, ANIMMODE_LOOP, -12.0f },
};

typedef struct {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
} EnMmPathInfo;

static EnMmPathInfo sPathInfo[] = {
    { 0, 1, 0, 0 },
    { 1, 1, 0, 1 },
    { 1, 3, 2, 1 },
    { -1, 0, 2, 0 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_STOP),
};

void EnMm_ChangeAnim(EnMm* this, s32 index, s32* currentIndex) {
    f32 morphFrames;

    if ((*currentIndex < 0) || (index == *currentIndex)) {
        morphFrames = 0.0f;
    } else {
        morphFrames = sAnimationInfo[index].morphFrames;
    }

    if (sAnimationInfo[index].playSpeed >= 0.0f) {
        Animation_Change(&this->skelAnime, sAnimationInfo[index].animation, sAnimationInfo[index].playSpeed, 0.0f,
                         Animation_GetLastFrame(sAnimationInfo[index].animation), sAnimationInfo[index].mode,
                         morphFrames);
    } else {
        Animation_Change(&this->skelAnime, sAnimationInfo[index].animation, sAnimationInfo[index].playSpeed,
                         Animation_GetLastFrame(sAnimationInfo[index].animation), 0.0f, sAnimationInfo[index].mode,
                         morphFrames);
    }

    *currentIndex = index;
}

void EnMm_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnMm* this = (EnMm*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 21.0f);

    SkelAnime_InitFlex(play, &this->skelAnime, &gRunningManSkel, NULL, this->jointTable, this->morphTable, 16);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, sColChkInfoInit);

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    Animation_Change(&this->skelAnime, sAnimationInfo[RM_ANIM_RUN].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[RM_ANIM_RUN].animation), sAnimationInfo[RM_ANIM_RUN].mode,
                     sAnimationInfo[RM_ANIM_RUN].morphFrames);

    this->path = this->actor.params & 0xFF;
    this->unk_1F0 = 2;
    this->unk_1E8 = 0;
    this->actor.targetMode = 2;
    this->actor.gravity = -1.0f;
    this->speedXZ = 3.0f;
    this->unk_204 = this->actor.objBankIndex;

    if (func_80AADA70() == 1) {
        this->mouthTexIndex = RM_MOUTH_OPEN;
        EnMm_ChangeAnim(this, RM_ANIM_RUN, &this->curAnimIndex);
        this->actionFunc = func_80AAE598;
    } else {
        this->mouthTexIndex = RM_MOUTH_CLOSED;
        EnMm_ChangeAnim(this, RM_ANIM_SIT_WAIT, &this->curAnimIndex);
        this->actionFunc = func_80AAE294;
    }
}

void EnMm_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnMm* this = (EnMm*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

s32 func_80AADA70(void) {
    s32 isDay = false;

    if ((gSaveContext.dayTime > 0x3555) && (gSaveContext.dayTime <= 0xD556)) {
        isDay = true;
    }

    return isDay;
}

s32 func_80AADAA0(EnMm* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 sp1C = 1;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_DONE_FADING:
            break;
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                if (play->msgCtx.choiceIndex == 0) {
                    player->actor.textId = 0x202D;
                    this->unk_254 &= ~1;
                    EnMm_ChangeAnim(this, RM_ANIM_HAPPY, &this->curAnimIndex);
                } else {
                    player->actor.textId = 0x202C;
                    gSaveContext.infTable[23] |= 0x1000;
                }
                sp1C = 2;
            }
            break;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                Player_UnsetMask(play);
                Item_Give(play, ITEM_SOLD_OUT);
                gSaveContext.itemGetInf[3] |= 0x800;
                Rupees_ChangeBy(500);
                player->actor.textId = 0x202E;
                sp1C = 2;
            }
            break;
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                if ((player->actor.textId == 0x202E) || (player->actor.textId == 0x202C)) {
                    this->unk_254 |= 1;
                    EnMm_ChangeAnim(this, RM_ANIM_SIT_WAIT, &this->curAnimIndex);
                }
                sp1C = 0;
            }
            break;
    }

    return sp1C;
}

s32 EnMm_GetTextId(EnMm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 textId;

    textId = Text_GetFaceReaction(play, 0x1C);

    if (gSaveContext.itemGetInf[3] & 0x800) {
        if (textId == 0) {
            textId = 0x204D;
        }
    } else if (player->currentMask == PLAYER_MASK_BUNNY) {
        textId = (gSaveContext.infTable[23] & 0x1000) ? 0x202B : 0x202A;
    } else if (textId == 0) {
        textId = 0x2029;
    }

    return textId;
}

void func_80AADCD0(EnMm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 yawDiff;
    s16 sp26;
    s16 sp24;

    if (this->unk_1E0 == 2) {
        Message_ContinueTextbox(play, player->actor.textId);
        this->unk_1E0 = 1;
    } else if (this->unk_1E0 == 1) {
        this->unk_1E0 = func_80AADAA0(this, play);
    } else {
        if (Actor_ProcessTalkRequest(&this->actor, play)) {
            this->unk_1E0 = 1;

            if (this->curAnimIndex != 5) {
                if ((this->actor.textId == 0x202A) || (this->actor.textId == 0x202B)) {
                    EnMm_ChangeAnim(this, RM_ANIM_EXCITED, &this->curAnimIndex);
                    func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
                }
            }
        } else {
            Actor_GetScreenPos(play, &this->actor, &sp26, &sp24);
            yawDiff = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y));

            if ((sp26 >= 0) && (sp26 <= 0x140) && (sp24 >= 0) && (sp24 <= 0xF0) && (yawDiff <= 17152.0f) &&
                (this->unk_1E0 != 3) && func_8002F2CC(&this->actor, play, 100.0f)) {
                this->actor.textId = EnMm_GetTextId(this, play);
            }
        }
    }
}

s32 EnMm_GetPointCount(Path* pathList, s32 pathNum) {
    return (pathList + pathNum)->count;
}

s32 func_80AADE60(Path* pathList, Vec3f* pos, s32 pathNum, s32 waypoint) {
    Vec3s* pointPos;
    pointPos = &((Vec3s*)SEGMENTED_TO_VIRTUAL((pathList + pathNum)->points))[waypoint];

    pos->x = pointPos->x;
    pos->y = pointPos->y;
    pos->z = pointPos->z;

    return 0;
}

s32 func_80AADEF0(EnMm* this, PlayState* play) {
    f32 xDiff;
    f32 zDiff;
    Vec3f waypointPos;
    s32 phi_a2;
    s32 phi_v1;

    func_80AADE60(play->setupPathList, &waypointPos, this->path, this->waypoint);

    xDiff = waypointPos.x - this->actor.world.pos.x;
    zDiff = waypointPos.z - this->actor.world.pos.z;

    this->yawToWaypoint = (s32)(Math_FAtan2F(xDiff, zDiff) * (0x8000 / M_PI));
    this->distToWaypoint = sqrtf(SQ(xDiff) + SQ(zDiff));

    while ((this->distToWaypoint <= 10.44f) && (this->unk_1E8 != 0)) {
        this->waypoint += sPathInfo[this->unk_1E8].unk_00;

        phi_a2 = sPathInfo[this->unk_1E8].unk_08;

        switch (phi_a2) {
            case 0:
                phi_a2 = 0;
                break;
            case 1:
                phi_a2 = EnMm_GetPointCount(play->setupPathList, this->path) - 1;
                break;
            case 2:
                phi_a2 = this->unk_1F0;
                break;
        }

        phi_v1 = sPathInfo[this->unk_1E8].unk_0C;

        switch (phi_v1) {
            case 0:
                phi_v1 = 0;
                break;
            case 1:
                phi_v1 = EnMm_GetPointCount(play->setupPathList, this->path) - 1;
                break;
            case 2:
                phi_v1 = this->unk_1F0;
                break;
        }

        if ((sPathInfo[this->unk_1E8].unk_00 >= 0 && (this->waypoint < phi_a2 || phi_v1 < this->waypoint)) ||
            (sPathInfo[this->unk_1E8].unk_00 < 0 && (phi_a2 < this->waypoint || this->waypoint < phi_v1))) {
            this->unk_1E8 = sPathInfo[this->unk_1E8].unk_04;
            this->waypoint = sPathInfo[this->unk_1E8].unk_08;
        }

        func_80AADE60(play->setupPathList, &waypointPos, this->path, this->waypoint);

        xDiff = waypointPos.x - this->actor.world.pos.x;
        zDiff = waypointPos.z - this->actor.world.pos.z;

        this->yawToWaypoint = (s32)(Math_FAtan2F(xDiff, zDiff) * (0x8000 / M_PI));
        this->distToWaypoint = sqrtf(SQ(xDiff) + SQ(zDiff));
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->yawToWaypoint, 1, 2500, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    Math_SmoothStepToF(&this->actor.speedXZ, this->speedXZ, 0.6f, this->distToWaypoint, 0.0f);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);

    return 0;
}

void func_80AAE224(EnMm* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->actionFunc = func_80AAE598;
        this->unk_1E8 = 0;
        this->mouthTexIndex = RM_MOUTH_CLOSED;
        this->unk_254 |= 1;
        this->unk_1E0 = 0;
        this->actor.speedXZ = 0.0f;
        EnMm_ChangeAnim(this, RM_ANIM_SIT_WAIT, &this->curAnimIndex);
    }
}

void func_80AAE294(EnMm* this, PlayState* play) {
    f32 floorYNorm;
    Vec3f dustPos;

    if (!Player_InCsMode(play)) {
        SkelAnime_Update(&this->skelAnime);

        if (this->curAnimIndex == 0) {
            if (((s32)this->skelAnime.curFrame == 1) || ((s32)this->skelAnime.curFrame == 6)) {
                Audio_PlayActorSound2(&this->actor, NA_SE_PL_WALK_GROUND);
            }
        }

        if (this->curAnimIndex == 4) {
            if (((this->skelAnime.curFrame - this->skelAnime.playSpeed < 9.0f) && (this->skelAnime.curFrame >= 9.0f)) ||
                ((this->skelAnime.curFrame - this->skelAnime.playSpeed < 19.0f) &&
                 (this->skelAnime.curFrame >= 19.0f))) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_WALK);
            }
        }

        if (gSaveContext.itemGetInf[3] & 0x800) {
            this->speedXZ = 10.0f;
            this->skelAnime.playSpeed = 2.0f;
        } else {
            this->speedXZ = 3.0f;
            this->skelAnime.playSpeed = 1.0f;
        }

        func_80AADEF0(this, play);

        if (func_80AADA70() == 0) {
            if (this->actor.floorPoly != NULL) {
                floorYNorm = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.y);

                if ((floorYNorm > 0.9848f) || (floorYNorm < -0.9848f)) {
                    if (this->sitTimer > 30) {
                        EnMm_ChangeAnim(this, RM_ANIM_SIT, &this->curAnimIndex);
                        this->actionFunc = func_80AAE224;
                    } else {
                        this->sitTimer++;
                    }
                } else {
                    this->sitTimer = 0;
                }
            }
        }

        if (gSaveContext.itemGetInf[3] & 0x800) {
            dustPos.x = this->actor.world.pos.x;
            dustPos.y = this->actor.world.pos.y;
            dustPos.z = this->actor.world.pos.z;

            if (gSaveContext.gameMode != 3) {
                func_80033480(play, &dustPos, 50.0f, 2, 350, 20, 0);
            }

            if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
                func_8002F71C(play, &this->actor, 3.0f, this->actor.yawTowardsPlayer, 4.0f);
            }
        }
    }
}

void func_80AAE50C(EnMm* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->sitTimer = 0;
        this->actionFunc = func_80AAE294;

        if (gSaveContext.itemGetInf[3] & 0x800) {
            EnMm_ChangeAnim(this, RM_ANIM_SPRINT, &this->curAnimIndex);
            this->mouthTexIndex = RM_MOUTH_CLOSED;
        } else {
            EnMm_ChangeAnim(this, RM_ANIM_RUN, &this->curAnimIndex);
            this->mouthTexIndex = RM_MOUTH_OPEN;
        }

        this->unk_1E8 = 1;
    }
}

void func_80AAE598(EnMm* this, PlayState* play) {
    func_80038290(play, &this->actor, &this->unk_248, &this->unk_24E, this->actor.focus.pos);
    SkelAnime_Update(&this->skelAnime);

    if ((func_80AADA70() != 0) && (this->unk_1E0 == 0)) {
        this->unk_1E0 = 3;
        this->actionFunc = func_80AAE50C;
        this->unk_254 &= ~1;
        EnMm_ChangeAnim(this, RM_ANIM_STAND, &this->curAnimIndex);
    }
}

void EnMm_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnMm* this = (EnMm*)thisx;

    this->actionFunc(this, play);
    func_80AADCD0(this, play);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnMm_Draw(Actor* thisx, PlayState* play) {
    static void* mouthTextures[] = { gRunningManMouthOpenTex, gRunningManMouthClosedTex };
    s32 pad;
    EnMm* this = (EnMm*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(mouthTextures[this->mouthTexIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnMm_OverrideLimbDraw, EnMm_PostLimbDraw, this);

    if (gSaveContext.itemGetInf[3] & 0x800) {
        s32 linkChildObjBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_LINK_CHILD);

        if (linkChildObjBankIndex >= 0) {
            Mtx* mtx;
            Vec3s sp50;
            Mtx* mtx2;

            mtx = Graph_Alloc(play->state.gfxCtx, sizeof(Mtx) * 2);

            Matrix_Put(&this->unk_208);
            mtx2 = MATRIX_NEWMTX(play->state.gfxCtx);

            gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.status[linkChildObjBankIndex].segment);
            gSPSegment(POLY_OPA_DISP++, 0x0B, mtx);
            gSPSegment(POLY_OPA_DISP++, 0x0D, mtx2 - 7);

            sp50.x = 994;
            sp50.y = 3518;
            sp50.z = -13450;

            Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f, -240.0f, &sp50);
            MATRIX_TOMTX(mtx++);

            sp50.x = -994;
            sp50.y = -3518;
            sp50.z = -13450;

            Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f, 240.0f, &sp50);
            MATRIX_TOMTX(mtx);

            gSPDisplayList(POLY_OPA_DISP++, gLinkChildBunnyHoodDL);
            gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.status[this->actor.objBankIndex].segment);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnMm_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMm* this = (EnMm*)thisx;

    if (this->unk_254 & 1) {
        switch (limbIndex) {
            case 8:
                rot->x += this->unk_24E.y;
                rot->y -= this->unk_24E.x;
                break;
            case 15:
                rot->x += this->unk_248.y;
                rot->z += (this->unk_248.x + 0xFA0);
                break;
            default:
                break;
        }
    }

    return 0;
}

void EnMm_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f headOffset = { 200.0f, 800.0f, 0.0f };
    EnMm* this = (EnMm*)thisx;

    if (limbIndex == 15) {
        Matrix_MultVec3f(&headOffset, &this->actor.focus.pos);
        Matrix_Translate(260.0f, 20.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateY(0.0f, MTXMODE_APPLY);
        Matrix_RotateX(0.0f, MTXMODE_APPLY);
        Matrix_RotateZ(4.0f * M_PI / 5.0f, MTXMODE_APPLY);
        Matrix_Translate(-260.0f, 58.0f, 10.0f, MTXMODE_APPLY);
        Matrix_Get(&this->unk_208);
    }
}
