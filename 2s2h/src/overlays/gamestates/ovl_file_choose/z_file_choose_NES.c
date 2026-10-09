/*
 * File: z_file_choose_NES.c
 * Overlay: ovl_file_choose
 * Description:
 */

#include "z_file_select.h"
#include "overlays/gamestates/ovl_opening/z_opening.h"
#include "z64rumble.h"
#include "z64save.h"
#include "z64shrink_window.h"
#include "z64view.h"
#include "interface/parameter_static/parameter_static.h"
#include "misc/title_static/title_static.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h_assets.h"
#include <string.h>
#include "BenPort.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>

s32 D_808144F10 = 100;
f32 D_808144F14 = 8.0f;
f32 D_808144F18 = 100.0f;
s32 D_808144F1C = 0;

FileSelectState* gFileSelectState = NULL;

// 2S2H [Enhancement] To handle File 3 support and toggle, we undef the file num max and replace it for a CVar check
#undef FILE_NUM_MAX
#define FILE_NUM_MAX (CVarGetInteger("gEnhancements.Saving.FileSlot3", true) ? 3 : 2)

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

s16 sFileInfoBoxPartWidths[] = {
    36, 36, 36, 36, 24, 28, 28,
};

s16 sWindowContentColors[] = { 100, 150, 255 };

s16 sFileSelectSkyboxRotation = 0;

s16 sWalletFirstDigit[] = {
    1, // tens (Default Wallet)
    0, // hundreds (Adult Wallet)
    0, // hundreds (Giant Wallet)
};

void FileSelect_IncrementConfigMode(FileSelectState* this) {
    this->configMode++;
}

void FileSelect_Noop1(void) {
}

void FileSelect_Noop2(FileSelectState* this) {
}

void FileSelect_InitModeUpdate(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    if (this->configMode == CM_FADE_IN_START) {
        if (gSaveContext.options.optionId != 0xA51D) { // Magic number?
            this->configMode++;
        } else {
            this->menuMode = FS_MENU_MODE_CONFIG;
            this->configMode = CM_FADE_IN_START;
            this->titleLabel = FS_TITLE_SELECT_FILE;
            this->nextTitleLabel = FS_TITLE_OPEN_FILE;
        }
    }

    if (this->configMode == CM_FADE_IN_END) {
        this->screenFillAlpha -= 40;
        if (this->screenFillAlpha <= 0) {
            this->screenFillAlpha = 0;
            this->configMode++; // CM_MAIN_MENU
        }
    } else if (this->configMode == CM_MAIN_MENU) {
        FileSelect_IncrementConfigMode(this); // CM_SETUP_COPY_SOURCE
    } else {
        this->screenFillAlpha += 40;
        if (this->screenFillAlpha >= 255) {
            this->screenFillAlpha = 255;
            this->menuMode = FS_MENU_MODE_CONFIG;
            this->configMode = CM_FADE_IN_START;
            this->titleLabel = FS_TITLE_SELECT_FILE;
            this->nextTitleLabel = FS_TITLE_OPEN_FILE;
        }
    }
}

void FileSelect_InitModeDraw(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    Gfx_SetupDL39_Opa(this->state.gfxCtx);
    FileSelect_Noop2(this);
}

void FileSelect_SetView(FileSelectState* this, f32 eyeX, f32 eyeY, f32 eyeZ) {
    Vec3f eye;
    Vec3f lookAt;
    Vec3f up;

    eye.x = eyeX;
    eye.y = eyeY;
    eye.z = eyeZ;

    lookAt.x = lookAt.y = lookAt.z = 0.0f;

    up.x = up.z = 0.0f;
    up.y = 1.0f;

    View_LookAt(&this->view, &eye, &lookAt, &up);
    View_Apply(&this->view, VIEW_ALL | VIEW_FORCE_VIEWING | VIEW_FORCE_VIEWPORT | VIEW_FORCE_PROJECTION_PERSPECTIVE);
}

