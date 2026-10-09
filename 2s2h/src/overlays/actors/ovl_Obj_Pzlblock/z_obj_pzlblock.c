/*
 * File: z_obj_pzlblock.c
 * Overlay: ovl_Obj_Pzlblock
 * Description: Puzzle Block
 */

#include "z_obj_pzlblock.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "objects/object_secom_obj/object_secom_obj.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_CAN_PRESS_SWITCHES)

void ObjPzlblock_Init(Actor* thisx, PlayState* play);
void ObjPzlblock_Destroy(Actor* thisx, PlayState* play);
void ObjPzlblock_Update(Actor* thisx, PlayState* play);

void func_809A3A48(ObjPzlblock* this);
void func_809A3A74(ObjPzlblock* this, PlayState* play);
void func_809A3BA4(ObjPzlblock* this);
void func_809A3BC0(ObjPzlblock* this, PlayState* play);
void func_809A3D1C(ObjPzlblock* this);
void func_809A3D38(ObjPzlblock* this, PlayState* play);
void func_809A3E58(Actor* thisx, PlayState* play);
void func_809A3F0C(Actor* thisx, PlayState* play);

ActorProfile Obj_Pzlblock_Profile = {
    /**/ ACTOR_OBJ_PZLBLOCK,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjPzlblock),
    /**/ ObjPzlblock_Init,
    /**/ ObjPzlblock_Destroy,
    /**/ ObjPzlblock_Update,
    /**/ NULL,
};

s16 D_809A4050[] = { 1, -1, 0, 0 };
s16 D_809A4058[] = { 0, 0, 1, -1 };

typedef struct {
    /* 0x0 */ s16 objectId;
    /* 0x4 */ CollisionHeader* unk_04;
    /* 0x8 */ Gfx* unk_08;
} ObjPzlblockStruct; // size = 0xC

