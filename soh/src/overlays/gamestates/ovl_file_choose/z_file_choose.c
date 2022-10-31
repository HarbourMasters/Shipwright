#include "file_choose.h"

#include <string.h>

#include "textures/title_static/title_static.h"
#include "textures/parameter_static/parameter_static.h"
#include <textures/icon_item_static/icon_item_static.h>
#include "soh/frame_interpolation.h"
#include <libultraship/GameVersions.h>

static s16 sUnused = 106;

static s16 sScreenFillAlpha = 255;

static u8 isFastFileIdIncompatible = 0;

static Gfx sScreenFillSetupDL[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsSPEndDisplayList(),
};

static s16 sFileInfoBoxPartWidths[] = { 36, 36, 36, 36, 24 };

static s16 sWindowContentColors[2][3] = {
    { 100, 150, 255 }, // blue
    { 100, 100, 100 }, // gray
};

static int FileChoose_IsSaveCompatible(const SaveFileMetaInfo* restrict meta) {
    bool valid = true;
    if (meta->requiresMasterQuest) {
        valid = valid && ResourceMgr_GameHasMasterQuest();
    }
    if (meta->requiresOriginal) {
        valid = valid && ResourceMgr_GameHasOriginal();
    }
    return valid;
}

void FileChoose_SetView(FileChooseContext* this, f32 eyeX, f32 eyeY, f32 eyeZ) {
    Vec3f eye;
    Vec3f lookAt;
    Vec3f up;

    lookAt.x = lookAt.y = lookAt.z = 0.0f;
    up.x = up.z = 0.0f;

    eye.x = eyeX;
    eye.y = eyeY;
    eye.z = eyeZ;

    up.y = 1.0f;

    func_800AA358(&this->view, &eye, &lookAt, &up);
    func_800AAA50(&this->view, 0x7F);
}

Gfx* FileChoose_QuadTextureIA8(Gfx* gfx, void* texture, s16 width, s16 height, s16 point)
{
    gDPLoadTextureBlock(gfx++, texture, G_IM_FMT_IA, G_IM_SIZ_8b, width, height, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSP1Quadrangle(gfx++, point, point + 2, point + 3, point + 1, 0);

    return gfx;
}

void FileChoose_InitModeUpdate(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    if (this->menuMode == FS_MENU_MODE_INIT) {
        this->menuMode = FS_MENU_MODE_CONFIG;
        this->configMode = CM_FADE_IN_START;
        this->nextTitleLabel = FS_TITLE_OPEN_FILE;
    }
}

void FileChoose_InitModeDraw(GameState* thisx) {
}

/**
 * Fade in the menu window and title label.
 * If a file is occupied fade in the name, name box, and connector.
 * Fade in the copy erase and options button according to the window alpha.
 */
void FileChoose_FadeInMenuElements(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 i;

    this->titleAlpha[0] += VREG(1);
    this->windowAlpha += VREG(2);

    for (i = 0; i < 3; i++) {
        this->fileButtonAlpha[i] = this->windowAlpha;

        if (Save_GetSaveMetaInfo(i)->valid) {
            this->nameBoxAlpha[i] = this->nameAlpha[i] = this->windowAlpha;
            this->connectorAlpha[i] += VREG(1);
            if (this->connectorAlpha[i] >= 255) {
                this->connectorAlpha[i] = 255;
            }
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        this->optionButtonAlpha = this->windowAlpha;
}

/**
 * Converts a numerical value to ones-tens-hundreds digits
 */
void FileChoose_SplitNumber(u16 value, s16* hundreds, s16* tens, s16* ones) {
    *hundreds = 0;
    *tens = 0;
    *ones = value;

    while (true) {
        if ((*ones - 100) < 0) {
            break;
        }
        (*hundreds)++;
        *ones -= 100;
    }

    while (true) {
        if ((*ones - 10) < 0) {
            break;
        }
        (*tens)++;
        *ones -= 10;
    }
}

/**
 * Reduce the alpha of the black screen fill to create a fade in effect.
 * Additionally, slide the window from the right to the center of the screen.
 * Update function for `CM_FADE_IN_START`
 */
void FileChoose_StartFadeIn(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    FileChoose_FadeInMenuElements(&this->state);
    sScreenFillAlpha -= 40;
    this->windowPosX -= 20;

    if (this->windowPosX <= -94) {
        this->windowPosX = -94;
        this->configMode = CM_FADE_IN_END;
        sScreenFillAlpha = 0;
    }
}

/**
 * Finish fading in the remaining menu elements.
 * Fade in the controls text at the bottom of the screen.
 * Update function for `CM_FADE_IN_END`
 */
void FileChoose_FinishFadeIn(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->controlsAlpha += VREG(1);
    FileChoose_FadeInMenuElements(&this->state);

    if (this->titleAlpha[0] >= 255) {
        this->titleAlpha[0] = 255;
        this->controlsAlpha = 255;
        this->windowAlpha = 200;
        this->configMode = CM_MAIN_MENU;
    }
}

void SpriteLoad(FileChooseContext* this, Sprite* sprite) {
    OPEN_DISPS(this->state.gfxCtx);

    if (sprite->im_siz == G_IM_SIZ_16b) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, sprite->tex, sprite->im_fmt,
                            G_IM_SIZ_16b, // @TEMP until I figure out how to use sprite->im_siz
                            sprite->width, sprite->height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    } else {
        gDPLoadTextureBlock(POLY_OPA_DISP++, sprite->tex, sprite->im_fmt,
                            G_IM_SIZ_32b, // @TEMP until I figure out how to use sprite->im_siz
                            sprite->width, sprite->height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    }

    CLOSE_DISPS(this->state.gfxCtx);
}

void SpriteDraw(FileChooseContext* this, Sprite* sprite, int left, int top, int width, int height) {
    int width_factor = (1 << 10) * sprite->width / width;
    int height_factor = (1 << 10) * sprite->height / height;

    OPEN_DISPS(this->state.gfxCtx);

    gSPWideTextureRectangle(POLY_OPA_DISP++, left << 2, top << 2, (left + width) << 2, (top + height) << 2,
                            G_TX_RENDERTILE, 0, 0, width_factor, height_factor);

    CLOSE_DISPS(this->state.gfxCtx);
}

bool fileSelectSpoilerFileLoaded = false;

void DrawSeedHashSprites(FileChooseContext* this) {
    OPEN_DISPS(this->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    if (this->windowRot == 0) {
        if (this->selectMode == SM_CONFIRM_FILE) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0xFF, 0xFF, 0xFF, this->fileInfoAlpha[this->buttonIndex]);

            u16 xStart = 64;
            // Draw Seed Icons
            for (unsigned int i = 0; i < 5; i++) {
                if (Save_GetSaveMetaInfo(this->selectedFileIndex)->randoSave == 1) {
                    SpriteLoad(this, GetSeedTexture(Save_GetSaveMetaInfo(this->selectedFileIndex)->seedHash[i]));
                    SpriteDraw(this, GetSeedTexture(Save_GetSaveMetaInfo(this->selectedFileIndex)->seedHash[i]),
                               xStart + (18 * i), 136, 16, 16);
                }
            }
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0xFF, 0xFF, 0xFF, this->fileButtonAlpha[this->buttonIndex]);

        if (CVar_GetS32("gRandomizer", 0) && strnlen(CVar_GetString("gSpoilerLog", ""), 1) != 0 && fileSelectSpoilerFileLoaded) {
            u16 xStart = 64;
            for (unsigned int i = 0; i < 5; i++) {
                SpriteLoad(this, GetSeedTexture(gSaveContext.seedIcons[i]));
                SpriteDraw(this, GetSeedTexture(gSaveContext.seedIcons[i]), xStart + (40 * i), 10, 24, 24);
            }
            }
    }

    gDPPipeSync(POLY_OPA_DISP++);

    CLOSE_DISPS(this->state.gfxCtx);
}

u8 generating;

/**
 * Update the cursor and wait for the player to select a button to change menus accordingly.
 * If an empty file is selected, enter the name entry config mode.
 * If an occupied file is selected, enter the `Select` menu mode.
 * If copy, erase, or options is selected, set config mode accordingly.
 * Lastly, set any warning labels if appropriate.
 * Update function for `CM_MAIN_MENU`
 */
void FileChoose_UpdateMainMenu(GameState* thisx) {
    static u8 emptyName[] = { 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E };
    FileChooseContext* this = (FileChooseContext*)thisx;
    Input* input = &this->state.input[0];
    bool dpad = CVar_GetS32("gDpadText", 0);

    if (CVar_GetS32("gRandoGenerating", 0) != 0 && generating == 0) {
        generating = 1;
        func_800F5E18(SEQ_PLAYER_BGM_MAIN, NA_BGM_HORSE, 0, 7, 1);
        return;
    } else if (CVar_GetS32("gRandoGenerating", 0) == 0 && generating) {
        Audio_PlayFanfare(NA_BGM_HORSE_GOAL);
        func_800F5E18(SEQ_PLAYER_BGM_MAIN, NA_BGM_FILE_SELECT, 0, 7, 1);
        generating = 0;
        return;
    } else if (generating) {
        return;
    }

    if (!SpoilerFileExists(CVar_GetString("gSpoilerLog", ""))) {
        CVar_SetString("gSpoilerLog", "");
        fileSelectSpoilerFileLoaded = false;
    }

    if ((CVar_GetS32("gNewFileDropped", 0) != 0) ||
        (CVar_GetS32("gNewSeedGenerated", 0) != 0) ||
        (!fileSelectSpoilerFileLoaded && SpoilerFileExists(CVar_GetString("gSpoilerLog", "")))) {
        if (CVar_GetS32("gNewFileDropped", 0) != 0) {
            CVar_SetString("gSpoilerLog", CVar_GetString("gDroppedFile", "None"));
        }
        bool silent = true;
        if ((CVar_GetS32("gNewFileDropped", 0) != 0) ||
            (CVar_GetS32("gNewSeedGenerated", 0) != 0)) {
            silent = false;
        }
        CVar_SetS32("gNewSeedGenerated", 0);
        CVar_SetS32("gNewFileDropped", 0);
        CVar_SetString("gDroppedFile", "");
        fileSelectSpoilerFileLoaded = false;
        const char* fileLoc = CVar_GetString("gSpoilerLog", "");
        Randomizer_LoadSettings(fileLoc);
        Randomizer_LoadHintLocations(fileLoc);
        Randomizer_LoadRequiredTrials(fileLoc);
        Randomizer_LoadItemLocations(fileLoc, silent);
        Randomizer_LoadMerchantMessages(fileLoc);
        Randomizer_LoadMasterQuestDungeons(fileLoc);
        fileSelectSpoilerFileLoaded = true;
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->press.button, BTN_A)) {
        if (this->buttonIndex <= FS_BTN_MAIN_FILE_3) {
            if (!Save_GetSaveMetaInfo(this->buttonIndex)->valid) {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->configMode = CM_ROTATE_TO_NAME_ENTRY;
                CVar_SetS32("gOnFileSelectNameEntry", 1);
                this->kbdButton = FS_KBD_BTN_NONE;
                this->charPage = FS_CHAR_PAGE_ENG;
                this->kbdX = 0;
                this->kbdY = 0;
                this->charIndex = 0;
                this->charBgAlpha = 0;
                this->newFileNameCharCount = 0;
                this->nameEntryBoxPosX = 120;
                this->nameEntryBoxAlpha = 0;
                memcpy(Save_GetSaveMetaInfo(this->buttonIndex)->playerName, &emptyName, 8);
            } else if(!FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(this->buttonIndex))) {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }  
            else {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->actionTimer = 8;
                this->selectMode = SM_FADE_MAIN_TO_SELECT;
                this->selectedFileIndex = this->buttonIndex;
                this->menuMode = FS_MENU_MODE_SELECT;
                this->nextTitleLabel = FS_TITLE_OPEN_FILE;
            }
        } else {
            if (this->warningLabel == FS_WARNING_NONE) {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->prevConfigMode = this->configMode;

                if (this->buttonIndex == FS_BTN_MAIN_COPY) {
                    this->configMode = CM_SETUP_COPY_SOURCE;
                    this->nextTitleLabel = FS_TITLE_COPY_FROM;
                } else if (this->buttonIndex == FS_BTN_MAIN_ERASE) {
                    this->configMode = CM_SETUP_ERASE_SELECT;
                    this->nextTitleLabel = FS_TITLE_ERASE_FILE;
                } else {
                    this->configMode = CM_MAIN_TO_OPTIONS;
                    this->kbdButton = 0;
                    this->kbdX = 0;
                    this->kbdY = 0;
                    this->charBgAlpha = 0;
                    this->newFileNameCharCount = 0;
                    this->nameEntryBoxPosX = 120;
                }

                this->actionTimer = 8;
            } else {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        }
    } else {
        if ((ABS(this->stickRelY) > 30) || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN | BTN_DUP))) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

            if ((this->stickRelY > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DUP))) {
                this->buttonIndex--;
                if (this->buttonIndex < FS_BTN_MAIN_FILE_1) {
                    this->buttonIndex = FS_BTN_MAIN_OPTIONS;
                }
            } else {
                this->buttonIndex++;
                if (this->buttonIndex > FS_BTN_MAIN_OPTIONS) {
                    this->buttonIndex = FS_BTN_MAIN_FILE_1;
                }
            }
        }

        if (this->buttonIndex == FS_BTN_MAIN_COPY) {
            if (!Save_GetSaveMetaInfo(0)->valid && !Save_GetSaveMetaInfo(1)->valid && !Save_GetSaveMetaInfo(2)->valid) {
                this->warningButtonIndex = this->buttonIndex;
                this->warningLabel = FS_WARNING_NO_FILE_COPY;
                this->emptyFileTextAlpha = 255;
            } else if (Save_GetSaveMetaInfo(0)->valid && Save_GetSaveMetaInfo(1)->valid &&
                       Save_GetSaveMetaInfo(2)->valid) {
                this->warningButtonIndex = this->buttonIndex;
                this->warningLabel = FS_WARNING_NO_EMPTY_FILES;
                this->emptyFileTextAlpha = 255;
            } else {
                this->warningLabel = FS_WARNING_NONE;
            }
        } else if (this->buttonIndex == FS_BTN_MAIN_ERASE) {
            if (!Save_GetSaveMetaInfo(0)->valid && !Save_GetSaveMetaInfo(1)->valid && !Save_GetSaveMetaInfo(2)->valid) {
                this->warningButtonIndex = this->buttonIndex;
                this->warningLabel = FS_WARNING_NO_FILE_ERASE;
                this->emptyFileTextAlpha = 255;
            } else {
                this->warningLabel = FS_WARNING_NONE;
            }
        } else {
            this->warningLabel = FS_WARNING_NONE;
        }
    }
}

