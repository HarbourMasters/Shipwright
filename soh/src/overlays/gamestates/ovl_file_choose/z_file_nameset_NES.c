#include "file_choose.h"
#include "textures/title_static/title_static.h"
#include "assets/overlays/ovl_File_Choose/ovl_file_choose.h"
#include "assets/soh_assets.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/SaveManager.h"
#include "soh/ResourceManagerHelpers.h"

// File based on matching decomp for z_file_nameset_NES on N64 1.0 https://decomp.me/scratch/1tru6
// Edited to follow port's stage of documentation and implemented enhancements found in z_file_nameset_PAL.c

// Language Enums were changed between NTSC and PAL, so using (gSaveContext.language == LANGUAGE_ENG)
// as a substitute for indexing arrays with just gSaveContext.language

#define NTSC_LANGUAGE_INDEX (gSaveContext.language == LANGUAGE_ENG)

// DATA

static void* sNameLabelTexturesNES[] = {
    gFileSelNameJPNTex,
    gFileSelNameENGTex,
};

static void* sBackspaceEndTexturesNES[] = {
    gFileSelHiraganaButtonTex,  gFileSelKatakanaButtonTex, gFileSelKanjiButtonTex,
    gFileSelBackspaceButtonTex, gFileSelENDButtonENGTex,
};

static s16 sBackspaceEndWidthsNES[] = { 44, 44, 28, 28, 44 };

typedef struct {
    void* texture[2];
    u16 width;
    u16 height;
} OptionsMenuTextureInfoNES; // size = 0x8

static OptionsMenuTextureInfoNES sOptionsMenuHeaders[] = {
    { { gFileSelOptionsJPNTex, gFileSelOptionsENGTex }, 128, 16 },
    { { gFileSelSOUNDENGTex, gFileSelSOUNDENGTex }, 64, 16 },
    { { gFileSelLTargetingJPNTex, gFileSelLTargetingENGTex }, 64, 16 },
    { { gFileSelCheckBrightnessJPNTex, gFileSelCheckBrightnessENGNTSCTex }, 96, 16 },
};

static OptionsMenuTextureInfoNES sOptionsMenuSettings[] = {
    { { gFileSelStereoJPNTex, gFileSelStereoENGTex }, 48, 16 },
    { { gFileSelMonoJPNTex, gFileSelMonoENGTex }, 48, 16 },
    { { gFileSelHeadsetJPNTex, gFileSelHeadsetENGTex }, 48, 16 },
    { { gFileSelSurroundJPNTex, gFileSelSurroundENGTex }, 48, 16 },
    { { gFileSelSwitchJPNTex, gFileSelSwitchENGTex }, 48, 16 },
    { { gFileSelHoldJPNTex, gFileSelHoldENGTex }, 48, 16 },
};

// CODE

void FileChoose_DrawCharacterNES(GraphicsContext* gfxCtx, void* texture, s16 vtx) {
    OPEN_DISPS(gfxCtx);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, texture, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    CLOSE_DISPS(gfxCtx);
}

