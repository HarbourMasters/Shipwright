#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "align_asset_macro.h"
extern PlayState* gPlayState;
}

#define CVAR_NAME CVAR_SILLY_FLAT_SHADING
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static const ALIGN_ASSET(2) char sFlatShader[] = "__OTR__shaders/flatavg";

void RegisterFlatShading() {
    COND_HOOK(OnPlayDrawWorldStart, CVAR, []() {
        OPEN_DISPS(gPlayState->state.gfxCtx);
        gSPPushShader(POLY_OPA_DISP++, sFlatShader);
        gSPPushShader(POLY_XLU_DISP++, sFlatShader);
        CLOSE_DISPS(gPlayState->state.gfxCtx);
    });

    COND_HOOK(OnPlayDrawWorldEnd, CVAR, []() {
        OPEN_DISPS(gPlayState->state.gfxCtx);
        gSPPopShader(POLY_OPA_DISP++);
        gSPPopShader(POLY_XLU_DISP++);
        CLOSE_DISPS(gPlayState->state.gfxCtx);
    });
}

static RegisterShipInitFunc initFunc(RegisterFlatShading, { CVAR_NAME });
