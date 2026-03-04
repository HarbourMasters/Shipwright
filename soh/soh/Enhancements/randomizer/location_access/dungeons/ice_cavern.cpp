#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_IceCavern() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_ICE_CAVERN_ENTRYWAY] = Region("Ice Cavern Entryway", SCENE_ICE_CAVERN, {}, {}, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_BEGINNING,    ctx->GetDungeon(ICE_CAVERN)->IsVanilla()),
        ENTRANCE(RR_ICE_CAVERN_MQ_BEGINNING, ctx->GetDungeon(ICE_CAVERN)->IsMQ() && logic->CanUseProjectile()),
        ENTRANCE(RR_ZF_LEDGE,                true),
    });

#pragma region Vanilla

    areaTable[RR_ICE_CAVERN_BEGINNING] = Region("Ice Cavern Beginning", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_ENTRANCE_STORMS_FAIRY, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ICE_CAVERN_LOBBY_RUPEE,           logic->BlueFire()), // can get with rang trick
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_ENTRYWAY,        true),
        ENTRANCE(RR_ICE_CAVERN_HUB,             AnyAgeTime([]{return logic->CanKillEnemy(RE_FREEZARD, ED_CLOSE, true, 4);})),
        ENTRANCE(RR_ICE_CAVERN_ABOVE_BEGINNING, false),
    });

    areaTable[RR_ICE_CAVERN_HUB] = Region("Ice Cavern Hub", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM, logic->HookshotOrBoomerang()),
        LOCATION(RC_ICE_CAVERN_HALL_POT_1,              logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_HALL_POT_2,              logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_SPINNING_BLADE_POT_1,    logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_SPINNING_BLADE_POT_2,    logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_SPINNING_BLADE_POT_3,    logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_BEGINNING,    true),
        ENTRANCE(RR_ICE_CAVERN_MAP_ROOM,     (logic->IsAdult || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump())) && logic->CanClearStalagmite()),
        ENTRANCE(RR_ICE_CAVERN_COMPASS_ROOM, AnyAgeTime([]{return logic->BlueFire();})),
        ENTRANCE(RR_ICE_CAVERN_BLOCK_ROOM,   AnyAgeTime([]{return logic->BlueFire();}) && (logic->CanClearStalagmite() || ctx->GetTrickOption(RT_ICE_STALAGMITE_CLIP))),
    });

    areaTable[RR_ICE_CAVERN_MAP_ROOM] = Region("Ice Cavern Map Room", SCENE_ICE_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_BLUE_FIRE_ACCESS, true),
    }, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MAP_CHEST,             logic->BlueFire() && logic->HasItem(RG_OPEN_CHEST)),
        // very easy to break pot through ice
        LOCATION(RC_ICE_CAVERN_FROZEN_POT_1,          (logic->CanBreakPots() && logic->BlueFire()) || logic->HasExplosives() ||
                                                      (ctx->GetTrickOption(RT_VISIBLE_COLLISION) && ((logic->CanStandingShield() && logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER))) ||
                                                      (ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION) && logic->CanUse(RG_HOOKSHOT))),
        LOCATION(RC_ICE_CAVERN_MAP_ROOM_LEFT_HEART,   true),
        LOCATION(RC_ICE_CAVERN_MAP_ROOM_MIDDLE_HEART, true),
        LOCATION(RC_ICE_CAVERN_MAP_ROOM_RIGHT_HEART,  true),
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_HUB, true),
    });

    areaTable[RR_ICE_CAVERN_COMPASS_ROOM] = Region("Ice Cavern Map Room", SCENE_ICE_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_BLUE_FIRE_ACCESS, true),
    }, {
        //Locations
        LOCATION(RC_ICE_CAVERN_COMPASS_CHEST,       (logic->IsChild || logic->CanClearStalagmite() || ctx->GetTrickOption(RT_ICE_STALAGMITE_CLIP)) && logic->BlueFire() && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_ICE_CAVERN_FREESTANDING_POH,    (logic->CanClearStalagmite() || ctx->GetTrickOption(RT_ICE_STALAGMITE_CLIP)) && logic->BlueFire()), // can skip blue fire with rang trick
        LOCATION(RC_ICE_CAVERN_GS_HEART_PIECE_ROOM, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_HUB, true),
    });

    areaTable[RR_ICE_CAVERN_BLOCK_ROOM] = Region("Ice Cavern Block Room", SCENE_ICE_CAVERN, {}, {
        //Locations
        // trick involves backflip, could be merged into general trick
        LOCATION(RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM,    logic->HookshotOrBoomerang() || (ctx->GetTrickOption(RT_ICE_BLOCK_GS) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH))),
        LOCATION(RC_ICE_CAVERN_SLIDING_BLOCK_RUPEE_1, logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_ICE_CAVERN_SLIDING_BLOCK_RUPEE_2, logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_ICE_CAVERN_SLIDING_BLOCK_RUPEE_3, logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_HUB,                  logic->CanClearStalagmite() || ctx->GetTrickOption(RT_ICE_STALAGMITE_CLIP)),
        ENTRANCE(RR_ICE_CAVERN_BLOCK_ROOM_BLUE_FIRE, logic->HasItem(RG_POWER_BRACELET) || (logic->IsAdult && (logic->CanGroundJump() || ctx->GetTrickOption(RT_SLIDE_JUMP)))),
        ENTRANCE(RR_ICE_CAVERN_BEFORE_FINAL_ROOM,    (logic->HasItem(RG_POWER_BRACELET) || (logic->IsAdult && (logic->CanGroundJump() || ctx->GetTrickOption(RT_SLIDE_JUMP)))) && AnyAgeTime([]{return logic->BlueFire();})),
    });

    areaTable[RR_ICE_CAVERN_BLOCK_ROOM_BLUE_FIRE] = Region("Ice Cavern Block Room Blue Fire", SCENE_ICE_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_BLUE_FIRE_ACCESS, true),
    }, {
        //Locations
        LOCATION(RC_ICE_CAVERN_SLIDING_BLOCK_RUPEE_1, logic->CanUse(RG_SONG_OF_TIME)),
        LOCATION(RC_ICE_CAVERN_SLIDING_BLOCK_RUPEE_2, logic->CanUse(RG_SONG_OF_TIME)),
        LOCATION(RC_ICE_CAVERN_SLIDING_BLOCK_RUPEE_3, logic->CanUse(RG_SONG_OF_TIME)),
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_BLOCK_ROOM, true),
    });

    // this represents being past the red ice barricade, not just past the silver rupee door
    areaTable[RR_ICE_CAVERN_BEFORE_FINAL_ROOM] = Region("Ice Cavern Before Final Room", SCENE_ICE_CAVERN, {}, {
        //Locations
        //Assumes RR_ICE_CAVERN_BLOCK_ROOM access
        LOCATION(RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM, ctx->GetTrickOption(RT_ICE_BLOCK_GS) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && logic->BlueFire() && logic->HasItem(RG_POWER_BRACELET)),
        LOCATION(RC_ICE_CAVERN_NEAR_END_POT_1,     logic->CanBreakPots() && logic->BlueFire()),
        LOCATION(RC_ICE_CAVERN_NEAR_END_POT_2,     logic->CanBreakPots() && logic->BlueFire()),
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_BLOCK_ROOM, AnyAgeTime([]{return logic->BlueFire();})),
        ENTRANCE(RR_ICE_CAVERN_FINAL_ROOM, true),
    });

    areaTable[RR_ICE_CAVERN_FINAL_ROOM] = Region("Ice Cavern Final Room", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_IRON_BOOTS_CHEST, AnyAgeTime([]{return logic->CanKillEnemy(RE_WOLFOS);}) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHEIK_IN_ICE_CAVERN,         AnyAgeTime([]{return logic->CanKillEnemy(RE_WOLFOS);}) && logic->HasItem(RG_OPEN_CHEST)), // rando enables this for child
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_BEFORE_FINAL_ROOM,     AnyAgeTime([]{return logic->CanKillEnemy(RE_WOLFOS);})),
        ENTRANCE(RR_ICE_CAVERN_FINAL_ROOM_UNDERWATER, AnyAgeTime([]{return logic->CanKillEnemy(RE_WOLFOS);}) && logic->CanUse(RG_IRON_BOOTS)),
    });

    areaTable[RR_ICE_CAVERN_FINAL_ROOM_UNDERWATER] = Region("Ice Cavern Final Room Underwater", SCENE_ICE_CAVERN, {}, {}, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_FINAL_ROOM,      logic->CanUse(RG_BRONZE_SCALE)),
        ENTRANCE(RR_ICE_CAVERN_ABOVE_BEGINNING, logic->CanUse(RG_IRON_BOOTS)),
    });

    areaTable[RR_ICE_CAVERN_ABOVE_BEGINNING] = Region("Ice Cavern Above Beginning", SCENE_ICE_CAVERN, {}, {}, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_FINAL_ROOM_UNDERWATER, logic->CanUse(RG_IRON_BOOTS)),
        ENTRANCE(RR_ICE_CAVERN_BEGINNING,             true),
    });
