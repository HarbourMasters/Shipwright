#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_DeathMountainTrail() {
    areaTable[RR_DEATH_MOUNTAIN_TRAIL] = Region("Death Mountain", "Death Mountain", {RA_DEATH_MOUNTAIN_TRAIL}, DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->BeanPlantFairy, []{return logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET));}),
    }, {
        //Locations
        LOCATION(RC_DMT_CHEST,                    logic->BlastOrSmash() || (ctx->GetTrickOption(RT_DMT_BOMBABLE) && logic->IsChild && logic->HasItem(RG_GORONS_BRACELET))),
        LOCATION(RC_DMT_FREESTANDING_POH,         logic->TakeDamage() || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL) && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET)))),
        LOCATION(RC_DMT_GS_BEAN_PATCH,            logic->CanSpawnSoilSkull() && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET) || (ctx->GetTrickOption(RT_DMT_SOIL_GS) && (logic->TakeDamage() || logic->CanUse(RG_HOVER_BOOTS)) && logic->CanUse(RG_BOOMERANG)))),
        LOCATION(RC_DMT_GS_NEAR_KAK,              logic->BlastOrSmash()),
        LOCATION(RC_DMT_GS_ABOVE_DODONGOS_CAVERN, logic->IsAdult && logic->AtNight && (logic->CanUse(RG_MEGATON_HAMMER) || (ctx->GetTrickOption(RT_DMT_HOOKSHOT_LOWER_GS) && logic->CanUse(RG_HOOKSHOT)) || (ctx->GetTrickOption(RT_DMT_BEAN_LOWER_GS) && CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL)) || (ctx->GetTrickOption(RT_DMT_HOVERS_LOWER_GS) && logic->CanUse(RG_HOVER_BOOTS)) || ctx->GetTrickOption(RT_DMT_JS_LOWER_GS)) && logic->CanGetNightTimeGS()),
        LOCATION(RC_DMT_BLUE_RUPEE,               logic->IsChild && logic->BlastOrSmash()),
        LOCATION(RC_DMT_RED_RUPEE,                logic->IsChild && logic->BlastOrSmash()),
        LOCATION(RC_DMT_BEAN_SPROUT_FAIRY_1,      logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET))),
        LOCATION(RC_DMT_BEAN_SPROUT_FAIRY_2,      logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET))),
        LOCATION(RC_DMT_BEAN_SPROUT_FAIRY_3,      logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET))),
        LOCATION(RC_DMT_FLAG_SUN_FAIRY,           logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        Entrance(RR_KAK_BEHIND_GATE,          []{return true;}),
        Entrance(RR_GORON_CITY,               []{return true;}),
        Entrance(RR_DEATH_MOUNTAIN_SUMMIT,    []{return Here(RR_DEATH_MOUNTAIN_TRAIL, []{return logic->BlastOrSmash();}) || (logic->IsAdult && ((CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL) && logic->HasItem(RG_GORONS_BRACELET)) || (logic->CanUse(RG_HOVER_BOOTS) && ctx->GetTrickOption(RT_DMT_CLIMB_HOVERS))));}),
        Entrance(RR_DODONGOS_CAVERN_ENTRYWAY, []{return logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET) || logic->IsAdult;}),
        Entrance(RR_DMT_STORMS_GROTTO,        []{return logic->CanOpenStormsGrotto();}),
    });

    areaTable[RR_DEATH_MOUNTAIN_SUMMIT] = Region("Death Mountain Summit", "Death Mountain", {RA_DEATH_MOUNTAIN_TRAIL}, DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GossipStoneFairy, []{return logic->CallGossipFairy();}),
        EventAccess(&logic->BugRock,          []{return logic->BugRock            || logic->IsChild;}),
    }, {
        //Locations
        LOCATION(RC_DMT_TRADE_BROKEN_SWORD,     logic->IsAdult && logic->CanUse(RG_BROKEN_SWORD)),
        LOCATION(RC_DMT_TRADE_EYEDROPS,         logic->IsAdult && logic->CanUse(RG_EYEDROPS)),
        LOCATION(RC_DMT_TRADE_CLAIM_CHECK,      logic->IsAdult && logic->CanUse(RG_CLAIM_CHECK)),
        LOCATION(RC_DMT_GS_FALLING_ROCKS_PATH,  logic->IsAdult && logic->AtNight && (logic->CanUse(RG_MEGATON_HAMMER) || ctx->GetTrickOption(RT_DMT_UPPER_GS)) && logic->CanGetNightTimeGS()),
        LOCATION(RC_DMT_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_DMT_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMT_GOSSIP_STONE,           true),
    }, {
        //Exits
        Entrance(RR_DEATH_MOUNTAIN_TRAIL,     []{return true;}),
        Entrance(RR_DMC_UPPER_LOCAL,          []{return true;}),
        Entrance(RR_DMT_OWL_FLIGHT,           []{return logic->IsChild;}, false),
        Entrance(RR_DMT_COW_GROTTO,           []{return Here(RR_DEATH_MOUNTAIN_SUMMIT, []{return logic->BlastOrSmash();});}),
        Entrance(RR_DMT_GREAT_FAIRY_FOUNTAIN, []{return Here(RR_DEATH_MOUNTAIN_SUMMIT, []{return logic->BlastOrSmash();});}),
    });

    areaTable[RR_DMT_OWL_FLIGHT] = Region("DMT Owl Flight", "Death Mountain", {RA_DEATH_MOUNTAIN_TRAIL}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_KAK_IMPAS_ROOFTOP, []{return true;}),
    });

    areaTable[RR_DMT_COW_GROTTO] = Region("DMT Cow Grotto", "DMT Cow Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DMT_COW_GROTTO_COW,                logic->CanUse(RG_EPONAS_SONG)),
        LOCATION(RC_DMT_COW_GROTTO_BEEHIVE,            logic->CanBreakLowerBeehives()),
        LOCATION(RC_DMT_COW_GROTTO_LEFT_HEART,         true),
        LOCATION(RC_DMT_COW_GROTTO_MIDDLE_LEFT_HEART,  true),
        LOCATION(RC_DMT_COW_GROTTO_MIDDLE_RIGHT_HEART, true),
        LOCATION(RC_DMT_COW_GROTTO_RIGHT_HEART,        true),
        LOCATION(RC_DMT_COW_GROTTO_RUPEE_1,            true),
        LOCATION(RC_DMT_COW_GROTTO_RUPEE_2,            true),
        LOCATION(RC_DMT_COW_GROTTO_RUPEE_3,            true),
        LOCATION(RC_DMT_COW_GROTTO_RUPEE_4,            true),
        LOCATION(RC_DMT_COW_GROTTO_RUPEE_5,            true),
        LOCATION(RC_DMT_COW_GROTTO_RUPEE_6,            true),
        LOCATION(RC_DMT_COW_GROTTO_RED_RUPEE,          true),
    }, {
        //Exits
        Entrance(RR_DEATH_MOUNTAIN_SUMMIT, []{return true;}),
    });

    areaTable[RR_DMT_STORMS_GROTTO] = Region("DMT Storms Grotto", "DMT Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, grottoEvents, {
        //Locations
        LOCATION(RC_DMT_STORMS_GROTTO_CHEST,                  true),
        LOCATION(RC_DMT_STORMS_GROTTO_FISH,                   logic->HasBottle()),
        LOCATION(RC_DMT_STORMS_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_DMT_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMT_STORMS_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_DMT_STORMS_GROTTO_BEEHIVE_LEFT,           logic->CanBreakLowerBeehives()),
        LOCATION(RC_DMT_STORMS_GROTTO_BEEHIVE_RIGHT,          logic->CanBreakLowerBeehives()),
    }, {
        //Exits
        Entrance(RR_DEATH_MOUNTAIN_TRAIL, []{return true;}),
    });

    areaTable[RR_DMT_GREAT_FAIRY_FOUNTAIN] = Region("DMT Great Fairy Fountain", "DMT Great Fairy Fountain", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DMT_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        Entrance(RR_DEATH_MOUNTAIN_SUMMIT, []{return true;}),
    });
}