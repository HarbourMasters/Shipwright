/*
 * File: z_file_copy_erase.c
 * Overlay: ovl_file_choose
 * Description: Copying and Erasing Files
 */

#include "z_file_select.h"
#include "z64rumble.h"
#include <libultraship/bridge/consolevariablebridge.h>

// When choosing a file to copy or erase, the 6 main menu buttons are placed at these offsets
s16 sChooseFileYOffsets[] = { -48, -48, -48, -24, -24, 0 };

s16 D_8081424C[3][3] = {
    { 0, -48, -48 },
    { -64, 16, -48 },
    { -64, -64, 32 },
};

s16 sEraseDelayTimer = 15;

/**
 * Move buttons into place for the select source screen and fade in the proper labels.
 * Update function for `CM_SETUP_COPY_SOURCE`
 */
void FileSelect_SetupCopySource(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s32 yStep;
    s16 i;

    for (i = 0; i < 5; i++) {
        yStep = ABS_ALT(this->buttonYOffsets[i] - sChooseFileYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= sChooseFileYOffsets[i]) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] -= 200 / 4;
    this->actionButtonAlpha[FS_BTN_ACTION_ERASE] -= 200 / 4;
    this->optionButtonAlpha -= 200 / 4;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] += 200 / 4;
    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 4;

        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
            this->optionButtonAlpha = 0;

        this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 200;
        this->titleLabel = this->nextTitleLabel;

        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->buttonIndex = FS_BTN_COPY_QUIT;
        this->configMode++; // CM_SELECT_COPY_SOURCE
    }
}

/**
 * Allow the player to select a file to copy or exit back to the main menu.
 * Update function for `CM_SELECT_COPY_SOURCE`
 */
void FileSelect_SelectCopySource(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = CONTROLLER1(&this->state);

    if (((this->buttonIndex == FS_BTN_COPY_QUIT) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actionTimer = 4;
        this->buttonIndex = FS_BTN_MAIN_COPY;
        this->nextTitleLabel = FS_TITLE_SELECT_FILE;
        this->configMode = CM_COPY_RETURN_MAIN;
        this->warningLabel = FS_WARNING_NONE;
        Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        if (!gSaveContext.flashSaveAvailable) {
            if (NO_FLASH_SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
                this->actionTimer = 4;
                this->selectedFileIndex = this->buttonIndex;
                this->configMode = CM_SETUP_COPY_DEST_1;
                this->nextTitleLabel = FS_TITLE_COPY_TO;
                Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
            } else {
                Audio_PlaySfx(NA_SE_SY_FSEL_ERROR);
            }
        } else if (SLOT_OCCUPIED(this, this->buttonIndex)) {
            this->actionTimer = 4;
            this->selectedFileIndex = this->buttonIndex;
            this->configMode = CM_SETUP_COPY_DEST_1;
            this->nextTitleLabel = FS_TITLE_COPY_TO;
            Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
        } else {
            Audio_PlaySfx(NA_SE_SY_FSEL_ERROR);
        }
    } else {
        if (ABS_ALT(this->stickAdjY) >= 30) {
            Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
            if (this->stickAdjY >= 30) {
                this->buttonIndex--;
                // Instead of removing File 3 entirely, the index is manually adjusted to skip it
                if (!CVarGetInteger("gEnhancements.Saving.FileSlot3", true) &&
                    this->buttonIndex == FS_BTN_COPY_FILE_3) {
                    this->buttonIndex = FS_BTN_COPY_FILE_2;
                }
                if (this->buttonIndex < FS_BTN_COPY_FILE_1) {
                    this->buttonIndex = FS_BTN_COPY_QUIT;
                }
            } else {
                this->buttonIndex++;
                // Instead of removing File 3 entirely, the index is manually adjusted to skip it
                if (!CVarGetInteger("gEnhancements.Saving.FileSlot3", true) &&
                    this->buttonIndex == FS_BTN_COPY_FILE_3) {
                    this->buttonIndex = FS_BTN_COPY_QUIT;
                }
                if (this->buttonIndex > FS_BTN_COPY_QUIT) {
                    this->buttonIndex = FS_BTN_COPY_FILE_1;
                }
            }
        }

        if (this->buttonIndex != FS_BTN_COPY_QUIT) {
            if (!gSaveContext.flashSaveAvailable) {
                if (!NO_FLASH_SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
                    this->warningLabel = FS_WARNING_FILE_EMPTY;
                    this->warningButtonIndex = this->buttonIndex;
                    this->emptyFileTextAlpha = 255;
                } else {
                    this->warningLabel = FS_WARNING_NONE;
                }
            } else if (!SLOT_OCCUPIED(this, this->buttonIndex)) {
                this->warningLabel = FS_WARNING_FILE_EMPTY;
                this->warningButtonIndex = this->buttonIndex;
                this->emptyFileTextAlpha = 255;
            } else {
                this->warningLabel = FS_WARNING_NONE;
            }
        }
    }
}

/**
 * Move the menu buttons into place for the copy destination selection and switch titles.
 * Update function for `CM_SETUP_COPY_DEST_1`
 */
void FileSelect_SetupCopyDest1(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s32 yStep;
    s16 i;

    for (i = 0; i < 3; i++) {
        yStep = ABS_ALT(this->buttonYOffsets[i] - D_8081424C[this->buttonIndex][i]) / this->actionTimer;

        if (D_8081424C[this->buttonIndex][i] >= this->buttonYOffsets[i]) {
            this->buttonYOffsets[i] += yStep;
        } else {
            this->buttonYOffsets[i] -= yStep;
        }
    }

    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    this->nameBoxAlpha[this->buttonIndex] -= 200 / 4;

    this->actionTimer--;
    if (this->actionTimer == 0) {
        this->buttonYOffsets[this->buttonIndex] = D_8081424C[this->buttonIndex][this->buttonIndex];
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->actionTimer = 4;
        this->configMode++; // CM_SETUP_COPY_DEST_2
    }
}

/**
 * Show the file info of the file selected to copy from.
 * Update function for `CM_SETUP_COPY_DEST_2`
 */
void FileSelect_SetupCopyDest2(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->nameBoxAlpha[this->buttonIndex] -= 200 / 4;
    this->fileInfoAlpha[this->buttonIndex] += 200 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->nameBoxAlpha[this->buttonIndex] = 0;
        this->fileInfoAlpha[this->buttonIndex] = 200;
        this->buttonIndex = FS_BTN_COPY_QUIT;
        this->actionTimer = 4;
        this->configMode = CM_SELECT_COPY_DEST;
    }
}

