#include "menu.hpp"
#include "../static_data.h"
#include "../item_location.h"
#include "../location_access.h"
#include "rando_main.hpp"
#include "../context.h"
#include <libultraship/bridge.h>
#include <ship/Context.h>
#include <libultraship/libultra/types.h>
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"

void RandoMain::GenerateRando(std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks,
                              std::string seedString) {

    Rando::Context::GetInstance()->SetSeedGenerated(GenerateRandomizer(excludedLocations, enabledTricks, seedString));

    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}