Gfx* FileSelect_DrawTexQuadIA8(Gfx* gfx, TexturePtr texture, s16 width, s16 height, s16 point) {
    gDPLoadTextureBlock(gfx++, texture, G_IM_FMT_IA, G_IM_SIZ_8b, width, height, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSP1Quadrangle(gfx++, point, point + 2, point + 3, point + 1, 0);

    return gfx;
}

/**
 * Fade in the menu window and title label.
 * If a file is occupied fade in the name, name box, and connector.
 * Fade in the copy erase and options button according to the window alpha.
 */
void FileSelect_FadeInMenuElements(FileSelectState* this) {
    SramContext* sramCtx = &this->sramCtx;
    s16 i;

    this->titleAlpha[FS_TITLE_CUR] += 20;
    this->windowAlpha += 16;

    for (i = 0; i < 3; i++) {
        this->fileButtonAlpha[i] = this->windowAlpha;

        if (!gSaveContext.flashSaveAvailable) {
            if (NO_FLASH_SLOT_OCCUPIED(sramCtx, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->windowAlpha;
                this->connectorAlpha[i] += 20;
                if (this->connectorAlpha[i] >= 255) {
                    this->connectorAlpha[i] = 255;
                }
            }
        } else if (SLOT_OCCUPIED(this, i)) {
            this->nameBoxAlpha[i] = this->nameAlpha[i] = this->windowAlpha;
            this->connectorAlpha[i] += 20;

            if (this->connectorAlpha[i] >= 255) {
                this->connectorAlpha[i] = 255;
            }
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        this->optionButtonAlpha = this->windowAlpha;
}

/**
 * Converts a numerical value to hundreds-tens-ones digits
 */
void FileSelect_SplitNumber(u16 value, u16* hundreds, u16* tens, u16* ones) {
    *hundreds = 0;
    *tens = 0;
    *ones = value;

    do {
        if ((*ones - 100) < 0) {
            break;
        }
        (*hundreds)++;
        *ones -= 100;
    } while (true);

    do {
        if ((*ones - 10) < 0) {
            break;
        }
        (*tens)++;
        *ones -= 10;
    } while (true);
}

// Start of Config Mode Update Functions

/**
 * Reduce the alpha of the black screen fill to create a fade in effect.
 * Additionally, slide the window from the right to the center of the screen.
 * Update function for `CM_FADE_IN_START`
 */
void FileSelect_StartFadeIn(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    FileSelect_FadeInMenuElements(this);
    this->screenFillAlpha -= 40;
    this->windowPosX -= 20;

    if (this->windowPosX <= -94) {
        this->windowPosX = -94;
        this->configMode = CM_FADE_IN_END;
        this->screenFillAlpha = 0;
    }
}

/**
 * Finish fading in the remaining menu elements.
 * Fade in the controls text at the bottom of the screen.
 * Update function for `CM_FADE_IN_END`
 */
void FileSelect_FinishFadeIn(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->controlsAlpha += 20;
    FileSelect_FadeInMenuElements(this);

    if (this->titleAlpha[FS_TITLE_CUR] >= 255) {
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->controlsAlpha = 255;
        this->windowAlpha = 200;
        this->configMode = CM_MAIN_MENU;
    }
}

u8 sEmptyName[] = { 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E };

/**
 * Update the cursor and wait for the player to select a button to change menus accordingly.
 * If an empty file is selected, enter the name entry config mode.
 * If an occupied file is selected, enter the `Select` menu mode.
 * If copy, erase, or options is selected, set config mode accordingly.
 * Lastly, set any warning labels if appropriate.
 * Update function for `CM_MAIN_MENU`
 */
void FileSelect_UpdateMainMenu(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = CONTROLLER1(&this->state);

    if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->press.button, BTN_A)) {
        if (this->buttonIndex <= FS_BTN_MAIN_FILE_3) {
            if (!gSaveContext.flashSaveAvailable) {
                if (!NO_FLASH_SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
                    Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
                    this->configMode = CM_ROTATE_TO_NAME_ENTRY;
                    this->kbdButton = FS_KBD_BTN_NONE;
                    this->charPage = FS_CHAR_PAGE_HIRA;
                    if (gSaveContext.options.language != LANGUAGE_JPN) {
                        this->charPage = FS_CHAR_PAGE_ENG;
                    }
                    this->kbdX = 0;
                    this->kbdY = 0;
                    this->charIndex = 0;
                    this->charBgAlpha = 0;
                    this->newFileNameCharCount = 0;
                    this->nameEntryBoxPosX = 120;
                    this->nameEntryBoxAlpha = 0;
                    memcpy(&this->fileNames[this->buttonIndex], &sEmptyName, ARRAY_COUNT(sEmptyName));
                } else {
                    Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
                    this->actionTimer = 4;
                    this->selectMode = SM_FADE_MAIN_TO_SELECT;
                    this->selectedFileIndex = this->buttonIndex;
                    this->menuMode = FS_MENU_MODE_SELECT;
                    this->nextTitleLabel = FS_TITLE_OPEN_FILE;
                }
            } else if (!SLOT_OCCUPIED(this, this->buttonIndex)) {
                Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
                this->configMode = CM_ROTATE_TO_NAME_ENTRY;
                this->kbdButton = FS_KBD_BTN_NONE;
                this->charPage = FS_CHAR_PAGE_HIRA;
                if (gSaveContext.options.language != LANGUAGE_JPN) {
                    this->charPage = FS_CHAR_PAGE_ENG;
                }
                this->kbdX = 0;
                this->kbdY = 0;
                this->charIndex = 0;
                this->charBgAlpha = 0;
                this->newFileNameCharCount = 0;
                this->nameEntryBoxPosX = 120;
                this->nameEntryBoxAlpha = 0;
                memcpy(&this->fileNames[this->buttonIndex], &sEmptyName, ARRAY_COUNT(sEmptyName));
            } else {
                Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
                this->actionTimer = 4;
                this->selectMode = SM_FADE_MAIN_TO_SELECT;
                this->selectedFileIndex = this->buttonIndex;
                this->menuMode = FS_MENU_MODE_SELECT;
                this->nextTitleLabel = FS_TITLE_OPEN_FILE;
            }
        } else if (this->warningLabel == FS_WARNING_NONE) {
            Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
            this->prevConfigMode = this->configMode;

            if (this->buttonIndex == FS_BTN_MAIN_COPY) {
                this->configMode = CM_SETUP_COPY_SOURCE;
                this->nextTitleLabel = FS_TITLE_COPY_FROM;
            } else if (this->buttonIndex == FS_BTN_MAIN_ERASE) {
                this->configMode = CM_SETUP_ERASE_SELECT;
                this->nextTitleLabel = FS_TITLE_ERASE_FILE;
            } else {
                this->configMode = CM_MAIN_TO_OPTIONS;
                this->kbdButton = FS_KBD_BTN_HIRA;
                this->kbdX = 0;
                this->kbdY = 0;
                this->charBgAlpha = 0;
                this->newFileNameCharCount = 0;
                this->nameEntryBoxPosX = 120;
            }
            this->actionTimer = 4;
        } else {
            Audio_PlaySfx(NA_SE_SY_FSEL_ERROR);
        }
    } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        gSaveContext.gameMode = GAMEMODE_TITLE_SCREEN;
        STOP_GAMESTATE(&this->state);
        SET_NEXT_GAMESTATE(&this->state, TitleSetup_Init, sizeof(TitleSetupState));
    } else {
        if (ABS_ALT(this->stickAdjY) > 30) {
            Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
            if (this->stickAdjY > 30) {
                this->buttonIndex--;
                if (!CVarGetInteger("gEnhancements.Saving.FileSlot3", true) &&
                    this->buttonIndex == FS_BTN_MAIN_FILE_3) {
                    this->buttonIndex = FS_BTN_MAIN_FILE_2;
                }
                if (this->buttonIndex < FS_BTN_MAIN_FILE_1) {
                    this->buttonIndex = FS_BTN_MAIN_OPTIONS;
                }
            } else {
                this->buttonIndex++;
                if (!CVarGetInteger("gEnhancements.Saving.FileSlot3", true) &&
                    this->buttonIndex == FS_BTN_MAIN_FILE_3) {
                    this->buttonIndex = FS_BTN_MAIN_COPY;
                }
                if (this->buttonIndex > FS_BTN_MAIN_OPTIONS) {
                    this->buttonIndex = FS_BTN_MAIN_FILE_1;
                }
            }
        }

        if (this->buttonIndex == FS_BTN_MAIN_COPY) {
            if (!gSaveContext.flashSaveAvailable) {
                if (!NO_FLASH_SLOT_OCCUPIED(sramCtx, 0) && !NO_FLASH_SLOT_OCCUPIED(sramCtx, 1) &&
                    !NO_FLASH_SLOT_OCCUPIED(sramCtx, 2)) {
                    this->warningButtonIndex = this->buttonIndex;
                    this->warningLabel = FS_WARNING_NO_FILE_COPY;
                    this->emptyFileTextAlpha = 255;
                } else if (NO_FLASH_SLOT_OCCUPIED(sramCtx, 0) && NO_FLASH_SLOT_OCCUPIED(sramCtx, 1) &&
                           NO_FLASH_SLOT_OCCUPIED(sramCtx, 2)) {
                    this->warningButtonIndex = this->buttonIndex;
                    this->warningLabel = FS_WARNING_NO_EMPTY_FILES;
                    this->emptyFileTextAlpha = 255;
                } else {
                    this->warningLabel = FS_WARNING_NONE;
                }
            } else {
                if (!SLOT_OCCUPIED(this, 0) && !SLOT_OCCUPIED(this, 1) && !SLOT_OCCUPIED(this, 2)) {
                    this->warningButtonIndex = this->buttonIndex;
                    this->warningLabel = FS_WARNING_NO_FILE_COPY;
                    this->emptyFileTextAlpha = 255;
                } else if (SLOT_OCCUPIED(this, 0) && SLOT_OCCUPIED(this, 1) && SLOT_OCCUPIED(this, 2)) {
                    this->warningButtonIndex = this->buttonIndex;
                    this->warningLabel = FS_WARNING_NO_EMPTY_FILES;
                    this->emptyFileTextAlpha = 255;
                } else {
                    this->warningLabel = FS_WARNING_NONE;
                }
            }
        } else if (this->buttonIndex == FS_BTN_MAIN_ERASE) {
            if (!gSaveContext.flashSaveAvailable) {
                if (!NO_FLASH_SLOT_OCCUPIED(sramCtx, 0) && !NO_FLASH_SLOT_OCCUPIED(sramCtx, 1) &&
                    !NO_FLASH_SLOT_OCCUPIED(sramCtx, 2)) {
                    this->warningButtonIndex = this->buttonIndex;
                    this->warningLabel = FS_WARNING_NO_FILE_ERASE;
                    this->emptyFileTextAlpha = 255;
                } else {
                    this->warningLabel = FS_WARNING_NONE;
                }
            } else {
                if (!SLOT_OCCUPIED(this, 0) && !SLOT_OCCUPIED(this, 1) && !SLOT_OCCUPIED(this, 2)) {
                    this->warningButtonIndex = this->buttonIndex;
                    this->warningLabel = FS_WARNING_NO_FILE_ERASE;
                    this->emptyFileTextAlpha = 255;
                } else {
                    this->warningLabel = FS_WARNING_NONE;
                }
            }
        } else {
            this->warningLabel = FS_WARNING_NONE;
        }
    }
}

/**
 * Update function for `CM_UNUSED_31`
 */
void FileSelect_UnusedCM31(GameState* thisx) {
}

/**
 * Delay the next config mode from running until `D_80814564` reaches 254.
 * Because the timer increments by 2, the delay is 127 frames (assuming the value was not changed by reg editor).
 * Unused in the final game, was possibly used for debugging.
 * Update function for `CM_UNUSED_DELAY`
 */
void FileSelect_UnusedCMDelay(GameState* thisx) {
    static s16 D_80814564 = 0;
    FileSelectState* this = (FileSelectState*)thisx;

    D_80814564 += 2;

    if (D_80814564 == 254) {
        this->configMode = this->nextConfigMode;
        D_80814564 = 0;
    }
}

/**
 * Rotate the window from the main menu to the name entry menu.
 * Update function for `CM_ROTATE_TO_NAME_ENTRY`
 */
void FileSelect_RotateToNameEntry(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    if (!GameInteractor_Should(VB_FILE_SELECT_ROTATE_TO_NAME_ENTRY, true, this)) {
        return;
    }

    this->windowRot += 50.0f;

    if (this->windowRot >= 314.0f) {
        this->windowRot = 314.0f;
        this->configMode = CM_START_NAME_ENTRY;
    }
}

/**
 * Rotate the window from the main menu to the options menu.
 * Update function for `CM_MAIN_TO_OPTIONS`
 */
void FileSelect_RotateToOptions(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->windowRot += 50.0f;

    if (this->windowRot >= 314.0f) {
        this->windowRot = 314.0f;
        this->configMode = CM_START_OPTIONS;
    }
}

/**
 * Rotate the window from the options menu to the main menu.
 * Update function for `CM_NAME_ENTRY_TO_MAIN` and `CM_OPTIONS_TO_MAIN`
 */
void FileSelect_RotateToMain(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->windowRot += 50.0f;

    if (this->windowRot >= 628.0f) {
        this->windowRot = 0.0f;
        this->configMode = CM_MAIN_MENU;
    }
}

void (*sConfigModeUpdateFuncs[])(GameState*) = {
    // Main Menu
    FileSelect_StartFadeIn,    // CM_FADE_IN_START
    FileSelect_FinishFadeIn,   // CM_FADE_IN_END
    FileSelect_UpdateMainMenu, // CM_MAIN_MENU
    // Copy File
    FileSelect_SetupCopySource,      // CM_SETUP_COPY_SOURCE
    FileSelect_SelectCopySource,     // CM_SELECT_COPY_SOURCE
    FileSelect_SetupCopyDest1,       // CM_SETUP_COPY_DEST_1
    FileSelect_SetupCopyDest2,       // CM_SETUP_COPY_DEST_2
    FileSelect_SelectCopyDest,       // CM_SELECT_COPY_DEST
    FileSelect_ExitToCopySource1,    // CM_EXIT_TO_COPY_SOURCE_1
    FileSelect_ExitToCopySource2,    // CM_EXIT_TO_COPY_SOURCE_2
    FileSelect_SetupCopyConfirm1,    // CM_SETUP_COPY_CONFIRM_1
    FileSelect_SetupCopyConfirm2,    // CM_SETUP_COPY_CONFIRM_2
    FileSelect_CopyConfirm,          // CM_COPY_CONFIRM
    FileSelect_CopyWaitForFlashSave, // CM_COPY_WAIT_FOR_FLASH_SAVE
    FileSelect_ReturnToCopyDest,     // CM_RETURN_TO_COPY_DEST
    FileSelect_CopyAnim1,            // CM_COPY_ANIM_1
    FileSelect_CopyAnim2,            // CM_COPY_ANIM_2
    FileSelect_CopyAnim3,            // CM_COPY_ANIM_3
    FileSelect_CopyAnim4,            // CM_COPY_ANIM_4
    FileSelect_CopyAnim5,            // CM_COPY_ANIM_5
    FileSelect_ExitCopyToMain,       // CM_COPY_RETURN_MAIN
    // Erase File
    FileSelect_SetupEraseSelect,      // CM_SETUP_ERASE_SELECT
    FileSelect_EraseSelect,           // CM_ERASE_SELECT
    FileSelect_SetupEraseConfirm1,    // CM_SETUP_ERASE_CONFIRM_1
    FileSelect_SetupEraseConfirm2,    // CM_SETUP_ERASE_CONFIRM_2
    FileSelect_EraseConfirm,          // CM_ERASE_CONFIRM
    FileSelect_ExitToEraseSelect1,    // CM_EXIT_TO_ERASE_SELECT_1
    FileSelect_ExitToEraseSelect2,    // CM_EXIT_TO_ERASE_SELECT_2
    FileSelect_EraseAnim1,            // CM_ERASE_ANIM_1
    FileSelect_EraseWaitForFlashSave, // CM_ERASE_WAIT_FOR_FLASH_SAVE
    FileSelect_EraseAnim2,            // CM_ERASE_ANIM_2
    FileSelect_EraseAnim3,            // CM_ERASE_ANIM_3
    FileSelect_ExitEraseToMain,       // CM_EXIT_ERASE_TO_MAIN
    FileSelect_UnusedCM31,            // CM_UNUSED_31
    // New File Name Entry
    FileSelect_RotateToNameEntry,         // CM_ROTATE_TO_NAME_ENTRY
    FileSelect_StartNameEntry,            // CM_START_NAME_ENTRY
    FileSelect_UpdateKeyboardCursor,      // CM_NAME_ENTRY
    FileSelect_NameEntryWaitForFlashSave, // CM_NAME_ENTRY_WAIT_FOR_FLASH_SAVE
    FileSelect_RotateToMain,              // CM_NAME_ENTRY_TO_MAIN
    // Options
    FileSelect_RotateToOptions,         // CM_MAIN_TO_OPTIONS
    FileSelect_StartOptions,            // CM_START_OPTIONS
    FileSelect_UpdateOptionsMenu,       // CM_OPTIONS_MENU
    FileSelect_OptionsWaitForFlashSave, // CM_OPTIONS_WAIT_FOR_FLASH_SAVE
    FileSelect_RotateToMain,            // CM_OPTIONS_TO_MAIN
    // Possible Debug
    FileSelect_UnusedCMDelay, // CM_UNUSED_DELAY
    // 2S2H [Enhancement] New File Setup
    FileSelect_RotateToNewFileSetup, // CM_2S2H_ROTATE_TO_NEW_FILE_SETUP
    FileSelect_UpdateNewFileSetup,   // CM_2S2H_NEW_FILE_SETUP
    FileSelect_RotateToMain,         // CM_2S2H_NEW_FILE_SETUP_TO_MAIN
};

s16 sCursorAlphaTargets[] = { 70, 200 };

/**
 * Updates the alpha of the cursor to make it pulsate.
 */
void FileSelect_PulsateCursor(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s32 alphaStep =
        ABS_ALT(this->highlightColor[3] - sCursorAlphaTargets[this->highlightPulseDir]) / this->highlightTimer;

    if (this->highlightColor[3] >= sCursorAlphaTargets[this->highlightPulseDir]) {
        this->highlightColor[3] -= alphaStep;
    } else {
        this->highlightColor[3] += alphaStep;
    }

    this->highlightTimer--;

    if (this->highlightTimer == 0) {
        this->highlightColor[3] = sCursorAlphaTargets[this->highlightPulseDir];
        this->highlightTimer = 20;
        this->highlightPulseDir ^= 1;
    }
}

void FileSelect_ConfigModeUpdate(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    sConfigModeUpdateFuncs[this->configMode](&this->state);
}

void FileSelect_SetWindowVtx(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 i;
    s16 j;
    s16 x;
    s32 tmp;
    s32 tmp2;
    s32 tmp3;

    this->windowVtx = GRAPH_ALLOC(this->state.gfxCtx, 80 * sizeof(Vtx));
    tmp = this->windowPosX - 90;

    for (x = 0, i = 0; i < 4; i++) {
        tmp += 0x40;
        tmp2 = (i == 3) ? 0x30 : 0x40;

        for (j = 0, tmp3 = 0x50; j < 5; j++, x += 4, tmp3 -= 0x20) {
            this->windowVtx[x].v.ob[0] = this->windowVtx[x + 2].v.ob[0] = tmp;

            this->windowVtx[x + 1].v.ob[0] = this->windowVtx[x + 3].v.ob[0] = tmp2 + tmp;

            this->windowVtx[x].v.ob[1] = this->windowVtx[x + 1].v.ob[1] = tmp3;

            this->windowVtx[x + 2].v.ob[1] = this->windowVtx[x + 3].v.ob[1] = tmp3 - 0x20;

            this->windowVtx[x].v.ob[2] = this->windowVtx[x + 1].v.ob[2] = this->windowVtx[x + 2].v.ob[2] =
                this->windowVtx[x + 3].v.ob[2] = 0;

            this->windowVtx[x].v.flag = this->windowVtx[x + 1].v.flag = this->windowVtx[x + 2].v.flag =
                this->windowVtx[x + 3].v.flag = 0;

            this->windowVtx[x].v.tc[0] = this->windowVtx[x].v.tc[1] = this->windowVtx[x + 1].v.tc[1] =
                this->windowVtx[x + 2].v.tc[0] = 0;

            this->windowVtx[x + 1].v.tc[0] = this->windowVtx[x + 3].v.tc[0] = tmp2 << 5;

            this->windowVtx[x + 2].v.tc[1] = this->windowVtx[x + 3].v.tc[1] = 32 << 5;

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

s16 D_80814620[] = { 8, 8, 8, 0 };
s16 D_80814628[] = { 12, 12, 12, 0 };
s16 D_80814630[] = { 12, 12, 12, 0 };
s16 D_80814638[] = {
    88, 104, 120, 940, 944, 948,
};
s16 D_80814644[] = { 88, 104, 120, 944 };
s16 D_8081464C[] = { 940, 944 };
s16 D_80814650[] = { 940, 944 };

/*
 * fileSelect->windowContentVtx[0]    -> Title Label (4)
 *
 * fileSelect->windowContentVtx[4]    -> File 1 InfoBox (28)
 * fileSelect->windowContentVtx[32]   -> File 2 InfoBox (28)
 * fileSelect->windowContentVtx[60]   -> File 3 InfoBox (28)
 *
 * ** FILE 1 **
 *
 * fileSelect->windowContentVtx[88]   -> File Button
 * fileSelect->windowContentVtx[92]   -> File Name Box
 * fileSelect->windowContentVtx[96]   -> Connectors
 * fileSelect->windowContentVtx[100]  -> Blank Button (Owl Save)
 *
 * ** FILE 2 **
 *
 * fileSelect->windowContentVtx[104]  -> Repeat of File 1 above
 *
 * ** FILE 3 **
 *
 * fileSelect->windowContentVtx[120]  -> Repeat of File 1 above
 *
 * ** FILE 1 Info **
 *
 * fileSelect->windowContentVtx[136]  -> File Name (32)
 * fileSelect->windowContentVtx[168]  -> File Name Shadow (32)
 * fileSelect->windowContentVtx[200]  -> Rupee Digits (12)
 * fileSelect->windowContentVtx[212]  -> Rupee Digits Shadow (12)
 * fileSelect->windowContentVtx[224]  -> Mask Count (8)
 * fileSelect->windowContentVtx[232]  -> Mask Count Shadow (8)
 * fileSelect->windowContentVtx[240]  -> Hearts (80)
 * fileSelect->windowContentVtx[320]  -> Remains (16)
 * fileSelect->windowContentVtx[336]  -> Rupee Icon (4)
 * fileSelect->windowContentVtx[340]  -> Heart Piece Count (4)
 * fileSelect->windowContentVtx[344]  -> Mask Text (8)
 * fileSelect->windowContentVtx[352]  -> Owl Save Icon (4)
 * fileSelect->windowContentVtx[356]  -> Day Text (8)
 * fileSelect->windowContentVtx[364]  -> Time Digits (20)
 * fileSelect->windowContentVtx[384]  -> Time Digits Shadow (20)
 *
 * ** FILE 2 Info **
 *
 * fileSelect->windowContentVtx[404]  -> Repeat of File 1 above
 *
 * ** FILE 3 Info **
 *
 * fileSelect->windowContentVtx[672]  -> Repeat of File 1 above
 *
 * fileSelect->windowContentVtx[940]  -> Action buttons (copy/erase/yes/quit)
 * fileSelect->windowContentVtx[944]  -> Action buttons (copy/erase/yes/quit)
 * fileSelect->windowContentVtx[948]  -> Option Button
 * fileSelect->windowContentVtx[952]  -> Highlight over currently selected button
 * fileSelect->windowContentVtx[956]  -> Warning labels
 */

void FileSelect_SetWindowContentVtx(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    u16 vtxId;
    s16 j;
    s16 i;
    s16 spAC;
    u16 spA4[3];
    u16* ptr;
    s32 posY;
    s32 relPosY;
    s32 tempPosY;
    s32 posX;
    s32 index;

    this->windowContentVtx = GRAPH_ALLOC(this->state.gfxCtx, 960 * sizeof(Vtx));

    // Initialize all windowContentVtx
    for (vtxId = 0; vtxId < 960; vtxId += 4) {
        // x-coord (left)
        this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = 0x12C;
        // x-coord (right)
        this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
            this->windowContentVtx[vtxId + 0].v.ob[0] + 16;

        // y-coord (top)
        this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] = 0;
        // y-coord (bottom)
        this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
            this->windowContentVtx[vtxId + 0].v.ob[1] - 16;

        // z-coordinate
        this->windowContentVtx[vtxId + 0].v.ob[2] = this->windowContentVtx[vtxId + 1].v.ob[2] =
            this->windowContentVtx[vtxId + 2].v.ob[2] = this->windowContentVtx[vtxId + 3].v.ob[2] = 0;

        // flag
        this->windowContentVtx[vtxId + 0].v.flag = this->windowContentVtx[vtxId + 1].v.flag =
            this->windowContentVtx[vtxId + 2].v.flag = this->windowContentVtx[vtxId + 3].v.flag = 0;

        // texture coordinates
        this->windowContentVtx[vtxId + 0].v.tc[0] = this->windowContentVtx[vtxId + 0].v.tc[1] =
            this->windowContentVtx[vtxId + 1].v.tc[1] = this->windowContentVtx[vtxId + 2].v.tc[0] = 0;
        this->windowContentVtx[vtxId + 1].v.tc[0] = this->windowContentVtx[vtxId + 2].v.tc[1] =
            this->windowContentVtx[vtxId + 3].v.tc[0] = this->windowContentVtx[vtxId + 3].v.tc[1] = 0x200;

        // alpha
        this->windowContentVtx[vtxId + 0].v.cn[0] = this->windowContentVtx[vtxId + 1].v.cn[0] =
            this->windowContentVtx[vtxId + 2].v.cn[0] = this->windowContentVtx[vtxId + 3].v.cn[0] =
                this->windowContentVtx[vtxId + 0].v.cn[1] = this->windowContentVtx[vtxId + 1].v.cn[1] =
                    this->windowContentVtx[vtxId + 2].v.cn[1] = this->windowContentVtx[vtxId + 3].v.cn[1] =
                        this->windowContentVtx[vtxId + 0].v.cn[2] = this->windowContentVtx[vtxId + 1].v.cn[2] =
                            this->windowContentVtx[vtxId + 2].v.cn[2] = this->windowContentVtx[vtxId + 3].v.cn[2] =
                                this->windowContentVtx[vtxId + 0].v.cn[3] = this->windowContentVtx[vtxId + 1].v.cn[3] =
                                    this->windowContentVtx[vtxId + 2].v.cn[3] =
                                        this->windowContentVtx[vtxId + 3].v.cn[3] = 255;
    }

    /** Title Label **/

    // x-coord (left)
    this->windowContentVtx[0].v.ob[0] = this->windowContentVtx[2].v.ob[0] = this->windowPosX;
    // x-coord (right)
    this->windowContentVtx[1].v.ob[0] = this->windowContentVtx[3].v.ob[0] = this->windowContentVtx[0].v.ob[0] + 0x80;
    // y-coord (top)
    this->windowContentVtx[0].v.ob[1] = this->windowContentVtx[1].v.ob[1] = 0x48;
    // y-coord (bottom)
    this->windowContentVtx[2].v.ob[1] = this->windowContentVtx[3].v.ob[1] = this->windowContentVtx[0].v.ob[1] - 0x10;
    // texture coordinates
    this->windowContentVtx[1].v.tc[0] = this->windowContentVtx[3].v.tc[0] = 0x1000;

    /** File InfoBox **/

    // Loop through 3 files
    for (vtxId = 4, i = 0; i < 3; i++) {
        posX = this->windowPosX - 6;

        // Loop through 7 textures
        for (j = 0; j < 7; j++, vtxId += 4) {
            // x-coord (left)
            this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = posX;
            // x-coord (right)
            this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
                this->windowContentVtx[vtxId + 0].v.ob[0] + sFileInfoBoxPartWidths[j];

            // y-coord(top)
            this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] =
                this->fileNamesY[i] + 0x2C;
            // y-coord (bottom)
            this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
                this->windowContentVtx[vtxId + 0].v.ob[1] - 0x38;

            // texture coordinates
            this->windowContentVtx[vtxId + 1].v.tc[0] = this->windowContentVtx[vtxId + 3].v.tc[0] =
                sFileInfoBoxPartWidths[j] << 5;
            this->windowContentVtx[vtxId + 2].v.tc[1] = this->windowContentVtx[vtxId + 3].v.tc[1] = 0x700;

            // Update X position
            posX += sFileInfoBoxPartWidths[j];
        }
    }

    // File Buttons

    posX = this->windowPosX - 6;
    posY = 44;

    // Loop through 3 files
    for (j = 0; j < 3; j++, vtxId += 16, posY -= 0x10) {

        /* File Button */

        // x-coord (left)
        this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = posX;
        // x-coord (right)
        this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
            this->windowContentVtx[vtxId + 0].v.ob[0] + 0x40;

        // y-coord(top)
        this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] =
            this->buttonYOffsets[j] + posY;
        // y-coord (bottom)
        this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
            this->windowContentVtx[vtxId + 0].v.ob[1] - 0x10;

        // texture coordinates
        this->windowContentVtx[vtxId + 1].v.tc[0] = this->windowContentVtx[vtxId + 3].v.tc[0] = 0x800;

        /* File Name Box */

        // x-coord (left)
        this->windowContentVtx[vtxId + 4].v.ob[0] = this->windowContentVtx[vtxId + 6].v.ob[0] = posX + 0x40;
        // x-coord (right)
        this->windowContentVtx[vtxId + 5].v.ob[0] = this->windowContentVtx[vtxId + 7].v.ob[0] =
            this->windowContentVtx[vtxId + 4].v.ob[0] + 0x6C;

        // y-coord(top)
        this->windowContentVtx[vtxId + 4].v.ob[1] = this->windowContentVtx[vtxId + 5].v.ob[1] =
            this->buttonYOffsets[j] + posY;
        // y-coord (bottom)
        this->windowContentVtx[vtxId + 6].v.ob[1] = this->windowContentVtx[vtxId + 7].v.ob[1] =
            this->windowContentVtx[vtxId + 4].v.ob[1] - 0x10;

        // texture coordinates
        this->windowContentVtx[vtxId + 5].v.tc[0] = this->windowContentVtx[vtxId + 7].v.tc[0] = 0xD80;

        /* Connectors */

        // x-coord (left)
        this->windowContentVtx[vtxId + 8].v.ob[0] = this->windowContentVtx[vtxId + 10].v.ob[0] = posX + 0x34;
        // x-coord (right)
        this->windowContentVtx[vtxId + 9].v.ob[0] = this->windowContentVtx[vtxId + 11].v.ob[0] =
            this->windowContentVtx[vtxId + 8].v.ob[0] + 0x18;

        // y-coord(top)
        this->windowContentVtx[vtxId + 8].v.ob[1] = this->windowContentVtx[vtxId + 9].v.ob[1] =
            this->buttonYOffsets[j] + posY;
        // y-coord (bottom)
        this->windowContentVtx[vtxId + 10].v.ob[1] = this->windowContentVtx[vtxId + 11].v.ob[1] =
            this->windowContentVtx[vtxId + 8].v.ob[1] - 0x10;

        // texture coordinates
        this->windowContentVtx[vtxId + 9].v.tc[0] = this->windowContentVtx[vtxId + 11].v.tc[0] = 0x300;

        /* Blank Button (Owl Save) */

        // x-coord (left)
        this->windowContentVtx[vtxId + 12].v.ob[0] = this->windowContentVtx[vtxId + 14].v.ob[0] = posX + 0xA9;
        // x-coord (right)
        this->windowContentVtx[vtxId + 13].v.ob[0] = this->windowContentVtx[vtxId + 15].v.ob[0] =
            this->windowContentVtx[vtxId + 12].v.ob[0] + 0x34;

        // y-coord(top)
        this->windowContentVtx[vtxId + 12].v.ob[1] = this->windowContentVtx[vtxId + 13].v.ob[1] =
            this->buttonYOffsets[j] + posY;
        // y-coord (bottom)
        this->windowContentVtx[vtxId + 14].v.ob[1] = this->windowContentVtx[vtxId + 15].v.ob[1] =
            this->windowContentVtx[vtxId + 12].v.ob[1] - 0x10;

        // texture coordinates
        this->windowContentVtx[vtxId + 13].v.tc[0] = this->windowContentVtx[vtxId + 15].v.tc[0] = 0x680;
    }

    posY = 44;

    // Loop through 3 files
    for (j = 0; j < 3; j++, posY -= 16) {
        if (!gSaveContext.flashSaveAvailable) {
            // Should skip vtxId
            // vtxId += 268;
            continue;
        }

        // Account for owl-save offset

        spAC = j;
        if (this->isOwlSave[j + FILE_NUM_OWL_SAVE_OFFSET]) {
            spAC = j + FILE_NUM_OWL_SAVE_OFFSET;
        }

        /* File name */

        posX = this->windowPosX - 6;

        if ((this->configMode == 0x10) && (j == this->copyDestFileIndex)) {
            relPosY = this->fileNamesY[j] + 0x2C;
        } else if (((this->configMode == 0x11) || (this->configMode == 0x12)) && (j == this->copyDestFileIndex)) {
            relPosY = this->buttonYOffsets[j] + posY;
        } else {
            relPosY = posY + this->buttonYOffsets[j] + this->fileNamesY[j];
        }

        tempPosY = relPosY - 2;

        // Loop through 8 characters of file name
        for (i = 0; i < 8; i++, vtxId += 4) {

            index = this->fileNames[j][i];

            /* File Name */

            // x-coord (left)
            this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] =
                D_80814280[index] + posX + 0x4E;
            // x-coord (right)
            this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
                this->windowContentVtx[vtxId + 0].v.ob[0] + 0xB;

            // y-coord(top)
            this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] = tempPosY;
            // y-coord (bottom)
            this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
                this->windowContentVtx[vtxId + 0].v.ob[1] - 0xC;

            /* File Name Shadow */

            // x-coord (left)
            this->windowContentVtx[vtxId + 32].v.ob[0] = this->windowContentVtx[vtxId + 34].v.ob[0] =
                D_80814280[index] + posX + 0x4F;
            // x-coord (right)
            this->windowContentVtx[vtxId + 33].v.ob[0] = this->windowContentVtx[vtxId + 35].v.ob[0] =
                this->windowContentVtx[vtxId + 32].v.ob[0] + 0xB;

            // y-coord(top)
            this->windowContentVtx[vtxId + 32].v.ob[1] = this->windowContentVtx[vtxId + 33].v.ob[1] = tempPosY - 1;
            // y-coord (bottom)
            this->windowContentVtx[vtxId + 34].v.ob[1] = this->windowContentVtx[vtxId + 35].v.ob[1] =
                this->windowContentVtx[vtxId + 32].v.ob[1] - 0xC;

            // Update X position
            posX += 10;
        }
        // Account for the shadow
        vtxId += 32;

        /* Rupee Digits */

        posX = this->windowPosX + 14;
        tempPosY = relPosY - 0x18;

        FileSelect_SplitNumber(this->rupees[spAC], &spA4[0], &spA4[1], &spA4[2]);

        index = sWalletFirstDigit[this->walletUpgrades[spAC]];

        ptr = &spA4[index];

        for (i = 0; i < 3; i++, vtxId += 4, ptr++) {

            /* Rupee Digits */

            // x-coord (left)
            this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] =
                D_80814280[spA4[i]] + posX;
            // x-coord (right)
            this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
                this->windowContentVtx[vtxId + 0].v.ob[0] + D_80814628[i];

            // y-coord(top)
            this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] = tempPosY;
            // y-coord (bottom)
            this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
                this->windowContentVtx[vtxId + 0].v.ob[1] - D_80814630[i];

            /* Rupee Digits Shadow */

            // x-coord (left)
            this->windowContentVtx[vtxId + 12].v.ob[0] = this->windowContentVtx[vtxId + 14].v.ob[0] =
                this->windowContentVtx[vtxId + 0].v.ob[0] + 1;
            // x-coord (right)
            this->windowContentVtx[vtxId + 13].v.ob[0] = this->windowContentVtx[vtxId + 15].v.ob[0] =
                this->windowContentVtx[vtxId + 12].v.ob[0] + D_80814628[i];

            // y-coord(top)
            this->windowContentVtx[vtxId + 12].v.ob[1] = this->windowContentVtx[vtxId + 13].v.ob[1] = tempPosY - 1;
            // y-coord (bottom)
            this->windowContentVtx[vtxId + 14].v.ob[1] = this->windowContentVtx[vtxId + 15].v.ob[1] =
                this->windowContentVtx[vtxId + 12].v.ob[1] - D_80814630[i];

            // Update X position
            posX += D_80814620[i];
        }

        // Account for the shadow
        vtxId += 12;

        /* Mask Count */

        posX = this->windowPosX + 42;
        tempPosY = relPosY - 0x2A;

        FileSelect_SplitNumber(this->maskCount[spAC], &spA4[0], &spA4[1], &spA4[2]);

        for (i = 1; i < 3; i++, vtxId += 4) {

            /* Mask Count */

            // x-coord (left)
            this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] =
                D_80814280[spA4[i]] + posX;
            // x-coord (right)
            this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
                this->windowContentVtx[vtxId + 0].v.ob[0] + D_80814628[i];

            // y-coord(top)
            this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] = tempPosY;
            // y-coord (bottom)
            this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
                this->windowContentVtx[vtxId + 0].v.ob[1] - D_80814630[i];

            /* Mask Count Shadow */

            // x-coord (left)
            this->windowContentVtx[vtxId + 8].v.ob[0] = this->windowContentVtx[vtxId + 10].v.ob[0] =
                this->windowContentVtx[vtxId + 0].v.ob[0] + 1;
            // x-coord (right)
            this->windowContentVtx[vtxId + 9].v.ob[0] = this->windowContentVtx[vtxId + 11].v.ob[0] =
                this->windowContentVtx[vtxId + 8].v.ob[0] + D_80814628[i];

            // y-coord(top)
            this->windowContentVtx[vtxId + 8].v.ob[1] = this->windowContentVtx[vtxId + 9].v.ob[1] = tempPosY - 1;
            // y-coord (bottom)
            this->windowContentVtx[vtxId + 10].v.ob[1] = this->windowContentVtx[vtxId + 11].v.ob[1] =
                this->windowContentVtx[vtxId + 8].v.ob[1] - D_80814630[i];

            // Update X position
            posX += D_80814620[i];
        }

        // Account for the shadow
        vtxId += 8;

        /* Hearts */

        posX = this->windowPosX + 63;
        tempPosY = relPosY - 0x10;

        // Loop through 20 hearts
        for (i = 0; i < 20; i++, vtxId += 4, posX += 9) {
            // x-coord (left)
            this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = posX;
            // x-coord (right)
            this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
                this->windowContentVtx[vtxId + 0].v.ob[0] + 0xA;

            // y-coord(top)
            this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] = tempPosY;
            // y-coord (bottom)
            this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
                this->windowContentVtx[vtxId + 0].v.ob[1] - 0xA;

            // New row of hearts next iteration
            if (i == 9) {
                posX = this->windowPosX + (63 - 9);
                tempPosY -= 8;
            }
        }

        /* Quest Remains */

        posX = this->windowPosX + 64;
        tempPosY = relPosY - 0x20;

        // Loop through 4 Remains
        for (i = 0; i < 4; i++, vtxId += 4, posX += 0x18) {
            // x-coord (left)
            this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = posX;
            // x-coord (right)
            this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
                this->windowContentVtx[vtxId + 0].v.ob[0] + 0x14;

            // y-coord(top)
            this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] = tempPosY;
            // y-coord (bottom)
            this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
                this->windowContentVtx[vtxId + 0].v.ob[1] - 0x14;

            // texture coordinates
            this->windowContentVtx[vtxId + 1].v.tc[0] = this->windowContentVtx[vtxId + 2].v.tc[1] =
                this->windowContentVtx[vtxId + 3].v.tc[0] = this->windowContentVtx[vtxId + 3].v.tc[1] = 0x400;
        }

        /* Rupee Icon */

        // posX = this->windowPosX - 1;
        tempPosY = relPosY - 0x15;

        // x-coord (left)
        this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = this->windowPosX - 1;
        // x-coord (right)
        this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
            this->windowContentVtx[vtxId + 0].v.ob[0] + 0x10;

        // y-coord(top)
        this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] = tempPosY;
        // y-coord (bottom)
        this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
            this->windowContentVtx[vtxId + 0].v.ob[1] - 0x10;

        // texture coordinates
        this->windowContentVtx[vtxId + 1].v.tc[0] = this->windowContentVtx[vtxId + 3].v.tc[0] = 0x200;
        this->windowContentVtx[vtxId + 2].v.tc[1] = this->windowContentVtx[vtxId + 3].v.tc[1] = 0x200;

        vtxId += 4;

        /* Heart Piece Count */

        // posX = this->windowPosX + 0x27;
        tempPosY = relPosY - 0x15;

        // x-coord (left)
        this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = this->windowPosX + 0x27;
        // x-coord (right)
        this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
            this->windowContentVtx[vtxId + 0].v.ob[0] + 0x18;

        // y-coord(top)
        this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] = tempPosY;
        // y-coord (bottom)
        this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
            this->windowContentVtx[vtxId + 0].v.ob[1] - 0x10;

        // texture coordinates
        this->windowContentVtx[vtxId + 1].v.tc[0] = this->windowContentVtx[vtxId + 3].v.tc[0] = 0x300;
        this->windowContentVtx[vtxId + 2].v.tc[1] = this->windowContentVtx[vtxId + 3].v.tc[1] = 0x200;

        vtxId += 4;

        /* Mask Text */

        // posX = this->windowPosX - 10;
        tempPosY = relPosY - 0x27;

        this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = this->windowPosX - 10;

        this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
            this->windowContentVtx[vtxId + 0].v.ob[0] + 0x40;

        this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] = tempPosY;

        this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
            this->windowContentVtx[vtxId + 0].v.ob[1] - 0x10;

        this->windowContentVtx[vtxId + 1].v.tc[0] = this->windowContentVtx[vtxId + 3].v.tc[0] = 0x800;
        this->windowContentVtx[vtxId + 2].v.tc[1] = this->windowContentVtx[vtxId + 3].v.tc[1] = 0x200;

        this->windowContentVtx[vtxId + 4].v.ob[0] = this->windowContentVtx[vtxId + 6].v.ob[0] =
            this->windowContentVtx[vtxId + 0].v.ob[0] + 1;

        this->windowContentVtx[vtxId + 5].v.ob[0] = this->windowContentVtx[vtxId + 7].v.ob[0] =
            this->windowContentVtx[vtxId + 4].v.ob[0] + 0x40;

        this->windowContentVtx[vtxId + 4].v.ob[1] = this->windowContentVtx[vtxId + 5].v.ob[1] = tempPosY - 1;

        this->windowContentVtx[vtxId + 6].v.ob[1] = this->windowContentVtx[vtxId + 7].v.ob[1] =
            this->windowContentVtx[vtxId + 4].v.ob[1] - 0x10;

        this->windowContentVtx[vtxId + 5].v.tc[0] = this->windowContentVtx[vtxId + 7].v.tc[0] = 0x800;
        this->windowContentVtx[vtxId + 6].v.tc[1] = this->windowContentVtx[vtxId + 7].v.tc[1] = 0x200;

        vtxId += 8;

        /* Owl Save Icon */

        posX = this->windowPosX + 0xA3;

        if ((this->configMode == 0x10) && (j == this->copyDestFileIndex)) {
            tempPosY = this->fileNamesY[j] + 0x2C;
        } else if (((this->configMode == 0x11) || (this->configMode == 0x12)) && (j == this->copyDestFileIndex)) {
            tempPosY = this->buttonYOffsets[j] + posY;
        } else {
            tempPosY = posY + this->buttonYOffsets[j] + this->fileNamesY[j];
        }

        this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = posX + 0xE;

        this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
            this->windowContentVtx[vtxId + 0].v.ob[0] + 0x18;

        this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] = tempPosY - 2;

        this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
            this->windowContentVtx[vtxId + 0].v.ob[1] - 0xC;

        this->windowContentVtx[vtxId + 1].v.tc[0] = this->windowContentVtx[vtxId + 3].v.tc[0] = 0x300;
        this->windowContentVtx[vtxId + 2].v.tc[1] = this->windowContentVtx[vtxId + 3].v.tc[1] = 0x180;

        vtxId += 4;

        /* Day Text */

        for (i = 0; i < 2; i++, vtxId += 4) {

            this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = 2 + posX + i;

            this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
                this->windowContentVtx[vtxId + 0].v.ob[0] + 0x30;

            this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] = tempPosY - i - 0x12;

            this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
                this->windowContentVtx[vtxId + 0].v.ob[1] - 0x18;

            this->windowContentVtx[vtxId + 1].v.tc[0] = this->windowContentVtx[vtxId + 3].v.tc[0] = 0x600;

            this->windowContentVtx[vtxId + 2].v.tc[1] = this->windowContentVtx[vtxId + 3].v.tc[1] = 0x300;
        }

        /* Time Digits */

        posX += 6;
        index = vtxId;

        for (i = 0; i < 5; i++, vtxId += 4, posX += 8) {

            this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = posX;

            this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
                this->windowContentVtx[vtxId + 0].v.ob[0] + 0xC;

            this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] = tempPosY - 0x2A;

            this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
                this->windowContentVtx[vtxId + 0].v.ob[1] - 0xC;

            this->windowContentVtx[vtxId + 0x14].v.ob[0] = this->windowContentVtx[vtxId + 0x16].v.ob[0] = posX + 1;

            this->windowContentVtx[vtxId + 0x15].v.ob[0] = this->windowContentVtx[vtxId + 0x17].v.ob[0] =
                this->windowContentVtx[vtxId + 0x14].v.ob[0] + 0xC;

            this->windowContentVtx[vtxId + 0x14].v.ob[1] = this->windowContentVtx[vtxId + 0x15].v.ob[1] =
                tempPosY - 0x2B;

            this->windowContentVtx[vtxId + 0x16].v.ob[1] = this->windowContentVtx[vtxId + 0x17].v.ob[1] =
                this->windowContentVtx[vtxId + 0x14].v.ob[1] - 0xC;
        }

        // Adjust the colon to the right
        this->windowContentVtx[index + 8].v.ob[0] = this->windowContentVtx[index + 10].v.ob[0] =
            this->windowContentVtx[index + 8].v.ob[0] + 3;

        this->windowContentVtx[index + 9].v.ob[0] = this->windowContentVtx[index + 11].v.ob[0] =
            this->windowContentVtx[index + 8].v.ob[0] + 0xC;

        this->windowContentVtx[index + 0x1C].v.ob[0] = this->windowContentVtx[index + 0x1E].v.ob[0] =
            this->windowContentVtx[index + 8].v.ob[0] + 1;

        this->windowContentVtx[index + 0x1D].v.ob[0] = this->windowContentVtx[index + 0x1F].v.ob[0] =
            this->windowContentVtx[index + 0x1C].v.ob[0] + 0xC;

        vtxId += 20;
    }

    posX = this->windowPosX - 6;
    posY = -0xC;

    for (j = 0; j < 2; j++, vtxId += 4, posY -= 0x10) {

        this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = posX;

        this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
            this->windowContentVtx[vtxId + 0].v.ob[0] + 0x40;

        this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] =
            this->buttonYOffsets[j + 3] + posY;

        this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
            this->windowContentVtx[vtxId + 0].v.ob[1] - 0x10;

        this->windowContentVtx[vtxId + 1].v.tc[0] = this->windowContentVtx[vtxId + 3].v.tc[0] = 0x800;
    }

    this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = posX;

    this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
        this->windowContentVtx[vtxId + 0].v.ob[0] + 0x40;

    this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] =
        this->buttonYOffsets[5] - 0x34;

    this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
        this->windowContentVtx[vtxId + 0].v.ob[1] - 0x10;

    this->windowContentVtx[vtxId + 1].v.tc[0] = this->windowContentVtx[vtxId + 3].v.tc[0] = 0x800;

    vtxId += 4;

    if (((this->menuMode == FS_MENU_MODE_CONFIG) && (this->configMode >= 2)) ||
        ((this->menuMode == FS_MENU_MODE_SELECT) && (this->selectMode == 3))) {
        if (this->menuMode == FS_MENU_MODE_CONFIG) {
            if ((this->configMode == 4) || (this->configMode == 7) || (this->configMode == 0x16)) {
                j = D_80814644[this->buttonIndex];
            } else if ((this->configMode == 0x19) || (this->configMode == 0xC)) {
                j = D_8081464C[this->buttonIndex];
            } else {
                j = D_80814638[this->buttonIndex];
            }
        } else {
            j = D_80814650[this->confirmButtonIndex];
        }

        this->windowContentVtx[vtxId + 0].v.ob[0] = this->windowContentVtx[vtxId + 2].v.ob[0] = this->windowPosX - 0xA;

        this->windowContentVtx[vtxId + 1].v.ob[0] = this->windowContentVtx[vtxId + 3].v.ob[0] =
            this->windowContentVtx[vtxId + 0].v.ob[0] + 0x48;

        this->windowContentVtx[vtxId + 0].v.ob[1] = this->windowContentVtx[vtxId + 1].v.ob[1] =
            this->windowContentVtx[j].v.ob[1] + 4;

        this->windowContentVtx[vtxId + 2].v.ob[1] = this->windowContentVtx[vtxId + 3].v.ob[1] =
            this->windowContentVtx[vtxId + 0].v.ob[1] - 0x18;

        this->windowContentVtx[vtxId + 1].v.tc[0] = this->windowContentVtx[vtxId + 3].v.tc[0] = 0x900;

        this->windowContentVtx[vtxId + 2].v.tc[1] = this->windowContentVtx[vtxId + 3].v.tc[1] = 0x300;
    }

    this->windowContentVtx[vtxId + 4].v.ob[0] = this->windowContentVtx[vtxId + 6].v.ob[0] = this->windowPosX + 0x3A;

    this->windowContentVtx[vtxId + 5].v.ob[0] = this->windowContentVtx[vtxId + 7].v.ob[0] =
        this->windowContentVtx[vtxId + 4].v.ob[0] + 0x80;

    this->windowContentVtx[vtxId + 4].v.ob[1] = this->windowContentVtx[vtxId + 5].v.ob[1] =
        this->windowContentVtx[D_80814638[this->warningButtonIndex]].v.ob[1];

    this->windowContentVtx[vtxId + 6].v.ob[1] = this->windowContentVtx[vtxId + 7].v.ob[1] =
        this->windowContentVtx[vtxId + 4].v.ob[1] - 0x10;

    this->windowContentVtx[vtxId + 5].v.tc[0] = this->windowContentVtx[vtxId + 7].v.tc[0] = 0x1000;
}

