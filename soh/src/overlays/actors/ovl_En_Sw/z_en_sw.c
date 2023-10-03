#include "z_en_sw.h"
#include "objects/object_st/object_st.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_WHILE_CULLED)

void EnSw_Init(Actor* thisx, PlayState* play);
void EnSw_Destroy(Actor* thisx, PlayState* play);
void EnSw_Update(Actor* thisx, PlayState* play);
void EnSw_Draw(Actor* thisx, PlayState* play);
s32 func_80B0DFFC(EnSw* this, PlayState* play);
void func_80B0D364(EnSw* this, PlayState* play);
void func_80B0E5E0(EnSw* this, PlayState* play);
void func_80B0D590(EnSw* this, PlayState* play);
void func_80B0E90C(EnSw* this, PlayState* play);
void func_80B0E9BC(EnSw* this, PlayState* play);
void func_80B0E728(EnSw* this, PlayState* play);
void func_80B0DC7C(EnSw* this, PlayState* play);
s32 func_80B0C0CC(EnSw* this, PlayState* play, s32);
void func_80B0D3AC(EnSw* this, PlayState* play);
void func_80B0DB00(EnSw* this, PlayState* play);
void func_80B0D878(EnSw* this, PlayState* play);

const ActorInit En_Sw_InitVars = {
    ACTOR_EN_SW,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ST,
    sizeof(EnSw),
    (ActorFunc)EnSw_Init,
    (ActorFunc)EnSw_Destroy,
    (ActorFunc)EnSw_Update,
    (ActorFunc)EnSw_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphItemsInit[1] = {
    {
        { ELEMTYPE_UNK0, { 0xFFCFFFFF, 0x00, 0x08 }, { 0xFFC3FFFE, 0x00, 0x00 }, 0x01, 0x05, 0x01 },
        { 2, { { 0, -300, 0 }, 21 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    { COLTYPE_HIT6, 0x11, 0x09, 0x39, 0x10, COLSHAPE_JNTSPH },
    1,
    sJntSphItemsInit,
};

static CollisionCheckInfoInit2 D_80B0F074 = { 1, 2, 25, 25, MASS_IMMOVABLE };

typedef enum {
    /* 0 */ ENSW_ANIM_0,
    /* 1 */ ENSW_ANIM_1,
    /* 2 */ ENSW_ANIM_2,
    /* 3 */ ENSW_ANIM_3
} EnSwAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &object_st_Anim_000304, 1.0f, 0.0f, -1.0f, 0x01, 0.0f },
    { &object_st_Anim_000304, 1.0f, 0.0f, -1.0f, 0x01, -8.0f },
    { &object_st_Anim_0055A8, 1.0f, 0.0f, -1.0f, 0x01, -8.0f },
    { &object_st_Anim_005B98, 1.0f, 0.0f, -1.0f, 0x01, -8.0f },
};

char D_80B0F630[0x80]; // unused

void EnSw_CrossProduct(Vec3f* a, Vec3f* b, Vec3f* dst) {
    dst->x = (a->y * b->z) - (a->z * b->y);
    dst->y = (a->z * b->x) - (a->x * b->z);
    dst->z = (a->x * b->y) - (a->y * b->x);
}

s32 func_80B0BE20(EnSw* this, CollisionPoly* poly) {
    Vec3f sp44;
    Vec3f sp38;
    f32 sp34;
    f32 temp_f0;
    s32 pad;

    this->actor.floorPoly = poly;
    sp44.x = COLPOLY_GET_NORMAL(poly->normal.x);
    sp44.y = COLPOLY_GET_NORMAL(poly->normal.y);
    sp44.z = COLPOLY_GET_NORMAL(poly->normal.z);
    sp34 = Math_FAcosF(DOTXYZ(sp44, this->unk_364));
    EnSw_CrossProduct(&this->unk_364, &sp44, &sp38);
    Matrix_RotateAxis(sp34, &sp38, MTXMODE_NEW);
    Matrix_MultVec3f(&this->unk_370, &sp38);
    this->unk_370 = sp38;
    EnSw_CrossProduct(&this->unk_370, &sp44, &this->unk_37C);
    temp_f0 = Math3D_Vec3fMagnitude(&this->unk_37C);
    if (temp_f0 < 0.001f) {
        return 0;
    }
    this->unk_37C.x = this->unk_37C.x * (1.0f / temp_f0);
    this->unk_37C.y = this->unk_37C.y * (1.0f / temp_f0);
    this->unk_37C.z = this->unk_37C.z * (1.0f / temp_f0);
    this->unk_364 = sp44;
    this->unk_3D8.xx = this->unk_370.x;
    this->unk_3D8.yx = this->unk_370.y;
    this->unk_3D8.zx = this->unk_370.z;
    this->unk_3D8.wx = 0.0f;
    this->unk_3D8.xy = this->unk_364.x;
    this->unk_3D8.yy = this->unk_364.y;
    this->unk_3D8.zy = this->unk_364.z;
    this->unk_3D8.wy = 0.0f;
    this->unk_3D8.xz = this->unk_37C.x;
    this->unk_3D8.yz = this->unk_37C.y;
    this->unk_3D8.zz = this->unk_37C.z;
    this->unk_3D8.wz = 0.0f;
    this->unk_3D8.xw = 0.0f;
    this->unk_3D8.yw = 0.0f;
    this->unk_3D8.zw = 0.0f;
    this->unk_3D8.ww = 1.0f;
    Matrix_MtxFToYXZRotS(&this->unk_3D8, &this->actor.world.rot, 0);
    //! @bug: Does not return.
}

CollisionPoly* func_80B0C020(PlayState* play, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, s32* arg4) {
    CollisionPoly* sp3C;
    s32 pad;

    if (!BgCheck_EntityLineTest1(&play->colCtx, arg1, arg2, arg3, &sp3C, true, true, true, false, arg4)) {
        return NULL;
    }

    if (func_80041DB8(&play->colCtx, sp3C, *arg4) & 0x30) {
        return NULL;
    }

    if (SurfaceType_IsIgnoredByProjectiles(&play->colCtx, sp3C, *arg4)) {
        return NULL;
    }

    return sp3C;
}

s32 func_80B0C0CC(EnSw* this, PlayState* play, s32 arg2) {
    CollisionPoly* temp_v0_2;
    CollisionPoly* temp_s1;
    Vec3f sp9C;
    Vec3f sp90;
    Vec3f sp84;
    Vec3f sp78;
    s32 pad;
    s32 sp70;
    s32 sp6C;
    s32 phi_s1;
    s32 sp64;

    sp64 = 0;
    this->unk_42C = 1;
    sp84 = sp78 = this->actor.world.pos;
    sp84.x += this->unk_364.x * 18.0f;
    sp84.y += this->unk_364.y * 18.0f;
    sp84.z += this->unk_364.z * 18.0f;
    sp78.x -= this->unk_364.x * 18.0f;
    sp78.y -= this->unk_364.y * 18.0f;
    sp78.z -= this->unk_364.z * 18.0f;
    temp_s1 = func_80B0C020(play, &sp84, &sp78, &sp90, &sp70);

    if ((temp_s1 != NULL) && (this->unk_360 == 0)) {
        sp78.x = sp84.x + (this->unk_37C.x * 24);
        sp78.y = sp84.y + (this->unk_37C.y * 24);
        sp78.z = sp84.z + (this->unk_37C.z * 24);
        temp_v0_2 = func_80B0C020(play, &sp84, &sp78, &sp9C, &sp6C);
        if (temp_v0_2 != NULL) {
            if (arg2 == 1) {
                func_80B0BE20(this, temp_v0_2);
                this->actor.world.pos = sp9C;
                this->actor.floorBgId = sp6C;
            }
        } else {
            if (this->actor.floorPoly != temp_s1) {
                func_80B0BE20(this, temp_s1);
            }
            this->actor.world.pos = sp90;
            this->actor.floorBgId = sp70;
        }
        sp64 = 1;
    } else {
        sp84 = sp78;
        for (phi_s1 = 0; phi_s1 < 3; phi_s1++) {
            if (phi_s1 == 0) {
                sp78.x = sp84.x - (this->unk_37C.x * 24.0f);
                sp78.y = sp84.y - (this->unk_37C.y * 24.0f);
                sp78.z = sp84.z - (this->unk_37C.z * 24.0f);
            } else if (phi_s1 == 1) {
                sp78.x = sp84.x + (this->unk_370.x * 24.0f);
                sp78.y = sp84.y + (this->unk_370.y * 24.0f);
                sp78.z = sp84.z + (this->unk_370.z * 24.0f);
            } else {
                sp78.x = sp84.x - (this->unk_370.x * 24.0f);
                sp78.y = sp84.y - (this->unk_370.y * 24.0f);
                sp78.z = sp84.z - (this->unk_370.z * 24.0f);
            }
            temp_v0_2 = func_80B0C020(play, &sp84, &sp78, &sp9C, &sp6C);
            if (temp_v0_2 != NULL) {
                if (arg2 == 1) {
                    func_80B0BE20(this, temp_v0_2);
                    this->actor.world.pos = sp9C;
                    this->actor.floorBgId = sp6C;
                }
                sp64 = 1;
                break;
            }
        }
    }

    Math_SmoothStepToS(&this->actor.shape.rot.x, this->actor.world.rot.x, 8, 0xFA0, 1);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 8, 0xFA0, 1);
    Math_SmoothStepToS(&this->actor.shape.rot.z, this->actor.world.rot.z, 8, 0xFA0, 1);

    return sp64;
}

// Presumably, due to the removal of object dependency, there is a race condition where
// the GS on the Kak construction site spawns to early and fails to detect the
// construction site dyna poly. This custom action func rechecks moving the GS
// to the nearest poly one frame after init. Further explanation available:
// https://github.com/HarbourMasters/Shipwright/issues/2310#issuecomment-1492829517
void EnSw_MoveGoldLater(EnSw* this, PlayState* play) {
    func_80B0C0CC(this, play, 1);
    this->actionFunc = func_80B0D590;
}

void EnSw_Init(Actor* thisx, PlayState* play) {
    EnSw* this = (EnSw*)thisx;
    s32 phi_v0;
    Vec3f sp4C = { 0.0f, 0.0f, 0.0f };
    s32 pad;

    if (thisx->params & 0x8000) {
        phi_v0 = (((thisx->params - 0x8000) & 0xE000) >> 0xD) + 1;
        thisx->params = (thisx->params & 0x1FFF) | (phi_v0 << 0xD);
    }

    if (((thisx->params & 0xE000) >> 0xD) > 0) {
        phi_v0 = ((thisx->params & 0x1F00) >> 8) - 1;
        thisx->params = (thisx->params & 0xE0FF) | (phi_v0 << 8);
    }

    // Check to see if this gold skull token has already been retrieved.
    if (GET_GS_FLAGS((thisx->params & 0x1F00) >> 8) & (thisx->params & 0xFF)) {
        Actor_Kill(&this->actor);
        return;
    }

    SkelAnime_Init(play, &this->skelAnime, &object_st_Skel_005298, NULL, this->jointTable, this->morphTable, 30);
    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENSW_ANIM_0);
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 0.0f);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->sphs);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0xE), &D_80B0F074);
    this->actor.scale.x = 0.02f;

    if (((thisx->params & 0xE000) >> 0xD) == 0) {
        this->actor.world.rot.x = 0;
        this->actor.world.rot.z = 0;
        thisx->shape.rot = this->actor.world.rot;
        this->unk_484.y = this->actor.world.pos.y;
        this->unk_484.x = this->actor.world.pos.x + (Math_SinS(this->actor.world.rot.y) * -60.0f);
        this->unk_484.z = this->actor.world.pos.z + (Math_CosS(this->actor.world.rot.y) * -60.0f);
        func_80B0DFFC(this, play);
        this->actor.home.pos = this->actor.world.pos;
    } else {
        this->unk_370.x = Math_SinS(thisx->shape.rot.y + 0x4000);
        this->unk_370.y = 0.0f;
        this->unk_370.z = Math_CosS(thisx->shape.rot.y + 0x4000);
        this->unk_364.x = 0.0f;
        this->unk_364.y = 1.0f;
        this->unk_364.z = 0.0f;
        this->unk_37C.x = Math_SinS(thisx->shape.rot.y);
        this->unk_37C.y = 0.0f;
        this->unk_37C.z = Math_CosS(thisx->shape.rot.y);
        func_80B0C0CC(this, play, 1);
    }

    if (((thisx->params & 0xE000) >> 0xD) >= 3) {
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }

    switch ((thisx->params & 0xE000) >> 0xD) {
        case 3:
        case 4:
            this->unk_360 = 1;
            this->actor.velocity.y = 8.0f;
            this->actor.speedXZ = 4.0f;
            this->actor.gravity = -1.0f;
        case 2:
            this->actor.scale.x = 0.0f;
        case 1:
            this->collider.elements[0].info.toucher.damage *= 2;
            this->actor.naviEnemyId = 0x20;
            this->actor.colChkInfo.health *= 2;
            this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
            break;
        default:
            Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
            this->actor.naviEnemyId = 0x1F;
            break;
    }

    this->unk_38E = Rand_S16Offset(0xF, 0x1E);
    Actor_SetScale(&this->actor, this->actor.scale.x);
    this->actor.home.pos = this->actor.world.pos;
    thisx->shape.rot = this->actor.world.rot;

    if (((thisx->params & 0xE000) >> 0xD) >= 3) {
        this->unk_38C = 0x28;
        this->unk_394 = 1;
        this->actionFunc = func_80B0D364;
    } else if (((thisx->params & 0xE000) >> 0xD) == 0) {
        this->actionFunc = func_80B0E5E0;
    } else {
        this->actionFunc = func_80B0D590;
    }

    // If a normal GS failed to get attached to a poly during init
    // try once more on the next frame via a custom action func
    if ((((thisx->params & 0xE000) >> 0xD) == 1 ||
         ((thisx->params & 0xE000) >> 0xD) == 2) &&
        this->actor.floorPoly == NULL) {
        this->actionFunc = EnSw_MoveGoldLater;
    }
}