#pragma endregion

#pragma region MQ

    areaTable[RR_ICE_CAVERN_MQ_BEGINNING] = Region("Ice Cavern MQ Beginning", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_ENTRANCE_POT, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_ENTRYWAY,           true),
        //It is in logic to use a pot to hit the toggle switch here.
        ENTRANCE(RR_ICE_CAVERN_MQ_HUB,             logic->HasItem(RG_POWER_BRACELET) || logic->CanHitSwitch(ED_BOMB_THROW) || (logic->IsAdult && logic->CanUse(RG_BIGGORON_SWORD))),
        ENTRANCE(RR_ICE_CAVERN_MQ_ABOVE_BEGINNING, false),
    });

    areaTable[RR_ICE_CAVERN_MQ_HUB] = Region("Ice Cavern MQ Hub", SCENE_ICE_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_FIRST_CRYSTAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_FIRST_CRYSTAL_POT_2, logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_EARLY_WOLFOS_POT_1,  logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_EARLY_WOLFOS_POT_2,  logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_EARLY_WOLFOS_POT_3,  logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_EARLY_WOLFOS_POT_4,  logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_MQ_MAP_ROOM,       AnyAgeTime([]{return logic->CanKillEnemy(RE_WHITE_WOLFOS) && logic->CanKillEnemy(RE_FREEZARD);})),
        ENTRANCE(RR_ICE_CAVERN_MQ_COMPASS_ROOM,   (logic->IsAdult || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump())) && logic->BlueFire()),
        ENTRANCE(RR_ICE_CAVERN_MQ_SCARECROW_ROOM, logic->BlueFire()),
    });

    areaTable[RR_ICE_CAVERN_MQ_MAP_ROOM] = Region("Ice Cavern MQ Map Room", SCENE_ICE_CAVERN, {
        //Events
        //Child can fit between the stalagmites on the left hand side
        EVENT_ACCESS(LOGIC_BLUE_FIRE_ACCESS, logic->IsChild || logic->CanClearStalagmite() || ctx->GetTrickOption(RT_ICE_STALAGMITE_CLIP)),
    }, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_MAP_CHEST, logic->BlueFire() && AnyAgeTime([]{return logic->CanHitSwitch();}) && logic->HasItem(RG_OPEN_CHEST)),
    }, {});

    areaTable[RR_ICE_CAVERN_MQ_SCARECROW_ROOM] = Region("Ice Cavern MQ Scarecrow Room", SCENE_ICE_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_BLUE_FIRE_ACCESS, logic->CanUse(RG_SONG_OF_TIME) || (logic->IsAdult && (logic->CanGroundJump() || ctx->GetTrickOption(RT_SLIDE_JUMP)))),
    }, {
        //Locations
        //Implies being able to kill the skull if you hit the switch
        LOCATION(RC_ICE_CAVERN_MQ_GS_ICE_BLOCK, (logic->BlueFire() && logic->HasItem(RG_POWER_BRACELET) && logic->CanKillEnemy(RE_GOLD_SKULLTULA)) || logic->CanHitSwitch(logic->IsAdult ? ED_LONG_JUMPSLASH : ED_BOMB_THROW)),
        LOCATION(RC_ICE_CAVERN_MQ_GS_SCARECROW, logic->ReachScarecrow() || (logic->IsAdult && (logic->CanUse(RG_LONGSHOT) || logic->CanGroundJump() || ctx->GetTrickOption(RT_SLIDE_JUMP)))),
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_MQ_HUB,           logic->BlueFire()),
        //The switch defaults into the state where the block exists, and is a temp flag
        ENTRANCE(RR_ICE_CAVERN_MQ_WEST_CORRIDOR, logic->IsAdult && logic->BlueFire()),
    });

    areaTable[RR_ICE_CAVERN_MQ_WEST_CORRIDOR] = Region("Ice Cavern MQ West Corridor", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_PUSH_BLOCK_POT_1, logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_PUSH_BLOCK_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_MQ_SCARECROW_ROOM, logic->BlueFire()),
        ENTRANCE(RR_ICE_CAVERN_MQ_STALFOS_ROOM,   true),
    });

    areaTable[RR_ICE_CAVERN_MQ_COMPASS_ROOM] = Region("Ice Cavern MQ Compass Room", SCENE_ICE_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_BLUE_FIRE_ACCESS, true),
    }, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_COMPASS_CHEST,    logic->HasItem(RG_OPEN_CHEST)),
        //It is possible for child with master, BGS or sticks, or adult with BGS, to hit this switch through the ice with a crouchstab, but it's precise and unintuitive for a trick
        LOCATION(RC_ICE_CAVERN_MQ_FREESTANDING_POH, logic->HasExplosives()), // can get with rang trick
        //doing RT_ICE_MQ_RED_ICE_GS as child is untested, as I could not perform the trick reliably even as adult
        LOCATION(RC_ICE_CAVERN_MQ_GS_RED_ICE,       (logic->CanUse(RG_BOTTLE_WITH_BLUE_FIRE) && (logic->CanUse(RG_SONG_OF_TIME) || (logic->IsAdult && ctx->GetTrickOption(RT_ICE_MQ_RED_ICE_GS))) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)) ||
                                                    (ctx->GetOption(RSK_BLUE_FIRE_ARROWS) && logic->CanUse(RG_ICE_ARROWS)) || (ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_HOOKSHOT))),
        LOCATION(RC_ICE_CAVERN_MQ_COMPASS_POT_1,    logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_COMPASS_POT_2,    logic->CanBreakPots()),
    }, {});

    areaTable[RR_ICE_CAVERN_MQ_STALFOS_ROOM] = Region("Ice Cavern MQ Stalfos Room", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST, logic->CanKillEnemy(RE_STALFOS) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHEIK_IN_ICE_CAVERN,            logic->CanKillEnemy(RE_STALFOS) && logic->HasItem(RG_OPEN_CHEST)), // rando enables this for child
    }, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_MQ_WEST_CORRIDOR,           AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS);})),
        ENTRANCE(RR_ICE_CAVERN_MQ_STALFOS_ROOM_UNDERWATER, logic->CanUse(RG_IRON_BOOTS) && AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS);})),
    });

    areaTable[RR_ICE_CAVERN_MQ_STALFOS_ROOM_UNDERWATER] = Region("Ice Cavern MQ Stalfos Room Underwater", SCENE_ICE_CAVERN, {}, {}, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_MQ_STALFOS_ROOM,    logic->CanUse(RG_BRONZE_SCALE)),
        ENTRANCE(RR_ICE_CAVERN_MQ_ABOVE_BEGINNING, logic->CanUse(RG_IRON_BOOTS)),
    });

    areaTable[RR_ICE_CAVERN_MQ_ABOVE_BEGINNING] = Region("Ice Cavern MQ Above Beginning", SCENE_ICE_CAVERN, {}, {}, {
        //Exits
        ENTRANCE(RR_ICE_CAVERN_MQ_STALFOS_ROOM_UNDERWATER, logic->CanUse(RG_IRON_BOOTS)),
        ENTRANCE(RR_ICE_CAVERN_MQ_BEGINNING,               true),
    });

#pragma endregion
    // clang-format on
}