u16 D_80814654[] = {
    0x88,
    0x194,
    0x2A0,
};

TexturePtr sFileSelRemainsTextures[] = {
    gFileSelOdolwasRemainsTex,
    gFileSelGohtsRemainsTex,
    gFileSelGyorgsRemainsTex,
    gFileSelTwinmoldsRemainsTex,
};

// 2S2H [Port] When making an owl save on day 4 the texture was currupt. We also decided to add a texture for going past
// day 4.

TexturePtr sFileSelDayENGTextures[] = {
    gFileSelFirstDayENGTex, gFileSelFirstDayENGTex, gFileSelSecondDayENGTex,
    gFileSelFinalDayENGTex, gFileSelFourthDayTex,   gFileSelCheatingDayTex,
};

TexturePtr sFileSelHeartPieceTextures[] = {
    gFileSel0QuarterHeartENGTex,
    gFileSel1QuarterHeartENGTex,
    gFileSel2QuarterHeartENGTex,
    gFileSel3QuarterHeartENGTex,
};
static TexturePtr sHeartTextures[2][5] = {
    {
        gHeartEmptyTex,
        gHeartQuarterTex,
        gHeartHalfTex,
        gHeartThreeQuarterTex,
        gHeartFullTex,
    },
    {
        gDefenseHeartEmptyTex,
        gDefenseHeartQuarterTex,
        gDefenseHeartHalfTex,
        gDefenseHeartThreeQuarterTex,
        gDefenseHeartFullTex,
    },
};
u8 sHealthToQuarterHeartCount[] = {
    0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3,
};
s16 sFileSelRupeePrimColors[3][3] = {
    { 200, 255, 100 }, // Default Wallet
    { 170, 170, 255 }, // Adult Wallet
    { 255, 105, 105 }, // Giant Wallet
};
s16 sFileSelRupeeEnvColors[3][3] = {
    { 0, 80, 0 },   // Default Wallet
    { 10, 10, 80 }, // Adult Wallet
    { 40, 10, 0 },  // Giant Wallet
};
static s16 sHeartPrimColors[2][3] = {
    { 255, 70, 50 },
    { 200, 0, 0 },
};
static s16 sHeartEnvColors[2][3] = {
    { 50, 40, 60 },
    { 255, 255, 255 },
};