/**
 * Update function for `CM_UNUSED_31`
 */
void FileChoose_UnusedCM31(GameState* thisx) {
}

/**
 * Delay the next config mode from running until `XREG(73)` reaches 254.
 * Because the timer increments by 2, the delay is 127 frames (assuming the value was not changed by reg editor).
 * Unused in the final game, was possibly used for debugging.
 * Update function for `CM_UNUSED_DELAY`
 */
void FileChoose_UnusedCMDelay(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    XREG(73) += 2;

    if (XREG(73) == 254) {
        this->configMode = this->nextConfigMode;
        XREG(73) = 0;
    }
}

/**
 * Rotate the window from the main menu to the name entry menu.
 * Update function for `CM_ROTATE_TO_NAME_ENTRY`
 */
void FileChoose_RotateToNameEntry(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->windowRot += VREG(16);

    if (this->windowRot >= 314.0f) {
        this->windowRot = 314.0f;
        this->configMode = CM_START_NAME_ENTRY;
    }
}

/**
 * Rotate the window from the main menu to the options menu.
 * Update function for `CM_MAIN_TO_OPTIONS`
 */
void FileChoose_RotateToOptions(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->windowRot += VREG(16);

    if (this->windowRot >= 314.0f) {
        this->windowRot = 314.0f;
        this->configMode = CM_START_OPTIONS;
    }
}

/**
 * Rotate the window from the options menu to the main menu.
 * Update function for `CM_NAME_ENTRY_TO_MAIN` and `CM_OPTIONS_TO_MAIN`
 */
void FileChoose_RotateToMain(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->windowRot += VREG(16);

    if (this->windowRot >= 628.0f) {
        this->windowRot = 0.0f;
        this->configMode = CM_MAIN_MENU;
    }
}

static void (*gConfigModeUpdateFuncs[])(GameState*) = {
    FileChoose_StartFadeIn,        FileChoose_FinishFadeIn,
    FileChoose_UpdateMainMenu,     FileChoose_SetupCopySource,
    FileChoose_SelectCopySource,   FileChoose_SetupCopyDest1,
    FileChoose_SetupCopyDest2,     FileChoose_SelectCopyDest,
    FileChoose_ExitToCopySource1,  FileChoose_ExitToCopySource2,
    FileChoose_SetupCopyConfirm1,  FileChoose_SetupCopyConfirm2,
    FileChoose_CopyConfirm,        FileChoose_ReturnToCopyDest,
    FileChoose_CopyAnim1,          FileChoose_CopyAnim2,
    FileChoose_CopyAnim3,          FileChoose_CopyAnim4,
    FileChoose_CopyAnim5,          FileChoose_ExitCopyToMain,
    FileChoose_SetupEraseSelect,   FileChoose_EraseSelect,
    FileChoose_SetupEraseConfirm1, FileChoose_SetupEraseConfirm2,
    FileChoose_EraseConfirm,       FileChoose_ExitToEraseSelect1,
    FileChoose_ExitToEraseSelect2, FileChoose_EraseAnim1,
    FileChoose_EraseAnim2,         FileChoose_EraseAnim3,
    FileChoose_ExitEraseToMain,    FileChoose_UnusedCM31,
    FileChoose_RotateToNameEntry,  FileChoose_UpdateKeyboardCursor,
    FileChoose_StartNameEntry,     FileChoose_RotateToMain,
    FileChoose_RotateToOptions,    FileChoose_UpdateOptionsMenu,
    FileChoose_StartOptions,       FileChoose_RotateToMain,
    FileChoose_UnusedCMDelay,
};

/**
 * Updates the alpha of the cursor to make it pulsate.
 * On the debug rom, this function also handles switching languages with controller 3.
 */
void FileChoose_PulsateCursor(GameState* thisx) {
    static s16 cursorAlphaTargets[] = { 70, 200 };
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 alphaStep;

    alphaStep = ABS(this->highlightColor[3] - cursorAlphaTargets[this->highlightPulseDir]) / XREG(35);

    if (this->highlightColor[3] >= cursorAlphaTargets[this->highlightPulseDir]) {
        this->highlightColor[3] -= alphaStep;
    } else {
        this->highlightColor[3] += alphaStep;
    }

    XREG(35)--;

    if (XREG(35) == 0) {
        this->highlightColor[3] = cursorAlphaTargets[this->highlightPulseDir];
        XREG(35) = XREG(36 + this->highlightPulseDir);
        this->highlightPulseDir ^= 1;
    }
}

void FileChoose_ConfigModeUpdate(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    gConfigModeUpdateFuncs[this->configMode](&this->state);
}

void FileChoose_SetWindowVtx(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 i;
    s16 j;
    s16 x;
    s16 tmp;
    s16 tmp2;
    s16 tmp3;

    this->windowVtx = Graph_Alloc(this->state.gfxCtx, sizeof(Vtx) * 80);
    tmp = this->windowPosX - 90;

    for (x = 0, i = 0; i < 4; i++) {
        tmp += 0x40;
        tmp2 = (i == 3) ? 0x30 : 0x40;

        for (j = 0, tmp3 = 0x50; j < 5; j++, x += 4, tmp3 -= 0x20) {
            this->windowVtx[x].v.ob[0] = this->windowVtx[x + 2].v.ob[0] = tmp;

            this->windowVtx[x + 1].v.ob[0] = this->windowVtx[x + 3].v.ob[0] = tmp + tmp2;

            this->windowVtx[x].v.ob[1] = this->windowVtx[x + 1].v.ob[1] = tmp3;

            this->windowVtx[x + 2].v.ob[1] = this->windowVtx[x + 3].v.ob[1] = tmp3 - 0x20;

            this->windowVtx[x].v.ob[2] = this->windowVtx[x + 1].v.ob[2] = this->windowVtx[x + 2].v.ob[2] =
                this->windowVtx[x + 3].v.ob[2] = 0;

            this->windowVtx[x].v.flag = this->windowVtx[x + 1].v.flag = this->windowVtx[x + 2].v.flag =
                this->windowVtx[x + 3].v.flag = 0;

            this->windowVtx[x].v.tc[0] = this->windowVtx[x].v.tc[1] = this->windowVtx[x + 1].v.tc[1] =
                this->windowVtx[x + 2].v.tc[0] = 0;

            this->windowVtx[x + 1].v.tc[0] = this->windowVtx[x + 3].v.tc[0] = tmp2 * 0x20;

            this->windowVtx[x + 2].v.tc[1] = this->windowVtx[x + 3].v.tc[1] = 0x400;

            this->windowVtx[x].v.cn[0] = this->windowVtx[x + 2].v.cn[0] = this->windowVtx[x].v.cn[1] =
                this->windowVtx[x + 2].v.cn[1] = this->windowVtx[x].v.cn[2] = this->windowVtx[x + 2].v.cn[2] =
                    this->windowVtx[x + 1].v.cn[0] = this->windowVtx[x + 3].v.cn[0] = this->windowVtx[x + 1].v.cn[1] =
                        this->windowVtx[x + 3].v.cn[1] = this->windowVtx[x + 1].v.cn[2] =
                            this->windowVtx[x + 3].v.cn[2] = this->windowVtx[x].v.cn[3] =
                                this->windowVtx[x + 2].v.cn[3] = this->windowVtx[x + 1].v.cn[3] =
                                    this->windowVtx[x + 3].v.cn[3] = 255;
        }
    }
}

