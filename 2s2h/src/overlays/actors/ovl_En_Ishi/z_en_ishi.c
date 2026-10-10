/*
 * File: z_en_ishi.c
 * Overlay: ovl_En_Ishi
 * Description: Liftable rocks and silver boulders
 */

#include "z_en_ishi.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_ishi/object_ishi.h"
#include "overlays/actors/ovl_En_Insect/z_en_insect.h"

#include "2s2h/ShipUtils.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_THROW_ONLY)

void EnIshi_Init(Actor* thisx, PlayState* play);
void EnIshi_Destroy(Actor* thisx, PlayState* play2);
void EnIshi_Update(Actor* thisx, PlayState* play);

void EnIshi_SpawnDebrisSmallRock(Actor* thisx, PlayState* play);
void EnIshi_SpawnDebrisBoulder(Actor* thisx, PlayState* play);
void EnIshi_SpawnDustSmallRock(Actor* thisx, PlayState* play);
void EnIshi_SpawnDustBoulder(Actor* thisx, PlayState* play);
void EnIshi_SetupWaitForObject(EnIshi* this);
void EnIshi_WaitForObject(EnIshi* this, PlayState* play);
void EnIshi_SetupIdle(EnIshi* this);
void EnIshi_Idle(EnIshi* this, PlayState* play);
void EnIshi_SetupHeldByPlayer(EnIshi* this);
void EnIshi_HeldByPlayer(EnIshi* this, PlayState* play);
void EnIshi_SetupThrown(EnIshi* this);
void EnIshi_Thrown(EnIshi* this, PlayState* play);
void EnIshi_SetupCutsceneExplode(EnIshi* this);
void EnIshi_CutsceneExplode(EnIshi* this, PlayState* play);
void EnIshi_SetupKill(EnIshi* this);
void EnIshi_Kill(EnIshi* this, PlayState* play);
void EnIshi_DrawGameplayKeepSmallRock(EnIshi* this, PlayState* play);
void EnIshi_DrawGameplayKeepBoulder(EnIshi* this, PlayState* play);
void EnIshi_DrawGameplayKeepObject(Actor* thisx, PlayState* play);
void EnIshi_DrawIshiObject(Actor* thisx, PlayState* play);

static s16 sIshiThrownXRotationVel = 0;
static s16 sIshiThrownYRotationVel = 0;

ActorProfile En_Ishi_Profile = {
    /**/ ACTOR_EN_ISHI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnIshi),
    /**/ EnIshi_Init,
    /**/ EnIshi_Destroy,
    /**/ EnIshi_Update,
    /**/ NULL,
};

static f32 sIshiSizes[] = { 0.1f, 0.4f };

static f32 sIshiShapeYOffsets[] = { 58.0f, 80.0f };

static f32 sIshiVelocities[] = { 0.0f, 0.005f };

static u16 sCutsceneSfxId[] = { NA_SE_EV_ROCK_BROKEN, NA_SE_EV_WALL_BROKEN };

static u8 sCutsceneSfxDuration[] = { 25, 40 };

static EnIshiSpawnFunc sIshiSpawnDebrisFuncs[] = { EnIshi_SpawnDebrisSmallRock, EnIshi_SpawnDebrisBoulder };

static EnIshiSpawnFunc sIshiSpawnDustFuncs[] = {
    EnIshi_SpawnDustSmallRock,
    EnIshi_SpawnDustBoulder,
};

static s16 sObjectIds[] = { GAMEPLAY_FIELD_KEEP, OBJECT_ISHI };

