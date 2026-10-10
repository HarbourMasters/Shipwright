/*
 * File: z_en_mushi2.c
 * Overlay: ovl_En_Mushi2
 * Description: Group of three bugs that burrow underground
 */

#include "overlays/actors/ovl_Obj_Bean/z_obj_bean.h"
#include "z_en_mushi2.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnMushi2_Init(Actor* thisx, PlayState* play);
void EnMushi2_Destroy(Actor* thisx, PlayState* play);
void EnMushi2_Update(Actor* thisx, PlayState* play);
void EnMushi2_Draw(Actor* thisx, PlayState* play);

void func_80A6A300(EnMushi2* this);
void func_80A6A36C(EnMushi2* this, PlayState* play);
void func_80A6A508(Actor* thisx);
void func_80A6A5C0(EnMushi2* this, PlayState* play);
void func_80A6A794(EnMushi2* this);
void func_80A6A824(EnMushi2* this, PlayState* play);
void func_80A6A984(EnMushi2* this);
void func_80A6A9E4(EnMushi2* this, PlayState* play);
void func_80A6AAA4(EnMushi2* this);
void func_80A6AB08(EnMushi2* this, PlayState* play);
void func_80A6AE14(EnMushi2* this);
void func_80A6AE7C(EnMushi2* this, PlayState* play);
void func_80A6B078(EnMushi2* this);
void func_80A6B0D8(EnMushi2* this, PlayState* play);

ActorProfile En_Mushi2_Profile = {
    /**/ ACTOR_EN_MUSHI2,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnMushi2),
    /**/ EnMushi2_Init,
    /**/ EnMushi2_Destroy,
    /**/ EnMushi2_Update,
    /**/ EnMushi2_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 5 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static Color_RGBA8 D_80A6B984[] = {
    { 170, 130, 90, 255 },
    { 250, 250, 250, 255 },
};

static Color_RGBA8 D_80A6B98C[] = {
    { 100, 60, 20, 255 },
    { 180, 180, 180, 255 },
};

static s32 D_80A6B994 = 0;

static EnMushi2Struct D_80A6B998[] = {
    { 1, 0x1F40 },
    { 2, 0xE0C0 },
};

static Vec3f D_80A6B9A0 = { 1.0f, 0.0f, 0.0f };
static Vec3f D_80A6B9AC = { 0.0f, 1.0f, 0.0f };
static Vec3f D_80A6B9B8 = { 0.0f, 0.0f, 1.0f };

static MtxF D_80A6B9C4 = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 20, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 20, ICHAIN_STOP),
};

f32 D_80A6BA14[] = {
    0.06f, // ENMUSHI2_0
    0.1f,  // ENMUSHI2_1
    0.13f, // ENMUSHI2_2
};

void func_80A687A0(EnMushi2* this) {
    MtxF* matrix = Matrix_GetCurrent();

    matrix->mf[3][0] += this->unk_348 * this->unk_31C.x;
    matrix->mf[3][1] += this->unk_348 * this->unk_31C.y;
    matrix->mf[3][2] += this->unk_348 * this->unk_31C.z;
}

void func_80A68808(EnMushi2* this) {
    Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                 &this->actor.shape.rot);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    Collider_UpdateSpheres(0, &this->collider);
}

s32 func_80A68860(EnMushi2* this, PlayState* play) {
    s32 pad;
    s32 bgId;
    CollisionPoly* sp3C;
    f32 temp_f0 = BgCheck_EntityRaycastFloor5(&play->colCtx, &sp3C, &bgId, &this->actor, &this->actor.world.pos);
    WaterBox* waterBox;
    f32 sp30;

    if ((temp_f0 > (BGCHECK_Y_MIN + 1)) && ((this->actor.world.pos.y - 150.0f) < temp_f0)) {
        return true;
    }

    return WaterBox_GetSurface1_2(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp30,
                                  &waterBox);
}

s32 func_80A68910(EnMushi2* this, PlayState* play) {
    Actor* bean = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;
    f32 minDistSq = SQ(100.0f);
    s32 ret = false;
    f32 temp_f0;
    f32 temp_f2;
    s32 params;

    this->unk_34C = NULL;

    while (bean != NULL) {
        if (bean->id == ACTOR_OBJ_BEAN) {
            params = OBJBEAN_GET_C000(bean);
            if (((params == ENOBJBEAN_GET_C000_1) || (params == ENOBJBEAN_GET_C000_2)) &&
                (bean->room == this->actor.room)) {
                temp_f0 = Math3D_Dist2DSq(this->actor.world.pos.x, this->actor.world.pos.z, bean->world.pos.x,
                                          bean->world.pos.z);
                temp_f2 = bean->world.pos.y - this->actor.world.pos.y;
                if ((temp_f0 < minDistSq) && (temp_f2 < 100.0f) && (temp_f2 > -50.0f)) {
                    ret = true;
                    minDistSq = temp_f0;
                    this->unk_34C = (ObjBean*)bean;
                    if (params == ENOBJBEAN_GET_C000_1) {
                        this->unk_350 = SQ(30.0f);
                    } else {
                        this->unk_350 = SQ(39.0f);
                    }
                }
            }
        }
        bean = bean->next;
    }

    return ret;
}

void func_80A68A78(EnMushi2* this, PlayState* play) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(D_80A6B998); i++) {
        Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_EN_MUSHI2, this->actor.world.pos.x,
                                      this->actor.world.pos.y, this->actor.world.pos.z, this->actor.shape.rot.x,
                                      this->actor.shape.rot.y + (D_80A6B998 + i)->unk_02, this->actor.shape.rot.z,
                                      (D_80A6B998 + i)->unk_00, this->actor.csId, this->actor.halfDaysBits, NULL);
    }
}