static s16 D_80812818[] = { 0x001A, 0x000A, 0x000A, 0x000A };
static s16 D_80812820[] = { 0x0020, 0x000C, 0x000C, 0x000C };
static s16 D_80812828[] = { 0x0010, 0x000C, 0x000C, 0x000C };
static s16 D_80812830[] = { 0x0040, 0x0054, 0x0068, 0x0274, 0x0278, 0x027C };
static s16 D_8081283C[] = { 0x0040, 0x0054, 0x0068, 0x0278 };
static s16 D_80812844[] = { 0x0274, 0x0278 };
static s16 D_80812848[] = { 0x0274, 0x0278 };

void FileChoose_SetWindowContentVtx(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 phi_t2;
    s16 phi_t0;
    s16 phi_t5;
    s16 phi_a1;
    s16 phi_ra;
    s16 temp_t1;

    this->windowContentVtx = Graph_Alloc(this->state.gfxCtx, 0x288 * sizeof(Vtx));

    for (phi_t2 = 0; phi_t2 < 0x288; phi_t2 += 4) {
        this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = 0x12C;
        this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
            this->windowContentVtx[phi_t2].v.ob[0] + 0x10;

        this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] = 0;
        this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
            this->windowContentVtx[phi_t2].v.ob[1] - 0x10;

        this->windowContentVtx[phi_t2].v.ob[2] = this->windowContentVtx[phi_t2 + 1].v.ob[2] =
            this->windowContentVtx[phi_t2 + 2].v.ob[2] = this->windowContentVtx[phi_t2 + 3].v.ob[2] = 0;

        this->windowContentVtx[phi_t2].v.flag = this->windowContentVtx[phi_t2 + 1].v.flag =
            this->windowContentVtx[phi_t2 + 2].v.flag = this->windowContentVtx[phi_t2 + 3].v.flag = 0;

        this->windowContentVtx[phi_t2].v.tc[0] = this->windowContentVtx[phi_t2].v.tc[1] =
            this->windowContentVtx[phi_t2 + 1].v.tc[1] = this->windowContentVtx[phi_t2 + 2].v.tc[0] = 0;

        this->windowContentVtx[phi_t2 + 1].v.tc[0] = this->windowContentVtx[phi_t2 + 2].v.tc[1] =
            this->windowContentVtx[phi_t2 + 3].v.tc[0] = this->windowContentVtx[phi_t2 + 3].v.tc[1] = 0x200;

        this->windowContentVtx[phi_t2].v.cn[0] = this->windowContentVtx[phi_t2 + 1].v.cn[0] =
            this->windowContentVtx[phi_t2 + 2].v.cn[0] = this->windowContentVtx[phi_t2 + 3].v.cn[0] =
                this->windowContentVtx[phi_t2].v.cn[1] = this->windowContentVtx[phi_t2 + 1].v.cn[1] =
                    this->windowContentVtx[phi_t2 + 2].v.cn[1] = this->windowContentVtx[phi_t2 + 3].v.cn[1] =
                        this->windowContentVtx[phi_t2].v.cn[2] = this->windowContentVtx[phi_t2 + 1].v.cn[2] =
                            this->windowContentVtx[phi_t2 + 2].v.cn[2] = this->windowContentVtx[phi_t2 + 3].v.cn[2] =
                                this->windowContentVtx[phi_t2].v.cn[3] = this->windowContentVtx[phi_t2 + 1].v.cn[3] =
                                    this->windowContentVtx[phi_t2 + 2].v.cn[3] =
                                        this->windowContentVtx[phi_t2 + 3].v.cn[3] = 0xFF;
    }

    this->windowContentVtx[0].v.ob[0] = this->windowContentVtx[2].v.ob[0] = this->windowPosX;
    this->windowContentVtx[1].v.ob[0] = this->windowContentVtx[3].v.ob[0] = this->windowContentVtx[0].v.ob[0] + 0x80;
    this->windowContentVtx[0].v.ob[1] = this->windowContentVtx[1].v.ob[1] = 0x48;
    this->windowContentVtx[2].v.ob[1] = this->windowContentVtx[3].v.ob[1] = this->windowContentVtx[0].v.ob[1] - 0x10;
    this->windowContentVtx[1].v.tc[0] = this->windowContentVtx[3].v.tc[0] = 0x1000;

    for (phi_a1 = 0, phi_t2 = 4; phi_a1 < 3; phi_a1++) {
        phi_t0 = this->windowPosX - 6;

        for (phi_t5 = 0; phi_t5 < 5; phi_t5++, phi_t2 += 4) {
            this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
            this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
                this->windowContentVtx[phi_t2].v.ob[0] + sFileInfoBoxPartWidths[phi_t5];

            this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] =
                this->fileNamesY[phi_a1] + 0x2C;

            this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
                this->windowContentVtx[phi_t2].v.ob[1] - 0x38;

            this->windowContentVtx[phi_t2 + 1].v.tc[0] = this->windowContentVtx[phi_t2 + 3].v.tc[0] =
                sFileInfoBoxPartWidths[phi_t5] << 5;
            this->windowContentVtx[phi_t2 + 2].v.tc[1] = this->windowContentVtx[phi_t2 + 3].v.tc[1] = 0x700;
            phi_t0 += sFileInfoBoxPartWidths[phi_t5];
        }
    }

    phi_t0 = this->windowPosX - 6;
    phi_ra = 0x2C;

    for (phi_t5 = 0; phi_t5 < 3; phi_t5++, phi_t2 += 20, phi_ra -= 0x10) {
        this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;

        this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
            this->windowContentVtx[phi_t2].v.ob[0] + 0x40;

        this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] =
            this->buttonYOffsets[phi_t5] + phi_ra;
        this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
            this->windowContentVtx[phi_t2].v.ob[1] - 0x10;

        this->windowContentVtx[phi_t2 + 1].v.tc[0] = this->windowContentVtx[phi_t2 + 3].v.tc[0] = 0x800;

        this->windowContentVtx[phi_t2 + 4].v.ob[0] = this->windowContentVtx[phi_t2 + 6].v.ob[0] = phi_t0 + 0x40;

        this->windowContentVtx[phi_t2 + 5].v.ob[0] = this->windowContentVtx[phi_t2 + 7].v.ob[0] =
            this->windowContentVtx[phi_t2 + 4].v.ob[0] + 0x6C;

        this->windowContentVtx[phi_t2 + 4].v.ob[1] = this->windowContentVtx[phi_t2 + 5].v.ob[1] =
            this->buttonYOffsets[phi_t5] + phi_ra;

        this->windowContentVtx[phi_t2 + 6].v.ob[1] = this->windowContentVtx[phi_t2 + 7].v.ob[1] =
            this->windowContentVtx[phi_t2 + 4].v.ob[1] - 0x10;

        this->windowContentVtx[phi_t2 + 5].v.tc[0] = this->windowContentVtx[phi_t2 + 7].v.tc[0] = 0xD80;

        if ((this->configMode == CM_COPY_ANIM_2) && (phi_t5 == this->copyDestFileIndex)) {
            temp_t1 = this->fileNamesY[phi_t5] + 0x2C;
        } else if (((this->configMode == CM_COPY_ANIM_3) || (this->configMode == CM_COPY_ANIM_4)) &&
                   (phi_t5 == this->copyDestFileIndex)) {
            temp_t1 = this->buttonYOffsets[phi_t5] + phi_ra;
        } else {
            temp_t1 = phi_ra + this->buttonYOffsets[phi_t5] + this->fileNamesY[phi_t5];
        }

        this->windowContentVtx[phi_t2 + 8].v.ob[0] = this->windowContentVtx[phi_t2 + 10].v.ob[0] = phi_t0 + 0xA8;

        this->windowContentVtx[phi_t2 + 9].v.ob[0] = this->windowContentVtx[phi_t2 + 11].v.ob[0] =
            this->windowContentVtx[phi_t2 + 8].v.ob[0] + 0x2C;

        this->windowContentVtx[phi_t2 + 8].v.ob[1] = this->windowContentVtx[phi_t2 + 9].v.ob[1] = temp_t1;
        this->windowContentVtx[phi_t2 + 10].v.ob[1] = this->windowContentVtx[phi_t2 + 11].v.ob[1] =
            this->windowContentVtx[phi_t2 + 8].v.ob[1] - 0x10;

        this->windowContentVtx[phi_t2 + 9].v.tc[0] = this->windowContentVtx[phi_t2 + 11].v.tc[0] = 0x580;
        this->windowContentVtx[phi_t2 + 12].v.ob[0] = this->windowContentVtx[phi_t2 + 14].v.ob[0] = phi_t0 + 0x34;
        this->windowContentVtx[phi_t2 + 13].v.ob[0] = this->windowContentVtx[phi_t2 + 15].v.ob[0] =
            this->windowContentVtx[phi_t2 + 12].v.ob[0] + 0x18;

        this->windowContentVtx[phi_t2 + 12].v.ob[1] = this->windowContentVtx[phi_t2 + 13].v.ob[1] =
            this->buttonYOffsets[phi_t5] + phi_ra;

        this->windowContentVtx[phi_t2 + 14].v.ob[1] = this->windowContentVtx[phi_t2 + 15].v.ob[1] =
            this->windowContentVtx[phi_t2 + 12].v.ob[1] - 0x10;

        this->windowContentVtx[phi_t2 + 13].v.tc[0] = this->windowContentVtx[phi_t2 + 15].v.tc[0] = 0x300;
        this->windowContentVtx[phi_t2 + 16].v.ob[0] = this->windowContentVtx[phi_t2 + 18].v.ob[0] = phi_t0 + 0x9C;
        this->windowContentVtx[phi_t2 + 17].v.ob[0] = this->windowContentVtx[phi_t2 + 19].v.ob[0] =
            this->windowContentVtx[phi_t2 + 16].v.ob[0] + 0x18;

        this->windowContentVtx[phi_t2 + 16].v.ob[1] = this->windowContentVtx[phi_t2 + 17].v.ob[1] =
            this->buttonYOffsets[phi_t5] + phi_ra;

        this->windowContentVtx[phi_t2 + 18].v.ob[1] = this->windowContentVtx[phi_t2 + 19].v.ob[1] =
            this->windowContentVtx[phi_t2 + 16].v.ob[1] - 0x10;

        this->windowContentVtx[phi_t2 + 17].v.tc[0] = this->windowContentVtx[phi_t2 + 19].v.tc[0] = 0x300;
    }

    phi_ra = 0x2C;

    for (phi_t5 = 0; phi_t5 < 3; phi_t5++, phi_ra -= WREG(38)) {
        if (Save_GetSaveMetaInfo(phi_t5)->valid) {
            phi_t0 = this->windowPosX - WREG(39);

            if ((this->configMode == 0xF) && (phi_t5 == this->copyDestFileIndex)) {
                temp_t1 = this->fileNamesY[phi_t5] + 0x2C;
            } else if (((this->configMode == CM_COPY_ANIM_3) || (this->configMode == CM_COPY_ANIM_4)) &&
                       (phi_t5 == this->copyDestFileIndex)) {
                temp_t1 = this->buttonYOffsets[phi_t5] + phi_ra;
            } else {
                temp_t1 = phi_ra + this->buttonYOffsets[phi_t5] + this->fileNamesY[phi_t5];
            }

            temp_t1 += 2;

            for (phi_a1 = 0; phi_a1 < 8; phi_a1++, phi_t2 += 4, phi_t0 += WREG(40)) {
                this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] =
                    WREG(41) + phi_t0 + 0x40;
                this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
                    this->windowContentVtx[phi_t2].v.ob[0] + WREG(42);
                this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] = temp_t1 - 3;
                this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
                    this->windowContentVtx[phi_t2].v.ob[1] - WREG(43);
            }

            phi_t0 = this->windowPosX - 14;
            temp_t1 -= 0x16;

            for (phi_a1 = 0; phi_a1 < 4; phi_a1++, phi_t2 += 4) {
                this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
                this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
                    this->windowContentVtx[phi_t2].v.ob[0] + D_80812820[phi_a1];
                this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] = temp_t1;
                this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
                    this->windowContentVtx[phi_t2].v.ob[1] - D_80812828[phi_a1];
                phi_t0 += D_80812818[phi_a1];
            }

            this->windowContentVtx[phi_t2 - 15].v.tc[0] = this->windowContentVtx[phi_t2 - 13].v.tc[0] = 0x400;

            phi_t0 = this->windowPosX + 63;
            temp_t1 += 4;

            for (phi_a1 = 0; phi_a1 < 20; phi_a1++, phi_t2 += 4, phi_t0 += 9) {
                this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
                this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
                    this->windowContentVtx[phi_t2].v.ob[0] + 0xA;
                this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] = temp_t1;
                this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
                    this->windowContentVtx[phi_t2].v.ob[1] - 0xA;

                if (phi_a1 == 9) {
                    phi_t0 = this->windowPosX + 54;
                    temp_t1 -= 8;
                }
            }

            phi_t0 = this->windowPosX + 4;
            temp_t1 -= 0xA;

            for (phi_a1 = 0; phi_a1 < 10; phi_a1++, phi_t2 += 4, phi_t0 += 0x10) {
                this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
                this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
                    this->windowContentVtx[phi_t2].v.ob[0] + 0x10;
                this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] = temp_t1;
                this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
                    this->windowContentVtx[phi_t2].v.ob[1] - 0x10;
            }
        } else {
            phi_t2 += 0xA8;
        }
    }

    phi_t0 = this->windowPosX - 6;
    phi_ra = -0xC;

    for (phi_t5 = 0; phi_t5 < 2; phi_t5++, phi_t2 += 4, phi_ra -= 0x10) {
        this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
        this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
            this->windowContentVtx[phi_t2].v.ob[0] + 0x40;
        this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] =
            this->buttonYOffsets[phi_t5 + 3] + phi_ra;
        this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
            this->windowContentVtx[phi_t2].v.ob[1] - 0x10;
        this->windowContentVtx[phi_t2 + 1].v.tc[0] = this->windowContentVtx[phi_t2 + 3].v.tc[0] = 0x800;
    }

    this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
    this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
        this->windowContentVtx[phi_t2].v.ob[0] + 0x40;
    this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] =
        this->buttonYOffsets[5] - 0x34;
    this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
        this->windowContentVtx[phi_t2].v.ob[1] - 0x10;
    this->windowContentVtx[phi_t2 + 1].v.tc[0] = this->windowContentVtx[phi_t2 + 3].v.tc[0] = 0x800;

    phi_t2 += 4;

    if (((this->menuMode == FS_MENU_MODE_CONFIG) && (this->configMode >= CM_MAIN_MENU)) ||
        ((this->menuMode == FS_MENU_MODE_SELECT) && (this->selectMode == SM_CONFIRM_FILE))) {
        if (this->menuMode == FS_MENU_MODE_CONFIG) {
            if ((this->configMode == CM_SELECT_COPY_SOURCE) || (this->configMode == CM_SELECT_COPY_DEST) ||
                (this->configMode == CM_ERASE_SELECT)) {
                phi_t5 = D_8081283C[this->buttonIndex];
            } else if ((this->configMode == CM_ERASE_CONFIRM) || (this->configMode == CM_COPY_CONFIRM)) {
                phi_t5 = D_80812844[this->buttonIndex];
            } else {
                phi_t5 = D_80812830[this->buttonIndex];
            }
        } else {
            phi_t5 = D_80812848[this->confirmButtonIndex];
        }

        this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = this->windowPosX - 0xA;
        this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
            this->windowContentVtx[phi_t2].v.ob[0] + 0x48;
        this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] =
            this->windowContentVtx[phi_t5].v.ob[1] + 4;
        this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
            this->windowContentVtx[phi_t2].v.ob[1] - 0x18;
        this->windowContentVtx[phi_t2 + 1].v.tc[0] = this->windowContentVtx[phi_t2 + 3].v.tc[0] = 0x900;
        this->windowContentVtx[phi_t2 + 2].v.tc[1] = this->windowContentVtx[phi_t2 + 3].v.tc[1] = 0x300;
    }

    this->windowContentVtx[phi_t2 + 4].v.ob[0] = this->windowContentVtx[phi_t2 + 6].v.ob[0] = this->windowPosX + 0x3A;
    this->windowContentVtx[phi_t2 + 5].v.ob[0] = this->windowContentVtx[phi_t2 + 7].v.ob[0] =
        this->windowContentVtx[phi_t2 + 4].v.ob[0] + 0x80;
    this->windowContentVtx[phi_t2 + 4].v.ob[1] = this->windowContentVtx[phi_t2 + 5].v.ob[1] =
        this->windowContentVtx[D_80812830[this->warningButtonIndex]].v.ob[1];
    this->windowContentVtx[phi_t2 + 6].v.ob[1] = this->windowContentVtx[phi_t2 + 7].v.ob[1] =
        this->windowContentVtx[phi_t2 + 4].v.ob[1] - 0x10;
    this->windowContentVtx[phi_t2 + 5].v.tc[0] = this->windowContentVtx[phi_t2 + 7].v.tc[0] = 0x1000;
}

