#include "gameplaystats.h"
#include "GameplayStats/gameplaystats2.h"

#include "soh/SaveManager.h"
#include "functions.h"
#include "macros.h"
#include "soh/cvar_prefixes.h"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"
#include "soh/util.h"

#include <vector>
#include <string>
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include <z64.h>
#include "variables.h"
extern PlayState* gPlayState;
uint64_t GetUnixTimestamp();
}

// New
#include <fstream>
#include <filesystem>
#include "TimeDisplay/TimeDisplay.h"

using json = nlohmann::json;

#define CVAR_NAME "gSettings.Gameplaystats.Enable"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

std::vector<GameplayStatObject> currentStatList;

std::unordered_map<uint32_t, GameplayStatEntry> sceneList = {
    { SCENE_DEKU_TREE,                          { "Inside the Deku Tree",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_DODONGOS_CAVERN,                    { "Dodongo's Cavern",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_JABU_JABU,                          { "Inside Jabu-Jabu's Belly",       UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_FOREST_TEMPLE,                      { "Forest Temple",                  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_FIRE_TEMPLE,                        { "Fire Temple",                    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_WATER_TEMPLE,                       { "Water Temple",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_SPIRIT_TEMPLE,                      { "Spirit Temple",                  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_SHADOW_TEMPLE,                      { "Shadow Temple",                  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_BOTTOM_OF_THE_WELL,                 { "Bottom of the Well",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_ICE_CAVERN,                         { "Ice Cavern",                     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GANONS_TOWER,                       { "Ganon's Tower",                  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GERUDO_TRAINING_GROUND,             { "Gerudo Training Ground",         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_THIEVES_HIDEOUT,                    { "Thieves' Hideout",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_INSIDE_GANONS_CASTLE,               { "Inside Ganon's Castle",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GANONS_TOWER_COLLAPSE_INTERIOR,     { "Tower Collapse",                 UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_INSIDE_GANONS_CASTLE_COLLAPSE,      { "Castle Collapse",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_TREASURE_BOX_SHOP,                  { "Treasure Box Shop",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_DEKU_TREE_BOSS,                     { "Gohma's Lair",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_DODONGOS_CAVERN_BOSS,               { "King Dodongo's Lair",            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_JABU_JABU_BOSS,                     { "Barinade's Lair",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_FOREST_TEMPLE_BOSS,                 { "Phantom Ganon's Lair",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_FIRE_TEMPLE_BOSS,                   { "Volvagia's Lair",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_WATER_TEMPLE_BOSS,                  { "Morpha's Lair",                  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_SPIRIT_TEMPLE_BOSS,                 { "Twinrova's Lair",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_SHADOW_TEMPLE_BOSS,                 { "Bongo Bongo's Lair",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GANONDORF_BOSS,                     { "Ganondorf's Lair",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR,     { "Ganon's Lair",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_MARKET_ENTRANCE_DAY,                { "Market Entrance (Day)",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_MARKET_ENTRANCE_NIGHT,              { "Market Entrance (Night)",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_MARKET_ENTRANCE_RUINS,              { "Market Entrance (Adult)",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_BACK_ALLEY_DAY,                     { "Back Alley (Day)",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_BACK_ALLEY_NIGHT,                   { "Back Alley (Night)",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_MARKET_DAY,                         { "Market (Day)",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_MARKET_NIGHT,                       { "Market (Night)",                 UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_MARKET_RUINS,                       { "Market (Adult)",                 UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY,        { "Outside ToT (Day)",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT,      { "Outside ToT (Night)",            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS,      { "Outside ToT (Adult)",            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_KNOW_IT_ALL_BROS_HOUSE,             { "Know-It-All Bros' House",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_TWINS_HOUSE,                        { "Twins' House",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_MIDOS_HOUSE,                        { "Mido's House",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_SARIAS_HOUSE,                       { "Saria's House",                  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_KAKARIKO_CENTER_GUEST_HOUSE,        { "Carpenter Boss's House",         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_BACK_ALLEY_HOUSE,                   { "Man in Green's House",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_BAZAAR,                             { "Bazaar",                         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_KOKIRI_SHOP,                        { "Kokiri Shop",                    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GORON_SHOP,                         { "Goron Shop",                     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_ZORA_SHOP,                          { "Zora Shop",                      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_POTION_SHOP_KAKARIKO,               { "Kakariko Potion Shop",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_POTION_SHOP_MARKET,                 { "Market Potion Shop",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_BOMBCHU_SHOP,                       { "Bombchu Shop",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_HAPPY_MASK_SHOP,                    { "Happy Mask Shop",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_LINKS_HOUSE,                        { "Link's House",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_DOG_LADY_HOUSE,                     { "Richard's House",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_STABLE,                             { "Stable",                         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_IMPAS_HOUSE,                        { "Impa's House",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_LAKESIDE_LABORATORY,                { "Lakeside Lab",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_CARPENTERS_TENT,                    { "Carpenters' Tent",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GRAVEKEEPERS_HUT,                   { "Gravekeeper's Hut",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC,        { "Great Fairy",                    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_FAIRYS_FOUNTAIN,                    { "Fairy Fountain",                 UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS,       { "Great Fairy",                    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GROTTOS,                            { "Grotto",                         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_REDEAD_GRAVE,                       { "Redead Grave",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GRAVE_WITH_FAIRYS_FOUNTAIN,         { "Fairy Fountain Grave",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_ROYAL_FAMILYS_TOMB,                 { "Royal Family's Tomb",            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_SHOOTING_GALLERY,                   { "Shooting Gallery",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_TEMPLE_OF_TIME,                     { "Temple of Time",                 UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_CHAMBER_OF_THE_SAGES,               { "Chamber of Sages",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_CASTLE_COURTYARD_GUARDS_DAY,        { "Castle Maze (Day)",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_CASTLE_COURTYARD_GUARDS_NIGHT,      { "Castle Maze (Night)",            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_CUTSCENE_MAP,                       { "Cutscene Map",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_WINDMILL_AND_DAMPES_GRAVE,          { "Dampe's Grave",                  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_FISHING_POND,                       { "Fishing Pond",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_CASTLE_COURTYARD_ZELDA,             { "Castle Courtyard",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_BOMBCHU_BOWLING_ALLEY,              { "Bombchu Bowling Alley",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_LON_LON_BUILDINGS,                  { "Ranch House",                    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_MARKET_GUARD_HOUSE,                 { "Guard House",                    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_POTION_SHOP_GRANNY,                 { "Granny's Potion Shop",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GANON_BOSS,                         { "Ganon Fight",                    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_HOUSE_OF_SKULLTULA,                 { "House of Skulltula",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_HYRULE_FIELD,                       { "Hyrule Field",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_KAKARIKO_VILLAGE,                   { "Kakariko Village",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GRAVEYARD,                          { "Graveyard",                      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_ZORAS_RIVER,                        { "Zora's River",                   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_KOKIRI_FOREST,                      { "Kokiri Forest",                  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_SACRED_FOREST_MEADOW,               { "Sacred Forest Meadow",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_LAKE_HYLIA,                         { "Lake Hylia",                     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_ZORAS_DOMAIN,                       { "Zora's Domain",                  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_ZORAS_FOUNTAIN,                     { "Zora's Fountain",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GERUDO_VALLEY,                      { "Gerudo Valley",                  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_LOST_WOODS,                         { "Lost Woods",                     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_DESERT_COLOSSUS,                    { "Desert Colossus",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GERUDOS_FORTRESS,                   { "Gerudo's Fortress",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_HAUNTED_WASTELAND,                  { "Haunted Wasteland",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_HYRULE_CASTLE,                      { "Hyrule Castle",                  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_DEATH_MOUNTAIN_TRAIL,               { "Death Mountain Trail",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_DEATH_MOUNTAIN_CRATER,              { "Death Mountain Crater",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_GORON_CITY,                         { "Goron City",                     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_LON_LON_RANCH,                      { "Lon Lon Ranch",                  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_OUTSIDE_GANONS_CASTLE,              { "Outside Ganon's Castle",         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    // Debug Only Scenes
    { SCENE_TEST01, 		                    { "Test Map", 				        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_BESITU, 		                    { "Test Room", 				        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_DEPTH_TEST, 	                    { "Depth Test", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_SYOTES, 		                    { "Stalfos Mini-Boss", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_SYOTES2, 		                    { "Stalfos Boss", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_SUTARU, 		                    { "Dark Link", 				        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_HAIRAL_NIWA2, 	                    { "Castle Maze (Broken)", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_SASATEST, 		                    { "SRD Room", 				        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { SCENE_TESTROOM, 		                    { "Chest Room", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
};

std::unordered_map<uint32_t, GameplayStatEntry> itemList = {
    { ITEM_STICK,            { "Deku Stick", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_NUT,              { "Deku Nut", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BOMB,             { "Bombs", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BOW,              { "Fairy Bow", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_ARROW_FIRE,       { "Fire Arrows", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_DINS_FIRE,        { "Din's Fire", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SLINGSHOT,        { "Slingshot", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_OCARINA_FAIRY,    { "Fairy Ocarina", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_OCARINA_TIME,     { "Ocarina of Time", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BOMBCHU,          { "Bombchus", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_HOOKSHOT,         { "Hookshot", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_LONGSHOT,         { "Longshot", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_ARROW_ICE,        { "Ice Arrows", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_FARORES_WIND,     { "Farore's Wind", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BOOMERANG,        { "Boomerang", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_LENS,             { "Lens of Truth", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BEAN,             { "Magic Beans", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_HAMMER,           { "Megaton Hammer", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_ARROW_LIGHT,      { "Light Arrows", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_NAYRUS_LOVE,      { "Nayru's Love", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BOTTLE,           { "Bottle", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_POTION_RED,       { "Red Potion", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_POTION_GREEN,     { "Green Potion", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_POTION_BLUE,      { "Blue Potion", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_FAIRY,            { "Fairy", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_FISH,             { "Fish", 				        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_MILK_BOTTLE,      { "Milk (Full)", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_LETTER_RUTO,      { "Ruto's Letter", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BLUE_FIRE,        { "Blue Fire", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BUG,              { "Bugs", 				        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BIG_POE,          { "Big Poe", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_MILK_HALF,        { "Milk (Half)", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_POE,              { "Poe", 				        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_WEIRD_EGG,        { "Weird Egg", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_CHICKEN,          { "Chicken", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_LETTER_ZELDA,     { "Zelda's Letter", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_MASK_KEATON,      { "Keaton Mask", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_MASK_SKULL,       { "Skull Mask", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_MASK_SPOOKY,      { "Spooky Mask", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_MASK_BUNNY,       { "Bunny Hood", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_MASK_GORON,       { "Goron Mask", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_MASK_ZORA,        { "Zora Mask", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_MASK_GERUDO,      { "Gerudo Mask", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_MASK_TRUTH,       { "Mask of Truth", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SOLD_OUT,         { "Sold Out", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_POCKET_EGG,       { "Pocket Egg", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_POCKET_CUCCO,     { "Pocket Cucco", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_COJIRO,           { "Cojiro", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_ODD_MUSHROOM,     { "Odd Mushroom", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_ODD_POTION,       { "Odd Potion", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SAW,              { "Poacher's Saw", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SWORD_BROKEN,     { "Broken Goron Sword",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_PRESCRIPTION,     { "Prescription", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_FROG,             { "Eyeball Frog", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_EYEDROPS,         { "Eye Drops", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_CLAIM_CHECK,      { "Claim Check", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SWORD_KOKIRI,     { "Kokiri Sword", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SWORD_MASTER,     { "Master Sword", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SWORD_BGS,        { "Biggoron's Sword", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SHIELD_DEKU,      { "Deku Shield", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SHIELD_HYLIAN,    { "Hylian Shield", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SHIELD_MIRROR,    { "Mirror Shield", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_TUNIC_GORON,      { "Goron Tunic", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_TUNIC_ZORA,       { "Zora Tunic", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BOOTS_IRON,       { "Iron Boots", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BOOTS_HOVER,      { "Hover Boots", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BULLET_BAG_30,    { "Bullet Bag", 			    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BULLET_BAG_40,    { "Bigger Bullet Bag", 	    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BULLET_BAG_50,    { "Biggest Bullet Bag", 	    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_QUIVER_30,        { "Quiver", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_QUIVER_40,        { "Big Quiver", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_QUIVER_50,        { "Biggest Quiver", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BOMB_BAG_20,      { "Bomb Bag", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BOMB_BAG_30,      { "Big Bomb Bag", 			    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BOMB_BAG_40,      { "Biggest Bomb Bag", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BRACELET,         { "Goron's Bracelet", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_GAUNTLETS_SILVER, { "Silver Gauntlets", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_GAUNTLETS_GOLD,   { "Gold Gauntlets", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SCALE_SILVER,     { "Silver Scale", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SCALE_GOLDEN,     { "Gold Scale", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SWORD_KNIFE,      { "Giant's Knife", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_WALLET_ADULT,     { "Adult's Wallet", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_WALLET_GIANT,     { "Giant's Wallet", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_FISHING_POLE,     { "Fishing Pole", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SONG_MINUET,      { "Minuet of Forest", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Green) } },
    { ITEM_SONG_BOLERO,      { "Bolero of Fire", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Red) } },
    { ITEM_SONG_SERENADE,    { "Serenade of Water",         UIWidgets::ColorValues.at(UIWidgets::Colors::Blue) } },
    { ITEM_SONG_REQUIEM,     { "Requiem of Spirit",         UIWidgets::ColorValues.at(UIWidgets::Colors::Orange) } },
    { ITEM_SONG_NOCTURNE,    { "Nocturne of Shadow",        UIWidgets::ColorValues.at(UIWidgets::Colors::Purple) } },
    { ITEM_SONG_PRELUDE,     { "Prelude of Light", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Yellow) } },
    { ITEM_SONG_LULLABY,     { "Zelda's Lullaby", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Purple) } },
    { ITEM_SONG_EPONA,       { "Epona's Song", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::Orange) } },
    { ITEM_SONG_SARIA,       { "Saria's Song", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::Green) } },
    { ITEM_SONG_SUN,         { "Sun's Song", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::Yellow) } },
    { ITEM_SONG_TIME,        { "Song of Time", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::LightBlue) } },
    { ITEM_SONG_STORMS,      { "Song of Storms", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Gray) } },
    { ITEM_MEDALLION_FOREST, { "Forest Medallion", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Green) } },
    { ITEM_MEDALLION_FIRE,   { "Fire Medallion", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Red) } },
    { ITEM_MEDALLION_WATER,  { "Water Medallion", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Blue) } },
    { ITEM_MEDALLION_SPIRIT, { "Spirit Medallion", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Orange) } },
    { ITEM_MEDALLION_SHADOW, { "Shadow Medallion", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Purple) } },
    { ITEM_MEDALLION_LIGHT,  { "Light Medallion", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Yellow) } },
    { ITEM_KOKIRI_EMERALD,   { "Kokiri's Emerald", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Green) } },
    { ITEM_GORON_RUBY,       { "Goron's Ruby", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::Red) } },
    { ITEM_ZORA_SAPPHIRE,    { "Zora's Sapphire", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Blue) } },
    { ITEM_STONE_OF_AGONY,   { "Stone of Agony", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_GERUDO_CARD,      { "Gerudo's Card", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SKULL_TOKEN,      { "", 					        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_HEART_CONTAINER,  { "", 					        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_HEART_PIECE,      { "Piece of Heart", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_KEY_BOSS,         { "Ganon's Boss Key", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_COMPASS,          { "Compass", 					UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_DUNGEON_MAP,      { "Dungeon Map", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_KEY_SMALL,        { "Small Key", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_HEART_PIECE_2,    { "", 					        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_SINGLE_MAGIC,     { "Magic", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_DOUBLE_MAGIC,     { "Double Magic", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_DOUBLE_DEFENSE,   { "Double Defense", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { RG_GREG_RUPEE,         { "Greg", 					    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_STICKS_5,         { "Deku Stick", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_STICKS_10,        { "Deku Stick", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_NUTS_5,           { "Deku Nut", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_NUTS_10,          { "Deku Nut", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BOMBCHUS_5,       { "Bombchus", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_BOMBCHUS_20,      { "Bombchus", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_STICK_UPGRADE_20, { "Deku Stick Upgrade (20)",   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_STICK_UPGRADE_30, { "Deku Stick Upgrade (30)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_NUT_UPGRADE_30,   { "Deku Nut Upgrade (30)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { ITEM_NUT_UPGRADE_40,   { "Deku Nut Upgrade (30)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
};

std::unordered_map<uint32_t, GameplayStatEntry> eventList = {
    { TIMESTAMP_DEFEAT_GOHMA,           { "Queen Gohma Defeated",   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { TIMESTAMP_DEFEAT_KING_DODONGO,    { "King Dodongo Defeated",  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { TIMESTAMP_DEFEAT_BARINADE,        { "Barinade Defeated",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { TIMESTAMP_DEFEAT_PHANTOM_GANON,   { "Phantom Ganon Defeated", UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { TIMESTAMP_DEFEAT_VOLVAGIA,        { "Volvagia Defeated",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { TIMESTAMP_DEFEAT_MORPHA,          { "Morpha Defeated",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { TIMESTAMP_DEFEAT_BONGO_BONGO,     { "Bongo Bongo Defeated",   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { TIMESTAMP_DEFEAT_TWINROVA,        { "Twinrova Defeated",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { TIMESTAMP_DEFEAT_GANONDORF,       { "Ganondorf Defeated",     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { TIMESTAMP_DEFEAT_GANON,           { "Ganon Defeated",         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { TIMESTAMP_BOSSRUSH_FINISH,        { "Boss Rush Complete",     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { TIMESTAMP_FOUND_GREG,             { "Greg Found",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
    { TIMESTAMP_TRIFORCE_COMPLETED,     { "Triforce Complete",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
};

// End

const char* const sceneMappings[] = {
    "Inside the Deku Tree",
    "Dodongo's Cavern",
    "Inside Jabu-Jabu's Belly",
    "Forest Temple",
    "Fire Temple",
    "Water Temple",
    "Spirit Temple",
    "Shadow Temple",
    "Bottom of the Well",
    "Ice Cavern",
    "Ganon's Tower",
    "Gerudo Training Ground",
    "Thieves' Hideout",
    "Inside Ganon's Castle",
    "Tower Collapse",
    "Castle Collapse",
    "Treasure Box Shop",
    "Gohma's Lair",
    "King Dodongo's Lair",
    "Barinade's Lair",
    "Phantom Ganon's Lair",
    "Volvagia's Lair",
    "Morpha's Lair",
    "Twinrova's Lair",
    "Bongo Bongo's Lair",
    "Ganondorf's Lair",
    "Ganon's Lair",
    "Market Entrance (Day)",
    "Market Entrance (Night)",
    "Market Entrance (Adult)",
    "Back Alley (Day)",
    "Back Alley (Night)",
    "Market (Day)",
    "Market (Night)",
    "Market (Adult)",
    "Outside ToT (Day)",
    "Outside ToT (Night)",
    "Outside ToT (Adult)",
    "Know-It-All Bros' House",
    "Twins' House",
    "Mido's House",
    "Saria's House",
    "Carpenter Boss's House",
    "Man in Green's House",
    "Bazaar",
    "Kokiri Shop",
    "Goron Shop",
    "Zora Shop",
    "Kakariko Potion Shop",
    "Market Potion Shop",
    "Bombchu Shop",
    "Happy Mask Shop",
    "Link's House",
    "Richard's House",
    "Stable",
    "Impa's House",
    "Lakeside Lab",
    "Carpenters' Tent",
    "Gravekeeper's Hut",
    "Great Fairy",
    "Fairy Fountain",
    "Great Fairy",
    "Grotto",
    "Redead Grave",
    "Fairy Fountain Grave",
    "Royal Family's Tomb",
    "Shooting Gallery",
    "Temple of Time",
    "Chamber of Sages",
    "Castle Maze (Day)",
    "Castle Maze (Night)",
    "Cutscene Map",
    "Dampe's Grave",
    "Fishing Pond",
    "Castle Courtyard",
    "Bombchu Bowling Alley",
    "Ranch House",
    "Guard House",
    "Granny's Potion Shop",
    "Ganon Fight",
    "House of Skulltula",
    "Hyrule Field",
    "Kakariko Village",
    "Graveyard",
    "Zora's River",
    "Kokiri Forest",
    "Sacred Forest Meadow",
    "Lake Hylia",
    "Zora's Domain",
    "Zora's Fountain",
    "Gerudo Valley",
    "Lost Woods",
    "Desert Colossus",
    "Gerudo's Fortress",
    "Haunted Wasteland",
    "Hyrule Castle",
    "Death Mountain Trail",
    "Death Mountain Crater",
    "Goron City",
    "Lon Lon Ranch",
    "Outside Ganon's Castle",
    // Debug Rooms
    "Test Map",
    "Test Room",
    "Depth Test",
    "Stalfos Mini-Boss",
    "Stalfos Boss",
    "Dark Link",
    "Castle Maze (Broken)",
    "SRD Room",
    "Chest Room",
};

const char* const countMappings[] = {
    "Anubis:",
    "Armos:",
    "Arwing:",
    "Bari:",
    "Biri:",
    "Beamos:",
    "Big Octo:",
    "Bubble (Blue):",
    "Bubble (Green):",
    "Bubble (Red):",
    "Bubble (White):",
    "Business Scrub:",
    "Dark Link:",
    "Dead Hand:",
    "Deku Baba:",
    "Deku Baba (Big):",
    "Deku Scrub:",
    "Dinolfos:",
    "Dodongo:",
    "Dodongo (Baby):",
    "Door Mimic:",
    "Flare Dancer:",
    "Floormaster:",
    "Flying Floor Tile:",
    "Flying Pot:",
    "Freezard:",
    "Gerudo Thief:",
    "Gibdo:",
    "Gohma Larva:",
    "Guay:",
    "Iron Knuckle:",
    "Iron Knuckle (Nab):",
    "Keese:",
    "Keese (Fire):",
    "Keese (Ice):",
    "Leever:",
    "Leever (Big):",
    "Like-Like:",
    "Lizalfos:",
    "Mad Scrub:",
    "Moblin:",
    "Moblin (Club):",
    "Octorok:",
    "Parasitic Tentacle:",
    "Peahat:",
    "Peahat Larva:",
    "Poe:",
    "Poe (Big):",
    "Poe (Composer):",
    "Poe Sisters:",
    "Redead:",
    "Shabom:",
    "Shell Blade:",
    "Skull Kid:",
    "Skulltula:",
    "Skulltula (Big):",
    "Skulltula (Gold):",
    "Skullwalltula:",
    "Spike:",
    "Stalchild:",
    "Stalfos:",
    "Stinger:",
    "Tailpasaran:",
    "Tektite (Blue):",
    "Tektite (Red):",
    "Torch Slug:",
    "Wallmaster:",
    "Withered Deku Baba:",
    "Wolfos:",
    "Wolfos (White):",
    "Deku Sticks:",
    "Deku Nuts:",
    "Bombs:",
    "Arrows:",
    "Deku Seeds:",
    "Bombchus:",
    "Beans:",
    "A:",
    "B:",
    "L:",
    "R:",
    "Z:",
    "C-Up:",
    "C-Right:",
    "C-Down:",
    "C-Left:",
    "D-Up:",
    "D-Right:",
    "D-Down:",
    "D-Left:",
    "Start:",
};

#define COLOR_WHITE ImVec4(1.00f, 1.00f, 1.00f, 1.00f)
#define COLOR_RED ImVec4(1.00f, 0.00f, 0.00f, 1.00f)
#define COLOR_GREEN ImVec4(0.10f, 1.00f, 0.10f, 1.00f)
#define COLOR_BLUE ImVec4(0.00f, 0.33f, 1.00f, 1.00f)
#define COLOR_PURPLE ImVec4(0.54f, 0.19f, 0.89f, 1.00f)
#define COLOR_YELLOW ImVec4(1.00f, 1.00f, 0.00f, 1.00f)
#define COLOR_ORANGE ImVec4(1.00f, 0.67f, 0.11f, 1.00f)
#define COLOR_LIGHT_BLUE ImVec4(0.00f, 0.88f, 1.00f, 1.00f)
#define COLOR_GREY ImVec4(0.78f, 0.78f, 0.78f, 1.00f)

std::string formatIntGameplayStat(uint32_t value) {
    return fmt::format("{}", value);
}

std::string formatHexGameplayStat(uint32_t value) {
    return fmt::format("{:#x} ({:d})", value, value);
}

std::string formatHexOnlyGameplayStat(uint32_t value) {
    return fmt::format("{:#x}", value, value);
}

void SaveStats(SaveContext* saveContext, int sectionID, bool fullSave) {
    SaveManager::Instance->SaveData("buildVersion", saveContext->ship.stats.buildVersion);
    SaveManager::Instance->SaveData("buildVersionMajor", saveContext->ship.stats.buildVersionMajor);
    SaveManager::Instance->SaveData("buildVersionMinor", saveContext->ship.stats.buildVersionMinor);
    SaveManager::Instance->SaveData("buildVersionPatch", saveContext->ship.stats.buildVersionPatch);
    //
    //SaveManager::Instance->SaveData("heartPieces", saveContext->ship.stats.heartPieces);
    //SaveManager::Instance->SaveData("heartContainers", saveContext->ship.stats.heartContainers);
    //SaveManager::Instance->SaveArray("dungeonKeys", ARRAY_COUNT(saveContext->ship.stats.dungeonKeys), [&](size_t i) {
    //    SaveManager::Instance->SaveData("", saveContext->ship.stats.dungeonKeys[i]);
    //});
    //SaveManager::Instance->SaveData("rtaTiming", saveContext->ship.stats.rtaTiming);
    //SaveManager::Instance->SaveData("fileCreatedAt", saveContext->ship.stats.fileCreatedAt);
    //SaveManager::Instance->SaveData("playTimer", saveContext->ship.stats.playTimer);
    //SaveManager::Instance->SaveData("pauseTimer", saveContext->ship.stats.pauseTimer);
    //SaveManager::Instance->SaveArray(
    //    "itemTimestamps", ARRAY_COUNT(saveContext->ship.stats.itemTimestamp),
    //    [&](size_t i) { SaveManager::Instance->SaveData("", saveContext->ship.stats.itemTimestamp[i]); });
    //SaveManager::Instance->SaveArray(
    //    "sceneTimestamps", ARRAY_COUNT(saveContext->ship.stats.sceneTimestamps), [&](size_t i) {
    //        if (saveContext->ship.stats.sceneTimestamps[i].scene != 254 &&
    //            saveContext->ship.stats.sceneTimestamps[i].room != 254) {
    //            SaveManager::Instance->SaveStruct("", [&]() {
    //                SaveManager::Instance->SaveData("scene", saveContext->ship.stats.sceneTimestamps[i].scene);
    //                SaveManager::Instance->SaveData("room", saveContext->ship.stats.sceneTimestamps[i].room);
    //                SaveManager::Instance->SaveData("sceneTime", saveContext->ship.stats.sceneTimestamps[i].sceneTime);
    //                SaveManager::Instance->SaveData("roomTime", saveContext->ship.stats.sceneTimestamps[i].roomTime);
    //                SaveManager::Instance->SaveData("isRoom", saveContext->ship.stats.sceneTimestamps[i].isRoom);
    //            });
    //        }
    //    });
    //SaveManager::Instance->SaveData("tsIdx", saveContext->ship.stats.tsIdx);
    //SaveManager::Instance->SaveArray("counts", ARRAY_COUNT(saveContext->ship.stats.count), [&](size_t i) {
    //    SaveManager::Instance->SaveData("", saveContext->ship.stats.count[i]);
    //});
    //SaveManager::Instance->SaveArray(
    //    "scenesDiscovered", ARRAY_COUNT(saveContext->ship.stats.scenesDiscovered),
    //    [&](size_t i) { SaveManager::Instance->SaveData("", saveContext->ship.stats.scenesDiscovered[i]); });
    //SaveManager::Instance->SaveArray(
    //    "entrancesDiscovered", ARRAY_COUNT(saveContext->ship.stats.entrancesDiscovered),
    //    [&](size_t i) { SaveManager::Instance->SaveData("", saveContext->ship.stats.entrancesDiscovered[i]); });
}

//void GameplayStatsRow(const char* label, const std::string& value, ImVec4 color = COLOR_WHITE,
//                      const char* tooltip = "") {
//    ImGui::PushStyleColor(ImGuiCol_Text, color);
//    ImGui::TableNextRow();
//    ImGui::TableNextColumn();
//    ImGui::Text("%s", label);
//    ImGui::SameLine(ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize(value.c_str()).x));
//    ImGui::Text("%s", value.c_str());
//    ImGui::PopStyleColor();
//    if (tooltip != "" && ImGui::IsItemHovered()) {
//        ImGui::SetTooltip("%s", tooltip);
//    }
//}

//bool compareTimestampInfoByTime(const TimestampInfo& a, const TimestampInfo& b) {
//    return CVarGetInteger(CVAR_GAMEPLAY_STATS("ReverseTimestamps"), 0) ? a.time > b.time : a.time < b.time;
//}

//const char* ResolveSceneID(int sceneID, int roomID) {
//    if (sceneID == SCENE_GROTTOS) {
//        switch (roomID) {
//            case 0:
//                return "Generic Grotto";
//            case 1:
//                return "Lake Hylia Scrub Grotto";
//            case 2:
//                return "Redead Grotto";
//            case 3:
//                return "Cow Grotto";
//            case 4:
//                return "Scrub Trio";
//            case 5:
//                return "Flooded Grotto";
//            case 6:
//                return "Scrub Duo (Upgrade)";
//            case 7:
//                return "Wolfos Grotto";
//            case 8:
//                return "Hyrule Castle Storms Grotto";
//            case 9:
//                return "Scrub Duo";
//            case 10:
//                return "Tektite Grotto";
//            case 11:
//                return "Forest Stage";
//            case 12:
//                return "Webbed Grotto";
//            case 13:
//                return "Big Skulltula Grotto";
//        };
//    } else if (sceneID == SCENE_WINDMILL_AND_DAMPES_GRAVE) {
//        // Only the last room of Dampe's Grave (rm 6) is considered the windmill.
//        return roomID == 6 ? "Windmill" : "Dampe's Grave";
//    } else if (sceneID < SCENE_ID_MAX) {
//        return sceneMappings[sceneID];
//    }
//
//    return "???";
//}

void DrawGameplayStatsHeader() {
    //ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.0f, 4.0f });
    //ImGui::BeginTable("gameplayStatsHeader", 1, ImGuiTableFlags_BordersOuter);
    //ImGui::TableSetupColumn("stat", ImGuiTableColumnFlags_WidthStretch);
    //// if tag is empty (not a release build)
    //if (gGitCommitTag[0] == 0) {
    //    GameplayStatsRow("Git Branch:", (char*)gGitBranch);
    //    GameplayStatsRow("Git Commit Hash:", (char*)gGitCommitHash);
    //} else {
    //    GameplayStatsRow("Build Version:", (char*)gBuildVersion);
    //}
    //if (gSaveContext.ship.stats.rtaTiming) {
    //    GameplayStatsRow("Total Time (RTA):", formatTimeDisplay(GAMEPLAYSTAT_TOTAL_TIME),
    //                     gSaveContext.ship.stats.gameComplete ? COLOR_GREEN : COLOR_WHITE);
    //} else {
    //    GameplayStatsRow("Total Game Time:", formatTimeDisplay(GAMEPLAYSTAT_TOTAL_TIME),
    //                     gSaveContext.ship.stats.gameComplete ? COLOR_GREEN : COLOR_WHITE);
    //}
    //if (CVarGetInteger(CVAR_GAMEPLAY_STATS("ShowAdditionalTimers"), 0)) { // !Only display total game time
    //    GameplayStatsRow("Gameplay Time:", formatTimeDisplay(gSaveContext.ship.stats.playTimer / 2),
    //                     COLOR_GREY);
    //    GameplayStatsRow("Pause Menu Time:", formatTimeDisplay(gSaveContext.ship.stats.pauseTimer / 3),
    //                     COLOR_GREY);
    //    GameplayStatsRow("Time in scene:", formatTimeDisplay(gSaveContext.ship.stats.sceneTimer / 2),
    //                     COLOR_LIGHT_BLUE);
    //    GameplayStatsRow("Time in room:", formatTimeDisplay(gSaveContext.ship.stats.roomTimer / 2),
    //                     COLOR_LIGHT_BLUE);
    //}
    //if (gPlayState != NULL && CVarGetInteger(CVAR_GAMEPLAY_STATS("ShowDebugInfo"), 0)) { // && display debug info
    //    GameplayStatsRow("play->sceneNum:", formatHexGameplayStat(gPlayState->sceneNum), COLOR_YELLOW);
    //    GameplayStatsRow("gSaveContext.entranceIndex:", formatHexGameplayStat(gSaveContext.entranceIndex),
    //                     COLOR_YELLOW);
    //    GameplayStatsRow("gSaveContext.cutsceneIndex:", formatHexOnlyGameplayStat(gSaveContext.cutsceneIndex),
    //                     COLOR_YELLOW);
    //    GameplayStatsRow("play->roomCtx.curRoom.num:", formatIntGameplayStat(gPlayState->roomCtx.curRoom.num),
    //                     COLOR_YELLOW);
    //}
    //ImGui::EndTable();
    //ImGui::PopStyleVar(1);
}

void DrawGameplayStatsTimestampsTab() {
    //// Set up the array of item timestamps and then sort it chronologically
    //for (int i = 0; i < TIMESTAMP_MAX; i++) {
    //    strcpy(itemTimestampDisplay[i].name, itemTimestampDisplayName[i]);
    //    itemTimestampDisplay[i].time = gSaveContext.ship.stats.itemTimestamp[i];
    //    itemTimestampDisplay[i].color = itemTimestampDisplayColor[i];
    //}
    //
    //std::sort(itemTimestampDisplay, itemTimestampDisplay + TIMESTAMP_MAX, compareTimestampInfoByTime);
    //
    //ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.0f, 4.0f });
    //ImGui::BeginTable("gameplayStatsTimestamps", 1, ImGuiTableFlags_BordersOuter);
    //ImGui::TableSetupColumn("stat", ImGuiTableColumnFlags_WidthStretch);
    //for (int i = 0; i < TIMESTAMP_MAX; i++) {
    //    // To be shown, the entry must have a non-zero time and a string for its display name
    //    if (itemTimestampDisplay[i].time > 0 && strnlen(itemTimestampDisplay[i].name, 21) > 1) {
    //        GameplayStatsRow(itemTimestampDisplay[i].name, formatTimeDisplay(itemTimestampDisplay[i].time),
    //                         itemTimestampDisplay[i].color);
    //    }
    //}
    //ImGui::EndTable();
    //ImGui::PopStyleVar(1);
}

void DrawGameplayStatsCountsTab() {
//    u32 enemiesDefeated = 0;
//    u32 ammoUsed = 0;
//    u32 buttonPresses = 0;
//
//    // Sum of all enemies defeated
//    for (int i = COUNT_ENEMIES_DEFEATED_ANUBIS; i <= COUNT_ENEMIES_DEFEATED_WOLFOS; i++) {
//        if (i == COUNT_ENEMIES_DEFEATED_FLOORMASTER) {
//            // Special case: You must kill 3 mini Floormasters for it count as one defeated Floormaster
//            enemiesDefeated += gSaveContext.ship.stats.count[i] / 3;
//        } else {
//            enemiesDefeated += gSaveContext.ship.stats.count[i];
//        }
//    }
//    // Sum of all ammo used
//    for (int i = COUNT_AMMO_USED_STICK; i <= COUNT_AMMO_USED_BEAN; i++) {
//        ammoUsed += gSaveContext.ship.stats.count[i];
//    }
//    // Sum of all button presses
//    for (int i = COUNT_BUTTON_PRESSES_A; i <= COUNT_BUTTON_PRESSES_START; i++) {
//        buttonPresses += gSaveContext.ship.stats.count[i];
//    }
//
//    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.0f, 4.0f });
//    ImGui::BeginTable("gameplayStatsCounts", 1, ImGuiTableFlags_BordersOuter);
//    ImGui::TableSetupColumn("stat", ImGuiTableColumnFlags_WidthStretch);
//    GameplayStatsRow("Enemies Defeated:", formatIntGameplayStat(enemiesDefeated));
//    if (enemiesDefeated > 0) {
//        ImGui::TableNextRow();
//        ImGui::TableNextColumn();
//        if (ImGui::TreeNodeEx("Enemy Details...", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
//            for (int i = COUNT_ENEMIES_DEFEATED_ANUBIS; i <= COUNT_ENEMIES_DEFEATED_WOLFOS; i++) {
//                if (i == COUNT_ENEMIES_DEFEATED_FLOORMASTER) {
//                    GameplayStatsRow(countMappings[i], formatIntGameplayStat(gSaveContext.ship.stats.count[i] / 3));
//                } else {
//                    GameplayStatsRow(countMappings[i], formatIntGameplayStat(gSaveContext.ship.stats.count[i]));
//                }
//            }
//        }
//    }
//    GameplayStatsRow("Rupees Collected:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_RUPEES_COLLECTED]),
//                     COLOR_WHITE, "Includes rupees collected with a full wallet.");
//    GameplayStatsRow("Rupees Spent:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_RUPEES_SPENT]));
//    GameplayStatsRow("Chests Opened:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_CHESTS_OPENED]));
//    GameplayStatsRow("Ammo Used:", formatIntGameplayStat(ammoUsed));
//    if (ammoUsed > 0) {
//        ImGui::TableNextRow();
//        ImGui::TableNextColumn();
//        if (ImGui::TreeNodeEx("Ammo Details...", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
//            for (int i = COUNT_AMMO_USED_STICK; i <= COUNT_AMMO_USED_BEAN; i++) {
//                GameplayStatsRow(countMappings[i], formatIntGameplayStat(gSaveContext.ship.stats.count[i]));
//            }
//        }
//    }
//    GameplayStatsRow("Damage Taken:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_DAMAGE_TAKEN]));
//    GameplayStatsRow("Sword Swings:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_SWORD_SWINGS]));
//    GameplayStatsRow("Steps Taken:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_STEPS]));
//    // If using MM Bunny Hood enhancement, show how long it's been equipped (not counting pause time)
//    if (CVarGetInteger(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_VANILLA) != BUNNY_HOOD_VANILLA ||
//        gSaveContext.ship.stats.count[COUNT_TIME_BUNNY_HOOD] > 0) {
//        GameplayStatsRow("Bunny Hood Time:",
//                         formatTimeDisplay(gSaveContext.ship.stats.count[COUNT_TIME_BUNNY_HOOD] / 2));
//    }
//    GameplayStatsRow("Rolls:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_ROLLS]));
//    GameplayStatsRow("Bonks:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_BONKS]));
//    GameplayStatsRow("Sidehops:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_SIDEHOPS]));
//    GameplayStatsRow("Backflips:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_BACKFLIPS]));
//    GameplayStatsRow("Ice Traps:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_ICE_TRAPS]));
//    GameplayStatsRow("Pauses:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_PAUSES]));
//    GameplayStatsRow("Pots Smashed:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_POTS_BROKEN]));
//    GameplayStatsRow("Bushes Cut:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_BUSHES_CUT]));
//    GameplayStatsRow("Buttons Pressed:", formatIntGameplayStat(buttonPresses));
//    if (buttonPresses > 0) {
//        ImGui::TableNextRow();
//        ImGui::TableNextColumn();
//        if (ImGui::TreeNodeEx("Buttons...", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
//            for (int i = COUNT_BUTTON_PRESSES_A; i <= COUNT_BUTTON_PRESSES_START; i++) {
//                GameplayStatsRow(countMappings[i], formatIntGameplayStat(gSaveContext.ship.stats.count[i]));
//            }
//        }
//    }
//    ImGui::EndTable();
//    ImGui::PopStyleVar(1);
}

// NEW REGION

void DrawGameplayStatsOptionsTab() {
    if (ImGui::BeginTable("Options", 3)) {
        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Enable Gameplay Stats Tracking", CVAR_SETTING("Gameplaystats.Enable"),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR));

        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Show latest timestamps on top", CVAR_GAMEPLAY_STATS("ReverseTimestamps"),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR));

        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Room Breakdown", CVAR_GAMEPLAY_STATS("RoomBreakdown"),
                                UIWidgets::CheckboxOptions()
                                    .Tooltip("Allows a more in-depth perspective of time spent in a certain map.")
                                    .Color(THEME_COLOR));

        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("RTA Timing on new files", CVAR_GAMEPLAY_STATS("RTATiming"),
                                UIWidgets::CheckboxOptions()
                                    .Tooltip("Timestamps are relative to starting timestamp rather than in game time, "
                                             "usually necessary for races/speedruns.\n\n"
                                             "Starting timestamp is on first non-C-up input after intro cutscene.\n\n"
                                             "NOTE: THIS NEEDS TO BE SET BEFORE CREATING A FILE TO TAKE EFFECT")
                                    .Color(THEME_COLOR));

        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Show additional detail timers", CVAR_GAMEPLAY_STATS("ShowAdditionalTimers"),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR));

        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Show Debug Info", CVAR_GAMEPLAY_STATS("ShowDebugInfo"),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR));

        ImGui::EndTable();
    }
}

void GameplayStats_DrawTimeStamps() {
    if (ImGui::BeginTable("Timestamps", 2)) {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Timestamp");
        ImGui::TableHeadersRow();

        for (auto& entry : currentStatList) {
            ImGui::TableNextColumn();
            ImGui::TextColored(entry.entryColor, entry.entryName.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(entry.entryColor, formatTimeDisplay(entry.entryTimestamp).c_str());
        }

        ImGui::EndTable();
    }
}

void GameplayStatsWindow::DrawElement() {
    DrawGameplayStatsOptionsTab();
    UIWidgets::PaddedSeparator();

    GameplayStats_DrawTimeStamps();
}

nlohmann::json GameplayStats_ObjectToJson(const GameplayStatObject& entry) {
    return nlohmann::json {
        { "entryType", entry.entryType },
        { "entryName", entry.entryName },
        { "entryTimestamp", entry.entryTimestamp },
    };
}

GameplayStatObject GameplayStats_JsonToObject(const nlohmann::json& jsonEntry) {
    GameplayStatObject entry;
    std::unordered_map<uint32_t, GameplayStatEntry> entryList;

    entry.entryType = jsonEntry["entryType"];
    entry.entryName = jsonEntry["entryName"];
    entry.entryTimestamp = jsonEntry["entryTimestamp"];

    if (entry.entryType == STAT_TYPE_SCENE) {
        entryList = sceneList;
    } else if (entry.entryType == STAT_TYPE_ITEM) {
        entryList = itemList;
    } else {
        entryList = eventList;
    }

    for (auto& list : entryList) {
        auto check = list;
        if (list.second.entryName == entry.entryName) {
            entry.entryColor = list.second.entryColor;
            break;
        }
    }

    return entry;
}

uint32_t GameplayStats_ConvertRandoItem(uint32_t item) {
    switch (item) {
        case RG_MAGIC_SINGLE:
            return ITEM_SINGLE_MAGIC;
        case RG_DOUBLE_DEFENSE:
            return ITEM_DOUBLE_DEFENSE;
        default:
            break;
    }
}

void GameplayStats_SaveFileActions(uint32_t action, int32_t fileNum) {
    std::string filename = Ship::Context::GetPathRelativeToAppDirectory("SoHGameplayStats.json");
    json saveFile;
    json listArray = nlohmann::json::array();

    std::ifstream inputFile(filename);
    if (inputFile.is_open()) {
        inputFile >> saveFile;
        inputFile.close();
    }

    if (action == STAT_ACTION_SAVE) {
        for (auto& save : currentStatList) {
            listArray.push_back(GameplayStats_ObjectToJson(save));
        }
        saveFile[std::to_string(gSaveContext.fileNum + 1)]["Timestamps"] = listArray;
    }

    if (action == STAT_ACTION_LOAD) {
        currentStatList.clear();
        if (saveFile.contains(std::to_string(fileNum + 1))) {
            for (auto& load : saveFile[std::to_string(fileNum + 1)]["Timestamps"]) {
                currentStatList.push_back(GameplayStats_JsonToObject(load));
            }
        }
    }

    if (action == STAT_ACTION_DELETE) {
        if (saveFile.contains(std::to_string(fileNum + 1))) {
            saveFile.erase(std::to_string(fileNum + 1));
        }
    }

    std::ofstream outputFile(filename);
    if (outputFile.is_open()) {
        outputFile << saveFile.dump(4);
        outputFile.close();
    }
}

void GameplayStats_GetTimestampByActorId(uint32_t actorId) {
    uint32_t timestampId = -1;
    switch (actorId) {
        case ACTOR_BOSS_DODONGO:
            timestampId = TIMESTAMP_DEFEAT_KING_DODONGO;
            break;
        case ACTOR_BOSS_FD2:
            timestampId = TIMESTAMP_DEFEAT_VOLVAGIA;
            break;
        case ACTOR_BOSS_GANON:
            timestampId = TIMESTAMP_DEFEAT_GANONDORF;
            break;
        case ACTOR_BOSS_GANON2:
            timestampId =  TIMESTAMP_DEFEAT_GANON;
            break;
        case ACTOR_BOSS_GANONDROF:
            timestampId = TIMESTAMP_DEFEAT_PHANTOM_GANON;
            break;
        case ACTOR_BOSS_GOMA:
            timestampId = TIMESTAMP_DEFEAT_GOHMA;
            break;
        case ACTOR_BOSS_MO:
            timestampId = TIMESTAMP_DEFEAT_MORPHA;
            break;
        case ACTOR_BOSS_SST:
            timestampId = TIMESTAMP_DEFEAT_BONGO_BONGO;
            break;
        case ACTOR_BOSS_TW:
            timestampId = TIMESTAMP_DEFEAT_TWINROVA;
            break;
        case ACTOR_BOSS_VA:
            timestampId = TIMESTAMP_DEFEAT_BARINADE;
            break;
    }

    if (timestampId != -1) {
        GameplayStats_AddTimestamp(timestampId, STAT_TYPE_EVENT);
    }
}

void GameplayStats_AddTimestamp(uint32_t entryId, uint32_t entryType) {
    bool fileInit = false;
    uint32_t itemId = entryId;

    if (IS_RANDO && entryType == STAT_TYPE_ITEM) {
        itemId = GameplayStats_ConvertRandoItem(entryId);
    }

    std::unordered_map<uint32_t, GameplayStatEntry> entryList;
    if (entryType == STAT_TYPE_SCENE) {
        entryList = sceneList;
    } else if (entryType == STAT_TYPE_ITEM) {
        entryList = itemList;
    } else {
        entryList = eventList;
    }

    auto entryIt = entryList.find(itemId);
    if (entryIt == entryList.end()) {
        return;
    }

    const GameplayStatEntry& entry = entryIt->second;
    auto it = std::find_if(currentStatList.begin(), currentStatList.end(), [&](const GameplayStatObject& obj) {
        return std::strcmp(obj.entryName.c_str(), entry.entryName.c_str()) == 0;
    });

    if (it != currentStatList.end()) {
        return;
    }

    if (currentStatList.size() == 0 && IS_RANDO && !gPlayState) {
        fileInit = true;
    }

    GameplayStatObject statObject;
    statObject.entryType = entryType;
    statObject.entryName = entry.entryName;
    statObject.entryTimestamp = (gSaveContext.ship.stats.playTimer / 2 + gSaveContext.ship.stats.pauseTimer / 3);
    statObject.entryColor = entry.entryColor;
    currentStatList.push_back(statObject);

    if (fileInit) {
        GameplayStats_SaveFileActions(STAT_ACTION_SAVE, gSaveContext.fileNum + 1);
        fileInit = false;
    }
}

void InitStats(bool isDebug) {
    SohUtils::CopyStringToCharArray(gSaveContext.ship.stats.buildVersion, std::string((char*)gBuildVersion),
                                    ARRAY_COUNT(gSaveContext.ship.stats.buildVersion));
    gSaveContext.ship.stats.buildVersionMajor = gBuildVersionMajor;
    gSaveContext.ship.stats.buildVersionMinor = gBuildVersionMinor;
    gSaveContext.ship.stats.buildVersionPatch = gBuildVersionPatch;
}

void GameplayStatsWindow::InitElement() {
    // Add main section save, no parent.
    SaveManager::Instance->AddSaveFunction("sohStats", 1, SaveStats, true, SECTION_PARENT_NONE);
    // Add subsections, parent of "sohStats". Not sure how to do this without the redundant references to "SaveStats".
    SaveManager::Instance->AddInitFunction(InitStats);
}

void RegisterGameplayStats() {
    if (!std::filesystem::exists(Ship::Context::GetPathRelativeToAppDirectory("SoHGameplayStats.json"))) {
        json initFile;
        std::ofstream file(Ship::Context::GetPathRelativeToAppDirectory("SoHGameplayStats.json"));
        file << initFile.dump(4);
        file.close();
    }

    COND_HOOK(OnItemReceive, CVAR,
              [](GetItemEntry itemEntry) { GameplayStats_AddTimestamp(itemEntry.itemId, STAT_TYPE_ITEM); });
    COND_HOOK(OnBossDefeat, CVAR, [](void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);
        GameplayStats_GetTimestampByActorId(actor->id);
    })
    COND_HOOK(OnDeleteFile, true, [](int32_t fileNum) { GameplayStats_SaveFileActions(STAT_ACTION_DELETE, fileNum); });
    COND_HOOK(OnLoadFile, true, [](int32_t fileNum) { GameplayStats_SaveFileActions(STAT_ACTION_LOAD, fileNum); });
    COND_HOOK(OnSaveFile, true, [](int32_t fileNum) { GameplayStats_SaveFileActions(STAT_ACTION_SAVE, fileNum); });
}

static RegisterShipInitFunc initFunc(RegisterGameplayStats, { CVAR_NAME });
