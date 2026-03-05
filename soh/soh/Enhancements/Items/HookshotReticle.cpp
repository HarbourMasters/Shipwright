#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
#include "macros.h"
#include "functions.h"
#include "objects/object_link_boy/object_link_boy.h"
}

const Color_RGBA8 defaultTargetableColor = { .r = 0, .g = 255, .b = 0, .a = 255 };
const Color_RGBA8 defaultNonTargetableColor = { .r = 255, .g = 0, .b = 0, .a = 255 };

#define CVAR_TARGETABLE_HOOKSHOT_NAME CVAR_ENHANCEMENT("HookshotableReticle")
#define CVAR_TARGETABLE_HOOKSHOT_DEFAULT 0
#define CVAR_TARGETABLE_HOOKSHOT_VALUE CVarGetInteger(CVAR_TARGETABLE_HOOKSHOT_NAME, CVAR_TARGETABLE_HOOKSHOT_DEFAULT)

#define CVAR_NON_TARGETABLE_HOOKSHOT_NAME CVAR_COSMETIC("HookshotReticle.NonTarget.Value")
#define CVAR_NON_TARGETABLE_HOOKSHOT_DEFAULT defaultNonTargetableColor
#define CVAR_NON_TARGETABLE_HOOKSHOT_VALUE \
    CVarGetColor(CVAR_NON_TARGETABLE_HOOKSHOT_NAME, CVAR_NON_TARGETABLE_HOOKSHOT_DEFAULT)

void RegisterTargetableHookshotReticle() {
    Color_RGBA8 nonTargetColor = CVAR_NON_TARGETABLE_HOOKSHOT_VALUE;
    bool nonTargetColorIsNotDefault =
        nonTargetColor.r != defaultNonTargetableColor.r || nonTargetColor.g != defaultNonTargetableColor.g ||
        nonTargetColor.b != defaultNonTargetableColor.b || nonTargetColor.a != defaultNonTargetableColor.a;
    bool shouldRegister = CVAR_TARGETABLE_HOOKSHOT_VALUE || nonTargetColorIsNotDefault;

    COND_VB_SHOULD(VB_TARGETABLE_HOOKSHOT_RETICLE, shouldRegister, {
        Player* player = GET_PLAYER(gPlayState);
        Actor* heldActor = player->heldActor;
        CollisionPoly* colPoly = va_arg(args, CollisionPoly*);
        s32 bgId = va_arg(args, s32);
        GraphicsContext* __gfxCtx = gPlayState->state.gfxCtx;

        const Color_RGBA8 colorTargetable =
            CVarGetColor(CVAR_COSMETIC("HookshotReticle.Target.Value"), defaultTargetableColor);
        const Color_RGBA8 colorNonTargetable =
            CVarGetColor(CVAR_COSMETIC("HookshotReticle.NonTarget.Value"), defaultNonTargetableColor);

        gSPMatrix(OVERLAY_DISP++, SEG_ADDR(1, 0), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        gSPTexture(OVERLAY_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        gDPLoadTextureBlock(OVERLAY_DISP++, gLinkAdultHookshotReticleTex, G_IM_FMT_I, G_IM_SIZ_8b, 64, 64, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
        if (CVAR_TARGETABLE_HOOKSHOT_VALUE && SurfaceType_IsHookshotSurface(&gPlayState->colCtx, colPoly, bgId) &&
            (player->heldItemAction == PLAYER_IA_HOOKSHOT || player->heldItemAction == PLAYER_IA_LONGSHOT)) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, colorTargetable.r, colorTargetable.g, colorTargetable.b,
                            colorTargetable.a);
        } else {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, colorNonTargetable.r, colorNonTargetable.g, colorNonTargetable.b,
                            colorNonTargetable.a);
        }
        gSPVertex(OVERLAY_DISP++, (uintptr_t)gLinkAdultHookshotReticleVtx, 3, 0);
        gSP1Triangle(OVERLAY_DISP++, 0, 1, 2, 0);
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterTargetableHookshotReticle,
                                     { CVAR_TARGETABLE_HOOKSHOT_NAME, CVAR_NON_TARGETABLE_HOOKSHOT_NAME });