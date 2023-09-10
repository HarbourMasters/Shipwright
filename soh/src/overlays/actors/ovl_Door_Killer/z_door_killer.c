/*
 * File: z_door_killer.c
 * Overlay: ovl_Door_Killer
 * Description: Fake doors which attack player
 */

#include "z_door_killer.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"
#include "objects/object_mizu_objects/object_mizu_objects.h"
#include "objects/object_haka_door/object_haka_door.h"
#include "objects/object_door_killer/object_door_killer.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS ACTOR_FLAG_UPDATE_WHILE_CULLED

typedef enum {
    /* 0 */ DOOR_KILLER_DOOR,
    /* 1 */ DOOR_KILLER_RUBBLE_PIECE_1,
    /* 2 */ DOOR_KILLER_RUBBLE_PIECE_2,
    /* 3 */ DOOR_KILLER_RUBBLE_PIECE_3,
    /* 4 */ DOOR_KILLER_RUBBLE_PIECE_4
} DoorKillerBehaviour;

void DoorKiller_Init(Actor* thisx, PlayState* play);
void DoorKiller_Destroy(Actor* thisx, PlayState* play);
void DoorKiller_Update(Actor* thisx, PlayState* play);
void DoorKiller_Wait(DoorKiller* this, PlayState* play);
void DoorKiller_SetProperties(DoorKiller* this, PlayState* play);
void DoorKiller_DrawDoor(Actor* thisx, PlayState* play);
void DoorKiller_DrawRubble(Actor* thisx, PlayState* play);

const ActorInit Door_Killer_InitVars = {
    ACTOR_DOOR_KILLER,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_DOOR_KILLER,
    sizeof(DoorKiller),
    (ActorFunc)DoorKiller_Init,
    (ActorFunc)DoorKiller_Destroy,
    (ActorFunc)DoorKiller_Update,
    NULL,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0x0001FFEE, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 20, 100, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphItemsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 100 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphItemsInit,
};

static DoorKillerTextureEntry sDoorTextures[4] = {
    { OBJECT_HIDAN_OBJECTS, gFireTempleDoorKillerTex },
    { OBJECT_MIZU_OBJECTS, object_mizu_objects_Tex_0035C0 },
    { OBJECT_HAKA_DOOR, object_haka_door_Tex_000000 },
    { OBJECT_GAMEPLAY_KEEP, gWoodenDoorTex },
};