static u16 D_8081284C[] = { 0x007C, 0x0124, 0x01CC };

static void* sQuestItemTextures[] = {
    gFileSelKokiriEmeraldTex,   gFileSelGoronRubyTex,       gFileSelZoraSapphireTex,
    gFileSelForestMedallionTex, gFileSelFireMedallionTex,   gFileSelWaterMedallionTex,
    gFileSelSpiritMedallionTex, gFileSelShadowMedallionTex, gFileSelLightMedallionTex,
};

static s16 sQuestItemRed[] = { 255, 255, 255, 0, 255, 0, 255, 200, 200 };
static s16 sQuestItemGreen[] = { 255, 255, 255, 255, 60, 100, 130, 50, 200 };
static s16 sQuestItemBlue[] = { 255, 255, 255, 0, 0, 255, 0, 255, 0 };
static s16 sQuestItemFlags[] = { 0x0012, 0x0013, 0x0014, 0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005 };
static s16 sNamePrimColors[2][3] = { { 255, 255, 255 }, { 100, 100, 100 } };
static void* sHeartTextures[] = { gHeartFullTex, gDefenseHeartFullTex };
static s16 sHeartPrimColors[2][3] = { { 255, 70, 50 }, { 200, 0, 0 } };
static s16 sHeartEnvColors[2][3] = { { 50, 40, 60 }, { 255, 255, 255 } };

void FileChoose_DrawFileInfo(GameState* thisx, s16 fileIndex, s16 isActive) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    Font* sp54 = &this->font;
    s32 heartType;
    s16 i;
    s16 vtxOffset;
    s16 j;
    s16 deathCountSplit[3];

    OPEN_DISPS(this->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);

    // draw file name
    if (this->nameAlpha[fileIndex] != 0) {
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_8081284C[fileIndex]], 32, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, sNamePrimColors[isActive][0], sNamePrimColors[isActive][1],
                        sNamePrimColors[isActive][2], this->nameAlpha[fileIndex]);

        for (i = 0, vtxOffset = 0; vtxOffset < 0x20; i++, vtxOffset += 4) {
            FileChoose_DrawCharacter(
                this->state.gfxCtx, sp54->fontBuf + Save_GetSaveMetaInfo(fileIndex)->playerName[i] * FONT_CHAR_TEX_SIZE,
                vtxOffset);
        }
    }

    if ((fileIndex == this->selectedFileIndex) || (fileIndex == this->copyDestFileIndex)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_8081284C[fileIndex]] + 0x24, 12, 0);

        FileChoose_SplitNumber(Save_GetSaveMetaInfo(fileIndex)->deaths, &deathCountSplit[0], &deathCountSplit[1],
                               &deathCountSplit[2]);

        // draw death count
        for (i = 0, vtxOffset = 0; i < 3; i++, vtxOffset += 4) {
            FileChoose_DrawCharacter(this->state.gfxCtx, sp54->fontBuf + deathCountSplit[i] * FONT_CHAR_TEX_SIZE,
                                     vtxOffset);
        }

        gDPPipeSync(POLY_OPA_DISP++);

        heartType = (Save_GetSaveMetaInfo(fileIndex)->defense == 0) ? 0 : 1;

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, sHeartPrimColors[heartType][0], sHeartPrimColors[heartType][1],
                        sHeartPrimColors[heartType][2], this->fileInfoAlpha[fileIndex]);
        gDPSetEnvColor(POLY_OPA_DISP++, sHeartEnvColors[heartType][0], sHeartEnvColors[heartType][1],
                       sHeartEnvColors[heartType][2], 255);

        i = Save_GetSaveMetaInfo(fileIndex)->healthCapacity / 0x10;

        // draw hearts
        for (vtxOffset = 0, j = 0; j < i; j++, vtxOffset += 4) {
            gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_8081284C[fileIndex] + vtxOffset] + 0x30, 4, 0);

            POLY_OPA_DISP = FileChoose_QuadTextureIA8(POLY_OPA_DISP, sHeartTextures[heartType], 0x10, 0x10, 0);
        }

        gDPPipeSync(POLY_OPA_DISP++);

        // draw quest items
        for (vtxOffset = 0, j = 0; j < 9; j++, vtxOffset += 4) {
            if (Save_GetSaveMetaInfo(fileIndex)->questItems & gBitFlags[sQuestItemFlags[j]]) {
                gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_8081284C[fileIndex] + vtxOffset] + 0x80, 4, 0);
                gDPPipeSync(POLY_OPA_DISP++);
                gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, sQuestItemRed[j], sQuestItemGreen[j], sQuestItemBlue[j],
                                this->fileInfoAlpha[fileIndex]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

                if (j < 3) {
                    gDPLoadTextureBlock(POLY_OPA_DISP++, sQuestItemTextures[j], G_IM_FMT_RGBA, G_IM_SIZ_32b, 16, 16, 0,
                                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP,
                                        G_TX_NOMASK, G_TX_NOLOD);
                    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

                } else {
                    POLY_OPA_DISP = FileChoose_QuadTextureIA8(POLY_OPA_DISP, sQuestItemTextures[j], 0x10, 0x10, 0);
                }
            }
        }
    }

    CLOSE_DISPS(this->state.gfxCtx);
}

