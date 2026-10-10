#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Syateki_Okuta/z_en_syateki_okuta.h"
#include "assets/overlays/ovl_En_Syateki_Okuta/ovl_En_Syateki_Okuta.h"
extern void EnSyatekiOkuta_Die(EnSyatekiOkuta* enSyatekiOkuta, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Minigames.MarkShootingGalleryOctoroks"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// This is almost identical to how EnSyatekiOkuta_Draw draws the symbols, except this draws a white symbol before the
// Octorok has been hit.
void RegisterMarkShootingGalleryOctoroks() {
    COND_ID_HOOK(OnActorDraw, ACTOR_EN_SYATEKI_OKUTA, CVAR, [](Actor* actor) {
        EnSyatekiOkuta* enSyatekiOkuta = (EnSyatekiOkuta*)actor;

        if (enSyatekiOkuta->actionFunc != EnSyatekiOkuta_Die) {
            OPEN_DISPS(gPlayState->state.gfxCtx);

            Gfx_SetupDL25_Xlu(gPlayState->state.gfxCtx);
            Matrix_Translate(enSyatekiOkuta->actor.world.pos.x, enSyatekiOkuta->actor.world.pos.y + 30.0f,
                             enSyatekiOkuta->actor.world.pos.z + 20.0f, MTXMODE_NEW);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 192);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gPlayState->state.gfxCtx);

            if (enSyatekiOkuta->type == SG_OCTO_TYPE_BLUE) {
                gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gShootingGalleryOctorokCrossDL);
            } else {
                gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gShootingGalleryOctorokCircleDL);
            }

            CLOSE_DISPS(gPlayState->state.gfxCtx);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterMarkShootingGalleryOctoroks, { CVAR_NAME });
