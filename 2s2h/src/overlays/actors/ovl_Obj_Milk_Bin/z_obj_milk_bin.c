/*
 * File: z_obj_milk_bin.c
 * Overlay: ovl_Obj_Milk_Bin
 * Description: Milk jar
 */

#include "z_obj_milk_bin.h"
#include "objects/object_obj_milk_bin/object_obj_milk_bin.h"

#define FLAGS 0x00000000

void ObjMilkBin_Init(Actor* thisx, PlayState* play);
void ObjMilkBin_Destroy(Actor* thisx, PlayState* play);
void ObjMilkBin_Update(Actor* thisx, PlayState* play2);
void ObjMilkBin_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Milk_Bin_Profile = {
    /**/ ACTOR_OBJ_MILK_BIN,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_OBJ_MILK_BIN,
    /**/ sizeof(ObjMilkBin),
    /**/ ObjMilkBin_Init,
    /**/ ObjMilkBin_Destroy,
    /**/ ObjMilkBin_Update,
    /**/ ObjMilkBin_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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
    { 12, 30, 0, { 0, 0, 0 } },
};

void ObjMilkBin_Init(Actor* thisx, PlayState* play) {
    ObjMilkBin* this = (ObjMilkBin*)thisx;

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider);

    this->actor.shape.yOffset = 1100.0f;
    this->disableDraw = 0;
    this->type = thisx->params;

    if ((this->type == OBJ_MILK_BIN_TYPE_2) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA)) {
        this->disableDraw |= 1;
    }
}

void ObjMilkBin_Destroy(Actor* thisx, PlayState* play) {
    ObjMilkBin* this = (ObjMilkBin*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void ObjMilkBin_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjMilkBin* this = (ObjMilkBin*)thisx;

    if (this->type == OBJ_MILK_BIN_TYPE_1) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
            if (((gSaveContext.save.day == 2) && (gSaveContext.save.isNight == 1)) || (gSaveContext.save.day >= 3)) {
                Actor_Kill(&this->actor);
                return;
            }
        }
    } else if (this->type == OBJ_MILK_BIN_TYPE_2) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA)) {
            this->disableDraw &= ~1;
        } else {
            this->disableDraw |= 1;
        }
    }

    if (!(this->disableDraw & 1)) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void ObjMilkBin_Draw(Actor* thisx, PlayState* play) {
    ObjMilkBin* this = (ObjMilkBin*)thisx;

    if (!(this->disableDraw & 1)) {
        Gfx_DrawDListOpa(play, gMilkBinMilkJarDL);
    }
}
