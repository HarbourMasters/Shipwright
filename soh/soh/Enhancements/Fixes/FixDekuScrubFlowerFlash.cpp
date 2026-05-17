#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Dnt_Nomal/z_en_dnt_nomal.h"
}

#define CVAR_DEKU_SCRUB_FLOWER_FLASH_FIX_NAME CVAR_ENHANCEMENT("FixDekuScrubFlowerFlash")
#define CVAR_DEKU_SCRUB_FLOWER_FLASH_FIX_VALUE CVarGetInteger(CVAR_DEKU_SCRUB_FLOWER_FLASH_FIX_NAME, 0)

// https://github.com/HarbourMasters/Shipwright/issues/2796
//
// En_Dnt_Nomal enables its draw function in WaitForObject but doesn't set flowerPos until SetFlower (which waits for
// ground contact).  Both draw functions use flowerPos with MTXMODE_NEW, so the flower renders at the zero-initialized
// origin for the intervening frames.
//
// Defaulting flowerPos to the actor's spawn position closes the gap.

static void FixDekuScrubFlowerFlash(void* actorRef) {
    const auto enDntNomal = static_cast<EnDntNomal*>(actorRef);
    enDntNomal->flowerPos = enDntNomal->actor.world.pos;
}

static void RegisterFixDekuScrubFlowerFlash() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_DNT_NOMAL, CVAR_DEKU_SCRUB_FLOWER_FLASH_FIX_VALUE, FixDekuScrubFlowerFlash);
}

static RegisterShipInitFunc initFunc(RegisterFixDekuScrubFlowerFlash, { CVAR_DEKU_SCRUB_FLOWER_FLASH_FIX_NAME });
