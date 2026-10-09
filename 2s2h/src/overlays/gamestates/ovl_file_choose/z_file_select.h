#ifndef FILE_SELECT_H
#define FILE_SELECT_H

#include "global.h"
#include "z64game.h"
#include "z64message.h"
#include "z64skybox.h"
#include "z64view.h"


// `sramCtx->noFlashSaveBuf` is never allocated space, so should never be used
// Slot offsets are also based on OoT SaveContext sizes, and contains incorrect sizes from MM
// Macros appear to be remnant from OoT
#define NO_FLASH_GET_NEWF(sramCtx, slotNum, index) \
    (sramCtx->noFlashSaveBuf[gSramSlotOffsets[slotNum] + offsetof(SaveContext, save.saveInfo.playerData.newf[index])])
#define NO_FLASH_SLOT_OCCUPIED(sramCtx, slotNum)                                                  \
    ((NO_FLASH_GET_NEWF(sramCtx, slotNum, 0) == 'Z') || (NO_FLASH_GET_NEWF(sramCtx, slotNum, 1) == 'E') || \
     (NO_FLASH_GET_NEWF(sramCtx, slotNum, 2) == 'L') || (NO_FLASH_GET_NEWF(sramCtx, slotNum, 3) == 'D') || \
     (NO_FLASH_GET_NEWF(sramCtx, slotNum, 4) == 'A') || (NO_FLASH_GET_NEWF(sramCtx, slotNum, 5) == '3'))

#define GET_NEWF(fileSelect, slotNum, index) (fileSelect->newf[slotNum][index])
#define SLOT_OCCUPIED(fileSelect, slotNum)                                                                 \
    ((GET_NEWF(fileSelect, slotNum, 0) == 'Z') && (GET_NEWF(fileSelect, slotNum, 1) == 'E') && \
     (GET_NEWF(fileSelect, slotNum, 2) == 'L') && (GET_NEWF(fileSelect, slotNum, 3) == 'D') && \
     (GET_NEWF(fileSelect, slotNum, 4) == 'A') && (GET_NEWF(fileSelect, slotNum, 5) == '3'))

// Init mode: Initial setup as the file select is starting up, fades and slides in various menu elements
// Config mode: Handles the bulk of the file select, various configuration tasks like picking a file, copy/erase, and the options menu
// Select mode: Displays the selected file with various details about it, and allows the player to confirm and open it
typedef enum {
    /* 0 */ FS_MENU_MODE_INIT,
    /* 1 */ FS_MENU_MODE_CONFIG,
    /* 2 */ FS_MENU_MODE_SELECT
} MenuMode;