static ColliderCylinderInit sCylinderInit[] = {
    {
        {
            COL_MATERIAL_HARD,
            AT_ON | AT_TYPE_PLAYER,
            AC_ON | AC_HARD | AC_TYPE_PLAYER,
            OC1_ON | OC1_TYPE_ALL,
            OC2_TYPE_2,
            COLSHAPE_CYLINDER,
        },
        {
            ELEM_MATERIAL_UNK0,
            { 0x00400000, 0x00, 0x02 },
            { 0x01C37FBE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 10, 18, -2, { 0, 0, 0 } },
    },
    {
        {
            COL_MATERIAL_HARD,
            AT_ON | AT_TYPE_PLAYER,
            AC_ON | AC_HARD | AC_TYPE_PLAYER,
            OC1_ON | OC1_TYPE_ALL,
            OC2_TYPE_2,
            COLSHAPE_CYLINDER,
        },
        {
            ELEM_MATERIAL_UNK0,
            { 0x00400000, 0x00, 0x02 },
            { 0x01C37BB6, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 55, 70, 0, { 0, 0, 0 } },
    },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 60, MASS_IMMOVABLE };

static s16 sIshiSmallRockDebrisScales[] = { 16, 13, 11, 9, 7, 5 };

static s16 sIshiBoulderDebrisScales[] = { 145, 135, 120, 100, 70, 50, 45, 40, 35 };

static s16 sIshiItemDrops[] = {
    ITEM00_NO_DROP, ITEM00_RUPEE_BLUE, ITEM00_RUPEE_RED, ITEM00_RUPEE_PURPLE, ITEM00_ARROWS_30, ITEM00_RUPEE_GREEN,
};

// used for spawning the collectable item drops
static Vec3f sUnitVecY = { 0.0f, 1.0f, 0.0f };

static InitChainEntry sInitChain[][5] = {
    {
        ICHAIN_F32_DIV1000(gravity, -1200, ICHAIN_CONTINUE),
        ICHAIN_F32_DIV1000(terminalVelocity, -20000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDistance, 1200, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_STOP),
    },
    {
        ICHAIN_F32_DIV1000(gravity, -2500, ICHAIN_CONTINUE),
        ICHAIN_F32_DIV1000(terminalVelocity, -20000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 250, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_STOP),
    },
};

static u16 sIshiPullRockSfx[] = { NA_SE_PL_PULL_UP_ROCK, NA_SE_PL_PULL_UP_BIGROCK };

static EnIshiDrawFunc sIshiGameplayKeepObjectDrawFuncs[] = {
    EnIshi_DrawGameplayKeepSmallRock,
    EnIshi_DrawGameplayKeepBoulder,
};

void EnIshi_InitCollider(Actor* thisx, PlayState* play) {
    EnIshi* this = (EnIshi*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit[ENISHI_GET_SIZE_FLAG(&this->actor)]);
    Collider_UpdateCylinder(&this->actor, &this->collider);
}

// Return true/false if the rock is able to snap to the floor and is above BGCHECK_Y_MIN.
s32 EnIshi_SnapToFloor(EnIshi* this, PlayState* play, f32 yOffset) {
    Vec3f pos;
    s32 bgId;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + 30.0f;
    pos.z = this->actor.world.pos.z;
    this->actor.floorHeight =
        BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &pos);
    if (this->actor.floorHeight > BGCHECK_Y_MIN) {
        this->actor.world.pos.y = this->actor.floorHeight + yOffset;
        Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
        return true;
    } else {
        return false;
    }
}

void EnIshi_SpawnDebrisSmallRock(Actor* thisx, PlayState* play) {
    EnIshi* this = (EnIshi*)thisx;
    s32 i;
    s16 objectId;
    Gfx* debrisModel;
    Vec3f velocity;
    Vec3f pos;

    if (!ENISHI_GET_USE_OBJECT(&this->actor)) {
        debrisModel = gFieldSmallRockOpaDL; // same dl in gameplay keep
    } else {
        debrisModel = gSmallRockDL;
    }

    objectId = sObjectIds[ENISHI_GET_USE_OBJECT(&this->actor)];

    for (i = 0; i < ARRAY_COUNT(sIshiSmallRockDebrisScales); i++) {
        pos.x = ((Rand_ZeroOne() - 0.5f) * 8.0f) + this->actor.world.pos.x;
        pos.y = (Rand_ZeroOne() * 5.0f) + this->actor.world.pos.y + 5.0f;
        pos.z = ((Rand_ZeroOne() - 0.5f) * 8.0f) + this->actor.world.pos.z;
        Math_Vec3f_Copy(&velocity, &this->actor.velocity);

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            velocity.x *= 0.6f;
            velocity.y *= -0.3f;
            velocity.z *= 0.6f;
        } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            velocity.x *= -0.5f;
            velocity.y *= 0.5f;
            velocity.z *= -0.5f;
        }

        velocity.x += (Rand_ZeroOne() - 0.5f) * 11.0f;
        velocity.y += (Rand_ZeroOne() * 7.0f) + 6.0f;
        velocity.z += (Rand_ZeroOne() - 0.5f) * 11.0f;

        // -420: gravity
        // 40:life
        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -420, ((s32)Rand_Next() > 0) ? 65 : 33, 30, 5, 0,
                             sIshiSmallRockDebrisScales[i], 3, 10, 40, -1, objectId, debrisModel);
    }
}