void EnSw_Destroy(Actor* thisx, PlayState* play) {
    EnSw* this = (EnSw*)thisx;

    Collider_DestroyJntSph(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

s32 func_80B0C9F0(EnSw* this, PlayState* play) {
    s32 phi_v1 = false;

    if (this->actor.xyzDistToPlayerSq < SQ(400.0f) && ((this->actor.params & 0xE000) >> 0xD) == 0 &&
        play->actorCtx.unk_02 != 0) {

        this->actor.colChkInfo.damage = this->actor.colChkInfo.health;
        phi_v1 = true;
    }

    if (this->unk_392 == 0) {
        if ((this->collider.base.acFlags & 2) || phi_v1) {
            this->collider.base.acFlags &= ~2;
            this->unk_392 = 0x10;
            Actor_SetColorFilter(&this->actor, 0x4000, 0xC8, 0, this->unk_392);
            if (Actor_ApplyDamage(&this->actor) != 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_DAMAGE);
                return true;
            }
            Enemy_StartFinishingBlow(play, &this->actor);
            if (((this->actor.params & 0xE000) >> 0xD) != 0) {
                if (CVarGetInteger("gGsCutscene", 0)) {
                    OnePointCutscene_Init(play, 2200, 90, &this->actor, MAIN_CAM);
                }
                this->skelAnime.playSpeed = 8.0f;
                if ((play->state.frames & 1) == 0) {
                    this->unk_420 = 0.1f;
                } else {
                    this->unk_420 = -0.1f;
                }
                this->unk_394 = 0xA;
                this->unk_38A = 1;
                this->unk_420 *= 4.0f;
                this->actionFunc = func_80B0D878;
            } else {
                this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
                this->actor.shape.shadowAlpha = 0xFF;
                this->unk_38A = 2;
                this->actor.shape.shadowScale = 16.0f;
                this->actor.gravity = -1.0f;
                this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
                this->actionFunc = func_80B0DB00;
            }
            
            GameInteractor_ExecuteOnEnemyDefeat(&this->actor);

            Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALWALL_DEAD);
            return true;
        }
    }

    if ((this->unk_390 == 0) && (this->collider.base.atFlags & 2)) {
        this->unk_390 = 30;
    }

    return false;
}