typedef enum {
    /* 0x00 */ CM_FADE_IN_START,
    /* 0x01 */ CM_FADE_IN_END,
    /* 0x02 */ CM_MAIN_MENU,
    /* 0x03 */ CM_SETUP_COPY_SOURCE,
    /* 0x04 */ CM_SELECT_COPY_SOURCE,
    /* 0x05 */ CM_SETUP_COPY_DEST_1,
    /* 0x06 */ CM_SETUP_COPY_DEST_2,
    /* 0x07 */ CM_SELECT_COPY_DEST,
    /* 0x08 */ CM_EXIT_TO_COPY_SOURCE_1,
    /* 0x09 */ CM_EXIT_TO_COPY_SOURCE_2,
    /* 0x0A */ CM_SETUP_COPY_CONFIRM_1,
    /* 0x0B */ CM_SETUP_COPY_CONFIRM_2,
    /* 0x0C */ CM_COPY_CONFIRM,
    /* 0x0D */ CM_COPY_WAIT_FOR_FLASH_SAVE,
    /* 0x0E */ CM_RETURN_TO_COPY_DEST,
    /* 0x0F */ CM_COPY_ANIM_1,
    /* 0x10 */ CM_COPY_ANIM_2,
    /* 0x11 */ CM_COPY_ANIM_3,
    /* 0x12 */ CM_COPY_ANIM_4,
    /* 0x13 */ CM_COPY_ANIM_5,
    /* 0x14 */ CM_COPY_RETURN_MAIN,
    /* 0x15 */ CM_SETUP_ERASE_SELECT,
    /* 0x16 */ CM_ERASE_SELECT,
    /* 0x17 */ CM_SETUP_ERASE_CONFIRM_1,
    /* 0x18 */ CM_SETUP_ERASE_CONFIRM_2,
    /* 0x19 */ CM_ERASE_CONFIRM,
    /* 0x1A */ CM_EXIT_TO_ERASE_SELECT_1,
    /* 0x1B */ CM_EXIT_TO_ERASE_SELECT_2,
    /* 0x1C */ CM_ERASE_ANIM_1,
    /* 0x1D */ CM_ERASE_WAIT_FOR_FLASH_SAVE,
    /* 0x1E */ CM_ERASE_ANIM_2,
    /* 0x1F */ CM_ERASE_ANIM_3,
    /* 0x20 */ CM_EXIT_ERASE_TO_MAIN,
    /* 0x21 */ CM_UNUSED_31,
    /* 0x22 */ CM_ROTATE_TO_NAME_ENTRY,
    /* 0x23 */ CM_START_NAME_ENTRY,
    /* 0x24 */ CM_NAME_ENTRY,
    /* 0x25 */ CM_NAME_ENTRY_WAIT_FOR_FLASH_SAVE,
    /* 0x26 */ CM_NAME_ENTRY_TO_MAIN,
    /* 0x27 */ CM_MAIN_TO_OPTIONS,
    /* 0x28 */ CM_START_OPTIONS,
    /* 0x29 */ CM_OPTIONS_MENU,
    /* 0x2A */ CM_OPTIONS_WAIT_FOR_FLASH_SAVE,
    /* 0x2B */ CM_OPTIONS_TO_MAIN,
    /* 0x2C */ CM_UNUSED_DELAY,
    // 2S2H [Enhancement] New file setup, shown between picking an empty file and entering a name.
    /* 0x2D */ CM_2S2H_ROTATE_TO_NEW_FILE_SETUP,
    /* 0x2E */ CM_2S2H_NEW_FILE_SETUP,
    /* 0x2F */ CM_2S2H_NEW_FILE_SETUP_TO_MAIN
} ConfigMode;

typedef enum {
    /* 0 */ SM_FADE_MAIN_TO_SELECT,
    /* 1 */ SM_MOVE_FILE_TO_TOP,
    /* 2 */ SM_FADE_IN_FILE_INFO,
    /* 3 */ SM_CONFIRM_FILE,
    /* 4 */ SM_FADE_OUT_FILE_INFO,
    /* 5 */ SM_MOVE_FILE_TO_SLOT,
    /* 6 */ SM_FADE_OUT,
    /* 7 */ SM_LOAD_GAME
} SelectMode;

typedef enum {
    /* 0 */ FS_TITLE_SELECT_FILE,   // "Please select a file."
    /* 1 */ FS_TITLE_OPEN_FILE,     // "Open this file?"
    /* 2 */ FS_TITLE_COPY_FROM,     // "Copy which file?"
    /* 3 */ FS_TITLE_COPY_TO,       // "Copy to which file?"
    /* 4 */ FS_TITLE_COPY_CONFIRM,  // "Are you sure?"
    /* 5 */ FS_TITLE_COPY_COMPLETE, // "File copied."
    /* 6 */ FS_TITLE_ERASE_FILE,    // "Erase which file?"
    /* 7 */ FS_TITLE_ERASE_CONFIRM, // "Are you sure?"
    /* 8 */ FS_TITLE_ERASE_COMPLETE // "File erased."
} TitleLabel;

typedef enum {
    /* 0 */ FS_TITLE_CUR,
    /* 1 */ FS_TITLE_NEXT
} TitleIndex;

typedef enum {
    /* -1 */ FS_WARNING_NONE = -1,
    /*  0 */ FS_WARNING_NO_FILE_COPY,   // "No file to copy."
    /*  1 */ FS_WARNING_NO_FILE_ERASE,  // "No file to erase."
    /*  2 */ FS_WARNING_NO_EMPTY_FILES, // "There is no empty file."
    /*  3 */ FS_WARNING_FILE_EMPTY,     // "This is an empty file."
    /*  4 */ FS_WARNING_FILE_IN_USE     // "This file is in use."
} WarningLabel;

