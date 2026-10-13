/*
 * File: z_obj_smork.c
 * Overlay: ovl_Obj_Smork
 * Description: Romani Ranch Chimney Smoke
 */

#include "z_obj_smork.h"
#include "objects/object_f53_obj/object_f53_obj.h"
#include "BenPort.h"
#include <string.h>

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjSmork_Init(Actor* thisx, PlayState* play);
void ObjSmork_Destroy(Actor* thisx, PlayState* play);
void ObjSmork_Update(Actor* thisx, PlayState* play);
void ObjSmork_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Smork_Profile = {
    ACTOR_OBJ_SMORK,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_F53_OBJ,
    sizeof(ObjSmork),
    (ActorFunc)ObjSmork_Init,
    (ActorFunc)ObjSmork_Destroy,
    (ActorFunc)ObjSmork_Update,
    (ActorFunc)ObjSmork_Draw,
};

#include "overlays/ovl_Obj_Smork/ovl_Obj_Smork.h"

static Vtx* ovl_Obj_Smork_Vtx_000C10Data;

u8 func_80A3D680(s16 arg0) {
    u8 ret = 0;
    s16 temp_f18 = TIME_TO_MINUTES_F(CURRENT_TIME);
    s32 hours = temp_f18 / 60;
    s32 minutes = temp_f18 % 60;

    if (hours >= 16) {
        if ((hours == 16) && (minutes < arg0)) {
            return 0;
        }

        if (hours >= 20) {
            return 0;
        }

        if ((hours == 19) && (arg0 < minutes)) {
            return 0;
        }
        ret = 3;
    } else if (hours >= 11) {
        if ((hours == 11) && (minutes < arg0)) {
            return 0;
        }

        if (hours >= 14) {
            return 0;
        }

        if ((hours == 13) && (arg0 < minutes)) {
            return 0;
        }
        ret = 2;
    } else if (hours >= 6) {
        if ((hours == 6) && (minutes < arg0)) {
            return 0;
        }
        if (hours >= 9) {
            return 0;
        }

        if ((hours == 8) && (arg0 < minutes)) {
            return 0;
        }
        ret = 1;
    }

    return ret;
}

void func_80A3D940(ObjSmork* this) {
    u8 temp_v0 = func_80A3D680(this->unk_1C6);

    if (temp_v0 != this->unk_1C4) {
        this->unk_1C4 = temp_v0;
        this->unk_1C6 = Rand_S16Offset(0, 59);
    }

    Math_ApproachF(&this->unk_1B8, (this->unk_1C4 == 0) ? 0.0f : 1.0f, 0.02f, 1000.0f);
}

void func_80A3D9C4(ObjSmork* this, PlayState* play) {
    u8 sp57;
    u8 sp56;
    s32 i;

    this->unk_1BC += 1.8f;
    this->unk_1C0 += 0.6f;

    sp57 = 127 - (u8)this->unk_1BC;
    sp56 = 127 - (u8)this->unk_1C0;

    this->unk_1B8 = CLAMP(this->unk_1B8, 0.0f, 1.0f);

    for (i = 0; i < ARRAY_COUNT(this->unk_148); i++) {
        this->unk_148[i].v.cn[3] = ovl_Obj_Smork_Vtx_000C10Data[i].v.cn[3] * this->unk_1B8;
    }

    if (this->unk_1B8 > 0.0f) {
        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        this->actor.shape.rot.y = BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)));
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
        Matrix_Scale(0.1f, 0.1f, 0.0f, MTXMODE_APPLY);

        OPEN_DISPS(play->state.gfxCtx);

        //! @bug Uses Gfx_SetupDL25_Opa instead of Gfx_SetupDL25_Xlu. Gfx setup from previous actor is inherited.
        // 2S2H [Port] Opting to use correct XLU setup for proper rendering
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, sp57, 0x20, 0x20, 1, 0, sp56, 0x20, 0x20));
        gSPSegment(POLY_XLU_DISP++, 0x09, Lib_SegmentedToVirtual(this->unk_148));
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, object_f53_obj_DL_001C00);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void ObjSmork_Init(Actor* thisx, PlayState* play) {
    ObjSmork* this = (ObjSmork*)thisx;
    ovl_Obj_Smork_Vtx_000C10Data = ResourceMgr_LoadVtxArrayByName(ovl_Obj_Smork_Vtx_000C10);

    memcpy(this->unk_148, ovl_Obj_Smork_Vtx_000C10Data,
           sizeof(Vtx) * ResourceMgr_GetVtxArraySizeByName(ovl_Obj_Smork_Vtx_000C10));
    this->unk_1C6 = Rand_S16Offset(0, 59);
    this->unk_1C4 = 0;
}

void ObjSmork_Destroy(Actor* thisx, PlayState* play) {
}

void ObjSmork_Update(Actor* thisx, PlayState* play) {
    ObjSmork* this = (ObjSmork*)thisx;

    func_80A3D940(this);
}

void ObjSmork_Draw(Actor* thisx, PlayState* play) {
    ObjSmork* this = (ObjSmork*)thisx;

    func_80A3D9C4(this, play);
}