// Spawn fragments after the boulder breaks apart.
void EnIshi_SpawnDebrisBoulder(Actor* thisx, PlayState* play) {
    EnIshi* this = (EnIshi*)thisx;
    Vec3f velocity;
    Vec3f pos;
    f32 randMagnitude;
    s16 angle = 0x1000;
    s32 i;
    s16 arg5;
    s16 gravity;

    for (i = 0; i < ARRAY_COUNT(sIshiBoulderDebrisScales); i++) {
        angle += 0x4E20;
        randMagnitude = Rand_ZeroOne() * 10.0f;

        pos.x = (Math_SinS(angle) * randMagnitude) + this->actor.world.pos.x;
        pos.y = (Rand_ZeroOne() * 40.0f) + this->actor.world.pos.y + 5.0f;
        pos.z = (Math_CosS(angle) * randMagnitude) + this->actor.world.pos.z;

        Math_Vec3f_Copy(&velocity, &this->actor.velocity);

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            velocity.x *= 0.9f;
            velocity.y *= -0.8f;
            velocity.z *= 0.9f;
        } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            velocity.x *= -0.9f;
            velocity.y *= 0.8f;
            velocity.z *= -0.9f;
        }

        randMagnitude = Rand_ZeroOne() * 10.0f;
        velocity.x += randMagnitude * Math_SinS(angle);
        velocity.y += (Rand_ZeroOne() * 4.0f) + (Rand_ZeroOne() * i * 0.7f);
        velocity.z += randMagnitude * Math_CosS(angle);

        if (i == 0) {
            arg5 = 41;
            gravity = -450;
        } else if (i < 4) {
            arg5 = 37;
            gravity = -380;
        } else {
            arg5 = 69;
            gravity = -320;
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, &this->actor.world.pos, gravity, arg5, 30, 5, 0,
                             sIshiBoulderDebrisScales[i], 5, 2, 70, 0, GAMEPLAY_FIELD_KEEP,
                             gFieldSilverBoulderDebrisDL);
    }
}

void EnIshi_SpawnDustSmallRock(Actor* thisx, PlayState* play) {
    Vec3f pos;

    Math_Vec3f_Copy(&pos, &thisx->world.pos);
    if (thisx->bgCheckFlags & BGCHECKFLAG_GROUND) {
        pos.x += 2.0f * thisx->velocity.x;
        pos.y -= 2.0f * thisx->velocity.y;
        pos.z += 2.0f * thisx->velocity.z;
    } else if (thisx->bgCheckFlags & BGCHECKFLAG_WALL) {
        pos.x -= 2.0f * thisx->velocity.x;
        pos.y += 2.0f * thisx->velocity.y;
        pos.z -= 2.0f * thisx->velocity.z;
    }

    // 60: distance from center
    // 3: count of particle
    // 80: scale multiplier
    // 60: scale scale step,
    func_800BBFB0(play, &pos, 60.0f, 3, 80, 60, true);
}