void DoorKiller_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    f32 randF;
    DoorKiller* this = (DoorKiller*)thisx;
    s32 bankIndex;
    s32 i;

    /*
    // Look in the object bank for one of the four objects containing door textures
    bankIndex = -1;
    for (i = 0; bankIndex < 0; i++) {
        bankIndex = Object_GetIndex(&play->objectCtx, sDoorTextures[i].objectId);
        this->textureEntryIndex = i;
    }
    */

    // For SoH where all objects are loaded, hardcode the index to match the current map.
    switch (play->sceneNum) {
        case SCENE_FIRE_TEMPLE:
            this->textureEntryIndex = 0;
            break;
        case SCENE_WATER_TEMPLE:
            this->textureEntryIndex = 1;
            break;
        case SCENE_SHADOW_TEMPLE:
        case SCENE_BOTTOM_OF_THE_WELL:
            this->textureEntryIndex = 2;
            break;
        default:
            this->textureEntryIndex = 3;
    }
    bankIndex = Object_GetIndex(&play->objectCtx, sDoorTextures[this->textureEntryIndex].objectId);

    osSyncPrintf("bank_ID = %d\n", bankIndex);
    osSyncPrintf("status = %d\n", this->textureEntryIndex);
    this->doorObjBankIndex = bankIndex;
    this->texture = sDoorTextures[this->textureEntryIndex].texture;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    Actor_SetScale(&this->actor, 0.01f);
    this->timer = 0;
    this->hasHitPlayerOrGround = 0;
    this->animStyle = 0;
    this->playerIsOpening = 0;

    switch ((u8)(this->actor.params & 0xFF)) {
        case DOOR_KILLER_DOOR:
            // `jointTable` is used for both the `jointTable` and `morphTable` args here. Because this actor doesn't
            // play any animations it does not cause problems, but it would need to be changed otherwise.
            SkelAnime_InitFlex(play, &this->skelAnime, &object_door_killer_Skel_001BC8, NULL, this->jointTable,
                               this->jointTable, 9);
            this->actionFunc = DoorKiller_SetProperties;
            DoorKiller_SetProperties(this, play);

            // manually set the overall rotation of the door
            this->jointTable[1].x = this->jointTable[1].z = 0x4000;

            // Set a cylinder collider to detect link attacks and larger sphere collider to detect explosions
            Collider_InitCylinder(play, &this->colliderCylinder);
            Collider_SetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInit);
            Collider_InitJntSph(play, &this->colliderJntSph);
            Collider_SetJntSph(play, &this->colliderJntSph, &this->actor, &sJntSphInit, this->colliderJntSphItems);
            this->colliderJntSph.elements[0].dim.worldSphere.radius = 80;
            this->colliderJntSph.elements[0].dim.worldSphere.center.x = (s16)this->actor.world.pos.x;
            this->colliderJntSph.elements[0].dim.worldSphere.center.y = (s16)this->actor.world.pos.y + 50;
            this->colliderJntSph.elements[0].dim.worldSphere.center.z = (s16)this->actor.world.pos.z;

            // If tied to a switch flag and that switch flag is already set, kill the actor.
            if ((((this->actor.params >> 8) & 0x3F) != 0x3F) &&
                Flags_GetSwitch(play, ((this->actor.params >> 8) & 0x3F))) {
                Actor_Kill(&this->actor);
            }
            break;
        case DOOR_KILLER_RUBBLE_PIECE_1:
        case DOOR_KILLER_RUBBLE_PIECE_2:
        case DOOR_KILLER_RUBBLE_PIECE_3:
        case DOOR_KILLER_RUBBLE_PIECE_4:
            this->actionFunc = DoorKiller_SetProperties;
            DoorKiller_SetProperties(this, play);

            this->actor.gravity = -0.6f;
            this->actor.minVelocityY = -6.0f;

            // Random trajectories for rubble pieces
            randF = Rand_CenteredFloat(8.0f);
            this->actor.velocity.z = Rand_ZeroFloat(8.0f);
            this->actor.velocity.x = (Math_CosS(this->actor.world.rot.y) * randF) +
                                     (Math_SinS(this->actor.world.rot.y) * this->actor.velocity.z);
            this->actor.velocity.z = (-Math_SinS(this->actor.world.rot.y) * randF) +
                                     (Math_CosS(this->actor.world.rot.y) * this->actor.velocity.z);
            this->actor.velocity.y = Rand_ZeroFloat(4.0f) + 4.0f;

            // These are used as the x,y,z rotational velocities in DoorKiller_FallAsRubble
            this->actor.world.rot.x = Rand_CenteredFloat(0x1000);
            this->actor.world.rot.y = Rand_CenteredFloat(0x1000);
            this->actor.world.rot.z = Rand_CenteredFloat(0x1000);
            this->timer = 80;
            break;
    }
}

void DoorKiller_Destroy(Actor* thisx, PlayState* play) {
    DoorKiller* this = (DoorKiller*)thisx;

    if ((thisx->params & 0xFF) == DOOR_KILLER_DOOR) {
        Collider_DestroyCylinder(play, &this->colliderCylinder);
        Collider_DestroyJntSph(play, &this->colliderJntSph);

        ResourceMgr_UnregisterSkeleton(&this->skelAnime);
    }
}

void DoorKiller_SpawnRubble(Actor* thisx, PlayState* play) {
    Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_KILLER, thisx->world.pos.x, thisx->world.pos.y + 9.0f,
                thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, thisx->shape.rot.z,
                DOOR_KILLER_RUBBLE_PIECE_1, true);
    Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_KILLER, thisx->world.pos.x + 7.88f,
                thisx->world.pos.y + 39.8f, thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y,
                thisx->shape.rot.z, DOOR_KILLER_RUBBLE_PIECE_2, true);
    Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_KILLER, thisx->world.pos.x - 15.86f,
                thisx->world.pos.y + 61.98f, thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y,
                thisx->shape.rot.z, DOOR_KILLER_RUBBLE_PIECE_3, true);
    Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_KILLER, thisx->world.pos.x + 3.72f,
                thisx->world.pos.y + 85.1f, thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y,
                thisx->shape.rot.z, DOOR_KILLER_RUBBLE_PIECE_4, true);
}

