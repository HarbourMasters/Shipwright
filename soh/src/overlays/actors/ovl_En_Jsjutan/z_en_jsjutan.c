/*
 * File: z_en_jsjutan.c
 * Overlay: ovl_En_Jsjutan
 * Description: Magic carpet man's carpet
 */

#include "z_en_jsjutan.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnJsjutan_Init(Actor* thisx, PlayState* play);
void EnJsjutan_Destroy(Actor* thisx, PlayState* play);
void EnJsjutan_Update(Actor* thisx, PlayState* play);
void EnJsjutan_Draw(Actor* thisx, PlayState* play);

const ActorInit En_Jsjutan_InitVars = {
    ACTOR_EN_JSJUTAN,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnJsjutan),
    (ActorFunc)EnJsjutan_Init,
    (ActorFunc)EnJsjutan_Destroy,
    (ActorFunc)EnJsjutan_Update,
    (ActorFunc)EnJsjutan_Draw,
    NULL,
};

// Shadow texture. 32x64 I8.
static u8 sShadowTex[0x800];

static Vec3s D_80A8EE10[0x90];

static s32 sUnused[2] = { 0, 0 };

#include "overlays/ovl_En_Jsjutan/ovl_En_Jsjutan.h"

void EnJsjutan_Init(Actor* thisx, PlayState* play) {
    EnJsjutan* this = (EnJsjutan*)thisx;
    s32 pad;
    CollisionHeader* header = NULL;

    this->dyna.actor.flags &= ~ACTOR_FLAG_0;
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&sCol, &header);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, header);
    Actor_SetScale(thisx, 0.02f);
    this->unk_164 = true;
    this->shadowAlpha = 100.0f;
}

