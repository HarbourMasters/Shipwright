#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_Root() {
    // clang-format off
    areaTable[RR_ROOT] = Region("Root", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {
        //Events
        EventAccess(LOGIC_KAKARIKO_GATE_OPEN,                  []{return ctx->GetOption(RSK_KAK_GATE).Is(RO_KAK_GATE_OPEN);}),
        EventAccess(LOGIC_TH_COULD_FREE_1_TORCH_CARPENTER,     []{return ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE);}),
        EventAccess(LOGIC_TH_COULD_FREE_DOUBLE_CELL_CARPENTER, []{return ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) || ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FAST);}),
        EventAccess(LOGIC_TH_COULD_FREE_DEAD_END_CARPENTER,    []{return ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) || ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FAST);}),
        EventAccess(LOGIC_TH_COULD_FREE_SLOPE_CARPENTER,       []{return ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) || ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FAST);}),
        EventAccess(LOGIC_TH_RESCUED_ALL_CARPENTERS,           []{return ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE);}),
        EventAccess(LOGIC_FREED_EPONA,                         []{return (bool)ctx->GetOption(RSK_SKIP_EPONA_RACE);}),
    }, {
        //Locations
        LOCATION(RC_LINKS_POCKET,       true),
        LOCATION(RC_TRIFORCE_COMPLETED, logic->GetSaveContext()->ship.quest.data.randomizer.triforcePiecesCollected >= ctx->GetOption(RSK_TRIFORCE_HUNT_PIECES_REQUIRED).Get() + 1;),
        LOCATION(RC_SARIA_SONG_HINT,    logic->CanUse(RG_SARIAS_SONG)),
        LOCATION(RC_SONG_FROM_IMPA,     (bool)ctx->GetOption(RSK_SKIP_CHILD_ZELDA)),
        LOCATION(RC_TOT_MASTER_SWORD,   (bool)ctx->GetOption(RSK_SELECTED_STARTING_AGE).Is(RO_AGE_ADULT)),
    }, {
        //Exits
        Entrance(RR_ROOT_EXITS, []{return true;}),
    });

    areaTable[RR_ROOT_EXITS] = Region("Root Exits", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_CHILD_SPAWN,             []{return logic->IsChild;}),
        Entrance(RR_ADULT_SPAWN,             []{return logic->IsAdult;}),
        Entrance(RR_MINUET_OF_FOREST_WARP,   []{return logic->CanUse(RG_MINUET_OF_FOREST);}),
        Entrance(RR_BOLERO_OF_FIRE_WARP,     []{return logic->CanUse(RG_BOLERO_OF_FIRE);}),
        Entrance(RR_SERENADE_OF_WATER_WARP,  []{return logic->CanUse(RG_SERENADE_OF_WATER);}),
        Entrance(RR_NOCTURNE_OF_SHADOW_WARP, []{return logic->CanUse(RG_NOCTURNE_OF_SHADOW);}),
        Entrance(RR_REQUIEM_OF_SPIRIT_WARP,  []{return logic->CanUse(RG_REQUIEM_OF_SPIRIT);}),
        Entrance(RR_PRELUDE_OF_LIGHT_WARP,   []{return logic->CanUse(RG_PRELUDE_OF_LIGHT);}),
    });

    areaTable[RR_CHILD_SPAWN] = Region("Child Spawn", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_KF_LINKS_HOUSE, []{return true;}),
    });

    areaTable[RR_ADULT_SPAWN] = Region("Adult Spawn", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_TEMPLE_OF_TIME, []{return true;}),
    });

    areaTable[RR_MINUET_OF_FOREST_WARP] = Region("Minuet of Forest Warp", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_SACRED_FOREST_MEADOW, []{return true;}),
    });

    areaTable[RR_BOLERO_OF_FIRE_WARP] = Region("Bolero of Fire Warp", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_DMC_CENTRAL_LOCAL, []{return true;}),
    });

    areaTable[RR_SERENADE_OF_WATER_WARP] = Region("Serenade of Water Warp", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_LAKE_HYLIA, []{return true;}),
    });

    areaTable[RR_REQUIEM_OF_SPIRIT_WARP] = Region("Requiem of Spirit Warp", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_DESERT_COLOSSUS, []{return true;}),
    });

    areaTable[RR_NOCTURNE_OF_SHADOW_WARP] = Region("Nocturne of Shadow Warp", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_GRAVEYARD_WARP_PAD_REGION, []{return true;}),
    });

    areaTable[RR_PRELUDE_OF_LIGHT_WARP] = Region("Prelude of Light Warp", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_TEMPLE_OF_TIME, []{return true;}),
    });
    // clang-format on
}