void FileChoose_DrawMultiTexQuadI4NES(GraphicsContext* gfxCtx, void* texture1, void* texture2, s16 vtx) {
    OPEN_DISPS(gfxCtx);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, texture1, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gDPLoadMultiBlock_4b(POLY_OPA_DISP++, texture2, 0x0080, 1, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                         G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    CLOSE_DISPS(gfxCtx);
}

void FileChoose_SetKeyboardVtxNES(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 val;
    s16 phi_t2;
    s16 phi_t0;
    s16 phi_t3;
    s16 phi_s1;
    s16 phi_t1;
    s16 phi_s2;

    this->keyboardVtx = Graph_Alloc(this->state.gfxCtx, sizeof(Vtx) * 4 * 5 * 13);
    phi_s1 = 0x26;

    for (phi_s2 = 0, phi_t3 = 0; phi_s2 < 5; phi_s2++) {
        for (phi_t0 = -0x60, phi_t1 = 0; phi_t1 < 13; phi_t1++, phi_t3 += 4) {
            this->keyboardVtx[phi_t3 + 2].v.ob[0] = phi_t0;

            this->keyboardVtx[phi_t3].v.ob[0] = this->keyboardVtx[phi_t3 + 2].v.ob[0];

            this->keyboardVtx[phi_t3 + 1].v.ob[0] = this->keyboardVtx[phi_t3 + 3].v.ob[0] = phi_t0 + 0xC;

            this->keyboardVtx[phi_t3 + 1].v.ob[1] = phi_s1;

            this->keyboardVtx[phi_t3].v.ob[1] = this->keyboardVtx[phi_t3 + 1].v.ob[1];

            this->keyboardVtx[phi_t3 + 2].v.ob[1] = this->keyboardVtx[phi_t3 + 3].v.ob[1] = phi_s1 - 0xC;

            this->keyboardVtx[phi_t3].v.ob[2] = this->keyboardVtx[phi_t3 + 1].v.ob[2] =
                this->keyboardVtx[phi_t3 + 2].v.ob[2] = this->keyboardVtx[phi_t3 + 3].v.ob[2] = 0;

            this->keyboardVtx[phi_t3].v.flag = this->keyboardVtx[phi_t3 + 1].v.flag =
                this->keyboardVtx[phi_t3 + 2].v.flag = this->keyboardVtx[phi_t3 + 3].v.flag = 0;

            this->keyboardVtx[phi_t3].v.tc[0] = this->keyboardVtx[phi_t3].v.tc[1] =
                this->keyboardVtx[phi_t3 + 1].v.tc[1] = this->keyboardVtx[phi_t3 + 2].v.tc[0] = 0;

            this->keyboardVtx[phi_t3 + 1].v.tc[0] = this->keyboardVtx[phi_t3 + 2].v.tc[1] =
                this->keyboardVtx[phi_t3 + 3].v.tc[0] = this->keyboardVtx[phi_t3 + 3].v.tc[1] = 16 << 5;

            this->keyboardVtx[phi_t3].v.cn[0] = this->keyboardVtx[phi_t3 + 1].v.cn[0] =
                this->keyboardVtx[phi_t3 + 2].v.cn[0] = this->keyboardVtx[phi_t3 + 3].v.cn[0] =
                    this->keyboardVtx[phi_t3].v.cn[1] = this->keyboardVtx[phi_t3 + 1].v.cn[1] =
                        this->keyboardVtx[phi_t3 + 2].v.cn[1] = this->keyboardVtx[phi_t3 + 3].v.cn[1] =
                            this->keyboardVtx[phi_t3].v.cn[2] = this->keyboardVtx[phi_t3 + 1].v.cn[2] =
                                this->keyboardVtx[phi_t3 + 2].v.cn[2] = this->keyboardVtx[phi_t3 + 3].v.cn[2] =
                                    this->keyboardVtx[phi_t3].v.cn[3] = this->keyboardVtx[phi_t3 + 1].v.cn[3] =
                                        this->keyboardVtx[phi_t3 + 2].v.cn[3] = this->keyboardVtx[phi_t3 + 3].v.cn[3] =
                                            255;
            phi_t0 += 0x10;
        }
        phi_s1 -= 0x10;
    }
}

void FileChoose_SetNameEntryVtxNES(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    Font* font = &this->font;
    s16 var_s0;
    s16 var_t2;
    s16 var_v0;
    u8* filename = Save_GetSaveMetaInfo(this->buttonIndex)->playerName;

    OPEN_DISPS(this->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    gSPVertex(POLY_OPA_DISP++, D_80811BB0_NTSC, 24, 0);

    gDPLoadTextureBlock(POLY_OPA_DISP++, sNameLabelTexturesNES[NTSC_LANGUAGE_INDEX], G_IM_FMT_IA, G_IM_SIZ_8b, 56, 16,
                        0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    gDPPipeSync(POLY_OPA_DISP++);

    for (var_t2 = 0, var_s0 = 4; var_t2 < 5; var_t2++, var_s0 += 4) {
        if (gSaveContext.language == LANGUAGE_JPN) {
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            255);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

            gDPLoadTextureBlock(POLY_OPA_DISP++, sBackspaceEndTexturesNES[var_t2], G_IM_FMT_IA, G_IM_SIZ_16b,
                                sBackspaceEndWidthsNES[var_t2], 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(POLY_OPA_DISP++, var_s0, var_s0 + 2, var_s0 + 3, var_s0 + 1, 0);
        } else if (var_t2 >= 3) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            255);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

            gDPLoadTextureBlock(POLY_OPA_DISP++, sBackspaceEndTexturesNES[var_t2], G_IM_FMT_IA, G_IM_SIZ_16b,
                                sBackspaceEndWidthsNES[var_t2], 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(POLY_OPA_DISP++, var_s0, var_s0 + 2, var_s0 + 3, var_s0 + 1, 0);
        }
    }

    this->nameEntryVtx = Graph_Alloc(this->state.gfxCtx, 44 * sizeof(Vtx));

    for (var_s0 = 0, var_t2 = 0; var_t2 < 44; var_t2 += 4, var_s0++) {
        this->nameEntryVtx[var_t2].v.ob[0] = this->nameEntryVtx[var_t2 + 2].v.ob[0] =
            D_808125EC[var_s0] + this->nameEntryBoxPosX;

        this->nameEntryVtx[var_t2 + 1].v.ob[0] = this->nameEntryVtx[var_t2 + 3].v.ob[0] =
            this->nameEntryVtx[var_t2].v.ob[0] + 0xA;

        this->nameEntryVtx[var_t2].v.ob[1] = this->nameEntryVtx[var_t2 + 1].v.ob[1] = D_80812604[var_s0];

        this->nameEntryVtx[var_t2 + 2].v.ob[1] = this->nameEntryVtx[var_t2 + 3].v.ob[1] =
            this->nameEntryVtx[var_t2].v.ob[1] - 0xA;

        this->nameEntryVtx[var_t2].v.ob[2] = this->nameEntryVtx[var_t2 + 1].v.ob[2] =
            this->nameEntryVtx[var_t2 + 2].v.ob[2] = this->nameEntryVtx[var_t2 + 3].v.ob[2] = 0;

        this->nameEntryVtx[var_t2].v.flag = this->nameEntryVtx[var_t2 + 1].v.flag =
            this->nameEntryVtx[var_t2 + 2].v.flag = this->nameEntryVtx[var_t2 + 3].v.flag = 0;

        this->nameEntryVtx[var_t2].v.tc[0] = this->nameEntryVtx[var_t2].v.tc[1] =
            this->nameEntryVtx[var_t2 + 1].v.tc[1] = this->nameEntryVtx[var_t2 + 2].v.tc[0] = 0;

        this->nameEntryVtx[var_t2 + 1].v.tc[0] = this->nameEntryVtx[var_t2 + 2].v.tc[1] =
            this->nameEntryVtx[var_t2 + 3].v.tc[0] = this->nameEntryVtx[var_t2 + 3].v.tc[1] = 0x200;

        this->nameEntryVtx[var_t2].v.cn[0] = this->nameEntryVtx[var_t2 + 1].v.cn[0] =
            this->nameEntryVtx[var_t2 + 2].v.cn[0] = this->nameEntryVtx[var_t2 + 3].v.cn[0] =
                this->nameEntryVtx[var_t2].v.cn[1] = this->nameEntryVtx[var_t2 + 1].v.cn[1] =
                    this->nameEntryVtx[var_t2 + 2].v.cn[1] = this->nameEntryVtx[var_t2 + 3].v.cn[1] =
                        this->nameEntryVtx[var_t2].v.cn[2] = this->nameEntryVtx[var_t2 + 1].v.cn[2] =
                            this->nameEntryVtx[var_t2 + 2].v.cn[2] = this->nameEntryVtx[var_t2 + 3].v.cn[2] =
                                this->nameEntryVtx[var_t2].v.cn[3] = this->nameEntryVtx[var_t2 + 1].v.cn[3] =
                                    this->nameEntryVtx[var_t2 + 2].v.cn[3] = this->nameEntryVtx[var_t2 + 3].v.cn[3] =
                                        0xFF;
    }

    this->nameEntryVtx[1].v.ob[0] = this->nameEntryVtx[3].v.ob[0] = this->nameEntryVtx[0].v.ob[0] + 0x6C;
    this->nameEntryVtx[2].v.ob[1] = this->nameEntryVtx[3].v.ob[1] = this->nameEntryVtx[0].v.ob[1] - 0x10;
    this->nameEntryVtx[1].v.tc[0] = this->nameEntryVtx[3].v.tc[0] = 108 << 5;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                    this->nameEntryBoxAlpha);
    gSPVertex(POLY_OPA_DISP++, this->nameEntryVtx, 4, 0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelNameBoxTex, G_IM_FMT_IA, G_IM_SIZ_16b, 108, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);
    gSPVertex(POLY_OPA_DISP++, this->nameEntryVtx + 4, 32, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->nameEntryBoxAlpha);

    for (var_s0 = 0, var_v0 = 0; var_s0 < 0x20; var_s0 += 4, var_v0++) {
        FileChoose_DrawCharacterNES(this->state.gfxCtx, font->fontBuf + filename[var_v0] * FONT_CHAR_TEX_SIZE, var_s0);
    }

    this->nameEntryVtx[37].v.tc[0] = this->nameEntryVtx[38].v.tc[1] = this->nameEntryVtx[39].v.tc[0] =
        this->nameEntryVtx[39].v.tc[1] = this->nameEntryVtx[41].v.tc[0] = this->nameEntryVtx[42].v.tc[1] =
            this->nameEntryVtx[43].v.tc[0] = this->nameEntryVtx[43].v.tc[1] = 24 << 5;

    if ((this->kbdButton == FS_KBD_BTN_HIRA) || (this->kbdButton == FS_KBD_BTN_KATA) ||
        (this->kbdButton == FS_KBD_BTN_END)) {
        this->nameEntryVtx[41].v.tc[0] = this->nameEntryVtx[43].v.tc[0] = 56 << 5;
    } else if ((this->kbdButton == FS_KBD_BTN_ENG) || (this->kbdButton == FS_KBD_BTN_BACKSPACE)) {
        this->nameEntryVtx[41].v.tc[0] = this->nameEntryVtx[43].v.tc[0] = 40 << 5;
    }

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileChoose_DrawKeyboardNES(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    Font* font = &this->font;
    s16 i;
    s16 vtx;
    s16 var_s1;

    vtx = 0;
    i = 0;

    OPEN_DISPS(this->state.gfxCtx);

    Gfx_SetupDL_42Opa(this->state.gfxCtx);

    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_PASS, G_RM_XLU_SURF2);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, 0, 0, 0, COMBINED, 0,
                      0, 0, COMBINED);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, this->charBgAlpha, 255, 255, 255, 255);

    if ((this->charPage == 0) || (this->charPage == 3) || (this->charPage == 5)) {
        if (this->charPage != 5) {
            for (; vtx < 0x100; vtx += 0x20) {
                gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

                for (var_s1 = 0; var_s1 < 0x20; i++, var_s1 += 4) {
                    FileChoose_DrawMultiTexQuadI4NES(
                        this->state.gfxCtx, font->fontBuf + gKeyboardCharactersHiragana[i] * FONT_CHAR_TEX_SIZE,
                        font->fontBuf + gKeyboardCharactersKatakana[i] * FONT_CHAR_TEX_SIZE, var_s1);
                }
            }
            gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
            FileChoose_DrawMultiTexQuadI4NES(this->state.gfxCtx,
                                             font->fontBuf + gKeyboardCharactersHiragana[i] * FONT_CHAR_TEX_SIZE,
                                             font->fontBuf + gKeyboardCharactersKatakana[i] * FONT_CHAR_TEX_SIZE, 0);
        } else {
            for (; vtx < 0x100; vtx += 0x20) {
                gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

                for (var_s1 = 0; var_s1 < 0x20; i++, var_s1 += 4) {
                    FileChoose_DrawMultiTexQuadI4NES(
                        this->state.gfxCtx, font->fontBuf + gKeyboardCharactersHiragana[i] * FONT_CHAR_TEX_SIZE,
                        font->fontBuf + gKeyboardCharactersAlphanumeric[i] * FONT_CHAR_TEX_SIZE, var_s1);
                }
            }
            gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
            FileChoose_DrawMultiTexQuadI4NES(
                this->state.gfxCtx, font->fontBuf + gKeyboardCharactersHiragana[i] * FONT_CHAR_TEX_SIZE,
                font->fontBuf + gKeyboardCharactersAlphanumeric[i] * FONT_CHAR_TEX_SIZE, 0);
        }
    } else if ((this->charPage == 1) || (this->charPage == 4) || (this->charPage == 7)) {
        if (this->charPage != 7) {
            for (; vtx < 0x100; vtx += 0x20) {
                gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

                for (var_s1 = 0; var_s1 < 0x20; i++, var_s1 += 4) {
                    FileChoose_DrawMultiTexQuadI4NES(
                        this->state.gfxCtx, font->fontBuf + gKeyboardCharactersKatakana[i] * FONT_CHAR_TEX_SIZE,
                        font->fontBuf + gKeyboardCharactersHiragana[i] * FONT_CHAR_TEX_SIZE, var_s1);
                }
            }
            gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
            FileChoose_DrawMultiTexQuadI4NES(this->state.gfxCtx,
                                             font->fontBuf + gKeyboardCharactersKatakana[i] * FONT_CHAR_TEX_SIZE,
                                             font->fontBuf + gKeyboardCharactersHiragana[i] * FONT_CHAR_TEX_SIZE, 0);
        } else {
            for (; vtx < 0x100; vtx += 0x20) {
                gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

                for (var_s1 = 0; var_s1 < 0x20; i++, var_s1 += 4) {
                    FileChoose_DrawMultiTexQuadI4NES(
                        this->state.gfxCtx, font->fontBuf + gKeyboardCharactersKatakana[i] * FONT_CHAR_TEX_SIZE,
                        font->fontBuf + gKeyboardCharactersAlphanumeric[i] * FONT_CHAR_TEX_SIZE, var_s1);
                }
            }
            gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
            FileChoose_DrawMultiTexQuadI4NES(
                this->state.gfxCtx, font->fontBuf + gKeyboardCharactersKatakana[i] * FONT_CHAR_TEX_SIZE,
                font->fontBuf + gKeyboardCharactersAlphanumeric[i] * FONT_CHAR_TEX_SIZE, 0);
        }
    } else if (this->charPage != 8) {
        for (; vtx < 0x100; vtx += 0x20) {
            gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

            for (var_s1 = 0; var_s1 < 0x20; i++, var_s1 += 4) {
                FileChoose_DrawMultiTexQuadI4NES(
                    this->state.gfxCtx, font->fontBuf + gKeyboardCharactersAlphanumeric[i] * FONT_CHAR_TEX_SIZE,
                    font->fontBuf + gKeyboardCharactersHiragana[i] * FONT_CHAR_TEX_SIZE, var_s1);
            }
        }
        gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
        FileChoose_DrawMultiTexQuadI4NES(this->state.gfxCtx,
                                         font->fontBuf + gKeyboardCharactersAlphanumeric[i] * FONT_CHAR_TEX_SIZE,
                                         font->fontBuf + gKeyboardCharactersHiragana[i] * FONT_CHAR_TEX_SIZE, 0);
    } else {
        for (; vtx < 0x100; vtx += 0x20) {
            gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

            for (var_s1 = 0; var_s1 < 0x20; i++, var_s1 += 4) {
                FileChoose_DrawMultiTexQuadI4NES(
                    this->state.gfxCtx, font->fontBuf + gKeyboardCharactersAlphanumeric[i] * FONT_CHAR_TEX_SIZE,
                    font->fontBuf + gKeyboardCharactersKatakana[i] * FONT_CHAR_TEX_SIZE, var_s1);
            }
        }
        gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
        FileChoose_DrawMultiTexQuadI4NES(this->state.gfxCtx,
                                         font->fontBuf + gKeyboardCharactersAlphanumeric[i] * FONT_CHAR_TEX_SIZE,
                                         font->fontBuf + gKeyboardCharactersKatakana[i] * FONT_CHAR_TEX_SIZE, 0);
    }

    CLOSE_DISPS(this->state.gfxCtx);
}