/**
 * Allow the player to select a slot to copy to or exit to the copy select screen.
 * Update function for `CM_SELECT_COPY_DEST`
 */
void FileSelect_SelectCopyDest(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = CONTROLLER1(&this->state);

    if (((this->buttonIndex == FS_BTN_COPY_QUIT) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->buttonIndex = this->selectedFileIndex;
        this->nextTitleLabel = FS_TITLE_COPY_FROM;
        this->actionTimer = 4;
        this->configMode = CM_EXIT_TO_COPY_SOURCE_1;
        Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        if (!gSaveContext.flashSaveAvailable) {
            if (!NO_FLASH_SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
                this->copyDestFileIndex = this->buttonIndex;
                this->nextTitleLabel = FS_TITLE_COPY_CONFIRM;
                this->actionTimer = 4;
                this->configMode = CM_SETUP_COPY_CONFIRM_1;
                Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
            } else {
                Audio_PlaySfx(NA_SE_SY_FSEL_ERROR);
            }
        } else if (!SLOT_OCCUPIED(this, this->buttonIndex)) {
            this->copyDestFileIndex = this->buttonIndex;
            this->nextTitleLabel = FS_TITLE_COPY_CONFIRM;
            this->actionTimer = 4;
            this->configMode = CM_SETUP_COPY_CONFIRM_1;
            Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
        } else {
            Audio_PlaySfx(NA_SE_SY_FSEL_ERROR);
        }
    } else {
        if (ABS_ALT(this->stickAdjY) >= 30) {
            Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
            if (this->stickAdjY >= 30) {
                this->buttonIndex--;
                // Instead of removing File 3 entirely, the index is manually adjusted to skip it
                if (!CVarGetInteger("gEnhancements.Saving.FileSlot3", true) &&
                    this->buttonIndex == FS_BTN_COPY_FILE_3) {
                    this->buttonIndex = FS_BTN_COPY_FILE_2;
                }
                if (this->buttonIndex == this->selectedFileIndex) {
                    this->buttonIndex--;
                    if (this->buttonIndex < FS_BTN_COPY_FILE_1) {
                        this->buttonIndex = FS_BTN_COPY_QUIT;
                    }
                    // Instead of removing File 3 entirely, the index is manually adjusted to skip it
                    if (!CVarGetInteger("gEnhancements.Saving.FileSlot3", true) &&
                        this->buttonIndex == FS_BTN_COPY_FILE_3) {
                        this->buttonIndex = FS_BTN_COPY_FILE_2;
                    }
                } else if (this->buttonIndex < FS_BTN_COPY_FILE_1) {
                    this->buttonIndex = FS_BTN_COPY_QUIT;
                }
            } else {
                this->buttonIndex++;
                if (this->buttonIndex > FS_BTN_COPY_QUIT) {
                    this->buttonIndex = FS_BTN_COPY_FILE_1;
                }
                if (this->buttonIndex == this->selectedFileIndex) {
                    this->buttonIndex++;
                }
                // Instead of removing File 3 entirely, the index is manually adjusted to skip it
                if (!CVarGetInteger("gEnhancements.Saving.FileSlot3", true) &&
                    this->buttonIndex == FS_BTN_COPY_FILE_3) {
                    this->buttonIndex = FS_BTN_COPY_QUIT;
                }
            }
        }
        if (this->buttonIndex != FS_BTN_COPY_QUIT) {
            if (!gSaveContext.flashSaveAvailable) {
                if (NO_FLASH_SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
                    this->warningLabel = FS_WARNING_FILE_IN_USE;
                    this->warningButtonIndex = this->buttonIndex;
                    this->emptyFileTextAlpha = 255;
                } else {
                    this->warningLabel = FS_WARNING_NONE;
                }
            } else if (SLOT_OCCUPIED(this, this->buttonIndex)) {
                this->warningLabel = FS_WARNING_FILE_IN_USE;
                this->warningButtonIndex = this->buttonIndex;
                this->emptyFileTextAlpha = 255;
            } else {
                this->warningLabel = FS_WARNING_NONE;
            }
        }
    }
}

/**
 * Fade out file info, bring back the name box, and get ready to return to copy source screen.
 * Update function for `CM_EXIT_TO_COPY_SOURCE_1`
 */
void FileSelect_ExitToCopySource1(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->fileInfoAlpha[this->buttonIndex] -= 200 / 4;
    this->nameBoxAlpha[this->buttonIndex] += 200 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->nextTitleLabel = FS_TITLE_COPY_FROM;
        this->nameBoxAlpha[this->buttonIndex] = 200;
        this->fileInfoAlpha[this->buttonIndex] = 0;
        this->actionTimer = 4;
        this->configMode++; // CM_EXIT_TO_COPY_SOURCE_2
    }
}

