/*
 * File: z_bg_gnd_darkmeiro.c
 * Overlay: ovl_Bg_Gnd_Darkmeiro
 * Description: Shadow trial actors (invisible path, clear block, and timer)
 */

#include "z_bg_gnd_darkmeiro.h"
#include "objects/object_demo_kekkai/object_demo_kekkai.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgGndDarkmeiro_Init(Actor* thisx, PlayState* play);
void BgGndDarkmeiro_Destroy(Actor* thisx, PlayState* play);
void BgGndDarkmeiro_Update(Actor* thisx, PlayState* play);
void BgGndDarkmeiro_DrawInvisiblePath(Actor* thisx, PlayState* play);
void BgGndDarkmeiro_DrawSwitchBlock(Actor* thisx, PlayState* play);
void BgGndDarkmeiro_DrawStaticBlock(Actor* thisx, PlayState* play);

void BgGndDarkmeiro_Noop(BgGndDarkmeiro* this, PlayState* play);
void BgGndDarkmeiro_UpdateBlockTimer(BgGndDarkmeiro* this, PlayState* play);
void BgGndDarkmeiro_UpdateStaticBlock(BgGndDarkmeiro* this, PlayState* play);
void BgGndDarkmeiro_UpdateSwitchBlock(BgGndDarkmeiro* this, PlayState* play);

const ActorInit Bg_Gnd_Darkmeiro_InitVars = {
    ACTOR_BG_GND_DARKMEIRO,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_DEMO_KEKKAI,
    sizeof(BgGndDarkmeiro),
    (ActorFunc)BgGndDarkmeiro_Init,
    (ActorFunc)BgGndDarkmeiro_Destroy,
    (ActorFunc)BgGndDarkmeiro_Update,
    NULL,
    NULL,
};

void BgGndDarkmeiro_ToggleBlock(BgGndDarkmeiro* this, PlayState* play) {
    if (this->actionFlags & 2) {
        if (this->timer1 == 0) {
            func_8003EBF8(play, &play->colCtx.dyna, this->dyna.bgId);
            this->actionFlags &= ~2;
        }
    } else if (this->timer1 != 0) {
        func_8003EC50(play, &play->colCtx.dyna, this->dyna.bgId);
        this->actionFlags |= 2;
    }
}

void BgGndDarkmeiro_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    CollisionHeader* colHeader = NULL;
    BgGndDarkmeiro* this = (BgGndDarkmeiro*)thisx;

    this->updateFunc = BgGndDarkmeiro_Noop;
    Actor_SetScale(&this->dyna.actor, 0.1f);
    switch (this->dyna.actor.params & 0xFF) {
        case DARKMEIRO_INVISIBLE_PATH:
            this->dyna.actor.draw = BgGndDarkmeiro_DrawInvisiblePath;
            this->dyna.actor.flags |= ACTOR_FLAG_7;
            break;
        case DARKMEIRO_CLEAR_BLOCK:
            CollisionHeader_GetVirtual(&gClearBlockCol, &colHeader);
            this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            if (((this->dyna.actor.params >> 8) & 0x3F) == 0x3F) {
                this->updateFunc = BgGndDarkmeiro_UpdateStaticBlock;
                this->dyna.actor.draw = BgGndDarkmeiro_DrawStaticBlock;
            } else {
                this->actionFlags = this->timer1 = this->timer2 = 0;
                thisx->draw = BgGndDarkmeiro_DrawSwitchBlock;
                this->updateFunc = BgGndDarkmeiro_UpdateSwitchBlock;
                if (!Flags_GetSwitch(play, (this->dyna.actor.params >> 8) & 0x3F)) {
                    func_8003EBF8(play, &play->colCtx.dyna, this->dyna.bgId);
                } else {
                    this->timer1 = 64;
                    this->actionFlags |= 2;
                }
            }
            break;
        case DARKMEIRO_BLOCK_TIMER:
            this->actionFlags = this->timer1 = this->timer2 = 0;
            this->updateFunc = BgGndDarkmeiro_UpdateBlockTimer;
            thisx->draw = NULL;
            if (Flags_GetSwitch(play, ((this->dyna.actor.params >> 8) & 0x3F) + 1)) {
                this->timer1 = 64;
                this->actionFlags |= 4;
            }
            if (Flags_GetSwitch(play, ((this->dyna.actor.params >> 8) & 0x3F) + 2)) {
                this->timer2 = 64;
                this->actionFlags |= 8;
            }
            if ((this->timer1 != 0) || (this->timer2 != 0)) {
                Flags_SetSwitch(play, (this->dyna.actor.params >> 8) & 0x3F);
            } else {
                Flags_UnsetSwitch(play, (this->dyna.actor.params >> 8) & 0x3F);
            }
            break;
    }
}

