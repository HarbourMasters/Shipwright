/*
 * File: z_en_hakurock.c
 * Overlay: En_Hakurock
 * Description: Interactive rocks in Goht's fight
 *
 * This actor is responsible for most of the various rocks that the player and/or Goht can interact with. It can be
 * divided into three main subtypes:
 * - Boulder: These are the large rocks that Goht can kick up while it's running. These boulders fly through the air and
 *   damage the player when they collide with them.
 * - Stalactite: These are the stalactites that fall from the ceiling during the Goht fight. As it falls through the
 *   air, it can be destroyed if it touches Goht. Once it lands and gets stuck in the ground, it can stop the player's
 *   Goron Roll if they run into it, and it can be destroyed by Goht or other falling stalactites.
 * - LargeStalactite: These are the stalactites that keep the player confined to a small part of the boss room when they
 *   first enter it. Goht destroys these stalactites as part of its intro cutscene after it's been thawed, allowing the
 *   player to access the rest of the room.
 *
 * This actor heavily assumes that it will only be spawned by Goht's (BossHakugin) actor, so it won't act correctly if
 * spawned directly within the scene or by any other actor. When this actor is visibly destroyed, it's not actually
 * killed; its params are set to a specific value, and it's placed into a waiting state until BossHakugin updates its
 * params again to reuse the instance. Additionally, the rock effects that appear when this actor is destroyed are
 * spawned by directly writing values to the parent actor's instance without ever checking its ID or if this actor even
 * has a parent; it just assumes its parent is a BossHakugin instance and tries to update its rock effects array
 * accordingly.
 */

#include "z_en_hakurock.h"
#include "overlays/actors/ovl_Boss_Hakugin/z_boss_hakugin.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_boss_hakugin/object_boss_hakugin.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnHakurock_Init(Actor* thisx, PlayState* play);
void EnHakurock_Destroy(Actor* thisx, PlayState* play);
void EnHakurock_Update(Actor* thisx, PlayState* play);

void EnHakurock_AddRockEffect(BossHakugin* parent, Vec3f* pos, s32 type);
void EnHakurock_SpawnEffect(EnHakurock* this, s32 type);
void EnHakurock_SetupWaitForSignal(EnHakurock* this);
void EnHakurock_WaitForSignal(EnHakurock* this, PlayState* play);
void EnHakurock_Boulder_SetupMove(EnHakurock* this);
void EnHakurock_Boulder_Move(EnHakurock* this, PlayState* play);
void EnHakurock_Stalactite_SetupFall(EnHakurock* this, PlayState* play);
void EnHakurock_Stalactite_Fall(EnHakurock* this, PlayState* play);
void EnHakurock_Stalactite_SetupStuckInGround(EnHakurock* this);
void EnHakurock_Stalactite_StuckInGround(EnHakurock* this, PlayState* play);
void EnHakurock_LargeStalactite_SetupWait(EnHakurock* this);
void EnHakurock_LargeStalactite_Wait(EnHakurock* this, PlayState* play);
void EnHakurock_DrawBoulder(Actor* thisx, PlayState* play);
void EnHakurock_DrawStalactite(Actor* thisx, PlayState* play);

typedef enum EnHakurockEffectType {
    /* 0 */ EN_HAKUROCK_EFFECT_TYPE_BOULDER_DESTROYED,
    /* 1 */ EN_HAKUROCK_EFFECT_TYPE_FALLING_STALACTITE_DESTROYED,
    /* 2 */ EN_HAKUROCK_EFFECT_TYPE_STALACTITE_STUCK,
    /* 3 */ EN_HAKUROCK_EFFECT_TYPE_STALACTITE_DESTROYED
} EnHakurockEffectType;

ActorProfile En_Hakurock_Profile = {
    /**/ ACTOR_EN_HAKUROCK,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_BOSS_HAKUGIN,
    /**/ sizeof(EnHakurock),
    /**/ EnHakurock_Init,
    /**/ EnHakurock_Destroy,
    /**/ EnHakurock_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HARD,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x00, 0x00 },
        { 0xF3CFBBFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 60, 60, -30, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 60, 60, MASS_IMMOVABLE };

