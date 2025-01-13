#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_BottomOfTheWell() {
    // Vanilla/MQ Decider
    areaTable[RR_BOTW_ENTRYWAY] = Region("Bottom of the Well Entryway", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        //Technically involves an fake wall, but passing it lensless is intended in vanilla and it is well telegraphed
        Entrance(RR_BOTW_LOBBY   , []{return ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla() && logic->IsChild && logic->CanPassEnemy(RE_BIG_SKULLTULA);}),
        Entrance(RR_BOTW_MQ_LOBBY, []{return ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsMQ()      && logic->IsChild;}),
        Entrance(RR_KAK_WELL     , []{return true;}),
    });

#pragma region Vanilla

    areaTable[RR_BOTW_LOBBY] = Region("Bottom of the Well Lobby", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->StickPot        , []{return true;}),
        EventAccess(&logic->NutPot          , []{return true;}),
        EventAccess(&logic->LoweredBotwWater, []{return logic->CanUse(RG_ZELDAS_LULLABY);}),
    }, {
        //Locations
        LOCATION(RC_BOTW_SOUTH_SUNKEN_CHEST, logic->LoweredBotwWater),
        LOCATION(RC_BOTW_SOUTH_BURIED_CHEST, logic->HasExplosives()),
        LOCATION(RC_BOTW_WEST_SUNKEN_CHEST , logic->LoweredBotwWater),
        LOCATION(RC_BOTW_ENTRYWAY_WEST_POT , logic->CanBreakPots()),
        LOCATION(RC_BOTW_ENTRYWAY_EAST_POT , logic->CanBreakPots()),
        LOCATION(RC_BOTW_SUNKEN_POT        , (logic->CanBreakPots() && logic->LoweredBotwWater) || logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        Entrance(RR_BOTW_ENTRYWAY          , []{return logic->IsChild && logic->CanPassEnemy(RE_BIG_SKULLTULA);}),
        Entrance(RR_BOTW_REQUIRES_LENS     , []{return ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH);}),
        Entrance(RR_BOTW_SOUTHWEST_ROOM    , []{return ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH);}),
        Entrance(RR_BOTW_3_HIDDEN_PITS_ROOM, []{return logic->IsChild && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3);}),
        Entrance(RR_BOTW_COFFIN_ROOM       , []{return logic->LoweredBotwWater || logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_BOTW_DEAD_HAND_ROOM    , []{return logic->LoweredBotwWater && logic->IsChild;}),
        //Falling down into basement requires nothing, but falling down somewhere specific requires lens or lens trick
        //kinda questionable given several drops are blocked by rocks, but that's how it was handled before and on N64
        Entrance(RR_BOTW_B3                , []{return true;}),
        Entrance(RR_BOTW_OUTER_CAGE        , []{return false;}),
    });

    //This region combines the Middle with the perimeter's hidden areas. If a warp puts link into the middle without crossing the perimeter or using lens, it will need it's own region
    areaTable[RR_BOTW_REQUIRES_LENS] = Region("Bottom of the Well Requires Lens", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_BOTW_WEST_FAKE_WALL_CHEST    , true),
        LOCATION(RC_BOTW_EAST_FAKE_WALL_CHEST    , true),
        LOCATION(RC_BOTW_WOODEN_X_ROOM_WEST_CHEST, true),
        //N64 has no extra check here, but I can't get past without dealing with the spider or taking a hit
        LOCATION(RC_BOTW_WOODEN_X_ROOM_EAST_CHEST, logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->TakeDamage()),
        //Not technically behind a wall, but still logically needs lens due to pits
        LOCATION(RC_BOTW_NORTH_BURIED_CHEST      , logic->HasExplosives()),
    }, {
        //Exits
        Entrance(RR_BOTW_LOBBY      , []{return ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH);}),
        Entrance(RR_BOTW_INNER_ROOMS, []{return logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3);}),
        Entrance(RR_BOTW_B3         , []{return true;}),
        Entrance(RR_BOTW_B3_PLATFORM, []{return ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH);}),
    });

    //This area can be reached without lens in logic from basement, but that could require silver rupees if they are shuffled.
    areaTable[RR_BOTW_SOUTHWEST_ROOM] = Region("Bottom of the Well Southwest Room", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_BOTW_FAKE_WALL_NORTH_POT  , logic->CanBreakPots()),
        LOCATION(RC_BOTW_FAKE_WALL_CENTRAL_POT, logic->CanBreakPots()),
        LOCATION(RC_BOTW_FAKE_WALL_SOUTH_POT  , logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_BOTW_LOBBY, []{return ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH);}),
    });

    //Passing through this area needs lens, but entering doesn't, so that the fire keese can be killed without crossing the pits if enemy drops are ever shuffled
    areaTable[RR_BOTW_3_HIDDEN_PITS_ROOM] = Region("Bottom of the Well 3 Hidden Pits Room", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_BOTW_3_HIDDEN_PITS_CHEST, ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        LOCATION(RC_BOTW_3_HIDDEN_PITS_POT  , logic->CanBreakPots() && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))),
    }, {
        //Exits
        Entrance(RR_BOTW_LOBBY               , []{return logic->IsChild && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH));}),
        Entrance(RR_BOTW_OUTER_CAGE          , []{return ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH);}),
        //not sure if this lens check is needed, these holes are a bit too easy to find, but it matches existing logic
        Entrance(RR_BOTW_B3_EAST_BOMB_FLOWERS, []{return ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH);}),
    });

    areaTable[RR_BOTW_OUTER_CAGE] = Region("Bottom of the Well Outer Cage", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_BOTW_OUTER_CAGE_CHEST, true),
        LOCATION(RC_BOTW_OUTER_CAGE_GS   , logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        Entrance(RR_BOTW_3_HIDDEN_PITS_ROOM, []{return true;}),
        Entrance(RR_BOTW_LOBBY             , []{return false;}),
    });

    //If the player can voidwarp into one of these rooms they will need splitting up, and Fake walls will need specifying into middle and the rest moved to perimeter
    areaTable[RR_BOTW_INNER_ROOMS] = Region("Bottom of the Well Inner Rooms", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->DekuBabaSticks, []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(&logic->DekuBabaNuts  , []{return logic->CanGetDekuBabaNuts();}),
    }, {
        //Locations
        LOCATION(RC_BOTW_INNER_WEST_ROOM_GS, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        LOCATION(RC_BOTW_INVISIBLE_LEDGE_GS, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        Entrance(RR_BOTW_REQUIRES_LENS, []{return logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3);}),
    });

    areaTable[RR_BOTW_COFFIN_ROOM] = Region("Bottom of the Well Coffin Room", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_BOTW_COFFIN_KEY       , logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)),
        LOCATION(RC_BOTW_OPEN_COFFIN_HEART, true),
        LOCATION(RC_BOTW_SE_COFFIN_HEART  , logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Exits
        Entrance(RR_BOTW_LOBBY, []{return logic->LoweredBotwWater || logic->HasItem(RG_BRONZE_SCALE);}),
    });

    areaTable[RR_BOTW_DEAD_HAND_ROOM] = Region("Bottom of the Well Dead Hand Room", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_BOTW_DEAD_HAND_CHEST, logic->CanKillEnemy(RE_DEAD_HAND)),
        LOCATION(RC_BOTW_INVISIBLE_CHEST, (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))),
    }, {
        //Exits
        //This assumes we spawned in dead hand's room, if whatever trick made this relevant instead puts us in the previous room, remove the kill Dead Hand check.
        Entrance(RR_BOTW_LOBBY, []{return logic->IsChild && logic->CanKillEnemy(RE_DEAD_HAND);}),
    });

    areaTable[RR_BOTW_B3] = Region("Bottom of the Well B3", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_BOTW_B3_CHEST          , logic->BlastOrSmash()),
        LOCATION(RC_BOTW_B3_WEST_CENTER_POT, logic->CanBreakPots()),
        LOCATION(RC_BOTW_B3_EAST_CENTER_POT, logic->CanBreakPots()),
        LOCATION(RC_BOTW_B3_WEST_V_N_POT   , logic->CanBreakPots()),
        LOCATION(RC_BOTW_B3_WEST_V_NW_POT  , logic->CanBreakPots()),
        LOCATION(RC_BOTW_B3_WEST_V_W_POT   , logic->CanBreakPots()),
        LOCATION(RC_BOTW_B3_WEST_V_SW_POT  , logic->CanBreakPots()),
        LOCATION(RC_BOTW_B3_WEST_V_S_POT   , logic->CanBreakPots()),
        LOCATION(RC_BOTW_B3_EAST_V_N_POT   , logic->CanBreakPots()),
        LOCATION(RC_BOTW_B3_EAST_V_NW_POT  , logic->CanBreakPots()),
        LOCATION(RC_BOTW_B3_EAST_V_W_POT   , logic->CanBreakPots()),
        LOCATION(RC_BOTW_B3_EAST_V_S_POT   , logic->CanBreakPots()),
        LOCATION(RC_BOTW_B3_EAST_V_SW_POT  , logic->CanBreakPots()),
        LOCATION(RC_BOTW_B3_SUN_FAIRY      , logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        Entrance(RR_BOTW_SOUTHWEST_ROOM      , []{return true;}),
        //It's possible to abuse the boulder's limited range of collision detection to detonate the flowers through the boulder with bow, but this is a glitch
        //the exact range is just past the furthest away plank in the green goo section
        Entrance(RR_BOTW_B3_EAST_BOMB_FLOWERS, []{return Here(RR_BOTW_B3, []{return logic->BlastOrSmash() || logic->CanUse(RG_DINS_FIRE) || (logic->CanUse(RG_STICKS) && ctx->GetTrickOption(RT_BOTW_BASEMENT));});}),
    });

    areaTable[RR_BOTW_B3_EAST_BOMB_FLOWERS] = Region("Bottom of the Well B3 East Bomb Flowers", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        //Assumes RR_BOTW_B3 access
        LOCATION(RC_BOTW_B3_CHEST, logic->HasItem(RG_GORONS_BRACELET)),
    }, {
        //Exits
        Entrance(RR_BOTW_B3, []{return logic->CanDetonateUprightBombFlower();}),
    });

    areaTable[RR_BOTW_B3_PLATFORM] = Region("Bottom of the Well B3 Platform", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_BOTW_B3_PLATFORM_N_RUPEE , true),
        LOCATION(RC_BOTW_B3_PLATFORM_NW_RUPEE, true),
        LOCATION(RC_BOTW_B3_PLATFORM_NE_RUPEE, true),
        LOCATION(RC_BOTW_B3_PLATFORM_W_RUPEE , true),
        LOCATION(RC_BOTW_B3_PLATFORM_E_RUPEE , true),
    }, {
        //Exits
        Entrance(RR_BOTW_B3, []{return true;}),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_BOTW_MQ_LOBBY] = Region("Bottom of the Well MQ Lobby", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //technically obsolete due to a wonder item fairy which only needs a projectile, but we don't have an event var for it yet
        EventAccess(&logic->FairyPot        , []{return Here(RR_BOTW_MQ_LOBBY, []{return logic->BlastOrSmash();}) && logic->CanHitEyeTargets();}),
        //It is possible to hit the water switch with a pot from RR_BOTW_MQ_WOODEN_X_ROOM, however the hitbox for making it activate is very unintuitive
        //You have to throw the pot from further back to hit the switch from the front instead of the top, trying to hit the "fingers" directly
        //This unintuitiveness means it should be a trick. ZL is needed to get a clear path to carry the pot
        EventAccess(&logic->LoweredBotwWater, []{return logic->CanJumpslash() || logic->CanUseProjectile();}),
    }, {
        //Locations
        //Implies CanBreakPots()
        LOCATION(RC_BOTW_MQ_CAGED_POT     , Here(RR_BOTW_MQ_LOBBY, []{return logic->BlastOrSmash();}) && logic->CanHitEyeTargets()),
        LOCATION(RC_BOTW_MQ_N_BURIED_HEART, logic->HasExplosives()),
        LOCATION(RC_BOTW_MQ_S_BURIED_HEART, logic->HasExplosives()),
    }, {
        //Exits
        Entrance(RR_BOTW_ENTRYWAY         , []{return logic->IsChild;}),
        Entrance(RR_BOTW_MQ_WEST_SWITCH   , []{return Here(RR_BOTW_MQ_LOBBY, []{return logic->BlastOrSmash();}) && logic->CanPassEnemy(RE_BIG_SKULLTULA);}),
        Entrance(RR_BOTW_MQ_COFFIN_ROOM   , []{return (logic->LoweredBotwWater || logic->HasItem(RG_BRONZE_SCALE)) && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 2);}),
        Entrance(RR_BOTW_MQ_OUTER_CAGE    , []{return logic->IsChild && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 2) && logic->CanUseProjectile();}),
        Entrance(RR_BOTW_MQ_DEAD_HAND_ROOM, []{return logic->IsChild && logic->LoweredBotwWater;}),
        Entrance(RR_BOTW_MQ_WOODEN_X_ROOM , []{return logic->CanUse(RG_ZELDAS_LULLABY);}),
        Entrance(RR_BOTW_MQ_B3            , []{return true;}),
    });

    areaTable[RR_BOTW_MQ_WEST_SWITCH] = Region("Bottom of the Well MQ West Switch", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->OpenedWestRoomMQBotw, []{return true;}),
    }, {}, {
        //Exits
        Entrance(RR_BOTW_MQ_LOBBY        , []{return logic->BlastOrSmash() && (logic->CanPassEnemy(RE_BIG_SKULLTULA) || ctx->GetTrickOption(RT_BOTW_MQ_PITS));}),
        Entrance(RR_BOTW_MQ_WOODEN_X_ROOM, []{return (bool)ctx->GetTrickOption(RT_BOTW_MQ_PITS);}),
        Entrance(RR_BOTW_MQ_B3           , []{return true;}),
    });

    areaTable[RR_BOTW_MQ_COFFIN_ROOM] = Region("Bottom of the Well MQ Coffin Room", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_BOTW_MQ_COFFIN_ROOM_GS , logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
        LOCATION(RC_BOTW_MQ_NE_COFFIN_HEART, logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)),
        LOCATION(RC_BOTW_MQ_S_COFFIN_HEART , logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Exits
        Entrance(RR_BOTW_MQ_LOBBY,  []{return (logic->LoweredBotwWater || logic->HasItem(RG_BRONZE_SCALE)) && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 2);}),
    });

    areaTable[RR_BOTW_MQ_OUTER_CAGE] = Region("Bottom of the Well MQ Outer Cage", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->OpenedMiddleHoleMQBotw, []{return logic->HasExplosives();}),
    }, {}, {
        //Exits
        Entrance(RR_BOTW_MQ_LOBBY, []{return logic->IsChild && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 2);}),
    });

    areaTable[RR_BOTW_MQ_DEAD_HAND_ROOM] = Region("Bottom of the Well MQ Dead Hand Room", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_BOTW_MQ_DEAD_HAND_CHEST     , logic->CanKillEnemy(RE_DEAD_HAND)),
        LOCATION(RC_BOTW_MQ_DEAD_HAND_BURIED_KEY, logic->HasExplosives() || (ctx->GetTrickOption(RT_BOTW_MQ_DEADHAND_KEY) && logic->CanUse(RG_BOOMERANG))),
    }, {
        //Exits
        //This assumes we spawned in dead hand's room, if whatever trick made this relevant instead puts us in the previous room, remove the kill Dead Hand check.
        Entrance(RR_BOTW_MQ_LOBBY, []{return logic->IsChild && logic->CanKillEnemy(RE_DEAD_HAND);}),
    });

    areaTable[RR_BOTW_MQ_WOODEN_X_ROOM] = Region("Bottom of the Well MQ Wooden X Room", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_BOTW_MQ_WOODEN_X_CHEST        , true),
        //This location technically involves an invisible platform, but it's intended to do lensless in vanilla and is clearly signposted by pots.
        LOCATION(RC_BOTW_MQ_INVISIBLE_LEDGE_KEY   , true),
        //The enemies in this room are invisible and crowd around the player, being awkward to deal with blind unless you already know how.
        //the right wall is safe, and can be followed to get behind the grave which you can then pull easily assuming you can tank invisible keese
        //Using a deku nut however stuns everything easily. and if you have a melee weapon you can kill the skull through the grave then grab the drop
        //though it can be hard to tell where the safe direct path to the grave is without lens.
        //Also you get cheap shotted on entry sometimes.
        //An MQ lens trick is recommended here, and a review of this room for OHKO logic what that is added is advised.
        //In the meantime I assume damage taken or the easy answer (nuts)
        LOCATION(RC_BOTW_MQ_UNDER_GRAVE_GS        , logic->OpenedWestRoomMQBotw && (logic->TakeDamage() || logic->CanUse(RG_NUTS)) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
        LOCATION(RC_BOTW_MQ_WOODEN_X_ROOM_W_POT   , logic->CanBreakPots()),
        LOCATION(RC_BOTW_MQ_WOODEN_X_ROOM_C_POT   , logic->CanBreakPots()),
        LOCATION(RC_BOTW_MQ_WOODEN_X_ROOM_E_POT   , logic->CanBreakPots()),
        LOCATION(RC_BOTW_MQ_INVISIBLE_LEDGE_NW_POT, logic->CanBreakPots()),
        LOCATION(RC_BOTW_MQ_INVISIBLE_LEDGE_NE_POT, logic->CanBreakPots()),
        LOCATION(RC_BOTW_MQ_INVISIBLE_LEDGE_S_POT , logic->CanBreakPots()),
        LOCATION(RC_BOTW_MQ_NEAR_POTS_SUN_FAIRY   , logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        //If a relevant trick causes you to be able to warp into here without going through LOBBY, a new eventAccess will be needed for lowering the gates with ZL
        Entrance(RR_BOTW_MQ_B3_SWITCH, []{return logic->OpenedMiddleHoleMQBotw;}),
        Entrance(RR_BOTW_MQ_B3       , []{return true;}),
    });

    areaTable[RR_BOTW_MQ_B3] = Region("Bottom of the Well MQ B3", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        //behind invisible big skulltulas, but with navi spotting it's easy to avoid them, or at worst, tank your way through as they do not block the path
        LOCATION(RC_BOTW_MQ_B3_GS       , logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
        LOCATION(RC_BOTW_MQ_B3_S_HEART  , true),
        LOCATION(RC_BOTW_MQ_B3_NW_HEART , true),
        LOCATION(RC_BOTW_MQ_B3_NE_HEART , true),
        LOCATION(RC_BOTW_MQ_B3_SUN_FAIRY, logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        Entrance(RR_BOTW_MQ_LOBBY, []{return true;}),
    });

    areaTable[RR_BOTW_MQ_B3_SWITCH] = Region("Bottom of the Well MQ B3 Switch", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        //Assumes RR_BOTW_MQ_B3 access
        //it is technically possible to get the chest before you get screamed at without rolling, but hard enough to be a trick if that is the requirement for something to be logical
        //With some kind of movement tech it's much easier, easy enough to be default logic, as the redeads don't lock on immediately in addition to the extra speed
        //leaving with no requirements for now but up for discussion.
        LOCATION(RC_BOTW_MQ_REDEAD_CHEST, true),
    }, {
        //Exits
        Entrance(RR_BOTW_MQ_B3, []{return true;}),
    });

#pragma endregion
}