/*
 * File: z_obj_toge.c
 * Overlay: ovl_Obj_Toge
 * Description: Blade Trap
 */

#include "z_obj_toge.h"
#include "objects/object_trap/object_trap.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjToge_Init(Actor* thisx, PlayState* play);
void ObjToge_Destroy(Actor* thisx, PlayState* play2);
void ObjToge_Update(Actor* thisx, PlayState* play);
void ObjToge_Draw(Actor* thisx, PlayState* play);

void func_809A4744(ObjToge* this);
void func_809A477C(ObjToge* this, PlayState* play);
void func_809A4804(ObjToge* this);
void func_809A481C(ObjToge* this, PlayState* play);
void func_809A488C(ObjToge* this);
void func_809A48AC(ObjToge* this, PlayState* play);

ActorProfile Obj_Toge_Profile = {
    /**/ ACTOR_OBJ_TOGE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_TRAP,
    /**/ sizeof(ObjToge),
    /**/ ObjToge_Init,
    /**/ ObjToge_Destroy,
    /**/ ObjToge_Update,
    /**/ ObjToge_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x01C37BB6, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 20, 0, { 0, 0, 0 } },
};

f32 D_809A4CDC[] = { 2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f };

s16 D_809A4CFC[] = { 0, 10, 20, 30, 40, 50, 60, 70 };

f32 D_809A4D0C[] = { 1.0f, 2.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(terminalVelocity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 150, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 150, ICHAIN_STOP),
};

void func_809A41C0(ObjToge* this, PlayState* play) {
    Vec3f sp1C = this->actor.world.pos;

    sp1C.y += 10.0f;
    sp1C.z += 10.0f;
    func_800B2B44(play, &this->actor, &sp1C, 1.8f);

    sp1C.x += 10.0f;
    sp1C.z -= 20.0f;
    func_800B2B44(play, &this->actor, &sp1C, 1.8f);

    sp1C.x -= 20.0f;
    func_800B2B44(play, &this->actor, &sp1C, 1.8f);
}

void func_809A42A0(ObjToge* this, PlayState* play) {
    Vec3f sp54;
    s32 i;
    s16 phi_s0;

    sp54.y = this->actor.world.pos.y + 15.0f;

    for (i = 0, phi_s0 = 0; i < 5; i++, phi_s0 += 0x3333) {
        sp54.x = (Math_SinS(phi_s0) * 15.0f) + this->actor.world.pos.x;
        sp54.z = (Math_CosS(phi_s0) * 15.0f) + this->actor.world.pos.z;
        func_800B2B44(play, &this->actor, &sp54, (Rand_ZeroOne() * 0.6f) + 2.8f);
    }
}

void func_809A43A8(ObjToge* this, PlayState* play) {
    if (!OBJTOGE_GET_4000(&this->actor)) {
        func_809A41C0(this, play);
    } else {
        func_809A42A0(this, play);
    }
}

bool func_809A43EC(ObjToge* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 temp_fv1 = player->actor.world.pos.x - this->unk_1B8;
    f32 temp_fa0 = player->actor.world.pos.z - this->unk_1BC;
    f32 temp = -(this->unk_1CC * temp_fa0) + (temp_fv1 * this->unk_1C8);
    f32 temp2 = (this->unk_1CC * temp_fv1) + (temp_fa0 * this->unk_1C8);

    return fabsf(temp) <= this->unk_1C0 && fabsf(temp2) <= this->unk_1C4;
}

void ObjToge_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjToge* this = (ObjToge*)thisx;
    Path* path;
    Vec3s* points;
    s16 sp3E;
    s32 sp38 = OBJTOGE_GET_4000(thisx);

    Actor_ProcessInitChain(thisx, sInitChain);
    Actor_SetScale(thisx, D_809A4D0C[sp38] * 0.1f);

    sp3E = thisx->home.rot.z;
    thisx->home.rot.z = thisx->world.rot.z = thisx->shape.rot.z = 0;

    Collider_InitCylinder(play, &this->collider);

    if (OBJTOGE_GET_PATH_INDEX(thisx) == OBJTOGE_PATH_INDEX_NONE) {
        Actor_Kill(thisx);
        return;
    }

    path = &play->setupPathList[OBJTOGE_GET_PATH_INDEX(thisx)];
    if (path->count != 2) {
        Actor_Kill(thisx);
        return;
    }

    points = Lib_SegmentedToVirtual(path->points);
    Math_Vec3s_ToVec3f(&this->unk_198[0], &points[0]);
    Math_Vec3s_ToVec3f(&this->unk_198[1], &points[1]);
    Math_Vec3f_Copy(&thisx->world.pos, &this->unk_198[0]);
    thisx->world.rot.y = Math_Vec3f_Yaw(&this->unk_198[0], &this->unk_198[1]);
    this->unk_194 = 0;
    thisx->speed = 0.0f;

    if (sp3E > 0) {
        s16 sp36;

        this->unk_1B4 = true;
        this->unk_1B8 = (this->unk_198[0].x + this->unk_198[1].x) * 0.5f;
        this->unk_1BC = (this->unk_198[0].z + this->unk_198[1].z) * 0.5f;

        sp36 = Math_Vec3f_Yaw(&this->unk_198[0], &this->unk_198[1]);
        this->unk_1C8 = Math_CosS(sp36);
        this->unk_1CC = Math_SinS(sp36);
        this->unk_1C0 = sp3E * 7.5f;
        this->unk_1C4 = Math3D_Vec3f_DistXYZ(&this->unk_198[0], &this->unk_198[1]) * 0.5f;
    } else {
        this->unk_1B4 = false;
    }

    Collider_SetCylinder(play, &this->collider, thisx, &sCylinderInit);
    Collider_UpdateCylinder(thisx, &this->collider);
    thisx->colChkInfo.mass = MASS_IMMOVABLE;

    if (sp38 == 1) {
        this->collider.dim.radius = 60;
        this->collider.dim.height = 40;
    }

    if (this->unk_1B4) {
        func_809A4804(this);
    } else {
        func_809A4744(this);
    }
}

