/*
 * File: z_en_sda.c
 * Overlay: ovl_En_Sda
 * Description: Dynamic Shadow for Player
 */

#include "z_en_sda.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnSda_Init(Actor* thisx, PlayState* play);
void EnSda_Destroy(Actor* thisx, PlayState* play);
void EnSda_Update(Actor* thisx, PlayState* play);
void EnSda_Draw(Actor* thisx, PlayState* play);

void func_8094702C(EnSda* this, u8* shadowTexture, Player* player, PlayState* play);
void func_80947668(u8* shadowTexture, Player* player, PlayState* play);

Vec3f D_80947EA0[16];

ActorProfile En_Sda_Profile = {
    /**/ ACTOR_EN_SDA,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnSda),
    /**/ EnSda_Init,
    /**/ EnSda_Destroy,
    /**/ EnSda_Update,
    /**/ EnSda_Draw,
};

Vec3f D_80947A60 = { 0.0f, 0.0f, 0.0f };

s16 D_80947A6C[] = { 1, 2, 3, 3, 2, 1 };

s16 D_80947A78[] = { 2, 3, 4, 4, 4, 3, 2, 0 };

s16 D_80947A88[] = { 2, 3, 4, 4, 4, 4, 3, 2 };

s16 D_80947A98[] = { 2, 4, 5, 5, 6, 6, 6, 6, 5, 5, 4, 2 };

s16 D_80947AB0[] = { 2, 4, 5, 6, 7, 8, 8, 8, 8, 7, 6, 5, 4, 2 };

s16 D_80947ACC[] = { 1, -1, 1, 1, 3, 4, 1, 6, 7, 2, 9, 0xA, 2, 0xC, 0xD, 0 };

u8 D_80947AEC[] = {
    2, 2, 2, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3,
};

s8 D_80947AFC[PLAYER_BODYPART_MAX] = {
    2, 9, 10, 11, 12, 13, 14, 0, 15, -1, 3, 4, 5, 6, 7, 8, -1, 1,
};

Vec3f D_80947B10[] = {
    { -1.0f, 2.0f, -0.2f },  { 0.0f, 2.0f, -0.5f },   { 1.0f, 2.0f, -0.2f },   { -2.0f, 1.0f, -0.5f },
    { -1.0f, 1.0f, -0.2f },  { 0.0f, 1.0f, -0.2f },   { 1.0f, 1.0f, -0.2f },   { 2.0f, 1.0f, -0.5f },
    { -2.0f, 0.0f, -0.5f },  { -1.0f, 0.0f, -0.2f },  { 0.0f, 0.0f, 0.0f },    { 1.0f, 0.0f, -0.2f },
    { 2.0f, 0.0f, -0.5f },   { -2.0f, -1.0f, -0.5f }, { -1.0f, -1.0f, -0.2f }, { 0.0f, -1.0f, -0.1f },
    { 1.0f, -1.0f, -0.2f },  { 2.0f, -1.0f, -0.5f },  { -1.0f, -2.0f, -0.2f }, { 0.0f, -2.0f, -0.2f },
    { 1.0f, -2.0f, -0.2f },  { 0.0f, -3.0f, -0.5f },  { -1.0f, 2.0f, -0.2f },  { 0.0f, 2.0f, -0.5f },
    { 1.0f, 2.0f, -0.2f },   { -2.0f, 1.0f, -0.5f },  { -1.0f, 1.0f, -0.2f },  { 0.0f, 1.0f, -0.2f },
    { 1.0f, 1.0f, -0.2f },   { 2.0f, 1.0f, -0.5f },   { -2.0f, 0.0f, -0.5f },  { -1.0f, 0.0f, -0.2f },
    { 0.0f, 0.0f, 0.0f },    { 1.0f, 0.0f, -0.2f },   { 2.0f, 0.0f, -0.5f },   { -2.0f, -1.0f, -0.5f },
    { -1.0f, -1.0f, -0.2f }, { 0.0f, -1.0f, -0.1f },  { 1.0f, -1.0f, -0.2f },  { 2.0f, -1.0f, -0.5f },
    { -1.0f, -2.0f, -0.2f }, { 0.0f, -2.0f, -0.2f },  { 1.0f, -2.0f, -0.2f },  { 0.0f, -3.0f, -0.5f },
};

static s32 sPad = 0;

#include "overlays/ovl_En_Sda/ovl_En_Sda.h"

void EnSda_Init(Actor* thisx, PlayState* play) {
}

void EnSda_Destroy(Actor* thisx, PlayState* play) {
}

void EnSda_Update(Actor* thisx, PlayState* play) {
    EnSda* this = (EnSda*)thisx;
    Player* player;

    if (this->actor.params == ENSDA_1) {
        player = (Player*)this->actor.parent;
    } else {
        player = GET_PLAYER(play);
    }
    this->actor.world.pos = player->actor.world.pos;
}

