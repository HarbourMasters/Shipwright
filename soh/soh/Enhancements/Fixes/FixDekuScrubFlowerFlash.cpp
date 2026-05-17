#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Dnt_Nomal/z_en_dnt_nomal.h"
}

#define CVAR_DEKU_SCRUB_FLOWER_FLASH_FIX_NAME CVAR_ENHANCEMENT("FixDekuScrubFlowerFlash")
#define CVAR_DEKU_SCRUB_FLOWER_FLASH_FIX_VALUE CVarGetInteger(CVAR_DEKU_SCRUB_FLOWER_FLASH_FIX_NAME, 0)

static void FixDekuScrubFlowerFlash(void* actorRef) {
    const auto enDntNomal = static_cast<EnDntNomal*>(actorRef);
    enDntNomal->flowerPos = enDntNomal->actor.world.pos;
}

static void RegisterFixDekuScrubFlowerFlash() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_DNT_NOMAL, CVAR_DEKU_SCRUB_FLOWER_FLASH_FIX_VALUE, FixDekuScrubFlowerFlash);
}

static RegisterShipInitFunc initFunc(RegisterFixDekuScrubFlowerFlash, { CVAR_DEKU_SCRUB_FLOWER_FLASH_FIX_NAME });
