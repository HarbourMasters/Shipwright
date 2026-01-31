#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_GerudoValley() {
    // clang-format off
    areaTable[RR_GERUDO_VALLEY] = Region("Gerudo Valley", SCENE_GERUDO_VALLEY, {
        //Events
        EVENT_ACCESS(LOGIC_BUG_ACCESS, logic->IsChild && logic->HasItem(RG_POWER_BRACELET)),
    }, {
        //Locations
        LOCATION(RC_GV_GS_SMALL_BRIDGE,            logic->IsChild && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        ENTRANCE(RR_HYRULE_FIELD,          true),
        ENTRANCE(RR_GV_UPPER_STREAM,       (logic->IsChild && logic->HasItem(RG_POWER_BRACELET)) || logic->TakeDamage()),
        ENTRANCE(RR_GV_UPPER_STREAM_WATER, true),
        ENTRANCE(RR_GV_CRATE_LEDGE,        (logic->IsChild && logic->HasItem(RG_POWER_BRACELET)) || logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_GV_GROTTO_LEDGE,       true),
        ENTRANCE(RR_GV_FORTRESS_SIDE,      (logic->IsAdult && (logic->SummonEpona() || logic->CanUse(RG_LONGSHOT) || ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) || logic->Get(LOGIC_TH_RESCUED_ALL_CARPENTERS))) || (ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_HOVER_BOOTS)) ||
                                           ((logic->IsChild || ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION)) && logic->CanUse(RG_HOOKSHOT)) || (logic->IsChild && ctx->GetTrickOption(RT_GV_CHILD_CUCCO_JUMP) && logic->HasItem(RG_POWER_BRACELET) && logic->CanJumpslash())),
        ENTRANCE(RR_GV_WATERFALL_ALCOVE,   logic->IsChild && logic->HasItem(RG_POWER_BRACELET)),
        ENTRANCE(RR_GV_LOWER_STREAM,       logic->IsChild && logic->HasItem(RG_POWER_BRACELET)),
    });

    areaTable[RR_GV_UPPER_STREAM] = Region("GV Upper Stream", SCENE_GERUDO_VALLEY, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CallGossipFairy() || (logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_GERUDO_VALLEY_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS))),
    }, {
        //Locations
        LOCATION(RC_GV_GS_BEAN_PATCH,          logic->CanSpawnSoilSkull(RG_GERUDO_VALLEY_BEAN_SOUL) && logic->CanAttack()),
        LOCATION(RC_GV_COW,                    logic->IsChild && logic->CanUse(RG_EPONAS_SONG)),
        LOCATION(RC_GV_BEAN_SPROUT_FAIRY_1,    logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_GERUDO_VALLEY_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GV_BEAN_SPROUT_FAIRY_2,    logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_GERUDO_VALLEY_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GV_BEAN_SPROUT_FAIRY_3,    logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_GERUDO_VALLEY_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GV_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_GV_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GV_GOSSIP_STONE,           true),
        LOCATION(RC_GV_NEAR_COW_CRATE,         logic->IsChild && logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_GV_UPPER_STREAM_WATER, true),
        ENTRANCE(RR_GV_WATERFALL_ALCOVE,   (logic->CanUse(RG_HOOKSHOT) && logic->HasItem(RG_CLIMB)) || logic->CanUse(RG_LONGSHOT) || CanPlantBean(RR_GV_UPPER_STREAM, RG_GERUDO_VALLEY_BEAN_SOUL)),
    });

    areaTable[RR_GV_UPPER_STREAM_WATER] = Region("GV Upper Stream Water", SCENE_GERUDO_VALLEY, {}, {}, {
        //Exits
        ENTRANCE(RR_GV_UPPER_STREAM,     logic->HasItem(RG_BRONZE_SCALE)),
        ENTRANCE(RR_GV_LOWER_STREAM,     logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS)),
        ENTRANCE(RR_GV_WATERFALL_ALCOVE, (logic->HasItem(RG_BRONZE_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))) && logic->HasItem(RG_CLIMB)),
    });

    // scale/boots logic is outside lower stream, as lower stream combines access to lake hylia for entrance randomizer's sake
    areaTable[RR_GV_LOWER_STREAM] = Region("GV Lower Stream", SCENE_GERUDO_VALLEY, {}, {}, {
        //Exits
        ENTRANCE(RR_LAKE_HYLIA, true),
    });

    areaTable[RR_GV_WATERFALL_ALCOVE] = Region("GV Waterfall Alcove", SCENE_GERUDO_VALLEY, {}, {
        //Locations
        LOCATION(RC_GV_WATERFALL_FREESTANDING_POH, true),
    }, {
        //Exits
        ENTRANCE(RR_GV_UPPER_STREAM,       logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_GV_UPPER_STREAM_WATER, true),
    });

    areaTable[RR_GV_GROTTO_LEDGE] = Region("GV Grotto Ledge", SCENE_GERUDO_VALLEY, {}, {}, {
        //Exits
        ENTRANCE(RR_GV_UPPER_STREAM,   ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives() && logic->TakeDamage()),
        ENTRANCE(RR_GV_LOWER_STREAM,   logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS)),
        ENTRANCE(RR_GV_OCTOROK_GROTTO, logic->CanUse(RG_SILVER_GAUNTLETS)),
        ENTRANCE(RR_GV_CRATE_LEDGE,    logic->CanUse(RG_LONGSHOT)),
    });

    areaTable[RR_GV_CRATE_LEDGE] = Region("GV Crate Ledge", SCENE_GERUDO_VALLEY, {}, {
        //Locations
        LOCATION(RC_GV_CRATE_FREESTANDING_POH, logic->CanBreakCrates()),
        LOCATION(RC_GV_FREESTANDING_POH_CRATE, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_GV_UPPER_STREAM, ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives() && logic->TakeDamage()),
        ENTRANCE(RR_GV_LOWER_STREAM, logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS)),
    });

    areaTable[RR_GV_FORTRESS_SIDE] = Region("GV Fortress Side", SCENE_GERUDO_VALLEY, {}, {
        //Locations
        LOCATION(RC_GV_CHEST,          logic->IsAdult && (logic->CanUse(RG_MEGATON_HAMMER) || (ctx->GetTrickOption(RT_DISTANT_BOULDER_COLLISION) && logic->CanUse(RG_LONGSHOT))) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GV_TRADE_SAW,      logic->IsAdult && logic->CanUse(RG_POACHERS_SAW)),
        LOCATION(RC_GV_GS_BEHIND_TENT, logic->IsAdult && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
        LOCATION(RC_GV_GS_PILLAR,      logic->IsAdult && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
        LOCATION(RC_GV_CRATE_BRIDGE_1, logic->IsChild && logic->CanBreakCrates()),
        LOCATION(RC_GV_CRATE_BRIDGE_2, logic->IsChild && logic->CanBreakCrates()),
        LOCATION(RC_GV_CRATE_BRIDGE_3, logic->IsChild && logic->CanBreakCrates()),
        LOCATION(RC_GV_CRATE_BRIDGE_4, logic->IsChild && logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_GF_OUTSKIRTS,          true),
        ENTRANCE(RR_GV_UPPER_STREAM,       logic->TakeDamage()),
        ENTRANCE(RR_GV_UPPER_STREAM_WATER, true),
        ENTRANCE(RR_GERUDO_VALLEY,         logic->IsChild || logic->SummonEpona() || logic->CanUse(RG_LONGSHOT) || ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) || logic->Get(LOGIC_TH_RESCUED_ALL_CARPENTERS) || (ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_HOVER_BOOTS))),
        ENTRANCE(RR_GV_CARPENTER_TENT,     logic->IsAdult || ctx->GetTrickOption(RT_GV_CHILD_TENT)),
        ENTRANCE(RR_GV_STORMS_GROTTO,      logic->IsAdult && logic->CanOpenStormsGrotto()),
        ENTRANCE(RR_GV_CRATE_LEDGE,        (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives()) || (ctx->GetTrickOption(RT_GV_CRATE_HOVERS) && logic->TakeDamage() && logic->CanUse(RG_HOVER_BOOTS) && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)))),
    });

    areaTable[RR_GV_CARPENTER_TENT] = Region("GV Carpenter Tent", SCENE_CARPENTERS_TENT, {}, {}, {
        //Exits
        ENTRANCE(RR_GV_FORTRESS_SIDE, true),
    });

    areaTable[RR_GV_OCTOROK_GROTTO] = Region("GV Octorok Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_GV_OCTOROK_GROTTO_FRONT_LEFT_BLUE_RUPEE,   logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_FRONT_RIGHT_BLUE_RUPEE,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_BACK_BLUE_RUPEE,         logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_FRONT_LEFT_GREEN_RUPEE,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_FRONT_RIGHT_GREEN_RUPEE, logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_BACK_LEFT_GREEN_RUPEE,   logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_BACK_RIGHT_GREEN_RUPEE,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_RED_RUPEE,               logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_GV_GROTTO_LEDGE, true),
    });

    areaTable[RR_GV_STORMS_GROTTO] = Region("GV Storms Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_GV_DEKU_SCRUB_GROTTO_REAR,    logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_GV_DEKU_SCRUB_GROTTO_FRONT,   logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_GV_DEKU_SCRUB_GROTTO_BEEHIVE, logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        ENTRANCE(RR_GV_FORTRESS_SIDE, true),
    });

    // clang-format on
}
