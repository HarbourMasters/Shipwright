/*
 * File: z_obj_y2shutter.c
 * Overlay: ovl_Obj_Y2shutter
 * Description: Pirates' Fortress sliding grated/barred shutters
 */

#include "z_obj_y2shutter.h"
#include "objects/object_kaizoku_obj/object_kaizoku_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjY2shutter_Init(Actor* thisx, PlayState* play);
void ObjY2shutter_Destroy(Actor* thisx, PlayState* play);
void ObjY2shutter_Update(Actor* thisx, PlayState* play);
void ObjY2shutter_Draw(Actor* thisx, PlayState* play);

typedef struct ShutterInfo {
    /* 0x00 */ Gfx* dList;
    /* 0x04 */ CollisionHeader* colHeader;
    /* 0x08 */ f32 openedOffsetY;
    /* 0x0C */ f32 openVelocity;
    /* 0x10 */ f32 openAccel;
    /* 0x14 */ f32 closeVelocity;
    /* 0x18 */ f32 closeAccel;
    /* 0x1C */ u8 openStartSettleTimer;
    /* 0x1D */ u8 openEndAndCloseSettleTimer;
    /* 0x1E */ u8 openTimer;
} ShutterInfo; // size = 0x20

ActorProfile Obj_Y2shutter_Profile = {
    /**/ ACTOR_OBJ_Y2SHUTTER,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_KAIZOKU_OBJ,
    /**/ sizeof(ObjY2shutter),
    /**/ ObjY2shutter_Init,
    /**/ ObjY2shutter_Destroy,
    /**/ ObjY2shutter_Update,
    /**/ ObjY2shutter_Draw,
};

ShutterInfo sShutterInfo[] = {
    { gPirateBarredShutterDL, &gPirateBarredShutterCol, 120.0f, 20.0f, 3.0f, -20.0f, 3.0f, 4, 8, 160 },
    { gPirateGratedShutterDL, &gPirateGratedShutterCol, 150.0f, 1.0f, 0.04f, -1.0f, 0.04f, 6, 12, 160 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_STOP),
};

void ObjY2shutter_Init(Actor* thisx, PlayState* play) {
    s32 pad[2];
    ShutterInfo* info = &sShutterInfo[OBJY2SHUTTER_GET_TYPE(thisx)];
    ObjY2shutter* this = (ObjY2shutter*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, info->colHeader);
}

void ObjY2shutter_Destroy(Actor* thisx, PlayState* play) {
    ObjY2shutter* this = (ObjY2shutter*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjY2shutter_SetupOpen(ObjY2shutter* this, ShutterInfo* info, ShutterType shutterType) {
    this->openTimer = info->openTimer;
    this->settleTimer = info->openStartSettleTimer;
    if (shutterType == SHUTTER_BARRED) {
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_METALDOOR_OPEN);
    }
}

void ObjY2shutter_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjY2shutter* this = (ObjY2shutter*)thisx;
    ShutterType shutterType = OBJY2SHUTTER_GET_TYPE(&this->dyna.actor);
    ShutterInfo* info = &sShutterInfo[shutterType];
    f32 targetPosY = this->dyna.actor.world.pos.y;
    f32 targetVelocityY = 0.0f;
    f32 accelY = 0.0f;

    sShutterInfo[0].openTimer = DREG(84) + 160;
    sShutterInfo[1].openTimer = DREG(85) + 160;

    if (((shutterType == SHUTTER_BARRED) && (DREG(86) != 0)) || ((shutterType != SHUTTER_BARRED) && (DREG(87) != 0))) {
        if (shutterType == SHUTTER_BARRED) {
            DREG(86) = 0;
        } else {
            DREG(87) = 0;
        }

        if (Flags_GetSwitch(play, OBJY2SHUTTER_GET_SWITCH_FLAG(&this->dyna.actor))) {
            Flags_UnsetSwitch(play, OBJY2SHUTTER_GET_SWITCH_FLAG(&this->dyna.actor));
        } else {
            Flags_SetSwitch(play, OBJY2SHUTTER_GET_SWITCH_FLAG(&this->dyna.actor));
        }
    }

    if (this->settleTimer == 0) {
        if (Flags_GetSwitch(play, OBJY2SHUTTER_GET_SWITCH_FLAG(&this->dyna.actor))) {
            s16 csId = this->dyna.actor.csId;

            if (this->openTimer == 0) {
                if ((csId > CS_ID_NONE) && !CutsceneManager_IsNext(csId)) {
                    CutsceneManager_Queue(csId);
                } else if (csId > CS_ID_NONE) {
                    CutsceneManager_StartWithPlayerCs(csId, &this->dyna.actor);
                    this->openTimer = -1;
                } else {
                    ObjY2shutter_SetupOpen(this, info, shutterType);
                }
            } else if (this->openTimer < 0) {
                if (func_800F22C4(csId, &this->dyna.actor) != 0) {
                    ObjY2shutter_SetupOpen(this, info, shutterType);
                }
            } else {
                targetPosY = this->dyna.actor.home.pos.y + info->openedOffsetY;
                targetVelocityY = info->openVelocity;
                accelY = info->openAccel;
                if (this->openTimer < 2) {
                    Flags_UnsetSwitch(play, OBJY2SHUTTER_GET_SWITCH_FLAG(&this->dyna.actor));
                } else {
                    this->openTimer--;
                }
            }
        } else if (this->openTimer != 0) {
            this->openTimer = 0;
            this->settleTimer = info->openStartSettleTimer;
            if (shutterType == SHUTTER_BARRED) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_METALDOOR_CLOSE);
            }
        } else {
            targetPosY = this->dyna.actor.home.pos.y;
            targetVelocityY = info->closeVelocity;
            accelY = info->closeAccel;
        }
    }

    Math_StepToF(&this->dyna.actor.velocity.y, targetVelocityY, accelY);
    this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;

    if (((this->dyna.actor.world.pos.y - targetPosY) * targetVelocityY) >= 0.0f) {
        this->dyna.actor.world.pos.y = targetPosY;
        this->dyna.actor.velocity.y = 0.0f;
        if (!this->isStationary) {
            this->isStationary = true;
            this->settleTimer = info->openEndAndCloseSettleTimer;
            if (shutterType != SHUTTER_BARRED) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_METALDOOR_STOP);
            }
        }
    } else {
        this->isStationary = false;
        if (shutterType != SHUTTER_BARRED) {
            Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
        }
    }

    if (DECR(this->settleTimer)) {
        this->dyna.actor.shape.yOffset = 2.0f * (this->settleTimer & 1) * this->settleTimer;
    }
}

void ObjY2shutter_Draw(Actor* thisx, PlayState* play) {
    ObjY2shutter* this = (ObjY2shutter*)thisx;
    ShutterInfo* info = &sShutterInfo[(OBJY2SHUTTER_GET_TYPE(&this->dyna.actor))];

    Gfx_DrawDListOpa(play, info->dList);
}