static void* sFileInfoBoxTextures[] = {
    gFileSelFileInfoBox1Tex, gFileSelFileInfoBox2Tex, gFileSelFileInfoBox3Tex,
    gFileSelFileInfoBox4Tex, gFileSelFileInfoBox5Tex,
};

static void* sTitleLabels[3][9] = {
    { gFileSelPleaseSelectAFileENGTex, gFileSelOpenThisFileENGTex, gFileSelCopyWhichFileENGTex,
      gFileSelCopyToWhichFileENGTex, gFileSelAreYouSureENGTex, gFileSelFileCopiedENGTex, gFileSelEraseWhichFileENGTex,
      gFileSelAreYouSure2ENGTex, gFileSelFileErasedENGTex },
    { gFileSelPleaseSelectAFileGERTex, gFileSelOpenThisFileGERTex, gFileSelWhichFile1GERTex,
      gFileSelCopyToWhichFileGERTex, gFileSelAreYouSureGERTex, gFileSelFileCopiedGERTex, gFileSelWhichFile2GERTex,
      gFileSelAreYouSure2GERTex, gFileSelFileErasedGERTex },
    { gFileSelPleaseSelectAFileFRATex, gFileSelOpenThisFileFRATex, gFileSelCopyWhichFileFRATex,
      gFileSelCopyToWhichFileFRATex, gFileSelAreYouSureFRATex, gFileSelFileCopiedFRATex, gFileSelEraseWhichFileFRATex,
      gFileSelAreYouSure2FRATex, gFileSelFileErasedFRATex }
};

static void* sWarningLabels[3][5] = {
    { gFileSelNoFileToCopyENGTex, gFileSelNoFileToEraseENGTex, gFileSelNoEmptyFileENGTex, gFileSelFileEmptyENGTex,
      gFileSelFileInUseENGTex },
    { gFileSelNoFileToCopyGERTex, gFileSelNoFileToEraseGERTex, gFileSelNoEmptyFileGERTex, gFileSelFileEmptyGERTex,
      gFileSelFileInUseGERTex },
    { gFileSelNoFileToCopyFRATex, gFileSelNoFileToEraseFRATex, gFileSelNoEmptyFileFRATex, gFileSelFileEmptyFRATex,
      gFileSelFileInUseFRATex },
};

static void* sFileButtonTextures[3][3] = {
    { gFileSelFile1ButtonENGTex, gFileSelFile2ButtonENGTex, gFileSelFile3ButtonENGTex },
    { gFileSelFile1ButtonGERTex, gFileSelFile2ButtonGERTex, gFileSelFile3ButtonGERTex },
    { gFileSelFile1ButtonFRATex, gFileSelFile2ButtonFRATex, gFileSelFile3ButtonFRATex },
};

static void* sActionButtonTextures[3][4] = {
    { gFileSelCopyButtonENGTex, gFileSelEraseButtonENGTex, gFileSelYesButtonENGTex, gFileSelQuitButtonENGTex },
    { gFileSelCopyButtonGERTex, gFileSelEraseButtonGERTex, gFileSelYesButtonGERTex, gFileSelQuitButtonGERTex },
    { gFileSelCopyButtonFRATex, gFileSelEraseButtonFRATex, gFileSelYesButtonFRATex, gFileSelQuitButtonFRATex },
};

static void* sOptionsButtonTextures[] = {
    gFileSelOptionsButtonENGTex,
    gFileSelOptionsButtonGERTex,
    gFileSelOptionsButtonENGTex,
};

/**
 * Draw most window contents including buttons, labels, and icons.
 * Does not include anything from the keyboard and settings windows.
 */
void FileChoose_DrawWindowContents(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 fileIndex;
    s16 temp;
    s16 i;
    s16 quadVtxIndex;
    s16 isActive;
    s16 pad;
    Color_RGB8 Background_Color = { this->windowColor[0], this->windowColor[1], this->windowColor[2] };

    OPEN_DISPS(this->state.gfxCtx);

    // draw title label
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    gSPVertex(POLY_OPA_DISP++, this->windowContentVtx, 4, 0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, sTitleLabels[gSaveContext.language][this->titleLabel], G_IM_FMT_IA,
                        G_IM_SIZ_8b, 128, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    // draw next title label
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[1]);
    gDPLoadTextureBlock(POLY_OPA_DISP++, sTitleLabels[gSaveContext.language][this->nextTitleLabel], G_IM_FMT_IA,
                        G_IM_SIZ_8b, 128, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    temp = 4;

    gDPPipeSync(POLY_OPA_DISP++);

    // draw file info box (large box when a file is selected)
    for (fileIndex = 0; fileIndex < 3; fileIndex++, temp += 20) {
        gDPPipeSync(POLY_OPA_DISP++);

        if (CVar_GetS32("gHudColors", 1) == 2) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->fileInfoAlpha[fileIndex]);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, Background_Color.r, Background_Color.g, Background_Color.b, this->fileInfoAlpha[fileIndex]);
        }
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[temp], 20, 0);

        for (quadVtxIndex = 0, i = 0; i < 5; i++, quadVtxIndex += 4) {
            gDPLoadTextureBlock(POLY_OPA_DISP++, sFileInfoBoxTextures[i], G_IM_FMT_IA, G_IM_SIZ_16b,
                                sFileInfoBoxPartWidths[i], 56, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
        }
    }

    for (i = 0; i < 3; i++, temp += 20) {
        // draw file button
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[temp], 20, 0);

        isActive = 0;

        if (!FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i)) && Save_GetSaveMetaInfo(i)->valid) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[1][0], sWindowContentColors[1][1],
                            sWindowContentColors[1][2], this->fileButtonAlpha[i]);
        } else if (CVar_GetS32("gHudColors", 1) == 2) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r,
                            CVar_GetRGB("gCCFileChoosePrim", Background_Color).g,
                            CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->fileButtonAlpha[i]);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0], sWindowContentColors[isActive][1],
                            sWindowContentColors[isActive][2], this->fileButtonAlpha[i]);
        }

        gDPLoadTextureBlock(POLY_OPA_DISP++, sFileButtonTextures[gSaveContext.language][i], G_IM_FMT_IA, G_IM_SIZ_16b,
                            64, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

        // draw file name box
        if (CVar_GetS32("gHudColors", 1) == 2 && FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i))) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->nameBoxAlpha[i]);
        } else if (!FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i))) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[1][0], sWindowContentColors[1][1],
                            sWindowContentColors[1][2], this->nameBoxAlpha[i]);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0], sWindowContentColors[isActive][1],
                            sWindowContentColors[isActive][2], this->nameBoxAlpha[i]);
        }

        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelNameBoxTex, G_IM_FMT_IA, G_IM_SIZ_16b, 108, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);

        // draw rando label
        if (Save_GetSaveMetaInfo(i)->randoSave) {
            if (CVar_GetS32("gHudColors", 1) == 2 && FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i))) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->nameAlpha[i]);
            } else if (!FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i))) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[1][0], sWindowContentColors[1][1],
                                sWindowContentColors[1][2], this->nameBoxAlpha[i]);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0],
                                sWindowContentColors[isActive][1], sWindowContentColors[isActive][2],
                                this->nameAlpha[i]);
            }
            gDPLoadTextureBlock(POLY_OPA_DISP++, ResourceMgr_LoadFileRaw("assets/textures/title_static/gFileSelRANDButtonTex"), G_IM_FMT_IA, G_IM_SIZ_16b, 44, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);
        }
        //Draw MQ label
        if (Save_GetSaveMetaInfo(i)->requiresMasterQuest && !Save_GetSaveMetaInfo(i)->randoSave && Save_GetSaveMetaInfo(i)->valid) {
            if (CVar_GetS32("gHudColors", 1) == 2 && FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i))) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->nameAlpha[i]);
            } else if (!FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i))) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[1][0], sWindowContentColors[1][1],
                                sWindowContentColors[1][2], this->nameBoxAlpha[i]);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0],
                                sWindowContentColors[isActive][1], sWindowContentColors[isActive][2],
                                this->nameAlpha[i]);
            }
            gDPLoadTextureBlock(POLY_OPA_DISP++, ResourceMgr_LoadFileRaw("assets/textures/title_static/gFileSelMQButtonTex"), G_IM_FMT_IA, G_IM_SIZ_16b, 44, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);
        }

        // draw connectors
        if (CVar_GetS32("gHudColors", 1) == 2 && FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i))) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->connectorAlpha[i]);
        } else if (!FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i)) && Save_GetSaveMetaInfo(i)->valid) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[1][0], sWindowContentColors[1][1],
                            sWindowContentColors[1][2], this->fileButtonAlpha[i]);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0], sWindowContentColors[isActive][1],
                            sWindowContentColors[isActive][2], this->connectorAlpha[i]);
        }
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelConnectorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 12, 14, 15, 13, 0);

        if (Save_GetSaveMetaInfo(i)->randoSave || Save_GetSaveMetaInfo(i)->requiresMasterQuest) {
            gSP1Quadrangle(POLY_OPA_DISP++, 16, 18, 19, 17, 0);
        }
    }

    // draw file info
    for (fileIndex = 0; fileIndex < 3; fileIndex++) {
        isActive = 0;
        FileChoose_DrawFileInfo(&this->state, fileIndex, isActive);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[0x274], 20, 0);

    // draw primary action buttons (copy/erase)
    for (quadVtxIndex = 0, i = 0; i < 2; i++, quadVtxIndex += 4) {
        gDPPipeSync(POLY_OPA_DISP++);

        if (CVar_GetS32("gHudColors", 1) == 2) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->actionButtonAlpha[i]);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            this->actionButtonAlpha[i]);
        }
        gDPLoadTextureBlock(POLY_OPA_DISP++, sActionButtonTextures[gSaveContext.language][i], G_IM_FMT_IA, G_IM_SIZ_16b,
                            64, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);

    // draw confirm buttons (yes/quit)
    for (quadVtxIndex = 0, i = 0; i < 2; i++, quadVtxIndex += 4) {
        temp = this->confirmButtonTexIndices[i];

        if (CVar_GetS32("gHudColors", 1) == 2) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->confirmButtonAlpha[i]);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            this->confirmButtonAlpha[i]);
        }
        gDPLoadTextureBlock(POLY_OPA_DISP++, sActionButtonTextures[gSaveContext.language][temp], G_IM_FMT_IA,
                            G_IM_SIZ_16b, 64, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
    }

    // draw options button
    gDPPipeSync(POLY_OPA_DISP++);

    if (CVar_GetS32("gHudColors", 1) == 2) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->optionButtonAlpha);
    } else {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                        this->optionButtonAlpha);
    }
    gDPLoadTextureBlock(POLY_OPA_DISP++, sOptionsButtonTextures[gSaveContext.language], G_IM_FMT_IA, G_IM_SIZ_16b, 64,
                        16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                        G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);

    // draw highlight over currently selected button
    if (((this->menuMode == FS_MENU_MODE_CONFIG) &&
         ((this->configMode == CM_MAIN_MENU) || (this->configMode == CM_SELECT_COPY_SOURCE) ||
          (this->configMode == CM_SELECT_COPY_DEST) || (this->configMode == CM_COPY_CONFIRM) ||
          (this->configMode == CM_ERASE_SELECT) || (this->configMode == CM_ERASE_CONFIRM))) ||
        ((this->menuMode == FS_MENU_MODE_SELECT) && (this->selectMode == SM_CONFIRM_FILE))) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);

        if (CVar_GetS32("gHudColors", 1) == 2) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->highlightColor[3]);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->highlightColor[0], this->highlightColor[1],
                            this->highlightColor[2], this->highlightColor[3]);
        }
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelBigButtonHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 72, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 12, 14, 15, 13, 0);
    }

    // draw warning labels
    if (this->warningLabel > FS_WARNING_NONE) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->emptyFileTextAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sWarningLabels[gSaveContext.language][this->warningLabel], G_IM_FMT_IA,
                            G_IM_SIZ_8b, 128, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 16, 18, 19, 17, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileChoose_ConfigModeDraw(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    f32 eyeX;
    f32 eyeY;
    f32 eyeZ;
    Color_RGB8 Background_Color = {this->windowColor[0], this->windowColor[1], this->windowColor[2]};

    OPEN_DISPS(this->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);

    eyeX = 1000.0f * Math_CosS(ZREG(11)) - 1000.0f * Math_SinS(ZREG(11));
    eyeY = ZREG(13);
    eyeZ = 1000.0f * Math_SinS(ZREG(11)) + 1000.0f * Math_CosS(ZREG(11));

    FileChoose_SetView(this, eyeX, eyeY, eyeZ);
    SkyboxDraw_Draw(&this->skyboxCtx, this->state.gfxCtx, 1, this->envCtx.skyboxBlend, eyeX, eyeY, eyeZ);
    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_NONE);
    ZREG(11) += ZREG(10);
    Environment_UpdateSkybox(NULL, SKYBOX_NORMAL_SKY, &this->envCtx, &this->skyboxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    func_800949A8(this->state.gfxCtx);
    FileChoose_SetView(this, 0.0f, 0.0f, 64.0f);
    FileChoose_SetWindowVtx(&this->state);
    FileChoose_SetWindowContentVtx(&this->state);

    FrameInterpolation_RecordOpenChild(this, this->configMode);

    if ((this->configMode != CM_NAME_ENTRY) && (this->configMode != CM_START_NAME_ENTRY)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        if (CVar_GetS32("gHudColors", 1) == 2) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->windowAlpha);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            this->windowAlpha);
        }
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);

        if (this->windowRot != 0) {
            Matrix_RotateX(this->windowRot / 100.0f, MTXMODE_APPLY);
        }

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileChoose_DrawWindowContents(&this->state);
    }

    // draw name entry menu
    if ((this->configMode >= CM_ROTATE_TO_NAME_ENTRY) && (this->configMode <= CM_NAME_ENTRY_TO_MAIN)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        if (CVar_GetS32("gHudColors", 1) == 2) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->windowAlpha);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            this->windowAlpha);
        }
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
        Matrix_RotateX((this->windowRot - 314.0f) / 100.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileChoose_DrawNameEntry(&this->state);
    }

    // draw options menu
    if ((this->configMode >= CM_MAIN_TO_OPTIONS) && (this->configMode <= CM_OPTIONS_TO_MAIN)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        if (CVar_GetS32("gHudColors", 1) == 2) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->windowAlpha);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            this->windowAlpha);
        }
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
        Matrix_RotateX((this->windowRot - 314.0f) / 100.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileChoose_DrawOptions(&this->state);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    FileChoose_SetView(this, 0.0f, 0.0f, 64.0f);

    DrawSeedHashSprites(this);

    FrameInterpolation_RecordCloseChild();

    CLOSE_DISPS(this->state.gfxCtx);
}

