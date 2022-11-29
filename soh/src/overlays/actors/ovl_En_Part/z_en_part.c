/*
 * File: z_en_part.c
 * Overlay: ovl_En_Part
 * Description: Effect spawner for enemies' death
 */

#include "z_en_part.h"
#include "objects/object_tite/object_tite.h"
#include "objects/object_ik/object_ik.h"

#define FLAGS ACTOR_FLAG_4

void EnPart_Init(Actor* thisx, PlayState* play);
void EnPart_Destroy(Actor* thisx, PlayState* play);
void EnPart_Update(Actor* thisx, PlayState* play);
void EnPart_Draw(Actor* thisx, PlayState* play);

const ActorInit En_Part_InitVars = {
    ACTOR_EN_PART,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnPart),
    (ActorFunc)EnPart_Init,
    (ActorFunc)EnPart_Destroy,
    (ActorFunc)EnPart_Update,
    (ActorFunc)EnPart_Draw,
    NULL,
};

void EnPart_Init(Actor* thisx, PlayState* play) {
}

void EnPart_Destroy(Actor* thisx, PlayState* play) {
}

void func_80ACDDE8(EnPart* this, PlayState* play) {
    f32 sign = 1.0f;

    this->action = 1;
    this->actor.world.rot.y = Rand_ZeroOne() * 20000.0f;

    switch (this->actor.params) {
        case 0:
            this->actor.velocity.y = 0.0f;
            this->actor.gravity = -0.3f - Rand_ZeroOne() * 0.5f;
            this->rotZSpeed = 0.3f;
            this->timer = 25;
            this->actor.speedXZ = (Rand_ZeroOne() - 0.5f) * 2.0f;
            break;
        case 13:
            this->timer = 400;
        case 12:
            this->actor.speedXZ = Rand_CenteredFloat(6.0f);
            this->actor.home.pos = this->actor.world.pos;
            this->timer += 60;
            this->actor.velocity.y = Rand_ZeroOne() * 5.0f + 4.0f;
            this->actor.gravity = -0.6f - Rand_ZeroOne() * 0.5f;
            this->rotZSpeed = 0.15f;
            break;
        case 14:
            EffectSsEnFire_SpawnVec3f(play, &this->actor, &this->actor.world.pos, 40, 0x8001, 0, -1);
        case 1:
        case 4:
        case 9:
        case 10:
            this->timer += (s16)(Rand_ZeroOne() * 17.0f) + 5;
        case 2:
            this->actor.velocity.y = Rand_ZeroOne() * 5.0f + 4.0f;
            this->actor.gravity = -0.6f - Rand_ZeroOne() * 0.5f;
            this->rotZSpeed = 0.15f;
            break;
        case 11:
            EffectSsEnFire_SpawnVec3f(play, &this->actor, &this->actor.world.pos, 40, 0x8001, 0, -1);
        case 3:
            this->actor.speedXZ = (Rand_ZeroOne() - 0.5f) * 3.0f;
            this->timer = (s16)(Rand_ZeroOne() * 17.0f) + 10;
            this->actor.velocity.y = Rand_ZeroOne() * 3.0f + 8.0f;
            this->actor.gravity = -0.6f - Rand_ZeroOne() * 0.3f;
            this->rotZSpeed = 0.15f;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            this->actor.world.rot.y = this->actor.parent->shape.rot.y;
            if (this->displayList == object_ik_DL_015380) {
                sign = -1.0f;
            }
            this->actor.velocity.y = 0.0f;
            this->actor.speedXZ = 6.0f * sign;
            this->actor.gravity = -1.2f;
            this->rotZSpeed = 0.15f * sign;
            ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
            this->timer = 18;
            break;
    }
}

