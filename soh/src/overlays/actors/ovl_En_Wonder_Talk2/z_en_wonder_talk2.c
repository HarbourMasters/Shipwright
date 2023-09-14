/*
 * File: z_en_wonder_talk2.c
 * Overlay: ovl_En_Wonder_Talk2
 * Description: Dialog spot
 */

#include "z_en_wonder_talk2.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_NO_LOCKON)

void EnWonderTalk2_Init(Actor* thisx, PlayState* play);
void EnWonderTalk2_Destroy(Actor* thisx, PlayState* play);
void EnWonderTalk2_Update(Actor* thisx, PlayState* play);

void func_80B3A10C(EnWonderTalk2* this, PlayState* play);
void func_80B3A4F8(EnWonderTalk2* this, PlayState* play);
void func_80B3A15C(EnWonderTalk2* this, PlayState* play);
void func_80B3A3D4(EnWonderTalk2* this, PlayState* play);
void EnWonderTalk2_DoNothing(EnWonderTalk2* this, PlayState* play);

const ActorInit En_Wonder_Talk2_InitVars = {
    ACTOR_EN_WONDER_TALK2,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnWonderTalk2),
    (ActorFunc)EnWonderTalk2_Init,
    (ActorFunc)EnWonderTalk2_Destroy,
    (ActorFunc)EnWonderTalk2_Update,
    NULL,
    NULL,
};

static s16 D_80B3A8E0[] = { 6, 0, 1, 2, 3, 4, 5 };

void EnWonderTalk2_Destroy(Actor* thisx, PlayState* play) {
}

void EnWonderTalk2_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnWonderTalk2* this = (EnWonderTalk2*)thisx;

    osSyncPrintf("\n\n");
    // "Transparent message"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 透明メッセージ君 ☆☆☆☆☆ %x\n" VT_RST, this->actor.params);
    this->baseMsgId = (this->actor.params >> 6) & 0xFF;
    if (this->actor.world.rot.z > 0) {
        s32 rangeIndex = 0;
        s16 rotZmod10 = this->actor.world.rot.z;

        while (rotZmod10 > 10) {
            rotZmod10 -= 10;
            rangeIndex++;
        }
        // rangeIndex = rot.z/10 here
        this->triggerRange = rotZmod10 * 40.0f;
        if (rangeIndex > 6) {
            rangeIndex = 0;
        }

        this->actor.targetMode = D_80B3A8E0[rangeIndex];

        osSyncPrintf("\n\n");
        // "originally?"
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 元は？       ☆☆☆☆☆ %d\n" VT_RST, this->actor.world.rot.z);
        // "The range is?"
        osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ レンジは？   ☆☆☆☆☆ %d\n" VT_RST, this->actor.targetMode);
        // "Is the range?"
        osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ は、範囲わ？ ☆☆☆☆☆ %f\n" VT_RST, this->triggerRange);
        osSyncPrintf("\n\n");
        osSyncPrintf("\n\n");
        osSyncPrintf("\n\n");
    }

    this->initPos = this->actor.world.pos;
    this->switchFlag = (this->actor.params & 0x3F);
    this->talkMode = ((this->actor.params >> 0xE) & 3);

    if (this->switchFlag == 0x3F) {
        this->switchFlag = -1;
    }
    if (this->switchFlag >= 0 && Flags_GetSwitch(play, this->switchFlag)) {
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ Ｙｏｕ ａｒｅ Ｓｈｏｃｋ！  ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
        Actor_Kill(&this->actor);
        return;
    }
    if ((this->talkMode == 1) && (play->sceneNum == SCENE_GERUDO_TRAINING_GROUND) && (this->switchFlag != 0x08) &&
        (this->switchFlag != 0x16) && (this->switchFlag != 0x2F)) {

        this->unk_15A = false;
        this->talkMode = 4;
    }
    if (this->talkMode == 3) {
        this->actor.flags &= ~ACTOR_FLAG_NO_LOCKON;
        this->actionFunc = EnWonderTalk2_DoNothing;
    } else {
        this->actionFunc = func_80B3A10C;
    }
}

