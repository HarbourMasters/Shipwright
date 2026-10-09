/*
 * File: z_en_gb2.c
 * Overlay: ovl_En_Gb2
 * Description: Spirit House - Owner
 */

#include "z_en_gb2.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnGb2_Init(Actor* thisx, PlayState* play);
void EnGb2_Destroy(Actor* thisx, PlayState* play);
void EnGb2_Update(Actor* thisx, PlayState* play);
void EnGb2_Draw(Actor* thisx, PlayState* play);

void func_80B0FBF0(EnGb2* this, PlayState* play);
void func_80B0FEBC(EnGb2* this, PlayState* play);
void func_80B0FFA8(EnGb2* this, PlayState* play);
void func_80B10240(EnGb2* this, PlayState* play);
void func_80B10344(EnGb2* this, PlayState* play);
void func_80B10584(EnGb2* this, PlayState* play);
void func_80B10634(EnGb2* this, PlayState* play);
void func_80B10868(EnGb2* this, PlayState* play);
void func_80B10924(EnGb2* this, PlayState* play);
void func_80B109DC(EnGb2* this, PlayState* play);
void func_80B10A48(EnGb2* this, PlayState* play);
void func_80B10B5C(EnGb2* this, PlayState* play);
void func_80B10DAC(EnGb2* this, PlayState* play);
void func_80B10E98(EnGb2* this, PlayState* play);
void func_80B11048(EnGb2* this, PlayState* play);
void func_80B110F8(EnGb2* this, PlayState* play);
void func_80B111AC(EnGb2* this, PlayState* play);
void func_80B11268(EnGb2* this, PlayState* play);
void func_80B11344(EnGb2* this, PlayState* play);

ActorProfile En_Gb2_Profile = {
    /**/ ACTOR_EN_GB2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_PS,
    /**/ sizeof(EnGb2),
    /**/ EnGb2_Init,
    /**/ EnGb2_Destroy,
    /**/ EnGb2_Update,
    /**/ EnGb2_Draw,
};

typedef struct {
    /* 0x00 */ s16 unk_00;
    /* 0x04 */ Vec3f unk_04;
    /* 0x10 */ s16 unk_10;
} EnGbStruct; // size = 0x14

static EnGbStruct D_80B119B0[] = {
    { ACTOR_EN_PO_SISTERS, { 0.0f, 0.0f, 0.0f }, 0x300 },
    { ACTOR_EN_PO_SISTERS, { 100.0f, 0.0f, 0.0f }, 0x200 },
    { ACTOR_EN_PO_SISTERS, { -100.0f, 0.0f, 0.0f }, 0x100 },
    { ACTOR_EN_PO_SISTERS, { 0.0f, 0.0f, 0.0f }, 0 },
};

static f32 D_80B11A00[][4] = {
    { 0.0f, -13.0f, 549.0f, 0.0f },
    { -242.0f, -13.0f, 390.0f, -90.0f },
    { 0.0f, -13.0f, 148.0f, 180.0f },
    { 244.0f, -13.0f, 390.0f, 90.0f },
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 40, 75, 0, { 0, 0, 0 } },
};

void func_80B0F5E0(EnGb2* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 func_80B0F660(EnGb2* this, PlayState* play) {
    Actor* actorIter = NULL;

    while (true) {
        actorIter = SubS_FindActor(play, actorIter, ACTORCAT_NPC, ACTOR_EN_GB2);
        if (actorIter == NULL) {
            break;
        }

        if ((EnGb2*)actorIter != this) {
            return true;
        }

        if (actorIter->next == NULL) {
            break;
        }
        actorIter = actorIter->next;
    };

    return false;
}

void func_80B0F6DC(EnGb2* this) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_54_20)) {
        SET_WEEKEVENTREG(WEEKEVENTREG_54_20);
        this->unk_26E = 0x14D0;
    } else {
        this->unk_26E = 0x14D1;
    }
    this->unk_288 = 30;
    this->actionFunc = func_80B0FEBC;
}

