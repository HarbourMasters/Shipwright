/*
 * File: z_bg_last_bwall.c
 * Overlay: ovl_Bg_Last_Bwall
 * Description: Link Moon Dungeon - Bombable, Climbable Wall
 */

#include "z_bg_last_bwall.h"

#define FLAGS 0x00000000

typedef struct {
    /* 0x0 */ Vec3s* posOffsets;
    /* 0x4 */ s16* indices;
    /* 0x8 */ s32 count;
} BgLastBwallInitColliderStruct; // size = 0xC

typedef struct {
    /* 0x0 */ Gfx* dList;
    /* 0x4 */ void* pad4;
    /* 0x8 */ CollisionHeader* colHeader;
} BgLastBwallModelInfo; // size = 0xC

void BgLastBwall_Init(Actor* thisx, PlayState* play);
void BgLastBwall_Destroy(Actor* thisx, PlayState* play);
void BgLastBwall_Update(Actor* thisx, PlayState* play);
void BgLastBwall_Draw(Actor* thisx, PlayState* play2);

void BgLastBwall_InitCollider(ColliderTrisInit* init, Vec3f* pos, Vec3s* rot, ColliderTris* collider,
                              BgLastBwallInitColliderStruct* arg4);
void func_80C187E4(BgLastBwall* this);
void func_80C184EC(BgLastBwall* this, PlayState* play);
void func_80C187F8(BgLastBwall* this, PlayState* play);
void func_80C1886C(BgLastBwall* this, PlayState* play);
void func_80C18884(BgLastBwall* this, PlayState* play);
void func_80C188C4(BgLastBwall* this, PlayState* play);
void BgLastBwall_DoNothing(BgLastBwall* this, PlayState* play);

ActorProfile Bg_Last_Bwall_Profile = {
    /**/ ACTOR_BG_LAST_BWALL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_LAST_OBJ,
    /**/ sizeof(BgLastBwall),
    /**/ BgLastBwall_Init,
    /**/ BgLastBwall_Destroy,
    /**/ BgLastBwall_Update,
    /**/ BgLastBwall_Draw,
};

static ColliderTrisElementInit sTrisElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -70.0f, 0.0f, 3.0f }, { 70.0f, 0.0f, 3.0f }, { -70.0f, 200.0f, 3.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 70.0f, 0.0f, 3.0f }, { 70.0f, 200.0f, 3.0f }, { -70.0f, 200.0f, 3.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit),
    sTrisElementsInit,
};

static BgLastBwallModelInfo D_80C18A48[] = {
    { object_last_obj_DL_0001A8, NULL, &object_last_obj_Colheader_000288 },
    { object_last_obj_DL_001318, NULL, &object_last_obj_Colheader_001450 },
};

static Vec3s D_80C18A60[] = {
    { 280, 280, -40 },
    { 280, 280, 120 },
    { 280, 400, 120 },
    { 280, 400, -40 },
};

static s16 D_80C18A78[] = { 0, 1, 2, 0, 2, 3 };

static BgLastBwallInitColliderStruct D_80C18A84 = { D_80C18A60, D_80C18A78, 2 };

static Vec3s D_80C18A90[] = {
    { -80, 400, -80 },
    { -80, 400, 80 },
    { 80, 400, 80 },
    { 80, 400, -80 },
};

static s16 D_80C18AA8[] = { 0, 3, 2, 0, 2, 1 };

static BgLastBwallInitColliderStruct D_80C18AB4 = { D_80C18A90, D_80C18AA8, 2 };

static BgLastBwallInitColliderStruct* D_80C18AC0[] = { &D_80C18A84, &D_80C18AB4 };

static InitChainEntry D_80C18AC8[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgLastBwall_InitCollider(ColliderTrisInit* init, Vec3f* pos, Vec3s* rot, ColliderTris* collider,
                              BgLastBwallInitColliderStruct* arg4) {
    s32 i;
    s32 j;
    Vec3f sp6C[3];
    Vec3f sp60;
    Vec3f sp54;

    Matrix_RotateYS(rot->y, MTXMODE_NEW);
    Matrix_RotateXS(rot->x, MTXMODE_APPLY);
    Matrix_RotateZS(rot->z, MTXMODE_APPLY);
    for (i = 0; i < init->count; i++) {
        for (j = 0; j < 3; j++) {
            Math_Vec3s_ToVec3f(&sp60, &arg4->posOffsets[arg4->indices[j]]);
            Matrix_MultVec3f(&sp60, &sp54);
            Math_Vec3f_Sum(&sp54, pos, &sp6C[j]);
        }

        arg4->indices += 3;
        Collider_SetTrisVertices(collider, i, &sp6C[0], &sp6C[1], &sp6C[2]);
    }
}

void BgLastBwall_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgLastBwall* this = (BgLastBwall*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, D_80C18AC8);
    this->type = BGLASTBWALL_GET_TYPE(&this->dyna.actor);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, D_80C18A48[this->type].colHeader);
    Collider_InitTris(play, &this->colliderTris);
    if (Flags_GetSwitch(play, BGLASTBWALL_GET_SWITCH_FLAG(&this->dyna.actor))) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    if (!Collider_SetTris(play, &this->colliderTris, &this->dyna.actor, &sTrisInit, this->colliderTrisElement)) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    BgLastBwall_InitCollider(&sTrisInit, &this->dyna.actor.world.pos, &this->dyna.actor.shape.rot, &this->colliderTris,
                             D_80C18AC0[this->type]);
    SubS_FillCutscenesList(&this->dyna.actor, this->csIdList, ARRAY_COUNT(this->csIdList));
    func_80C187E4(this);
}

