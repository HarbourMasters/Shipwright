#include "z_bg_ice_shelter.h"
#include "objects/object_ice_objects/object_ice_objects.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS 0

void BgIceShelter_Init(Actor* thisx, PlayState* play);
void BgIceShelter_Destroy(Actor* thisx, PlayState* play);
void BgIceShelter_Update(Actor* thisx, PlayState* play);
void BgIceShelter_Draw(Actor* thisx, PlayState* play);

void BgIceShelter_SetupIdle(BgIceShelter* this);
void BgIceShelter_SetupMelt(BgIceShelter* this);

void BgIceShelter_Idle(BgIceShelter* this, PlayState* play);
void BgIceShelter_Melt(BgIceShelter* this, PlayState* play);

const ActorInit Bg_Ice_Shelter_InitVars = {
    ACTOR_BG_ICE_SHELTER,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_ICE_OBJECTS,
    sizeof(BgIceShelter),
    (ActorFunc)BgIceShelter_Init,
    (ActorFunc)BgIceShelter_Destroy,
    (ActorFunc)BgIceShelter_Update,
    (ActorFunc)BgIceShelter_Draw,
    NULL,
};

static f32 sRedIceScales[] = { 0.1f, 0.06f, 0.1f, 0.1f, 0.25f };

static Color_RGBA8 sSteamPrimColor = { 250, 250, 250, 255 };
static Color_RGBA8 sSteamEnvColor = { 180, 180, 180, 255 };