void func_80ACE13C(EnPart* this, PlayState* play) {
    s32 i;
    Vec3f pos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if ((this->actor.params == 12) || (this->actor.params == 13)) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 5.0f, 15.0f, 0.0f, 0x1D);

        if ((this->actor.bgCheckFlags & 1) || (this->actor.world.pos.y <= this->actor.floorHeight)) {
            this->action = 4;
            this->actor.speedXZ = 0.0f;
            this->actor.gravity = 0.0f;
            this->actor.velocity.y = 0.0f;
        }

        if ((this->actor.params == 13) && (this->actor.parent != NULL) && (this->actor.parent->update == NULL)) {
            this->actor.parent = NULL;
        }
    } else if (this->timer <= 0) {
        switch (this->actor.params) {
            case 1:
            case 9:
            case 10:
            case 14:
                EffectSsDeadDb_Spawn(play, &this->actor.world.pos, &zeroVec, &zeroVec,
                                     (s16)(this->actor.scale.y * 100.0f) * 40, 7, 255, 255, 255, 255, 0, 255, 0, 1, 9,
                                     true);
                break;
            case 3:
            case 11:
                EffectSsDeadDb_Spawn(play, &this->actor.world.pos, &zeroVec, &zeroVec,
                                     (s16)(this->actor.scale.y * 100.0f) * 40, 7, 255, 255, 255, 255, 0, 0, 255, 1, 9,
                                     true);
                break;
            case 4:
                for (i = 7; i >= 0; i--) {
                    pos.x = this->actor.world.pos.x + Rand_CenteredFloat(60.0f);
                    pos.y = this->actor.world.pos.y + this->actor.shape.yOffset * this->actor.scale.y +
                            Rand_CenteredFloat(50.0f);
                    pos.z = this->actor.world.pos.z + Rand_CenteredFloat(60.0f);
                    velocity.y = Rand_ZeroOne() + 1.0f;
                    EffectSsDtBubble_SpawnColorProfile(play, &pos, &velocity, &accel, Rand_S16Offset(80, 100), 25,
                                                       0, true);
                }
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                for (i = 4; i >= 0; i--) {
                    pos.x = this->actor.world.pos.x + Rand_CenteredFloat(25.0f);
                    pos.y = this->actor.world.pos.y + Rand_CenteredFloat(40.0f);
                    pos.z = this->actor.world.pos.z + Rand_CenteredFloat(25.0f);
                    EffectSsDeadDb_Spawn(play, &pos, &zeroVec, &zeroVec, 40, 7, 255, 255, 255, 255, 0, 0, 255, 1,
                                         9, true);
                }
                break;
        }

        Actor_Kill(&this->actor);
        return;
    }

    this->timer--;
    this->rotZ += this->rotZSpeed;
}

void func_80ACE5B8(EnPart* this, PlayState* play) {
    this->action = 3;
}

void func_80ACE5C8(EnPart* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->timer--;
    if (this->timer == 0) {
        Actor_Kill(&this->actor);
    } else {
        Vec3f velocity = { 0.0f, 8.0f, 0.0f };
        Vec3f accel = { 0.0f, -1.5f, 0.0f };

        if (sqrt(this->actor.xyzDistToPlayerSq) <= 40.0f) {
            u8 prevInvincibilityTimer = player->invincibilityTimer;

            if (player->invincibilityTimer <= 0) {
                if (player->invincibilityTimer <= -40) {
                    player->invincibilityTimer = 0;
                } else {
                    player->invincibilityTimer = 0;
                    play->damagePlayer(play, -8);
                }
            }
            func_8002F71C(play, this->actor.parent, (650.0f - this->actor.parent->xzDistToPlayer) * 0.04f + 4.0f,
                          this->actor.parent->world.rot.y, 8.0f);
            player->invincibilityTimer = prevInvincibilityTimer;
            this->timer = 1;
        }

        func_80033480(play, &this->actor.world.pos, 0.0f, 1, 300, 150, 1);
        velocity.x = Rand_CenteredFloat(16.0f);
        EffectSsHahen_Spawn(play, &this->actor.world.pos, &velocity, &accel, 20,
                            (s32)((Rand_ZeroOne() * 5.0f + 12.0f) * 2), -1, 10, NULL);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_MONBLIN_GNDWAVE - SFX_FLAG);
    }
}

void func_80ACE7E8(EnPart* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if ((this->actor.parent == NULL) || (this->actor.parent->update == NULL)) {
        EffectSsDeadDb_Spawn(play, &this->actor.world.pos, &zeroVec, &zeroVec,
                             (s16)(this->actor.scale.y * 100.0f) * 40, 7, 255, 255, 255, 255, 0, 255, 0, 1, 9, true);
        Actor_Kill(&this->actor);
        return;
    }

    if (this->timer == 0) {
        f32 diffsSum = Math_SmoothStepToF(&this->actor.world.pos.x, this->actor.home.pos.x, 1.0f, 5.0f, 0.0f);

        diffsSum += Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 1.0f, 5.0f, 0.0f);
        diffsSum += Math_SmoothStepToF(&this->actor.world.pos.z, this->actor.home.pos.z, 1.0f, 5.0f, 0.0f);
        diffsSum += Math_SmoothStepToF(&this->rotZ, 0.0f, 1.0f, 0.25f, 0.0f);
        if (diffsSum == 0.0f) {
            this->actor.parent->home.rot.x--;
            this->timer--;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_DAMAGE);
        }
    } else if (this->timer > 0) {
        this->timer--;
    }

    if (this->actor.parent->colChkInfo.health != 0) {
        Actor_Kill(&this->actor);
    }
}

