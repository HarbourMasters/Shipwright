/*
 * File: z_obj_demo.c
 * Overlay: ovl_Obj_Demo
 * Description: Cutscene triggers when player is in range
 */

#include "z_obj_demo.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjDemo_Init(Actor* thisx, PlayState* play);
void ObjDemo_Update(Actor* thisx, PlayState* play);

void func_80983678(ObjDemo* this, PlayState* play);
void func_80983704(ObjDemo* this, PlayState* play);
void func_80983634(PlayState* play);

ActorProfile Obj_Demo_Profile = {
    /**/ ACTOR_OBJ_DEMO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjDemo),
    /**/ ObjDemo_Init,
    /**/ Actor_Noop,
    /**/ ObjDemo_Update,
    /**/ NULL,
};

void ObjDemo_Init(Actor* thisx, PlayState* play) {
    ObjDemo* this = (ObjDemo*)thisx;

    thisx->params = OBJDEMO_GET_SWITCH_FLAG_MASK(thisx);
    if ((OBJDEMO_GET_SWITCH_FLAG(thisx) != 0xFF) && Flags_GetSwitch(play, OBJDEMO_GET_SWITCH_FLAG(thisx))) {
        Actor_Kill(&this->actor);
        return;
    }
    if (thisx->shape.rot.x < 0) {
        this->unk_148 = 1;
        thisx->shape.rot.x = -thisx->shape.rot.x;
    }
    if (thisx->shape.rot.x == 0) {
        this->xzRange = 80.0f;
    } else {
        this->xzRange = thisx->shape.rot.x * 4.0f;
        thisx->shape.rot.x = 0;
        thisx->world.rot.x = 0;
    }
    if (thisx->shape.rot.z == 0) {
        this->yRange = 60.0f;
    } else {
        this->yRange = thisx->shape.rot.z * 4.0f;
        thisx->shape.rot.z = 0;
        thisx->world.rot.z = 0;
    }
    func_80983634(play);
    this->actionFunc = func_80983678;
}

void func_80983634(PlayState* play) {
    if ((play->sceneId == SCENE_CASTLE) && (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) == NA_BGM_IKANA_CASTLE)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
    }
}

void func_80983678(ObjDemo* this, PlayState* play) {
    func_80983634(play);
    if ((this->actor.xzDistToPlayer < this->xzRange) && (fabsf(this->actor.playerHeightRel) < this->yRange)) {
        if (this->unk_148 == 1) {
            CutsceneManager_Stop(CS_ID_GLOBAL_DOOR);
        }
        CutsceneManager_Queue(this->actor.csId);
        this->actionFunc = func_80983704;
    }
}

void func_80983704(ObjDemo* this, PlayState* play) {
    if ((this->unk_148 == 1) && (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_DOOR)) {
        CutsceneManager_Stop(CS_ID_GLOBAL_DOOR);
        CutsceneManager_Queue(this->actor.csId);
    } else {
        if (CutsceneManager_IsNext(this->actor.csId)) {
            if (this->unk_148 == 1) {
                CutsceneManager_Start(this->actor.csId, &this->actor);
                func_800E0348(play->cameraPtrs[CAM_ID_MAIN]);
            } else {
                CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
            }
            if (play->sceneId == SCENE_CASTLE) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_IKANA_CASTLE | SEQ_FLAG_ASYNC);
            }
            this->actor.csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);
            if (this->actor.csId == CS_ID_NONE) {
                if (OBJDEMO_GET_SWITCH_FLAG(&this->actor) != 0xFF) {
                    Flags_SetSwitch(play, OBJDEMO_GET_SWITCH_FLAG(&this->actor));
                }
                Actor_Kill(&this->actor);
            }
        } else {
            CutsceneManager_Queue(this->actor.csId);
            func_80983634(play);
        }
    }
}

void ObjDemo_Update(Actor* thisx, PlayState* play) {
    ObjDemo* this = (ObjDemo*)thisx;

    if ((OBJDEMO_GET_SWITCH_FLAG(&this->actor) != 0xFF) &&
        Flags_GetSwitch(play, OBJDEMO_GET_SWITCH_FLAG(&this->actor))) {
        Actor_Kill(&this->actor);
        return;
    }
    this->actionFunc(this, play);
}
