#include "z_efc_erupc.h"
#include "objects/object_efc_erupc/object_efc_erupc.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EfcErupc_Init(Actor* thisx, GlobalContext* globalCtx);
void EfcErupc_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EfcErupc_Update(Actor* thisx, GlobalContext* globalCtx);
void EfcErupc_Draw(Actor* thisx, GlobalContext* globalCtx);

void EfcErupc_UpdateAction(EfcErupc* this, GlobalContext* globalCtx);
void EfcErupc_DrawParticles(EfcErupcParticles* particles, GlobalContext* globalCtx);
void EfcErupc_UpdateParticles(EfcErupc* this, GlobalContext* globalCtx);
void EfcErupc_AddParticle(EfcErupcParticles* particles, Vec3f* pos, Vec3f* vel, Vec3f* accel, f32 scaleFactor);
void EfcErupc_InitParticles(EfcErupcParticles* particles);

const ActorInit Efc_Erupc_InitVars = {
    ACTOR_EFC_ERUPC,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_EFC_ERUPC,
    sizeof(EfcErupc),
    (ActorFunc)EfcErupc_Init,
    (ActorFunc)EfcErupc_Destroy,
    (ActorFunc)EfcErupc_Update,
    (ActorFunc)EfcErupc_Draw,
    NULL,
};

void EfcErupc_SetupAction(EfcErupc* this, EfcErupcActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EfcErupc_Init(Actor* thisx, GlobalContext* globalCtx) {
    EfcErupc* this = (EfcErupc*)thisx;

    EfcErupc_SetupAction(this, EfcErupc_UpdateAction);
    Actor_SetScale(&this->actor, 1.0f);
    EfcErupc_InitParticles(this->particles);
    this->unk14C = this->unk14E = this->unk150 = 0;
    this->unk152 = 5;
    this->unk154 = -100;
}

void EfcErupc_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EfcErupc_UpdateAction(EfcErupc* this, GlobalContext* globalCtx) {
    Vec3f pos;
    Vec3f vel;
    Vec3f accel;
    s32 i;

    if (globalCtx->csCtx.state != 0) {
        if (globalCtx->csCtx.npcActions[1] != NULL) {
            if (globalCtx->csCtx.npcActions[1]->action == 2) {
                if (this->unk150 == 30) {
                    func_800788CC(NA_SE_IT_EARTHQUAKE);
                }
                if (this->unk150 <= 64) {
                    if (this->unk154 < 200) {
                        this->unk154 += 10;
                    }
                } else {
                    if (this->unk154 > -100) {
                        this->unk154 -= 10;
                    }
                }
                this->unk150++;
            } else {
                if (this->unk154 > -100) {
                    this->unk154 -= 10;
                }
            }
        }
    }
    if (globalCtx->csCtx.state != 0) {
        if (globalCtx->csCtx.npcActions[2] != NULL) {
            switch (globalCtx->csCtx.npcActions[2]->action) {
                case 2:
                    if (this->unk14E == 0) {
                        func_800F3F3C(6);
                        gSaveContext.eventChkInf[2] |= 0x8000;
                    }
                    this->unk14E++;
                    break;
                case 3:
                    this->unk14E = 30;
            }
            this->unk14C++;
        }
    }
    accel.z = 0.0f;
    accel.x = 0.0f;
    pos.y = this->actor.world.pos.y + 300.0f;
    for (i = 0; i < this->unk152; i++) {
        pos.x = Rand_CenteredFloat(100.0f) + this->actor.world.pos.x;
        pos.z = Rand_CenteredFloat(100.0f) + this->actor.world.pos.z;
        vel.x = Rand_CenteredFloat(100.0f);
        vel.y = Rand_ZeroFloat(100.0f);
        vel.z = Rand_CenteredFloat(100.0f);
        accel.y = this->unk154 * 0.1f;
        EfcErupc_AddParticle(this->particles, &pos, &vel, &accel, 80.0f);
    }
}

void EfcErupc_Update(Actor* thisx, GlobalContext* globalCtx) {
    EfcErupc* this = (EfcErupc*)thisx;

    this->actionFunc(this, globalCtx);
    EfcErupc_UpdateParticles(this, globalCtx);
}

void EfcErupc_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EfcErupc* this = (EfcErupc*)thisx;
    u16 csAction;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, this->unk14C * 1, this->unk14E * -4, 32, 64, 1,
                                this->unk14C * 4, this->unk14E * -20, 64, 64));

    gSPSegment(
        POLY_XLU_DISP++, 0x09,
        Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, this->unk150 * -4, 16, 128, 1, 0, this->unk150 * 12, 32, 32));

    gSPSegment(
        POLY_XLU_DISP++, 0x0A,
        Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, this->unk150 * -4, 16, 128, 1, 0, this->unk150 * 12, 32, 32));

    Matrix_Push();
    Matrix_Scale(0.8f, 0.8f, 0.8f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (globalCtx->csCtx.state != 0) {
        if ((globalCtx->csCtx.npcActions[1] != 0) && (globalCtx->csCtx.npcActions[1]->action == 2)) {
            gSPDisplayList(POLY_XLU_DISP++, object_efc_erupc_DL_002570);
        }
    }
    Matrix_Pop();
    Matrix_Scale(3.4f, 3.4f, 3.4f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    if (globalCtx->csCtx.state != 0) {
        CsCmdActorAction* csActorAction = globalCtx->csCtx.npcActions[2];
        if (csActorAction != 0) {
            csAction = csActorAction->action;
            if ((csAction == 2) || (csAction == 3)) {
                gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 200, 255);
                gDPSetEnvColor(POLY_XLU_DISP++, 100, 0, 0, 255);
                gSPDisplayList(POLY_XLU_DISP++, object_efc_erupc_DL_001720);
            }
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx);
    EfcErupc_DrawParticles(this->particles, globalCtx);
}

void EfcErupc_DrawParticles(EfcErupcParticles* particles, GlobalContext* globalCtx) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s16 i;
    s32 pad;

    OPEN_DISPS(gfxCtx);
    for (i = 0; i < EFC_ERUPC_NUM_PARTICLES; i++, particles++) {
        if (particles->isActive) {
            func_80093D84(globalCtx->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, object_efc_erupc_DL_002760);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, particles->color.r, particles->color.g, particles->color.b,
                            particles->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 150, 0, 0, 0);
            gDPPipeSync(POLY_XLU_DISP++);
            Matrix_Translate(particles->pos.x, particles->pos.y, particles->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(particles->scale, particles->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_efc_erupc_DL_0027D8);
        }
    }
    CLOSE_DISPS(gfxCtx);
}

