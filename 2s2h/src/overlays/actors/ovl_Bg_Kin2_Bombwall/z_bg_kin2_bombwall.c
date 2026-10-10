/*
 * File: z_bg_kin2_bombwall.c
 * Overlay: ovl_Bg_Kin2_Bombwall
 * Description: Ocean Spider House - Bombable Wall
 */
#include "z_bg_kin2_bombwall.h"
#include "objects/object_kin2_obj/object_kin2_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UCODE_POINT_LIGHT_ENABLED)

void BgKin2Bombwall_Init(Actor* thisx, PlayState* play);
void BgKin2Bombwall_Destroy(Actor* thisx, PlayState* play);
void BgKin2Bombwall_Update(Actor* thisx, PlayState* play);
void BgKin2Bombwall_Draw(Actor* thisx, PlayState* play);

void BgKin2Bombwall_SetupWait(BgKin2Bombwall* this);
void BgKin2Bombwall_Wait(BgKin2Bombwall* this, PlayState* play);
void BgKin2Bombwall_SetupPlayCutscene(BgKin2Bombwall* this);
void BgKin2Bombwall_PlayCutscene(BgKin2Bombwall* this, PlayState* play);
void BgKin2Bombwall_SetupEndCutscene(BgKin2Bombwall* this);
void BgKin2Bombwall_EndCutscene(BgKin2Bombwall* this, PlayState* play);

ActorProfile Bg_Kin2_Bombwall_Profile = {
    /**/ ACTOR_BG_KIN2_BOMBWALL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_KIN2_OBJ,
    /**/ sizeof(BgKin2Bombwall),
    /**/ BgKin2Bombwall_Init,
    /**/ BgKin2Bombwall_Destroy,
    /**/ BgKin2Bombwall_Update,
    /**/ BgKin2Bombwall_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 60, 60, 0, { 0, 0, 0 } },
};

s32 BgKin2Bombwall_IsHitFromNearby(BgKin2Bombwall* this, PlayState* play) {
    Actor* bombwallCollider;

    if (this->collider.base.acFlags & AC_HIT) {
        bombwallCollider = this->collider.base.ac;
        // Distance check required to only react to sufficiently close explosions.
        if ((bombwallCollider != NULL) &&
            (Math3D_Vec3fDistSq(&this->dyna.actor.world.pos, &bombwallCollider->world.pos) < 6400.0f)) {
            return true;
        }
    }
    return false;
}

static Color_RGBA8 sPrimColor = { 210, 210, 210, 255 };
static Color_RGBA8 sEnvColor = { 140, 140, 140, 255 };
static Vec3f sDustAccel = { 0.0f, 0.33f, 0.0f };
static s8 sRandomYOffsets[] = { -60, -34, -8, 18, 44 };
static s16 sScales[] = { 25, 23, 21, 19, 17, 15, 13, 10 }; // Scales for random explosion debris.

