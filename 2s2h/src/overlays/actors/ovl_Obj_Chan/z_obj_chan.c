/*
 * File: z_obj_chan.c
 * Overlay: ovl_Obj_Chan
 * Description: Goron Shrine chandelier
 *
 * This actor class is used for both the chandelier and its sub-components.
 * The distinction is made using the subtype actor parameter:
 *  -  0 for the chandelier itself
 *  -  1 for one of the 5 breakable pots that are attached to the chandelier
 */

#include "z_obj_chan.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_obj_chan/object_obj_chan.h"
#include "objects/object_tsubo/object_tsubo.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

#define OBJCHAN_ROTATION_SPEED 364 // == (65536 * 2/360) i.e. 2 degrees per second

void ObjChan_Init(Actor* thisx, PlayState* play);
void ObjChan_Destroy(Actor* thisx, PlayState* play);
void ObjChan_Update(Actor* thisx, PlayState* play);
void ObjChan_Draw(Actor* thisx, PlayState* play);
void ObjChan_Reset(void);

void ObjChan_ChandelierAction(ObjChan* this, PlayState* play);
void ObjChan_PotAction(ObjChan* this, PlayState* play);

ActorProfile Obj_Chan_Profile = {
    /**/ ACTOR_OBJ_CHAN,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_OBJ_CHAN,
    /**/ sizeof(ObjChan),
    /**/ ObjChan_Init,
    /**/ ObjChan_Destroy,
    /**/ ObjChan_Update,
    /**/ ObjChan_Draw,
    /**/ ObjChan_Reset,
};

static ColliderCylinderInit sObjChanCylinderInit = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 48, 76, -60, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void ObjChan_InitChandelier(ObjChan* this, PlayState* play);
void ObjChan_InitPot(ObjChan* this, PlayState* play);
void ObjChan_CreateSmashEffects(ObjChan* this, PlayState* play);
void ObjChan_DrawPot(Actor* thisx, PlayState* play);
void ObjChan_DrawFire(ObjChan* this, PlayState* play);

static Vec3f sObjChanFlameSize[2] = {
    { 0.16f, 0.11f, 1.0f },
    { 0.13f, 0.09f, 1.0f },
};
static f32 sObjChanFlameYOffset[2] = { 1800, 1800 };
static s32 sObjChanLoaded;

void ObjChan_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjChan* this = (ObjChan*)thisx;

    if (OBJCHAN_SUBTYPE(&this->actor) == OBJCHAN_SUBTYPE_CHANDELIER) {
        if (sObjChanLoaded) {
            Actor_Kill(&this->actor);
            return;
        }
        this->actor.room = -1;
        sObjChanLoaded = true;
    }
    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sObjChanCylinderInit);
    SubS_FillCutscenesList(&this->actor, this->csIdList, ARRAY_COUNT(this->csIdList));
    switch (OBJCHAN_SUBTYPE(&this->actor)) {
        case OBJCHAN_SUBTYPE_CHANDELIER:
            this->rotation = this->actor.shape.rot.y;
            this->actor.shape.rot.y = 0;
            ObjChan_InitChandelier(this, play);
            break;

        case OBJCHAN_SUBTYPE_POT:
            this->actor.draw = ObjChan_DrawPot;
            ObjChan_InitPot(this, play);
            break;

        default:
            break;
    }
}

