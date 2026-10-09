/*
 * File: z_bg_iknv_obj.c
 * Overlay: ovl_Bg_Iknv_Obj
 * Description: Ikana - waterwheel, stone tower door, sakon's hideout door
 */

#include "z_bg_iknv_obj.h"
#include "objects/object_iknv_obj/object_iknv_obj.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgIknvObj_Init(Actor* thisx, PlayState* play);
void BgIknvObj_Destroy(Actor* thisx, PlayState* play);
void BgIknvObj_Update(Actor* thisx, PlayState* play);
void BgIknvObj_Draw(Actor* thisx, PlayState* play);

void BgIknvObj_DoNothing(BgIknvObj* this, PlayState* play);
void BgIknvObj_UpdateWaterwheel(BgIknvObj* this, PlayState* play);
void BgIknvObj_UpdateRaisedDoor(BgIknvObj* this, PlayState* play);
void BgIknvObj_UpdateSakonDoor(BgIknvObj* this, PlayState* play);

ActorProfile Bg_Iknv_Obj_Profile = {
    /**/ ACTOR_BG_IKNV_OBJ,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_IKNV_OBJ,
    /**/ sizeof(BgIknvObj),
    /**/ BgIknvObj_Init,
    /**/ BgIknvObj_Destroy,
    /**/ BgIknvObj_Update,
    /**/ BgIknvObj_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 40, 40, 0, { 0, 0, 0 } },
};

void BgIknvObj_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgIknvObj* this = (BgIknvObj*)thisx;
    CollisionHeader* colHeader = NULL;

    Actor_SetScale(&this->dyna.actor, 0.1f);
    this->actionFunc = BgIknvObj_DoNothing;
    switch (IKNV_OBJ_TYPE(this)) {
        case IKNV_OBJ_WATERWHEEL:
            this->displayListPtr = object_iknv_obj_DL_013058;
            this->actionFunc = BgIknvObj_UpdateWaterwheel;
            this->dyna.actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
            this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            break;
        case IKNV_OBJ_RAISED_DOOR:
            this->displayListPtr = object_iknv_obj_DL_011880;
            DynaPolyActor_Init(&this->dyna, 0);
            CollisionHeader_GetVirtual(&object_iknv_obj_Colheader_0119D4, &colHeader);
            this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            this->actionFunc = BgIknvObj_UpdateRaisedDoor;
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 120.0f;
            break;
        case IKNV_OBJ_SAKON_DOOR:
            this->displayListPtr = object_iknv_obj_DL_0129C8;
            this->actionFunc = BgIknvObj_UpdateSakonDoor;
            DynaPolyActor_Init(&this->dyna, 0);
            CollisionHeader_GetVirtual(&object_iknv_obj_Colheader_012CA4, &colHeader);
            this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            Collider_InitAndSetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
            Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
            this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_51_10);
            Actor_SetFocus(&this->dyna.actor, IREG(88));
            break;
        default:
            Actor_Kill(&this->dyna.actor);
    }
}

void BgIknvObj_Destroy(Actor* thisx, PlayState* play) {
    BgIknvObj* this = (BgIknvObj*)thisx;

    if (IKNV_OBJ_TYPE(this) != IKNV_OBJ_RAISED_DOOR) {
        if (IKNV_OBJ_TYPE(this) == IKNV_OBJ_SAKON_DOOR) {
            Collider_DestroyCylinder(play, &this->collider);
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_51_10);
        } else {
            return;
        }
    }
    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

s32 func_80BD7CEC(BgIknvObj* this) {
    if (this->dyna.actor.csId == CS_ID_NONE) {
        return true;
    }

    if (CutsceneManager_GetCurrentCsId() == this->dyna.actor.csId) {
        return true;
    }

    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        return true;
    }

    CutsceneManager_Queue(this->dyna.actor.csId);
    return false;
}

void BgIknvObj_UpdateWaterwheel(BgIknvObj* this, PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)) {
        this->dyna.actor.shape.rot.z -= 0x64;
        Actor_PlaySeq_FlaggedMusicBoxHouse(&this->dyna.actor);
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_WOOD_WATER_WHEEL - SFX_FLAG);
    }

    if ((play->csCtx.state != CS_STATE_IDLE) && (gSaveContext.sceneLayer == 1) && (play->csCtx.scriptIndex == 4) &&
        (play->csCtx.curFrame == 1495)) {
        Audio_PlaySfx_2(NA_SE_EV_DOOR_UNLOCK);
    }
}

s32 func_80BD7E0C(BgIknvObj* this, s16 targetRotation, PlayState* play) {
    this->dyna.actor.shape.yOffset = 0.0f;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    if (targetRotation != this->dyna.actor.shape.rot.y) {
        Math_SmoothStepToS(&this->dyna.actor.shape.rot.y, targetRotation, 2, 0x64, 0x64);
        this->dyna.actor.world.rot.y = this->dyna.actor.shape.rot.y;
        if ((play->gameplayFrames % 2) != 0) {
            this->dyna.actor.shape.yOffset = 5.0f;
        }
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_STONEDOOR_OPEN_S - SFX_FLAG);
        return false;
    }
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_STONEDOOR_STOP);
    return true;
}

void func_80BD7ED8(BgIknvObj* this, PlayState* play) {
    if (func_80BD7E0C(this, this->dyna.actor.home.rot.y, play)) {
        this->actionFunc = BgIknvObj_UpdateSakonDoor;
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_51_10);
    }
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void func_80BD7F4C(BgIknvObj* this, PlayState* play) {
    if (CURRENT_TIME > CLOCK_TIME(19, 30)) {
        this->actionFunc = func_80BD7ED8;
    }
    if ((this->dyna.actor.home.rot.x == 1) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_58_80)) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        this->dyna.actor.home.rot.x = 0;
    }
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void func_80BD7FDC(BgIknvObj* this, PlayState* play) {
    if (func_80BD7E0C(this, this->dyna.actor.home.rot.y + 0x4000, play)) {
        this->actionFunc = func_80BD7F4C;
        SET_WEEKEVENTREG(WEEKEVENTREG_51_10);
        this->dyna.actor.home.rot.x = 1;
    }
}

void func_80BD8040(BgIknvObj* this, PlayState* play) {
    if (func_80BD7CEC(this)) {
        this->actionFunc = func_80BD7FDC;
    }
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void BgIknvObj_UpdateSakonDoor(BgIknvObj* this, PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_58_80)) {
        this->actionFunc = func_80BD8040;
        SET_WEEKEVENTREG(WEEKEVENTREG_89_80);
    }
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void BgIknvObj_UpdateRaisedDoor(BgIknvObj* this, PlayState* play) {
}

void BgIknvObj_DoNothing(BgIknvObj* this, PlayState* play) {
}

void BgIknvObj_Update(Actor* thisx, PlayState* play) {
    BgIknvObj* this = (BgIknvObj*)thisx;

    this->actionFunc(this, play);
}

void BgIknvObj_Draw(Actor* thisx, PlayState* play) {
    BgIknvObj* this = (BgIknvObj*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, this->displayListPtr);

    CLOSE_DISPS(play->state.gfxCtx);
}
