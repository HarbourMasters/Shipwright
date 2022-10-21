/*
 * File: z_opening.c
 * Overlay: ovl_opening
 * Description: Initializes the game into the title screen
 */

#include "global.h"

void Opening_SetupTitleScreen(OpeningContext* this) {
    gSaveContext.gameMode = 1;
    this->state.running = false;
    gSaveContext.linkAge = 0;
    gSaveContext.fileNum = 0xFF;
    gWalkSpeedToggle1 = 0;
    gWalkSpeedToggle2 = 0;
    Sram_InitDebugSave();
    gSaveContext.cutsceneIndex = 0xFFF3;
    gSaveContext.sceneSetupIndex = 7;
    SET_NEXT_GAMESTATE(&this->state, Gameplay_Init, GlobalContext);
}

void func_80803C5C(OpeningContext* this) {
}

void Opening_Main(GameState* thisx) {
    OpeningContext* this = (OpeningContext*)thisx;

    func_80095248(this->state.gfxCtx, 0, 0, 0);
    Opening_SetupTitleScreen(this);
    func_80803C5C(this);
}

void Opening_Destroy(GameState* thisx) {
}

void Opening_Init(GameState* thisx) {
    OpeningContext* this = (OpeningContext*)thisx;

    R_UPDATE_RATE = 1;
    Matrix_Init(&this->state);
    View_Init(&this->view, this->state.gfxCtx);
    this->state.main = Opening_Main;
    this->state.destroy = Opening_Destroy;
}
