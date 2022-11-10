/*
 * File: z_en_vb_ball.c
 * Overlay: ovl_En_Vb_Ball
 * Description: Volvagia's rocks and bones
 */

#include "z_en_vb_ball.h"
#include "objects/object_fd/object_fd.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Boss_Fd/z_boss_fd.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnVbBall_Init(Actor* thisx, PlayState* play);
void EnVbBall_Destroy(Actor* thisx, PlayState* play);
void EnVbBall_Update(Actor* thisx, PlayState* play);
void EnVbBall_Draw(Actor* thisx, PlayState* play);

const ActorInit En_Vb_Ball_InitVars = {
    0,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_FD,
    sizeof(EnVbBall),
    (ActorFunc)EnVbBall_Init,
    (ActorFunc)EnVbBall_Destroy,
    (ActorFunc)EnVbBall_Update,
    (ActorFunc)EnVbBall_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK6,
        { 0x00100700, 0x00, 0x20 },
        { 0x00100700, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 30, 10, { 0, 0, 0 } },
};

void EnVbBall_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnVbBall* this = (EnVbBall*)thisx;
    s32 pad2;
    f32 angle;

    if (this->actor.params >= 200) { // Volvagia's bones
        this->yRotVel = Rand_CenteredFloat(0x300);
        this->xRotVel = Rand_CenteredFloat(0x300);
        angle = Math_FAtan2F(this->actor.world.pos.x, this->actor.world.pos.z);
        this->actor.velocity.y = Rand_ZeroFloat(3.0f);
        this->actor.velocity.x = 2.0f * sinf(angle);
        this->actor.velocity.z = 2.0f * cosf(angle);
        this->actor.gravity = -0.8f;
    } else { // Volvagia's rocks
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        Actor_SetScale(&this->actor, this->actor.world.rot.z / 10000.0f);
        this->collider.dim.radius = this->actor.scale.y * 3000.0f;
        this->collider.dim.height = this->actor.scale.y * 5000.0f;
        this->collider.dim.yShift = this->actor.scale.y * -2500.0f;
        this->xRotVel = Rand_CenteredFloat(0x2000);
        this->yRotVel = Rand_CenteredFloat(0x2000);
        this->shadowSize = this->actor.scale.y * 68.0f;
    }
}

void EnVbBall_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnVbBall* this = (EnVbBall*)thisx;

    if (this->actor.params < 200) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void EnVbBall_SpawnDebris(PlayState* play, BossFdEffect* effect, Vec3f* position, Vec3f* velocity,
                          Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_DEBRIS;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->vFdFxRotX = Rand_ZeroFloat(100.0f);
            effect->vFdFxRotY = Rand_ZeroFloat(100.0f);
            break;
        }
    }
}

void EnVbBall_SpawnDust(PlayState* play, BossFdEffect* effect, Vec3f* position, Vec3f* velocity,
                        Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_DUST;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->timer2 = 0;
            effect->scale = scale / 400.0f;
            break;
        }
    }
}

void EnVbBall_UpdateBones(EnVbBall* this, PlayState* play) {
    BossFd* bossFd = (BossFd*)this->actor.parent;
    f32 pad2;
    f32 pad1;
    f32 angle;
    s16 i;

    Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 50.0f, 100.0f, 4);
    if ((this->actor.bgCheckFlags & 1) && (this->actor.velocity.y <= 0.0f)) {
        this->xRotVel = Rand_CenteredFloat((f32)0x4000);
        this->yRotVel = Rand_CenteredFloat((f32)0x4000);
        angle = Math_FAtan2F(this->actor.world.pos.x, this->actor.world.pos.z);
        this->actor.velocity.x = sinf(angle) * 10.0f;
        this->actor.velocity.z = cosf(angle) * 10.0f;
        this->actor.velocity.y *= -0.5f;
        if (this->actor.params & 1) {
            Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_LAND, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
        for (i = 0; i < 10; i++) {
            Vec3f dustVel = { 0.0f, 0.0f, 0.0f };
            Vec3f dustAcc = { 0.0f, 0.0f, 0.0f };
            Vec3f dustPos;

            dustVel.x = Rand_CenteredFloat(8.0f);
            dustVel.y = Rand_ZeroFloat(1.0f);
            dustVel.z = Rand_CenteredFloat(8.0f);

            dustAcc.y = 0.3f;

            dustPos.x = Rand_CenteredFloat(20.0f) + this->actor.world.pos.x;
            dustPos.y = this->actor.floorHeight + 10.0f;
            dustPos.z = Rand_CenteredFloat(20.0f) + this->actor.world.pos.z;

            EnVbBall_SpawnDust(play, bossFd->effects, &dustPos, &dustVel, &dustAcc,
                               Rand_ZeroFloat(80.0f) + 200.0f);
        }
    }
    if (this->actor.world.pos.y < 50.0f) {
        Actor_Kill(&this->actor);
    }
}

