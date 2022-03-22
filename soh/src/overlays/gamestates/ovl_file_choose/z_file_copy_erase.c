#include "file_choose.h"

// when choosing a file to copy or erase, the 6 main menu buttons are placed at these offsets
static s16 sChooseFileYOffsets[] = { -48, -48, -48, -24, -24, 0 };

static s16 D_8081248C[3][3] = {
    { 0, -48, -48 },
    { -64, 16, -48 },
    { -64, -64, 32 },
};

static s16 sEraseDelayTimer = 15;

/**
 * Move buttons into place for the select source screen and fade in the proper labels.
 * Update function for `CM_SETUP_COPY_SOURCE`
 */
void FileChoose_SetupCopySource(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 yStep;
    s16 i;

    for (i = 0; i < 5; i++) {
        yStep = (ABS(this->buttonYOffsets[i] - sChooseFileYOffsets[i])) / this->actionTimer;

        if (this->buttonYOffsets[i] >= sChooseFileYOffsets[i]) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] -= 25;
    this->actionButtonAlpha[FS_BTN_ACTION_ERASE] -= 25;
    this->optionButtonAlpha -= 25;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] += 25;
    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 8;

        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
            this->optionButtonAlpha = 0;

        this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 200;
        this->titleLabel = this->nextTitleLabel;

        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->buttonIndex = FS_BTN_COPY_QUIT;
        this->configMode++;
    }
}

/**
 * Allow the player to select a file to copy or exit back to the main menu.
 * Update function for `CM_SELECT_COPY_SOURCE`
 */