void EnIshi_SpawnDustBoulder(Actor* thisx, PlayState* play) {
    Vec3f pos;

    Math_Vec3f_Copy(&pos, &thisx->world.pos);
    if (thisx->bgCheckFlags & BGCHECKFLAG_GROUND) {
        pos.x += 2.0f * thisx->velocity.x;
        pos.y -= 2.0f * thisx->velocity.y;
        pos.z += 2.0f * thisx->velocity.z;
    } else if (thisx->bgCheckFlags & BGCHECKFLAG_WALL) {
        pos.x -= 2.0f * thisx->velocity.x;
        pos.y += 2.0f * thisx->velocity.y;
        pos.z -= 2.0f * thisx->velocity.z;
    }

    // 140: distance from center
    // 10: count of particle
    // 180: scale multiplier
    // 90: scale scale step,
    func_800BBFB0(play, &pos, 140.0f, 10, 180, 90, true);
}

void EnIshi_DropItem(EnIshi* this, PlayState* play) {
    if ((ENISHI_GET_SIZE_FLAG(&this->actor) == ISHI_SIZE_SMALL_ROCK) &&
        !ENISHI_GET_IGNORE_DROP_TABLE_FLAG(&this->actor)) {
        Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, ENISHI_GET_DROP_TABLE(&this->actor) * 0x10);
    }
}

void EnIshi_DropCollectable(EnIshi* this, PlayState* play) {
    s32 pad;
    s32 collectableItem00Id = sIshiItemDrops[ENISHI_GET_COLLECTABLE_ID(&this->actor)];
    Actor* item;
    Vec3f sp30;
    f32 cosResult;
    f32 verticalVelMagnitude;
    s16 temp_v1_2;

    if (collectableItem00Id >= ITEM00_RUPEE_GREEN) { // not ITEM00_NO_DROP, the 0 index of the list
        item = Item_DropCollectible(play, &this->actor.world.pos,
                                    collectableItem00Id | (ENISHI_GET_COLLECTABLE_FLAG(&this->actor) << 8));
        if (item != NULL) {
            // random direction/rotation calculations?
            // none of the other Item_DropCollectible rotation changes are similar enough
            // kinda looks like ObjSwitch_IsEyeSwitchHit code
            Matrix_Push();
            Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
            Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);
            Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
            Matrix_MultVecY(1.0f, &sp30);
            cosResult = Math3D_Cos(&sp30, &sUnitVecY);
            if (cosResult < 0.707f) {
                temp_v1_2 = Math_Atan2S_XY(sp30.z, sp30.x) - item->world.rot.y;
                if (ABS_ALT(temp_v1_2) > 0x4000) {
                    item->world.rot.y = BINANG_ROT180(item->world.rot.y);
                }
                verticalVelMagnitude = cosResult + 0.5f;
                if (verticalVelMagnitude < 0.5f) {
                    verticalVelMagnitude = 0.5f;
                }
                item->velocity.y *= verticalVelMagnitude;
            }
            Matrix_Pop();
        }
    }
}

void EnIshi_ApplyGravity(EnIshi* this) {
    this->actor.velocity.y += this->actor.gravity;
    if (this->actor.velocity.y < this->actor.terminalVelocity) {
        this->actor.velocity.y = this->actor.terminalVelocity;
    }
}

void EnIshi_SetVelocity(Vec3f* velocity, f32 scale) {
    scale += ((Rand_ZeroOne() * 0.2f) - 0.1f) * scale;

    velocity->x -= velocity->x * scale;
    velocity->y -= velocity->y * scale;
    velocity->z -= velocity->z * scale;
}

void EnIshi_SpawnBugs(EnIshi* this, PlayState* play) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_EN_INSECT, this->actor.world.pos.x,
                                          this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0,
                                          ENINSECT_PARAMS(true), this->actor.csId, this->actor.halfDaysBits,
                                          NULL) == NULL) {
            break;
        }
    }
}