void EnHakurock_Init(Actor* thisx, PlayState* play) {
    EnHakurock* this = (EnHakurock*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 52.0f);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);

    if (EN_HAKUROCK_GET_TYPE(&this->actor) == EN_HAKUROCK_TYPE_BOULDER) {
        this->actor.gravity = -1.5f;
    } else {
        this->collider.base.ocFlags1 &= ~OC1_NO_PUSH;
        this->actor.terminalVelocity = -100.0f;
        this->actor.gravity = -7.0f;
    }

    EnHakurock_SetupWaitForSignal(this);
}

void EnHakurock_Destroy(Actor* thisx, PlayState* play) {
    EnHakurock* this = (EnHakurock*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

/**
 * Adds a single effect of the given type to the BossHakugin's rock effect array. This is more of an "internal" function
 * only intended to be called by EnHakurock_SpawnEffect, which calls this function numerous times (and, sometimes, with
 * a variety of different positions) to create a more complex-looking effect.
 */
void EnHakurock_AddRockEffect(BossHakugin* parent, Vec3f* pos, s32 type) {
    s32 i;

    for (i = 0; i < GOHT_ROCK_EFFECT_COUNT; i++) {
        GohtRockEffect* rockEffect = &parent->rockEffects[i];

        if (rockEffect->timer < 0) {
            VecGeo velocityGeo;

            Math_Vec3f_Copy(&rockEffect->pos, pos);

            velocityGeo.pitch = Rand_S16Offset(0x1000, 0x3000);
            velocityGeo.yaw = Rand_Next() >> 0x10;
            velocityGeo.r = Rand_ZeroFloat(5.0f) + 10.0f;
            rockEffect->velocity.x = velocityGeo.r * Math_CosS(velocityGeo.pitch) * Math_SinS(velocityGeo.yaw);
            rockEffect->velocity.y = velocityGeo.r * Math_SinS(velocityGeo.pitch);
            rockEffect->velocity.z = velocityGeo.r * Math_CosS(velocityGeo.pitch) * Math_CosS(velocityGeo.yaw);

            if ((type == EN_HAKUROCK_EFFECT_TYPE_FALLING_STALACTITE_DESTROYED) ||
                (type == EN_HAKUROCK_EFFECT_TYPE_STALACTITE_DESTROYED)) {
                rockEffect->scale = ((Rand_ZeroFloat(5.0f) + 25.0f) * 0.0012f);
                rockEffect->pos.x = pos->x + ((Rand_ZeroFloat(2.0f) + 9.0f) * rockEffect->velocity.x);
                rockEffect->pos.y = pos->y + ((Rand_ZeroFloat(2.0f) + 3.0f) * rockEffect->velocity.y);
                rockEffect->pos.z = pos->z + ((Rand_ZeroFloat(2.0f) + 9.0f) * rockEffect->velocity.z);
                rockEffect->type = GOHT_ROCK_EFFECT_TYPE_STALACTITE;
            } else {
                rockEffect->scale = ((Rand_ZeroFloat(5.0f) + 18.0f) * 0.0001f);
                rockEffect->pos.x = pos->x + ((Rand_ZeroFloat(2.0f) + 3.0f) * rockEffect->velocity.x);
                rockEffect->pos.y = pos->y + ((Rand_ZeroFloat(3.0f) + 1.0f) * rockEffect->velocity.y);
                rockEffect->pos.z = pos->z + ((Rand_ZeroFloat(2.0f) + 3.0f) * rockEffect->velocity.z);
                rockEffect->type = GOHT_ROCK_EFFECT_TYPE_BOULDER;
            }

            rockEffect->rot.x = (s32)Rand_Next() >> 0x10;
            rockEffect->rot.y = (s32)Rand_Next() >> 0x10;
            rockEffect->rot.z = (s32)Rand_Next() >> 0x10;
            rockEffect->timer = 40;
            return;
        }
    }
}

/**
 * This function spawns various effects depending on what type is supplied. It assumes that the parent is a BossHakugin
 * instance and calls EnHakurock_AddRockEffect multiple times to create effects with numerous rocks.
 */
void EnHakurock_SpawnEffect(EnHakurock* this, s32 type) {
    Vec3f pos;
    s32 i;
    s32 j;
    f32 offset;
    s32 count;

    if (type == EN_HAKUROCK_EFFECT_TYPE_BOULDER_DESTROYED) {
        for (i = 0; i < 20; i++) {
            EnHakurock_AddRockEffect((BossHakugin*)this->actor.parent, &this->actor.world.pos, type);
        }
    } else if (type == EN_HAKUROCK_EFFECT_TYPE_STALACTITE_STUCK) {
        for (i = 0; i < 10; i++) {
            EnHakurock_AddRockEffect((BossHakugin*)this->actor.parent, &this->actor.world.pos, type);
        }
    } else {
        Math_Vec3f_Copy(&pos, &this->actor.world.pos);
        offset = this->actor.scale.x * 600.0f;
        if (type == EN_HAKUROCK_EFFECT_TYPE_FALLING_STALACTITE_DESTROYED) {
            pos.y -= offset;
            i = 0;
        } else {
            i = 1;
        }

        for (; i < 3; i++) {
            count = (i * 5) + 5;
            for (j = 0; j < count; j++) {
                EnHakurock_AddRockEffect((BossHakugin*)this->actor.parent, &pos, type);
            }

            pos.y += offset;
        }
    }
}

void EnHakurock_SetupWaitForSignal(EnHakurock* this) {
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->collider.base.atFlags &= ~AT_HIT;
    this->collider.base.ocFlags1 &= ~OC1_HIT;
    this->actor.draw = NULL;
    this->actor.params = EN_HAKUROCK_TYPE_NONE;
    this->actionFunc = EnHakurock_WaitForSignal;
}

/**
 * Waits and does nothing until its params are changed, after which it will setup the appropriate action function for
 * its new type. When an instance of this actor is visibly destroyed, it is put into this state to wait for BossHakugin
 * to signal that its instance should be reused. If an instance of this actor is spawned by anything other than Goht, it
 * will end up stuck in this state forever once it's destroyed.
 */
void EnHakurock_WaitForSignal(EnHakurock* this, PlayState* play) {
    if (EN_HAKUROCK_GET_TYPE(&this->actor) == EN_HAKUROCK_TYPE_BOULDER) {
        EnHakurock_Boulder_SetupMove(this);
    } else if (EN_HAKUROCK_GET_TYPE(&this->actor) == EN_HAKUROCK_TYPE_FALLING_STALACTITE) {
        EnHakurock_Stalactite_SetupFall(this, play);
    } else if (EN_HAKUROCK_GET_TYPE(&this->actor) == EN_HAKUROCK_TYPE_LARGE_STALACTITE) {
        EnHakurock_LargeStalactite_SetupWait(this);
    }
}

void EnHakurock_Boulder_SetupMove(EnHakurock* this) {
    this->actor.params = EN_HAKUROCK_TYPE_BOULDER;
    this->actor.draw = EnHakurock_DrawBoulder;
    this->actor.speed = Rand_ZeroFloat(3.5f) + 2.5f;
    this->actor.velocity.y = Rand_ZeroFloat(4.5f) + 18.0f;
    Actor_SetScale(&this->actor, (Rand_ZeroFloat(5.0f) + 15.0f) * 0.001f);
    this->actor.world.rot.y = ((s32)Rand_Next() >> 0x12) + this->actor.parent->shape.rot.y + 0x8000;
    this->actor.shape.rot.x = (s32)Rand_Next() >> 0x10;
    this->actor.shape.rot.y = (s32)Rand_Next() >> 0x10;
    this->actor.shape.rot.z = (s32)Rand_Next() >> 0x10;
    this->collider.dim.radius = (this->actor.scale.x * 2500.0f);
    this->collider.dim.yShift = -this->collider.dim.radius;
    this->collider.dim.height = this->collider.dim.radius * 2;
    this->timer = 10;
    Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
    this->actionFunc = EnHakurock_Boulder_Move;
}

/**
 * Flies through the air while spinning until it hits something, then it breaks apart into a bunch of small rocks.
 */
void EnHakurock_Boulder_Move(EnHakurock* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    }

    this->actor.shape.rot.x += 0x700;
    this->actor.shape.rot.y += 0x900;
    this->actor.shape.rot.z += 0xB00;

    if ((this->collider.base.atFlags & AT_HIT) || ((this->timer == 0) && (this->collider.base.ocFlags1 & OC1_HIT)) ||
        ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.velocity.y < 0.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_ROCK_BROKEN);
        EnHakurock_SpawnEffect(this, EN_HAKUROCK_EFFECT_TYPE_BOULDER_DESTROYED);
        EnHakurock_SetupWaitForSignal(this);
    }
}

