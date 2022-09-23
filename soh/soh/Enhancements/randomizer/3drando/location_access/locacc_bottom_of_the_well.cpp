#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"
#include "../dungeon.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_BottomOfTheWell() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[BOTTOM_OF_THE_WELL_ENTRYWAY] = Area("Bottom of the Well Entryway", "Bottom of the Well", BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(BOTTOM_OF_THE_WELL_MAIN_AREA,    {[]{return Dungeon::BottomOfTheWell.IsVanilla() && IsChild && (CanChildAttack || Nuts);},
                                                 /*Glitched*/[]{return Dungeon::BottomOfTheWell.IsVanilla() && IsChild && CanUse(MEGATON_HAMMER);}}),
                  Entrance(BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return Dungeon::BottomOfTheWell.IsMQ()      && IsChild;},
                                                 /*Glitched*/[]{return Dungeon::BottomOfTheWell.IsMQ()      && CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::INTERMEDIATE) && Longshot;}}),
                  Entrance(KAKARIKO_VILLAGE,                {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::BottomOfTheWell.IsVanilla()) {
  areaTable[BOTTOM_OF_THE_WELL_MAIN_AREA] = Area("Bottom of the Well Main Area", "Bottom of the Well", BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&StickPot, {[]{return true;}}),
                  EventAccess(&NutPot,   {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST,   {[]{return LogicLensBotw || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST,  {[]{return HasExplosives;}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST, {[]{return LogicLensBotw || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_COMPASS_CHEST,                {[]{return LogicLensBotw || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST,       {[]{return LogicLensBotw || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST,     {[]{return (LogicLensBotw || CanUse(LENS_OF_TRUTH)) && HasExplosives;}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_FREESTANDING_KEY,             {[]{return Sticks || CanUse(DINS_FIRE);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST,          {[]{return CanPlay(ZeldasLullaby) && (KokiriSword || (Sticks && LogicChildDeadhand));}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_INVISIBLE_CHEST,              {[]{return CanPlay(ZeldasLullaby) && (LogicLensBotw || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST,       {[]{return CanPlay(ZeldasLullaby);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST,        {[]{return CanPlay(ZeldasLullaby);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_MAP_CHEST,                    {[]{return HasExplosives || (((SmallKeys(BOTTOM_OF_THE_WELL, 3) && (LogicLensBotw || CanUse(LENS_OF_TRUTH))) || CanUse(DINS_FIRE)) && GoronBracelet);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST,             {[]{return SmallKeys(BOTTOM_OF_THE_WELL, 3) && (LogicLensBotw || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST,              {[]{return SmallKeys(BOTTOM_OF_THE_WELL, 3) && (LogicLensBotw || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM,           {[]{return Boomerang && (LogicLensBotw || CanUse(LENS_OF_TRUTH)) && SmallKeys(BOTTOM_OF_THE_WELL, 3);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM,           {[]{return Boomerang && (LogicLensBotw || CanUse(LENS_OF_TRUTH)) && SmallKeys(BOTTOM_OF_THE_WELL, 3);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE,            {[]{return SmallKeys(BOTTOM_OF_THE_WELL, 3) && (LogicLensBotw || CanUse(LENS_OF_TRUTH)) && Boomerang;}}),
                }, {
                  //Exits
                  Entrance(BOTTOM_OF_THE_WELL_ENTRYWAY, {[]{return true;}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::BottomOfTheWell.IsMQ()) {
  areaTable[BOTTOM_OF_THE_WELL_MQ_PERIMETER] = Area("Bottom of the Well MQ Perimeter", "Bottom of the Well", BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || Slingshot;}}),
  }, {
                  //Locations
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST,              {[]{return KokiriSword || (Sticks && LogicChildDeadhand);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY, {[]{return HasExplosives;}}),
                    //Trick: HasExplosives || (LogicBotWMQDeadHandKey && Boomerang)
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT,                {[]{return CanChildAttack;}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM,             {[]{return CanChildAttack && SmallKeys(BOTTOM_OF_THE_WELL, 2);}}),
  }, {
                  //Exits
                  Entrance(BOTTOM_OF_THE_WELL_ENTRYWAY,  {[]{return true;}}),
                  Entrance(BOTTOM_OF_THE_WELL_MQ_MIDDLE, {[]{return CanPlay(ZeldasLullaby);}}),
                    //Trick: CanPlay(ZeldasLullaby) || (LogicBotWMQPits && HasExplosives)
  });

  areaTable[BOTTOM_OF_THE_WELL_MQ_MIDDLE] = Area("Bottom of the Well MQ Middle", "Bottom of the Well", BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_MAP_CHEST,                        {[]{return true;}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST,              {[]{return HasExplosives && SmallKeys(BOTTOM_OF_THE_WELL, 2);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY, {[]{return true;}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM,               {[]{return CanChildAttack && HasExplosives;}}),
                    //Trick: CanChildAttack && (LogicBotWMQPits || HasExplosives)
  }, {
                  //Exits
                  Entrance(BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return true;}}),
  });
  }
}
