/*
 * File: z_bg_haka_trap.c
 * Overlay: ovl_Bg_Haka_Trap
 * Description: Shadow Temple Objects
 */

#include "z_bg_haka_trap.h"
#include "objects/object_haka_objects/object_haka_objects.h"

#define FLAGS 0

void BgHakaTrap_Init(Actor* thisx, PlayState* play);
void BgHakaTrap_Destroy(Actor* thisx, PlayState* play);
void BgHakaTrap_Update(Actor* thisx, PlayState* play);
void BgHakaTrap_Draw(Actor* thisx, PlayState* play);
void BgHakaTrap_Reset(void);

void func_8087FFC0(BgHakaTrap* this, PlayState* play);
void func_808801B8(BgHakaTrap* this, PlayState* play);
void func_808802D8(BgHakaTrap* this, PlayState* play);
void func_80880484(BgHakaTrap* this, PlayState* play);
void func_808805C0(BgHakaTrap* this, PlayState* play);
void func_808806BC(BgHakaTrap* this, PlayState* play);
void func_808808F4(BgHakaTrap* this, PlayState* play);
void func_808809B0(BgHakaTrap* this, PlayState* play);
void func_808809E4(BgHakaTrap* this, PlayState* play, s16 arg2);
void func_80880AE8(BgHakaTrap* this, PlayState* play);
void func_80880C0C(BgHakaTrap* this, PlayState* play);
void func_80880D68(BgHakaTrap* this);

UNK_TYPE D_80880F30 = 0;

const ActorInit Bg_Haka_Trap_InitVars = {
    ACTOR_BG_HAKA_TRAP,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HAKA_OBJECTS,
    sizeof(BgHakaTrap),
    (ActorFunc)BgHakaTrap_Init,
    (ActorFunc)BgHakaTrap_Destroy,
    (ActorFunc)BgHakaTrap_Update,
    (ActorFunc)BgHakaTrap_Draw,
    (ActorResetFunc)BgHakaTrap_Reset,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 30, 90, 0, { 0, 0, 0 } },
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00020000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 1800.0f, 1200.0f, 0.0f }, { -1800.0f, 1200.0f, 0.0f }, { -1800.0f, 0.0f, 0.0f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00020000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 1800.0f, 1200.0f, 0.0f }, { -1800.0f, 0.0f, 0.0f }, { 1800.0f, 0.0f, 0.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    2,
    sTrisElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 80, 100, MASS_IMMOVABLE };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

UNK_TYPE D_80881014 = 0;
void BgHakaTrap_Init(Actor* thisx, PlayState* play) {
    BgHakaTrap* this = (BgHakaTrap*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(thisx, sInitChain);
    thisx->params &= 0xFF;

    if (thisx->params != HAKA_TRAP_PROPELLER) {
        Collider_InitCylinder(play, &this->colliderCylinder);
        Collider_SetCylinder(play, &this->colliderCylinder, thisx, &sCylinderInit);

        if ((thisx->params == HAKA_TRAP_GUILLOTINE_SLOW) || (thisx->params == HAKA_TRAP_GUILLOTINE_FAST)) {
            this->timer = 20;
            this->colliderCylinder.dim.yShift = 10;
            thisx->velocity.y = 0.1f;

            if (thisx->params == HAKA_TRAP_GUILLOTINE_FAST) {
                thisx->params = HAKA_TRAP_GUILLOTINE_SLOW;
                this->unk_16A = 1;
            }

            this->actionFunc = func_80880484;
        } else {
            DynaPolyActor_Init(&this->dyna, DPM_PLAYER);
            thisx->flags |= ACTOR_FLAG_4;

            if (thisx->params == HAKA_TRAP_SPIKED_BOX) {
                CollisionHeader_GetVirtual(&object_haka_objects_Col_009CD0, &colHeader);
                this->timer = 30;

                if (D_80881014 != 0) {
                    this->actionFunc = func_808808F4;
                    D_80881014 = 0;
                } else {
                    D_80881014 = 1;
                    this->actionFunc = func_808806BC;
                    thisx->velocity.y = 0.5f;
                }

                thisx->floorHeight = thisx->home.pos.y - 225.0f;
                this->unk_16A = (thisx->floorHeight + 50.0f) - 25.0f;

                this->colliderCylinder.dim.radius = 10;
                this->colliderCylinder.dim.height = 40;
            } else {
                if (thisx->params == HAKA_TRAP_SPIKED_WALL) {
                    CollisionHeader_GetVirtual(&object_haka_objects_Col_0081D0, &colHeader);
                    thisx->home.pos.x -= 200.0f;
                } else {
                    thisx->home.pos.x += 200.0f;
                    CollisionHeader_GetVirtual(&object_haka_objects_Col_008D10, &colHeader);
                }

                Collider_InitTris(play, &this->colliderSpikes);
                Collider_SetTris(play, &this->colliderSpikes, thisx, &sTrisInit, this->colliderSpikesItem);

                this->colliderCylinder.dim.radius = 18;
                this->colliderCylinder.dim.height = 115;

                this->colliderCylinder.info.toucherFlags = this->colliderCylinder.info.toucherFlags;
                this->colliderCylinder.info.toucherFlags |= TOUCH_SFX_WOOD;

                this->actionFunc = func_808801B8;
            }

            this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);
        }
    } else {
        this->timer = 40;
        this->actionFunc = func_808809B0;
        thisx->uncullZoneScale = 500.0f;
    }

    CollisionCheck_SetInfo(&thisx->colChkInfo, 0, &sColChkInfoInit);
}