void func_80B0CBE8(EnSw* this, PlayState* play) {
    if ((((this->actor.params & 0xE000) >> 0xD) > 0) && (this->actionFunc != func_80B0D590)) {
        if (this->unk_392 != 0) {
            this->unk_392--;
        }
    } else {
        if ((DECR(this->unk_390) == 0) && (this->actor.colChkInfo.health != 0)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }

        if ((DECR(this->unk_392) == 0) && (this->actor.colChkInfo.health != 0)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }

        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 func_80B0CCF4(EnSw* this, f32* arg1) {
    CollisionPoly* temp_v1;
    f32 temp_f0;
    Vec3f sp6C;
    MtxF sp2C;

    if (this->actor.floorPoly == NULL) {
        return false;
    }

    temp_v1 = this->actor.floorPoly;
    sp6C.x = COLPOLY_GET_NORMAL(temp_v1->normal.x);
    sp6C.y = COLPOLY_GET_NORMAL(temp_v1->normal.y);
    sp6C.z = COLPOLY_GET_NORMAL(temp_v1->normal.z);
    Matrix_RotateAxis(*arg1, &sp6C, MTXMODE_NEW);
    Matrix_MultVec3f(&this->unk_370, &sp6C);
    this->unk_370 = sp6C;
    EnSw_CrossProduct(&this->unk_370, &this->unk_364, &this->unk_37C);
    temp_f0 = Math3D_Vec3fMagnitude(&this->unk_37C);
    if (temp_f0 < 0.001f) {
        return false;
    }
    temp_f0 = 1.0f / temp_f0;
    this->unk_37C.x *= temp_f0;
    this->unk_37C.y *= temp_f0;
    this->unk_37C.z *= temp_f0;
    sp2C.xx = this->unk_370.x;
    sp2C.yx = this->unk_370.y;
    sp2C.zx = this->unk_370.z;
    sp2C.wx = 0.0f;
    sp2C.xy = this->unk_364.x;
    sp2C.yy = this->unk_364.y;
    sp2C.zy = this->unk_364.z;
    sp2C.wy = 0.0f;
    sp2C.xz = this->unk_37C.x;
    sp2C.yz = this->unk_37C.y;
    sp2C.zz = this->unk_37C.z;
    sp2C.wz = 0.0f;
    sp2C.xw = 0.0f;
    sp2C.yw = 0.0f;
    sp2C.zw = 0.0f;
    sp2C.ww = 1.0f;
    Matrix_MtxFToYXZRotS(&sp2C, &this->actor.world.rot, 0);
    return true;
}

void func_80B0CEA8(EnSw* this, PlayState* play) {
    if (!(this->actor.scale.x < 0.0139999995f)) {
        Camera* activeCam = GET_ACTIVE_CAM(play);

        if (!(Math_Vec3f_DistXYZ(&this->actor.world.pos, &activeCam->eye) >= 380.0f)) {
            Audio_PlayActorSound2(&this->actor, ((this->actor.params & 0xE000) >> 0xD) > 0 ? NA_SE_EN_STALGOLD_ROLL
                                                                                           : NA_SE_EN_STALWALL_ROLL);
        }
    }
}

void func_80B0CF44(EnSw* this, PlayState* play, s32 cnt) {
    Color_RGBA8 primColor = { 80, 80, 50, 255 };
    Color_RGBA8 envColor = { 100, 100, 80, 0 };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos;
    s16 angle = (Rand_ZeroOne() - 0.5f) * 65536.0f;
    s32 i;

    for (i = cnt; i >= 0; i--, angle += (s16)(0x10000 / cnt)) {
        accel.x = (Rand_ZeroOne() - 0.5f) * 2.0f;
        accel.z = (Rand_ZeroOne() - 0.5f) * 2.0f;
        pos.x = this->actor.world.pos.x + (Math_SinS(angle) * 2.0f);
        pos.y = this->actor.world.pos.y;
        pos.z = this->actor.world.pos.z + (Math_CosS(angle) * 2.0f);
        func_8002836C(play, &pos, &velocity, &accel, &primColor, &envColor, 20, 30, 12);
    }
}

void func_80B0D14C(EnSw* this, PlayState* play, s32 cnt) {
    Color_RGBA8 primColor = { 80, 80, 50, 255 };
    Color_RGBA8 envColor = { 100, 100, 80, 0 };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos;
    s16 angle = (Rand_ZeroOne() - 0.5f) * 65536.0f;
    s32 i;

    for (i = cnt; i >= 0; i--, angle += (s16)(0x10000 / cnt)) {
        accel.x = (Rand_ZeroOne() - 0.5f) * 2.0f;
        accel.z = (Rand_ZeroOne() - 0.5f) * 2.0f;
        pos.x = this->actor.world.pos.x + (Math_SinS(angle) * 14.0f);
        pos.y = this->actor.world.pos.y;
        pos.z = this->actor.world.pos.z + (Math_CosS(angle) * 14.0f);
        func_8002836C(play, &pos, &velocity, &accel, &primColor, &envColor, 20, 40, 10);
    }
}

void func_80B0D364(EnSw* this, PlayState* play) {
    if (((this->actor.params & 0xE000) >> 0xD) == 4) {
        this->unk_38C = 0;
        this->actionFunc = func_80B0D3AC;
    } else {
        this->unk_38C = 10;
        this->actionFunc = func_80B0D3AC;
    }
}

void func_80B0D3AC(EnSw* this, PlayState* play) {
    if (this->unk_38C != 0) {
        if ((this->unk_38C & 4) != 0) {
            func_80B0CF44(this, play, 5);
        }
        this->unk_38C--;
        if (this->unk_38C == 0) {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALGOLD_UP_CRY);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_DODO_M_UP);
        } else {
            return;
        }
    }

    Math_ApproachF(&this->actor.scale.x, 0.02f, 0.2f, 0.01f);
    Actor_SetScale(&this->actor, this->actor.scale.x);
    this->actor.world.pos.x += this->unk_364.x * this->actor.velocity.y;
    this->actor.world.pos.y += this->unk_364.y * this->actor.velocity.y;
    this->actor.world.pos.z += this->unk_364.z * this->actor.velocity.y;
    this->actor.world.pos.x += this->unk_37C.x * this->actor.speedXZ;
    this->actor.world.pos.y += this->unk_37C.y * this->actor.speedXZ;
    this->actor.world.pos.z += this->unk_37C.z * this->actor.speedXZ;
    this->actor.velocity.y += this->actor.gravity;
    this->actor.velocity.y = CLAMP_MIN(this->actor.velocity.y, this->actor.minVelocityY);

    if (this->actor.velocity.y < 0.0f) {
        this->unk_360 = 0;
    }

    if (func_80B0C0CC(this, play, 1) == 1) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        func_80B0D14C(this, play, 8);
        this->actor.scale.x = 0.02f;
        Actor_SetScale(&this->actor, 0.02f);
        this->actionFunc = func_80B0D590;
        this->actor.velocity.y = 0.0f;
        this->actor.speedXZ = 0.0f;
        this->actor.gravity = 0.0f;
    }
}