void ObjChan_Destroy(Actor* thisx, PlayState* play) {
    ObjChan* this = (ObjChan*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

u32 func_80BB9A1C(ObjChan* this, f32 arg1) {
    f32 temp_f6;
    f32 sp20;

    sp20 = Math_SinS(this->unk1D4) * this->unk1D0;
    temp_f6 = (Math_CosS(this->unk1D4) * (400 * M_PIf / 0x8000) * this->unk1D0) + arg1;
    if (temp_f6 != 0.0f) {
        this->unk1D4 = RAD_TO_BINANG(Math_FAtan2F(sp20 * (400 * M_PIf / 0x8000), temp_f6));
    } else if (sp20 >= 0.0f) {
        this->unk1D4 = 0x4000;
    } else {
        this->unk1D4 = -0x4000;
    }
    if (Math_CosS(this->unk1D4) != 0.0f) {
        this->unk1D0 = (temp_f6 / (Math_CosS(this->unk1D4) * (400 * M_PIf / 0x8000)));
    } else {
        this->unk1D0 = sp20;
    }
    return 0;
}

void ObjChan_CalculatePotPosition(Vec3f* childPosOut, Vec3s* childRotOut, Vec3f* parentPos, Vec3s* parentRot,
                                  s16 childAngle) {
    Vec3f offset;

    Matrix_RotateYS(parentRot->y, MTXMODE_NEW);
    Matrix_RotateXS(parentRot->x, MTXMODE_APPLY);
    Matrix_RotateZS(parentRot->z, MTXMODE_APPLY);
    Matrix_RotateYS(childAngle, MTXMODE_APPLY);
    Matrix_MultVecX(-280.0f, &offset);

    childPosOut->x = parentPos->x + offset.x;
    childPosOut->y = parentPos->y + offset.y;
    childPosOut->z = parentPos->z + offset.z;
    Math_Vec3s_Copy(childRotOut, parentRot);
    childRotOut->y += childAngle;
}

void ObjChan_InitChandelier(ObjChan* this, PlayState* play) {
    Actor* thisx = &this->actor;
    s32 i;
    ObjChan* pot;
    Vec3f childPos;
    Vec3s childRot;
    CollisionPoly* sp94;
    s32 bgId;
    Vec3f sp84;

    Math_Vec3f_Copy(&this->unk1C0, &thisx->world.pos);

    Math_Vec3f_Copy(&sp84, &thisx->world.pos);
    sp84.y += 1600.0f;

    if (BgCheck_EntityLineTest1(&play->colCtx, &thisx->world.pos, &sp84, &this->unk1C0, &sp94, false, false, true, true,
                                &bgId)) {
        this->unk1CC = thisx->world.pos.y - this->unk1C0.y;
    } else {
        Actor_Kill(thisx);
        return;
    }

    for (i = 0; i < 5; i++) {
        ObjChan_CalculatePotPosition(&childPos, &childRot, &thisx->world.pos, &thisx->shape.rot,
                                     (s32)DEG_TO_BINANG_ALT3(i * 360.0f / 5.0f) + this->rotation);
        pot = (ObjChan*)Actor_SpawnAsChildAndCutscene(
            &play->actorCtx, play, ACTOR_OBJ_CHAN, childPos.x, childPos.y, childPos.z, childRot.x, childRot.y,
            childRot.z, (thisx->params & 0xFFF) | 0x1000, thisx->csId, thisx->halfDaysBits, thisx);
        if (pot != NULL) {
            this->pots[i] = pot;
            pot->myPotIndex = i;
            pot->actor.csId = thisx->csId;
        } else {
            Actor_Kill(thisx);
        }
    }

    if (Flags_GetSwitch(play, OBJCHAN_GET_SWITCH_FLAG(thisx))) {
        this->stateFlags |= OBJCHAN_STATE_FIRE_DELAY;
        this->stateFlags |= OBJCHAN_STATE_ON_FIRE;

        this->rotationSpeed = OBJCHAN_ROTATION_SPEED;
        this->flameSize = 1.0f;

        for (i = 0; i < 5; i++) {
            if (this->pots[i] != NULL) {
                this->pots[i]->stateFlags |= OBJCHAN_STATE_FIRE_DELAY;
                this->pots[i]->stateFlags |= OBJCHAN_STATE_ON_FIRE;
                this->pots[i]->flameSize = 1.0f;
            }
        }
    }
    this->collider.dim.radius = 45;
    this->collider.dim.height = 60;
    this->collider.dim.yShift = -20;
    this->actionFunc = ObjChan_ChandelierAction;
}

//! TODO: More descriptive name than Action?
void ObjChan_ChandelierAction(ObjChan* this, PlayState* play) {
    Actor* thisx = &this->actor;
    ObjChan* pot;
    s32 i;
    Vec3f sp60;
    Vec3s sp58;

    if (this->unk1D0 > 0.0f) {
        this->unk1D4 += 0x190;
        if (this->unk1D0 <= 400.0f) {
            this->unk1D0 *= 0.99f;
        } else {
            this->unk1D0 -= 1.0f;
        }
        if (this->unk1D0 < 0.0f) {
            this->unk1D4 = 0;
            this->unk1D0 = 0.0f;
        }
    }
    thisx->shape.rot.z = (Math_SinS(this->unk1D4) * this->unk1D0);
    if ((this->stateFlags & OBJCHAN_STATE_START_CUTSCENE) &&
        SubS_StartCutscene(&this->actor, this->csIdList[0], CS_ID_NONE, SUBS_CUTSCENE_WITH_PLAYER)) {
        this->stateFlags |= OBJCHAN_STATE_CUTSCENE;
        this->stateFlags &= ~OBJCHAN_STATE_START_CUTSCENE;
    }
    if ((this->stateFlags & OBJCHAN_STATE_CUTSCENE) && this->rotationSpeed == OBJCHAN_ROTATION_SPEED) {
        this->stateFlags &= ~OBJCHAN_STATE_CUTSCENE;
        CutsceneManager_Stop(this->csIdList[0]);
    }
    Matrix_RotateYS(thisx->shape.rot.y, MTXMODE_NEW);
    Matrix_RotateXS(thisx->shape.rot.x, MTXMODE_APPLY);
    Matrix_RotateZS(thisx->shape.rot.z, MTXMODE_APPLY);
    Matrix_RotateYS(this->rotation, MTXMODE_APPLY);
    Matrix_MultVecY(this->unk1CC, &thisx->world.pos);
    Math_Vec3f_Sum(&thisx->world.pos, &this->unk1C0, &thisx->world.pos);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    for (i = 0; i < 5; i++) {
        pot = this->pots[i];
        if (pot != NULL) {
            ObjChan_CalculatePotPosition(&sp60, &sp58, &thisx->world.pos, &thisx->shape.rot,
                                         (s32)DEG_TO_BINANG_ALT3(i * 360.0f / 5.0f) + this->rotation);
            Math_Vec3f_Copy(&pot->actor.world.pos, &sp60);
            Math_Vec3s_Copy(&pot->actor.shape.rot, &thisx->shape.rot);
            pot->actor.shape.rot.y = this->rotation;
            Math_Vec3f_ToVec3s(&pot->collider.dim.pos, &pot->actor.world.pos);
        }
    }
    if ((this->collider.base.acFlags & AC_HIT) && (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x800)) {
        Flags_SetSwitch(play, OBJCHAN_GET_SWITCH_FLAG(thisx));
    }
    if (Flags_GetSwitch(play, OBJCHAN_GET_SWITCH_FLAG(thisx))) {
        if (!(this->stateFlags & OBJCHAN_STATE_FIRE_DELAY)) {
            this->rotationSpeed = 0;
            this->flameSize = 0.0f;
            for (i = 0; i < 5; i++) {
                pot = this->pots[i];
                if (pot != NULL) {
                    this->pots[i]->stateFlags |= OBJCHAN_STATE_FIRE_DELAY;
                    this->pots[i]->fireDelayFrames = 20 + i * 5;
                    this->pots[i]->flameSize = 0.0f;
                }
            }
            this->stateFlags |= OBJCHAN_STATE_FIRE_DELAY;
            this->stateFlags |= OBJCHAN_STATE_ON_FIRE;
            this->stateFlags |= OBJCHAN_STATE_START_CUTSCENE;
        } else {
            Math_StepToF(&this->flameSize, 1.0f, 0.05f);
            this->rotation += this->rotationSpeed;
            Math_StepToS(&this->rotationSpeed, OBJCHAN_ROTATION_SPEED, 5);
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_CHANDELIER_ROLL - SFX_FLAG);
        }
    }
}

void ObjChan_InitPot(ObjChan* this, PlayState* play) {
    this->actionFunc = ObjChan_PotAction;
}

void ObjChan_PotAction(ObjChan* this, PlayState* play) {
    s32 potBreaks;
    s32 phi_v1;

    potBreaks = false;
    if ((this->collider.base.acFlags & AC_HIT) && (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x4004000)) {
        potBreaks = true;
    }
    if (this->stateFlags & OBJCHAN_STATE_ON_FIRE) {
        Math_StepToF(&this->flameSize, 1.0f, 0.05f);
    } else if (this->stateFlags & OBJCHAN_STATE_FIRE_DELAY) {
        this->fireDelayFrames--;
        if (this->fireDelayFrames <= 0) {
            this->stateFlags |= OBJCHAN_STATE_ON_FIRE;
        }
    }
    if (potBreaks) {
        ObjChan_CreateSmashEffects(this, play);
        ((ObjChan*)this->actor.parent)->pots[this->myPotIndex] = NULL;
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EV_CHANDELIER_BROKEN);
        func_80BB9A1C((ObjChan*)this->actor.parent, 40.0f);
        if (GameInteractor_Should(VB_FIND_ROCK_SIRLOIN, this->myPotIndex == 4)) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_37_10)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_37_10);
                Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_EN_MM, this->actor.world.pos.x,
                                              this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0x8000,
                                              this->actor.csId, this->actor.halfDaysBits, NULL);
            }
        }
        Actor_Kill(&this->actor);
    }
}

