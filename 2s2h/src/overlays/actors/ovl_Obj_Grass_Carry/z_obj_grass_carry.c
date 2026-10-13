/*
 * File: z_obj_grass_carry.c
 * Overlay: ovl_Obj_Grass_Carry
 * Description: Grass that the player is holding (spawned by Obj_Grass_Unit)
 */

#include "z_obj_grass_carry.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Obj_Grass/z_obj_grass.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_THROW_ONLY)

void ObjGrassCarry_Init(Actor* thisx, PlayState* play);
void ObjGrassCarry_Destroy(Actor* thisx, PlayState* play);
void ObjGrassCarry_Update(Actor* thisx, PlayState* play);

void ObjGrassCarry_SetupAwaitSpawnManager(ObjGrassCarry* this);
void ObjGrassCarry_AwaitSpawnManager(ObjGrassCarry* this, PlayState* play);
void ObjGrassCarry_SetupIdle(ObjGrassCarry* this);
void ObjGrassCarry_Idle(ObjGrassCarry* this, PlayState* play);
void ObjGrassCarry_Reset(ObjGrassCarry* this);
void ObjGrassCarry_Main(ObjGrassCarry* this, PlayState* play);
void ObjGrassCarry_SetupLiftedUp(ObjGrassCarry* this);
void ObjGrassCarry_LiftedUp(ObjGrassCarry* this, PlayState* play);
void ObjGrassCarry_SetupFall(ObjGrassCarry* this);
void ObjGrassCarry_Fall(ObjGrassCarry* this, PlayState* play);
void ObjGrassCarry_Draw(Actor* this, PlayState* play);

ActorProfile Obj_Grass_Carry_Profile = {
    /**/ ACTOR_OBJ_GRASS_CARRY,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(ObjGrassCarry),
    /**/ ObjGrassCarry_Init,
    /**/ ObjGrassCarry_Destroy,
    /**/ ObjGrassCarry_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00400000, 0x00, 0x02 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 10, 44, 0, { 0, 0, 0 } },
};

static s16 sRotSpeedXTarget = 0;
static s16 sRotSpeedX = 0;
static s16 sRotSpeedYTarget = 0;
static s16 sRotSpeedY = 0;

static Vec3f sUnitDirections[] = {
    { 0.0f, 0.7071f, 0.7071f },
    { 0.7071f, 0.7071f, 0.0f },
    { 0.0f, 0.7071f, -0.7071f },
    { -0.7071f, 0.7071f, 0.0f },
};

static s16 sFragmentScales[] = { 108, 102, 96, 84, 66, 55, 42, 38 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -3200, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(terminalVelocity, -17000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 400, ICHAIN_STOP),
};

void ObjGrassCarry_UpdateVelY(ObjGrassCarry* this) {
    this->actor.velocity.y += this->actor.gravity;
    if (this->actor.velocity.y < this->actor.terminalVelocity) {
        this->actor.velocity.y = this->actor.terminalVelocity;
    }
}

void ObjGrassCarry_RandScaleVecToZero(Vec3f* velocity, f32 scale) {
    scale += ((Rand_ZeroOne() * 0.2f) - 0.1f) * scale;

    velocity->x -= velocity->x * scale;
    velocity->y -= velocity->y * scale;
    velocity->z -= velocity->z * scale;
}

void ObjGrassCarry_UpdatePos(ObjGrassCarry* this) {
    ObjGrassCarry_UpdateVelY(this);
    ObjGrassCarry_RandScaleVecToZero(&this->actor.velocity, 0.05f);
    Actor_UpdatePos(&this->actor);
}

void ObjGrassCarry_UpdateBgCheckInfo(ObjGrassCarry* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 7.5f, 35.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40 |
                                UPDBGCHECKINFO_FLAG_80);
}

void ObjGrassCarry_DropCollectible(Vec3f* pos, s16 dropTable, PlayState* play) {
    if ((dropTable & 0x10) == 0) {
        Item_DropCollectibleRandom(play, NULL, pos, dropTable * 0x10);
    }
}

