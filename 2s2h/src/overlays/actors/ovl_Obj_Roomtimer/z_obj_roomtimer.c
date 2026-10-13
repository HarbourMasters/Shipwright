/*
 * File: z_obj_roomtimer.c
 * Overlay: ovl_Obj_Roomtimer
 * Description:
 */

#include "z_obj_roomtimer.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjRoomtimer_Init(Actor* thisx, PlayState* play);
void ObjRoomtimer_Destroy(Actor* thisx, PlayState* play);
void ObjRoomtimer_Update(Actor* thisx, PlayState* play);

void func_80973CD8(ObjRoomtimer* this, PlayState* play);
void func_80973D3C(ObjRoomtimer* this, PlayState* play);
void func_80973DE0(ObjRoomtimer* this, PlayState* play);

ActorProfile Obj_Roomtimer_Profile = {
    /**/ ACTOR_OBJ_ROOMTIMER,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjRoomtimer),
    /**/ ObjRoomtimer_Init,
    /**/ ObjRoomtimer_Destroy,
    /**/ ObjRoomtimer_Update,
    /**/ NULL,
};

void ObjRoomtimer_Init(Actor* thisx, PlayState* play) {
    ObjRoomtimer* this = (ObjRoomtimer*)thisx;

    this->switchFlag = ROOMTIMER_GET_SWITCH_FLAG(thisx);
    this->actor.params &= 0x1FF;

    if (this->actor.params != 0x1FF) {
        this->actor.params = CLAMP_MAX(this->actor.params, 500);
    }
    this->actionFunc = func_80973CD8;
}

void ObjRoomtimer_Destroy(Actor* thisx, PlayState* play) {
    ObjRoomtimer* this = (ObjRoomtimer*)thisx;

    if ((this->actor.params != 0x1FF) && (gSaveContext.timerStates[TIMER_ID_MINIGAME_2] >= TIMER_STATE_START)) {
        gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
    }
}

void func_80973CD8(ObjRoomtimer* this, PlayState* play) {
    if (this->actor.params != 0x1FF) {
        Interface_StartTimer(TIMER_ID_MINIGAME_2, this->actor.params);
    }

    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
    this->actionFunc = func_80973D3C;
}

void func_80973D3C(ObjRoomtimer* this, PlayState* play) {
    if (Flags_GetClearTemp(play, this->actor.room)) {
        if (this->actor.params != 0x1FF) {
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
        }
        CutsceneManager_Queue(this->actor.csId);
        this->actionFunc = func_80973DE0;
    } else if ((this->actor.params != 0x1FF) && (gSaveContext.timerStates[TIMER_ID_MINIGAME_2] == TIMER_STATE_OFF)) {
        Audio_PlaySfx(NA_SE_OC_ABYSS);
        func_80169EFC(play);
        Actor_Kill(&this->actor);
    }
}

void func_80973DE0(ObjRoomtimer* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->actor.csId)) {
        Flags_SetClear(play, this->actor.room);
        Flags_SetSwitch(play, this->switchFlag);
        if (CutsceneManager_GetLength(this->actor.csId) != -1) {
            CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        }
        Actor_Kill(&this->actor);
        return;
    }

    CutsceneManager_Queue(this->actor.csId);
}

void ObjRoomtimer_Update(Actor* thisx, PlayState* play) {
    ObjRoomtimer* this = (ObjRoomtimer*)thisx;

    this->actionFunc(this, play);
}
