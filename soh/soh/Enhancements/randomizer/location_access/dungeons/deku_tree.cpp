#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_DekuTree() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_DEKU_TREE_ENTRYWAY] = Region("Deku Tree Entryway", SCENE_DEKU_TREE, {}, {}, {
        //Exits
        Entrance(RR_DEKU_TREE_LOBBY,      []{return ctx->GetDungeon(DEKU_TREE)->IsVanilla();}),
        Entrance(RR_DEKU_TREE_MQ_1F,      []{return ctx->GetDungeon(DEKU_TREE)->IsMQ();}),
        Entrance(RR_KF_OUTSIDE_DEKU_TREE, []{return true;}),
    });

#pragma region Vanilla

    areaTable[RR_DEKU_TREE_LOBBY] = Region("Deku Tree Lobby", SCENE_DEKU_TREE, {
        //Events
        EventAccess(&logic->DekuBabaSticks, []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(&logic->DekuBabaNuts,   []{return logic->CanGetDekuBabaNuts();}),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MAP_CHEST,         true),
        LOCATION(RC_DEKU_TREE_LOBBY_LOWER_HEART, true),
        LOCATION(RC_DEKU_TREE_LOBBY_UPPER_HEART, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        LOCATION(RC_DEKU_TREE_LOBBY_GRASS_1,     logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_LOBBY_GRASS_2,     logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_LOBBY_GRASS_3,     logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_LOBBY_GRASS_4,     logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_LOBBY_GRASS_5,     logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_ENTRYWAY,          []{return true;}),
        Entrance(RR_DEKU_TREE_2F_MIDDLE_ROOM,    []{return true;}),
        Entrance(RR_DEKU_TREE_COMPASS_ROOM,      []{return true;}),
        Entrance(RR_DEKU_TREE_BASEMENT_LOWER,    []{return Here(RR_DEKU_TREE_LOBBY, []{return logic->CanAttack() || logic->CanUse(RG_NUTS);});}),
        Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, []{return false;}),
        Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY,     []{return false;}),
    });

    areaTable[RR_DEKU_TREE_2F_MIDDLE_ROOM] = Region("Deku Tree 2F Middle Room", SCENE_DEKU_TREE, {}, {}, {
        //Exits
        Entrance(RR_DEKU_TREE_LOBBY,          []{return Here(RR_DEKU_TREE_2F_MIDDLE_ROOM, []{return logic->CanReflectNuts() || logic->CanUse(RG_MEGATON_HAMMER);});}),
        Entrance(RR_DEKU_TREE_SLINGSHOT_ROOM, []{return Here(RR_DEKU_TREE_2F_MIDDLE_ROOM, []{return logic->CanReflectNuts() || logic->CanUse(RG_MEGATON_HAMMER);});}),
    });

    areaTable[RR_DEKU_TREE_SLINGSHOT_ROOM] = Region("Deku Tree Slingshot Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_SLINGSHOT_CHEST,           true),
        LOCATION(RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST, true),
        LOCATION(RC_DEKU_TREE_SLINGSHOT_GRASS_1,         logic->CanCutShrubs() && logic->CanReflectNuts()),
        LOCATION(RC_DEKU_TREE_SLINGSHOT_GRASS_2,         logic->CanCutShrubs() && logic->CanReflectNuts()),
        LOCATION(RC_DEKU_TREE_SLINGSHOT_GRASS_3,         logic->CanCutShrubs() && logic->CanReflectNuts()),
        LOCATION(RC_DEKU_TREE_SLINGSHOT_GRASS_4,         logic->CanCutShrubs() && logic->CanReflectNuts()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_2F_MIDDLE_ROOM, []{return logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_HOVER_BOOTS);}),
    });

    areaTable[RR_DEKU_TREE_COMPASS_ROOM] = Region("Deku Tree Compass Room", SCENE_DEKU_TREE, {
        //Events
        EventAccess(&logic->DekuBabaSticks, []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(&logic->DekuBabaNuts,   []{return logic->CanGetDekuBabaNuts();}),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_COMPASS_CHEST,           true),
        LOCATION(RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST, true),
        LOCATION(RC_DEKU_TREE_GS_COMPASS_ROOM,         logic->CanAttack()),
        LOCATION(RC_DEKU_TREE_COMPASS_GRASS_1,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_COMPASS_GRASS_2,         logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_LOBBY,         []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);}),
        Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY, []{return false;}),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_LOWER] = Region("Deku Tree Basement Lower", SCENE_DEKU_TREE, {
        //Events
        EventAccess(&logic->DekuBabaSticks, []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(&logic->DekuBabaNuts,   []{return logic->CanGetDekuBabaNuts();}),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_BASEMENT_CHEST,    true),
        LOCATION(RC_DEKU_TREE_GS_BASEMENT_GATE,  logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH)),
        LOCATION(RC_DEKU_TREE_GS_BASEMENT_VINES, logic->CanKillEnemy(RE_GOLD_SKULLTULA, ctx->GetTrickOption(RT_DEKU_MQ_COMPASS_GS) ? ED_SHORT_JUMPSLASH : ED_BOMB_THROW)),
        LOCATION(RC_DEKU_TREE_BASEMENT_GRASS_1,  logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_GRASS_2,  logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_LOBBY,               []{return true;}),
        Entrance(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, []{return Here(RR_DEKU_TREE_BASEMENT_LOWER, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);});}),
        Entrance(RR_DEKU_TREE_BASEMENT_UPPER,      []{return logic->IsAdult || ctx->GetTrickOption(RT_DEKU_B1_SKIP) || HasAccessTo(RR_DEKU_TREE_BASEMENT_UPPER);}),
        Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM,   []{return false;}),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_SCRUB_ROOM] = Region("Deku Tree Basement Scrub Room", SCENE_DEKU_TREE, {}, {
        //Location
        LOCATION(RC_DEKU_TREE_BASEMENT_SCRUB_ROOM_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_SCRUB_ROOM_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_SCRUB_ROOM_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_SCRUB_ROOM_GRASS_4, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_BASEMENT_LOWER,            []{return true;}),
        Entrance(RR_DEKU_TREE_BASEMENT_WATER_ROOM_FRONT, []{return Here(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, []{return logic->CanHitEyeTargets();});}),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_WATER_ROOM_FRONT] = Region("Deku Tree Basement Water Room Front", SCENE_DEKU_TREE, {}, {}, {
        //Exits
        Entrance(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM,      []{return true;}),
        Entrance(RR_DEKU_TREE_BASEMENT_WATER_ROOM_BACK, []{return logic->HasItem(RG_BRONZE_SCALE) || ctx->GetTrickOption(RT_DEKU_B1_BACKFLIP_OVER_SPIKED_LOG);}),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_WATER_ROOM_BACK] = Region("Deku Tree Basement Water Room Back", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_BASEMENT_SPIKE_ROLLER_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_SPIKE_ROLLER_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_BASEMENT_WATER_ROOM_FRONT, []{return logic->HasItem(RG_BRONZE_SCALE) || ctx->GetTrickOption(RT_DEKU_B1_BACKFLIP_OVER_SPIKED_LOG);}),
        Entrance(RR_DEKU_TREE_BASEMENT_TORCH_ROOM,       []{return true;}),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_TORCH_ROOM] = Region("Deku Tree Basement Torch Room", SCENE_DEKU_TREE, {
        //Events
        EventAccess(&logic->DekuBabaSticks, []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(&logic->DekuBabaNuts,   []{return logic->CanGetDekuBabaNuts();}),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_BASEMENT_TORCHES_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_TORCHES_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_BASEMENT_WATER_ROOM_BACK, []{return true;}),
        Entrance(RR_DEKU_TREE_BASEMENT_BACK_LOBBY,      []{return Here(RR_DEKU_TREE_BASEMENT_TORCH_ROOM, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);});}),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_BACK_LOBBY] = Region("Deku Tree Basement Back Lobby", SCENE_DEKU_TREE, {
        //Events
        EventAccess(&logic->DekuBabaSticks, []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(&logic->DekuBabaNuts,   []{return logic->CanGetDekuBabaNuts();}),
    }, {
        //Location
        LOCATION(RC_DEKU_TREE_BASEMENT_LARVAE_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_BASEMENT_LARVAE_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_BASEMENT_TORCH_ROOM, []{return true;}),
        Entrance(RR_DEKU_TREE_BASEMENT_BACK_ROOM,  []{return Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);}) && Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return logic->BlastOrSmash();});}),
        Entrance(RR_DEKU_TREE_BASEMENT_UPPER,      []{return Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);}) && logic->IsChild;}),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_BACK_ROOM] = Region("Deku Tree Basement Back Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return true;}),
    });

    areaTable[RR_DEKU_TREE_BASEMENT_UPPER] = Region("Deku Tree Basement Upper", SCENE_DEKU_TREE, {
        //Events
        EventAccess(&logic->DekuBabaSticks, []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(&logic->DekuBabaNuts,   []{return logic->CanGetDekuBabaNuts();}),
    }, {}, {
        //Exits
        Entrance(RR_DEKU_TREE_BASEMENT_LOWER,      []{return true;}),
        Entrance(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return logic->IsChild;}),
        Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM,   []{return Here(RR_DEKU_TREE_BASEMENT_UPPER, []{return logic->HasFireSourceWithTorch() || (ctx->GetTrickOption(RT_DEKU_B1_BOW_WEBS) && logic->IsAdult && logic->CanUse(RG_FAIRY_BOW));});}),
    });

    areaTable[RR_DEKU_TREE_OUTSIDE_BOSS_ROOM] = Region("Deku Tree Outside Boss Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_BEFORE_BOSS_LEFT_HEART,       logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_BEFORE_BOSS_MIDDLE_HEART,     logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_BEFORE_BOSS_RIGHT_HEART,      logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_BEFORE_BOSS_GRASS_1,          logic->CanCutShrubs() && logic->HasFireSourceWithTorch()),
        LOCATION(RC_DEKU_TREE_BEFORE_BOSS_GRASS_2,          logic->CanCutShrubs() && logic->HasFireSourceWithTorch()),
        LOCATION(RC_DEKU_TREE_BEFORE_BOSS_GRASS_3,          logic->CanCutShrubs() && logic->HasFireSourceWithTorch()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_BASEMENT_UPPER, []{return true;}),
        Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY,  []{return (logic->HasItem(RG_BRONZE_SCALE) || Here(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, []{return logic->CanUse(RG_IRON_BOOTS);})) && Here(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, []{return logic->CanReflectNuts();});}),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_DEKU_TREE_MQ_1F] = Region("Deku Tree MQ 1F", SCENE_DEKU_TREE, {
        //Events
        EventAccess(&logic->DekuBabaSticks, []{return logic->CanKillEnemy(RE_WITHERED_DEKU_BABA);}),
        EventAccess(&logic->BrokeDeku1FWeb, []{return logic->HasFireSource();}),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_GRASS_4, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_GRASS_5, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_ENTRYWAY,    []{return true;}),
        //may need canAvoid logic with enemy shuffle
        Entrance(RR_DEKU_TREE_MQ_2F,       []{return true;}),
        //Swim is not required because you can jump with enough momentum to hit land.
        //You even avoid fall damage if you hit the shallow water, though it's obscure knowledge so may be a trick
        //if it is, then we need a landing room with (IsAdult || HasItem(RG_BRONZE_SCALE) || TakeDamage() || that trick) to reach basement
        Entrance(RR_DEKU_TREE_MQ_BASEMENT, []{return logic->BrokeDeku1FWeb;}),
        //is it possible to recoil from here to the ledge with a trick?
    });

    areaTable[RR_DEKU_TREE_MQ_2F] = Region("Deku Tree MQ 2F", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_MAP_CHEST,     true),
        LOCATION(RC_DEKU_TREE_MQ_GS_LOBBY,      logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_HEART,   true),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_GRASS_6, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_GRASS_7, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_LOBBY_CRATE,   logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_MQ_1F,              []{return true;}),
        //Will need canAvoid logic with enemy shuffle
        Entrance(RR_DEKU_TREE_MQ_3F,              []{return true;}),
        Entrance(RR_DEKU_TREE_MQ_EYE_TARGET_ROOM, []{return Here(RR_DEKU_TREE_MQ_2F, []{return logic->HasFireSource();});}),
    });

    areaTable[RR_DEKU_TREE_MQ_3F] = Region("Deku Tree MQ 3F", SCENE_DEKU_TREE, {
        //Events
        EventAccess(&logic->DekuBabaSticks,    []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(&logic->DekuBabaNuts,      []{return logic->CanGetDekuBabaNuts();}),
        EventAccess(&logic->BrokeDeku1FWeb,    []{return true;}),
    }, {
        //Locations
        //Implies CanKillEnemy(RE_GOHMA_LARVA)
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_CHEST,           logic->CanKillEnemy(RE_DEKU_BABA)),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST, logic->HasFireSourceWithTorch() || (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW))),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_HEART,      true),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_GRASS_1,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_GRASS_2,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_GRASS_3,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_GRASS_4,         logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_CRATE_1,    logic->CanBreakCrates()),
        LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_CRATE_2,    logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_MQ_2F,              []{return true;}),
        //Assumes RR_DEKU_TREE_MQ_2F access
        Entrance(RR_DEKU_TREE_MQ_EYE_TARGET_ROOM, []{return Here(RR_DEKU_TREE_MQ_3F, []{return logic->CanUse(RG_STICKS) || logic->CanUse(RG_FAIRY_BOW);});}),
        Entrance(RR_DEKU_TREE_MQ_BASEMENT,        []{return true;}),
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
        Entrance(RR_DEKU_TREE_MQ_COMPASS_ROOM, []{return Here(RR_DEKU_TREE_MQ_EYE_TARGET_ROOM, []{return logic->CanHitEyeTargets();});}),
        Entrance(RR_DEKU_TREE_MQ_2F,           []{return true;}),
    });

    areaTable[RR_DEKU_TREE_MQ_COMPASS_ROOM] = Region("Deku Tree MQ Compass Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_COMPASS_CHEST,   true),
        LOCATION(RC_DEKU_TREE_MQ_COMPASS_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_COMPASS_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_COMPASS_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_COMPASS_GRASS_4, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_MQ_EYE_TARGET_ROOM,    []{return logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_DEKU_TREE_MQ_PAST_BOULDER_VINES, []{return Here(RR_DEKU_TREE_MQ_COMPASS_ROOM, []{return logic->CanUse(RG_BOMBCHU_5) || (logic->CanUse(RG_BOMB_BAG) && (logic->CanUse(RG_SONG_OF_TIME) || logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS))) || (logic->CanUse(RG_MEGATON_HAMMER) && (logic->CanUse(RG_SONG_OF_TIME) || ctx->GetTrickOption(RT_DEKU_MQ_COMPASS_GS)));});}),
    });

    areaTable[RR_DEKU_TREE_MQ_PAST_BOULDER_VINES] = Region("Deku Tree MQ Past Boulder Vines", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_GS_PAST_BOULDER_VINES, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_MQ_COMPASS_ROOM_HEART,    true),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_MQ_COMPASS_ROOM, []{return logic->BlastOrSmash();}),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT] = Region("Deku Tree MQ Basement", SCENE_DEKU_TREE, {
        //Events
        EventAccess(&logic->DekuBabaSticks, []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(&logic->DekuBabaNuts,   []{return logic->CanGetDekuBabaNuts();}),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_CHEST,         logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_LOWER_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_LOWER_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_LOWER_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_LOWER_GRASS_4, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_MQ_1F,                        []{return true;}),
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM,   []{return Here(RR_DEKU_TREE_MQ_BASEMENT, []{return logic->CanHitEyeTargets();});}),
        //includes RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM Access, other fire sources clear directly from there
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, []{return Here(RR_DEKU_TREE_MQ_BASEMENT, []{return logic->CanHitEyeTargets();}) && logic->ClearedMQDekuSERoom && Here(RR_DEKU_TREE_MQ_BASEMENT, []{return logic->CanUse(RG_STICKS);});}),
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_LEDGE,            []{return ctx->GetTrickOption(RT_DEKU_B1_SKIP) || logic->PushedDekuBasementBlock || logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS);}),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM] = Region("Deku Tree MQ Southeast Room", SCENE_DEKU_TREE, {
        //Events
        //Implies CanKillEnemy(RE_GOHMA_LARVA)
        EventAccess(&logic->ClearedMQDekuSERoom, []{return logic->CanKillEnemy(RE_MAD_SCRUB);}),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_TORCHES_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_TORCHES_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_TORCHES_GRASS_3, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_TORCHES_GRASS_4, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, []{return logic->HasFireSource();}),
        Entrance(RR_DEKU_TREE_MQ_BASEMENT,                  []{return logic->ClearedMQDekuSERoom;}),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT] = Region("Deku Tree MQ Basement Water Room Front", SCENE_DEKU_TREE, {
        //Events
        //It's possible to get this with bow if you have move while in first person and one-point skips on, noticeably harder and jankier as child, but that's a trick
        EventAccess(&logic->MQDekuWaterRoomTorches, []{return logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_STICKS) && (ctx->GetTrickOption(RT_DEKU_MQ_LOG) || (logic->IsChild && logic->CanShield())));}),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST,           true),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_SPIKE_ROLLER_FRONT_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_SPIKE_ROLLER_FRONT_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_SPIKE_ROLLER_FRONT_GRASS_3, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, []{return ctx->GetTrickOption(RT_DEKU_MQ_LOG) || (logic->IsChild && logic->CanShield()) || logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS));}),
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM,  []{return true;}),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK] = Region("Deku Tree MQ Basement Water Room Back", SCENE_DEKU_TREE, {
        //Events
        EventAccess(&logic->DekuBabaSticks,         []{return logic->CanKillEnemy(RE_WITHERED_DEKU_BABA);}),
        EventAccess(&logic->MQDekuWaterRoomTorches, []{return logic->HasFireSource();}),
    }, {
        //Locations
        //it blocks the chest while stunned unless you stun it from afar while it's slightly off the ground
        LOCATION(RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST,           logic->CanUse(RG_SONG_OF_TIME) && logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_SPIKE_ROLLER_BACK_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_SPIKE_ROLLER_BACK_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM,   []{return logic->MQDekuWaterRoomTorches && logic->CanPassEnemy(RE_BIG_SKULLTULA, logic->CanUse(RG_SONG_OF_TIME) ? ED_CLOSE : ED_SHORT_JUMPSLASH);}),
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, []{return ctx->GetTrickOption(RT_DEKU_MQ_LOG) || (logic->IsChild && logic->CanShield()) || logic->CanUse(RG_LONGSHOT) || logic->HasItem(RG_BRONZE_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && (logic->IsAdult || logic->CanUse(RG_HOOKSHOT)));}),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM] = Region("Deku Tree MQ Basement Southwest Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_LARVAE_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_LARVAE_GRASS_2, logic->CanCutShrubs()),
        }, {
        //Exits
        //both imply CanKillEnemy(RE_GOHMA_LARVA)
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM,      []{return Here(RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM, []{return logic->CanKillEnemy(RE_MAD_SCRUB) && logic->CanKillEnemy(RE_KEESE);});}),
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, []{return Here(RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM, []{return logic->CanKillEnemy(RE_MAD_SCRUB) && logic->CanKillEnemy(RE_KEESE);});}),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM] = Region("Deku Tree MQ Basement Grave Room", SCENE_DEKU_TREE, {
        //Events
        EventAccess(&logic->DekuBabaSticks, []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(&logic->DekuBabaNuts,   []{return logic->CanGetDekuBabaNuts();})
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
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_LEDGE,          []{return logic->IsChild && Here(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);});}),
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM, []{return true;}),
        //Using a bow to get past here as adult is a bit precise on standing position but simple, doing as as child requires a side-hop with the bow out to shoot through the torch and may be trick worthy
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM,      []{return Here(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);});}),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM] = Region("Deku Tree MQ Basement Back Room", SCENE_DEKU_TREE, {}, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_BACK_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_BACK_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_BACK_GRASS_3, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM, []{return true;}),
    });

    areaTable[RR_DEKU_TREE_MQ_BASEMENT_LEDGE] = Region("Deku Tree MQ Basement Ledge", SCENE_DEKU_TREE, {
        //Events
        EventAccess(&logic->PushedDekuBasementBlock, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_DEKU_TREE_MQ_DEKU_SCRUB,             logic->CanStunDeku()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_UPPER_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_UPPER_GRASS_2, logic->CanCutShrubs()),
        LOCATION(RC_DEKU_TREE_MQ_BASEMENT_UPPER_GRASS_3, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM, []{return logic->IsChild;}),
        Entrance(RR_DEKU_TREE_MQ_BASEMENT,            []{return true;}),
        //If strength 0 is shuffled, add hovers or block push to the stick check
        //recoiling to skip swim is possible, but would be a trick
        Entrance(RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM,   []{return Here(RR_DEKU_TREE_MQ_BASEMENT_LEDGE, []{return logic->HasFireSource() || (/*logic->PushedDekuBasementBlock && */logic->CanUse(RG_STICKS));}) && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS));}),
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
        Entrance(RR_DEKU_TREE_MQ_BASEMENT_LEDGE, []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY,     []{return Here(RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM, []{return logic->CanReflectNuts();});}),
    });

