#include "z_en_bom_bowl_man.h"
#include "vt.h"
#include "overlays/actors/ovl_En_Syateki_Niw/z_en_syateki_niw.h"
#include "overlays/actors/ovl_En_Ex_Item/z_en_ex_item.h"
#include "objects/object_bg/object_bg.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_27)

typedef enum {
    /* 0 */ CHU_GIRL_EYES_ASLEEP,
    /* 1 */ CHU_GIRL_EYES_OPEN_SLOWLY,
    /* 2 */ CHU_GIRL_EYES_BLINK_RAPIDLY,
    /* 3 */ CHU_GIRL_EYES_AWAKE
} BombchuGirlEyeMode;

void EnBomBowlMan_Init(Actor* thisx, PlayState* play);
void EnBomBowlMan_Destroy(Actor* thisx, PlayState* play);
void EnBomBowlMan_Update(Actor* thisx, PlayState* play);
void EnBomBowlMan_Draw(Actor* thisx, PlayState* play);

void EnBomBowMan_SetupWaitAsleep(EnBomBowlMan* this, PlayState* play);
void EnBomBowMan_WaitAsleep(EnBomBowlMan* this, PlayState* play);
void EnBomBowMan_TalkAsleep(EnBomBowlMan* this, PlayState* play);
void EnBomBowMan_WakeUp(EnBomBowlMan* this, PlayState* play);
void EnBomBowMan_BlinkAwake(EnBomBowlMan* this, PlayState* play);
void EnBomBowMan_CheckBeatenDC(EnBomBowlMan* this, PlayState* play);
void EnBomBowMan_WaitNotBeatenDC(EnBomBowlMan* this, PlayState* play);
void EnBomBowMan_TalkNotBeatenDC(EnBomBowlMan* this, PlayState* play);
void EnBomBowMan_SetupRunGame(EnBomBowlMan* this, PlayState* play);
void EnBomBowMan_RunGame(EnBomBowlMan* this, PlayState* play);
void EnBomBowlMan_HandlePlayChoice(EnBomBowlMan* this, PlayState* play);
void func_809C41FC(EnBomBowlMan* this, PlayState* play);
void EnBomBowMan_SetupChooseShowPrize(EnBomBowlMan* this, PlayState* play);
void EnBomBowMan_ChooseShowPrize(EnBomBowlMan* this, PlayState* play);
void EnBomBowlMan_BeginPlayGame(EnBomBowlMan* this, PlayState* play);

const ActorInit En_Bom_Bowl_Man_InitVars = {
    ACTOR_EN_BOM_BOWL_MAN,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_BG,
    sizeof(EnBomBowlMan),
    (ActorFunc)EnBomBowlMan_Init,
    (ActorFunc)EnBomBowlMan_Destroy,
    (ActorFunc)EnBomBowlMan_Update,
    (ActorFunc)EnBomBowlMan_Draw,
    NULL,
};

