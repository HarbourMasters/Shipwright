#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

#define CVAR_ADULT_CRAWLSPACES_NAME CVAR_CHEAT("AdultCrawlspaces")
#define CVAR_ADULT_CRAWLSPACES_VALUE CVarGetInteger(CVAR_ADULT_CRAWLSPACES_NAME, 0)

void RegisterAdultCrawlspaces() {
    COND_VB_SHOULD(VB_LINK_BE_ABLE_TO_ENTER_CRAWLSPACE, CVAR_ADULT_CRAWLSPACES_VALUE, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterAdultCrawlspaces, { CVAR_ADULT_CRAWLSPACES_NAME });