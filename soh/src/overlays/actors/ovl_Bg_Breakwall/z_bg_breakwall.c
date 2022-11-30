/*
 * File: z_bg_breakwall.c
 * Overlay: Bg_Breakwall
 * Description: Bombable Wall
 */

#include "z_bg_breakwall.h"
#include "scenes/dungeons/ddan/ddan_scene.h"
#include "objects/object_bwall/object_bwall.h"
#include "objects/object_kingdodongo/object_kingdodongo.h"

#define FLAGS ACTOR_FLAG_4

typedef struct {
    /* 0x00 */ CollisionHeader* colHeader;
    /* 0x04 */ Gfx* dList;
    /* 0x08 */ s8 colType;
} BombableWallInfo;

void BgBreakwall_Init(Actor* thisx, PlayState* play);
void BgBreakwall_Destroy(Actor* thisx, PlayState* play);
void BgBreakwall_Update(Actor* thisx, PlayState* play);
void BgBreakwall_Draw(Actor* thisx, PlayState* play);

void BgBreakwall_WaitForObject(BgBreakwall* this, PlayState* play);
void BgBreakwall_Wait(BgBreakwall* this, PlayState* play);
void BgBreakwall_LavaCoverMove(BgBreakwall* this, PlayState* play);

const ActorInit Bg_Breakwall_InitVars = {
    ACTOR_BG_BREAKWALL,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(BgBreakwall),
    (ActorFunc)BgBreakwall_Init,
    (ActorFunc)BgBreakwall_Destroy,
    (ActorFunc)BgBreakwall_Update,
    NULL,
    NULL,
};

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000048, 0x00, 0x00 },
        { 0x00000048, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

// Replacement quad used for "Blue Fire Arrows" enhancement
static ColliderQuadInit sIceArrowQuadInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000048, 0x00, 0x00 },
        { 0x00001048, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static BombableWallInfo sBombableWallInfo[] = {
    { &object_bwall_Col_000118, object_bwall_DL_000040, 0 },
    { &object_bwall_Col_000118, object_bwall_DL_000040, 0 },
    { &object_kingdodongo_Col_0264A8, object_kingdodongo_DL_025BD0, 1 },
    { &object_kingdodongo_Col_025B64, NULL, -1 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 400, ICHAIN_STOP),
};

bool blueFireArrowsEnabledOnMudwallLoad = false;

void BgBreakwall_SetupAction(BgBreakwall* this, BgBreakwallActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgBreakwall_Init(Actor* thisx, PlayState* play) {
    BgBreakwall* this = (BgBreakwall*)thisx;
    s32 pad;
    s32 wallType = ((this->dyna.actor.params >> 13) & 3) & 0xFF;

    // Initialize this with the mud wall, so it can't be affected by toggling while the actor is loaded
    blueFireArrowsEnabledOnMudwallLoad = CVar_GetS32("gBlueFireArrows", 0) || (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_BLUE_FIRE_ARROWS));

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    this->bombableWallDList = sBombableWallInfo[wallType].dList;
    this->colType = sBombableWallInfo[wallType].colType;

    if (this->colType == 1) {
        this->dyna.actor.world.rot.x = 0x4000;
    }

    if (this->bombableWallDList != NULL) {
        if (Flags_GetSwitch(play, this->dyna.actor.params & 0x3F)) {
            Actor_Kill(&this->dyna.actor);
            return;
        }

        ActorShape_Init(&this->dyna.actor.shape, 0.0f, NULL, 0.0f);

        // If "Blue Fire Arrows" are enabled, set up this collider for them
        if (blueFireArrowsEnabledOnMudwallLoad) {
            Collider_InitQuad(play, &this->collider);
            Collider_SetQuad(play, &this->collider, &this->dyna.actor, &sIceArrowQuadInit);
        } else {
            Collider_InitQuad(play, &this->collider);
            Collider_SetQuad(play, &this->collider, &this->dyna.actor, &sQuadInit);
        }
    } else {
        this->dyna.actor.world.pos.y -= 40.0f;
    }

    this->bankIndex = (wallType >= BWALL_KD_FLOOR) ? Object_GetIndex(&play->objectCtx, OBJECT_KINGDODONGO)
                                                   : Object_GetIndex(&play->objectCtx, OBJECT_BWALL);

    if (this->bankIndex < 0) {
        Actor_Kill(&this->dyna.actor);
    } else {
        BgBreakwall_SetupAction(this, BgBreakwall_WaitForObject);
    }
}

void BgBreakwall_Destroy(Actor* thisx, PlayState* play) {
    BgBreakwall* this = (BgBreakwall*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

/**
 * Spawns fragments using ACTOR_EN_A_OBJ whenever the wall or floor is exploded.
 * Returns the last spawned actor
 */
Actor* BgBreakwall_SpawnFragments(PlayState* play, BgBreakwall* this, Vec3f* pos, f32 velocity, f32 scaleY,
                                  f32 scaleX, s32 count, f32 accel) {
    Actor* actor;
    Vec3f actorPos;
    s32 k;
    s32 j;
    s32 i;
    s16 angle1;
    s16 angle2 = 0;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f }; // unused
    Vec3s actorRotList[] = { { 0, 0, 0 }, { 0, 0, 0x4000 }, { 0, 0, -0x4000 }, { 0, 0, 0 } };
    Vec3f actorScaleList[] = {
        { 0.004f, 0.004f, 0.004f },
        { 0.004f, 0.004f, 0.004f },
        { 0.004f, 0.004f, 0.004f },
        { 0.004f, 0.004f, 0.004f },
    };
    Vec3f actorPosList[][4] = {
        { { 40.0f, 15.0f, 0.0f }, { 30.0f, 57.0f, 0.0f }, { 50.0f, 57.0f, 0.0f }, { 40.0f, 70.0f, 0.0f } },
        { { 55.0f, -15.0f, 0.0f }, { 30.0f, -32.0f, 0.0f }, { 50.0f, -32.0f, 0.0f }, { 20.0f, -10.0f, 0.0f } },
        { { -40.0f, 14.0f, 0.0f }, { -50.0f, 57.0f, 0.0f }, { -30.0f, 57.0f, 0.0f }, { -40.0f, 70.0f, 0.0f } },
        { { -55.0f, -15.0f, 0.0f }, { -55.0f, -32.0f, 0.0f }, { -30.0f, -32.0f, 0.0f }, { -20.0f, -10.0f, 0.0f } },
    };
    s32 pad;

    for (k = 3; k >= 0; k--) {
        if ((k == 0) || (k == 3)) {
            actorScaleList[k].x *= scaleX;
            actorScaleList[k].y *= scaleY;
            actorScaleList[k].z *= scaleY;
        } else {
            actorScaleList[k].x *= scaleY;
            actorScaleList[k].y *= scaleX;
            actorScaleList[k].z *= scaleX;
        }
    }

    for (i = 0; i < count; angle2 += 0x4000, i++) {
        angle1 = ABS(this->dyna.actor.world.rot.y) + angle2;
        Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                         MTXMODE_NEW);
        Matrix_RotateZYX(this->dyna.actor.world.rot.x, this->dyna.actor.world.rot.y, this->dyna.actor.world.rot.z,
                         MTXMODE_APPLY);
        Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);

        for (j = 3; j >= 0; j--) {
            for (k = 3; k >= 0; k--) {
                Matrix_MultVec3f(&actorPosList[j][k], &actorPos);
                actor =
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_A_OBJ, Rand_CenteredFloat(20.0f) + actorPos.x,
                                Rand_CenteredFloat(20.0f) + actorPos.y, Rand_CenteredFloat(20.0f) + actorPos.z,
                                actorRotList[k].x, actorRotList[k].y + angle1, actorRotList[k].z, 0x000B);

                if ((j & 1) == 0) {
                    func_80033480(play, &actorPos, velocity * 200.0f, 1, 650, 150, 1);
                }

                if (actor != NULL) {
                    actor->speedXZ = Rand_ZeroOne() + (accel * 0.6f);
                    actor->velocity.y = Rand_ZeroOne() + (accel * 0.6f);
                    actor->world.rot.y += (s16)((Rand_ZeroOne() - 0.5f) * 3000.0f);
                    actor->world.rot.x = (s16)(Rand_ZeroOne() * 3500.0f) + 2000;
                    actor->world.rot.z = (s16)(Rand_ZeroOne() * 3500.0f) + 2000;
                    actor->parent = &this->dyna.actor;
                    actor->scale.x = actorScaleList[k].x + Rand_CenteredFloat(0.001f);
                    actor->scale.y = actorScaleList[k].y + Rand_CenteredFloat(0.001f);
                    actor->scale.z = actorScaleList[k].z + Rand_CenteredFloat(0.001f);
                }
            }
        }
    }

    return actor;
}

