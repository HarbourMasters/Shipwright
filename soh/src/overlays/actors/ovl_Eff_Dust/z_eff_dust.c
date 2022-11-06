/**
 * File z_eff_dust.c
 * Overlay: ovl_Eff_Dust
 * Description: Dust effects
 */

#include "z_eff_dust.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "soh/frame_interpolation.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EffDust_Init(Actor* thisx, PlayState* play);
void EffDust_Destroy(Actor* thisx, PlayState* play);
void EffDust_Update(Actor* thisx, PlayState* play);
void EffDust_Draw(Actor* thisx, PlayState* play);

void EffDust_InitPosAndDistance(EffDust* this);

void EffDust_UpdateFunc_8099DB28(EffDust* this, PlayState* play);
void EffDust_UpdateFunc_8099DD74(EffDust* this, PlayState* play);
void EffDust_UpdateFunc_8099DFC0(EffDust* this, PlayState* play);
void EffDust_DrawFunc_8099E4F4(Actor* thisx, PlayState* play);
void EffDust_DrawFunc_8099E784(Actor* thisx, PlayState* play);

const ActorInit Eff_Dust_InitVars = {
    ACTOR_EFF_DUST,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EffDust),
    (ActorFunc)EffDust_Init,
    (ActorFunc)EffDust_Destroy,
    (ActorFunc)EffDust_Update,
    (ActorFunc)EffDust_Draw,
    NULL,
};

static Gfx sEmptyDL[] = {
    gsSPEndDisplayList(),
};