/**
 * action function for the individual door pieces that spawn and fall down when the door is destroyed
 */
void DoorKiller_FallAsRubble(DoorKiller* this, PlayState* play) {
    this->actor.velocity.y += this->actor.gravity;
    if (this->actor.velocity.y < this->actor.minVelocityY) {
        this->actor.velocity.y = this->actor.minVelocityY;
    }

    this->actor.velocity.x *= 0.98f;
    this->actor.velocity.z *= 0.98f;

    // world.rot is repurposed to be the rotation velocity for the rubble pieces
    this->actor.shape.rot.x += this->actor.world.rot.x;
    this->actor.shape.rot.y += this->actor.world.rot.y;
    this->actor.shape.rot.z += this->actor.world.rot.z;

    if (this->timer != 0) {
        this->timer--;
    } else {
        Actor_Kill(&this->actor);
    }
    func_8002D7EC(&this->actor);
}

s32 DoorKiller_IsHit(Actor* thisx, PlayState* play) {
    DoorKiller* this = (DoorKiller*)thisx;
    if ((this->colliderCylinder.base.acFlags & 2) && (this->colliderCylinder.info.acHitInfo != NULL)) {
        return true;
    }
    return false;
}

void DoorKiller_SetAC(DoorKiller* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderCylinder.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderJntSph.base);
}

void DoorKiller_Die(DoorKiller* this, PlayState* play) {
    s32 switchFlag = (this->actor.params >> 8) & 0x3F;

    // Can set a switch flag on death based on params
    if (switchFlag != 0x3F) {
        Flags_SetSwitch(play, switchFlag);
    }
    Actor_Kill(&this->actor);
    GameInteractor_ExecuteOnEnemyDefeat(&this->actor);
}

/**
 * After slamming on the floor, rise back upright
 */
void DoorKiller_RiseBackUp(DoorKiller* this, PlayState* play) {
    s32 i;
    s16 rotation;

    if (this->timer > 0) {
        this->timer--;
    } else {
        this->actionFunc = DoorKiller_Wait;
        this->timer = 16;
        DoorKiller_SetAC(this, play);
        return;
    }

    this->actor.shape.rot.x = (this->timer >= 8) ? (this->timer * 0x800) - 0x4000 : 0;

    if (this->timer >= 12) {
        rotation = (-this->timer * -500) - 8000;
    } else if (this->timer >= 8) {
        rotation = -2000;
    } else if (this->timer >= 5) {
        rotation = (this->timer * -500) + 2000;
    } else {
        rotation = 0;
    }

    for (i = 2; i < 9; i++) {
        this->jointTable[i].z = -rotation;
    }

    if (this->timer < 8) {
        rotation = Math_SinS(this->timer * 0x2000) * this->timer * 100.0f;
        for (i = 2; i < 9; i++) {
            this->jointTable[i].y = rotation;
        }
    }
}

/**
 * After wobbling, fall over and slam onto the floor, damaging the player if they are in the way. Uses manual distance
 * check for damaging player, not AT system.
 */