typedef enum {
    /* 0 */ FS_BTN_MAIN_FILE_1,
    /* 1 */ FS_BTN_MAIN_FILE_2,
    /* 2 */ FS_BTN_MAIN_FILE_3,
    /* 3 */ FS_BTN_MAIN_COPY,
    /* 4 */ FS_BTN_MAIN_ERASE,
    /* 5 */ FS_BTN_MAIN_OPTIONS
} MainMenuButtonIndex;

typedef enum {
    /* 0 */ FS_BTN_COPY_FILE_1,
    /* 1 */ FS_BTN_COPY_FILE_2,
    /* 2 */ FS_BTN_COPY_FILE_3,
    /* 3 */ FS_BTN_COPY_QUIT
} CopyMenuButtonIndex;

typedef enum {
    /* 0 */ FS_BTN_ERASE_FILE_1,
    /* 1 */ FS_BTN_ERASE_FILE_2,
    /* 2 */ FS_BTN_ERASE_FILE_3,
    /* 3 */ FS_BTN_ERASE_QUIT
} EraseMenuButtonIndex;

typedef enum {
    /* 0 */ FS_BTN_SELECT_FILE_1,
    /* 1 */ FS_BTN_SELECT_FILE_2,
    /* 2 */ FS_BTN_SELECT_FILE_3,
    /* 3 */ FS_BTN_SELECT_YES,
    /* 4 */ FS_BTN_SELECT_QUIT
} SelectMenuButtonIndex;

typedef enum {
    /* 0 */ FS_BTN_CONFIRM_YES,
    /* 1 */ FS_BTN_CONFIRM_QUIT
} ConfirmButtonIndex;

typedef enum {
    /* 0 */ FS_BTN_ACTION_COPY,
    /* 1 */ FS_BTN_ACTION_ERASE
} ActionButtonIndex;

typedef enum {
    /* 0 */ FS_SETTING_AUDIO,
    /* 1 */ FS_SETTING_ZTARGET
} SettingIndex;

typedef enum {
    /* 0 */ FS_CHAR_PAGE_HIRA,
    /* 1 */ FS_CHAR_PAGE_KATA,
    /* 2 */ FS_CHAR_PAGE_ENG
} CharPage;

typedef enum {
    /*  0 */ FS_KBD_BTN_HIRA,
    /*  1 */ FS_KBD_BTN_KATA,
    /*  2 */ FS_KBD_BTN_ENG,
    /*  3 */ FS_KBD_BTN_BACKSPACE,
    /*  4 */ FS_KBD_BTN_END,
    /* 99 */ FS_KBD_BTN_NONE = 99
} KeyboardButton;

