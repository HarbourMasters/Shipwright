/*
 * File: z_demo_shd.c
 * Overlay: Demo_Shd
 * Description: Bongo Bongo's Shadow
 */

#include "z_demo_shd.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void DemoShd_Init(Actor* thisx, GlobalContext* globalCtx);
void DemoShd_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DemoShd_Update(Actor* thisx, GlobalContext* globalCtx);
void DemoShd_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80991298(DemoShd* this, GlobalContext* globalCtx);

const ActorInit Demo_Shd_InitVars = {
    ACTOR_DEMO_SHD,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(DemoShd),
    (ActorFunc)DemoShd_Init,
    (ActorFunc)DemoShd_Destroy,
    (ActorFunc)DemoShd_Update,
    (ActorFunc)DemoShd_Draw,
    NULL,
};

#include "overlays/ovl_Demo_Shd/ovl_Demo_Shd.h"

void DemoShd_SetupAction(DemoShd* this, DemoShdActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void DemoShd_Init(Actor* thisx, GlobalContext* globalCtx) {
    DemoShd* this = (DemoShd*)thisx;

    this->unk_14C = 0;
    DemoShd_SetupAction(this, func_80991298);
    Actor_SetScale(&this->actor, 0.4f);
    this->actor.world.pos.y = 0.0f;
    this->actor.world.pos.x = 0.0f;
}

void DemoShd_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void func_80991298(DemoShd* this, GlobalContext* globalCtx) {
    if ((globalCtx->csCtx.state != CS_STATE_IDLE && globalCtx->csCtx.npcActions[0] != NULL) ||
        (globalCtx->csCtx.state != CS_STATE_IDLE && globalCtx->csCtx.npcActions[1] != NULL)) {
        if (globalCtx->csCtx.frames == 800) {
            func_800F3F3C(9);
        }
        if (globalCtx->csCtx.frames == 1069) {
            func_800F3F3C(8);
        }
    }

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction0 = globalCtx->csCtx.npcActions[0];

        if (npcAction0 != NULL) {
            if (npcAction0->action == 2) {
                if (!(this->unk_14C & 1)) {
                    this->unk_14E = npcAction0->startPos.x;
                }
                this->unk_14C |= 1;
            } else {
                this->unk_14C &= ~1;
            }
        }
    }

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction1 = globalCtx->csCtx.npcActions[1];

        if (npcAction1 != NULL) {
            if (npcAction1->action == 2) {
                if (!(this->unk_14C & 2)) {
                    this->unk_14E = npcAction1->startPos.x;
                }
                this->unk_14C |= 2;
            } else {
                this->unk_14C &= ~2;
            }
        }
    }

    this->unk_14E++;
}

void DemoShd_Update(Actor* thisx, GlobalContext* globalCtx) {
    DemoShd* this = (DemoShd*)thisx;

    this->actionFunc(this, globalCtx);
}

void DemoShd_Draw(Actor* thisx, GlobalContext* globalCtx) {
    DemoShd* this = (DemoShd*)thisx;
    s32 pad;
    u32 unk_14E = this->unk_14E;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, D_809932D0);

    if (this->unk_14C & 1) {
        gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0x3FF - ((unk_14E * 5) & 0x3FF),
                                                         16, 256, 1, 0, 255 - ((unk_14E * 5) & 255), 32, 32));
        gSPDisplayList(POLY_XLU_DISP++, D_80993390);
    } else if (this->unk_14C & 2) {
        gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0x3FF - ((unk_14E * 5) & 0x3FF),
                                                         16, 256, 1, 0, 255 - ((unk_14E * 5) & 255), 32, 32));
        gSPDisplayList(POLY_XLU_DISP++, D_809934B8);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
