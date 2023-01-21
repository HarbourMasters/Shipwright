/*
 * File: z_bg_gnd_firemeiro.c
 * Overlay: ovl_Bg_Gnd_Firemeiro
 * Description: Sinking lava platform (Ganon's Castle)
 */

#include "z_bg_gnd_firemeiro.h"
#include "objects/object_demo_kekkai/object_demo_kekkai.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgGndFiremeiro_Init(Actor* thisx, PlayState* play);
void BgGndFiremeiro_Destroy(Actor* thisx, PlayState* play);
void BgGndFiremeiro_Update(Actor* thisx, PlayState* play);
void BgGndFiremeiro_Draw(Actor* thisx, PlayState* play);

void BgGndFiremeiro_Sink(BgGndFiremeiro* this, PlayState* play);
void BgGndFiremeiro_Shake(BgGndFiremeiro* this, PlayState* play);
void BgGndFiremeiro_Rise(BgGndFiremeiro* this, PlayState* play);

const ActorInit Bg_Gnd_Firemeiro_InitVars = {
    ACTOR_BG_GND_FIREMEIRO,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_DEMO_KEKKAI,
    sizeof(BgGndFiremeiro),
    (ActorFunc)BgGndFiremeiro_Init,
    (ActorFunc)BgGndFiremeiro_Destroy,
    (ActorFunc)BgGndFiremeiro_Update,
    (ActorFunc)BgGndFiremeiro_Draw,
    NULL,
};

void BgGndFiremeiro_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGndFiremeiro* this = (BgGndFiremeiro*)thisx;
    CollisionHeader* colHeader = NULL;

    ActorShape_Init(&this->dyna.actor.shape, 0.0f, NULL, 0.0f);
    Actor_SetScale(&this->dyna.actor, 0.1f);
    this->initPos = this->dyna.actor.world.pos;

    if (this->dyna.actor.params == 0) {
        DynaPolyActor_Init(&this->dyna, DPM_UNK);
        CollisionHeader_GetVirtual(&gFireTrialPlatformCol, &colHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
        this->actionFunc = BgGndFiremeiro_Rise;
    }
}

void BgGndFiremeiro_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgGndFiremeiro* this = (BgGndFiremeiro*)thisx;

    if (this->dyna.actor.params == 0) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void BgGndFiremeiro_Sink(BgGndFiremeiro* this, PlayState* play) {
    f32 sunkHeight = this->initPos.y - 150.0f;

    if (func_8004356C(&this->dyna)) {
        this->timer = 10;
    }

    if (sunkHeight < this->dyna.actor.world.pos.y) {
        this->dyna.actor.world.pos.y -= 0.5f;

        if (this->dyna.actor.world.pos.y < sunkHeight) {
            this->dyna.actor.world.pos.y = sunkHeight;
        }

        func_8002F948(&this->dyna.actor, NA_SE_EV_ROLL_STAND_2 - SFX_FLAG);
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        this->actionFunc = BgGndFiremeiro_Rise;
    }
}

void BgGndFiremeiro_Shake(BgGndFiremeiro* this, PlayState* play) {
    s32 pad;
    f32 randSign;

    if (func_8004356C(&this->dyna)) { // Player standing on it
        if (this->timer > 0) {
            this->timer--;

            randSign = ((this->timer & 1) ? 2.0f : -2.0f);

            this->dyna.actor.world.pos = this->initPos;
            this->dyna.actor.world.pos.x += randSign * Math_SinS(this->timer * 0x2FFF);
            this->dyna.actor.world.pos.z += randSign * Math_CosS(this->timer * 0x2FFF);
            this->dyna.actor.world.pos.y += Math_CosS(this->timer * 0x7FFF);

            if (!(this->timer % 4)) {
                Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
            }
        } else {
            this->timer = 10;
            this->dyna.actor.world.pos = this->initPos;
            this->actionFunc = BgGndFiremeiro_Sink;
        }
    } else {
        this->dyna.actor.world.pos = this->initPos;
        this->actionFunc = BgGndFiremeiro_Rise;
    }
}

void BgGndFiremeiro_Rise(BgGndFiremeiro* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* thisx = &this->dyna.actor;

    if ((player->currentBoots != PLAYER_BOOTS_HOVER) && func_8004356C(&this->dyna)) { // Player standing on it
        if (thisx->world.pos.y < this->initPos.y) {
            this->actionFunc = BgGndFiremeiro_Sink;
            this->timer = 20;
        } else {
            this->actionFunc = BgGndFiremeiro_Shake;
            this->timer = 20;
        }
    } else {
        if (thisx->world.pos.y < this->initPos.y) {
            thisx->world.pos.y += 2.0f;
            if (this->initPos.y < thisx->world.pos.y) {
                thisx->world.pos.y = this->initPos.y;
            }
        }
    }
}

void BgGndFiremeiro_Update(Actor* thisx, PlayState* play) {
    BgGndFiremeiro* this = (BgGndFiremeiro*)thisx;

    this->actionFunc(this, play);
}

void BgGndFiremeiro_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_37Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gFireTrialPlatformDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
