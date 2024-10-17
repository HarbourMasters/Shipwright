#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"

extern "C" {
    #include "src/overlays/actors/ovl_Bg_Treemouth/z_bg_treemouth.h"
}

/**
 * This will skip the Deku Tree intro, and simply open the mouth as you approach it.
*/
void SkipDekuTreeIntro_Register() {
    REGISTER_VB_SHOULD(VB_PLAY_DEKU_TREE_INTRO_CS, {
        if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)) {
            BgTreemouth* treeMouth = va_arg(args, BgTreemouth*);
            Flags_SetEventChkInf(EVENTCHKINF_DEKU_TREE_OPENED_MOUTH);
            Audio_PlaySoundGeneral(NA_SE_EV_WOODDOOR_OPEN, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            BgTreemouth_SetupAction(treeMouth, func_808BC6F8);
            *should = false;
        }
    });
}
