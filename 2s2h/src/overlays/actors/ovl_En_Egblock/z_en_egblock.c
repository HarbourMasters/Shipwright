/*
 * File: z_en_egblock.c
 * Overlay: ovl_En_Egblock
 * Description: Unused pillar. Eyegore can destroy it.
 */

#include "z_en_egblock.h"
#include "objects/object_eg/object_eg.h"

#define FLAGS (ACTOR_FLAG_LOCK_ON_DISABLED)

typedef enum {
    /* 0 */ EGBLOCK_EFFECT_DEBRIS_SOLID,
    /* 1 */ EGBLOCK_EFFECT_DEBRIS_FLAT
} EnEgblockEffectType;

void EnEgblock_Init(Actor* thisx, PlayState* play);
void EnEgblock_Destroy(Actor* thisx, PlayState* play);
void EnEgblock_Update(Actor* thisx, PlayState* play);
void EnEgblock_Draw(Actor* thisx, PlayState* play2);

void EnEgblock_Active(EnEgblock* this, PlayState* play);
void EnEgblock_DoNothing(EnEgblock* this, PlayState* play);
void EnEgblock_Inactive(EnEgblock* this, PlayState* play);

void EnEgblock_SpawnEffect(EnEgblock* this, Vec3f* pos, s16 lifetime, s16 arg3);
void EnEgblock_UpdateEffects(EnEgblock* this, PlayState* play);
void EnEgblock_DrawEffects(EnEgblock* this, PlayState* play);

ActorProfile En_Egblock_Profile = {
    /**/ ACTOR_EN_EGBLOCK,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_EG,
    /**/ sizeof(EnEgblock),
    /**/ EnEgblock_Init,
    /**/ EnEgblock_Destroy,
    /**/ EnEgblock_Update,
    /**/ EnEgblock_Draw,
};

void EnEgblock_Init(Actor* thisx, PlayState* play) {
    EnEgblock* this = (EnEgblock*)thisx;
    CollisionHeader* colHeader = NULL;
    s32 pad;

    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&gEyegoreBlockCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->inactive = false;
    Actor_SetScale(&this->dyna.actor, 0.5f);
    this->type = EGBLOCK_GET_TYPE(&this->dyna.actor);
    this->paramF80 = EGBLOCK_GET_PARAM_F80(&this->dyna.actor);
    this->param7F = EGBLOCK_GET_PARAM_7F(&this->dyna.actor);
    if (this->type == EGBLOCK_TYPE_ACTIVE) {
        this->dyna.actor.colChkInfo.health = 1;
        this->actionFunc = EnEgblock_Active;
    } else {
        this->actionFunc = EnEgblock_DoNothing;
    }
}

void EnEgblock_Destroy(Actor* thisx, PlayState* play) {
    EnEgblock* this = (EnEgblock*)thisx;

    if (this->dyna.actor.colChkInfo.health == 1) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void EnEgblock_Active(EnEgblock* this, PlayState* play) {
    if (this->dyna.actor.colChkInfo.health <= 0) {
        s32 i;

        //! @bug Egblock doesn't have an effect with index 2, so half of these spawns do nothing.
        for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
            EnEgblock_SpawnEffect(this, &this->dyna.actor.world.pos, 30, (i & 1) + EGBLOCK_EFFECT_DEBRIS_FLAT);
        }
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
        Actor_SpawnFloorDustRing(play, &this->dyna.actor, &this->dyna.actor.world.pos, 30.0f, 30, 10.0f, 100, 30, true);
        this->inactive = true;
        this->timer = 50;
        this->actionFunc = EnEgblock_Inactive;
    }
}

void EnEgblock_Inactive(EnEgblock* this, PlayState* play) {
    if (this->timer == 0) {
        Actor_Kill(&this->dyna.actor);
    }
}

void EnEgblock_DoNothing(EnEgblock* this, PlayState* play) {
}

void EnEgblock_Update(Actor* thisx, PlayState* play) {
    EnEgblock* this = (EnEgblock*)thisx;

    this->actionFunc(this, play);

    DECR(this->timer);

    EnEgblock_UpdateEffects(this, play);
}

void EnEgblock_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnEgblock* this = (EnEgblock*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->inactive != true) {
        OPEN_DISPS(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x80, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
        gSPDisplayList(POLY_OPA_DISP++, gEyegoreBlockDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }

    EnEgblock_DrawEffects(this, play);
}

void EnEgblock_SpawnEffect(EnEgblock* this, Vec3f* pos, s16 lifetime, s16 arg3) {
    EnEgblockEffect* effect = this->effects;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (!effect->isActive) {
            effect->isActive = true;
            effect->pos = *pos;
            effect->pos.x += Rand_CenteredFloat(50.0f);
            effect->pos.z += Rand_CenteredFloat(50.0f);
            effect->timer = lifetime;
            effect->alpha = 255;
            effect->type = arg3;
            effect->accel.x = Rand_ZeroOne() - 0.5f;
            effect->accel.y = -1.0f;
            effect->accel.z = Rand_ZeroOne() - 0.5f;
            effect->velocity.x = 2.0f * (Rand_ZeroOne() - 0.5f);
            effect->velocity.y = 5.0f + (10.0f * Rand_ZeroOne());
            effect->velocity.z = 2.0f * (Rand_ZeroOne() - 0.5f);
            effect->scale = 0.5f + (0.2f * Rand_ZeroFloat(1.0f));
            effect->rot.x = Rand_CenteredFloat(0x7530);
            effect->rot.y = Rand_CenteredFloat(0x7530);
            effect->rot.z = Rand_CenteredFloat(0x7530);
            break;
        }
    }
}

void EnEgblock_UpdateEffects(EnEgblock* this, PlayState* play) {
    EnEgblockEffect* effect = this->effects;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isActive) {
            if (effect->type != EGBLOCK_EFFECT_DEBRIS_SOLID) {
                effect->rot.x -= 0x64;
                effect->rot.y -= 0x64;
                effect->rot.z -= 0x64;
            }

            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;

            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;

            if (effect->timer) {
                effect->timer--;
            } else {
                effect->alpha -= 10;
                if (effect->alpha < 10) {
                    effect->isActive = false;
                }
            }
        }
    }
}

void EnEgblock_DrawEffects(EnEgblock* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    EnEgblockEffect* effect = this->effects;
    s16 i;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isActive) {
            switch (effect->type) {
                case EGBLOCK_EFFECT_DEBRIS_SOLID:
                case EGBLOCK_EFFECT_DEBRIS_FLAT:
                    Matrix_Push();

                    Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
                    Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
                    Matrix_RotateYS(effect->rot.y, MTXMODE_APPLY);
                    Matrix_RotateXS(effect->rot.x, MTXMODE_APPLY);
                    Matrix_RotateZS(effect->rot.z, MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
                    gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x80, 255, 255, 255, 255);
                    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, effect->alpha);

                    if (effect->type == EGBLOCK_EFFECT_DEBRIS_SOLID) {
                        gSPDisplayList(POLY_OPA_DISP++, gEyegoreEffectSolidDebrisDL);
                    } else {
                        gSPDisplayList(POLY_OPA_DISP++, gEyegoreEffectFlatDebrisDL);
                    }

                    Matrix_Pop();
                    break;

                default:
                    break;
            }
        }
    }

    CLOSE_DISPS(gfxCtx);
}