void func_80B0D590(EnSw* this, PlayState* play) {
    f32 sp2C;

    if (((this->actor.params & 0xE000) >> 0xD) == 2) {
        if (this->actor.scale.x < 0.0139999995f) {
            this->collider.elements[0].info.toucherFlags = 0;
            this->collider.elements[0].info.bumperFlags = 0;
            this->collider.elements[0].info.ocElemFlags = 0;
        }

        if (this->actor.scale.x >= 0.0139999995f) {
            this->collider.elements[0].info.toucherFlags = 1;
            this->collider.elements[0].info.bumperFlags = 1;
            this->collider.elements[0].info.ocElemFlags = 1;
        }

        Math_ApproachF(&this->actor.scale.x, !IS_DAY || CVarGetInteger("gNightGSAlwaysSpawn", 0) ? 0.02f : 0.0f, 0.2f, 0.01f);
        Actor_SetScale(&this->actor, this->actor.scale.x);
    }

    if (this->unk_38E != 0) {
        this->unk_38E--;
        if (this->unk_38E == 0) {
            func_80B0CEA8(this, play);
            this->unk_420 = ((play->state.frames % 2) == 0) ? 0.1f : -0.1f;
            this->unk_38A = 1;
            this->unk_38C = Rand_S16Offset(30, 60);
            if (((this->actor.params & 0xE000) >> 0xD) != 0) {
                this->unk_38C *= 2;
                this->unk_420 *= 2.0f;
            }
        }
    } else {
        this->unk_38C--;
        if (this->unk_38C == 0) {
            this->unk_38E = Rand_S16Offset(15, 30);
            this->unk_38A = 0;
            this->skelAnime.playSpeed = 0.0f;
            if (((this->actor.params & 0xE000) >> 0xD) != 0) {
                this->unk_38E /= 2;
            }
        } else if (this->unk_38A != 0) {
            this->unk_38A--;
            this->skelAnime.playSpeed = (this->unk_38A == 0) ? 4.0f : 0.0f;

            if (this->skelAnime.playSpeed > 0.0f) {
                func_80B0CEA8(this, play);
            }
            if (((this->actor.params & 0xE000) >> 0xD) != 0) {
                this->skelAnime.playSpeed *= 2.0f;
            }
        } else {
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame) == 1) {
                this->unk_38A = 2;
            }
            sp2C = 32768.0f / this->skelAnime.endFrame;
            sp2C *= this->skelAnime.curFrame;
            sp2C = Math_SinS(sp2C) * this->unk_420;
            func_80B0CCF4(this, &sp2C);
            this->actor.shape.rot = this->actor.world.rot;
        }
    }
}