void ObjChan_CreateSmashEffects(ObjChan* this, PlayState* play) {
    s16 new_var = 0;
    s32 phi_s0;
    Vec3f spDC;
    Vec3f spD0;
    f32 temp_f0;
    f32 new_var2;
    s16 temp_s1 = 0;
    s32 temp_s2;
    f32 spAC;
    f32 spA8 = new_var + 15.0f;
    f32 spA4 = new_var + (spAC = 110.0f);

    for (temp_s2 = 0, temp_s1 = 0; temp_s2 != 18; temp_s2++, temp_s1 += 0x4E20) {
        f32 sin = Math_SinS(temp_s1);
        f32 cos = Math_CosS(temp_s1);

        spDC.x = sin * 8.0f;
        spDC.y = Rand_ZeroOne() * 12.0f + 2.0f;
        spDC.z = cos * 8.0f;
        spD0.x = spDC.x * 0.23f;
        spD0.y = Rand_ZeroOne() * 5.0f + 2.5f;
        spD0.z = spDC.z * 0.23f;
        temp_f0 = Rand_ZeroOne();
        if (temp_f0 < 0.2f) {
            phi_s0 = 0x60;
        } else if (temp_f0 < 0.6f) {
            phi_s0 = 0x40;
        } else {
            phi_s0 = 0x20;
        }
        new_var2 = spA4 * Rand_ZeroOne();
        EffectSsKakera_Spawn(play, &spDC, &spD0, &this->actor.world.pos, -260, phi_s0, 20, 0, 0, spA8 + new_var2, 0, 0,
                             50, -1, OBJECT_TSUBO, gPotShardDL);
    }
    func_800BBFB0(play, &this->actor.world.pos, 30.0f, 2, 20, 50, true);
    func_800BBFB0(play, &this->actor.world.pos, 30.0f, 2, 10, 80, true);
}

