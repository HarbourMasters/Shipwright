/*
 * File: z_en_eg.c
 * Overlay: ovl_En_Eg
 * Description: Triggers a void out (used in the tower collapse sequence?)
 */

#include "z_en_eg.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void EnEg_Init(Actor* thisx, PlayState* play);
void EnEg_Destroy(Actor* thisx, PlayState* play);
void EnEg_Update(Actor* thisx, PlayState* play);
void EnEg_Draw(Actor* thisx, PlayState* play);

void func_809FFDC8(EnEg* this, PlayState* play);

static s32 voided = false;

static EnEgActionFunc sActionFuncs[] = {
    func_809FFDC8,
};

const ActorInit En_Eg_InitVars = {
    ACTOR_EN_EG,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_ZL2,
    sizeof(EnEg),
    (ActorFunc)EnEg_Init,
    (ActorFunc)EnEg_Destroy,
    (ActorFunc)EnEg_Update,
    (ActorFunc)EnEg_Draw,
    NULL,
};

void EnEg_PlayVoidOutSFX() {
    func_800788CC(NA_SE_OC_ABYSS);
}

void EnEg_Destroy(Actor* thisx, PlayState* play) {
    voided = false;
}

void EnEg_Init(Actor* thisx, PlayState* play) {
    EnEg* this = (EnEg*)thisx;

    this->action = 0;
}

void func_809FFDC8(EnEg* this, PlayState* play) {
    if (!voided && (gSaveContext.timer2Value < 1) && Flags_GetSwitch(play, 0x36) && (kREG(0) == 0)) {
        // Void the player out
        Play_TriggerRespawn(play);
        gSaveContext.respawnFlag = -2;
        Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);
        play->fadeTransition = 2;
        EnEg_PlayVoidOutSFX();
        voided = true;
    }
}

void EnEg_Update(Actor* thisx, PlayState* play) {
    EnEg* this = (EnEg*)thisx;
    s32 action = this->action;

    if (((action < 0) || (0 < action)) || (sActionFuncs[action] == NULL)) {
        // "Main Mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        sActionFuncs[action](this, play);
    }
}

void EnEg_Draw(Actor* thisx, PlayState* play) {
}