void EnBomBowlMan_Init(Actor* thisx, PlayState* play2) {
    static f32 cuccoColliderDims[][3] = { { 16.0f, 46.0f, 0.0f }, { 36.0f, 56.0f, 0.0f } };
    static Vec3f cuccoSpawnPos[] = { { 60, -60, -430 }, { 0, -120, -620 } };
    static f32 cuccoScales[] = { 0.01f, 0.03f };
    EnBomBowlMan* this = (EnBomBowlMan*)thisx;
    EnSyatekiNiw* cucco;
    s32 i;
    PlayState* play = play2;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gChuGirlSkel, &gChuGirlNoddingOffAnim, this->jointTable,
                       this->morphTable, 11);
    // "☆ Man, my shoulders hurt~ ☆"
    osSyncPrintf(VT_FGCOL(GREEN) "☆ もー 肩こっちゃうよねぇ〜 \t\t ☆ \n" VT_RST);
    // "☆ Isn't there some sort of job that will pay better and be more relaxing? ☆ %d"
    osSyncPrintf(VT_FGCOL(GREEN) "☆ もっとラクしてもうかるバイトないかしら？ ☆ %d\n" VT_RST,
                 play->bombchuBowlingStatus);
    this->posCopy = this->actor.world.pos;
    this->actor.shape.yOffset = -60.0f;
    Actor_SetScale(&this->actor, 0.013f);

    for (i = 0; i < 2; i++) {
        cucco = (EnSyatekiNiw*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_SYATEKI_NIW, cuccoSpawnPos[i].x,
                                           cuccoSpawnPos[i].y, cuccoSpawnPos[i].z, 0, 0, 0, 1);

        if (cucco != NULL) {
            cucco->unk_2F4 = cuccoScales[i];
            cucco->collider.dim.radius = (s16)cuccoColliderDims[i][0];
            cucco->collider.dim.height = (s16)cuccoColliderDims[i][1];
        }
    }

    this->prizeSelect = gSaveContext.n64ddFlag ? 0 : (s16)Rand_ZeroFloat(4.99f);
    this->actor.targetMode = 1;
    this->actionFunc = EnBomBowMan_SetupWaitAsleep;
}

void EnBomBowlMan_Destroy(Actor* thisx, PlayState* play) {
}

void EnBomBowMan_SetupWaitAsleep(EnBomBowlMan* this, PlayState* play) {
    this->frameCount = (f32)Animation_GetLastFrame(&gChuGirlNoddingOffAnim);
    Animation_Change(&this->skelAnime, &gChuGirlNoddingOffAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP, -10.0f);
    this->actor.textId = 0xC0;
    this->dialogState = TEXT_STATE_EVENT;
    this->actionFunc = EnBomBowMan_WaitAsleep;
}

void EnBomBowMan_WaitAsleep(EnBomBowlMan* this, PlayState* play) {
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = EnBomBowMan_TalkAsleep;
    } else {
        yawDiff = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y));

        if (!(this->actor.xzDistToPlayer > 120.0f) && (yawDiff < 0x4300)) {
            func_8002F2CC(&this->actor, play, 120.0f);
        }
    }
}

void EnBomBowMan_TalkAsleep(EnBomBowlMan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if ((Message_GetState(&play->msgCtx) == this->dialogState) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        this->actionFunc = EnBomBowMan_WakeUp;
    }
}

void EnBomBowMan_WakeUp(EnBomBowlMan* this, PlayState* play) {
    this->frameCount = (f32)Animation_GetLastFrame(&gChuGirlWakeUpAnim);
    Animation_Change(&this->skelAnime, &gChuGirlWakeUpAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_ONCE, -10.0f);
    this->eyeMode = CHU_GIRL_EYES_OPEN_SLOWLY;
    this->actionFunc = EnBomBowMan_BlinkAwake;
}

void EnBomBowMan_BlinkAwake(EnBomBowlMan* this, PlayState* play) {
    f32 frameCount = this->skelAnime.curFrame;

    SkelAnime_Update(&this->skelAnime);
    if (frameCount == 30.0f) {
        this->dialogState = TEXT_STATE_EVENT;

        // Check for beaten Dodongo's Cavern if Rando is disabled
        if (!gSaveContext.n64ddFlag) {
            if ((gSaveContext.eventChkInf[2] & 0x20) || BREG(2)) {
                this->actor.textId = 0xBF;
            } else {
                this->actor.textId = 0x7058;
            }
        }

        // In randomizer, only check for bomb bag when bombchus aren't in logic
        // and only check for bombchus when bombchus are in logic
        if (gSaveContext.n64ddFlag) {
            u8 bombchusInLogic = Randomizer_GetSettingValue(RSK_BOMBCHUS_IN_LOGIC);
            if ((!bombchusInLogic && INV_CONTENT(ITEM_BOMB) == ITEM_NONE) ||
                (bombchusInLogic && INV_CONTENT(ITEM_BOMBCHU) == ITEM_NONE)) {
                this->actor.textId = 0x7058;
            } else {
                this->actor.textId = 0xBF;
            }
        }
    }
    Message_ContinueTextbox(play, this->actor.textId);

    if ((this->eyeTextureIndex == 0) && (this->eyeMode == CHU_GIRL_EYES_BLINK_RAPIDLY) && (this->blinkTimer == 0)) {
        // Blink twice, then move on
        this->eyeTextureIndex = 2;
        this->blinkCount++;
        if (this->blinkCount >= 3) {
            // we're still making it in here when rando'd
            this->actionFunc = EnBomBowMan_CheckBeatenDC;
        }
    }
}

