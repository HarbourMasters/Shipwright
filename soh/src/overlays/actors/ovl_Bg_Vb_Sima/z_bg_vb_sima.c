/*
 * File: z_bg_vb_sima.c
 * Overlay: ovl_Bg_Vb_Sima
 * Description: Volvagia's platform
 */

#include "z_bg_vb_sima.h"
#include "objects/object_fd/object_fd.h"
#include "overlays/actors/ovl_Boss_Fd/z_boss_fd.h"

#define FLAGS 0

void BgVbSima_Init(Actor* thisx, PlayState* play);
void BgVbSima_Destroy(Actor* thisx, PlayState* play);
void BgVbSima_Update(Actor* thisx, PlayState* play);
void BgVbSima_Draw(Actor* thisx, PlayState* play);

const ActorInit Bg_Vb_Sima_InitVars = {
    ACTOR_BG_VB_SIMA,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_FD,
    sizeof(BgVbSima),
    (ActorFunc)BgVbSima_Init,
    (ActorFunc)BgVbSima_Destroy,
    (ActorFunc)BgVbSima_Update,
    (ActorFunc)BgVbSima_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgVbSima_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgVbSima* this = (BgVbSima*)thisx;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(&gVolvagiaPlatformCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void BgVbSima_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    BgVbSima* this = (BgVbSima*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgVbSima_SpawnEmber(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_EMBER;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->alpha = 255;
            effect->timer1 = (s16)Rand_ZeroFloat(10.0f);
            break;
        }
    }
}

void BgVbSima_Update(Actor* thisx, PlayState* play) {
    static Color_RGBA8 colorYellow = { 255, 255, 0, 255 };
    static Color_RGBA8 colorRed = { 255, 10, 0, 255 };
    s32 pad;
    BgVbSima* this = (BgVbSima*)thisx;
    BossFd* bossFd = (BossFd*)this->dyna.actor.parent;
    f32 minus1 = -1.0f;

    this->shakeTimer++;
    if (!Flags_GetClear(play, play->roomCtx.curRoom.num)) {
        s32 signal = bossFd->platformSignal;

        if (signal == VBSIMA_COLLAPSE) {
            Math_SmoothStepToF(&this->dyna.actor.world.pos.y, -1000.0f, 1.0f, 1.5f, 0.0f);
            this->dyna.actor.world.pos.z += 2.0f * Math_CosS(this->shakeTimer * 0x8000);
            this->dyna.actor.shape.rot.x = (s16)Math_SinS(this->shakeTimer * 0x7000) * 0x37;
            this->dyna.actor.shape.rot.z = (s16)Math_SinS(this->shakeTimer * 0x5000) * 0x37;
            Audio_PlaySoundGeneral(NA_SE_EV_BLOCKSINK - SFX_FLAG, &this->dyna.actor.projectedPos, 4, &D_801333E0,
                                   &D_801333E0, &D_801333E8);
        } else if (signal == VBSIMA_KILL) {
            Actor_Kill(&this->dyna.actor);
        }
        if (bossFd->platformSignal != VBSIMA_STAND) {
            s16 i2;
            s16 i1;
            Vec3f splashVel;
            Vec3f splashAcc;
            Vec3f splashPos;
            Vec3f emberPos;
            Vec3f emberVel;
            Vec3f emberAcc;
            f32 edgeX;
            f32 edgeZ;

            for (i1 = 0; i1 < 10; i1++) {
                if (Rand_ZeroOne() < 0.33f) {
                    edgeX = -80.0f;
                    edgeZ = Rand_CenteredFloat(160.0f);
                } else {
                    edgeZ = 80.0f;
                    if (Rand_ZeroOne() < 0.5f) {
                        edgeZ = 80.0f * minus1;
                    }
                    edgeX = Rand_CenteredFloat(160.0f);
                }

                splashVel.x = edgeX * 0.05f;
                splashVel.y = Rand_ZeroFloat(3.0f) + 3.0f;
                splashVel.z = edgeZ * 0.05f;

                splashAcc.y = -0.3f;
                splashAcc.x = splashVel.x;
                splashAcc.z = splashVel.z;

                splashPos.x = this->dyna.actor.world.pos.x + edgeX;
                splashPos.y = -80.0f;
                splashPos.z = this->dyna.actor.world.pos.z + edgeZ;

                func_8002836C(play, &splashPos, &splashVel, &splashAcc, &colorYellow, &colorRed,
                              (s16)Rand_ZeroFloat(100.0f) + 500, 10, 20);

                for (i2 = 0; i2 < 3; i2++) {
                    emberVel.x = splashVel.x;
                    emberVel.y = Rand_ZeroFloat(5.0f);
                    emberVel.z = splashVel.z;

                    emberAcc.y = 0.4f;
                    emberAcc.x = Rand_CenteredFloat(0.5f);
                    emberAcc.z = Rand_CenteredFloat(0.5f);

                    emberPos.x = Rand_CenteredFloat(60.0f) + splashPos.x;
                    emberPos.y = Rand_ZeroFloat(40.0f) + splashPos.y;
                    emberPos.z = Rand_CenteredFloat(60.0f) + splashPos.z;

                    BgVbSima_SpawnEmber(bossFd->effects, &emberPos, &emberVel, &emberAcc,
                                        (s16)Rand_ZeroFloat(2.0f) + 8);
                }
            }
        }
    }
}

void BgVbSima_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gVolvagiaPlatformDL);
    CLOSE_DISPS(play->state.gfxCtx);
}
