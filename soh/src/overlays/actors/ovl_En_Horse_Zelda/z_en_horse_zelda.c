/*
 * File: z_en_horse_zelda.c
 * Overlay: ovl_En_Horse_Zelda
 * Description: Zelda's Horse
 */

#include "z_en_horse_zelda.h"
#include "objects/object_horse_zelda/object_horse_zelda.h"

#define FLAGS ACTOR_FLAG_4

void EnHorseZelda_Init(Actor* thisx, PlayState* play);
void EnHorseZelda_Destroy(Actor* thisx, PlayState* play);
void EnHorseZelda_Update(Actor* thisx, PlayState* play);
void EnHorseZelda_Draw(Actor* thisx, PlayState* play);

void func_80A6DCCC(EnHorseZelda* this, PlayState* play);
void func_80A6DDFC(EnHorseZelda* this, PlayState* play);
void func_80A6DC7C(EnHorseZelda* this);

const ActorInit En_Horse_Zelda_InitVars = {
    ACTOR_EN_HORSE_ZELDA,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HORSE_ZELDA,
    sizeof(EnHorseZelda),
    (ActorFunc)EnHorseZelda_Init,
    (ActorFunc)EnHorseZelda_Destroy,
    (ActorFunc)EnHorseZelda_Update,
    (ActorFunc)EnHorseZelda_Draw,
    NULL,
};

static AnimationHeader* sAnimationHeaders[] = { &gHorseZeldaGallopingAnim };

static f32 splaySpeeds[] = { 2.0f / 3.0f };

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 40, 100, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 10, 35, 100, MASS_HEAVY };

typedef struct {
    /* 0x0 */ Vec3s unk_0;
    /* 0x6 */ u8 unk_6;
} unknownStruct; // size = 0x8

static unknownStruct D_80A6E240[] = {
    { -1682, -500, 12578, 0x07 }, { -3288, -500, 13013, 0x07 }, { -5142, -417, 11630, 0x07 },
    { -5794, -473, 9573, 0x07 },  { -6765, -500, 8364, 0x07 },  { -6619, -393, 6919, 0x07 },
    { -5193, 124, 5433, 0x07 },   { -2970, 2, 4537, 0x07 },     { -2949, -35, 4527, 0x07 },
    { -1907, -47, 2978, 0x07 },   { 2488, 294, 3628, 0x07 },    { 3089, 378, 4713, 0x07 },
    { 1614, -261, 7596, 0x07 },   { 754, -187, 9295, 0x07 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1200, ICHAIN_STOP),
};

static EnHorseZeldaActionFunc sActionFuncs[] = {
    func_80A6DCCC,
    func_80A6DDFC,
};

void func_80A6D8D0(unknownStruct* data, s32 index, Vec3f* vec) {
    vec->x = data[index].unk_0.x;
    vec->y = data[index].unk_0.y;
    vec->z = data[index].unk_0.z;
}

void func_80A6D918(EnHorseZelda* this, PlayState* play) {
    s32 pad;
    Vec3f sp28;
    s16 yawDiff;

    func_80A6D8D0(D_80A6E240, this->unk_1EC, &sp28);
    if (Math3D_Vec3f_DistXYZ(&sp28, &this->actor.world.pos) <= 400.0f) {
        this->unk_1EC++;
        if (this->unk_1EC >= 14) {
            this->unk_1EC = 0;
            func_80A6D8D0(D_80A6E240, 0, &sp28);
        }
    }
    yawDiff = Math_Vec3f_Yaw(&this->actor.world.pos, &sp28) - this->actor.world.rot.y;
    if (yawDiff >= 0x12D) {
        this->actor.world.rot.y += 0x12C;
    } else if (yawDiff < -0x12C) {
        this->actor.world.rot.y -= 0x12C;
    } else {
        this->actor.world.rot.y += yawDiff;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (Actor_WorldDistXZToActor(&this->actor, &GET_PLAYER(play)->actor) <= 300.0f) {
        if (this->actor.speedXZ < 12.0f) {
            this->actor.speedXZ += 1.0f;
        } else {
            this->actor.speedXZ -= 1.0f;
        }
    } else if (this->actor.speedXZ < D_80A6E240[this->unk_1EC].unk_6) {
        this->actor.speedXZ += 0.5f;
    } else {
        this->actor.speedXZ -= 0.5f;
    }
}

void EnHorseZelda_Init(Actor* thisx, PlayState* play) {
    EnHorseZelda* this = (EnHorseZelda*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Actor_SetScale(&this->actor, 0.0115f);
    this->actor.gravity = -3.5f;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawHorse, 20.0f);
    this->actor.speedXZ = 0.0f;
    this->actor.focus.pos = this->actor.world.pos;
    this->action = 0;
    this->actor.focus.pos.y += 70.0f;
    Skin_Init(play, &this->skin, &gHorseZeldaSkel, &gHorseZeldaGallopingAnim);
    this->animationIndex = 0;
    Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[0]);
    Collider_InitCylinder(play, &this->colliderCylinder);
    Collider_SetCylinderType1(play, &this->colliderCylinder, &this->actor, &sCylinderInit);
    Collider_InitJntSph(play, &this->colliderSphere);
    Collider_SetJntSph(play, &this->colliderSphere, &this->actor, &sJntSphInit, &this->colliderSphereItem);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    this->animationIndex = 0;
    func_80A6DC7C(this);
}

