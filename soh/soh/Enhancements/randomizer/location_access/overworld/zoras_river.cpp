#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_ZoraRiver() {
    // clang-format off
    areaTable[RR_ZR_FRONT] = Region("ZR Front", SCENE_ZORAS_RIVER, {}, {
        //Locations
        LOCATION(RC_ZR_GS_TREE,  logic->IsChild && logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_CLOSE) && logic->CanBonkTrees()),
        LOCATION(RC_ZR_GRASS_1,  logic->CanCutShrubs()),
        LOCATION(RC_ZR_GRASS_2,  logic->CanCutShrubs()),
        LOCATION(RC_ZR_GRASS_3,  logic->CanCutShrubs()),
        LOCATION(RC_ZR_GRASS_4,  logic->CanCutShrubs()),
        LOCATION(RC_ZR_GRASS_5,  logic->CanCutShrubs()),
        LOCATION(RC_ZR_GRASS_6,  logic->CanCutShrubs()),
        LOCATION(RC_ZR_GRASS_7,  logic->CanCutShrubs()),
        LOCATION(RC_ZR_GRASS_8,  logic->CanCutShrubs()),
        LOCATION(RC_ZR_GRASS_9,  logic->CanCutShrubs()),
        LOCATION(RC_ZR_GRASS_10, logic->CanCutShrubs()),
        LOCATION(RC_ZR_GRASS_11, logic->CanCutShrubs()),
        LOCATION(RC_ZR_GRASS_12, logic->CanCutShrubs()),
        LOCATION(RC_ZR_TREE,     logic->IsChild && logic->CanBonkTrees()),
    }, {
        //Exits
        ENTRANCE(RR_ZORAS_RIVER,  logic->IsAdult || logic->BlastOrSmash()),
        ENTRANCE(RR_HYRULE_FIELD, true),
    });

    areaTable[RR_ZORAS_RIVER] = Region("Zora River", SCENE_ZORAS_RIVER, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CallGossipFairy() || (logic->IsChild && logic->CanUse(RG_STICKS)) || (logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_ZORAS_RIVER_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS))),
    }, {
        //Locations
        LOCATION(RC_ZR_MAGIC_BEAN_SALESMAN,                  logic->IsChild && logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_ZR_FROGS_OCARINA_GAME,                   logic->IsChild && logic->CanUse(RG_ZELDAS_LULLABY) && logic->CanUse(RG_SARIAS_SONG) && logic->CanUse(RG_SUNS_SONG) && logic->CanUse(RG_EPONAS_SONG) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZR_FROGS_IN_THE_RAIN,                    logic->IsChild && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZR_FROGS_ZELDAS_LULLABY,                 logic->IsChild && logic->CanUse(RG_ZELDAS_LULLABY)),
        LOCATION(RC_ZR_FROGS_EPONAS_SONG,                    logic->IsChild && logic->CanUse(RG_EPONAS_SONG)),
        LOCATION(RC_ZR_FROGS_SARIAS_SONG,                    logic->IsChild && logic->CanUse(RG_SARIAS_SONG)),
        LOCATION(RC_ZR_FROGS_SUNS_SONG,                      logic->IsChild && logic->CanUse(RG_SUNS_SONG)),
        LOCATION(RC_ZR_FROGS_SONG_OF_TIME,                   logic->IsChild && logic->CanUse(RG_SONG_OF_TIME)),
        LOCATION(RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH,    logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_ZR_NEAR_DOMAIN_FREESTANDING_POH,         (logic->IsChild && logic->HasItem(RG_POWER_BRACELET)) || logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && ctx->GetTrickOption(RT_ZR_UPPER))),
        LOCATION(RC_ZR_GS_LADDER,                            logic->IsChild && logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH) && logic->CanGetNightTimeGS()),
        LOCATION(RC_ZR_GS_NEAR_RAISED_GROTTOS,               logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_LONGSHOT) && logic->CanGetNightTimeGS()),
        LOCATION(RC_ZR_GS_ABOVE_BRIDGE,                      logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_HOOKSHOT) && logic->CanGetNightTimeGS()),
        LOCATION(RC_ZR_BEAN_SPROUT_FAIRY_1,                  logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_ZORAS_RIVER_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZR_BEAN_SPROUT_FAIRY_2,                  logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_ZORAS_RIVER_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZR_BEAN_SPROUT_FAIRY_3,                  logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_ZORAS_RIVER_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZR_NEAR_DOMAIN_GOSSIP_STONE_FAIRY,       logic->CallGossipFairy()),
        LOCATION(RC_ZR_NEAR_DOMAIN_GOSSIP_STONE_FAIRY_BIG,   logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZR_BENEATH_WATERFALL_LEFT_RUPEE,         logic->IsAdult && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_ZR_BENEATH_WATERFALL_MIDDLE_LEFT_RUPEE,  logic->IsAdult && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_ZR_BENEATH_WATERFALL_MIDDLE_RIGHT_RUPEE, logic->IsAdult && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_ZR_BENEATH_WATERFALL_RIGHT_RUPEE,        logic->IsAdult && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_ZR_NEAR_DOMAIN_GOSSIP_STONE,             true),
        LOCATION(RC_ZR_NEAR_FREESTANDING_POH_GRASS,          logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_ZR_FRONT,            true),
        ENTRANCE(RR_ZR_ATOP_LADDER,      (logic->IsAdult || logic->HasItem(RG_POWER_BRACELET)) && (logic->CanClimbLadder() || CanPlantBean(RR_ZORAS_RIVER, RG_ZORAS_RIVER_BEAN_SOUL))),
        ENTRANCE(RR_ZR_PILLAR,           (logic->IsChild && logic->HasItem(RG_POWER_BRACELET)) || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && ctx->GetTrickOption(RT_ZR_LOWER))),
        ENTRANCE(RR_ZR_FROM_SHORTCUT,    logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS)),
        ENTRANCE(RR_ZR_STORMS_GROTTO,    logic->CanOpenStormsGrotto()),
        ENTRANCE(RR_ZR_BEHIND_WATERFALL, ctx->GetOption(RSK_SLEEPING_WATERFALL).Is(RO_WATERFALL_OPEN) || AnyAgeTime([]{return logic->CanUse(RG_ZELDAS_LULLABY);}) || (logic->IsChild && ctx->GetTrickOption(RT_ZR_CUCCO) && logic->HasItem(RG_POWER_BRACELET)) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && ctx->GetTrickOption(RT_ZR_HOVERS))),
    });

    areaTable[RR_ZR_ATOP_LADDER] = Region("ZR Atop Ladder", SCENE_ZORAS_RIVER, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CallGossipFairy()),
    }, {
        //Locations
        LOCATION(RC_ZR_GS_NEAR_RAISED_GROTTOS,              logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) && logic->CanGetNightTimeGS()),
        LOCATION(RC_ZR_NEAR_GROTTOS_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_ZR_NEAR_GROTTOS_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZR_NEAR_GROTTOS_GOSSIP_STONE,           true),
    }, {
        //Exits
        ENTRANCE(RR_ZORAS_RIVER,     true),
        ENTRANCE(RR_ZR_PILLAR,       (logic->IsChild && logic->HasItem(RG_POWER_BRACELET)) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_ZR_OPEN_GROTTO,  true),
        ENTRANCE(RR_ZR_FAIRY_GROTTO, AnyAgeTime([]{return logic->BlastOrSmash();})),
    });

    areaTable[RR_ZR_PILLAR] = Region("ZR Pillar", SCENE_ZORAS_RIVER, {
        //Events
        EVENT_ACCESS(LOGIC_BUG_ACCESS, logic->CanCutShrubs()),
    }, {
        //Locations
        LOCATION(RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH, true),
        LOCATION(RC_ZR_NEAR_FREESTANDING_POH_GRASS,       logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_ZORAS_RIVER, true),
    });

    areaTable[RR_ZR_FROM_SHORTCUT] = Region("ZR From Shortcut", SCENE_ZORAS_RIVER, {}, {}, {
        //Exits
        ENTRANCE(RR_ZORAS_RIVER,    logic->Hearts() > 1 || logic->HasItem(RG_BOTTLE_WITH_FAIRY) || logic->HasItem(RG_BRONZE_SCALE)),
        ENTRANCE(RR_THE_LOST_WOODS, logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS)),
    });

    areaTable[RR_ZR_BEHIND_WATERFALL] = Region("ZR Behind Waterfall", SCENE_ZORAS_RIVER, {}, {}, {
        //Exits
        ENTRANCE(RR_ZORAS_RIVER,  true),
        ENTRANCE(RR_ZORAS_DOMAIN, true),
    });

    areaTable[RR_ZR_OPEN_GROTTO] = Region("ZR Open Grotto", SCENE_GROTTOS, grottoEvents, {
        //Locations
        LOCATION(RC_ZR_OPEN_GROTTO_CHEST,                  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_ZR_OPEN_GROTTO_FISH,                   logic->HasBottle()),
        LOCATION(RC_ZR_OPEN_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_ZR_OPEN_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZR_OPEN_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_ZR_OPEN_GROTTO_BEEHIVE_LEFT,           logic->CanBreakLowerBeehives()),
        LOCATION(RC_ZR_OPEN_GROTTO_BEEHIVE_RIGHT,          logic->CanBreakLowerBeehives()),
        LOCATION(RC_ZR_OPEN_GROTTO_GRASS_1,                logic->CanCutShrubs()),
        LOCATION(RC_ZR_OPEN_GROTTO_GRASS_2,                logic->CanCutShrubs()),
        LOCATION(RC_ZR_OPEN_GROTTO_GRASS_3,                logic->CanCutShrubs()),
        LOCATION(RC_ZR_OPEN_GROTTO_GRASS_4,                logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_ZR_ATOP_LADDER, true),
    });

    areaTable[RR_ZR_FAIRY_GROTTO] = Region("ZR Fairy Grotto", SCENE_GROTTOS, {
        //Event
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, true),
    }, {
        //Locations
        LOCATION(RC_ZR_FAIRY_GROTTO_FAIRY_1, true),
        LOCATION(RC_ZR_FAIRY_GROTTO_FAIRY_2, true),
        LOCATION(RC_ZR_FAIRY_GROTTO_FAIRY_3, true),
        LOCATION(RC_ZR_FAIRY_GROTTO_FAIRY_4, true),
        LOCATION(RC_ZR_FAIRY_GROTTO_FAIRY_5, true),
        LOCATION(RC_ZR_FAIRY_GROTTO_FAIRY_6, true),
        LOCATION(RC_ZR_FAIRY_GROTTO_FAIRY_7, true),
        LOCATION(RC_ZR_FAIRY_GROTTO_FAIRY_8, true),
    }, {
        //Exits
        ENTRANCE(RR_ZR_ATOP_LADDER, true),
    });

    areaTable[RR_ZR_STORMS_GROTTO] = Region("ZR Storms Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_ZR_DEKU_SCRUB_GROTTO_REAR,  logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_ZR_DEKU_SCRUB_GROTTO_FRONT, logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_ZR_STORMS_GROTTO_BEEHIVE,   logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        ENTRANCE(RR_ZORAS_RIVER, true),
    });

    // clang-format on
}