void EnVbBall_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnVbBall* this = (EnVbBall*)thisx;
    BossFd* bossFd = (BossFd*)this->actor.parent;
    f32 radius;
    f32 pad2;
    s16 spawnNum;
    s16 i;

    this->unkTimer2++;
    if (this->unkTimer1 != 0) {
        this->unkTimer1--;
    }
    this->actor.shape.rot.x += (s16)this->xRotVel;
    this->actor.shape.rot.y += (s16)this->yRotVel;
    this->actor.velocity.y += -1.0f;
    this->actor.gravity = -1.0f;
    func_8002D7EC(&this->actor);
    if (this->actor.params >= 200) {
        EnVbBall_UpdateBones(this, play);
    } else {
        Math_ApproachF(&this->shadowOpacity, 175.0f, 1.0f, 40.0f);
        radius = this->actor.scale.y * 1700.0f;
        this->actor.world.pos.y -= radius;
        Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 50.0f, 100.0f, 4);
        this->actor.world.pos.y += radius;
        if ((this->actor.bgCheckFlags & 1) && (this->actor.velocity.y <= 0.0f)) {
            if ((this->actor.params == 100) || (this->actor.params == 101)) {
                Actor_Kill(&this->actor);
                if (this->actor.params == 100) {
                    func_80033E88(&this->actor, play, 5, 0xA);
                }
                if (this->actor.params == 100) {
                    spawnNum = 2;
                } else {
                    spawnNum = 2;
                }
                for (i = 0; i < spawnNum; i++) {
                    Vec3f spawnOffset;
                    EnVbBall* newActor;
                    f32 xRotVel;

                    if (this->actor.params == 100) {
                        spawnOffset.x = Rand_CenteredFloat(13.0f);
                        spawnOffset.y = Rand_ZeroFloat(5.0f) + 6.0f;
                        spawnOffset.z = Rand_CenteredFloat(13);
                    } else {
                        spawnOffset.x = Rand_CenteredFloat(10.0f);
                        spawnOffset.y = Rand_ZeroFloat(3.0f) + 4.0f;
                        spawnOffset.z = Rand_CenteredFloat(10.0f);
                    }
                    newActor = (EnVbBall*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play,
                                                             ACTOR_EN_VB_BALL, this->actor.world.pos.x + spawnOffset.x,
                                                             this->actor.world.pos.y + spawnOffset.y,
                                                             this->actor.world.pos.z + spawnOffset.z, 0, 0,
                                                             this->actor.world.rot.z * 0.5f, this->actor.params + 1);
                    if (newActor != NULL) {
                        if ((i == 0) && (this->actor.params == 100)) {
                            Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_ROCK, &newActor->actor.projectedPos, 4,
                                                   &D_801333E0, &D_801333E0, &D_801333E8);
                        }
                        newActor->actor.parent = this->actor.parent;
                        newActor->actor.velocity = spawnOffset;
                        newActor->yRotVel = 0.0f;
                        xRotVel = sqrtf(SQ(spawnOffset.x) + SQ(spawnOffset.z));
                        newActor->xRotVel = 0x1000 / 10.0f * xRotVel;
                        newActor->actor.shape.rot.y = Math_FAtan2F(spawnOffset.x, spawnOffset.z) * ((f32)0x8000 / M_PI);
                        newActor->shadowOpacity = 200.0f;
                    }
                }
                for (i = 0; i < 15; i++) {
                    Vec3f debrisVel1 = { 0.0f, 0.0f, 0.0f };
                    Vec3f debrisAcc1 = { 0.0f, -1.0f, 0.0f };
                    Vec3f debrisPos1;

                    debrisVel1.x = Rand_CenteredFloat(25.0f);
                    debrisVel1.y = Rand_ZeroFloat(5.0f) + 8;
                    debrisVel1.z = Rand_CenteredFloat(25.0f);

                    debrisPos1.x = Rand_CenteredFloat(10.0f) + this->actor.world.pos.x;
                    debrisPos1.y = Rand_CenteredFloat(10.0f) + this->actor.world.pos.y;
                    debrisPos1.z = Rand_CenteredFloat(10.0f) + this->actor.world.pos.z;

                    EnVbBall_SpawnDebris(play, bossFd->effects, &debrisPos1, &debrisVel1, &debrisAcc1,
                                         (s16)Rand_ZeroFloat(12.0f) + 15);
                }
                for (i = 0; i < 10; i++) {
                    Vec3f dustVel = { 0.0f, 0.0f, 0.0f };
                    Vec3f dustAcc = { 0.0f, 0.0f, 0.0f };
                    Vec3f dustPos;

                    dustVel.x = Rand_CenteredFloat(8.0f);
                    dustVel.y = Rand_ZeroFloat(1.0f);
                    dustVel.z = Rand_CenteredFloat(8.0f);

                    dustAcc.y = 1.0f / 2;

                    dustPos.x = Rand_CenteredFloat(30.0f) + this->actor.world.pos.x;
                    dustPos.y = Rand_CenteredFloat(30.0f) + this->actor.world.pos.y;
                    dustPos.z = Rand_CenteredFloat(30.0f) + this->actor.world.pos.z;

                    EnVbBall_SpawnDust(play, bossFd->effects, &dustPos, &dustVel, &dustAcc,
                                       Rand_ZeroFloat(100.0f) + 350.0f);
                }
            } else {
                for (i = 0; i < 5; i++) {
                    Vec3f debrisVel2 = { 0.0f, 0.0f, 0.0f };
                    Vec3f debrisAcc2 = { 0.0f, -1.0f, 0.0f };
                    Vec3f debrisPos2;

                    debrisVel2.x = Rand_CenteredFloat(10.0f);
                    debrisVel2.y = Rand_ZeroFloat(3.0f) + 3.0f;
                    debrisVel2.z = Rand_CenteredFloat(10.0f);

                    debrisPos2.x = Rand_CenteredFloat(5.0f) + this->actor.world.pos.x;
                    debrisPos2.y = Rand_CenteredFloat(5.0f) + this->actor.world.pos.y;
                    debrisPos2.z = Rand_CenteredFloat(5.0f) + this->actor.world.pos.z;

                    EnVbBall_SpawnDebris(play, bossFd->effects, &debrisPos2, &debrisVel2, &debrisAcc2,
                                         (s16)Rand_ZeroFloat(12.0f) + 15);
                }
                Actor_Kill(&this->actor);
            }
        }
        if (this->collider.base.atFlags & AT_HIT) {
            Player* player = GET_PLAYER(play);

            this->collider.base.atFlags &= ~AT_HIT;
            Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
        }
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnVbBall_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnVbBall* this = (EnVbBall*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D18(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (this->actor.params >= 200) {
        gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gVolvagiaRibsDL));
    } else {
        gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gVolvagiaRockDL));
        func_80094044(play->state.gfxCtx);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, (s8)this->shadowOpacity);
        Matrix_Translate(this->actor.world.pos.x, 100.0f, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_Scale(this->shadowSize, 1.0f, this->shadowSize, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gCircleShadowDL));
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
