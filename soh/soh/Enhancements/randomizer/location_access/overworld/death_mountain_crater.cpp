#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

/* Notes for calculating timers:
    - RT_DMC_BOLERO_JUMP takes about 16 seconds to perform.
    - RT_DMC_BOULDER_SKIP assumes a setup that takes 14 seconds to fully execute
    - Spawning and killing the bean skull takes around 10 seconds, as does getting the bean sprout,
      as does getting the rupees on the near plank platform but as it's child only does not stack with
   RT_DMC_BOLERO_JUMP or RT_DMC_BOULDER_SKIP
    - Whatever number of hearts is used for Fewer Tunic Requirements, the default time check is that +50%, rounded up.
   This applies to both adult and child
    - All child only checks must be possible with 3 hearts, or minimal item pool breaks.
    - I have Pot Grotto separate but not Rocks Grotto because it needs a different amount of hearts to reach blocked
   exit, which will matter with mods RANDOTODO: when randomised entrances can be in more than 1 region, make it so the
   player can always turn around and go back the way they came
*/
void RegionTable_Init_DeathMountainCrater() {
    // clang-format off
    areaTable[RR_DMC_UPPER_ENTRY] = Region("DMC Upper Entry", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH,    (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_VOLCANO_FREESTANDING_POH, ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS)) ||
                                                  (logic->IsAdult && (logic->FireTimer() >= 56 || logic->Hearts() >= 11) && logic->DMCPotsToPad() && logic->DMCUpperToPots() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
    }, {
        //Exits
        ENTRANCE(RR_DMC_CRATE,           true),
        ENTRANCE(RR_DMC_ROCK_GROTTO,     logic->FireTimer() >= 8 || logic->Hearts() >= 2),
        ENTRANCE(RR_DMC_CRACKED_WALL,    (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_SCRUB,           logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_BLOCKED_EXIT,    ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && logic->DMCUpperToPots()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 56 || logic->Hearts() >= 11) && logic->ReachDistantScarecrow() && logic->TakeDamage())),
        ENTRANCE(RR_DMC_POTS,            ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->DMCUpperToPots()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 48  || logic->Hearts() >= 9) && logic->ReachDistantScarecrow() && logic->TakeDamage())),
        ENTRANCE(RR_DMC_POT_GROTTO_EXIT, ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->DMCUpperToPots()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 48  || logic->Hearts() >= 9) && logic->ReachDistantScarecrow() && logic->TakeDamage())),
        ENTRANCE(RR_DMC_CENTRAL,         ((logic->FireTimer() >= 64 || logic->Hearts() >= 12) && logic->DMCUpperToPots() && logic->DMCPotsToPad()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->ReachDistantScarecrow() && logic->TakeDamage())),
        ENTRANCE(RR_DMC_FAR_PLATFORM,    (logic->IsAdult && (logic->FireTimer() >= 72 || logic->Hearts() >= 14) && logic->DMCUpperToPots() && logic->DMCPotsToPad() && logic->ReachDistantScarecrow()) ||
                                         (logic->FireTimer() >= 24 || logic->Hearts() >= 5) && logic->TakeDamage()),
        ENTRANCE(RR_DMC_TEMPLE_EXIT,     ((logic->FireTimer() >= 72 || logic->Hearts() >= 14) && logic->DMCUpperToPots() && logic->DMCPotsToPad()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->ReachDistantScarecrow() && logic->TakeDamage())),
    });

    areaTable[RR_DMC_ROCKS_GROTTO_ENTRY] = Region("DMC Rocks Grotto Entry", SCENE_DEATH_MOUNTAIN_CRATER, {
    }, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH,    logic->FireTimer() >= 8 || logic->Hearts() >= 2),
        LOCATION(RC_DMC_VOLCANO_FREESTANDING_POH, ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS)) ||
                                                  (logic->IsAdult && (logic->FireTimer() >= 64 || logic->Hearts() >= 12) && logic->DMCPotsToPad() && logic->DMCUpperToPots() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
    }, {
        //Exits
        ENTRANCE(RR_DMC_CRATE,           logic->FireTimer() >= 8 || logic->Hearts() >= 2),
        ENTRANCE(RR_DMC_ROCK_GROTTO,     true),
        ENTRANCE(RR_DMC_CRACKED_WALL,    (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_SCRUB,           logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_BLOCKED_EXIT,    ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->DMCUpperToPots()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->ReachDistantScarecrow() && logic->TakeDamage())),
        ENTRANCE(RR_DMC_POTS,            ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->DMCUpperToPots()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->ReachDistantScarecrow() && logic->TakeDamage())),
        ENTRANCE(RR_DMC_POT_GROTTO_EXIT, ((logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->DMCUpperToPots()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->ReachDistantScarecrow() && logic->TakeDamage())),
        ENTRANCE(RR_DMC_CENTRAL,         ((logic->FireTimer() >= 64 || logic->Hearts() >= 12) && logic->DMCUpperToPots() && logic->DMCPotsToPad()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 3) && logic->ReachDistantScarecrow() && logic->TakeDamage())),
        ENTRANCE(RR_DMC_FAR_PLATFORM,    (logic->IsAdult && (logic->FireTimer() >= 72 || logic->Hearts() >= 14) && logic->DMCUpperToPots() && logic->DMCPotsToPad() && logic->ReachDistantScarecrow()) ||
                                         (logic->FireTimer() >= 16 || logic->Hearts() >= 3) && logic->TakeDamage()),
        ENTRANCE(RR_DMC_TEMPLE_EXIT,     ((logic->FireTimer() >= 64 || logic->Hearts() >= 12) && logic->DMCUpperToPots() && logic->DMCPotsToPad()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 3) && logic->ReachDistantScarecrow() && logic->TakeDamage())),
    });

    areaTable[RR_DMC_BLOCKED_ENTRY] = Region("DMC Blocked Entry", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH,    ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && logic->CanClimbLadder()) || 
                                                  (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                                  ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT))),
        LOCATION(RC_DMC_VOLCANO_FREESTANDING_POH, ((logic->FireTimer() >= 16 || logic->Hearts() >= 3) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS)) || 
                                                  (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
    }, {
        //Exits
        ENTRANCE(RR_DMC_CRATE,           ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && logic->CanClimbLadder()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 56 || logic->Hearts() >= 11) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                         ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_DMC_ROCK_GROTTO,     ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && logic->CanClimbLadder()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                         ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_DMC_CRACKED_WALL,    (logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->CanClimbLadder() || 
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                         ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_DMC_SCRUB,           logic->FireTimer() >= 8 || logic->Hearts() >= 2),
        ENTRANCE(RR_DMC_BLOCKED_EXIT,    true),
        ENTRANCE(RR_DMC_POTS,            logic->FireTimer() >= 8 || logic->Hearts() >= 2),
        ENTRANCE(RR_DMC_POT_GROTTO_EXIT, logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_CENTRAL,         ((logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->DMCPotsToPad()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->ReachDistantScarecrow() && logic->TakeDamage() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)))),
        ENTRANCE(RR_DMC_FAR_PLATFORM,    (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->DMCPotsToPad() && logic->ReachDistantScarecrow()) ||
                                         ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->TakeDamage() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))) || 
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                         ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->TakeDamage() && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS))),
        ENTRANCE(RR_DMC_TEMPLE_EXIT,     ((logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->DMCPotsToPad()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 56 || logic->Hearts() >= 11) && logic->TakeDamage() && logic->ReachDistantScarecrow() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)))),
    });

    areaTable[RR_DMC_POTS_ENTRY] = Region("DMC Pots Entry", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH,    (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->CanClimbLadder() || 
                                                  (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                                  ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT))),
        LOCATION(RC_DMC_VOLCANO_FREESTANDING_POH, ((logic->FireTimer() >= 8 || logic->Hearts() >= 2) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS)) || 
                                                  (logic->IsAdult && (logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
    }, {
        //Exits
        ENTRANCE(RR_DMC_CRATE,           (logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->CanClimbLadder() ||
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                         ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_DMC_ROCK_GROTTO,     (logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->CanClimbLadder() ||
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                         ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_DMC_CRACKED_WALL,    (logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->CanClimbLadder() ||
                                         (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                         ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_DMC_SCRUB,           logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_BLOCKED_EXIT,    logic->FireTimer() >= 8 || logic->Hearts() >= 2),
        ENTRANCE(RR_DMC_POTS,            true),
        ENTRANCE(RR_DMC_POT_GROTTO_EXIT, logic->FireTimer() >= 8 || logic->Hearts() >= 2),
        ENTRANCE(RR_DMC_CENTRAL,         ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->DMCPotsToPad()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 56 || logic->Hearts() >= 11) && logic->TakeDamage() && logic->ReachDistantScarecrow() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)))),
        ENTRANCE(RR_DMC_FAR_PLATFORM,    (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->DMCPotsToPad() && logic->ReachDistantScarecrow()) ||
                                         ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->TakeDamage() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)))),
        ENTRANCE(RR_DMC_TEMPLE_EXIT,     ((logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->DMCPotsToPad()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 56 || logic->Hearts() >= 11) && logic->TakeDamage() && logic->ReachDistantScarecrow() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)))),
    });

    areaTable[RR_DMC_POT_GROTTO_ENTRY] = Region("DMC Pot Grotto Entry", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH,    (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->CanClimbLadder() || 
                                                  (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                                  ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT))),
        LOCATION(RC_DMC_VOLCANO_FREESTANDING_POH, (logic->FireTimer() >= 16 || logic->Hearts() >= 3) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) || 
                                                  (logic->IsAdult && (logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
    }, {
        //Exits
        ENTRANCE(RR_DMC_CRATE,           (logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->CanClimbLadder() ||
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                         ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_DMC_ROCK_GROTTO,     (logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->CanClimbLadder() ||
                                         (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                         ((logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_DMC_CRACKED_WALL,    (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->CanClimbLadder() ||
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->DMCPotsToPad() && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) ||
                                         ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_DMC_SCRUB,           logic->FireTimer() >= 24 || logic->Hearts() >= 5),
        ENTRANCE(RR_DMC_BLOCKED_EXIT,    logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_POTS,            logic->FireTimer() >= 8 || logic->Hearts() >= 2),
        ENTRANCE(RR_DMC_POT_GROTTO_EXIT, true),
        ENTRANCE(RR_DMC_CENTRAL,         ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && logic->DMCPotsToPad()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 56 || logic->Hearts() >= 11) && logic->TakeDamage() && logic->ReachDistantScarecrow() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)))),
        ENTRANCE(RR_DMC_FAR_PLATFORM,    (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->DMCPotsToPad() && logic->ReachDistantScarecrow()) ||
                                         ((logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->TakeDamage() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)))),
        ENTRANCE(RR_DMC_TEMPLE_EXIT,     ((logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->DMCPotsToPad()) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 56 || logic->Hearts() >= 11) && logic->TakeDamage() && logic->ReachDistantScarecrow() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)))),
    });

    areaTable[RR_DMC_PAD_ENTRY] = Region("DMC Pad Entry", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH,    ((logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->CanClimbLadder() && logic->DMCPadToPots()) ||
                                                  ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT)) ||
                                                  (logic->IsAdult && (logic->FireTimer() >= 16 || logic->Hearts() >= 3) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
        LOCATION(RC_DMC_VOLCANO_FREESTANDING_POH, (logic->FireTimer() >= 24 || logic->Hearts() >= 5) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->DMCPadToPots() || 
                                                  (logic->IsAdult && (logic->FireTimer() >= 8 || logic->Hearts() >= 2) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
    }, {
        //Exits
        ENTRANCE(RR_DMC_CRATE,           ((logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->CanClimbLadder() && logic->DMCPadToPots()) ||
                                         ((logic->FireTimer() >= 40 || logic->Hearts() >= 8) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT)) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 24 || logic->Hearts() >= 5) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
        ENTRANCE(RR_DMC_ROCK_GROTTO,     ((logic->FireTimer() >= 40 || logic->Hearts() >= 8) && logic->CanClimbLadder() && logic->DMCPadToPots()) ||
                                         ((logic->FireTimer() >= 32 || logic->Hearts() >= 6) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT)) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 16 || logic->Hearts() >= 3) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
        ENTRANCE(RR_DMC_CRACKED_WALL,    (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->CanClimbLadder() && logic->DMCPadToPots() ||
                                         ((logic->FireTimer() >= 40 || logic->Hearts() >= 8) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT)) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 24 || logic->Hearts() >= 5) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
        ENTRANCE(RR_DMC_SCRUB,           (logic->FireTimer() >= 24 || logic->Hearts() >= 5) && logic->DMCPadToPots() || 
                                         (logic->IsAdult && (logic->FireTimer() >= 16 || logic->Hearts() >= 3) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
        ENTRANCE(RR_DMC_BLOCKED_EXIT,    (logic->FireTimer() >= 24 || logic->Hearts() >= 5) && logic->DMCPadToPots() || 
                                         (logic->IsAdult && (logic->FireTimer() >= 16 || logic->Hearts() >= 3) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
        ENTRANCE(RR_DMC_POTS,            (logic->FireTimer() >= 16 || logic->Hearts() >= 3) && logic->DMCPadToPots() || 
                                         (logic->IsAdult && (logic->FireTimer() >= 16 || logic->Hearts() >= 3) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
        ENTRANCE(RR_DMC_POT_GROTTO_EXIT, (logic->FireTimer() >= 16 || logic->Hearts() >= 3) && logic->DMCPadToPots() || 
                                         ((logic->IsAdult && logic->FireTimer() >= 24 || logic->Hearts() >= 5) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))),
        ENTRANCE(RR_DMC_CENTRAL,         (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_FAR_PLATFORM,    ((logic->FireTimer() >= 56 || logic->Hearts() >= 11) && logic->TakeDamage() && logic->CanClimbLadder() && logic->DMCPadToPots()) ||
                                         ((logic->FireTimer() >= 40 || logic->Hearts() >= 8) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT)) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 24 || logic->Hearts() >= 5) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))||
                                         (logic->IsAdult && (logic->FireTimer() >= 16 || logic->Hearts() >= 3) && logic->ReachDistantScarecrow())),
        ENTRANCE(RR_DMC_TEMPLE_EXIT,     (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
    });

    areaTable[RR_DMC_TEMPLE_ENTRY] = Region("DMC Temple Entry", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH,    logic->HasItem(RG_CLIMB) &&
                                                  (((logic->FireTimer() >= 72 || logic->Hearts() >= 14) && logic->DMCPadToPots()) ||
                                                   ((logic->FireTimer() >= 64 || logic->Hearts() >= 12) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT)) ||
                                                   (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)))),
        LOCATION(RC_DMC_VOLCANO_FREESTANDING_POH, logic->HasItem(RG_CLIMB) &&
                                                  ((logic->FireTimer() >= 56 || logic->Hearts() >= 11) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->DMCPadToPots() || 
                                                   (logic->IsAdult && (logic->FireTimer() >= 40 || logic->Hearts() >= 8) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)))),
    }, {
        //Exits
        ENTRANCE(RR_DMC_CRATE,          logic->HasItem(RG_CLIMB) &&
                                        (((logic->FireTimer() >= 72 || logic->Hearts() >= 14) && logic->DMCPadToPots()) ||
                                         ((logic->FireTimer() >= 72 || logic->Hearts() >= 14) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT)) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 56 || logic->Hearts() >= 11) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)))),
        ENTRANCE(RR_DMC_ROCK_GROTTO,    logic->HasItem(RG_CLIMB) &&
                                        (((logic->FireTimer() >= 72 || logic->Hearts() >= 14) && logic->DMCPadToPots()) ||
                                         ((logic->FireTimer() >= 64 || logic->Hearts() >= 12) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT)) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)))),
        ENTRANCE(RR_DMC_CRACKED_WALL,   logic->HasItem(RG_CLIMB) &&
                                        (((logic->FireTimer() >= 80 || logic->Hearts() >= 15) && logic->DMCPadToPots()) ||
                                         ((logic->FireTimer() >= 72 || logic->Hearts() >= 14) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT)) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 56 || logic->Hearts() >= 11) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)))),
        ENTRANCE(RR_DMC_SCRUB,          logic->HasItem(RG_CLIMB) &&
                                        (((logic->FireTimer() >= 56 || logic->Hearts() >= 11) && logic->DMCPadToPots()) || 
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)))),
        ENTRANCE(RR_DMC_BLOCKED_EXIT,   logic->HasItem(RG_CLIMB) &&
                                        (((logic->FireTimer() >= 56 || logic->Hearts() >= 11) && logic->DMCPadToPots()) || 
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)))),
        ENTRANCE(RR_DMC_POTS,           logic->HasItem(RG_CLIMB) &&
                                        (((logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->DMCPadToPots()) || 
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)))),
        ENTRANCE(RR_DMC_POT_GROTTO_EXIT, logic->HasItem(RG_CLIMB) &&
                                         (((logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->DMCPadToPots()) || 
                                         (logic->IsAdult && (logic->FireTimer() >= 56 || logic->Hearts() >= 11) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)))),
        ENTRANCE(RR_DMC_CENTRAL,        logic->HasItem(RG_CLIMB) && (logic->FireTimer() >= 48 || logic->Hearts() >= 9)),
        ENTRANCE(RR_DMC_FAR_PLATFORM,   logic->HasItem(RG_CLIMB) &&
                                        (((logic->FireTimer() >= 88 || logic->Hearts() >= 3) && logic->TakeDamage() && logic->CanClimbLadder() && logic->DMCPadToPots()) ||
                                         ((logic->FireTimer() >= 72 || logic->Hearts() >= 14) && ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT)) ||
                                         (logic->IsAdult && (logic->FireTimer() >= 56 || logic->Hearts() >= 11) && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL))||
                                         (logic->IsAdult && (logic->FireTimer() >= 48 || logic->Hearts() >= 9) && logic->ReachDistantScarecrow()))),
        ENTRANCE(RR_DMC_TEMPLE_EXIT,    true),
    });   

    areaTable[RR_DMC_CRATE] = Region("DMC Crate", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations
        LOCATION(RC_DMC_GS_CRATE, logic->IsChild && logic->CanKillEnemy(RE_GOLD_SKULLTULA) && logic->CanBreakCrates()),
        LOCATION(RC_DMC_CRATE,    logic->IsChild && logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DEATH_MOUNTAIN_SUMMIT, true),
    });

    areaTable[RR_DMC_ROCK_GROTTO] = Region("DMC Rock Grotto", SCENE_DEATH_MOUNTAIN_CRATER, {}, {}, {
        //Exits
        ENTRANCE(RR_DMC_UPPER_GROTTO, AnyAgeTime([]{return logic->BlastOrSmash();})),
    });

    areaTable[RR_DMC_CRACKED_WALL] = Region("DMC Cracked Wall", SCENE_DEATH_MOUNTAIN_CRATER, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->HasExplosives() && logic->CallGossipFairyExceptSuns()),
    }, {
        //Locations
        LOCATION(RC_DMC_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns() && logic->HasExplosives()),
        LOCATION(RC_DMC_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS) && logic->HasExplosives()),
        LOCATION(RC_DMC_GOSSIP_STONE,           true && logic->HasExplosives()),
    }, {});

    areaTable[RR_DMC_SCRUB] = Region("DMC Scrub", SCENE_DEATH_MOUNTAIN_CRATER, {
        //Events
        EVENT_ACCESS(LOGIC_DMC_BOULDER, logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER) && ctx->GetTrickOption(RT_DMC_BOULDER_JS)),
    }, {
        //Locations
        LOCATION(RC_DMC_DEKU_SCRUB, logic->IsChild && logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
    }, {});

    areaTable[RR_DMC_BLOCKED_EXIT] = Region("DMC Blocked Exit", SCENE_DEATH_MOUNTAIN_CRATER, {
        //Events
        //All boulders here are only a perm flag for adult.
        EVENT_ACCESS(LOGIC_DMC_BOULDER, logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER)),
    }, {}, {
        //Exits
        ENTRANCE(RR_DMC_GREAT_FAIRY_FOUNTAIN, logic->CanUse(RG_MEGATON_HAMMER)),
    });

    areaTable[RR_DMC_POTS] = Region("DMC Pots", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        // Locations
        LOCATION(RC_DMC_NEAR_GC_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DMC_NEAR_GC_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DMC_NEAR_GC_POT_3, logic->CanBreakPots()),
        LOCATION(RC_DMC_NEAR_GC_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GC_DARUNIAS_CHAMBER, true),
    });
    
    areaTable[RR_DMC_POT_GROTTO_EXIT] = Region("DMC Pot Grotto Exit", SCENE_DEATH_MOUNTAIN_CRATER, {}, {}, {
        //Exits
        //Perm flag only as Adult
        ENTRANCE(RR_DMC_SCRUB_GROTTO, logic->CanUse(RG_MEGATON_HAMMER)),
    });

    areaTable[RR_DMC_CENTRAL] = Region("DMC Central", SCENE_DEATH_MOUNTAIN_CRATER, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
    }, {
        //Locations
        LOCATION(RC_SHEIK_IN_CRATER,                  logic->IsAdult),
        LOCATION(RC_DMC_GS_BEAN_PATCH,                logic->CanSpawnSoilSkull(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) && logic->CanKillEnemy(RE_GOLD_SKULLTULA)),
        LOCATION(RC_DMC_NEAR_PLATFORM_RED_RUPEE,      logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_RED_RUPEE,    logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_1, logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_2, logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_3, logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_4, logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_5, logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_6, logic->IsChild),
        LOCATION(RC_DMC_BEAN_SPROUT_FAIRY_1,          logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMC_BEAN_SPROUT_FAIRY_2,          logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMC_BEAN_SPROUT_FAIRY_3,          logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
    }, {});


    areaTable[RR_DMC_FAR_PLATFORM] = Region("DMC Far Platform", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_1, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_2, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_3, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_4, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_5, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_6, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_RED_RUPEE,     logic->IsAdult),
    }, {});

    areaTable[RR_DMC_TEMPLE_EXIT] = Region("DMC Temple Exit", SCENE_DEATH_MOUNTAIN_CRATER, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_ENTRYWAY, true),
    });

    areaTable[RR_DMC_GREAT_FAIRY_FOUNTAIN] = Region("DMC Great Fairy Fountain", SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, {}, {
        //Locations
        LOCATION(RC_DMC_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        ENTRANCE(RR_DMC_BLOCKED_ENTRY, true),
    });

    areaTable[RR_DMC_UPPER_GROTTO] = Region("DMC Upper Grotto", SCENE_GROTTOS, grottoEvents, {
        //Locations
        LOCATION(RC_DMC_UPPER_GROTTO_CHEST,                  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DMC_UPPER_GROTTO_FISH,                   logic->HasBottle()),
        LOCATION(RC_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMC_UPPER_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_DMC_UPPER_GROTTO_BEEHIVE_LEFT,           logic->CanBreakLowerBeehives()),
        LOCATION(RC_DMC_UPPER_GROTTO_BEEHIVE_RIGHT,          logic->CanBreakLowerBeehives()),
        LOCATION(RC_DMC_UPPER_GROTTO_GRASS_1,                logic->CanCutShrubs()),
        LOCATION(RC_DMC_UPPER_GROTTO_GRASS_2,                logic->CanCutShrubs()),
        LOCATION(RC_DMC_UPPER_GROTTO_GRASS_3,                logic->CanCutShrubs()),
        LOCATION(RC_DMC_UPPER_GROTTO_GRASS_4,                logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DMC_ROCKS_GROTTO_ENTRY, true),
    });

    areaTable[RR_DMC_SCRUB_GROTTO] = Region("DMC Scrub Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DMC_HAMMER_GROTTO_BEEHIVE,    logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        ENTRANCE(RR_DMC_POT_GROTTO_ENTRY, true),
    });

    // clang-format on
}
