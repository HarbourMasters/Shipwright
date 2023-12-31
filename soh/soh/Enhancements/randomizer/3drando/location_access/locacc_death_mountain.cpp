#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_DeathMountain() {
  areaTable[DEATH_MOUNTAIN_TRAIL] = Area("Death Mountain", "Death Mountain", DEATH_MOUNTAIN_TRAIL, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BeanPlantFairy, {[]{return BeanPlantFairy || (CanPlantBean(DEATH_MOUNTAIN_TRAIL) && CanPlay(SongOfStorms) && (HasExplosives || GoronBracelet));}}),
                }, {
                  //Locations
                  LocationAccess(DMT_CHEST,                    {[]{return CanBlastOrSmash || (LogicDMTBombable && IsChild && GoronBracelet);}}),
                  LocationAccess(DMT_FREESTANDING_POH,         {[]{return CanTakeDamage || CanUse(HOVER_BOOTS) || (IsAdult && CanPlantBean(DEATH_MOUNTAIN_TRAIL) && (HasExplosives || GoronBracelet));}}),
                  LocationAccess(DMT_GS_BEAN_PATCH,            {[]{return CanPlantBugs && (HasExplosives || GoronBracelet || (LogicDMTSoilGS && (CanTakeDamage || CanUse(HOVER_BOOTS)) && CanUse(BOOMERANG)));}}),
                  LocationAccess(DMT_GS_NEAR_KAK,              {[]{return CanBlastOrSmash;}}),
                  LocationAccess(DMT_GS_ABOVE_DODONGOS_CAVERN, {[]{return IsAdult && AtNight && (CanUse(MEGATON_HAMMER) || (LogicDMTGSLowerHookshot && CanUse(HOOKSHOT)) || (LogicDMTGSLowerBean && CanPlantBean(DEATH_MOUNTAIN_TRAIL)) || (LogicDMTGSLowerHovers && CanUse(HOVER_BOOTS)) || LogicDMTGSLowerJS) && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(KAK_BEHIND_GATE,          {[]{return true;}}),
                  Entrance(GORON_CITY,               {[]{return true;}}),
                  Entrance(DEATH_MOUNTAIN_SUMMIT,    {[]{return Here(DEATH_MOUNTAIN_TRAIL, []{return CanBlastOrSmash;}) || (IsAdult && ((CanPlantBean(DEATH_MOUNTAIN_TRAIL) && GoronBracelet) || (HoverBoots && LogicDMTClimbHovers)));}}),
                  Entrance(DODONGOS_CAVERN_ENTRYWAY, {[]{return HasExplosives || GoronBracelet || IsAdult;}}),
                  Entrance(DMT_STORMS_GROTTO,        {[]{return CanOpenStormGrotto;}}),
  });

  areaTable[DEATH_MOUNTAIN_SUMMIT] = Area("Death Mountain Summit", "Death Mountain", DEATH_MOUNTAIN_TRAIL, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&PrescriptionAccess, {[]{return PrescriptionAccess || (IsAdult && (BrokenSwordAccess || BrokenSword));}}),
                  EventAccess(&GossipStoneFairy,   {[]{return GossipStoneFairy   || CanSummonGossipFairy;}}),
                  EventAccess(&BugRock,            {[]{return BugRock            || IsChild;}}),
                }, {
                  //Locations
                  LocationAccess(DMT_TRADE_BROKEN_SWORD,    {[]{return IsAdult && BrokenSword;}}),
                  LocationAccess(DMT_TRADE_EYEDROPS,        {[]{return IsAdult && Eyedrops;}}),
                  LocationAccess(DMT_TRADE_CLAIM_CHECK,     {[]{return IsAdult && ClaimCheck;}}),
                  LocationAccess(DMT_GS_FALLING_ROCKS_PATH, {[]{return IsAdult && AtNight && (CanUse(MEGATON_HAMMER) || LogicDMTGSUpper) && CanGetNightTimeGS;}}),
                  LocationAccess(DMT_GOSSIP_STONE,          {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DEATH_MOUNTAIN_TRAIL,     {[]{return true;}}),
                  Entrance(DMC_UPPER_LOCAL,          {[]{return true;}}),
                  Entrance(DMT_OWL_FLIGHT,           {[]{return IsChild;}}),
                  Entrance(DMT_COW_GROTTO,           {[]{return Here(DEATH_MOUNTAIN_SUMMIT, []{return CanBlastOrSmash;});}}),
                  Entrance(DMT_GREAT_FAIRY_FOUNTAIN, {[]{return Here(DEATH_MOUNTAIN_SUMMIT, []{return CanBlastOrSmash;});}}),
  });

  areaTable[DMT_OWL_FLIGHT] = Area("DMT Owl Flight", "Death Mountain", DEATH_MOUNTAIN_TRAIL, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAK_IMPAS_ROOFTOP, {[]{return true;}}),
  });

  areaTable[DMT_COW_GROTTO] = Area("DMT Cow Grotto", "DMT Cow Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DMT_COW_GROTTO_COW, {[]{return CanPlay(EponasSong);}}),
                }, {
                  //Exits
                  Entrance(DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),

  });

  areaTable[DMT_STORMS_GROTTO] = Area("DMT Storms Grotto", "DMT Storms Grotto", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(DMT_STORMS_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(DMT_STORMS_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
  });

  areaTable[DMT_GREAT_FAIRY_FOUNTAIN] = Area("DMT Great Fairy Fountain", "DMT Great Fairy Fountain", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DMT_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);}}),
                }, {
                  //Exits
                  Entrance(DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),
  });

  areaTable[GORON_CITY] = Area("Goron City", "Goron City", GORON_CITY, NO_DAY_NIGHT_CYCLE, {
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
                  LocationAccess(GC_MAZE_LEFT_CHEST,        {[]{return CanUse(MEGATON_HAMMER) || CanUse(SILVER_GAUNTLETS) || (LogicGoronCityLeftMost && HasExplosives && CanUse(HOVER_BOOTS));}}),
                  LocationAccess(GC_MAZE_CENTER_CHEST,      {[]{return CanBlastOrSmash  || CanUse(SILVER_GAUNTLETS);}}),
                  LocationAccess(GC_MAZE_RIGHT_CHEST,       {[]{return CanBlastOrSmash  || CanUse(SILVER_GAUNTLETS);}}),
                  LocationAccess(GC_POT_FREESTANDING_POH,   {[]{return IsChild && GoronCityChildFire && (Bombs || (GoronBracelet && LogicGoronCityPotWithStrength) || (HasBombchus && LogicGoronCityPot));}}),
                  LocationAccess(GC_ROLLING_GORON_AS_CHILD, {[]{return IsChild && (HasExplosives || (GoronBracelet && LogicChildRollingWithStrength));}}),
                  LocationAccess(GC_ROLLING_GORON_AS_ADULT, {[]{return StopGCRollingGoronAsAdult;}}),
                  LocationAccess(GC_GS_BOULDER_MAZE,        {[]{return IsChild && CanBlastOrSmash;}}),
                  LocationAccess(GC_GS_CENTER_PLATFORM,     {[]{return CanAdultAttack;}}),
                  LocationAccess(GC_MEDIGORON,              {[]{return IsAdult && AdultsWallet && (CanBlastOrSmash || GoronBracelet);}}),
                  LocationAccess(GC_MAZE_GOSSIP_STONE,      {[]{return CanBlastOrSmash || CanUse(SILVER_GAUNTLETS);}}),
                  LocationAccess(GC_MEDIGORON_GOSSIP_STONE, {[]{return CanBlastOrSmash || GoronBracelet;}}),
                }, {
                  //Exits
                  Entrance(DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
                  Entrance(GC_WOODS_WARP,        {[]{return GCWoodsWarpOpen;}}),
                  Entrance(GC_SHOP,              {[]{return (IsAdult && StopGCRollingGoronAsAdult) || (IsChild && (CanBlastOrSmash || GoronBracelet || GoronCityChildFire || CanUse(BOW)));}}),
                  Entrance(GC_DARUNIAS_CHAMBER,  {[]{return (IsAdult && StopGCRollingGoronAsAdult) || (IsChild && GCDaruniasDoorOpenChild);}}),
                  Entrance(GC_GROTTO_PLATFORM,   {[]{return IsAdult && ((CanPlay(SongOfTime) && ((EffectiveHealth > 2) || CanUse(GORON_TUNIC) || CanUse(LONGSHOT) || CanUse(NAYRUS_LOVE))) || (EffectiveHealth > 1 && CanUse(GORON_TUNIC) && CanUse(HOOKSHOT)) || (CanUse(NAYRUS_LOVE) && CanUse(HOOKSHOT)) || (EffectiveHealth > 2 && CanUse(HOOKSHOT) && LogicGoronCityGrotto));}}),
  });

  areaTable[GC_WOODS_WARP] = Area("GC Woods Warp", "Goron City", GORON_CITY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GCWoodsWarpOpen, {[]{return GCWoodsWarpOpen || (CanBlastOrSmash || CanUse(DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(GORON_CITY,     {[]{return CanLeaveForest && GCWoodsWarpOpen;}}),
                  Entrance(THE_LOST_WOODS, {[]{return true;}}),
  });

  areaTable[GC_DARUNIAS_CHAMBER] = Area("GC Darunias Chamber", "Goron City", GORON_CITY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GoronCityChildFire, {[]{return GoronCityChildFire || (IsChild && CanUse(STICKS));}}),
                }, {
                  //Locations
                  LocationAccess(GC_DARUNIAS_JOY, {[]{return IsChild && CanPlay(SariasSong);}}),
                }, {
                  //Exits
                  Entrance(GORON_CITY,      {[]{return true;}}),
                  Entrance(DMC_LOWER_LOCAL, {[]{return IsAdult;}}),
  });

  areaTable[GC_GROTTO_PLATFORM] = Area("GC Grotto Platform", "Goron City", GORON_CITY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GC_GROTTO,  {[]{return true;}}),
                  Entrance(GORON_CITY, {[]{return EffectiveHealth > 2 || CanUse(GORON_TUNIC) || CanUse(NAYRUS_LOVE) || ((IsChild || CanPlay(SongOfTime)) && CanUse(LONGSHOT));}}),
  });

  areaTable[GC_SHOP] = Area("GC Shop", "GC Shop", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GC_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(GORON_CITY, {[]{return true;}}),
  });

  areaTable[GC_GROTTO] = Area("GC Grotto", "GC Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GC_DEKU_SCRUB_GROTTO_LEFT,   {[]{return CanStunDeku;}}),
                  LocationAccess(GC_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return CanStunDeku;}}),
                  LocationAccess(GC_DEKU_SCRUB_GROTTO_CENTER, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(GC_GROTTO_PLATFORM, {[]{return true;}}),
  });

  areaTable[DMC_UPPER_NEARBY] = Area("DMC Upper Nearby", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DMC_UPPER_LOCAL,       {[]{return FireTimer >= 48;}}),
                  Entrance(DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),
                  Entrance(DMC_UPPER_GROTTO,      {[]{return Here(DMC_UPPER_NEARBY, []{return CanBlastOrSmash && (FireTimer >= 8 || Hearts >= 3);});}})
  });

  areaTable[DMC_UPPER_LOCAL] = Area("DMC Upper Local", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || (HasExplosives && CanSummonGossipFairyWithoutSuns && (FireTimer >= 16 || Hearts >= 3));}}),
                }, {
                  //Locations
                  LocationAccess(DMC_WALL_FREESTANDING_POH, {[]{return FireTimer >= 16 || Hearts >= 3;}}),
                  LocationAccess(DMC_GS_CRATE,              {[]{return (FireTimer >= 8 || Hearts >= 3) && IsChild && CanChildAttack;}}),
                  LocationAccess(DMC_GOSSIP_STONE,          {[]{return HasExplosives && (FireTimer >= 16 || Hearts >= 3);}}),
                }, {
                  //Exits
                  Entrance(DMC_UPPER_NEARBY,         {[]{return true;}}),
                  Entrance(DMC_LADDER_AREA_NEARBY,   {[]{return FireTimer >= 16 || Hearts >= 3;}}),
                  Entrance(DMC_CENTRAL_NEARBY,       {[]{return IsAdult && CanUse(GORON_TUNIC) && CanUse(DISTANT_SCARECROW) && ((EffectiveHealth > 2) || (Fairy && ShuffleDungeonEntrances.IsNot(SHUFFLEDUNGEONS_OFF)) || CanUse(NAYRUS_LOVE));}}),
                  Entrance(DMC_LOWER_NEARBY,         {[]{return false;}}),
  });

  areaTable[DMC_LADDER_AREA_NEARBY] = Area("DMC Ladder Area Nearby", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DMC_DEKU_SCRUB, {[]{return IsChild && CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(DMC_UPPER_NEARBY, {[]{return Hearts >= 3;}}),
                  Entrance(DMC_LOWER_NEARBY, {[]{return Hearts >= 3 && (CanUse(HOVER_BOOTS) || (LogicCraterBoulderJS && IsAdult && CanUse(MEGATON_HAMMER)) || (LogicCraterBoulderSkip && IsAdult));}}),
  });

  areaTable[DMC_LOWER_NEARBY] = Area("DMC Lower Nearby", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DMC_LOWER_LOCAL,          {[]{return FireTimer >= 48;}}),
                  Entrance(GC_DARUNIAS_CHAMBER,      {[]{return true;}}),
                  Entrance(DMC_GREAT_FAIRY_FOUNTAIN, {[]{return CanUse(MEGATON_HAMMER);}}),
                  Entrance(DMC_HAMMER_GROTTO,        {[]{return IsAdult && CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[DMC_LOWER_LOCAL] = Area("DMC Lower Local", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DMC_LOWER_NEARBY,       {[]{return true;}}),
                  Entrance(DMC_LADDER_AREA_NEARBY, {[]{return FireTimer >= 8 || Hearts >= 3;}}),
                  Entrance(DMC_CENTRAL_NEARBY,     {[]{return (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT)) && (FireTimer >= 8 || Hearts >= 3);}}),
                  Entrance(DMC_CENTRAL_LOCAL,      {[]{return (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT) || (IsAdult && CanShield && LogicCraterBoleroJump)) && FireTimer >= 24;}}),
  });

  areaTable[DMC_CENTRAL_NEARBY] = Area("DMC Central Nearby", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DMC_VOLCANO_FREESTANDING_POH, {[]{return IsAdult && Hearts >= 3 && (CanPlantBean(DMC_CENTRAL_LOCAL) || (LogicCraterBeanPoHWithHovers && HoverBoots));}}),
                  LocationAccess(SHEIK_IN_CRATER,              {[]{return IsAdult && (FireTimer >= 8 || Hearts >= 3);}}),
                }, {
                  //Exits
                  Entrance(DMC_CENTRAL_LOCAL, {[]{return FireTimer >= 48;}}),
  });

  areaTable[DMC_CENTRAL_LOCAL] = Area("DMC Central Local", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BeanPlantFairy, {[]{return BeanPlantFairy || (CanPlantBean(DMC_CENTRAL_LOCAL) && CanPlay(SongOfStorms));}}),
                }, {
                  //Locations
                  LocationAccess(DMC_GS_BEAN_PATCH, {[]{return (FireTimer >= 8 || Hearts >= 3) && CanPlantBugs && CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(DMC_CENTRAL_NEARBY,   {[]{return true;}}),
                  Entrance(DMC_LOWER_NEARBY,     {[]{return (IsAdult && CanPlantBean(DMC_CENTRAL_LOCAL)) || CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT);}}),
                  Entrance(DMC_UPPER_NEARBY,     {[]{return IsAdult && CanPlantBean(DMC_CENTRAL_LOCAL);}}),
                  Entrance(FIRE_TEMPLE_ENTRYWAY, {[]{return (IsChild && Hearts >= 3 && ShuffleDungeonEntrances.IsNot(SHUFFLEDUNGEONS_OFF)) || (IsAdult && FireTimer >= 24);}}),
  });

  areaTable[DMC_GREAT_FAIRY_FOUNTAIN] = Area("DMC Great Fairy Fountain", "DMC Great Fairy Fountain", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DMC_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);}}),
                }, {
                  //Exits
                  Entrance(DMC_LOWER_LOCAL, {[]{return true;}}),
  });

  areaTable[DMC_UPPER_GROTTO] = Area("DMC Upper Grotto", "DMC Upper Grotto", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(DMC_UPPER_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(DMC_UPPER_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DMC_UPPER_LOCAL, {[]{return true;}}),
  });

  areaTable[DMC_HAMMER_GROTTO] = Area("DMC Hammer Grotto", "DMC Hammer Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DMC_DEKU_SCRUB_GROTTO_LEFT,   {[]{return CanStunDeku;}}),
                  LocationAccess(DMC_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return CanStunDeku;}}),
                  LocationAccess(DMC_DEKU_SCRUB_GROTTO_CENTER, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(DMC_LOWER_LOCAL, {[]{return true;}}),
  });
}