void FileSelect_DrawFileInfo(GameState* thisx, s16 fileIndex) {
    FileSelectState* this = (FileSelectState*)thisx;
    Font* font = &this->font;
    s16 j;
    s16 vtxOffset;
    s32 heartType;
    s16 i;
    s16 sp20C;
    s16 health;
    s16 timeDigits[5];
    u16 digits[3]; // rupees and mask count
    u8 quarterHeartCount;

    OPEN_DISPS(this->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);

    sp20C = fileIndex;

    // draw file name
    if (this->nameAlpha[fileIndex] != 0) {
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + (4 * 8)], 32, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, this->nameAlpha[fileIndex]);

        for (vtxOffset = 0, i = 0; vtxOffset < (4 * 8); i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx,
                                     font->fontBuf + this->fileNames[fileIndex][i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex]], 32, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->nameAlpha[fileIndex]);

        for (vtxOffset = 0, i = 0; vtxOffset < (4 * 8); i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx,
                                     font->fontBuf + this->fileNames[fileIndex][i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }
    }

    if ((fileIndex == this->selectedFileIndex) || (fileIndex == this->copyDestFileIndex)) {
        if (this->isOwlSave[fileIndex + FILE_NUM_OWL_SAVE_OFFSET]) {
            sp20C = fileIndex + FILE_NUM_OWL_SAVE_OFFSET;
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, sFileSelRupeePrimColors[this->walletUpgrades[sp20C]][0],
                        sFileSelRupeePrimColors[this->walletUpgrades[sp20C]][1],
                        sFileSelRupeePrimColors[this->walletUpgrades[sp20C]][2], this->fileInfoAlpha[fileIndex]);
        gDPSetEnvColor(POLY_OPA_DISP++, sFileSelRupeeEnvColors[this->walletUpgrades[sp20C]][0],
                       sFileSelRupeeEnvColors[this->walletUpgrades[sp20C]][1],
                       sFileSelRupeeEnvColors[this->walletUpgrades[sp20C]][2], 255);

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xC8], 4, 0);

        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelRupeeTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, this->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0x4C], 12, 0);

        FileSelect_SplitNumber((u16)this->rupees[sp20C], &digits[0], &digits[1], &digits[2]);

        for (vtxOffset = 0, i = sWalletFirstDigit[this->walletUpgrades[sp20C]]; i < 3; i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + digits[i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }

        if (this->rupees[sp20C] == gUpgradeCapacities[4][this->walletUpgrades[sp20C]]) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 120, 255, 0, this->fileInfoAlpha[fileIndex]);
        } else if (this->rupees[sp20C] != 0) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 100, 100, 100, this->fileInfoAlpha[fileIndex]);
        }

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0x40], 12, 0);

        for (vtxOffset = 0, i = sWalletFirstDigit[this->walletUpgrades[sp20C]]; i < 3; i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + digits[i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 0, 0, this->fileInfoAlpha[fileIndex]);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xCC], 4, 0);

        POLY_OPA_DISP = FileSelect_DrawTexQuadIA8(
            POLY_OPA_DISP, sFileSelHeartPieceTextures[this->heartPieceCount[sp20C]], 0x18, 0x10, 0);

        if (this->defenseHearts[sp20C] == 0) {
            heartType = 0;
        } else {
            heartType = 1;
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, sHeartPrimColors[heartType][0], sHeartPrimColors[heartType][1],
                        sHeartPrimColors[heartType][2], this->fileInfoAlpha[fileIndex]);
        gDPSetEnvColor(POLY_OPA_DISP++, sHeartEnvColors[heartType][0], sHeartEnvColors[heartType][1],
                       sHeartEnvColors[heartType][2], 255);

        i = this->healthCapacity[sp20C] / 0x10;

        health = this->health[sp20C];
        if (health <= 0x30) {
            health = 0x30;
        }

        quarterHeartCount = 4;
        for (vtxOffset = 0, j = 0; j < i; j++, vtxOffset += 4) {
            if (health < 0x10) {
                if (health != 0) {
                    quarterHeartCount = sHealthToQuarterHeartCount[health];
                    health = 0;
                } else {
                    quarterHeartCount = 0;
                }
            } else {
                health -= 0x10;
            }

            gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0x68 + vtxOffset], 4, 0);
            POLY_OPA_DISP =
                FileSelect_DrawTexQuadIA8(POLY_OPA_DISP, sHeartTextures[heartType][quarterHeartCount], 0x10, 0x10, 0);
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

        for (vtxOffset = 0, j = 0; j < 4; j++, vtxOffset += 4) {
            if (this->questItems[sp20C] & gBitFlags[j]) {
                gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xB8 + vtxOffset], 4, 0);
                gDPLoadTextureBlock(POLY_OPA_DISP++, sFileSelRemainsTextures[j], G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, 0,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                    G_TX_NOLOD, G_TX_NOLOD);
                gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
            }
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, this->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xD0], 8, 0);
        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelMASKSENGTex, G_IM_FMT_I, 64, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);
        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

        gDPPipeSync(POLY_OPA_DISP++);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, this->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0x60], 8, 0);

        FileSelect_SplitNumber(this->maskCount[sp20C], &digits[0], &digits[1], &digits[2]);

        for (vtxOffset = 0, i = 1; i < 3; i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + digits[i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0x58], 8, 0);

        for (vtxOffset = 0, i = 1; i < 3; i++, vtxOffset += 4) {

            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + digits[i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }
    }

    if (GameInteractor_Should(VB_DRAW_FILE_SELECT_EXTRA_INFO_DETAILS,
                              this->isOwlSave[fileIndex + FILE_NUM_OWL_SAVE_OFFSET], fileIndex)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->nameAlpha[fileIndex]);

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xD8], 4, 0);

        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelOwlSaveIconTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 24, 12, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

        if (GameInteractor_Should(VB_DRAW_FILE_SELECT_OWL_SAVE_ICON, true, fileIndex)) {
            gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xDC], 8, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, this->fileInfoAlpha[fileIndex]);
        // 2S2H [Port] When making an owl save on day 4 the texture was currupt. We also decided to add a texture for
        // going past day 4.
        uint32_t dayIndex = CLAMP_MAX(this->day[sp20C], 5);

        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, sFileSelDayENGTextures[dayIndex], G_IM_FMT_I, 48, 24, 0,
                               G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);
        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

        timeDigits[0] = 0;
        timeDigits[1] = TIME_TO_MINUTES_F(this->time[sp20C]) / 60.0f;

        while (timeDigits[1] >= 10) {
            timeDigits[0]++;
            timeDigits[1] -= 10;
        }

        timeDigits[3] = 0;
        timeDigits[4] = (s32)TIME_TO_MINUTES_F(this->time[sp20C]) % 60;

        while (timeDigits[4] >= 10) {
            timeDigits[3]++;
            timeDigits[4] -= 10;
        }
        timeDigits[2] = 0x41;

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, this->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xF8], 20, 0);

        for (i = 0, vtxOffset = 0; i < 5; i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + timeDigits[i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xE4], 20, 0);

        for (i = 0, vtxOffset = 0; i < 5; i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + timeDigits[i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);

    CLOSE_DISPS(this->state.gfxCtx);
}

