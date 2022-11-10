#include "global.h"
#include "vt.h"

#include <stdlib.h>

#include "z64environment.h"
#include "assets/textures/backgrounds/vr_ALVR_static.h"
#include "assets/textures/backgrounds/vr_ALVR_pal_static.h"
#include "assets/textures/backgrounds/vr_DGVR_static.h"
#include "assets/textures/backgrounds/vr_DGVR_pal_static.h"
#include "assets/textures/backgrounds/vr_FCVR_static.h"
#include "assets/textures/backgrounds/vr_FCVR_pal_static.h"
#include "assets/textures/backgrounds/vr_GLVR_static.h"
#include "assets/textures/backgrounds/vr_GLVR_pal_static.h"
#include "assets/textures/backgrounds/vr_IPVR_static.h"
#include "assets/textures/backgrounds/vr_IPVR_pal_static.h"
#include "assets/textures/backgrounds/vr_K3VR_static.h"
#include "assets/textures/backgrounds/vr_K3VR_pal_static.h"
#include "assets/textures/backgrounds/vr_K4VR_static.h"
#include "assets/textures/backgrounds/vr_K4VR_pal_static.h"
#include "assets/textures/backgrounds/vr_K5VR_static.h"
#include "assets/textures/backgrounds/vr_K5VR_pal_static.h"
#include "assets/textures/backgrounds/vr_KHVR_static.h"
#include "assets/textures/backgrounds/vr_KHVR_pal_static.h"
#include "assets/textures/backgrounds/vr_KKRVR_static.h"
#include "assets/textures/backgrounds/vr_KKRVR_pal_static.h"
#include "assets/textures/backgrounds/vr_KR3VR_static.h"
#include "assets/textures/backgrounds/vr_KR3VR_pal_static.h"
#include "assets/textures/backgrounds/vr_KSVR_static.h"
#include "assets/textures/backgrounds/vr_KSVR_pal_static.h"
#include "assets/textures/backgrounds/vr_LBVR_static.h"
#include "assets/textures/backgrounds/vr_LBVR_pal_static.h"
#include "assets/textures/backgrounds/vr_LHVR_static.h"
#include "assets/textures/backgrounds/vr_LHVR_pal_static.h"
#include "assets/textures/backgrounds/vr_MDVR_static.h"
#include "assets/textures/backgrounds/vr_MDVR_pal_static.h"
#include "assets/textures/backgrounds/vr_MLVR_static.h"
#include "assets/textures/backgrounds/vr_MLVR_pal_static.h"
#include "assets/textures/backgrounds/vr_MNVR_static.h"
#include "assets/textures/backgrounds/vr_MNVR_pal_static.h"
#include "assets/textures/backgrounds/vr_NSVR_static.h"
#include "assets/textures/backgrounds/vr_NSVR_pal_static.h"
#include "assets/textures/backgrounds/vr_RUVR_static.h"
#include "assets/textures/backgrounds/vr_RUVR_pal_static.h"
#include "assets/textures/backgrounds/vr_SP1a_static.h"
#include "assets/textures/backgrounds/vr_SP1a_pal_static.h"
#include "assets/textures/backgrounds/vr_TTVR_static.h"
#include "assets/textures/backgrounds/vr_TTVR_pal_static.h"
#include "assets/textures/backgrounds/vr_ZRVR_static.h"
#include "assets/textures/backgrounds/vr_ZRVR_pal_static.h"

#include "assets/textures/skyboxes/vr_cloud0_static.h"
#include "assets/textures/skyboxes/vr_cloud0_pal_static.h"
#include "assets/textures/skyboxes/vr_cloud1_static.h"
#include "assets/textures/skyboxes/vr_cloud1_pal_static.h"
#include "assets/textures/skyboxes/vr_cloud2_static.h"
#include "assets/textures/skyboxes/vr_cloud2_pal_static.h"
#include "assets/textures/skyboxes/vr_cloud3_static.h"
#include "assets/textures/skyboxes/vr_cloud3_pal_static.h"
#include "assets/textures/skyboxes/vr_fine0_static.h"
#include "assets/textures/skyboxes/vr_fine0_pal_static.h"
#include "assets/textures/skyboxes/vr_fine1_static.h"
#include "assets/textures/skyboxes/vr_fine1_pal_static.h"
#include "assets/textures/skyboxes/vr_fine2_static.h"
#include "assets/textures/skyboxes/vr_fine2_pal_static.h"
#include "assets/textures/skyboxes/vr_fine3_static.h"
#include "assets/textures/skyboxes/vr_fine3_pal_static.h"
#include "assets/textures/skyboxes/vr_holy0_static.h"
#include "assets/textures/skyboxes/vr_holy0_pal_static.h"
#include "assets/textures/skyboxes/vr_holy1_static.h"
#include "assets/textures/skyboxes/vr_holy1_pal_static.h"


u32 D_8012AC90[4] = {
    0x00000000,
    0x00010000,
    0x00020000,
    0x00030000,
};

u16 D_8012ACA0[2][0x20] = {
    { 0x00, 0x02, 0x0A, 0x0C, 0x02, 0x04, 0x0C, 0x0E, 0x0A, 0x0C, 0x14, 0x16, 0x0C, 0x0E, 0x16, 0x18,
      0x01, 0x03, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0B, 0x0D, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x15, 0x17 },
    { 0x14, 0x16, 0x1E, 0x20, 0x16, 0x18, 0x20, 0x22, 0x1E, 0x20, 0x28, 0x2A, 0x20, 0x22, 0x2A, 0x2C,
      0x15, 0x17, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1F, 0x21, 0x23, 0x24, 0x25, 0x26, 0x27, 0x29, 0x2B },
};