void func_80B0F728(EnGb2* this, PlayState* play) {
    if (CHECK_EVENTINF(EVENTINF_44)) {
        func_80B0FBF0(this, play);
        this->unk_26E = 0x14E1;
        if (CHECK_EVENTINF(EVENTINF_47)) {
            this->unk_288 = 10;
        } else {
            this->unk_288 = 30;
        }
    } else if (CHECK_EVENTINF(EVENTINF_45)) {
        func_80B0FBF0(this, play);
        this->unk_26E = 0x14E0;
        this->unk_26C |= 2;
    } else {
        this->unk_26E = 0x14DC;
    }

    CLEAR_EVENTINF(EVENTINF_44);
    CLEAR_EVENTINF(EVENTINF_45);
    CLEAR_EVENTINF(EVENTINF_46);
    this->actionFunc = func_80B10584;
}

u16 func_80B0F7FC(EnGb2* this) {
    switch (this->unk_26E) {
        case 0x14D0:
            return 0x14D1;

        case 0x14D1:
            if (CHECK_EVENTINF(EVENTINF_47)) {
                return 0x14E4;
            }

            if (gSaveContext.save.saveInfo.playerData.health > 0x30) {
                return 0x14D2;
            }

            this->unk_26C |= 2;
            return 0x14D3;

        case 0x14E4:
            if (gSaveContext.save.saveInfo.playerData.health > 0x30) {
                return 0x14D2;
            }

            this->unk_26C |= 2;
            return 0x14D3;

        case 0x14D2:
            if (CHECK_EVENTINF(EVENTINF_47)) {
                return 0x14E5;
            }
            return 0x14D4;

        case 0x14D4:
        case 0x14E5:
            return 0x14D5;

        case 0x14D8:
            return 0x14D9;

        case 0x14D9:
            return 0x14DA;

        default:
            return 0;
    }
}

u16 func_80B0F8F8(EnGb2* this) {
    switch (this->unk_26E) {
        case 0x14DC:
            this->unk_26C |= 2;
            return 0x14DD;

        case 0x14DD:
            return 0x14DE;

        case 0x14DE:
            this->unk_26C |= 2;
            SET_WEEKEVENTREG(WEEKEVENTREG_54_80);
            return 0x14DF;

        case 0x14E1:
            return 0x14E2;

        default:
            return 0;
    }
}

u16 func_80B0F97C(EnGb2* this) {
    switch (this->unk_26E) {
        case 0x14F5:
            return 0x14F6;

        case 0x14F6:
            return 0x14F7;

        case 0x14F7:
            SET_WEEKEVENTREG(WEEKEVENTREG_76_80);
            this->unk_26C |= 2;
            return 0x14F8;

        case 0x14F9:
            return 0x14FA;

        case 0x14FA:
            this->unk_26C |= 2;
            return 0x14FB;

        default:
            return 0;
    }
}

void func_80B0FA04(EnGb2* this) {
    this->csIdList[0] = this->actor.csId;
    this->csIdList[1] = CutsceneManager_GetAdditionalCsId(this->csIdList[0]);
    this->csIdList[2] = CutsceneManager_GetAdditionalCsId(this->csIdList[1]);
}

s32 func_80B0FA48(EnGb2* this, PlayState* play) {
    switch (Player_GetMask(play)) {
        case PLAYER_MASK_GIBDO:
            this->unk_26C |= 4;
            this->unk_26E = 0x14E6;
            return false;

        case PLAYER_MASK_GARO:
            this->unk_26C |= 8;
            this->unk_26E = 0x14E6;
            return false;

        case PLAYER_MASK_CAPTAIN:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_80_40)) {
                this->unk_26E = 0x14EB;
                return false;
            } else {
                this->unk_26E = 0x14EE;
                return true;
            }

        default:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_80_20)) {
                this->unk_26E = 0x14EF;
                return false;
            } else {
                this->unk_26E = 0x14F4;
                return true;
            }
    }
}

u16 func_80B0FB24(EnGb2* this) {
    switch (this->unk_26E) {
        case 0x14E6:
            if (this->unk_26C & 4) {
                return 0x14E7;
            }

            if (this->unk_26C & 8) {
                return 0x14E9;
            }

        case 0x14E7:
            this->unk_26C |= 2;
            return 0x14E8;

        case 0x14E9:
            this->unk_26C |= 2;
            return 0x14EA;

        case 0x14EB:
            return 0x14EC;

        case 0x14EC:
            this->unk_26C |= 2;
            return 0x14ED;

        case 0x14EF:
            return 0x14F0;

        case 0x14F0:
            return 0x14F1;

        case 0x14F1:
            return 0x14F2;

        case 0x14F2:
            this->unk_26C |= 2;
            return 0x14F3;

        default:
            return 0;
    }
}

