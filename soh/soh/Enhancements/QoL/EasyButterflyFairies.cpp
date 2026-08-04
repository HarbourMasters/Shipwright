#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/randomizer/SeedContext.h"

extern "C" {
#include "src/overlays/actors/ovl_En_Butte/z_en_butte.h"
#include "variables.h"
extern void EnButte_SetupTransformIntoFairy(EnButte* enButte);
}

void EasyButterflyFairies_Register() {
    COND_VB_SHOULD(VB_SPAWN_BUTTERFLY_FAIRY_EASY,
                   CVarGetInteger(CVAR_ENHANCEMENT("EasyButterflyFairies"), 0) ||
                       (IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_BUTTERFLY_FAIRIES)),
                   {
                       EnButte* enButte = va_arg(args, EnButte*);
                       Player* player = GET_PLAYER(gPlayState);
                       if (player->heldItemAction == PLAYER_IA_DEKU_STICK && enButte->actor.xzDistToPlayer < 60.0f) {
                           EnButte_SetupTransformIntoFairy(enButte);
                           *should = false;
                       }
                   });
}

static RegisterShipInitFunc initFunc(EasyButterflyFairies_Register,
                                     { CVAR_ENHANCEMENT("EasyButterflyFairies"), "IS_RANDO" });