/**
 * Move the buttons back into place and return to copy source select.
 * Update function for `CM_EXIT_TO_COPY_SOURCE_2`
 */
void FileSelect_ExitToCopySource2(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s32 yStep;

    for (i = 0; i < 3; i++) {
        yStep = ABS_ALT(this->buttonYOffsets[i] - sChooseFileYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= sChooseFileYOffsets[i]) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->buttonIndex = FS_BTN_COPY_QUIT;
        this->configMode = CM_SELECT_COPY_SOURCE;
    }
}

s16 D_80814264[] = { -56, -40, -24, 0 };

/**
 * Rearrange buttons on the screen to prepare for copy confirmation.
 * Update function for `CM_SETUP_COPY_CONFIRM_1`
 */
void FileSelect_SetupCopyConfirm1(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s32 yStep;

    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;

    for (i = 0; i < 3; i++) {
        if ((i != this->copyDestFileIndex) && (i != this->selectedFileIndex)) {
            this->fileButtonAlpha[i] -= 200 / 4;

            if (!gSaveContext.flashSaveAvailable) {
                if (NO_FLASH_SLOT_OCCUPIED(sramCtx, i)) {
                    this->connectorAlpha[i] -= 255 / 4;
                    this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                }
            } else if (SLOT_OCCUPIED(this, i)) {
                this->connectorAlpha[i] -= 255 / 4;
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
            }
        } else if (i == this->copyDestFileIndex) {
            yStep = ABS_ALT(this->buttonYOffsets[i] - D_80814264[i]) / this->actionTimer;
            this->buttonYOffsets[i] += yStep;

            if (this->buttonYOffsets[i] >= D_80814264[i]) {
                this->buttonYOffsets[i] = D_80814264[i];
            }
        }
    }

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->actionTimer = 4;
        this->configMode++;
    }
}

/**
 * Fade in the 'Yes' button before allowing the player to decide.
 * Update function for `CM_SETUP_COPY_CONFIRM_2`
 */
void FileSelect_SetupCopyConfirm2(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] += 200 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->configMode = CM_COPY_CONFIRM;
        this->buttonIndex = FS_BTN_CONFIRM_QUIT;
    }
}

/**
 * Allow the player to confirm the copy, or quit back to the destination select.
 * If yes is selected, the actual copy occurs in this function before moving on to the animation.
 * Update function for `CM_COPY_CONFIRM`
 */