void ObjGrassCarry_SpawnFragments(Vec3f* basePos, PlayState* play) {
    Vec3f velocity;
    Vec3f pos;
    s32 i;
    Vec3f* dir;

    for (i = 0; i < ARRAY_COUNT(sUnitDirections); i++) {
        dir = &sUnitDirections[i];

        pos.x = basePos->x + (dir->x * 8.0f);
        pos.y = basePos->y + (dir->y * 8.0f) + 10.0f;
        pos.z = basePos->z + (dir->z * 8.0f);

        velocity.x = (Rand_ZeroOne() - 0.5f) * 8.0f;
        velocity.y = Rand_ZeroOne() * 10.0f;
        velocity.z = (Rand_ZeroOne() - 0.5f) * 8.0f;

        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -100, 64, 40, 3, 0,
                             sFragmentScales[(s32)(Rand_ZeroOne() * 111.1f) & 7], 0, 0, 80, -1, GAMEPLAY_KEEP,
                             gKakeraLeafMiddleDL);

        pos.x = basePos->x + (dir->x * 16.0f);
        pos.y = basePos->y + (dir->y * 16.0f) + 10.0f;
        pos.z = basePos->z + (dir->z * 16.0f);

        velocity.x = (Rand_ZeroOne() - 0.5f) * 6.0f;
        velocity.y = Rand_ZeroOne() * 10.0f;
        velocity.z = (Rand_ZeroOne() - 0.5f) * 6.0f;

        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -100, 64, 40, 3, 0,
                             sFragmentScales[(s32)(Rand_ZeroOne() * 111.1f) % 7], 0, 0, 80, -1, GAMEPLAY_KEEP,
                             gKakeraLeafTipDL);
    }
}

