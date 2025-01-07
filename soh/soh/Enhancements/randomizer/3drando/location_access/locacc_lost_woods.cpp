#include "../location_access.hpp"
#include "../../entrance.h"

using namespace Rando;

void RegionTable_Init_LostWoods() {
  areaTable[RR_KOKIRI_FOREST] = Region("Kokiri Forest", "Kokiri Forest", {RA_KOKIRI_FOREST}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BeanPlantFairy,           {[]{return logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS);}}),
                  EventAccess(&logic->GossipStoneFairy,         {[]{return logic->CallGossipFairyExceptSuns();}}),
                  EventAccess(&logic->ShowedMidoSwordAndShield, {[]{return logic->ShowedMidoSwordAndShield || (logic->IsChild && logic->CanUse(RG_KOKIRI_SWORD) && logic->CanUse(RG_DEKU_SHIELD));}}),
                }, {
                  //Locations
                  LOCATION(RC_KF_KOKIRI_SWORD_CHEST,      logic->IsChild),
                  LOCATION(RC_KF_GS_KNOW_IT_ALL_HOUSE,    logic->IsChild && logic->CanAttack() && (/*TODO: HasNightStart ||*/ logic->CanLeaveForest() || logic->CanUse(RG_SUNS_SONG)) && logic->CanGetNightTimeGS()),
                  LOCATION(RC_KF_GS_BEAN_PATCH,           logic->CanSpawnSoilSkull() && logic->CanAttack()),
                  LOCATION(RC_KF_GS_HOUSE_OF_TWINS,       logic->IsAdult && (logic->HookshotOrBoomerang() || (ctx->GetTrickOption(RT_KF_ADULT_GS) && logic->CanUse(RG_HOVER_BOOTS))) && logic->CanGetNightTimeGS()),
                  LOCATION(RC_KF_BEAN_SPROUT_FAIRY_1,     logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_KF_BEAN_SPROUT_FAIRY_2,     logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_KF_BEAN_SPROUT_FAIRY_3,     logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_KF_GOSSIP_STONE_FAIRY,      logic->CallGossipFairyExceptSuns()),
                  LOCATION(RC_KF_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_KF_BRIDGE_RUPEE,            logic->IsChild),
                  LOCATION(RC_KF_BEHIND_MIDOS_RUPEE,      logic->IsChild),
                  LOCATION(RC_KF_SOUTH_GRASS_WEST_RUPEE,  logic->IsChild),
                  LOCATION(RC_KF_SOUTH_GRASS_EAST_RUPEE,  logic->IsChild),
                  LOCATION(RC_KF_NORTH_GRASS_WEST_RUPEE,  logic->IsChild),
                  LOCATION(RC_KF_NORTH_GRASS_EAST_RUPEE,  logic->IsChild),
                  LOCATION(RC_KF_BOULDER_RUPEE_1,         logic->IsChild),
                  LOCATION(RC_KF_BOULDER_RUPEE_2,         logic->IsChild),
                  LOCATION(RC_KF_BEAN_RUPEE_1,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
                  LOCATION(RC_KF_BEAN_RUPEE_2,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
                  LOCATION(RC_KF_BEAN_RUPEE_3,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
                  LOCATION(RC_KF_BEAN_RUPEE_4,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
                  LOCATION(RC_KF_BEAN_RUPEE_5,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
                  LOCATION(RC_KF_BEAN_RUPEE_6,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
                  LOCATION(RC_KF_BEAN_RED_RUPEE,          logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
                  LOCATION(RC_KF_SARIAS_ROOF_WEST_HEART,  logic->IsChild),
                  LOCATION(RC_KF_SARIAS_ROOF_EAST_HEART,  logic->IsChild),
                  LOCATION(RC_KF_SARIAS_ROOF_NORTH_HEART, logic->IsChild),
                  LOCATION(RC_KF_GOSSIP_STONE,            true),
                }, {
                  //Exits
                  Entrance(RR_KF_LINKS_HOUSE,        {[]{return true;}}),
                  Entrance(RR_KF_MIDOS_HOUSE,        {[]{return true;}}),
                  Entrance(RR_KF_SARIAS_HOUSE,       {[]{return true;}}),
                  Entrance(RR_KF_HOUSE_OF_TWINS,     {[]{return true;}}),
                  Entrance(RR_KF_KNOW_IT_ALL_HOUSE,  {[]{return true;}}),
                  Entrance(RR_KF_KOKIRI_SHOP,        {[]{return true;}}),
                  Entrance(RR_KF_OUTSIDE_DEKU_TREE,  {[]{return (logic->IsAdult && (logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->ForestTempleClear)) || ctx->GetOption(RSK_FOREST).Is(RO_CLOSED_FOREST_OFF) || logic->ShowedMidoSwordAndShield;}}),
                  Entrance(RR_THE_LOST_WOODS,        {[]{return true;}}),
                  Entrance(RR_LW_BRIDGE_FROM_FOREST, {[]{return logic->IsAdult || ctx->GetOption(RSK_FOREST).IsNot(RO_CLOSED_FOREST_ON) || logic->DekuTreeClear;}}),
                  Entrance(RR_KF_STORMS_GROTTO,      {[]{return logic->CanOpenStormsGrotto();}}),
  });

  areaTable[RR_KF_OUTSIDE_DEKU_TREE] = Region("KF Outside Deku Tree", "Kokiri Forest", {RA_KOKIRI_FOREST}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks,           {[]{return logic->CanKillEnemy(RE_WITHERED_DEKU_BABA);}}),
                  EventAccess(&logic->DekuBabaNuts,             {[]{return logic->CanGetDekuBabaNuts();}}),
                  EventAccess(&logic->ShowedMidoSwordAndShield, {[]{return logic->ShowedMidoSwordAndShield || (logic->IsChild && logic->CanUse(RG_KOKIRI_SWORD) && logic->CanUse(RG_DEKU_SHIELD));}}),
                }, {
                  //Locations
                  LOCATION(RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE_FAIRY,       logic->CallGossipFairyExceptSuns()),
                  LOCATION(RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE_FAIRY_BIG,   logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE_FAIRY,      logic->CallGossipFairyExceptSuns()),
                  LOCATION(RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE,             true),
                  LOCATION(RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE,            true),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_ENTRYWAY, {[]{return logic->IsChild || (ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) && (ctx->GetOption(RSK_FOREST).Is(RO_CLOSED_FOREST_OFF) || logic->ShowedMidoSwordAndShield));}}),
                  Entrance(RR_KOKIRI_FOREST,      {[]{return (logic->IsAdult && (logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->ForestTempleClear)) || ctx->GetOption(RSK_FOREST).Is(RO_CLOSED_FOREST_OFF) || logic->ShowedMidoSwordAndShield;}}),
  });

  areaTable[RR_KF_LINKS_HOUSE] = Region("KF Link's House", "KF Link's House", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_KF_LINKS_HOUSE_COW, logic->IsAdult && logic->CanUse(RG_EPONAS_SONG) && logic->LinksCow),
                  LOCATION(RC_KF_LINKS_HOUSE_POT, logic->CanBreakPots()),
                }, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}})
  });

  areaTable[RR_KF_MIDOS_HOUSE] = Region("KF Mido's House", "KF Mido's House", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_KF_MIDOS_TOP_LEFT_CHEST,     true),
                  LOCATION(RC_KF_MIDOS_TOP_RIGHT_CHEST,    true),
                  LOCATION(RC_KF_MIDOS_BOTTOM_LEFT_CHEST,  true),
                  LOCATION(RC_KF_MIDOS_BOTTOM_RIGHT_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[RR_KF_SARIAS_HOUSE] = Region("KF Saria's House", "KF Saria's House", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_KF_SARIAS_TOP_LEFT_HEART,     true),
                  LOCATION(RC_KF_SARIAS_TOP_RIGHT_HEART,    true),
                  LOCATION(RC_KF_SARIAS_BOTTOM_LEFT_HEART,  true),
                  LOCATION(RC_KF_SARIAS_BOTTOM_RIGHT_HEART, true),
                }, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[RR_KF_HOUSE_OF_TWINS] = Region("KF House of Twins", "KF House of Twins", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_KF_TWINS_HOUSE_POT_1, logic->CanBreakPots()),
                  LOCATION(RC_KF_TWINS_HOUSE_POT_2, logic->CanBreakPots()),
                }, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[RR_KF_KNOW_IT_ALL_HOUSE] = Region("KF Know It All House", "KF Know It All House", {}, NO_DAY_NIGHT_CYCLE, {}, {
                 // Locations
                 LOCATION(RC_KF_BROTHERS_HOUSE_POT_1, logic->CanBreakPots()),
                 LOCATION(RC_KF_BROTHERS_HOUSE_POT_2, logic->CanBreakPots()),
                }, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[RR_KF_KOKIRI_SHOP] = Region("KF Kokiri Shop", "KF Kokiri Shop", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_KF_SHOP_ITEM_1, true),
                  LOCATION(RC_KF_SHOP_ITEM_2, true),
                  LOCATION(RC_KF_SHOP_ITEM_3, true),
                  LOCATION(RC_KF_SHOP_ITEM_4, true),
                  LOCATION(RC_KF_SHOP_ITEM_5, true),
                  LOCATION(RC_KF_SHOP_ITEM_6, true),
                  LOCATION(RC_KF_SHOP_ITEM_7, true),
                  LOCATION(RC_KF_SHOP_ITEM_8, true),
                }, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[RR_KF_STORMS_GROTTO] = Region("KF Storms Grotto", "KF Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_KF_STORMS_GROTTO_CHEST,         true),
                  LOCATION(RC_KF_STORMS_GROTTO_FISH,          logic->HasBottle()),
                  LOCATION(RC_KF_STORMS_GROTTO_GOSSIP_STONE_FAIRY,      logic->CallGossipFairy()),
                  LOCATION(RC_KF_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_KF_STORMS_GROTTO_GOSSIP_STONE,  true),
                  LOCATION(RC_KF_STORMS_GROTTO_BEEHIVE_LEFT,  logic->CanBreakLowerBeehives()),
                  LOCATION(RC_KF_STORMS_GROTTO_BEEHIVE_RIGHT, logic->CanBreakLowerBeehives()),
                }, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}})
  });

  areaTable[RR_LW_FOREST_EXIT] = Region("LW Forest Exit", "Lost Woods", {RA_THE_LOST_WOODS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}})
  });

  areaTable[RR_THE_LOST_WOODS] = Region("Lost Woods", "Lost Woods", {RA_THE_LOST_WOODS}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy,  {[]{return logic->CallGossipFairyExceptSuns();}}),
                  EventAccess(&logic->BeanPlantFairy,    {[]{return logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS);}}),
                  EventAccess(&logic->BugShrub,          {[]{return logic->IsChild && logic->CanCutShrubs();}}),
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
                }, {
                  //Exits
                  Entrance(RR_LW_FOREST_EXIT,           {[]{return true;}}),
                  Entrance(RR_GC_WOODS_WARP,            {[]{return true;}}),
                  Entrance(RR_LW_BRIDGE,                {[]{return logic->CanLeaveForest() && ((logic->IsAdult && (CanPlantBean(RR_THE_LOST_WOODS) || ctx->GetTrickOption(RT_LW_BRIDGE))) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT));}}),
                  Entrance(RR_ZORAS_RIVER,              {[]{return logic->CanLeaveForest() && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_LW_BEYOND_MIDO,           {[]{return logic->IsChild || logic->CanUse(RG_SARIAS_SONG) || ctx->GetTrickOption(RT_LW_MIDO_BACKFLIP);}}),
                  Entrance(RR_LW_NEAR_SHORTCUTS_GROTTO, {[]{return Here(RR_THE_LOST_WOODS, []{return logic->BlastOrSmash();});}}),
  });

  areaTable[RR_LW_BEYOND_MIDO] = Region("LW Beyond Mido", "Lost Woods", {RA_THE_LOST_WOODS}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ButterflyFairy, {[]{return logic->ButterflyFairy || logic->CanUse(RG_STICKS);}}),
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
                }, {
                  //Exits
                  Entrance(RR_LW_FOREST_EXIT,   {[]{return true;}}),
                  Entrance(RR_THE_LOST_WOODS,   {[]{return logic->IsChild || logic->CanUse(RG_SARIAS_SONG);}}),
                  Entrance(RR_SFM_ENTRYWAY,     {[]{return true;}}),
                  Entrance(RR_DEKU_THEATER,     {[]{return true;}}),
                  Entrance(RR_LW_SCRUBS_GROTTO, {[]{return Here(RR_LW_BEYOND_MIDO, []{return logic->BlastOrSmash();});}}),
  });

  areaTable[RR_LW_NEAR_SHORTCUTS_GROTTO] = Region("LW Near Shortcuts Grotto", "LW Near Shortcuts Grotto", {}, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST,                   true),
                  LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_FISH,                    logic->HasBottle()),
                  LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE_FAIRY,      logic->CallGossipFairy()),
                  LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE,            true),
                  LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_BEEHIVE_LEFT,            logic->CanBreakLowerBeehives()),
                  LOCATION(RC_LW_NEAR_SHORTCUTS_GROTTO_BEEHIVE_RIGHT,           logic->CanBreakLowerBeehives()),
                }, {
                  //Exits
                  Entrance(RR_THE_LOST_WOODS, {[]{return true;}}),
  });

  areaTable[RR_DEKU_THEATER] = Region("Deku Theater", "Deku Theater", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DEKU_THEATER_SKULL_MASK,    logic->IsChild && logic->SkullMask),
                  LOCATION(RC_DEKU_THEATER_MASK_OF_TRUTH, logic->IsChild && logic->MaskOfTruth),
                }, {
                  //Exits
                  Entrance(RR_LW_BEYOND_MIDO, {[]{return true;}}),
  });

  areaTable[RR_LW_SCRUBS_GROTTO] = Region("LW Scrubs Grotto", "LW Scrubs Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LW_DEKU_SCRUB_GROTTO_REAR,    logic->CanStunDeku()),
                  LOCATION(RC_LW_DEKU_SCRUB_GROTTO_FRONT,   logic->CanStunDeku()),
                  LOCATION(RC_LW_DEKU_SCRUB_GROTTO_BEEHIVE, logic->CanBreakUpperBeehives()),
                }, {
                  //Exits
                  Entrance(RR_LW_BEYOND_MIDO, {[]{return true;}}),
  });

  areaTable[RR_SFM_ENTRYWAY] = Region("SFM Entryway", "Sacred Forest Meadow", {RA_SACRED_FOREST_MEADOW}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_LW_BEYOND_MIDO,       {[]{return true;}}),
                  Entrance(RR_SACRED_FOREST_MEADOW, {[]{return logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE);}}),
                  Entrance(RR_SFM_WOLFOS_GROTTO,    {[]{return logic->CanOpenBombGrotto();}}),
  });

  areaTable[RR_SACRED_FOREST_MEADOW] = Region("Sacred Forest Meadow", "Sacred Forest Meadow", {RA_SACRED_FOREST_MEADOW}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->CallGossipFairyExceptSuns();}}),
                }, {
                  //Locations
                  LOCATION(RC_SONG_FROM_SARIA,                        logic->IsChild && logic->HasItem(RG_ZELDAS_LETTER)),
                  LOCATION(RC_SHEIK_IN_FOREST,                        logic->IsAdult),
                  LOCATION(RC_SFM_GS,                                 logic->IsAdult && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
                  LOCATION(RC_SFM_MAZE_LOWER_GOSSIP_STONE_FAIRY,      logic->CallGossipFairyExceptSuns()),
                  LOCATION(RC_SFM_MAZE_LOWER_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_SFM_MAZE_UPPER_GOSSIP_STONE_FAIRY,      logic->CallGossipFairyExceptSuns()),
                  LOCATION(RC_SFM_MAZE_UPPER_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_SFM_SARIA_GOSSIP_STONE_FAIRY,           logic->CallGossipFairyExceptSuns()),
                  LOCATION(RC_SFM_SARIA_GOSSIP_STONE_FAIRY_BIG,       logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_SFM_MAZE_LOWER_GOSSIP_STONE,            true),
                  LOCATION(RC_SFM_MAZE_UPPER_GOSSIP_STONE,            true),
                  LOCATION(RC_SFM_SARIA_GOSSIP_STONE,                 true),
                }, {
                  //Exits
                  Entrance(RR_SFM_ENTRYWAY,           {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_ENTRYWAY, {[]{return logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_SFM_FAIRY_GROTTO,       {[]{return true;}}),
                  Entrance(RR_SFM_STORMS_GROTTO,      {[]{return logic->CanOpenStormsGrotto();}}),
  });

  areaTable[RR_SFM_FAIRY_GROTTO] = Region("SFM Fairy Grotto", "SFM Fairy Grotto", {}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FreeFairies, {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_1, true),
                  LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_2, true),
                  LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_3, true),
                  LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_4, true),
                  LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_5, true),
                  LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_6, true),
                  LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_7, true),
                  LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_8, true),
                }, {
                  //Exits
                  Entrance(RR_SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[RR_SFM_WOLFOS_GROTTO] = Region("SFM Wolfos Grotto", "SFM Wolfos Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SFM_WOLFOS_GROTTO_CHEST, logic->CanKillEnemy(RE_WOLFOS, ED_CLOSE, true, 2)),
                }, {
                  //Exits
                  Entrance(RR_SFM_ENTRYWAY, {[]{return true;}}),
  });

  areaTable[RR_SFM_STORMS_GROTTO] = Region("SFM Storms Grotto", "SFM Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SFM_DEKU_SCRUB_GROTTO_REAR,  logic->CanStunDeku()),
                  LOCATION(RC_SFM_DEKU_SCRUB_GROTTO_FRONT, logic->CanStunDeku()),
                  LOCATION(RC_SFM_STORMS_GROTTO_BEEHIVE,   logic->CanBreakUpperBeehives()),
                }, {
                  //Exits
                  Entrance(RR_SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[RR_LW_BRIDGE_FROM_FOREST] = Region("LW Bridge From Forest", "Lost Woods", {RA_THE_LOST_WOODS}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LW_GIFT_FROM_SARIA, true),
                }, {
                  //Exits
                  Entrance(RR_LW_BRIDGE, {[]{return true;}}),
  });

  areaTable[RR_LW_BRIDGE] = Region("LW Bridge", "Lost Woods", {RA_THE_LOST_WOODS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST,  {[]{return true;}}),
                  Entrance(RR_HYRULE_FIELD,   {[]{return true;}}),
                  Entrance(RR_THE_LOST_WOODS, {[]{return logic->CanUse(RG_LONGSHOT);}}),
  });
}