void func_80A68B3C(EnMushi2* this) {
    if (!(this->unk_30C & 0x100)) {
        this->unk_30C |= 0x100;
        D_80A6B994++;
    }
}

void func_80A68B6C(EnMushi2* this) {
    if (this->unk_30C & 0x100) {
        this->unk_30C &= ~0x100;
        D_80A6B994--;
    }
}

bool func_80A68BA0(EnMushi2* this) {
    return (D_80A6B994 > 3) && this->unk_34C == NULL;
}

void func_80A68BC8(EnMushi2* this) {
    if (this->unk_36C > 0) {
        this->unk_36C--;
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_EN_MUSI_WALK);

        this->unk_36C = 3.0f / CLAMP_MIN(this->skelAnime.playSpeed, 0.12f);
        if (this->unk_36C < 2) {
            this->unk_36C = 2;
        }
    }
}

s32 func_80A68C5C(Vec3f* arg0, Vec3f* arg1) {
    f32 temp_f0 = Math3D_Vec3fMagnitude(arg0);

    if (temp_f0 < 0.005f) {
        Math_Vec3f_Copy(arg0, &gZeroVec3f);
        return false;
    } else {
        arg1->x = arg0->x * (1.0f / temp_f0);
        arg1->y = arg0->y * (1.0f / temp_f0);
        arg1->z = arg0->z * (1.0f / temp_f0);
        return true;
    }
}

s32 func_80A68CE4(Vec3f* arg0, Vec3f* arg1, f32 arg2) {
    Vec3f sp34;
    f32 temp_f0;
    f32 temp_f2;

    Math_Vec3f_Diff(arg1, arg0, &sp34);
    temp_f0 = Math3D_Vec3fMagnitudeSq(&sp34);
    temp_f2 = arg2 + 0.01f;
    if (temp_f0 <= SQ(temp_f2)) {
        Math_Vec3f_Copy(arg0, arg1);
        return 1;
    }

    Math_Vec3f_Scale(&sp34, arg2 / sqrtf(temp_f0));
    sp34.x += arg0->x;
    sp34.y += arg0->y;
    sp34.z += arg0->z;

    if (func_80A68C5C(&sp34, arg0)) {
        return 0;
    }

    Math_Vec3f_Copy(arg0, arg1);
    return -1;
}

s32 func_80A68DD4(EnMushi2* this, PlayState* play) {
    s32 pad;
    Player* player;
    s16 sp2E;
    s32 pad2;
    f32 sp24;

    if ((this->actor.xzDistToPlayer < 32.0f) && (this->actor.playerHeightRel > -10.0f) &&
        (this->actor.playerHeightRel < 31.0f)) {
        player = GET_PLAYER(play);
        sp2E = BINANG_ROT180(this->actor.yawTowardsPlayer);
        sp24 = Math_SinS(sp2E);
        if (Math3D_Dist2DSq((sp24 * 16.0f) + player->actor.world.pos.x,
                            (Math_CosS(sp2E) * 16.0f) + player->actor.world.pos.z, this->actor.world.pos.x,
                            this->actor.world.pos.z) < SQ(20.0f)) {
            return true;
        }
    }
    return false;
}

void func_80A68ED8(EnMushi2* this) {
    this->actor.velocity.x = this->actor.speed * this->unk_328.x;
    this->actor.velocity.y = this->actor.speed * this->unk_328.y;
    this->actor.velocity.z = this->actor.speed * this->unk_328.z;
    Actor_UpdatePos(&this->actor);
}

void func_80A68F24(EnMushi2* this) {
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
    Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);
    Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_MultVec3f(&D_80A6B9A0, &this->unk_310);
    Matrix_MultVec3f(&D_80A6B9AC, &this->unk_31C);
    Matrix_MultVec3f(&D_80A6B9B8, &this->unk_328);
}

s32 func_80A68F9C(EnMushi2* this, s16 arg1) {
    MtxF* matrix;

    if (arg1 == 0) {
        return false;
    }

    matrix = Matrix_GetCurrent();

    matrix->mf[0][0] = this->unk_310.x;
    matrix->mf[0][1] = this->unk_310.y;
    matrix->mf[0][2] = this->unk_310.z;
    matrix->mf[0][3] = 0.0f;

    matrix->mf[1][0] = this->unk_31C.x;
    matrix->mf[1][1] = this->unk_31C.y;
    matrix->mf[1][2] = this->unk_31C.z;
    matrix->mf[1][3] = 0.0f;

    matrix->mf[2][0] = this->unk_328.x;
    matrix->mf[2][1] = this->unk_328.y;
    matrix->mf[2][2] = this->unk_328.z;
    matrix->mf[2][3] = 0.0f;

    matrix->mf[3][0] = 0.0f;
    matrix->mf[3][1] = 0.0f;
    matrix->mf[3][2] = 0.0f;
    matrix->mf[3][3] = 0.0f;

    Matrix_RotateYS(arg1, MTXMODE_APPLY);

    this->unk_310.x = matrix->mf[0][0];
    this->unk_310.y = matrix->mf[0][1];
    this->unk_310.z = matrix->mf[0][2];

    this->unk_31C.x = matrix->mf[1][0];
    this->unk_31C.y = matrix->mf[1][1];
    this->unk_31C.z = matrix->mf[1][2];

    this->unk_328.x = matrix->mf[2][0];
    this->unk_328.y = matrix->mf[2][1];
    this->unk_328.z = matrix->mf[2][2];

    this->unk_30C |= 0x40;
    return true;
}