void func_80B0D878(EnSw* this, PlayState* play) {
    Actor* temp_v0;
    Vec3f pos;
    Vec3f velAndAccel = { 0.0f, 0.5f, 0.0f };
    f32 x;
    f32 y;
    f32 z;

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame) == 1) {
        func_80B0CEA8(this, play);
    }

    func_80B0CCF4(this, &this->unk_420);
    this->actor.shape.rot = this->actor.world.rot;

    if ((this->unk_394 == 0) && (this->unk_392 == 0)) {
        Audio_PlaySoundGeneral(NA_SE_SY_KINSTA_MARK_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        x = (this->unk_364.x * 10.0f);
        y = (this->unk_364.y * 10.0f);
        z = (this->unk_364.z * 10.0f);
        temp_v0 = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_SI,
                                        this->actor.world.pos.x + x, this->actor.world.pos.y + y,
                                        this->actor.world.pos.z + z, 0, 0, 0, this->actor.params);
        if (temp_v0 != NULL) {
            temp_v0->parent = NULL;
        }
        Actor_Kill(&this->actor);
        return;
    }

    if ((this->unk_392 == 0) && (DECR(this->unk_394) != 0)) {
        pos = this->actor.world.pos;
        pos.y += 10.0f + ((Rand_ZeroOne() - 0.5f) * 6.0f);
        pos.x += (Rand_ZeroOne() - 0.5f) * 32.0f;
        pos.z += (Rand_ZeroOne() - 0.5f) * 32.0f;
        EffectSsDeadDb_Spawn(play, &pos, &velAndAccel, &velAndAccel, 42, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9,
                             true);
    }
}