void func_80B3A10C(EnWonderTalk2* this, PlayState* play) {
    this->actor.textId = 0x200;
    this->actor.textId |= this->baseMsgId;
    if (this->talkMode == 1 || this->talkMode == 4) {
        this->actionFunc = func_80B3A4F8;
    } else {
        this->actionFunc = func_80B3A15C;
    }
}

void func_80B3A15C(EnWonderTalk2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->unk_158++;
    if ((this->switchFlag >= 0) && Flags_GetSwitch(play, this->switchFlag)) {
        if (!this->unk_15A) {
            this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
            this->unk_15A = true;
        }
    } else if (Actor_ProcessTalkRequest(&this->actor, play)) {
        if ((this->switchFlag >= 0) && (this->talkMode != 2)) {
            Flags_SetSwitch(play, this->switchFlag);
            // "I saved it! All of it!"
            osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ セーブしたよ！おもいっきり！ %x\n" VT_RST, this->switchFlag);
        }

        this->actionFunc = func_80B3A10C;
    } else {
        s16 yawDiff = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.world.rot.y));

        if (!((this->actor.xzDistToPlayer > 40.0f + this->triggerRange) ||
              (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) > 100.0f) || (yawDiff >= 0x4000))) {
            if (this->unk_158 >= 2) {
                osSyncPrintf("\n\n");
                // "Transparent Message Kimi Set"
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 透明メッセージ君せっと %x\n" VT_RST, this->actor.params);
                // "Save Information"
                osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ セーブ情報 \t           %x\n" VT_RST, this->switchFlag);
                // "Specified message type"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 指定メッセージ種類     %x\n" VT_RST, this->baseMsgId);
                // "Actual message type"
                osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 実質メッセージ種類     %x\n" VT_RST, this->actor.textId);
                // "Specified range"
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 指定範囲               %d\n" VT_RST, this->actor.world.rot.z);
                // "Processing range"
                osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 処理範囲               %f\n" VT_RST, this->triggerRange);
                switch (this->talkMode) {
                    case 0:
                        // "Normal"
                        osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆ 通常 ☆☆ \n" VT_RST);
                        break;
                    case 2:
                        // "Check only"
                        osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆ チェックのみ ☆☆ \n" VT_RST);
                        break;
                    case 3:
                        // "Lock only"
                        osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆ ロックのみ ☆☆ \n" VT_RST);
                        break;
                }
            }

            this->unk_158 = 0;
            func_8002F1C4(&this->actor, play, this->triggerRange + 50.0f, 100.0f, EXCH_ITEM_NONE);
        }
    }
}

void func_80B3A3D4(EnWonderTalk2* this, PlayState* play) {
    if (BREG(2) != 0) {
        // "Oh"
        osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ わー %d\n" VT_RST, Message_GetState(&play->msgCtx));
    }

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                if (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) {
                    Message_CloseTextbox(play);
                }
            } else {
                break;
            }
        case TEXT_STATE_NONE:
            if ((this->switchFlag >= 0) && (this->talkMode != 4)) {
                Flags_SetSwitch(play, this->switchFlag);
                // "(Forced) I saved it! All of it!"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ (強制)セーブしたよ！おもいっきり！ %x\n" VT_RST, this->switchFlag);
            }

            if (this->talkMode == 4) {
                this->unk_15A = true;
            }
            this->actor.flags &= ~(ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_UPDATE_WHILE_CULLED);
            func_8002DF54(play, NULL, 7);
            this->unk_156 = true;
            this->actionFunc = func_80B3A4F8;
            break;
    }
}