void ObjGrassCarry_Init(Actor* thisx, PlayState* play) {
    ObjGrassCarry* this = (ObjGrassCarry*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = 80;
    ObjGrassCarry_SetupAwaitSpawnManager(this);
}

void ObjGrassCarry_Destroy(Actor* thisx, PlayState* play) {
    ObjGrassCarry* this = (ObjGrassCarry*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    if (this->grassManager != NULL) {
        ObjGrassCarry** carry = &this->grassManager->grassCarry[this->actor.params];

        if (this == *carry) {
            *carry = NULL;
            this->grassManager = NULL;
        }
    }
}

void ObjGrassCarry_SetupAwaitSpawnManager(ObjGrassCarry* this) {
    this->actionFunc = ObjGrassCarry_AwaitSpawnManager;
}

void ObjGrassCarry_AwaitSpawnManager(ObjGrassCarry* this, PlayState* play) {
    if (this->grassManager != NULL) {
        if (this->actor.params != this->grassManager->activeGrassCarry) {
            ObjGrassCarry_Reset(this);
        } else {
            ObjGrassCarry_SetupIdle(this);
        }
    }
}

void ObjGrassCarry_SetupIdle(ObjGrassCarry* this) {
    this->actionFunc = ObjGrassCarry_Idle;
}

void ObjGrassCarry_Idle(ObjGrassCarry* this, PlayState* play) {
    if (this->actor.params != this->grassManager->activeGrassCarry) {
        ObjGrassCarry_Reset(this);
    }
}

void ObjGrassCarry_Reset(ObjGrassCarry* this) {
    this->actionFunc = ObjGrassCarry_Main;
    this->grassElem = NULL;
    this->actor.world.rot.x = this->actor.world.rot.y = this->actor.world.rot.z = this->actor.shape.rot.x =
        this->actor.shape.rot.y = this->actor.shape.rot.z = 0;
}

void ObjGrassCarry_Main(ObjGrassCarry* this, PlayState* play) {
    Actor* thisx = &this->actor;

    if (Actor_HasParent(thisx, play)) {
        ObjGrassCarry_SetupLiftedUp(this);
        if (this->grassElem != NULL) {
            this->grassElem->flags |= OBJ_GRASS_ELEM_REMOVED;
        }
        if (GameInteractor_Should(VB_CARRY_GRASS_DRAW_BE_OVERRIDDEN, true, this)) {
            thisx->draw = ObjGrassCarry_Draw;
        }
        thisx->shape.shadowDraw = ActorShadow_DrawCircle;
        thisx->shape.shadowAlpha = 60;
        thisx->shape.shadowScale = 1.0f;
        this->grassManager->activeGrassCarry ^= 1;
        thisx->room = -1;
        SoundSource_PlaySfxAtFixedWorldPos(play, &thisx->world.pos, 20, NA_SE_PL_PULL_UP_PLANT);
    } else if (this->grassManager->carryGrassElem != NULL) {
        Player* player = GET_PLAYER(play);

        this->grassElem = this->grassManager->carryGrassElem;
        Math_Vec3f_Copy(&thisx->world.pos, &this->grassElem->pos);
        thisx->shape.rot.y = thisx->world.rot.y = this->grassElem->rotY;
        this->dropTable = this->grassElem->dropTable;
        thisx->xzDistToPlayer = Actor_WorldDistXZToActor(&this->actor, &player->actor);
        thisx->playerHeightRel = Actor_HeightDiff(&this->actor, &player->actor);
        thisx->xyzDistToPlayerSq = SQ(thisx->xzDistToPlayer) + SQ(thisx->playerHeightRel);
        thisx->yawTowardsPlayer = Actor_WorldYawTowardActor(&this->actor, &player->actor);
        Actor_OfferCarry(&this->actor, play);
    }
}

void ObjGrassCarry_SetupLiftedUp(ObjGrassCarry* this) {
    this->actionFunc = ObjGrassCarry_LiftedUp;
}

void ObjGrassCarry_LiftedUp(ObjGrassCarry* this, PlayState* play) {
    s32 pad;

    if (Actor_HasNoParent(&this->actor, play)) {
        ObjGrassCarry_SetupFall(this);
        this->actor.velocity.x = Math_SinS(this->actor.world.rot.y) * this->actor.speed;
        this->actor.velocity.z = Math_CosS(this->actor.world.rot.y) * this->actor.speed;
        this->actor.gravity = -0.1f;
        this->actor.terminalVelocity = -17.0f;
        ObjGrassCarry_UpdatePos(this);
        ObjGrassCarry_UpdateBgCheckInfo(this, play);
        this->actor.gravity = -3.2f;
    } else {
        Vec3f pos;
        s32 bgId;

        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 20.0f;
        pos.z = this->actor.world.pos.z;
        this->actor.floorHeight =
            BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &pos);
    }
}

void ObjGrassCarry_SetupFall(ObjGrassCarry* this) {
    this->actionFunc = ObjGrassCarry_Fall;
    sRotSpeedXTarget = -0xBB8;
    sRotSpeedYTarget = (Rand_ZeroOne() - 0.5f) * 1600.0f;
    sRotSpeedX = 0;
    sRotSpeedY = 0;
    this->fallTimer = 60;
}

void ObjGrassCarry_Fall(ObjGrassCarry* this, PlayState* play) {
    s16 angle;
    s32 atHit = (this->collider.base.atFlags & AT_HIT) != 0;
    Vec3f pos;
    s32 i;

    if (atHit) {
        this->collider.base.atFlags &= ~AT_HIT;
    }

    this->fallTimer--;

    if ((this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_WALL)) || atHit ||
        (this->fallTimer <= 0)) {
        ObjGrassCarry_SpawnFragments(&this->actor.world.pos, play);
        if (GameInteractor_Should(VB_GRASS_DROP_COLLECTIBLE, true, ACTOR_OBJ_GRASS_CARRY, this)) {
            ObjGrassCarry_DropCollectible(&this->actor.world.pos, this->dropTable, play);
        }

        this->actor.draw = NULL;
        this->actor.shape.shadowDraw = NULL;

        if (this->grassManager != NULL) {
            this->actor.room = this->grassManager->actor.room;
        }

        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EV_PLANT_BROKEN);
        }
        ObjGrassCarry_SetupIdle(this);
        return;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
        pos.y = this->actor.world.pos.y + this->actor.depthInWater;

        for (angle = 0, i = 0; i < 4; i++, angle += 0x4000) {
            pos.x = this->actor.world.pos.x + (Math_SinS((s32)(Rand_ZeroOne() * 7200.0f) + angle) * 15.0f);
            pos.z = this->actor.world.pos.z + (Math_CosS((s32)(Rand_ZeroOne() * 7200.0f) + angle) * 15.0f);
            EffectSsGSplash_Spawn(play, &pos, NULL, NULL, 0, 190);
        }

        pos.x = this->actor.world.pos.x;
        pos.z = this->actor.world.pos.z;

        EffectSsGSplash_Spawn(play, &pos, NULL, NULL, 0, 280);
        EffectSsGRipple_Spawn(play, &pos, 300, 700, 0);

        this->actor.terminalVelocity = -3.0f;
        this->actor.velocity.x *= 0.1f;
        this->actor.velocity.y *= 0.4f;
        this->actor.velocity.z *= 0.1f;
        this->actor.gravity *= 0.5f;

        sRotSpeedX >>= 1;
        sRotSpeedXTarget >>= 1;
        sRotSpeedY >>= 1;
        sRotSpeedYTarget >>= 1;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER_TOUCH;
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EV_DIVE_INTO_WATER_L);
    }

    Math_StepToS(&sRotSpeedX, sRotSpeedXTarget, 0x1F4);
    Math_StepToS(&sRotSpeedY, sRotSpeedYTarget, 0xAA);
    this->actor.shape.rot.x += sRotSpeedX;
    this->actor.shape.rot.y += sRotSpeedY;
    ObjGrassCarry_UpdatePos(this);
    ObjGrassCarry_UpdateBgCheckInfo(this, play);

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void ObjGrassCarry_Update(Actor* thisx, PlayState* play) {
    ObjGrassCarry* this = (ObjGrassCarry*)thisx;

    if (this->grassManager == NULL) {
        if ((this->actionFunc != ObjGrassCarry_LiftedUp) && (this->actionFunc != ObjGrassCarry_Fall)) {
            Actor_Kill(&this->actor);
            return;
        }
    }

    this->actionFunc(this, play);
}

void ObjGrassCarry_Draw(Actor* this, PlayState* play) {
    Gfx_DrawDListOpa(play, gKusaBushType1DL);
}