s32 func_80A690C4(EnMushi2* this, s16 arg1) {
    MtxF* matrix;

    if (arg1 == 0) {
        return false;
    }

    matrix = Matrix_GetCurrent();

    matrix->mf[0][0] = this->unk_310.x;
    matrix->mf[0][1] = this->unk_310.y;
    matrix->mf[0][2] = this->unk_310.z;
    matrix->mf[0][3] = 0.0f;

    matrix->mf[1][0] = this->unk_31C.x;
    matrix->mf[1][1] = this->unk_31C.y;
    matrix->mf[1][2] = this->unk_31C.z;
    matrix->mf[1][3] = 0.0f;

    matrix->mf[2][0] = this->unk_328.x;
    matrix->mf[2][1] = this->unk_328.y;
    matrix->mf[2][2] = this->unk_328.z;
    matrix->mf[2][3] = 0.0f;

    matrix->mf[3][0] = 0.0f;
    matrix->mf[3][1] = 0.0f;
    matrix->mf[3][2] = 0.0f;
    matrix->mf[3][3] = 0.0f;

    Matrix_RotateXS(arg1, MTXMODE_APPLY);

    this->unk_310.x = matrix->mf[0][0];
    this->unk_310.y = matrix->mf[0][1];
    this->unk_310.z = matrix->mf[0][2];

    this->unk_31C.x = matrix->mf[1][0];
    this->unk_31C.y = matrix->mf[1][1];
    this->unk_31C.z = matrix->mf[1][2];

    this->unk_328.x = matrix->mf[2][0];
    this->unk_328.y = matrix->mf[2][1];
    this->unk_328.z = matrix->mf[2][2];

    this->unk_30C |= 0x40;
    return true;
}

s32 func_80A691EC(EnMushi2* this, CollisionPoly* floorPoly, f32 arg2) {
    Vec3f sp64;
    Vec3f sp58;
    Vec3f sp4C;
    Vec3f sp40;
    Vec3f sp34;
    s32 sp30;

    if (arg2 > 0.0f) {
        Vec3f sp24;

        sp24.x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
        sp24.y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
        sp24.z = COLPOLY_GET_NORMAL(floorPoly->normal.z);
        Math_Vec3f_Copy(&sp4C, &this->unk_31C);
        sp30 = func_80A68CE4(&sp4C, &sp24, arg2);
    } else {
        sp4C.x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
        sp4C.y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
        sp4C.z = COLPOLY_GET_NORMAL(floorPoly->normal.z);
        sp30 = 1;
    }

    Math3D_Vec3f_Cross(&this->unk_310, &sp4C, &sp40);
    Math3D_Vec3f_Cross(&sp4C, &sp40, &sp64);

    if (func_80A68C5C(&sp64, &sp58) && func_80A68C5C(&sp40, &sp34)) {
        Math_Vec3f_Copy(&this->unk_31C, &sp4C);
        Math_Vec3f_Copy(&this->unk_310, &sp58);
        Math_Vec3f_Copy(&this->unk_328, &sp34);
        this->unk_30C |= 0x40;
        return sp30;
    }

    return -1;
}

void func_80A69388(EnMushi2* this) {
    D_80A6B9C4.mf[0][0] = this->unk_310.x;
    D_80A6B9C4.mf[0][1] = this->unk_310.y;
    D_80A6B9C4.mf[0][2] = this->unk_310.z;

    D_80A6B9C4.mf[1][0] = this->unk_31C.x;
    D_80A6B9C4.mf[1][1] = this->unk_31C.y;
    D_80A6B9C4.mf[1][2] = this->unk_31C.z;

    D_80A6B9C4.mf[2][0] = this->unk_328.x;
    D_80A6B9C4.mf[2][1] = this->unk_328.y;
    D_80A6B9C4.mf[2][2] = this->unk_328.z;

    Matrix_MtxFToYXZRot(&D_80A6B9C4, &this->actor.world.rot, false);
    this->actor.shape.rot = this->actor.world.rot;
}

void func_80A69424(EnMushi2* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 8.0f, 9.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40);
}

s32 func_80A69468(EnMushi2* this, PlayState* play) {
    s32 pad;
    Vec3f sp68;
    Vec3f posB;
    Vec3f sp50;
    f32 x;
    f32 y;
    f32 z;

    sp50.x = 2.0f * this->unk_31C.x;
    sp50.y = 2.0f * this->unk_31C.y;
    sp50.z = 2.0f * this->unk_31C.z;

    Math_Vec3f_Sum(&sp50, &this->actor.prevPos, &sp68);

    x = (this->unk_328.x * 4.0f);
    y = (this->unk_328.y * 4.0f);
    z = (this->unk_328.z * 4.0f);

    posB.x = (x + sp50.x) + this->actor.world.pos.x;
    posB.y = (y + sp50.y) + this->actor.world.pos.y;
    posB.z = (z + sp50.z) + this->actor.world.pos.z;

    if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.prevPos, &posB, &this->unk_33C, &this->poly, true, true,
                                true, true, &this->polyBgId)) {
        this->unk_30C |= 6;
        return true;
    }
    return false;
}

s32 func_80A6958C(EnMushi2* this, PlayState* play) {
    s32 pad;
    Vec3f posA;
    Vec3f posB;

    posA.x = (2.0f * this->unk_31C.x) + this->actor.world.pos.x;
    posA.y = (2.0f * this->unk_31C.y) + this->actor.world.pos.y;
    posA.z = (2.0f * this->unk_31C.z) + this->actor.world.pos.z;

    posB.x = (this->unk_31C.x * -4.0f) + this->actor.world.pos.x;
    posB.y = (this->unk_31C.y * -4.0f) + this->actor.world.pos.y;
    posB.z = (this->unk_31C.z * -4.0f) + this->actor.world.pos.z;

    if (BgCheck_EntityLineTest1(&play->colCtx, &posA, &posB, &this->unk_33C, &this->poly, true, true, true, true,
                                &this->polyBgId)) {
        this->unk_30C |= 0xA;
        return true;
    }
    return false;
}