ObjPzlblockStruct D_809A4060[] = {
    { GAMEPLAY_DANGEON_KEEP, &gameplay_dangeon_keep_Colheader_01D488, gameplay_dangeon_keep_DL_01C228 },
    { OBJECT_SECOM_OBJ, &object_secom_obj_Colheader_002CB8, object_secom_obj_DL_001A58 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3S(world.rot, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 200, ICHAIN_STOP),
};

Color_RGB8 D_809A4088[] = {
    { 180, 150, 250 }, { 100, 180, 150 }, { 0, 0, 255 }, { 255, 255, 0 },
    { 0, 255, 255 },   { 255, 0, 255 },   { 0, 0, 0 },   { 255, 255, 255 },
};

bool func_809A33E0(ObjPzlblock* this, PlayState* play, s16 arg2) {
    return !DynaPolyActor_ValidateMove(play, &this->dyna, 30, arg2, 1) ||
           !DynaPolyActor_ValidateMove(play, &this->dyna, 30, arg2, 28);
}

s32 func_809A3448(ObjPzlblock* this) {
    s16 temp_v0;

    if (fabsf(this->dyna.pushForce) > 0.1f) {
        temp_v0 = BINANG_ADD(this->dyna.yRotation, 0x2000);

        if (this->dyna.pushForce < 0.0f) {
            temp_v0 = BINANG_ROT180(temp_v0);
        }

        if (temp_v0 < -0x4000) {
            return 3;
        }

        if (temp_v0 < 0) {
            return 1;
        }

        if (temp_v0 < 0x4000) {
            return 2;
        }
        return 0;
    }
    return -1;
}

bool func_809A34E0(ObjPzlblock* this, s32 arg1) {
    s32 temp_v0 = OBJPZLBLOCK_GET_ROTZ(&this->dyna.actor);

    if (temp_v0 == 0) {
        return arg1 == 0;
    }

    if (temp_v0 == 1) {
        return arg1 == 1;
    }

    if (temp_v0 == 2) {
        return arg1 == 2;
    }

    if (temp_v0 == 3) {
        return arg1 == 3;
    }

    if (temp_v0 == 4) {
        return (arg1 == 0) || (arg1 == 1);
    }

    if (temp_v0 == 5) {
        return (arg1 == 2) || (arg1 == 3);
    }

    if (temp_v0 == 6) {
        if (this->unk_176 != 0) {
            return (arg1 == 0) || (arg1 == 1);
        }

        if (this->unk_178 != 0) {
            return (arg1 == 2) || (arg1 == 3);
        }
        return true;
    }
    return false;
}

bool func_809A35EC(ObjPzlblock* this, s32 arg1) {
    s32 temp_v0 = OBJPZLBLOCK_GET_ROTZ(&this->dyna.actor);
    s32 temp_v1 = this->dyna.actor.home.rot.x & 0xF;
    s32 temp;

    if (temp_v0 == 0) {
        return this->unk_176 < temp_v1;
    }

    if (temp_v0 == 1) {
        return -temp_v1 < this->unk_176;
    }

    if (temp_v0 == 2) {
        return this->unk_178 < temp_v1;
    }

    if (temp_v0 == 3) {
        return -temp_v1 < this->unk_178;
    }

    if (temp_v0 == 4) {
        temp = D_809A4050[arg1] + this->unk_176;
        return (temp_v1 >= temp) && (-temp_v1 <= temp);
    }

    if (temp_v0 == 5) {
        temp = D_809A4058[arg1] + this->unk_178;
        return (temp_v1 >= temp) && (-temp_v1 <= temp);
    }

    if (temp_v0 == 6) {
        if ((arg1 == 0) || (arg1 == 1)) {
            temp = D_809A4050[arg1] + this->unk_176;
            return (temp_v1 >= temp) && (-temp_v1 <= temp);
        }

        temp = D_809A4058[arg1] + this->unk_178;
        return (temp_v1 >= temp) && (-temp_v1 <= temp);
    }

    return false;
}

void func_809A376C(ObjPzlblock* this, s32 arg1) {
    this->unk_176 += D_809A4050[arg1];
    this->unk_178 += D_809A4058[arg1];
    if ((arg1 == 0) || (arg1 == 1)) {
        this->unk_164 = &this->dyna.actor.world.pos.x;
        this->unk_168 = this->dyna.actor.home.pos.x + (this->unk_176 * 60);
    } else {
        this->unk_164 = &this->dyna.actor.world.pos.z;
        this->unk_168 = this->dyna.actor.home.pos.z + (this->unk_178 * 60);
    }
    this->unk_16C = arg1;
}

void ObjPzlblock_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjPzlblock* this = (ObjPzlblock*)thisx;
    s32 sp2C = OBJPZLBLOCK_GET_ROTZ(&this->dyna.actor);
    s32 sp28 = this->dyna.actor.home.rot.x & 0xF;
    ObjPzlblockStruct* sp24 = &D_809A4060[OBJPZLBLOCK_GET_1000(&this->dyna.actor)];

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    Actor_SetScale(&this->dyna.actor, 0.1f);

    this->dyna.actor.home.rot.y = 0;
    this->dyna.actor.shape.rot.x = 0;
    this->dyna.actor.shape.rot.z = 0;

    DynaPolyActor_Init(&this->dyna, 0);

    this->objectSlot = Object_GetSlot(&play->objectCtx, sp24->objectId);

    if (sp28 == 0) {
        func_809A3D1C(this);
    } else if (Flags_GetSwitch(play, OBJPZLBLOCK_GET_SWITCH_FLAG(&this->dyna.actor))) {
        if (sp2C == 0) {
            this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + (sp28 * 60);
            func_809A3D1C(this);
        } else if (sp2C == 1) {
            this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x - (sp28 * 60);
            func_809A3D1C(this);
        } else if (sp2C == 2) {
            this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + (sp28 * 60);
            func_809A3D1C(this);
        } else if (sp2C == 3) {
            this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z - (sp28 * 60);
            func_809A3D1C(this);
        } else {
            func_809A3A48(this);
        }
    } else {
        func_809A3A48(this);
    }
}