void BgHakaTrap_Destroy(Actor* thisx, PlayState* play) {
    BgHakaTrap* this = (BgHakaTrap*)thisx;

    if (this->dyna.actor.params != HAKA_TRAP_PROPELLER) {
        if (this->dyna.actor.params != HAKA_TRAP_GUILLOTINE_SLOW) {
            DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
            if ((this->dyna.actor.params == HAKA_TRAP_SPIKED_WALL) ||
                (this->dyna.actor.params == HAKA_TRAP_SPIKED_WALL_2)) {
                Collider_DestroyTris(play, &this->colliderSpikes);
            }
        }

        Collider_DestroyCylinder(play, &this->colliderCylinder);
    }

    Audio_StopSfxByPos(&this->unk_16C);
}

void func_8087FFC0(BgHakaTrap* this, PlayState* play) {
    f32 cosine;
    Vec3f sp28;
    f32 sine;
    f32 zNonNegative;
    Player* player = GET_PLAYER(play);

    func_8002DBD0(&this->dyna.actor, &sp28, &player->actor.world.pos);

    sine = Math_SinS(this->dyna.actor.shape.rot.y);
    cosine = Math_CosS(this->dyna.actor.shape.rot.y);
    if (this->dyna.actor.params == HAKA_TRAP_GUILLOTINE_SLOW) {
        sp28.x = CLAMP(sp28.x, -50.0f, 50.0f);
        zNonNegative = (sp28.z >= 0.0f) ? 1.0f : -1.0f;
        sp28.z = zNonNegative * -15.0f;
    } else {
        sp28.x = -CLAMP(sp28.x, -162.0f, 162.0f);
        zNonNegative = (sp28.z >= 0.0f) ? 1.0f : -1.0f;
        sp28.z = zNonNegative * 15.0f;
    }

    this->colliderCylinder.dim.pos.x = this->dyna.actor.world.pos.x + sp28.x * cosine + sp28.z * sine;
    this->colliderCylinder.dim.pos.z = this->dyna.actor.world.pos.z + sp28.x * sine + sp28.z * cosine;
}

static UNK_TYPE D_80881018 = 0;
void func_808801B8(BgHakaTrap* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((D_80880F30 == 0) && (!Player_InCsMode(play))) {
        if (!Math_StepToF(&this->dyna.actor.world.pos.x, this->dyna.actor.home.pos.x, 0.5f)) {
            func_8002F974(&this->dyna.actor, NA_SE_EV_TRAP_OBJ_SLIDE - SFX_FLAG);
        } else if (this->dyna.actor.params == HAKA_TRAP_SPIKED_WALL) {
            D_80881018 |= 1;
        } else if (this->dyna.actor.params == HAKA_TRAP_SPIKED_WALL_2) {
            D_80881018 |= 2;
        }
    }

    func_8087FFC0(this, play);

    if (this->colliderSpikes.base.acFlags & AC_HIT) {
        this->timer = 20;
        D_80880F30 = 1;
        this->actionFunc = func_808802D8;
    } else if (D_80881018 == 3) {
        D_80881018 = 4;
        player->actor.bgCheckFlags |= 0x100;
    }
}

void func_808802D8(BgHakaTrap* this, PlayState* play) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f vector;
    f32 xScale;
    s32 i;

    if (this->timer != 0) {
        this->timer--;
    }

    func_8002F974(&this->dyna.actor, NA_SE_EV_BURN_OUT - SFX_FLAG);

    for (i = 0; i < 2; i++) {
        f32 rand = Rand_ZeroOne();

        xScale = (this->dyna.actor.params == HAKA_TRAP_SPIKED_WALL) ? -30.0f : 30.0f;

        vector.x = xScale * rand + this->dyna.actor.world.pos.x;
        vector.y = Rand_ZeroOne() * 10.0f + this->dyna.actor.world.pos.y + 30.0f;
        vector.z = Rand_CenteredFloat(320.0f) + this->dyna.actor.world.pos.z;

        EffectSsDeadDb_Spawn(play, &vector, &zeroVec, &zeroVec, 130, 20, 255, 255, 150, 170, 255, 0, 0, 1, 9,
                             false);
    }

    if (this->timer == 0) {
        D_80880F30 = 0;
        Actor_Kill(&this->dyna.actor);
    }
}

