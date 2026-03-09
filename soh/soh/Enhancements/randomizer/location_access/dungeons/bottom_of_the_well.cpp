#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_BottomOfTheWell() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_BOTW_ENTRYWAY] = Region("Bottom of the Well Entryway", SCENE_BOTTOM_OF_THE_WELL, {}, {}, {
        //Exits
        //Technically involves an fake wall, but passing it lensless is intended in vanilla and it is well telegraphed
        //Backshot should be implemented here, or new regions should be added
        ENTRANCE(RR_BOTW_CORRIDOR,     ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla() && logic->CanUse(RG_CRAWL)),
        ENTRANCE(RR_BOTW_MQ_PERIMETER, ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsMQ()      && logic->CanUse(RG_CRAWL)),
        ENTRANCE(RR_KAK_WELL,          true),
    });

#pragma region Vanilla

    areaTable[RR_BOTW_CORRIDOR] = Region("Bottom of the Well Corridor", SCENE_BOTTOM_OF_THE_WELL, {}, {}, {
        //Exits
        ENTRANCE(RR_BOTW_ENTRYWAY,  logic->CanUse(RG_CRAWL) && logic->CanClimbLadder()),
        ENTRANCE(RR_BOTW_PERIMETER, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
    });

    areaTable[RR_BOTW_PERIMETER] = Region("Bottom of the Well Perimeter", SCENE_BOTTOM_OF_THE_WELL, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS,       logic->CanBreakPots()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,         logic->CanBreakPots()),
        EVENT_ACCESS(LOGIC_BOTW_LOWERED_WATER, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST,   ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST, ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST,  logic->HasExplosives() && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST,     logic->HasExplosives() && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST,       (logic->Get(LOGIC_BOTW_LOWERED_WATER) && logic->HasItem(RG_OPEN_CHEST)) || logic->CanOpenUnderwaterChest()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST,        (logic->Get(LOGIC_BOTW_LOWERED_WATER) && logic->HasItem(RG_OPEN_CHEST)) || logic->CanOpenUnderwaterChest()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_NEAR_ENTRANCE_POT_1,          logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_NEAR_ENTRANCE_POT_2,          logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_UNDERWATER_POT,               (logic->CanBreakPots() && logic->Get(LOGIC_BOTW_LOWERED_WATER)) || logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_CORRIDOR,          logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        ENTRANCE(RR_BOTW_MIDDLE,            ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_BOTW_PIT_CAGE,          ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_BOTW_HIDDEN_POTS,       ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_BOTW_CORNER_CRAWLSPACE, logic->CanUse(RG_CRAWL)),
        //Climb always needed in case water is lowered out of logic
        ENTRANCE(RR_BOTW_BEHIND_MOAT,           ((logic->Get(LOGIC_BOTW_LOWERED_WATER) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))) || logic->HasItem(RG_BRONZE_SCALE) || 
                                                (logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT)))),
        ENTRANCE(RR_BOTW_NEAR_BOSS_LOWER,   logic->Get(LOGIC_BOTW_LOWERED_WATER) && logic->CanUse(RG_CRAWL)),
        //Falling down into basement requires nothing, but falling down somewhere specific requires lens or lens trick
        //kinda questionable given several drops are blocked by rocks, but that's how it was handled before and on N64
        ENTRANCE(RR_BOTW_B3_OOZE,           true),
        ENTRANCE(RR_BOTW_B3_BLOCKED_GRASS,  ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
    });

    //This region combines the Middle with the perimeter's hidden areas. If a warp puts link into the middle without crossing the perimeter or using lens, it will need it's own region
    areaTable[RR_BOTW_MIDDLE] = Region("Bottom of the Well Middle", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        //You can just barely pass the spider on the right side without damage or items, but it's probably tight enough to count as as a trick
        LOCATION(RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST, (logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->TakeDamage()) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_PERIMETER,       ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_BOTW_PIT_CAGE,        ctx->GetTrickOption(RT_BOTW_PITS) && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))),
        ENTRANCE(RR_BOTW_SKULL_WALL_ROOM, logic->SmallKeys(SCENE_BOTTOM_OF_THE_WELL, 3)),
        ENTRANCE(RR_BOTW_INVISIBLE_PATH,  logic->SmallKeys(SCENE_BOTTOM_OF_THE_WELL, 3)),
        ENTRANCE(RR_BOTW_B3_OOZE,         true),
        ENTRANCE(RR_BOTW_B3_PLATFORM,     ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
    });

    areaTable[RR_BOTW_HIDDEN_POTS] = Region("Bottom of the Well Hidden Pots", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_LEFT_SIDE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_LEFT_SIDE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_LEFT_SIDE_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_PERIMETER, ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
    });

    areaTable[RR_BOTW_CORNER_CRAWLSPACE] = Region("Bottom of the Well Corner Crawlspace", SCENE_BOTTOM_OF_THE_WELL, {}, {}, {
        //Exits
        ENTRANCE(RR_BOTW_PERIMETER,        logic->CanUse(RG_CRAWL)),
        ENTRANCE(RR_BOTW_HIDDEN_PITS_ROOM, logic->SmallKeys(SCENE_BOTTOM_OF_THE_WELL, 3)),
    });

    //Passing through this area needs lens, but entering doesn't, so that the fire keese can be killed without crossing the pits if enemy drops are ever shuffled
    areaTable[RR_BOTW_HIDDEN_PITS_ROOM] = Region("Bottom of the Well Hidden Pits Room", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST, (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_POT_1, logic->CanBreakPots() && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_CORNER_CRAWLSPACE, logic->SmallKeys(SCENE_BOTTOM_OF_THE_WELL, 3) && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))),
        ENTRANCE(RR_BOTW_LOCKED_CAGE,       ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        //not sure if this lens check is needed, these holes are a bit too easy to find, but it matches existing logic
        ENTRANCE(RR_BOTW_B3_BOMB_FLOWERS,   ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
    });

    areaTable[RR_BOTW_LOCKED_CAGE] = Region("Bottom of the Well Locked Cage", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST,   logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_HIDDEN_PITS_ROOM, true),
    });

    areaTable[RR_BOTW_PIT_CAGE] = Region("Bottom of the Well Pit Cage", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_PERIMETER, ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_BOTW_MIDDLE,    ctx->GetTrickOption(RT_BOTW_PITS) && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))),
        ENTRANCE(RR_BOTW_B3_OOZE,   true),
    });

    areaTable[RR_BOTW_SKULL_WALL_ROOM] = Region("Bottom of the Well SKull Wall Room", SCENE_BOTTOM_OF_THE_WELL, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_MIDDLE, logic->SmallKeys(SCENE_BOTTOM_OF_THE_WELL, 3)),
    });

    areaTable[RR_BOTW_INVISIBLE_PATH] = Region("Bottom of the Well Invisible Path", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_MIDDLE,  logic->SmallKeys(SCENE_BOTTOM_OF_THE_WELL, 3)),
        ENTRANCE(RR_BOTW_B3_OOZE, true),
    });

    areaTable[RR_BOTW_BEHIND_MOAT] = Region("Bottom of the Well Behind Moat", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST, (logic->Get(LOGIC_BOTW_LOWERED_WATER)  && logic->HasItem(RG_OPEN_CHEST)) || logic->CanOpenUnderwaterChest()),
    }, {
        //Exits
        //Climb always needed in case water is lowered out of logic
        ENTRANCE(RR_BOTW_PERIMETER, (logic->Get(LOGIC_BOTW_LOWERED_WATER) || logic->HasItem(RG_BRONZE_SCALE) || 
                                     (logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_BOTW_CRYPT,     true),
    });

    areaTable[RR_BOTW_CRYPT] = Region("Bottom of the Well Crypt", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY,               logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_COFFIN_ROOM_FRONT_LEFT_HEART,   true),
        LOCATION(RC_BOTTOM_OF_THE_WELL_COFFIN_ROOM_MIDDLE_RIGHT_HEART, logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_BEHIND_MOAT, true),
    });

    areaTable[RR_BOTW_NEAR_BOSS_LOWER] = Region("Bottom of the Well Near Boss Lower", SCENE_BOTTOM_OF_THE_WELL, {}, {}, {
        //Exits
        //Climb always needed in case the water is lowered out of logic
        //Adult can ground jump out of the pit without climb but needs a way through the crawlspace
        ENTRANCE(RR_BOTW_PERIMETER,       logic->CanUse(RG_CRAWL) && (logic->Get(LOGIC_BOTW_LOWERED_WATER) || logic->HasItem(RG_BRONZE_SCALE)) && logic->HasItem(RG_CLIMB)),
        ENTRANCE(RR_BOTW_NEAR_BOSS_UPPER,  logic->HasItem(RG_CLIMB) || (logic->IsAdult && logic->CanGroundJump())),
    });

    areaTable[RR_BOTW_NEAR_BOSS_UPPER] = Region("Bottom of the Well Near Boss Upper", SCENE_BOTTOM_OF_THE_WELL, {}, {}, {
        //Exits
        ENTRANCE(RR_BOTW_NEAR_BOSS_LOWER, true),
        ENTRANCE(RR_BOTW_DEAD_HAND_ROOM,  true),
    });

    areaTable[RR_BOTW_DEAD_HAND_ROOM] = Region("Bottom of the Well Dead Hand Room", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST, logic->CanKillEnemy(RE_DEAD_HAND) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST,     (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_NEAR_BOSS_UPPER, logic->CanKillEnemy(RE_DEAD_HAND)),
    });

    areaTable[RR_BOTW_B3_OOZE] = Region("Bottom of the Well B3 Ooze", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_1,     logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_2,     logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_3,     logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_4,     logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_5,     logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_6,     logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_7,     logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_8,     logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_9,     logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_10,    logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_11,    logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_12,    logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_SUN_FAIRY, logic->CanUse(RG_SUNS_SONG)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_GRASS_1,   logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_GRASS_2,   logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_GRASS_3,   logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_HIDDEN_POTS,      logic->CanClimbHighLadder()),
        //It's possible to abuse boulder's limited range of collision detection to detonate the flowers through the boulder with bow, but this is a glitch
        //the exact range is just past the furthest away plank in the green goo section
        ENTRANCE(RR_BOTW_B3_BOMB_FLOWERS,  AnyAgeTime([]{return logic->BlastOrSmash() || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_BOTW_BASEMENT) && logic->CanUse(RG_STICKS)) || (ctx->GetTrickOption(RT_DISTANT_BOULDER_COLLISION) && logic->CanUse(RG_FAIRY_BOW));})),
        ENTRANCE(RR_BOTW_B3_BLOCKED_GRASS, AnyAgeTime([]{return logic->BlastOrSmash();})),
        ENTRANCE(RR_BOTW_B3_CHEST_AREA,    AnyAgeTime([]{return logic->BlastOrSmash();})),
    });

    areaTable[RR_BOTW_B3_BOMB_FLOWERS] = Region("Bottom of the Well B3 Bomb Flowers", SCENE_BOTTOM_OF_THE_WELL, {}, {}, {
        //Exits
        ENTRANCE(RR_BOTW_B3_OOZE,          logic->CanDetonateUprightBombFlower()),
        ENTRANCE(RR_BOTW_B3_BLOCKED_GRASS, logic->HasItem(RG_GORONS_BRACELET)),
        ENTRANCE(RR_BOTW_B3_CHEST_AREA,    logic->HasItem(RG_GORONS_BRACELET)),
    });

    areaTable[RR_BOTW_B3_BLOCKED_GRASS] = Region("Bottom of the Well B3 Blocked Grass", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_4, logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_5, logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_6, logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_7, logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_8, logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_9, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_B3_OOZE, AnyAgeTime([]{return logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_BOTW_B3_CHEST_AREA] = Region("Bottom of the Well B3 Chest Area", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_MAP_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_B3_OOZE, AnyAgeTime([]{return logic->BlastOrSmash();})),
    });

    areaTable[RR_BOTW_B3_PLATFORM] = Region("Bottom of the Well B3 Platform", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_PLATFORM_LEFT_RUPEE,       true),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_PLATFORM_BACK_LEFT_RUPEE,  true),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_PLATFORM_MIDDLE_RUPEE,     true),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_PLATFORM_BACK_RIGHT_RUPEE, true),
        LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_PLATFORM_RIGHT_RUPEE,      true),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_B3_OOZE, true),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_BOTW_MQ_PERIMETER] = Region("Bottom of the Well MQ Perimeter", SCENE_BOTTOM_OF_THE_WELL, {
        //Events
        // Fairies are in slingshot wonder item, & pot behind grate. Pot can also be broken with boomerang trick
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS,         (logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT)) || ((AnyAgeTime([]{return logic->BlastOrSmash();}) || ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION)) && logic->CanHitEyeTargets())),
        //It is possible to hit the water switch with a pot from RR_BOTW_MQ_MIDDLE, however the hitbox for making it activate is very unintuitive
        //You have to throw the pot from further back to hit the switch from the front instead of the top, trying to hit the "fingers" directly
        //This unintuitiveness means it should be a trick. ZL is needed to get a clear path to carry the pot
        EVENT_ACCESS(LOGIC_BOTW_LOWERED_WATER,   logic->CanHitSwitch(ED_SHORT_JUMPSLASH)),
        EVENT_ACCESS(LOGIC_BOTW_MQ_OPENED_GATES, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Locations
        //Implies CanBreakPots(). Hitting this with rang through the wall is possible but would be a trick.
        //Instead of blowing up the boulder, you can aim through the lower left side with sling(either age) or as child with bow
        //Not even bow extension seems to get adult's bow to work
        //this would be a trick
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_OUTER_LOBBY_POT,  AnyAgeTime([]{return logic->BlastOrSmash();}) && logic->CanHitEyeTargets()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_BOMB_LEFT_HEART,  logic->HasExplosives()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_BOMB_RIGHT_HEART, logic->HasExplosives()),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_ENTRYWAY,             logic->CanUse(RG_CRAWL) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_BOTW_MQ_MIDDLE,            logic->Get(LOGIC_BOTW_MQ_OPENED_GATES)),
        ENTRANCE(RR_BOTW_MQ_PIT_CAGE,          AnyAgeTime([]{return logic->BlastOrSmash();}) && logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        //Climb always needed in case water is lowered out of logic
        ENTRANCE(RR_BOTW_MQ_BEHIND_MOAT,       (logic->Get(LOGIC_BOTW_LOWERED_WATER) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))) || logic->HasItem(RG_BRONZE_SCALE) || 
                                                (logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_BOTW_MQ_CORNER_CRAWLSPACE, logic->CanUse(RG_CRAWL)),
        ENTRANCE(RR_BOTW_MQ_NEAR_BOSS_LOWER,   logic->CanUse(RG_CRAWL) && logic->Get(LOGIC_BOTW_LOWERED_WATER)),
        ENTRANCE(RR_BOTW_MQ_B3,                true),
    });

    areaTable[RR_BOTW_MQ_MIDDLE] = Region("Bottom of the Well MQ Middle", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST,         logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_INNER_LOBBY_POT_1, logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_INNER_LOBBY_POT_2, logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_INNER_LOBBY_POT_3, logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_CELL_SUN_FAIRY,    logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_MQ_PERIMETER,      logic->Get(LOGIC_BOTW_MQ_OPENED_GATES)),
        ENTRANCE(RR_BOTW_MQ_PIT_CAGE,       (bool)ctx->GetTrickOption(RT_BOTW_PITS)),
        ENTRANCE(RR_BOTW_MQ_B3_PLATFORM,    logic->Get(LOGIC_BOTW_MQ_OPENED_MIDDLE_HOLE)),
        ENTRANCE(RR_BOTW_MQ_B3,             true),
        ENTRANCE(RR_BOTW_MQ_INVISIBLE_PATH, AnyAgeTime([]{return logic->HasItem(RG_POWER_BRACELET) || logic->CanHitSwitch(ED_BOMB_THROW);})),
        ENTRANCE(RR_BOTW_MQ_GRAVE_ROOM,     logic->Get(LOGIC_BOTW_MQ_OPENED_WEST_ROOM)),
    });

    areaTable[RR_BOTW_MQ_INVISIBLE_PATH] = Region("Bottom of the Well Invisible Path", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        //This location technically involves an invisible platform, but it's intended to do lensless in vanilla and is clearly signposted by pots.
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY, true),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_POT_1, logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_POT_2, logic->CanBreakPots()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_MQ_MIDDLE, true),
        ENTRANCE(RR_BOTW_MQ_B3,     true),
    });

    areaTable[RR_BOTW_MQ_GRAVE_ROOM] = Region("Bottom of the Well Grave Room", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        //The enemies in this room are invisible and crowd around the player, being awkward to deal with blind unless you already know how.
        //the right wall is safe, and can be followed to get behind the grave which you can then pull easily assuming you can tank invisible keese
        //Using a deku nut however stuns everything easily. and if you have a melee weapon you can kill the skull through the grave then grab the drop
        //though it can be hard to tell where the safe direct path to the grave is without lens.
        //Also you get cheap shotted on entry sometimes.
        //An MQ lens trick is recommended here, and a review of this room for OHKO logic when that is added is advised.
        //In the meantime I assume damage taken or the easy answer (nuts)

        //Without power bracelet, the skull can be killed through the Grave with any valid weapon.
        //Bow, Sling and Hammer should aim for the left side (from the door's perspective) ot eh grave, hookshot should get close to the grave and aim inside it (no extension needed).
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM, (logic->TakeDamage() || logic->CanUse(RG_NUTS)) && 
                                                              (logic->HasItem(RG_POWER_BRACELET) || ctx->GetTrickOption(RT_VISIBLE_COLLISION)) &&
                                                              logic->CanKillEnemy(RE_GOLD_SKULLTULA)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_MQ_MIDDLE, true),
    });

    areaTable[RR_BOTW_MQ_PIT_CAGE] = Region("Bottom of the Well MQ Pit Cage", SCENE_BOTTOM_OF_THE_WELL, {
        //Events
        EVENT_ACCESS(LOGIC_BOTW_MQ_OPENED_WEST_ROOM, true),
    }, {}, {
        //Exits
        ENTRANCE(RR_BOTW_MQ_PERIMETER, logic->BlastOrSmash() && (logic->CanPassEnemy(RE_BIG_SKULLTULA) || ctx->GetTrickOption(RT_BOTW_PITS))),
        ENTRANCE(RR_BOTW_MQ_MIDDLE,    (bool)ctx->GetTrickOption(RT_BOTW_PITS)),
        ENTRANCE(RR_BOTW_MQ_B3,   true),
    });

    areaTable[RR_BOTW_MQ_BEHIND_MOAT] = Region("Bottom of the Well MQ Behind Moat", SCENE_BOTTOM_OF_THE_WELL, {}, {}, {
        //Exits
        //Climb always needed in case water is lowered out of logic
        ENTRANCE(RR_BOTW_MQ_PERIMETER, ((logic->Get(LOGIC_BOTW_LOWERED_WATER) || logic->HasItem(RG_BRONZE_SCALE)) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))) || 
                                        (logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_BOTW_MQ_CRYPT,     logic->SmallKeys(SCENE_BOTTOM_OF_THE_WELL, 2)),
    });

    areaTable[RR_BOTW_MQ_CRYPT] = Region("Bottom of the Well MQ Crypt", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM,                 logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_COFFIN_ROOM_FRONT_RIGHT_HEART,  logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_COFFIN_ROOM_MIDDLE_LEFT_HEART,  logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_MQ_BEHIND_MOAT, logic->SmallKeys(SCENE_BOTTOM_OF_THE_WELL, 2)),
    });

    areaTable[RR_BOTW_MQ_CORNER_CRAWLSPACE] = Region("Bottom of the Well MQ Northeast Crawlspace", SCENE_BOTTOM_OF_THE_WELL, {}, {}, {
        ENTRANCE(RR_BOTW_MQ_PERIMETER,        logic->CanUse(RG_CRAWL)),
        ENTRANCE(RR_BOTW_MQ_FLOORMASTER_ROOM, logic->CanUseProjectile()),
    });

    areaTable[RR_BOTW_MQ_FLOORMASTER_ROOM] = Region("Bottom of the Well MQ Floormaster Room", SCENE_BOTTOM_OF_THE_WELL, {}, {}, {
        ENTRANCE(RR_BOTW_MQ_CORNER_CRAWLSPACE, true),
        ENTRANCE(RR_BOTW_MQ_LOCKED_CAGE,       logic->SmallKeys(SCENE_BOTTOM_OF_THE_WELL, 2)),
    });

    areaTable[RR_BOTW_MQ_LOCKED_CAGE] = Region("Bottom of the Well MQ Locked Cage", SCENE_BOTTOM_OF_THE_WELL, {
        //Events
        EVENT_ACCESS(LOGIC_BOTW_MQ_OPENED_MIDDLE_HOLE, logic->HasExplosives()),
    }, {}, {
        //Exits
        ENTRANCE(RR_BOTW_MQ_FLOORMASTER_ROOM, logic->CanUse(RG_CRAWL) && logic->SmallKeys(SCENE_BOTTOM_OF_THE_WELL, 2)),
    });

    areaTable[RR_BOTW_MQ_NEAR_BOSS_LOWER] = Region("Bottom of the Well MQ Near Boss Lower", SCENE_BOTTOM_OF_THE_WELL, {}, {}, {
        //Exits
        //Climb always needed in case the water is lowered out of logic
        //Adult can ground jump out of the pit without climb but needs a way through the crawlspace
        ENTRANCE(RR_BOTW_MQ_PERIMETER,       logic->CanUse(RG_CRAWL) && (logic->Get(LOGIC_BOTW_LOWERED_WATER) || logic->HasItem(RG_BRONZE_SCALE)) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_BOTW_MQ_NEAR_BOSS_UPPER, logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_BOTW_MQ_NEAR_BOSS_UPPER] = Region("Bottom of the Well MQ Near Boss Upper", SCENE_BOTTOM_OF_THE_WELL, {}, {}, {
        //Exits
        ENTRANCE(RR_BOTW_MQ_NEAR_BOSS_LOWER, true),
        ENTRANCE(RR_BOTW_MQ_DEAD_HAND_ROOM,  true),
    });

    areaTable[RR_BOTW_MQ_DEAD_HAND_ROOM] = Region("Bottom of the Well MQ Dead Hand Room", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST,              logic->CanKillEnemy(RE_DEAD_HAND) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY, logic->HasExplosives() || (ctx->GetTrickOption(RT_BOTW_MQ_DEADHAND_KEY) && logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_GRASS_1,          logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_GRASS_2,          logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_GRASS_3,          logic->CanCutShrubs()),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_GRASS_4,          logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_MQ_NEAR_BOSS_UPPER, logic->CanKillEnemy(RE_DEAD_HAND)),
    });

    areaTable[RR_BOTW_MQ_B3] = Region("Bottom of the Well MQ B3", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        //behind invisible big skulltulas, but with navi spotting it's easy to avoid them, or at worst, tank your way through as they do not block the path
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT,                  logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_BASEMENT_HALLWAY_FRONT_HEART, true),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_BASEMENT_HALLWAY_LEFT_HEART,  true),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_BASEMENT_HALLWAY_RIGHT_HEART, true),
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_BASEMENT_SUN_FAIRY,           logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_MQ_PERIMETER, logic->CanClimbHighLadder()),
    });

    areaTable[RR_BOTW_MQ_B3_PLATFORM] = Region("Bottom of the Well MQ B3 Platform", SCENE_BOTTOM_OF_THE_WELL, {}, {
        //Locations
        //Assumes RR_BOTW_MQ_B3 access
        LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST, logic->CanPassEnemy(RE_REDEAD) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_BOTW_MQ_B3, true),
    });

#pragma endregion
    // clang-format on
}
