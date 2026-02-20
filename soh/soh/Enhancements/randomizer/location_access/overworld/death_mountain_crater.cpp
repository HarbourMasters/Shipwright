#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_DeathMountainCrater() {
    // clang-format off
    areaTable[RR_DMC_UPPER_ENTRY] = Region("DMC Upper Entry", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH, (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
    }, {
        //Exits
        ENTRANCE(RR_DMC_UPPER,            (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_GOSSIP_STONE,     (logic->FireTimer() >= 16 || logic->Hearts() >= 3) && logic->HasExplosives()),
        ENTRANCE(RR_DMC_SCRUB,            logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_POTS,             logic->DMCUpperToPots() && (logic->FireTimer() >= 24 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_POT_GROTTO,       logic->DMCUpperToPots() && (logic->FireTimer() >= 24 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_FAIRY_EXIT,       logic->DMCUpperToPots() && (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_DISTANT_PLATFORM, (logic->FireTimer() >= 24 && logic->TakeDamage()) || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_CENTRAL,          ((logic->FireTimer() >= 48 || logic->Hearts() >= 3) && logic->DMCUpperToPots() && logic->DMCPotsToPad()) ||
            
            logic->IsAdult && logic->CanUse(RG_GORON_TUNIC) && logic->ReachDistantScarecrow() && ((logic->EffectiveHealth() > 2) || (logic->CanUse(RG_BOTTLE_WITH_FAIRY) && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)) || logic->CanUse(RG_NAYRUS_LOVE))),
        
    });

    areaTable[RR_DMC_ROCKS_GROTTO_ENTRY] = Region("DMC Rocks Grotto Entry", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH, (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
    }, {
        //Exits
        ENTRANCE(RR_DMC_UPPER,            (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_GOSSIP_STONE,     (logic->FireTimer() >= 16 || logic->Hearts() >= 3) && logic->HasExplosives()),
        ENTRANCE(RR_DMC_SCRUB,    logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_POTS,             logic->DMCUpperToPots() && (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_POT_GROTTO,       logic->DMCUpperToPots() && (logic->FireTimer() >= 24 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_FAIRY_EXIT,       logic->DMCUpperToPots() && (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_DISTANT_PLATFORM, (logic->FireTimer() >= 16 && logic->TakeDamage()) || logic->Hearts() >= 3),

        ENTRANCE(RR_DMC_PAD_ENTRY,   logic->IsAdult && logic->CanUse(RG_GORON_TUNIC) && logic->ReachDistantScarecrow() && ((logic->EffectiveHealth() > 2) || (logic->CanUse(RG_BOTTLE_WITH_FAIRY) && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)) || logic->CanUse(RG_NAYRUS_LOVE))),
    });

    areaTable[RR_DMC_FAIRY_ENTRY] = Region("DMC Fairy Entry", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH, (logic->FireTimer() >= 24 || logic->Hearts() >= 3) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
    }, {
        //Exits
        ENTRANCE(RR_DMC_UPPER,            (logic->FireTimer() >= 24 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_GOSSIP_STONE,     (logic->FireTimer() >= 32 || logic->Hearts() >= 4) && logic->HasExplosives()),
        ENTRANCE(RR_DMC_SCRUB,    logic->FireTimer() >= 8 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_FAIRY_EXIT,       logic->DMCUpperToPots() && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_POTS,             logic->DMCUpperToPots() && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_POT_GROTTO,       logic->DMCUpperToPots() && (logic->FireTimer() >= 16 || logic->Hearts() >= 3)),
        ENTRANCE(RR_DMC_DISTANT_PLATFORM, (logic->FireTimer() >= 32 && logic->TakeDamage()) || logic->Hearts() >= 3),
    });

    areaTable[RR_DMC_POTS_ENTRY] = Region("DMC Pots Entry", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH, (logic->FireTimer() >= 32 || logic->Hearts() >= 3) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
    }, {
        //Exits
        ENTRANCE(RR_DMC_UPPER,            (logic->FireTimer() >= 32 || logic->Hearts() >= 3) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_DMC_GOSSIP_STONE,     (logic->FireTimer() >= 32 || logic->Hearts() >= 4) && logic->HasExplosives() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_DMC_SCRUB,    logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_FAIRY_EXIT,       logic->FireTimer() >= 8 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_POTS,             logic->FireTimer() >= 8 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_POT_GROTTO,       logic->FireTimer() >= 8 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_DISTANT_PLATFORM, ((logic->FireTimer() >= 40 && logic->TakeDamage()) || logic->Hearts() >= 3) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
    });

    areaTable[RR_DMC_POT_GROTTO_ENTRY] = Region("DMC Pot Grotto Entry", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH, (logic->FireTimer() >= 32 || logic->Hearts() >= 3) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
    }, {
        //Exits
        ENTRANCE(RR_DMC_UPPER,            (logic->FireTimer() >= 32 || logic->Hearts() >= 3) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_DMC_GOSSIP_STONE,     (logic->FireTimer() >= 40 || logic->Hearts() >= 4) && logic->HasExplosives() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_DMC_SCRUB,    logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_FAIRY_EXIT,       logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_POTS,             logic->FireTimer() >= 8 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_POT_GROTTO,       logic->FireTimer() >= 8 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_DISTANT_PLATFORM, ((logic->FireTimer() >= 40 && logic->TakeDamage()) || logic->Hearts() >= 3) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
    });

    areaTable[RR_DMC_PAD_ENTRY] = Region("DMC Pad Entry", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations 
        LOCATION(RC_DMC_WALL_FREESTANDING_POH, (logic->FireTimer() >= 32 || logic->Hearts() >= 3) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
    }, {
        //Exits
        ENTRANCE(RR_DMC_UPPER,            (logic->FireTimer() >= 32 || logic->Hearts() >= 3) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_DMC_GOSSIP_STONE,     (logic->FireTimer() >= 40 || logic->Hearts() >= 4) && logic->HasExplosives() && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_DMC_SCRUB,    logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_FAIRY_EXIT,       logic->FireTimer() >= 16 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_POTS,             logic->FireTimer() >= 8 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_POT_GROTTO,       logic->FireTimer() >= 8 || logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_DISTANT_PLATFORM, ((logic->FireTimer() >= 40 && logic->TakeDamage()) || logic->Hearts() >= 3) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
    });


    areaTable[RR_DMC_UPPER] = Region("DMC Upper", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations
        LOCATION(RC_DMC_GS_CRATE, logic->IsChild && logic->CanKillEnemy(RE_GOLD_SKULLTULA) && logic->CanBreakCrates()),
        LOCATION(RC_DMC_CRATE,    logic->IsChild && logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DEATH_MOUNTAIN_SUMMIT, true),
        ENTRANCE(RR_DMC_UPPER_GROTTO,      AnyAgeTime([]{return logic->BlastOrSmash();})),
    });

    areaTable[RR_DMC_GOSSIP_STONE] = Region("DMC Gossip Stone", SCENE_DEATH_MOUNTAIN_CRATER, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->HasExplosives() && logic->CallGossipFairyExceptSuns()),
    }, {
        //Locations
        LOCATION(RC_DMC_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_DMC_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMC_GOSSIP_STONE,           true),
    }, {});

    areaTable[RR_DMC_SCRUB] = Region("DMC Ladder Bottom", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations
        LOCATION(RC_DMC_DEKU_SCRUB, logic->IsChild && logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
    }, {
        //Exits
        ENTRANCE(RR_DMC_UPPER_ENTRY, (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)) && logic->Hearts() >= 3),
        ENTRANCE(RR_DMC_POTS_ENTRY,  logic->Hearts() >= 3),
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
    
    areaTable[RR_DMC_POT_GROTTO] = Region("DMC Pots", SCENE_DEATH_MOUNTAIN_CRATER, {}, {}, {
        //Exits
        ENTRANCE(RR_DMC_SCRUB_GROTTO, logic->CanUse(RG_MEGATON_HAMMER)),
    });

    areaTable[RR_DMC_FAIRY_EXIT] = Region("DMC Fairy Exit", SCENE_DEATH_MOUNTAIN_CRATER, {}, {}, {
        //Exits
        ENTRANCE(RR_DMC_GREAT_FAIRY_FOUNTAIN, logic->CanUse(RG_MEGATON_HAMMER)),
    });

    areaTable[RR_DMC_CENTRAL] = Region("DMC Central Local", SCENE_DEATH_MOUNTAIN_CRATER, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
    }, {
        //Locations
        LOCATION(RC_SHEIK_IN_CRATER,                  logic->IsAdult && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_GS_BEAN_PATCH,                (logic->FireTimer() >= 8 || logic->Hearts() >= 3) && logic->CanSpawnSoilSkull(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) && logic->CanAttack()),
        LOCATION(RC_DMC_NEAR_PLATFORM_RED_RUPEE,      logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_RED_RUPEE,    logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_1, logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_2, logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_3, logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_4, logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_5, logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_6, logic->IsChild && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_BEAN_SPROUT_FAIRY_1,          logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_BEAN_SPROUT_FAIRY_2,          logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
        LOCATION(RC_DMC_BEAN_SPROUT_FAIRY_3,          logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->FireTimer() >= 8 || logic->Hearts() >= 3)),
    }, {
        //Exits
        ENTRANCE(RR_DMC_PAD_ENTRY,         true),
        ENTRANCE(RR_DMC_POTS_ENTRY,        (logic->IsAdult && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)) || (logic->CanUse(RG_HOVER_BOOTS) && (logic->IsAdult || logic->HasItem(RG_CLIMB))) || logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_DMC_UPPER_ENTRY,       logic->IsAdult && CanPlantBean(RR_DMC_CENTRAL, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)),
        ENTRANCE(RR_DMC_FIRE_EXIT,         (logic->IsChild && logic->Hearts() >= 3 && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)) || (logic->IsAdult && logic->FireTimer() >= 24)),
        ENTRANCE(RR_DMC_DISTANT_PLATFORM,  logic->FireTimer() >= 48 && logic->IsAdult && logic->ReachDistantScarecrow()),
    });

    areaTable[RR_DMC_GREAT_FAIRY_FOUNTAIN] = Region("DMC Great Fairy Fountain", SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, {}, {
        //Locations
        LOCATION(RC_DMC_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        ENTRANCE(RR_DMC_POTS, true),
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
        ENTRANCE(RR_DMC_UPPER, true),
    });

    areaTable[RR_DMC_SCRUB_GROTTO] = Region("DMC Hammer Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DMC_HAMMER_GROTTO_BEEHIVE,    logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        ENTRANCE(RR_DMC_POTS, true),
    });

    areaTable[RR_DMC_DISTANT_PLATFORM] = Region("DMC Distant Platform", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
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
        ENTRANCE(RR_DMC_CENTRAL, logic->FireTimer() >= 48 && logic->IsAdult && logic->ReachDistantScarecrow()),
    });

    areaTable[RR_DMC_FIRE_EXIT] = Region("DMC Outside Fire Temple", SCENE_DEATH_MOUNTAIN_CRATER, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_ENTRYWAY, true),
        ENTRANCE(RR_DMC_CENTRAL,    logic->FireTimer() >= 48 && logic->HasItem(RG_CLIMB)),
    });

    // clang-format on
}