void BgLastBwall_Destroy(Actor* thisx, PlayState* play) {
    BgLastBwall* this = (BgLastBwall*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80C184EC(BgLastBwall* this, PlayState* play) {
    f32 randVar;
    Vec3f spD0;
    Vec3f effectPosAndAccel;
    s32 var_v0;
    Vec3f effectVelocity;
    s32 i;

    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);
    Matrix_RotateXS(this->dyna.actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_RotateZS(this->dyna.actor.shape.rot.y, MTXMODE_APPLY);
    for (i = 0; i < 30; i++) {
        switch (this->type) {
            case BGLASTBWALL_TYPE_0:
                spD0.x = 280.0f;
                spD0.y = Rand_Centered() * 160.0f + 320.0f;
                spD0.z = Rand_Centered() * 160.0f;
                Matrix_MultVec3f(&spD0, &effectPosAndAccel);
                Math_Vec3f_Sum(&this->dyna.actor.world.pos, &effectPosAndAccel, &effectPosAndAccel);
                break;
            case BGLASTBWALL_TYPE_1:
                spD0.x = Rand_Centered() * 160.0f;
                spD0.y = 400.0f;
                spD0.z = Rand_Centered() * 160.0f;
                Matrix_MultVec3f(&spD0, &effectPosAndAccel);
                Math_Vec3f_Sum(&this->dyna.actor.world.pos, &effectPosAndAccel, &effectPosAndAccel);
                break;
        }
        func_800BBFB0(play, &effectPosAndAccel, 50.0f, 2, Rand_ZeroOne() * 120.0f + 20.0f,
                      Rand_ZeroOne() * 240.0f + 20.0f, 0);
        effectVelocity.x = Rand_ZeroOne() * 2.5f;
        effectVelocity.y = Rand_ZeroOne() * 2.5f + 1.0f;
        effectVelocity.z = Rand_ZeroOne() * 2.5f;
        randVar = Rand_ZeroOne();
        if (randVar < 0.2f) {
            var_v0 = 0x60;
        } else if (randVar < 0.6f) {
            var_v0 = 0x40;
        } else {
            var_v0 = 0x20;
        }
        EffectSsKakera_Spawn(play, &effectPosAndAccel, &effectVelocity, &effectPosAndAccel, -260, var_v0, 20, 0, 0, 10,
                             0, 0, 50, -1, OBJECT_LAST_OBJ, object_last_obj_DL_000098);
    }
}

void func_80C187E4(BgLastBwall* this) {
    this->actionFunc = func_80C187F8;
}

void func_80C187F8(BgLastBwall* this, PlayState* play) {
    if (this->colliderTris.base.acFlags & AC_HIT) {
        this->colliderTris.base.acFlags &= ~AC_HIT;
        Flags_SetSwitch(play, BGLASTBWALL_GET_SWITCH_FLAG(&this->dyna.actor));
        func_80C1886C(this, play);
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderTris.base);
    }
}

void func_80C1886C(BgLastBwall* this, PlayState* play) {
    this->actionFunc = func_80C18884;
}

void func_80C18884(BgLastBwall* this, PlayState* play) {
    if (SubS_StartCutscene(&this->dyna.actor, this->csIdList[0], CS_ID_NONE, SUBS_CUTSCENE_WITH_PLAYER)) {
        func_80C188C4(this, play);
    }
}

void func_80C188C4(BgLastBwall* this, PlayState* play) {
    DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
    this->dyna.actor.draw = NULL;
    func_80C184EC(this, play);
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WALL_BROKEN);
    this->actionFunc = BgLastBwall_DoNothing;
}

void BgLastBwall_DoNothing(BgLastBwall* this, PlayState* play) {
}

void BgLastBwall_Update(Actor* thisx, PlayState* play) {
    BgLastBwall* this = (BgLastBwall*)thisx;

    this->actionFunc(this, play);
}

void BgLastBwall_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgLastBwall* this = (BgLastBwall*)thisx;

    Gfx_DrawDListOpa(play, D_80C18A48[this->type].dList);
}