typedef struct FileSelectState {
    /* 0x00000 */ GameState state;
    /* 0x000A4 */ Vtx* windowVtx;
    /* 0x000A8 */ u8* staticSegment;
    /* 0x000AC */ u8* parameterSegment;
    /* 0x000B0 */ u8* titleSegment;
    /* 0x000B8 */ View view;
    /* 0x00220 */ SramContext sramCtx;
    /* 0x00248 */ SkyboxContext skyboxCtx;
    /* 0x00470 */ MessageContext msgCtx;
    /* 0x12550 */ Font font;
    /* 0x242E0 */ EnvironmentContext envCtx;
    /* 0x243E0 */ UNK_TYPE1 pad243E0[0x4];
    /* 0x243E4 */ Vtx* windowContentVtx;
    /* 0x243E8 */ Vtx* keyboardVtx;
    /* 0x243EC */ Vtx* nameEntryVtx;
    /* 0x243F0 */ Vtx* keyboard2Vtx;
    /* 0x243F4 */ u8 newf[FILE_NUM_MAX_WITH_OWL_SAVE][6];
    /* 0x2440C */ u16 threeDayResetCount[FILE_NUM_MAX_WITH_OWL_SAVE];
    /* 0x24414 */ char fileNames[FILE_NUM_MAX_WITH_OWL_SAVE][8];
    /* 0x24434 */ s16 healthCapacity[FILE_NUM_MAX_WITH_OWL_SAVE];
    /* 0x2443C */ s16 health[FILE_NUM_MAX_WITH_OWL_SAVE];
    /* 0x24444 */ u32 questItems[FILE_NUM_MAX_WITH_OWL_SAVE];
    /* 0x24454 */ s8 defenseHearts[FILE_NUM_MAX_WITH_OWL_SAVE];
    /* 0x24458 */ u16 time[FILE_NUM_MAX_WITH_OWL_SAVE];
    /* 0x24460 */ s16 day[FILE_NUM_MAX_WITH_OWL_SAVE];
    /* 0x24468 */ u8 isOwlSave[FILE_NUM_MAX_WITH_OWL_SAVE];
    /* 0x2446C */ s16 rupees[FILE_NUM_MAX_WITH_OWL_SAVE];
    /* 0x24474 */ u8 walletUpgrades[FILE_NUM_MAX_WITH_OWL_SAVE];
    /* 0x24478 */ u8 maskCount[FILE_NUM_MAX_WITH_OWL_SAVE];
    /* 0x2447C */ u8 heartPieceCount[FILE_NUM_MAX_WITH_OWL_SAVE];
    /* 0x24480 */ s16 buttonIndex; // enum will depend on `ConfigMode`
    /* 0x24482 */ s16 confirmButtonIndex; // see `ConfirmButtonIndex` enum
    /* 0x24484 */ s16 menuMode; // see `MenuMode` enum
    /* 0x24486 */ s16 configMode; // see `ConfigMode` enum
    /* 0x24488 */ s16 prevConfigMode; // see `ConfigMode` enum
    /* 0x2448A */ s16 nextConfigMode; // see `ConfigMode` enum
    /* 0x2448C */ s16 selectMode; // see `SelectMode` enum
    /* 0x2448E */ s16 selectedFileIndex;
    /* 0x24490 */ UNK_TYPE1 pad24490[0x2];
    /* 0x24492 */ s16 fileNamesY[3];
    /* 0x24498 */ s16 actionTimer;
    /* 0x2449A */ s16 buttonYOffsets[6];
    /* 0x244A6 */ s16 copyDestFileIndex;
    /* 0x244A8 */ s16 warningLabel;
    /* 0x244AA */ s16 warningButtonIndex;
    /* 0x244AC */ s16 titleLabel; // see `TitleLabel` enum
    /* 0x244AE */ s16 nextTitleLabel; // see `TitleLabel` enum
    /* 0x244B0 */ s16 windowColor[3];
    /* 0x244B6 */ s16 titleAlpha[2]; // see `TitleIndex` enum
    /* 0x244BA */ s16 windowAlpha;
    /* 0x244BC */ s16 fileButtonAlpha[3];
    /* 0x244C2 */ s16 nameBoxAlpha[3];
    /* 0x244C8 */ s16 nameAlpha[3];
    /* 0x244CE */ s16 connectorAlpha[3];
    /* 0x244D4 */ s16 fileInfoAlpha[3];
    /* 0x244DA */ s16 actionButtonAlpha[2];
    /* 0x244DA */ s16 confirmButtonAlpha[2];
    /* 0x244E2 */ s16 optionButtonAlpha;
    /* 0x244E4 */ s16 nameEntryBoxAlpha;
    /* 0x244E6 */ s16 controlsAlpha;
    /* 0x244E8 */ s16 emptyFileTextAlpha;
    /* 0x244EA */ s16 highlightColor[4];
    /* 0x244F2 */ s16 highlightPulseDir;
    /* 0x244F4 */ s16 unk_244F4;
    /* 0x244F6 */ s16 confirmButtonTexIndices[2];
    /* 0x244FA */ s16 inputTimerX;
    /* 0x244FC */ s16 inputTimerY;
    /* 0x244FE */ s16 stickXDir;
    /* 0x24500 */ s16 stickYDir;
    /* 0x24502 */ s16 stickAdjX;
    /* 0x24504 */ s16 stickAdjY;
    /* 0x24506 */ s16 nameEntryBoxPosX;
    /* 0x24508 */ s16 windowPosX;
    /* 0x2450A */ s16 screenFillAlpha;
    /* 0x2450C */ f32 windowRot;
    /* 0x24510 */ s16 kbdButton;
    /* 0x24512 */ s16 charPage;
    /* 0x24514 */ s16 charBgAlpha;
    /* 0x24516 */ s16 charIndex;
    /* 0x24518 */ s16 kbdX;
    /* 0x2451A */ s16 kbdY;
    /* 0x2451C */ s16 newFileNameCharCount;
    /* 0x2451E */ s16 unk_2451E[5];
    /* 0x24528 */ s16 highlightTimer;
    /* 0x2452A */ s16 unk_2452A;
    /* 0x2452C */ s16 unk_2452C[4];
    /* 0x24534 */ s16 unk_24534[4];
    /* 0x2453C */ s16 unk_2453C[4];
    /* 0x24544 */ s16 unk_24544[4];
    /* 0x2454C */ s16 unk_2454C;
    /* 0x2454E */ s16 unk_2454E;
    /* 0x24550 */ s16 unk_24550;
} FileSelectState; // size = 0x24558