static ColliderCylinderInit sCylinder1Init = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinder2Init = {
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
        { 0x4FC1FFF6, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

/**
 * Initializes either one or both cylinder colliders, depending on the actor's type.
 */
void BgIceShelter_InitColliders(BgIceShelter* this, PlayState* play) {
    static s16 cylinderRadii[] = { 47, 33, 44, 41, 100 };
    static s16 cylinderHeights[] = { 80, 54, 90, 60, 200 };
    s32 pad;
    s32 type = (this->dyna.actor.params >> 8) & 7;

    Collider_InitCylinder(play, &this->cylinder1);
    Collider_SetCylinder(play, &this->cylinder1, &this->dyna.actor, &sCylinder1Init);
    Collider_UpdateCylinder(&this->dyna.actor, &this->cylinder1);

    this->cylinder1.dim.radius = cylinderRadii[type];
    this->cylinder1.dim.height = cylinderHeights[type];

    // The wall and platform types use DynaPoly for collision, so they don't need the second collider
    if (type == RED_ICE_LARGE || type == RED_ICE_SMALL || type == RED_ICE_KING_ZORA) {
        Collider_InitCylinder(play, &this->cylinder2);
        Collider_SetCylinder(play, &this->cylinder2, &this->dyna.actor, &sCylinder2Init);
        Collider_UpdateCylinder(&this->dyna.actor, &this->cylinder2);
        this->cylinder2.dim.radius = cylinderRadii[type];
        this->cylinder2.dim.height = cylinderHeights[type];
    }

    if (type == RED_ICE_KING_ZORA) {
        this->cylinder1.dim.pos.z += 30;
        this->cylinder2.dim.pos.z += 30;
    }
}

void BgIceShelter_InitDynaPoly(BgIceShelter* this, PlayState* play, CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&this->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if (this->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG registration failed"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.id, this->dyna.actor.params);
    }
}

void BgIceShelter_RotateY(Vec3f* dest, Vec3f* src, s16 angle) {
    f32 sin = Math_SinS(angle);
    f32 cos = Math_CosS(angle);

    dest->x = (src->z * sin) + (src->x * cos);
    dest->y = src->y;
    dest->z = (src->z * cos) - (src->x * sin);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgIceShelter_Init(Actor* thisx, PlayState* play) {
    static Vec3f kzIceScale = { 0.18f, 0.27f, 0.24f };
    BgIceShelter* this = (BgIceShelter*)thisx;
    s16 type = (this->dyna.actor.params >> 8) & 7;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    if (type == RED_ICE_KING_ZORA) {
        this->dyna.actor.world.rot.x += 0xBB8;
        this->dyna.actor.world.pos.y -= 45.0f;
        this->dyna.actor.shape.rot.x = this->dyna.actor.world.rot.x;
        this->dyna.actor.world.pos.z -= 38.0f;
    }

    if (type == RED_ICE_KING_ZORA) {
        Math_Vec3f_Copy(&this->dyna.actor.scale, &kzIceScale);
    } else {
        Actor_SetScale(&this->dyna.actor, sRedIceScales[type]);
    }

    // Only 2 types use DynaPoly
    switch (type) {
        case RED_ICE_PLATFORM:
            BgIceShelter_InitDynaPoly(this, play, &gRedIcePlatformCol, 0);
            break;
        case RED_ICE_WALL:
            BgIceShelter_InitDynaPoly(this, play, &gRedIceWallCol, 0);
            break;
    }

    // All types use at least one collider
    BgIceShelter_InitColliders(this, play);

    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;

    if (!((this->dyna.actor.params >> 6) & 1) && (Flags_GetSwitch(play, this->dyna.actor.params & 0x3F))) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    BgIceShelter_SetupIdle(this);

    osSyncPrintf("(ice shelter)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void BgIceShelter_Destroy(Actor* thisx, PlayState* play) {
    BgIceShelter* this = (BgIceShelter*)thisx;

    switch ((this->dyna.actor.params >> 8) & 7) {
        case RED_ICE_PLATFORM:
        case RED_ICE_WALL:
            DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
            break;

        case RED_ICE_LARGE:
        case RED_ICE_SMALL:
        case RED_ICE_KING_ZORA:
            Collider_DestroyCylinder(play, &this->cylinder2);
            break;
    }

    Collider_DestroyCylinder(play, &this->cylinder1);
}

/**
 * Angles used to spawn steam particles in a circle.
 */
static s16 sSteamCircleAngles[] = { 0x0000, 0x4000, 0x2000, 0x6000, 0x1000, 0x5000, 0x3000, 0x7000 };

/**
 * Positions used to spawn steam particles in a straight line.
 */
static s16 sSteamLinePositions[] = { 0x0000, 0x003C, 0x0018, 0x0054, 0x0030, 0x000C, 0x0048, 0x0024 };

/**
 * Spawns steam particle effects in a circle around the ice block.
 *
 * On each frame the function is called, two particles have a chance to appear, at the same distance and opposite
 * sides from the center.
 */
void BgIceShelter_SpawnSteamAround(BgIceShelter* this, PlayState* play, f32 particleSpawningChance,
                                   f32 steamEffectScale) {
    f32 cos;
    f32 sin;
    f32 distance;
    Vec3f* icePos;
    s16 angle;
    s16 frameCounter;
    s32 i;
    s32 pad[2];
    Vec3f steamPos;
    Vec3f steamVel;
    Vec3f steamAccel;

    frameCounter = (s16)play->state.frames & 7;

    for (i = 0; i < 2; i++) {
        if (particleSpawningChance < Rand_ZeroOne()) {
            continue;
        }

        // The steamEffectScale is used here to make the particles appear at the edges of the red ice.
        distance = 42.0f * steamEffectScale;
        icePos = &this->dyna.actor.world.pos;
        angle = sSteamCircleAngles[frameCounter] + (i * 0x8000);
        sin = Math_SinS(angle);
        cos = Math_CosS(angle);

        steamPos.x = (distance * sin) + icePos->x;
        steamPos.y = (16.0f * steamEffectScale) + icePos->y;
        steamPos.z = (distance * cos) + icePos->z;

        steamVel.x = ((Rand_ZeroOne() * 3.0f) - 1.0f) * sin;
        steamVel.y = 0.0f;
        steamVel.z = ((Rand_ZeroOne() * 3.0f) - 1.0f) * cos;

        steamAccel.x = 0.07f * sin;
        steamAccel.y = 0.8f;
        steamAccel.z = 0.07f * cos;

        func_8002829C(play, &steamPos, &steamVel, &steamAccel, &sSteamPrimColor, &sSteamEnvColor,
                      450.0f * steamEffectScale, (s16)((Rand_ZeroOne() * 40.0f) + 40.0f) * steamEffectScale);
    }
}

/**
 * Spawns steam particle effects in a straight line. Only used for the ice wall type.
 *
 * On each frame the function is called, two particles have a chance to appear, at the same distance and opposite
 * sides from the midpoint.
 *
 * The last argument is unused because only one red ice type can call this function, so the scale isn't needed.
 */
void BgIceShelter_SpawnSteamAlong(BgIceShelter* this, PlayState* play, f32 particleSpawningChance, f32 unusedArg) {
    static f32 signs[] = { -1.0f, 1.0f };
    Vec3f* icePos;
    s16 frameCounter;
    s32 pad[2];
    Vec3f steamPos;
    Vec3f steamVel;
    Vec3f steamAccel;
    Vec3f posOffset;
    s32 i;

    frameCounter = (s16)play->state.frames & 7;

    for (i = 0; i < 2; i++) {
        icePos = &this->dyna.actor.world.pos;

        if (particleSpawningChance < Rand_ZeroOne()) {
            continue;
        }

        posOffset.x = (sSteamLinePositions[frameCounter] + ((Rand_ZeroOne() * 12.0f) - 6.0f)) * signs[i];
        posOffset.y = 15.0f;
        posOffset.z = ((84.0f - posOffset.x) * 0.2f) + (Rand_ZeroOne() * 20.0f);

        // Convert the position offset from relative to the ice wall to absolute.
        BgIceShelter_RotateY(&steamPos, &posOffset, this->dyna.actor.world.rot.y);
        Math_Vec3f_Sum(&steamPos, icePos, &steamPos);

        steamVel.x = (Rand_ZeroOne() * 3.0f) - 1.5f;
        steamVel.y = 0.0f;
        steamVel.z = (Rand_ZeroOne() * 3.0f) - 1.5f;

        steamAccel.x = (Rand_ZeroOne() * 0.14f) - 0.07f;
        steamAccel.y = 0.8f;
        steamAccel.z = (Rand_ZeroOne() * 0.14f) - 0.07f;

        func_8002829C(play, &steamPos, &steamVel, &steamAccel, &sSteamPrimColor, &sSteamEnvColor, 450,
                      (Rand_ZeroOne() * 40.0f) + 40.0f);
    }
}

void BgIceShelter_SetupIdle(BgIceShelter* this) {
    this->actionFunc = BgIceShelter_Idle;
    this->alpha = 255;
}

void BgIceShelter_Idle(BgIceShelter* this, PlayState* play) {
    s32 pad;
    s16 type = (this->dyna.actor.params >> 8) & 7;

    // Freeze King Zora
    if (type == RED_ICE_KING_ZORA) {
        if (this->dyna.actor.parent != NULL) {
            this->dyna.actor.parent->freezeTimer = 10000;
        }
    }

    if (GameInteractor_Should(VB_BG_ICE_SHELTER_HIT, this->cylinder1.base.acFlags & AC_HIT, this)) {
        this->cylinder1.base.acFlags &= ~AC_HIT;

        if (GameInteractor_Should(
                VB_BG_ICE_SHELTER_MELT,
                (this->cylinder1.base.ac != NULL) && (this->cylinder1.base.ac->id == ACTOR_EN_ICE_HONO), this)) {
            if (type == RED_ICE_KING_ZORA) {
                if (this->dyna.actor.parent != NULL) {
                    this->dyna.actor.parent->freezeTimer = 50;
                }
            }

            BgIceShelter_SetupMelt(this);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_ICE_MELT);
        }
    }

    switch (type) {
        case RED_ICE_LARGE:
        case RED_ICE_SMALL:
        case RED_ICE_KING_ZORA:
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->cylinder1.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->cylinder2.base);
            break;
    }

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->cylinder1.base);
}

void BgIceShelter_SetupMelt(BgIceShelter* this) {
    this->actionFunc = BgIceShelter_Melt;
    this->alpha = 255;
}

/**
 * Values added to the ice block's height every frame while it's melting.
 */
static f32 sMeltingRates[] = { -0.0015f, -0.0009f, -0.0016f, -0.0016f, -0.00375f };

/**
 * Values used to scale and position the steam effects so they match the ice block's size.
 */
static f32 sSteamEffectScales[] = { 1.0f, 0.6f, 1.2f, 1.0f, 1.8f };

/**
 * Functions used to spawn steam effects at the base of the red ice.
 */
static void (*sSteamSpawnFuncs[])(BgIceShelter* this, PlayState* play, f32 particleSpawningChance,
                                  f32 steamEffectScale) = {
    BgIceShelter_SpawnSteamAround, BgIceShelter_SpawnSteamAround, BgIceShelter_SpawnSteamAround,
    BgIceShelter_SpawnSteamAlong,  BgIceShelter_SpawnSteamAround,
};

/**
 * Progressively reduces the height and opacity of the red ice, while spawning steam effects at its base.
 */
void BgIceShelter_Melt(BgIceShelter* this, PlayState* play) {

    s32 pad;
    s32 type = (this->dyna.actor.params >> 8) & 7;
    f32 particleSpawningChance;

    this->alpha -= 5;
    this->alpha = CLAMP(this->alpha, 0, 255);

    this->dyna.actor.scale.y += sMeltingRates[type];
    this->dyna.actor.scale.y = CLAMP_MIN(this->dyna.actor.scale.y, 0.0001f);

    if (this->alpha > 80) {
        switch (type) {
            case RED_ICE_LARGE:
            case RED_ICE_SMALL:
            case RED_ICE_KING_ZORA:
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->cylinder1.base);
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->cylinder2.base);
                break;
        }
    }

    if (this->alpha > 180) {
        particleSpawningChance = 1.0f;
    } else if (this->alpha > 60) {
        particleSpawningChance = 0.5f;
    } else {
        particleSpawningChance = 0.0f;
    }

    sSteamSpawnFuncs[type](this, play, particleSpawningChance, sSteamEffectScales[type]);

    if (this->alpha <= 0) {
        if (!((this->dyna.actor.params >> 6) & 1)) {
            Flags_SetSwitch(play, this->dyna.actor.params & 0x3F);
        }

        if (type == 4) {
            Sfx_PlaySfxCentered(NA_SE_SY_CORRECT_CHIME);
        }

        Actor_Kill(&this->dyna.actor);
    }
}