void FileSelect_CopyConfirm(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = CONTROLLER1(&this->state);
    u16 time;

    if (((this->buttonIndex != FS_BTN_CONFIRM_YES) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actionTimer = 4;
        this->nextTitleLabel = FS_TITLE_COPY_TO;
        this->configMode = CM_RETURN_TO_COPY_DEST;
        Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        gSaveContext.save.time = time = CURRENT_TIME; // Set to itself with unused temp
        this->nameAlpha[this->copyDestFileIndex] = 0;
        this->fileInfoAlpha[this->copyDestFileIndex] = this->nameAlpha[this->copyDestFileIndex];
        this->nextTitleLabel = FS_TITLE_COPY_COMPLETE;
        this->actionTimer = 4;
        Sram_CopySave(this, sramCtx);
        if (!gSaveContext.flashSaveAvailable) {
            this->configMode = CM_COPY_ANIM_1;
        } else {
            Sram_SetFlashPagesDefault(sramCtx,
                                      gFlashSaveStartPages[this->copyDestFileIndex * FLASH_SAVE_MAIN_MULTIPLIER],
                                      gFlashSpecialSaveNumPages[this->copyDestFileIndex * FLASH_SAVE_MAIN_MULTIPLIER]);
            Sram_StartWriteToFlashDefault(sramCtx);
            this->configMode = CM_COPY_WAIT_FOR_FLASH_SAVE;
        }
        Rumble_Request(300.0f, 180, 20, 100);
        Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
    } else if (ABS_ALT(this->stickAdjY) >= 30) {
        Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
        this->buttonIndex ^= 1;
    }
}

/**
 * Update and wait for the save to flash to complete.
 * Update function for `CM_COPY_WAIT_FOR_FLASH_SAVE`
 */
void FileSelect_CopyWaitForFlashSave(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    u16 i;

    Sram_UpdateWriteToFlashDefault(sramCtx);

    if (sramCtx->status == 0) {
        this->configMode = CM_COPY_ANIM_1;

        for (i = 0; i < 6; i++) {
            this->newf[this->copyDestFileIndex][i] = gSaveContext.save.saveInfo.playerData.newf[i];
        }

        this->threeDayResetCount[this->copyDestFileIndex] = gSaveContext.save.saveInfo.playerData.threeDayResetCount;

        for (i = 0; i < 8; i++) {
            this->fileNames[this->copyDestFileIndex][i] = gSaveContext.save.saveInfo.playerData.playerName[i];
        }

        this->healthCapacity[this->copyDestFileIndex] = gSaveContext.save.saveInfo.playerData.healthCapacity;
        this->health[this->copyDestFileIndex] = gSaveContext.save.saveInfo.playerData.health;
        this->defenseHearts[this->copyDestFileIndex] = gSaveContext.save.saveInfo.inventory.defenseHearts;
    }
}

/**
 * Move buttons back in place and return to copy destination select.
 * Update function for `CM_RETURN_TO_COPY_DEST`
 */
void FileSelect_ReturnToCopyDest(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s32 yStep;

    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] -= 200 / 4;

    for (i = 0; i < 3; i++) {
        if ((i != this->copyDestFileIndex) && (i != this->selectedFileIndex)) {
            this->fileButtonAlpha[i] += 200 / 4;

            if (!gSaveContext.flashSaveAvailable) {
                if (NO_FLASH_SLOT_OCCUPIED(sramCtx, i)) {
                    this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                    this->connectorAlpha[i] += 255 / 4;
                }
            } else if (SLOT_OCCUPIED(this, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] += 255 / 4;
            }
        }

        yStep = ABS_ALT(this->buttonYOffsets[i] - D_8081424C[this->selectedFileIndex][i]) / this->actionTimer;

        if (D_8081424C[this->selectedFileIndex][i] >= this->buttonYOffsets[i]) {
            this->buttonYOffsets[i] += yStep;
        } else {
            this->buttonYOffsets[i] -= yStep;
        }
    }

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->actionTimer = 4;
        this->buttonIndex = FS_BTN_COPY_QUIT;
        this->configMode = CM_SELECT_COPY_DEST;
    }
}

/**
 * Hide title
 * Update function for `CM_COPY_ANIM_1`
 */
void FileSelect_CopyAnim1(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] -= 200 / 4;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] -= 200 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleAlpha[FS_TITLE_CUR] = 0;
        this->actionTimer = 4;
        this->configMode++; // CM_COPY_ANIM_2
    }
}

/**
 * Move a copy of the file window down and fade in the file info.
 * Update function for `CM_COPY_ANIM_2`
 */
void FileSelect_CopyAnim2(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s32 yStep;

    this->fileInfoAlpha[this->copyDestFileIndex] += 200 / 4;
    this->nameAlpha[this->copyDestFileIndex] += 200 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    yStep = ABS_ALT(this->fileNamesY[this->copyDestFileIndex] + 56) / this->actionTimer;
    this->fileNamesY[this->copyDestFileIndex] -= yStep;

    if (this->fileNamesY[this->copyDestFileIndex] <= -56) {
        this->fileNamesY[this->copyDestFileIndex] = -56;
    }

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 45;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->configMode++; // CM_COPY_ANIM_3
    }
}

/**
 * Play a sound effect to indicate that the copy has completed. Wait for a timer or for
 * the player to press a button before moving on.
 * Update function for `CM_COPY_ANIM_3`
 */
