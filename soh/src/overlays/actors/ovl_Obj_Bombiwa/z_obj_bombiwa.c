/*
 * File: z_obj_bombiwa.c
 * Overlay: ovl_Obj_Bombiwa
 * Description: Round, brown, breakable boulder
 */

#include "z_obj_bombiwa.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/object_bombiwa/object_bombiwa.h"

#define FLAGS 0

void ObjBombiwa_Init(Actor* thisx, PlayState* play);
void ObjBombiwa_InitCollision(Actor* thisx, PlayState* play);
void ObjBombiwa_Destroy(Actor* thisx, PlayState* play);
void ObjBombiwa_Update(Actor* thisx, PlayState* play);
void ObjBombiwa_Draw(Actor* thisx, PlayState* play);

void ObjBombiwa_Break(ObjBombiwa* this, PlayState* play);

const ActorInit Obj_Bombiwa_InitVars = {
    ACTOR_OBJ_BOMBIWA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_BOMBIWA,
    sizeof(ObjBombiwa),
    (ActorFunc)ObjBombiwa_Init,
    (ActorFunc)ObjBombiwa_Destroy,
    (ActorFunc)ObjBombiwa_Update,
    (ActorFunc)ObjBombiwa_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x4FC1FFFE, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 55, 70, 0, { 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 60, MASS_IMMOVABLE };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 350, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

static s16 sEffectScales[] = {
    17, 14, 10, 8, 7, 5, 3, 2,
};

void ObjBombiwa_InitCollision(Actor* thisx, PlayState* play) {
    ObjBombiwa* this = (ObjBombiwa*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider);
}

void ObjBombiwa_Init(Actor* thisx, PlayState* play) {
    Actor_ProcessInitChain(thisx, sInitChain);
    ObjBombiwa_InitCollision(thisx, play);
    if ((Flags_GetSwitch(play, thisx->params & 0x3F) != 0)) {
        Actor_Kill(thisx);
    } else {
        CollisionCheck_SetInfo(&thisx->colChkInfo, NULL, &sColChkInfoInit);
        if (thisx->shape.rot.y == 0) {
            s16 rand = (s16)Rand_ZeroFloat(65536.0f);

            thisx->world.rot.y = rand;
            thisx->shape.rot.y = rand;
        }
        thisx->shape.yOffset = -200.0f;
        thisx->world.pos.y = thisx->home.pos.y + 20.0f;
    }
}

void ObjBombiwa_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjBombiwa* this = (ObjBombiwa*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void ObjBombiwa_Break(ObjBombiwa* this, PlayState* play) {
    Vec3f pos;
    Vec3f velocity;
    Gfx* dlist;
    s16 arg5;
    s16 scale;
    s32 i;

    dlist = object_bombiwa_DL_0009E0;
    for (i = 0; i < ARRAY_COUNT(sEffectScales); i++) {
        pos.x = ((Rand_ZeroOne() - 0.5f) * 10.0f) + this->actor.home.pos.x;
        pos.y = ((Rand_ZeroOne() * 5.0f) + this->actor.home.pos.y) + 8.0f;
        pos.z = ((Rand_ZeroOne() - 0.5f) * 10.0f) + this->actor.home.pos.z;
        velocity.x = (Rand_ZeroOne() - 0.5f) * 15.0f;
        velocity.y = (Rand_ZeroOne() * 16.0f) + 5.0f;
        velocity.z = (Rand_ZeroOne() - 0.5f) * 15.0f;
        scale = sEffectScales[i];
        arg5 = (scale >= 11) ? 37 : 33;
        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -400, arg5, 10, 2, 0, scale, 1, 0, 80, KAKERA_COLOR_NONE,
                             OBJECT_BOMBIWA, dlist);
    }
    func_80033480(play, &this->actor.world.pos, 60.0f, 8, 100, 160, 1);
}

void ObjBombiwa_Update(Actor* thisx, PlayState* play) {
    ObjBombiwa* this = (ObjBombiwa*)thisx;
    s32 pad;

    // #region SOH [Co-op]
    if ((func_80033684(play, &this->actor) != NULL) ||
        ((this->collider.base.acFlags & AC_HIT) && (this->collider.info.acHitInfo->toucher.dmgFlags & 0x40000040)) || Flags_GetSwitch(play, this->actor.params & 0x3F)) {
    // #endregion
        ObjBombiwa_Break(this, play);
        Flags_SetSwitch(play, this->actor.params & 0x3F);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 80, NA_SE_EV_WALL_BROKEN);
        if (((this->actor.params >> 0xF) & 1) != 0) {
            func_80078884(NA_SE_SY_CORRECT_CHIME);
        }
        Actor_Kill(&this->actor);
    } else {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->actor.xzDistToPlayer < 800.0f) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void ObjBombiwa_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_bombiwa_DL_0009E0);
}
