/*
 * File: z_obj_chikuwa.c
 * Overlay: ovl_Obj_Chikuwa
 * Description: Falling row of blocks (unused)
 */

#include "z_obj_chikuwa.h"
#include "z64quake.h"
#include "objects/object_d_lift/object_d_lift.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjChikuwa_Init(Actor* thisx, PlayState* play);
void ObjChikuwa_Destroy(Actor* thisx, PlayState* play);
void ObjChikuwa_Update(Actor* thisx, PlayState* play);
void ObjChikuwa_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Chikuwa_Profile = {
    /**/ ACTOR_OBJ_CHIKUWA,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_D_LIFT,
    /**/ sizeof(ObjChikuwa),
    /**/ ObjChikuwa_Init,
    /**/ ObjChikuwa_Destroy,
    /**/ ObjChikuwa_Update,
    /**/ ObjChikuwa_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1500, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

Vec3f D_809B1FD0 = { 0.0f, -0.3f, 0.0f };
Vec3f D_809B1FDC = { 0.0f, 0.7f, 0.0f };

void func_809B1550(Actor* thisx, PlayState* play) {
    ObjChikuwa* this = (ObjChikuwa*)thisx;
    f32 sp18;

    if (this->unk_2A0 < this->unk_29C) {
        thisx->scale.z = (this->unk_29C - this->unk_2A0) * 0.05f;
        sp18 = (((this->unk_29C + this->unk_2A0) * 60) - 60) * 0.5f;
        thisx->world.pos.x = (Math_SinS(thisx->shape.rot.y) * sp18) + thisx->home.pos.x;
        thisx->world.pos.z = (Math_CosS(thisx->shape.rot.y) * sp18) + thisx->home.pos.z;
    } else {
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void ObjChikuwa_Init(Actor* thisx, PlayState* play) {
    ObjChikuwa* this = (ObjChikuwa*)thisx;
    s32 i;
    s32 val;
    ObjChikuwaStruct* temp;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    this->dyna.actor.scale.x = 0.15f;
    this->dyna.actor.scale.y = 0.2f;
    this->dyna.actor.scale.z = 0.05f;

    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gDampeGraveBrownElevatorCol);

    this->dyna.actor.home.rot.y += 0x2000;
    this->dyna.actor.home.rot.y &= 0xC000;
    this->unk_29C = 20;

    this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y;
    this->dyna.actor.world.rot.y = this->dyna.actor.home.rot.y;

    for (val = 0, i = 0; i < this->unk_29C; i++, val += 7) {
        temp = &this->unk_15C[i];

        temp->unk_00.x = thisx->home.pos.x + (Math_SinS(thisx->shape.rot.y) * (i * 60));
        temp->unk_00.y = thisx->home.pos.y;
        temp->unk_00.z = thisx->home.pos.z + (Math_CosS(thisx->shape.rot.y) * (i * 60));
        temp->unk_0C = val + 100;
    }

    func_809B1550(thisx, play);
}

void ObjChikuwa_Destroy(Actor* thisx, PlayState* play) {
    ObjChikuwa* this = (ObjChikuwa*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_809B17D0(PlayState* play, ObjChikuwa* this, Vec3f* arg2) {
    ObjChikuwaStruct2* temp_s0;
    f32 var_fs2;
    f32 temp;
    s32 i;

    for (i = 0, var_fs2 = -78.75f; i < 8; var_fs2 += 22.5f) {
        temp_s0 = &this->unk_2A4[this->unk_9A4];

        temp_s0->unk_00.x = (Rand_ZeroOne() * 0.05f) + 0.01f;
        temp_s0->unk_00.y = (Rand_ZeroOne() * 0.16f) + 0.04f;
        temp_s0->unk_00.z = (Rand_ZeroOne() * 0.05f) + 0.01f;

        temp_s0->unk_0C.x = (Math_CosS(this->dyna.actor.shape.rot.y) * var_fs2) + arg2->x;
        temp_s0->unk_0C.y = arg2->y;
        temp_s0->unk_0C.z = (Math_SinS(this->dyna.actor.shape.rot.y) * var_fs2) + arg2->z;

        temp_s0->unk_18 = (2.0f * Rand_ZeroOne()) - 1.0f;
        temp_s0->unk_1C = (Rand_ZeroOne() * 4.0f) - 1.0f;
        temp_s0->unk_20 = (2.0f * Rand_ZeroOne()) - 1.0f;

        temp = Math3D_Vec3fMagnitude(&temp_s0->unk_00);
        temp_s0->unk_24 = (-Rand_ZeroOne() * 0.5f) - temp * 25.0f;

        temp_s0->unk_28.x = this->dyna.actor.shape.rot.x;
        temp_s0->unk_28.y = this->dyna.actor.shape.rot.y;
        temp_s0->unk_28.z = this->dyna.actor.shape.rot.z;

        temp_s0->unk_2E.x = (Rand_ZeroOne() - 0.5f) * 5000.0f;
        temp_s0->unk_2E.y = (Rand_ZeroOne() - 0.5f) * 5000.0f;
        temp_s0->unk_2E.z = (Rand_ZeroOne() - 0.5f) * 5000.0f;

        temp_s0->unk_34 = 80;
        func_800B1210(play, &temp_s0->unk_0C, &D_809B1FDC, &D_809B1FD0, (Rand_ZeroOne() * 60.0f) + 80.0f,
                      (Rand_ZeroOne() * 30.0f) + 60.0f);
        this->unk_9A4 = (this->unk_9A4 + 1) & 0x1F;
        i++;
    }
}

void func_809B1AA0(ObjChikuwa* this) {
    ObjChikuwaStruct2* temp_v0;
    s32 i;

    this->unk_9A6 = 0;

    for (i = 0; i < ARRAY_COUNT(this->unk_2A4); i++) {
        temp_v0 = &this->unk_2A4[i];
        if (temp_v0->unk_34 > 0) {
            temp_v0->unk_34--;
            this->unk_9A6 = 1;
            temp_v0->unk_1C += temp_v0->unk_24;
            temp_v0->unk_0C.x += temp_v0->unk_18;
            temp_v0->unk_0C.y += temp_v0->unk_1C;
            temp_v0->unk_0C.z += temp_v0->unk_20;
            temp_v0->unk_28.x += temp_v0->unk_2E.x;
            temp_v0->unk_28.y += temp_v0->unk_2E.y;
            temp_v0->unk_28.z += temp_v0->unk_2E.z;
        }
    }
}

void ObjChikuwa_Update(Actor* thisx, PlayState* play) {
    ObjChikuwa* this = (ObjChikuwa*)thisx;
    ObjChikuwaStruct* temp;
    f32 temp_fs0;
    s16 quakeVerticalMag;
    s32 i;
    s32 quakeIndex;

    this->unk_29E++;

    for (i = 0; i < this->unk_29C; i++) {
        temp = &this->unk_15C[i];
        if ((this->unk_29E >= temp->unk_0C) && !(temp->unk_0E & 1)) {
            temp->unk_0E |= 1;
            this->unk_2A0++;
            func_809B1550(&this->dyna.actor, play);
            func_809B17D0(play, this, &temp->unk_00);
            temp_fs0 = Math3D_Vec3fDistSq(&temp->unk_00, &GET_PLAYER(play)->actor.world.pos);
            if (temp_fs0 < SQ(240.0f)) {
                quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
                if (temp_fs0 < SQ(120.0f)) {
                    quakeVerticalMag = 4;
                } else {
                    quakeVerticalMag = 3;
                }

                Quake_SetSpeed(quakeIndex, 17232);
                Quake_SetPerturbations(quakeIndex, quakeVerticalMag, 0, 0, 0);
                Quake_SetDuration(quakeIndex, 7);
            }
        }
    }

    func_809B1AA0(this);
}

void ObjChikuwa_Draw(Actor* thisx, PlayState* play) {
    ObjChikuwa* this = (ObjChikuwa*)thisx;
    ObjChikuwaStruct* temp;
    ObjChikuwaStruct2* temp2;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->unk_2A0 < this->unk_29C) {
        for (i = 0; i < this->unk_29C; i++) {
            temp = &this->unk_15C[i];
            if (!(temp->unk_0E & 1)) {
                Matrix_SetTranslateRotateYXZ(temp->unk_00.x, temp->unk_00.y, temp->unk_00.z,
                                             &this->dyna.actor.shape.rot);
                Matrix_Scale(0.15f, 0.2f, 0.05f, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gDampeGraveBrownElevatorDL);
            }
        }
    }

    if (this->unk_9A6 != 0) {
        for (i = 0; i < ARRAY_COUNT(this->unk_2A4); i++) {
            temp2 = &this->unk_2A4[i];
            if (temp2->unk_34 > 0) {
                Matrix_SetTranslateRotateYXZ(temp2->unk_0C.x, temp2->unk_0C.y, temp2->unk_0C.z, &temp2->unk_28);
                Matrix_Scale(temp2->unk_00.x, temp2->unk_00.y, temp2->unk_00.z, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gDampeGraveBrownElevatorDL);
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