void FileSelect_CopyAnim3(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    Input* input = CONTROLLER1(&this->state);

    if (this->actionTimer == 38) {
        this->connectorAlpha[this->copyDestFileIndex] = 255;
        Audio_PlaySfx(NA_SE_EV_DIAMOND_SWITCH);
    }

    this->actionTimer--;

    if (this->actionTimer < 37) {
        if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B | BTN_START) || (this->actionTimer == 0)) {
            this->actionTimer = 4;
            this->nextTitleLabel = FS_TITLE_SELECT_FILE;
            Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
            this->configMode = CM_COPY_ANIM_4;
        }
    }
}

/**
 * Fade out the info boxes for both files and bring in their name boxes. Fade out title.
 * Update function for `CM_COPY_ANIM_4`
 */
void FileSelect_CopyAnim4(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->fileInfoAlpha[this->selectedFileIndex] -= 200 / 4;
    this->fileInfoAlpha[this->copyDestFileIndex] -= 200 / 4;
    this->nameBoxAlpha[this->selectedFileIndex] += 200 / 4;
    this->nameBoxAlpha[this->copyDestFileIndex] += 200 / 4;
    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->fileNamesY[this->copyDestFileIndex] = this->buttonYOffsets[3] = 0;
        this->actionTimer = 4;
        this->titleAlpha[FS_TITLE_CUR] = 0;
        this->configMode++;
    }
}

/**
 * Restore all buttons and labels back to their original place and go back to the main menu.
 * Update function for `CM_COPY_ANIM_5`
 */
void FileSelect_CopyAnim5(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s32 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS_ALT(this->buttonYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= 0) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] += 200 / 4;

            if (!gSaveContext.flashSaveAvailable) {
                if (NO_FLASH_SLOT_OCCUPIED(sramCtx, i)) {
                    this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                    this->connectorAlpha[i] += 255 / 4;
                }
            } else if (SLOT_OCCUPIED(this, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] += 255 / 4;
            }
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] += 200 / 4;
    this->actionButtonAlpha[FS_BTN_ACTION_ERASE] += 200 / 4;
    this->optionButtonAlpha += 200 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        for (i = 0; i < 3; i++) {
            this->connectorAlpha[i] = 0;
            this->fileButtonAlpha[i] = 200;
            this->nameBoxAlpha[i] = this->nameAlpha[i] = this->connectorAlpha[i];

            if (!gSaveContext.flashSaveAvailable) {
                if (NO_FLASH_SLOT_OCCUPIED(sramCtx, i)) {
                    this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                    this->connectorAlpha[i] = 255;
                }
            } else if (SLOT_OCCUPIED(this, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] = 255;
            }
        }

        this->fileNamesY[this->selectedFileIndex] = 0;
        this->highlightColor[3] = 70;
        this->highlightPulseDir = 1;
        this->highlightTimer = 20;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->configMode = CM_MAIN_MENU;
    }
}

/**
 * Exit from the copy source screen to the main menu. Return all buttons and labels to their original place.
 * Update function for `CM_COPY_RETURN_MAIN`
 */
void FileSelect_ExitCopyToMain(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 i;
    s32 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS_ALT(this->buttonYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= 0) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] += 200 / 4;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] -= 200 / 4;
    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = 200;
        this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 0;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->configMode = CM_MAIN_MENU;
    }

    this->optionButtonAlpha = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        this->actionButtonAlpha[FS_BTN_ACTION_COPY];
}

/**
 * Move buttons into place for the erase select screen and fade in the proper labels.
 * Update function for `CM_SETUP_ERASE_SELECT`
 */
void FileSelect_SetupEraseSelect(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 i;
    s32 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS_ALT(this->buttonYOffsets[i] - sChooseFileYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= sChooseFileYOffsets[i]) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] -= 100;
    this->actionButtonAlpha[FS_BTN_ACTION_ERASE] -= 100;
    this->optionButtonAlpha -= 100;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] += 200 / 4;

    if (this->actionButtonAlpha[FS_BTN_ACTION_COPY] <= 0) {
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
            this->optionButtonAlpha = 0;
    }

    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->highlightColor[3] = 70;
        this->highlightPulseDir = 1;
        this->highlightTimer = 20;
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
            this->optionButtonAlpha = 0;
        this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 200;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->buttonIndex = FS_BTN_ERASE_QUIT;
        this->configMode++; // CM_ERASE_SELECT
    }
}

/**
 * Allow the player to select a file to erase or exit back to the main menu.
 * Update function for `CM_ERASE_SELECT`
 */
