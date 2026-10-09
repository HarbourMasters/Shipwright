#include "global.h"
#include "z_prenmi.h"

void PreNMI_Stop(PreNMIState* this) {
    STOP_GAMESTATE(&this->state);
    SET_NEXT_GAMESTATE(&this->state, NULL, 0);
}

void PreNMI_Update(PreNMIState* this) {
    if (this->timer == 0) {
        ViConfig_UpdateVi(true);
        PreNMI_Stop(this);
    } else {
        this->timer--;
    }
}

void PreNMI_Draw(PreNMIState* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;

    func_8012CF0C(gfxCtx, true, true, 0, 0, 0);

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL36_Opa(gfxCtx);

    gDPSetFillColor(POLY_OPA_DISP++, (GPACK_RGBA5551(255, 255, 255, 1) << 16) | GPACK_RGBA5551(255, 255, 255, 1));
    gDPFillRectangle(POLY_OPA_DISP++, 0, this->timer + 100, SCREEN_WIDTH - 1, this->timer + 100);

    CLOSE_DISPS(gfxCtx);
}

void PreNMI_Main(GameState* thisx) {
    PreNMIState* this = (PreNMIState*)thisx;

    PreNMI_Update(this);
    PreNMI_Draw(this);

    this->state.unk_A3 = 1;
}

void PreNMI_Destroy(GameState* thisx) {
}

void PreNMI_Init(GameState* thisx) {
    PreNMIState* this = (PreNMIState*)thisx;

    this->state.main = PreNMI_Main;
    this->state.destroy = PreNMI_Destroy;
    this->timer = 30;
    this->unk_A8 = 10;

    GameState_SetFramerateDivisor(&this->state, 1);
}