void func_80B0FBF0(EnGb2* this, PlayState* play) {
    Vec3f sp90[] = {
        { 120.0f, 0.0f, 800.0f },
        { -120.0f, 0.0f, 750.0f },
        { 60.0f, 0.0f, 750.0f },
        { -60.0f, 0.0f, 800.0f },
    };
    Vec3s sp78[] = {
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
    };
    s32 sp68[] = {
        0x1000,
        0x1100,
        0x1200,
        0x1300,
    };
    s16 i;
    s16 end = ARRAY_COUNT(sp90);

    for (i = 0; i < end; i++) {
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_PO_SISTERS, sp90[i].x, sp90[i].y, sp90[i].z,
                           sp78[i].x, sp78[i].y, sp78[i].z, sp68[i]);
    }
}

void func_80B0FD8C(EnGb2* this, PlayState* play) {
    this->unk_280++;
    this->unk_268 = this->actor.child;
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_PO_SISTERS, D_80B119B0[this->unk_280].unk_04.x,
                       D_80B119B0[this->unk_280].unk_04.y, D_80B119B0[this->unk_280].unk_04.z, 0, 0, 0,
                       D_80B119B0[this->unk_280].unk_10);
}

void func_80B0FE18(PlayState* play) {
    Environment_ForcePlaySequence(NA_BGM_MINI_BOSS);
    play->nextEntrance = ENTRANCE(GHOST_HUT, 1);
    play->transitionType = TRANS_TYPE_64;
    gSaveContext.nextTransitionType = TRANS_TYPE_64;
    play->transitionTrigger = TRANS_TRIGGER_START;
}

void func_80B0FE7C(PlayState* play) {
    play->nextEntrance = ENTRANCE(GHOST_HUT, 2);
    play->transitionType = TRANS_TYPE_64;
    gSaveContext.nextTransitionType = TRANS_TYPE_64;
    play->transitionTrigger = TRANS_TRIGGER_START;
}

void func_80B0FEBC(EnGb2* this, PlayState* play) {
    if ((play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) && (play->msgCtx.lastPlayedSong == OCARINA_SONG_HEALING)) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
        SET_EVENTINF(EVENTINF_47);
        this->unk_26E = 0x14D1;
        this->unk_288 = 10;
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, this->unk_26E, &this->actor);
        this->actionFunc = func_80B0FFA8;
    } else if ((this->actor.xzDistToPlayer < 300.0f) || this->actor.isLockedOn) {
        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
    }
}

void func_80B0FFA8(EnGb2* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (talkState == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            if (this->unk_26C & 2) {
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->unk_26E = 0x14D1;
                this->unk_288 = 30;
                CLEAR_EVENTINF(EVENTINF_47);
                this->unk_26C &= ~2;
                this->actionFunc = func_80B0FEBC;
            } else {
                this->unk_26E = func_80B0F7FC(this);
                Message_StartTextbox(play, this->unk_26E, &this->actor);
            }
        }
    } else if ((talkState == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        if (this->unk_26E == 0x14D5) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    if (gSaveContext.save.saveInfo.playerData.rupees < this->unk_288) {
                        Audio_PlaySfx(NA_SE_SY_ERROR);
                        this->unk_26E = 0x14D7;
                        this->unk_26C |= 2;
                        Message_StartTextbox(play, this->unk_26E, &this->actor);
                    } else {
                        Audio_PlaySfx_MessageDecide();
                        this->unk_26E = 0x14D8;
                        Message_StartTextbox(play, this->unk_26E, &this->actor);
                    }
                    break;

                case 1:
                    Audio_PlaySfx_MessageCancel();
                    this->unk_26E = 0x14D6;
                    this->unk_26C |= 2;
                    Message_StartTextbox(play, this->unk_26E, &this->actor);
                    break;

                default:
                    break;
            }
        } else if (this->unk_26E == 0x14DA) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    Audio_PlaySfx_MessageDecide();
                    Rupees_ChangeBy(-this->unk_288);
                    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                    play->msgCtx.stateTimer = 4;
                    Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_WAIT);
                    this->actionFunc = func_80B11344;
                    break;

                case 1:
                    Audio_PlaySfx_MessageCancel();
                    this->unk_26E = 0x14DB;
                    this->unk_26C |= 2;
                    Message_StartTextbox(play, this->unk_26E, &this->actor);
                    break;

                default:
                    break;
            }
        }
    }
}