void FileSelect_EraseSelect(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = CONTROLLER1(&this->state);

    if (((this->buttonIndex == FS_BTN_COPY_QUIT) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->buttonIndex = FS_BTN_MAIN_ERASE;
        this->actionTimer = 4;
        this->nextTitleLabel = FS_TITLE_SELECT_FILE;
        this->configMode = CM_EXIT_ERASE_TO_MAIN;
        this->warningLabel = FS_WARNING_NONE;
        Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {

        if (!gSaveContext.flashSaveAvailable) {
            if (NO_FLASH_SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
                this->actionTimer = 4;
                this->selectedFileIndex = this->buttonIndex;
                this->configMode = CM_SETUP_ERASE_CONFIRM_1;
                this->nextTitleLabel = FS_TITLE_ERASE_CONFIRM;
                Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
            } else {
                Audio_PlaySfx(NA_SE_SY_FSEL_ERROR);
            }
        } else if (SLOT_OCCUPIED(this, this->buttonIndex)) {
            this->actionTimer = 4;
            this->selectedFileIndex = this->buttonIndex;
            this->configMode = CM_SETUP_ERASE_CONFIRM_1;
            this->nextTitleLabel = FS_TITLE_ERASE_CONFIRM;
            Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
        } else {
            Audio_PlaySfx(NA_SE_SY_FSEL_ERROR);
        }
    } else {
        if (ABS_ALT(this->stickAdjY) >= 30) {
            Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);

            if (this->stickAdjY >= 30) {
                this->buttonIndex--;
                if (!CVarGetInteger("gEnhancements.Saving.FileSlot3", true) &&
                    this->buttonIndex == FS_BTN_ERASE_FILE_3) {
                    this->buttonIndex = FS_BTN_ERASE_FILE_2;
                }
                if (this->buttonIndex < FS_BTN_ERASE_FILE_1) {
                    this->buttonIndex = FS_BTN_ERASE_QUIT;
                }
            } else {
                this->buttonIndex++;
                if (!CVarGetInteger("gEnhancements.Saving.FileSlot3", true) &&
                    this->buttonIndex == FS_BTN_ERASE_FILE_3) {
                    this->buttonIndex = FS_BTN_ERASE_QUIT;
                }
                if (this->buttonIndex > FS_BTN_ERASE_QUIT) {
                    this->buttonIndex = FS_BTN_ERASE_FILE_1;
                }
            }
        }

        if (this->buttonIndex != FS_BTN_ERASE_QUIT) {
            if (!gSaveContext.flashSaveAvailable) {
                if (!NO_FLASH_SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
                    this->warningLabel = FS_WARNING_FILE_EMPTY;
                    this->warningButtonIndex = this->buttonIndex;
                    this->emptyFileTextAlpha = 255;
                } else {
                    this->warningLabel = FS_WARNING_NONE;
                }
            } else if (!SLOT_OCCUPIED(this, this->buttonIndex)) {
                this->warningLabel = FS_WARNING_FILE_EMPTY;
                this->warningButtonIndex = this->buttonIndex;
                this->emptyFileTextAlpha = 255;
            } else {
                this->warningLabel = FS_WARNING_NONE;
            }
        } else {
            this->warningLabel = FS_WARNING_NONE;
        }
    }
}

s16 D_8081426C[] = { 0, 16, 32 };

/**
 * Update function for `CM_SETUP_ERASE_CONFIRM_1`
 */
void FileSelect_SetupEraseConfirm1(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s32 yStep;

    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] -= 200 / 4;

            if (!gSaveContext.flashSaveAvailable) {
                if (NO_FLASH_SLOT_OCCUPIED(sramCtx, i)) {
                    this->connectorAlpha[i] -= 255 / 4;
                    this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                }
            } else if (SLOT_OCCUPIED(this, i)) {
                this->connectorAlpha[i] -= 255 / 4;
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
            }
        } else {
            this->nameBoxAlpha[i] -= 200 / 4;
        }
    }
    this->titleAlpha[FS_TITLE_CUR] -= 255 / 8;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 8;

    yStep = ABS_ALT(this->buttonYOffsets[this->buttonIndex] - D_8081426C[this->buttonIndex]) / this->actionTimer;

    if (this->buttonYOffsets[this->buttonIndex] >= D_8081426C[this->buttonIndex]) {
        this->buttonYOffsets[this->buttonIndex] -= yStep;
    } else {
        this->buttonYOffsets[this->buttonIndex] += yStep;
    }

    this->actionTimer--;

    if (this->actionTimer == 0) {
        for (i = 0; i < 3; i++) {
            if (i != this->buttonIndex) {
                this->fileButtonAlpha[i] = 0;

                if (!gSaveContext.flashSaveAvailable) {
                    if (NO_FLASH_SLOT_OCCUPIED(sramCtx, i)) {
                        this->connectorAlpha[i] = 0;
                        this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i] = 0;
                    }
                } else if (SLOT_OCCUPIED(this, i)) {
                    this->connectorAlpha[i] = 0;
                    this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i] = 0;
                }
            } else {
                this->nameBoxAlpha[i] = 0;
            }
        }

        this->buttonYOffsets[this->buttonIndex] = D_8081426C[this->buttonIndex];
        this->actionTimer = 4;
        this->configMode++;
    }
}

