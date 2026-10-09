/*
 * File: z_en_zob.c
 * Overlay: ovl_En_Zob
 * Description: Zora Bassist Japas
 */

#include "z_en_zob.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnZob_Init(Actor* thisx, PlayState* play);
void EnZob_Destroy(Actor* thisx, PlayState* play);
void EnZob_Update(Actor* thisx, PlayState* play);
void EnZob_Draw(Actor* thisx, PlayState* play);

void EnZob_ChangeAnim(EnZob* this, s16 animIndex, u8 animMode);
void func_80B9FD24(EnZob* this, PlayState* play);
void func_80B9FDDC(EnZob* this, PlayState* play);
void func_80B9FE1C(EnZob* this, PlayState* play);
void func_80B9FE5C(EnZob* this, PlayState* play);
void func_80B9FF20(EnZob* this, PlayState* play);
void func_80B9FF80(EnZob* this, PlayState* play);
void func_80BA005C(EnZob* this, PlayState* play);
void func_80BA00BC(EnZob* this, PlayState* play);
void func_80BA0318(EnZob* this, PlayState* play);
void func_80BA0374(EnZob* this, PlayState* play);
void func_80BA0610(EnZob* this, PlayState* play);
void func_80BA06BC(EnZob* this, PlayState* play);
void func_80BA0728(EnZob* this, PlayState* play);
void func_80BA09E0(EnZob* this, PlayState* play);
void func_80BA0A04(EnZob* this, PlayState* play);
void func_80BA0AD8(EnZob* this, PlayState* play);
void func_80BA0BB4(EnZob* this, PlayState* play);
void func_80BA0C14(EnZob* this, PlayState* play);
void func_80BA0CF4(EnZob* this, PlayState* play);

ActorProfile En_Zob_Profile = {
    /**/ ACTOR_EN_ZOB,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZOB,
    /**/ sizeof(EnZob),
    /**/ EnZob_Init,
    /**/ EnZob_Destroy,
    /**/ EnZob_Update,
    /**/ EnZob_Draw,
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
    { 30, 40, 0, { 0, 0, 0 } },
};

typedef enum EnZobAnimation {
    /* 0 */ ENZOB_ANIM_0,
    /* 1 */ ENZOB_ANIM_1,
    /* 2 */ ENZOB_ANIM_2,
    /* 3 */ ENZOB_ANIM_3,
    /* 4 */ ENZOB_ANIM_4,
    /* 5 */ ENZOB_ANIM_5,
    /* 6 */ ENZOB_ANIM_6,
    /* 7 */ ENZOB_ANIM_7,
    /* 8 */ ENZOB_ANIM_8,
    /* 9 */ ENZOB_ANIM_MAX
} EnZobAnimation;

static AnimationHeader* sAnimations[ENZOB_ANIM_MAX] = {
    &object_zob_Anim_0027D0, // ENZOB_ANIM_0
    &object_zob_Anim_002B38, // ENZOB_ANIM_1
    &object_zob_Anim_0037A0, // ENZOB_ANIM_2
    &object_zob_Anim_0043C4, // ENZOB_ANIM_3
    &object_zob_Anim_005224, // ENZOB_ANIM_4
    &object_zob_Anim_005E90, // ENZOB_ANIM_5
    &object_zob_Anim_006998, // ENZOB_ANIM_6
    &object_zob_Anim_011144, // ENZOB_ANIM_7
    &object_zob_Anim_001FD4, // ENZOB_ANIM_8
};