void FileSelect_Init(GameState* thisx);
void FileSelect_Destroy(GameState* thisx);

void FileSelect_PulsateCursor(GameState* thisx);
void FileSelect_DrawNameEntry(GameState* thisx);
void FileSelect_DrawOptions(GameState* thisx);
void FileSelect_DrawTexQuadI4(GraphicsContext* gfxCtx, TexturePtr texture, s16 point);

// Copying Files
void FileSelect_SetupCopySource(GameState* thisx);
void FileSelect_SelectCopySource(GameState* thisx);
void FileSelect_SetupCopyDest1(GameState* thisx);
void FileSelect_SetupCopyDest2(GameState* thisx);
void FileSelect_SelectCopyDest(GameState* thisx);
void FileSelect_ExitToCopySource1(GameState* thisx);
void FileSelect_ExitToCopySource2(GameState* thisx);
void FileSelect_SetupCopyConfirm1(GameState* thisx);
void FileSelect_SetupCopyConfirm2(GameState* thisx);
void FileSelect_CopyConfirm(GameState* thisx);
void FileSelect_CopyWaitForFlashSave(GameState* thisx);
void FileSelect_ReturnToCopyDest(GameState* thisx);
void FileSelect_CopyAnim1(GameState* thisx);
void FileSelect_CopyAnim2(GameState* thisx);
void FileSelect_CopyAnim3(GameState* thisx);
void FileSelect_CopyAnim4(GameState* thisx);
void FileSelect_CopyAnim5(GameState* thisx);
void FileSelect_ExitCopyToMain(GameState* thisx);

// Erasing Files
void FileSelect_SetupEraseSelect(GameState* thisx);
void FileSelect_EraseSelect(GameState* thisx);
void FileSelect_SetupEraseConfirm1(GameState* thisx);
void FileSelect_SetupEraseConfirm2(GameState* thisx);
void FileSelect_EraseConfirm(GameState* thisx);
void FileSelect_ExitToEraseSelect1(GameState* thisx);
void FileSelect_ExitToEraseSelect2(GameState* thisx);
void FileSelect_EraseAnim1(GameState* thisx);
void FileSelect_EraseWaitForFlashSave(GameState* thisx);
void FileSelect_EraseAnim2(GameState* thisx);
void FileSelect_EraseAnim3(GameState* thisx);
void FileSelect_ExitEraseToMain(GameState* thisx);

// Entering Name
void FileSelect_StartNameEntry(GameState* thisx);
void FileSelect_UpdateKeyboardCursor(GameState* thisx);
void FileSelect_NameEntryWaitForFlashSave(GameState* thisx);

// Selecting Options
void FileSelect_StartOptions(GameState* thisx);
void FileSelect_UpdateOptionsMenu(GameState* thisx);
void FileSelect_OptionsWaitForFlashSave(GameState* thisx);

// 2S2H [Enhancement] source: mm/2s2h/Enhancements/Saving/NewFileSetup.cpp
void FileSelect_RotateToNewFileSetup(GameState* thisx);
void FileSelect_UpdateNewFileSetup(GameState* thisx);
void FileSelect_DrawNewFileSetup(GameState* thisx);

extern u8 D_808141F0[];
extern s16 D_80814280[];

#endif
