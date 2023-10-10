#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_DeathMountain() {
  areaTable[RR_DEATH_MOUNTAIN_TRAIL] = Area("Death Mountain", "Death Mountain", RHT_DEATH_MOUNTAIN_TRAIL, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BeanPlantFairy, {[]{return BeanPlantFairy || (CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL) && CanPlay(SongOfStorms) && (HasExplosives || GoronBracelet));}}),
                }, {
                  //Locations
                  LocationAccess(RC_DMT_CHEST,                    {[]{return CanBlastOrSmash || (LogicDMTBombable && IsChild && GoronBracelet);}}),
                  LocationAccess(RC_DMT_FREESTANDING_POH,         {[]{return CanTakeDamage || CanUse(HOVER_BOOTS) || (IsAdult && CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL) && (HasExplosives || GoronBracelet));}}),
                  LocationAccess(RC_DMT_GS_BEAN_PATCH,            {[]{return CanPlantBugs && (HasExplosives || GoronBracelet || (LogicDMTSoilGS && (CanTakeDamage || CanUse(HOVER_BOOTS)) && CanUse(BOOMERANG)));}}),
                  LocationAccess(RC_DMT_GS_NEAR_KAK,              {[]{return CanBlastOrSmash;}}),
                  LocationAccess(RC_DMT_GS_ABOVE_DODONGOS_CAVERN, {[]{return IsAdult && AtNight && (CanUse(MEGATON_HAMMER) || (LogicDMTGSLowerHookshot && CanUse(HOOKSHOT)) || (LogicDMTGSLowerBean && CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL)) || (LogicDMTGSLowerHovers && CanUse(HOVER_BOOTS)) || LogicDMTGSLowerJS) && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(RR_KAK_BEHIND_GATE,          {[]{return true;}}),
                  Entrance(RR_GORON_CITY,               {[]{return true;}}),
                  Entrance(RR_DEATH_MOUNTAIN_SUMMIT,    {[]{return Here(RR_DEATH_MOUNTAIN_TRAIL, []{return CanBlastOrSmash;}) || (IsAdult && ((CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL) && GoronBracelet) || (HoverBoots && LogicDMTClimbHovers)));}}),
                  Entrance(RR_DODONGOS_CAVERN_ENTRYWAY, {[]{return HasExplosives || GoronBracelet || IsAdult;}}),
                  Entrance(RR_DMT_STORMS_GROTTO,        {[]{return CanOpenStormGrotto;}}),
  });

  areaTable[RR_DEATH_MOUNTAIN_SUMMIT] = Area("Death Mountain Summit", "Death Mountain", RHT_DEATH_MOUNTAIN_TRAIL, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&PrescriptionAccess, {[]{return PrescriptionAccess || (IsAdult && (BrokenSwordAccess || BrokenSword));}}),
                  EventAccess(&GossipStoneFairy,   {[]{return GossipStoneFairy   || CanSummonGossipFairy;}}),
                  EventAccess(&BugRock,            {[]{return BugRock            || IsChild;}}),
                }, {
                  //Locations
                  LocationAccess(RC_DMT_TRADE_BROKEN_SWORD,    {[]{return IsAdult && BrokenSword;}}),
                  LocationAccess(RC_DMT_TRADE_EYEDROPS,        {[]{return IsAdult && Eyedrops;}}),
                  LocationAccess(RC_DMT_TRADE_CLAIM_CHECK,     {[]{return IsAdult && ClaimCheck;}}),
                  LocationAccess(RC_DMT_GS_FALLING_ROCKS_PATH, {[]{return IsAdult && AtNight && (CanUse(MEGATON_HAMMER) || LogicDMTGSUpper) && CanGetNightTimeGS;}}),
                  LocationAccess(RC_DMT_GOSSIP_STONE,          {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DEATH_MOUNTAIN_TRAIL,     {[]{return true;}}),
                  Entrance(RR_DMC_UPPER_LOCAL,          {[]{return true;}}),
                  Entrance(RR_DMT_OWL_FLIGHT,           {[]{return IsChild;}}),
                  Entrance(RR_DMT_COW_GROTTO,           {[]{return Here(RR_DEATH_MOUNTAIN_SUMMIT, []{return CanBlastOrSmash;});}}),
                  Entrance(RR_DMT_GREAT_FAIRY_FOUNTAIN, {[]{return Here(RR_DEATH_MOUNTAIN_SUMMIT, []{return CanBlastOrSmash;});}}),
  });

  areaTable[RR_DMT_OWL_FLIGHT] = Area("DMT Owl Flight", "Death Mountain", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KAK_IMPAS_ROOFTOP, {[]{return true;}}),
  });

  areaTable[RR_DMT_COW_GROTTO] = Area("DMT Cow Grotto", "DMT Cow Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DMT_COW_GROTTO_COW, {[]{return CanPlay(EponasSong);}}),
                }, {
                  //Exits
                  Entrance(RR_DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),

  });

  areaTable[RR_DMT_STORMS_GROTTO] = Area("DMT Storms Grotto", "DMT Storms Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_DMT_STORMS_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_DMT_STORMS_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
  });

  areaTable[RR_DMT_GREAT_FAIRY_FOUNTAIN] = Area("DMT Great Fairy Fountain", "DMT Great Fairy Fountain", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DMT_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);}}),
                }, {
                  //Exits
                  Entrance(RR_DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),
  });

  areaTable[RR_GORON_CITY] = Area("Goron City", "Goron City", RHT_GORON_CITY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy,          {[]{return GossipStoneFairy          || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&StickPot,                  {[]{return StickPot                  || IsChild;}}),
                  EventAccess(&BugRock,                   {[]{return BugRock                   || (CanBlastOrSmash || CanUse(SILVER_GAUNTLETS));}}),
                  EventAccess(&GoronCityChildFire,        {[]{return GoronCityChildFire        || (IsChild && CanUse(DINS_FIRE));}}),
                  EventAccess(&GCWoodsWarpOpen,           {[]{return GCWoodsWarpOpen           || (CanBlastOrSmash || CanUse(DINS_FIRE) || CanUse(BOW) || GoronBracelet || GoronCityChildFire);}}),
                  EventAccess(&GCDaruniasDoorOpenChild,   {[]{return GCDaruniasDoorOpenChild   || (IsChild && CanPlay(ZeldasLullaby));}}),
                  EventAccess(&StopGCRollingGoronAsAdult, {[]{return StopGCRollingGoronAsAdult || (IsAdult && (GoronBracelet || HasExplosives || Bow || (LogicGoronCityLinkGoronDins && CanUse(DINS_FIRE))));}}),
                }, {
                  //Locations
                  LocationAccess(RC_GC_MAZE_LEFT_CHEST,        {[]{return CanUse(MEGATON_HAMMER) || CanUse(SILVER_GAUNTLETS) || (LogicGoronCityLeftMost && HasExplosives && CanUse(HOVER_BOOTS));}}),
                  LocationAccess(RC_GC_MAZE_CENTER_CHEST,      {[]{return CanBlastOrSmash  || CanUse(SILVER_GAUNTLETS);}}),
                  LocationAccess(RC_GC_MAZE_RIGHT_CHEST,       {[]{return CanBlastOrSmash  || CanUse(SILVER_GAUNTLETS);}}),
                  LocationAccess(RC_GC_POT_FREESTANDING_POH,   {[]{return IsChild && GoronCityChildFire && (Bombs || (GoronBracelet && LogicGoronCityPotWithStrength) || (HasBombchus && LogicGoronCityPot));}}),
                  LocationAccess(RC_GC_ROLLING_GORON_AS_CHILD, {[]{return IsChild && (HasExplosives || (GoronBracelet && LogicChildRollingWithStrength));}}),
                  LocationAccess(RC_GC_ROLLING_GORON_AS_ADULT, {[]{return StopGCRollingGoronAsAdult;}}),
                  LocationAccess(RC_GC_GS_BOULDER_MAZE,        {[]{return IsChild && CanBlastOrSmash;}}),
                  LocationAccess(RC_GC_GS_CENTER_PLATFORM,     {[]{return IsAdult;}}),
                  LocationAccess(RC_GC_MEDIGORON,              {[]{return IsAdult && AdultsWallet && (CanBlastOrSmash || GoronBracelet);}}),
                  LocationAccess(RC_GC_MAZE_GOSSIP_STONE,      {[]{return CanBlastOrSmash || CanUse(SILVER_GAUNTLETS);}}),
                  LocationAccess(RC_GC_MEDIGORON_GOSSIP_STONE, {[]{return CanBlastOrSmash || GoronBracelet;}}),
                }, {
                  //Exits
                  Entrance(RR_DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
                  Entrance(RR_GC_WOODS_WARP,        {[]{return GCWoodsWarpOpen;}}),
                  Entrance(RR_GC_SHOP,              {[]{return (IsAdult && StopGCRollingGoronAsAdult) || (IsChild && (CanBlastOrSmash || GoronBracelet || GoronCityChildFire || CanUse(BOW)));}}),
                  Entrance(RR_GC_DARUNIAS_CHAMBER,  {[]{return (IsAdult && StopGCRollingGoronAsAdult) || GCDaruniasDoorOpenChild;}}),
                  Entrance(RR_GC_GROTTO_PLATFORM,   {[]{return IsAdult && ((CanPlay(SongOfTime) && ((EffectiveHealth > 2) || CanUse(GORON_TUNIC) || CanUse(LONGSHOT) || CanUse(NAYRUS_LOVE))) || (EffectiveHealth > 1 && CanUse(GORON_TUNIC) && CanUse(HOOKSHOT)) || (CanUse(NAYRUS_LOVE) && CanUse(HOOKSHOT)) || (EffectiveHealth > 2 && CanUse(HOOKSHOT) && LogicGoronCityGrotto));}}),
  });

  areaTable[RR_GC_WOODS_WARP] = Area("GC Woods Warp", "Goron City", RHT_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GCWoodsWarpOpen, {[]{return GCWoodsWarpOpen || (CanBlastOrSmash || CanUse(DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_GORON_CITY,     {[]{return CanLeaveForest && GCWoodsWarpOpen;}}),
                  Entrance(RR_THE_LOST_WOODS, {[]{return true;}}),
  });

  areaTable[RR_GC_DARUNIAS_CHAMBER] = Area("GC Darunias Chamber", "Goron City", RHT_GORON_CITY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GoronCityChildFire, {[]{return GoronCityChildFire || (IsChild && CanUse(STICKS));}}),
                }, {
                  //Locations
                  LocationAccess(RC_GC_DARUNIAS_JOY, {[]{return IsChild && CanPlay(SariasSong);}}),
                }, {
                  //Exits
                  Entrance(RR_GORON_CITY,      {[]{return true;}}),
                  Entrance(RR_DMC_LOWER_LOCAL, {[]{return IsAdult;}}),
  });

  areaTable[RR_GC_GROTTO_PLATFORM] = Area("GC Grotto Platform", "Goron City", RHT_GORON_CITY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GC_GROTTO,  {[]{return true;}}),
                  Entrance(RR_GORON_CITY, {[]{return EffectiveHealth > 2 || CanUse(GORON_TUNIC) || CanUse(NAYRUS_LOVE) || ((IsChild || CanPlay(SongOfTime)) && CanUse(LONGSHOT));}}),
  });

  areaTable[RR_GC_SHOP] = Area("GC Shop", "GC Shop", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GC_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_GORON_CITY, {[]{return true;}}),
  });

  areaTable[RR_GC_GROTTO] = Area("GC Grotto", "GC Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GC_DEKU_SCRUB_GROTTO_LEFT,   {[]{return CanStunDeku;}}),
                  LocationAccess(RC_GC_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return CanStunDeku;}}),
                  LocationAccess(RC_GC_DEKU_SCRUB_GROTTO_CENTER, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_GC_GROTTO_PLATFORM, {[]{return true;}}),
  });

  areaTable[RR_DMC_UPPER_NEARBY] = Area("DMC Upper Nearby", "Death Mountain Crater", RHT_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DMC_UPPER_LOCAL,       {[]{return FireTimer >= 48;}}),
                  Entrance(RR_DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),
                  Entrance(RR_DMC_UPPER_GROTTO,      {[]{return Here(RR_DMC_UPPER_NEARBY, []{return CanBlastOrSmash && (FireTimer >= 8 || Hearts >= 3);});}})
  });

  areaTable[RR_DMC_UPPER_LOCAL] = Area("DMC Upper Local", "Death Mountain Crater", RHT_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || (HasExplosives && CanSummonGossipFairyWithoutSuns && (FireTimer >= 16 || Hearts >= 3));}}),
                }, {
                  //Locations
                  LocationAccess(RC_DMC_WALL_FREESTANDING_POH, {[]{return FireTimer >= 16 || Hearts >= 3;}}),
                  LocationAccess(RC_DMC_GS_CRATE,              {[]{return (FireTimer >= 8 || Hearts >= 3) && IsChild && CanChildAttack;}}),
                  LocationAccess(RC_DMC_GOSSIP_STONE,          {[]{return HasExplosives && (FireTimer >= 16 || Hearts >= 3);}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_UPPER_NEARBY,         {[]{return true;}}),
                  Entrance(RR_DMC_LADDER_AREA_NEARBY,   {[]{return FireTimer >= 16 || Hearts >= 3;}}),
                  Entrance(RR_DMC_CENTRAL_NEARBY,       {[]{return IsAdult && CanUse(GORON_TUNIC) && CanUse(DISTANT_SCARECROW) && ((EffectiveHealth > 2) || (Fairy && ShuffleDungeonEntrances.IsNot(SHUFFLEDUNGEONS_OFF)) || CanUse(NAYRUS_LOVE));}}),
                  Entrance(RR_DMC_LOWER_NEARBY,         {[]{return false;}}),
  });

  areaTable[RR_DMC_LADDER_AREA_NEARBY] = Area("DMC Ladder Area Nearby", "Death Mountain Crater", RHT_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DMC_DEKU_SCRUB, {[]{return IsChild && CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_UPPER_NEARBY, {[]{return Hearts >= 3;}}),
                  Entrance(RR_DMC_LOWER_NEARBY, {[]{return Hearts >= 3 && (CanUse(HOVER_BOOTS) || (LogicCraterBoulderJS && IsAdult && CanUse(MEGATON_HAMMER)) || (LogicCraterBoulderSkip && IsAdult));}}),
  });

  areaTable[RR_DMC_LOWER_NEARBY] = Area("DMC Lower Nearby", "Death Mountain Crater", RHT_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DMC_LOWER_LOCAL,          {[]{return FireTimer >= 48;}}),
                  Entrance(RR_GC_DARUNIAS_CHAMBER,      {[]{return true;}}),
                  Entrance(RR_DMC_GREAT_FAIRY_FOUNTAIN, {[]{return CanUse(MEGATON_HAMMER);}}),
                  Entrance(RR_DMC_HAMMER_GROTTO,        {[]{return IsAdult && CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[RR_DMC_LOWER_LOCAL] = Area("DMC Lower Local", "Death Mountain Crater", RHT_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DMC_LOWER_NEARBY,       {[]{return true;}}),
                  Entrance(RR_DMC_LADDER_AREA_NEARBY, {[]{return FireTimer >= 8 || Hearts >= 3;}}),
                  Entrance(RR_DMC_CENTRAL_NEARBY,     {[]{return (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT)) && (FireTimer >= 8 || Hearts >= 3);}}),
                  Entrance(RR_DMC_CENTRAL_LOCAL,      {[]{return (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT) || (IsAdult && CanShield && LogicCraterBoleroJump)) && FireTimer >= 24;}}),
  });

  areaTable[RR_DMC_CENTRAL_NEARBY] = Area("DMC Central Nearby", "Death Mountain Crater", RHT_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DMC_VOLCANO_FREESTANDING_POH, {[]{return IsAdult && Hearts >= 3 && (CanPlantBean(RR_DMC_CENTRAL_LOCAL) || (LogicCraterBeanPoHWithHovers && HoverBoots));}}),
                  LocationAccess(RC_SHEIK_IN_CRATER,              {[]{return IsAdult && (FireTimer >= 8 || Hearts >= 3);}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_CENTRAL_LOCAL, {[]{return FireTimer >= 48;}}),
  });

  areaTable[RR_DMC_CENTRAL_LOCAL] = Area("DMC Central Local", "Death Mountain Crater", RHT_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BeanPlantFairy, {[]{return BeanPlantFairy || (CanPlantBean(RR_DMC_CENTRAL_LOCAL) && CanPlay(SongOfStorms));}}),
                }, {
                  //Locations
                  LocationAccess(RC_DMC_GS_BEAN_PATCH, {[]{return (FireTimer >= 8 || Hearts >= 3) && CanPlantBugs && CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_CENTRAL_NEARBY,   {[]{return true;}}),
                  Entrance(RR_DMC_LOWER_NEARBY,     {[]{return (IsAdult && CanPlantBean(RR_DMC_CENTRAL_LOCAL)) || CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT);}}),
                  Entrance(RR_DMC_UPPER_NEARBY,     {[]{return IsAdult && CanPlantBean(RR_DMC_CENTRAL_LOCAL);}}),
                  Entrance(RR_FIRE_TEMPLE_ENTRYWAY, {[]{return (IsChild && Hearts >= 3 && ShuffleDungeonEntrances.IsNot(SHUFFLEDUNGEONS_OFF)) || (IsAdult && FireTimer >= 24);}}),
  });

  areaTable[RR_DMC_GREAT_FAIRY_FOUNTAIN] = Area("DMC Great Fairy Fountain", "DMC Great Fairy Fountain", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DMC_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_LOWER_LOCAL, {[]{return true;}}),
  });

  areaTable[RR_DMC_UPPER_GROTTO] = Area("DMC Upper Grotto", "DMC Upper Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_DMC_UPPER_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_DMC_UPPER_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_UPPER_LOCAL, {[]{return true;}}),
  });

  areaTable[RR_DMC_HAMMER_GROTTO] = Area("DMC Hammer Grotto", "DMC Hammer Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DMC_DEKU_SCRUB_GROTTO_LEFT,   {[]{return CanStunDeku;}}),
                  LocationAccess(RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return CanStunDeku;}}),
                  LocationAccess(RC_DMC_DEKU_SCRUB_GROTTO_CENTER, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_LOWER_LOCAL, {[]{return true;}}),
  });
}