void EnZob_Init(Actor* thisx, PlayState* play) {
    EnZob* this = (EnZob*)thisx;
    s32 i;
    s16 csId;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.0115f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_zob_Skel_010810, &object_zob_Anim_006998, this->jointTable,
                       this->morphTable, OBJECT_ZOB_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &object_zob_Anim_006998);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->unk_2F4 = 0;
    this->csIdIndex = -1;
    this->cueId = 0;
    this->animIndex = ENZOB_ANIM_MAX;
    this->unk_304 = 0;
    this->actor.terminalVelocity = -4.0f;
    this->actor.gravity = -4.0f;
    EnZob_ChangeAnim(this, ENZOB_ANIM_6, ANIMMODE_ONCE);
    this->actionFunc = func_80BA0728;
    this->actor.textId = 0;

    csId = this->actor.csId;
    for (i = 0; i < ARRAY_COUNT(this->csIdList); i++) {
        this->csIdList[i] = csId;
        if (csId != CS_ID_NONE) {
            this->actor.csId = csId;
            csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);
        }
    }

    this->actor.csId = this->csIdList[0];
    this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;

    switch (ENZOB_GET_F(&this->actor)) {
        case ENZOB_F_1:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_78_01)) {
                this->actionFunc = func_80BA0BB4;
            } else {
                this->actionFunc = func_80BA0AD8;
            }

            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
                Actor_Kill(&this->actor);
                return;
            }
            break;

        case ENZOB_F_2:
            this->actionFunc = func_80BA0CF4;
            this->unk_2F4 |= 0x20;
            this->unk_312 = -1;
            EnZob_ChangeAnim(this, ENZOB_ANIM_0, ANIMMODE_ONCE);
            this->unk_304 = 5;
            break;

        default:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
                Actor_Kill(&this->actor);
            }
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            break;
    }
}

void EnZob_Destroy(Actor* thisx, PlayState* play) {
    EnZob* this = (EnZob*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnZob_ChangeAnim(EnZob* this, s16 animIndex, u8 animMode) {
    Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimations[animIndex]), animMode, -5.0f);
    this->animIndex = animIndex;
}

void func_80B9F86C(EnZob* this) {
    if (SkelAnime_Update(&this->skelAnime)) {
        switch (this->unk_304) {
            case 0:
                if (Rand_ZeroFloat(1.0f) > 0.7f) {
                    if (this->animIndex == ENZOB_ANIM_6) {
                        EnZob_ChangeAnim(this, ENZOB_ANIM_7, ANIMMODE_ONCE);
                    } else {
                        EnZob_ChangeAnim(this, ENZOB_ANIM_6, ANIMMODE_ONCE);
                    }
                } else {
                    EnZob_ChangeAnim(this, this->animIndex, ANIMMODE_ONCE);
                }
                break;

            case 1:
                EnZob_ChangeAnim(this, ENZOB_ANIM_3, ANIMMODE_LOOP);
                break;

            case 2:
                EnZob_ChangeAnim(this, ENZOB_ANIM_4, ANIMMODE_LOOP);
                break;

            case 3:
                EnZob_ChangeAnim(this, ENZOB_ANIM_5, ANIMMODE_LOOP);
                break;

            case 4:
                if (this->animIndex == ENZOB_ANIM_3) {
                    EnZob_ChangeAnim(this, ENZOB_ANIM_0, ANIMMODE_LOOP);
                } else {
                    EnZob_ChangeAnim(this, ENZOB_ANIM_3, ANIMMODE_ONCE);
                }
                break;

            case 5:
                if (Rand_ZeroFloat(1.0f) < 0.8f) {
                    EnZob_ChangeAnim(this, this->animIndex, ANIMMODE_ONCE);
                } else if (this->animIndex == ENZOB_ANIM_0) {
                    EnZob_ChangeAnim(this, ENZOB_ANIM_1, ANIMMODE_ONCE);
                } else {
                    EnZob_ChangeAnim(this, ENZOB_ANIM_0, ANIMMODE_ONCE);
                }
                break;

            default:
                break;
        }

        SkelAnime_Update(&this->skelAnime);
    }
}