void FileChoose_SelectCopySource(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = &this->state.input[0];

    if (((this->buttonIndex == FS_BTN_COPY_QUIT) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actionTimer = 8;
        this->buttonIndex = FS_BTN_MAIN_COPY;
        this->nextTitleLabel = FS_TITLE_SELECT_FILE;
        this->configMode = CM_COPY_RETURN_MAIN;
        this->warningLabel = FS_WARNING_NONE;
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        if (SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
            this->actionTimer = 8;
            this->selectedFileIndex = this->buttonIndex;
            this->configMode = CM_SETUP_COPY_DEST_1;
            this->nextTitleLabel = FS_TITLE_COPY_TO;
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    } else {
        if (ABS(this->stickRelY) >= 30) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

            if (this->stickRelY >= 30) {
                this->buttonIndex--;

                if (this->buttonIndex < FS_BTN_COPY_FILE_1) {
                    this->buttonIndex = FS_BTN_COPY_QUIT;
                }
            } else {
                this->buttonIndex++;

                if (this->buttonIndex > FS_BTN_COPY_QUIT) {
                    this->buttonIndex = FS_BTN_COPY_FILE_1;
                }
            }
        }

        if (this->buttonIndex != FS_BTN_COPY_QUIT) {
            if (!SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
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
void FileChoose_SetupCopyDest1(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 yStep;
    s16 i;

    for (i = 0; i < 3; i++) {
        yStep = ABS(this->buttonYOffsets[i] - D_8081248C[this->buttonIndex][i]) / this->actionTimer;

        if (D_8081248C[this->buttonIndex][i] >= this->buttonYOffsets[i]) {
            this->buttonYOffsets[i] += yStep;
        } else {
            this->buttonYOffsets[i] -= yStep;
        }
    }

    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->nameBoxAlpha[this->buttonIndex] -= 25;

    this->actionTimer--;
    if (this->actionTimer == 0) {
        this->buttonYOffsets[this->buttonIndex] = D_8081248C[this->buttonIndex][this->buttonIndex];
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->actionTimer = 8;
        this->configMode++;
    }
}

/**
 * Show the file info of the file selected to copy from.
 * Update function for `CM_SETUP_COPY_DEST_2`
 */
void FileChoose_SetupCopyDest2(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->nameBoxAlpha[this->buttonIndex] -= 25;
    this->fileInfoAlpha[this->buttonIndex] += 25;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->nameBoxAlpha[this->buttonIndex] = 0;
        this->fileInfoAlpha[this->buttonIndex] = 200;
        this->buttonIndex = FS_BTN_COPY_QUIT;
        this->actionTimer = 8;
        this->configMode = CM_SELECT_COPY_DEST;
    }
}

/**
 * Allow the player to select a slot to copy to or exit to the copy select screen.
 * Update function for `CM_SELECT_COPY_DEST`
 */
void FileChoose_SelectCopyDest(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = &this->state.input[0];

    if (((this->buttonIndex == FS_BTN_COPY_QUIT) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->buttonIndex = this->selectedFileIndex;
        this->nextTitleLabel = FS_TITLE_COPY_FROM;
        this->actionTimer = 8;
        this->configMode = CM_EXIT_TO_COPY_SOURCE_1;
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        if (!SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
            this->copyDestFileIndex = this->buttonIndex;
            this->nextTitleLabel = FS_TITLE_COPY_CONFIRM;
            this->actionTimer = 8;
            this->configMode = CM_SETUP_COPY_CONFIRM_1;
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    } else {

        if (ABS(this->stickRelY) >= 30) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

            if (this->stickRelY >= 30) {
                this->buttonIndex--;

                if ((this->buttonIndex == this->selectedFileIndex)) {
                    this->buttonIndex--;

                    if (this->buttonIndex < FS_BTN_COPY_FILE_1) {
                        this->buttonIndex = FS_BTN_COPY_QUIT;
                    }
                } else {
                    if (this->buttonIndex < FS_BTN_COPY_FILE_1) {
                        this->buttonIndex = FS_BTN_COPY_QUIT;
                    }
                }
            } else {
                this->buttonIndex++;

                if (this->buttonIndex > FS_BTN_COPY_QUIT) {
                    this->buttonIndex = FS_BTN_COPY_FILE_1;
                }

                if (this->buttonIndex == this->selectedFileIndex) {
                    this->buttonIndex++;
                }
            }
        }

        if (this->buttonIndex != FS_BTN_COPY_QUIT) {
            if (SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
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
void FileChoose_ExitToCopySource1(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->fileInfoAlpha[this->buttonIndex] -= 25;
    this->nameBoxAlpha[this->buttonIndex] += 25;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->nextTitleLabel = FS_TITLE_COPY_FROM;
        this->nameBoxAlpha[this->buttonIndex] = 200;
        this->fileInfoAlpha[this->buttonIndex] = 0;
        this->actionTimer = 8;
        this->configMode++;
    }
}

/**
 * Move the buttons back into place and return to copy source select.
 * Update function for `CM_EXIT_TO_COPY_SOURCE_2`
 */
void FileChoose_ExitToCopySource2(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 3; i++) {
        yStep = ABS(this->buttonYOffsets[i] - sChooseFileYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= sChooseFileYOffsets[i]) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->buttonIndex = 3;
        this->configMode = CM_SELECT_COPY_SOURCE;
    }
}

/**
 * Rearrange buttons on the screen to prepare for copy confirmation.
 * Update function for `CM_SETUP_COPY_CONFIRM_1`
 */
void FileChoose_SetupCopyConfirm1(GameState* thisx) {
    static s16 D_808124A4[] = { -56, -40, -24, 0 };
    FileChooseContext* this = (FileChooseContext*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;

    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;

    for (i = 0; i < 3; i++) {
        if ((i != this->copyDestFileIndex) && (i != this->selectedFileIndex)) {
            this->fileButtonAlpha[i] -= 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                this->connectorAlpha[i] -= 31;
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
            }
        } else {
            if (this->copyDestFileIndex == i) {
                yStep = ABS(this->buttonYOffsets[i] - D_808124A4[i]) / this->actionTimer;
                this->buttonYOffsets[i] += yStep;

                if (this->buttonYOffsets[i] >= D_808124A4[i]) {
                    this->buttonYOffsets[i] = D_808124A4[i];
                }
            }
        }
    }

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->actionTimer = 8;
        this->configMode++;
    }
}

/**
 * Fade in the 'Yes' button before allowing the player to decide.
 * Update function for `CM_SETUP_COPY_CONFIRM_2`
 */
void FileChoose_SetupCopyConfirm2(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] += 25;
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
void FileChoose_CopyConfirm(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = &this->state.input[0];
    u16 dayTime;

    if (((this->buttonIndex != FS_BTN_CONFIRM_YES) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actionTimer = 8;
        this->nextTitleLabel = FS_TITLE_COPY_TO;
        this->configMode = CM_RETURN_TO_COPY_DEST;
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        dayTime = gSaveContext.dayTime;
        Sram_CopySave(this, sramCtx);
        gSaveContext.dayTime = dayTime;
        this->fileInfoAlpha[this->copyDestFileIndex] = this->nameAlpha[this->copyDestFileIndex] = 0;
        this->nextTitleLabel = FS_TITLE_COPY_COMPLETE;
        this->actionTimer = 8;
        this->configMode = CM_COPY_ANIM_1;
        func_800AA000(300.0f, 0xB4, 0x14, 0x64);
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (ABS(this->stickRelY) >= 30) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        this->buttonIndex ^= 1;
    }
}

/**
 * Move buttons back in place and return to copy destination select.
 * Update function for `CM_RETURN_TO_COPY_DEST`
 */
void FileChoose_ReturnToCopyDest(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;

    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] -= 25;

    for (i = 0; i < 3; i++) {
        if ((i != this->copyDestFileIndex) && (i != this->selectedFileIndex)) {
            this->fileButtonAlpha[i] += 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] += 31;
            }
        }

        yStep = ABS(this->buttonYOffsets[i] - D_8081248C[this->selectedFileIndex][i]) / this->actionTimer;

        if (D_8081248C[this->selectedFileIndex][i] >= this->buttonYOffsets[i]) {
            this->buttonYOffsets[i] += yStep;
        } else {
            this->buttonYOffsets[i] -= yStep;
        }
    }

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->actionTimer = 8;
        this->buttonIndex = FS_BTN_COPY_QUIT;
        this->configMode = CM_SELECT_COPY_DEST;
    }
}

/**
 * Hide title
 * Update function for `CM_COPY_ANIM_1`
 */
void FileChoose_CopyAnim1(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->titleAlpha[0] -= 31;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] -= 25;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] -= 25;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleAlpha[0] = 0;
        this->actionTimer = 8;
        this->configMode++;
        osSyncPrintf("connect_alpha=%d  decision_alpha[%d]=%d\n", this->connectorAlpha[this->copyDestFileIndex],
                     this->copyDestFileIndex, this->fileInfoAlpha[this->copyDestFileIndex]);
    }
}