void BgIceShelter_Update(Actor* thisx, PlayState* play) {
    BgIceShelter* this = (BgIceShelter*)thisx;

    this->actionFunc(this, play);
}

void BgIceShelter_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgIceShelter* this = (BgIceShelter*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    switch ((this->dyna.actor.params >> 8) & 7) {
        case RED_ICE_LARGE:
        case RED_ICE_SMALL:
        case RED_ICE_PLATFORM:
        case RED_ICE_KING_ZORA:
            func_8002ED80(&this->dyna.actor, play, 0);
            break;
    }

    if (CVarGetInteger(CVAR_COSMETIC("World.RedIce.Changed"), 0)) {
        Color_RGB8 color = CVarGetColor24(CVAR_COSMETIC("World.RedIce.Value"), (Color_RGB8){ 255, 0, 0 });
        gDPSetEnvColor(POLY_XLU_DISP++, color.r, color.g, color.b, this->alpha);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, this->alpha);
    }

    switch ((this->dyna.actor.params >> 8) & 7) {
        case RED_ICE_LARGE:
        case RED_ICE_SMALL:
        case RED_ICE_KING_ZORA:
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, -play->gameplayFrames & 0x7F,
                                          -play->gameplayFrames & 0x7F, 0x20, 0x20, 1, -play->gameplayFrames & 0x7F,
                                          play->gameplayFrames & 0x7F, 0x20, 0x20, -1, -1, -1, 1));
            gSPDisplayList(POLY_XLU_DISP++, gRedIceBlockDL);
            break;

        case RED_ICE_PLATFORM:
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, play->gameplayFrames & 0xFF, 0x40, 0x40, 1, 0,
                                          -play->gameplayFrames & 0xFF, 0x40, 0x40, 0, 1, 0, -1));
            gSPSegment(POLY_XLU_DISP++, 0x09,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, -play->gameplayFrames & 0xFF,
                                          play->gameplayFrames & 0xFF, 0x40, 0x40, 1, play->gameplayFrames & 0xFF,
                                          play->gameplayFrames & 0xFF, 0x40, 0x40, -1, 1, 1, 1));
            gSPDisplayList(POLY_XLU_DISP++, gRedIcePlatformDL);
            break;

        case RED_ICE_WALL:
            gSPDisplayList(POLY_XLU_DISP++, gRedIceWallDL);
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