void func_80B9FA3C(EnZob* this, PlayState* play) {
    u16 textId;

    this->unk_2F4 |= 1;

    if (GET_PLAYER_FORM != PLAYER_FORM_ZORA) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_30_02)) {
            textId = 0x11F9;
        } else {
            textId = 0x11F8;
        }
        this->unk_304 = 3;
        EnZob_ChangeAnim(this, ENZOB_ANIM_5, ANIMMODE_LOOP);
    } else if (this->unk_2F4 & 0x10) {
        textId = 0x1210;
        this->unk_304 = 3;
        EnZob_ChangeAnim(this, ENZOB_ANIM_5, ANIMMODE_ONCE);
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_31_08)) {
        textId = 0x1205;
        this->unk_304 = 1;
        EnZob_ChangeAnim(this, ENZOB_ANIM_3, ANIMMODE_LOOP);
    } else if (this->unk_2F4 & 8) {
        textId = 0x1215;
        this->unk_304 = 3;
        EnZob_ChangeAnim(this, ENZOB_ANIM_5, ANIMMODE_ONCE);
    } else if (this->unk_2F4 & 2) {
        textId = 0x1203;
        this->unk_304 = 1;
        EnZob_ChangeAnim(this, ENZOB_ANIM_2, ANIMMODE_ONCE);
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_30_08)) {
        textId = 0x11FA;
        this->unk_304 = 1;
        EnZob_ChangeAnim(this, ENZOB_ANIM_2, ANIMMODE_ONCE);
    } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_30_04)) {
        SET_WEEKEVENTREG(WEEKEVENTREG_30_04);
        textId = 0x11FB;
        this->unk_304 = 1;
        EnZob_ChangeAnim(this, ENZOB_ANIM_2, ANIMMODE_ONCE);
    } else {
        textId = 0x1201;
        this->unk_304 = 3;
        EnZob_ChangeAnim(this, ENZOB_ANIM_4, ANIMMODE_ONCE);
    }

    Message_StartTextbox(play, textId, &this->actor);
}

void func_80B9FC0C(EnZob* this) {
    if (this->csIdIndex != -1) {
        if (CutsceneManager_GetCurrentCsId() == this->csIdList[this->csIdIndex]) {
            CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
        }
        this->csIdIndex = -1;
    }
}

void func_80B9FC70(EnZob* this, s16 csIdIndex) {
    func_80B9FC0C(this);
    this->csIdIndex = csIdIndex;
}

void func_80B9FCA0(EnZob* this, PlayState* play) {
    Message_CloseTextbox(play);
    play->msgCtx.ocarinaMode = OCARINA_MODE_END;
    func_80B9FC0C(this);
    this->unk_2F4 &= ~1;
    this->actionFunc = func_80BA0728;
    this->unk_304 = 0;
    EnZob_ChangeAnim(this, ENZOB_ANIM_6, ANIMMODE_ONCE);
    Actor_OcarinaInteractionAccepted(&this->actor, &play->state);
}

void func_80B9FD24(EnZob* this, PlayState* play) {
    s32 cueChannel;
    s16 cueId;

    func_80B9F86C(this);

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_500)) {
        this->csIdIndex = -1;
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_500);
        cueId = play->csCtx.actorCues[cueChannel]->id;

        if (this->cueId != cueId) {
            this->cueId = cueId;
            switch (cueId) {
                case 1:
                    EnZob_ChangeAnim(this, ENZOB_ANIM_8, ANIMMODE_LOOP);
                    break;

                case 2:
                    EnZob_ChangeAnim(this, ENZOB_ANIM_7, ANIMMODE_LOOP);
                    break;
            }
        }
    }
}

void func_80B9FDDC(EnZob* this, PlayState* play) {
    Message_StartTextbox(play, 0x120C, &this->actor);
    this->actionFunc = func_80BA00BC;
}

void func_80B9FE1C(EnZob* this, PlayState* play) {
    Message_StartTextbox(play, 0x1211, &this->actor);
    this->actionFunc = func_80BA00BC;
}

void func_80B9FE5C(EnZob* this, PlayState* play) {
    func_80B9F86C(this);
    if (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) {
        play->msgCtx.msgLength = 0;
        this->actionFunc = func_80B9FDDC;
        func_80B9FC70(this, 0);
    } else if (Message_GetState(&play->msgCtx) == TEXT_STATE_11) {
        play->msgCtx.msgLength = 0;
        this->actionFunc = func_80B9FE1C;
        this->unk_304 = 3;
        EnZob_ChangeAnim(this, ENZOB_ANIM_5, ANIMMODE_ONCE);
        func_80B9FC70(this, 0);
    }
}

void func_80B9FF20(EnZob* this, PlayState* play) {
    func_80B9F86C(this);
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_SONG_DEMO_DONE) {
        Message_DisplayOcarinaStaff(play, OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF);
        this->actionFunc = func_80B9FE5C;
        func_80B9FC70(this, 2);
    }
}