/**
 * Move a copy of the file window down and fade in the file info.
 * Update function for `CM_COPY_ANIM_2`
 */
void FileChoose_CopyAnim2(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 yStep;

    this->fileInfoAlpha[this->copyDestFileIndex] += 25;
    this->nameAlpha[this->copyDestFileIndex] += 25;
    this->titleAlpha[1] += 31;
    yStep = ABS(this->fileNamesY[this->copyDestFileIndex] + 56) / this->actionTimer;
    this->fileNamesY[this->copyDestFileIndex] -= yStep;

    if (this->fileNamesY[this->copyDestFileIndex] <= -56) {
        this->fileNamesY[this->copyDestFileIndex] = -56;
    }

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 90;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->configMode++;
    }
}

/**
 * Play sound to indicate that the copy has completed. Wait for a timer or for
 * the player to press a button before moving on.
 * Update function for `CM_COPY_ANIM_3`
 */
void FileChoose_CopyAnim3(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    Input* input = &this->state.input[0];

    if (this->actionTimer == 75) {
        this->connectorAlpha[this->copyDestFileIndex] = 255;
        Audio_PlaySoundGeneral(NA_SE_EV_DIAMOND_SWITCH, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }

    this->actionTimer--;

    if (this->actionTimer < 74) {
        if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B | BTN_START) || (this->actionTimer == 0)) {
            this->actionTimer = 8;
            this->nextTitleLabel = FS_TITLE_SELECT_FILE;
            this->configMode++;
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }
}

/**
 * Fade out the info boxes for both files and bring in their name boxes. Fade out title.
 * Update function for `CM_COPY_ANIM_4`
 */
void FileChoose_CopyAnim4(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->fileInfoAlpha[this->selectedFileIndex] -= 25;
    this->fileInfoAlpha[this->copyDestFileIndex] -= 25;
    this->nameBoxAlpha[this->selectedFileIndex] += 25;
    this->nameBoxAlpha[this->copyDestFileIndex] += 25;
    this->titleAlpha[0] -= 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->fileNamesY[this->copyDestFileIndex] = this->buttonYOffsets[3] = 0;
        this->actionTimer = 8;
        this->titleAlpha[0] = 0;
        this->configMode++;
    }
}

