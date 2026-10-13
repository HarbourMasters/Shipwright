#include "z_title_setup.h"
#include "overlays/gamestates/ovl_title/z_title.h"
#include "z64save.h"

void Setup_InitRegs(void) {
    XREG(2) = 0;
    XREG(10) = 26;
    XREG(11) = 20;
    XREG(12) = 14;
    XREG(13) = 0;
    R_A_BTN_Y_OFFSET = 0;
    R_MAGIC_CONSUME_TIMER_GIANTS_MASK = 80;

    R_THREE_DAY_CLOCK_Y_POS = 64596;
    R_THREE_DAY_CLOCK_SUN_MOON_CUTOFF = 215;
    R_THREE_DAY_CLOCK_HOUR_DIGIT_CUTOFF = 218;

    XREG(68) = 0x61;
    XREG(69) = 0x93;
    XREG(70) = 0x28;
    XREG(73) = 0x1E;
    XREG(74) = 0x42;
    XREG(75) = 0x1E;
    XREG(76) = 0x1C;
    XREG(77) = 0x3C;
    XREG(78) = 0x2F;
    XREG(79) = 0x62;
    R_PAUSE_OWL_WARP_ALPHA = 0;
    XREG(88) = 0x56;
    XREG(89) = 0x258;
    XREG(90) = 0x1C2;

    R_STORY_FILL_SCREEN_ALPHA = 0;
    R_PLAYER_FLOOR_REVERSE_INDEX = 0;
    R_MINIMAP_DISABLED = false;

    R_PICTO_FOCUS_BORDER_TOPLEFT_X = 80;
    R_PICTO_FOCUS_BORDER_TOPLEFT_Y = 60;
    R_PICTO_FOCUS_BORDER_TOPRIGHT_X = 220;
    R_PICTO_FOCUS_BORDER_TOPRIGHT_Y = 60;
    R_PICTO_FOCUS_BORDER_BOTTOMLEFT_X = 80;
    R_PICTO_FOCUS_BORDER_BOTTOMLEFT_Y = 160;
    R_PICTO_FOCUS_BORDER_BOTTOMRIGHT_X = 220;
    R_PICTO_FOCUS_BORDER_BOTTOMRIGHT_Y = 160;
    R_PICTO_FOCUS_ICON_X = 142;
    R_PICTO_FOCUS_ICON_Y = 108;
    R_PICTO_FOCUS_TEXT_X = 204;
    R_PICTO_FOCUS_TEXT_Y = 177;
}

void Setup_InitImpl(SetupState* this) {
    SysFlashrom_InitFlash();
    SaveContext_Init();
    Sram_LoadGlobalOptions();
    Setup_InitRegs();

    STOP_GAMESTATE(&this->state);
    SET_NEXT_GAMESTATE(&this->state, ConsoleLogo_Init, sizeof(ConsoleLogoState));
}

void Setup_Destroy(GameState* thisx) {
}

void Setup_Init(GameState* thisx) {
    SetupState* this = (SetupState*)thisx;

    this->state.destroy = Setup_Destroy;
    Setup_InitImpl(this);
}