void func_80B9FF80(EnZob* this, PlayState* play) {
    func_80B9F86C(this);
    if (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) {
        this->actionFunc = func_80B9FF20;
        this->unk_304 = 6;
        EnZob_ChangeAnim(this, ENZOB_ANIM_1, ANIMMODE_LOOP);
        Message_DisplayOcarinaStaff(play, OCARINA_ACTION_DEMONSTRATE_EVAN_PART2_FIRST_HALF);
        func_80B9FC70(this, 1);
    } else if (Message_GetState(&play->msgCtx) == TEXT_STATE_11) {
        play->msgCtx.msgLength = 0;
        this->actionFunc = func_80B9FE1C;
        this->unk_304 = 3;
        EnZob_ChangeAnim(this, ENZOB_ANIM_5, ANIMMODE_ONCE);
        func_80B9FC70(this, 0);
    }
}

void func_80BA005C(EnZob* this, PlayState* play) {
    func_80B9F86C(this);
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_SONG_DEMO_DONE) {
        Message_DisplayOcarinaStaff(play, OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF);
        this->actionFunc = func_80B9FF80;
        func_80B9FC70(this, 2);
    }
}

void func_80BA00BC(EnZob* this, PlayState* play) {
    func_80B9F86C(this);

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play) && (play->msgCtx.currentTextId == 0x1212)) {
                switch (play->msgCtx.choiceIndex) {
                    case 1:
                        Audio_PlaySfx_MessageDecide();
                        Message_ContinueTextbox(play, 0x1209);
                        this->unk_304 = 1;
                        EnZob_ChangeAnim(this, ENZOB_ANIM_2, ANIMMODE_ONCE);
                        break;

                    case 0:
                        Audio_PlaySfx_MessageCancel();
                        Message_ContinueTextbox(play, 0x1213);
                        break;
                }
            }
            break;

        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x1208:
                    case 0x120E:
                    case 0x1216:
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x120C:
                        play->msgCtx.msgLength = 0;
                        this->actionFunc = func_80B9FD24;
                        EnZob_ChangeAnim(this, ENZOB_ANIM_8, ANIMMODE_LOOP);
                        func_80B9FC70(this, 3);
                        break;

                    case 0x120D:
                    case 0x1211:
                    case 0x1213:
                    case 0x1217:
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        this->unk_304 = 3;
                        EnZob_ChangeAnim(this, ENZOB_ANIM_4, ANIMMODE_ONCE);
                        break;

                    case 0x1218:
                        func_80B9FCA0(this, play);
                        break;

                    case 0x1214:
                        this->unk_2F4 |= 8;
                        func_80B9FCA0(this, play);
                        break;

                    case 0x120F:
                        SET_WEEKEVENTREG(WEEKEVENTREG_31_08);
                        this->unk_2F4 |= 0x10;
                        func_80B9FCA0(this, play);
                        break;

                    case 0x1209:
                        Message_DisplayOcarinaStaff(play, OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_FIRST_HALF);
                        this->unk_304 = 4;
                        EnZob_ChangeAnim(this, ENZOB_ANIM_0, ANIMMODE_LOOP);
                        this->actionFunc = func_80BA005C;
                        func_80B9FC70(this, 1);
                        break;
                }
            }
            break;
    }
}

void func_80BA0318(EnZob* this, PlayState* play) {
    Message_DisplayOcarinaStaff(play, OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_FIRST_HALF);
    this->unk_304 = 4;
    EnZob_ChangeAnim(this, ENZOB_ANIM_0, ANIMMODE_LOOP);
    this->actionFunc = func_80BA005C;
    func_80B9FC70(this, 1);
}