#pragma endregion

    // Boss Room
    areaTable[RR_DEKU_TREE_BOSS_ENTRYWAY] = Region("Deku Tree Boss Entryway", SCENE_DEKU_TREE, {}, {}, {
        // Exits
        Entrance(RR_DEKU_TREE_BOSS_ROOM, []{return true;}),
    });

    areaTable[RR_DEKU_TREE_BOSS_EXIT] = Region("Deku Tree Boss Exit", SCENE_DEKU_TREE, {}, {}, {
        // Exits
        Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM,    []{return ctx->GetDungeon(DEKU_TREE)->IsVanilla();}),
        Entrance(RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM, []{return ctx->GetDungeon(DEKU_TREE)->IsMQ();}),
    });

    areaTable[RR_DEKU_TREE_BOSS_ROOM] = Region("Deku Tree Boss Room", SCENE_DEKU_TREE_BOSS, {
        // Events
        EventAccess(&logic->DekuTreeClear, []{return logic->CanKillEnemy(RE_GOHMA);}),
    }, {
        // Locations
        LOCATION(RC_QUEEN_GOHMA,                   logic->DekuTreeClear),
        LOCATION(RC_DEKU_TREE_QUEEN_GOHMA_HEART,   logic->DekuTreeClear),
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
        Entrance(RR_DEKU_TREE_BOSS_EXIT,  []{return true;}),
        Entrance(RR_KF_OUTSIDE_DEKU_TREE, []{return logic->DekuTreeClear;}, false),
    });

    // clang-format on
}
