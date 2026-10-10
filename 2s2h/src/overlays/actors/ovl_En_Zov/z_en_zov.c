/*
 * File: z_en_zov.c
 * Overlay: ovl_En_Zov
 * Description: Zora Vocalist - Lulu
 */

#include "z_en_zov.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnZov_Init(Actor* thisx, PlayState* play);
void EnZov_Destroy(Actor* thisx, PlayState* play);
void EnZov_Update(Actor* thisx, PlayState* play);
void EnZov_Draw(Actor* thisx, PlayState* play);

void EnZov_ChangeAnimMorph(EnZov* this, s16 animIndex, u8 animMode);
void func_80BD187C(EnZov* this, PlayState* play);
void func_80BD19FC(EnZov* this, PlayState* play);
void func_80BD1BF0(EnZov* this, PlayState* play);
void func_80BD1C38(EnZov* this, PlayState* play);
void func_80BD1C84(EnZov* this, PlayState* play);
void func_80BD1D94(EnZov* this, PlayState* play);
void func_80BD1DB8(EnZov* this, PlayState* play);
void func_80BD1F1C(EnZov* this, PlayState* play);
s32 EnZov_ValidatePictograph(PlayState* play, Actor* thisx);

ActorProfile En_Zov_Profile = {
    /**/ ACTOR_EN_ZOV,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZOV,
    /**/ sizeof(EnZov),
    /**/ EnZov_Init,
    /**/ EnZov_Destroy,
    /**/ EnZov_Update,
    /**/ EnZov_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 40, 0, { 0, 0, 0 } },
};

typedef enum LuluAnimation {
    /*  -1 */ LULU_ANIM_NONE = -1,
    /* 0x0 */ LULU_ANIM_LOOK_DOWN,
    /* 0x1 */ LULU_ANIM_PUT_HANDS_DOWN,
    /* 0x2 */ LULU_ANIM_LOOK_FORWARD_AND_DOWN,
    /* 0x3 */ LULU_ANIM_LOOK_AROUND,
    /* 0x4 */ LULU_ANIM_ANGLE_HEAD,
    /* 0x5 */ LULU_ANIM_NOD,
    /* 0x6 */ LULU_ANIM_PUT_HANDS_DOWN_2,
    /* 0x7 */ LULU_ANIM_LOOK_AROUND_2,
    /* 0x8 */ LULU_ANIM_SING_START,
    /* 0x9 */ LULU_ANIM_SING_LOOP,
    /* 0xA */ LULU_ANIM_LOOK_FORWARDS_AND_LEFT,
    /* 0xB */ LULU_ANIM_LOOK_LEFT_LOOP,
    /* 0xC */ LULU_ANIM_TURN_AND_WALK,
    /* 0xD */ LULU_ANIM_WALK_LOOP,
    /* 0xE */ LULU_ANIM_MAX
} LuluAnimation;

static AnimationHeader* sAnimations[LULU_ANIM_MAX] = {
    &gLuluLookDownAnim,           // LULU_ANIM_LOOK_DOWN
    &gLuluPutHandsDownAnim,       // LULU_ANIM_PUT_HANDS_DOWN
    &gLuluLookForwardAndDownAnim, // LULU_ANIM_LOOK_FORWARD_AND_DOWN
    &gLuluLookAroundAnim,         // LULU_ANIM_LOOK_AROUND
    &gLuluAngleHeadAnim,          // LULU_ANIM_ANGLE_HEAD
    &gLuluNodAnim,                // LULU_ANIM_NOD
    &gLuluPutHandsDownAnim,       // LULU_ANIM_PUT_HANDS_DOWN_2
    &gLuluLookAroundAnim,         // LULU_ANIM_LOOK_AROUND_2
    &gLuluSingStartAnim,          // LULU_ANIM_SING_START
    &gLuluSingLoopAnim,           // LULU_ANIM_SING_LOOP
    &gLuluLookForwardAndLeftAnim, // LULU_ANIM_LOOK_FORWARDS_AND_LEFT
    &gLuluLookLeftLoopAnim,       // LULU_ANIM_LOOK_LEFT_LOOP
    &gLuluTurnAndWalkAnim,        // LULU_ANIM_TURN_AND_WALK
    &gLuluWalkLoopAnim,           // LULU_ANIM_WALK_LOOP
};