s32 EnIshi_IsUnderwater(EnIshi* this, PlayState* play) {
    s32 pad;
    WaterBox* waterBox;
    f32 waterSurface;
    s32 bgId;

    if (WaterBox_GetSurfaceImpl(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &waterSurface,
                                &waterBox, &bgId) &&
        (this->actor.world.pos.y < waterSurface)) {
        return true;
    } else {
        return false;
    }
}

void EnIshi_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnIshi* this = (EnIshi*)thisx;
    s32 rockSize = ENISHI_GET_SIZE_FLAG(&this->actor);
    s32 ignoreSnapToFloor = ENISHI_GET_LEVITATE_FLAG(&this->actor);

    if ((rockSize == ISHI_SIZE_SMALL_ROCK) && ignoreSnapToFloor) {
        this->flags |= ISHI_FLAG_CUTSCENE_ROCK;
    }

    Actor_ProcessInitChain(&this->actor, sInitChain[rockSize]);

    if (play->csCtx.state != CS_STATE_IDLE) {
        this->actor.cullingVolumeDistance += 1000.0f;
    }

    if ((this->actor.shape.rot.y == ISHI_RANDOM_ROTATION_FLAG) && !(this->flags & ISHI_FLAG_CUTSCENE_ROCK)) {
        this->actor.shape.rot.y = this->actor.world.rot.y = Rand_Next() >> 0x10;
    }

    Actor_SetScale(&this->actor, sIshiSizes[rockSize]);
    EnIshi_InitCollider(&this->actor, play);

    if ((rockSize == ISHI_SIZE_SILVER_BOULDER) && Flags_GetSwitch(play, ENISHI_GET_SWITCH_FLAG(&this->actor))) {
        Actor_Kill(&this->actor);
        return;
    }

    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);

    if (rockSize == ISHI_SIZE_SILVER_BOULDER) {
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->actor.shape.shadowScale = 2.3f;
    } else {
        this->actor.shape.shadowScale = 2.4f;
        this->actor.shape.shadowAlpha = 160;
    }

    this->actor.shape.yOffset = sIshiShapeYOffsets[rockSize];

    if (!ignoreSnapToFloor && !EnIshi_SnapToFloor(this, play, 0)) {
        Actor_Kill(&this->actor);
        return;
    }

    if (EnIshi_IsUnderwater(this, play)) {
        this->flags |= ISHI_FLAG_UNDERWATER;
    }

    this->objectSlot = Object_GetSlot(&play->objectCtx, sObjectIds[ENISHI_GET_USE_OBJECT(&this->actor)]);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    EnIshi_SetupWaitForObject(this);
}

void EnIshi_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnIshi* this = (EnIshi*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnIshi_SetupWaitForObject(EnIshi* this) {
    this->actionFunc = EnIshi_WaitForObject;
}

void EnIshi_WaitForObject(EnIshi* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        if (!ENISHI_GET_USE_OBJECT(&this->actor)) {
            this->actor.draw = EnIshi_DrawGameplayKeepObject;
        } else {
            this->actor.draw = EnIshi_DrawIshiObject;
        }
        EnIshi_SetupIdle(this);
    }
}

void EnIshi_SetupIdle(EnIshi* this) {
    this->actionFunc = EnIshi_Idle;
}