void func_80880484(BgHakaTrap* this, PlayState* play) {
    s32 sp24;
    s32 timer;

    if (this->unk_16A) {
        this->dyna.actor.velocity.y *= 3.0f;
    } else {
        this->dyna.actor.velocity.y *= 2.0f;
    }

    if (this->timer != 0) {
        this->timer -= 1;
    }

    sp24 =
        Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 185.0f, this->dyna.actor.velocity.y);
    timer = this->timer;

    if ((timer == 10 && !this->unk_16A) || (timer == 13 && this->unk_16A)) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_GUILLOTINE_BOUND);
    }

    if (this->timer == 0) {
        this->dyna.actor.velocity.y = 0.0f;
        this->timer = (this->unk_16A) ? 10 : 40;
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_GUILLOTINE_UP);
        this->actionFunc = func_808805C0;
    }

    func_8087FFC0(this, play);

    if (sp24 == 0) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderCylinder.base);
    }
}

void func_808805C0(BgHakaTrap* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->unk_16A) {
        Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 27.0f);
    } else {
        if (this->timer > 20) {
            Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 90.0f, 9.0f);
        } else {
            Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 4.5f);
        }

        if (this->timer == 20) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_GUILLOTINE_UP);
        }
    }

    if (this->timer == 0) {
        this->timer = 20;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
        this->dyna.actor.velocity.y = 0.1f;
        this->actionFunc = func_80880484;
    }

    func_8087FFC0(this, play);
}

void func_808806BC(BgHakaTrap* this, PlayState* play) {
    Vec3f vector;
    f32 tempf20;
    f32 temp;
    s32 i;
    s32 sp64;

    this->dyna.actor.velocity.y *= 1.6f;

    if (this->timer != 0) {
        this->timer--;
    }

    vector.x = this->dyna.actor.world.pos.x + 90.0f;
    vector.y = (this->dyna.actor.world.pos.y + 1.0f) + 25.0f;
    vector.z = this->dyna.actor.world.pos.z;

    tempf20 = this->dyna.actor.floorHeight;

    for (i = 0; i < 3; i++) {
        temp = BgCheck_EntityRaycastFloor4(&play->colCtx, &this->dyna.actor.floorPoly, &sp64, &this->dyna.actor,
                                           &vector) -
               25.0f;
        if (tempf20 < temp) {
            tempf20 = temp;
        }

        vector.x -= 90.0f;
    }

    if (Math_StepToF(&this->dyna.actor.world.pos.y, tempf20, this->dyna.actor.velocity.y)) {
        if (this->dyna.actor.velocity.y > 0.01f) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_TRAP_BOUND);
        }
        this->dyna.actor.velocity.y = 0.0f;
    }

    if (this->dyna.actor.velocity.y >= 0.01f) {
        func_8002F974(&this->dyna.actor, NA_SE_EV_CHINETRAP_DOWN - SFX_FLAG);
    }

    if (this->timer == 0) {
        this->dyna.actor.velocity.y = 0.0f;
        this->timer = 30;
        this->unk_16A = (s16)this->dyna.actor.world.pos.y + 50.0f;
        this->unk_16A = CLAMP_MAX(this->unk_16A, this->dyna.actor.home.pos.y);

        this->actionFunc = func_808808F4;
    }
}

void func_808808F4(BgHakaTrap* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer > 20) {
        this->unk_169 = Math_StepToF(&this->dyna.actor.world.pos.y, this->unk_16A, 15.0f);
    } else {
        this->unk_169 = Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 20.0f);
    }

    if (this->timer == 0) {
        this->timer = 30;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
        this->dyna.actor.velocity.y = 0.5f;
        this->actionFunc = func_808806BC;
    }
}

void func_808809B0(BgHakaTrap* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer -= 1;
    }

    if (this->timer == 0) {
        this->actionFunc = func_80880AE8;
    }
}

void func_808809E4(BgHakaTrap* this, PlayState* play, s16 arg2) {
    Player* player = GET_PLAYER(play);
    Vec3f sp18;

    func_8002DBD0(&this->dyna.actor, &sp18, &player->actor.world.pos);

    if ((fabsf(sp18.x) < 70.0f) && (fabsf(sp18.y) < 100.0f) && (sp18.z < 500.0f) &&
        (GET_PLAYER(play)->currentBoots != PLAYER_BOOTS_IRON)) {
        player->windSpeed = ((500.0f - sp18.z) * 0.06f + 5.0f) * arg2 * (1.0f / 0x3A00) * (2.0f / 3.0f);
        player->windDirection = this->dyna.actor.shape.rot.y;
    }
}