/**
 * Restore all buttons and labels back to their original place and go back to the main menu.
 * Update function for `CM_COPY_ANIM_5`
 */
void FileChoose_CopyAnim5(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS(this->buttonYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= 0) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] += 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] += 31;
            }
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] += 25;
    this->actionButtonAlpha[FS_BTN_ACTION_ERASE] += 25;
    this->optionButtonAlpha += 25;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        for (i = 0; i < 3; i++) {
            this->connectorAlpha[i] = 0;
            this->fileButtonAlpha[i] = 200;
            this->nameBoxAlpha[i] = this->nameAlpha[i] = this->connectorAlpha[i];

            if (SLOT_OCCUPIED(sramCtx, i)) {
                this->connectorAlpha[i] = 255;
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
            }
        }

        this->fileNamesY[this->selectedFileIndex] = 0;
        this->highlightColor[3] = 70;
        this->highlightPulseDir = 1;
        XREG(35) = XREG(36);
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->configMode = CM_MAIN_MENU;
    }
}

/**
 * Exit from the copy source screen to the main menu. Return all buttons and labels to their original place.
 * Update function for `CM_COPY_RETURN_MAIN`
 */
void FileChoose_ExitCopyToMain(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS(this->buttonYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= 0) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] += 25;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] -= 25;
    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = 200;
        this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 0;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->configMode = CM_MAIN_MENU;
    }

    this->optionButtonAlpha = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        this->actionButtonAlpha[FS_BTN_ACTION_COPY];
}

/**
 * Move buttons into place for the erase select screen and fade in the proper labels.
 * Update function for `CM_SETUP_ERASE_SELECT`
 */
void FileChoose_SetupEraseSelect(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS(this->buttonYOffsets[i] - sChooseFileYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= sChooseFileYOffsets[i]) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] -= 50;
    this->actionButtonAlpha[FS_BTN_ACTION_ERASE] -= 50;
    this->optionButtonAlpha -= 50;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] += 25;

    if (this->actionButtonAlpha[FS_BTN_ACTION_COPY] <= 0) {
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
            this->optionButtonAlpha = 0;
    }

    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->highlightColor[3] = 70;
        this->highlightPulseDir = 1;
        XREG(35) = XREG(36);
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
            this->optionButtonAlpha = 0;
        this->confirmButtonAlpha[1] = 200;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->buttonIndex = FS_BTN_ERASE_QUIT;
        this->configMode++;
    }
}

/**
 * Allow the player to select a file to erase or exit back to the main menu.
 * Update function for `CM_ERASE_SELECT`
 */