TexturePtr sFileInfoBoxTextures[] = {
    gFileSelFileInfoBox0Tex, gFileSelFileInfoBox1Tex,      gFileSelFileInfoBox2Tex,      gFileSelFileInfoBox3Tex,
    gFileSelFileInfoBox4Tex, gFileSelFileExtraInfoBox0Tex, gFileSelFileExtraInfoBox1Tex,
};

TexturePtr sTitleLabels[] = {
    gFileSelPleaseSelectAFileENGTex, gFileSelOpenThisFileENGTex,    gFileSelCopyWhichFileENGTex,
    gFileSelCopyToWhichFileENGTex,   gFileSelAreYouSureCopyENGTex,  gFileSelFileCopiedENGTex,
    gFileSelEraseWhichFileENGTex,    gFileSelAreYouSureEraseENGTex, gFileSelFileErasedENGTex,
};

TexturePtr sWarningLabels[] = {
    gFileSelNoFileToCopyENGTex, gFileSelNoFileToEraseENGTex, gFileSelNoEmptyFileENGTex,
    gFileSelFileEmptyENGTex,    gFileSelFileInUseENGTex,
};

TexturePtr sWarningLabelsIA4[] = {
    gFileSelNoFileToCopyIA4ENGTex, gFileSelNoFileToEraseIA4ENGTex, gFileSelNoEmptyFileIA4ENGTex,
    gFileSelFileEmptyIA4ENGTex,    gFileSelFileInUseIA4ENGTex,
};