void EnZov_Init(Actor* thisx, PlayState* play) {
    EnZov* this = (EnZov*)thisx;

    ActorShape_Init(&this->picto.actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    this->picto.actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->picto.actor, 0.01f);
    Collider_InitAndSetCylinder(play, &this->collider, &this->picto.actor, &sCylinderInit);
    SkelAnime_InitFlex(play, &this->skelAnime, &gLuluSkel, &gLuluLookDownAnim, this->jointTable, this->morphTable,
                       LULU_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gLuluLookDownAnim);

    this->unk_320 = 0;
    this->csIdIndex = -1;
    this->cueId = -1;
    this->csIdList[0] = this->picto.actor.csId;
    this->csIdList[1] = CS_ID_GLOBAL_TALK;
    this->animIndex = LULU_ANIM_LOOK_DOWN;
    this->actionFunc = func_80BD1C84;
    this->picto.validationFunc = EnZov_ValidatePictograph;

    Math_Vec3f_Copy(&this->unk_2FC, &this->picto.actor.world.pos);
    Math_Vec3f_Copy(&this->unk_308, &this->picto.actor.world.pos);
    Math_Vec3f_Copy(&this->unk_314, &this->picto.actor.world.pos);

    switch (ENZOV_GET_F(&this->picto.actor)) {
        case ENZOV_F_1:
            this->actionFunc = func_80BD1F1C;
            EnZov_ChangeAnimMorph(this, LULU_ANIM_SING_LOOP, ANIMMODE_LOOP);
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
                Actor_Kill(&this->picto.actor);
                return;
            }
            break;

        case ENZOV_F_2:
            this->actionFunc = func_80BD1C38;
            this->picto.actor.shape.shadowDraw = NULL;
            break;

        default:
            this->unk_320 |= 2;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE) || CHECK_WEEKEVENTREG(WEEKEVENTREG_53_20)) {
                Actor_Kill(&this->picto.actor);
            }
            break;
    }
}

void EnZov_Destroy(Actor* thisx, PlayState* play) {
    EnZov* this = (EnZov*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80BD13DC(EnZov* this) {
    if (this->csIdIndex != -1) {
        if (CutsceneManager_GetCurrentCsId() == this->csIdList[this->csIdIndex]) {
            CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
        }
        this->csIdIndex = -1;
    }
}

void func_80BD1440(EnZov* this, s16 csIdIndex) {
    func_80BD13DC(this);
    this->csIdIndex = csIdIndex;
}

void EnZov_ChangeAnim(EnZov* this, s16 animIndex, u8 animMode, f32 morphFrames) {
    f32 startFrame;

    if (((animIndex != this->animIndex) || (animMode != ANIMMODE_LOOP)) && (animIndex > LULU_ANIM_NONE) &&
        (animIndex < LULU_ANIM_MAX)) {
        switch (animIndex) {
            case LULU_ANIM_PUT_HANDS_DOWN_2:
                startFrame = 30.0f;
                break;

            case LULU_ANIM_LOOK_AROUND_2:
                startFrame = 57.0f;
                break;

            default:
                startFrame = 0.0f;
                break;
        }
        Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, startFrame,
                         Animation_GetLastFrame(sAnimations[animIndex]), animMode, morphFrames);
        this->animIndex = animIndex;
    }
}

void EnZov_ChangeAnimMorph(EnZov* this, s16 animIndex, u8 animMode) {
    EnZov_ChangeAnim(this, animIndex, animMode, 5.0f);
}

s32 func_80BD15A4(EnZov* this, PlayState* play) {
    if ((this->picto.actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->picto.actor, 0x3000, play) &&
        Actor_IsFacingPlayer(&this->picto.actor, 0x3000)) {
        return true;
    }
    return false;
}

void func_80BD160C(EnZov* this, PlayState* play) {
    s32 textId = 0;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_53_20)) {
        this->unk_320 &= ~2;
        if (GET_PLAYER_FORM != PLAYER_FORM_ZORA) {
            textId = 0x1024;
            if ((this->animIndex == LULU_ANIM_LOOK_DOWN) || (this->animIndex == LULU_ANIM_ANGLE_HEAD)) {
                EnZov_ChangeAnimMorph(this, LULU_ANIM_ANGLE_HEAD, ANIMMODE_ONCE);
            } else {
                EnZov_ChangeAnimMorph(this, LULU_ANIM_PUT_HANDS_DOWN_2, ANIMMODE_ONCE);
            }
        } else if (this->unk_320 & 4) {
            textId = 0x1023;
        } else {
            textId = 0x1022;
            this->unk_320 |= 4;
            EnZov_ChangeAnimMorph(this, LULU_ANIM_LOOK_AROUND, ANIMMODE_ONCE);
        }
    } else if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
        EnZov_ChangeAnimMorph(this, LULU_ANIM_LOOK_FORWARD_AND_DOWN, ANIMMODE_ONCE);
        this->actionFunc = func_80BD19FC;
        this->unk_324 = 10;
        func_80BD1440(this, 0);
    } else {
        textId = 0x1020;
        EnZov_ChangeAnimMorph(this, LULU_ANIM_NOD, ANIMMODE_ONCE);
    }

    this->unk_320 |= 1;
    if (textId != 0) {
        Message_StartTextbox(play, textId, &this->picto.actor);
    }
}