void FileChoose_EraseSelect(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = &this->state.input[0];

    if (((this->buttonIndex == FS_BTN_COPY_QUIT) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->buttonIndex = FS_BTN_MAIN_ERASE;
        this->actionTimer = 8;
        this->nextTitleLabel = FS_TITLE_SELECT_FILE;
        this->configMode = CM_EXIT_ERASE_TO_MAIN;
        this->warningLabel = FS_WARNING_NONE;
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        if (SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
            this->actionTimer = 8;
            this->selectedFileIndex = this->buttonIndex;
            this->configMode = CM_SETUP_ERASE_CONFIRM_1;
            this->nextTitleLabel = FS_TITLE_ERASE_CONFIRM;
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    } else {
        if (ABS(this->stickRelY) >= 30) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

            if (this->stickRelY >= 30) {
                this->buttonIndex--;
                if (this->buttonIndex < FS_BTN_ERASE_FILE_1) {
                    this->buttonIndex = FS_BTN_ERASE_QUIT;
                }
            } else {
                this->buttonIndex++;
                if (this->buttonIndex > FS_BTN_ERASE_QUIT) {
                    this->buttonIndex = FS_BTN_ERASE_FILE_1;
                }
            }
        }

        if (this->buttonIndex != FS_BTN_ERASE_QUIT) {
            if (!SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
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

/**
 * ...
 * Update function for `CM_SETUP_ERASE_CONFIRM_1`
 */
void FileChoose_SetupEraseConfirm1(GameState* thisx) {
    static s16 D_808124AC[] = { 0, 16, 32 };
    FileChooseContext* this = (FileChooseContext*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] -= 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                this->connectorAlpha[i] -= 31;
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
            }
        } else {
            this->nameBoxAlpha[i] -= 25;
        }
    }
    this->titleAlpha[0] -= 15;
    this->titleAlpha[1] += 15;

    yStep = ABS(this->buttonYOffsets[this->buttonIndex] - D_808124AC[this->buttonIndex]) / this->actionTimer;

    if (this->buttonYOffsets[this->buttonIndex] >= D_808124AC[this->buttonIndex]) {
        this->buttonYOffsets[this->buttonIndex] -= yStep;
    } else {
        this->buttonYOffsets[this->buttonIndex] += yStep;
    }

    this->actionTimer--;

    if (this->actionTimer == 0) {
        for (i = 0; i < 3; i++) {
            if (i != this->buttonIndex) {
                this->fileButtonAlpha[i] = 0;

                if (SLOT_OCCUPIED(sramCtx, i)) {
                    this->connectorAlpha[i] = 0;
                    this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i] = 0;
                }
            } else {
                this->nameBoxAlpha[i] = 0;
            }
        }

        this->buttonYOffsets[this->buttonIndex] = D_808124AC[this->buttonIndex];
        this->actionTimer = 8;
        this->configMode++;
    }
}

/**
 * Show the file info of the file selected to erase.
 * Update function for `CM_SETUP_ERASE_CONFIRM_2`
 */
void FileChoose_SetupEraseConfirm2(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] += 25;
    this->titleAlpha[0] -= 15;
    this->titleAlpha[1] += 15;
    this->fileInfoAlpha[this->buttonIndex] += 25;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 8;
        this->titleLabel = this->nextTitleLabel;
        this->fileInfoAlpha[this->buttonIndex] = this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] = 200;
        this->buttonIndex = FS_BTN_ERASE_FILE_2;
        this->configMode = CM_ERASE_CONFIRM;
    }
}

/**
 * Allow the player to confirm their chioce to erase or return back to erase select.
 * Update function for `CM_ERASE_CONFIRM`
 */
void FileChoose_EraseConfirm(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    Input* input = &this->state.input[0];

    if (((this->buttonIndex != FS_BTN_CONFIRM_YES) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->buttonIndex = this->selectedFileIndex;
        this->nextTitleLabel = FS_TITLE_ERASE_FILE;
        this->configMode = CM_EXIT_TO_ERASE_SELECT_1;
        this->actionTimer = 8;
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        this->n64ddFlags[this->selectedFileIndex] = this->connectorAlpha[this->selectedFileIndex] = 0;
        Audio_PlaySoundGeneral(NA_SE_EV_DIAMOND_SWITCH, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        this->actionTimer = 8;
        this->configMode = CM_ERASE_ANIM_1;
        this->nextTitleLabel = FS_TITLE_ERASE_COMPLETE;
        func_800AA000(200.0f, 0xFF, 0x14, 0x96);
        sEraseDelayTimer = 15;
    } else if (ABS(this->stickRelY) >= 30) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        this->buttonIndex ^= 1;
    }
}

/**
 * Fade out file info, bring back the name box, and get ready to return to erase select screen.
 * Update function for `CM_EXIT_TO_ERASE_SELECT_1`
 */
void FileChoose_ExitToEraseSelect1(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->fileInfoAlpha[this->buttonIndex] -= 25;
    this->nameBoxAlpha[this->buttonIndex] += 25;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] -= 25;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 8;
        this->fileInfoAlpha[this->buttonIndex] = 0;
        this->configMode++;
    }
}

/**
 * Move the buttons back into place and return to erase select.
 * Update function for `CM_EXIT_TO_ERASE_SELECT_2`
 */
void FileChoose_ExitToEraseSelect2(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;

    yStep = ABS(this->buttonYOffsets[this->buttonIndex] - sChooseFileYOffsets[this->buttonIndex]) / this->actionTimer;

    if (this->buttonYOffsets[this->buttonIndex] >= sChooseFileYOffsets[this->buttonIndex]) {
        this->buttonYOffsets[this->buttonIndex] -= yStep;
    } else {
        this->buttonYOffsets[this->buttonIndex] += yStep;
    }

    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] += 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] += 31;
            }
        }
    }

    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->buttonYOffsets[this->buttonIndex] = sChooseFileYOffsets[this->buttonIndex];
        this->actionTimer = 8;
        this->buttonIndex = FS_BTN_ERASE_QUIT;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->configMode = CM_ERASE_SELECT;
    }
}