TexturePtr sFileButtonTextures[] = {
    gFileSelFile1ButtonENGTex,
    gFileSelFile2ButtonENGTex,
    gFileSelFile3ButtonENGTex,
};

TexturePtr sActionButtonTextures[] = {
    gFileSelCopyButtonENGTex,
    gFileSelEraseButtonENGTex,
    gFileSelYesButtonENGTex,
    gFileSelQuitButtonENGTex,
};

/**
 * Draw most window contents including buttons, labels, and icons.
 * Does not include anything from the keyboard and settings windows.
 */
void FileSelect_DrawWindowContents(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 fileIndex;
    s16 temp;
    s16 i;
    s16 quadVtxIndex;

    OPEN_DISPS(this->state.gfxCtx);

    // draw title label
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[0], 4, 0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, sTitleLabels[this->titleLabel], G_IM_FMT_IA, G_IM_SIZ_8b, 128, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    // draw next title label
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_NEXT]);
    gDPLoadTextureBlock(POLY_OPA_DISP++, sTitleLabels[this->nextTitleLabel], G_IM_FMT_IA, G_IM_SIZ_8b, 128, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    temp = 4;

    gDPPipeSync(POLY_OPA_DISP++);

    // draw file info box (large box when a file is selected)
    for (fileIndex = 0; fileIndex < 3; fileIndex++, temp += 28) {
        if (fileIndex < FILE_NUM_MAX) {
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1],
                                    this->windowColor[2], this->fileInfoAlpha[fileIndex],
                                    COSMETIC_ID("Menus.FilePlates"));
            gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[temp], 28, 0);

            for (quadVtxIndex = 0, i = 0; i < 7; i++, quadVtxIndex += 4) {
                if ((i < 5) || (this->isOwlSave[fileIndex + FILE_NUM_OWL_SAVE_OFFSET] && (i >= 5))) {
                    gDPLoadTextureBlock(POLY_OPA_DISP++, sFileInfoBoxTextures[i], G_IM_FMT_IA, G_IM_SIZ_16b,
                                        sFileInfoBoxPartWidths[i], 56, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                    gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1,
                                   0);
                }
            }
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);

    for (i = 0; i < 3; i++, temp += 16) {
        if (i < FILE_NUM_MAX) {
            // draw file button
            gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[temp], 16, 0);
            gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, sWindowContentColors[0], sWindowContentColors[1],
                                    sWindowContentColors[2], this->fileButtonAlpha[i], COSMETIC_ID("Menus.FilePlates"));
            gDPLoadTextureBlock(POLY_OPA_DISP++, sFileButtonTextures[i], G_IM_FMT_IA, G_IM_SIZ_16b, 64, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

            // draw file name box
            gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, sWindowContentColors[0], sWindowContentColors[1],
                                    sWindowContentColors[2], this->nameBoxAlpha[i], COSMETIC_ID("Menus.FilePlates"));
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelFileNameBoxTex, G_IM_FMT_IA, G_IM_SIZ_16b, 108, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);
            gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, sWindowContentColors[0], sWindowContentColors[1],
                                    sWindowContentColors[2], this->nameBoxAlpha[i], COSMETIC_ID("Menus.FilePlates"));
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelConnectorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);

            if (GameInteractor_Should(VB_DRAW_FILE_SELECT_SMALL_EXTRA_INFO_BOX,
                                      this->isOwlSave[i + FILE_NUM_OWL_SAVE_OFFSET], i)) {
                gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, sWindowContentColors[0], sWindowContentColors[1],
                                        sWindowContentColors[2], this->nameBoxAlpha[i],
                                        COSMETIC_ID("Menus.FilePlates"));
                gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelBlankButtonTex, G_IM_FMT_IA, G_IM_SIZ_16b, 52, 16, 0,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                    G_TX_NOLOD, G_TX_NOLOD);
                gSP1Quadrangle(POLY_OPA_DISP++, 12, 14, 15, 13, 0);
            }
        }
    }

    // draw file info
    for (fileIndex = 0; fileIndex < FILE_NUM_MAX; fileIndex++) {
        FileSelect_DrawFileInfo(&this->state, fileIndex);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[0x3AC], 20, 0);

    // draw primary action buttons (copy/erase)
    for (quadVtxIndex = 0, i = 0; i < 2; i++, quadVtxIndex += 4) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                                this->actionButtonAlpha[i], COSMETIC_ID("Menus.FilePlates"));
        gDPLoadTextureBlock(POLY_OPA_DISP++, sActionButtonTextures[i], G_IM_FMT_IA, G_IM_SIZ_16b, 64, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);

    // draw confirm buttons (yes/quit)
    for (quadVtxIndex = 0, i = FS_BTN_CONFIRM_YES; i <= FS_BTN_CONFIRM_QUIT; i++, quadVtxIndex += 4) {
        temp = this->confirmButtonTexIndices[i];
        gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                                this->confirmButtonAlpha[i], COSMETIC_ID("Menus.FilePlates"));
        gDPLoadTextureBlock(POLY_OPA_DISP++, sActionButtonTextures[temp], G_IM_FMT_IA, G_IM_SIZ_16b, 64, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
    }

    // draw options button
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            this->optionButtonAlpha, COSMETIC_ID("Menus.FilePlates"));
    gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelOptionsButtonENGTex, G_IM_FMT_IA, G_IM_SIZ_16b, 64, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
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
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->highlightColor[0], this->highlightColor[1],
                        this->highlightColor[2], this->highlightColor[3]);
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
        if (ResourceMgr_GetGameVersion(0) == MM_NTSC_US_GC) {
            gDPLoadTextureBlock_4b(POLY_OPA_DISP++, sWarningLabelsIA4[this->warningLabel], G_IM_FMT_IA, 128, 16, 0,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                   G_TX_NOLOD, G_TX_NOLOD);
        } else { // MM_NTSC_US_10
            gDPLoadTextureBlock(POLY_OPA_DISP++, sWarningLabels[this->warningLabel], G_IM_FMT_IA, G_IM_SIZ_8b, 128, 16,
                                0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
        }
        gSP1Quadrangle(POLY_OPA_DISP++, 16, 18, 19, 17, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);

    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileSelect_ConfigModeDraw(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    OPEN_DISPS(this->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);

    Gfx_SetupDL42_Opa(this->state.gfxCtx);
    FileSelect_SetView(this, 0.0f, 0.0f, 64.0f);
    FileSelect_SetWindowVtx(&this->state);
    FileSelect_SetWindowContentVtx(&this->state);

    FrameInterpolation_RecordOpenChild(this, this->configMode);

    if ((this->configMode != CM_NAME_ENTRY) && (this->configMode != CM_START_NAME_ENTRY)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        // File Select Window
        gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                                this->windowAlpha, COSMETIC_ID("Menus.FileWindow"));
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);

        if (this->windowRot != 0) {
            Matrix_RotateXFApply(this->windowRot / 100.0f);
        }

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileSelect_DrawWindowContents(&this->state);
    }

    // draw name entry menu
    if ((this->configMode >= CM_ROTATE_TO_NAME_ENTRY) && (this->configMode <= CM_NAME_ENTRY_TO_MAIN)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                                this->windowAlpha, COSMETIC_ID("Menus.FileWindow"));
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
        Matrix_RotateXFApply((this->windowRot - 314.0f) / 100.0f);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileSelect_DrawNameEntry(&this->state);
    }

    // draw options menu
    if ((this->configMode >= CM_MAIN_TO_OPTIONS) && (this->configMode <= CM_OPTIONS_TO_MAIN)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                                this->windowAlpha, COSMETIC_ID("Menus.FileWindow"));
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
        Matrix_RotateXFApply((this->windowRot - 314.0f) / 100.0f);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileSelect_DrawOptions(&this->state);
    }

    // #region 2S2H [Enhancement]
    if ((this->configMode >= CM_2S2H_ROTATE_TO_NEW_FILE_SETUP) &&
        (this->configMode <= CM_2S2H_NEW_FILE_SETUP_TO_MAIN)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                                this->windowAlpha, COSMETIC_ID("Menus.FileWindow"));
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
        Matrix_RotateXFApply((this->windowRot - 314.0f) / 100.0f);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileSelect_DrawNewFileSetup(&this->state);
    }
    // #endregion

    gDPPipeSync(POLY_OPA_DISP++);

    FileSelect_SetView(this, 0.0f, 0.0f, 64.0f);

    FrameInterpolation_RecordCloseChild();

    CLOSE_DISPS(this->state.gfxCtx);
}