void func_80B0DB00(EnSw* this, PlayState* play) {
    Actor_MoveForward(&this->actor);
    this->actor.shape.rot.x += 0x1000;
    this->actor.shape.rot.z += 0x1000;
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 0.0f, 5);

    if ((this->actor.bgCheckFlags & 1) && (!(0.0f <= this->actor.velocity.y))) {
        if (this->actor.floorHeight <= BGCHECK_Y_MIN || this->actor.floorHeight >= 32000.0f) {
            Actor_Kill(&this->actor);
            return;
        }

        this->actor.bgCheckFlags &= ~1;

        if (this->unk_38A == 0) {
            this->actionFunc = func_80B0DC7C;
            this->unk_394 = 10;
        } else {
            this->actor.velocity.y = ((this->unk_38A--) * 8.0f) * 0.5f;
        }

        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 16.0f, 12, 2.0f, 120, 10, false);
    }
}

void func_80B0DC7C(EnSw* this, PlayState* play) {
    Vec3f velAndAccel = { 0.0f, 0.5f, 0.0f };
    Vec3f pos = { 0.0f, 0.0f, 0.0f };

    if (DECR(this->unk_394) != 0) {
        pos.y = ((Rand_ZeroOne() - 0.5f) * 6.0f) + (this->actor.world.pos.y + 10.0f);
        pos.x = ((Rand_ZeroOne() - 0.5f) * 32.0f) + this->actor.world.pos.x;
        pos.z = ((Rand_ZeroOne() - 0.5f) * 32.0f) + this->actor.world.pos.z;
        EffectSsDeadDb_Spawn(play, &pos, &velAndAccel, &velAndAccel, 42, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9,
                             1);
        this->actor.shape.rot.x += 0x1000;
        this->actor.shape.rot.z += 0x1000;
    } else {
        Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, 0x30);
        Actor_Kill(&this->actor);
    }
}

s16 func_80B0DE34(EnSw* this, Vec3f* arg1) {
    s16 pitch;
    s16 yaw;

    yaw = Math_Vec3f_Yaw(&this->actor.world.pos, arg1) - this->actor.wallYaw;
    pitch = Math_Vec3f_Pitch(&this->actor.world.pos, arg1) - 0x4000;
    return pitch * (yaw >= 0 ? -1 : 1);
}

s32 func_80B0DEA8(EnSw* this, PlayState* play, s32 arg2) {
    Player* player = GET_PLAYER(play);
    CollisionPoly* sp58;
    s32 sp54;
    Vec3f sp48;

    if (!(player->stateFlags1 & 0x200000) && arg2) {
        return false;
    } else if (func_8002DDF4(play) && arg2) {
        return false;
    } else if (ABS(func_80B0DE34(this, &player->actor.world.pos) - this->actor.shape.rot.z) >= 0x1FC2) {
        return false;
    } else if (Math_Vec3f_DistXYZ(&this->actor.world.pos, &player->actor.world.pos) >= 130.0f) {
        return false;
    } else if (!BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &player->actor.world.pos, &sp48,
                                        &sp58, true, false, false, true, &sp54)) {
        return true;
    } else {
        return false;
    }
}