void EnBomBowMan_CheckBeatenDC(EnBomBowlMan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if ((Message_GetState(&play->msgCtx) == this->dialogState) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->frameCount = (f32)Animation_GetLastFrame(&gChuGirlLeanOverCounterAnim);
        Animation_Change(&this->skelAnime, &gChuGirlLeanOverCounterAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP,
                         -10.0f);
        this->eyeMode = CHU_GIRL_EYES_AWAKE;
        this->blinkTimer = (s16)Rand_ZeroFloat(60.0f) + 20;

        bool bombchuBowlingClosed;
        if (gSaveContext.n64ddFlag) {
            // when rando'd, check if we have bombchus if chus are in logic
            // and check if we have a bomb bag if chus aren't in logic
            u8 explosive = Randomizer_GetSettingValue(RSK_BOMBCHUS_IN_LOGIC) ? ITEM_BOMBCHU : ITEM_BOMB;
            bombchuBowlingClosed = (INV_CONTENT(explosive) == ITEM_NONE);
        } else {
            // if not rando'd, check if we have beaten Dodongo's Cavern
            bombchuBowlingClosed = !((gSaveContext.eventChkInf[2] & 0x20) || BREG(2));
        }
        if (bombchuBowlingClosed) {
            this->actionFunc = EnBomBowMan_WaitNotBeatenDC;
        } else {
            this->actor.textId = 0x18;
            this->dialogState = TEXT_STATE_CHOICE;
            Message_ContinueTextbox(play, this->actor.textId);
            this->actionFunc = EnBomBowlMan_HandlePlayChoice;
        }
    }
}

void EnBomBowMan_WaitNotBeatenDC(EnBomBowlMan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = EnBomBowMan_TalkNotBeatenDC;
    } else {
        func_8002F2CC(&this->actor, play, 120.0f);
    }
}

void EnBomBowMan_TalkNotBeatenDC(EnBomBowlMan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if ((Message_GetState(&play->msgCtx) == this->dialogState) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->actionFunc = EnBomBowMan_WaitNotBeatenDC;
    }
}

void EnBomBowMan_SetupRunGame(EnBomBowlMan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->minigamePlayStatus == 0) {
        if (!this->startedPlaying) {
            this->actor.textId = 0x18;
        } else {
            this->actor.textId = 0x1A;
        }

        this->dialogState = TEXT_STATE_CHOICE;
    } else {
        this->actor.textId = 0x19;
        this->dialogState = TEXT_STATE_EVENT;
    }

    this->actionFunc = EnBomBowMan_RunGame;
}