s16 D_8012AD20[5] = {
    0x0000, 0x0FC0, 0x1F80, 0x2F40, 0x3F00,
};

s16 D_8012AD2C[9] = {
    0x0000, 0x07C0, 0x0F80, 0x1740, 0x1F00, 0x26C0, 0x2E80, 0x3640, 0x3E00,
};

s16 D_8012AD40[0x40] = {
    0x00, 0x10, 0x13, 0x12, 0x10, 0x01, 0x14, 0x13, 0x01, 0x11, 0x15, 0x14, 0x11, 0x05, 0x16, 0x15,
    0x12, 0x13, 0x17, 0x02, 0x13, 0x14, 0x03, 0x17, 0x14, 0x15, 0x18, 0x03, 0x15, 0x16, 0x07, 0x18,
    0x02, 0x17, 0x1A, 0x19, 0x17, 0x03, 0x1B, 0x1A, 0x03, 0x18, 0x1C, 0x1B, 0x18, 0x07, 0x1D, 0x1C,
    0x19, 0x1A, 0x1E, 0x0A, 0x1A, 0x1B, 0x0B, 0x1E, 0x1B, 0x1C, 0x1F, 0x0B, 0x1C, 0x1D, 0x0F, 0x1F,
};

u32 D_8012ADC0[6] = {
    0x00000000, 0x00002000, 0x00004000, 0x00006000, 0x00008000, 0x0000C000,
};

u16 D_8012ADD8[0x20] = {
    0x00, 0x02, 0x0A, 0x0C, 0x02, 0x04, 0x0C, 0x0E, 0x0A, 0x0C, 0x14, 0x16, 0x0C, 0x0E, 0x16, 0x18,
    0x01, 0x03, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0B, 0x0D, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x15, 0x17,
};

s16 D_8012AE18[5] = {
    0x0000, 0x07C0, 0x0F80, 0x1740, 0x1F00,
};

s16 D_8012AE24[5] = {
    0x0000, 0x07C0, 0x0F80, 0x1740, 0x1F00,
};

s16 D_8012AE30[5] = {
    0x0000, 0x07C0, 0x0F80, 0x07C0, 0x0000,
};

s16 D_8012AE3C[0x40] = {
    0x00, 0x10, 0x13, 0x12, 0x10, 0x01, 0x14, 0x13, 0x01, 0x11, 0x15, 0x14, 0x11, 0x05, 0x16, 0x15,
    0x12, 0x13, 0x17, 0x02, 0x13, 0x14, 0x03, 0x17, 0x14, 0x15, 0x18, 0x03, 0x15, 0x16, 0x07, 0x18,
    0x02, 0x17, 0x1A, 0x19, 0x17, 0x03, 0x1B, 0x1A, 0x03, 0x18, 0x1C, 0x1B, 0x18, 0x07, 0x1D, 0x1C,
    0x19, 0x1A, 0x1E, 0x0A, 0x1A, 0x1B, 0x0B, 0x1E, 0x1B, 0x1C, 0x1F, 0x0B, 0x1C, 0x1D, 0x0F, 0x1F,
};

typedef struct {
    /* 0x000 */ s32 unk_0;
    /* 0x004 */ s32 unk_4;
    /* 0x008 */ s32 unk_8;
    /* 0x00C */ s32 unk_C;
    /* 0x010 */ s32 unk_10;
} Struct_8012AF0C; // size = 0x14

Struct_8012AF0C D_8012AEBC[4] = {
    { -0x7E, 0x7C, -0x7E, 0x3F, -0x1F },
    { 0x7E, 0x7C, -0x7E, 0x3F, -0x1F },
    { 0x7E, 0x7C, 0x7E, -0x3F, -0x1F },
    { -0x7E, 0x7C, 0x7E, -0x3F, -0x1F },
};

Struct_8012AF0C D_8012AF0C[6] = {
    { -0x40, 0x40, -0x40, 0x20, -0x20 }, { 0x40, 0x40, 0x40, -0x20, -0x20 }, { -0x40, 0x40, 0x40, -0x20, -0x20 },
    { 0x40, 0x40, -0x40, 0x20, -0x20 },  { -0x40, 0x40, 0x40, 0x20, -0x20 }, { -0x40, -0x40, -0x40, 0x20, 0x20 },
};