void EffDust_SetupAction(EffDust* this, EffDustActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EffDust_SetupDraw(EffDust* this, EffDustDrawFunc drawFunc) {
    this->drawFunc = drawFunc;
}

void EffDust_InitPosAndDistance(EffDust* this) {
    s32 i;

    for (i = 0; i < 64; i++) {
        this->initialPositions[i].z = 0.0f;
        this->initialPositions[i].y = 0.0f;
        this->initialPositions[i].x = 0.0f;

        this->distanceTraveled[i] = 1.0f;
    }
    this->index = 0;
}

void EffDust_Init(Actor* thisx, PlayState* play) {
    EffDust* this = (EffDust*)thisx;
    EffDustType dustEffect = this->actor.params;

    EffDust_InitPosAndDistance(this);

    switch (dustEffect) {
        case EFF_DUST_TYPE_0:
            EffDust_SetupAction(this, EffDust_UpdateFunc_8099DB28);
            EffDust_SetupDraw(this, EffDust_DrawFunc_8099E4F4);
            this->dy = 0.8f;
            this->dz = 0.8f;
            this->dx = 1.0f;
            this->scalingFactor = 0.1f;
            break;
        case EFF_DUST_TYPE_1:
            EffDust_SetupAction(this, EffDust_UpdateFunc_8099DD74);
            EffDust_SetupDraw(this, EffDust_DrawFunc_8099E4F4);
            this->dx = 0.8f;
            this->dz = 0.8f;
            this->dy = 1.0f;
            this->scalingFactor = 0.5f;
            break;
        case EFF_DUST_TYPE_2:
            EffDust_SetupAction(this, EffDust_UpdateFunc_8099DFC0);
            EffDust_SetupDraw(this, EffDust_DrawFunc_8099E784);
            this->dx = 0.5f;
            this->scalingFactor = 15.0f;
            break;
        case EFF_DUST_TYPE_3:
            EffDust_SetupAction(this, EffDust_UpdateFunc_8099DFC0);
            EffDust_SetupDraw(this, EffDust_DrawFunc_8099E784);
            this->dx = 0.5f;
            this->scalingFactor = 10.0f;
            break;
        case EFF_DUST_TYPE_4:
            EffDust_SetupAction(this, EffDust_UpdateFunc_8099DFC0);
            EffDust_SetupDraw(this, EffDust_DrawFunc_8099E784);
            this->actor.room = -1;
            this->dx = 0.5f;
            this->scalingFactor = 20.0f;
            break;
        default:
            SYSTEM_ARENA_FREE_DEBUG(this);
            break;
    }

    this->life = 10;
}

void EffDust_Destroy(Actor* thisx, PlayState* play) {
}

void EffDust_UpdateFunc_8099DB28(EffDust* this, PlayState* play) {
    s16 theta;
    s16 fi;
    f32* distanceTraveled = this->distanceTraveled;
    s32 i;
    s32 j;

    for (i = 0; i < 64; i++) {
        if ((*distanceTraveled) < 1.0f) {
            *distanceTraveled += 0.05f;
        }
        distanceTraveled++;
    }

    for (j = 0; j < 3; j++) {
        i = this->index & 0x3F;
        if (this->distanceTraveled[i] >= 1.0f) {
            // Spherical coordinate system.
            fi = Rand_CenteredFloat(8192.0f);
            theta = Rand_CenteredFloat(4096.0f);
            this->initialPositions[i].x = -800.0f * Math_CosS(fi) * Math_CosS(theta);
            this->initialPositions[i].y = -800.0f * Math_SinS(theta);
            this->initialPositions[i].z = -800.0f * Math_SinS(fi) * Math_CosS(theta);
            this->distanceTraveled[i] = 0.0f;
            this->index += 1;
        }
    }
}

void EffDust_UpdateFunc_8099DD74(EffDust* this, PlayState* play) {
    s16 theta;
    s16 fi;
    f32* distanceTraveled = this->distanceTraveled;
    s32 i;
    s32 j;

    for (i = 0; i < 64; i++) {
        if ((*distanceTraveled) < 1.0f) {
            *distanceTraveled += 0.03f;
        }
        distanceTraveled++;
    }

    for (j = 0; j < 2; j++) {
        i = this->index & 0x3F;
        if (this->distanceTraveled[i] >= 1.0f) {
            // Spherical coordinate system.
            fi = Rand_CenteredFloat(65536.0f);
            theta = Rand_ZeroFloat(8192.0f);
            this->initialPositions[i].x = 400.0f * Math_CosS(fi) * Math_CosS(theta);
            this->initialPositions[i].y = 400.0f * Math_SinS(theta);
            this->initialPositions[i].z = 400.0f * Math_SinS(fi) * Math_CosS(theta);
            this->distanceTraveled[i] = 0.0f;
            this->index += 1;
        }
    }
}

void EffDust_UpdateFunc_8099DFC0(EffDust* this, PlayState* play) {
    s16 theta;
    Player* player = GET_PLAYER(play);
    Actor* parent = this->actor.parent;
    f32* distanceTraveled = this->distanceTraveled;
    s32 i;
    s32 j;

    if (parent == NULL || parent->update == NULL || !(player->stateFlags1 & 0x1000)) {
        if (this->life != 0) {
            this->life -= 1;
        } else {
            Actor_Kill(&this->actor);
        }

        for (i = 0; i < 64; i++) {
            if ((*distanceTraveled) < 1.0f) {
                *distanceTraveled += 0.2f;
            }
            distanceTraveled++;
        }

        return;
    }

    for (i = 0; i < 64; i++) {
        if ((*distanceTraveled) < 1.0f) {
            *distanceTraveled += 0.1f;
        }
        distanceTraveled++;
    }

    this->actor.world.pos = parent->world.pos;

    for (j = 0; j < 3; j++) {
        i = this->index & 0x3F;
        if (this->distanceTraveled[i] >= 1.0f) {

            theta = Rand_CenteredFloat(65536.0f);
            switch (this->actor.params) {
                case EFF_DUST_TYPE_2:
                    this->initialPositions[i].x = (Rand_ZeroOne() * 4500.0f) + 700.0f;
                    if (this->initialPositions[i].x > 3000.0f) {
                        this->initialPositions[i].y = (3000.0f * Rand_ZeroOne()) * Math_SinS(theta);
                        this->initialPositions[i].z = (3000.0f * Rand_ZeroOne()) * Math_CosS(theta);
                    } else {
                        this->initialPositions[i].y = 3000.0f * Math_SinS(theta);
                        this->initialPositions[i].z = 3000.0f * Math_CosS(theta);
                    }
                    break;

                case EFF_DUST_TYPE_3:
                    this->initialPositions[i].x = (Rand_ZeroOne() * 2500.0f) + 700.0f;
                    if (this->initialPositions[i].x > 2000.0f) {
                        this->initialPositions[i].y = (2000.0f * Rand_ZeroOne()) * Math_SinS(theta);
                        this->initialPositions[i].z = (2000.0f * Rand_ZeroOne()) * Math_CosS(theta);
                    } else {
                        this->initialPositions[i].y = 2000.0f * Math_SinS(theta);
                        this->initialPositions[i].z = 2000.0f * Math_CosS(theta);
                    }
                    break;

                case EFF_DUST_TYPE_4:
                    this->initialPositions[i].x = (Rand_ZeroOne() * 8500.0f) + 1700.0f;
                    if (this->initialPositions[i].x > 5000.0f) {
                        this->initialPositions[i].y = (4000.0f * Rand_ZeroOne()) * Math_SinS(theta);
                        this->initialPositions[i].z = (4000.0f * Rand_ZeroOne()) * Math_CosS(theta);
                    } else {
                        this->initialPositions[i].y = 4000.0f * Math_SinS(theta);
                        this->initialPositions[i].z = 4000.0f * Math_CosS(theta);
                    }

                    break;
                default:
                    break;
            }

            this->distanceTraveled[i] = 0.0f;
            this->index += 1;
        }
    }
}

void EffDust_Update(Actor* thisx, PlayState* play) {
    EffDust* this = (EffDust*)thisx;

    this->actionFunc(this, play);
}

void EffDust_DrawFunc_8099E4F4(Actor* thisx, PlayState* play2) {
    EffDust* this = (EffDust*)thisx;
    PlayState* play = play2;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    Vec3f* initialPositions;
    f32* distanceTraveled;
    s32 i;
    f32 aux;

    OPEN_DISPS(gfxCtx);

    func_80093D18(gfxCtx);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 128, 128, 128, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 0);

    initialPositions = this->initialPositions;
    distanceTraveled = this->distanceTraveled;

    gSPSegment(POLY_XLU_DISP++, 0x08, sEmptyDL);

    for (i = 0; i < 64; i++) {
        FrameInterpolation_RecordOpenChild("Dust 8099E4F4", i);

        if (*distanceTraveled < 1.0f) {
            aux = 1.0f - (*distanceTraveled * *distanceTraveled);
            Matrix_Translate(this->actor.world.pos.x + (initialPositions->x * ((this->dx * aux) + (1.0f - this->dx))),
                             this->actor.world.pos.y + (initialPositions->y * ((this->dy * aux) + (1.0f - this->dy))),
                             this->actor.world.pos.z + (initialPositions->z * ((this->dz * aux) + (1.0f - this->dz))),
                             MTXMODE_NEW);

            Matrix_Scale(this->scalingFactor, this->scalingFactor, this->scalingFactor, MTXMODE_APPLY);
            Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gEffSparklesDL));
        }

        FrameInterpolation_RecordCloseChild();

        initialPositions++;
        distanceTraveled++;
    }

    CLOSE_DISPS(gfxCtx);
}

