/*
 * File: z_obj_purify.c
 * Overlay: ovl_Obj_Purify
 * Description: Poisoned/Purified Water Elements
 */

#include "z_obj_purify.h"
#include "objects/object_numa_obj/object_numa_obj.h"
#include "objects/object_dekucity_obj/object_dekucity_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjPurify_Init(Actor* thisx, PlayState* play);
void ObjPurify_Destroy(Actor* thisx, PlayState* play);
void ObjPurify_Update(Actor* thisx, PlayState* play);

void func_80A84EC0(ObjPurify* this, PlayState* play);
void func_80A851C8(Actor* thisx, PlayState* play);
void func_80A85074(ObjPurify* this, PlayState* play);
void func_80A850B0(ObjPurify* this);
void func_80A850E8(ObjPurify* this, PlayState* play);
void func_80A85304(Actor* thisx, PlayState* play);
void func_80A84FB4(ObjPurify* this, PlayState* play);
void func_80A85010(ObjPurify* this, PlayState* play);
void ObjPurify_DoNothing(ObjPurify* this, PlayState* play);
void func_80A85048(ObjPurify* this);
void func_80A84FA0(ObjPurify* this);
void func_80A8515C(ObjPurify* this);
void func_80A84FEC(ObjPurify* this);
void func_80A84EAC(ObjPurify* this);

typedef struct ObjPurifyInfo {
    /* 0x00 */ s16 objectId;
    /* 0x04 */ f32 scale;
    /* 0x08 */ Gfx* opaDLists[2];
    /* 0x10 */ Gfx* xluDLists[2];
    /* 0x18 */ AnimatedMaterial* animMat[2];
    /* 0x20 */ CollisionHeader* colHeader;
    /* 0x24 */ s32 isDekuCity;
} ObjPurifyInfo; // size = 0x28

ActorProfile Obj_Purify_Profile = {
    /**/ ACTOR_OBJ_PURIFY,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjPurify),
    /**/ ObjPurify_Init,
    /**/ ObjPurify_Destroy,
    /**/ ObjPurify_Update,
    /**/ NULL,
};

ObjPurifyInfo sObjPurifyInfo[] = {
    {
        OBJECT_NUMA_OBJ,
        0.1f,
        { NULL, NULL },
        { object_numa_obj_DL_0128E0, object_numa_obj_DL_012D90 },
        { object_numa_obj_Matanimheader_012CF8, object_numa_obj_Matanimheader_0131A8 },
        &object_numa_obj_Colheader_012818,
        false,
    },
    {
        OBJECT_DEKUCITY_OBJ,
        1.0f,
        { NULL, NULL },
        { object_dekucity_obj_DL_0004E0, object_dekucity_obj_DL_001030 },
        { object_dekucity_obj_Matanimheader_000E28, object_dekucity_obj_Matanimheader_001978 },
        &object_dekucity_obj_Colheader_0002AC,
        true,
    },
    {
        OBJECT_DEKUCITY_OBJ,
        1.0f,
        { object_dekucity_obj_DL_001ED8, object_dekucity_obj_DL_003998 },
        { object_dekucity_obj_DL_001D80, NULL },
        { object_dekucity_obj_Matanimheader_003820, NULL },
        NULL,
        true,
    },
    {
        OBJECT_NUMA_OBJ,
        0.1f,
        { NULL, NULL },
        { object_numa_obj_DL_012A60, object_numa_obj_DL_012F10 },
        { object_numa_obj_Matanimheader_012B58, object_numa_obj_Matanimheader_013008 },
        &object_numa_obj_Colheader_012818,
        false,
    },
    {
        OBJECT_NUMA_OBJ,
        0.1f,
        { NULL, NULL },
        { object_numa_obj_DL_012BF0, object_numa_obj_DL_0130A0 },
        { object_numa_obj_Matanimheader_012CE8, object_numa_obj_Matanimheader_013198 },
        &object_numa_obj_Colheader_012818,
        false,
    },
};

void ObjPurify_SetSysMatrix(f32 yPos) {
    Matrix_GetCurrent()->yw = yPos;
}

s32 ObjPurify_IsPurified(ObjPurify* this) {
    ObjPurifyInfo* info = &sObjPurifyInfo[OBJPURIFY_GET_INFO_INDEX(&this->dyna.actor)];

    if (!info->isDekuCity) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_12_01)) {
            return true;
        }
    } else {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) {
            return true;
        }
    }
    return false;
}

void ObjPurify_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjPurify* this = (ObjPurify*)thisx;
    ObjPurifyInfo* info = &sObjPurifyInfo[OBJPURIFY_GET_INFO_INDEX(&this->dyna.actor)];
    s32 sp20 = OBJPURIFY_GET_UNK_FLAG(thisx);

    Actor_SetScale(&this->dyna.actor, info->scale);
    if (sp20 == 1) {
        DynaPolyActor_Init(&this->dyna, 0);
    }
    this->objectSlot = Object_GetSlot(&play->objectCtx, info->objectId);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->dyna.actor);
    } else if (sp20 == 0) {
        func_80A84EAC(this);
    } else if (ObjPurify_IsPurified(this)) {
        Actor_Kill(&this->dyna.actor);
    } else {
        func_80A84EAC(this);
    }
}

