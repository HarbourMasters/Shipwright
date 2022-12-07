/*
 * File: z_bg_haka_tubo.c
 * Overlay: ovl_Bg_Haka_Tubo
 * Description: Shadow Temple Giant Skull Jar
 */

#include "z_bg_haka_tubo.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_haka_objects/object_haka_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgHakaTubo_Init(Actor* thisx, PlayState* play);
void BgHakaTubo_Destroy(Actor* thisx, PlayState* play);
void BgHakaTubo_Update(Actor* thisx, PlayState* play);
void BgHakaTubo_Draw(Actor* thisx, PlayState* play);

void BgHakaTubo_Idle(BgHakaTubo* this, PlayState* play);
void BgHakaTubo_DropCollectible(BgHakaTubo* this, PlayState* play);

const ActorInit Bg_Haka_Tubo_InitVars = {
    ACTOR_BG_HAKA_TUBO,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HAKA_OBJECTS,
    sizeof(BgHakaTubo),
    (ActorFunc)BgHakaTubo_Init,
    (ActorFunc)BgHakaTubo_Destroy,
    (ActorFunc)BgHakaTubo_Update,
    (ActorFunc)BgHakaTubo_Draw,
    NULL,
};

static ColliderCylinderInit sPotColliderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 25, 60, 30, { 0, 0, 0 } },
};

static ColliderCylinderInit sFlamesColliderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x20000000, 0x01, 0x04 },
        { 0x00000008, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 60, 45, 235, { 0, 0, 0 } },
};