void EffDust_DrawFunc_8099E784(Actor* thisx, PlayState* play2) {
    EffDust* this = (EffDust*)thisx;
    PlayState* play = play2;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32* distanceTraveled;
    Vec3f* initialPositions;
    s32 i;
    f32 aux;
    Player* player = GET_PLAYER(play);

    OPEN_DISPS(gfxCtx);

    func_80093D18(gfxCtx);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
    if (player->unk_858 >= 0.85f) {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 255, 0);
    }

    initialPositions = this->initialPositions;
    distanceTraveled = this->distanceTraveled;

    gSPSegment(POLY_XLU_DISP++, 0x08, sEmptyDL);

    for (i = 0; i < 64; i++) {
        FrameInterpolation_RecordOpenChild("Dust 8099E784", i);

        if (*distanceTraveled < 1.0f) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, *distanceTraveled * 255);

            // Needed to match.
            if (!this) {}

            aux = 1.0f - (*distanceTraveled * *distanceTraveled);

            Matrix_Mult(&player->mf_9E0, MTXMODE_NEW);

            Matrix_Translate(initialPositions->x * ((this->dx * aux) + (1.0f - this->dx)),
                             initialPositions->y * (1.0f - *distanceTraveled) + 320.0f,
                             initialPositions->z * (1.0f - *distanceTraveled) + -20.0f, MTXMODE_APPLY);

            Matrix_Scale(*distanceTraveled * this->scalingFactor, *distanceTraveled * this->scalingFactor,
                         *distanceTraveled * this->scalingFactor, MTXMODE_APPLY);

            Matrix_ReplaceRotation(&play->billboardMtxF);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gEffSparklesDL));
        }

        FrameInterpolation_RecordCloseChild();

        initialPositions++;
        distanceTraveled++;
    }

    CLOSE_DISPS(gfxCtx);
}

void EffDust_Draw(Actor* thisx, PlayState* play) {
    EffDust* this = (EffDust*)thisx;

    this->drawFunc(thisx, play);
}
