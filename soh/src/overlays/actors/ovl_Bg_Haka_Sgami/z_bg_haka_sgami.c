/*
 * File: z_bg_haka_sgami.c
 * Overlay: ovl_Bg_Haka_Sgami
 * Description: Spinning Scythe Trap
 */

#include "z_bg_haka_sgami.h"
#include "objects/object_haka_objects/object_haka_objects.h"
#include "objects/object_ice_objects/object_ice_objects.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4)

typedef enum {
    /* 0 */ SCYTHE_TRAP_SHADOW_TEMPLE,
    /* 1 */ SCYTHE_TRAP_SHADOW_TEMPLE_INVISIBLE,
    /* 2 */ SCYTHE_TRAP_ICE_CAVERN
} SpinningScytheTrapMode;

#define SCYTHE_SPIN_TIME 32

void BgHakaSgami_Init(Actor* thisx, PlayState* play);
void BgHakaSgami_Destroy(Actor* thisx, PlayState* play);
void BgHakaSgami_Update(Actor* thisx, PlayState* play);
void BgHakaSgami_Draw(Actor* thisx, PlayState* play);

void BgHakaSgami_SetupSpin(BgHakaSgami* this, PlayState* play);
void BgHakaSgami_Spin(BgHakaSgami* this, PlayState* play);

const ActorInit Bg_Haka_Sgami_InitVars = {
    ACTOR_BG_HAKA_SGAMI,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(BgHakaSgami),
    (ActorFunc)BgHakaSgami_Init,
    (ActorFunc)BgHakaSgami_Destroy,
    (ActorFunc)BgHakaSgami_Update,
    NULL,
    NULL,
};

