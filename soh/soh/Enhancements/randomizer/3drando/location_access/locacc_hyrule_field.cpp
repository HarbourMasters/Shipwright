#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_HyruleField() {
  areaTable[RR_HYRULE_FIELD] = Area("Hyrule Field", "Hyrule Field", RHT_HYRULE_FIELD, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BigPoeKill, {[]{return CanUse(BOW) && CanRideEpona && HasBottle;}}),
                }, {
                  //Locations
                  LocationAccess(RC_HF_OCARINA_OF_TIME_ITEM,   {[]{return IsChild && HasAllStones;}}),
                  LocationAccess(RC_SONG_FROM_OCARINA_OF_TIME, {[]{return IsChild && HasAllStones;}}),
                }, {
                  //Exits
                  Entrance(RR_LW_BRIDGE,              {[]{return true;}}),
                  Entrance(RR_LAKE_HYLIA,             {[]{return true;}}),
                  Entrance(RR_GERUDO_VALLEY,          {[]{return true;}}),
                  Entrance(RR_MARKET_ENTRANCE,        {[]{return true;}}),
                  Entrance(RR_KAKARIKO_VILLAGE,       {[]{return true;}}),
                  Entrance(RR_ZR_FRONT,               {[]{return true;}}),
                  Entrance(RR_LON_LON_RANCH,          {[]{return true;}}),
                  Entrance(RR_HF_SOUTHEAST_GROTTO,    {[]{return Here(RR_HYRULE_FIELD, []{return CanBlastOrSmash;});}}),
                  Entrance(RR_HF_OPEN_GROTTO,         {[]{return true;}}),
                  Entrance(RR_HF_INSIDE_FENCE_GROTTO, {[]{return CanOpenBombGrotto;}}),
                  Entrance(RR_HF_COW_GROTTO,          {[]{return (CanUse(MEGATON_HAMMER) || IsChild) && CanOpenBombGrotto;}}),
                  Entrance(RR_HF_NEAR_MARKET_GROTTO,  {[]{return Here(RR_HYRULE_FIELD, []{return CanBlastOrSmash;});}}),
                  Entrance(RR_HF_FAIRY_GROTTO,        {[]{return Here(RR_HYRULE_FIELD, []{return CanBlastOrSmash;});}}),
                  Entrance(RR_HF_NEAR_KAK_GROTTO,     {[]{return CanOpenBombGrotto;}}),
                  Entrance(RR_HF_TEKTITE_GROTTO,      {[]{return CanOpenBombGrotto;}}),
  });

  areaTable[RR_HF_SOUTHEAST_GROTTO] = Area("HF Southeast Grotto", "HF Southeast Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_HF_SOUTHEAST_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_HF_SOUTHEAST_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_OPEN_GROTTO] = Area("HF Open Grotto", "HF Open Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_HF_OPEN_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_HF_OPEN_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_INSIDE_FENCE_GROTTO] = Area("HF Inside Fence Grotto", "HF Inside Fence Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_HF_DEKU_SCRUB_GROTTO, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_COW_GROTTO] = Area("HF Cow Grotto", "HF Cow Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_HF_GS_COW_GROTTO,           {[]{return HasFireSource && HookshotOrBoomerang;}}),
                  LocationAccess(RC_HF_COW_GROTTO_COW,          {[]{return HasFireSource && CanPlay(EponasSong);}}),
                  LocationAccess(RC_HF_COW_GROTTO_GOSSIP_STONE, {[]{return HasFireSource;}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_NEAR_MARKET_GROTTO] = Area("HF Near Market Grotto", "HF Near Market Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_HF_NEAR_MARKET_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_HF_NEAR_MARKET_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_FAIRY_GROTTO] = Area("HF Fairy Grotto", "HF Fairy Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_NEAR_KAK_GROTTO] = Area("HF Near Kak Grotto", "HF Near Kak Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_HF_GS_NEAR_KAK_GROTTO, {[]{return HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_TEKTITE_GROTTO] = Area("HF Tektite Grotto", "HF Tektite Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_HF_TEKTITE_GROTTO_FREESTANDING_POH, {[]{return ProgressiveScale >= 2 || CanUse(IRON_BOOTS);}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_LAKE_HYLIA] = Area("Lake Hylia", "Lake Hylia", RHT_LAKE_HYLIA, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
                  EventAccess(&BeanPlantFairy,   {[]{return BeanPlantFairy   || (CanPlantBean(RR_LAKE_HYLIA) && CanPlay(SongOfStorms));}}),
                  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || CanUse(STICKS);}}),
                  EventAccess(&BugShrub,         {[]{return BugShrub         || (IsChild && CanCutShrubs);}}),
                  EventAccess(&ChildScarecrow,   {[]{return ChildScarecrow   || (IsChild && Ocarina);}}),
                  EventAccess(&AdultScarecrow,   {[]{return AdultScarecrow   || (IsAdult && Ocarina);}}),
                }, {
                  //Locations
                  LocationAccess(RC_LH_UNDERWATER_ITEM,        {[]{return IsChild && CanDive;}}),
                  LocationAccess(RC_LH_SUN,                    {[]{return IsAdult && WaterTempleClear && CanUse(BOW);}}),
                  LocationAccess(RC_LH_FREESTANDING_POH,       {[]{return IsAdult && (CanUse(SCARECROW) || CanPlantBean(RR_LAKE_HYLIA));}}),
                  LocationAccess(RC_LH_GS_BEAN_PATCH,          {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(RC_LH_GS_LAB_WALL,            {[]{return IsChild && (HookshotOrBoomerang || (LogicLabWallGS && (Sticks || KokiriSword || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD)))) && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_LH_GS_SMALL_ISLAND,        {[]{return IsChild && CanChildAttack && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_LH_GS_TREE,                {[]{return IsAdult && CanUse(LONGSHOT) && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_LH_LAB_GOSSIP_STONE,       {[]{return true;}}),
                  LocationAccess(RC_LH_SOUTHEAST_GOSSIP_STONE, {[]{return true;}}),
                  LocationAccess(RC_LH_SOUTHWEST_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,          {[]{return true;}}),
                  Entrance(RR_ZORAS_DOMAIN,          {[]{return IsChild && (CanDive || CanUse(IRON_BOOTS));}}),
                  Entrance(RR_LH_OWL_FLIGHT,         {[]{return IsChild;}}),
                  Entrance(RR_LH_FISHING_ISLAND,     {[]{return IsChild || CanUse(SCARECROW) || CanPlantBean(RR_LAKE_HYLIA) || WaterTempleClear;}}),
                  Entrance(RR_LH_LAB,                {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_ENTRYWAY, {[]{return CanUse(HOOKSHOT) && ((CanUse(IRON_BOOTS) || (LogicWaterHookshotEntry && ProgressiveScale >= 2)) || (IsAdult && CanUse(LONGSHOT) && ProgressiveScale >= 2));}}),
                  Entrance(RR_LH_GROTTO,             {[]{return true;}}),
  });

  areaTable[RR_LH_FISHING_ISLAND] = Area("LH Fishing Island", "Lake Hylia", RHT_LAKE_HYLIA, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA,      {[]{return true;}}),
                  Entrance(RR_LH_FISHING_HOLE, {[]{return true;}}),
  });

  areaTable[RR_LH_OWL_FLIGHT] = Area("LH Owl Flight", "Lake Hylia", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_LH_LAB] = Area("LH Lab", "LH Lab", RHT_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&EyedropsAccess, {[]{return EyedropsAccess || (IsAdult && (EyeballFrogAccess || (EyeballFrog && DisableTradeRevert)));}}),
                }, {
                  //Locations
                  LocationAccess(RC_LH_LAB_DIVE,     {[]{return ProgressiveScale >= 2 || (LogicLabDiving && CanUse(IRON_BOOTS) && CanUse(HOOKSHOT));}}),
                  LocationAccess(RC_LH_TRADE_FROG,   {[]{return IsAdult && EyeballFrog;}}),
                  LocationAccess(RC_LH_GS_LAB_CRATE, {[]{return CanUse(IRON_BOOTS) && CanUse(HOOKSHOT);}}),
                }, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[RR_LH_FISHING_HOLE] = Area("LH Fishing Hole", "LH Fishing Hole", RHT_NONE, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_LH_CHILD_FISHING, {[]{return IsChild;}}),
                  LocationAccess(RC_LH_ADULT_FISHING, {[]{return IsAdult;}}),
                }, {
                  //Exits
                  Entrance(RR_LH_FISHING_ISLAND, {[]{return true;}}),
  });

  areaTable[RR_LH_GROTTO] = Area("LH Grotto", "LH Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_LH_DEKU_SCRUB_GROTTO_LEFT,   {[]{return CanStunDeku;}}),
                  LocationAccess(RC_LH_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return CanStunDeku;}}),
                  LocationAccess(RC_LH_DEKU_SCRUB_GROTTO_CENTER, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[RR_LON_LON_RANCH] = Area("Lon Lon Ranch", "Lon Lon Ranch", RHT_LON_LON_RANCH, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&Epona,    {[]{return Epona    || (CanPlay(EponasSong) && IsAdult && AtDay);}}),
                  EventAccess(&LinksCow, {[]{return LinksCow || (CanPlay(EponasSong) && IsAdult && AtDay);}}),
                }, {
                  //Locations
                  LocationAccess(RC_SONG_FROM_MALON,     {[]{return IsChild && ZeldasLetter && Ocarina && AtDay;}}),
                  LocationAccess(RC_LLR_GS_TREE,         {[]{return IsChild;}}),
                  LocationAccess(RC_LLR_GS_RAIN_SHED,    {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_LLR_GS_HOUSE_WINDOW, {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_LLR_GS_BACK_WALL,    {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,     {[]{return true;}}),
                  Entrance(RR_LLR_TALONS_HOUSE, {[]{return true;}}),
                  Entrance(RR_LLR_STABLES,      {[]{return true;}}),
                  Entrance(RR_LLR_TOWER,        {[]{return true;}}),
                  Entrance(RR_LLR_GROTTO,       {[]{return IsChild;}}),
  });

  areaTable[RR_LLR_TALONS_HOUSE] = Area("LLR Talons House", "LLR Talons House", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_LLR_TALONS_CHICKENS, {[]{return IsChild && AtDay && ZeldasLetter;}}),
                }, {
                  //Exits
                  Entrance(RR_LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[RR_LLR_STABLES] = Area("LLR Stables", "LLR Stables", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_LLR_STABLES_LEFT_COW,  {[]{return CanPlay(EponasSong);}}),
                  LocationAccess(RC_LLR_STABLES_RIGHT_COW, {[]{return CanPlay(EponasSong);}}),
                }, {
                  //Exits
                  Entrance(RR_LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[RR_LLR_TOWER] = Area("LLR Tower", "LLR Tower", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_LLR_FREESTANDING_POH, {[]{return IsChild;}}),
                  LocationAccess(RC_LLR_TOWER_LEFT_COW,   {[]{return CanPlay(EponasSong);}}),
                  LocationAccess(RC_LLR_TOWER_RIGHT_COW,  {[]{return CanPlay(EponasSong);}}),
                }, {
                  //Exits
                  Entrance(RR_LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[RR_LLR_GROTTO] = Area("LLR Grotto", "LLR Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_LLR_DEKU_SCRUB_GROTTO_LEFT,   {[]{return CanStunDeku;}}),
                  LocationAccess(RC_LLR_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return CanStunDeku;}}),
                  LocationAccess(RC_LLR_DEKU_SCRUB_GROTTO_CENTER, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_LON_LON_RANCH, {[]{return true;}}),
  });
}