/**
 * Fade out the main menu elements to transition to select mode.
 * Update function for `SM_FADE_MAIN_TO_SELECT`
 */
void FileSelect_FadeMainToSelect(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;

    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] -= 50;
            this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
                this->optionButtonAlpha = this->fileButtonAlpha[i];

            if (!gSaveContext.flashSaveAvailable) {
                if (NO_FLASH_SLOT_OCCUPIED(sramCtx, i)) {
                    this->nameAlpha[i] = this->nameBoxAlpha[i] = this->fileButtonAlpha[i];
                    this->connectorAlpha[i] -= 255 / 4;
                }
            } else {
                if (SLOT_OCCUPIED(this, i)) {
                    this->nameAlpha[i] = this->nameBoxAlpha[i] = this->fileButtonAlpha[i];
                    this->connectorAlpha[i] -= 255 / 4;
                }
            }
        }
    }

    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 4;
        this->selectMode++; // SM_MOVE_FILE_TO_TOP
        this->confirmButtonIndex = FS_BTN_CONFIRM_YES;
    }
}

// Amount to move by to reach the top of the screen
s16 sFileYOffsets[] = { 0, 16, 32 };

/**
 * Moves the selected file to the top of the window.
 * Update function for `SM_MOVE_FILE_TO_TOP`
 */
void FileSelect_MoveSelectedFileToTop(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s32 yStep;

    yStep = ABS_ALT(this->buttonYOffsets[this->buttonIndex] - sFileYOffsets[this->buttonIndex]) / this->actionTimer;
    this->buttonYOffsets[this->buttonIndex] += yStep;
    this->actionTimer--;

    if ((this->actionTimer == 0) || (this->buttonYOffsets[this->buttonIndex] == sFileYOffsets[this->buttonIndex])) {
        this->buttonYOffsets[FS_BTN_SELECT_YES] = this->buttonYOffsets[FS_BTN_SELECT_QUIT] = -24;
        this->actionTimer = 4;
        this->selectMode++; // SM_FADE_IN_FILE_INFO
    }
}

/**
 * Fade in the file info for the selected file.
 * Update function for `SM_FADE_IN_FILE_INFO`
 */
void FileSelect_FadeInFileInfo(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->fileInfoAlpha[this->buttonIndex] += 50;
    this->nameBoxAlpha[this->buttonIndex] -= 100;

    if (this->nameBoxAlpha[this->buttonIndex] <= 0) {
        this->nameBoxAlpha[this->buttonIndex] = 0;
    }

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->fileInfoAlpha[this->buttonIndex] = 200;
        this->actionTimer = 4;
        this->selectMode++; // SM_CONFIRM_FILE
    }

    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] = this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] =
        this->fileInfoAlpha[this->buttonIndex];
}

/**
 * Update the cursor and handle the option that the player picks for confirming the selected file.
 * Update function for `SM_CONFIRM_FILE`
 */
void FileSelect_ConfirmFile(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    Input* input = CONTROLLER1(&this->state);

    if (CHECK_BTN_ALL(input->press.button, BTN_START) || (CHECK_BTN_ALL(input->press.button, BTN_A))) {
        if (this->confirmButtonIndex == FS_BTN_CONFIRM_YES) {
            Rumble_Request(300.0f, 180, 20, 100);
            Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
            this->selectMode = SM_FADE_OUT;
            Audio_MuteAllSeqExceptSystemAndOcarina(15);
        } else { // FS_BTN_CONFIRM_QUIT
            Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
            this->selectMode++; // SM_FADE_OUT_FILE_INFO
        }
    } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
        this->selectMode++; // SM_FADE_OUT_FILE_INFO
    } else if (ABS_ALT(this->stickAdjY) >= 30) {
        Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
        this->confirmButtonIndex ^= 1;
    }
}

/**
 * Fade out the file info for the selected file before returning to the main menu.
 * Update function for `SM_FADE_OUT_FILE_INFO`
 */
void FileSelect_FadeOutFileInfo(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->fileInfoAlpha[this->buttonIndex] -= 200 / 4;
    this->nameBoxAlpha[this->buttonIndex] += 200 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->buttonYOffsets[FS_BTN_SELECT_YES] = this->buttonYOffsets[FS_BTN_SELECT_QUIT] = 0;
        this->nameBoxAlpha[this->buttonIndex] = 200;
        this->fileInfoAlpha[this->buttonIndex] = 0;
        this->nextTitleLabel = FS_TITLE_SELECT_FILE;
        this->actionTimer = 4;
        this->selectMode++;
    }

    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] = this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] =
        this->fileInfoAlpha[this->buttonIndex];
}

/**
 * Move the selected file back to the slot position then go to config mode for the main menu.
 * Update function for `SM_MOVE_FILE_TO_SLOT`
 */
void FileSelect_MoveSelectedFileToSlot(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s32 yStep;
    s16 i;

    yStep = ABS_ALT(this->buttonYOffsets[this->buttonIndex]) / this->actionTimer;
    this->buttonYOffsets[this->buttonIndex] -= yStep;

    if (this->buttonYOffsets[this->buttonIndex] <= 0) {
        this->buttonYOffsets[this->buttonIndex] = 0;
    }

    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] += 200 / 4;

            if (this->fileButtonAlpha[i] >= 200) {
                this->fileButtonAlpha[i] = 200;
            }

            this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
                this->optionButtonAlpha = this->fileButtonAlpha[i];

            if (!gSaveContext.flashSaveAvailable) {
                if (NO_FLASH_SLOT_OCCUPIED(sramCtx, i)) {
                    this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                    this->connectorAlpha[i] += 255 / 4;
                }
            } else {
                if (SLOT_OCCUPIED(this, i)) {
                    this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                    this->connectorAlpha[i] += 255 / 4;
                }
            }
        }
    }

    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->titleLabel = this->nextTitleLabel;
        this->actionTimer = 4;
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
void FileSelect_FadeOut(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->screenFillAlpha += 40;

    if (this->screenFillAlpha >= 255) {
        this->screenFillAlpha = 255;
        this->selectMode++; // SM_LOAD_GAME
    }
}

/**
 * Load the save for the appropriate file and start the game.
 * Update function for `SM_LOAD_GAME`
 */
void FileSelect_LoadGame(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    u16 i;

    gSaveContext.fileNum = this->buttonIndex;
    Sram_OpenSave(this, &this->sramCtx);

    gSaveContext.gameMode = GAMEMODE_NORMAL;

    STOP_GAMESTATE(&this->state);
    SET_NEXT_GAMESTATE(&this->state, Play_Init, sizeof(PlayState));

    gSaveContext.respawnFlag = 0;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = ENTR_LOAD_OPENING;
    gSaveContext.seqId = NA_BGM_DISABLED;
    gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
    gSaveContext.showTitleCard = true;
    gSaveContext.dogParams = 0;

    for (i = 0; i < TIMER_ID_MAX; i++) {
        gSaveContext.timerStates[i] = TIMER_STATE_OFF;
    }

    gSaveContext.prevHudVisibility = HUD_VISIBILITY_ALL;
    gSaveContext.nayrusLoveTimer = 0;
    gSaveContext.healthAccumulator = 0;
    gSaveContext.magicFlag = 0;
    gSaveContext.forcedSeqId = 0;
    gSaveContext.skyboxTime = CLOCK_TIME(0, 0);
    gSaveContext.nextTransitionType = TRANS_NEXT_TYPE_DEFAULT;
    gSaveContext.cutsceneTrigger = 0;
    gSaveContext.chamberCutsceneNum = 0;
    gSaveContext.nextDayTime = NEXT_TIME_NONE;
    gSaveContext.retainWeatherMode = false;

    gSaveContext.buttonStatus[EQUIP_SLOT_B] = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] = BTN_ENABLED;
    // #region 2S2H
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT] = BTN_ENABLED;
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_LEFT] = BTN_ENABLED;
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_DOWN] = BTN_ENABLED;
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_UP] = BTN_ENABLED;
    // This is only zeroed out when playing SoT, so if they give masks and soft reset it's wrongly persisted.
    // To fix, we just always zero it out when loading a file.
    memset(gSaveContext.masksGivenOnMoon, 0, sizeof(gSaveContext.masksGivenOnMoon));
    // #endregion
    gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;

    gSaveContext.hudVisibilityForceButtonAlphasByStatus = false;
    gSaveContext.nextHudVisibility = HUD_VISIBILITY_IDLE;
    gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
    gSaveContext.hudVisibilityTimer = 0;

    gSaveContext.save.saveInfo.playerData.tatlTimer = 0;

    GameInteractor_ExecuteOnSaveLoad(gSaveContext.fileNum);
}

void (*sSelectModeUpdateFuncs[])(GameState*) = {
    FileSelect_FadeMainToSelect,       // SM_FADE_MAIN_TO_SELECT
    FileSelect_MoveSelectedFileToTop,  // SM_MOVE_FILE_TO_TOP
    FileSelect_FadeInFileInfo,         // SM_FADE_IN_FILE_INFO
    FileSelect_ConfirmFile,            // SM_CONFIRM_FILE
    FileSelect_FadeOutFileInfo,        // SM_FADE_OUT_FILE_INFO
    FileSelect_MoveSelectedFileToSlot, // SM_MOVE_FILE_TO_SLOT
    FileSelect_FadeOut,                // SM_FADE_OUT
    FileSelect_LoadGame,               // SM_LOAD_GAME
};

void FileSelect_SelectModeUpdate(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    sSelectModeUpdateFuncs[this->selectMode](&this->state);
}

