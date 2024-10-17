#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include "z64save.h"
#include "functions.h"
#include "soh/Enhancements/randomizer/randomizer_entrance.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

void SkipIntro_Register() {
    REGISTER_VB_SHOULD(VB_PLAY_TRANSITION_CS, {
        // If we're playing rando and if starting age is adult and/or overworld spawns are shuffled we need to skip
        // the cutscene regardless of the enhancement being on.
        bool adultStart = gSaveContext.linkAge == LINK_AGE_ADULT;
        bool shuffleEntrances = OTRGlobals::Instance->gRandoContext->GetOption(RSK_SHUFFLE_ENTRANCES).Is(true);
        if ((CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Intro"), IS_RANDO) ||
             (IS_RANDO && (adultStart || shuffleEntrances))) &&
            gSaveContext.cutsceneIndex == 0xFFF1) {
            // Calculate spawn location. Start with vanilla, Link's house.
            int32_t spawnEntrance = ENTR_LINKS_HOUSE_0;
            // If we're not in rando, we can skip all of the below.
            if (IS_RANDO) {
                // If starting age is shuffled, use vanilla adult spawn/prelude warp.
                if (adultStart) {
                    spawnEntrance = ENTR_TEMPLE_OF_TIME_7;
                }
                // If we're shuffling any entrances we'll need to get the Entrance Override
                if (shuffleEntrances) {
                    // If we're shuffling any entrances, the adult spawn is ENTR_HYRULE_FIELD_10 instead of
                    // ENTR_TEMPLE_OF_TIME_7, so that spawn and Prelude don't share an entrance.
                    if (adultStart) {
                        spawnEntrance = ENTR_HYRULE_FIELD_10;
                    }
                    spawnEntrance = Entrance_PeekNextIndexOverride(spawnEntrance);
                }
            }
            // Skip the intro cutscene for whatever the spawnEntrance is calculated to be.
            if (gSaveContext.entranceIndex == spawnEntrance) {
                gSaveContext.cutsceneIndex = 0;
                *should = false;
            }
        }
    });
}