void EnHorseZelda_Destroy(Actor* thisx, PlayState* play) {
    EnHorseZelda* this = (EnHorseZelda*)thisx;

    Collider_DestroyCylinder(play, &this->colliderCylinder);
    Collider_DestroyJntSph(play, &this->colliderSphere);
    Skin_Free(play, &this->skin);
}

void func_80A6DC7C(EnHorseZelda* this) {
    this->action = 0;
    this->animationIndex++;
    if (this->animationIndex > 0) {
        this->animationIndex = 0;
    }
    Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->animationIndex]);
}

void func_80A6DCCC(EnHorseZelda* this, PlayState* play) {
    this->actor.speedXZ = 0.0f;
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        func_80A6DC7C(this);
    }
}

void func_80A6DD14(EnHorseZelda* this) {
    f32 sp34;

    this->action = 1;
    this->animationIndex = 0;
    sp34 = this->actor.speedXZ / 6.0f;
    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->animationIndex],
                     splaySpeeds[this->animationIndex] * sp34 * 1.5f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->animationIndex]), ANIMMODE_ONCE, 0.0f);
}

void func_80A6DDFC(EnHorseZelda* this, PlayState* play) {
    func_80A6D918(this, play);
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        func_80A6DD14(this);
    }
}

void func_80A6DE38(EnHorseZelda* this, PlayState* play) {
    s32 pad;
    CollisionPoly* poly;
    s32 pad2;
    Vec3f pos;
    s32 bgId;

    pos.x = (Math_SinS(this->actor.shape.rot.y) * 30.0f) + this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + 60.0f;
    pos.z = (Math_CosS(this->actor.shape.rot.y) * 30.0f) + this->actor.world.pos.z;
    this->unk_1F4 = BgCheck_EntityRaycastFloor3(&play->colCtx, &poly, &bgId, &pos);
    this->actor.shape.rot.x = Math_FAtan2F(this->actor.world.pos.y - this->unk_1F4, 30.0f) * (0x8000 / M_PI);
}

void EnHorseZelda_Update(Actor* thisx, PlayState* play) {
    EnHorseZelda* this = (EnHorseZelda*)thisx;
    s32 pad;

    sActionFuncs[this->action](this, play);
    this->actor.speedXZ = 0.0f;
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 55.0f, 100.0f, 0x1D);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
}

void EnHorseZelda_PostDraw(Actor* thisx, PlayState* play, Skin* skin) {
    Vec3f sp4C;
    Vec3f sp40;
    EnHorseZelda* this = (EnHorseZelda*)thisx;
    s32 i;

    for (i = 0; i < this->colliderSphere.count; i++) {
        sp4C.x = this->colliderSphere.elements[i].dim.modelSphere.center.x;
        sp4C.y = this->colliderSphere.elements[i].dim.modelSphere.center.y;
        sp4C.z = this->colliderSphere.elements[i].dim.modelSphere.center.z;

        Skin_GetLimbPos(skin, this->colliderSphere.elements[i].dim.limb, &sp4C, &sp40);

        this->colliderSphere.elements[i].dim.worldSphere.center.x = sp40.x;
        this->colliderSphere.elements[i].dim.worldSphere.center.y = sp40.y;
        this->colliderSphere.elements[i].dim.worldSphere.center.z = sp40.z;

        this->colliderSphere.elements[i].dim.worldSphere.radius =
            this->colliderSphere.elements[i].dim.modelSphere.radius * this->colliderSphere.elements[i].dim.scale;
    }

    //! @bug see relevant comment in `EnHorse_SkinCallback1`
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderSphere.base);
}

void EnHorseZelda_Draw(Actor* thisx, PlayState* play) {
    EnHorseZelda* this = (EnHorseZelda*)thisx;

    func_80A6DE38(this, play);
    func_80093D18(play->state.gfxCtx);
    func_800A6330(&this->actor, play, &this->skin, EnHorseZelda_PostDraw, true);
}
