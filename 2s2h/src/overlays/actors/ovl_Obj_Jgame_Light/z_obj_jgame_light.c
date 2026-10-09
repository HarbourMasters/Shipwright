/*
 * File: z_obj_jgame_light.c
 * Overlay: ovl_Obj_Jgame_Light
 * Description: Fisherman's Jumping Game - Torch
 */

#include "z_obj_jgame_light.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_syokudai/object_syokudai.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

typedef enum {
    /* 0 */ OBJJGAMELIGHT_NONE,
    /* 1 */ OBJJGAMELIGHT_CORRECT,
    /* 2 */ OBJJGAMELIGHT_INCORRECT
} ObjJgameLightSignal;

void ObjJgameLight_Init(Actor* thisx, PlayState* play);
void ObjJgameLight_Destroy(Actor* thisx, PlayState* play);
void ObjJgameLight_Update(Actor* thisx, PlayState* play);
void ObjJgameLight_Draw(Actor* thisx, PlayState* play);

void func_80C15474(ObjJgameLight* this, PlayState* play);
void ObjJgameLight_UpdateCollision(ObjJgameLight* this, PlayState* play);
void func_80C15718(ObjJgameLight* this, PlayState* play);

ActorProfile Obj_Jgame_Light_Profile = {
    /**/ ACTOR_OBJ_JGAME_LIGHT,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_SYOKUDAI,
    /**/ sizeof(ObjJgameLight),
    /**/ ObjJgameLight_Init,
    /**/ ObjJgameLight_Destroy,
    /**/ ObjJgameLight_Update,
    /**/ ObjJgameLight_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00100000, 0x00, 0x00 },
        { 0xF6CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 12, 45, 0, { 0, 0, 0 } },
};

#include "assets/overlays/ovl_Obj_Jgame_Light/ovl_Obj_Jgame_Light.h"

void ObjJgameLight_Init(Actor* thisx, PlayState* play) {
    ObjJgameLight* this = (ObjJgameLight*)thisx;
    LightInfo* lights = &this->lightInfo;

    Actor_SetScale(&this->actor, 1.0f);
    func_800B4AEC(play, &this->actor, 50.0f);
    ActorShape_Init(&this->actor.shape, 0.0f, func_800B4B50, 1.0f);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Lights_PointGlowSetInfo(lights, this->actor.world.pos.x, (this->actor.world.pos.y + 70.0f), this->actor.world.pos.z,
                            255, 255, 180, -1);
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    Actor_SetFocus(&this->actor, 60.0f);
    this->actor.colChkInfo.health = 0;
    this->prevHealth = 0;
    this->isOn = false;
    this->lightRadius = 0;
    this->alpha = 0;
    this->signal = OBJJGAMELIGHT_NONE;
    this->flameScaleProportion = 0.0f;
}

void ObjJgameLight_Destroy(Actor* thisx, PlayState* play) {
    ObjJgameLight* this = (ObjJgameLight*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
}

void func_80C15474(ObjJgameLight* this, PlayState* play) {
    u8 temp_a1;

    if ((this->actor.colChkInfo.health & OBJLUPYGAMELIFT_IGNITE_FIRE) && !this->isOn) {
        if (this->lightRadius < 160) {
            this->lightRadius += 40;
        } else {
            this->lightRadius = 200;
            this->isOn = true;
        }
        if (this->flameScaleProportion < 0.7f) {
            this->flameScaleProportion += 0.3f;
        } else {
            this->flameScaleProportion = 1.0f;
        }
    } else if (this->actor.colChkInfo.health & OBJLUPYGAMELIFT_SNUFF_FIRE) {
        if (this->lightRadius > 40) {
            this->lightRadius -= 40;
        } else {
            this->lightRadius = -1;
            if (this->flameScaleProportion == 0.0f) {
                this->isOn = false;
                this->actor.colChkInfo.health &= ~OBJLUPYGAMELIFT_IGNITE_FIRE;
                this->actor.colChkInfo.health &= ~OBJLUPYGAMELIFT_SNUFF_FIRE;
            }
        }
        if (this->flameScaleProportion > 0.3f) {
            this->flameScaleProportion -= 0.3f;
        } else {
            this->flameScaleProportion = 0.0f;
        }
    }
    if (this->flameScaleProportion > 0.1f) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_TORCH - SFX_FLAG);
    }
    temp_a1 = (s32)(Rand_ZeroOne() * 127.0f) + 128;
    Lights_PointSetColorAndRadius(&this->lightInfo, temp_a1, temp_a1 * 0.7f, 0, this->lightRadius);
}

void ObjJgameLight_UpdateCollision(ObjJgameLight* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void func_80C15718(ObjJgameLight* this, PlayState* play) {
    if ((this->actor.colChkInfo.health & OBJLUPYGAMELIFT_IGNITE_FIRE) &&
        !(this->prevHealth & OBJLUPYGAMELIFT_IGNITE_FIRE)) {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_FLAME_IGNITION);
        this->prevHealth = this->actor.colChkInfo.health;
    }
    if (this->actor.colChkInfo.health & OBJLUPYGAMELIFT_DISPLAY_CORRECT) {
        this->actor.colChkInfo.health &= ~OBJLUPYGAMELIFT_DISPLAY_CORRECT;
        this->alpha = 300;
        this->signal = OBJJGAMELIGHT_CORRECT;
    } else if (this->actor.colChkInfo.health & OBJLUPYGAMELIFT_DISPLAY_INCORRECT) {
        this->actor.colChkInfo.health &= ~OBJLUPYGAMELIFT_DISPLAY_INCORRECT;
        this->alpha = 300;
        this->signal = OBJJGAMELIGHT_INCORRECT;
    }
    if (this->alpha > 15) {
        this->alpha -= 15;
    } else {
        this->alpha = 0;
    }
}

void ObjJgameLight_Update(Actor* thisx, PlayState* play) {
    ObjJgameLight* this = (ObjJgameLight*)thisx;

    func_80C15718(this, play);
    func_80C15474(this, play);
    ObjJgameLight_UpdateCollision(this, play);
    this->flameScroll++;
}

void ObjJgameLight_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjJgameLight* this = (ObjJgameLight*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, &gObjectSyokudaiTypeSwitchCausesFlameDL);
    if (this->alpha > 0) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        if (this->alpha > 255) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 210, 64, 32, 255);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 210, 64, 32, this->alpha);
        }
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        if (this->signal == OBJJGAMELIGHT_CORRECT) {
            gSPDisplayList(POLY_XLU_DISP++, gObjJgameLightCorrectDL);
        } else if (this->signal == OBJJGAMELIGHT_INCORRECT) {
            gSPDisplayList(POLY_XLU_DISP++, gObjJgameLightIncorrectDL);
        }
    }
    if (this->flameScaleProportion != 0.0f) {
        f32 scale;

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        scale = (this->flameScaleProportion * 27.0f) / 10000.0f;
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, (this->flameScroll * -20) & 0x1FF,
                                      0x20, 0x80, 0, 0, 0, -20));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
        Matrix_Translate(0.0f, 52.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateYS(((Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y) + 0x8000),
                        MTXMODE_APPLY);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}