s32 func_800ADBB0(SkyboxContext* skyboxCtx, Vtx* roomVtx, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7,
                  s32 arg8, s32 arg9) {
    u32 pad42C;
    s32 pad428;
    s32 sp424;
    s32 i;
    s32 j;
    s32 k;
    u16 index;
    s16 phi_t1;
    s16 phi_a2_4;
    s16 phi_a0_4;
    s16 phi_t2_4;
    s16 phi_ra;
    s32 sp358[9 * 5];
    s32 sp2A4[9 * 5];
    s32 sp1F0[9 * 5];
    s32 sp13C[9 * 5];
    s32 sp88[9 * 5];
    s32 pad;

    switch (arg8) {
        case 0:
        case 2:
            pad428 = arg4;

            for (i = 0, k = 0; k < 45; i++) {
                pad42C = arg3;
                for (j = 0; j < 5; j++, k++) {
                    sp1F0[k] = arg5;
                    sp358[k] = pad42C;
                    sp2A4[k] = pad428;
                    sp13C[k] = D_8012AD20[j];
                    sp88[k] = D_8012AD2C[i];
                    pad42C += arg6;
                }
                pad428 += arg7;
            }
            break;

        case 1:
        case 3:
            pad428 = arg4;

            for (i = 0, k = 0; k < 45; i++) {
                pad42C = arg5;
                for (j = 0; j < 5; j++, k++) {
                    sp358[k] = arg3;
                    sp2A4[k] = pad428;
                    sp1F0[k] = pad42C;
                    sp13C[k] = D_8012AD20[j];
                    sp88[k] = D_8012AD2C[i];
                    pad42C += arg6;
                }
                pad428 += arg7;
            }
            break;

        case 4:
        case 5:
            pad428 = arg5;

            for (i = 0, k = 0; k < 45; i++) {
                pad42C = arg3;
                for (j = 0; j < 5; j++, k++) {
                    sp2A4[k] = arg4;
                    sp358[k] = pad42C;
                    sp1F0[k] = pad428;
                    sp13C[k] = D_8012AD20[j];
                    sp88[k] = D_8012AD2C[i];
                    pad42C += arg6;
                }
                pad428 += arg7;
            }
            break;
    }

    for (phi_a2_4 = 0, sp424 = 0; sp424 < 2; sp424++) {
        skyboxCtx->unk_138 = skyboxCtx->dListBuf[arg9 + sp424];

        for (i = 0; i < 0x20; i++) {
            index = D_8012ACA0[sp424][i];

            roomVtx[arg2 + i].v.ob[0] = sp358[index];
            roomVtx[arg2 + i].v.ob[1] = sp2A4[index];
            roomVtx[arg2 + i].v.ob[2] = sp1F0[index];
            roomVtx[arg2 + i].v.flag = 0;
            roomVtx[arg2 + i].v.tc[0] = sp13C[index];
            roomVtx[arg2 + i].v.tc[1] = sp88[index];
            roomVtx[arg2 + i].v.cn[1] = 0;
            roomVtx[arg2 + i].v.cn[2] = 0;
            roomVtx[arg2 + i].v.cn[0] = 255;
        }
        gSPVertex(skyboxCtx->unk_138++, &roomVtx[arg2], 32, 0);
        arg2 += i;
        gSPCullDisplayList(skyboxCtx->unk_138++, 0, 15);


        for (phi_t2_4 = 0, phi_ra = 0; phi_ra < 4; phi_ra++, phi_a2_4 += 0x1F) {
            for (phi_a0_4 = 0, phi_t1 = 0; phi_t1 < 4; phi_t1++, phi_a0_4 += 0x3F, phi_t2_4 += 4) {
                gDPLoadTextureTile(skyboxCtx->unk_138++, skyboxCtx->textures[0][arg8],
                                   G_IM_FMT_CI, G_IM_SIZ_8b, 256, 0, phi_a0_4, phi_a2_4, phi_a0_4 + 0x3F,
                                   phi_a2_4 + 0x1F, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);

                gSP1Quadrangle(skyboxCtx->unk_138++, D_8012AD40[phi_t2_4 + 1], D_8012AD40[phi_t2_4 + 2],
                               D_8012AD40[phi_t2_4 + 3], D_8012AD40[phi_t2_4 + 0], 3);
            }
        }
        gSPEndDisplayList(skyboxCtx->unk_138++);
    }
    return arg2;
}