static Color_RGB8 D_8099D770[] = {
    { 255, 128, 0 },
    { 255, 0, 0 },
    { 255, 255, 0 },
    { 255, 0, 0 },
};

void EfcErupc_UpdateParticles(EfcErupc* this, GlobalContext* globalCtx) {
    s16 i;
    s16 index;
    Color_RGB8 particleColors[] = {
        { 255, 128, 0 },
        { 255, 0, 0 },
        { 0, 0, 0 },
        { 100, 0, 0 },
    };
    Color_RGB8* color;
    EfcErupcParticles* cur = this->particles;

    for (i = 0; i < EFC_ERUPC_NUM_PARTICLES; i++, cur++) {
        if (cur->isActive) {
            cur->pos.x += cur->vel.x;
            cur->pos.y += cur->vel.y;
            cur->pos.z += cur->vel.z;
            cur->vel.x += cur->accel.x;
            cur->vel.y += cur->accel.y;
            cur->vel.z += cur->accel.z;
            cur->animTimer += 1;
            index = cur->animTimer % 4;
            color = &particleColors[index];
            cur->color.r = color->r;
            cur->color.g = color->g;
            cur->color.b = color->b;
            cur->alpha -= 20;
            if (cur->alpha <= 0) {
                cur->alpha = 0;
                cur->isActive = false;
            }
        }
    }
}

void EfcErupc_AddParticle(EfcErupcParticles* particles, Vec3f* pos, Vec3f* vel, Vec3f* accel, f32 scaleFactor) {
    s16 i;

    for (i = 0; i < EFC_ERUPC_NUM_PARTICLES; i++, particles++) {
        if (!particles->isActive) {
            particles->isActive = true;
            particles->pos = *pos;
            particles->vel = *vel;
            particles->accel = *accel;
            particles->scale = scaleFactor / 1000.0f;
            particles->alpha = 255;
            particles->animTimer = (s16)Rand_ZeroFloat(10.0f);
            return;
        }
    }
}

void EfcErupc_InitParticles(EfcErupcParticles* particles) {
    s16 i;

    for (i = 0; i < EFC_ERUPC_NUM_PARTICLES; i++, particles++) {
        particles->isActive = false;
    }
}