/**
 * Wait for an initial delay, then start fading out the selected file.
 * The actual file deletion occurs in this function
 * Update function for `CM_ERASE_ANIM_1`
 */
void FileChoose_EraseAnim1(GameState* thisx) {
    static s16 D_80813800;
    FileChooseContext* this = (FileChooseContext*)thisx;
    SramContext* sramCtx = &this->sramCtx;

    if (sEraseDelayTimer == 0) {
        if (this->actionTimer == 8) {
            D_80813800 = 1;
        }

        if (this->actionTimer != 0) {
            this->titleAlpha[0] -= 31;
            this->titleAlpha[1] += 31;
            this->fileInfoAlpha[this->selectedFileIndex] -= 25;
            this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] -= 25;
            this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] -= 25;
        }

        this->fileNamesY[this->selectedFileIndex] -= D_80813800;
        this->actionTimer--;
        D_80813800 += 2;

        if (this->actionTimer == 0) {
            Sram_EraseSave(this, sramCtx);
            this->titleLabel = this->nextTitleLabel;
            this->titleAlpha[0] = 255;
            this->titleAlpha[1] = this->connectorAlpha[this->selectedFileIndex] = 0;

            // probably a fake match, there should be a better chained assignment
            this->confirmButtonAlpha[0] = this->confirmButtonAlpha[1] = 0;
            if (1) {}
            this->fileInfoAlpha[this->selectedFileIndex] = this->nameBoxAlpha[this->selectedFileIndex] =
                this->confirmButtonAlpha[1];

            this->configMode++;
            this->actionTimer = 90;
        }
    } else {
        sEraseDelayTimer--;

        if (sEraseDelayTimer == 0) {
            Audio_PlaySoundGeneral(NA_SE_OC_ABYSS, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }
}

/**
 * Wait for a delay timer or for the palyer to press a button before returning to the main menu.
 * Update function for `CM_ERASE_ANIM_2`
 */
void FileChoose_EraseAnim2(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    Input* input = &this->state.input[0];

    if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B | BTN_START) || (--this->actionTimer == 0)) {
        this->buttonYOffsets[3] = 0;
        this->actionTimer = 8;
        this->nextTitleLabel = FS_TITLE_SELECT_FILE;
        this->configMode++;
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

/**
 * Exit from the erase animation to the main menu. Return all buttons and labels to their original place.
 * Update function for `CM_ERASE_ANIM_3`
 */
void FileChoose_EraseAnim3(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS(this->buttonYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= 0) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    for (i = 0; i < 3; i++) {
        this->fileButtonAlpha[i] += 25;

        if (SLOT_OCCUPIED(sramCtx, i)) {
            this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
            this->connectorAlpha[i] += 31;
        }
    }

    if (this->fileButtonAlpha[this->selectedFileIndex] >= 200) {
        this->fileButtonAlpha[this->selectedFileIndex] = 200;
    }

    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->fileNamesY[this->selectedFileIndex] = 0;
        this->highlightColor[3] = 70;
        this->highlightPulseDir = 1;
        XREG(35) = XREG(36);
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = 200;
        this->confirmButtonAlpha[0] = this->confirmButtonAlpha[1] = 0;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->configMode = CM_MAIN_MENU;
    }

    this->optionButtonAlpha = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        this->actionButtonAlpha[FS_BTN_ACTION_COPY];
}

/**
 * Exit from the erase select screen to the main menu. Return all buttons and labels to their original place.
 * Update function for `CM_EXIT_ERASE_TO_MAIN`
 */
void FileChoose_ExitEraseToMain(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS(this->buttonYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= 0) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] += 25;
    this->actionButtonAlpha[FS_BTN_ACTION_ERASE] += 25;
    this->optionButtonAlpha += 25;
    this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] -= 50;

    if (this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] <= 0) {
        this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 0;
    }

    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->highlightColor[3] = 70;
        this->highlightPulseDir = 1;
        XREG(35) = XREG(36);
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = 200;
        this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 0;
        this->titleLabel = this->nextTitleLabel;
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->configMode = CM_MAIN_MENU;
    }

    this->optionButtonAlpha = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        this->actionButtonAlpha[FS_BTN_ACTION_COPY];
}