void func_80BA0374(EnZob* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    func_80B9F86C(this);

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play) && (play->msgCtx.currentTextId == 0x1205)) {
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        Audio_PlaySfx_MessageDecide();
                        Message_ContinueTextbox(play, 0x1207);
                        EnZob_ChangeAnim(this, ENZOB_ANIM_2, ANIMMODE_ONCE);
                        break;

                    case 1:
                        Audio_PlaySfx_MessageCancel();
                        Message_ContinueTextbox(play, 0x1206);
                        break;
                }
            }
            break;

        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x11F8:
                        SET_WEEKEVENTREG(WEEKEVENTREG_30_02);
                        Message_ContinueTextbox(play, 0x11F9);
                        break;

                    case 0x11F9:
                        Message_CloseTextbox(play);
                        this->actionFunc = func_80BA0728;
                        this->unk_304 = 0;
                        EnZob_ChangeAnim(this, ENZOB_ANIM_6, ANIMMODE_ONCE);
                        this->unk_2F4 &= ~1;
                        break;

                    case 0x11FB:
                    case 0x11FC:
                    case 0x11FF:
                    case 0x1201:
                    case 0x1203:
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x11FD:
                        this->unk_304 = 3;
                        EnZob_ChangeAnim(this, ENZOB_ANIM_4, ANIMMODE_ONCE);
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x11FE:
                        this->unk_304 = 1;
                        EnZob_ChangeAnim(this, ENZOB_ANIM_3, ANIMMODE_LOOP);
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x11FA:
                    case 0x1200:
                    case 0x1202:
                    case 0x1204:
                    case 0x1206:
                    case 0x120F:
                    case 0x1210:
                    case 0x1215:
                        Message_CloseTextbox(play);
                        this->actionFunc = func_80BA0728;
                        this->unk_304 = 0;
                        EnZob_ChangeAnim(this, ENZOB_ANIM_6, ANIMMODE_ONCE);
                        this->unk_2F4 &= ~1;
                        this->unk_2F4 |= 2;
                        break;

                    case 0x1207:
                        Message_CloseTextbox(play);
                        this->actionFunc = func_80BA0318;
                        player->ocarinaInteractionActor = &this->actor;
                        player->stateFlags3 |= PLAYER_STATE3_20;
                        break;
                }
            }
            break;
    }
}

void func_80BA0610(EnZob* this, PlayState* play) {
    func_80B9F86C(this);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Message_StartTextbox(play, 0x120D, &this->actor);
        this->unk_304 = 3;
        EnZob_ChangeAnim(this, ENZOB_ANIM_5, ANIMMODE_ONCE);
        func_80B9FC70(this, 0);
        this->actionFunc = func_80BA00BC;
    } else {
        Actor_OfferTalk(&this->actor, play, 500.0f);
    }
}

void func_80BA06BC(EnZob* this, PlayState* play) {
    func_80B9FD24(this, play);
    if (!Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_500)) {
        this->actionFunc = func_80BA0610;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        func_80BA0610(this, play);
    }
}

void func_80BA0728(EnZob* this, PlayState* play) {
    s32 pad;
    Vec3f seqPos;

    func_80B9F86C(this);

    if (Actor_OcarinaInteractionAccepted(&this->actor, &play->state)) {
        if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
            Message_StartTextbox(play, 0x1208, NULL);
            SET_WEEKEVENTREG(WEEKEVENTREG_30_08);
        } else {
            Message_StartTextbox(play, 0x1216, NULL);
        }
        this->actionFunc = func_80BA00BC;
        this->unk_304 = 1;
        EnZob_ChangeAnim(this, ENZOB_ANIM_2, ANIMMODE_ONCE);
        this->csIdIndex = 0;
        this->unk_2F4 |= 1;
    } else if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80BA0374;
        func_80B9FA3C(this, play);
    } else if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_500)) {
        this->actionFunc = func_80BA06BC;
    } else if ((this->actor.xzDistToPlayer < 180.0f) && (this->actor.xzDistToPlayer > 60.0f) &&
               Player_IsFacingActor(&this->actor, 0x3000, play) && Actor_IsFacingPlayer(&this->actor, 0x3000)) {
        Actor_OfferTalk(&this->actor, play, 150.0f);
        Actor_OfferOcarinaInteraction(&this->actor, play, 200.0f, 150.0f);
    }

    seqPos.x = this->actor.projectedPos.x;
    seqPos.y = this->actor.projectedPos.y;
    seqPos.z = this->actor.projectedPos.z;
    Audio_PlaySequenceAtPos(SEQ_PLAYER_BGM_SUB, &seqPos, NA_BGM_BASS_PLAY, 1000.0f);
}

void func_80BA08E8(EnZob* this, PlayState* play) {
    s32 textId;

    if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_79_01)) {
            textId = 0x1257;
            this->unk_304 = 3;
            EnZob_ChangeAnim(this, ENZOB_ANIM_4, ANIMMODE_ONCE);
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_78_40)) {
            textId = 0x1256;
            this->unk_304 = 1;
            EnZob_ChangeAnim(this, ENZOB_ANIM_2, ANIMMODE_ONCE);
        } else {
            textId = 0x1255;
            SET_WEEKEVENTREG(WEEKEVENTREG_78_40);
            this->unk_304 = 1;
            EnZob_ChangeAnim(this, ENZOB_ANIM_2, ANIMMODE_ONCE);
        }
    } else {
        textId = 0x1254;
        this->unk_304 = 3;
        EnZob_ChangeAnim(this, ENZOB_ANIM_5, ANIMMODE_ONCE);
    }

    Message_StartTextbox(play, textId, &this->actor);
}

