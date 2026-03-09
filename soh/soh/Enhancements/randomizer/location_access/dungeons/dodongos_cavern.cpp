#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_DodongosCavern() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_DODONGOS_CAVERN_ENTRYWAY] = Region("Dodongos Cavern Entryway", SCENE_DODONGOS_CAVERN, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BEGINNING,    ctx->GetDungeon(DODONGOS_CAVERN)->IsVanilla()),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BEGINNING, ctx->GetDungeon(DODONGOS_CAVERN)->IsMQ()),
        ENTRANCE(RR_DEATH_MOUNTAIN_TRAIL,         true),
    });

#pragma region Vanilla

    areaTable[RR_DODONGOS_CAVERN_BEGINNING] = Region("Dodongos Cavern Beginning", SCENE_DODONGOS_CAVERN, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_ENTRYWAY, true),
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY,    AnyAgeTime([]{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_DODONGOS_CAVERN_LOBBY] = Region("Dodongos Cavern Lobby", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, (AnyAgeTime([]{return logic->CanBreakMudWalls();}) || logic->HasItem(RG_GORONS_BRACELET)) && logic->CallGossipFairy()),
        EVENT_ACCESS(LOGIC_DC_EYES_LIT,  ctx->GetTrickOption(RT_DC_EYES_CHU) && logic->CanUse(RG_BOMBCHU_5)),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MAP_CHEST,              (logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET)) && logic->HasItem(RG_OPEN_CHEST);),
        LOCATION(RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY,       (logic->CanStunDeku() || logic->HasItem(RG_GORONS_BRACELET)) && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DODONGOS_CAVERN_GOSSIP_STONE_FAIRY,     AnyAgeTime([]{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET); }) && logic->CallGossipFairy()),
        LOCATION(RC_DODONGOS_CAVERN_GOSSIP_STONE_FAIRY_BIG, AnyAgeTime([]{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET); }) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DODONGOS_CAVERN_GOSSIP_STONE,           AnyAgeTime([]{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET); })),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BEGINNING,     true),
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY_SWITCH,  logic->IsAdult || logic->CanGroundJump(true)),
        ENTRANCE(RR_DODONGOS_CAVERN_SE_CORRIDOR,   AnyAgeTime([]{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
        ENTRANCE(RR_DODONGOS_CAVERN_STAIRS_LOWER,  logic->Get(LOGIC_DC_STAIRS_ROOM_DOOR)),
        ENTRANCE(RR_DODONGOS_CAVERN_FAR_BRIDGE,    logic->Get(LOGIC_DC_LIFT_PLATFORM)),
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_AREA,     logic->Get(LOGIC_DC_EYES_LIT)),
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY, false),
    });

    areaTable[RR_DODONGOS_CAVERN_LOBBY_SWITCH] = Region("Dodongos Cavern Lobby Switch", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_DC_STAIRS_ROOM_DOOR, true),
    }, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY,        true),
        ENTRANCE(RR_DODONGOS_CAVERN_DODONGO_ROOM, true),
    });

    areaTable[RR_DODONGOS_CAVERN_SE_CORRIDOR] = Region("Dodongos Cavern SE Corridor", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_GS_SCARECROW,    logic->ReachScarecrow() || (logic->IsAdult && logic->CanUse(RG_LONGSHOT)) || (ctx->GetTrickOption(RT_DC_SCARECROW_GS) && logic->HasItem(RG_POWER_BRACELET) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA))),
        LOCATION(RC_DODONGOS_CAVERN_SIDE_ROOM_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SIDE_ROOM_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SIDE_ROOM_POT_3, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SIDE_ROOM_POT_4, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SIDE_ROOM_POT_5, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SIDE_ROOM_POT_6, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY,               true),
        //Shield seems to be in logic to drop a pot on their head as they hit you to blow up the wall
        ENTRANCE(RR_DODONGOS_CAVERN_SE_ROOM,             AnyAgeTime([]{return logic->CanBreakMudWalls() || logic->CanAttack() || (logic->TakeDamage() && logic->CanShield() && logic->HasItem(RG_POWER_BRACELET));})),
        ENTRANCE(RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, logic->HasItem(RG_POWER_BRACELET)),
    });

    areaTable[RR_DODONGOS_CAVERN_SE_ROOM] = Region("Dodongos Cavern SE Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS, logic->CanAttack()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_SE_CORRIDOR, true),
    });

    areaTable[RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS] = Region("Dodongos Cavern Near Lower Lizalfos", SCENE_DODONGOS_CAVERN, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_SE_CORRIDOR,    true),
        ENTRANCE(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, true),
    });

    areaTable[RR_DODONGOS_CAVERN_LOWER_LIZALFOS] = Region("Dodongos Cavern Lower Lizalfos", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_DC_KILLED_LOWER_LIZALFOS, logic->CanKillEnemy(RE_LIZALFOS, ED_CLOSE, true, 2)),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_LIZALFOS_POT_1,       logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_LIZALFOS_POT_2,       logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_LIZALFOS_POT_3,       logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_LIZALFOS_POT_4,       logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_LOWER_LIZALFOS_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, logic->Get(LOGIC_DC_KILLED_LOWER_LIZALFOS)),
        ENTRANCE(RR_DODONGOS_CAVERN_DODONGO_ROOM,        logic->Get(LOGIC_DC_KILLED_LOWER_LIZALFOS)),
    });

    areaTable[RR_DODONGOS_CAVERN_DODONGO_ROOM] = Region("Dodongos Cavern Dodongo Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_TORCH_ROOM_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_TORCH_ROOM_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_TORCH_ROOM_POT_3, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_TORCH_ROOM_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY_SWITCH,      logic->HasFireSourceWithTorch()),
        ENTRANCE(RR_DODONGOS_CAVERN_LOWER_LIZALFOS,    true),
        ENTRANCE(RR_DODONGOS_CAVERN_NEAR_DODONGO_ROOM, AnyAgeTime([]{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_DODONGOS_CAVERN_NEAR_DODONGO_ROOM] = Region("Dodongos Cavern Near Dodongo Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS, logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_DODONGO_ROOM, true),
    });

    areaTable[RR_DODONGOS_CAVERN_STAIRS_LOWER] = Region("Dodongos Cavern Stairs Lower", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS, ctx->GetTrickOption(RT_DC_VINES_GS) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_LONGSHOT)),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY,        true),
        ENTRANCE(RR_DODONGOS_CAVERN_STAIRS_UPPER, logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET) || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_DC_STAIRS_WITH_BOW) && logic->CanUse(RG_FAIRY_BOW))),
        ENTRANCE(RR_DODONGOS_CAVERN_COMPASS_ROOM, AnyAgeTime([]{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_DODONGOS_CAVERN_STAIRS_UPPER] = Region("Dodongos Cavern Stairs Upper", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, logic->Get(LOGIC_DC_LIFT_PLATFORM) ? ED_BOOMERANG : ED_LONGSHOT)),
        LOCATION(RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS,  (logic->HasItem(RG_CLIMB) && logic->HasItem(RG_POWER_BRACELET)) || logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        LOCATION(RC_DODONGOS_CAVERN_STAIRCASE_POT_1,        logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_STAIRCASE_POT_2,        logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_STAIRCASE_POT_3,        logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_STAIRCASE_POT_4,        logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_STAIRS_LOWER, true),
        ENTRANCE(RR_DODONGOS_CAVERN_ARMOS_ROOM,   true),
    });

    areaTable[RR_DODONGOS_CAVERN_COMPASS_ROOM] = Region("Dodongos Cavern Compass Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_COMPASS_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_STAIRS_LOWER, logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET)),
    });

    areaTable[RR_DODONGOS_CAVERN_ARMOS_ROOM] = Region("Dodongos Cavern Armos Room", SCENE_DODONGOS_CAVERN, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_STAIRS_UPPER,    true),
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, logic->IsAdult || (logic->HasItem(RG_POWER_BRACELET) && logic->HasItem(RG_CLIMB)) || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) || logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER] = Region("Dodongos Cavern Bomb Room Lower", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DODONGOS_CAVERN_BLADE_ROOM_HEART,           logic->HasItem(RG_POWER_BRACELET)),
        LOCATION(RC_DODONGOS_CAVERN_FIRST_BRIDGE_GRASS,         logic->CanCutShrubs()),
        LOCATION(RC_DODONGOS_CAVERN_BLADE_GRASS,                logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_ARMOS_ROOM,           logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_DODONGOS_CAVERN_2F_SIDE_ROOM,         AnyAgeTime([]{return logic->CanBreakMudWalls() || (ctx->GetTrickOption(RT_DC_SCRUB_ROOM) && logic->HasItem(RG_GORONS_BRACELET));})),
        ENTRANCE(RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM, AnyAgeTime([]{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER,      (logic->IsAdult && (ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) || logic->CanGroundJump())) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives() && logic->CanJumpslash())),
    });

    areaTable[RR_DODONGOS_CAVERN_2F_SIDE_ROOM] = Region("Dodongos Cavern 2F Side Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,  logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT, logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, true),
    });

    areaTable[RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM] = Region("Dodongos Cavern First Slingshot Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_SINGLE_EYE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SINGLE_EYE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_SINGLE_EYE_GRASS, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, true),
        ENTRANCE(RR_DODONGOS_CAVERN_UPPER_LIZALFOS,  logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || ctx->GetTrickOption(RT_DC_SLINGSHOT_SKIP) || (logic->IsAdult && logic->CanGroundJump()) || (ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && (logic->CanUse(RG_MEGATON_HAMMER) || (logic->CanStandingShield() && logic->CanJumpslash())))),
    });

    areaTable[RR_DODONGOS_CAVERN_UPPER_LIZALFOS] = Region("Dodongos Cavern Upper Lizalfos", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_LOWER_LIZALFOS_HEART,       true),
        LOCATION(RC_DODONGOS_CAVERN_UPPER_LIZALFOS_LEFT_HEART,  true),
        LOCATION(RC_DODONGOS_CAVERN_UPPER_LIZALFOS_RIGHT_HEART, true),
        //This room is in different states based on what entrance you entered by,
        //So it's simpler to treat them as separate regions with shared checks in both
        LOCATION(RC_DODONGOS_CAVERN_LIZALFOS_POT_1,             logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_LIZALFOS_POT_2,             logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_LIZALFOS_POT_3,             logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_LIZALFOS_POT_4,             logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_LOWER_LIZALFOS_HEART,       true),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM,  AnyAgeTime([]{return logic->CanKillEnemy(RE_LIZALFOS, ED_CLOSE, true, 2);})),
        ENTRANCE(RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_LIZALFOS, ED_CLOSE, true, 2);})),
        ENTRANCE(RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS,   logic->Get(LOGIC_DC_KILLED_LOWER_LIZALFOS)),
        ENTRANCE(RR_DODONGOS_CAVERN_DODONGO_ROOM,          logic->Get(LOGIC_DC_KILLED_LOWER_LIZALFOS)),
    });

    areaTable[RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM] = Region("Dodongos Cavern Second Slingshot Room", SCENE_DODONGOS_CAVERN, {}, {
        //Location
        LOCATION(RC_DODONGOS_CAVERN_DOUBLE_EYE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_DOUBLE_EYE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_UPPER_LIZALFOS,  true),
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER, logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || ctx->GetTrickOption(RT_DC_SLINGSHOT_SKIP) || (logic->IsAdult && logic->CanGroundJump()) || (ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && (logic->CanUse(RG_MEGATON_HAMMER) || (logic->CanStandingShield() && logic->CanJumpslash())))),
    });

    areaTable[RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER] = Region("Dodongos Cavern Bomb Room Upper", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_BOMB_BAG_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DODONGOS_CAVERN_BLADE_POT_1,    logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_BLADE_POT_2,    logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER,       true),
        ENTRANCE(RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, true),
        ENTRANCE(RR_DODONGOS_CAVERN_FAR_BRIDGE,            true),
    });

    areaTable[RR_DODONGOS_CAVERN_FAR_BRIDGE] = Region("Dodongos Cavern Far Bridge", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_DC_EYES_LIT,      logic->HasExplosives()),
        EVENT_ACCESS(LOGIC_DC_LIFT_PLATFORM, true),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST, logic->CanBreakMudWalls() && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY,           true),
        ENTRANCE(RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER, true),
    });

    areaTable[RR_DODONGOS_CAVERN_BOSS_AREA] = Region("Dodongos Cavern Boss Region", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CanBreakPots()),
    }, {
        //Location
        LOCATION(RC_DODONGOS_CAVERN_BEFORE_BOSS_GRASS, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_LOBBY,         true),
        ENTRANCE(RR_DODONGOS_CAVERN_BACK_ROOM,     AnyAgeTime([]{return logic->CanBreakMudWalls();})),
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY, logic->HasItem(RG_POWER_BRACELET)),
    });

    areaTable[RR_DODONGOS_CAVERN_BACK_ROOM] = Region("Dodongos Cavern Back Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_GS_BACK_ROOM,    logic->CanAttack()),
        LOCATION(RC_DODONGOS_CAVERN_BACK_ROOM_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_BACK_ROOM_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_BACK_ROOM_POT_3, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_BACK_ROOM_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_AREA, true),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_DODONGOS_CAVERN_MQ_BEGINNING] = Region("Dodongos Cavern MQ Beginning", SCENE_DODONGOS_CAVERN, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_ENTRYWAY, true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY, AnyAgeTime([]{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_LOBBY] = Region("Dodongos Cavern MQ Lobby", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_DC_EYES_LIT, ctx->GetTrickOption(RT_DC_EYES_CHU) && logic->CanUse(RG_BOMBCHU_5)),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_MAP_CHEST,              (logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR,  logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT, logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BEGINNING,         true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_GOSSIP_STONE,      AnyAgeTime([]{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_OUTSIDE_POES_ROOM, logic->IsAdult || logic->CanUse(RG_HOOKSHOT) || logic->CanGroundJump(true)),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE, AnyAgeTime([]{return logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET);})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER,      AnyAgeTime([]{return logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET);})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE,  AnyAgeTime([]{return logic->CanBreakMudWalls();}) || AnyAgeTime([]{return logic->HasItem(RG_GORONS_BRACELET) && logic->TakeDamage();})), //strength 1 and bunny speed works too
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH,      logic->Get(LOGIC_DC_EYES_LIT)),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_GOSSIP_STONE] = Region("Dodongos Cavern MQ Gossip Stone", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CallGossipFairy()),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_GOSSIP_STONE,              true),
        LOCATION(RC_DODONGOS_CAVERN_MQ_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY, true),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_OUTSIDE_POES_ROOM] = Region("Dodongos Cavern MQ Outside Poes Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Events
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY,     true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_POES_ROOM, true),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE] = Region("Dodongos Cavern MQ Mouth Side Bridge", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_DC_MQ_CLEAR_UPPER_LOBBY_ROCKS, logic->BlastOrSmash() || logic->CanUse(RG_DINS_FIRE)),
        EVENT_ACCESS(LOGIC_DC_EYES_LIT,                   logic->HasExplosives() || (logic->Get(LOGIC_DC_MQ_CLEAR_UPPER_LOBBY_ROCKS) && logic->HasItem(RG_GORONS_BRACELET) && ((logic->IsAdult && ctx->GetTrickOption(RT_DC_MQ_ADULT_EYES)) || (logic->IsChild && ctx->GetTrickOption(RT_DC_MQ_CHILD_EYES))))),
    }, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY,              true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER, logic->Get(LOGIC_DC_MQ_CLEAR_UPPER_LOBBY_ROCKS)),
        //Bunny hood jump + jumpslash can also make it directly from the raising platform
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_OUTSIDE_POES_ROOM,  logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_DC_MQ_CHILD_BOMBS) && logic->CanJumpslashExceptHammer() && logic->TakeDamage())), //RANDOTODO is this possible with equip swapped hammer?
        //it is possible to use bunny hood speed, hovers and a jumpslash to go between here and the other bridge (included with TORCH_ROOM_LOWER), but this would be a trick
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER] = Region("Dodongos Cavern MQ Stairs Lower", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_POT_1,         logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_POT_2,         logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_POT_3,         logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_POT_4,         logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_LOWER_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_LOWER_CRATE_2, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY,               true),
        //This is possible with sticks and shield, igniting a first flower by "touch" then very quickly crouch stabbing in a way that cuts the corner to light the 3rd bomb on the other side, but that's a trick
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_UPPER,         AnyAgeTime([]{return logic->HasExplosives() || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_DC_STAIRS_WITH_BOW) && logic->CanUse(RG_FAIRY_BOW));})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_MUD_WALL, AnyAgeTime([]{return logic->HasItem(RG_GORONS_BRACELET) || logic->CanBreakMudWalls();})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_MUD_WALL] = Region("Dodongos Cavern MQ Stairs Past Mud Wall", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS, logic->CanGetDekuBabaSticks()),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM, logic->CanUse(RG_SONG_OF_TIME) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_UPPER, logic->HasExplosives() || (logic->HasItem(RG_GORONS_BRACELET) && (logic->CanUse(RG_STICKS) || ctx->GetTrickOption(RT_DC_MQ_STAIRS_WITH_ONLY_STRENGTH))) || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_DC_STAIRS_WITH_BOW) && logic->CanUse(RG_FAIRY_BOW))),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER, true),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_STAIRS_UPPER] = Region("Dodongos Cavern MQ Stairs Upper", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_DC_MQ_STAIRS_SILVER_RUPEES, logic->HasItem(RG_CLIMB)),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE,    logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_UPPER_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_UPPER_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_UPPER_CRATE_3, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_STAIRCASE_UPPER_CRATE_4, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER,               true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_BIG_SKULLTULAS, logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->CanUse(RG_HOVER_BOOTS)),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_BIG_SKULLTULAS] = Region("Dodongos Cavern MQ Past Big Skulltulas", SCENE_DODONGOS_CAVERN, {}, {}, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_UPPER, logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER, logic->TakeDamage()),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM, logic->Get(LOGIC_DC_MQ_STAIRS_SILVER_RUPEES)),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM] = Region("Dodongos Cavern MQ Dodongo Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST,   (logic->CanKillEnemy(RE_DODONGO) || (logic->HasItem(RG_GORONS_BRACELET) && logic->CanClimbLadder())) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DODONGOS_CAVERN_MQ_COMPASS_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_COMPASS_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_COMPASS_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_COMPASS_GRASS_4, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_BIG_SKULLTULAS, true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER,         AnyAgeTime([]{return logic->CanKillEnemy(RE_DODONGO) || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER] = Region("Dodongos Cavern MQ Torch Puzzle Lower", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_DC_MQ_CLEAR_UPPER_LOBBY_ROCKS, (((logic->IsAdult /*or bunny hood jump*/) && ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS)) || logic->CanUse(RG_HOVER_BOOTS)) && logic->CanUse(RG_STICKS)),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_MIDDLE_POT, logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY,              logic->TakeDamage()),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM,       true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LARVAE_ROOM,        logic->HasFireSource() || (logic->CanUse(RG_STICKS) && logic->HasItem(RG_POWER_BRACELET))),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM,     AnyAgeTime([]{return logic->HasFireSourceWithTorch();})), //Includes an implied CanPass(RE_BIG_SKULLTULA)
        //Bunny hood jump can make it as child
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER, (logic->IsAdult && (logic->HasItem(RG_POWER_BRACELET) || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) || logic->CanGroundJump())) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS,     logic->CanUse(RG_STICKS) && logic->HasItem(RG_GORONS_BRACELET)), //Implies access to RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM from here
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM] = Region("Dodongos Cavern MQ Big Block Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_BIG_BLOCK_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_BIG_BLOCK_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS,     (logic->IsAdult || logic->HasItem(RG_POWER_BRACELET) || logic->CanUse(RG_HOVER_BOOTS)) && ((logic->HasFireSource() && logic->HasItem(RG_GORONS_BRACELET)) || logic->CanBreakMudWalls())), // If you can somehow warp into this room, add logic->CanPassEnemy(RE_BIG_SKULLTULA)
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_LARVAE_ROOM] = Region("Dodongos Cavern MQ Larvae Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST,   logic->HasItem(RG_OPEN_CHEST)), //implied logic->CanKillEnemy(RE_GOHMA_LARVA) based on entry reqs with a trick to kill with nuts
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM,      logic->CanBreakCrates()), //implied logic->CanKillEnemy(RE_GOLD_SKULTULLA) based on entry reqs. Add crate logic when BONKO is added
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CRATE_3, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CRATE_4, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CRATE_5, logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CRATE_6, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, true), //implied logic->CanKillEnemy(RE_GOHMA_LARVA) based on entry reqs with a trick to kill with nuts
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS] = Region("Dodongos Cavern MQ Before Upper Lizalfos", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM,     logic->BlastOrSmash()), //Implied CanGetEnemyDrop(RE_GOLD_SKULLTULA)
        LOCATION(RC_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS_POT_3, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS_POT_4, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_HEART,  logic->BlastOrSmash()),
    }, {
        //Exits
        //Falling down gets you stuck with nothing there, not a useful exit for logic
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_LIZALFOS);})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_LIZALFOS);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM] = Region("Dodongos Cavern MQ Before Upper Lizalfos", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_TWO_FLAMES_POT_1,    logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_TWO_FLAMES_POT_2,    logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_TWO_FLAMES_CRATE_1,  logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_TWO_FLAMES_CRATE_2,  logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS,     true),
        //crate platforming skips the puzzle
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER, logic->IsAdult || (AnyAgeTime([]{return logic->BlastOrSmash() || (logic->CanAttack() && logic->HasItem(RG_GORONS_BRACELET));}))),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER] = Region("Dodongos Cavern MQ Torch Puzzle Upper", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_DC_MQ_CLEAR_UPPER_LOBBY_ROCKS, logic->CanDetonateUprightBombFlower() || logic->CanUse(RG_MEGATON_HAMMER)),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_CORNER_POT, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_MIDDLE_POT, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE,  logic->Get(LOGIC_DC_MQ_CLEAR_UPPER_LOBBY_ROCKS)),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM,     true),
        // Implied drop to LOWER_RIGHT_SIDE
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE,   logic->HasItem(RG_GORONS_BRACELET) && logic->TakeDamage()), //strength 1 and bunny speed works too
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE] = Region("Dodongos Cavern MQ Lower Right Side", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_RIGHT_SIDE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_RIGHT_SIDE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_RIGHT_SIDE_POT_3, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_RIGHT_SIDE_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY,                  true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE_SCRUB, logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET)),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS,         AnyAgeTime([]{return logic->CanDetonateBombFlowers() || logic->HasItem(RG_GORONS_BRACELET);}) && logic->CanHitEyeTargets()),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE_SCRUB] = Region("Dodongos Cavern MQ Lower Right Side Scrub", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, true),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS] = Region("Dodongos Cavern MQ Lower Lizalfos", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, AnyAgeTime([]{return logic->CanKillEnemy(RE_LIZALFOS);})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_POES_ROOM,        AnyAgeTime([]{return logic->CanKillEnemy(RE_LIZALFOS);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_POES_ROOM] = Region("Dodongos Cavern MQ Poes Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_POT_1,   logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_POT_2,   logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_POT_3,   logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_POT_4,   logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_1, logic->HasItem(RG_GORONS_BRACELET) || logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_2, logic->HasItem(RG_GORONS_BRACELET) || logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_3, logic->HasItem(RG_GORONS_BRACELET) || logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_4, logic->HasItem(RG_GORONS_BRACELET) || logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_5, logic->HasItem(RG_GORONS_BRACELET) || logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_6, logic->HasItem(RG_GORONS_BRACELET) || logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_7, logic->HasItem(RG_GORONS_BRACELET) || logic->CanBreakCrates()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_POE_ROOM_CRATE_8, logic->HasItem(RG_GORONS_BRACELET) || logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_OUTSIDE_POES_ROOM, AnyAgeTime([]{return logic->CanDetonateBombFlowers() || logic->HasItem(RG_GORONS_BRACELET);})),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS,    true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_MAD_SCRUB_ROOM,    AnyAgeTime([]{return logic->CanDetonateBombFlowers() || logic->HasItem(RG_GORONS_BRACELET);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_MAD_SCRUB_ROOM] = Region("Dodongos Cavern Mad Scrub Room", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM, (logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG, true))), //Implies you can avoid/kill the enemies with what you use on the skull, if this assumption is broken, add
                                                                                                    //&& (AnyAgeTime([]{return logic->CanKillEnemy(RE_FIRE_KEESE) && logic->CanKillEnemy(RE_MAD_SCRUB);}) || (logic->CanAvoidEnemy(RE_FIRE_KEESE) && logic->CanAvoidEnemy(RE_MAD_SCRUB)))
        LOCATION(RC_DODONGOS_CAVERN_MQ_SCRUB_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_SCRUB_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_POES_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_FIRE_KEESE) && logic->CanKillEnemy(RE_MAD_SCRUB);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH] = Region("Dodongos Cavern MQ Behind Mouth", SCENE_DODONGOS_CAVERN, {}, {
        //Locations
        //This can be gotten with only str0 as adult by using 1 pot to kill the skull from the top of a nearby grave,
        //then another to hit the switch to turn of the fire then running up the ledge to grab it. This assumes RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE access
        //Alternatively, the grave can be pulled and roll jumped off of.
        //This may be trickworthy, but it can only come up with doorsanity or decoupled boss door rando, making it too niche to be it's own trick
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA,       logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) || 
                                                           (logic->IsAdult && logic->HasItem(RG_POWER_BRACELET) && 
                                                            (logic->CanPassEnemy(RE_ARMOS) || logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS)/* && Roll*/)/* || bunny jumps*/))),
        LOCATION(RC_DODONGOS_CAVERN_MQ_BEFORE_BOSS_SW_POT, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_BEFORE_BOSS_NE_POT, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_ARMOS_ROOM_SE_POT,  logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_ARMOS_ROOM_SW_POT,  logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_LOBBY,             true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE,  logic->HasItem(RG_POWER_BRACELET) || logic->HasExplosives() || 
                                                          (logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS)/* && Roll*/)/* || bunny jumps*/))),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE, logic->IsAdult),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE] = Region("Dodongos Cavern MQ Back Behind Fire", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_DC_MQ_BEHIND_FIRE_SWITCH, logic->CanDetonateBombFlowers()),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH,      logic->CanHitSwitch()),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BACK_POE_ROOM,     true),
        //There's a trick N64 rolls into the child eyes trick for using armos blow up the bomb flowers when dieing, which would be killing an armos
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE, (logic->CanPassEnemy(RE_ARMOS) && (logic->IsAdult || logic->Get(LOGIC_DC_MQ_BEHIND_FIRE_SWITCH))) || logic->CanUse(RG_HOVER_BOOTS)),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_BACK_POE_ROOM] = Region("Dodongos Cavern MQ Back Poe Room", SCENE_DODONGOS_CAVERN, {}, {
        //the chest is in a grave, but you can open it through it
        LOCATION(RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST, (ctx->GetTrickOption(RT_VISIBLE_COLLISION) || logic->HasItem(RG_POWER_BRACELET)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DODONGOS_CAVERN_MQ_BACKROOM_POT_1,    logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_BACKROOM_POT_2,    logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_BACK_POE_GRASS,    logic->CanCutShrubs()),
    }, {
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE, AnyAgeTime([]{return logic->CanKillEnemy(RE_POE);})),
    });

    areaTable[RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE] = Region("Dodongos Cavern MQ Back Switch Grave", SCENE_DODONGOS_CAVERN, {
        //Events
        EVENT_ACCESS(LOGIC_DC_MQ_BEHIND_FIRE_SWITCH, logic->HasItem(RG_POWER_BRACELET) || logic->CanHitSwitch() || logic->CanDetonateBombFlowers()),
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS,             logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA,      logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA) || logic->HasItem(RG_GORONS_BRACELET)),
        LOCATION(RC_DODONGOS_CAVERN_MQ_ARMOS_ROOM_NW_POT, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_ARMOS_ROOM_NE_POT, logic->CanBreakPots()),
        LOCATION(RC_DODONGOS_CAVERN_MQ_ARMOS_GRASS,       logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH,     true),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE, true),
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY,       logic->HasItem(RG_POWER_BRACELET)),
    });