void FileSelect_SelectModeDraw(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    OPEN_DISPS(this->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);

    Gfx_SetupDL42_Opa(this->state.gfxCtx);
    FileSelect_SetView(this, 0.0f, 0.0f, 64.0f);
    FileSelect_SetWindowVtx(&this->state);
    FileSelect_SetWindowContentVtx(&this->state);

    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColorOverride(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            this->windowAlpha, COSMETIC_ID("Menus.FileWindow"));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
    Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
    Matrix_RotateXFApply(this->windowRot / 100.0f);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx);

    gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
    gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

    gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
    gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

    gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
    gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

    FileSelect_DrawWindowContents(&this->state);
    gDPPipeSync(POLY_OPA_DISP++);
    FileSelect_SetView(this, 0.0f, 0.0f, 64.0f);

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileSelect_UpdateAndDrawSkybox(FileSelectState* this) {
    s32 pad;
    f32 eyeX;
    f32 eyeY;
    f32 eyeZ;

    OPEN_DISPS(this->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);

    eyeX = 1000.0f * Math_CosS(sFileSelectSkyboxRotation) - 1000.0f * Math_SinS(sFileSelectSkyboxRotation);
    eyeY = -700.0f;
    eyeZ = 1000.0f * Math_SinS(sFileSelectSkyboxRotation) + 1000.0f * Math_CosS(sFileSelectSkyboxRotation);

    FileSelect_SetView(this, eyeX, eyeY, eyeZ);
    Skybox_Draw(&this->skyboxCtx, this->state.gfxCtx, SKYBOX_NORMAL_SKY, this->envCtx.skyboxBlend, eyeX, eyeY, eyeZ);

    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_NONE);

    sFileSelectSkyboxRotation += -0xA;

    CLOSE_DISPS(this->state.gfxCtx);
}

void (*gFileSelectDrawFuncs[])(GameState*) = {
    FileSelect_InitModeDraw,   // FS_MENU_MODE_INIT
    FileSelect_ConfigModeDraw, // FS_MENU_MODE_CONFIG
    FileSelect_SelectModeDraw, // FS_MENU_MODE_SELECT
};
void (*gFileSelectUpdateFuncs[])(GameState*) = {
    FileSelect_InitModeUpdate,   // FS_MENU_MODE_INIT
    FileSelect_ConfigModeUpdate, // FS_MENU_MODE_CONFIG
    FileSelect_SelectModeUpdate, // FS_MENU_MODE_SELECT
};

TexturePtr D_808147B4[] = { gFileSelPleaseWaitENGTex, gFileSelDecideCancelENGTex, gFileSelDecideSaveENGTex };
s16 D_808147C0[] = { 144, 144, 152 };
s16 D_808147C8[] = { 90, 90, 86 };

void FileSelect_Main(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    Input* input = CONTROLLER1(&this->state);
    s32 texIndex;
    s32 pad;

    func_8012CF0C(this->state.gfxCtx, 0, 1, 0, 0, 0);

    OPEN_DISPS(this->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x01, this->staticSegment);
    gSPSegment(POLY_OPA_DISP++, 0x02, this->parameterSegment);
    gSPSegment(POLY_OPA_DISP++, 0x06, this->titleSegment);

    this->stickAdjX = input->rel.stick_x;
    this->stickAdjY = input->rel.stick_y;

    if (this->stickAdjX < -30) {
        if (this->stickXDir == -1) {
            this->inputTimerX--;
            if (this->inputTimerX < 0) {
                this->inputTimerX = 2;
            } else {
                this->stickAdjX = 0;
            }
        } else {
            this->inputTimerX = 10;
            this->stickXDir = -1;
        }
    } else if (this->stickAdjX > 30) {
        if (this->stickXDir == 1) {
            this->inputTimerX--;
            if (this->inputTimerX < 0) {
                this->inputTimerX = 2;
            } else {
                this->stickAdjX = 0;
            }
        } else {
            this->inputTimerX = 10;
            this->stickXDir = 1;
        }
    } else {
        this->stickXDir = 0;
    }

    if (this->stickAdjY < -30) {
        if (this->stickYDir == -1) {
            this->inputTimerY--;
            if (this->inputTimerY < 0) {
                this->inputTimerY = 2;
            } else {
                this->stickAdjY = 0;
            }
        } else {
            this->inputTimerY = 10;
            this->stickYDir = -1;
        }
    } else if (this->stickAdjY > 30) {
        if (this->stickYDir == 1) {
            this->inputTimerY--;
            if (this->inputTimerY < 0) {
                this->inputTimerY = 2;
            } else {
                this->stickAdjY = 0;
            }
        } else {
            this->inputTimerY = 10;
            this->stickYDir = 1;
        }
    } else {
        this->stickYDir = 0;
    }

    this->emptyFileTextAlpha = 0;

    FileSelect_PulsateCursor(&this->state);
    gFileSelectUpdateFuncs[this->menuMode](&this->state);

    FrameInterpolation_StartRecord();
    FileSelect_UpdateAndDrawSkybox(this);
    gFileSelectDrawFuncs[this->menuMode](&this->state);
    FrameInterpolation_StopRecord();

    Gfx_SetupDL39_Opa(this->state.gfxCtx);

    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 100, 255, 255, this->controlsAlpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    if (this->sramCtx.status > 0) {
        texIndex = 0;
    } else if ((this->configMode >= CM_MAIN_TO_OPTIONS) && (this->configMode <= CM_OPTIONS_TO_MAIN)) {
        texIndex = 2;
    } else {
        texIndex = 1;
    }

    gDPLoadTextureBlock(POLY_OPA_DISP++, D_808147B4[texIndex], G_IM_FMT_IA, G_IM_SIZ_8b, D_808147C0[texIndex], 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                        G_TX_NOLOD);

    gSPTextureRectangle(POLY_OPA_DISP++, D_808147C8[texIndex] << 2, 204 << 2,
                        (D_808147C8[texIndex] + D_808147C0[texIndex]) << 2, (204 + 16) << 2, G_TX_RENDERTILE, 0, 0,
                        1 << 10, 1 << 10);

    gDPPipeSync(POLY_OPA_DISP++);
    gSPDisplayList(POLY_OPA_DISP++, sScreenFillSetupDL);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, this->screenFillAlpha);
    gSPDisplayList(POLY_OPA_DISP++, D_0E000000_TO_SEGMENTED(fillRect));

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileSelect_InitContext(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    EnvironmentContext* envCtx = &this->envCtx;

    Sram_Alloc(&this->state, &this->sramCtx);
    func_801457CC(&this->state, &this->sramCtx);

    this->menuMode = FS_MENU_MODE_INIT;

    this->buttonIndex = this->selectMode = this->selectedFileIndex = this->copyDestFileIndex =
        this->confirmButtonIndex = 0;

    this->confirmButtonTexIndices[0] = 2;
    this->confirmButtonTexIndices[1] = 3;
    this->titleLabel = FS_TITLE_SELECT_FILE;
    this->nextTitleLabel = FS_TITLE_OPEN_FILE;

    this->screenFillAlpha = 255;
    this->highlightPulseDir = 1;
    this->unk_244F4 = 0xC;
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

    this->windowAlpha = this->titleAlpha[FS_TITLE_CUR] = this->titleAlpha[FS_TITLE_NEXT] = this->fileButtonAlpha[0] =
        this->fileButtonAlpha[1] = this->fileButtonAlpha[2] = this->nameBoxAlpha[0] = this->nameBoxAlpha[1] =
            this->nameBoxAlpha[2] = this->nameAlpha[0] = this->nameAlpha[1] = this->nameAlpha[2] =
                this->connectorAlpha[0] = this->connectorAlpha[1] = this->connectorAlpha[2] = this->fileInfoAlpha[0] =
                    this->fileInfoAlpha[1] = this->fileInfoAlpha[2] = this->actionButtonAlpha[FS_BTN_ACTION_COPY] =
                        this->actionButtonAlpha[FS_BTN_ACTION_ERASE] = this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] =
                            this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = this->optionButtonAlpha =
                                this->nameEntryBoxAlpha = this->controlsAlpha = this->emptyFileTextAlpha = 0;

    this->windowPosX = 6;
    this->actionTimer = 4;
    this->warningLabel = FS_WARNING_NONE;

    this->warningButtonIndex = this->buttonYOffsets[0] = this->buttonYOffsets[1] = this->buttonYOffsets[2] =
        this->buttonYOffsets[3] = this->buttonYOffsets[4] = this->buttonYOffsets[5] = this->fileNamesY[0] =
            this->fileNamesY[1] = this->fileNamesY[2] = 0;

    this->unk_2451E[0] = 0;
    this->unk_2451E[1] = 3;
    this->unk_2451E[2] = 6;
    this->unk_2451E[3] = 8;
    this->unk_2451E[4] = 10;
    this->highlightTimer = 20;

    ShrinkWindow_Letterbox_SetSizeTarget(0);

    gSaveContext.skyboxTime = 0;
    gSaveContext.save.time = CLOCK_TIME(0, 0);

    Skybox_Init(&this->state, &this->skyboxCtx, 1);
    R_TIME_SPEED = 10;

    envCtx->changeSkyboxState = CHANGE_SKYBOX_INACTIVE;
    envCtx->changeSkyboxTimer = 0;
    envCtx->changeLightEnabled = false;
    envCtx->changeLightTimer = 0;
    envCtx->skyboxDmaState = 0;
    envCtx->skybox1Index = 99;
    envCtx->skybox2Index = 99;
    envCtx->lightConfig = 0;
    envCtx->changeLightNextConfig = 0;
    envCtx->lightSetting = 0;
    envCtx->skyboxConfig = SKYBOX_CONFIG_2;
    envCtx->skyboxDisabled = 0;
    envCtx->skyboxBlend = 0;
    envCtx->glareAlpha = 0.0f;
    envCtx->lensFlareAlphaScale = 0.0f;

    gSaveContext.buttonStatus[EQUIP_SLOT_B] = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] = BTN_ENABLED;
    // #region 2S2H
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT] = BTN_ENABLED;
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_LEFT] = BTN_ENABLED;
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_DOWN] = BTN_ENABLED;
    gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_UP] = BTN_ENABLED;
    // This is only zeroed out when playing SoT, so if they give masks and soft reset it's wrongly persisted.
    // To fix, we just always zero it out when loading a file.
    memset(gSaveContext.masksGivenOnMoon, 0, sizeof(gSaveContext.masksGivenOnMoon));
    // #endregion
    gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;
}

void FileSelect_Destroy(GameState* this) {
    ShrinkWindow_Destroy();
    gFileSelectState = NULL;
}

void FileSelect_Init(GameState* thisx) {
    s32 pad;
    FileSelectState* this = (FileSelectState*)thisx;
    gFileSelectState = this;
    size_t size;

    GameState_SetFramerateDivisor(&this->state, 1);
    Matrix_Init(&this->state);
    ShrinkWindow_Init();
    View_Init(&this->view, this->state.gfxCtx);
    this->state.main = FileSelect_Main;
    this->state.destroy = FileSelect_Destroy;
    FileSelect_InitContext(&this->state);
    Font_LoadOrderedFont(&this->font);

    size = SEGMENT_ROM_SIZE(title_static);
    this->staticSegment = THA_AllocTailAlign16(&this->state.tha, size);
    DmaMgr_SendRequest0(this->staticSegment, SEGMENT_ROM_START(title_static), size);

    size = SEGMENT_ROM_SIZE(parameter_static);
    this->parameterSegment = THA_AllocTailAlign16(&this->state.tha, size);
    DmaMgr_SendRequest0(this->parameterSegment, SEGMENT_ROM_START(parameter_static), size);

    size = gObjectTable[OBJECT_MAG].vromEnd - gObjectTable[OBJECT_MAG].vromStart;
    this->titleSegment = THA_AllocTailAlign16(&this->state.tha, size);
    DmaMgr_SendRequest0(this->titleSegment, gObjectTable[OBJECT_MAG].vromStart, size);

    Audio_SetSpec(0xA);
    // Setting ioData to 1 and writing it to ioPort 7 will skip the harp intro
    Audio_PlaySequenceWithSeqPlayerIO(SEQ_PLAYER_BGM_MAIN, NA_BGM_FILE_SELECT, 0, 7, 1);
}