/**
 * Fade out the main menu elements to transition to select mode.
 * Update function for `SM_FADE_MAIN_TO_SELECT`
 */
void FileChoose_FadeMainToSelect(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 i;

    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] -= 25;
            this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
                this->optionButtonAlpha = this->fileButtonAlpha[i];

            if (Save_GetSaveMetaInfo(i)->valid) {
                this->nameAlpha[i] = this->nameBoxAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] -= 31;
            }
        }
    }

    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 8;
        this->selectMode++;
        this->confirmButtonIndex = FS_BTN_CONFIRM_YES;
    }
}

/**
 * Moves the selected file to the top of the window.
 * Update function for `SM_MOVE_FILE_TO_TOP`
 */
void FileChoose_MoveSelectedFileToTop(GameState* thisx) {
    static s16 fileYOffsets[] = { 0, 16, 32 }; // amount to move by to reach the top of the screen
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 yStep;

    yStep = ABS(this->buttonYOffsets[this->buttonIndex] - fileYOffsets[this->buttonIndex]) / this->actionTimer;
    this->buttonYOffsets[this->buttonIndex] += yStep;
    this->actionTimer--;

    if ((this->actionTimer == 0) || (this->buttonYOffsets[this->buttonIndex] == fileYOffsets[this->buttonIndex])) {
        this->buttonYOffsets[FS_BTN_SELECT_YES] = this->buttonYOffsets[FS_BTN_SELECT_QUIT] = -24;
        this->actionTimer = 8;
        this->selectMode++;
    }
}

/**
 * Fade in the file info for the selected file.
 * Update function for `SM_FADE_IN_FILE_INFO`
 */
void FileChoose_FadeInFileInfo(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->fileInfoAlpha[this->buttonIndex] += 25;
    this->nameBoxAlpha[this->buttonIndex] -= 50;

    if (this->nameBoxAlpha[this->buttonIndex] <= 0) {
        this->nameBoxAlpha[this->buttonIndex] = 0;
    }

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->fileInfoAlpha[this->buttonIndex] = 200;
        this->actionTimer = 8;
        this->selectMode++;
    }

    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] = this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] =
        this->fileInfoAlpha[this->buttonIndex];
}

/**
 * Update the cursor and handle the option that the player picks for confirming the selected file.
 * Update function for `SM_CONFIRM_FILE`
 */
void FileChoose_ConfirmFile(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    Input* input = &this->state.input[0];
    bool dpad = CVar_GetS32("gDpadText", 0);

    if (CHECK_BTN_ALL(input->press.button, BTN_START) || (CHECK_BTN_ALL(input->press.button, BTN_A))) {
        if (this->confirmButtonIndex == FS_BTN_CONFIRM_YES) {
            func_800AA000(300.0f, 180, 20, 100);
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            this->selectMode = SM_FADE_OUT;
            func_800F6964(0xF);
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            this->selectMode++;
        }
    } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        this->selectMode++;
    } else if ((ABS(this->stickRelY) >= 30) || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN | BTN_DUP))) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        this->confirmButtonIndex ^= 1;
    }
}

/**
 * Fade out the file info for the selected file before returning to the main menu.
 * Update function for `SM_FADE_OUT_FILE_INFO`
 */
void FileChoose_FadeOutFileInfo(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->fileInfoAlpha[this->buttonIndex] -= 25;
    this->nameBoxAlpha[this->buttonIndex] += 25;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->buttonYOffsets[FS_BTN_SELECT_YES] = this->buttonYOffsets[FS_BTN_SELECT_QUIT] = 0;
        this->nameBoxAlpha[this->buttonIndex] = 200;
        this->fileInfoAlpha[this->buttonIndex] = 0;
        this->nextTitleLabel = FS_TITLE_SELECT_FILE;
        this->actionTimer = 8;
        this->selectMode++;
    }

    this->confirmButtonAlpha[0] = this->confirmButtonAlpha[1] = this->fileInfoAlpha[this->buttonIndex];
}

/**
 * Move the selected file back to the slot position then go to config mode for the main menu.
 * Update function for `SM_MOVE_FILE_TO_SLOT`
 */
void FileChoose_MoveSelectedFileToSlot(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 yStep;
    s16 i;

    yStep = ABS(this->buttonYOffsets[this->buttonIndex]) / this->actionTimer;
    this->buttonYOffsets[this->buttonIndex] -= yStep;

    if (this->buttonYOffsets[this->buttonIndex] <= 0) {
        this->buttonYOffsets[this->buttonIndex] = 0;
    }

    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] += 25;

            if (this->fileButtonAlpha[i] >= 200) {
                this->fileButtonAlpha[i] = 200;
            }

            this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
                this->optionButtonAlpha = this->fileButtonAlpha[i];

            if (Save_GetSaveMetaInfo(i)->valid) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] += 31;
            }
        }
    }

    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->titleLabel = this->nextTitleLabel;
        this->actionTimer = 8;
        this->menuMode = FS_MENU_MODE_CONFIG;
        this->configMode = CM_MAIN_MENU;
        this->nextConfigMode = CM_MAIN_MENU;
        this->selectMode = SM_FADE_MAIN_TO_SELECT;
    }
}

/**
 * Fill the screen with black to fade out.
 * Update function for `SM_FADE_OUT`
 */