#pragma endregion

    // Boss Room
    areaTable[RR_DODONGOS_CAVERN_BOSS_ENTRYWAY] = Region("Dodongos Cavern Boss Entryway", SCENE_DODONGOS_CAVERN, {}, {}, {
        // Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_ROOM, true),
    });

    areaTable[RR_DODONGOS_CAVERN_BOSS_EXIT] = Region("Dodongos Cavern Boss Exit", SCENE_DODONGOS_CAVERN, {}, {}, {
        // Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_AREA,       ctx->GetDungeon(DODONGOS_CAVERN)->IsVanilla()),
        ENTRANCE(RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH, ctx->GetDungeon(DODONGOS_CAVERN)->IsMQ()),
    });

    areaTable[RR_DODONGOS_CAVERN_BOSS_ROOM] = Region("Dodongos Cavern Boss Room", SCENE_DODONGOS_CAVERN_BOSS, {
        // Events
        // Blue Fire Arrows need similar accuracy as hammer trick, only put in logic when both hammer & blue fire tricks enabled
        EVENT_ACCESS(LOGIC_DODONGOS_CAVERN_CLEAR, AnyAgeTime([]{return logic->HasExplosives() ||
                                                                  (ctx->GetTrickOption(RT_DC_HAMMER_FLOOR) ? logic->CanUse(RG_MEGATON_HAMMER) || (ctx->GetTrickOption(RT_BLUE_FIRE_MUD_WALLS) && logic->BlueFire()) :
                                                                                                             ctx->GetTrickOption(RT_BLUE_FIRE_MUD_WALLS) && logic->CanUse(RG_BOTTLE_WITH_BLUE_FIRE));})
                                                                  && logic->CanKillEnemy(RE_KING_DODONGO)),
    }, {
        // Locations
        LOCATION(RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST,    logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DODONGOS_CAVERN_KING_DODONGO_HEART, logic->Get(LOGIC_DODONGOS_CAVERN_CLEAR)),
        LOCATION(RC_KING_DODONGO,                       logic->Get(LOGIC_DODONGOS_CAVERN_CLEAR)),
    }, {
        // Exits
        ENTRANCE(RR_DODONGOS_CAVERN_BOSS_EXIT, true),
        ENTRANCE(RR_DEATH_MOUNTAIN_TRAIL,      logic->Get(LOGIC_DODONGOS_CAVERN_CLEAR), false),
    });

    // clang-format on
}