s32 func_80B0DFFC(EnSw* this, PlayState* play) {
    s32 pad;
    CollisionPoly* sp60;
    s32 sp5C;
    Vec3f sp50;
    s32 sp4C = true;

    if (this->collider.base.ocFlags1 & OC1_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        sp4C = false;
    } else if (((play->state.frames % 4) == 0) &&
               !BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &this->unk_454, &sp50, &sp60, true,
                                        false, false, true, &sp5C)) {
        sp4C = false;
    } else if (((play->state.frames % 4) == 1) &&
               BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &this->unk_460, &sp50, &sp60, true,
                                       false, false, true, &sp5C)) {
        sp4C = false;
    } else if (((play->state.frames % 4) == 2) &&
               !BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &this->subCamId, &sp50, &sp60, true,
                                        false, false, true, &sp5C)) {
        sp4C = false;
    } else if (((play->state.frames % 4) == 3) &&
               BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &this->unk_478, &sp50, &sp60, true,
                                       false, false, true, &sp5C)) {
        sp4C = false;
    }

    if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &this->unk_484, &sp50, &this->unk_430, true,
                                false, false, true, &sp5C)) {
        this->actor.wallYaw = Math_FAtan2F(this->unk_430->normal.x, this->unk_430->normal.z) * (0x8000 / M_PI);
        this->actor.world.pos = sp50;
        this->actor.world.pos.x += 6.0f * Math_SinS(this->actor.world.rot.y);
        this->actor.world.pos.z += 6.0f * Math_CosS(this->actor.world.rot.y);
        this->unk_434 = sp50;
        this->unk_434.x += Math_SinS(this->actor.world.rot.y);
        this->unk_434.z += Math_CosS(this->actor.world.rot.y);
    }

    return sp4C;
}

void func_80B0E314(EnSw* this, Vec3f arg1, f32 arg4) {
    f32 xDist;
    f32 yDist;
    f32 zDist;
    f32 dist;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;

    Math_SmoothStepToF(&this->actor.speedXZ, arg4, 0.3f, 100.0f, 0.1f);
    xDiff = arg1.x - this->actor.world.pos.x;
    yDiff = arg1.y - this->actor.world.pos.y;
    zDiff = arg1.z - this->actor.world.pos.z;
    dist = sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff));
    if (dist == 0.0f) {
        xDist = yDist = zDist = 0.0f;
    } else {
        xDist = xDiff / dist;
        yDist = yDiff / dist;
        zDist = zDiff / dist;
    }
    xDist *= this->actor.speedXZ;
    yDist *= this->actor.speedXZ;
    zDist *= this->actor.speedXZ;
    this->actor.world.pos.x += xDist;
    this->actor.world.pos.y += yDist;
    this->actor.world.pos.z += zDist;
}

s32 func_80B0E430(EnSw* this, f32 arg1, s16 arg2, s32 arg3, PlayState* play) {
    Camera* activeCam;
    f32 lastFrame = Animation_GetLastFrame(&object_st_Anim_000304);

    if (DECR(this->unk_388) != 0) {
        Math_SmoothStepToF(&this->skelAnime.playSpeed, 0.0f, 0.6f, 1000.0f, 0.01f);
        return 0;
    }

    Math_SmoothStepToF(&this->skelAnime.playSpeed, arg1, 0.6f, 1000.0f, 0.01f);

    if ((arg3 == 1) && (lastFrame < (this->skelAnime.curFrame + this->skelAnime.playSpeed))) {
        return 0;
    }

    activeCam = GET_ACTIVE_CAM(play);

    if (Math_Vec3f_DistXYZ(&this->actor.world.pos, &activeCam->eye) < 380.0f) {
        if (DECR(this->unk_440) == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALWALL_ROLL);
            this->unk_440 = 4;
        }
    } else {
        this->unk_440 = 0;
    }
    Math_SmoothStepToS(&this->actor.shape.rot.z, this->unk_444, 4, arg2, arg2);
    this->actor.world.rot = this->actor.shape.rot;
    if (this->actor.shape.rot.z == this->unk_444) {
        return 1;
    }
    return 0;
}

void func_80B0E5E0(EnSw* this, PlayState* play) {
    s32 pad[2];
    f32 rand;

    if (func_80B0E430(this, 6.0f, 0x3E8, 1, play)) {
        rand = Rand_ZeroOne();
        this->unk_444 =
            ((s16)(20000.0f * rand) + 0x2EE0) * (Rand_ZeroOne() >= 0.5f ? 1.0f : -1.0f) + this->actor.world.rot.z;
        this->unk_388 = Rand_S16Offset(10, 30);
    }

    if ((DECR(this->unk_442) == 0) && (func_80B0DEA8(this, play, 1))) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALWALL_LAUGH);
        this->unk_442 = 20;
        this->actionFunc = func_80B0E728;
    }
}

void func_80B0E728(EnSw* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;

    if (DECR(this->unk_442) != 0) {
        if (func_80B0DEA8(this, play, 1)) {
            this->unk_448 = player->actor.world.pos;
            this->unk_448.y += 30.0f;
            this->unk_444 = func_80B0DE34(this, &this->unk_448);
            func_80B0E430(this, 6.0f, (u16)0xFA0, 0, play);
        } else {
            this->actionFunc = func_80B0E5E0;
        }
    } else {
        if (!func_80B0DFFC(this, play)) {
            this->unk_442 = Rand_S16Offset(20, 10);
            this->unk_444 = func_80B0DE34(this, &this->actor.home.pos);
            this->unk_448 = this->actor.home.pos;
            this->actionFunc = func_80B0E9BC;
        } else {
            func_80B0E314(this, this->unk_448, 8.0f);

            if (DECR(this->unk_440) == 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALWALL_DASH);
                this->unk_440 = 4;
            }

            if (!(Math_Vec3f_DistXYZ(&this->actor.world.pos, &this->unk_448) > 13.0f) || func_8002DDF4(play)) {
                this->actionFunc = func_80B0E90C;
            }
        }
    }
}

