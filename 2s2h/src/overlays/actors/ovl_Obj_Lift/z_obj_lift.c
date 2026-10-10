/*
 * File: z_obj_lift.c
 * Overlay: ovl_Obj_Lift
 * Description: Dampé's Grave - Brown Elevator
 */

#include "z_obj_lift.h"
#include "z64quake.h"
#include "objects/object_d_lift/object_d_lift.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjLift_Init(Actor* thisx, PlayState* play);
void ObjLift_Destroy(Actor* thisx, PlayState* play);
void ObjLift_Update(Actor* thisx, PlayState* play);
void ObjLift_Draw(Actor* thisx, PlayState* play);

void func_8093D760(ObjLift* this);
void func_8093D7A0(ObjLift* this, PlayState* play);
void func_8093D9C0(ObjLift* this);
void func_8093D8B4(ObjLift* this, PlayState* play);
void func_8093D88C(ObjLift* this);
void func_8093DA48(ObjLift* this, PlayState* play);
void func_8093DB70(ObjLift* this);
void func_8093DB90(ObjLift* this, PlayState* play);
void func_8093DC90(Actor* thisx, PlayState* play);

ActorProfile Obj_Lift_Profile = {
    /**/ ACTOR_OBJ_LIFT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_D_LIFT,
    /**/ sizeof(ObjLift),
    /**/ ObjLift_Init,
    /**/ ObjLift_Destroy,
    /**/ ObjLift_Update,
    /**/ ObjLift_Draw,
};

static s16 D_8093DD50[] = { 0, 10, 20, 30, 40, 50, 60 };

Vec2s D_8093DD60[] = {
    { 120, -120 }, { 120, 0 },     { 120, 120 }, { 0, -120 },   { 0, 0 },
    { 0, 120 },    { -120, -120 }, { -120, 0 },  { -120, 120 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -600, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(terminalVelocity, -15000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 350, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 350, ICHAIN_STOP),
};

static f32 D_8093DD98[] = { 0.1f, 0.05f };

static f32 yOffsets[] = { -18.0f, -9.0f };

void func_8093D3C0(ObjLift* this, PlayState* play) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f* actorPos = &this->dyna.actor.world.pos;
    s32 i;
    s32 rand;
    s32 pad;

    for (i = 0; i < ARRAY_COUNT(D_8093DD60); i++) {
        pos.x = (D_8093DD60[i].x * this->dyna.actor.scale.x) + actorPos->x;
        pos.y = actorPos->y;
        pos.z = (D_8093DD60[i].z * this->dyna.actor.scale.z) + actorPos->z;

        velocity.x = D_8093DD60[i].x * this->dyna.actor.scale.x * 0.8f;
        velocity.y = (Rand_ZeroOne() * 10.0f) + 6.0f;
        velocity.z = D_8093DD60[i].z * this->dyna.actor.scale.z * 0.8f;

        if ((s32)Rand_Next() > 0) {
            rand = 0x40;
        } else {
            rand = 0x20;
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, actorPos, -0x100, rand, 15, 15, 0,
                             ((Rand_ZeroOne() * 50.0f) + 50.0f) * this->dyna.actor.scale.x, 0, 32, 50, -1,
                             OBJECT_D_LIFT, gDampeGraveBrownElevatorDL);
    }
    if (OBJLIFT_GET_1(&this->dyna.actor) == 0) {
        func_800BBFB0(play, &this->dyna.actor.world.pos, 120.0f, 12, 120, 100, 1);
    } else if (OBJLIFT_GET_1(&this->dyna.actor) == 1) {
        func_800BBFB0(play, &this->dyna.actor.world.pos, 60.0f, 8, 60, 100, 1);
    }
}