void func_80BD1764(EnZov* this) {
    if (SkelAnime_Update(&this->skelAnime)) {
        switch (this->animIndex) {
            case LULU_ANIM_PUT_HANDS_DOWN:
            case LULU_ANIM_PUT_HANDS_DOWN_2:
                EnZov_ChangeAnimMorph(this, LULU_ANIM_PUT_HANDS_DOWN_2, ANIMMODE_ONCE);
                break;

            case LULU_ANIM_LOOK_AROUND:
            case LULU_ANIM_LOOK_AROUND_2:
                EnZov_ChangeAnimMorph(this, LULU_ANIM_LOOK_AROUND_2, ANIMMODE_ONCE);
                break;

            case LULU_ANIM_ANGLE_HEAD:
                EnZov_ChangeAnimMorph(this, LULU_ANIM_LOOK_DOWN, ANIMMODE_LOOP);
                break;

            case LULU_ANIM_SING_START:
                EnZov_ChangeAnimMorph(this, LULU_ANIM_SING_LOOP, ANIMMODE_LOOP);
                break;

            case LULU_ANIM_LOOK_FORWARDS_AND_LEFT:
                EnZov_ChangeAnimMorph(this, LULU_ANIM_LOOK_LEFT_LOOP, ANIMMODE_LOOP);
                break;

            case LULU_ANIM_TURN_AND_WALK:
                EnZov_ChangeAnimMorph(this, LULU_ANIM_WALK_LOOP, ANIMMODE_LOOP);
                break;

            default:
                EnZov_ChangeAnimMorph(this, LULU_ANIM_LOOK_DOWN, ANIMMODE_LOOP);
                this->unk_320 &= ~1;
                break;
        }

        SkelAnime_Update(&this->skelAnime);
    }
}

void func_80BD187C(EnZov* this, PlayState* play) {
    func_80BD1764(this);

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x1022:
                        Message_ContinueTextbox(play, 0x1023);
                        break;

                    case 0x1023:
                        if ((this->animIndex != LULU_ANIM_PUT_HANDS_DOWN_2) &&
                            (this->animIndex != LULU_ANIM_PUT_HANDS_DOWN)) {
                            EnZov_ChangeAnimMorph(this, LULU_ANIM_PUT_HANDS_DOWN, ANIMMODE_ONCE);
                        }
                        Message_CloseTextbox(play);
                        this->actionFunc = func_80BD1C84;
                        break;

                    case 0x1024:
                        if (this->animIndex != LULU_ANIM_PUT_HANDS_DOWN_2) {
                            EnZov_ChangeAnimMorph(this, LULU_ANIM_LOOK_DOWN, ANIMMODE_LOOP);
                        }
                        Message_CloseTextbox(play);
                        this->actionFunc = func_80BD1C84;
                        break;

                    default:
                        this->unk_320 &= ~1;
                        EnZov_ChangeAnimMorph(this, LULU_ANIM_LOOK_DOWN, ANIMMODE_LOOP);
                        Message_CloseTextbox(play);
                        this->actionFunc = func_80BD1C84;
                        break;
                }
            }
            break;

        case TEXT_STATE_CLOSING:
            Message_CloseTextbox(play);
            this->actionFunc = func_80BD1C84;
            this->unk_320 &= ~1;
            EnZov_ChangeAnimMorph(this, LULU_ANIM_LOOK_DOWN, ANIMMODE_LOOP);
            break;
    }
}

