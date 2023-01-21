/**
 * File: z_en_sda.c
 * Overlay: ovl_En_Sda
 * Description: Dynamic shadow for Link
 */

#include "z_en_sda.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnSda_Init(Actor* thisx, PlayState* play);
void EnSda_Destroy(Actor* thisx, PlayState* play);
void EnSda_Update(Actor* thisx, PlayState* play);
void EnSda_Draw(Actor* thisx, PlayState* play);

void func_80AF95C4(EnSda* this, u8* shadowTexture, Player* player, PlayState* play);
void func_80AF9C70(u8* shadowTexture, Player* player, PlayState* play);
void func_80AF8F60(Player* player, u8* shadowTexture, f32 arg2);

const ActorInit En_Sda_InitVars = {
    ACTOR_EN_SDA,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnSda),
    (ActorFunc)EnSda_Init,
    (ActorFunc)EnSda_Destroy,
    (ActorFunc)EnSda_Update,
    (ActorFunc)EnSda_Draw,
    NULL,
};

static Vec3f D_80AFA0D0 = { 0.0f, 0.0f, 0.0f };

static s16 D_80AFA0DC[] = {
    1, 2, 3, 3, 2, 1,
};

static s16 D_80AFA0E8[] = {
    2, 3, 4, 4, 4, 3, 2, 0,
};

static s16 D_80AFA0F8[] = {
    2, 3, 4, 4, 4, 4, 3, 2,
};

static s16 D_80AFA108[] = {
    2, 4, 5, 5, 6, 6, 6, 6, 5, 5, 4, 2,
};

static s16 D_80AFA120[] = {
    2, 4, 5, 6, 7, 8, 8, 8, 8, 7, 6, 5, 4, 2,
};

static s16 D_80AFA13C[] = {
    1, -1, 1, 1, 3, 4, 1, 6, 7, 2, 9, 10, 2, 12, 13, 0,
};

static u8 D_80AFA15C[] = {
    2, 2, 2, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3,
};

static s8 D_80AFA16C[] = {
    2, 9, 10, 11, 12, 13, 14, 0, 15, -1, 3, 4, 5, 6, 7, 8, -1, 1, 0, 0,
};

static Vec3f D_80AFA180[] = {
    { -1.0f, 2.0f, -0.2f }, { 0.0f, 2.0f, -0.5f },   { 1.0f, 2.0f, -0.2f },   { -2.0f, 1.0f, -0.5f },
    { -1.0f, 1.0f, -0.2f }, { 0.0f, 1.0f, -0.2f },   { 1.0f, 1.0f, -0.2f },   { 2.0f, 1.0f, -0.5f },
    { -2.0f, 0.0f, -0.5f }, { -1.0f, 0.0f, -0.2f },  { 0.0f, 0.0f, 0.0f },    { 1.0f, 0.0f, -0.2f },
    { 2.0f, 0.0f, -0.5f },  { -2.0f, -1.0f, -0.5f }, { -1.0f, -1.0f, -0.2f }, { 0.0f, -1.0f, -0.1f },
    { 1.0f, -1.0f, -0.2f }, { 2.0f, -1.0f, -0.5f },  { -1.0f, -2.0f, -0.2f }, { 0.0f, -2.0f, -0.2f },
    { 1.0f, -2.0f, -0.2f }, { 0.0f, -3.0f, -0.5f },
};

// Unused, identical to D_80AFA180
static Vec3f D_80AFA288[] = {
    { -1.0f, 2.0f, -0.2f }, { 0.0f, 2.0f, -0.5f },   { 1.0f, 2.0f, -0.2f },   { -2.0f, 1.0f, -0.5f },
    { -1.0f, 1.0f, -0.2f }, { 0.0f, 1.0f, -0.2f },   { 1.0f, 1.0f, -0.2f },   { 2.0f, 1.0f, -0.5f },
    { -2.0f, 0.0f, -0.5f }, { -1.0f, 0.0f, -0.2f },  { 0.0f, 0.0f, 0.0f },    { 1.0f, 0.0f, -0.2f },
    { 2.0f, 0.0f, -0.5f },  { -2.0f, -1.0f, -0.5f }, { -1.0f, -1.0f, -0.2f }, { 0.0f, -1.0f, -0.1f },
    { 1.0f, -1.0f, -0.2f }, { 2.0f, -1.0f, -0.5f },  { -1.0f, -2.0f, -0.2f }, { 0.0f, -2.0f, -0.2f },
    { 1.0f, -2.0f, -0.2f }, { 0.0f, -3.0f, -0.5f },
};

static u32 D_80AFA390[] = { 0, 0 };

#include "overlays/ovl_En_Sda/ovl_En_Sda.h"

static Vec3f D_80AFA660[16];

void EnSda_Init(Actor* thisx, PlayState* play) {
}

void EnSda_Destroy(Actor* thisx, PlayState* play) {
}

