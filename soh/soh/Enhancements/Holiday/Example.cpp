#include "Holiday.hpp"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;

// TODO: Include anything you need here from C land
}

// TODO: Change this to YourName
#define AUTHOR "Example"
#define CVAR(v) "gHoliday." AUTHOR "." v

static void OnConfigurationChanged() {
    // TODO: Register any hooks or things that need to run on startup and when the main CVar is toggled
    // Note: Hooks should be registered/unregistered depending on the CVar state (Use COND_HOOK or COND_ID_HOOK)

    // COND_HOOK(OnSceneSpawnActors, CVarGetInteger(CVAR("Enabled"), 0), []() {
    //     // Spawn your own actors?
    // });
    
    // COND_ID_HOOK(OnActorInit, ACTOR_OBJ_TSUBO, CVarGetInteger(CVAR("DoSomethingWithPots"), 0), [](void* actorRef) {
    //     // Do something with pots?
    // });
}

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);
    if (UIWidgets::EnhancementCheckbox("DoSomethingWithPots", CVAR("DoSomethingWithPots"))) {
        OnConfigurationChanged();
    }
}

static void RegisterMod() {
    // #region Leave this alone unless you know what you are doing
    OnConfigurationChanged();
    // #endregion

    // TODO: Anything you want to run once on startup
}

// TODO: Uncomment this line to enable the mod
// static Holiday holiday(DrawMenu, RegisterMod);