void func_80B10240(EnGb2* this, PlayState* play) {
    this->unk_27C = D_80B119B0[this->unk_280].unk_00;
    this->unk_27E = D_80B119B0[this->unk_280].unk_10;
    this->csIdList[0] = this->actor.csId;
    this->unk_268 = NULL;
    if (CutsceneManager_IsNext(this->csIdList[0])) {
        CutsceneManager_Start(this->csIdList[0], &this->actor);
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, this->unk_27C, D_80B119B0[this->unk_280].unk_04.x,
                           D_80B119B0[this->unk_280].unk_04.y, D_80B119B0[this->unk_280].unk_04.z, 0, 0, 0,
                           this->unk_27E);
        if (this->unk_280 == 0) {
            Interface_StartTimer(TIMER_ID_MINIGAME_1, 180);
        }
        this->actionFunc = func_80B10344;
    } else {
        CutsceneManager_Queue(this->csIdList[0]);
    }
}

void func_80B10344(EnGb2* this, PlayState* play) {
    if (this->unk_280 == 1) {
        func_80B0FD8C(this, play);
    }

    if (this->unk_280 == 2) {
        if ((this->unk_268 != NULL) && (this->unk_268->update == NULL)) {
            this->unk_268 = NULL;
            this->unk_26C |= 0x400;
        }
    }

    if ((this->actor.child != NULL) && (this->actor.child->update == NULL)) {
        this->actor.child = NULL;
        this->unk_26C |= 0x200;
    }

    if (this->unk_26C & 0x200) {
        if (this->unk_280 == 3) {
            this->unk_26C &= ~0x200;
            gSaveContext.timerStates[TIMER_ID_MINIGAME_1] = TIMER_STATE_STOP;
            Environment_StartTime();
            SET_EVENTINF(EVENTINF_46);
            func_80B0FE7C(play);
        } else if (this->unk_280 == 2) {
            if (this->unk_26C & 0x400) {
                this->unk_280++;
                this->unk_26C &= ~0x200;
                this->unk_26C &= ~0x400;
                this->actionFunc = func_80B10240;
            }
        } else {
            this->unk_280++;
            this->unk_26C &= ~0x200;
            this->actionFunc = func_80B10240;
        }
    }

    if (gSaveContext.save.saveInfo.playerData.health <= 0x30) {
        gSaveContext.timerStates[TIMER_ID_MINIGAME_1] = TIMER_STATE_STOP;
        SET_EVENTINF(EVENTINF_46);
        SET_EVENTINF(EVENTINF_45);

        if ((this->unk_268 != NULL) && (this->unk_268->update == NULL)) {
            this->unk_268 = NULL;
        }

        if ((this->actor.child != NULL) && (this->actor.child->update == NULL)) {
            this->actor.child = NULL;
        }

        func_80B0FE7C(play);
    } else if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1] == SECONDS_TO_TIMER(0)) {
        gSaveContext.timerStates[TIMER_ID_MINIGAME_1] = TIMER_STATE_STOP;
        SET_EVENTINF(EVENTINF_46);
        SET_EVENTINF(EVENTINF_44);

        if ((this->unk_268 != NULL) && (this->unk_268->update == NULL)) {
            this->unk_268 = NULL;
        }

        if ((this->actor.child != NULL) && (this->actor.child->update == NULL)) {
            this->actor.child = NULL;
        }

        func_80B0FE7C(play);
    }
}

void func_80B10584(EnGb2* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, this->unk_26E, &this->actor);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = func_80B10634;
    } else if (this->actor.xzDistToPlayer < 300.0f) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalk(&this->actor, play, 300.0f);
    }
}