void func_80BD19FC(EnZov* this, PlayState* play) {
    func_80BD1764(this);
    if (this->animIndex == LULU_ANIM_LOOK_DOWN) {
        if (!(this->unk_320 & 2)) {
            this->unk_320 |= 2;
            this->unk_2EE = 3;
        }

        if (this->unk_324 > 0) {
            this->unk_324--;
        } else {
            func_80BD13DC(this);
            Message_StartTextbox(play, 0x1021, &this->picto.actor);
            ((EnElf*)(GET_PLAYER(play)->tatlActor))->unk_264 |= 4;
            Actor_ChangeFocus(&this->picto.actor, play, GET_PLAYER(play)->tatlActor);
            this->actionFunc = func_80BD187C;
        }
    } else if (Animation_OnFrame(&this->skelAnime, 10.0f)) {
        this->unk_320 &= ~2;
        this->unk_2EE = 3;
    }
}

s32 func_80BD1AE0(EnZov* this, PlayState* play) {
    func_80BD1764(this);

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_504)) {
        s16 cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_504)]->id;

        if (this->cueId != cueId) {
            this->cueId = cueId;
            switch (this->cueId) {
                case 1:
                    EnZov_ChangeAnimMorph(this, LULU_ANIM_LOOK_DOWN, ANIMMODE_LOOP);
                    break;

                case 2:
                    EnZov_ChangeAnimMorph(this, LULU_ANIM_SING_START, ANIMMODE_ONCE);
                    this->unk_320 |= 0x10;
                    break;

                case 3:
                    EnZov_ChangeAnim(this, LULU_ANIM_LOOK_FORWARDS_AND_LEFT, ANIMMODE_ONCE, 0.0f);
                    break;

                case 4:
                    EnZov_ChangeAnimMorph(this, LULU_ANIM_TURN_AND_WALK, ANIMMODE_ONCE);
                    break;
            }
        }
        return true;
    }

    return false;
}

void func_80BD1BF0(EnZov* this, PlayState* play) {
    if (!func_80BD1AE0(this, play)) {
        EnZov_ChangeAnimMorph(this, LULU_ANIM_LOOK_DOWN, ANIMMODE_LOOP);
        this->actionFunc = func_80BD1C84;
    }
}

void func_80BD1C38(EnZov* this, PlayState* play) {
    if (func_80BD1AE0(this, play)) {
        Cutscene_ActorTranslateAndYaw(&this->picto.actor, play, Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_504));
    }
}

void func_80BD1C84(EnZov* this, PlayState* play) {
    func_80BD1764(this);

    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        this->actionFunc = func_80BD187C;
        func_80BD160C(this, play);
    } else if (func_80BD15A4(this, play)) {
        Actor_OfferTalk(&this->picto.actor, play, 120.0f);
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_504)) {
        this->actionFunc = func_80BD1BF0;
        func_80BD1BF0(this, play);
    }
}

void func_80BD1D30(EnZov* this, PlayState* play) {
    u16 textId;

    if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_79_01)) {
            textId = 0x1032;
        } else {
            textId = 0x1033;
        }
    } else {
        textId = 0x1031;
    }
    Message_StartTextbox(play, textId, &this->picto.actor);
}

void func_80BD1D94(EnZov* this, PlayState* play) {
    func_80BD1764(this);
}