void EnHakurock_Stalactite_SetupFall(EnHakurock* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 angle;

    this->actor.draw = EnHakurock_DrawStalactite;
    angle = ((s32)Rand_Next() >> 0x13) + player->actor.shape.rot.y;
    this->actor.shape.rot.y = (s32)Rand_Next() >> 0x10;
    this->actor.world.pos.x = (Math_SinS(angle) * 600.0f) + player->actor.world.pos.x;
    this->actor.world.pos.y = player->actor.world.pos.y + 700.0f;
    this->actor.world.pos.z = (Math_CosS(angle) * 600.0f) + player->actor.world.pos.z;
    this->actor.shape.shadowScale = 5.0f;
    this->actor.velocity.y = -10.0f;
    Actor_SetScale(&this->actor, (Rand_ZeroFloat(5.0f) + 17.0f) * 0.01f);
    this->actor.scale.x *= 0.6f;
    this->actor.scale.z *= 0.6f;
    this->collider.dim.radius = (this->actor.scale.x * 150.0f);
    this->collider.dim.yShift = (this->actor.scale.y * -750.0f);
    this->collider.dim.height = this->collider.dim.yShift * -2;
    Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
    this->actor.params = EN_HAKUROCK_TYPE_FALLING_STALACTITE;
    this->actionFunc = EnHakurock_Stalactite_Fall;
}