s32 func_80A6969C(EnMushi2* this, PlayState* play) {
    s32 pad;
    Vec3f posA;
    Vec3f posB;
    Vec3f sp38;

    sp38.x = this->unk_31C.x * -4.0f;
    sp38.y = this->unk_31C.y * -4.0f;
    sp38.z = this->unk_31C.z * -4.0f;

    Math_Vec3f_Sum(&sp38, &this->actor.world.pos, &posA);

    posB.x = this->actor.prevPos.x + sp38.x + (this->unk_328.x * -4.0f);
    posB.y = this->actor.prevPos.y + sp38.y + (this->unk_328.y * -4.0f);
    posB.z = this->actor.prevPos.z + sp38.z + (this->unk_328.z * -4.0f);

    if (BgCheck_EntityLineTest1(&play->colCtx, &posA, &posB, &this->unk_33C, &this->poly, true, true, true, true,
                                &this->polyBgId)) {
        this->unk_30C |= (0x10 | 0x2);
        return true;
    }
    return false;
}

void func_80A697C4(EnMushi2* this, PlayState* play) {
    s32 pad;
    CollisionPoly* poly = this->poly;
    s32 bgId = this->polyBgId;

    if (!(this->unk_30C & (0x10 | 0x4))) {
        WaterBox* waterBox;
        f32 sp2C;

        this->unk_30C &= ~(0x40 | 0x8 | 0x2);
        if (!func_80A69468(this, play) && !func_80A6958C(this, play)) {
            func_80A6969C(this, play);
        }

        if (WaterBox_GetSurface1_2(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp2C,
                                   &waterBox)) {
            this->actor.depthInWater = sp2C - this->actor.world.pos.y;
            if (this->actor.depthInWater >= 1.0f) {
                this->unk_30C |= 0x20;
            } else {
                this->unk_30C &= ~0x20;
            }
        } else {
            this->actor.depthInWater = BGCHECK_Y_MIN;
            this->unk_30C &= ~0x20;
        }
    }

    if (this->unk_30C & 4) {
        Math_Vec3f_Copy(&this->actor.world.pos, &this->unk_33C);
        if (func_80A691EC(this, this->poly, 0.4f) == 1) {
            this->unk_30C &= ~0x4;
        }
    } else if (this->unk_30C & 0x10) {
        if (func_80A691EC(this, this->poly, 0.4f) == 1) {
            this->unk_30C &= ~0x10;
            Math_Vec3f_Copy(&this->actor.world.pos, &this->unk_33C);
        } else {
            Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
        }
    } else if (this->unk_30C & 8) {
        if (!(this->unk_30C & 0x20)) {
            Math_Vec3f_Copy(&this->actor.world.pos, &this->unk_33C);
            func_80A691EC(this, this->poly, -1.0f);
        }
    } else if (!(this->unk_30C & 2)) {
        Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
        Math_Vec3f_Copy(&this->unk_33C, &this->actor.prevPos);
        this->poly = poly;
        this->polyBgId = bgId;
    }
}

s32 func_80A699E4(EnMushi2* this, PlayState* play) {
    s32 pad;
    WaterBox* waterBox;
    f32 sp3C;

    if (this->unk_328.y < 0.0f) {
        f32 x = (2.0f * this->unk_328.x) + this->actor.world.pos.x;
        f32 y = (2.0f * this->unk_328.y) + this->actor.world.pos.y;
        f32 z = (2.0f * this->unk_328.z) + this->actor.world.pos.z;

        if (WaterBox_GetSurface1_2(play, &play->colCtx, x, z, &sp3C, &waterBox) && (y <= sp3C)) {
            return true;
        }
    }
    return false;
}

f32 func_80A69AA8(f32 x, f32 y, f32 z, Vec3f* pos) {
    return -((pos->x * x) + (y * pos->y) + (z * pos->z));
}

void func_80A69ADC(Actor* thisx) {
    EnMushi2* this = (EnMushi2*)thisx;
    ObjBean* bean = this->unk_34C;
    f32 sp44;
    f32 sp40;
    f32 sp3C;
    s32 pad2;

    if (this->unk_358 < 1.0f) {
        sp44 = Math3D_PlaneF(this->unk_310.x, this->unk_310.y, this->unk_310.z,
                             func_80A69AA8(this->unk_310.x, this->unk_310.y, this->unk_310.z, &this->actor.world.pos),
                             &bean->dyna.actor.world.pos);
        sp40 = Math3D_PlaneF(this->unk_328.x, this->unk_328.y, this->unk_328.z,
                             func_80A69AA8(this->unk_328.x, this->unk_328.y, this->unk_328.z, &this->actor.world.pos),
                             &bean->dyna.actor.world.pos);
        sp3C = Math3D_Dist1D(sp44, sp40);

        if (fabsf(sp3C) > 0.1f) {
            this->unk_360 = (Rand_ZeroOne() * 3000.0f * (this->unk_358 - 1.0f)) + (((sp40 / sp3C) - 1.0f) * -1500.0f);
            this->unk_364 = 0;
            if (Rand_ZeroOne() < this->unk_358) {
                this->unk_366 = (sp44 >= 0.0f) ? 2000 : -2000;
            } else {
                this->unk_366 = ((s32)Rand_Next() > 0) ? 2000 : -2000;
            }
            this->unk_366 += TRUNCF_BINANG(((Rand_ZeroOne() * (1.0f - this->unk_358)) - 0.5f) * 0x400);
        } else {
            this->unk_360 = 0.0f;
            this->unk_364 = -0x8000;
            this->unk_366 = 0;
        }
    }
}

void func_80A69CE0(Actor* thisx) {
    EnMushi2* this = (EnMushi2*)thisx;

    this->unk_360 = Rand_ZeroOne() * 1500.0f;
    this->unk_364 = 0;
    if ((s32)Rand_Next() > 0) {
        this->unk_366 = 2000;
    } else {
        this->unk_366 = -2000;
    }
}

