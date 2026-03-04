#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_DekuTree() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_DEKU_TREE_ENTRYWAY] = Region("Deku Tree Entryway", SCENE_DEKU_TREE, {}, {}, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_LOBBY,      ctx->GetDungeon(DEKU_TREE)->IsVanilla()),
        ENTRANCE(RR_DEKU_TREE_MQ_1F,      ctx->GetDungeon(DEKU_TREE)->IsMQ()),
        ENTRANCE(RR_KF_OUTSIDE_DEKU_TREE, true),
    });

#pragma region Vanilla

    areaTable[RR_DEKU_TREE_LOBBY] = Region("Deku Tree Lobby", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS,           logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,             logic->CanGetDekuBabaNuts()),
        EVENT_ACCESS(LOGIC_DEKU_TREE_1F_BROKE_WEB, logic->HasFireSource()),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_LOBBY_LOWER_HEART, logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_LOBBY_GRASS_1,     logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_LOBBY_GRASS_2,     logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_LOBBY_GRASS_3,     logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_ENTRYWAY,          true),
        ENTRANCE(RR_DEKU_TREE_LOBBY_2F,          logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_DEKU_TREE_BASEMENT_LOWER,    logic->Get(LOGIC_DEKU_TREE_1F_BROKE_WEB)),
        ENTRANCE(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, false),
        ENTRANCE(RR_DEKU_TREE_BOSS_ENTRYWAY,     false),
    });

    areaTable[RR_DEKU_TREE_LOBBY_2F] = Region("Deku Tree Lobby 2F", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_MAP_CHEST,         logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_LOBBY_LOWER_HEART, true),
        LOCATION(RC_DEKU_TREE_2F_GRASS_1,        logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_2F_GRASS_2,        logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_LOBBY,          true),
        ENTRANCE(RR_DEKU_TREE_LOBBY_3F,       logic->HasItem(RG_CLIMB) || logic->CanUse(RG_LONGSHOT)), // precise hookshot atop chest can make it as adult
        ENTRANCE(RR_DEKU_TREE_2F_MIDDLE_ROOM, true),
    });

    areaTable[RR_DEKU_TREE_LOBBY_3F] = Region("Deku Tree Lobby 3F", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_DEKU_TREE_1F_BROKE_WEB, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_LOBBY_UPPER_HEART, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_LOBBY_2F,     true),
        ENTRANCE(RR_DEKU_TREE_COMPASS_ROOM, true),
    });

    areaTable[RR_DEKU_TREE_2F_MIDDLE_ROOM] = Region("Deku Tree 2F Middle Room", SCENE_DEKU_TREE, {}, {}, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_LOBBY,          AnyAgeTime([]{return logic->CanReflectNuts() || logic->CanUse(RG_MEGATON_HAMMER);})),
        ENTRANCE(RR_DEKU_TREE_SLINGSHOT_ROOM, AnyAgeTime([]{return logic->CanReflectNuts() || logic->CanUse(RG_MEGATON_HAMMER);})),
    });

    areaTable[RR_DEKU_TREE_SLINGSHOT_ROOM] = Region("Deku Tree Slingshot Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_SLINGSHOT_CHEST,           logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST, (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_SLINGSHOT_GRASS_1,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_SLINGSHOT_GRASS_2,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_SLINGSHOT_GRASS_3,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_SLINGSHOT_GRASS_4,         logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_2F_MIDDLE_ROOM, true),
    });

    areaTable[RR_DEKU_TREE_COMPASS_ROOM] = Region("Deku Tree Compass Room", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_COMPASS_CHEST,           logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_GS_COMPASS_ROOM,         logic->CanAttack()),
        LOCATION(RC_DEKU_TREE_COMPASS_GRASS_1,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_COMPASS_GRASS_2,         logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_LOBBY,         logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)),
        ENTRANCE(RR_DEKU_TREE_BOSS_ENTRYWAY, false),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_LOWER] = Region("Deku Tree Basement Lower", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS,           logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,             logic->CanGetDekuBabaNuts()),
        EVENT_ACCESS(LOGIC_DEKU_TREE_B1_BROKE_WEB, (logic->CanUse(RG_STICKS) && (logic->Get(LOGIC_DEKU_TREE_PUSHED_BASEMENT_BLOCK) || logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS))) ||
                                                   (ctx->GetTrickOption(RT_DEKU_B1_BOW_WEBS) && logic->IsAdult && logic->CanUse(RG_FAIRY_BOW))),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_BASEMENT_CHEST,    logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_GS_BASEMENT_GATE,  logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH)),
        LOCATION(RC_DEKU_TREE_GS_BASEMENT_VINES, logic->CanKillEnemy(RE_GOLD_SKULLTULA, ctx->GetTrickOption(RT_DEKU_MQ_COMPASS_GS) ? ED_SHORT_JUMPSLASH : ED_BOMB_THROW) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_DEKU_TREE_BASEMENT_GRASS_1,  logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_GRASS_2,  logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_LOBBY,               logic->HasItem(RG_CLIMB) || (logic->IsAdult && logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, AnyAgeTime([]{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);})),
        ENTRANCE(RR_DEKU_TREE_BASEMENT_UPPER,      logic->IsAdult || ctx->GetTrickOption(RT_DEKU_B1_SKIP) || logic->CanGroundJump() || logic->Get(LOGIC_DEKU_TREE_PUSHED_BASEMENT_BLOCK)),
        ENTRANCE(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM,   false),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_SCRUB_ROOM] = Region("Deku Tree Basement Scrub Room", SCENE_DEKU_TREE, {}, {
        //Location
        LOCATION(RC_DEKU_TREE_BASEMENT_SCRUB_ROOM_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_SCRUB_ROOM_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_SCRUB_ROOM_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_SCRUB_ROOM_GRASS_4, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_BASEMENT_LOWER,            true),
        ENTRANCE(RR_DEKU_TREE_BASEMENT_WATER_ROOM_FRONT, AnyAgeTime([]{return logic->CanHitEyeTargets();})),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_WATER_ROOM_FRONT] = Region("Deku Tree Basement Water Room Front", SCENE_DEKU_TREE, {}, {}, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM,      true),
        ENTRANCE(RR_DEKU_TREE_BASEMENT_WATER_ROOM_BACK, logic->HasItem(RG_BRONZE_SCALE) || ctx->GetTrickOption(RT_DEKU_B1_BACKFLIP_OVER_SPIKED_LOG)),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_WATER_ROOM_BACK] = Region("Deku Tree Basement Water Room Back", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_BASEMENT_SPIKE_ROLLER_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_SPIKE_ROLLER_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_BASEMENT_WATER_ROOM_FRONT, logic->HasItem(RG_BRONZE_SCALE) || ctx->GetTrickOption(RT_DEKU_B1_BACKFLIP_OVER_SPIKED_LOG)),
        ENTRANCE(RR_DEKU_TREE_BASEMENT_TORCH_ROOM,       true),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_TORCH_ROOM] = Region("Deku Tree Basement Torch Room", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_BASEMENT_TORCHES_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_TORCHES_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_BASEMENT_WATER_ROOM_BACK, AnyAgeTime([]{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);})),
        ENTRANCE(RR_DEKU_TREE_BASEMENT_BACK_LOBBY,      AnyAgeTime([]{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);})),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_BACK_LOBBY] = Region("Deku Tree Basement Back Lobby", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Location
        LOCATION(RC_DEKU_TREE_BASEMENT_LARVAE_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_LARVAE_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_BASEMENT_TORCH_ROOM, true),
        ENTRANCE(RR_DEKU_TREE_BASEMENT_BACK_ROOM,  AnyAgeTime([]{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);}) && AnyAgeTime([]{return logic->BlastOrSmash();})),
        ENTRANCE(RR_DEKU_TREE_BASEMENT_UPPER,      AnyAgeTime([]{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);}) && logic->CanUse(RG_CRAWL)),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_BACK_ROOM] = Region("Deku Tree Basement Back Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, true),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_UPPER] = Region("Deku Tree Basement Upper", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS,                    logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,                      logic->CanGetDekuBabaNuts()),
        EVENT_ACCESS(LOGIC_DEKU_TREE_PUSHED_BASEMENT_BLOCK, logic->HasItem(RG_POWER_BRACELET)),
        EVENT_ACCESS(LOGIC_DEKU_TREE_B1_BROKE_WEB,          logic->HasFireSource()),
    }, {}, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_BASEMENT_LOWER,      true),
        ENTRANCE(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, logic->CanUse(RG_CRAWL)),
        //It's possible to light a stick on the upper torch with a backflip into a jumpslash, but a trick as it seems to involve landing on the torch platform
        //Adult can simply backflip against a nearby wall to the same effect, which is much simpler but still unintuitive enough top be a trick
        ENTRANCE(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM,   logic->Get(LOGIC_DEKU_TREE_B1_BROKE_WEB) && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
    });

    areaTable[RR_DEKU_TREE_OUTSIDE_BOSS_ROOM] = Region("Deku Tree Outside Boss Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_BEFORE_BOSS_LEFT_HEART,   logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_BEFORE_BOSS_MIDDLE_HEART, logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_BEFORE_BOSS_RIGHT_HEART,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_BEFORE_BOSS_GRASS_1,      logic->CanCutShrubs() && logic->HasFireSourceWithTorch()),
        LOCATION(RC_DEKU_TREE_BEFORE_BOSS_GRASS_2,      logic->CanCutShrubs() && logic->HasFireSourceWithTorch()),
        LOCATION(RC_DEKU_TREE_BEFORE_BOSS_GRASS_3,      logic->CanCutShrubs() && logic->HasFireSourceWithTorch()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_BASEMENT_UPPER, logic->HasItem(RG_CLIMB) && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_DEKU_TREE_BOSS_ENTRYWAY,  AnyAgeTime([]{return logic->CanReflectNuts();})),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_DEKU_TREE_MQ_1F] = Region("Deku Tree MQ 1F", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS, logic->CanKillEnemy(RE_WITHERED_DEKU_BABA)),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->HasFireSource()),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_HEART,   logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_GRASS_4, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_GRASS_5, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_ENTRYWAY,    true),
        //may need canAvoid logic with enemy shuffle
        ENTRANCE(RR_DEKU_TREE_MQ_2F,       logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)),
        //Swim is not required because you can jump with enough momentum to hit land. You even avoid fall damage if you hit the shallow water.
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT, logic->Get(LOGIC_DEKU_TREE_1F_BROKE_WEB)),
        //is it possible to recoil from here to the ledge with a trick?
    });

    areaTable[RR_DEKU_TREE_MQ_2F] = Region("Deku Tree MQ 2F", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_DEKU_TREE_MQ_2F_BURNED_WEB, logic->HasFireSource()),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_MAP_CHEST,     logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_MQ_GS_LOBBY,      logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_HEART,   true),
        LOCATION(RC_DEKU_TREE_MQ_2F_GRASS_1,    logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_2F_GRASS_2,    logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_CRATE,   logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_1F,              true),
        //Will need canAvoid logic with enemy shuffle
        ENTRANCE(RR_DEKU_TREE_MQ_3F,              logic->HasItem(RG_CLIMB) || logic->CanUse(RG_LONGSHOT)), // precise hookshot atop chest can make it as adult
        ENTRANCE(RR_DEKU_TREE_MQ_EYE_TARGET_ROOM, logic->Get(LOGIC_DEKU_TREE_MQ_2F_BURNED_WEB)),
    });

    areaTable[RR_DEKU_TREE_MQ_3F] = Region("Deku Tree MQ 3F", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS,               logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,                 logic->CanGetDekuBabaNuts()),
        EVENT_ACCESS(LOGIC_DEKU_TREE_1F_BROKE_WEB,     true),
        EVENT_ACCESS(LOGIC_DEKU_TREE_MQ_2F_BURNED_WEB, logic->CanUse(RG_STICKS) || logic->CanUse(RG_FAIRY_BOW)),
    }, {}, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_2F,             true),
        ENTRANCE(RR_DEKU_TREE_MQ_SLINGSHOT_ROOM, true),
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT,       true),
    });

    areaTable[RR_DEKU_TREE_MQ_SLINGSHOT_ROOM] = Region("Deku Tree MQ Slingshot Room", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        //Implies CanKillEnemy(RE_GOHMA_LARVA)
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_CHEST,           logic->CanKillEnemy(RE_DEKU_BABA) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST, (logic->HasFireSourceWithTorch() || (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW))) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_HEART,      true),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_GRASS_1,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_GRASS_2,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_GRASS_3,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_GRASS_4,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_CRATE_1,    logic->CanBreakCrates()),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_CRATE_2,    logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_3F, logic->CanKillEnemy(RE_DEKU_BABA)),
    });

    areaTable[RR_DEKU_TREE_MQ_EYE_TARGET_ROOM] = Region("Deku Tree MQ Eye Target Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_DEKU_BABA_HEART,        true),
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_COMPASS_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_COMPASS_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_COMPASS_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_COMPASS_GRASS_4, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_COMPASS_GRASS_5, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_COMPASS_GRASS_6, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_COMPASS_GRASS_7, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_COMPASS_ROOM, AnyAgeTime([]{return logic->CanHitEyeTargets();})),
        ENTRANCE(RR_DEKU_TREE_MQ_2F,           true),
    });

    areaTable[RR_DEKU_TREE_MQ_COMPASS_ROOM] = Region("Deku Tree MQ Compass Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_COMPASS_CHEST,   logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_MQ_COMPASS_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_COMPASS_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_COMPASS_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_COMPASS_GRASS_4, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_EYE_TARGET_ROOM,    true),
        ENTRANCE(RR_DEKU_TREE_MQ_PAST_BOULDER_VINES, (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME))) && AnyAgeTime([]{return logic->CanUse(RG_BOMBCHU_5) || (logic->CanUse(RG_BOMB_BAG) && (logic->CanUse(RG_SONG_OF_TIME) || logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS))) || (logic->CanUse(RG_MEGATON_HAMMER) && ((logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME)) || (ctx->GetTrickOption(RT_DEKU_MQ_COMPASS_GS) && logic->HasItem(RG_CLIMB))));})),
    });

    areaTable[RR_DEKU_TREE_MQ_PAST_BOULDER_VINES] = Region("Deku Tree MQ Past Boulder Vines", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_GS_PAST_BOULDER_VINES, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_MQ_COMPASS_ROOM_HEART,    true),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_COMPASS_ROOM, logic->BlastOrSmash()),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT] = Region("Deku Tree MQ Basement", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS,           logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,             logic->CanGetDekuBabaNuts()),
        EVENT_ACCESS(LOGIC_DEKU_TREE_B1_BROKE_WEB, logic->CanUse(RG_STICKS) && (logic->Get(LOGIC_DEKU_TREE_PUSHED_BASEMENT_BLOCK) || logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS))),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_CHEST,         (logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_LOWER_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_LOWER_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_LOWER_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_LOWER_GRASS_4, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_1F,                        logic->HasItem(RG_CLIMB) || logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM,   AnyAgeTime([]{return logic->CanHitEyeTargets();})),
        //includes RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM Access, other fire sources clear directly from there
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, AnyAgeTime([]{return logic->CanHitEyeTargets();}) && logic->Get(LOGIC_DEKU_TREE_MQ_CLEARED_SE_ROOM) && AnyAgeTime([]{return logic->CanUse(RG_STICKS);})),
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_LEDGE,            logic->IsAdult || ctx->GetTrickOption(RT_DEKU_B1_SKIP) || logic->CanGroundJump() || logic->Get(LOGIC_DEKU_TREE_PUSHED_BASEMENT_BLOCK) || logic->CanUse(RG_HOVER_BOOTS)),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM] = Region("Deku Tree MQ Southeast Room", SCENE_DEKU_TREE, {
        //Events
        //Implies CanKillEnemy(RE_GOHMA_LARVA)
        EVENT_ACCESS(LOGIC_DEKU_TREE_MQ_CLEARED_SE_ROOM, logic->CanKillEnemy(RE_MAD_SCRUB)),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_TORCHES_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_TORCHES_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_TORCHES_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_TORCHES_GRASS_4, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, logic->HasFireSource()),
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT,                  logic->Get(LOGIC_DEKU_TREE_MQ_CLEARED_SE_ROOM)),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT] = Region("Deku Tree MQ Basement Water Room Front", SCENE_DEKU_TREE, {
        //Events
        //It's possible to get this with bow if you have move while in first person and one-point skips on, noticeably harder and jankier as child, but that's a trick
        EVENT_ACCESS(LOGIC_DEKU_TREE_MQ_WATER_ROOM_TORCHES, logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_STICKS) && (ctx->GetTrickOption(RT_DEKU_MQ_LOG) || (logic->IsChild && logic->CanShield())))),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST,           logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_SPIKE_ROLLER_FRONT_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_SPIKE_ROLLER_FRONT_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_SPIKE_ROLLER_FRONT_GRASS_3, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, ctx->GetTrickOption(RT_DEKU_MQ_LOG) || (logic->IsChild && logic->CanShield()) || logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS))),
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM,  true),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK] = Region("Deku Tree MQ Basement Water Room Back", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS, logic->CanKillEnemy(RE_WITHERED_DEKU_BABA)),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->HasFireSource()),
    }, {
        //Locations
        //it blocks the chest while stunned unless you stun it from afar while it's slightly off the ground
        LOCATION(RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST,           logic->CanUse(RG_SONG_OF_TIME) && logic->CanPassEnemy(RE_BIG_SKULLTULA) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_SPIKE_ROLLER_BACK_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_SPIKE_ROLLER_BACK_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM,   logic->Get(LOGIC_DEKU_TREE_MQ_WATER_ROOM_TORCHES) && logic->CanPassEnemy(RE_BIG_SKULLTULA, logic->CanUse(RG_SONG_OF_TIME) ? ED_CLOSE : ED_SHORT_JUMPSLASH)),
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, ctx->GetTrickOption(RT_DEKU_MQ_LOG) || (logic->IsChild && logic->CanShield()) || logic->CanUse(RG_LONGSHOT) || logic->HasItem(RG_BRONZE_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && (logic->IsAdult || logic->CanUse(RG_HOOKSHOT)))),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM] = Region("Deku Tree MQ Basement Southwest Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_LARVAE_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_LARVAE_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        //both imply CanKillEnemy(RE_GOHMA_LARVA)
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM,      AnyAgeTime([]{return logic->CanKillEnemy(RE_MAD_SCRUB) && logic->CanKillEnemy(RE_KEESE);})),
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, AnyAgeTime([]{return logic->CanKillEnemy(RE_MAD_SCRUB) && logic->CanKillEnemy(RE_KEESE);})),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM] = Region("Deku Tree MQ Basement Grave Room", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM, logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_SONG_OF_TIME) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG))),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_GRAVES_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_GRAVES_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_GRAVES_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_GRAVES_GRASS_4, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_GRAVES_GRASS_5, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_LEDGE,          logic->CanUse(RG_CRAWL) && AnyAgeTime([]{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);})),
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM, true),
        //Using a bow to get past here as adult is a bit precise on standing position but simple, doing as as child requires a side-hop with the bow out to shoot through the torch and may be trick worthy
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM,      AnyAgeTime([]{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);})),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM] = Region("Deku Tree MQ Basement Back Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_BACK_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_BACK_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_BACK_GRASS_3, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM, true),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_LEDGE] = Region("Deku Tree MQ Basement Ledge", SCENE_DEKU_TREE, {
        //Events
        EVENT_ACCESS(LOGIC_DEKU_TREE_PUSHED_BASEMENT_BLOCK, logic->HasItem(RG_POWER_BRACELET)),
        //You can also recoil from RR_DEKU_TREE_MQ_1F with a lit stick to burn the web, but such recoils would be tricks
        EVENT_ACCESS(LOGIC_DEKU_TREE_B1_BROKE_WEB,          logic->HasFireSource()),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_DEKU_SCRUB,             logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_UPPER_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_UPPER_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_UPPER_GRASS_3, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM, logic->CanUse(RG_CRAWL)),
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT,            true),
        //recoiling to skip swim is possible, but would be a trick
        ENTRANCE(RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM,   logic->Get(LOGIC_DEKU_TREE_B1_BROKE_WEB) && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
    });

    areaTable[RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM] = Region("Deku Tree MQ Outside Boss Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_BOSS_LEFT_HEART,   logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_BOSS_MIDDLE_HEART, logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_BOSS_RIGHT_HEART,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_BOSS_GRASS_1,      logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_BOSS_GRASS_2,      logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_BOSS_GRASS_3,      logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_MQ_BASEMENT_LEDGE, logic->HasItem(RG_CLIMB) && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_DEKU_TREE_BOSS_ENTRYWAY,     AnyAgeTime([]{return logic->CanReflectNuts();})),
    });