s16 D_80812C48_ne0[] = { 15, 35, 65, 80, 85, 95, 115, 145, 160, 165, 92, 170 };

s16 D_80812C60_ne0[] = { 30 - 1, 40 - 1, 80 - 1, 85 - 1, 90 - 1, 110 - 1, 120 - 1, 160 - 1, 165 - 1, 170 - 1, 92, 170 };

s16 D_80812C78_ne0[] = { 50, 45, -50, 5, -50, 50, 45, -50, 5, -50, 78, -78 };

s16 D_80812C90_ne0[] = { 15, 35, 85, 95, 115, 165, 92, 170 };

s16 D_80812CA0_ne0[] = { 30 - 1, 40 - 1, 90 - 1, 110 - 1, 120 - 1, 170 - 1, 92, 170 };

s16 D_80812CB0_ne0[] = { 50, 45, -5, 50, 45, -5, 78, -78 };

s16 D_80812CC0_ne0[] = { 35, 80, 115, 160 };

s16 D_80812CC8_ne0[] = { 40 - 1, 85 - 1, 120 - 1, 165 - 1 };

s16 D_80812CD0_ne0[] = { 50, 5, 50, 5 };

s32 func_80808480_ne0(GameState* thisx, s16 arg1, s16 arg2) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 var_v1;
    s32 temp_a1;
    u8* filename = Save_GetSaveMetaInfo(this->buttonIndex)->playerName;

    if (arg1 == 0xDF) {
        for (var_v1 = 0; var_v1 < 12U; var_v1++) {
            if (filename[arg2] >= D_80812C48_ne0[var_v1] && D_80812C60_ne0[var_v1] >= filename[arg2]) {
                filename[arg2] = D_80812C78_ne0[var_v1] + filename[arg2];
                return true;
            }
        }
    } else if (arg1 == 0xE7) {
        for (var_v1 = 0; var_v1 < 8U; var_v1++) {
            if (filename[arg2] >= D_80812C90_ne0[var_v1] && D_80812CA0_ne0[var_v1] >= filename[arg2]) {
                filename[arg2] = D_80812CB0_ne0[var_v1] + filename[arg2];
                return true;
            }
        }
    } else if (arg1 == 0xE8) {
        for (var_v1 = 0; var_v1 < 4U; var_v1++) {
            if ((filename[arg2] >= D_80812CC0_ne0[var_v1]) && (D_80812CC8_ne0[var_v1] >= filename[arg2])) {
                filename[arg2] = D_80812CD0_ne0[var_v1] + filename[arg2];
                return true;
            }
        }
    }
    return false;
}