void ObjChan_Update(Actor* thisx, PlayState* play) {
    ObjChan* this = (ObjChan*)thisx;

    this->actionFunc(this, play);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void ObjChan_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjChan* this = (ObjChan*)thisx;
    Gfx* opa;
    Gfx* xlu;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_RotateYS(this->rotation, MTXMODE_APPLY);

    opa = Gfx_SetupDL(POLY_OPA_DISP, SETUPDL_25);
    MATRIX_FINALIZE_AND_LOAD(&opa[0], play->state.gfxCtx);
    gSPDisplayList(&opa[1], gChandelierCenterDL);
    POLY_OPA_DISP = &opa[2];

    xlu = Gfx_SetupDL71(POLY_XLU_DISP);
    MATRIX_FINALIZE_AND_LOAD(&xlu[0], play->state.gfxCtx);
    gSPDisplayList(&xlu[1], gChandelierPotHolderDL);
    POLY_XLU_DISP = &xlu[2];

    CLOSE_DISPS(play->state.gfxCtx);

    Matrix_Push();
    if (this->stateFlags & OBJCHAN_STATE_ON_FIRE) {
        ObjChan_DrawFire(this, play);
    }
    Matrix_Pop();
}

void ObjChan_DrawPot(Actor* thisx, PlayState* play) {
    ObjChan* this = (ObjChan*)thisx;
    s32 pad;
    Gfx* dl;

    OPEN_DISPS(play->state.gfxCtx);

    dl = Gfx_SetupDL(POLY_OPA_DISP, SETUPDL_25);
    MATRIX_FINALIZE_AND_LOAD(&dl[0], play->state.gfxCtx);
    gSPDisplayList(&dl[1], gChandelierPotDL);
    POLY_OPA_DISP = &dl[2];

    CLOSE_DISPS(play->state.gfxCtx);

    if (this->stateFlags & OBJCHAN_STATE_ON_FIRE) {
        ObjChan_DrawFire(this, play);
    }
}

void ObjChan_DrawFire(ObjChan* this, PlayState* play) {
    u32 sp4C;
    Gfx* dl;

    OPEN_DISPS(play->state.gfxCtx);

    sp4C = play->gameplayFrames;

    Matrix_RotateYS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y - this->rotation + 0x8000,
                    MTXMODE_APPLY);
    Matrix_Scale(sObjChanFlameSize[OBJCHAN_SUBTYPE(&this->actor)].x * this->flameSize,
                 sObjChanFlameSize[OBJCHAN_SUBTYPE(&this->actor)].y * this->flameSize, 1.0f, MTXMODE_APPLY);
    Matrix_Translate(0.0f, sObjChanFlameYOffset[OBJCHAN_SUBTYPE(&this->actor)], 0.0f, MTXMODE_APPLY);

    dl = Gfx_SetupDL71(POLY_XLU_DISP);
    MATRIX_FINALIZE_AND_LOAD(&dl[0], play->state.gfxCtx);
    gSPSegment(&dl[1], 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0, -sp4C * 20, 32, 128, 0, 0, 0, -20));
    gDPSetPrimColor(&dl[2], 128, 128, 255, 255, 0, 255);
    gDPSetEnvColor(&dl[3], 255, 0, 0, 0);
    gSPDisplayList(&dl[4], gEffFire1DL);
    POLY_XLU_DISP = &dl[5];

    CLOSE_DISPS(play->state.gfxCtx);
}

void ObjChan_Reset(void) {
    sObjChanLoaded = false;
}