void EnIshi_Idle(EnIshi* this, PlayState* play) {
    s32 pad;
    s32 rockSize = ENISHI_GET_SIZE_FLAG(&this->actor);
    s32 colliderACHit = (this->collider.base.acFlags & AC_HIT) != 0;
    s32 cutsceneRock = this->flags & ISHI_FLAG_CUTSCENE_ROCK;

    if (colliderACHit) {
        this->collider.base.acFlags &= ~AC_HIT;
    }

    if (Actor_HasParent(&this->actor, play)) { // has been picked up by player
        EnIshi_SetupHeldByPlayer(this);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, sIshiPullRockSfx[rockSize]);
        if (ENISHI_GET_SPAWN_BUGS_FLAG(&this->actor)) {
            EnIshi_SpawnBugs(this, play);
        }
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        return;
    }

    if (colliderACHit && (rockSize == ISHI_SIZE_SMALL_ROCK) &&
        (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x508)) {
        if (cutsceneRock) {
            EnIshi_DropCollectable(this, play);
            EnIshi_SetupCutsceneExplode(this);
            return;
        }
        EnIshi_DropItem(this, play);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, sCutsceneSfxDuration[rockSize],
                                           sCutsceneSfxId[rockSize]);
        sIshiSpawnDebrisFuncs[rockSize](&this->actor, play);
        sIshiSpawnDustFuncs[rockSize](&this->actor, play);
        Actor_Kill(&this->actor);
        return;
    }

    if (colliderACHit) {
        this->hitTimer = 5;
    }

    if ((this->actor.xzDistToPlayer < 600.0f) || cutsceneRock) {
        if (this->hitTimer > 0) {
            this->hitTimer--;
            if (this->hitTimer == 0) {
                this->collider.base.colMaterial = sCylinderInit[rockSize].base.colMaterial;
            } else {
                this->collider.base.colMaterial = COL_MATERIAL_NONE;
            }
        }

        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

        // pick up mechanic
        if ((this->actor.xzDistToPlayer < 90.0f) && !cutsceneRock) {
            if (rockSize == ISHI_SIZE_SILVER_BOULDER) {
                // Silver can only be picked up by goron form
                Actor_OfferGetItem(&this->actor, play, GI_NONE, 80.0f, 20.0f);
            } else {
                Actor_OfferGetItem(&this->actor, play, GI_NONE, 50.0f, 10.0f);
            }
        }
    }
}

void EnIshi_SetupHeldByPlayer(EnIshi* this) {
    this->actionFunc = EnIshi_HeldByPlayer;
    this->actor.room = -1;
    // huh? is there someplace in the game where the camera can gain enough distance
    // where the player is in camera view, in the distance, far enough to make it cull?
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
}

void EnIshi_HeldByPlayer(EnIshi* this, PlayState* play) {
    s32 pad;
    Vec3f pos;
    s32 bgId;

    if (Actor_HasNoParent(&this->actor, play)) { // player has tossed us
        this->actor.room = play->roomCtx.curRoom.num;
        if (ENISHI_GET_SIZE_FLAG(&this->actor) == ISHI_SIZE_SILVER_BOULDER) {
            Flags_SetSwitch(play, ENISHI_GET_SWITCH_FLAG(&this->actor));
        }
        EnIshi_SetupThrown(this);
        EnIshi_ApplyGravity(this);
        EnIshi_SetVelocity(&this->actor.velocity, sIshiVelocities[ENISHI_GET_SIZE_FLAG(&this->actor)]);
        Actor_UpdatePos(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 7.5f, 35.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40 |
                                    UPDBGCHECKINFO_FLAG_80);
    } else { // still being held, follow player
        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 20.0f;
        pos.z = this->actor.world.pos.z;
        // why does it track floor height? it uses bgcollision to detect floor hit in Thrown
        this->actor.floorHeight =
            BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &pos);
    }
}

void EnIshi_SetupThrown(EnIshi* this) {
    f32 randomStart;

    this->actor.velocity.x = Math_SinS(this->actor.world.rot.y) * this->actor.speed;
    this->actor.velocity.z = Math_CosS(this->actor.world.rot.y) * this->actor.speed;
    if (!ENISHI_GET_SIZE_FLAG(&this->actor)) {
        randomStart = Rand_ZeroOne() - 0.9f;
        sIshiThrownXRotationVel = randomStart * 11000.0f;
        sIshiThrownYRotationVel = ((Rand_ZeroOne() - 0.5f) * 3000.0f) * (fabsf(randomStart) + 0.1f);
    } else {
        randomStart = Rand_ZeroOne() - 0.5f;
        sIshiThrownXRotationVel = randomStart * 6000.0f;
        sIshiThrownYRotationVel = ((Rand_ZeroOne() - 0.5f) * 1200.0f) * (fabsf(randomStart) + 0.5f);
    }

    this->actor.colChkInfo.mass = 200;
    this->thrownTimer = 100;
    this->actionFunc = EnIshi_Thrown;
}