s32 func_80808688_ne0(GameState* thisx, s16 arg1) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s32 pad;

    if (!func_80808480_ne0(&this->state, arg1, this->newFileNameCharCount)) {
        if (this->newFileNameCharCount != 0) {
            if (!func_80808480_ne0(&this->state, arg1, this->newFileNameCharCount - 1)) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

void FileChoose_DrawNameEntryNES(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    Font* font = &this->font;
    Input* input = &this->state.input[0];
    s16 i;
    s16 tmp;
    u16 dayTime;
    s16 validName;
    u8* filename = Save_GetSaveMetaInfo(this->buttonIndex)->playerName;
    OPEN_DISPS(this->state.gfxCtx);

    FileChoose_SetKeyboardVtxNES(&this->state);
    FileChoose_SetNameEntryVtxNES(&this->state);
    FileChoose_PulsateCursor(&this->state);

    Vtx* vertices = ResourceMgr_LoadVtxByName(D_80811BB0_NTSC);

    tmp = (this->newFileNameCharCount * 4) + 4;

    this->nameEntryVtx[0x24].v.ob[0] = this->nameEntryVtx[0x26].v.ob[0] = this->nameEntryVtx[tmp].v.ob[0] - 6;

    this->nameEntryVtx[0x25].v.ob[0] = this->nameEntryVtx[0x27].v.ob[0] = this->nameEntryVtx[0x24].v.ob[0] + 24;

    this->nameEntryVtx[0x24].v.ob[1] = this->nameEntryVtx[0x25].v.ob[1] = this->nameEntryVtx[tmp].v.ob[1] + 7;

    this->nameEntryVtx[0x26].v.ob[1] = this->nameEntryVtx[0x27].v.ob[1] = this->nameEntryVtx[0x24].v.ob[1] - 24;

    if ((this->kbdButton == 0) || (this->kbdButton == 1) || (this->kbdButton == 4)) {
        if (this->kbdX != this->kbdButton) {
            osSyncPrintf("014 xpos=%d  contents=%d\n", this->kbdX, this->kbdButton);
        }
        this->nameEntryVtx[40].v.ob[0] = this->nameEntryVtx[42].v.ob[0] = vertices[(this->kbdX + 1) * 4].v.ob[0] - 4;

        this->nameEntryVtx[41].v.ob[0] = this->nameEntryVtx[43].v.ob[0] = this->nameEntryVtx[40].v.ob[0] + 52;

        this->nameEntryVtx[40].v.ob[1] = this->nameEntryVtx[41].v.ob[1] = vertices[(this->kbdX + 1) * 4].v.ob[1] + 4;
    } else if ((this->kbdButton == 2) || (this->kbdButton == 3)) {
        if (this->kbdX != this->kbdButton) {
            osSyncPrintf("23 xpos=%d  contents=%d\n", this->kbdX, this->kbdButton);
        }
        this->nameEntryVtx[40].v.ob[0] = this->nameEntryVtx[42].v.ob[0] = vertices[(this->kbdX + 1) * 4].v.ob[0] - 4;

        this->nameEntryVtx[41].v.ob[0] = this->nameEntryVtx[43].v.ob[0] = this->nameEntryVtx[40].v.ob[0] + 40;

        this->nameEntryVtx[40].v.ob[1] = this->nameEntryVtx[41].v.ob[1] = vertices[(this->kbdX + 1) * 4].v.ob[1] + 4;
    } else {
        if (this->charIndex >= 65) {
            osSyncPrintf("mjp=%d  xpos=%d  ypos=%d  name_contents=%d\n", this->charIndex, this->kbdX, this->kbdY,
                         this->kbdButton);
        }
        this->nameEntryVtx[40].v.ob[0] = this->nameEntryVtx[42].v.ob[0] =
            this->keyboardVtx[this->charIndex * 4].v.ob[0] - 6;

        this->nameEntryVtx[41].v.ob[0] = this->nameEntryVtx[43].v.ob[0] = this->nameEntryVtx[40].v.ob[0] + 0x18;

        this->nameEntryVtx[40].v.ob[1] = this->nameEntryVtx[41].v.ob[1] =
            this->keyboardVtx[this->charIndex * 4].v.ob[1] + 6;
    }

    this->nameEntryVtx[42].v.ob[1] = this->nameEntryVtx[43].v.ob[1] = this->nameEntryVtx[40].v.ob[1] - 24;

    gSPVertex(POLY_OPA_DISP++, &this->nameEntryVtx[36], 8, 0);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->highlightColor[0], this->highlightColor[1], this->highlightColor[2],
                    this->highlightColor[3]);
    gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelCharHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 24, 24, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    if ((this->kbdButton == 0) || (this->kbdButton == 1) || (this->kbdButton == 4)) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelMediumButtonHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 56, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
    } else if ((this->kbdButton == 2) || (this->kbdButton == 3)) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelSmallButtonHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 40, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
    }

    gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);

    FileChoose_DrawKeyboardNES(&this->state);

    gDPPipeSync(POLY_OPA_DISP++);

    Gfx_SetupDL_42Opa(this->state.gfxCtx);

    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

    if (this->configMode == CM_NAME_ENTRY) {
        if (CHECK_BTN_ALL(input->press.button, BTN_START)) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            this->kbdY = 5;
            this->kbdX = 4;
        } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
            if ((this->newFileNameCharCount == 7) && (filename[7] != 0xDF)) {
                for (i = this->newFileNameCharCount; i < 7; i++) {
                    filename[i] = filename[i + 1];
                }
                filename[i] = 0xDF;
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            } else {
                this->newFileNameCharCount--;
                if (this->newFileNameCharCount < 0) {
                    this->newFileNameCharCount = 0;
                    if (this->prevConfigMode == CM_QUEST_MENU) {
                        this->configMode = CM_NAME_ENTRY_TO_QUEST_MENU;
                    } else if (this->prevConfigMode == CM_RANDOMIZER_SETTINGS_MENU) {
                        this->configMode = CM_NAME_ENTRY_TO_RANDOMIZER_SETTINGS_MENU;
                    } else {
                        this->configMode = CM_NAME_ENTRY_TO_MAIN;
                    }
                    this->prevConfigMode = CM_NAME_ENTRY;
                    sLastCharIndex = -1;
                    CVarSetInteger(CVAR_GENERAL("OnFileSelectNameEntry"), 0);
                } else {
                    for (i = this->newFileNameCharCount; i < 7; i++) {
                        filename[i] = filename[i + 1];
                    }
                    filename[i] = 0xDF;
                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                }
            }
        } else {
            if (this->charPage < 3) {
                if (this->kbdY != 5) {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 255);

                    if (this->charPage == 0) {
                        gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[(this->charIndex * 4)], 4, 0);
                        FileChoose_DrawCharacterNES(
                            this->state.gfxCtx,
                            font->fontBuf + gKeyboardCharactersHiragana[this->charIndex] * FONT_CHAR_TEX_SIZE, 0);
                        if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                            if ((gKeyboardCharactersHiragana[this->charIndex] == 0xE7) ||
                                (gKeyboardCharactersHiragana[this->charIndex] == 0xE8)) {
                                if (func_80808688_ne0(&this->state, gKeyboardCharactersHiragana[this->charIndex]) ==
                                    0) {
                                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &gSfxDefaultPos, 4,
                                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                                                           &gSfxDefaultReverb);
                                } else {
                                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &gSfxDefaultPos, 4,
                                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                                                           &gSfxDefaultReverb);
                                }
                            } else {
                                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &gSfxDefaultPos, 4,
                                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                                                       &gSfxDefaultReverb);
                                filename[this->newFileNameCharCount] = gKeyboardCharactersHiragana[this->charIndex];
                                this->newFileNameCharCount++;
                                if (this->newFileNameCharCount >= 8) {
                                    this->newFileNameCharCount = 7;
                                }
                            }
                        }
                    } else if (this->charPage == 1) {
                        gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[(this->charIndex * 4)], 4, 0);
                        FileChoose_DrawCharacterNES(
                            this->state.gfxCtx,
                            font->fontBuf + gKeyboardCharactersKatakana[this->charIndex] * FONT_CHAR_TEX_SIZE, 0);
                        if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                            if ((gKeyboardCharactersHiragana[this->charIndex] == 0xE7) ||
                                (gKeyboardCharactersHiragana[this->charIndex] == 0xE8)) {
                                if (func_80808688_ne0(&this->state, gKeyboardCharactersHiragana[this->charIndex]) ==
                                    0) {
                                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &gSfxDefaultPos, 4,
                                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                                                           &gSfxDefaultReverb);
                                } else {
                                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &gSfxDefaultPos, 4,
                                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                                                           &gSfxDefaultReverb);
                                }
                            } else {
                                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &gSfxDefaultPos, 4,
                                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                                                       &gSfxDefaultReverb);
                                filename[this->newFileNameCharCount] = gKeyboardCharactersKatakana[this->charIndex];
                                this->newFileNameCharCount++;
                                if (this->newFileNameCharCount >= 8) {
                                    this->newFileNameCharCount = 7;
                                }
                            }
                        }
                    } else {
                        gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[(this->charIndex * 4)], 4, 0);
                        FileChoose_DrawCharacterNES(
                            this->state.gfxCtx,
                            font->fontBuf + gKeyboardCharactersAlphanumeric[this->charIndex] * FONT_CHAR_TEX_SIZE, 0);
                        if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &gSfxDefaultPos, 4,
                                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                                                   &gSfxDefaultReverb);
                            filename[this->newFileNameCharCount] = gKeyboardCharactersAlphanumeric[this->charIndex];
                            this->newFileNameCharCount++;
                            if (this->newFileNameCharCount >= 8) {
                                this->newFileNameCharCount = 7;
                            }
                        }
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                    if (this->charPage != this->kbdButton) {
                        if (this->kbdButton == 0) {
                            if (this->charPage == 1) {
                                this->charPage = 4;
                            } else {
                                this->charPage = 6;
                            }
                            Audio_PlaySoundGeneral(NA_SE_SY_WIN_OPEN, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                        } else if (this->kbdButton == 1) {
                            if (this->charPage == 0) {
                                this->charPage = 3;
                            } else {
                                this->charPage = 8;
                            }
                            Audio_PlaySoundGeneral(NA_SE_SY_WIN_OPEN, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                        } else if (this->kbdButton == 2) {
                            if (this->charPage == 0) {
                                this->charPage = 5;
                            } else {
                                this->charPage = 7;
                            }
                            Audio_PlaySoundGeneral(NA_SE_SY_WIN_OPEN, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                        } else if (this->kbdButton == 3) {
                            if (this->newFileNameCharCount == 7 && filename[7] != 0xDF) {
                                for (i = this->newFileNameCharCount; i < 7; i++) {
                                    filename[i] = filename[i + 1];
                                }
                                filename[i] = 0xDF;
                                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &gSfxDefaultPos, 4,
                                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                                                       &gSfxDefaultReverb);
                            } else {
                                this->newFileNameCharCount--;
                                if (this->newFileNameCharCount < 0) {
                                    this->newFileNameCharCount = 0;
                                }
                                for (i = this->newFileNameCharCount; i < 7; i++) {
                                    filename[i] = filename[i + 1];
                                }
                                filename[i] = 0xDF;
                                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &gSfxDefaultPos, 4,
                                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                                                       &gSfxDefaultReverb);
                            }
                        } else if (this->kbdButton == 4) {
                            validName = false;
                            for (i = 0; i < 8; i++) {
                                if (filename[i] != 0xDF) {
                                    validName = true;
                                    break;
                                }
                            }

                            if (validName) {
                                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &gSfxDefaultPos, 4,
                                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                                                       &gSfxDefaultReverb);
                                gSaveContext.fileNum = this->buttonIndex;
                                dayTime = ((void)0, gSaveContext.dayTime);
                                Sram_InitSave(this);
                                gSaveContext.dayTime = dayTime;
                                this->prevConfigMode = CM_MAIN_MENU;
                                this->configMode = CM_NAME_ENTRY_TO_MAIN;
                                CVarSetInteger(CVAR_GENERAL("OnFileSelectNameEntry"), 0);
                                this->nameBoxAlpha[this->buttonIndex] = this->nameAlpha[this->buttonIndex] = 200;
                                this->connectorAlpha[this->buttonIndex] = 255;
                                func_800AA000(300.0f, 180, 20, 100);
                            } else {
                                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &gSfxDefaultPos, 4,
                                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                                                       &gSfxDefaultReverb);
                            }
                        }
                    }
                }
                if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    this->newFileNameCharCount++;
                    if (this->newFileNameCharCount >= 8) {
                        this->newFileNameCharCount = 7;
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    this->newFileNameCharCount--;
                    if (this->newFileNameCharCount < 0) {
                        this->newFileNameCharCount = 0;
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_Z)) {
                    if (func_80808688_ne0(&this->state, 0xDF) != 0) {
                        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                               &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    }
                }
            }
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileChoose_StartNameEntryNES(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->nameEntryBoxAlpha += 25;

    if (this->nameEntryBoxAlpha >= 255) {
        this->nameEntryBoxAlpha = 255;
    }

    this->nameEntryBoxPosX -= 30;

    if (this->nameEntryBoxPosX <= 0) {
        this->nameEntryBoxPosX = 0;
        this->nameEntryBoxAlpha = 255;
        this->kbdX = 0;
        this->kbdY = 0;
        this->kbdButton = 99;
        this->configMode = CM_NAME_ENTRY;
    }
}