/**
 * Show the file info of the file selected to erase.
 * Update function for `CM_SETUP_ERASE_CONFIRM_2`
 */
void FileSelect_SetupEraseConfirm2(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] += 200 / 4;
    this->titleAlpha[FS_TITLE_CUR] -= 255 / 8;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 8;
    this->fileInfoAlpha[this->buttonIndex] += 200 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 4;
        this->titleLabel = this->nextTitleLabel;
        this->fileInfoAlpha[this->buttonIndex] = this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] = 200;
        this->buttonIndex = FS_BTN_CONFIRM_QUIT;
        this->configMode = CM_ERASE_CONFIRM;
    }
}

/**
 * Allow the player to confirm their choice to erase or return back to erase select.
 * Update function for `CM_ERASE_CONFIRM`
 */
void FileSelect_EraseConfirm(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = CONTROLLER1(&this->state);

    if (((this->buttonIndex != FS_BTN_CONFIRM_YES) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->buttonIndex = this->selectedFileIndex;
        this->nextTitleLabel = FS_TITLE_ERASE_FILE;
        this->configMode = CM_EXIT_TO_ERASE_SELECT_1;
        this->actionTimer = 4;
        Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        Sram_EraseSave(this, sramCtx, this->selectedFileIndex);
        if (!gSaveContext.flashSaveAvailable) {
            this->configMode = CM_ERASE_ANIM_1;
        } else {
            Sram_SetFlashPagesDefault(sramCtx,
                                      gFlashSaveStartPages[this->selectedFileIndex * FLASH_SAVE_MAIN_MULTIPLIER],
                                      gFlashSpecialSaveNumPages[this->selectedFileIndex * FLASH_SAVE_MAIN_MULTIPLIER]);
            Sram_StartWriteToFlashDefault(sramCtx);
            this->configMode = CM_ERASE_WAIT_FOR_FLASH_SAVE;
        }
        this->connectorAlpha[this->selectedFileIndex] = 0;
        Audio_PlaySfx(NA_SE_EV_DIAMOND_SWITCH);
        this->actionTimer = 4;
        this->nextTitleLabel = FS_TITLE_ERASE_COMPLETE;
        Rumble_Request(200.0f, 255, 20, 150);
        sEraseDelayTimer = 15;
    } else if (ABS_ALT(this->stickAdjY) >= 30) {
        Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
        this->buttonIndex ^= 1;
    }
}

/**
 * Fade out file info, bring back the name box, and get ready to return to erase select screen.
 * Update function for `CM_EXIT_TO_ERASE_SELECT_1`
 */
void FileSelect_ExitToEraseSelect1(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->fileInfoAlpha[this->buttonIndex] -= 200 / 4;
    this->nameBoxAlpha[this->buttonIndex] += 200 / 4;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] -= 200 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 4;
        this->fileInfoAlpha[this->buttonIndex] = 0;
        this->configMode++; // CM_EXIT_TO_ERASE_SELECT_2
    }
}

/**
 * Move the buttons back into place and return to erase select.
 * Update function for `CM_EXIT_TO_ERASE_SELECT_2`
 */
void FileSelect_ExitToEraseSelect2(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s32 yStep;

    yStep =
        ABS_ALT(this->buttonYOffsets[this->buttonIndex] - sChooseFileYOffsets[this->buttonIndex]) / this->actionTimer;

    if (this->buttonYOffsets[this->buttonIndex] >= sChooseFileYOffsets[this->buttonIndex]) {
        this->buttonYOffsets[this->buttonIndex] -= yStep;
    } else {
        this->buttonYOffsets[this->buttonIndex] += yStep;
    }

    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] += 200 / 4;

            if (!gSaveContext.flashSaveAvailable) {
                if (NO_FLASH_SLOT_OCCUPIED(sramCtx, i)) {
                    this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                    this->connectorAlpha[i] += 255 / 4;
                }
            } else if (SLOT_OCCUPIED(this, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] += 255 / 4;
            }
        }
    }

    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->buttonYOffsets[this->buttonIndex] = sChooseFileYOffsets[this->buttonIndex];
        this->actionTimer = 4;
        this->buttonIndex = FS_BTN_ERASE_QUIT;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->configMode = CM_ERASE_SELECT;
    }
}

/**
 * Wait for an initial delay, then start fading out the selected file.
 * The actual file deletion occurs in this function.
 * Update function for `CM_ERASE_ANIM_1`
 */