void EnSda_Draw(Actor* thisx, PlayState* play) {
    EnSda* this = (EnSda*)thisx;
    Player* player;
    u8* shadowTex = GRAPH_ALLOC(play->state.gfxCtx, 64 * 64);

    if (this->actor.params == ENSDA_1) {
        player = (Player*)this->actor.parent;
    } else {
        player = GET_PLAYER(play);
    }

    player->actor.shape.shadowAlpha = 0;
    func_8094702C(this, shadowTex, player, play);

    if (KREG(0) < 5) {
        func_80947668(shadowTex, player, play);
    }
}

void func_809469C0(Player* player, u8* shadowTexture, f32 arg2) {
    s16 temp_t0;
    s16 temp_t1;
    s16 temp_v1;
    s16 temp_v0;
    s16 phi_a0;
    s16 phi_a3;
    s16 i;
    s16 j;
    Vec3f lerp;
    Vec3f sp88;
    Vec3f sp7C;

    for (i = 0; i < 16; i++) {
        // TODO: match with a continue
        if ((arg2 == 0.0f) || ((j = D_80947ACC[i]) >= 0)) {
            if (arg2 > 0.0f) {
                lerp.x = D_80947EA0[i].x + (D_80947EA0[j].x - D_80947EA0[i].x) * arg2;
                lerp.y = D_80947EA0[i].y + (D_80947EA0[j].y - D_80947EA0[i].y) * arg2;
                lerp.z = D_80947EA0[i].z + (D_80947EA0[j].z - D_80947EA0[i].z) * arg2;

                sp88.x = lerp.x - player->actor.world.pos.x;
                sp88.y = lerp.y - player->actor.world.pos.y + BREG(48) + 76.0f + 30.0f - 105.0f + 15.0f;
                sp88.z = lerp.z - player->actor.world.pos.z;
            } else {
                sp88.x = D_80947EA0[i].x - player->actor.world.pos.x;
                sp88.y = D_80947EA0[i].y - player->actor.world.pos.y + BREG(48) + 76.0f + 30.0f - 105.0f + 15.0f;
                sp88.z = D_80947EA0[i].z - player->actor.world.pos.z;
            }

            Matrix_MultVec3f(&sp88, &sp7C);
            sp7C.x *= (1.0f + (BREG(49) / 100.0f));
            sp7C.y *= (1.0f + (BREG(49) / 100.0f));
            temp_t0 = sp7C.x + 32.0f;
            temp_t1 = sp7C.y * 64.0f;

            if (D_80947AEC[i] == 2) {
                for (j = 0, phi_a3 = -384; j < 12; j++, phi_a3 += 64) {
                    for (phi_a0 = -D_80947A98[j]; phi_a0 < D_80947A98[j]; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 64)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            } else if (D_80947AEC[i] == 1) {
                for (j = 0, phi_a3 = -256; j < 8; j++, phi_a3 += 64) {
                    for (phi_a0 = -D_80947A88[j]; phi_a0 < D_80947A88[j]; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 64)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            } else if (D_80947AEC[i] == 0) {
                for (j = 0, phi_a3 = -192; j < 7; j++, phi_a3 += 64) {
                    for (phi_a0 = -D_80947A78[j]; phi_a0 < D_80947A78[j] - 1; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 64)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            } else if (D_80947AEC[i] == 4) {
                for (j = 0, phi_a3 = -448; j < 14; j++, phi_a3 += 64) {
                    for (phi_a0 = -D_80947AB0[j]; phi_a0 < D_80947AB0[j]; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 64)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            } else {
                for (j = 0, phi_a3 = -128; j < 6; j++, phi_a3 += 64) {
                    for (phi_a0 = -D_80947A6C[j]; phi_a0 < D_80947A6C[j] - 1; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 64)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            }
        }
    }
}

void func_8094702C(EnSda* this, u8* shadowTexture, Player* player, PlayState* play) {
    s16 temp_t0;
    s16 temp_t1;
    s16 temp_v0;
    s16 temp_v1;
    s16 phi_a0;
    s16 phi_a3;
    s16 i;
    s16 j;
    Vec3f sp194;
    Vec3f sp188;
    s32* shadowTextureTemp32;
    u8* shadowTextureTemp;
    Vec3s sp178;
    Vec3f sp16C;
    Vec3f sp64[22];

    if (BREG(57) != 0) {
        for (shadowTextureTemp = shadowTexture, i = 0; i < 0x1000; i++, shadowTextureTemp++) {
            if (((i >= 0) && (i < 0x40)) || ((i >= 0xFC0) && (i < 0x1000)) || ((i & 0x3F) == 0) ||
                ((i & 0x3F) == 0x3F)) {
                *shadowTextureTemp = 255;
            } else {
                *shadowTextureTemp = 0;
            }
        }
    } else {
        for (shadowTextureTemp32 = (s32*)shadowTexture, i = 0; i < 0x400; i++, shadowTextureTemp32++) {
            *shadowTextureTemp32 = 0;
        }
    }

    Matrix_RotateXFNew((BREG(50) + 70) / 100.0f);

    for (i = 0; i < PLAYER_BODYPART_MAX; i++) {
        if (D_80947AFC[i] >= 0) {
            D_80947EA0[D_80947AFC[i]] = player->bodyPartsPos[i];
        }
    }

    D_80947EA0[0].y += 3.0f;
    D_80947EA0[15].x = D_80947EA0[0].x + ((D_80947EA0[15].x - D_80947EA0[0].x) * 1.2f);
    D_80947EA0[15].y = D_80947EA0[0].y + ((D_80947EA0[15].y - D_80947EA0[0].y) * -1.2f);
    D_80947EA0[15].z = D_80947EA0[0].z + ((D_80947EA0[15].z - D_80947EA0[0].z) * 1.2f);

    for (i = 0; i < 6; i++) {
        func_809469C0(player, shadowTexture, i / 5.0f);
    }

    if (this->actor.params != ENSDA_1) {
        Matrix_MtxFToYXZRot(&player->shieldMf, &sp178, false);

        sp178.y += (KREG(87) * 0x8000) + 0x8000;
        sp178.x *= (KREG(88) - 1);

        Matrix_Mult(&player->shieldMf, MTXMODE_NEW);
        Matrix_MultVec3f(&D_80947A60, &sp16C);
        Matrix_RotateYS(sp178.y, MTXMODE_NEW);
        Matrix_RotateXS(sp178.x, MTXMODE_APPLY);

        for (i = 0; i < 22; i++) {
            Matrix_MultVec3f(&D_80947B10[i], &sp188);
            sp64[i].x = (((KREG(82) / 100.0f) + 4.0f) * sp188.x) + sp16C.x;
            sp64[i].y = (((KREG(82) / 100.0f) + 4.0f) * sp188.y) + sp16C.y;
            sp64[i].z = (((KREG(82) / 100.0f) + 4.0f) * sp188.z) + sp16C.z;
        }

        Matrix_RotateXFNew((BREG(50) + 70) / 100.0f);

        for (i = 0; i < 22; i++) {
            sp194.x = sp64[i].x - player->actor.world.pos.x;
            sp194.y = sp64[i].y - player->actor.world.pos.y + KREG(80) + 16.0f;
            sp194.z = sp64[i].z - player->actor.world.pos.z;

            Matrix_MultVec3f(&sp194, &sp188);

            sp188.x *= (1.0f + (KREG(90) / 100.0f));
            sp188.y *= (1.0f + (KREG(90) / 100.0f));
            temp_t0 = sp188.x + 32.0f;
            temp_t1 = sp188.y * 64.0f;

            do {
                for (j = 0, phi_a3 = -0xC0; j < 7; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -D_80947A78[j]; phi_a0 < D_80947A78[j] - 1; phi_a0++) {
                        temp_v0 = temp_t0 + phi_a0;
                        if ((temp_v0 >= 0) && (temp_v0 < 0x40)) {
                            temp_v1 = temp_t1 + phi_a3;
                            if ((temp_v1 >= 0) && (temp_v1 < 0x1000)) {
                                shadowTexture[temp_v0 + temp_v1] = 255;
                            }
                        }
                    }
                }
                j++;
            } while (j < 6);
        }
    }

    if (BREG(61) == 1) {
        for (shadowTextureTemp = shadowTexture, i = 0; i < 0x1000; i++, shadowTextureTemp++) {
            if (*shadowTextureTemp != 0) {
                *shadowTextureTemp = -((i >> 6) * (BREG(60) + 4)) + (255 - BREG(61));
            }
        }
    }
}

void func_80947668(u8* shadowTexture, Player* player, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 tempx;
    f32 tempz;
    s16 i;

    OPEN_DISPS(gfxCtx);

    gSPInvalidateTexCache(POLY_XLU_DISP++, shadowTexture);

    Gfx_SetupDL44_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, 0, 0, 0, (BREG(52) + 50));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 0);

    Matrix_Translate(player->actor.world.pos.x, player->actor.floorHeight, player->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYF(BREG(51) / 100.0f, MTXMODE_APPLY);
    Matrix_Scale(1.0f, 1.0f, (BREG(63) / 10.0f) + 1.0f, MTXMODE_APPLY);

    tempx = (BREG(62) / 10.0f) + 2.0f;
    tempz = ((player->actor.world.pos.y - player->actor.floorHeight + BREG(54)) * (BREG(55) - 5) / 10.0f) + BREG(58) -
            20.0f;

    Matrix_Translate(tempx, 0.0f, tempz, MTXMODE_APPLY);
    Matrix_Scale(((BREG(56) - 250) / 1000.0f) + 0.6f, 1.0f, ((BREG(59) - 250) / 1000.0f) + 0.6f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, ovl_En_Sda_DL_1498);
    gDPLoadTextureBlock(POLY_XLU_DISP++, shadowTexture, G_IM_FMT_I, G_IM_SIZ_8b, 64, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPDisplayList(POLY_XLU_DISP++, ovl_En_Sda_DL_14B8);

    for (i = 0; i < KREG(78); i++) {
        Matrix_Scale((KREG(79) / 100.0f) + 1.0f, 1.0f, (KREG(79) / 100.0f) + 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, ovl_En_Sda_DL_14B8);
    }

    CLOSE_DISPS(gfxCtx);
}
