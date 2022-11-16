/*
 * File: z_demo_ext.c
 * Overlay: Demo_Ext
 * Description: Magic Vortex in Silver Gauntlets Cutscene
 */

#include "z_demo_ext.h"
#include "vt.h"
#include "objects/object_fhg/object_fhg.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    /* 0x00 */ EXT_WAIT,
    /* 0x01 */ EXT_MAINTAIN,
    /* 0x02 */ EXT_DISPELL
} DemoExtAction;

typedef enum {
    /* 0x00 */ EXT_DRAW_NOTHING,
    /* 0x01 */ EXT_DRAW_VORTEX
} DemoExtDrawMode;

void DemoExt_Init(Actor* thisx, PlayState* play);
void DemoExt_Destroy(Actor* thisx, PlayState* play);
void DemoExt_Update(Actor* thisx, PlayState* play);
void DemoExt_Draw(Actor* thisx, PlayState* play);

void DemoExt_Destroy(Actor* thisx, PlayState* play) {
}

void DemoExt_Init(Actor* thisx, PlayState* play) {
    DemoExt* this = (DemoExt*)thisx;

    this->scrollIncr[0] = 25;
    this->scrollIncr[1] = 40;
    this->scrollIncr[2] = 5;
    this->scrollIncr[3] = 30;
    this->primAlpha = kREG(28) + 255;
    this->envAlpha = kREG(32) + 255;
    this->scale.x = kREG(19) + 400.0f;
    this->scale.y = kREG(20) + 100.0f;
    this->scale.z = kREG(21) + 400.0f;
}

void DemoExt_PlayVortexSFX(DemoExt* this) {
    if (this->alphaTimer <= (kREG(35) + 40.0f) - 15.0f) {
        Audio_PlaySoundGeneral(NA_SE_EV_FANTOM_WARP_L - SFX_FLAG, &this->actor.projectedPos, 4, &D_801333E0,
                               &D_801333E0, &D_801333E8);
    }
}

CsCmdActorAction* DemoExt_GetNpcAction(PlayState* play, s32 npcActionIndex) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        return play->csCtx.npcActions[npcActionIndex];
    }
    return NULL;
}

void DemoExt_SetupWait(DemoExt* this) {
    this->action = EXT_WAIT;
    this->drawMode = EXT_DRAW_NOTHING;
}

void DemoExt_SetupMaintainVortex(DemoExt* this, PlayState* play) {
    CsCmdActorAction* npcAction = DemoExt_GetNpcAction(play, 5);

    if (npcAction != NULL) {
        this->actor.world.pos.x = npcAction->startPos.x;
        this->actor.world.pos.y = npcAction->startPos.y;
        this->actor.world.pos.z = npcAction->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = npcAction->rot.y;
    }
    this->action = EXT_MAINTAIN;
    this->drawMode = EXT_DRAW_VORTEX;
}

void DemoExt_SetupDispellVortex(DemoExt* this) {
    this->action = EXT_DISPELL;
    this->drawMode = EXT_DRAW_VORTEX;
}

void DemoExt_FinishClosing(DemoExt* this) {
    this->alphaTimer += 1.0f;
    if ((kREG(35) + 40.0f) <= this->alphaTimer) {
        Actor_Kill(&this->actor);
    }
}