void func_80B10634(EnGb2* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (talkState == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            if (this->unk_26C & 2) {
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->unk_26C &= ~2;
                if (this->unk_26E == 0x14DD) {
                    this->unk_26E = 0x14DE;
                    this->actionFunc = func_80B10924;
                } else if (this->unk_26E == 0x14DF) {
                    this->actionFunc = func_80B10A48;
                } else {
                    this->unk_26E = 0x14D1;
                    this->unk_288 = 30;
                    CLEAR_EVENTINF(EVENTINF_47);
                    this->actionFunc = func_80B0FEBC;
                }
            } else {
                this->unk_26E = func_80B0F8F8(this);
                Message_StartTextbox(play, this->unk_26E, &this->actor);
            }
        }
    } else if ((talkState == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                if (gSaveContext.save.saveInfo.playerData.rupees < this->unk_288) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    this->unk_26E = 0x14D7;
                    this->unk_26C |= 2;
                    Message_StartTextbox(play, this->unk_26E, &this->actor);
                } else {
                    Audio_PlaySfx_MessageDecide();
                    Rupees_ChangeBy(-this->unk_288);
                    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                    play->msgCtx.stateTimer = 4;
                    Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_WAIT);
                    this->actionFunc = func_80B11344;
                }
                break;

            case 1:
                Audio_PlaySfx_MessageCancel();
                this->unk_26E = 0x14E3;
                this->unk_26C |= 2;
                Message_StartTextbox(play, this->unk_26E, &this->actor);
                break;

            default:
                break;
        }
    }
}

void func_80B10868(EnGb2* this, PlayState* play) {
    if (CutsceneManager_GetCurrentCsId() != this->csIdList[2]) {
        if (CutsceneManager_IsNext(this->csIdList[this->csIdIndex])) {
            this->actionFunc = func_80B10A48;
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csIdList[this->csIdIndex], &this->actor);
        } else {
            CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
        }
    } else {
        this->csIdIndex = 1;
        CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
    }
}

void func_80B10924(EnGb2* this, PlayState* play) {
    s32 getItemId;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SPIRIT_HOUSE_HEART_PIECE)) {
        getItemId = GI_RUPEE_PURPLE;
    } else {
        getItemId = GI_HEART_PIECE;
    }

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        if (getItemId == GI_HEART_PIECE) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SPIRIT_HOUSE_HEART_PIECE);
        } else {
            Rupees_ChangeBy(50);
        }
        this->actionFunc = func_80B109DC;
    } else {
        Actor_OfferGetItem(&this->actor, play, getItemId, 300.0f, 300.0f);
    }
}

void func_80B109DC(EnGb2* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, this->unk_26E, &this->actor);
        this->actionFunc = func_80B10634;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 300.0f, PLAYER_IA_MINUS1);
    }
}

void func_80B10A48(EnGb2* this, PlayState* play) {
    this->unk_28A -= 5;
    if (this->unk_28A < 5) {
        this->unk_28A = 0;

        switch (ENGB2_GET_7(&this->actor)) {
            case ENGB2_7_0:
                Actor_Kill(&this->actor);
                break;

            case ENGB2_7_1:
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                Actor_Kill(&this->actor);
                break;

            case ENGB2_7_2:
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                if (this->unk_26E == 0x14FB) {
                    Flags_SetSwitch(play, ENGB2_GET_SWITCH_FLAG(&this->actor));
                    Actor_Kill(&this->actor);
                    return;
                }

                this->actor.draw = NULL;
                this->unk_26C |= 0x100;
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->actionFunc = func_80B111AC;
                break;

            default:
                break;
        }
    }
}

void func_80B10B5C(EnGb2* this, PlayState* play) {
    if (Player_GetMask(play) != this->unk_28C) {
        this->unk_28C = Player_GetMask(play);
        this->unk_26C &= ~0x80;
        this->unk_26C &= ~0x20;
        this->unk_26C &= ~0x40;
    }

    if (func_80B0FA48(this, play)) {
        this->unk_26C &= ~0x20;
        if (Actor_TalkOfferAccepted(&this->actor, &play->state) && (this->unk_26C & 0x40)) {
            if ((this->unk_26E == 0x14EE) || (this->unk_26E == 0x14F4)) {
                this->unk_26C |= 2;
            }
            Message_StartTextbox(play, this->unk_26E, &this->actor);
            this->csIdIndex = 1;
            this->unk_26C &= ~0x40;
            this->actionFunc = func_80B10DAC;
        } else if ((this->actor.xzDistToPlayer < 300.0f) && this->actor.isLockedOn) {
            this->unk_26C |= 0x40;
            Actor_OfferTalk(&this->actor, play, 300.0f);
        }
    } else {
        this->unk_26C &= ~0x40;
        if (Actor_TalkOfferAccepted(&this->actor, &play->state) && (this->unk_26C & 0x20)) {
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            Message_StartTextbox(play, this->unk_26E, &this->actor);
            if (this->unk_26E == 0x14EB) {
                SET_WEEKEVENTREG(WEEKEVENTREG_80_40);
            } else if (this->unk_26E == 0x14EF) {
                SET_WEEKEVENTREG(WEEKEVENTREG_80_20);
            }
            this->unk_26C &= ~0x20;
            this->csIdIndex = 0;
            this->unk_26C |= 0x80;
            this->actionFunc = func_80B10DAC;
        } else if (this->actor.xzDistToPlayer < 300.0f) {
            if (!(this->unk_26C & 0x80)) {
                this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                this->unk_26C |= 0x20;
                Actor_OfferTalk(&this->actor, play, 300.0f);
            }
        }
    }
}