void func_80B3A4F8(EnWonderTalk2* this, PlayState* play) {
    Player* player;

    player = GET_PLAYER(play);
    this->unk_158++;
    if (this->switchFlag >= 0 && Flags_GetSwitch(play, this->switchFlag)) {
        if (!this->unk_15A) {
            this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
            this->unk_15A = true;
        }
    } else if ((this->talkMode != 4) || !this->unk_15A) {
        if (BREG(2) != 0) {
            // "distance"
            osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ きょり %f\n" VT_RST, this->actor.xzDistToPlayer);
        }
        if (((this->actor.xzDistToPlayer < (40.0f + this->triggerRange)) &&
             (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 100.0f)) &&
            !Play_InCsMode(play)) {
            if (this->unk_158 >= 2) {
                osSyncPrintf("\n\n");
                // "Transparent Message Kimi Seto"
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 透明メッセージ君せっと %x\n" VT_RST, this->actor.params);
                // "Save Information"
                osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ セーブ情報 \t           %x\n" VT_RST, this->switchFlag);
                // "Specified message type"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 指定メッセージ種類     %x\n" VT_RST, this->baseMsgId);
                // "Real message type"
                osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 実質メッセージ種類     %x\n" VT_RST, this->actor.textId);
                // "Specified range"
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 指定範囲               %d\n" VT_RST, this->actor.world.rot.z);
                // "Processing range"
                osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 処理範囲               %f\n" VT_RST, this->triggerRange);
                // "What is your range?"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ レンジは？ \t\t   %d\n" VT_RST, this->actor.targetMode);
                osSyncPrintf("\n\n");
                osSyncPrintf("\n\n");
                switch (this->talkMode) {
                    case 1:
                        // "Compulsion"
                        osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆ 強制 ☆☆ \n" VT_RST);
                        break;
                    case 4:
                        // "Gerudo Training Grounds Forced Check Only"
                        osSyncPrintf(VT_FGCOL(RED) " ☆☆ ゲルドの修練場強制チェックのみ ☆☆ \n" VT_RST);
                        break;
                }

                osSyncPrintf("\n\n");
            }
            this->unk_158 = 0;
            if (!this->unk_156) {
                // Whether or not to skip the text in rando
                bool randoSkipText = false;
                if (gSaveContext.n64ddFlag) {
                    // Scenes for which all of this type of wonder talk should be skipped.
                    switch (play->sceneNum) {
                        case 0x0007: // Shadow Temple
                            randoSkipText = true;
                            break;
                        case 0x000B: // Gerudo Training Grounds
                            randoSkipText = true;
                            break;
                        case 0x000C: // Inside Gerudo Fortress
                            randoSkipText = true;
                            break;
                        default:
                            break;
                    }
                    // individual textIds that should be skipped, or that should be preserved
                    // in a scene that otherwise has all wonder talk skipped.
                    //switch (this->actor.textId) { 
                    //    case: 0x023c //textId we want to skip
                    //        randoSkipText = true;
                    //        break;
                    //    case 0x023c: // textId in a skipped scene that we don't want to skip
                    //        randoSkipText = false;
                    //        break;
                    //    default:
                    //        break;
                    //}
                }
                if (!(randoSkipText)) {
                    Message_StartTextbox(play, this->actor.textId, NULL);
                    func_8002DF54(play, NULL, 8);
                    this->actor.flags |= ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_UPDATE_WHILE_CULLED;
                    this->actionFunc = func_80B3A3D4;
                }
            }

        } else {
            this->unk_156 = false;
        }
    }
}

void EnWonderTalk2_DoNothing(EnWonderTalk2* this, PlayState* play) {
}

void EnWonderTalk2_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnWonderTalk2* this = (EnWonderTalk2*)thisx;

    this->actionFunc(this, play);
    this->actor.world.pos.y = this->initPos.y;

    Actor_SetFocus(&this->actor, this->height);

    if (BREG(0) != 0) {
        if (this->unk_158 != 0) {
            if ((this->unk_158 & 1) == 0) {
                DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                       this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 70, 70, 70, 255, 4, play->state.gfxCtx);
            }
        } else {
            DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                   this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                   1.0f, 1.0f, 0, 0, 255, 255, 4, play->state.gfxCtx);
        }
    }
}
