/*
 * File: z_obj_shutter.c
 * Overlay: ovl_Obj_Shutter
 * Description: Unused West Clock Town bank closing shutter
 */

#include "z_obj_shutter.h"
#include "objects/object_f53_obj/object_f53_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjShutter_Init(Actor* thisx, PlayState* play);
void ObjShutter_Destroy(Actor* thisx, PlayState* play);
void ObjShutter_Update(Actor* thisx, PlayState* play2);
void ObjShutter_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Shutter_Profile = {
    /**/ ACTOR_OBJ_SHUTTER,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_F53_OBJ,
    /**/ sizeof(ObjShutter),
    /**/ ObjShutter_Init,
    /**/ ObjShutter_Destroy,
    /**/ ObjShutter_Update,
    /**/ ObjShutter_Draw,
};

void ObjShutter_Init(Actor* thisx, PlayState* play) {
}

void ObjShutter_Destroy(Actor* thisx, PlayState* play) {
}

static u8 sScheduleScript[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 0, 20, 0, 0x9 - 0x6),
    /* 0x6 */ SCHEDULE_CMD_RET_VAL_L(2),
    /* 0x9 */ SCHEDULE_CMD_RET_VAL_L(1),
};

void ObjShutter_Update(Actor* thisx, PlayState* play2) {
    ObjShutter* this = (ObjShutter*)thisx;
    PlayState* play = play2;
    ScheduleOutput scheduleOutput;

    Schedule_RunScript(play, sScheduleScript, &scheduleOutput);
    if (scheduleOutput.result == 1) {
        if (this->scheduleResult != scheduleOutput.result) {
            this->actor.velocity.y = 0.0f;
        }
        if ((this->verticalOffset >= 80.0f) || (this->scheduleResult == 0)) {
            this->actor.velocity.y = 0.0f;
            this->verticalOffset = 80.0f;
        } else {
            this->verticalOffset += 10.0f;
        }
    } else {
        if (this->scheduleResult != scheduleOutput.result) {
            this->actor.velocity.y = 0.0f;
        }
        if (this->verticalOffset != 0.0f) {
            this->actor.velocity.y -= 3.0f;
            this->verticalOffset += this->actor.velocity.y;
            if (this->verticalOffset <= 0.0f) {
                this->actor.velocity.y = fabsf(this->actor.velocity.y) * 0.8f;
                if ((s32)this->actor.velocity.y == 0) {
                    this->actor.velocity.y = 0.0f;
                    this->verticalOffset = 0.0f;
                }
            }
        }
    }
    this->scheduleResult = scheduleOutput.result;
}

void ObjShutter_Draw(Actor* thisx, PlayState* play) {
    ObjShutter* this = (ObjShutter*)thisx;

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y + this->verticalOffset, this->actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_APPLY);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gBankShutterDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