void FileChoose_UpdateKeyboardCursorNES(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    Input* input = &this->state.input[0];
    s16 prevKbdX;
    s32 pad;
    bool dpad = CVarGetInteger(CVAR_SETTING("DpadInText"), 0);

    if (this->charPage < 3) {
        if (CHECK_BTN_ALL(input->press.button, BTN_R)) {
            if (gSaveContext.language == LANGUAGE_JPN) {
                if (this->charPage == 0) {
                    this->charPage = 3;
                    Audio_PlaySoundGeneral(NA_SE_SY_WIN_OPEN, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                } else if (this->charPage == 1) {
                    this->charPage = 7;
                    Audio_PlaySoundGeneral(NA_SE_SY_WIN_OPEN, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                } else if (this->charPage == 2) {
                    this->charPage = 6;
                    Audio_PlaySoundGeneral(NA_SE_SY_WIN_OPEN, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                }
            }
        } else {
            this->kbdButton = 99;
            if (this->kbdY != 5) {
                if ((this->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DLEFT))) {
                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    this->charIndex--;
                    this->kbdX--;
                    if (this->kbdX < 0) {
                        this->kbdX = 12;
                        this->charIndex = this->kbdY * 13 + this->kbdX;
                    }
                } else if ((this->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    this->charIndex++;
                    this->kbdX++;
                    if (this->kbdX >= 13) {
                        this->kbdX = 0;
                        this->charIndex = this->kbdY * 13 + this->kbdX;
                    }
                }
            } else if (gSaveContext.language == LANGUAGE_JPN) {
                if ((this->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DLEFT))) {
                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    this->kbdX--;
                    if (this->kbdX < 0) {
                        this->kbdX = 4;
                    }
                } else if ((this->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    this->kbdX++;
                    if (this->kbdX >= 5) {
                        this->kbdX = 0;
                    }
                }
            } else {
                if ((this->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DLEFT))) {
                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    this->kbdX--;
                    if (this->kbdX < 3) {
                        this->kbdX = 4;
                    }
                } else if ((this->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    this->kbdX++;
                    if (this->kbdX >= 5) {
                        this->kbdX = 3;
                    }
                }
            }

            if ((this->stickRelY > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DUP))) {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                this->kbdY--;
                if (this->kbdY < 0) {
                    if (gSaveContext.language == LANGUAGE_JPN) {
                        this->kbdY = 5;
                        this->charIndex += 52;
                        prevKbdX = this->kbdX;
                        if (this->kbdX < 3) {
                            this->kbdX = 0;
                        } else if (this->kbdX < 6) {
                            this->kbdX = 1;
                        } else if (this->kbdX < 8) {
                            this->kbdX = 2;
                        } else if (this->kbdX < 10) {
                            this->kbdX = 3;
                        } else if (this->kbdX < 13) {
                            this->kbdX = 4;
                        }

                        this->unk_1CAD6[this->kbdX] = prevKbdX;
                    } else if (this->kbdX < 8) {
                        this->kbdY = 4;
                        this->charIndex = this->kbdX + 52;
                    } else {
                        this->kbdY = 5;
                        this->charIndex += 52;
                        prevKbdX = this->kbdX;
                        if (this->kbdX < 10) {
                            this->kbdX = 3;
                        } else if (this->kbdX < 13) {
                            this->kbdX = 4;
                        }

                        this->unk_1CAD6[this->kbdX] = prevKbdX;
                    }
                } else {
                    this->charIndex -= 13;
                    if (this->kbdY == 4) {
                        this->charIndex = 52;
                        this->kbdX = this->unk_1CAD6[this->kbdX];
                        this->charIndex += this->kbdX;
                    }
                }
            } else if ((this->stickRelY < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DDOWN))) {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                this->kbdY++;
                if (this->kbdY >= 6) {
                    this->kbdY = 0;
                    this->kbdX = this->unk_1CAD6[this->kbdX];
                    this->charIndex = this->kbdX;
                } else {
                    this->charIndex += 13;
                    if (this->kbdY == 5) {
                        if (gSaveContext.language != LANGUAGE_JPN) {
                            if (this->kbdX < 8) {
                                this->kbdY = 0;
                                this->charIndex = this->kbdX;
                            } else {
                                prevKbdX = this->kbdX;
                                if (this->kbdX < 3) {
                                    this->kbdX = 0;
                                } else if (this->kbdX < 6) {
                                    this->kbdX = 1;
                                } else if (this->kbdX < 8) {
                                    this->kbdX = 2;
                                } else if (this->kbdX < 10) {
                                    this->kbdX = 3;
                                } else if (this->kbdX < 13) {
                                    this->kbdX = 4;
                                }

                                this->unk_1CAD6[this->kbdX] = prevKbdX;
                            }
                        } else {
                            prevKbdX = this->kbdX;
                            if (this->kbdX < 3) {
                                this->kbdX = 0;
                            } else if (this->kbdX < 6) {
                                this->kbdX = 1;
                            } else if (this->kbdX < 8) {
                                this->kbdX = 2;
                            } else if (this->kbdX < 10) {
                                this->kbdX = 3;
                            } else if (this->kbdX < 13) {
                                this->kbdX = 4;
                            }
                            this->unk_1CAD6[this->kbdX] = prevKbdX;
                        }
                    }
                }
            }
            if (this->kbdY == 5) {
                this->kbdButton = this->kbdX;

                if (sLastKbdX != this->kbdX) {
                    // NTSC TODO: Figure out japanese TTS
                    // GameInteractor_ExecuteOnUpdateFileNameSelection(0xF0 + this->kbdX);
                    sLastKbdX = this->kbdX;
                    sLastCharIndex = -1;
                }
            } else if (sLastCharIndex != this->charIndex && this->charIndex < 65) {
                // NTSC TODO: Figure out japanese TTS
                // GameInteractor_ExecuteOnUpdateFileNameSelection(D_808123F0[this->charIndex]);
                sLastCharIndex = this->charIndex;
                sLastKbdX = -1;
            }
        }
    } else {
        this->charBgAlpha += ZREG(9);
        if (this->charBgAlpha >= 255) {
            this->charBgAlpha = 0;
            this->charPage = D_808127DC_ne0[this->charPage];
        }
    }
}

void FileChoose_StartOptionsNES(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->nameEntryBoxAlpha += 25;

    if (this->nameEntryBoxAlpha >= 255) {
        this->nameEntryBoxAlpha = 255;
    }

    this->nameEntryBoxPosX -= 30;

    if (this->nameEntryBoxPosX <= 0) {
        this->nameEntryBoxPosX = 0;
        this->nameEntryBoxAlpha = 255;
        this->configMode = CM_OPTIONS_MENU;
    }
}

static u8 sSelectedSetting;

void FileChoose_UpdateOptionsMenuNES(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    // SramContext* sramCtx = &this->sramCtx;
    Input* input = &this->state.input[0];
    bool dpad = CVarGetInteger(CVAR_SETTING("DpadInText"), 0);

    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                               &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        this->prevConfigMode = this->configMode;
        this->configMode = CM_OPTIONS_TO_MAIN;
        sLastOptionButtonIndex = -1;
        // sramCtx->readBuff[0] = gSaveContext.audioSetting;
        // sramCtx->readBuff[1] = gSaveContext.zTargetSetting;
        // osSyncPrintf("ＳＡＶＥ");
        // Sram_WriteSramHeader(sramCtx);
        Save_SaveGlobal();
        CVarSave();
        // osSyncPrintf(VT_FGCOL(YELLOW));
        // osSyncPrintf("sram->read_buff[2] = J_N = %x\n", sramCtx->readBuff[2]);
        // osSyncPrintf("sram->read_buff[2] = J_N = %x\n", &sramCtx->readBuff[2]);
        // osSyncPrintf("Na_SetSoundOutputMode = %d\n", gSaveContext.audioSetting);
        // osSyncPrintf("Na_SetSoundOutputMode = %d\n", gSaveContext.audioSetting);
        // osSyncPrintf("Na_SetSoundOutputMode = %d\n", gSaveContext.audioSetting);
        // osSyncPrintf(VT_RST);
        func_800F6700(gSaveContext.audioSetting);
        // osSyncPrintf("終了\n");
        return;
    }

    if ((this->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DLEFT))) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                               &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

        if (sSelectedSetting == FS_SETTING_AUDIO) {
            gSaveContext.audioSetting--;

            // because audio setting is unsigned, can't check for < 0
            if (gSaveContext.audioSetting > 0xF0) {
                gSaveContext.audioSetting = FS_AUDIO_SURROUND;
            }
        } else {
            gSaveContext.zTargetSetting ^= 1;
        }
    } else if ((this->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                               &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

        if (sSelectedSetting == FS_SETTING_AUDIO) {
            gSaveContext.audioSetting++;

            if (gSaveContext.audioSetting > FS_AUDIO_SURROUND) {
                gSaveContext.audioSetting = FS_AUDIO_STEREO;
            }
        } else {
            gSaveContext.zTargetSetting ^= 1;
        }
    }

    if ((this->stickRelY < -30) || (this->stickRelY > 30) ||
        (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN | BTN_DUP))) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                               &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        sSelectedSetting ^= 1;
    } else if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                               &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        sSelectedSetting ^= 1;
    }

    if (sSelectedSetting == FS_SETTING_AUDIO) {
        if (sLastOptionButtonIndex != gSaveContext.audioSetting) {
            GameInteractor_ExecuteOnUpdateFileAudioSelection(gSaveContext.audioSetting);
            sLastOptionButtonIndex = gSaveContext.audioSetting;
        }
    } else if (sSelectedSetting == FS_SETTING_TARGET) {
        // offset to detect switching between modes
        u8 optionOffset = gSaveContext.zTargetSetting + FS_AUDIO_SURROUND + FS_SETTING_TARGET;
        if (sLastOptionButtonIndex != optionOffset) {
            GameInteractor_ExecuteOnUpdateFileTargetSelection(gSaveContext.zTargetSetting);
            sLastOptionButtonIndex = optionOffset;
        }
    }
}