/**
 * Sets up the collision model as well is the object dependency and action function to use.
 */
void BgBreakwall_WaitForObject(BgBreakwall* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->bankIndex)) {
        CollisionHeader* colHeader = NULL;
        s32 wallType = ((this->dyna.actor.params >> 13) & 3) & 0xFF;

        this->dyna.actor.objBankIndex = this->bankIndex;
        Actor_SetObjectDependency(play, &this->dyna.actor);
        this->dyna.actor.flags &= ~ACTOR_FLAG_4;
        this->dyna.actor.draw = BgBreakwall_Draw;
        CollisionHeader_GetVirtual(sBombableWallInfo[wallType].colHeader, &colHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

        if (wallType == BWALL_KD_LAVA_COVER) {
            BgBreakwall_SetupAction(this, BgBreakwall_LavaCoverMove);
        } else {
            BgBreakwall_SetupAction(this, BgBreakwall_Wait);
        }
    }
}

/**
 * Checks for an explosion using quad collision. If the wall or floor is exploded then it will spawn fragments and
 * despawn itself.
 */
void BgBreakwall_Wait(BgBreakwall* this, PlayState* play) {
    bool blueFireArrowHit = false;
    // If "Blue Fire Arrows" enabled, check this collider for a hit
    if (blueFireArrowsEnabledOnMudwallLoad) {
        if (this->collider.base.acFlags & AC_HIT) {
            if ((this->collider.base.ac != NULL) && (this->collider.base.ac->id == ACTOR_EN_ARROW)) {

                if (this->collider.base.ac->child != NULL &&
                    this->collider.base.ac->child->id == ACTOR_ARROW_ICE) {
                    blueFireArrowHit = true;
                }
            }
        }
    }
    
    if (this->collider.base.acFlags & 2 || blueFireArrowHit) {
        Vec3f effectPos;
        s32 wallType = ((this->dyna.actor.params >> 13) & 3) & 0xFF;

        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
        effectPos.y = effectPos.z = effectPos.x = 0.0f;

        if (this->dyna.actor.world.rot.x == 0) {
            effectPos.y = 55.0f;
        } else {
            effectPos.z = 25.0f;
            effectPos.y = -10.0f;
        }

        BgBreakwall_SpawnFragments(play, this, &effectPos, 0.0f, 6.4f, 5.0f, 1, 2.0f);
        Flags_SetSwitch(play, this->dyna.actor.params & 0x3F);

        if (wallType == BWALL_KD_FLOOR) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_EXPLOSION);
        } else {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_WALL_BROKEN);
        }

        if ((wallType == BWALL_DC_ENTRANCE) && (!(Flags_GetEventChkInf(0xB0)))) {
            Flags_SetEventChkInf(0xB0);
            Cutscene_SetSegment(play, gDcOpeningCs);
            gSaveContext.cutsceneTrigger = 1;
            Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            func_8002DF54(play, NULL, 0x31);
        }

        if (this->dyna.actor.params < 0) {
            Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }

        Actor_Kill(&this->dyna.actor);
    }
}