void func_80A69D3C(EnMushi2* this) {
    s32 pad;
    f32 sp40;
    f32 sp3C;
    f32 sp38;

    if ((this->unk_34C != NULL) && (this->unk_358 > 0.999f)) {
        sp40 = Math3D_PlaneF(this->unk_310.x, this->unk_310.y, this->unk_310.z,
                             func_80A69AA8(this->unk_310.x, this->unk_310.y, this->unk_310.z, &this->actor.world.pos),
                             &this->unk_34C->dyna.actor.world.pos);
        sp3C = Math3D_PlaneF(this->unk_328.x, this->unk_328.y, this->unk_328.z,
                             func_80A69AA8(this->unk_328.x, this->unk_328.y, this->unk_328.z, &this->actor.world.pos),
                             &this->unk_34C->dyna.actor.world.pos);
        sp38 = Math3D_Dist1DSq(sp40, sp3C);

        if (fabsf(sp38) > 0.010000001f) {
            s16 temp_v0 = Math_Atan2S_XY(sp3C, sp40);
            temp_v0 = CLAMP(temp_v0, -3000, 3000);
            func_80A68F9C(this, temp_v0);
        }
    } else {
        this->unk_364 += this->unk_366;
        if (this->unk_366 >= 0) {
            if (this->unk_364 < 0) {
                this->unk_364 = -0x8000;
            }
        } else if (this->unk_364 > 0) {
            this->unk_364 = -0x8000;
        }
        func_80A68F9C(this, Math_SinS(this->unk_364) * this->unk_360);
    }
}

s32 EnMushi2_IsUnderwater(EnMushi2* this, PlayState* play) {
    s32 pad;
    WaterBox* waterBox;
    f32 waterSurface;
    s32 bgId;

    if (WaterBox_GetSurfaceImpl(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &waterSurface,
                                &waterBox, &bgId) &&
        (this->actor.world.pos.y < waterSurface)) {
        return true;
    }
    return false;
}

void func_80A69F5C(Actor* thisx, PlayState* play) {
    EnMushi2* this = (EnMushi2*)thisx;
    s32 i;

    for (i = 0; i < 7; i++) {
        EffectSsBubble_Spawn(play, &this->actor.world.pos, -10.0f, 10.0f, 10.0f, (Rand_ZeroOne() * 0.09f) + 0.05f);
    }
}

s32 func_80A6A024(EnMushi2* this) {
    ObjBean* bean = this->unk_34C;

    if ((this->unk_34C != NULL) && (bean->unk_1E4 == 0)) {
        bean->unk_1E4 = 1;
        return true;
    }
    return false;
}

s32 func_80A6A058(EnMushi2* this) {
    ObjBean* bean = this->unk_34C;

    if ((this->unk_34C != NULL) && (bean->unk_1E4 == 2)) {
        bean->unk_1E0++;
        return true;
    }
    return false;
}

s32 func_80A6A094(EnMushi2* this) {
    if (this->unk_34C != NULL) {
        ObjBean* bean = this->unk_34C;

        if (GameInteractor_Should(VB_COUNT_BURROWED_BUGS, (bean->unk_1E4 == 2) || (bean->unk_1E4 == 1), bean)) {
            bean->unk_1E4 = 4;
            return true;
        }
    }
    return false;
}

void func_80A6A0D8(EnMushi2* this) {
    if (this->unk_34C != NULL) {
        this->unk_34C = NULL;
    }
}

void EnMushi2_Init(Actor* thisx, PlayState* play) {
    EnMushi2* this = (EnMushi2*)thisx;
    s32 pad;
    s32 sp3C;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (ENMUSHI2_GET_3(&this->actor) == ENMUSHI2_0) {
        func_80A68A78(this, play);
    }
    this->actor.shape.rot.y += Rand_S16Offset(-0x7D0, 0xFA0);
    this->actor.home.rot.y = this->actor.shape.rot.y;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    func_80A68F24(this);
    SkelAnime_Init(play, &this->skelAnime, &gameplay_keep_Skel_0527A0, &gameplay_keep_Anim_05140C, this->jointTable,
                   this->morphTable, BUG_LIMB_MAX);
    Animation_Change(&this->skelAnime, &gameplay_keep_Anim_05140C, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, 0.0f);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);
    func_80A68808(this);
    this->actor.colChkInfo.mass = 30;
    sp3C = EnMushi2_IsUnderwater(this, play);

    if ((sp3C == 0) && func_80A68860(this, play) && func_80A68910(this, play) &&
        (ENMUSHI2_GET_3(&this->actor) == ENMUSHI2_0)) {
        func_80A6A024(this);
    }

    this->unk_358 = 0.0f;
    this->unk_36A = Rand_S16Offset(240, 40);
    this->unk_30C = 1;

    if (sp3C != 0) {
        func_80A69F5C(&this->actor, play);
        this->actor.world.rot.y = Rand_Next() >> 0x10;
        func_80A6AE14(this);
    } else {
        func_80A6A300(this);
    }

    func_80A68B3C(this);
}