/**
 * Falls straight down until it hits either Goht or the ground. If it hits Goht, it will break apart into a bunch of
 * stalactite-shaped fragments. If it hits the ground, it will kick up some rocks and start transitioning to being stuck
 * in the ground.
 */
void EnHakurock_Stalactite_Fall(EnHakurock* this, PlayState* play) {
    if ((this->collider.base.ocFlags1 & OC1_HIT) && (this->collider.base.oc == this->actor.parent)) {
        EnHakurock_SpawnEffect(this, EN_HAKUROCK_EFFECT_TYPE_FALLING_STALACTITE_DESTROYED);
        EnHakurock_SetupWaitForSignal(this);
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        EnHakurock_SpawnEffect(this, EN_HAKUROCK_EFFECT_TYPE_STALACTITE_STUCK);
        Actor_PlaySfx(&this->actor, NA_SE_EV_OBJECT_STICK);
        EnHakurock_Stalactite_SetupStuckInGround(this);
    }
}

void EnHakurock_Stalactite_SetupStuckInGround(EnHakurock* this) {
    this->actor.params = EN_HAKUROCK_TYPE_STUCK_STALACTITE;
    this->timer = 10;
    this->actor.shape.shadowScale = 0.0f;
    this->actor.world.pos.y += 4.0f;
    this->actionFunc = EnHakurock_Stalactite_StuckInGround;
}

/**
 * Shakes vertically for 10 frames to make it look like it's settling into the ground. If it's struck by Goht or a
 * falling stalactite, it will break apart into a bunch of stalactite-shaped fragments. If the player runs into it while
 * Goron Rolling, it will stop their roll and bounce them back. If the stalactite gets too far from the player and is
 * "behind" the camera, it will disappear and allow Goht to reuse this instance for another stalactite.
 */