s32 func_800AE2C0(SkyboxContext* skyboxCtx, Vtx* roomVtx, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7,
                  s32 arg8) {
    s32 i;
    s32 j;
    s32 k;
    s16 phi_a0_4;
    s16 phi_t1;
    s32 temp1;
    u16 index;
    s16 phi_a2_4;
    s16 phi_ra;
    s16 phi_t2_4;
    s32 temp2;
    s32 sp2B4[5 * 5];
    s32 sp250[5 * 5];
    s32 sp1EC[5 * 5];
    s32 sp188[5 * 5];
    s32 sp124[5 * 5];
    s32 pad;

    switch (arg8) {
        case 0:
        case 1:
            temp1 = arg4;

            for (i = 0, k = 0; k < 25; i++) {
                temp2 = arg3;
                for (j = 0; j < 5; j++, k++) {
                    sp1EC[k] = arg5;
                    sp2B4[k] = temp2;
                    sp250[k] = temp1;
                    sp188[k] = D_8012AE18[j];
                    sp124[k] = D_8012AE30[i];
                    temp2 += arg6;
                }
                temp1 += arg7;
            }
            break;

        case 2:
        case 3:
            temp1 = arg4;

            for (i = 0, k = 0; k < 25; i++) {
                temp2 = arg5;
                for (j = 0; j < 5; j++, k++) {
                    sp2B4[k] = arg3;
                    sp250[k] = temp1;
                    sp1EC[k] = temp2;
                    sp188[k] = D_8012AE18[j];
                    sp124[k] = D_8012AE30[i];
                    temp2 += arg6;
                }
                temp1 += arg7;
            }
            break;

        case 4:
        case 5:
            temp1 = arg5;

            for (i = 0, k = 0; k < 25; i++) {
                temp2 = arg3;
                for (j = 0; j < 5; j++, k++) {
                    sp250[k] = arg4;
                    sp2B4[k] = temp2;
                    sp1EC[k] = temp1;
                    sp188[k] = D_8012AE18[j];
                    sp124[k] = D_8012AE24[i];
                    temp2 += arg6;
                }
                temp1 += arg7;
            }
            break;
    }
    skyboxCtx->unk_138 = &skyboxCtx->dListBuf[2 * arg8];

    for (i = 0; i < 0x20; i++) {
        index = D_8012ADD8[i];

        roomVtx[arg2 + i].v.ob[0] = sp2B4[index];
        roomVtx[arg2 + i].v.ob[1] = sp250[index];
        roomVtx[arg2 + i].v.ob[2] = sp1EC[index];
        roomVtx[arg2 + i].v.flag = 0;
        roomVtx[arg2 + i].v.tc[0] = sp188[index];
        roomVtx[arg2 + i].v.tc[1] = sp124[index];
        roomVtx[arg2 + i].v.cn[1] = 0;
        roomVtx[arg2 + i].v.cn[2] = 0;
        roomVtx[arg2 + i].v.cn[0] = 255;
    }
    gSPVertex(skyboxCtx->unk_138++, &roomVtx[arg2], 32, 0);
    arg2 += i;
    gSPCullDisplayList(skyboxCtx->unk_138++, 0, 15);

    if ((arg8 == 4) || (arg8 == 5)) {
        phi_a2_4 = 0;
        for (phi_t2_4 = 0, phi_ra = 0; phi_ra < 4; phi_ra++, phi_a2_4 += 0x1F) {
            for (phi_a0_4 = 0, phi_t1 = 0; phi_t1 < 4; phi_t1++, phi_a0_4 += 0x1F, phi_t2_4 += 4) {
                gDPLoadMultiTile(skyboxCtx->unk_138++, skyboxCtx->textures[0][arg8], 0,
                                 G_TX_RENDERTILE, G_IM_FMT_CI, G_IM_SIZ_8b, 128, 0, phi_a0_4, phi_a2_4, phi_a0_4 + 0x1F,
                                 phi_a2_4 + 0x1F, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD,
                                 G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
                gDPLoadMultiTile(skyboxCtx->unk_138++, skyboxCtx->textures[1][arg8], 0x80, 1,
                                 G_IM_FMT_CI, G_IM_SIZ_8b, 128, 0, phi_a0_4, phi_a2_4, phi_a0_4 + 0x1F, phi_a2_4 + 0x1F,
                                 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP,
                                 G_TX_NOMASK, G_TX_NOLOD);

                gDPNoOpString(skyboxCtx->unk_138++, "TEST2", 0);

                gSP1Quadrangle(skyboxCtx->unk_138++, D_8012AE3C[phi_t2_4 + 1], D_8012AE3C[phi_t2_4 + 2],
                               D_8012AE3C[phi_t2_4 + 3], D_8012AE3C[phi_t2_4 + 0], 3);
            }
        }
    } else {
        phi_a2_4 = 0;
        for (phi_t2_4 = 0, phi_ra = 0; phi_ra < 2; phi_ra++, phi_a2_4 += 0x1F) {
            for (phi_a0_4 = 0, phi_t1 = 0; phi_t1 < 4; phi_t1++, phi_a0_4 += 0x1F, phi_t2_4 += 4) {
                gDPLoadMultiTile(skyboxCtx->unk_138++, skyboxCtx->textures[0][arg8], 0,
                                 G_TX_RENDERTILE, G_IM_FMT_CI, G_IM_SIZ_8b, 128, 0, phi_a0_4, phi_a2_4, phi_a0_4 + 0x1F,
                                 phi_a2_4 + 0x1F, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD,
                                 G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
                gDPLoadMultiTile(skyboxCtx->unk_138++, skyboxCtx->textures[1][arg8], 0x80, 1,
                                 G_IM_FMT_CI, G_IM_SIZ_8b, 128, 0, phi_a0_4, phi_a2_4, phi_a0_4 + 0x1F, phi_a2_4 + 0x1F,
                                 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP,
                                 G_TX_NOMASK, G_TX_NOLOD);
                gSP1Quadrangle(skyboxCtx->unk_138++, D_8012AE3C[phi_t2_4 + 1], D_8012AE3C[phi_t2_4 + 2],
                               D_8012AE3C[phi_t2_4 + 3], D_8012AE3C[phi_t2_4 + 0], 3);
            }
        }
        phi_a2_4 -= 0x1F;
        for (phi_ra = 0; phi_ra < 2; phi_ra++, phi_a2_4 -= 0x1F) {
            for (phi_a0_4 = 0, phi_t1 = 0; phi_t1 < 4; phi_t1++, phi_a0_4 += 0x1F, phi_t2_4 += 4) {
                gDPLoadMultiTile(skyboxCtx->unk_138++, skyboxCtx->textures[0][arg8], 0,
                                 G_TX_RENDERTILE, G_IM_FMT_CI, G_IM_SIZ_8b, 128, 0, phi_a0_4, phi_a2_4, phi_a0_4 + 0x1F,
                                 phi_a2_4 + 0x1F, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD,
                                 G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
                gDPLoadMultiTile(skyboxCtx->unk_138++, skyboxCtx->textures[1][arg8], 0x80, 1,
                                 G_IM_FMT_CI, G_IM_SIZ_8b, 128, 0, phi_a0_4, phi_a2_4, phi_a0_4 + 0x1F, phi_a2_4 + 0x1F,
                                 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP,
                                 G_TX_NOMASK, G_TX_NOLOD);
                gSP1Quadrangle(skyboxCtx->unk_138++, D_8012AE3C[phi_t2_4 + 1], D_8012AE3C[phi_t2_4 + 2],
                               D_8012AE3C[phi_t2_4 + 3], D_8012AE3C[phi_t2_4 + 0], 3);
            }
        }
    }
    gSPEndDisplayList(skyboxCtx->unk_138++);
    return arg2;
}

void func_800AEFC8(SkyboxContext* skyboxCtx, s16 skyboxId) {
    s32 i;
    s32 j;
    s32 phi_s3 = 0;

    if (skyboxId == SKYBOX_BAZAAR || (skyboxId > SKYBOX_HOUSE_KAKARIKO && skyboxId <= SKYBOX_BOMBCHU_SHOP)) {
        for (j = 0, i = 0; i < 2; i++, j += 2) {
            phi_s3 = func_800ADBB0(skyboxCtx, skyboxCtx->roomVtx, phi_s3, D_8012AEBC[i].unk_0, D_8012AEBC[i].unk_4,
                                   D_8012AEBC[i].unk_8, D_8012AEBC[i].unk_C, D_8012AEBC[i].unk_10, i, j);
        }
    } else if (skyboxCtx->unk_140 == 2) {
        for (j = 0, i = 0; i < 3; i++, j += 2) {
            phi_s3 = func_800ADBB0(skyboxCtx, skyboxCtx->roomVtx, phi_s3, D_8012AEBC[i].unk_0, D_8012AEBC[i].unk_4,
                                   D_8012AEBC[i].unk_8, D_8012AEBC[i].unk_C, D_8012AEBC[i].unk_10, i, j);
        }
    } else {
        for (j = 0, i = 0; i < 4; i++, j += 2) {
            phi_s3 = func_800ADBB0(skyboxCtx, skyboxCtx->roomVtx, phi_s3, D_8012AEBC[i].unk_0, D_8012AEBC[i].unk_4,
                                   D_8012AEBC[i].unk_8, D_8012AEBC[i].unk_C, D_8012AEBC[i].unk_10, i, j);
        }
    }
}

void func_800AF178(SkyboxContext* skyboxCtx, s32 arg1) {
    s32 phi_s2 = 0;
    s32 i;

    for (i = 0; i < arg1; i++) {
        phi_s2 = func_800AE2C0(skyboxCtx, skyboxCtx->roomVtx, phi_s2, D_8012AF0C[i].unk_0, D_8012AF0C[i].unk_4,
                               D_8012AF0C[i].unk_8, D_8012AF0C[i].unk_C, D_8012AF0C[i].unk_10, i);
    }
}

void LoadSkyboxTex(SkyboxContext* skyboxCtx, int segmentIndex, int imageIndex, char* tex, int width, int height, int offsetW, int offsetH)
{
    skyboxCtx->textures[segmentIndex][imageIndex] = tex;
}

void LoadSkyboxTexAtOffset(SkyboxContext* skyboxCtx, int segmentIndex, int imageIndex, char* tex, int width, int height, int offset)
{
    skyboxCtx->textures[segmentIndex][imageIndex] = tex;
}

void LoadSkyboxPalette(SkyboxContext* skyboxCtx, int paletteIndex, char* palTex, int width,
                       int height) {
    skyboxCtx->palettes[paletteIndex] = palTex;
    skyboxCtx->palette_size = width * height;
}

static const char* sSBVRFine0Tex[] =
{
    gSunriseSkybox1Tex, gSunriseSkybox2Tex, gSunriseSkybox3Tex, gSunriseSkybox4Tex, gSunriseSkybox5Tex
};

static const char* sSBVRFine0Pal[] =
{
    gSunriseSkyboxTLUT
};

static const char* sSBVRFine1Tex[] =
{
    gDaySkybox1Tex, gDaySkybox2Tex, gDaySkybox3Tex, gDaySkybox4Tex, gDaySkybox5Tex
};

static const char* sSBVRFine1Pal[] =
{
    gDaySkyboxTLUT
};

static const char* sSBVRFine2Tex[] =
{
    gSunsetSkybox1Tex, gSunsetSkybox2Tex, gSunsetSkybox3Tex, gSunsetSkybox4Tex, gSunsetSkybox5Tex
};

static const char* sSBVRFine2Pal[] =
{
    gSunsetSkyboxTLUT
};

static const char* sSBVRFine3Tex[] =
{
    gNightSkybox1Tex, gNightSkybox2Tex, gNightSkybox3Tex, gNightSkybox4Tex, gNightSkybox5Tex
};

static const char* sSBVRFine3Pal[] =
{
    gNightSkyboxTLUT
};

static const char* sSBVRCloud0Tex[] =
{
    gSunriseOvercastSkybox1Tex, gSunriseOvercastSkybox2Tex, gSunriseOvercastSkybox3Tex, gSunriseOvercastSkybox4Tex, gSunriseOvercastSkybox5Tex
};

static const char* sSBVRCloud0Pal[] =
{
    gSunriseOvercastSkyboxTLUT
};

static const char* sSBVRCloud1Tex[] =
{
    gDayOvercastSkybox1Tex, gDayOvercastSkybox2Tex, gDayOvercastSkybox3Tex, gDayOvercastSkybox4Tex, gDayOvercastSkybox5Tex
};

static const char* sSBVRCloud1Pal[] =
{
    gDayOvercastSkyboxTLUT
};

static const char* sSBVRCloud2Tex[] =
{
    gSunsetOvercastSkybox1Tex, gSunsetOvercastSkybox2Tex, gSunsetOvercastSkybox3Tex, gSunsetOvercastSkybox4Tex, gSunsetOvercastSkybox5Tex
};

static const char* sSBVRCloud2Pal[] =
{
    gSunsetOvercastSkyboxTLUT
};

static const char* sSBVRCloud3Tex[] =
{
    gNightOvercastSkybox1Tex, gNightOvercastSkybox2Tex, gNightOvercastSkybox3Tex, gNightOvercastSkybox4Tex, gNightOvercastSkybox5Tex
};

static const char* sSBVRCloud3Pal[] =
{
    gNightOvercastSkyboxTLUT
};

static const char* sSBVRHoly0Tex[] =
{
    gHoly0Skybox1Tex, gHoly0Skybox2Tex, gHoly0Skybox3Tex, gHoly0Skybox4Tex, gHoly0Skybox5Tex
};

static const char* sSBVRHoly0Pal[] =
{
    gHoly0SkyboxTLUT
};


SkyboxTableEntry sSkyboxTable[] = 
{
    {
        sSBVRFine0Tex,
        sSBVRFine0Pal
        
    },
    {
        sSBVRFine1Tex,
        sSBVRFine1Pal
    },
    {
        sSBVRFine2Tex,
        sSBVRFine2Pal
    },
    {
        sSBVRFine3Tex,
        sSBVRFine3Pal
    },
    {
        sSBVRCloud0Tex,
        sSBVRCloud0Pal,
    },
    {
       sSBVRCloud1Tex,
       sSBVRCloud1Pal,
    },
    {
        sSBVRCloud2Tex,
       sSBVRCloud2Pal,
    },
    {
        sSBVRCloud3Tex,
        sSBVRCloud3Pal,
    },
    {
        sSBVRHoly0Tex,
        sSBVRHoly0Pal,
    },
};

void Skybox_Setup(PlayState* play, SkyboxContext* skyboxCtx, s16 skyboxId) {
    size_t size;
    s16 i;
    u8 sp41; // imageIdx
    u8 sp40; // imageIdx2
    uintptr_t start;
    s32 phi_v1;

    switch (skyboxId)
    {
    case SKYBOX_NORMAL_SKY:
        phi_v1 = 0;
        if (gSaveContext.unk_13C3 != 0 && gSaveContext.sceneSetupIndex < 4 && gWeatherMode > 0 &&
            gWeatherMode < 6) {
            phi_v1 = 1;
        }

        for (i = 0; i < 9; i++) {
            if (gSaveContext.skyboxTime >= D_8011FC1C[phi_v1][i].startTime &&
                (gSaveContext.skyboxTime < D_8011FC1C[phi_v1][i].endTime ||
                    D_8011FC1C[phi_v1][i].endTime == 0xFFFF)) {
                play->envCtx.skybox1Index = sp41 = D_8011FC1C[phi_v1][i].skybox1Index;
                play->envCtx.skybox2Index = sp40 = D_8011FC1C[phi_v1][i].skybox2Index;
                if (D_8011FC1C[phi_v1][i].blend != 0) {
                    play->envCtx.skyboxBlend =
                        Environment_LerpWeight(D_8011FC1C[phi_v1][i].endTime, D_8011FC1C[phi_v1][i].startTime,
                            ((void)0, gSaveContext.skyboxTime)) *
                        255.0f;
                }
                else {
                    play->envCtx.skyboxBlend = 0;
                }
                break;
            }
        }


        SkyboxTableEntry entryA = sSkyboxTable[sp41];

      for (int i = 0; i < 5; i++)
            LoadSkyboxTex(skyboxCtx, 0, i, entryA.textures[i], 128, i == 4 ? 128 : 64, 128, 64);

        SkyboxTableEntry entryB = sSkyboxTable[sp40];

        for (int i = 0; i < 5; i++)
            LoadSkyboxTex(skyboxCtx, 1, i, entryB.textures[i], 128, i == 4 ? 128 : 64, 128, 64);
        
        if ((sp41 & 1) ^ ((sp41 & 4) >> 2)) 
        {
            LoadSkyboxPalette(skyboxCtx, 0, entryA.palettes[0], 16, 8);
            LoadSkyboxPalette(skyboxCtx, 1, entryB.palettes[0], 16, 8);
        }
        else 
        {
            LoadSkyboxPalette(skyboxCtx, 0, entryB.palettes[0], 16, 8);
            LoadSkyboxPalette(skyboxCtx, 1, entryA.palettes[0], 16, 8);
        }
        break;
    case SKYBOX_BAZAAR:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gBazaarBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gBazaar2BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gBazaarBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gBazaarBg2TLUT, 16, 16);
        skyboxCtx->rot.y = 0.8f;
        break;
    case SKYBOX_HOUSE_LINK:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gLinksHouseBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gLinksHouse2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gLinksHouse3BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 3, gLinksHouse4BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gLinksHouseBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gLinksHouseBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gLinksHouseBg3TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 3, gLinksHouseBg4TLUT, 16, 16);
        break;
    case SKYBOX_OVERCAST_SUNSET:
        LoadSkyboxTexAtOffset(skyboxCtx, 0, 0, gSunsetOvercastSkybox1Tex, 128, 64, 0x0);
        LoadSkyboxTexAtOffset(skyboxCtx, 0, 1, gSunsetOvercastSkybox2Tex, 128, 64, 0x2000);
        LoadSkyboxTexAtOffset(skyboxCtx, 0, 2, gSunsetOvercastSkybox3Tex, 128, 64, 0x4000);
        LoadSkyboxTexAtOffset(skyboxCtx, 0, 3, gSunsetOvercastSkybox4Tex, 128, 64, 0x6000);
        LoadSkyboxTexAtOffset(skyboxCtx, 0, 4, gSunsetOvercastSkybox5Tex, 128, 128, 0x8000);

        LoadSkyboxPalette(skyboxCtx, 0, gSunsetOvercastSkyboxTLUT, 16, 8);
        break;
    case SKYBOX_MARKET_ADULT:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gMarketRuinsBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gMarketRuins2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gMarketRuins3BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 3, gMarketRuins4BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gMarketRuinsBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gMarketRuinsBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gMarketRuinsBg3TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 3, gMarketRuinsBg4TLUT, 16, 16);
        break;
    case SKYBOX_CUTSCENE_MAP:
        LoadSkyboxTexAtOffset(skyboxCtx, 0, 0, gHoly0Skybox1Tex, 128, 64, 0x0);
        LoadSkyboxTexAtOffset(skyboxCtx, 0, 1, gHoly0Skybox2Tex, 128, 64, 0x2000);
        LoadSkyboxTexAtOffset(skyboxCtx, 0, 2, gHoly0Skybox3Tex, 128, 64, 0x4000);
        LoadSkyboxTexAtOffset(skyboxCtx, 0, 3, gHoly0Skybox4Tex, 128, 64, 0x6000);
        LoadSkyboxTexAtOffset(skyboxCtx, 0, 4, gHoly0Skybox5Tex, 128, 128, 0x8000);
        LoadSkyboxTexAtOffset(skyboxCtx, 0, 5, gHoly0Skybox6Tex, 128, 128, 0xC000);

        LoadSkyboxPalette(skyboxCtx, 0, gHoly0SkyboxTLUT, 16, 8);

        LoadSkyboxTexAtOffset(skyboxCtx, 1, 0, gHoly1Skybox1Tex, 128, 64, 0x0);
        LoadSkyboxTexAtOffset(skyboxCtx, 1, 1, gHoly1Skybox2Tex, 128, 64, 0x2000);
        LoadSkyboxTexAtOffset(skyboxCtx, 1, 2, gHoly1Skybox3Tex, 128, 64, 0x4000);
        LoadSkyboxTexAtOffset(skyboxCtx, 1, 3, gHoly1Skybox4Tex, 128, 64, 0x6000);
        LoadSkyboxTexAtOffset(skyboxCtx, 1, 4, gHoly1Skybox5Tex, 128, 128, 0x8000);
        LoadSkyboxTexAtOffset(skyboxCtx, 1, 5, gHoly1Skybox6Tex, 128, 128, 0xC000);


        LoadSkyboxPalette(skyboxCtx, 1, gHoly1SkyboxTLUT, 16, 8);
        break;
    case SKYBOX_MARKET_CHILD_DAY:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gMarketDayBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gMarketDay2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gMarketDay3BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 3, gMarketDay4BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gMarketDayBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gMarketDayBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gMarketDayBg3TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 3, gMarketDayBg4TLUT, 16, 16);
        break;
    case SKYBOX_MARKET_CHILD_NIGHT:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gMarketNightBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gMarketNight2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gMarketNight3BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 3, gMarketNight4BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gMarketNightBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gMarketNightBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gMarketNightBg3TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 3, gMarketNightBg4TLUT, 16, 16);
        break;
    case SKYBOX_HAPPY_MASK_SHOP:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gMaskShopBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gMaskShop2BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gMaskShopBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gMaskShopBg2TLUT, 16, 16);

        skyboxCtx->rot.y = 0.8f;
        break;
    case SKYBOX_HOUSE_KNOW_IT_ALL_BROTHERS:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gKnowItAllBrosHouseBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gKnowItAllBrosHouse2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gKnowItAllBrosHouse3BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 3, gKnowItAllBrosHouse4BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gKnowItAllBrosHouseBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gKnowItAllBrosHouseBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gKnowItAllBrosHouseBg3TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 3, gKnowItAllBrosHouseBg4TLUT, 16, 16);
        break;
    case SKYBOX_HOUSE_OF_TWINS:
        skyboxCtx->unk_140 = 2;

        LoadSkyboxTex(skyboxCtx, 0, 0, gHouseOfTwinsBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gHouseOfTwins2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gHouseOfTwins3BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gHouseOfTwinsBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gHouseOfTwinsBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gHouseOfTwinsBg3TLUT, 16, 16);
        break;
    case SKYBOX_STABLES:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gStableBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gStable2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gStable3BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 3, gStable4BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gStableBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gStableBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gStableBg3TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 3, gStableBg4TLUT, 16, 16);
        break;
    case SKYBOX_HOUSE_KAKARIKO:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gCarpentersHouseBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gCarpentersHouse2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gCarpentersHouse3BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 3, gCarpentersHouse4BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gCarpentersHouseBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gCarpentersHouseBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gCarpentersHouseBg3TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 3, gCarpentersHouseBg4TLUT, 16, 16);
        break;
    case SKYBOX_KOKIRI_SHOP:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gKokiriShopBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gKokiriShop2BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gKokiriShopBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gKokiriShopBg2TLUT, 16, 16);

        skyboxCtx->rot.y = 0.8f;
        break;
    case SKYBOX_GORON_SHOP:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gGoronShopBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gGoronShop2BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gGoronShopBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gGoronShopBg2TLUT, 16, 16);

        skyboxCtx->rot.y = 0.8f;
        break;
    case SKYBOX_ZORA_SHOP:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gZoraShopBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gZoraShop2BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gZoraShopBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gZoraShopBg2TLUT, 16, 16);

        skyboxCtx->rot.y = 0.8f;
        break;
    case SKYBOX_POTION_SHOP_KAKARIKO:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gKakPotionShopBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gKakPotionShop2BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gKakPotionShopBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gKakPotionShopBg2TLUT, 16, 16);

        skyboxCtx->rot.y = 0.8f;
        break;
    case SKYBOX_POTION_SHOP_MARKET:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gMarketPotionShopBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gMarketPotionShop2BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gMarketPotionShopBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gMarketPotionShopBg2TLUT, 16, 16);

        skyboxCtx->rot.y = 0.8f;
        break;
    case SKYBOX_BOMBCHU_SHOP:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gBombchuShopBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gBombchuShop2BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gBombchuShopBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gBombchuShopBg2TLUT, 16, 16);

        skyboxCtx->rot.y = 0.8f;
        break;
    case SKYBOX_HOUSE_RICHARD:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gRichardsHouseBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gRichardsHouse2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gRichardsHouse3BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 3, gRichardsHouse4BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gRichardsHouseBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gRichardsHouseBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gRichardsHouseBg3TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 3, gRichardsHouseBg4TLUT, 16, 16);
        break;
    case SKYBOX_HOUSE_IMPA:
        skyboxCtx->unk_140 = 1;

        LoadSkyboxTex(skyboxCtx, 0, 0, gCowHouseBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gCowHouse2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gCowHouse3BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 3, gCowHouse4BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gCowHouseBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gCowHouseBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gCowHouseBg3TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 3, gCowHouseBg4TLUT, 16, 16);
        break;
    case SKYBOX_TENT:
        skyboxCtx->unk_140 = 2;

        LoadSkyboxTex(skyboxCtx, 0, 0, gCarpentersTentBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gCarpentersTent2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gCarpentersTent3BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gCarpentersTentBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gCarpentersTentBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gCarpentersTentBg3TLUT, 16, 16);
        break;
    case SKYBOX_HOUSE_MIDO:
        skyboxCtx->unk_140 = 2;

        LoadSkyboxTex(skyboxCtx, 0, 0, gMidosHouseBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gMidosHouse2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gMidosHouse3BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gMidosHouseBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gMidosHouseBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gMidosHouseBg3TLUT, 16, 16);
        break;
    case SKYBOX_HOUSE_SARIA:
        skyboxCtx->unk_140 = 2;

        LoadSkyboxTex(skyboxCtx, 0, 0, gSariasHouseBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gSariasHouse2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gSariasHouse3BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gSariasHouseBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gSariasHouseBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gSariasHouseBg3TLUT, 16, 16);
        break;
    case SKYBOX_HOUSE_ALLEY:
        skyboxCtx->unk_140 = 2;

        LoadSkyboxTex(skyboxCtx, 0, 0, gBackAlleyHouseBgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 1, gBackAlleyHouse2BgTex, 256, 256, 256, 256);
        LoadSkyboxTex(skyboxCtx, 0, 2, gBackAlleyHouse3BgTex, 256, 256, 256, 256);

        LoadSkyboxPalette(skyboxCtx, 0, gBackAlleyHouseBgTLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 1, gBackAlleyHouseBg2TLUT, 16, 16);
        LoadSkyboxPalette(skyboxCtx, 2, gBackAlleyHouseBg3TLUT, 16, 16);
        break;
    default:
        break;
    }
}