void DoorKiller_FallOver(DoorKiller* this, PlayState* play) {
    s32 i;
    s16 rotation;

    if (this->timer > 0) {
        this->timer--;
    } else {
        this->actionFunc = DoorKiller_RiseBackUp;
        this->timer = 16;
        return;
    }

    this->actor.shape.rot.x = (this->timer >= 4) ? 0x8000 + (-this->timer * 0x1000) : 0x4000;

    if (this->timer >= 6) {
        rotation = (-this->timer * -500) - 4000;
    } else if (this->timer >= 4) {
        rotation = -1000;
    } else if (this->timer >= 3) {
        rotation = (this->timer * -500) + 1000;
    } else {
        rotation = 0;
    }

    for (i = 2; i < 9; i++) {
        this->jointTable[i].z = rotation;
    }

    if (this->timer == 4) {
        // spawn 20 random dust particles just before slamming down
        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 1.0f, 0.0f };
        Vec3f pos;
        s32 j;
        f32 randF;

        for (j = 0; j != 20; j++) {
            pos.y = 0.0f;
            randF = Rand_CenteredFloat(40.0f);
            pos.z = Rand_ZeroFloat(100.0f);
            pos.x = (Math_CosS(this->actor.world.rot.y) * randF) + (Math_SinS(this->actor.world.rot.y) * pos.z);
            pos.z = (-Math_SinS(this->actor.world.rot.y) * randF) + (Math_CosS(this->actor.world.rot.y) * pos.z);
            velocity.x = pos.x * 0.2f;
            velocity.z = pos.z * 0.2f;
            accel.x = -(velocity.x) * 0.1f;
            accel.z = -(velocity.z) * 0.1f;
            pos.x += this->actor.world.pos.x;
            pos.y += this->actor.world.pos.y;
            pos.z += this->actor.world.pos.z;
            func_8002865C(play, &pos, &velocity, &accel, 300, 30);
        }
    }
    if (!(this->hasHitPlayerOrGround & 1)) {
        Vec3f playerPosRelToDoor;
        Player* player = GET_PLAYER(play);
        func_8002DBD0(&this->actor, &playerPosRelToDoor, &player->actor.world.pos);
        if ((fabsf(playerPosRelToDoor.y) < 20.0f) && (fabsf(playerPosRelToDoor.x) < 20.0f) &&
            (playerPosRelToDoor.z < 100.0f) && (playerPosRelToDoor.z > 0.0f)) {
            this->hasHitPlayerOrGround |= 1;
            func_8002F6D4(play, &this->actor, 6.0f, this->actor.yawTowardsPlayer, 6.0f, 16);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_KDOOR_HIT);
            func_8002F7DC(&player->actor, NA_SE_PL_BODY_HIT);
        }
    }
    if (!(this->hasHitPlayerOrGround & 1) && (this->timer == 2)) {
        this->hasHitPlayerOrGround |= 1;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_KDOOR_HIT_GND);
    }
}

/**
 * Wobble around, signifying the door is about to fall over. Does not set AC and so cannot be destroyed during this.
 */
void DoorKiller_Wobble(DoorKiller* this, PlayState* play) {
    s16 rotation;
    s32 i;

    if ((this->timer == 16) || (this->timer == 8)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_KDOOR_WAVE);
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        this->actionFunc = DoorKiller_FallOver;
        this->timer = 8;
        this->hasHitPlayerOrGround &= ~1;
        return;
    }

    rotation = Math_SinS(this->timer * 0x2000) * this->timer * 100.0f;
    for (i = 2; i < 9; i++) {
        this->jointTable[i].y = rotation;
    }
    rotation = (u16)(s32)(-Math_CosS(this->timer * 0x1000) * 1000.0f) + 1000;
    for (i = 2; i < 9; i++) {
        this->jointTable[i].z = rotation;
    }
}

/**
 * Idle while the player attempts to open the door and then begin to wobble
 */
void DoorKiller_WaitBeforeWobble(DoorKiller* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    } else {
        this->timer = 16;
        this->actionFunc = DoorKiller_Wobble;
    }
}

