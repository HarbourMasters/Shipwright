/*
 * File: z_en_takara_man.c
 * Overlay: ovl_En_Takara_Man
 * Description: Treasure Chest Game Man
 */

#include "z_en_takara_man.h"
#include "vt.h"
#include "objects/object_ts/object_ts.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_27)

void EnTakaraMan_Init(Actor* thisx, PlayState* play);
void EnTakaraMan_Reset(Actor* thisx, PlayState* play);
void EnTakaraMan_Update(Actor* thisx, PlayState* play);
void EnTakaraMan_Draw(Actor* thisx, PlayState* play);

void func_80B176E0(EnTakaraMan* this, PlayState* play);
void func_80B1778C(EnTakaraMan* this, PlayState* play);
void func_80B17B14(EnTakaraMan* this, PlayState* play);
void func_80B17934(EnTakaraMan* this, PlayState* play);
void func_80B17A6C(EnTakaraMan* this, PlayState* play);
void func_80B17AC4(EnTakaraMan* this, PlayState* play);

const ActorInit En_Takara_Man_InitVars = {
    ACTOR_EN_TAKARA_MAN,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_TS,
    sizeof(EnTakaraMan),
    (ActorFunc)EnTakaraMan_Init,
    NULL,
    (ActorFunc)EnTakaraMan_Update,
    (ActorFunc)EnTakaraMan_Draw,
    (ActorResetFunc)EnTakaraMan_Reset,
};

u8 sTakaraIsInitialized = false;

void EnTakaraMan_Reset(Actor* thisx, PlayState* play) {
    sTakaraIsInitialized = false;
}

void EnTakaraMan_Init(Actor* thisx, PlayState* play) {
    EnTakaraMan* this = (EnTakaraMan*)thisx;

    if (sTakaraIsInitialized) {
        Actor_Kill(&this->actor);
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ もういてる原 ☆☆☆☆☆ \n" VT_RST); // "Already initialized"
        return;
    }

    sTakaraIsInitialized = true;
    osSyncPrintf("\n\n");
    // "Bun! %x" (needs a better translation)
    osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ ばぅん！ ☆☆☆☆☆ %x\n" VT_RST, play->actorCtx.flags.chest);
    play->actorCtx.flags.chest = 0;
    gSaveContext.inventory.dungeonKeys[gSaveContext.mapIndex] = -1;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_ts_Skel_004FE0, &object_ts_Anim_000498, this->jointTable,
                       this->morphTable, 10);
    thisx->focus.pos = thisx->world.pos;
    this->pos = thisx->world.pos;
    thisx->world.pos.x = 133.0f;
    thisx->world.pos.y = -12.0f;
    thisx->world.pos.z = 102.0f;
    Actor_SetScale(&this->actor, 0.013f);
    this->height = 90.0f;
    this->originalRoomNum = thisx->room;
    thisx->room = -1;
    thisx->world.rot.y = thisx->shape.rot.y = -0x4E20;
    thisx->targetMode = 1;
    this->actionFunc = func_80B176E0;
}

void func_80B176E0(EnTakaraMan* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&object_ts_Anim_000498);

    Animation_Change(&this->skelAnime, &object_ts_Anim_000498, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    if (!this->unk_214) {
        this->actor.textId = 0x6D;
        this->dialogState = TEXT_STATE_CHOICE;
    }
    this->actionFunc = func_80B1778C;
}