void ObjLift_Init(Actor* thisx, PlayState* play) {
    f32 temp_fv0;
    ObjLift* this = (ObjLift*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->dyna.actor.scale.x = this->dyna.actor.scale.z = D_8093DD98[OBJLIFT_GET_1(&this->dyna.actor)];
    this->dyna.actor.scale.y = 5.0f / 90.0f;
    this->dyna.actor.shape.rot.z = 0;
    this->unk_178 = this->dyna.actor.home.rot.z;
    this->dyna.actor.home.rot.z = this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z;
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    if ((this->unk_178 <= 0) && Flags_GetSwitch(play, OBJLIFT_GET_SWITCH_FLAG(&this->dyna.actor))) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    DynaPolyActor_LoadMesh(play, &this->dyna, &gDampeGraveBrownElevatorCol);
    this->unk_160 = Rand_Next() >> 0x10;
    this->unk_162 = Rand_Next() >> 0x10;
    this->unk_164 = Rand_Next() >> 0x10;
    func_8093D760(this);
}

void ObjLift_Destroy(Actor* thisx, PlayState* play) {
    ObjLift* this = (ObjLift*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_8093D760(ObjLift* this) {
    this->timer = D_8093DD50[OBJLIFT_GET_7(&this->dyna.actor)];
    this->actionFunc = func_8093D7A0;
    this->dyna.actor.draw = ObjLift_Draw;
}

void func_8093D7A0(ObjLift* this, PlayState* play) {
    s32 pad;
    s16 quakeIndex;

    if (DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
        if (this->timer <= 0) {
            if (OBJLIFT_GET_7(&this->dyna.actor) == 7) {
                func_8093D9C0(this);
            } else {
                quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_1);
                Quake_SetSpeed(quakeIndex, 10000);
                Quake_SetPerturbations(quakeIndex, 2, 0, 0, 0);
                Quake_SetDuration(quakeIndex, 20);

                func_8093D88C(this);
            }
        }
    } else {
        this->timer = D_8093DD50[OBJLIFT_GET_7(&this->dyna.actor)];
    }
}

void func_8093D88C(ObjLift* this) {
    this->timer = 20;
    this->actionFunc = func_8093D8B4;
    this->dyna.actor.draw = func_8093DC90;
}

void func_8093D8B4(ObjLift* this, PlayState* play) {
    if (this->timer <= 0) {
        func_8093D9C0(this);
    } else {
        this->unk_160 += 0x2710;
        this->unk_174 = Math_SinS(this->unk_160) * 300.0f;
        this->unk_176 = Math_CosS(this->unk_160) * 300.0f;
        this->unk_162 += 0x4650;
        this->unk_168.y = Math_SinS(this->unk_162);
        this->unk_164 += 0x4650;
        this->unk_168.x = Math_SinS(this->unk_164) * 3.0f;
        this->unk_168.z = Math_CosS(this->unk_164) * 3.0f;
    }
    if ((this->timer & 3) == 3) {
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 16, NA_SE_EV_BLOCK_SHAKE);
    }
}

void func_8093D9C0(ObjLift* this) {
    this->actionFunc = func_8093DA48;
    Math_Vec3f_Copy(&this->dyna.actor.world.pos, &this->dyna.actor.home.pos);
    this->dyna.actor.shape.rot = this->dyna.actor.world.rot = this->dyna.actor.home.rot;
    this->dyna.actor.draw = ObjLift_Draw;
}

void func_8093DA48(ObjLift* this, PlayState* play) {
    s32 pad;
    s32 bgId;
    Vec3f pos;

    Actor_MoveWithGravity(&this->dyna.actor);
    Math_Vec3f_Copy(&pos, &this->dyna.actor.prevPos);
    pos.y += yOffsets[OBJLIFT_GET_1(&this->dyna.actor)];
    this->dyna.actor.floorHeight =
        BgCheck_EntityRaycastFloor5(&play->colCtx, &this->dyna.actor.floorPoly, &bgId, &this->dyna.actor, &pos);
    if ((yOffsets[OBJLIFT_GET_1(&this->dyna.actor)] - 0.001f) <=
        (this->dyna.actor.floorHeight - this->dyna.actor.world.pos.y)) {
        func_8093D3C0(this, play);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 20, NA_SE_EV_BOX_BREAK);
        if (this->unk_178 > 0) {
            func_8093DB70(this);
            DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        } else {
            Flags_SetSwitch(play, OBJLIFT_GET_SWITCH_FLAG(&this->dyna.actor));
            Actor_Kill(&this->dyna.actor);
        }
    }
}

void func_8093DB70(ObjLift* this) {
    this->timer = this->unk_178;
    this->actionFunc = func_8093DB90;
    this->dyna.actor.draw = NULL;
}

void func_8093DB90(ObjLift* this, PlayState* play) {
    if (this->timer <= 0) {
        Math_Vec3f_Copy(&this->dyna.actor.world.pos, &this->dyna.actor.home.pos);
        this->dyna.actor.world.rot = this->dyna.actor.shape.rot = this->dyna.actor.home.rot;
        DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        func_8093D760(this);
    }
}

void ObjLift_Update(Actor* thisx, PlayState* play) {
    ObjLift* this = (ObjLift*)thisx;

    if (this->timer > 0) {
        this->timer--;
    }
    this->actionFunc(this, play);
}

void ObjLift_Draw(Actor* thisx, PlayState* play) {
    ObjLift* this = (ObjLift*)thisx;

    Gfx_DrawDListOpa(play, gDampeGraveBrownElevatorDL);
}

void func_8093DC90(Actor* thisx, PlayState* play) {
    ObjLift* this = (ObjLift*)thisx;
    Vec3f pos;
    Vec3s rot;

    Math_Vec3f_Sum(&this->dyna.actor.world.pos, &this->unk_168, &pos);
    rot.x = this->unk_174 + this->dyna.actor.home.rot.x;
    rot.y = this->dyna.actor.home.rot.y;
    rot.z = this->unk_176 + this->dyna.actor.home.rot.z;
    Matrix_SetTranslateRotateYXZ(pos.x, pos.y, pos.z, &rot);
    Matrix_Scale(this->dyna.actor.scale.x, this->dyna.actor.scale.y, this->dyna.actor.scale.z, MTXMODE_APPLY);
    Gfx_DrawDListOpa(play, gDampeGraveBrownElevatorDL);
}
