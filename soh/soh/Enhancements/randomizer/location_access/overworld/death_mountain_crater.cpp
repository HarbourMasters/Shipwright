#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_DeathMountainCrater() {
    // clang-format off
    areaTable[RR_DMC_UPPER_NEARBY] = Region("DMC Upper Nearby", "Death Mountain Crater", {RA_DEATH_MOUNTAIN_CRATER}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_DMC_UPPER_LOCAL,       []{return logic->FireTimer() >= 48;}),
        Entrance(RR_DEATH_MOUNTAIN_SUMMIT, []{return true;}),
        Entrance(RR_DMC_UPPER_GROTTO,      []{return Here(RR_DMC_UPPER_NEARBY, []{return logic->BlastOrSmash() && (logic->FireTimer() >= 8 || logic->Hearts() >= 3);});})
    });

    areaTable[RR_DMC_UPPER_LOCAL] = Region("DMC Upper Local", "Death Mountain Crater", {RA_DEATH_MOUNTAIN_CRATER}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GossipStoneFairy, []{return logic->GossipStoneFairy || (logic->HasExplosives() && logic->CallGossipFairyExceptSuns() && (logic->FireTimer() >= 16 || logic->Hearts() >= 3));}),
    }, {
        //Locations
        LOCATION(RC_DMC_WALL_FREESTANDING_POH,  logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        LOCATION(RC_DMC_GS_CRATE,               (logic->FireTimer() >= 8 || logic->Hearts() >= 3) && logic->IsChild && logic->CanAttack()),
        LOCATION(RC_DMC_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns() && logic->HasExplosives() && (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS) && logic->HasExplosives() && (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_GOSSIP_STONE,           logic->HasExplosives() && (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
    }, {
        //Exits
        Entrance(RR_DMC_UPPER_NEARBY,       []{return true;}),
        Entrance(RR_DMC_LADDER_AREA_NEARBY, []{return logic->FireTimer() >= 16 || logic->Hearts() >= 3;}),
        Entrance(RR_DMC_CENTRAL_NEARBY,     []{return logic->IsAdult && logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_DISTANT_SCARECROW) && ((logic->EffectiveHealth() > 2) || (logic->CanUse(RG_BOTTLE_WITH_FAIRY) && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)) || logic->CanUse(RG_NAYRUS_LOVE));}),
        Entrance(RR_DMC_LOWER_NEARBY,       []{return false;}),
        Entrance(RR_DMC_DISTANT_PLATFORM,   []{return (logic->FireTimer() >= 48 && logic->Hearts() >= 2) || logic->Hearts() >= 3;}),
    });

    areaTable[RR_DMC_LADDER_AREA_NEARBY] = Region("DMC Ladder Region Nearby", "Death Mountain Crater", {RA_DEATH_MOUNTAIN_CRATER}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DMC_DEKU_SCRUB, logic->IsChild && logic->CanStunDeku()),
    }, {
        //Exits
        Entrance(RR_DMC_UPPER_NEARBY, []{return logic->Hearts() >= 3;}),
        Entrance(RR_DMC_LOWER_NEARBY, []{return logic->Hearts() >= 3 && (logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_DMC_BOULDER_JS) && logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER)) || (ctx->GetTrickOption(RT_DMC_BOULDER_SKIP) && logic->IsAdult));}),
    });

    areaTable[RR_DMC_LOWER_NEARBY] = Region("DMC Lower Nearby", "Death Mountain Crater", {RA_DEATH_MOUNTAIN_CRATER}, NO_DAY_NIGHT_CYCLE, {}, {
        // Locations
        LOCATION(RC_DMC_NEAR_GC_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DMC_NEAR_GC_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DMC_NEAR_GC_POT_3, logic->CanBreakPots()),
        LOCATION(RC_DMC_NEAR_GC_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_DMC_LOWER_LOCAL,          []{return logic->FireTimer() >= 48;}),
        Entrance(RR_GC_DARUNIAS_CHAMBER,      []{return true;}),
        Entrance(RR_DMC_GREAT_FAIRY_FOUNTAIN, []{return logic->CanUse(RG_MEGATON_HAMMER);}),
        Entrance(RR_DMC_HAMMER_GROTTO,        []{return logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER);}),
    });

    areaTable[RR_DMC_LOWER_LOCAL] = Region("DMC Lower Local", "Death Mountain Crater", {RA_DEATH_MOUNTAIN_CRATER}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_DMC_LOWER_NEARBY,       []{return true;}),
        Entrance(RR_DMC_LADDER_AREA_NEARBY, []{return logic->FireTimer() >= 8 || logic->Hearts() >= 3;}),
        Entrance(RR_DMC_CENTRAL_NEARBY,     []{return (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT)) && (logic->FireTimer() >= 8 || logic->Hearts() >= 3);}),
        Entrance(RR_DMC_CENTRAL_LOCAL,      []{return (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanShield() && ctx->GetTrickOption(RT_DMC_BOLERO_JUMP))) && logic->FireTimer() >= 24;}),
    });

    areaTable[RR_DMC_CENTRAL_NEARBY] = Region("DMC Central Nearby", "Death Mountain Crater", {RA_DEATH_MOUNTAIN_CRATER}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DMC_VOLCANO_FREESTANDING_POH, logic->IsAdult && logic->Hearts() >= 3 && (CanPlantBean(RR_DMC_CENTRAL_LOCAL) || (ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS)))),
        LOCATION(RC_SHEIK_IN_CRATER,              logic->IsAdult && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
    }, {
        //Exits
        Entrance(RR_DMC_CENTRAL_LOCAL, []{return logic->FireTimer() >= 48;}),
    });

    areaTable[RR_DMC_CENTRAL_LOCAL] = Region("DMC Central Local", "Death Mountain Crater", {RA_DEATH_MOUNTAIN_CRATER}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->BeanPlantFairy, []{return logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->FireTimer() >= 8 || logic->Hearts() >= 3);}),
    }, {
        //Locations
        LOCATION(RC_DMC_GS_BEAN_PATCH,                (logic->FireTimer() >= 8 || logic->Hearts() >= 3) && logic->CanSpawnSoilSkull() && logic->CanAttack()),
        LOCATION(RC_DMC_NEAR_PLATFORM_RED_RUPEE,      logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_RED_RUPEE,    logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_1, logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_2, logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_3, logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_4, logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_5, logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_6, logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_BEAN_SPROUT_FAIRY_1,          logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_BEAN_SPROUT_FAIRY_2,          logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_BEAN_SPROUT_FAIRY_3,          logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
    }, {
        //Exits
        Entrance(RR_DMC_CENTRAL_NEARBY,   []{return true;}),
        Entrance(RR_DMC_LOWER_NEARBY,     []{return (logic->IsAdult && CanPlantBean(RR_DMC_CENTRAL_LOCAL)) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_DMC_UPPER_NEARBY,     []{return logic->IsAdult && CanPlantBean(RR_DMC_CENTRAL_LOCAL);}),
        Entrance(RR_FIRE_TEMPLE_ENTRYWAY, []{return (logic->IsChild && logic->Hearts() >= 3 && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)) || (logic->IsAdult && logic->FireTimer() >= 24);}),
        Entrance(RR_DMC_DISTANT_PLATFORM, []{return logic->FireTimer() >= 48 && logic->CanUse(RG_DISTANT_SCARECROW);}),
    });

    areaTable[RR_DMC_GREAT_FAIRY_FOUNTAIN] = Region("DMC Great Fairy Fountain", "DMC Great Fairy Fountain", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DMC_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        Entrance(RR_DMC_LOWER_LOCAL, []{return true;}),
    });

    areaTable[RR_DMC_UPPER_GROTTO] = Region("DMC Upper Grotto", "DMC Upper Grotto", {}, NO_DAY_NIGHT_CYCLE, grottoEvents, {
        //Locations
        LOCATION(RC_DMC_UPPER_GROTTO_CHEST,                  true),
        LOCATION(RC_DMC_UPPER_GROTTO_FISH,                   logic->HasBottle()),
        LOCATION(RC_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMC_UPPER_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_DMC_UPPER_GROTTO_BEEHIVE_LEFT,           logic->CanBreakLowerBeehives()),
        LOCATION(RC_DMC_UPPER_GROTTO_BEEHIVE_RIGHT,          logic->CanBreakLowerBeehives()),
    }, {
        //Exits
        Entrance(RR_DMC_UPPER_LOCAL, []{return true;}),
    });

    areaTable[RR_DMC_HAMMER_GROTTO] = Region("DMC Hammer Grotto", "DMC Hammer Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku()),
        LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku()),
        LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku()),
        LOCATION(RC_DMC_HAMMER_GROTTO_BEEHIVE,    logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        Entrance(RR_DMC_LOWER_LOCAL, []{return true;}),
    });

    areaTable[RR_DMC_DISTANT_PLATFORM] = Region("DMC Distant Platform", "Death Mountain Crater", {RA_DEATH_MOUNTAIN_CRATER}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_1, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_2, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_3, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_4, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_5, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_6, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_RED_RUPEE,     logic->IsAdult),
    }, {
        //Exits
        Entrance(RR_DMC_CENTRAL_LOCAL, []{return logic->FireTimer() >= 48 && logic->CanUse(RG_DISTANT_SCARECROW);}),
    });

    // clang-format on
}