static s32 sPotsDestroyed = 0;

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHakaTubo_Init(Actor* thisx, PlayState* play) {
    BgHakaTubo* this = (BgHakaTubo*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK3);
    CollisionHeader_GetVirtual(&object_haka_objects_Col_0108B8, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    Collider_InitCylinder(play, &this->potCollider);
    Collider_SetCylinder(play, &this->potCollider, &this->dyna.actor, &sPotColliderInit);
    Collider_InitCylinder(play, &this->flamesCollider);
    Collider_SetCylinder(play, &this->flamesCollider, &this->dyna.actor, &sFlamesColliderInit);
    this->fireScroll = Rand_ZeroOne() * 15.0f;
    sPotsDestroyed = 0;
    this->actionFunc = BgHakaTubo_Idle;
}

void BgHakaTubo_Destroy(Actor* thisx, PlayState* play) {
    BgHakaTubo* this = (BgHakaTubo*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(play, &this->potCollider);
    Collider_DestroyCylinder(play, &this->flamesCollider);
}

void BgHakaTubo_Idle(BgHakaTubo* this, PlayState* play) {
    static Vec3f sZeroVector = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;

    if (this->dyna.actor.room == 12) { // 3 spinning pots room in Shadow Temple
        this->dyna.actor.shape.rot.y += 0x180;
        this->dyna.actor.world.pos.x = Math_SinS(this->dyna.actor.shape.rot.y - 0x4000) * 145.0f + -5559.0f;
        this->dyna.actor.world.pos.z = Math_CosS(this->dyna.actor.shape.rot.y - 0x4000) * 145.0f + -1587.0f;
    }
    // Colliding with flame circle
    if (this->flamesCollider.base.atFlags & AT_HIT) {
        this->flamesCollider.base.atFlags &= ~AT_HIT;
        func_8002F71C(play, &this->dyna.actor, 5.0f, this->dyna.actor.yawTowardsPlayer, 5.0f);
    }
    // Colliding with hitbox inside the pot
    if (this->potCollider.base.acFlags & AC_HIT) {
        this->potCollider.base.acFlags &= ~AC_HIT;
        // If the colliding actor is within a 50 unit radius and 50 unit height cylinder centered
        // on the actor's position, break the pot
        if (Actor_WorldDistXZToPoint(&this->dyna.actor, &this->potCollider.base.ac->world.pos) < 50.0f &&
            (this->potCollider.base.ac->world.pos.y - this->dyna.actor.world.pos.y) < 50.0f) {
            pos.x = this->dyna.actor.world.pos.x;
            pos.z = this->dyna.actor.world.pos.z;
            pos.y = this->dyna.actor.world.pos.y + 80.0f;
            EffectSsBomb2_SpawnLayered(play, &pos, &sZeroVector, &sZeroVector, 100, 45);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 50, NA_SE_EV_BOX_BREAK);
            EffectSsHahen_SpawnBurst(play, &pos, 20.0f, 0, 350, 100, 50, OBJECT_HAKA_OBJECTS, 40,
                                     gEffFragments2DL);
            this->dropTimer = 5;
            this->dyna.actor.draw = NULL;
            Actor_SetScale(&this->dyna.actor, 0.0f);
            this->actionFunc = BgHakaTubo_DropCollectible;
        }
    } else {
        Collider_UpdateCylinder(&this->dyna.actor, &this->flamesCollider);
        Collider_UpdateCylinder(&this->dyna.actor, &this->potCollider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->potCollider.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->flamesCollider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->flamesCollider.base);
    }
}

void BgHakaTubo_DropCollectible(BgHakaTubo* this, PlayState* play) {
    EnItem00* collectible;
    f32 rnd;
    Vec3f spawnPos;
    s32 i;
    s32 collectibleParams;

    this->dropTimer--;
    if (this->dropTimer == 0) { // Creates a delay between destroying the pot and dropping the collectible
        spawnPos.x = this->dyna.actor.world.pos.x;
        spawnPos.y = this->dyna.actor.world.pos.y + 200.0f;
        spawnPos.z = this->dyna.actor.world.pos.z;
        if (this->dyna.actor.room == 12) { // 3 spinning pots room in Shadow Temple
            rnd = Rand_ZeroOne();
            sPotsDestroyed++;
            if (sPotsDestroyed == 3) {
                // All 3 pots destroyed
                collectibleParams = -1;
                func_80078884(NA_SE_SY_CORRECT_CHIME);
                // Drop rupees
                for (i = 0; i < 9; i++) {
                    collectible = Item_DropCollectible(play, &spawnPos, i % 3);
                    if (collectible != NULL) {
                        collectible->actor.velocity.y = 15.0f;
                        collectible->actor.world.rot.y = this->dyna.actor.shape.rot.y + (i * 0x1C71);
                    }
                }
            } else if (rnd < 0.2f) {
                // Unlucky, no reward and spawn keese
                collectibleParams = -1;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_FIREFLY, this->dyna.actor.world.pos.x,
                            this->dyna.actor.world.pos.y + 80.0f, this->dyna.actor.world.pos.z, 0,
                            this->dyna.actor.shape.rot.y, 0, 2, true);
                func_80078884(NA_SE_SY_ERROR);
            } else {
                // Random rewards
                if (CVar_GetS32("gNoRandomDrops", 0)) {
                    collectibleParams = -1;
                } else if(rnd < 0.4f) {
                    collectibleParams = ITEM00_BOMBS_A;
                } else if (rnd < 0.6f) {
                    collectibleParams = ITEM00_MAGIC_LARGE;
                } else if (rnd < 0.8f) {
                    collectibleParams = ITEM00_MAGIC_SMALL;
                } else {
                    collectibleParams = ITEM00_ARROWS_SMALL;
                }
                func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
            }
        } else if (Flags_GetCollectible(play, this->dyna.actor.params) != 0) {
            // If small key already collected, drop recovery heart instead
            if (CVar_GetS32("gNoHeartDrops", 0)) {
                collectibleParams = -1;
            }
            else {
                collectibleParams = ITEM00_HEART;
            }
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
        } else {
            // Drops a small key and sets a collect flag
            collectibleParams = ((this->dyna.actor.params & 0x3F) << 8) | ITEM00_SMALL_KEY;
            func_80078884(NA_SE_SY_CORRECT_CHIME);
        }
        if (collectibleParams != -1) {
            collectible = Item_DropCollectible(play, &spawnPos, collectibleParams);
            if (collectible != NULL) {
                collectible->actor.velocity.y = 15.0f;
                collectible->actor.world.rot.y = this->dyna.actor.shape.rot.y;
            }
        }
        Actor_Kill(&this->dyna.actor);
    }
}

void BgHakaTubo_Update(Actor* thisx, PlayState* play) {
    BgHakaTubo* this = (BgHakaTubo*)thisx;

    this->actionFunc(this, play);
    this->fireScroll++;
}

void BgHakaTubo_DrawFlameCircle(BgHakaTubo* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y + 235.0f, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateY(this->dyna.actor.shape.rot.y * (M_PI / 0x8000), MTXMODE_APPLY);
    Matrix_Scale(0.07f, 0.04f, 0.07f, MTXMODE_APPLY);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 0, 170, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 255, 255);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, this->fireScroll & 127, 0, 32, 64, 1, 0,
                                (this->fireScroll * -15) & 0xFF, 32, 64));
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFireCircleDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void BgHakaTubo_Draw(Actor* thisx, PlayState* play) {
    BgHakaTubo* this = (BgHakaTubo*)thisx;

    Gfx_DrawDListOpa(play, object_haka_objects_DL_00FE40);
    BgHakaTubo_DrawFlameCircle(this, play);
}
