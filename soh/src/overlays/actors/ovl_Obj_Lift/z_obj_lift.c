/*
 * File: z_obj_lift.c
 * Overlay: ovl_Obj_Lift
 * Description: Square, collapsing platform
 */

#include "z_obj_lift.h"
#include "objects/object_d_lift/object_d_lift.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"

#define FLAGS ACTOR_FLAG_4

void ObjLift_Init(Actor* thisx, PlayState* play);
void ObjLift_Destroy(Actor* thisx, PlayState* play);
void ObjLift_Update(Actor* thisx, PlayState* play);
void ObjLift_Draw(Actor* thisx, PlayState* play);

void func_80B9651C(ObjLift* this);
void func_80B9664C(ObjLift* this);
void func_80B967C0(ObjLift* this);

void func_80B96560(ObjLift* this, PlayState* play);
void func_80B96678(ObjLift* this, PlayState* play);
void func_80B96840(ObjLift* this, PlayState* play);

const ActorInit Obj_Lift_InitVars = {
    ACTOR_OBJ_LIFT,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_D_LIFT,
    sizeof(ObjLift),
    (ActorFunc)ObjLift_Init,
    (ActorFunc)ObjLift_Destroy,
    (ActorFunc)ObjLift_Update,
    (ActorFunc)ObjLift_Draw,
    NULL,
};

static s16 sFallTimerDurations[] = { 0, 10, 20, 30, 40, 50, 60, 0 };

typedef struct {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 z;
} ObjLiftFramgentScale; // size = 0x4

static ObjLiftFramgentScale sFragmentScales[] = {
    { 120, -120 }, { 120, 0 },     { 120, 120 }, { 0, -120 },   { 0, 0 },
    { 0, 120 },    { -120, -120 }, { -120, 0 },  { -120, 120 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -600, ICHAIN_CONTINUE),   ICHAIN_F32_DIV1000(minVelocityY, -15000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE), ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 2000, ICHAIN_STOP),
};

static f32 sScales[] = { 0.1f, 0.05f };
static f32 sMaxFallDistances[] = { -18.0f, -9.0f };

void ObjLift_SetupAction(ObjLift* this, ObjLiftActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void ObjLift_InitDynaPoly(ObjLift* this, PlayState* play, CollisionHeader* collision, s32 flags) {
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&this->dyna, flags);
    CollisionHeader_GetVirtual(collision, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if (this->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.id, this->dyna.actor.params);
    }
}

void func_80B96160(ObjLift* this, PlayState* play) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f* temp_s3;
    s32 pad0;
    s32 i;

    temp_s3 = &this->dyna.actor.world.pos;

    for (i = 0; i < ARRAY_COUNT(sFragmentScales); i++) {
        pos.x = sFragmentScales[i].x * this->dyna.actor.scale.x + temp_s3->x;
        pos.y = temp_s3->y;
        pos.z = sFragmentScales[i].z * this->dyna.actor.scale.z + temp_s3->z;
        velocity.x = sFragmentScales[i].x * this->dyna.actor.scale.x * 0.8f;
        velocity.y = Rand_ZeroOne() * 10.0f + 6.0f;
        velocity.z = sFragmentScales[i].z * this->dyna.actor.scale.z * 0.8f;
        EffectSsKakera_Spawn(play, &pos, &velocity, temp_s3, -256, (Rand_ZeroOne() < 0.5f) ? 64 : 32, 15, 15, 0,
                             (Rand_ZeroOne() * 50.0f + 50.0f) * this->dyna.actor.scale.x, 0, 32, 50, KAKERA_COLOR_NONE,
                             OBJECT_D_LIFT, gCollapsingPlatformDL);
    }

    if (((this->dyna.actor.params >> 1) & 1) == 0) {
        func_80033480(play, &this->dyna.actor.world.pos, 120.0f, 12, 120, 100, 1);
    } else if (((this->dyna.actor.params >> 1) & 1) == 1) {
        func_80033480(play, &this->dyna.actor.world.pos, 60.0f, 8, 60, 100, 1);
    }
}