void FileChoose_DrawOptionsImplNES(GameState* thisx) {
    static s16 cursorPrimRed = 255;
    static s16 cursorPrimGreen = 255;
    static s16 cursorPrimBlue = 255;
    static s16 cursorEnvRed = 0;
    static s16 cursorEnvGreen = 0;
    static s16 cursorEnvBlue = 0;
    static s16 cursorPulseDir = 1;
    static s16 cursorFlashTimer = 20;
    static s16 cursorPrimColors[][3] = {
        { 255, 255, 255 },
        { 0, 255, 255 },
    };
    static s16 cursorEnvColors[][3] = {
        { 0, 0, 0 },
        { 0, 150, 150 },
    };
    if (CVarGetInteger(CVAR_COSMETIC("Title.FileChoose.Changed"), 0)) {
        Color_RGB8 backgroundColor =
            CVarGetColor24(CVAR_COSMETIC("Title.FileChoose.Value"), (Color_RGB8){ 100, 150, 255 });
        cursorPrimColors[1][0] = MIN(backgroundColor.r + 100, 255);
        cursorPrimColors[1][1] = MIN(backgroundColor.g + 100, 255);
        cursorPrimColors[1][2] = MIN(backgroundColor.b + 100, 255);
        cursorEnvColors[1][0] = MIN(backgroundColor.r + 50, 255);
        cursorEnvColors[1][1] = MIN(backgroundColor.g + 50, 255);
        cursorEnvColors[1][2] = MIN(backgroundColor.b + 50, 255);
    }

    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 cursorRed;
    s16 cursorGreen;
    s16 cursorBlue;
    s16 i;
    s16 j;
    s16 vtx;

    OPEN_DISPS(this->state.gfxCtx);

    cursorRed = ABS(cursorPrimRed - cursorPrimColors[cursorPulseDir][0]) / cursorFlashTimer;
    cursorGreen = ABS(cursorPrimGreen - cursorPrimColors[cursorPulseDir][1]) / cursorFlashTimer;
    cursorBlue = ABS(cursorPrimBlue - cursorPrimColors[cursorPulseDir][2]) / cursorFlashTimer;

    if (cursorPrimRed >= cursorPrimColors[cursorPulseDir][0]) {
        cursorPrimRed -= cursorRed;
    } else {
        cursorPrimRed += cursorRed;
    }

    if (cursorPrimGreen >= cursorPrimColors[cursorPulseDir][1]) {
        cursorPrimGreen -= cursorGreen;
    } else {
        cursorPrimGreen += cursorGreen;
    }

    if (cursorPrimBlue >= cursorPrimColors[cursorPulseDir][2]) {
        cursorPrimBlue -= cursorBlue;
    } else {
        cursorPrimBlue += cursorBlue;
    }

    cursorRed = ABS(cursorEnvRed - cursorEnvColors[cursorPulseDir][0]) / cursorFlashTimer;
    cursorGreen = ABS(cursorEnvGreen - cursorEnvColors[cursorPulseDir][1]) / cursorFlashTimer;
    cursorBlue = ABS(cursorEnvBlue - cursorEnvColors[cursorPulseDir][2]) / cursorFlashTimer;

    if (cursorEnvRed >= cursorEnvColors[cursorPulseDir][0]) {
        cursorEnvRed -= cursorRed;
    } else {
        cursorEnvRed += cursorRed;
    }

    if (cursorEnvGreen >= cursorEnvColors[cursorPulseDir][1]) {
        cursorEnvGreen -= cursorGreen;
    } else {
        cursorEnvGreen += cursorGreen;
    }

    if (cursorEnvBlue >= cursorEnvColors[cursorPulseDir][2]) {
        cursorEnvBlue -= cursorBlue;
    } else {
        cursorEnvBlue += cursorBlue;
    }

    if (--cursorFlashTimer == 0) {
        cursorPrimRed = cursorPrimColors[cursorPulseDir][0];
        cursorPrimGreen = cursorPrimColors[cursorPulseDir][1];
        cursorPrimBlue = cursorPrimColors[cursorPulseDir][2];

        cursorEnvRed = cursorEnvColors[cursorPulseDir][0];
        cursorEnvGreen = cursorEnvColors[cursorPulseDir][1];
        cursorEnvBlue = cursorEnvColors[cursorPulseDir][2];

        cursorFlashTimer = 20;

        if (++cursorPulseDir > 1) {
            cursorPulseDir = 0;
        }
    }

    gSPVertex(POLY_OPA_DISP++, D_80811D30_NTSC, 32, 0);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    for (i = 0, vtx = 0; i < 4; i++, vtx += 4) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, sOptionsMenuHeaders[i].texture[NTSC_LANGUAGE_INDEX], G_IM_FMT_IA,
                            G_IM_SIZ_8b, sOptionsMenuHeaders[i].width, sOptionsMenuHeaders[i].height, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    gSPVertex(POLY_OPA_DISP++, D_80811F30_NTSC, 32, 0);

    for (i = 0, vtx = 0; i < 4; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);
        if (i == gSaveContext.audioSetting) {
            if (sSelectedSetting == FS_SETTING_AUDIO) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, cursorPrimRed, cursorPrimGreen, cursorPrimBlue,
                                this->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, cursorEnvRed, cursorEnvGreen, cursorEnvBlue, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[0]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        //! @bug Mistakenly using sOptionsMenuHeaders instead of sOptionsMenuSettings for the height.
        //! This works out anyway because all heights are 16.
        gDPLoadTextureBlock(POLY_OPA_DISP++, sOptionsMenuSettings[i].texture[NTSC_LANGUAGE_INDEX], G_IM_FMT_IA,
                            G_IM_SIZ_8b, sOptionsMenuSettings[i].width, sOptionsMenuHeaders[i].height, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    for (; i < 6; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);

        if (i == (gSaveContext.zTargetSetting + 4)) {
            if (sSelectedSetting != FS_SETTING_AUDIO) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, cursorPrimRed, cursorPrimGreen, cursorPrimBlue,
                                this->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, cursorEnvRed, cursorEnvGreen, cursorEnvBlue, 0xFF);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[0]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        //! @bug Mistakenly using sOptionsMenuHeaders instead of sOptionsMenuSettings for the height.
        //! This is also an OOB read that happens to access the height of the first two elements in
        //! sOptionsMenuSettings, and since all heights are 16, it works out anyway.
        gDPLoadTextureBlock(POLY_OPA_DISP++, sOptionsMenuSettings[i].texture[NTSC_LANGUAGE_INDEX], G_IM_FMT_IA,
                            G_IM_SIZ_8b, sOptionsMenuSettings[i].width, sOptionsMenuHeaders[i].height, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);

    // check brightness bars
    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelBrightnessCheckTex, G_IM_FMT_IA, 96, 16, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 55, 55, 55, this->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 40, 40, 40, 255);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    vtx += 4;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 30, 30, 30, this->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    vtx += 4;

    // blue divider lines
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 255, 255, this->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelOptionsDividerTex, G_IM_FMT_IA, 256, 2, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    Matrix_Push();
    Matrix_Translate(0.0f, 0.1f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPVertex(POLY_OPA_DISP++, gOptionsDividerTopVtx, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_Pop();

    Matrix_Push();
    Matrix_Translate(0.0f, 0.2f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPVertex(POLY_OPA_DISP++, gOptionsDividerMiddleVtx, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_Pop();

    Matrix_Push();
    Matrix_Translate(0.0f, 0.4f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPVertex(POLY_OPA_DISP++, gOptionsDividerBottomVtx, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_Pop();

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileChoose_DrawOptionsNES(GameState* thisx) {
    FileChoose_DrawOptionsImplNES(thisx);
}