void DoorKiller_Wait(DoorKiller* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f playerPosRelToDoor;
    s16 angleToFacingPlayer;

    func_8002DBD0(&this->actor, &playerPosRelToDoor, &player->actor.world.pos);

    // playerIsOpening is set by player
    if (this->playerIsOpening) {
        this->actionFunc = DoorKiller_WaitBeforeWobble;
        this->timer = 10;
        this->playerIsOpening = 0;
        return;
    }

    if (DoorKiller_IsHit(&this->actor, play)) {
        // AC cylinder: wobble if hit by most weapons, die if hit by explosives or hammer
        if ((this->colliderCylinder.info.acHitInfo->toucher.dmgFlags & 0x1FFA6) != 0) {
            this->timer = 16;
            this->actionFunc = DoorKiller_Wobble;
        } else if ((this->colliderCylinder.info.acHitInfo->toucher.dmgFlags & 0x48) != 0) {
            DoorKiller_SpawnRubble(&this->actor, play);
            this->actionFunc = DoorKiller_Die;
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EN_KDOOR_BREAK);
        }
    } else if (Actor_GetCollidedExplosive(play, &this->colliderJntSph.base) != NULL) {
        // AC sphere: die if hit by explosive
        DoorKiller_SpawnRubble(&this->actor, play);
        this->actionFunc = DoorKiller_Die;
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EN_KDOOR_BREAK);
    } else if (!Player_InCsMode(play) && (fabsf(playerPosRelToDoor.y) < 20.0f) &&
               (fabsf(playerPosRelToDoor.x) < 20.0f) && (playerPosRelToDoor.z < 50.0f) &&
               (playerPosRelToDoor.z > 0.0f)) {
        // Set player properties to make the door openable if within range
        angleToFacingPlayer = player->actor.shape.rot.y - this->actor.shape.rot.y;
        if (playerPosRelToDoor.z > 0.0f) {
            angleToFacingPlayer = 0x8000 - angleToFacingPlayer;
        }
        if (ABS(angleToFacingPlayer) < 0x3000) {
            player->doorType = PLAYER_DOORTYPE_FAKE;
            player->doorDirection = (playerPosRelToDoor.z >= 0.0f) ? 1.0f : -1.0f;
            player->doorActor = &this->actor;
        }
    }

    DoorKiller_SetAC(this, play);
}

/**
 * Grabs the virtual address of the texture from the relevant door object
 */
void DoorKiller_UpdateTexture(Actor* thisx, PlayState* play) {
    DoorKiller* this = (DoorKiller*)thisx;

    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->doorObjBankIndex].segment);
    this->texture = SEGMENTED_TO_VIRTUAL(this->texture);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[thisx->objBankIndex].segment);
}

/**
 * Gets the correct door texture, defines the appropriate draw fucntion and action function based on type behaviour
 * (door or rubble).
 */
void DoorKiller_SetProperties(DoorKiller* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->doorObjBankIndex)) {
        DoorKiller_UpdateTexture(&this->actor, play);
        switch (this->actor.params & 0xFF) {
            case DOOR_KILLER_DOOR:
                this->actionFunc = DoorKiller_Wait;
                this->actor.draw = DoorKiller_DrawDoor;
                break;
            case DOOR_KILLER_RUBBLE_PIECE_1:
            case DOOR_KILLER_RUBBLE_PIECE_2:
            case DOOR_KILLER_RUBBLE_PIECE_3:
            case DOOR_KILLER_RUBBLE_PIECE_4:
                this->actionFunc = DoorKiller_FallAsRubble;
                this->actor.draw = DoorKiller_DrawRubble;
                break;
        }
    }
}

void DoorKiller_Update(Actor* thisx, PlayState* play) {
    DoorKiller* this = (DoorKiller*)thisx;

    this->actionFunc(this, play);
}

void DoorKiller_SetTexture(Actor* thisx, PlayState* play) {
    DoorKiller* this = (DoorKiller*)thisx;
    void* doorTexture = this->texture;

    OPEN_DISPS(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, doorTexture);
    CLOSE_DISPS(play->state.gfxCtx);
}

void DoorKiller_DrawDoor(Actor* thisx, PlayState* play) {
    DoorKiller* this = (DoorKiller*)thisx;

    Gfx_SetupDL_37Opa(play->state.gfxCtx);
    DoorKiller_SetTexture(&this->actor, play);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          NULL, NULL, NULL);
}

void DoorKiller_DrawRubble(Actor* thisx, PlayState* play) {
    static Gfx* dLists[] = { object_door_killer_DL_001250, object_door_killer_DL_001550, object_door_killer_DL_0017B8,
                             object_door_killer_DL_001A58 };
    s32 rubblePieceIndex = (thisx->params & 0xFF) - 1;
    DoorKiller* this = (DoorKiller*)thisx;

    if ((this->timer >= 20) || ((this->timer & 1) == 0)) {
        DoorKiller_SetTexture(thisx, play);
        Gfx_DrawDListOpa(play, dLists[rubblePieceIndex]);
    }
}