#pragma endregion

    // Boss Room
    areaTable[RR_DEKU_TREE_BOSS_ENTRYWAY] = Region("Deku Tree Boss Entryway", SCENE_DEKU_TREE, {}, {}, {
        // Exits
        ENTRANCE(RR_DEKU_TREE_BOSS_ROOM, true),
    });

    areaTable[RR_DEKU_TREE_BOSS_EXIT] = Region("Deku Tree Boss Exit", SCENE_DEKU_TREE, {}, {}, {
        // Exits
        ENTRANCE(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM,    ctx->GetDungeon(DEKU_TREE)->IsVanilla()),
        ENTRANCE(RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM, ctx->GetDungeon(DEKU_TREE)->IsMQ()),
    });

    areaTable[RR_DEKU_TREE_BOSS_ROOM] = Region("Deku Tree Boss Room", SCENE_DEKU_TREE_BOSS, {
        // Events
        EVENT_ACCESS(LOGIC_DEKU_TREE_CLEAR, logic->CanKillEnemy(RE_GOHMA)),
    }, {
        // Locations
        LOCATION(RC_QUEEN_GOHMA,                   logic->Get(LOGIC_DEKU_TREE_CLEAR)),
        LOCATION(RC_DEKU_TREE_QUEEN_GOHMA_HEART,   logic->Get(LOGIC_DEKU_TREE_CLEAR)),
        LOCATION(RC_DEKU_TREE_QUEEN_GOHMA_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_QUEEN_GOHMA_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_QUEEN_GOHMA_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_QUEEN_GOHMA_GRASS_4, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_QUEEN_GOHMA_GRASS_5, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_QUEEN_GOHMA_GRASS_6, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_QUEEN_GOHMA_GRASS_7, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_QUEEN_GOHMA_GRASS_8, logic->CanCutShrubs()),
    }, {
        // Exits
        ENTRANCE(RR_DEKU_TREE_BOSS_EXIT,  true),
        ENTRANCE(RR_KF_OUTSIDE_DEKU_TREE, logic->Get(LOGIC_DEKU_TREE_CLEAR), false),
    });

    // clang-format on
}