/**
 * Moves the actor's y position to cover the lava floor in King Dodongo's lair after he is defeated so the player is no
 * longer hurt by the lava.
 */
void BgBreakwall_LavaCoverMove(BgBreakwall* this, PlayState* play) {
    Math_StepToF(&this->dyna.actor.world.pos.y, KREG(80) + this->dyna.actor.home.pos.y, 1.0f);
}

void BgBreakwall_Update(Actor* thisx, PlayState* play) {
    BgBreakwall* this = (BgBreakwall*)thisx;

    this->actionFunc(this, play);
}

/**
 * These are the quads used for the wall and floor collision. These are used for the detecting when a bomb explosion has
 * collided with a wall, and can be adjusted for different wall or floor sizes.
 */
static Vec3f sColQuadList[][4] = {
    { { 800.0f, 1600.0f, 100.0f }, { -800.0f, 1600.0f, 100.0f }, { 800.0f, 0.0f, 100.0f }, { -800.0f, 0.0f, 100.0f } },
    { { 10.0f, 0.0f, 10.0f }, { -10.0f, 0.0f, 10.0f }, { 10.0f, 0.0f, -10.0f }, { -10.0f, 0.0f, -10.0f } },
};

void BgBreakwall_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgBreakwall* this = (BgBreakwall*)thisx;

    if (this->bombableWallDList != NULL) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, this->bombableWallDList);

        if (this->colType >= 0) {
            Vec3f colQuad[4];
            Vec3f* src = &sColQuadList[this->colType][0];
            Vec3f* dst = &colQuad[0];
            s32 i;

            for (i = 0; i < 4; i++) {
                Matrix_MultVec3f(src++, dst++);
            }

            Collider_SetQuadVertices(&this->collider, &colQuad[0], &colQuad[1], &colQuad[2], &colQuad[3]);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