void func_80BD1DB8(EnZov* this, PlayState* play) {
    func_80BD1764(this);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 2, 0x1000, 0x200);
    this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x1033:
            case 0x1034:
            case 0x1035:
            case 0x1036:
            case 0x1037:
            case 0x1038:
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x1039:
                play->nextEntrance = play->setupExitList[ENZOV_GET_FE00(&this->picto.actor)];
                play->transitionType = TRANS_TYPE_FADE_WHITE_FAST;
                play->transitionTrigger = TRANS_TRIGGER_START;
                SET_WEEKEVENTREG(WEEKEVENTREG_78_01);
                this->actionFunc = func_80BD1D94;
                play->msgCtx.msgLength = 0;
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 20);
                break;

            default:
                Message_CloseTextbox(play);
                this->actionFunc = func_80BD1F1C;
                break;
        }
    }
}

void func_80BD1F1C(EnZov* this, PlayState* play) {
    func_80BD1764(this);

    if (this->picto.actor.home.rot.y != this->picto.actor.shape.rot.y) {
        Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.home.rot.y, 2, 0x1000, 0x200);
        this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;
    }

    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        this->actionFunc = func_80BD1DB8;
        func_80BD1D30(this, play);
    } else if (func_80BD15A4(this, play)) {
        Actor_OfferTalk(&this->picto.actor, play, 120.0f);
    }
}

s32 EnZov_ValidatePictograph(PlayState* play, Actor* thisx) {
    s32 ret;
    EnZov* this = (EnZov*)thisx;

    ret = Snap_ValidatePictograph(play, &this->picto.actor, PICTO_VALID_LULU_HEAD, &this->picto.actor.focus.pos,
                                  &this->picto.actor.shape.rot, 10.0f, 300.0f, -1);
    ret |= Snap_ValidatePictograph(play, &this->picto.actor, PICTO_VALID_LULU_RIGHT_ARM, &this->unk_308,
                                   &this->picto.actor.shape.rot, 50.0f, 160.0f, 0x3000);
    ret |= Snap_ValidatePictograph(play, &this->picto.actor, PICTO_VALID_LULU_LEFT_ARM, &this->unk_314,
                                   &this->picto.actor.shape.rot, 50.0f, 160.0f, 0x3000);
    return ret;
}

void EnZov_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZov* this = (EnZov*)thisx;

    Actor_MoveWithGravity(&this->picto.actor);
    Collider_UpdateCylinder(&this->picto.actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->picto.actor, 10.0f, 10.0f, 10.0f, UPDBGCHECKINFO_FLAG_4);

    this->actionFunc(this, play);

    if (!Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_504)) {
        this->unk_320 &= ~0x10;
    }

    if ((this->unk_320 & 1) && func_80BD15A4(this, play)) {
        Actor_TrackPlayer(play, &this->picto.actor, &this->headRot, &this->torsoRot, this->picto.actor.focus.pos);
    } else {
        if ((this->unk_320 & 0x10) && (this->animIndex == LULU_ANIM_LOOK_DOWN)) {
            Math_SmoothStepToS(&this->headRot.x, -0x1B58, 6, 0x1838, 0x64);
        } else {
            Math_SmoothStepToS(&this->headRot.x, 0, 6, 0x1838, 0x64);
        }
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.y, 0, 6, 0x1838, 0x64);
    }

    if (DECR(this->unk_2EE) == 0) {
        this->unk_2EE = Rand_S16Offset(60, 60);
    }

    this->unk_2EC = this->unk_2EE;
    if (this->unk_2EC >= 3) {
        this->unk_2EC = 0;
    }

    if ((this->csIdIndex != -1) && (CutsceneManager_GetCurrentCsId() != this->csIdList[this->csIdIndex])) {
        if ((this->csIdIndex == 0) && (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK)) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
        } else if (CutsceneManager_IsNext(this->csIdList[this->csIdIndex])) {
            CutsceneManager_Start(this->csIdList[this->csIdIndex], &this->picto.actor);
        } else {
            CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
        }
    }
}