void BgGndDarkmeiro_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgGndDarkmeiro* this = (BgGndDarkmeiro*)thisx;

    if ((this->dyna.actor.params & 0xFF) == 1) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void BgGndDarkmeiro_Noop(BgGndDarkmeiro* this, PlayState* play) {
}

void BgGndDarkmeiro_UpdateBlockTimer(BgGndDarkmeiro* this, PlayState* play) {
    s16 timeLeft;

    if (Flags_GetSwitch(play, ((this->dyna.actor.params >> 8) & 0x3F) + 1)) {
        if (this->actionFlags & 4) {
            if (this->timer1 > 0) {
                this->timer1--;
            } else {
                Flags_UnsetSwitch(play, ((this->dyna.actor.params >> 8) & 0x3F) + 1);
                this->actionFlags &= ~4;
            }
        } else {
            this->actionFlags |= 4;
            this->timer1 = 304;
            Audio_PlaySoundGeneral(NA_SE_EV_RED_EYE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }

    if (Flags_GetSwitch(play, ((this->dyna.actor.params >> 8) & 0x3F) + 2)) {
        if (this->actionFlags & 8) {
            if (this->timer2 > 0) {
                this->timer2--;
            } else {
                Flags_UnsetSwitch(play, ((this->dyna.actor.params >> 8) & 0x3F) + 2);
                this->actionFlags &= ~8;
            }
        } else {
            this->actionFlags |= 8;
            this->timer2 = 304;
            Audio_PlaySoundGeneral(NA_SE_EV_RED_EYE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }

    timeLeft = CLAMP_MIN(this->timer1, this->timer2);
    if (timeLeft > 0) {
        func_8002F994(&this->dyna.actor, timeLeft);
    }
    if ((this->timer1 >= 64) || (this->timer2 >= 64)) {
        Flags_SetSwitch(play, (this->dyna.actor.params >> 8) & 0x3F);
    } else {
        Flags_UnsetSwitch(play, (this->dyna.actor.params >> 8) & 0x3F);
    }
}

void BgGndDarkmeiro_UpdateStaticBlock(BgGndDarkmeiro* this, PlayState* play) {
}

void BgGndDarkmeiro_UpdateSwitchBlock(BgGndDarkmeiro* this, PlayState* play) {
    if (this->timer1 > 0) {
        this->timer1--;
    }

    if (Flags_GetSwitch(play, (this->dyna.actor.params >> 8) & 0x3F)) {
        this->timer1 = 64;
    }

    BgGndDarkmeiro_ToggleBlock(this, play);
}

void BgGndDarkmeiro_Update(Actor* thisx, PlayState* play2) {
    BgGndDarkmeiro* this = (BgGndDarkmeiro*)thisx;
    PlayState* play = play2;

    this->updateFunc(this, play2);
}

void BgGndDarkmeiro_DrawInvisiblePath(Actor* thisx, PlayState* play) {
    Gfx_DrawDListXlu(play, gShadowTrialPathDL);
}

void BgGndDarkmeiro_DrawSwitchBlock(Actor* thisx, PlayState* play) {
    BgGndDarkmeiro* this = (BgGndDarkmeiro*)thisx;
    s16 vanishTimer;

    vanishTimer = this->timer1;
    if (vanishTimer != 0) {
        if (vanishTimer > 64) {
            this->timer2 = (this->timer2 < 120) ? this->timer2 + 8 : 127;
        } else if (vanishTimer > 16) {
            this->timer2 = (Math_CosS((u16)this->timer1 * 0x1000) * 64.0f) + 127.0f;
            if (this->timer2 > 127) {
                this->timer2 = 127;
            }
        } else {
            this->timer2 = vanishTimer * 8;
        }

        OPEN_DISPS(play->state.gfxCtx);
        //! @bug Due to a bug in the display list, the transparency data is not used.
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 198, 202, 208, this->timer2);
        CLOSE_DISPS(play->state.gfxCtx);

        Gfx_DrawDListXlu(play, gClearBlockDL);
    }
}

void BgGndDarkmeiro_DrawStaticBlock(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 198, 202, 208, 255);
    CLOSE_DISPS(play->state.gfxCtx);

    Gfx_DrawDListXlu(play, gClearBlockDL);
}
