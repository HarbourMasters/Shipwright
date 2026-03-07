#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_Kakariko() {
    // clang-format off
    areaTable[RR_KAKARIKO_VILLAGE] = Region("Kakariko Village", SCENE_KAKARIKO_VILLAGE, {
        //Events
        EVENT_ACCESS(LOGIC_BUG_ACCESS,         logic->HasItem(RG_POWER_BRACELET)),
        //Open Gate setting is applied in RR_ROOT
        EVENT_ACCESS(LOGIC_KAKARIKO_GATE_OPEN, logic->IsChild && logic->HasItem(RG_ZELDAS_LETTER)),
        //Needs wallet to be able to get another mask after selling Keaton
        EVENT_ACCESS(LOGIC_BORROW_SKULL_MASK,  logic->IsChild && logic->Get(LOGIC_CAN_BORROW_MASKS) && logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_SPEAK_HYLIAN)),
    }, {
        //Locations
        LOCATION(RC_SHEIK_IN_KAKARIKO,                     logic->IsAdult && logic->HasItem(RG_FOREST_MEDALLION) && logic->HasItem(RG_FIRE_MEDALLION) && logic->HasItem(RG_WATER_MEDALLION)),
        LOCATION(RC_KAK_ANJU_AS_CHILD,                     logic->IsChild && logic->AtDay && logic->HasItem(RG_CLIMB) && logic->HasItem(RG_POWER_BRACELET) && logic->CanBreakCrates() && logic->HasItem(RG_SPEAK_HYLIAN)), // RANDOTODO adjust requirements to cucco settings
        LOCATION(RC_KAK_ANJU_AS_ADULT,                     logic->IsAdult && logic->AtDay && logic->HasItem(RG_SPEAK_HYLIAN)),
        LOCATION(RC_KAK_TRADE_POCKET_CUCCO,                logic->IsAdult && logic->AtDay && (logic->CanUse(RG_POCKET_EGG) && logic->Get(LOGIC_WAKE_UP_ADULT_TALON))),
        //Can kill lower kak skulls with pots
        LOCATION(RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION,       logic->IsChild && logic->CanGetNightTimeGS() && (logic->HasItem(RG_POWER_BRACELET) || logic->CanKillEnemy(RE_GOLD_SKULLTULA))),
        LOCATION(RC_KAK_GS_SKULLTULA_HOUSE,                logic->IsChild && logic->CanGetNightTimeGS() && (logic->HasItem(RG_POWER_BRACELET) || logic->CanKillEnemy(RE_GOLD_SKULLTULA))),
        LOCATION(RC_KAK_GS_GUARDS_HOUSE,                   logic->IsChild && logic->CanGetNightTimeGS() && (logic->HasItem(RG_POWER_BRACELET) || logic->CanKillEnemy(RE_GOLD_SKULLTULA))),
        LOCATION(RC_KAK_GS_TREE,                           logic->IsChild && logic->CanGetNightTimeGS() && logic->CanBonkTrees() && (logic->HasItem(RG_POWER_BRACELET) || logic->CanKillEnemy(RE_GOLD_SKULLTULA))),
        LOCATION(RC_KAK_GS_WATCHTOWER,                     logic->IsChild &&  logic->HasItem(RG_CLIMB) && (logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_LONGSHOT) || (ctx->GetTrickOption(RT_KAK_TOWER_GS) && logic->CanJumpslashExceptHammer())) && logic->CanGetNightTimeGS()),
        LOCATION(RC_KAK_NEAR_POTION_SHOP_POT_1,            logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_KAK_NEAR_POTION_SHOP_POT_2,            logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_KAK_NEAR_POTION_SHOP_POT_3,            logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_KAK_NEAR_IMPAS_HOUSE_POT_1,            logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_KAK_NEAR_IMPAS_HOUSE_POT_2,            logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_KAK_NEAR_IMPAS_HOUSE_POT_3,            logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_KAK_NEAR_GUARDS_HOUSE_POT_1,           logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_KAK_NEAR_GUARDS_HOUSE_POT_2,           logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_KAK_NEAR_GUARDS_HOUSE_POT_3,           logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_KAK_GRASS_1,                           logic->CanCutShrubs()),
        LOCATION(RC_KAK_GRASS_2,                           logic->CanCutShrubs()),
        LOCATION(RC_KAK_GRASS_3,                           logic->CanCutShrubs()),
        LOCATION(RC_KAK_GRASS_4,                           logic->CanCutShrubs()),
        LOCATION(RC_KAK_GRASS_5,                           logic->CanCutShrubs()),
        LOCATION(RC_KAK_GRASS_6,                           logic->CanCutShrubs()),
        LOCATION(RC_KAK_GRASS_7,                           logic->CanCutShrubs()),
        LOCATION(RC_KAK_GRASS_8,                           logic->CanCutShrubs()),
        LOCATION(RC_KAK_NEAR_OPEN_GROTTO_ADULT_CRATE_1,    logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_OPEN_GROTTO_ADULT_CRATE_2,    logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_OPEN_GROTTO_ADULT_CRATE_3,    logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_OPEN_GROTTO_ADULT_CRATE_4,    logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_POTION_SHOP_ADULT_CRATE,      logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_SHOOTING_GALLERY_ADULT_CRATE, logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_BOARDING_HOUSE_ADULT_CRATE_1, logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_BOARDING_HOUSE_ADULT_CRATE_2, logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_IMPAS_HOUSE_ADULT_CRATE_1,    logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_IMPAS_HOUSE_ADULT_CRATE_2,    logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_BAZAAR_ADULT_CRATE_1,         logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_BAZAAR_ADULT_CRATE_2,         logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_BEHIND_GS_HOUSE_ADULT_CRATE,       logic->IsAdult && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_GY_CHILD_CRATE,               logic->IsChild && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_WINDMILL_CHILD_CRATE,         logic->IsChild && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_FENCE_CHILD_CRATE,            logic->IsChild && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_BOARDING_HOUSE_CHILD_CRATE,   logic->IsChild && logic->CanBreakCrates()),
        LOCATION(RC_KAK_NEAR_BAZAAR_CHILD_CRATE,           logic->IsChild && logic->CanBreakCrates()),
        LOCATION(RC_KAK_TREE,                              logic->CanBonkTrees()),
    }, {
        //Exits
        ENTRANCE(RR_HYRULE_FIELD,             true),
        ENTRANCE(RR_KAK_CARPENTER_BOSS_HOUSE, logic->CanOpenOverworldDoor(RG_BOSS_HOUSE_KEY)),
        ENTRANCE(RR_KAK_HOUSE_OF_SKULLTULA,   logic->CanOpenOverworldDoor(RG_SKULLTULA_HOUSE_KEY)),
        ENTRANCE(RR_KAK_IMPAS_HOUSE,          logic->CanOpenOverworldDoor(RG_IMPAS_HOUSE_KEY)),
        ENTRANCE(RR_KAK_WINDMILL_LOWER,       logic->CanOpenOverworldDoor(RG_WINDMILL_KEY)),
        ENTRANCE(RR_KAK_BAZAAR,               logic->IsAdult && logic->AtDay && logic->CanOpenOverworldDoor(RG_KAK_BAZAAR_KEY)),
        ENTRANCE(RR_KAK_SHOOTING_GALLERY,     logic->IsAdult && logic->AtDay && logic->CanOpenOverworldDoor(RG_KAK_SHOOTING_GALLERY_KEY)),
        ENTRANCE(RR_KAK_WELL,                 logic->IsAdult || logic->Get(LOGIC_DRAIN_WELL) || logic->CanUse(RG_IRON_BOOTS) || (ctx->GetTrickOption(RT_BOTTOM_OF_THE_WELL_NAVI_DIVE) && logic->IsChild && logic->HasItem(RG_BRONZE_SCALE) && logic->CanJumpslash())),
        ENTRANCE(RR_KAK_POTION_SHOP,          (logic->AtDay || logic->IsChild) && logic->CanOpenOverworldDoor(RG_KAK_POTION_SHOP_KEY)),
        ENTRANCE(RR_KAK_REDEAD_GROTTO,        logic->CanOpenBombGrotto()),
        ENTRANCE(RR_KAK_IMPAS_LEDGE,          (logic->IsChild && logic->AtDay && logic->HasItem(RG_POWER_BRACELET)) || (logic->IsAdult && ctx->GetTrickOption(RT_VISIBLE_COLLISION))),
        ENTRANCE(RR_KAK_WATCHTOWER,           logic->HasItem(RG_CLIMB) && (logic->IsAdult || logic->AtDay || logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_LONGSHOT) || (ctx->GetTrickOption(RT_KAK_TOWER_GS) && logic->CanJumpslashExceptHammer()))),
        ENTRANCE(RR_KAK_ROOFTOP,              logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) && logic->IsAdult)),
        ENTRANCE(RR_KAK_IMPAS_ROOFTOP,        logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_KAK_ROOFTOP_GS) && logic->CanUse(RG_HOVER_BOOTS))),
        ENTRANCE(RR_THE_GRAVEYARD,            true),
        ENTRANCE(RR_KAK_BEHIND_GATE,          logic->IsAdult || logic->Get(LOGIC_KAKARIKO_GATE_OPEN)),
        //adult can jump from the fence near the windmill to ledgegrab the fence near granny's shop. is in logic on N64
        ENTRANCE(RR_KAK_BACKYARD,             logic->IsAdult || (logic->AtDay && logic->HasItem(RG_POWER_BRACELET))),
        ENTRANCE(RR_KAK_BEHIND_POTION_SHOP,   logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) && logic->IsAdult)),
    });

    areaTable[RR_KAK_IMPAS_LEDGE] = Region("Kak Impas Ledge", SCENE_KAKARIKO_VILLAGE, {}, {}, {
        //Exits
        ENTRANCE(RR_KAK_IMPAS_HOUSE_BACK, true),
        ENTRANCE(RR_KAKARIKO_VILLAGE,     true),
    });

    areaTable[RR_KAK_IMPAS_ROOFTOP] = Region("Kak Impas Rooftop", SCENE_KAKARIKO_VILLAGE, {}, {
        //Locations
        LOCATION(RC_KAK_GS_ABOVE_IMPAS_HOUSE, logic->IsAdult && logic->CanGetNightTimeGS() && logic->CanKillEnemy(RE_GOLD_SKULLTULA)),
    }, {
        //Exits
        ENTRANCE(RR_KAK_IMPAS_LEDGE,  true),
        ENTRANCE(RR_KAKARIKO_VILLAGE, true),
    });

    areaTable[RR_KAK_WATCHTOWER] = Region("Kak Watchtower", SCENE_KAKARIKO_VILLAGE, {}, {
        //Locations
        //exists for when age change is in logic.
        LOCATION(RC_KAK_GS_WATCHTOWER, logic->IsChild && logic->CanUse(RG_DINS_FIRE) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        ENTRANCE(RR_KAKARIKO_VILLAGE, true),
        ENTRANCE(RR_KAK_ROOFTOP,      !!ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS)),
    });

    areaTable[RR_KAK_ROOFTOP] = Region("Kak Rooftop", SCENE_KAKARIKO_VILLAGE, {}, {
        //Locations
        LOCATION(RC_KAK_MAN_ON_ROOF, logic->HasItem(RG_SPEAK_HYLIAN)),
    }, {
        //Exits
        ENTRANCE(RR_KAK_BACKYARD,           true),
        ENTRANCE(RR_KAK_BEHIND_POTION_SHOP, logic->HasItem(RG_HOVER_BOOTS)),
        ENTRANCE(RR_KAKARIKO_VILLAGE,       true),
    });

    areaTable[RR_KAK_BACKYARD] = Region("Kak Backyard", SCENE_KAKARIKO_VILLAGE, {}, {
        //Locations
        //There's probably a trick to get these with rang from the main region
        LOCATION(RC_KAK_NEAR_MEDICINE_SHOP_POT_1, logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_KAK_NEAR_MEDICINE_SHOP_POT_2, logic->IsChild && logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_KAKARIKO_VILLAGE,        true),
        ENTRANCE(RR_KAK_OPEN_GROTTO,         true),
        ENTRANCE(RR_KAK_ODD_POTION_BUILDING, logic->IsAdult && logic->CanOpenOverworldDoor(RG_GRANNYS_POTION_SHOP_KEY)),
        ENTRANCE(RR_KAK_BEHIND_POTION_SHOP,  logic->HasItem(RG_CLIMB)),
    });

    areaTable[RR_KAK_BEHIND_POTION_SHOP] = Region("Kak Behind Potion Shop", SCENE_KAKARIKO_VILLAGE, {}, {}, {
        //Exits
        ENTRANCE(RR_KAK_BACKYARD,    true),
        ENTRANCE(RR_KAK_POTION_SHOP, logic->IsAdult && logic->AtDay && logic->CanOpenOverworldDoor(RG_KAK_POTION_SHOP_KEY)),
        //can ledgegrab fence to rooftop with hover boots, but that's more difficult than the unintuitive jump, so not including in default logic
        ENTRANCE(RR_KAK_ROOFTOP,     ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_MEGATON_HAMMER) && logic->IsAdult),
    });

    areaTable[RR_KAK_CARPENTER_BOSS_HOUSE] = Region("Kak Carpenter Boss House", SCENE_KAKARIKO_CENTER_GUEST_HOUSE, {
        //Events
        EVENT_ACCESS(LOGIC_WAKE_UP_ADULT_TALON, logic->IsAdult && logic->CanUse(RG_POCKET_EGG)),
    }, {}, {
        //Exits
        ENTRANCE(RR_KAKARIKO_VILLAGE, true),
    });

    areaTable[RR_KAK_HOUSE_OF_SKULLTULA] = Region("Kak House of Skulltula", SCENE_HOUSE_OF_SKULLTULA, {}, {
        //Locations
        LOCATION(RC_KAK_10_GOLD_SKULLTULA_REWARD,  logic->HasItem(RG_SPEAK_HYLIAN) && logic->GetGSCount() >= 10),
        LOCATION(RC_KAK_20_GOLD_SKULLTULA_REWARD,  logic->HasItem(RG_SPEAK_HYLIAN) && logic->GetGSCount() >= 20),
        LOCATION(RC_KAK_30_GOLD_SKULLTULA_REWARD,  logic->HasItem(RG_SPEAK_HYLIAN) && logic->GetGSCount() >= 30),
        LOCATION(RC_KAK_40_GOLD_SKULLTULA_REWARD,  logic->HasItem(RG_SPEAK_HYLIAN) && logic->GetGSCount() >= 40),
        LOCATION(RC_KAK_50_GOLD_SKULLTULA_REWARD,  logic->HasItem(RG_SPEAK_HYLIAN) && logic->GetGSCount() >= 50),
        LOCATION(RC_KAK_100_GOLD_SKULLTULA_REWARD, logic->HasItem(RG_SPEAK_HYLIAN) && logic->GetGSCount() >= 100),
    }, {
        //Exits
        ENTRANCE(RR_KAKARIKO_VILLAGE, true),
    });

    areaTable[RR_KAK_IMPAS_HOUSE] = Region("Kak Impas House", SCENE_IMPAS_HOUSE, {}, {
        //Locations
        LOCATION(RC_KAK_IMPAS_HOUSE_COW, logic->CanUse(RG_EPONAS_SONG)),
    }, {
        //Exits
        ENTRANCE(RR_KAKARIKO_VILLAGE, true),
    });

    areaTable[RR_KAK_IMPAS_HOUSE_BACK] = Region("Kak Impas House Back", SCENE_IMPAS_HOUSE, {}, {
        //Locations
        LOCATION(RC_KAK_IMPAS_HOUSE_FREESTANDING_POH, true),
        LOCATION(RC_KAK_IMPAS_HOUSE_COW,              logic->CanUse(RG_EPONAS_SONG)),
    }, {
        //Exits
        ENTRANCE(RR_KAK_IMPAS_LEDGE, true),
    });

    areaTable[RR_KAK_WINDMILL_LOWER] = Region("Kak Windmill Lower", SCENE_WINDMILL_AND_DAMPES_GRAVE, {
        //Events
        EVENT_ACCESS(LOGIC_DRAIN_WELL, logic->IsChild && logic->CanUse(RG_SONG_OF_STORMS)),
    }, {
        //Locations
        LOCATION(RC_KAK_WINDMILL_FREESTANDING_POH, logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_SONG_FROM_WINDMILL,            logic->IsAdult && logic->HasItem(RG_FAIRY_OCARINA)),
    }, {
        //Exits
        ENTRANCE(RR_KAKARIKO_VILLAGE,   true),
        ENTRANCE(RR_KAK_WINDMILL_UPPER, (logic->IsAdult && (ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) || logic->CanGroundJump())) || (logic->IsChild && logic->CanJumpslash() && ctx->GetTrickOption(RT_KAK_CHILD_WINDMILL_POH))),
    });

    areaTable[RR_KAK_WINDMILL_UPPER] = Region("Kak Windmill Upper", SCENE_WINDMILL_AND_DAMPES_GRAVE, {}, {
        //Locations
        LOCATION(RC_KAK_WINDMILL_FREESTANDING_POH, true),
    }, {
        //Exits
        ENTRANCE(RR_KAK_WINDMILL_LOWER, true),
    });

    areaTable[RR_KAK_BAZAAR] = Region("Kak Bazaar", SCENE_BAZAAR, {}, {
        //Locations
        LOCATION(RC_KAK_BAZAAR_ITEM_1, logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_BAZAAR_ITEM_2, logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_BAZAAR_ITEM_3, logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_BAZAAR_ITEM_4, logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_BAZAAR_ITEM_5, logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_BAZAAR_ITEM_6, logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_BAZAAR_ITEM_7, logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_BAZAAR_ITEM_8, logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
    }, {
        //Exits
        ENTRANCE(RR_KAKARIKO_VILLAGE, true),
    });

    areaTable[RR_KAK_SHOOTING_GALLERY] = Region("Kak Shooting Gallery", SCENE_SHOOTING_GALLERY, {}, {
        //Locations
        LOCATION(RC_KAK_SHOOTING_GALLERY_REWARD, logic->IsAdult && logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_SPEAK_HYLIAN) && logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Exits
        ENTRANCE(RR_KAKARIKO_VILLAGE, true),
    });

    areaTable[RR_KAK_POTION_SHOP] = Region("Kak Potion Shop", SCENE_POTION_SHOP_KAKARIKO, {}, {
        //Locations
        LOCATION(RC_KAK_POTION_SHOP_ITEM_1, logic->IsAdult && logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_POTION_SHOP_ITEM_2, logic->IsAdult && logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_POTION_SHOP_ITEM_3, logic->IsAdult && logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_POTION_SHOP_ITEM_4, logic->IsAdult && logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_POTION_SHOP_ITEM_5, logic->IsAdult && logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_POTION_SHOP_ITEM_6, logic->IsAdult && logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_POTION_SHOP_ITEM_7, logic->IsAdult && logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KAK_POTION_SHOP_ITEM_8, logic->IsAdult && logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
    }, {
        //Exits
        ENTRANCE(RR_KAKARIKO_VILLAGE,     true),
        ENTRANCE(RR_KAK_BEHIND_POTION_SHOP, logic->IsAdult),
    });

    areaTable[RR_KAK_ODD_POTION_BUILDING] = Region("Kak Granny's Potion Shop", SCENE_POTION_SHOP_GRANNY, {
        //Events
        // RANDOTODO blue pot access
    }, {
        //Locations
        LOCATION(RC_KAK_TRADE_ODD_MUSHROOM, logic->IsAdult && logic->CanUse(RG_ODD_MUSHROOM)),
        LOCATION(RC_KAK_GRANNYS_SHOP,       logic->IsAdult && logic->HasItem(RG_SPEAK_HYLIAN) && (logic->CanUse(RG_ODD_MUSHROOM) || logic->TradeQuestStep(RG_ODD_MUSHROOM))),
    }, {
        // Exits
        ENTRANCE(RR_KAK_BACKYARD, true),
    });

    areaTable[RR_KAK_REDEAD_GROTTO] = Region("Kak Redead Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_KAK_REDEAD_GROTTO_CHEST, logic->CanKillEnemy(RE_REDEAD, ED_CLOSE, true, 2) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_KAKARIKO_VILLAGE, true),
    });

    areaTable[RR_KAK_OPEN_GROTTO] = Region("Kak Open Grotto", SCENE_GROTTOS, grottoEvents, {
        //Locations
        LOCATION(RC_KAK_OPEN_GROTTO_CHEST,                  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_KAK_OPEN_GROTTO_FISH,                   logic->HasBottle()),
        LOCATION(RC_KAK_OPEN_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_KAK_OPEN_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_KAK_OPEN_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_KAK_OPEN_GROTTO_BEEHIVE_LEFT,           logic->CanBreakLowerBeehives()),
        LOCATION(RC_KAK_OPEN_GROTTO_BEEHIVE_RIGHT,          logic->CanBreakLowerBeehives()),
        LOCATION(RC_KAK_OPEN_GROTTO_GRASS_1,                logic->CanCutShrubs()),
        LOCATION(RC_KAK_OPEN_GROTTO_GRASS_2,                logic->CanCutShrubs()),
        LOCATION(RC_KAK_OPEN_GROTTO_GRASS_3,                logic->CanCutShrubs()),
        LOCATION(RC_KAK_OPEN_GROTTO_GRASS_4,                logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_KAK_BACKYARD, true),
    });

    areaTable[RR_KAK_BEHIND_GATE] = Region("Kak Behind Gate", SCENE_KAKARIKO_VILLAGE, {}, {}, {
        //Exits
        ENTRANCE(RR_KAKARIKO_VILLAGE,     logic->IsAdult || ctx->GetTrickOption(RT_VISIBLE_COLLISION) || logic->Get(LOGIC_KAKARIKO_GATE_OPEN)),
        ENTRANCE(RR_DEATH_MOUNTAIN_TRAIL, true),
    });

    areaTable[RR_KAK_WELL] = Region("Kak Well", SCENE_KAKARIKO_VILLAGE, {}, {}, {
        //Exits
        ENTRANCE(RR_KAKARIKO_VILLAGE, logic->HasItem(RG_CLIMB) && (logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE) || logic->Get(LOGIC_DRAIN_WELL))),
        ENTRANCE(RR_BOTW_ENTRYWAY,    logic->IsChild || (logic->Get(LOGIC_DRAIN_WELL) && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))),
    });

    // clang-format on
}