void func_80BA09E0(EnZob* this, PlayState* play) {
    func_80B9F86C(this);
}

void func_80BA0A04(EnZob* this, PlayState* play) {
    func_80B9F86C(this);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x1000, 0x200);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                this->actionFunc = func_80BA0AD8;
                this->unk_304 = 0;
                EnZob_ChangeAnim(this, ENZOB_ANIM_6, ANIMMODE_ONCE);
            }
            break;

        case TEXT_STATE_CLOSING:
            this->actionFunc = func_80BA0AD8;
            this->unk_304 = 0;
            EnZob_ChangeAnim(this, ENZOB_ANIM_6, ANIMMODE_ONCE);
            break;
    }
}

void func_80BA0AD8(EnZob* this, PlayState* play) {
    func_80B9F86C(this);

    if (this->actor.home.rot.y != this->actor.shape.rot.y) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 2, 0x1000, 0x200);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80BA0A04;
        func_80BA08E8(this, play);
    } else if ((this->actor.xzDistToPlayer < 120.0f) && Player_IsFacingActor(&this->actor, 0x3000, play) &&
               Actor_IsFacingPlayer(&this->actor, 0x3000)) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void func_80BA0BB4(EnZob* this, PlayState* play) {
    func_80B9F86C(this);
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_79_01)) {
        this->actionFunc = func_80BA09E0;
        EnZob_ChangeAnim(this, ENZOB_ANIM_0, ANIMMODE_ONCE);
        this->unk_304 = 5;
    }
}

void func_80BA0C14(EnZob* this, PlayState* play) {
    func_80B9F86C(this);

    if (this->unk_312 < 799) {
        this->unk_312 += 200;
    } else {
        this->unk_312 += 30;
    }

    if (this->unk_312 > 999) {
        this->unk_312 = 999;
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_515)) {
        if (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_515)]->id == 1) {
            this->actionFunc = func_80BA0CF4;
            this->unk_312 = -1;
        }
    } else {
        this->actionFunc = func_80BA0CF4;
        this->unk_312 = -1;
    }
}

void func_80BA0CF4(EnZob* this, PlayState* play) {
    func_80B9F86C(this);
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_515) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_515)]->id == 2)) {
        this->actionFunc = func_80BA0C14;
    }
}

void EnZob_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZob* this = (EnZob*)thisx;

    Actor_MoveWithGravity(&this->actor);

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 10.0f, UPDBGCHECKINFO_FLAG_4);

    this->actionFunc(this, play);

    if ((this->csIdIndex != -1) && (CutsceneManager_GetCurrentCsId() != this->csIdList[this->csIdIndex])) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
        } else if (CutsceneManager_IsNext(this->csIdList[this->csIdIndex])) {
            CutsceneManager_Start(this->csIdList[this->csIdIndex], &this->actor);
        } else {
            CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
        }
    }

    if (this->unk_2F4 & 1) {
        Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.y, 0, 6, 0x1838, 0x64);
    }
}

s32 EnZob_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnZob* this = (EnZob*)thisx;

    if (limbIndex == OBJECT_ZOB_LIMB_09) {
        rot->x += this->headRot.y;
        rot->y += this->headRot.x;
    }
    return false;
}

void EnZob_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80BA1120 = { 300.0f, 900.0f, 0.0f };
    EnZob* this = (EnZob*)thisx;

    if (limbIndex == OBJECT_ZOB_LIMB_09) {
        Matrix_MultVec3f(&D_80BA1120, &this->actor.focus.pos);
    }
}

void EnZob_Draw(Actor* thisx, PlayState* play) {
    EnZob* this = (EnZob*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->unk_2F4 & 0x20) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 0, 0, 0, 0, this->unk_312, 1000);
    }

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnZob_OverrideLimbDraw, EnZob_PostLimbDraw, &this->actor);

    if (this->unk_2F4 & 0x20) {
        POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