void FileSelect_EraseAnim1(GameState* thisx) {
    static s16 D_80814E80;
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;

    if (sEraseDelayTimer == 0) {
        if (this->actionTimer == 4) {
            D_80814E80 = 1;
        }

        if (this->actionTimer != 0) {
            this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
            this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
            this->fileInfoAlpha[this->selectedFileIndex] -= 200 / 4;
            this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] -= 200 / 4;
            this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] -= 200 / 4;
        }

        this->fileNamesY[this->selectedFileIndex] -= D_80814E80;
        D_80814E80 += 2;

        this->actionTimer--;
        if (this->actionTimer == 0) {
            this->configMode = CM_ERASE_ANIM_2;
            this->titleLabel = this->nextTitleLabel;
            this->titleAlpha[FS_TITLE_CUR] = 255;
            this->titleAlpha[FS_TITLE_NEXT] = this->connectorAlpha[this->selectedFileIndex] = 0;

            //! FAKE: there should be a better chained assignment
            this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] = this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 0;
            if (1) {}
            if (0) {}
            this->fileInfoAlpha[this->selectedFileIndex] = this->nameAlpha[this->selectedFileIndex] =
                this->nameBoxAlpha[this->selectedFileIndex] = this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT];

            this->actionTimer = 45;
        }
    } else {
        sEraseDelayTimer--;

        if (sEraseDelayTimer == 0) {
            Audio_PlaySfx(NA_SE_OC_ABYSS);
        }
    }
}

/**
 * Update and wait for the save to flash to complete.
 * Update function for `CM_ERASE_WAIT_FOR_FLASH_SAVE`
 */
void FileSelect_EraseWaitForFlashSave(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    u16 i;

    Sram_UpdateWriteToFlashDefault(sramCtx);

    if (sramCtx->status == 0) {
        this->configMode = CM_ERASE_ANIM_1;
        for (i = 0; i < 6; i++) {
            this->newf[this->selectedFileIndex][i] = gSaveContext.save.saveInfo.playerData.newf[i];
        }
    }
}

/**
 * Wait for a delay timer or for the palyer to press a button before returning to the main menu.
 * Update function for `CM_ERASE_ANIM_2`
 */
void FileSelect_EraseAnim2(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    Input* input = CONTROLLER1(&this->state);

    if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B | BTN_START) || (--this->actionTimer == 0)) {
        this->buttonYOffsets[3] = 0;
        this->actionTimer = 4;
        this->nextTitleLabel = FS_TITLE_SELECT_FILE;
        this->configMode++; // CM_ERASE_ANIM_3
        Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
    }
}

/**
 * Exit from the erase animation to the main menu. Return all buttons and labels to their original place.
 * Update function for `CM_ERASE_ANIM_3`
 */
void FileSelect_EraseAnim3(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s32 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS_ALT(this->buttonYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= 0) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    for (i = 0; i < 3; i++) {
        this->fileButtonAlpha[i] += 200 / 4;

        if (!gSaveContext.flashSaveAvailable) {
            if (NO_FLASH_SLOT_OCCUPIED(sramCtx, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] += 255 / 4;
            }
        } else if (SLOT_OCCUPIED(this, i)) {
            this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
            this->connectorAlpha[i] += 255 / 4;
        }
    }

    if (this->fileButtonAlpha[this->selectedFileIndex] >= 200) {
        this->fileButtonAlpha[this->selectedFileIndex] = 200;
    }

    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->fileNamesY[this->selectedFileIndex] = 0;
        this->highlightColor[3] = 70;
        this->highlightPulseDir = 1;
        this->highlightTimer = 20;
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = 200;
        this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] = this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 0;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->configMode = CM_MAIN_MENU;
    }

    this->optionButtonAlpha = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        this->actionButtonAlpha[FS_BTN_ACTION_COPY];
}

/**
 * Exit from the erase select screen to the main menu. Return all buttons and labels to their original place.
 * Update function for `CM_EXIT_ERASE_TO_MAIN`
 */
void FileSelect_ExitEraseToMain(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 i;
    s32 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS_ALT(this->buttonYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= 0) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] += 200 / 4;
    this->actionButtonAlpha[FS_BTN_ACTION_ERASE] += 200 / 4;
    this->optionButtonAlpha += 200 / 4;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] -= 200 / 2;

    if (this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] <= 0) {
        this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 0;
    }

    this->titleAlpha[FS_TITLE_CUR] -= 255 / 4;
    this->titleAlpha[FS_TITLE_NEXT] += 255 / 4;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->highlightColor[3] = 70;
        this->highlightPulseDir = 1;
        this->highlightTimer = 20;
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = 200;
        this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 0;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[FS_TITLE_CUR] = 255;
        this->titleAlpha[FS_TITLE_NEXT] = 0;
        this->configMode = CM_MAIN_MENU;
    }

    this->optionButtonAlpha = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        this->actionButtonAlpha[FS_BTN_ACTION_COPY];
}