void func_80B0E90C(EnSw* this, PlayState* play) {
    s32 pad;

    func_80B0E314(this, this->unk_448, 0.0f);
    if (this->actor.speedXZ == 0.0f) {
        this->unk_444 = func_80B0DE34(this, &this->actor.home.pos);
        this->unk_448 = this->actor.home.pos;
        this->actionFunc = func_80B0E9BC;
    }
}

void func_80B0E9BC(EnSw* this, PlayState* play) {
    s32 pad;

    if (func_80B0E430(this, 6.0f, 0x3E8, 0, play)) {
        func_80B0E314(this, this->unk_448, 2.0f);
        if (!(Math_Vec3f_DistXYZ(&this->actor.world.pos, &this->unk_448) > 4.0f)) {
            this->actionFunc = func_80B0E5E0;
        }
    }
}

void EnSw_Update(Actor* thisx, PlayState* play) {
    EnSw* this = (EnSw*)thisx;

    // #region SOH [Co-op]
    // Check to see if this gold skull token has already been retrieved.
    if (GET_GS_FLAGS((thisx->params & 0x1F00) >> 8) & (thisx->params & 0xFF)) {
        Actor_Kill(&this->actor);
        return;
    }
    // #endregion

    SkelAnime_Update(&this->skelAnime);
    func_80B0C9F0(this, play);
    this->actionFunc(this, play);
    func_80B0CBE8(this, play);
}

s32 EnSw_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Vec3f sp7C = { 1400.0f, -2600.0f, -800.0f };
    Vec3f sp70 = { 1400.0f, -1600.0f, 0.0f };
    Vec3f sp64 = { -1400.0f, -2600.0f, -800.0f };
    Vec3f sp58 = { -1400.0f, -1600.0f, 0.0f };
    Vec3f sp4C = { 0.0, 0.0f, -600.0f };
    EnSw* this = (EnSw*)thisx;
    Vec3f sp3C = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx);

    if (((this->actor.params & 0xE000) >> 0xD) != 0) {
        switch (limbIndex) {
            case 23:
                *dList = object_st_DL_004788;
                break;
            case 8:
                *dList = object_st_DL_0046F0;
                break;
            case 14:
                *dList = object_st_DL_004658;
                break;
            case 11:
                *dList = object_st_DL_0045C0;
                break;
            case 26:
                *dList = object_st_DL_004820;
                break;
            case 20:
                *dList = object_st_DL_0048B8;
                break;
            case 17:
                *dList = object_st_DL_004950;
                break;
            case 29:
                *dList = object_st_DL_0049E8;
                break;
            case 5:
                *dList = object_st_DL_003FB0;
                break;
            case 4:
                *dList = object_st_DL_0043D8;
                break;
        }
    }

    if (limbIndex == 1) {
        Matrix_MultVec3f(&sp7C, &this->unk_454);
        Matrix_MultVec3f(&sp70, &this->unk_460);
        Matrix_MultVec3f(&sp64, &this->subCamId);
        Matrix_MultVec3f(&sp58, &this->unk_478);
        Matrix_MultVec3f(&sp4C, &this->unk_484);
    }

    if (limbIndex == 5) {
        Matrix_MultVec3f(&sp3C, &this->actor.focus.pos);
    }

    if (limbIndex == 4) {
        gDPSetEnvColor(POLY_OPA_DISP++, this->unk_1F4.r, this->unk_1F4.g, this->unk_1F4.b, 0);
    }

    Collider_UpdateSpheres(limbIndex, &this->collider);

    CLOSE_DISPS(play->state.gfxCtx);

    return false;
}

void EnSw_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
}

void func_80B0EDB8(PlayState* play, Color_RGBA8* arg1, s16 arg2, s16 arg3) {
    f32 temp_f2;

    OPEN_DISPS(play->state.gfxCtx);

    temp_f2 = (11500.0f / arg3) * (arg3 - arg2);

    if (0.0f == temp_f2) {
        temp_f2 = 11500;
    }

    POLY_OPA_DISP = Gfx_SetFog2(POLY_OPA_DISP, arg1->r, arg1->g, arg1->b, arg1->a, 0, (s16)temp_f2);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80B0EEA4(PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnSw_Draw(Actor* thisx, PlayState* play) {
    EnSw* this = (EnSw*)thisx;
    Color_RGBA8 sp30 = { 184, 0, 228, 255 };

    if (((this->actor.params & 0xE000) >> 0xD) != 0) {
        Matrix_RotateX(DEGF_TO_RADF(-80), MTXMODE_APPLY);
        if (this->actor.colChkInfo.health != 0) {
            Matrix_Translate(0.0f, 0.0f, 200.0f, MTXMODE_APPLY);
        }
        func_8002EBCC(&this->actor, play, 0);
    } else if (this->actionFunc == func_80B0E728) {
        func_80B0EDB8(play, &sp30, 0x14, 0x1E);
    }

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, EnSw_OverrideLimbDraw,
                      EnSw_PostLimbDraw, this);
    if (this->actionFunc == func_80B0E728) {
        func_80B0EEA4(play);
    }
}