void func_80B1778C(EnTakaraMan* this, PlayState* play) {
    s16 absYawDiff;
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);
    if (Actor_ProcessTalkRequest(&this->actor, play) && this->dialogState != TEXT_STATE_DONE) {
        if (!this->unk_214) {
            this->actionFunc = func_80B17934;
        } else {
            this->actionFunc = func_80B17B14;
        }
    } else {
        yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        if (play->roomCtx.curRoom.num == 6 && !this->unk_21A) {
            this->actor.textId = 0x6E;
            this->unk_21A = 1;
            this->dialogState = TEXT_STATE_DONE;
        }

        if (!this->unk_21A && this->unk_214) {
            if (Flags_GetSwitch(play, 0x32)) {
                this->actor.textId = 0x84;
                this->dialogState = TEXT_STATE_EVENT;
            } else {
                this->actor.textId = 0x704C;
                this->dialogState = TEXT_STATE_DONE;
            }
        }

        absYawDiff = ABS(yawDiff);
        if (absYawDiff < 0x4300) {
            if (play->roomCtx.curRoom.num != this->originalRoomNum) {
                this->actor.flags &= ~ACTOR_FLAG_0;
                this->unk_218 = 0;
            } else {
                if (!this->unk_218) {
                    this->actor.flags |= ACTOR_FLAG_0;
                    this->unk_218 = 1;
                }
                func_8002F2CC(&this->actor, play, 100.0f);
            }
        }
    }
}

void func_80B17934(EnTakaraMan* this, PlayState* play) {
    if (this->dialogState == Message_GetState(&play->msgCtx) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // Yes
                if (gSaveContext.rupees >= 10) {
                    Message_CloseTextbox(play);
                    Rupees_ChangeBy(-10);
                    this->unk_214 = 1;
                    this->actor.parent = NULL;
                    func_8002F434(&this->actor, play, GI_DOOR_KEY, 2000.0f, 1000.0f);
                    this->actionFunc = func_80B17A6C;
                } else {
                    Message_CloseTextbox(play);
                    this->actor.textId = 0x85;
                    Message_ContinueTextbox(play, this->actor.textId);
                    this->dialogState = TEXT_STATE_EVENT;
                    this->actionFunc = func_80B17B14;
                }
                break;
            case 1: // No
                Message_CloseTextbox(play);
                this->actor.textId = 0x2D;
                Message_ContinueTextbox(play, this->actor.textId);
                this->dialogState = TEXT_STATE_EVENT;
                this->actionFunc = func_80B17B14;
                break;
        }
    }
}

void func_80B17A6C(EnTakaraMan* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actionFunc = func_80B17AC4;
    } else {
        func_8002F434(&this->actor, play, GI_DOOR_KEY, 2000.0f, 1000.0f);
    }
}

void func_80B17AC4(EnTakaraMan* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_DONE && Message_ShouldAdvance(play)) {
        this->actionFunc = func_80B176E0;
    }
}

void func_80B17B14(EnTakaraMan* this, PlayState* play) {
    if (this->dialogState == Message_GetState(&play->msgCtx) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->actionFunc = func_80B176E0;
    }
}

void EnTakaraMan_Update(Actor* thisx, PlayState* play) {
    EnTakaraMan* this = (EnTakaraMan*)thisx;

    if (this->eyeTimer != 0) {
        this->eyeTimer--;
    }

    Actor_SetFocus(&this->actor, this->height);
    func_80038290(play, &this->actor, &this->unk_22C, &this->unk_232, this->actor.focus.pos);
    if (this->eyeTimer == 0) {
        this->eyeTextureIdx++;
        if (this->eyeTextureIdx >= 2) {
            this->eyeTextureIdx = 0;
            this->eyeTimer = (s16)Rand_ZeroFloat(60.0f) + 20;
        }
    }
    this->unk_212++;
    this->actionFunc(this, play);
}

s32 EnTakaraMan_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                 void* thisx) {
    EnTakaraMan* this = (EnTakaraMan*)thisx;

    if (limbIndex == 1) {
        rot->x += this->unk_232.y;
    }
    if (limbIndex == 8) {
        rot->x += this->unk_22C.y;
        rot->z += this->unk_22C.z;
    }
    return false;
}

void EnTakaraMan_Draw(Actor* thisx, PlayState* play) {
    static void* eyeTextures[] = {
        object_ts_Tex_000970,
        object_ts_Tex_000D70,
    };
    EnTakaraMan* this = (EnTakaraMan*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTextureIdx]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnTakaraMan_OverrideLimbDraw, NULL, this);

    CLOSE_DISPS(play->state.gfxCtx);
}