void EnMushi2_Destroy(Actor* thisx, PlayState* play) {
    EnMushi2* this = (EnMushi2*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
    func_80A68B6C(this);
}

void func_80A6A300(EnMushi2* this) {
    this->unk_368 = 100;
    this->actor.gravity = -0.25f;
    this->actor.terminalVelocity = -3.5f;
    this->unk_370 = Rand_S16Offset(-1000, 2000);
    this->skelAnime.playSpeed = 1.5f;
    this->actionFunc = func_80A6A36C;
}

void func_80A6A36C(EnMushi2* this, PlayState* play) {
    s32 pad;
    s32 sp20 = false;

    Math_StepToF(&this->actor.speed, 0.0f, 0.2f);
    this->actor.velocity.y -= this->actor.velocity.y * D_80A6BA14[ENMUSHI2_GET_3(&this->actor)];
    Actor_MoveWithGravity(&this->actor);
    func_80A69424(this, play);
    this->actor.shape.rot.y += this->unk_370;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if ((this->actor.floorHeight < this->actor.world.pos.y) &&
        (this->actor.world.pos.y < (this->actor.floorHeight + 4.0f))) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x1770);
        this->actor.shape.rot.z += Rand_S16Offset(-2000, 4000);
    }

    if (this->actor.floorHeight <= (BGCHECK_Y_MIN + 1)) {
        sp20 = true;
        func_80A6A094(this);
        func_80A6A0D8(this);
    }

    if (sp20 && (this->unk_36A <= 0)) {
        Actor_Kill(&this->actor);
        return;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
        func_80A6AAA4(this);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_GERUDOFT_WALK);
        func_80A68F24(this);
        func_80A691EC(this, this->actor.floorPoly, -1.0f);
        func_80A69388(this);
        func_80A6A508(&this->actor);
    }
}

void func_80A6A508(Actor* thisx) {
    EnMushi2* this = (EnMushi2*)thisx;

    if (this->unk_36A > 100) {
        this->unk_35C = Rand_ZeroOne() + 1.0f;
    } else {
        this->unk_35C = (Rand_ZeroOne() * 0.8f) + 1.5f;
    }

    this->unk_368 = Rand_S16Offset(10, 30);
    if (this->unk_34C == NULL) {
        func_80A69CE0(thisx);
    } else {
        func_80A69ADC(thisx);
    }
    this->actionFunc = func_80A6A5C0;
}

void func_80A6A5C0(EnMushi2* this, PlayState* play) {
    Actor* thisx = &this->actor;

    func_80A69D3C(this);
    Math_SmoothStepToF(&thisx->speed, this->unk_35C, 0.1f, 0.5f, 0.0f);
    if (this->unk_354 < SQ(40.0f)) {
        f32 temp = 1.0f - ((SQ(40.0f) - this->unk_354) * (1.0f / (10.0f * SQ(40.0f))));

        thisx->speed *= temp;
    }

    func_80A68ED8(this);
    func_80A697C4(this, play);
    if (this->unk_30C & 0x40) {
        func_80A69388(this);
    }

    this->skelAnime.playSpeed = thisx->speed * 1.6f;
    this->skelAnime.playSpeed = CLAMP(this->skelAnime.playSpeed, 0.1f, 1.9f);

    if ((this->unk_36A <= 0) || func_80A68BA0(this)) {
        func_80A6B078(this);
    } else if (!(this->unk_30C & 2)) {
        func_80A6A984(this);
    } else if (this->unk_30C & 0x20) {
        func_80A6AAA4(this);
    } else if (this->unk_368 <= 0) {
        func_80A6A794(this);
    } else if (((play->gameplayFrames % 2) != 0) && func_80A699E4(this, play)) {
        func_80A6A984(this);
    }
}

void func_80A6A794(EnMushi2* this) {
    if (this->unk_354 < SQ(40.0f)) {
        this->unk_368 = Rand_S16Offset(2, 4);
    } else {
        this->unk_368 = Rand_S16Offset(5, 10);
    }

    this->unk_35C = Rand_ZeroOne() * 0.4f;
    this->actionFunc = func_80A6A824;
}

void func_80A6A824(EnMushi2* this, PlayState* play) {
    Actor* thisx = &this->actor;

    Math_SmoothStepToF(&thisx->speed, this->unk_35C, 0.1f, 0.5f, 0.0f);
    func_80A68ED8(this);
    func_80A697C4(this, play);

    if (this->unk_30C & 0x40) {
        func_80A69388(this);
    }

    this->skelAnime.playSpeed = (Rand_ZeroOne() * 0.8f) + (thisx->speed * 1.2f);
    this->skelAnime.playSpeed = CLAMP(this->skelAnime.playSpeed, 0.0f, 1.9f);

    if ((this->unk_36A <= 0) || func_80A68BA0(this)) {
        func_80A6B078(this);
    } else if (!(this->unk_30C & 2)) {
        func_80A6A984(this);
    } else if (this->unk_30C & 0x20) {
        func_80A6AAA4(this);
    } else if (this->unk_368 <= 0) {
        func_80A6A508(&this->actor);
    }
}

void func_80A6A984(EnMushi2* this) {
    if (Rand_Next() & 1) {
        this->unk_370 = 0x800;
    } else {
        this->unk_370 = -0x800;
    }
    this->skelAnime.playSpeed = 1.2f;
    this->unk_368 = 17;
    this->actionFunc = func_80A6A9E4;
}

void func_80A6A9E4(EnMushi2* this, PlayState* play) {
    func_80A68F9C(this, this->unk_370);
    func_80A69388(this);
    if ((this->unk_36A <= 0) || func_80A68BA0(this)) {
        func_80A6B078(this);
    } else if (this->unk_368 <= 0) {
        func_80A697C4(this, play);
        if (this->unk_30C & 0x40) {
            func_80A69388(this);
        }

        if (this->unk_30C & 2) {
            func_80A6A508(&this->actor);
        } else {
            func_80A6B078(this);
        }
    }
}

void func_80A6AAA4(EnMushi2* this) {
    this->actor.velocity.y = -1.2f;
    this->actor.terminalVelocity = -1.2f;
    this->actor.gravity = -0.5f;
    this->unk_368 = Rand_S16Offset(120, 50);
    this->unk_36E = 0;
    this->unk_370 = 0;
    this->actionFunc = func_80A6AB08;
}