void EnIshi_Thrown(EnIshi* this, PlayState* play) {
    s32 pad;
    s32 rockSize = ENISHI_GET_SIZE_FLAG(&this->actor);
    s16 pad2;
    s32 i;
    s16 spashAngle;
    Vec3f pos;
    s32 colliderATHit = (this->collider.base.atFlags & AT_HIT) != 0;

    if (colliderATHit) {
        this->collider.base.atFlags &= ~AT_HIT;
    }

    this->thrownTimer--;

    if ((this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_WALL)) || colliderATHit ||
        (this->thrownTimer <= 0)) {
        EnIshi_DropItem(this, play);
        sIshiSpawnDebrisFuncs[rockSize](&this->actor, play);

        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, sCutsceneSfxDuration[rockSize],
                                               sCutsceneSfxId[rockSize]);
            sIshiSpawnDustFuncs[rockSize](&this->actor, play);
        }

        if (rockSize == ISHI_SIZE_SILVER_BOULDER) {
            s16 quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);

            Quake_SetSpeed(quakeIndex, 17232);
            Quake_SetPerturbations(quakeIndex, 3, 0, 0, 0);
            Quake_SetDuration(quakeIndex, 7);

            Rumble_Request(this->actor.xyzDistToPlayerSq, 255, 20, 150);
        }

        Actor_Kill(&this->actor);
        return;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
        if (rockSize == ISHI_SIZE_SMALL_ROCK) {
            pos.x = this->actor.world.pos.x;
            pos.y = this->actor.world.pos.y + this->actor.depthInWater;
            pos.z = this->actor.world.pos.z;
            EffectSsGSplash_Spawn(play, &pos, NULL, NULL, 0, 350);
            EffectSsGRipple_Spawn(play, &pos, 150, 650, 0);
        } else {
            pos.y = this->actor.world.pos.y + this->actor.depthInWater;

            for (spashAngle = 0, i = 0; i < 11; i++, spashAngle += 0x1746) {
                pos.x = this->actor.world.pos.x + (Math_SinS((s32)(Rand_ZeroOne() * 2000.0f) + spashAngle) * 50.0f);
                pos.z = this->actor.world.pos.z + (Math_CosS((s32)(Rand_ZeroOne() * 2000.0f) + spashAngle) * 50.0f);
                EffectSsGSplash_Spawn(play, &pos, NULL, NULL, 0, 350);
            }

            pos.x = this->actor.world.pos.x;
            pos.z = this->actor.world.pos.z;
            EffectSsGRipple_Spawn(play, &pos, 500, 900, 4);
        }

        this->actor.terminalVelocity = -6.0f;
        this->actor.velocity.x *= 0.12f;
        this->actor.velocity.y *= 0.4f;
        this->actor.velocity.z *= 0.12f;
        this->actor.gravity *= 0.5f;

        sIshiThrownXRotationVel >>= 2;
        sIshiThrownYRotationVel >>= 2;

        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EV_DIVE_INTO_WATER_L);
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER_TOUCH;
    }

    Math_StepToF(&this->actor.shape.yOffset, 0.0f, 2.0f);
    EnIshi_ApplyGravity(this);
    EnIshi_SetVelocity(&this->actor.velocity, sIshiVelocities[rockSize]);
    Actor_UpdatePos(&this->actor);
    this->actor.shape.rot.x += sIshiThrownXRotationVel;
    this->actor.shape.rot.y += sIshiThrownYRotationVel;
    Actor_UpdateBgCheckInfo(play, &this->actor, 7.5f, 35.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40 |
                                UPDBGCHECKINFO_FLAG_80);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
}