void EnPart_Update(Actor* thisx, PlayState* play) {
    static EnPartActionFunc sActionFuncs[] = {
        func_80ACDDE8, func_80ACE13C, func_80ACE5B8, func_80ACE5C8, func_80ACE7E8,
    };

    EnPart* this = (EnPart*)thisx;

    Actor_MoveForward(&this->actor);

    if ((this->actor.params > 4 && this->actor.params < 9) || this->actor.params < 0) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 5.0f, 15.0f, 0.0f, 5);
        if (this->actor.params >= 0) {
            Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
            if (thisx->bgCheckFlags & 1) {
                thisx->bgCheckFlags &= ~1;
                thisx->velocity.y = 6.0f;
            }
        }
    }

    sActionFuncs[this->action](this, play);
}

Gfx* func_80ACEAC0(GraphicsContext* gfxCtx, u8 primR, u8 primG, u8 primB, u8 envR, u8 envG, u8 envB) {
    Gfx* dList;
    Gfx* dListHead;

    dList = Graph_Alloc(gfxCtx, 4 * sizeof(Gfx));
    dListHead = dList;

    gDPPipeSync(dListHead++);
    gDPSetPrimColor(dListHead++, 0, 0, primR, primG, primB, 255);
    gDPSetEnvColor(dListHead++, envR, envG, envB, 255);
    gSPEndDisplayList(dListHead++);

    return dList;
}

void EnPart_Draw(Actor* thisx, PlayState* play) {
    EnPart* this = (EnPart*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (thisx->params > 0) {
        Matrix_RotateZ(this->rotZ, MTXMODE_APPLY);
    }

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    func_8002EBCC(thisx, play, 0);

    if (thisx->params == 5) {
        gSPSegment(POLY_OPA_DISP++, 0x08, func_80ACEAC0(play->state.gfxCtx, 245, 255, 205, 30, 35, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, func_80ACEAC0(play->state.gfxCtx, 185, 135, 25, 20, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, func_80ACEAC0(play->state.gfxCtx, 255, 255, 255, 30, 40, 20));
    } else if (thisx->params == 6) {
        gSPSegment(POLY_OPA_DISP++, 0x08, func_80ACEAC0(play->state.gfxCtx, 55, 65, 55, 0, 0, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, func_80ACEAC0(play->state.gfxCtx, 205, 165, 75, 25, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, func_80ACEAC0(play->state.gfxCtx, 205, 165, 75, 25, 20, 0));
    } else if (thisx->params == 7) {
        gSPSegment(POLY_OPA_DISP++, 0x08, func_80ACEAC0(play->state.gfxCtx, 255, 255, 255, 180, 180, 180));
        gSPSegment(POLY_OPA_DISP++, 0x09, func_80ACEAC0(play->state.gfxCtx, 225, 205, 115, 25, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, func_80ACEAC0(play->state.gfxCtx, 225, 205, 115, 25, 20, 0));
    } else if ((thisx->params == 9) && (this->displayList == ResourceMgr_LoadGfxByName(object_tite_DL_002FF0))) {
        gSPSegment(POLY_OPA_DISP++, 0x08, object_tite_Tex_001300);
        gSPSegment(POLY_OPA_DISP++, 0x09, object_tite_Tex_001700);
        gSPSegment(POLY_OPA_DISP++, 0x0A, object_tite_Tex_001900);
    } else if ((thisx->params == 10) && (this->displayList == ResourceMgr_LoadGfxByName(object_tite_DL_002FF0))) {
        gSPSegment(POLY_OPA_DISP++, 0x08, object_tite_Tex_001B00);
        gSPSegment(POLY_OPA_DISP++, 0x09, object_tite_Tex_001F00);
        gSPSegment(POLY_OPA_DISP++, 0x0A, object_tite_Tex_002100);
    }

    if (this->displayList != NULL) {
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, this->displayList);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