void ObjLift_Init(Actor* thisx, PlayState* play) {
    ObjLift* this = (ObjLift*)thisx;

    ObjLift_InitDynaPoly(this, play, &gCollapsingPlatformCol, DPM_PLAYER);

    if (Flags_GetSwitch(play, (this->dyna.actor.params >> 2) & 0x3F)) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    Actor_SetScale(&this->dyna.actor, sScales[(this->dyna.actor.params >> 1) & 1]);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->unk_168.x = Rand_ZeroOne() * 65535.5f;
    this->unk_168.y = Rand_ZeroOne() * 65535.5f;
    this->unk_168.z = Rand_ZeroOne() * 65535.5f;
    func_80B9651C(this);
    osSyncPrintf("(Dungeon Lift)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void ObjLift_Destroy(Actor* thisx, PlayState* play) {
    ObjLift* this = (ObjLift*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80B9651C(ObjLift* this) {
    this->timer = sFallTimerDurations[(this->dyna.actor.params >> 8) & 7];
    ObjLift_SetupAction(this, func_80B96560);
}

void func_80B96560(ObjLift* this, PlayState* play) {
    s32 pad;
    s32 quakeIndex;

    if (func_8004356C(&this->dyna)) {
        if (this->timer <= 0) {
            if (((this->dyna.actor.params >> 8) & 7) == 7) {
                func_80B967C0(this);
            } else {
                quakeIndex = Quake_Add(GET_ACTIVE_CAM(play), 1);
                Quake_SetSpeed(quakeIndex, 10000);
                Quake_SetQuakeValues(quakeIndex, 2, 0, 0, 0);
                Quake_SetCountdown(quakeIndex, 20);
                func_80B9664C(this);
            }
        }
    } else {
        this->timer = sFallTimerDurations[(this->dyna.actor.params >> 8) & 7];
    }
}

void func_80B9664C(ObjLift* this) {
    this->timer = 20;
    ObjLift_SetupAction(this, func_80B96678);
}

void func_80B96678(ObjLift* this, PlayState* play) {
    if (this->timer <= 0) {
        func_80B967C0(this);
    } else {
        this->unk_168.x += 10000;
        this->dyna.actor.world.rot.x = (s16)(Math_SinS(this->unk_168.x) * 300.0f) + this->dyna.actor.home.rot.x;
        this->dyna.actor.world.rot.z = (s16)(Math_CosS(this->unk_168.x) * 300.0f) + this->dyna.actor.home.rot.z;
        this->dyna.actor.shape.rot.x = this->dyna.actor.world.rot.x;
        this->dyna.actor.shape.rot.z = this->dyna.actor.world.rot.z;
        this->unk_168.y += 18000;
        this->dyna.actor.world.pos.y = Math_SinS(this->unk_168.y) + this->dyna.actor.home.pos.y;
        this->unk_168.z += 18000;
        this->dyna.actor.world.pos.x = Math_SinS(this->unk_168.z) * 3.0f + this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = Math_CosS(this->unk_168.z) * 3.0f + this->dyna.actor.home.pos.z;
    }

    if ((this->timer & 3) == 3) {
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 16, NA_SE_EV_BLOCK_SHAKE);
    }
}

void func_80B967C0(ObjLift* this) {
    ObjLift_SetupAction(this, func_80B96840);
    Math_Vec3f_Copy(&this->dyna.actor.world.pos, &this->dyna.actor.home.pos);
    this->dyna.actor.shape.rot = this->dyna.actor.world.rot = this->dyna.actor.home.rot;
}

void func_80B96840(ObjLift* this, PlayState* play) {
    s32 pad;
    s32 bgId;
    Vec3f sp2C;

    Actor_MoveForward(&this->dyna.actor);
    Math_Vec3f_Copy(&sp2C, &this->dyna.actor.prevPos);
    sp2C.y += sMaxFallDistances[(this->dyna.actor.params >> 1) & 1];
    this->dyna.actor.floorHeight =
        BgCheck_EntityRaycastFloor4(&play->colCtx, &this->dyna.actor.floorPoly, &bgId, &this->dyna.actor, &sp2C);

    if ((this->dyna.actor.floorHeight - this->dyna.actor.world.pos.y) >=
        (sMaxFallDistances[(this->dyna.actor.params >> 1) & 1] - 0.001f)) {
        func_80B96160(this, play);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 20, NA_SE_EV_BOX_BREAK);
        Flags_SetSwitch(play, (this->dyna.actor.params >> 2) & 0x3F);
        Actor_Kill(&this->dyna.actor);
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
    Gfx_DrawDListOpa(play, gCollapsingPlatformDL);
}
