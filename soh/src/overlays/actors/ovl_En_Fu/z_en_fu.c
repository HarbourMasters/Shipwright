/*
 * File: z_en_fu.c
 * Overlay: ovl_En_Fu
 * Description: Windmill Man
 */

#include "z_en_fu.h"
#include "objects/object_fu/object_fu.h"
#include "scenes/indoors/hakasitarelay/hakasitarelay_scene.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_NO_FREEZE_OCARINA)

#define FU_RESET_LOOK_ANGLE (1 << 0)
#define FU_WAIT (1 << 1)

void EnFu_Init(Actor* thisx, PlayState* play);
void EnFu_Destroy(Actor* thisx, PlayState* play);
void EnFu_Update(Actor* thisx, PlayState* play);
void EnFu_Draw(Actor* thisx, PlayState* play);

void EnFu_WaitChild(EnFu* this, PlayState* play);
void func_80A1DA04(EnFu* this, PlayState* play);

void EnFu_WaitAdult(EnFu* this, PlayState* play);
void EnFu_TeachSong(EnFu* this, PlayState* play);
void EnFu_WaitForPlayback(EnFu* this, PlayState* play);
void func_80A1DBA0(EnFu* this, PlayState* play);
void func_80A1DBD4(EnFu* this, PlayState* play);
void func_80A1DB60(EnFu* this, PlayState* play);

const ActorInit En_Fu_InitVars = {
    ACTOR_EN_FU,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_FU,
    sizeof(EnFu),
    (ActorFunc)EnFu_Init,
    (ActorFunc)EnFu_Destroy,
    (ActorFunc)EnFu_Update,
    (ActorFunc)EnFu_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

static Vec3f sMtxSrc = {
    700.0f,
    700.0f,
    0.0f,
};

typedef enum {
    /* 0x00 */ FU_FACE_CALM,
    /* 0x01 */ FU_FACE_MAD
} EnFuFace;

void EnFu_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFu* this = (EnFu*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelanime, &gWindmillManSkel, &gWindmillManPlayStillAnim, this->jointTable,
                       this->morphTable, FU_LIMB_MAX);
    Animation_PlayLoop(&this->skelanime, &gWindmillManPlayStillAnim);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    if (!LINK_IS_ADULT) {
        this->actionFunc = EnFu_WaitChild;
        this->facialExpression = FU_FACE_CALM;
    } else {
        this->actionFunc = EnFu_WaitAdult;
        this->facialExpression = FU_FACE_MAD;
        this->skelanime.playSpeed = 2.0f;
    }
    this->behaviorFlags = 0;
    this->actor.targetMode = 6;
}

void EnFu_Destroy(Actor* thisx, PlayState* play) {
    EnFu* this = (EnFu*)thisx;
    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelanime);
}

s32 func_80A1D94C(EnFu* this, PlayState* play, u16 textID, EnFuActionFunc actionFunc) {
    s16 yawDiff;

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = actionFunc;
        return true;
    }
    this->actor.textId = textID;
    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if ((ABS(yawDiff) < 0x2301) && (this->actor.xzDistToPlayer < 100.0f)) {
        func_8002F2CC(&this->actor, play, 100.0f);
    } else {
        this->behaviorFlags |= FU_RESET_LOOK_ANGLE;
    }
    return false;
}

void func_80A1DA04(EnFu* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->behaviorFlags &= ~FU_WAIT;
        this->actionFunc = EnFu_WaitChild;

        if (this->skelanime.animation == &gWindmillManPlayAndMoveHeadAnim) {
            Animation_Change(&this->skelanime, &gWindmillManPlayStillAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gWindmillManPlayStillAnim), ANIMMODE_ONCE, -4.0f);
        }
    }
}

void EnFu_WaitChild(EnFu* this, PlayState* play) {
    u16 textID = Text_GetFaceReaction(play, 0xB);

    if (textID == 0) {
        textID = (Flags_GetEventChkInf(EVENTCHKINF_DRAINED_WELL_IN_KAKARIKO)) ? 0x5033 : 0x5032;
    }

    // if ACTOR_FLAG_PLAYER_TALKED_TO is set and textID is 0x5033, change animation
    // if func_80A1D94C returns 1, actionFunc is set to func_80A1DA04
    if (func_80A1D94C(this, play, textID, func_80A1DA04)) {
        if (textID == 0x5033) {
            Animation_Change(&this->skelanime, &gWindmillManPlayAndMoveHeadAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gWindmillManPlayAndMoveHeadAnim), ANIMMODE_ONCE, -4.0f);
        }
    }
}

void GivePlayerRandoRewardSongOfStorms(EnFu* windmillGuy, PlayState* play, RandomizerCheck check) {
    if (windmillGuy->actor.parent != NULL && windmillGuy->actor.parent->id == GET_PLAYER(play)->actor.id &&
        !Flags_GetTreasure(play, 0x1F)) {
        Flags_SetTreasure(play, 0x1F);
        windmillGuy->actionFunc = func_80A1DBD4;
    } else if (!Flags_GetTreasure(play, 0x1F)) {
        GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(check, RG_SONG_OF_STORMS);
        GiveItemEntryFromActor(&windmillGuy->actor, play, getItemEntry, 10000.0f, 100.0f);
    }
}

void func_WaitForSongGive(EnFu* this, PlayState* play) {
    GivePlayerRandoRewardSongOfStorms(this, play, RC_SONG_FROM_WINDMILL);
}

void func_80A1DB60(EnFu* this, PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        this->actionFunc = EnFu_WaitAdult;
        Flags_SetEventChkInf(EVENTCHKINF_LEARNED_SONG_OF_STORMS);
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    }

    if (IS_RANDO) {
        this->actionFunc = func_WaitForSongGive;
    }
}