void BgKin2Bombwall_SpawnEffects(BgKin2Bombwall* this, PlayState* play) {
    s32 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f spD8;
    Vec3f spCC;
    s32 j;
    s32 k;
    f32 temp_a0;
    s32 pad;
    s16 phi_s0;
    s16 phi_s1;

    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);

    for (i = 0, k = 0; i < 6; i++) {
        temp_a0 = (i + 1) * 15.f;
        for (j = 0; j < ARRAY_COUNT(sRandomYOffsets); j++) {
            k++;
            k &= 7;

            spD8.x = sRandomYOffsets[j] + (s32)(Rand_Next() >> 0x1C);
            spD8.y = ((Rand_ZeroOne() - 0.5f) * 15.0f) + temp_a0;
            spD8.z = (Rand_ZeroOne() * 20.0f) - 10.0f;

            spCC.x = (2.0f * (Rand_ZeroOne() - 0.5f)) + (spD8.x * (6.0f / 325.0f));
            spCC.y = (Rand_ZeroOne() * 7.0f) + 4.0f;
            spCC.z = spD8.z * 0.3f;

            Matrix_MultVec3f(&spD8, &pos);
            Matrix_MultVec3f(&spCC, &velocity);

            pos.x += this->dyna.actor.world.pos.x;
            pos.y += this->dyna.actor.world.pos.y;
            pos.z += this->dyna.actor.world.pos.z;

            if (Rand_Next() % 4 == 0) {
                phi_s0 = 0x20;
            } else {
                phi_s0 = 0x40;
            }

            if (k < 2 || (s32)Rand_Next() > 0) {
                phi_s0 |= 1;
                phi_s1 = 1;
                func_800B0E48(play, &pos, &gZeroVec3f, &sDustAccel, &sPrimColor, &sEnvColor, (Rand_Next() >> 0x1B) + 70,
                              (Rand_Next() >> 0x1A) + 60); // for dust spawn
            } else {
                phi_s1 = 0;
            }
            EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -550, phi_s0, 30, 0, 0, sScales[k], phi_s1, 0, 50, -1,
                                 OBJECT_KIN2_OBJ, gOceanSpiderHouseBombableWallDebrisDL);
        }
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgKin2Bombwall_Init(Actor* thisx, PlayState* play) {
    BgKin2Bombwall* this = (BgKin2Bombwall*)thisx;
    ColliderCylinder* bombwallCollider;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    bombwallCollider = &this->collider;
    Collider_InitCylinder(play, bombwallCollider);
    if (Flags_GetSwitch(play, BG_KIN2_BOMBWALL_GET_SWITCH_FLAG(&this->dyna.actor))) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    DynaPolyActor_LoadMesh(play, &this->dyna, &gOceanSpiderHouseBombableWallCol);
    Collider_SetCylinder(play, bombwallCollider, &this->dyna.actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->dyna.actor, bombwallCollider);
    Actor_SetFocus(&this->dyna.actor, 60.0f);
    BgKin2Bombwall_SetupWait(this);
}

void BgKin2Bombwall_Destroy(Actor* thisx, PlayState* play) {
    BgKin2Bombwall* this = (BgKin2Bombwall*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(play, &this->collider);
}

void BgKin2Bombwall_SetupWait(BgKin2Bombwall* this) {
    this->actionFunc = BgKin2Bombwall_Wait;
}

void BgKin2Bombwall_Wait(BgKin2Bombwall* this, PlayState* play) {
    if (BgKin2Bombwall_IsHitFromNearby(this, play)) {
        this->collider.base.acFlags &= ~AC_HIT;
        CutsceneManager_Queue(this->dyna.actor.csId);
        BgKin2Bombwall_SetupPlayCutscene(this);
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void BgKin2Bombwall_SetupPlayCutscene(BgKin2Bombwall* this) {
    this->actionFunc = BgKin2Bombwall_PlayCutscene;
}

void BgKin2Bombwall_PlayCutscene(BgKin2Bombwall* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        Flags_SetSwitch(play, BG_KIN2_BOMBWALL_GET_SWITCH_FLAG(&this->dyna.actor));
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 60, NA_SE_EV_WALL_BROKEN);
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        this->dyna.actor.draw = NULL;
        BgKin2Bombwall_SpawnEffects(this, play);
        BgKin2Bombwall_SetupEndCutscene(this);

    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void BgKin2Bombwall_SetupEndCutscene(BgKin2Bombwall* this) {
    this->timer = 40;
    this->actionFunc = BgKin2Bombwall_EndCutscene;
}

void BgKin2Bombwall_EndCutscene(BgKin2Bombwall* this, PlayState* play) {
    this->timer--;
    if (this->timer <= 0) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        Actor_Kill(&this->dyna.actor);
    }
}

void BgKin2Bombwall_Update(Actor* thisx, PlayState* play) {
    BgKin2Bombwall* this = (BgKin2Bombwall*)thisx;

    this->actionFunc(this, play);
}

void BgKin2Bombwall_Draw(Actor* thisx, PlayState* play) {
    BgKin2Bombwall* this = (BgKin2Bombwall*)thisx;

    Gfx_DrawDListOpa(play, gOceanSpiderHouseBombableWallDL);
    Gfx_DrawDListXlu(play, gOceanSpiderHouseBombableWallCrackDL);
}