void ObjPzlblock_Destroy(Actor* thisx, PlayState* play) {
    ObjPzlblock* this = (ObjPzlblock*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_809A3A48(ObjPzlblock* this) {
    this->actionFunc = func_809A3A74;
    this->unk_16E[1] = 0;
    this->unk_16E[2] = 0;
    this->unk_16E[3] = 0;
    this->unk_16E[0] = 0;
    this->updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_4;
}

void func_809A3A74(ObjPzlblock* this, PlayState* play) {
    s32 i;
    s32 sp20 = func_809A3448(this);

    for (i = 0; i < ARRAY_COUNT(this->unk_16E); i++) {
        if (sp20 == i) {
            this->unk_16E[i]++;
        } else if (this->unk_16E[i] > 0) {
            this->unk_16E[i]--;
        }
    }

    if (sp20 != -1) {
        if (GameInteractor_Should(VB_BLOCK_BEGIN_MOVE, (this->unk_16E[sp20] >= 11), this) &&
            func_809A34E0(this, sp20) && func_809A35EC(this, sp20)) {
            if (!func_809A33E0(this, play, (this->dyna.pushForce > 0.0f) ? 90 : 120)) {
                func_809A376C(this, sp20);
                func_809A3BA4(this);
                return;
            }
        }

        GET_PLAYER(play)->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;
    }
}

void func_809A3BA4(ObjPzlblock* this) {
    this->actionFunc = func_809A3BC0;
    this->updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4;
}

void func_809A3BC0(ObjPzlblock* this, PlayState* play) {
    if (GameInteractor_Should(VB_BLOCK_BE_FINISHED_PULLING, Math_StepToF(this->unk_164, this->unk_168, 2.3f),
                              this->unk_164, this->unk_168, 2.3f, 2.3f)) {
        Player* player = GET_PLAYER(play);
        s32 params = OBJPZLBLOCK_GET_ROTZ(&this->dyna.actor);
        s32 pad;
        s32 sp20 = 0;

        if ((params == 4) || (params == 5) || (params == 6)) {
            if (!func_809A35EC(this, this->unk_16C) || func_809A33E0(this, play, 0x5A)) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
            }
        } else if (func_809A35EC(this, this->unk_16C)) {
            if (func_809A33E0(this, play, 0x5A)) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
            }
        } else {
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
            Flags_SetSwitch(play, OBJPZLBLOCK_GET_SWITCH_FLAG(&this->dyna.actor));
            sp20 = 1;
        }

        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;
        if (sp20 == 0) {
            func_809A3A48(this);
        } else {
            func_809A3D1C(this);
        }
    } else {
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
    }
}

void func_809A3D1C(ObjPzlblock* this) {
    this->actionFunc = func_809A3D38;
    this->updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_4;
}

void func_809A3D38(ObjPzlblock* this, PlayState* play) {
    if (fabsf(this->dyna.pushForce) > 0.1f) {
        GET_PLAYER(play)->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;
    }
}

void ObjPzlblock_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjPzlblock* this = (ObjPzlblock*)thisx;

    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
    Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 15.0f, 30.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        ObjPzlblockStruct* sp2C = &D_809A4060[OBJPZLBLOCK_GET_1000(&this->dyna.actor)];

        this->dyna.actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->dyna.actor);
        DynaPolyActor_LoadMesh(play, &this->dyna, sp2C->unk_04);
        this->dyna.actor.update = func_809A3E58;
        this->dyna.actor.draw = func_809A3F0C;
    }
}

void func_809A3E58(Actor* thisx, PlayState* play) {
    ObjPzlblock* this = (ObjPzlblock*)thisx;

    this->actionFunc(this, play);

    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;

    if (this->updBgCheckInfoFlags != 0) {
        Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 15.0f, 30.0f, 0.0f, this->updBgCheckInfoFlags);
        if (((this->actionFunc == func_809A3A74) || (this->actionFunc == func_809A3D38)) &&
            (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
            !DynaPoly_GetActor(&play->colCtx, this->dyna.actor.floorBgId)) {
            this->updBgCheckInfoFlags = 0;
        }
    }
}

void func_809A3F0C(Actor* thisx, PlayState* play) {
    ObjPzlblockStruct* sp2C = &D_809A4060[OBJPZLBLOCK_GET_1000(thisx)];
    Color_RGB8* sp28 = &D_809A4088[OBJPZLBLOCK_GET_700(thisx)];

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sp28->r, sp28->g, sp28->b, 255);
    gSPDisplayList(POLY_OPA_DISP++, sp2C->unk_08);

    CLOSE_DISPS(play->state.gfxCtx);
}
