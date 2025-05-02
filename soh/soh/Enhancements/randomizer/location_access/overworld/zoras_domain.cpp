#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_ZorasDomain() {
    // clang-format off
    areaTable[RR_ZORAS_DOMAIN] = Region("Zoras Domain", "Zoras Domain", {RA_ZORAS_DOMAIN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GossipStoneFairy, []{return logic->CallGossipFairyExceptSuns();}),
        EventAccess(&logic->NutPot,           []{return true;}),
        EventAccess(&logic->StickPot,         []{return logic->StickPot          || logic->IsChild;}),
        EventAccess(&logic->FishGroup,        []{return logic->FishGroup         || logic->IsChild;}),
        EventAccess(&logic->KingZoraThawed,   []{return logic->KingZoraThawed    || (logic->IsAdult     && logic->BlueFire());}),
        EventAccess(&logic->DeliverLetter,    []{return logic->DeliverLetter     || (logic->CanUse(RG_RUTOS_LETTER) && logic->IsChild && ctx->GetOption(RSK_ZORAS_FOUNTAIN).IsNot(RO_ZF_OPEN));}),
    }, {
        //Locations
        LOCATION(RC_ZD_DIVING_MINIGAME,                     logic->HasItem(RG_BRONZE_SCALE) && logic->HasItem(RG_CHILD_WALLET) && logic->IsChild),
        LOCATION(RC_ZD_CHEST,                               logic->IsChild && logic->CanUse(RG_STICKS)),
        LOCATION(RC_ZD_KING_ZORA_THAWED,                    logic->IsAdult && logic->KingZoraThawed),
        LOCATION(RC_ZD_TRADE_PRESCRIPTION,                  logic->IsAdult && logic->KingZoraThawed && logic->CanUse(RG_PRESCRIPTION)),
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
        Entrance(RR_LAKE_HYLIA,          []{return logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS));}),
        Entrance(RR_ZD_BEHIND_KING_ZORA, []{return logic->DeliverLetter || ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_OPEN) || (ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_CLOSED_CHILD) && logic->IsAdult) || (ctx->GetTrickOption(RT_ZD_KING_ZORA_SKIP) && logic->IsAdult);}),
        Entrance(RR_ZD_SHOP,             []{return logic->IsChild || logic->BlueFire();}),
        Entrance(RR_ZORAS_DOMAIN_ISLAND, []{return true;}),
    });

    areaTable[RR_ZORAS_DOMAIN_ISLAND] = Region("Zoras Domain Island", "Zoras Domain", {RA_ZORAS_DOMAIN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_ZORAS_DOMAIN,     []{return logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_ZD_STORMS_GROTTO, []{return logic->CanOpenStormsGrotto();}),
    });

    areaTable[RR_ZD_BEHIND_KING_ZORA] = Region("ZD Behind King Zora", "Zoras Domain", {RA_ZORAS_DOMAIN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->KingZoraThawed, []{return logic->IsAdult && logic->BlueFire();}),
    }, {
        //Locations
        LOCATION(RC_ZD_BEHIND_KING_ZORA_BEEHIVE, logic->IsChild && logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        Entrance(RR_ZORAS_DOMAIN,   []{return logic->DeliverLetter || ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_OPEN) || (ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_CLOSED_CHILD) && logic->IsAdult);}),
        Entrance(RR_ZORAS_FOUNTAIN, []{return true;}),
    });

    areaTable[RR_ZD_SHOP] = Region("ZD Shop", "ZD Shop", {}, NO_DAY_NIGHT_CYCLE, {}, {
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

    areaTable[RR_ZD_STORMS_GROTTO] = Region("ZD Storms Grotto", "ZD Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->FreeFairies, []{return true;}),
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