void EnJsjutan_Destroy(Actor* thisx, PlayState* play) {
    EnJsjutan* this = (EnJsjutan*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80A89860(EnJsjutan* this, PlayState* play) {
    s16 i;
    Vtx* oddVtx;
    Vtx* evenVtx;
    Vec3f actorPos = this->dyna.actor.world.pos;

    oddVtx = ResourceMgr_LoadVtxByName(SEGMENTED_TO_VIRTUAL(gShadowOddVtx));
    evenVtx = ResourceMgr_LoadVtxByName(SEGMENTED_TO_VIRTUAL(sShadowEvenVtx));

    for (i = 0; i < ARRAY_COUNT(D_80A8EE10); i++, oddVtx++, evenVtx++) {
        D_80A8EE10[i].x = oddVtx->v.ob[0];
        D_80A8EE10[i].z = oddVtx->v.ob[2];
        if (this->dyna.actor.params == ENJSJUTAN_TYPE_01) {
            oddVtx->v.ob[1] = evenVtx->v.ob[1] = 0x585;
        } else {
            this->dyna.actor.world.pos.x = oddVtx->v.ob[0] * 0.02f + actorPos.x;
            this->dyna.actor.world.pos.z = oddVtx->v.ob[2] * 0.02f + actorPos.z;
            Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 10.0f, 10.0f, 10.0f, 4);
            oddVtx->v.ob[1] = evenVtx->v.ob[1] = this->dyna.actor.floorHeight;
            this->dyna.actor.world.pos = actorPos;
        }
    }
}

void func_80A89A6C(EnJsjutan* this, PlayState* play) {
    u8 isPlayerOnTop = false; // sp127
    s16 i;
    s16 j;
    Vtx* carpetVtx;
    Vtx* shadowVtx;
    Vtx* phi_s0_2;
    Vec3f sp108;
    Vec3f spFC;
    Actor* actorProfessor;
    Actor* actorBeanGuy;
    f32 dxVtx;
    f32 dyVtx;
    f32 dzVtx;
    f32 distVtx;
    // 0 if no actor in that index of diffToTracked
    u8 spE0[3];
    // Tracks distance to other actors.
    // Index 0 is always the Magic Carpet Man. 1 and 2 could be bombs, or EnMk and EnMs if in credits.
    f32 spD4[3]; // diffToTracked X
    f32 spC8[3]; // diffToTracked Y
    f32 spBC[3]; // diffToTracked Z
    // Tracks distance to Link
    f32 spB8; // diffToPlayer X
    f32 spB4; // diffToPlayer Y
    f32 spB0; // diffToPlayer Z
    f32 weight;
    f32 spA8; // wave amplitude (?)
    f32 offset;
    f32 maxOffset;
    f32 maxAmp;
    f32 waveform;
    Player* player = GET_PLAYER(play);
    Actor* parent = this->dyna.actor.parent;
    Actor* actorExplosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
    u8 isInCreditsScene = false; // sp8B

    if (play->gameplayFrames % 2 != 0) {
        carpetVtx = SEGMENTED_TO_VIRTUAL(sCarpetOddVtx);
        shadowVtx = SEGMENTED_TO_VIRTUAL(gShadowOddVtx);
    } else {
        carpetVtx = SEGMENTED_TO_VIRTUAL(sCarpetEvenVtx);
        shadowVtx = SEGMENTED_TO_VIRTUAL(sShadowEvenVtx);
    }

    carpetVtx = ResourceMgr_LoadVtxByName(carpetVtx);
    shadowVtx = ResourceMgr_LoadVtxByName(shadowVtx);

    // Distance of player to carpet.
    spB8 = (player->actor.world.pos.x - this->dyna.actor.world.pos.x) * 50.0f;
    spB4 = (player->actor.world.pos.y - this->unk_168) * 50.0f;
    spB0 = (player->actor.world.pos.z - this->dyna.actor.world.pos.z) * 50.0f;
    phi_s0_2 = carpetVtx;

    if ((fabsf(spB8) < 5500.0f) && (fabsf(spB4) < 3000.0f) && (fabsf(spB0) < 5500.0f)) {
        isPlayerOnTop = true;
    }

    // Distance of Magic Carpet Salesman to carpet.
    spD4[0] = (parent->world.pos.x - this->dyna.actor.world.pos.x) * 50.0f;
    spC8[0] = ((parent->world.pos.y - 8.0f) - this->unk_168) * 50.0f;
    spBC[0] = (parent->world.pos.z - this->dyna.actor.world.pos.z) * 50.0f;
    spE0[0] = 1;

    for (i = 1; i < 3; i++) {
        spE0[i] = 0;
    }

    i = 1;

    // Credits scene. The magic carpet man is friends with the bean guy and the lakeside professor.
    if ((gSaveContext.entranceIndex == 0x157) && (gSaveContext.sceneSetupIndex == 8)) {
        isInCreditsScene = true;

        actorProfessor = play->actorCtx.actorLists[ACTORCAT_NPC].head;
        while (actorProfessor != NULL) {
            if (actorProfessor->id == ACTOR_EN_MK) {
                break;
            }
            actorProfessor = actorProfessor->next;
        }

        actorBeanGuy = play->actorCtx.actorLists[ACTORCAT_NPC].head;
        while (actorBeanGuy != NULL) {
            if (actorBeanGuy->id == ACTOR_EN_MS) {
                break;
            }
            actorBeanGuy = actorBeanGuy->next;
        }

        spD4[1] = 50.0f * (actorProfessor->world.pos.x - this->dyna.actor.world.pos.x);
        spC8[1] = 50.0f * (actorProfessor->world.pos.y - this->unk_168);
        spBC[1] = 50.0f * (actorProfessor->world.pos.z - this->dyna.actor.world.pos.z);
        spE0[1] = 1;

        spD4[2] = 50.0f * (actorBeanGuy->world.pos.x - this->dyna.actor.world.pos.x);
        spC8[2] = 50.0f * (actorBeanGuy->world.pos.y - this->unk_168);
        spBC[2] = 50.0f * (actorBeanGuy->world.pos.z - this->dyna.actor.world.pos.z);
        spE0[2] = 1;
    } else {
        // Player can place bombs in carpet and it will react to it.
        while (actorExplosive != NULL) {
            if (i < 3) {
                spD4[i] = (actorExplosive->world.pos.x - this->dyna.actor.world.pos.x) * 50.0f;
                spC8[i] = (actorExplosive->world.pos.y - this->unk_168) * 50.0f;
                spBC[i] = (actorExplosive->world.pos.z - this->dyna.actor.world.pos.z) * 50.0f;

                if ((fabsf(spD4[i]) < 5500.0f) && (fabsf(spC8[i]) < 3000.0f) && (fabsf(spBC[i]) < 5500.0f)) {
                    if (actorExplosive->params == BOMB_EXPLOSION) {
                        spE0[i] = 35; // Code never checks this, so it goes unused. Maybe it was planned to damage the
                                      // carpet with explosions (?)
                    } else {
                        spE0[i] = 1;
                    }
                }
                i++;
            }
            actorExplosive = actorExplosive->next;
        }
    }

    // Fancy math to make a woobly and reactive carpet.
    for (i = 0; i < ARRAY_COUNT(D_80A8EE10); i++, carpetVtx++, shadowVtx++) {
        if (isPlayerOnTop) {
            // Linear distance from j-th wave to player, in XZ plane.
            dxVtx = carpetVtx->n.ob[0] - spB8;
            dzVtx = carpetVtx->n.ob[2] - spB0;
            distVtx = sqrtf(SQ(dxVtx) + SQ(dzVtx));

            // Distance percentage. 0.0f to 1.0f. 2500.0f is the max distance to an actor that this wave will consider.
            weight = (2500.0f - distVtx) / 2500.0f;
            if (weight < 0.0f) {
                weight = 0.0f;
            }
            offset = (spB4 * weight) + ((this->unk_170 - (this->unk_170 * weight)) - 200.0f);

            distVtx -= 1500.0f;
            if (distVtx < 0.0f) {
                distVtx = 0.0f;
            }

            spA8 = 100.0f * distVtx * 0.01f;
            spA8 = CLAMP_MAX(spA8, 100.0f);

        } else {
            offset = this->unk_170 - 200.f;
            spA8 = 100.0f;
        }

        for (j = 0; j < 3; j++) {
            if (spE0[j] != 0) {
                dxVtx = carpetVtx->n.ob[0] - spD4[j];
                dzVtx = carpetVtx->n.ob[2] - spBC[j];
                // Linear distance from j-th wave to whatever actor is there, in XZ plane.
                distVtx = sqrtf(SQ(dxVtx) + SQ(dzVtx));

                if ((j == 0) || isInCreditsScene) {
                    weight = (3000.0f - distVtx) / 3000.0f;
                } else {
                    weight = (2000.0f - distVtx) / 2000.0f;
                }
                if (weight < 0.0f) {
                    weight = 0.0f;
                }

                // should be the following, but doesn't match that way.
                // maxoffset = (spC8[i] * weight) + ((this->unk_170 - (this->unk_170 * weight)) - 200.0f);
                maxOffset = (spC8[j] * weight);
                maxOffset += ((this->unk_170 - (this->unk_170 * weight)) - 200.0f);

                distVtx -= 1500.0f;
                if (distVtx < 0.0f) {
                    distVtx = 0.0f;
                }

                maxAmp = 100.0f * distVtx * 0.01f;
                maxAmp = CLAMP_MAX(maxAmp, 100.0f);

                offset = CLAMP_MAX(offset, maxOffset);

                spA8 = CLAMP_MAX(spA8, maxAmp);
            }
        }

        /**
         * See https://en.wikipedia.org/wiki/Sine_wave#General_form
         * k: 10000
         * x: j
         * w: 4000
         * t: gameplayFrames
         * A: spA8
         * D: phi_f28
         */
        waveform = spA8 * Math_SinS(play->gameplayFrames * 4000 + i * 10000);

        if (this->unk_174) {
            s16 phi_v1_4 = offset + waveform;
            s16 temp_a0_3 = (shadowVtx->n.ob[1] - this->unk_168) * 50.0f;

            if (phi_v1_4 < temp_a0_3) {
                phi_v1_4 = temp_a0_3;
            }

            carpetVtx->n.ob[1] = phi_v1_4;
        } else {
            carpetVtx->n.ob[1] = offset + waveform;

            carpetVtx->n.ob[0] = D_80A8EE10[i].x + (s16)(waveform * 0.5f);
            carpetVtx->n.ob[2] = D_80A8EE10[i].z + (s16)(waveform * 0.5f);

            shadowVtx->n.ob[0] = D_80A8EE10[i].x + (s16)waveform;
            shadowVtx->n.ob[2] = D_80A8EE10[i].z + (s16)waveform;
        }
    }

    if (!this->unk_174) {
        u16 dayTime;

        this->dyna.actor.velocity.y = 0.0f;
        this->dyna.actor.world.pos.y = this->unk_168;

        dayTime = gSaveContext.dayTime;
        if (dayTime >= 0x8000) {
            dayTime = 0xFFFF - dayTime;
        }

        this->shadowAlpha = (dayTime * 0.00275f) + 10.0f; // (1.0f / 364.0f) ?
        this->unk_170 = 1000.0f;
    } else {
        Math_ApproachF(&this->dyna.actor.world.pos.y, this->unk_168 - 1000.0f, 1.0f, this->dyna.actor.velocity.y);
        Math_ApproachF(&this->dyna.actor.velocity.y, 5.0f, 1.0f, 0.5f);
        Math_ApproachF(&this->shadowAlpha, 0.0f, 1.0f, 3.0f);
        Math_ApproachF(&this->unk_170, -5000.0f, 1.0f, 100.0f);
    }

    carpetVtx = phi_s0_2;

    sp108.x = 0.0f;
    sp108.y = 0.0f;
    sp108.z = 120.0f;

    // Fancy math to smooth each part of the wave considering its neighborhood.
    for (i = 0; i < ARRAY_COUNT(sCarpetOddVtx); i++, carpetVtx++) {
        f32 rotX;
        f32 rotZ;
        s32 pad;

        // Carpet size is 12x12.
        if ((i % 12) == 11) { // Last column.
            j = i - 1;
            dzVtx = carpetVtx->n.ob[2] - phi_s0_2[j].n.ob[2];
        } else {
            j = i + 1;
            dzVtx = phi_s0_2[j].n.ob[2] - carpetVtx->n.ob[2];
        }

        dyVtx = phi_s0_2[j].n.ob[1] - carpetVtx->n.ob[1];

        rotX = Math_Atan2F(dzVtx, dyVtx);

        if (i >= 132) { // Last row.
            j = i - 12;
            dxVtx = carpetVtx->n.ob[0] - phi_s0_2[j].n.ob[0];
        } else {
            j = i + 12;
            dxVtx = phi_s0_2[j].n.ob[0] - carpetVtx->n.ob[0];
        }

        rotZ = Math_Atan2F(dxVtx, dyVtx);

        Matrix_RotateX(rotX, MTXMODE_NEW);
        Matrix_RotateZ(rotZ, MTXMODE_APPLY);
        Matrix_MultVec3f(&sp108, &spFC);

        carpetVtx->n.n[0] = spFC.x;
        carpetVtx->n.n[1] = spFC.y;
        carpetVtx->n.n[2] = spFC.z;
    }
}

void EnJsjutan_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;

    thisx->shape.rot.x = Math_SinS(play->gameplayFrames * 3000) * 300.0f;
    thisx->shape.rot.z = Math_CosS(play->gameplayFrames * 3500) * 300.0f;
}