void EnBomBowMan_RunGame(EnBomBowlMan* this, PlayState* play) {
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);

    if (BREG(3)) {
        osSyncPrintf(VT_FGCOL(RED) "☆ game_play->bomchu_game_flag ☆ %d\n" VT_RST, play->bombchuBowlingStatus);
        // "HOW'S THE FIRST WALL DOING?"
        osSyncPrintf(VT_FGCOL(RED) "☆ 壁１の状態どう？ ☆ %d\n" VT_RST, this->wallStatus[0]);
        // "HOW'S THE SECOND WALL DOING?"
        osSyncPrintf(VT_FGCOL(RED) "☆ 壁２の状態どう？ ☆ %d\n" VT_RST, this->wallStatus[1]);
        // "HOLE INFORMATION"
        osSyncPrintf(VT_FGCOL(RED) "☆ 穴情報\t     ☆ %d\n" VT_RST, this->bowlPit->status);
        osSyncPrintf("\n\n");
    }

    this->gameResult = 0;

    if (this->bowlPit != NULL) {
        if ((this->wallStatus[0] != 1) && (this->wallStatus[1] != 1) && (this->bowlPit->status == 2)) {
            this->gameResult = 1; // Won
            this->bowlPit->status = 0;
            // "Center HIT!"
            osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 中央ＨＩＴ！！！！ ☆☆☆☆☆ \n" VT_RST);
        }

        if ((play->bombchuBowlingStatus == -1) &&
            (play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].length == 0) && (this->bowlPit->status == 0) &&
            (this->wallStatus[0] != 1) && (this->wallStatus[1] != 1)) {
            this->gameResult = 2; // Lost
            // "Bombchu lost"
            osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ ボムチュウ消化 ☆☆☆☆☆ \n" VT_RST);
        }
    }

    if (this->gameResult != 0) { // won or lost
        this->actor.textId = 0x1A;
        this->dialogState = TEXT_STATE_CHOICE;
        this->minigamePlayStatus = 0;

        if ((this->exItem != NULL) && (this->exItem->actor.update != NULL)) {
            this->exItem->killItem = true;
            this->exItem = NULL;
        }

        play->bombchuBowlingStatus = 0;
        this->playingAgain = true;
        Message_StartTextbox(play, this->actor.textId, NULL);

        if (this->gameResult == 2) {
            func_8002DF54(play, NULL, 8);
        }
        this->actionFunc = EnBomBowlMan_HandlePlayChoice;
    } else {
        if (Actor_ProcessTalkRequest(&this->actor, play)) {
            if (this->minigamePlayStatus == 0) {
                this->actionFunc = EnBomBowlMan_HandlePlayChoice;
            } else {
                this->actionFunc = func_809C41FC;
            }
        } else {
            yawDiff = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y));

            if (!(this->actor.xzDistToPlayer > 120.0f) && (yawDiff < 0x4300)) {
                func_8002F2CC(&this->actor, play, 120.0f);
            }
        }
    }
}

void EnBomBowlMan_HandlePlayChoice(EnBomBowlMan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if ((Message_GetState(&play->msgCtx) == this->dialogState) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);

        switch (play->msgCtx.choiceIndex) {
            case 0: // Yes
                if (gSaveContext.rupees >= 30) {
                    Rupees_ChangeBy(-30);
                    this->minigamePlayStatus = 1;
                    this->wallStatus[0] = this->wallStatus[1] = 0;
                    play->bombchuBowlingStatus = 10;
                    Flags_SetSwitch(play, 0x38);

                    if (!this->startedPlaying && !this->playingAgain) {
                        this->actor.textId = 0x19;
                        Message_ContinueTextbox(play, this->actor.textId);
                        this->dialogState = TEXT_STATE_EVENT;
                        this->actionFunc = func_809C41FC;
                    } else {
                        this->actor.textId = 0x1B;
                        Message_ContinueTextbox(play, this->actor.textId);
                        this->dialogState = TEXT_STATE_EVENT;
                        OnePointCutscene_Init(play, 8010, -99, NULL, MAIN_CAM);
                        func_8002DF54(play, NULL, 8);
                        this->actionFunc = EnBomBowMan_SetupChooseShowPrize;
                    }
                } else {
                    this->playingAgain = false;
                    this->actor.textId = 0x85;
                    Message_ContinueTextbox(play, this->actor.textId);
                    this->dialogState = TEXT_STATE_EVENT;
                    this->actionFunc = func_809C41FC;
                }
                break;

            case 1: // No
                this->playingAgain = false;
                this->actor.textId = 0x2D;
                Message_ContinueTextbox(play, this->actor.textId);
                this->dialogState = TEXT_STATE_EVENT;
                this->actionFunc = func_809C41FC;
        }
    }
}