void func_80A1DBA0(EnFu* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actionFunc = EnFu_WaitAdult;
    }
}

void func_80A1DBD4(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (IS_RANDO && (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING)) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_03;
    }

    if (play->msgCtx.ocarinaMode >= OCARINA_MODE_04) {
        this->actionFunc = EnFu_WaitAdult;
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        this->actor.flags &= ~ACTOR_FLAG_WILL_TALK;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        this->actionFunc = func_80A1DB60;
        this->actor.flags &= ~ACTOR_FLAG_WILL_TALK;

        if (!IS_RANDO) {
            play->csCtx.segment = SEGMENTED_TO_VIRTUAL(gSongOfStormsCs);
            gSaveContext.cutsceneTrigger = 1;
            Item_Give(play, ITEM_SONG_STORMS);
        }

        play->msgCtx.ocarinaMode = OCARINA_MODE_00;
        Flags_SetEventChkInf(EVENTCHKINF_PLAYED_SONG_OF_STORMS_IN_WINDMILL);
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_02) {
        player->stateFlags2 &= ~0x1000000;
        this->actionFunc = EnFu_WaitAdult;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
        player->stateFlags2 |= 0x800000;
    }
}

void EnFu_WaitForPlayback(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags2 |= 0x800000;
    // if dialog state is 7, player has played back the song
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_SONG_DEMO_DONE) {
        func_8010BD58(play, OCARINA_ACTION_PLAYBACK_STORMS);
        this->actionFunc = func_80A1DBD4;
    }
}

void EnFu_TeachSong(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags2 |= 0x800000;
    // if dialog state is 2, start song demonstration
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->behaviorFlags &= ~FU_WAIT;
        Audio_OcaSetInstrument(4); // seems to be related to setting instrument type
        func_8010BD58(play, OCARINA_ACTION_TEACH_STORMS);
        this->actionFunc = EnFu_WaitForPlayback;
    }
}

void EnFu_WaitAdult(EnFu* this, PlayState* play) {
    static s16 yawDiff;
    Player* player = GET_PLAYER(play);

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    if ((Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SONG_OF_STORMS))) {
        func_80A1D94C(this, play, 0x508E, func_80A1DBA0);
    } else if (player->stateFlags2 & 0x1000000) {
        this->actor.textId = 0x5035;
        Message_StartTextbox(play, this->actor.textId, NULL);
        this->actionFunc = IS_RANDO ? func_80A1DBD4 : EnFu_TeachSong;
        this->behaviorFlags |= FU_WAIT;
    } else if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = func_80A1DBA0;
    } else if (ABS(yawDiff) < 0x2301) {
        if (this->actor.xzDistToPlayer < 100.0f) {
            this->actor.textId = 0x5034;
            func_8002F2CC(&this->actor, play, 100.0f);
            player->stateFlags2 |= 0x800000;
        }
    }
}

void EnFu_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFu* this = (EnFu*)thisx;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    if ((!(this->behaviorFlags & FU_WAIT)) && (SkelAnime_Update(&this->skelanime) != 0)) {
        Animation_Change(&this->skelanime, this->skelanime.animation, 1.0f, 0.0f,
                         Animation_GetLastFrame(this->skelanime.animation), ANIMMODE_ONCE, 0.0f);
    }
    this->actionFunc(this, play);
    if ((this->behaviorFlags & FU_RESET_LOOK_ANGLE)) {
        Math_SmoothStepToS(&this->lookAngleOffset.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->lookAngleOffset.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_2A2.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_2A2.y, 0, 6, 6200, 100);
        this->behaviorFlags &= ~FU_RESET_LOOK_ANGLE;
    } else {
        func_80038290(play, &this->actor, &this->lookAngleOffset, &this->unk_2A2, this->actor.focus.pos);
    }
}

s32 EnFu_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnFu* this = (EnFu*)thisx;
    s32 pad;

    if (limbIndex == FU_LIMB_UNK) {
        return false;
    }
    switch (limbIndex) {
        case FU_LIMB_HEAD:
            rot->x += this->lookAngleOffset.y;
            rot->z += this->lookAngleOffset.x;
            break;
        case FU_LIMB_CHEST_MUSIC_BOX:
            break;
    }

    if (!(this->behaviorFlags & FU_WAIT)) {
        return false;
    }

    if (limbIndex == FU_LIMB_CHEST_MUSIC_BOX) {
        rot->y += (Math_SinS((play->state.frames * (limbIndex * 50 + 0x814))) * 200.0f);
        rot->z += (Math_CosS((play->state.frames * (limbIndex * 50 + 0x940))) * 200.0f);
    }
    return false;
}

void EnFu_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnFu* this = (EnFu*)thisx;

    if (limbIndex == FU_LIMB_HEAD) {
        Matrix_MultVec3f(&sMtxSrc, &this->actor.focus.pos);
    }
}

void EnFu_Draw(Actor* thisx, PlayState* play) {
    static void* sEyesSegments[] = { gWindmillManEyeClosedTex, gWindmillManEyeAngryTex };
    static void* sMouthSegments[] = { gWindmillManMouthOpenTex, gWindmillManMouthAngryTex };
    s32 pad;
    EnFu* this = (EnFu*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_37Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyesSegments[this->facialExpression]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sMouthSegments[this->facialExpression]));
    SkelAnime_DrawSkeletonOpa(play, &this->skelanime, EnFu_OverrideLimbDraw, EnFu_PostLimbDraw, this);

    CLOSE_DISPS(play->state.gfxCtx);
}
