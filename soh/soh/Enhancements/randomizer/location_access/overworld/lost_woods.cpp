#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_LostWoods() {
    // clang-format off
    areaTable[RR_LW_FOREST_EXIT] = Region("LW Forest Exit", "Lost Woods", {RA_THE_LOST_WOODS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_KOKIRI_FOREST, []{return true;})
    });

    areaTable[RR_THE_LOST_WOODS] = Region("Lost Woods", "Lost Woods", {RA_THE_LOST_WOODS}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GossipStoneFairy, []{return logic->CallGossipFairyExceptSuns();}),
        EventAccess(&logic->BeanPlantFairy,   []{return logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS);}),
        EventAccess(&logic->BugShrub,         []{return logic->IsChild && logic->CanCutShrubs();}),
        EventAccess(&logic->BorrowSpookyMask, []{return logic->IsChild && logic->BorrowSkullMask && logic->CanUse(RG_SARIAS_SONG) && logic->HasItem(RG_CHILD_WALLET);}),
    }, {
        //Locations
        LOCATION(RC_LW_SKULL_KID,                       logic->IsChild && logic->CanUse(RG_SARIAS_SONG)),
        LOCATION(RC_LW_TRADE_COJIRO,                    logic->IsAdult && logic->CanUse(RG_COJIRO)),
        //I cannot think of a case where you can use Odd pot but not Cojiro to reset the quadrant should you have both. If one exists, add it to logic
        LOCATION(RC_LW_TRADE_ODD_POTION,                logic->IsAdult && logic->CanUse(RG_ODD_POTION)),
                                                                                                              //all 5 buttons are logically required for memory game
                                                                                                              //because the chances of being able to beat it
                                                                                                              //every time you attempt it are as follows:
                                                                                                              //0 or 1 button(s) => 0%
                                                                                                              //2 buttons        => 0.15625%
                                                                                                              //3 buttons        => 3.75%
                                                                                                              //4 buttons        => 25.3125%
                                                                                                              //5 buttons        => 100%
        LOCATION(RC_LW_OCARINA_MEMORY_GAME,             logic->IsChild && logic->HasItem(RG_FAIRY_OCARINA) && logic->OcarinaButtons() >= 5),
        LOCATION(RC_LW_TARGET_IN_WOODS,                 logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT)),
        LOCATION(RC_LW_DEKU_SCRUB_NEAR_BRIDGE,          logic->IsChild && logic->CanStunDeku()),
        LOCATION(RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE,       logic->CanSpawnSoilSkull() && logic->CanAttack()),
        //RANDOTODO handle collecting some of these as you leave the shortcut from the other side
        LOCATION(RC_LW_SHORTCUT_RUPEE_1,                logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
        LOCATION(RC_LW_SHORTCUT_RUPEE_2,                logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
        LOCATION(RC_LW_SHORTCUT_RUPEE_3,                logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
        LOCATION(RC_LW_SHORTCUT_RUPEE_4,                logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
        LOCATION(RC_LW_SHORTCUT_RUPEE_5,                logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
        LOCATION(RC_LW_SHORTCUT_RUPEE_6,                logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
        LOCATION(RC_LW_SHORTCUT_RUPEE_7,                logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
        LOCATION(RC_LW_SHORTCUT_RUPEE_8,                logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
        LOCATION(RC_LW_BEAN_SPROUT_NEAR_BRIDGE_FAIRY_1, logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LW_BEAN_SPROUT_NEAR_BRIDGE_FAIRY_2, logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LW_BEAN_SPROUT_NEAR_BRIDGE_FAIRY_3, logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LW_GOSSIP_STONE_FAIRY,              logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_LW_GOSSIP_STONE_FAIRY_BIG,          logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LW_SHORTCUT_STORMS_FAIRY,           logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LW_GOSSIP_STONE,                    true),
        LOCATION(RC_LW_GRASS_1,                         logic->CanCutShrubs()),
        LOCATION(RC_LW_GRASS_2,                         logic->CanCutShrubs()),
        LOCATION(RC_LW_GRASS_3,                         logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_LW_FOREST_EXIT,           []{return true;}),
        Entrance(RR_GC_WOODS_WARP,            []{return true;}),
        Entrance(RR_LW_BRIDGE,                []{return logic->CanLeaveForest() && ((logic->IsAdult && (CanPlantBean(RR_THE_LOST_WOODS) || ctx->GetTrickOption(RT_LW_BRIDGE))) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT));}),
        Entrance(RR_ZORAS_RIVER,              []{return logic->CanLeaveForest() && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS));}),
        Entrance(RR_LW_BEYOND_MIDO,           []{return logic->IsChild || logic->CanUse(RG_SARIAS_SONG) || ctx->GetTrickOption(RT_LW_MIDO_BACKFLIP);}),
        Entrance(RR_LW_NEAR_SHORTCUTS_GROTTO, []{return Here(RR_THE_LOST_WOODS, []{return logic->BlastOrSmash();});}),
    });

    areaTable[RR_LW_BEYOND_MIDO] = Region("LW Beyond Mido", "Lost Woods", {RA_THE_LOST_WOODS}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->ButterflyFairy, []{return logic->ButterflyFairy || logic->CanUse(RG_STICKS);}),
    }, {
        //Locations
        LOCATION(RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT, logic->IsChild && logic->CanStunDeku()),
        LOCATION(RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,  logic->IsChild && logic->CanStunDeku()),
        LOCATION(RC_LW_GS_ABOVE_THEATER,                   logic->IsAdult && ((CanPlantBean(RR_LW_BEYOND_MIDO) && logic->CanAttack()) || (ctx->GetTrickOption(RT_LW_GS_BEAN) && logic->CanUse(RG_HOOKSHOT) && (logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOMBCHU_5) || logic->CanUse(RG_DINS_FIRE)))) && logic->CanGetNightTimeGS()),
        LOCATION(RC_LW_GS_BEAN_PATCH_NEAR_THEATER,         logic->CanSpawnSoilSkull() && (logic->CanAttack() || (ctx->GetOption(RSK_SHUFFLE_SCRUBS).Is(RO_SCRUBS_OFF) && logic->CanReflectNuts()))),
        LOCATION(RC_LW_BOULDER_RUPEE,                      logic->BlastOrSmash()),
        LOCATION(RC_LW_BEAN_SPROUT_NEAR_THEATER_FAIRY_1,   logic->IsChild && logic->HasItem(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LW_BEAN_SPROUT_NEAR_THEATER_FAIRY_2,   logic->IsChild && logic->HasItem(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LW_BEAN_SPROUT_NEAR_THEATER_FAIRY_3,   logic->IsChild && logic->HasItem(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LW_GRASS_4,                            logic->CanCutShrubs()),
        LOCATION(RC_LW_GRASS_5,                            logic->CanCutShrubs()),
        LOCATION(RC_LW_GRASS_6,                            logic->CanCutShrubs()),
        LOCATION(RC_LW_GRASS_7,                            logic->CanCutShrubs()),
        LOCATION(RC_LW_GRASS_8,                            logic->CanCutShrubs()),
        LOCATION(RC_LW_GRASS_9,                            logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_LW_FOREST_EXIT,   []{return true;}),
        Entrance(RR_THE_LOST_WOODS,   []{return logic->IsChild || logic->CanUse(RG_SARIAS_SONG);}),
        Entrance(RR_SFM_ENTRYWAY,     []{return true;}),
        Entrance(RR_DEKU_THEATER,     []{return true;}),
        Entrance(RR_LW_SCRUBS_GROTTO, []{return Here(RR_LW_BEYOND_MIDO, []{return logic->BlastOrSmash();});}),
    });

    areaTable[RR_LW_NEAR_SHORTCUTS_GROTTO] = Region("LW Near Shortcuts Grotto", "LW Near Shortcuts Grotto", {}, NO_DAY_NIGHT_CYCLE, grottoEvents, {
        //Locations
        LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST,                  true),
        LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_FISH,                   logic->HasBottle()),
        LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_BEEHIVE_LEFT,           logic->CanBreakLowerBeehives()),
        LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_BEEHIVE_RIGHT,          logic->CanBreakLowerBeehives()),
        LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_GRASS_1,                logic->CanCutShrubs()),
        LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_GRASS_2,                logic->CanCutShrubs()),
        LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_GRASS_3,                logic->CanCutShrubs()),
        LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_GRASS_4,                logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_THE_LOST_WOODS, []{return true;}),
    });

    areaTable[RR_DEKU_THEATER] = Region("Deku Theater", "Deku Theater", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DEKU_THEATER_SKULL_MASK,    logic->IsChild && logic->BorrowSkullMask),
        LOCATION(RC_DEKU_THEATER_MASK_OF_TRUTH, logic->IsChild && logic->BorrowRightMasks),
    }, {
        //Exits
        Entrance(RR_LW_BEYOND_MIDO, []{return true;}),
    });

    areaTable[RR_LW_SCRUBS_GROTTO] = Region("LW Scrubs Grotto", "LW Scrubs Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_LW_DEKU_SCRUB_GROTTO_REAR,      logic->CanStunDeku()),
        LOCATION(RC_LW_DEKU_SCRUB_GROTTO_FRONT,     logic->CanStunDeku()),
        LOCATION(RC_LW_DEKU_SCRUB_GROTTO_BEEHIVE,   logic->CanBreakUpperBeehives()), 
        LOCATION(RC_LW_DEKU_SCRUB_GROTTO_SUN_FAIRY, logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        Entrance(RR_LW_BEYOND_MIDO, []{return true;}),
    });

    areaTable[RR_LW_BRIDGE_FROM_FOREST] = Region("LW Bridge From Forest", "Lost Woods", {RA_THE_LOST_WOODS}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_LW_GIFT_FROM_SARIA, true),
    }, {
        //Exits
        Entrance(RR_LW_BRIDGE, []{return true;}),
    });

    areaTable[RR_LW_BRIDGE] = Region("LW Bridge", "Lost Woods", {RA_THE_LOST_WOODS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_KOKIRI_FOREST,  []{return true;}),
        Entrance(RR_HYRULE_FIELD,   []{return true;}),
        Entrance(RR_THE_LOST_WOODS, []{return logic->CanUse(RG_LONGSHOT);}),
    });

    // clang-format on
}