s32 EnZov_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnZov* this = (EnZov*)thisx;

    if (limbIndex == LULU_LIMB_HEAD) {
        rot->x += this->headRot.y;
        if ((this->unk_320 & 0x10) && (this->animIndex == LULU_ANIM_LOOK_DOWN)) {
            rot->z += this->headRot.x;
        }
    }

    if (limbIndex == LULU_LIMB_TORSO) {
        rot->x += this->torsoRot.y;
    }
    return false;
}

void EnZov_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80BD2744 = { 400.0f, 600.0f, 0.0f };
    static Vec3f D_80BD2750 = { 400.0f, 600.0f, 0.0f };
    EnZov* this = (EnZov*)thisx;

    if (limbIndex == LULU_LIMB_HEAD) {
        Matrix_MultVec3f(&D_80BD2744, &this->picto.actor.focus.pos);
        Math_Vec3f_Copy(&this->unk_2FC, &this->picto.actor.focus.pos);
        this->unk_2FC.y += 10.0f;
    }

    if (limbIndex == LULU_LIMB_RIGHT_UPPER_ARM) {
        Matrix_MultVec3f(&D_80BD2750, &this->unk_308);
    }

    if (limbIndex == LULU_LIMB_LEFT_UPPER_ARM) {
        Matrix_MultVec3f(&D_80BD2750, &this->unk_314);
    }
}

void EnZov_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = { gLuluEyeOpenTex, gLuluEyeHalfTex, gLuluEyeClosedTex };
    static TexturePtr sMouthTextures[] = { gLuluMouthClosedTex, gLuluMouthOpenTex };
    static s8 D_80BD2770[] = {
        1, 2, 1, 0, 0, 1, 2, 1,
    };
    EnZov* this = (EnZov*)thisx;
    Gfx* gfx;
    s32 curFrame;
    s32 phi_a1;
    u8 phi_v1;

    if (1) {}

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    phi_a1 = 0;
    curFrame = this->skelAnime.curFrame;
    phi_v1 = this->unk_2EC;

    switch (this->animIndex) {
        case LULU_ANIM_LOOK_DOWN:
            if ((this->unk_2EC == 0) && !(this->unk_320 & 0x10)) {
                phi_v1 = 1;
            }
            break;

        case LULU_ANIM_NOD:
            if (this->unk_2EC == 0) {
                phi_v1 = 1;
            }
            break;

        case LULU_ANIM_LOOK_FORWARD_AND_DOWN:
            if (curFrame < 23) {
                phi_v1 = 0;
            } else if (curFrame >= 26) {
                phi_v1 = 1;
            } else if (curFrame == 24) {
                phi_v1 = 0;
            } else {
                phi_v1 = 1;
            }
            break;

        case LULU_ANIM_LOOK_AROUND:
        case LULU_ANIM_LOOK_AROUND_2:
            if (curFrame <= 50) {
                if (curFrame < 43) {
                    phi_v1 = 0;
                } else {
                    phi_v1 = D_80BD2770[(curFrame - 43) & 3];
                }
            }
            phi_a1 = 1;
            break;

        case LULU_ANIM_ANGLE_HEAD:
            if ((curFrame < 14) || (curFrame >= 24)) {
                phi_v1 = 1;
            } else {
                phi_v1 = D_80BD2770[4 + ((curFrame - 14) & 3)];
            }
            phi_a1 = 1;
            break;

        case LULU_ANIM_SING_START:
            phi_v1 = 0;
            break;

        case LULU_ANIM_SING_LOOP:
            phi_v1 = 0;
            phi_a1 = 1;
            break;
    }

    gfx = POLY_OPA_DISP;
    gSPSegment(&gfx[0], 0x09, Lib_SegmentedToVirtual(sEyeTextures[phi_v1]));
    gSPSegment(&gfx[1], 0x08, Lib_SegmentedToVirtual(sMouthTextures[phi_a1]));
    POLY_OPA_DISP = &gfx[2];

    CLOSE_DISPS(play->state.gfxCtx);

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnZov_OverrideLimbDraw, EnZov_PostLimbDraw, &this->picto.actor);
}
