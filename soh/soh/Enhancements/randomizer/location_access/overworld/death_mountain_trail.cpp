#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_DeathMountainTrail() {
    // clang-format off
    areaTable[RR_DEATH_MOUNTAIN_TRAIL] = Region("Death Mountain", SCENE_DEATH_MOUNTAIN_TRAIL, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET))),
    }, {
        //Locations
        LOCATION(RC_DMT_CHEST,                    (logic->BlastOrSmash() || (ctx->GetTrickOption(RT_DMT_BOMBABLE) && logic->IsChild && logic->HasItem(RG_GORONS_BRACELET))) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DMT_FREESTANDING_POH,         logic->TakeDamage() || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL, RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL) && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET)))),
        LOCATION(RC_DMT_GS_BEAN_PATCH,            logic->CanSpawnSoilSkull(RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL) && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET) || (ctx->GetTrickOption(RT_DMT_SOIL_GS) && (logic->TakeDamage() || logic->CanUse(RG_HOVER_BOOTS)) && logic->CanUse(RG_BOOMERANG)))),
        LOCATION(RC_DMT_GS_NEAR_KAK,              logic->BlastOrSmash() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_DMT_GS_ABOVE_DODONGOS_CAVERN, logic->IsAdult && logic->CanGetNightTimeGS() && (logic->CanUse(RG_MEGATON_HAMMER) || (ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION) && logic->CanUse(RG_HOOKSHOT)) || (ctx->GetTrickOption(RT_DMT_BEAN_LOWER_GS) && CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL, RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL)) || (ctx->GetTrickOption(RT_DMT_HOVERS_LOWER_GS) && logic->CanUse(RG_HOVER_BOOTS)) || (ctx->GetTrickOption(RT_DMT_JS_LOWER_GS) && logic->CanJumpslash()) || (ctx->GetTrickOption(RT_DISTANT_BOULDER_COLLISION) && logic->CanUse(RG_LONGSHOT)))),
        LOCATION(RC_DMT_BLUE_RUPEE,               logic->IsChild && logic->BlastOrSmash()),
        LOCATION(RC_DMT_RED_RUPEE,                logic->IsChild && logic->BlastOrSmash()),
        LOCATION(RC_DMT_BEAN_SPROUT_FAIRY_1,      logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET))),
        LOCATION(RC_DMT_BEAN_SPROUT_FAIRY_2,      logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET))),
        LOCATION(RC_DMT_BEAN_SPROUT_FAIRY_3,      logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET))),
        LOCATION(RC_DMT_FLAG_SUN_FAIRY,           logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        ENTRANCE(RR_KAK_BEHIND_GATE,          true),
        ENTRANCE(RR_GORON_CITY,               true),
        ENTRANCE(RR_DEATH_MOUNTAIN_ROCKFALL,  AnyAgeTime([]{return logic->BlastOrSmash();}) || (logic->IsAdult && ((CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL, RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL) && logic->HasItem(RG_GORONS_BRACELET)) || (logic->CanUse(RG_HOVER_BOOTS) && ctx->GetTrickOption(RT_DMT_CLIMB_HOVERS))))),
        ENTRANCE(RR_DODONGOS_CAVERN_ENTRYWAY, logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET) || logic->IsAdult),
        ENTRANCE(RR_DMT_STORMS_GROTTO,        logic->CanOpenStormsGrotto()),
    });

    areaTable[RR_DEATH_MOUNTAIN_ROCKFALL] = Region("Death Mountain Rockfall", SCENE_DEATH_MOUNTAIN_TRAIL, {}, {
        //Locations
        LOCATION(RC_DMT_GS_FALLING_ROCKS_PATH, logic->IsAdult && logic->CanGetNightTimeGS() && (logic->CanUse(RG_MEGATON_HAMMER) || (ctx->GetTrickOption(RT_DMT_UPPER_GS) && (logic->CanJumpslash() || logic->HasExplosives() || ((ctx->GetTrickOption(RT_DISTANT_BOULDER_COLLISION) || ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION)) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT))))) || (ctx->GetTrickOption(RT_DISTANT_BOULDER_COLLISION) && logic->CanUse(RG_LONGSHOT)))),
    }, {
        //Exits
        ENTRANCE(RR_DEATH_MOUNTAIN_TRAIL,  true),
        ENTRANCE(RR_DEATH_MOUNTAIN_SUMMIT, logic->HasItem(RG_CLIMB) &&
                                           (logic->IsAdult || ctx->GetTrickOption(RT_DMT_SHIELDLESS_CLIMB) || logic->HasItem(RG_HYLIAN_SHIELD) || logic->CanUse(RG_NAYRUS_LOVE))),
        ENTRANCE(RR_DMT_COW_GROTTO,        AnyAgeTime([]{return logic->BlastOrSmash();})),
    });

    areaTable[RR_DEATH_MOUNTAIN_SUMMIT] = Region("Death Mountain Summit", SCENE_DEATH_MOUNTAIN_TRAIL, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CallGossipFairy()),
        EVENT_ACCESS(LOGIC_BUG_ACCESS,   logic->IsChild && logic->HasItem(RG_POWER_BRACELET)),
    }, {
        //Locations
        LOCATION(RC_DMT_TRADE_BROKEN_SWORD,     logic->IsAdult && logic->CanUse(RG_BROKEN_SWORD)),
        LOCATION(RC_DMT_TRADE_EYEDROPS,         logic->IsAdult && logic->CanUse(RG_EYEDROPS)),
        LOCATION(RC_DMT_TRADE_CLAIM_CHECK,      logic->IsAdult && logic->CanUse(RG_CLAIM_CHECK)),
        LOCATION(RC_DMT_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_DMT_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMT_GOSSIP_STONE,           true),
        LOCATION(RC_BIGGORON_HINT,              logic->IsAdult && logic->HasItem(RG_SPEAK_GORON)),
    }, {
        //Exits
        ENTRANCE(RR_DEATH_MOUNTAIN_ROCKFALL,  true),
        ENTRANCE(RR_DMC_UPPER_ENTRY,          true),
        ENTRANCE(RR_DMT_OWL_FLIGHT,           logic->IsChild && (logic->HasItem(RG_SPEAK_DEKU) || logic->HasItem(RG_SPEAK_GERUDO) || logic->HasItem(RG_SPEAK_GORON) || logic->HasItem(RG_SPEAK_HYLIAN) || logic->HasItem(RG_SPEAK_ZORA))),
        ENTRANCE(RR_DMT_GREAT_FAIRY_FOUNTAIN, AnyAgeTime([]{return logic->BlastOrSmash();})),
    });

    areaTable[RR_DMT_OWL_FLIGHT] = Region("DMT Owl Flight", SCENE_DEATH_MOUNTAIN_TRAIL, {}, {}, {
        //Exits
        ENTRANCE(RR_KAK_IMPAS_ROOFTOP, true, false),
    });

    areaTable[RR_DMT_COW_GROTTO] = Region("DMT Cow Grotto", SCENE_GROTTOS, {}, {
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
        LOCATION(RC_DMT_COW_GROTTO_STORMS_FAIRY,       logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMT_COW_GROTTO_GRASS_1,            logic->CanCutShrubs()),
        LOCATION(RC_DMT_COW_GROTTO_GRASS_2,            logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEATH_MOUNTAIN_ROCKFALL, true),
    });

    areaTable[RR_DMT_STORMS_GROTTO] = Region("DMT Storms Grotto", SCENE_GROTTOS, grottoEvents, {
        //Locations
        LOCATION(RC_DMT_STORMS_GROTTO_CHEST,                  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DMT_STORMS_GROTTO_FISH,                   logic->HasBottle()),
        LOCATION(RC_DMT_STORMS_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_DMT_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMT_STORMS_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_DMT_STORMS_GROTTO_BEEHIVE_LEFT,           logic->CanBreakLowerBeehives()),
        LOCATION(RC_DMT_STORMS_GROTTO_BEEHIVE_RIGHT,          logic->CanBreakLowerBeehives()),
        LOCATION(RC_DMT_STORMS_GROTTO_GRASS_1,                logic->CanCutShrubs()),
        LOCATION(RC_DMT_STORMS_GROTTO_GRASS_2,                logic->CanCutShrubs()),
        LOCATION(RC_DMT_STORMS_GROTTO_GRASS_3,                logic->CanCutShrubs()),
        LOCATION(RC_DMT_STORMS_GROTTO_GRASS_4,                logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEATH_MOUNTAIN_TRAIL, true),
    });

    areaTable[RR_DMT_GREAT_FAIRY_FOUNTAIN] = Region("DMT Great Fairy Fountain", SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, {}, {
        //Locations
        LOCATION(RC_DMT_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        ENTRANCE(RR_DEATH_MOUNTAIN_SUMMIT, true),
    });

    // clang-format on
}