void EnJsjutan_Draw(Actor* thisx, PlayState* play2) {
    EnJsjutan* this = (EnJsjutan*)thisx;
    PlayState* play = play2;
    s16 i;
    Actor* parent = thisx->parent;

    OPEN_DISPS(play->state.gfxCtx);

    if (thisx->params == ENJSJUTAN_TYPE_01) {
        thisx->world.pos.x = parent->world.pos.x;
        thisx->world.pos.y = parent->world.pos.y;
        thisx->world.pos.z = parent->world.pos.z;
        this->unk_168 = thisx->world.pos.y;
        if (!this->unk_175) {
            this->unk_175 = true;
            func_80A89860(this, play);
        }
    } else if (!this->unk_175) {
        this->unk_175 = true;
        thisx->world.pos.x = Math_SinS(parent->shape.rot.y) * 60.0f + parent->world.pos.x;
        thisx->world.pos.y = (parent->world.pos.y + 5.0f) - 10.0f;
        thisx->world.pos.z = Math_CosS(parent->shape.rot.y) * 60.0f + parent->world.pos.z;
        this->unk_168 = thisx->world.pos.y;
        func_80A89860(this, play);
    }

    func_80A89A6C(this, play);

    if (this->unk_164) {
        this->unk_164 = false;
        u8* carpTex = ResourceMgr_LoadTexByName(sCarpetTex);
        u8* shadTex = sShadowTex;
        for (i = 0; i < ARRAY_COUNT(sShadowTex); i++) {
            if (((u16*)carpTex)[i] != 0) { // Hack to bypass ZAPD exporting textures as u64.
                shadTex[i] = 0xFF;
            } else {
                shadTex[i] = 0;
            }
        }
    }
    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, (s16)this->shadowAlpha);

    Matrix_Translate(thisx->world.pos.x, 3.0f, thisx->world.pos.z, MTXMODE_NEW);
    Matrix_Scale(thisx->scale.x, 1.0f, thisx->scale.z, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    // Draws the carpet's shadow texture.
    gSPSegment(POLY_OPA_DISP++, 0x0C, sShadowTex);
    gSPDisplayList(POLY_OPA_DISP++, sShadowMaterialDL);
    gDPPipeSync(POLY_OPA_DISP++);

    // Draws the carpet's shadow vertices. Swaps them between frames to get a smoother result.
    if (play->gameplayFrames % 2 != 0) {
        gSPSegment(POLY_OPA_DISP++, 0x0C, gShadowOddVtx);
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x0C, sShadowEvenVtx);
    }
    gSPDisplayList(POLY_OPA_DISP++, sModelDL);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Matrix_Translate(thisx->world.pos.x, this->unk_168 + 3.0f, thisx->world.pos.z, MTXMODE_NEW);
    Matrix_Scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    // Draws the carpet's texture.
    gSPDisplayList(POLY_OPA_DISP++, sCarpetMaterialDL);

    gDPPipeSync(POLY_OPA_DISP++);

    // Draws the carpet vertices.
    if (play->gameplayFrames % 2 != 0) {
        gSPSegment(POLY_OPA_DISP++, 0x0C, sCarpetOddVtx);
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x0C, sCarpetEvenVtx);
    }
    gSPDisplayList(POLY_OPA_DISP++, sModelDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