void ObjPurify_Destroy(Actor* thisx, PlayState* play) {
    ObjPurify* this = (ObjPurify*)thisx;
    if (OBJPURIFY_GET_UNK_FLAG(&this->dyna.actor) == 1) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void func_80A84EAC(ObjPurify* this) {
    this->actionFunc = func_80A84EC0;
}

void func_80A84EC0(ObjPurify* this, PlayState* play) {
    s32 pad;
    s32 sp28;
    s32 index;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        sp28 = OBJPURIFY_GET_UNK_FLAG(&this->dyna.actor);
        index = OBJPURIFY_GET_INFO_INDEX(&this->dyna.actor);
        this->dyna.actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->dyna.actor);
        if (sp28 == 1) {
            DynaPolyActor_LoadMesh(play, &this->dyna, sObjPurifyInfo[index].colHeader);
            func_80A84FA0(this);
        } else if (ObjPurify_IsPurified(this)) {
            func_80A8515C(this);
        } else {
            func_80A84FEC(this);
        }
    }
}

void func_80A84FA0(ObjPurify* this) {
    this->actionFunc = func_80A84FB4;
}

void func_80A84FB4(ObjPurify* this, PlayState* play) {
    if (ObjPurify_IsPurified(this)) {
        Actor_Kill(&this->dyna.actor);
    }
}

void func_80A84FEC(ObjPurify* this) {
    this->dyna.actor.draw = func_80A851C8;
    this->gfxIndex = 0;
    this->actionFunc = func_80A85010;
}

void func_80A85010(ObjPurify* this, PlayState* play) {
    if (ObjPurify_IsPurified(this)) {
        func_80A85048(this);
    }
}

void func_80A85048(ObjPurify* this) {
    this->dyna.actor.draw = func_80A851C8;
    this->gfxIndex = 0;
    this->actionFunc = func_80A85074;
    this->timer = 30;
}

void func_80A85074(ObjPurify* this, PlayState* play) {
    this->timer--;
    if (this->timer <= 0) {
        func_80A850B0(this);
    }
}

void func_80A850B0(ObjPurify* this) {
    f32 yPos = this->dyna.actor.home.pos.y;
    this->dyna.actor.draw = func_80A85304;
    this->unk168[1] = yPos - 90.0f;
    this->actionFunc = func_80A850E8;
    this->unk168[0] = yPos;
}

void func_80A850E8(ObjPurify* this, PlayState* play) {
    Math_StepToF(&this->unk168[0], this->dyna.actor.home.pos.y - 90.0f, 0.9f);
    if (Math_StepToF(&this->unk168[1], this->dyna.actor.home.pos.y, 0.9f)) {
        func_80A8515C(this);
    }
}

void func_80A8515C(ObjPurify* this) {
    this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->dyna.actor.draw = func_80A851C8;
    this->gfxIndex = 1;
    this->actionFunc = ObjPurify_DoNothing;
}

void ObjPurify_DoNothing(ObjPurify* this, PlayState* play) {
}

void ObjPurify_Update(Actor* thisx, PlayState* play) {
    ObjPurify* this = (ObjPurify*)thisx;

    this->actionFunc(this, play);
}

void func_80A851C8(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjPurify* this = (ObjPurify*)thisx;
    ObjPurifyInfo* info = &sObjPurifyInfo[OBJPURIFY_GET_INFO_INDEX(&this->dyna.actor)];
    Gfx* opaDList = info->opaDLists[this->gfxIndex];
    Gfx* xluDList = info->xluDLists[this->gfxIndex];
    AnimatedMaterial* animMat = info->animMat[this->gfxIndex];

    OPEN_DISPS(play->state.gfxCtx);

    if (animMat != NULL) {
        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(animMat));
    }
    if (opaDList != NULL) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, opaDList);
    }
    if (xluDList != NULL) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, xluDList);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80A85304(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjPurify* this = (ObjPurify*)thisx;
    ObjPurifyInfo* info = &sObjPurifyInfo[OBJPURIFY_GET_INFO_INDEX(&this->dyna.actor)];
    s32 sp6C[2];
    s32 i;
    s32 index;

    if (this->unk168[1] < this->unk168[0]) {
        sp6C[0] = 1;
        sp6C[1] = 0;
    } else {
        sp6C[0] = 0;
        sp6C[1] = 1;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(sp6C); i++) {
        index = sp6C[i];
        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(info->animMat[index]));
        ObjPurify_SetSysMatrix(this->unk168[index]);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, info->xluDLists[index]);
    };

    CLOSE_DISPS(play->state.gfxCtx);
}