void DemoExt_CheckCsMode(DemoExt* this, PlayState* play) {
    CsCmdActorAction* csCmdNPCAction = DemoExt_GetNpcAction(play, 5);
    s32 csAction;
    s32 previousCsAction;

    if (csCmdNPCAction != NULL) {
        csAction = csCmdNPCAction->action;
        previousCsAction = this->previousCsAction;

        if (csAction != previousCsAction) {
            switch (csAction) {
                case 1:
                    DemoExt_SetupWait(this);
                    break;
                case 2:
                    DemoExt_SetupMaintainVortex(this, play);
                    break;
                case 3:
                    DemoExt_SetupDispellVortex(this);
                    break;
                default:
                    // "Demo_Ext_Check_DemoMode: there is no such action!"
                    osSyncPrintf("Demo_Ext_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->previousCsAction = csAction;
        }
    }
}

void DemoExt_SetScrollAndRotation(DemoExt* this) {
    s16* scrollIncr = this->scrollIncr;
    s16* curScroll = this->curScroll;
    s32 i;

    for (i = 3; i != 0; i--) {
        curScroll[i] += scrollIncr[i];
    }
    this->rotationPitch += (s16)(kREG(34) + 1000);
}

void DemoExt_SetColorsAndScales(DemoExt* this) {
    Vec3f* scale = &this->scale;
    f32 shrinkFactor;

    shrinkFactor = ((kREG(35) + 40.0f) - this->alphaTimer) / (kREG(35) + 40.0f);
    if (shrinkFactor < 0.0f) {
        shrinkFactor = 0.0f;
    }

    this->primAlpha = (u32)(kREG(28) + 255) * shrinkFactor;
    this->envAlpha = (u32)(kREG(32) + 255) * shrinkFactor;
    scale->x = (kREG(19) + 400.0f) * shrinkFactor;
    scale->y = (kREG(20) + 100.0f) * shrinkFactor;
    scale->z = (kREG(21) + 400.0f) * shrinkFactor;
}

void DemoExt_Wait(DemoExt* this, PlayState* play) {
    DemoExt_CheckCsMode(this, play);
}

void DemoExt_MaintainVortex(DemoExt* this, PlayState* play) {
    DemoExt_PlayVortexSFX(this);
    DemoExt_SetScrollAndRotation(this);
    DemoExt_CheckCsMode(this, play);
}

void DemoExt_DispellVortex(DemoExt* this, PlayState* play) {
    DemoExt_PlayVortexSFX(this);
    DemoExt_SetScrollAndRotation(this);
    DemoExt_SetColorsAndScales(this);
    DemoExt_FinishClosing(this);
}

static DemoExtActionFunc sActionFuncs[] = {
    DemoExt_Wait,
    DemoExt_MaintainVortex,
    DemoExt_DispellVortex,
};

void DemoExt_Update(Actor* thisx, PlayState* play) {
    DemoExt* this = (DemoExt*)thisx;

    if ((this->action < EXT_WAIT) || (this->action > EXT_DISPELL) || sActionFuncs[this->action] == NULL) {
        // "Main mode is abnormal!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        sActionFuncs[this->action](this, play);
    }
}

void DemoExt_DrawNothing(Actor* thisx, PlayState* play) {
}

void DemoExt_DrawVortex(Actor* thisx, PlayState* play) {
    DemoExt* this = (DemoExt*)thisx;
    Mtx* mtx;
    GraphicsContext* gfxCtx;
    s16* curScroll;
    Vec3f* scale;

    scale = &this->scale;
    gfxCtx = play->state.gfxCtx;
    mtx = Graph_Alloc(gfxCtx, sizeof(Mtx));

    OPEN_DISPS(gfxCtx);
    Matrix_Push();
    Matrix_Scale(scale->x, scale->y, scale->z, MTXMODE_APPLY);
    Matrix_RotateZYX((s16)(kREG(16) + 0x4000), this->rotationPitch, kREG(18), MTXMODE_APPLY);
    Matrix_Translate(kREG(22), kREG(23), kREG(24), MTXMODE_APPLY);
    MATRIX_TOMTX(mtx);
    Matrix_Pop();
    func_80093D84(gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, kREG(33) + 128, kREG(25) + 140, kREG(26) + 80, kREG(27) + 140, this->primAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, kREG(29) + 90, kREG(30) + 50, kREG(31) + 95, this->envAlpha);

    curScroll = this->curScroll;
    gSPSegment(
        POLY_XLU_DISP++, 0x08,
        Gfx_TwoTexScroll(gfxCtx, 0, curScroll[0], curScroll[1], 0x40, 0x40, 1, curScroll[2], curScroll[3], 0x40, 0x40));

    gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gPhantomWarpDL);
    gSPPopMatrix(POLY_XLU_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx);
}

static DemoExtDrawFunc sDrawFuncs[] = {
    DemoExt_DrawNothing,
    DemoExt_DrawVortex,
};

void DemoExt_Draw(Actor* thisx, PlayState* play) {
    DemoExt* this = (DemoExt*)thisx;

    if ((this->drawMode < EXT_DRAW_NOTHING) || (this->drawMode > EXT_DRAW_VORTEX) ||
        sDrawFuncs[this->drawMode] == NULL) {
        // "Draw mode is abnormal!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        sDrawFuncs[this->drawMode](thisx, play);
    }
}

const ActorInit Demo_Ext_InitVars = {
    ACTOR_DEMO_EXT,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_FHG,
    sizeof(DemoExt),
    (ActorFunc)DemoExt_Init,
    (ActorFunc)DemoExt_Destroy,
    (ActorFunc)DemoExt_Update,
    (ActorFunc)DemoExt_Draw,
    NULL,
};
