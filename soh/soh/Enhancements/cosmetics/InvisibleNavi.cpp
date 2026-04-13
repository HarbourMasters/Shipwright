#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Elf/z_en_elf.h"
}

static constexpr int32_t CVAR_INVISIBLENAVI_DEFAULT = 0;
#define CVAR_INVISIBLENAVI_NAME CVAR_COSMETIC("InvisibleNavi")
#define CVAR_INVISIBLENAVI_VALUE CVarGetInteger(CVAR_INVISIBLENAVI_NAME, CVAR_INVISIBLENAVI_DEFAULT)

void RegisterInvisibleNavi() {
    COND_VB_SHOULD(VB_FAIRY_DRAW, CVAR_INVISIBLENAVI_VALUE, {
        EnElf* enElf = va_arg(args, EnElf*);
        if (enElf->actor.params == FAIRY_NAVI) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_FAIRY_SPAWN_SPARKLES, CVAR_INVISIBLENAVI_VALUE, {
        EnElf* enElf = va_arg(args, EnElf*);
        if (enElf->actor.params == FAIRY_NAVI) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_FAIRY_PLAY_C_UP_TALK_SOUND, CVAR_INVISIBLENAVI_VALUE, {
        EnElf* enElf = va_arg(args, EnElf*);
        if (enElf->actor.params == FAIRY_NAVI) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_FAIRY_PLAY_DASH_SOUND, CVAR_INVISIBLENAVI_VALUE, {
        EnElf* enElf = va_arg(args, EnElf*);
        if (enElf->actor.params == FAIRY_NAVI) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_FAIRY_PLAY_VANISH_SOUND, CVAR_INVISIBLENAVI_VALUE, {
        EnElf* enElf = va_arg(args, EnElf*);
        if (enElf->actor.params == FAIRY_NAVI) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_FAIRY_UPDATE_LIGHTS, CVAR_INVISIBLENAVI_VALUE, {
        EnElf* enElf = va_arg(args, EnElf*);
        if (enElf->actor.params == FAIRY_NAVI) {
            // Force Navi's light radius to zero.
            Lights_PointGlowSetInfo(&enElf->lightInfoGlow, enElf->actor.world.pos.x, enElf->actor.world.pos.y,
                                    enElf->actor.world.pos.z, 255, 255, 255, 0);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_PLAY_NAVI_CALL_SOUND, CVAR_INVISIBLENAVI_VALUE, { *should = false; });

    COND_VB_SHOULD(VB_PLAY_INTRO_NAVI_SOUNDS, CVAR_INVISIBLENAVI_VALUE, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterInvisibleNavi, { CVAR_INVISIBLENAVI_NAME });