void ObjToge_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjToge* this = (ObjToge*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_809A4744(ObjToge* this) {
    this->actionFunc = func_809A477C;
    this->unk_1B0 = 0;
    this->unk_1B2 = D_809A4CFC[OBJTOGE_GET_3800(&this->actor)];
}

void func_809A477C(ObjToge* this, PlayState* play) {
    if (this->unk_1B2 > 0) {
        this->unk_1B2--;
    }

    if (this->unk_1B0 >= 0x1770) {
        this->unk_1B0 = 0x1770;
        func_809A488C(this);
    } else if ((this->unk_1B0 > 0) || (this->unk_1B2 <= 0)) {
        this->unk_1B0 += 200;
    }

    if (!this->unk_1B4) {
        this->actor.shape.rot.y += this->unk_1B0;
    }
}

void func_809A4804(ObjToge* this) {
    this->actionFunc = func_809A481C;
    this->unk_1B0 = 0;
}

void func_809A481C(ObjToge* this, PlayState* play) {
    if (this->unk_1B0 >= 0x1770) {
        this->unk_1B0 = 0x1770;
        func_809A488C(this);
    } else if ((this->unk_1B0 > 0) || func_809A43EC(this, play)) {
        this->unk_1B0 += 200;
    }
    this->actor.shape.rot.y += this->unk_1B0;
}

void func_809A488C(ObjToge* this) {
    this->actor.speed = 1.0f;
    this->actionFunc = func_809A48AC;
}

void func_809A48AC(ObjToge* this, PlayState* play) {
    s32 pad;
    s32 sp30 = this->unk_194 ^ 1;

    if (this->unk_1B4 && (this->unk_194 == 1)) {
        Math_StepToF(&this->actor.speed, 2.0f, 0.4f);
    } else {
        Math_StepToF(&this->actor.speed, D_809A4CDC[OBJTOGE_GET_700(&this->actor)], 1.5f);
        this->actor.shape.rot.y += 0x1770;
    }

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, D_809A4D0C[OBJTOGE_GET_4000(&this->actor)] * 30.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_80);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_198[this->unk_194]);
        this->unk_194 = sp30;
        if (this->unk_1B4 && (this->unk_194 == 0)) {
            func_809A4804(this);
        } else {
            func_809A4744(this);
        }
        Actor_PlaySfx(&this->actor, NA_SE_EV_SPINE_TRAP_MOVE);
    } else {
        s16 yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_198[sp30]) - this->actor.world.rot.y;

        yaw = ABS_ALT(yaw);
        if ((yaw > 0x4000) || (yaw == -0x8000)) {
            this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_198[this->unk_194]);
            this->unk_194 = sp30;
            if (this->unk_1B4 && (this->unk_194 == 0)) {
                func_809A4804(this);
            } else {
                func_809A4744(this);
            }
            Actor_PlaySfx(&this->actor, NA_SE_EV_SPINE_TRAP_MOVE);
        }
    }
}

void ObjToge_Update(Actor* thisx, PlayState* play) {
    ObjToge* this = (ObjToge*)thisx;
    ColliderCylinder* collider = &this->collider;

    if (this->collider.base.acFlags & AC_HIT) {
        if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x1000) {
            func_809A43A8(this, play);
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 250, COLORFILTER_BUFFLAG_OPA, 250);
        }
        collider->base.acFlags &= ~AC_HIT;
    }

    if (this->actor.colorFilterTimer == 0) {
        if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
            func_800B8DD4(play, &this->actor, 6.0f, this->actor.yawTowardsPlayer, 6.0f, 4);
        }

        this->actionFunc(this, play);

        Collider_UpdateCylinder(&this->actor, &this->collider);
        if (this->actor.xzDistToPlayer < 1000.0f) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }

    if ((this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) || (this->actor.xzDistToPlayer < 300.0f)) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &collider->base);
    } else {
        this->collider.base.ocFlags1 &= ~OC1_HIT;
        this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
    }
}

void ObjToge_Draw(Actor* thisx, PlayState* play) {
    ObjToge* this = (ObjToge*)thisx;

    func_800B8050(&this->actor, play, 1);
    Gfx_DrawDListOpa(play, object_trap_DL_001400);
}