void func_809C41FC(EnBomBowlMan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((Message_GetState(&play->msgCtx) == this->dialogState) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);

        if (((this->actor.textId == 0x2D) || (this->actor.textId == 0x85)) && Flags_GetSwitch(play, 0x38)) {
            Flags_UnsetSwitch(play, 0x38);
        }

        if (this->minigamePlayStatus == 1) {
            this->actor.textId = 0x1B;
            Message_ContinueTextbox(play, this->actor.textId);
            this->dialogState = TEXT_STATE_EVENT;
            OnePointCutscene_Init(play, 8010, -99, NULL, MAIN_CAM);
            func_8002DF54(play, NULL, 8);
            this->actionFunc = EnBomBowMan_SetupChooseShowPrize;
        } else {
            if (this->gameResult == 2) {
                func_8002DF54(play, NULL, 7);
            }
            this->actionFunc = EnBomBowMan_SetupRunGame;
        }
    }
}

void EnBomBowMan_SetupChooseShowPrize(EnBomBowlMan* this, PlayState* play) {
    Vec3f accel = { 0.0f, 0.1f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;

    SkelAnime_Update(&this->skelAnime);

    if ((Message_GetState(&play->msgCtx) == this->dialogState) && Message_ShouldAdvance(play)) {
        pos.x = 148.0f;
        pos.y = 40.0f;
        pos.z = 300.0f;
        EffectSsBomb2_SpawnLayered(play, &pos, &velocity, &accel, 50, 15);
        Audio_PlayActorSound2(&this->actor, NA_SE_IT_GOODS_APPEAR);
        this->prizeRevealTimer = 10;
        this->actionFunc = EnBomBowMan_ChooseShowPrize;
    }
}

static Vec3f sPrizePosOffset[] = {
    { 0.0f, 22.0f, 0.0f }, { 0.0f, 22.0f, 0.0f }, { 0.0f, 8.0f, 0.0f }, { 0.0f, 9.0f, 0.0f }, { 0.0f, -2.0f, 0.0f },
};

static s16 sPrizeRot[] = { 0x4268, 0x4268, -0x03E8, 0x0000, 0x4268, 0x0000 };

void EnBomBowMan_ChooseShowPrize(EnBomBowlMan* this, PlayState* play) {
    s16 prizeTemp;
    s32 pad;

    SkelAnime_Update(&this->skelAnime);

    if (this->prizeRevealTimer == 0) {
        switch (this->prizeSelect) {
            case 0:
                prizeTemp = EXITEM_BOMB_BAG_BOWLING;
                if (gSaveContext.itemGetInf[1] & 2) {
                    prizeTemp = EXITEM_PURPLE_RUPEE_BOWLING;
                }
                break;
            case 1:
                if (!gSaveContext.n64ddFlag) {
                    prizeTemp = EXITEM_PURPLE_RUPEE_BOWLING;
                } else {
                    prizeTemp = EXITEM_HEART_PIECE_BOWLING;
                    if (gSaveContext.itemGetInf[1] & 4) {
                        prizeTemp = EXITEM_PURPLE_RUPEE_BOWLING;
                    }
                }
                break;
            case 2:
                prizeTemp = EXITEM_BOMBCHUS_BOWLING;
                break;
            case 3:
                if (!gSaveContext.n64ddFlag) {
                    prizeTemp = EXITEM_HEART_PIECE_BOWLING;
                    if (gSaveContext.itemGetInf[1] & 4) {
                        prizeTemp = EXITEM_PURPLE_RUPEE_BOWLING;
                    }
                } else {
                    prizeTemp = EXITEM_PURPLE_RUPEE_BOWLING;
                }
                break;
            case 4:
                prizeTemp = EXITEM_BOMBS_BOWLING;
                break;
        }

        this->prizeIndex = prizeTemp;

        if (BREG(7)) {
            this->prizeIndex = BREG(7) - 1;
        }

        this->exItem = (EnExItem*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_EX_ITEM,
                                                     sPrizePosOffset[this->prizeIndex].x + 148.0f,
                                                     sPrizePosOffset[this->prizeIndex].y + 40.0f,
                                                     sPrizePosOffset[this->prizeIndex].z + 300.0f, 0,
                                                     sPrizeRot[this->prizeIndex], 0, this->prizeIndex + EXITEM_COUNTER);

        if (!this->startedPlaying) {
            this->bowlPit = (EnBomBowlPit*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play,
                                                              ACTOR_EN_BOM_BOWL_PIT, 0.0f, 90.0f, -860.0f, 0, 0, 0, 0);
            if (this->bowlPit != NULL) {
                this->bowlPit->prizeIndex = this->prizeIndex;
            }
        } else {
            this->bowlPit->prizeIndex = this->prizeIndex;
        }

        this->bowlPit->start = 1;
        this->minigamePlayStatus = 2;
        this->actor.textId = 0x405A;
        Message_ContinueTextbox(play, this->actor.textId);
        this->dialogState = TEXT_STATE_EVENT;

        // Cycle through prizes in order
        this->prizeSelect++;
        if (this->prizeSelect >= 5) {
            this->prizeSelect = 0;
        }
        this->actionFunc = EnBomBowlMan_BeginPlayGame;
    }
}

void EnBomBowlMan_BeginPlayGame(EnBomBowlMan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if ((Message_GetState(&play->msgCtx) == this->dialogState) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        func_8005B1A4(GET_ACTIVE_CAM(play));
        this->startedPlaying = true;

        if (BREG(2)) {
            BREG(2) = 0;
        }

        // "Wow"
        osSyncPrintf(VT_FGCOL(YELLOW) "☆ わー ☆ %d\n" VT_RST, play->bombchuBowlingStatus);
        func_8002DF54(play, NULL, 7);
        this->actionFunc = EnBomBowMan_SetupRunGame;
    }
}

void EnBomBowlMan_Update(Actor* thisx, PlayState* play) {
    EnBomBowlMan* this = (EnBomBowlMan*)thisx;

    this->timer++;
    this->actor.focus.pos.y = 60.0f;
    Actor_SetFocus(&this->actor, 60.0f);

    switch (this->eyeMode) {
        case CHU_GIRL_EYES_ASLEEP:
            this->eyeTextureIndex = 2;
            break;
        case CHU_GIRL_EYES_OPEN_SLOWLY:
            if (this->eyeTextureIndex > 0) {
                this->eyeTextureIndex--;
            } else {
                this->blinkTimer = 30;
                this->eyeMode = CHU_GIRL_EYES_BLINK_RAPIDLY;
            }
            break;
        case CHU_GIRL_EYES_BLINK_RAPIDLY:
            if ((this->blinkTimer == 0) && (this->eyeTextureIndex > 0)) {
                this->eyeTextureIndex--;
            }
            break;
        default:
            if (this->blinkTimer == 0) {
                this->eyeTextureIndex++;
                if (this->eyeTextureIndex >= 3) {
                    this->eyeTextureIndex = 0;
                    this->blinkTimer = (s16)Rand_ZeroFloat(60.0f) + 20;
                }
            }

            func_80038290(play, &this->actor, &this->unk_218, &this->unk_224, this->actor.focus.pos);
            break;
    }

    DECR(this->prizeRevealTimer);
    DECR(this->blinkTimer);

    this->actionFunc(this, play);
}

s32 EnBomBowlMan_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* thisx) {
    EnBomBowlMan* this = (EnBomBowlMan*)thisx;

    if (limbIndex == 4) { // head
        rot->x += this->unk_218.y;
        rot->z += this->unk_218.z;
    }

    return false;
}

void EnBomBowlMan_Draw(Actor* thisx, PlayState* play) {
    static void* eyeTextures[] = { gChuGirlEyeOpenTex, gChuGirlEyeHalfTex, gChuGirlEyeClosedTex };
    EnBomBowlMan* this = (EnBomBowlMan*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTextureIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnBomBowlMan_OverrideLimbDraw, NULL, this);

    CLOSE_DISPS(play->state.gfxCtx);
}
