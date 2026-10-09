/*
 * File: z_en_nutsball.c
 * Overlay: ovl_En_Nutsball
 * Description: Deku nut Projectile
 */

#include "z_en_nutsball.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnNutsball_Init(Actor* thisx, PlayState* play);
void EnNutsball_Destroy(Actor* thisx, PlayState* play);
void EnNutsball_Update(Actor* thisx, PlayState* play2);
void EnNutsball_Draw(Actor* thisx, PlayState* play);

void EnNutsball_InitColliderParams(EnNutsball* this);

ActorProfile En_Nutsball_Profile = {
    /**/ ACTOR_EN_NUTSBALL,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnNutsball),
    /**/ EnNutsball_Init,
    /**/ EnNutsball_Destroy,
    /**/ EnNutsball_Update,
    /**/ EnNutsball_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_WOOD,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 13, 13, 0, { 0, 0, 0 } },
};

void EnNutsball_Init(Actor* thisx, PlayState* play) {
    EnNutsball* this = (EnNutsball*)thisx;

    ActorShape_Init(&this->actor.shape, 400.0f, ActorShadow_DrawCircle, 13.0f);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.shape.rot.y = 0;
    this->actor.speed = 10.0f;
    if (this->actor.params == 2) {
        this->timer = 1;
        this->timerThreshold = 0;
        this->actor.gravity = -1.0f;
    } else {
        this->timer = 20;
        this->timerThreshold = 19;
        this->actor.gravity = -0.5f;
    }
    this->actor.world.rot.x = -this->actor.shape.rot.x;
    this->actor.shape.rot.x = 0;
    if (this->actor.params == 1) {
        EnNutsball_InitColliderParams(this);
    }
}

void EnNutsball_Destroy(Actor* thisx, PlayState* play) {
    EnNutsball* this = (EnNutsball*)thisx;
    Collider_DestroyCylinder(play, &this->collider);
}

void EnNutsball_InitColliderParams(EnNutsball* this) {
    this->collider.base.atFlags &= ~(AT_HIT | AT_TYPE_ENEMY | AT_BOUNCED);
    this->collider.base.atFlags |= AT_TYPE_PLAYER;
    this->collider.elem.atDmgInfo.dmgFlags = 0x400000;
    this->collider.elem.atDmgInfo.damage = 2;
}

void EnNutsball_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnNutsball* this = (EnNutsball*)thisx;
    Player* player = GET_PLAYER(play);
    Vec3f worldPos;
    Vec3s worldRot;
    Vec3f spawnBurstPos;
    f32 speedXZ;
    s32 bgId;
    CollisionPoly* poly;

    if (!(player->stateFlags1 &
          (PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_10000000 | PLAYER_STATE1_20000000))) {
        this->timer--;
        if (this->timer < 0) {
            this->actor.velocity.y += this->actor.gravity;
            speedXZ = sqrtf((this->actor.velocity.x * this->actor.velocity.x) +
                            (this->actor.velocity.z * this->actor.velocity.z));
            this->actor.world.rot.x = Math_Atan2S_XY(speedXZ, this->actor.velocity.y);
        }
        this->actor.home.rot.z += 0x2AA8;
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
            (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) || (this->collider.base.atFlags & AT_HIT) ||
            (this->collider.base.acFlags & AC_HIT) || (this->collider.base.ocFlags1 & OC1_HIT)) {
            if ((player->currentShield == PLAYER_SHIELD_HEROS_SHIELD) && (this->collider.base.atFlags & AT_HIT) &&
                (this->collider.base.atFlags & AT_TYPE_ENEMY) && (this->collider.base.atFlags & AT_BOUNCED)) {
                EnNutsball_InitColliderParams(this);
                Matrix_MtxFToYXZRot(&player->shieldMf, &worldRot, false);
                this->actor.world.rot.y = BINANG_ROT180(worldRot.y);
                this->timer = 20;
            } else {
                spawnBurstPos.x = this->actor.world.pos.x;
                spawnBurstPos.y = this->actor.world.pos.y + 4.0f;
                spawnBurstPos.z = this->actor.world.pos.z;
                EffectSsHahen_SpawnBurst(play, &spawnBurstPos, 6.0f, 0, 7, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
                if (this->actor.params == 1) {
                    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EV_NUTS_BROKEN);
                } else {
                    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EN_OCTAROCK_ROCK);
                }
                Actor_Kill(&this->actor);
            }
        } else {
            if (this->timer == -300) {
                Actor_Kill(&this->actor);
            }
        }

        Actor_MoveWithoutGravity(&this->actor);
        Math_Vec3f_Copy(&worldPos, &this->actor.world.pos);
        Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 5.0f, 10.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            if (SurfaceType_GetWallFlags(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId) &
                (WALL_FLAG_4 | WALL_FLAG_5)) {
                this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
                if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.prevPos, &worldPos, &this->actor.world.pos,
                                            &poly, true, false, false, true, &bgId)) {
                    if (SurfaceType_GetWallFlags(&play->colCtx, poly, bgId) & (WALL_FLAG_4 | WALL_FLAG_5)) {
                        this->actor.world.pos.x += this->actor.velocity.x * 0.01f;
                        this->actor.world.pos.z += this->actor.velocity.z * 0.01f;
                    } else {
                        this->actor.bgCheckFlags |= BGCHECKFLAG_WALL;
                    }
                } else {
                    Math_Vec3f_Copy(&this->actor.world.pos, &worldPos);
                }
            }
        }
        Collider_UpdateCylinder(&this->actor, &this->collider);

        this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;

        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

        if (this->timer < this->timerThreshold) {
            CollisionCheck_SetOC(play, &play2->colChkCtx, &this->collider.base);
        }
    }
}

void EnNutsball_Draw(Actor* thisx, PlayState* play) {
    EnNutsball* this = (EnNutsball*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
    Matrix_RotateZS(this->actor.home.rot.z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gameplay_keep_DL_058BA0);

    CLOSE_DISPS(play->state.gfxCtx);
}