void EnHakurock_Stalactite_StuckInGround(EnHakurock* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer > 0) {
        this->timer--;
        if ((this->timer % 2) == 0) {
            this->actor.world.pos.y = (Math_SinF(this->timer * (M_PIf / 20)) * 4.0f) + this->actor.floorHeight;
        } else {
            this->actor.world.pos.y = this->actor.floorHeight;
        }
    }

    if (this->collider.base.ocFlags1 & OC1_HIT) {
        if ((this->collider.base.oc == this->actor.parent) ||
            ((this->collider.base.oc->id == ACTOR_EN_HAKUROCK) &&
             (EN_HAKUROCK_GET_TYPE(this->collider.base.oc) == EN_HAKUROCK_TYPE_FALLING_STALACTITE))) {
            EnHakurock_SpawnEffect(this, EN_HAKUROCK_EFFECT_TYPE_STALACTITE_DESTROYED);
            EnHakurock_SetupWaitForSignal(this);
        } else if ((&player->actor == this->collider.base.oc) &&
                   (player->stateFlags3 & (PLAYER_STATE3_1000 | PLAYER_STATE3_80000)) && (player->speedXZ > 8.0f)) {
            player->unk_B08 = player->speedXZ = -5.0f;
            player->unk_B0C += player->speedXZ * 0.05f;
            player->actor.velocity.y = 5.0f;
            player->yaw = player->actor.world.rot.y;
            player->actor.home.rot.y = player->actor.world.rot.y;
            player->actor.shape.rot.y = player->actor.world.rot.y;
            player->unk_B8C = 4;
        }
    } else if ((this->actor.xzDistToPlayer > 1000.0f) && (this->actor.projectedPos.z < 0.0f)) {
        EnHakurock_SetupWaitForSignal(this);
    }
}

void EnHakurock_LargeStalactite_SetupWait(EnHakurock* this) {
    f32 shiftFactor;

    this->actor.draw = EnHakurock_DrawStalactite;
    Actor_SetScale(&this->actor, 0.35f);
    this->actor.scale.y = 0.5f;
    this->collider.dim.radius = this->actor.scale.x * 270.0f;
    shiftFactor = -750.0f;
    this->collider.dim.yShift = 0.5f * shiftFactor;
    this->timer = 0;
    this->actor.shape.shadowScale = 0.0f;
    this->collider.dim.height = this->collider.dim.yShift * -2;
    this->actionFunc = EnHakurock_LargeStalactite_Wait;
}

/**
 * Waits until Goht changes its type as part of its intro cutscene. Once its type changes, the stalactite will break
 * apart into a bunch of stalactite-shaped fragments.
 */
void EnHakurock_LargeStalactite_Wait(EnHakurock* this, PlayState* play) {
    if (EN_HAKUROCK_GET_TYPE(&this->actor) == EN_HAKUROCK_TYPE_NONE) {
        EnHakurock_SpawnEffect(this, EN_HAKUROCK_EFFECT_TYPE_STALACTITE_DESTROYED);
        EnHakurock_SetupWaitForSignal(this);
    }
}

void EnHakurock_Update(Actor* thisx, PlayState* play) {
    EnHakurock* this = (EnHakurock*)thisx;
    s32 pad;

    this->actionFunc(this, play);

    if ((EN_HAKUROCK_GET_TYPE(thisx) == EN_HAKUROCK_TYPE_BOULDER) ||
        (EN_HAKUROCK_GET_TYPE(thisx) == EN_HAKUROCK_TYPE_FALLING_STALACTITE)) {
        Actor_MoveWithGravity(thisx);
        Actor_UpdateBgCheckInfo(play, thisx, 30.0f, this->collider.dim.radius, 0.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_80);

        if (thisx->floorHeight == BGCHECK_Y_MIN) {
            EnHakurock_SetupWaitForSignal(this);
        } else {
            Collider_UpdateCylinder(thisx, &this->collider);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
    } else if ((EN_HAKUROCK_GET_TYPE(thisx) == EN_HAKUROCK_TYPE_STUCK_STALACTITE) ||
               (EN_HAKUROCK_GET_TYPE(thisx) == EN_HAKUROCK_TYPE_LARGE_STALACTITE)) {
        Collider_UpdateCylinder(thisx, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnHakurock_DrawBoulder(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, 255, 185, 24, 255);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gameplay_keep_DL_06AB30);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnHakurock_DrawStalactite(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Matrix_Translate(-100.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gGohtStalactiteMaterialDL);
    gSPDisplayList(POLY_OPA_DISP++, gGohtStalactiteModelDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