void func_80880AE8(BgHakaTrap* this, PlayState* play) {
    if (this->timer != 0) {
        if (Math_ScaledStepToS(&this->dyna.actor.world.rot.z, 0, this->dyna.actor.world.rot.z * 0.03f + 5.0f)) {
            this->timer = 40;
            this->actionFunc = func_808809B0;
        }
    } else {
        if (Math_ScaledStepToS(&this->dyna.actor.world.rot.z, 0x3A00, this->dyna.actor.world.rot.z * 0.03f + 5.0f)) {
            this->timer = 100;
            this->actionFunc = func_80880C0C;
        }
    }

    this->dyna.actor.shape.rot.z += this->dyna.actor.world.rot.z;
    if (this->dyna.actor.world.rot.z >= 0x1801) {
        func_8002F974(&this->dyna.actor, NA_SE_EV_WIND_TRAP - SFX_FLAG);
    }

    func_808809E4(this, play, this->dyna.actor.world.rot.z);
}

void func_80880C0C(BgHakaTrap* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    func_8002F974(&this->dyna.actor, NA_SE_EV_WIND_TRAP - SFX_FLAG);

    if (this->timer == 0) {
        this->timer = 1;
        this->actionFunc = func_80880AE8;
    }

    this->dyna.actor.shape.rot.z += this->dyna.actor.world.rot.z;
    func_808809E4(this, play, this->dyna.actor.world.rot.z);
}

void BgHakaTrap_Update(Actor* thisx, PlayState* play) {
    BgHakaTrap* this = (BgHakaTrap*)thisx;
    Vec3f* actorPos = &this->dyna.actor.world.pos;

    this->actionFunc(this, play);

    if ((this->dyna.actor.params != HAKA_TRAP_PROPELLER) && (thisx->params != HAKA_TRAP_SPIKED_BOX)) {
        this->colliderCylinder.dim.pos.y = actorPos->y;

        if ((thisx->params == HAKA_TRAP_GUILLOTINE_SLOW) || (thisx->params == HAKA_TRAP_GUILLOTINE_FAST)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderCylinder.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
        } else {
            if (this->actionFunc == func_808801B8) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderSpikes.base);
            }

            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderCylinder.base);
        }
    }
}

void func_80880D68(BgHakaTrap* this) {
    Vec3f vec3;
    Vec3f vec2;
    Vec3f vec1;

    Matrix_MultVec3f(&sTrisElementsInit[0].dim.vtx[0], &vec1);
    Matrix_MultVec3f(&sTrisElementsInit[0].dim.vtx[1], &vec2);
    Matrix_MultVec3f(&sTrisElementsInit[0].dim.vtx[2], &vec3);
    Collider_SetTrisVertices(&this->colliderSpikes, 0, &vec1, &vec2, &vec3);

    Matrix_MultVec3f(&sTrisElementsInit[1].dim.vtx[2], &vec2);
    Collider_SetTrisVertices(&this->colliderSpikes, 1, &vec1, &vec3, &vec2);
}

void BgHakaTrap_Draw(Actor* thisx, PlayState* play) {
    static Gfx* sDLists[5] = {
        object_haka_objects_DL_007610, object_haka_objects_DL_009860, object_haka_objects_DL_007EF0,
        object_haka_objects_DL_008A20, object_haka_objects_DL_0072C0,
    };
    static Color_RGBA8 D_8088103C = { 0, 0, 0, 0 };
    BgHakaTrap* this = (BgHakaTrap*)thisx;
    s32 pad;
    Vec3f sp2C;

    if (this->actionFunc == func_808802D8) {
        func_80026230(play, &D_8088103C, this->timer + 20, 0x28);
    }

    Gfx_DrawDListOpa(play, sDLists[this->dyna.actor.params]);

    if (this->actionFunc == func_808801B8) {
        func_80880D68(this);
    }

    if (this->actionFunc == func_808802D8) {
        func_80026608(play);
    }

    if ((this->actionFunc == func_808808F4) && !this->unk_169) {
        sp2C.x = this->dyna.actor.world.pos.x;
        sp2C.z = this->dyna.actor.world.pos.z;
        sp2C.y = this->dyna.actor.world.pos.y + 110.0f;

        SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &sp2C, &this->unk_16C);
        func_80078914(&this->unk_16C, NA_SE_EV_BRIDGE_CLOSE - SFX_FLAG);
    }
}

void BgHakaTrap_Reset(void) {
    D_80880F30 = 0;
    D_80881014 = 0;
    D_80881018 = 0;
}