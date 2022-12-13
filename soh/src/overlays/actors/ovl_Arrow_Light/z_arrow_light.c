/*
 * File: z_arrow_light.c
 * Overlay: ovl_Arrow_Light
 * Description: Light Arrow. Spawned as a child of a normal arrow.
 */

#include "z_arrow_light.h"

#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void ArrowLight_Init(Actor* thisx, PlayState* play);
void ArrowLight_Destroy(Actor* thisx, PlayState* play);
void ArrowLight_Update(Actor* thisx, PlayState* play);
void ArrowLight_Draw(Actor* thisx, PlayState* play);

void ArrowLight_Charge(ArrowLight* this, PlayState* play);
void ArrowLight_Fly(ArrowLight* this, PlayState* play);
void ArrowLight_Hit(ArrowLight* this, PlayState* play);

#include "overlays/ovl_Arrow_Light/ovl_Arrow_Light.h"

const ActorInit Arrow_Light_InitVars = {
    ACTOR_ARROW_LIGHT,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ArrowLight),
    (ActorFunc)ArrowLight_Init,
    (ActorFunc)ArrowLight_Destroy,
    (ActorFunc)ArrowLight_Update,
    (ActorFunc)ArrowLight_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_STOP),
};

void ArrowLight_SetupAction(ArrowLight* this, ArrowLightActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void ArrowLight_Init(Actor* thisx, PlayState* play) {
    ArrowLight* this = (ArrowLight*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->radius = 0;
    this->unk_160 = 1.0f;
    ArrowLight_SetupAction(this, ArrowLight_Charge);
    Actor_SetScale(&this->actor, 0.01f);
    this->alpha = 130;
    this->timer = 0;
    this->unk_164 = 0.0f;
}

void ArrowLight_Destroy(Actor* thisx, PlayState* play) {
    func_800876C8(play);
    LOG_STRING("消滅"); // "Disappearance"
}

void ArrowLight_Charge(ArrowLight* this, PlayState* play) {
    EnArrow* arrow = (EnArrow*)this->actor.parent;

    if ((arrow == NULL) || (arrow->actor.update == NULL)) {
        Actor_Kill(&this->actor);
        return;
    }

    if (this->radius < 10) {
        this->radius += 1;
    }
    // copy position and rotation from arrow
    this->actor.world.pos = arrow->actor.world.pos;
    this->actor.shape.rot = arrow->actor.shape.rot;

    func_8002F974(&this->actor, NA_SE_PL_ARROW_CHARGE_LIGHT - SFX_FLAG);

    // if arrow has no parent, player has fired the arrow
    if (arrow->actor.parent == NULL) {
        this->unkPos = this->actor.world.pos;
        this->radius = 10;
        ArrowLight_SetupAction(this, ArrowLight_Fly);
        this->alpha = 255;
    }
}

void func_80869E6C(Vec3f* unkPos, Vec3f* lightPos, f32 scale) {
    unkPos->x += ((lightPos->x - unkPos->x) * scale);
    unkPos->y += ((lightPos->y - unkPos->y) * scale);
    unkPos->z += ((lightPos->z - unkPos->z) * scale);
}

void ArrowLight_Hit(ArrowLight* this, PlayState* play) {
    f32 scale;
    f32 offset;
    u16 timer;

    if (this->actor.projectedW < 50.0f) {
        scale = 10.0f;
    } else {
        if (950.0f < this->actor.projectedW) {
            scale = 310.0f;
        } else {
            scale = this->actor.projectedW;
            scale = ((scale - 50.0f) * (1.0f / 3.0f)) + 10.0f;
        }
    }

    timer = this->timer;
    if (timer != 0) {
        this->timer -= 1;

        if (this->timer >= 8) {
            offset = ((this->timer - 8) * (1.0f / 24.0f));
            offset = SQ(offset);
            this->radius = (((1.0f - offset) * scale) + 10.0f);
            this->unk_160 += ((2.0f - this->unk_160) * 0.1f);
            if (this->timer < 16) {
                this->alpha = ((this->timer * 0x23) - 0x118);
            }
        }
    }

    if (this->timer >= 9) {
        if (this->unk_164 < 1.0f) {
            this->unk_164 += 0.25f;
        }
    } else {
        if (this->unk_164 > 0.0f) {
            this->unk_164 -= 0.125f;
        }
    }

    if (this->timer < 8) {
        this->alpha = 0;
    }

    if (this->timer == 0) {
        this->timer = 255;
        Actor_Kill(&this->actor);
    }
}

void ArrowLight_Fly(ArrowLight* this, PlayState* play) {
    EnArrow* arrow = (EnArrow*)this->actor.parent;
    f32 distanceScaled;
    s32 pad;

    if ((arrow == NULL) || (arrow->actor.update == NULL)) {
        Actor_Kill(&this->actor);
        return;
    }
    // copy position and rotation from parent arrow
    this->actor.world.pos = arrow->actor.world.pos;
    this->actor.shape.rot = arrow->actor.shape.rot;
    distanceScaled = Math_Vec3f_DistXYZ(&this->unkPos, &this->actor.world.pos) * (1.0f / 24.0f);
    this->unk_160 = distanceScaled;
    if (distanceScaled < 1.0f) {
        this->unk_160 = 1.0f;
    }
    func_80869E6C(&this->unkPos, &this->actor.world.pos, 0.05f);

    if (arrow->hitFlags & 1) {
        Audio_PlayActorSound2(&this->actor, NA_SE_IT_EXPLOSION_LIGHT);
        ArrowLight_SetupAction(this, ArrowLight_Hit);
        this->timer = 32;
        this->alpha = 255;
    } else if (arrow->timer < 34) {
        if (this->alpha < 35) {
            Actor_Kill(&this->actor);
        } else {
            this->alpha -= 0x19;
        }
    }
}

void ArrowLight_Update(Actor* thisx, PlayState* play) {
    ArrowLight* this = (ArrowLight*)thisx;

    if (play->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK ||
        play->msgCtx.msgMode == MSGMODE_SONG_PLAYED) {
        Actor_Kill(&this->actor);
    } else {
        this->actionFunc(this, play);
    }
}

void ArrowLight_Draw(Actor* thisx, PlayState* play) {
    ArrowLight* this = (ArrowLight*)thisx;
    s32 pad;
    u32 stateFrames = play->state.frames;
    EnArrow* arrow = (EnArrow*)this->actor.parent;
    Actor* tranform;

    Color_RGB8 primaryColor = {255, 255, 170};
    if (CVar_GetS32("gCosmetics.Arrows_LightPrimary.Changed", 0)) {
        primaryColor = CVar_GetRGB("gCosmetics.Arrows_LightPrimary.Value", primaryColor);
    }
    Color_RGB8 secondaryColor = {255, 255, 0};
    if (CVar_GetS32("gCosmetics.Arrows_LightSecondary.Changed", 0)) {
        secondaryColor = CVar_GetRGB("gCosmetics.Arrows_LightSecondary.Value", secondaryColor);
    }

    if ((arrow != NULL) && (arrow->actor.update != NULL) && (this->timer < 255)) {
        tranform = (arrow->hitFlags & 2) ? &this->actor : &arrow->actor;

        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Translate(tranform->world.pos.x, tranform->world.pos.y, tranform->world.pos.z, MTXMODE_NEW);
        Matrix_RotateY(tranform->shape.rot.y * (M_PI / 0x8000), MTXMODE_APPLY);
        Matrix_RotateX(tranform->shape.rot.x * (M_PI / 0x8000), MTXMODE_APPLY);
        Matrix_RotateZ(tranform->shape.rot.z * (M_PI / 0x8000), MTXMODE_APPLY);
        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);

        // Draw yellow effect over the screen when arrow hits
        if (this->unk_164 > 0) {
            POLY_XLU_DISP = Gfx_SetupDL_57(POLY_XLU_DISP);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 
                (s32)((secondaryColor.r / 6) * this->unk_164) & 0xFF,
                (s32)((secondaryColor.g / 6) * this->unk_164) & 0xFF, 
                (s32)((secondaryColor.b / 6) * this->unk_164) & 0xFF,
                (s32)(150.0f * this->unk_164) & 0xFF);
            gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
            gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
            gDPFillRectangle(POLY_XLU_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
        }

        // Draw light on the arrow
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, primaryColor.r, primaryColor.g, primaryColor.b, this->alpha);
        gDPSetEnvColor(POLY_XLU_DISP++, secondaryColor.r, secondaryColor.g, secondaryColor.b, 128);
        Matrix_RotateZYX(0x4000, 0x0, 0x0, MTXMODE_APPLY);
        if (this->timer != 0) {
            Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        } else {
            Matrix_Translate(0.0f, 1500.0f, 0.0f, MTXMODE_APPLY);
        }
        Matrix_Scale(this->radius * 0.2f, this->unk_160 * 4.0f, this->radius * 0.2f, MTXMODE_APPLY);
        Matrix_Translate(0.0f, -700.0f, 0.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, sMaterialDL);
        gSPDisplayList(POLY_XLU_DISP++,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, 511 - (stateFrames * 5) % 512, 0, 4, 32, 1,
                                        511 - (stateFrames * 10) % 512, 511 - (stateFrames * 30) % 512, 8, 16));
        gSPDisplayList(POLY_XLU_DISP++, sModelDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