// This should trigger cutscenes, but I couldnt get it to trigger in testing
// and this should trigger for all wall rocks in TF
void EnIshi_SetupCutsceneExplode(EnIshi* this) {
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    CutsceneManager_Queue(this->actor.csId);
    this->actionFunc = EnIshi_CutsceneExplode;
}

void EnIshi_CutsceneExplode(EnIshi* this, PlayState* play) {
    s32 pad;
    // Weirdly, you shouldnt be able to get here as silver boulder,
    // it's explicitly ignored in EnIshi_Idle, the only path here
    s32 rockSize = ENISHI_GET_SIZE_FLAG(&this->actor);

    if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, sCutsceneSfxDuration[rockSize],
                                           sCutsceneSfxId[rockSize]);
        sIshiSpawnDebrisFuncs[rockSize](&this->actor, play);
        sIshiSpawnDustFuncs[rockSize](&this->actor, play);
        this->actor.draw = NULL;
        EnIshi_SetupKill(this);
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void EnIshi_SetupKill(EnIshi* this) {
    this->actionFunc = EnIshi_Kill;
}

void EnIshi_Kill(EnIshi* this, PlayState* play) {
    if (this->actor.csId <= CS_ID_NONE) {
        Actor_Kill(&this->actor);
    } else if (CutsceneManager_GetCurrentCsId() != this->actor.csId) {
        Actor_Kill(&this->actor);
    }
}

void EnIshi_Update(Actor* thisx, PlayState* play) {
    EnIshi* this = (EnIshi*)thisx;

    this->actionFunc(this, play);
}

void EnIshi_DrawGameplayKeepSmallRock(EnIshi* this, PlayState* play) {
    s32 pad;
    s32 alpha;

    Ship_ExtendedCullingActorAdjustProjectedZ(&this->actor);

    if ((this->actor.projectedPos.z <= 1200.0f) ||
        ((this->flags & ISHI_FLAG_UNDERWATER) && (this->actor.projectedPos.z < 1300.0f))) {
        Gfx_DrawDListOpa(play, gFieldSmallRockOpaDL);
        return;
    }

    if (this->actor.projectedPos.z < 1300.0f) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        alpha = (1300.0f - this->actor.projectedPos.z) * 2.55f; // lower alpha as the object falls toward the core

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s32)alpha);
        gSPDisplayList(POLY_XLU_DISP++, gFieldSmallRockXluDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }

    Ship_ExtendedCullingActorRestoreProjectedPos(play, &this->actor);
}

void EnIshi_DrawGameplayKeepBoulder(EnIshi* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Ship_ExtendedCullingActorAdjustProjectedZ(&this->actor);

    if ((this->actor.projectedPos.z <= 2150.0f) ||
        ((this->flags & ISHI_FLAG_UNDERWATER) && (this->actor.projectedPos.z < 2250.0f))) {
        this->actor.shape.shadowAlpha = 160;

        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, D_801AEFA0); // empty displaylist
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
        gSPDisplayList(POLY_OPA_DISP++, gFieldSilverBoulderDL);

    } else if (this->actor.projectedPos.z < 2250.0f) { // falling after breaking
        f32 alpha = (2250.0f - this->actor.projectedPos.z) * 2.55f;

        this->actor.shape.shadowAlpha = alpha * (160.0f / 255.0f);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gSPSegment(POLY_XLU_DISP++, 0x08, D_801AEF88); // in z_actor, transparency dl I think
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s32)alpha);
        gSPDisplayList(POLY_XLU_DISP++, gFieldSilverBoulderDL);

    } else {
        this->actor.shape.shadowAlpha = 0;
    }

    Ship_ExtendedCullingActorRestoreProjectedPos(play, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnIshi_DrawGameplayKeepObject(Actor* thisx, PlayState* play) {
    EnIshi* this = (EnIshi*)thisx;

    sIshiGameplayKeepObjectDrawFuncs[ENISHI_GET_SIZE_FLAG(&this->actor)](this, play);
}

void EnIshi_DrawIshiObject(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gSmallRockDL);
}