void EnSda_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSda* this = (EnSda*)thisx;
    Player* player;

    osSyncPrintf("SDA MOVE\n");

    if (this->actor.params == 1) {
        player = (Player*)this->actor.parent;
    } else {
        player = GET_PLAYER(play);
    }

    this->actor.world.pos = player->actor.world.pos;

    osSyncPrintf("SDA MOVE END\n");
}

void EnSda_Draw(Actor* thisx, PlayState* play) {
    EnSda* this = (EnSda*)thisx;
    Player* player;
    u8* shadowTexture = Graph_Alloc(play->state.gfxCtx, 0x1000);

    osSyncPrintf("SDA DRAW \n");

    if (this->actor.params == 1) {
        player = (Player*)this->actor.parent;
    } else {
        player = GET_PLAYER(play);
    }

    player->actor.shape.shadowAlpha = 0;
    func_80AF95C4(this, shadowTexture, player, play);

    if (KREG(0) < 5) {
        func_80AF9C70(shadowTexture, player, play);
    }

    osSyncPrintf("SDA DRAW END\n");
}

void func_80AF8F60(Player* player, u8* shadowTexture, f32 arg2) {
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
        if ((arg2 == 0.0f) || ((j = D_80AFA13C[i]) >= 0)) {
            if (arg2 > 0.0f) {
                lerp.x = D_80AFA660[i].x + (D_80AFA660[j].x - D_80AFA660[i].x) * arg2;
                lerp.y = D_80AFA660[i].y + (D_80AFA660[j].y - D_80AFA660[i].y) * arg2;
                lerp.z = D_80AFA660[i].z + (D_80AFA660[j].z - D_80AFA660[i].z) * arg2;

                sp88.x = lerp.x - player->actor.world.pos.x;
                sp88.y = lerp.y - player->actor.world.pos.y + BREG(48) + 76.0f + 30.0f - 105.0f + 15.0f;
                sp88.z = lerp.z - player->actor.world.pos.z;
            } else {
                sp88.x = D_80AFA660[i].x - player->actor.world.pos.x;
                sp88.y = D_80AFA660[i].y - player->actor.world.pos.y + BREG(48) + 76.0f + 30.0f - 105.0f + 15.0f;
                sp88.z = D_80AFA660[i].z - player->actor.world.pos.z;
            }
            Matrix_MultVec3f(&sp88, &sp7C);
            sp7C.x *= (1.0f + (BREG(49) / 100.0f));
            sp7C.y *= (1.0f + (BREG(49) / 100.0f));
            temp_t0 = sp7C.x + 32.0f;
            temp_t1 = (s16)sp7C.y << 6;

            if (D_80AFA15C[i] == 2) {
                for (j = 0, phi_a3 = -0x180; j < 12; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -D_80AFA108[j]; phi_a0 < D_80AFA108[j]; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 0x40)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            } else if (D_80AFA15C[i] == 1) {
                for (j = 0, phi_a3 = -0x100; j < 8; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -D_80AFA0F8[j]; phi_a0 < D_80AFA0F8[j]; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 0x40)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            } else if (D_80AFA15C[i] == 0) {
                for (j = 0, phi_a3 = -0xC0; j < 7; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -D_80AFA0E8[j]; phi_a0 < D_80AFA0E8[j] - 1; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 0x40)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            } else if (D_80AFA15C[i] == 4) {
                for (j = 0, phi_a3 = -0x1C0; j < 14; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -D_80AFA120[j]; phi_a0 < D_80AFA120[j]; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 0x40)) {
                            temp_v0 = temp_t1 + phi_a3;
                            if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                                shadowTexture[temp_v1 + temp_v0] = 255;
                            }
                        }
                    }
                }
            } else {
                for (j = 0, phi_a3 = -0x80; j < 6; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -D_80AFA0DC[j]; phi_a0 < D_80AFA0DC[j] - 1; phi_a0++) {
                        temp_v1 = temp_t0 + phi_a0;
                        if ((temp_v1 >= 0) && (temp_v1 < 0x40)) {
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

void func_80AF95C4(EnSda* this, u8* shadowTexture, Player* player, PlayState* play) {
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

    osSyncPrintf("SDA CONT \n");
    if (BREG(57) != 0) {
        for (shadowTextureTemp = shadowTexture, i = 0; i < 0x1000; i++, shadowTextureTemp++) {
            if ((i >= 0 && i < 0x40) || (i >= 0xFC0 && i < 0x1000) || ((i & 0x3F) == 0) || ((i & 0x3F) == 0x3F)) {
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
    Matrix_RotateX((BREG(50) + 70) / 100.0f, MTXMODE_NEW);
    for (i = 0; i < 18; i++) {
        if (D_80AFA16C[i] >= 0) {
            D_80AFA660[D_80AFA16C[i]] = player->bodyPartsPos[i];
        }
    }
    osSyncPrintf("SDA CONT 2\n");
    D_80AFA660[0].y += 3.0f;
    D_80AFA660[15].x = D_80AFA660[0].x + ((D_80AFA660[15].x - D_80AFA660[0].x) * 1.2f);
    D_80AFA660[15].y = D_80AFA660[0].y + ((D_80AFA660[15].y - D_80AFA660[0].y) * -1.2f);
    D_80AFA660[15].z = D_80AFA660[0].z + ((D_80AFA660[15].z - D_80AFA660[0].z) * 1.2f);
    for (i = 0; i < 6; i++) {
        func_80AF8F60(player, shadowTexture, i / 5.0f);
    }
    osSyncPrintf("SDA CONT 3\n");
    if (this->actor.params != 1) {
        Matrix_MtxFToYXZRotS(&player->shieldMf, &sp178, false);
        sp178.y += (KREG(87) << 0xF) + 0x8000;
        sp178.x *= (KREG(88) - 1);
        Matrix_Mult(&player->shieldMf, MTXMODE_NEW);
        Matrix_MultVec3f(&D_80AFA0D0, &sp16C);
        Matrix_RotateY((sp178.y / 32768.0f) * M_PI, MTXMODE_NEW);
        Matrix_RotateX((sp178.x / 32768.0f) * M_PI, MTXMODE_APPLY);
        for (i = 0; i < 22; i++) {
            Matrix_MultVec3f(&D_80AFA180[i], &sp188);
            sp64[i].x = (((KREG(82) / 100.0f) + 4.0f) * sp188.x) + sp16C.x;
            sp64[i].y = (((KREG(82) / 100.0f) + 4.0f) * sp188.y) + sp16C.y;
            sp64[i].z = (((KREG(82) / 100.0f) + 4.0f) * sp188.z) + sp16C.z;
        }
        Matrix_RotateX((BREG(50) + 70) / 100.0f, MTXMODE_NEW);
        for (i = 0; i < 22; i++) {
            sp194.x = sp64[i].x - player->actor.world.pos.x;
            sp194.y = sp64[i].y - player->actor.world.pos.y + KREG(80) + 16.0f;
            sp194.z = sp64[i].z - player->actor.world.pos.z;
            Matrix_MultVec3f(&sp194, &sp188);
            sp188.x *= (1.0f + (KREG(90) / 100.0f));
            sp188.y *= (1.0f + (KREG(90) / 100.0f));
            temp_t0 = sp188.x + 32.0f;
            temp_t1 = (s16)sp188.y << 6;

            do {
                for (j = 0, phi_a3 = -0xC0; j < 7; j++, phi_a3 += 0x40) {
                    for (phi_a0 = -D_80AFA0E8[j]; phi_a0 < D_80AFA0E8[j] - 1; phi_a0++) {
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
    osSyncPrintf("SDA CONT 4\n");
}

void func_80AF9C70(u8* shadowTexture, Player* player, PlayState* play) {
    s32 pad;
    f32 tempx;
    f32 tempz;
    s16 phi_s1;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    osSyncPrintf("SDA D 1\n");
    Gfx_SetupDL_44Xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, 0, 0, 0, (BREG(52) + 50));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 0);
    Matrix_Translate(player->actor.world.pos.x, player->actor.floorHeight, player->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateY(BREG(51) / 100.0f, MTXMODE_APPLY);
    Matrix_Scale(1.0f, 1.0f, (BREG(63) / 10.0f) + 1.0f, MTXMODE_APPLY);
    tempx = (BREG(62) / 10.0f) + 2.0f;
    tempz = ((player->actor.world.pos.y - player->actor.floorHeight + BREG(54)) * (BREG(55) - 5) / 10.0f) + BREG(58) -
            20.0f;
    Matrix_Translate(tempx, 0.0f, tempz, MTXMODE_APPLY);
    Matrix_Scale(((BREG(56) - 250) / 1000.0f) + 0.6f, 1.0f, ((BREG(59) - 250) / 1000.0f) + 0.6f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, D_80AFA3D8);
    gDPLoadTextureBlock(POLY_XLU_DISP++, shadowTexture, G_IM_FMT_I, G_IM_SIZ_8b, 0x40, 0x40, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPInvalidateTexCache(POLY_XLU_DISP++, (uintptr_t)shadowTexture); // Don't remember this texture, it will only be used this frame.
    gSPDisplayList(POLY_XLU_DISP++, D_80AFA3F8);

    for (phi_s1 = 0; phi_s1 < KREG(78); phi_s1++) {
        Matrix_Scale((KREG(79) / 100.0f) + 1.0f, 1.0f, (KREG(79) / 100.0f) + 1.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, D_80AFA3F8);
    }
    osSyncPrintf("SDA D 2\n");
    CLOSE_DISPS(gfxCtx);
}
