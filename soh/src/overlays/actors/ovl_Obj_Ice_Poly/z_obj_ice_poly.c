/*
 * File: z_obj_ice_poly.c
 * Overlay: ovl_Obj_Ice_Poly
 * Description: Ice / Frozen Actors
 */

#include "z_obj_ice_poly.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS ACTOR_FLAG_4

void ObjIcePoly_Init(Actor* thisx, PlayState* play);
void ObjIcePoly_Destroy(Actor* thisx, PlayState* play);
void ObjIcePoly_Update(Actor* thisx, PlayState* play);
void ObjIcePoly_Draw(Actor* thisx, PlayState* play);

void ObjIcePoly_Idle(ObjIcePoly* this, PlayState* play);
void ObjIcePoly_Melt(ObjIcePoly* this, PlayState* play);

const ActorInit Obj_Ice_Poly_InitVars = {
    ACTOR_OBJ_ICE_POLY,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjIcePoly),
    (ActorFunc)ObjIcePoly_Init,
    (ActorFunc)ObjIcePoly_Destroy,
    (ActorFunc)ObjIcePoly_Update,
    (ActorFunc)ObjIcePoly_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInitIce = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x02, 0x00 },
        { 0x00020800, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 50, 120, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInitHard = {
    {
        COLTYPE_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x4E01F7F6, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 50, 120, 0, { 0, 0, 0 } },
};

static f32 sScale[] = { 0.5f, 1.0f, 1.5f };
static s16 sOffsetY[] = { -25, 0, -20 };
static Color_RGBA8 sColorWhite = { 250, 250, 250, 255 };
static Color_RGBA8 sColorGray = { 180, 180, 180, 255 };

void ObjIcePoly_Init(Actor* thisx, PlayState* play) {
    ObjIcePoly* this = (ObjIcePoly*)thisx;

    this->unk_151 = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;
    if (thisx->params < 0 || thisx->params >= 3) {
        Actor_Kill(thisx);
        return;
    }
    Actor_SetScale(thisx, sScale[thisx->params]);
    thisx->world.pos.y = sOffsetY[thisx->params] + thisx->home.pos.y;
    Collider_InitCylinder(play, &this->colliderIce);
    Collider_SetCylinder(play, &this->colliderIce, thisx, &sCylinderInitIce);
    Collider_InitCylinder(play, &this->colliderHard);
    Collider_SetCylinder(play, &this->colliderHard, thisx, &sCylinderInitHard);
    Collider_UpdateCylinder(thisx, &this->colliderIce);
    Collider_UpdateCylinder(thisx, &this->colliderHard);
    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    this->alpha = 255;
    this->colliderIce.dim.radius *= thisx->scale.x;
    this->colliderIce.dim.height *= thisx->scale.y;
    this->colliderHard.dim.radius *= thisx->scale.x;
    this->colliderHard.dim.height *= thisx->scale.y;
    Actor_SetFocus(thisx, thisx->scale.y * 30.0f);
    this->actionFunc = ObjIcePoly_Idle;
}

void ObjIcePoly_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjIcePoly* this = (ObjIcePoly*)thisx;

    if ((this->actor.params >= 0) && (this->actor.params < 3)) {
        Collider_DestroyCylinder(play, &this->colliderIce);
        Collider_DestroyCylinder(play, &this->colliderHard);
    }
}

void ObjIcePoly_Idle(ObjIcePoly* this, PlayState* play) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    s32 pad;
    Vec3f pos;

    if (this->colliderIce.base.acFlags & AC_HIT) {
        this->meltTimer = -this->colliderIce.info.acHitInfo->toucher.damage;
        this->actor.focus.rot.y = this->actor.yawTowardsPlayer;
        OnePointCutscene_Init(play, 5120, 40, &this->actor, MAIN_CAM);
        this->actionFunc = ObjIcePoly_Melt;
    } else if (this->actor.parent != NULL) {
        this->actor.parent->freezeTimer = 40;
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderIce.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderIce.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderIce.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderHard.base);
    } else {
        Actor_Kill(&this->actor);
    }
    pos.x = this->actor.world.pos.x + this->actor.scale.x * (Rand_S16Offset(15, 15) * (Rand_ZeroOne() < 0.5f ? -1 : 1));
    pos.y = this->actor.world.pos.y + this->actor.scale.y * Rand_S16Offset(10, 90);
    pos.z = this->actor.world.pos.z + this->actor.scale.z * (Rand_S16Offset(15, 15) * (Rand_ZeroOne() < 0.5f ? -1 : 1));
    if ((play->gameplayFrames % 7) == 0) {
        EffectSsKiraKira_SpawnDispersed(play, &pos, &zeroVec, &zeroVec, &sColorWhite, &sColorGray, 2000, 5);
    }
}

void ObjIcePoly_Melt(ObjIcePoly* this, PlayState* play) {
    Vec3f accel;
    Vec3f vel;
    Vec3f pos;
    s32 i;

    accel.x = 0.0f;
    accel.y = this->actor.scale.y;
    accel.z = 0.0f;
    vel.x = 0.0f;
    vel.y = this->actor.scale.y;
    vel.z = 0.0f;

    for (i = 0; i < 2; i++) {
        pos.x =
            this->actor.world.pos.x + this->actor.scale.x * (Rand_S16Offset(20, 20) * (Rand_ZeroOne() < 0.5f ? -1 : 1));
        pos.y = this->actor.world.pos.y + this->actor.scale.y * Rand_ZeroOne() * 50.0f;
        pos.z =
            this->actor.world.pos.z + this->actor.scale.x * (Rand_S16Offset(20, 20) * (Rand_ZeroOne() < 0.5f ? -1 : 1));
        func_8002829C(play, &pos, &vel, &accel, &sColorWhite, &sColorGray,
                      Rand_S16Offset(0x15E, 0x64) * this->actor.scale.x, this->actor.scale.x * 20.0f);
    }
    if (this->meltTimer < 0) {
        if (this->actor.parent != NULL) {
            this->actor.parent->freezeTimer = 40;
        }
        this->meltTimer++;
        if (this->meltTimer == 0) {
            this->meltTimer = 40;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_ICE_MELT);
        }
    } else {
        if (this->meltTimer != 0) {
            this->meltTimer--;
        }
        this->actor.scale.y = sScale[this->actor.params] * (0.5f + (this->meltTimer * 0.0125f));
        this->alpha -= 6;
        if (this->meltTimer == 0) {
            Actor_Kill(&this->actor);
        }
    }
}

void ObjIcePoly_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjIcePoly* this = (ObjIcePoly*)thisx;

    this->actionFunc(this, play);
}

void ObjIcePoly_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjIcePoly* this = (ObjIcePoly*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    func_8002ED80(&this->actor, play, 0);
    Matrix_RotateZYX(0x500, 0, -0x500, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, play->gameplayFrames % 0x100, 0x20, 0x10, 1, 0,
                                (play->gameplayFrames * 2) % 0x100, 0x40, 0x20));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, this->alpha);
    gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment3DL);

    CLOSE_DISPS(play->state.gfxCtx);
}
