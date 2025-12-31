#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_ZorasDomain() {
    // clang-format off
    areaTable[RR_ZORAS_DOMAIN] = Region("Zoras Domain", SCENE_ZORAS_DOMAIN, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS,         []{return logic->CallGossipFairyExceptSuns();}),
        EventAccess(LOGIC_NUT_ACCESS,           []{return true;}),
        EventAccess(LOGIC_STICK_ACCESS,         []{return logic->IsChild;}),
        EventAccess(LOGIC_FISH_ACCESS,          []{return logic->IsChild;}),
        EventAccess(LOGIC_KING_ZORA_THAWED,     []{return logic->IsAdult && logic->BlueFire();}),
        EventAccess(LOGIC_DELIVER_RUTOS_LETTER, []{return logic->CanUse(RG_RUTOS_LETTER) && logic->IsChild && ctx->GetOption(RSK_ZORAS_FOUNTAIN).IsNot(RO_ZF_OPEN);}),
    }, {
        //Locations
        LOCATION(RC_ZD_DIVING_MINIGAME,                     logic->HasItem(RG_BRONZE_SCALE) && logic->HasItem(RG_CHILD_WALLET) && logic->IsChild),
        LOCATION(RC_ZD_CHEST,                               logic->IsChild && logic->CanUse(RG_STICKS)),
        LOCATION(RC_ZD_KING_ZORA_THAWED,                    logic->IsAdult && logic->Get(LOGIC_KING_ZORA_THAWED)),
        LOCATION(RC_ZD_TRADE_PRESCRIPTION,                  logic->IsAdult && logic->Get(LOGIC_KING_ZORA_THAWED) && logic->CanUse(RG_PRESCRIPTION)),
        LOCATION(RC_ZD_GS_FROZEN_WATERFALL,                 logic->IsAdult && (logic->HookshotOrBoomerang() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || (logic->CanUse(RG_MAGIC_SINGLE) && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))) || (ctx->GetTrickOption(RT_ZD_GS) && logic->CanJumpslashExceptHammer())) && logic->CanGetNightTimeGS()),
        LOCATION(RC_ZD_FISH_1,                              logic->IsChild && logic->HasBottle()),
        LOCATION(RC_ZD_FISH_2,                              logic->IsChild && logic->HasBottle()),
        LOCATION(RC_ZD_FISH_3,                              logic->IsChild && logic->HasBottle()),
        LOCATION(RC_ZD_FISH_4,                              logic->IsChild && logic->HasBottle()),
        LOCATION(RC_ZD_FISH_5,                              logic->IsChild && logic->HasBottle()),
        LOCATION(RC_ZD_GOSSIP_STONE_FAIRY,                  logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_ZD_GOSSIP_STONE_FAIRY_BIG,              logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZD_GOSSIP_STONE,                        true),
        LOCATION(RC_ZD_IN_FRONT_OF_KING_ZORA_BEEHIVE_LEFT,  logic->IsChild && logic->CanBreakUpperBeehives()),
        LOCATION(RC_ZD_IN_FRONT_OF_KING_ZORA_BEEHIVE_RIGHT, logic->IsChild && logic->CanBreakUpperBeehives()),
        LOCATION(RC_ZD_NEAR_SHOP_POT_1,                     logic->CanBreakPots()),
        LOCATION(RC_ZD_NEAR_SHOP_POT_2,                     logic->CanBreakPots()),
        LOCATION(RC_ZD_NEAR_SHOP_POT_3,                     logic->CanBreakPots()),
        LOCATION(RC_ZD_NEAR_SHOP_POT_4,                     logic->CanBreakPots()),
        LOCATION(RC_ZD_NEAR_SHOP_POT_5,                     logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_ZR_BEHIND_WATERFALL, []{return true;}),
        Entrance(RR_LH_FROM_SHORTCUT,    []{return logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS));}),
        Entrance(RR_ZD_BEHIND_KING_ZORA, []{return logic->Get(LOGIC_DELIVER_RUTOS_LETTER) || ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_OPEN) || (ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_CLOSED_CHILD) && logic->IsAdult) || (ctx->GetTrickOption(RT_ZD_KING_ZORA_SKIP) && logic->IsAdult);}),
        Entrance(RR_ZD_SHOP,             []{return logic->IsChild || logic->BlueFire();}),
        Entrance(RR_ZORAS_DOMAIN_ISLAND, []{return true;}),
    });

    areaTable[RR_ZORAS_DOMAIN_ISLAND] = Region("Zoras Domain Island", SCENE_ZORAS_DOMAIN, {}, {}, {
        //Exits
        Entrance(RR_ZORAS_DOMAIN,     []{return logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_ZD_STORMS_GROTTO, []{return logic->CanOpenStormsGrotto();}),
    });

    areaTable[RR_ZD_BEHIND_KING_ZORA] = Region("ZD Behind King Zora", SCENE_ZORAS_DOMAIN, {
        //Events
        EventAccess(LOGIC_KING_ZORA_THAWED, []{return logic->IsAdult && logic->BlueFire();}),
    }, {
        //Locations
        LOCATION(RC_ZD_BEHIND_KING_ZORA_BEEHIVE, logic->IsChild && logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        Entrance(RR_ZORAS_DOMAIN,   []{return logic->Get(LOGIC_DELIVER_RUTOS_LETTER) || ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_OPEN) || (ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_CLOSED_CHILD) && logic->IsAdult);}),
        Entrance(RR_ZORAS_FOUNTAIN, []{return true;}),
    });

    areaTable[RR_ZD_SHOP] = Region("ZD Shop", SCENE_ZORA_SHOP, {}, {
        //Locations
        LOCATION(RC_ZD_SHOP_ITEM_1, true),
        LOCATION(RC_ZD_SHOP_ITEM_2, true),
        LOCATION(RC_ZD_SHOP_ITEM_3, true),
        LOCATION(RC_ZD_SHOP_ITEM_4, true),
        LOCATION(RC_ZD_SHOP_ITEM_5, true),
        LOCATION(RC_ZD_SHOP_ITEM_6, true),
        LOCATION(RC_ZD_SHOP_ITEM_7, true),
        LOCATION(RC_ZD_SHOP_ITEM_8, true),
    }, {
        //Exits
        Entrance(RR_ZORAS_DOMAIN, []{return true;}),
    });

    areaTable[RR_ZD_STORMS_GROTTO] = Region("ZD Storms Grotto", SCENE_GROTTOS, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_ZD_FAIRY_GROTTO_FAIRY_1, true),
        LOCATION(RC_ZD_FAIRY_GROTTO_FAIRY_2, true),
        LOCATION(RC_ZD_FAIRY_GROTTO_FAIRY_3, true),
        LOCATION(RC_ZD_FAIRY_GROTTO_FAIRY_4, true),
        LOCATION(RC_ZD_FAIRY_GROTTO_FAIRY_5, true),
        LOCATION(RC_ZD_FAIRY_GROTTO_FAIRY_6, true),
        LOCATION(RC_ZD_FAIRY_GROTTO_FAIRY_7, true),
        LOCATION(RC_ZD_FAIRY_GROTTO_FAIRY_8, true),
    }, {
        //Exits
        Entrance(RR_ZORAS_DOMAIN_ISLAND, []{return true;}),
    });

    // clang-format on
}