void FileChoose_FadeOut(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    sScreenFillAlpha += VREG(10);

    if (sScreenFillAlpha >= 255) {
        sScreenFillAlpha = 255;
        this->selectMode++;
    }
}

/**
 * Load the save for the appropriate file and start the game.
 * Note: On Debug ROM, File 1 will go to Map Select.
 * Update function for `SM_LOAD_GAME`
 */
void FileChoose_LoadGame(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    u16 swordEquipMask;
    s32 pad;

    if ((this->buttonIndex == FS_BTN_SELECT_FILE_1 && CVar_GetS32("gDebugEnabled", 0)) || this->buttonIndex == 0xFF) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        gSaveContext.fileNum = this->buttonIndex;
        if (this->buttonIndex == 0xFF) {
            Sram_InitDebugSave();
        } else {
            Sram_OpenSave();
        }
        gSaveContext.gameMode = 0;
        SET_NEXT_GAMESTATE(&this->state, Select_Init, SelectContext);
        this->state.running = false;
    } else {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        gSaveContext.fileNum = this->buttonIndex;
        Sram_OpenSave();
        gSaveContext.gameMode = 0;
        SET_NEXT_GAMESTATE(&this->state, Gameplay_Init, GlobalContext);
        this->state.running = false;
    }

    Randomizer_LoadSettings("");
    Randomizer_LoadHintLocations("");
    Randomizer_LoadItemLocations("", true);
    Randomizer_LoadRequiredTrials("");
    Randomizer_LoadMerchantMessages("");
    Randomizer_LoadMasterQuestDungeons("");

    gSaveContext.respawn[0].entranceIndex = -1;
    gSaveContext.respawnFlag = 0;
    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
    gSaveContext.natureAmbienceId = 0xFF;
    gSaveContext.showTitleCard = true;
    gSaveContext.dogParams = 0;
    gSaveContext.timer1State = 0;
    gSaveContext.timer2State = 0;
    gSaveContext.eventInf[0] = 0;
    gSaveContext.eventInf[1] = 0;
    gSaveContext.eventInf[2] = 0;
    gSaveContext.eventInf[3] = 0;
    gSaveContext.unk_13EE = 0x32;
    gSaveContext.nayrusLoveTimer = 0;
    gSaveContext.healthAccumulator = 0;
    gSaveContext.unk_13F0 = 0;
    gSaveContext.unk_13F2 = 0;
    gSaveContext.forcedSeqId = NA_BGM_GENERAL_SFX;
    gSaveContext.skyboxTime = 0;
    gSaveContext.nextTransition = 0xFF;
    gSaveContext.nextCutsceneIndex = 0xFFEF;
    gSaveContext.cutsceneTrigger = 0;
    gSaveContext.chamberCutsceneNum = 0;
    gSaveContext.nextDayTime = 0xFFFF;
    gSaveContext.unk_13C3 = 0;

    for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
        gSaveContext.buttonStatus[buttonIndex] = BTN_ENABLED;
    }

    gSaveContext.unk_13E7 = gSaveContext.unk_13E8 = gSaveContext.unk_13EA = gSaveContext.unk_13EC =
        gSaveContext.unk_13F4 = 0;

    gSaveContext.unk_13F6 = gSaveContext.magic;
    gSaveContext.magic = 0;
    gSaveContext.magicLevel = gSaveContext.magic;

    osSyncPrintf(VT_FGCOL(GREEN));
    osSyncPrintf("Z_MAGIC_NOW_NOW=%d  MAGIC_NOW=%d\n", ((void)0, gSaveContext.unk_13F6), gSaveContext.magic);
    osSyncPrintf(VT_RST);

    gSaveContext.naviTimer = 0;

    // SWORDLESS LINK IS BACK BABY
    if (CVar_GetS32("gSwordlessLink", 0) != 0)
    {
        if ((gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KOKIRI) &&
            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_MASTER) &&
            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_BGS) &&
            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KNIFE)) {

            gSaveContext.equips.buttonItems[0] = ITEM_NONE;
            swordEquipMask = BOMSWAP16(gEquipMasks[EQUIP_SWORD]) & gSaveContext.equips.equipment;
            gSaveContext.equips.equipment &= gEquipNegMasks[EQUIP_SWORD];
            gSaveContext.inventory.equipment ^= (gBitFlags[swordEquipMask - 1] << BOMSWAP16(gEquipShifts[EQUIP_SWORD]));
        }
    }
}

static void (*gSelectModeUpdateFuncs[])(GameState*) = {
    FileChoose_FadeMainToSelect, FileChoose_MoveSelectedFileToTop,  FileChoose_FadeInFileInfo, FileChoose_ConfirmFile,
    FileChoose_FadeOutFileInfo,  FileChoose_MoveSelectedFileToSlot, FileChoose_FadeOut,        FileChoose_LoadGame,
};

void FileChoose_SelectModeUpdate(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    gSelectModeUpdateFuncs[this->selectMode](&this->state);
}

void FileChoose_SelectModeDraw(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    f32 eyeX;
    f32 eyeY;
    f32 eyeZ;
    Color_RGB8 Background_Color = { this->windowColor[0], this->windowColor[1], this->windowColor[2] };

    OPEN_DISPS(this->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);

    eyeX = 1000.0f * Math_CosS(ZREG(11)) - 1000.0f * Math_SinS(ZREG(11));
    eyeY = ZREG(13);
    eyeZ = 1000.0f * Math_SinS(ZREG(11)) + 1000.0f * Math_CosS(ZREG(11));

    FileChoose_SetView(this, eyeX, eyeY, eyeZ);
    SkyboxDraw_Draw(&this->skyboxCtx, this->state.gfxCtx, 1, this->envCtx.skyboxBlend, eyeX, eyeY, eyeZ);
    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_NONE);
    ZREG(11) += ZREG(10);
    Environment_UpdateSkybox(NULL, SKYBOX_NORMAL_SKY, &this->envCtx, &this->skyboxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    func_800949A8(this->state.gfxCtx);
    FileChoose_SetView(this, 0.0f, 0.0f, 64.0f);
    FileChoose_SetWindowVtx(&this->state);
    FileChoose_SetWindowContentVtx(&this->state);

    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    if (CVar_GetS32("gHudColors", 1) == 2) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChoosePrim", Background_Color).r, CVar_GetRGB("gCCFileChoosePrim", Background_Color).g, CVar_GetRGB("gCCFileChoosePrim", Background_Color).b, this->windowAlpha);
    } else {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                        this->windowAlpha);
    }
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
    Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
    Matrix_RotateX(this->windowRot / 100.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
    gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

    gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
    gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

    gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
    gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

    FileChoose_DrawWindowContents(&this->state);
    gDPPipeSync(POLY_OPA_DISP++);
    FileChoose_SetView(this, 0.0f, 0.0f, 64.0f);

    DrawSeedHashSprites(this);

    CLOSE_DISPS(this->state.gfxCtx);
}

static void (*gFileSelectDrawFuncs[])(GameState*) = {
    FileChoose_InitModeDraw,
    FileChoose_ConfigModeDraw,
    FileChoose_SelectModeDraw,
};

static void (*gFileSelectUpdateFuncs[])(GameState*) = {
    FileChoose_InitModeUpdate,
    FileChoose_ConfigModeUpdate,
    FileChoose_SelectModeUpdate,
};