void func_80B10DAC(EnGb2* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csIdList[this->csIdIndex])) {
        if (ENGB2_GET_7(&this->actor) == ENGB2_7_1) {
            if (this->csIdIndex != 2) {
                this->actionFunc = func_80B10E98;
            } else {
                Flags_SetSwitch(play, ENGB2_GET_SWITCH_FLAG(&this->actor));
                this->actionFunc = func_80B10868;
            }
        } else {
            this->actionFunc = func_80B110F8;
        }
        CutsceneManager_StartWithPlayerCsAndSetFlag(this->csIdList[this->csIdIndex], &this->actor);
    } else {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
    }
}

void func_80B10E98(EnGb2* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->unk_26C & 2) {
            this->unk_26C &= ~2;
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            if ((this->unk_26E != 0x14E8) && (this->unk_26E != 0x14EA)) {
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                this->actionFunc = func_80B10B5C;
            } else if (Flags_GetSwitch(play, ENGB2_GET_SWITCH_FLAG(&this->actor))) {
                this->actionFunc = func_80B10A48;
            } else {
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                this->csIdIndex = 2;
                CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
                this->actionFunc = func_80B10DAC;
            }
        } else {
            s32 temp;

            this->unk_26E = func_80B0FB24(this);
            Message_StartTextbox(play, this->unk_26E, &this->actor);
            temp = this->unk_26E;
            if ((temp == 0x14E7) || (temp == 0x14E9) || (temp == 0x14EC) || (temp == 0x14F0)) {
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                this->csIdIndex = 1;
                CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
                this->actionFunc = func_80B10DAC;
            }
        }
    }
}

void func_80B11048(EnGb2* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Message_StartTextbox(play, this->unk_26E, &this->actor);
        this->actionFunc = func_80B10DAC;
    } else if (this->actor.xzDistToPlayer < 300.0f) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalk(&this->actor, play, 200.0f);
    }
}

void func_80B110F8(EnGb2* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->unk_26C & 2) {
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->unk_26C &= ~2;
            this->actionFunc = func_80B10A48;
        } else {
            this->unk_26E = func_80B0F97C(this);
            Message_StartTextbox(play, this->unk_26E, &this->actor);
        }
    }
}

void func_80B111AC(EnGb2* this, PlayState* play) {
    s32 index;

    if (play->roomCtx.curRoom.num == 1) {
        return;
    }

    switch (play->roomCtx.curRoom.num) {
        case 2:
            index = 1;
            break;

        case 3:
        case 4:
            index = 2;
            break;

        case 5:
            index = 3;
            break;

        default:
            index = 0;
    }

    this->actor.world.pos.x = D_80B11A00[index][0];
    this->actor.world.pos.y = D_80B11A00[index][1];
    this->actor.world.pos.z = D_80B11A00[index][2];
    this->actor.world.rot.y = DEG_TO_BINANG(D_80B11A00[index][3]);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actionFunc = func_80B11268;
}

void func_80B11268(EnGb2* this, PlayState* play) {
    if (play->roomCtx.curRoom.num == 1) {
        this->csIdIndex = 0;
        this->csIdList[0] = this->actor.csId;
        if (Flags_GetClear(play, 2) && Flags_GetClear(play, 3) && Flags_GetClear(play, 4) && Flags_GetClear(play, 5)) {
            this->unk_28A = 0xFF;
            this->unk_26C &= ~0x100;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.draw = EnGb2_Draw;
            this->unk_26E = 0x14F9;
            this->actionFunc = func_80B11048;
        } else {
            this->actionFunc = func_80B111AC;
        }
    }
}