void func_80A6AB08(EnMushi2* this, PlayState* play) {
    s32 pad;
    s16 temp;

    if (this->unk_368 > 80) {
        Math_StepToF(&this->actor.speed, 0.6f, 0.08f);
    } else {
        Math_StepToF(&this->actor.speed, 0.0f, 0.02f);
    }

    Actor_MoveWithGravity(&this->actor);
    func_80A69424(this, play);
    Math_StepToF(&this->actor.world.pos.y, this->actor.world.pos.y + this->actor.depthInWater, 2.8f);
    this->skelAnime.playSpeed = this->unk_368 * 0.018f;
    this->skelAnime.playSpeed = CLAMP(this->skelAnime.playSpeed, 0.1f, 1.9f);

    if (this->unk_368 > 80) {
        this->unk_36E += Rand_S16Offset(-50, 100);
        this->unk_370 += Rand_S16Offset(-300, 600);
    }

    temp = this->skelAnime.playSpeed * 200.0f;
    this->unk_36E = CLAMP(this->unk_36E, -temp, temp);

    temp = this->skelAnime.playSpeed * 1000.0f;
    this->unk_370 = CLAMP(this->unk_370, -temp, temp);

    this->actor.world.rot.y += this->unk_36E;
    this->actor.shape.rot.y += this->unk_370;
    Math_ScaledStepToS(&this->actor.world.rot.x, 0, 0xBB8);
    this->actor.shape.rot.x = this->actor.world.rot.x;
    Math_ScaledStepToS(&this->actor.world.rot.z, 0, 0xBB8);
    this->actor.shape.rot.z = this->actor.world.rot.z;

    if ((this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) && (Rand_ZeroOne() < 0.03f)) {
        Vec3f sp3C;

        sp3C.x = this->actor.world.pos.x;
        sp3C.y = this->actor.world.pos.y + this->actor.depthInWater;
        sp3C.z = this->actor.world.pos.z;
        EffectSsGRipple_Spawn(play, &sp3C, 40, 200, 4);
    }

    if ((this->unk_368 <= 0) || (this->unk_36A <= 0) || func_80A68BA0(this)) {
        func_80A6A094(this);
        func_80A6A0D8(this);
        func_80A6AE14(this);
    } else if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
        func_80A6A794(this);
    }
}

void func_80A6AE14(EnMushi2* this) {
    this->unk_30C &= ~1;
    this->unk_368 = 100;
    this->actor.velocity.y = 0.0f;
    this->actor.terminalVelocity = -0.8f;
    this->actor.gravity = -0.04f;
    func_80A68B6C(this);
    this->actionFunc = func_80A6AE7C;
}

void func_80A6AE7C(EnMushi2* this, PlayState* play) {
    f32 temp_f2;

    this->actor.shape.rot.x -= 0x1F4;
    this->actor.shape.rot.y += 0xC8;
    this->actor.speed += (Rand_ZeroOne() - 0.5f) * 0.16f;
    this->actor.speed *= 0.9f;
    this->actor.world.rot.y += TRUNCF_BINANG((Rand_ZeroOne() - 0.5f) * 2000.0f);
    this->actor.gravity = -0.04f - (Rand_ZeroOne() * 0.02f);
    this->actor.velocity.y *= 0.95f;
    Actor_MoveWithGravity(&this->actor);
    func_80A69424(this, play);
    temp_f2 = this->actor.scale.x - (1.0f / 20000.0f);
    Actor_SetScale(&this->actor, CLAMP_MIN(temp_f2, 0.001f));
    if ((this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) && (this->actor.depthInWater > 5.0f) &&
        (this->actor.depthInWater < 30.0f) && ((s32)(Rand_Next() & 0x1FF) < this->unk_368)) {
        EffectSsBubble_Spawn(play, &this->actor.world.pos, -5.0f, 5.0f, 5.0f,
                             ((Rand_ZeroOne() * 4.0f) + 2.0f) * this->actor.scale.x);
    }

    if (this->unk_368 <= 0) {
        Actor_Kill(&this->actor);
    }
}

void func_80A6B078(EnMushi2* this) {
    this->unk_30C &= ~1;
    this->unk_368 = 50;
    this->skelAnime.playSpeed = 1.9f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALTURA_BOUND);
    func_80A68B6C(this);
    this->actionFunc = func_80A6B0D8;
}

void func_80A6B0D8(EnMushi2* this, PlayState* play) {
    s32 pad[2];
    f32 temp_f2;

    Math_SmoothStepToF(&this->actor.speed, 0.0f, 0.4f, 1.2f, 0.0f);
    temp_f2 = this->actor.scale.x - 0.0002f;
    Actor_SetScale(&this->actor, CLAMP_MIN(temp_f2, 0.001f));

    if (this->unk_368 > 20) {
        func_80A690C4(this, 0x160);
    }

    if (this->unk_30C & 0x40) {
        func_80A69388(this);
    }

    if ((play->gameplayFrames % 2) != 0) {
        temp_f2 = 0.6f;
    } else {
        temp_f2 = -0.6f;
    }

    this->actor.velocity.x =
        (this->actor.speed * this->unk_328.x) + (-0.01f * this->unk_31C.x) + (this->unk_310.x * temp_f2);
    this->actor.velocity.y =
        (this->actor.speed * this->unk_328.y) + (-0.01f * this->unk_31C.y) + (this->unk_310.y * temp_f2);
    this->actor.velocity.z =
        (this->actor.speed * this->unk_328.z) + (-0.01f * this->unk_31C.z) + (this->unk_310.z * temp_f2);

    if ((this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) && (this->unk_368 > 20) && (Rand_ZeroOne() < 0.15f)) {
        Vec3f sp48;
        s32 sp44 = 0;

        if (this->poly != NULL) {
            FloorType floorType = SurfaceType_GetFloorType(&play->colCtx, this->poly, this->polyBgId);

            if ((floorType == FLOOR_TYPE_5) || (floorType == FLOOR_TYPE_14) || (floorType == FLOOR_TYPE_15)) {
                sp44 = 1;
            }
        }
        sp48.x = (this->unk_328.x * -0.6f) + (this->unk_31C.x * 0.1f);
        sp48.y = (this->unk_328.y * -0.6f) + (this->unk_31C.y * 0.1f);
        sp48.z = (this->unk_328.z * -0.6f) + (this->unk_31C.z * 0.1f);
        func_800B0E48(play, &this->actor.world.pos, &sp48, &gZeroVec3f, &D_80A6B984[sp44], &D_80A6B98C[sp44],
                      (Rand_ZeroOne() * 5.0f) + 8.0f, (Rand_ZeroOne() * 5.0f) + 8.0f);
    }

    if (this->unk_368 <= 0) {
        if (this->unk_30C & 0x80) {
            func_80A6A058(this);
            func_80A6A0D8(this);
        } else {
            func_80A6A094(this);
            func_80A6A0D8(this);
        }
        Actor_Kill(&this->actor);
    }
}