void FileChoose_Main(GameState* thisx) {
    static void* controlsTextures[] = {
        gFileSelControlsENGTex,
        gFileSelControlsGERTex,
        gFileSelControlsFRATex,
    };
    Color_RGB8 Text_Color = { 100, 255, 255 };
    FileChooseContext* this = (FileChooseContext*)thisx;
    Input* input = &this->state.input[0];

    if (CVar_GetS32("gTimeFlowFileSelect", 0) != 0) {
        gSaveContext.skyboxTime += 0x10;
    }

    if (CVar_GetS32("gSkipLogoTitle", 0) && CVar_GetS32("gSaveFileID", 0) < 3 && !isFastFileIdIncompatible) {
        if (Save_Exist(CVar_GetS32("gSaveFileID", 0)) && FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(CVar_GetS32("gSaveFileID", 0)))) {
            this->buttonIndex = CVar_GetS32("gSaveFileID", 0);
            this->menuMode = FS_MENU_MODE_SELECT;
            this->selectMode = SM_LOAD_GAME;
        } else {
            isFastFileIdIncompatible = 1;
        }
    } else if (CVar_GetS32("gSkipLogoTitle", 0) && CVar_GetS32("gSaveFileID", 0) == 3) {
        this->buttonIndex = 0xFF;
        this->menuMode = FS_MENU_MODE_SELECT;
        this->selectMode = SM_LOAD_GAME;
    }

    OPEN_DISPS(this->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    gSPSegment(POLY_OPA_DISP++, 0x01, this->staticSegment);
    gSPSegment(POLY_OPA_DISP++, 0x02, this->parameterSegment);

    func_80095248(this->state.gfxCtx, 0, 0, 0);

    this->stickRelX = input->rel.stick_x;
    this->stickRelY = input->rel.stick_y;

    if (CVar_GetS32("gDpadHoldChange", 1) && CVar_GetS32("gDpadText", 0)) {
        if (CHECK_BTN_ALL(input->cur.button, BTN_DLEFT)) {
            if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
                this->inputTimerX = 10;
                this->stickXDir = -1;
            } else if (--(this->inputTimerX) < 0) {
                this->inputTimerX = XREG(6);
                input->press.button |= BTN_DLEFT;
            }
        } else if (CHECK_BTN_ALL(input->rel.button, BTN_DLEFT)) {
            this->stickXDir = 0;
        } else if (CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT)) {
            if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
                this->inputTimerX = 10;
                this->stickXDir = 1;
            } else if (--(this->inputTimerX) < 0) {
                this->inputTimerX = XREG(6);
                input->press.button |= BTN_DRIGHT;
            }
        } else if (CHECK_BTN_ALL(input->rel.button, BTN_DRIGHT)) {
            this->stickXDir = 0;
        }

        if (CHECK_BTN_ALL(input->cur.button, BTN_DDOWN)) {
            if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
                this->inputTimerY = 10;
                this->stickYDir = -1;
            } else if (--(this->inputTimerY) < 0) {
                this->inputTimerY = XREG(6);
                input->press.button |= BTN_DDOWN;
            }
        } else if (CHECK_BTN_ALL(input->rel.button, BTN_DDOWN)) {
            this->stickYDir = 0;
        } else if (CHECK_BTN_ALL(input->cur.button, BTN_DUP)) {
            if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
                this->inputTimerY = 10;
                this->stickYDir = -1;
            } else if (--(this->inputTimerY) < 0) {
                this->inputTimerY = XREG(6);
                input->press.button |= BTN_DUP;
            }
        } else if (CHECK_BTN_ALL(input->rel.button, BTN_DUP)) {
            this->stickYDir = 0;
        }
    }

    if (this->stickRelX < -30) {
        if (this->stickXDir == -1) {
            this->inputTimerX--;
            if (this->inputTimerX < 0) {
                this->inputTimerX = 2;
            } else {
                this->stickRelX = 0;
            }
        } else {
            this->inputTimerX = 10;
            this->stickXDir = -1;
        }
    } else if (this->stickRelX > 30) {
        if (this->stickXDir == 1) {
            this->inputTimerX--;
            if (this->inputTimerX < 0) {
                this->inputTimerX = 2;
            } else {
                this->stickRelX = 0;
            }
        } else {
            this->inputTimerX = 10;
            this->stickXDir = 1;
        }
    } else {
        this->stickXDir = 0;
    }

    if (this->stickRelY < -30) {
        if (this->stickYDir == -1) {
            this->inputTimerY--;
            if (this->inputTimerY < 0) {
                this->inputTimerY = 2;
            } else {
                this->stickRelY = 0;
            }
        } else {
            this->inputTimerY = 10;
            this->stickYDir = -1;
        }
    } else if (this->stickRelY > 30) {
        if (this->stickYDir == 1) {
            this->inputTimerY--;
            if (this->inputTimerY < 0) {
                this->inputTimerY = 2;
            } else {
                this->stickRelY = 0;
            }
        } else {
            this->inputTimerY = 10;
            this->stickYDir = 1;
        }
    } else {
        this->stickYDir = 0;
    }

    this->emptyFileTextAlpha = 0;

    FileChoose_PulsateCursor(&this->state);
    gFileSelectUpdateFuncs[this->menuMode](&this->state);
    FrameInterpolation_StartRecord();
    gFileSelectDrawFuncs[this->menuMode](&this->state);
    FrameInterpolation_StopRecord();

    // do not draw controls text in the options menu
    if ((this->configMode <= CM_NAME_ENTRY_TO_MAIN) || (this->configMode >= CM_UNUSED_DELAY)) {
        func_800944C4(this->state.gfxCtx);

        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        if (CVar_GetS32("gHudColors", 1) == 2) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, CVar_GetRGB("gCCFileChooseTextPrim", Text_Color).r, CVar_GetRGB("gCCFileChooseTextPrim", Text_Color).g, CVar_GetRGB("gCCFileChooseTextPrim", Text_Color).b, this->controlsAlpha);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, Text_Color.r, Text_Color.g, Text_Color.b, this->controlsAlpha);
        }
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
        gDPLoadTextureBlock(POLY_OPA_DISP++, controlsTextures[gSaveContext.language], G_IM_FMT_IA, G_IM_SIZ_8b, 144, 16,
                            0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(POLY_OPA_DISP++, 0x0168, 0x0330, 0x03A8, 0x0370, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gSPDisplayList(POLY_OPA_DISP++, sScreenFillSetupDL);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, sScreenFillAlpha);
    gDPFillRectangle(POLY_OPA_DISP++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileChoose_InitContext(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    EnvironmentContext* envCtx = &this->envCtx;

    ZREG(7) = 32;
    ZREG(8) = 22;
    ZREG(9) = 20;
    ZREG(10) = -10;
    ZREG(11) = 0;
    ZREG(12) = 1000;
    ZREG(13) = -700;
    ZREG(14) = 164;
    ZREG(15) = 104;
    ZREG(16) = 160;
    ZREG(17) = 100;
    ZREG(18) = 162;
    ZREG(19) = 152;
    ZREG(20) = 214;

    XREG(13) = 580;
    XREG(14) = 400;
    XREG(35) = 20;
    XREG(36) = 20;
    XREG(37) = 20;
    XREG(43) = 8;
    XREG(44) = -78;
    XREG(45) = 0;
    XREG(46) = 0;
    XREG(47) = 0;
    XREG(48) = 0;
    XREG(49) = 3;
    XREG(50) = 8;
    XREG(51) = 8;
    XREG(52) = 10;
    XREG(73) = 0;

    VREG(0) = 14;
    VREG(1) = 5;
    VREG(2) = 4;
    VREG(4) = 1;
    VREG(5) = 6;
    VREG(6) = 2;
    VREG(7) = 6;
    VREG(8) = 80;

    sScreenFillAlpha = 255;

    VREG(10) = 10;
    VREG(11) = 30;
    VREG(12) = -100;
    VREG(13) = -85;
    VREG(14) = 4;
    VREG(16) = 25;
    VREG(17) = 1;
    VREG(18) = 1;
    VREG(20) = 92;
    VREG(21) = 171;
    VREG(22) = 11;
    VREG(23) = 10;
    VREG(24) = 26;
    VREG(25) = 2;
    VREG(26) = 1;
    VREG(27) = 0;
    VREG(28) = 0;
    VREG(29) = 160;
    VREG(30) = 64;
    VREG(31) = 154;
    VREG(32) = 152;
    VREG(33) = 106;

    WREG(38) = 16;
    WREG(39) = 9;
    WREG(40) = 10;
    WREG(41) = 14;
    WREG(42) = 11;
    WREG(43) = 12;

    this->menuMode = FS_MENU_MODE_INIT;

    this->buttonIndex = this->selectMode = this->selectedFileIndex = this->copyDestFileIndex =
        this->confirmButtonIndex = 0;

    this->confirmButtonTexIndices[0] = 2;
    this->confirmButtonTexIndices[1] = 3;
    this->titleLabel = FS_TITLE_SELECT_FILE;
    this->nextTitleLabel = FS_TITLE_OPEN_FILE;
    this->highlightPulseDir = 1;
    this->unk_1CAAC = 0xC;
    this->highlightColor[0] = 155;
    this->highlightColor[1] = 255;
    this->highlightColor[2] = 255;
    this->highlightColor[3] = 70;
    this->configMode = CM_FADE_IN_START;
    this->windowRot = 0.0f;
    this->stickXDir = this->inputTimerX = 0;
    this->stickYDir = this->inputTimerY = 0;
    this->kbdX = this->kbdY = this->charIndex = 0;
    this->kbdButton = FS_KBD_BTN_NONE;

    this->windowColor[0] = 100;
    this->windowColor[1] = 150;
    this->windowColor[2] = 255;

    this->windowAlpha = this->titleAlpha[0] = this->titleAlpha[1] = this->fileButtonAlpha[0] =
        this->fileButtonAlpha[1] = this->fileButtonAlpha[2] = this->nameBoxAlpha[0] = this->nameBoxAlpha[1] =
            this->nameBoxAlpha[2] = this->nameAlpha[0] = this->nameAlpha[1] = this->nameAlpha[2] =
                this->connectorAlpha[0] = this->connectorAlpha[1] = this->connectorAlpha[2] = this->fileInfoAlpha[0] =
                    this->fileInfoAlpha[1] = this->fileInfoAlpha[2] = this->actionButtonAlpha[FS_BTN_ACTION_COPY] =
                        this->actionButtonAlpha[FS_BTN_ACTION_ERASE] = this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] =
                            this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = this->optionButtonAlpha =
                                this->nameEntryBoxAlpha = this->controlsAlpha = this->emptyFileTextAlpha = 0;

    this->windowPosX = 6;
    this->actionTimer = 8;
    this->warningLabel = FS_WARNING_NONE;

    this->warningButtonIndex = this->buttonYOffsets[0] = this->buttonYOffsets[1] = this->buttonYOffsets[2] =
        this->buttonYOffsets[3] = this->buttonYOffsets[4] = this->buttonYOffsets[5] = this->fileNamesY[0] =
            this->fileNamesY[1] = this->fileNamesY[2] = 0;

    this->unk_1CAD6[0] = 0;
    this->unk_1CAD6[1] = 3;
    this->unk_1CAD6[2] = 6;
    this->unk_1CAD6[3] = 8;
    this->unk_1CAD6[4] = 10;

    ShrinkWindow_SetVal(0);

    gSaveContext.skyboxTime = 0;
    gSaveContext.dayTime = 0;

    Skybox_Init(&this->state, &this->skyboxCtx, SKYBOX_NORMAL_SKY);

    gTimeIncrement = 10;

    envCtx->unk_19 = 0;
    envCtx->unk_1A = 0;
    envCtx->unk_21 = 0;
    envCtx->unk_22 = 0;
    envCtx->skyboxDmaState = SKYBOX_DMA_INACTIVE;
    envCtx->skybox1Index = 99;
    envCtx->skybox2Index = 99;
    envCtx->unk_1F = 0;
    envCtx->unk_20 = 0;
    envCtx->unk_BD = 0;
    envCtx->unk_17 = 2;
    envCtx->skyboxDisabled = 0;
    envCtx->skyboxBlend = 0;
    envCtx->unk_84 = 0.0f;
    envCtx->unk_88 = 0.0f;

    Environment_UpdateSkybox(NULL, SKYBOX_NORMAL_SKY, &this->envCtx, &this->skyboxCtx);

    for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
        gSaveContext.buttonStatus[buttonIndex] = BTN_ENABLED;
    }
}

void FileChoose_Destroy(GameState* thisx) {
}

void FileChoose_Init(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    size_t size = (u32)_title_staticSegmentRomEnd - (u32)_title_staticSegmentRomStart;
    s32 pad;
    fileSelectSpoilerFileLoaded = false;
    isFastFileIdIncompatible = 0;
    CVar_SetS32("gOnFileSelectNameEntry", 0);

    SREG(30) = 1;
    osSyncPrintf("SIZE=%x\n", size);

    this->staticSegment = GAMESTATE_ALLOC_MC(&this->state, size);
    ASSERT(this->staticSegment != NULL);
    DmaMgr_SendRequest1(this->staticSegment, (u32)_title_staticSegmentRomStart, size, __FILE__, __LINE__);

    size = (u32)_parameter_staticSegmentRomEnd - (u32)_parameter_staticSegmentRomStart;
    this->parameterSegment = GAMESTATE_ALLOC_MC(&this->state, size);
    ASSERT(this->parameterSegment != NULL);
    DmaMgr_SendRequest1(this->parameterSegment, (u32)_parameter_staticSegmentRomStart, size, __FILE__,
                        __LINE__);

    Matrix_Init(&this->state);
    View_Init(&this->view, this->state.gfxCtx);
    this->state.main = FileChoose_Main;
    this->state.destroy = FileChoose_Destroy;
    FileChoose_InitContext(&this->state);
    Font_LoadOrderedFont(&this->font);
    Audio_QueueSeqCmd(0xF << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0xA);
    func_800F5E18(SEQ_PLAYER_BGM_MAIN, NA_BGM_FILE_SELECT, 0, 7, 1);
}