void func_80B11344(EnGb2* this, PlayState* play) {
    if (gSaveContext.rupeeAccumulator == 0) {
        func_80B0FE18(play);
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_4, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2200, ICHAIN_STOP),
};

void EnGb2_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGb2* this = (EnGb2*)thisx;

    if (func_80B0F660(this, play)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.room = -1;
    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_ps_Skel_007230, &object_ps_Anim_00049C, this->jointTable,
                       this->morphTable, OBJECT_PS_LIMB_MAX);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
    Actor_SetScale(&this->actor, 0.01f);

    switch (ENGB2_GET_7(&this->actor)) {
        case ENGB2_7_0:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_54_80)) {
                Actor_Kill(&this->actor);
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE)) {
                Actor_Kill(&this->actor);
            }

            if (gSaveContext.save.entrance == ENTRANCE(GHOST_HUT, 1)) {
                Environment_StopTime();
                this->actionFunc = func_80B10240;
                break;
            }

            this->unk_28A = 255;
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;

            if (CHECK_EVENTINF(EVENTINF_46)) {
                func_80B0F728(this, play);
            } else {
                func_80B0FBF0(this, play);
                func_80B0F6DC(this);
            }
            break;

        case ENGB2_7_1:
            if ((play->curSpawn == 1) || CHECK_WEEKEVENTREG(WEEKEVENTREG_80_80)) {
                Actor_Kill(&this->actor);
                return;
            }

            if (Flags_GetSwitch(play, ENGB2_GET_SWITCH_FLAG(thisx))) {
                Actor_Kill(&this->actor);
                return;
            }

            this->unk_28A = 255;
            func_80B0FA04(this);
            this->unk_28C = Player_GetMask(play);
            this->actionFunc = func_80B10B5C;
            break;

        case ENGB2_7_2:
            this->csIdIndex = 0;
            this->csIdList[0] = this->actor.csId;
            if (Flags_GetSwitch(play, ENGB2_GET_SWITCH_FLAG(thisx))) {
                Actor_Kill(&this->actor);
                return;
            }

            if (Flags_GetClear(play, 2) && Flags_GetClear(play, 3) && Flags_GetClear(play, 4) &&
                Flags_GetClear(play, 5)) {
                Actor_Kill(&this->actor);
                return;
            }

            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_76_80)) {
                this->actor.draw = NULL;
                this->unk_26C |= 0x100;
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->actionFunc = func_80B111AC;
            } else {
                this->unk_28A = 255;
                this->unk_26E = 0x14F5;
                this->actionFunc = func_80B11048;
            }
            break;

        default:
            Actor_Kill(&this->actor);
            return;
    }
}

void EnGb2_Destroy(Actor* thisx, PlayState* play) {
    EnGb2* this = (EnGb2*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnGb2_Update(Actor* thisx, PlayState* play) {
    EnGb2* this = (EnGb2*)thisx;

    SkelAnime_Update(&this->skelAnime);

    this->actionFunc(this, play);

    if (!(this->unk_26C & 0x100)) {
        func_80B0F5E0(this, play);
    }
    Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
}

s32 EnGb2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                           Gfx** gfx) {
    EnGb2* this = (EnGb2*)thisx;

    if (limbIndex == OBJECT_PS_LIMB_07) {
        Matrix_RotateYS(this->headRot.y, MTXMODE_APPLY);
    }

    if (limbIndex == OBJECT_PS_LIMB_01) {
        *dList = NULL;
    }

    return false;
}

void EnGb2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnGb2* this = (EnGb2*)thisx;
    Vec3f sp18 = { 2400.0f, 0.0f, 0.0f };

    if (limbIndex == OBJECT_PS_LIMB_07) {
        Matrix_MultVec3f(&sp18, &this->actor.focus.pos);
    }
}

void EnGb2_Draw(Actor* thisx, PlayState* play) {
    EnGb2* this = (EnGb2*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->unk_28A == 255) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);

        Scene_SetRenderModeXlu(play, 0, 1);
        POLY_OPA_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnGb2_OverrideLimbDraw, EnGb2_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    } else {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, this->unk_28A);

        Scene_SetRenderModeXlu(play, 1, 2);
        POLY_XLU_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnGb2_OverrideLimbDraw, EnGb2_PostLimbDraw, &this->actor, POLY_XLU_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
