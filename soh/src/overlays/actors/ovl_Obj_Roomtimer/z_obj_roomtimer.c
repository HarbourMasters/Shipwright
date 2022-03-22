/*
 * File: z_obj_roomtimer.c
 * Overlay: ovl_Obj_Roomtimer
 * Description: Starts Timer 1 with a value specified in params
 */

#include "z_obj_roomtimer.h"

#define FLAGS ACTOR_FLAG_4

void ObjRoomtimer_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjRoomtimer_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjRoomtimer_Update(Actor* thisx, GlobalContext* globalCtx);

void func_80B9D054(ObjRoomtimer* this, GlobalContext* globalCtx);
void func_80B9D0B0(ObjRoomtimer* this, GlobalContext* globalCtx);

const ActorInit Obj_Roomtimer_InitVars = {
    ACTOR_OBJ_ROOMTIMER,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjRoomtimer),
    (ActorFunc)ObjRoomtimer_Init,
    (ActorFunc)ObjRoomtimer_Destroy,
    (ActorFunc)ObjRoomtimer_Update,
    (ActorFunc)NULL,
    NULL,
};

void ObjRoomtimer_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjRoomtimer* this = (ObjRoomtimer*)thisx;
    s16 params = this->actor.params;

    this->switchFlag = (params >> 10) & 0x3F;
    this->actor.params = params & 0x3FF;
    params = this->actor.params;

    if (params != 0x3FF) {
        if (params > 600) {
            this->actor.params = 600;
        } else {
            this->actor.params = params;
        }
    }

    this->actionFunc = func_80B9D054;
}

void ObjRoomtimer_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    ObjRoomtimer* this = (ObjRoomtimer*)thisx;

    if ((this->actor.params != 0x3FF) && (gSaveContext.timer1Value > 0)) {
        gSaveContext.timer1State = 10;
    }
}

void func_80B9D054(ObjRoomtimer* this, GlobalContext* globalCtx) {
    if (this->actor.params != 0x3FF) {
        func_80088B34(this->actor.params);
    }

    Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &this->actor, ACTORCAT_PROP);
    this->actionFunc = func_80B9D0B0;
}

void func_80B9D0B0(ObjRoomtimer* this, GlobalContext* globalCtx) {
    if (Flags_GetTempClear(globalCtx, this->actor.room)) {
        if (this->actor.params != 0x3FF) {
            gSaveContext.timer1State = 10;
        }
        Flags_SetClear(globalCtx, this->actor.room);
        Flags_SetSwitch(globalCtx, this->switchFlag);
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        Actor_Kill(&this->actor);
    } else {
        if ((this->actor.params != 0x3FF) && (gSaveContext.timer1Value == 0)) {
            Audio_PlaySoundGeneral(NA_SE_OC_ABYSS, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            Gameplay_TriggerVoidOut(globalCtx);
            Actor_Kill(&this->actor);
        }
    }
}

void ObjRoomtimer_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjRoomtimer* this = (ObjRoomtimer*)thisx;

    this->actionFunc(this, globalCtx);
}