void EnMushi2_Update(Actor* thisx, PlayState* play) {
    EnMushi2* this = (EnMushi2*)thisx;
    s32 pad;
    f32 sp4C;
    f32 phi_f0;
    s32 temp;

    if ((this->unk_34C != NULL) && (this->unk_34C->dyna.actor.update == NULL)) {
        this->unk_34C = NULL;
    }

    if (Actor_HasParent(&this->actor, play)) {
        func_80A6A094(this);
        func_80A6A0D8(this);
        Actor_Kill(&this->actor);
        return;
    }

    if (this->unk_368 > 0) {
        this->unk_368--;
    }

    if (this->unk_36A > 0) {
        this->unk_36A--;
    }

    if ((this->unk_34C != NULL) && ((this->actionFunc == func_80A6A5C0) || (this->actionFunc == func_80A6A824)) &&
        (this->unk_354 < SQ(3.0f))) {
        this->unk_30C |= 0x80;
        func_80A6B078(this);
    }

    if (this->unk_354 < SQ(70.0f)) {
        phi_f0 = 0.017f;
    } else {
        phi_f0 = 0.025f;
    }
    this->unk_358 += phi_f0;

    if (this->unk_358 > 1.0f) {
        this->unk_358 = 1.0f;
    }

    temp = this->polyBgId;
    if ((temp != BGCHECK_SCENE) &&
        ((this->actionFunc == func_80A6A5C0) || (this->actionFunc == func_80A6A824) ||
         (this->actionFunc == func_80A6A9E4) || (this->actionFunc == func_80A6B0D8)) &&
        DynaPolyActor_TransformCarriedActor(&play->colCtx, temp, &this->actor)) {
        func_80A68F24(this);
    }

    this->actionFunc(this, play);

    if (this->actor.update != NULL) {
        if ((this->actionFunc == func_80A6AB08) || (this->actionFunc == func_80A6AE7C)) {
            this->unk_30C |= 0x200;
        } else {
            this->unk_30C &= ~0x200;
        }

        if ((this->actionFunc == func_80A6A5C0) || (this->actionFunc == func_80A6A824) ||
            (this->actionFunc == func_80A6A9E4)) {
            func_80A68BC8(this);
        }

        if (this->unk_34C != NULL) {
            sp4C = 0.0f;

            this->unk_354 = Math3D_Vec3fDistSq(&this->actor.world.pos, &this->unk_34C->dyna.actor.world.pos);
            if (this->unk_354 < this->unk_350) {
                f32 dist = Math3D_DistPlaneToPos(COLPOLY_GET_NORMAL(this->unk_34C->dyna.actor.floorPoly->normal.x),
                                                 COLPOLY_GET_NORMAL(this->unk_34C->dyna.actor.floorPoly->normal.y),
                                                 COLPOLY_GET_NORMAL(this->unk_34C->dyna.actor.floorPoly->normal.z),
                                                 this->unk_34C->dyna.actor.floorPoly->dist, &this->actor.world.pos);

                if (fabsf(dist) < 3.0f) {
                    sp4C = 1.9f;
                }
            }
            Math_StepToF(&this->unk_348, sp4C, 0.7f);
        }

        SkelAnime_Update(&this->skelAnime);

        if (this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) {
            if ((this->actionFunc != func_80A6AE7C) && (this->actionFunc != func_80A6B0D8) &&
                ((this->actionFunc != func_80A6A36C) || (this->unk_36A < 0xDD)) &&
                (((this->actionFunc != func_80A6A5C0) && (this->actionFunc != func_80A6A824) &&
                  (this->actionFunc != func_80A6A9E4)) ||
                 !(this->unk_358 > 0.999f) || !(this->unk_354 < SQ(20.0f)))) {
                s32 phi_v0 = true;

                if (this->unk_34C == NULL) {
                    this->collider.base.ocFlags1 |= OC1_TYPE_PLAYER;
                } else {
                    this->collider.base.ocFlags1 &= ~OC1_TYPE_PLAYER;
                    if ((this->unk_358 > 0.999f) && (this->unk_354 < SQ(20.0f))) {
                        phi_v0 = false;
                    }
                }

                if (phi_v0) {
                    ColliderJntSphElement* jntSphElem = &this->collider.elements[0];

                    jntSphElem->dim.worldSphere.center.x = this->actor.world.pos.x;
                    jntSphElem->dim.worldSphere.center.y = this->actor.world.pos.y;
                    jntSphElem->dim.worldSphere.center.z = this->actor.world.pos.z;
                    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
                }
            }
        }

        if ((this->unk_30C & 1) && func_80A68DD4(this, play)) {
            Actor_OfferGetItem(&this->actor, play, GI_MAX, 60.0f, 30.0f);
        }
    }
}

void EnMushi2_Draw(Actor* thisx, PlayState* play) {
    EnMushi2* this = (EnMushi2*)thisx;

    func_80A687A0(this);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, NULL);
}