static ColliderTrisElementInit sTrisElementsInit[4] = {
    {
        {
            ELEMTYPE_UNK2,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { { { 365.0f, 45.0f, 27.0f }, { 130.0f, 45.0f, 150.0f }, { 290.0f, 45.0f, 145.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { { { 250.0f, 45.0f, 90.0f }, { 50.0f, 45.0f, 80.0f }, { 160.0f, 45.0f, 160.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { { { -305.0f, 33.0f, -7.0f }, { -220.0f, 33.0f, 40.0f }, { -130.0f, 33.0f, -5.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { { { -190.0f, 33.0f, 40.0f }, { -30.0f, 33.0f, 15.0f }, { -70.0f, 33.0f, -30.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    4,
    sTrisElementsInit,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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
    { 80, 130, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 80, 130, MASS_IMMOVABLE };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 4, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHakaSgami_Init(Actor* thisx, PlayState* play) {
    static u8 sP1StartColor[] = { 250, 250, 250, 200 };
    static u8 sP2StartColor[] = { 200, 200, 200, 130 };
    static u8 sP1EndColor[] = { 200, 200, 200, 60 };
    static u8 sP2EndColor[] = { 150, 150, 150, 20 };
    BgHakaSgami* this = (BgHakaSgami*)thisx;
    EffectBlureInit1 blureInit;
    s32 i;
    ColliderTris* colliderScythe = &this->colliderScythe;

    Actor_ProcessInitChain(thisx, sInitChain);

    this->unk_151 = thisx->params & 0xFF;
    thisx->params = (thisx->params >> 8) & 0xFF;

    if (this->unk_151 != 0) {
        thisx->flags |= ACTOR_FLAG_7;
    }

    Collider_InitTris(play, colliderScythe);
    Collider_SetTris(play, colliderScythe, thisx, &sTrisInit, this->colliderScytheItems);
    Collider_InitCylinder(play, &this->colliderScytheCenter);
    Collider_SetCylinder(play, &this->colliderScytheCenter, thisx, &sCylinderInit);

    this->colliderScytheCenter.dim.pos.x = thisx->world.pos.x;
    this->colliderScytheCenter.dim.pos.y = thisx->world.pos.y;
    this->colliderScytheCenter.dim.pos.z = thisx->world.pos.z;

    CollisionCheck_SetInfo(&thisx->colChkInfo, NULL, &sColChkInfoInit);

    for (i = 0; i < 4; i++) {
        blureInit.p1StartColor[i] = sP1StartColor[i];
        blureInit.p2StartColor[i] = sP2StartColor[i];
        blureInit.p1EndColor[i] = sP1EndColor[i];
        blureInit.p2EndColor[i] = sP2EndColor[i];
    }
    blureInit.elemDuration = 10;
    blureInit.unkFlag = false;
    blureInit.calcMode = 2;
    Effect_Add(play, &this->blureEffectIndex[0], EFFECT_BLURE1, 0, 0, &blureInit);
    Effect_Add(play, &this->blureEffectIndex[1], EFFECT_BLURE1, 0, 0, &blureInit);

    if (thisx->params == SCYTHE_TRAP_SHADOW_TEMPLE) {
        this->requiredObjBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_HAKA_OBJECTS);
        thisx->flags &= ~ACTOR_FLAG_0;
    } else {
        this->requiredObjBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_ICE_OBJECTS);
        this->colliderScytheCenter.dim.radius = 30;
        this->colliderScytheCenter.dim.height = 70;
        Actor_SetFocus(thisx, 40.0f);
    }

    if (this->requiredObjBankIndex < 0) {
        Actor_Kill(thisx);
        return;
    }

    this->actionFunc = BgHakaSgami_SetupSpin;
}

void BgHakaSgami_Destroy(Actor* thisx, PlayState* play) {
    BgHakaSgami* this = (BgHakaSgami*)thisx;

    Effect_Delete(play, this->blureEffectIndex[0]);
    Effect_Delete(play, this->blureEffectIndex[1]);
    Collider_DestroyTris(play, &this->colliderScythe);
    Collider_DestroyCylinder(play, &this->colliderScytheCenter);
}

void BgHakaSgami_SetupSpin(BgHakaSgami* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->requiredObjBankIndex)) {
        this->actor.objBankIndex = this->requiredObjBankIndex;
        this->actor.draw = BgHakaSgami_Draw;
        this->timer = SCYTHE_SPIN_TIME;
        this->actor.flags &= ~ACTOR_FLAG_4;
        this->actionFunc = BgHakaSgami_Spin;
    }
}

void BgHakaSgami_Spin(BgHakaSgami* this, PlayState* play) {
    static Vec3f blureEffectVertices2[] = {
        { -20.0f, 50.0f, 130.0f },
        { -50.0f, 33.0f, 20.0f },
    };
    static Vec3f blureEffectVertices1[] = {
        { 380.0f, 50.0f, 50.0f },
        { 310.0f, 33.0f, 0.0f },
    };
    s32 i;
    s32 j;
    Vec3f scytheVertices[3];
    f32 actorRotYSin;
    f32 actorRotYCos;
    s32 iterateCount;
    ColliderTrisElementInit* elementInit;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.shape.rot.y += ((s16)(512.0f * sinf(this->timer * (M_PI / 16.0f))) + 0x400) >> 1;

    if (this->timer == 0) {
        this->timer = SCYTHE_SPIN_TIME;
    }

    actorRotYSin = Math_SinS(this->actor.shape.rot.y);
    actorRotYCos = Math_CosS(this->actor.shape.rot.y);

    iterateCount = (this->actor.params != 0) ? 4 : 2;

    for (i = iterateCount - 2; i < iterateCount; i++) {
        elementInit = &sTrisInit.elements[i];

        for (j = 0; j < 3; j++) {
            scytheVertices[j].x = this->actor.world.pos.x + elementInit->dim.vtx[j].z * actorRotYSin +
                                  elementInit->dim.vtx[j].x * actorRotYCos;
            scytheVertices[j].y = this->actor.world.pos.y + elementInit->dim.vtx[j].y;
            scytheVertices[j].z = this->actor.world.pos.z + elementInit->dim.vtx[j].z * actorRotYCos -
                                  elementInit->dim.vtx[j].x * actorRotYSin;
        }

        Collider_SetTrisVertices(&this->colliderScythe, i, &scytheVertices[0], &scytheVertices[1], &scytheVertices[2]);

        for (j = 0; j < 3; j++) {
            scytheVertices[j].x = (2 * this->actor.world.pos.x) - scytheVertices[j].x;
            scytheVertices[j].z = (2 * this->actor.world.pos.z) - scytheVertices[j].z;
        }

        Collider_SetTrisVertices(&this->colliderScythe, (i + 2) % 4, &scytheVertices[0], &scytheVertices[1],
                                 &scytheVertices[2]);
    }

    if ((this->unk_151 == 0) || play->actorCtx.lensActive) {
        scytheVertices[0].x = this->actor.world.pos.x + blureEffectVertices1[this->actor.params].z * actorRotYSin +
                              blureEffectVertices1[this->actor.params].x * actorRotYCos;
        scytheVertices[0].y = this->actor.world.pos.y + blureEffectVertices1[this->actor.params].y;
        scytheVertices[0].z = this->actor.world.pos.z + blureEffectVertices1[this->actor.params].z * actorRotYCos -
                              blureEffectVertices1[this->actor.params].x * actorRotYSin;
        scytheVertices[1].x = this->actor.world.pos.x + blureEffectVertices2[this->actor.params].z * actorRotYSin +
                              blureEffectVertices2[this->actor.params].x * actorRotYCos;
        scytheVertices[1].y = this->actor.world.pos.y + blureEffectVertices2[this->actor.params].y;
        scytheVertices[1].z = this->actor.world.pos.z + blureEffectVertices2[this->actor.params].z * actorRotYCos -
                              blureEffectVertices2[this->actor.params].x * actorRotYSin;
        EffectBlure_AddVertex(Effect_GetByIndex(this->blureEffectIndex[0]), &scytheVertices[0], &scytheVertices[1]);

        for (j = 0; j < 2; j++) {
            scytheVertices[j].x = (2 * this->actor.world.pos.x) - scytheVertices[j].x;
            scytheVertices[j].z = (2 * this->actor.world.pos.z) - scytheVertices[j].z;
        }

        EffectBlure_AddVertex(Effect_GetByIndex(this->blureEffectIndex[1]), &scytheVertices[0], &scytheVertices[1]);
    }

    CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderScythe.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderScytheCenter.base);
    func_8002F974(&this->actor, NA_SE_EV_ROLLCUTTER_MOTOR - SFX_FLAG);
}

void BgHakaSgami_Update(Actor* thisx, PlayState* play) {
    BgHakaSgami* this = (BgHakaSgami*)thisx;
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 & 0x300000C0) || (this->actionFunc == BgHakaSgami_SetupSpin)) {
        this->actionFunc(this, play);
    }
}

void BgHakaSgami_Draw(Actor* thisx, PlayState* play) {
    BgHakaSgami* this = (BgHakaSgami*)thisx;

    if (this->unk_151 != 0) {
        Gfx_DrawDListXlu(play, object_haka_objects_DL_00BF20);
    } else if (this->actor.params == SCYTHE_TRAP_SHADOW_TEMPLE) {
        Gfx_DrawDListOpa(play, object_haka_objects_DL_00BF20);
    } else {
        Gfx_DrawDListOpa(play, object_ice_objects_DL_0021F0);
    }
}