void Skybox_Init(GameState* state, SkyboxContext* skyboxCtx, s16 skyboxId) {
    PlayState* play = (PlayState*)state;

    skyboxCtx->skyboxId = skyboxId;
    skyboxCtx->unk_140 = 0;
    skyboxCtx->rot.x = skyboxCtx->rot.y = skyboxCtx->rot.z = 0.0f;

    Skybox_Setup(play, skyboxCtx, skyboxId);
    osSyncPrintf("\n\n\n＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊\n\n\n"
                 "ＴＹＰＥ＝%d"
                 "\n\n\n＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊\n\n\n",
                 skyboxId);

    if (skyboxId != SKYBOX_NONE) {
        osSyncPrintf(VT_FGCOL(GREEN));

        if (skyboxCtx->unk_140 != 0) {
            skyboxCtx->dListBuf = GAMESTATE_ALLOC_MC(state, 8 * 150 * sizeof(Gfx));
            ASSERT(skyboxCtx->dListBuf != NULL);

            skyboxCtx->roomVtx = GAMESTATE_ALLOC_MC(state, 256 * sizeof(Vtx));
            ASSERT(skyboxCtx->roomVtx != NULL);

            func_800AEFC8(skyboxCtx, skyboxId);
        } else {
            skyboxCtx->dListBuf = GAMESTATE_ALLOC_MC(state, 12 * 150 * sizeof(Gfx));
            ASSERT(skyboxCtx->dListBuf != NULL);

            if (skyboxId == SKYBOX_CUTSCENE_MAP) {
                skyboxCtx->roomVtx = GAMESTATE_ALLOC_MC(state, 192 * sizeof(Vtx));
                ASSERT(skyboxCtx->roomVtx != NULL);

                func_800AF178(skyboxCtx, 6);
            } else {
                skyboxCtx->roomVtx = GAMESTATE_ALLOC_MC(state, 160 * sizeof(Vtx));
                ASSERT(skyboxCtx->roomVtx != NULL);

                func_800AF178(skyboxCtx, 5);
            }
        }
        osSyncPrintf(VT_RST);
    }
}

void Skybox_Update(SkyboxContext* skyboxCtx) {
    if (skyboxCtx->skyboxId != SKYBOX_NONE) {
        osSyncPrintf(VT_FGCOL(GREEN));

        if (skyboxCtx->unk_140 != 0) {
            func_800AEFC8(skyboxCtx, skyboxCtx->skyboxId);
        } else {
            if (skyboxCtx->skyboxId == SKYBOX_CUTSCENE_MAP) {
                func_800AF178(skyboxCtx, 6);
            } else {
                func_800AF178(skyboxCtx, 5);
            }
        }
        osSyncPrintf(VT_RST);
    }
}
