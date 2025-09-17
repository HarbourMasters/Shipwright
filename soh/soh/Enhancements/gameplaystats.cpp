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

// Enemy Includes for actor->param Ids
#include "src/overlays/actors/ovl_En_Bb/z_en_bb.h"
#include "src/overlays/actors/ovl_En_Dekubaba/z_en_dekubaba.h"
#include "src/overlays/actors/ovl_En_Zf/z_en_zf.h"
#include "src/overlays/actors/ovl_En_Firefly/z_en_firefly.h"
#include "src/overlays/actors/ovl_En_Reeba/z_en_reeba.h"
#include "src/overlays/actors/ovl_En_Peehat/z_en_peehat.h"
#include "src/overlays/actors/ovl_En_Po_Field/z_en_po_field.h"
#include "src/overlays/actors/ovl_En_Poh/z_en_poh.h"
#include "src/overlays/actors/ovl_En_Tp/z_en_tp.h"
#include "src/overlays/actors/ovl_En_Tite/z_en_tite.h"
#include "src/overlays/actors/ovl_En_Wf/z_en_wf.h"

#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_En_Box/z_en_box.h"
#include "overlays/actors/ovl_En_Kusa/z_en_kusa.h"

extern "C" {
#include <z64.h>
#include "variables.h"
extern PlayState* gPlayState;
uint64_t GetUnixTimestamp();

void Player_Action_Roll(Player* thisx, PlayState* play);
void EnBox_Open(EnBox* thisx, PlayState* play);
}

#include <fstream>
#include <filesystem>
#include "TimeDisplay/TimeDisplay.h"

using json = nlohmann::json;

#define CVAR_NAME "gSettings.Gameplaystats.Enable"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

std::vector<GameplayStatObject> currentTimestamps;
std::vector<GameplayStatObject> currentCounts;
std::vector<GameplayStatObject> currentSceneTimers;
uint32_t typeIndex = STAT_TYPE_ALL;
ImVec4 emptyColor = { 0, 0, 0, 0 };
bool isRandoItem = false;
bool isRolling = false;
bool isHopping = false;

static std::unordered_map<uint32_t, const char*> statTypeNameMap = {
    { STAT_TYPE_SCENE, "Scenes" },
    { STAT_TYPE_ITEM, "Items" },
    { STAT_TYPE_EVENT, "Events" },
    { STAT_TYPE_ALL, "All" },
};

// clang-format off
std::unordered_map<uint32_t, std::map<uint32_t, GameplayStatObject>> gameplayStatList = {
    { STAT_TYPE_SCENE, 
        {
            { SCENE_DEKU_TREE, 							{ STAT_TYPE_SCENE, "Inside the Deku Tree", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_DODONGOS_CAVERN,                    { STAT_TYPE_SCENE, "Dodongo's Cavern",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_JABU_JABU,                          { STAT_TYPE_SCENE, "Inside Jabu-Jabu's Belly",  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_FOREST_TEMPLE,                      { STAT_TYPE_SCENE, "Forest Temple",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_FIRE_TEMPLE,                        { STAT_TYPE_SCENE, "Fire Temple",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_WATER_TEMPLE,                       { STAT_TYPE_SCENE, "Water Temple",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_SPIRIT_TEMPLE,                      { STAT_TYPE_SCENE, "Spirit Temple",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_SHADOW_TEMPLE,                      { STAT_TYPE_SCENE, "Shadow Temple",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_BOTTOM_OF_THE_WELL,                 { STAT_TYPE_SCENE, "Bottom of the Well",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_ICE_CAVERN,                         { STAT_TYPE_SCENE, "Ice Cavern",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GANONS_TOWER,                       { STAT_TYPE_SCENE, "Ganon's Tower",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GERUDO_TRAINING_GROUND,             { STAT_TYPE_SCENE, "Gerudo Training Ground",    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_THIEVES_HIDEOUT,                    { STAT_TYPE_SCENE, "Thieves' Hideout",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_INSIDE_GANONS_CASTLE,               { STAT_TYPE_SCENE, "Inside Ganon's Castle",     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GANONS_TOWER_COLLAPSE_INTERIOR,     { STAT_TYPE_SCENE, "Tower Collapse",            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_INSIDE_GANONS_CASTLE_COLLAPSE,      { STAT_TYPE_SCENE, "Castle Collapse",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_TREASURE_BOX_SHOP,                  { STAT_TYPE_SCENE, "Treasure Box Shop",         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_DEKU_TREE_BOSS,                     { STAT_TYPE_SCENE, "Gohma's Lair",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_DODONGOS_CAVERN_BOSS,               { STAT_TYPE_SCENE, "King Dodongo's Lair",       UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_JABU_JABU_BOSS,                     { STAT_TYPE_SCENE, "Barinade's Lair",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_FOREST_TEMPLE_BOSS,                 { STAT_TYPE_SCENE, "Phantom Ganon's Lair",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_FIRE_TEMPLE_BOSS,                   { STAT_TYPE_SCENE, "Volvagia's Lair",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_WATER_TEMPLE_BOSS,                  { STAT_TYPE_SCENE, "Morpha's Lair",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_SPIRIT_TEMPLE_BOSS,                 { STAT_TYPE_SCENE, "Twinrova's Lair",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_SHADOW_TEMPLE_BOSS,                 { STAT_TYPE_SCENE, "Bongo Bongo's Lair",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GANONDORF_BOSS,                     { STAT_TYPE_SCENE, "Ganondorf's Lair",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR,     { STAT_TYPE_SCENE, "Ganon's Lair",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_MARKET_ENTRANCE_DAY,                { STAT_TYPE_SCENE, "Market Entrance (Day)",     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_MARKET_ENTRANCE_NIGHT,              { STAT_TYPE_SCENE, "Market Entrance (Night)",   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_MARKET_ENTRANCE_RUINS,              { STAT_TYPE_SCENE, "Market Entrance (Adult)",   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_BACK_ALLEY_DAY,                     { STAT_TYPE_SCENE, "Back Alley (Day)",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_BACK_ALLEY_NIGHT,                   { STAT_TYPE_SCENE, "Back Alley (Night)",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_MARKET_DAY,                         { STAT_TYPE_SCENE, "Market (Day)",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_MARKET_NIGHT,                       { STAT_TYPE_SCENE, "Market (Night)",            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_MARKET_RUINS,                       { STAT_TYPE_SCENE, "Market (Adult)",            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY,        { STAT_TYPE_SCENE, "Outside ToT (Day)",         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT,      { STAT_TYPE_SCENE, "Outside ToT (Night)",       UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS,      { STAT_TYPE_SCENE, "Outside ToT (Adult)",       UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_KNOW_IT_ALL_BROS_HOUSE,             { STAT_TYPE_SCENE, "Know-It-All Bros' House",   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_TWINS_HOUSE,                        { STAT_TYPE_SCENE, "Twins' House",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_MIDOS_HOUSE,                        { STAT_TYPE_SCENE, "Mido's House",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_SARIAS_HOUSE,                       { STAT_TYPE_SCENE, "Saria's House",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_KAKARIKO_CENTER_GUEST_HOUSE,        { STAT_TYPE_SCENE, "Carpenter Boss's House",    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_BACK_ALLEY_HOUSE,                   { STAT_TYPE_SCENE, "Man in Green's House",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_BAZAAR,                             { STAT_TYPE_SCENE, "Bazaar",                    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_KOKIRI_SHOP,                        { STAT_TYPE_SCENE, "Kokiri Shop",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GORON_SHOP,                         { STAT_TYPE_SCENE, "Goron Shop",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_ZORA_SHOP,                          { STAT_TYPE_SCENE, "Zora Shop",                 UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_POTION_SHOP_KAKARIKO,               { STAT_TYPE_SCENE, "Kakariko Potion Shop",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_POTION_SHOP_MARKET,                 { STAT_TYPE_SCENE, "Market Potion Shop",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_BOMBCHU_SHOP,                       { STAT_TYPE_SCENE, "Bombchu Shop",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_HAPPY_MASK_SHOP,                    { STAT_TYPE_SCENE, "Happy Mask Shop",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_LINKS_HOUSE,                        { STAT_TYPE_SCENE, "Link's House",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_DOG_LADY_HOUSE,                     { STAT_TYPE_SCENE, "Richard's House",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_STABLE,                             { STAT_TYPE_SCENE, "Stable",                    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_IMPAS_HOUSE,                        { STAT_TYPE_SCENE, "Impa's House",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_LAKESIDE_LABORATORY,                { STAT_TYPE_SCENE, "Lakeside Lab",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_CARPENTERS_TENT,                    { STAT_TYPE_SCENE, "Carpenters' Tent",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GRAVEKEEPERS_HUT,                   { STAT_TYPE_SCENE, "Gravekeeper's Hut",         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC,        { STAT_TYPE_SCENE, "Great Fairy",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_FAIRYS_FOUNTAIN,                    { STAT_TYPE_SCENE, "Fairy Fountain",            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS,       { STAT_TYPE_SCENE, "Great Fairy",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GROTTOS,                            { STAT_TYPE_SCENE, "Grotto",                    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_REDEAD_GRAVE,                       { STAT_TYPE_SCENE, "Redead Grave",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GRAVE_WITH_FAIRYS_FOUNTAIN,         { STAT_TYPE_SCENE, "Fairy Fountain Grave",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_ROYAL_FAMILYS_TOMB,                 { STAT_TYPE_SCENE, "Royal Family's Tomb",       UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_SHOOTING_GALLERY,                   { STAT_TYPE_SCENE, "Shooting Gallery",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_TEMPLE_OF_TIME,                     { STAT_TYPE_SCENE, "Temple of Time",            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_CHAMBER_OF_THE_SAGES,               { STAT_TYPE_SCENE, "Chamber of Sages",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_CASTLE_COURTYARD_GUARDS_DAY,        { STAT_TYPE_SCENE, "Castle Maze (Day)",         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_CASTLE_COURTYARD_GUARDS_NIGHT,      { STAT_TYPE_SCENE, "Castle Maze (Night)",       UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_CUTSCENE_MAP,                       { STAT_TYPE_SCENE, "Cutscene Map",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_WINDMILL_AND_DAMPES_GRAVE,          { STAT_TYPE_SCENE, "Dampe's Grave",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_FISHING_POND,                       { STAT_TYPE_SCENE, "Fishing Pond",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_CASTLE_COURTYARD_ZELDA,             { STAT_TYPE_SCENE, "Castle Courtyard",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_BOMBCHU_BOWLING_ALLEY,              { STAT_TYPE_SCENE, "Bombchu Bowling Alley",     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_LON_LON_BUILDINGS,                  { STAT_TYPE_SCENE, "Ranch House",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_MARKET_GUARD_HOUSE,                 { STAT_TYPE_SCENE, "Guard House",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_POTION_SHOP_GRANNY,                 { STAT_TYPE_SCENE, "Granny's Potion Shop",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GANON_BOSS,                         { STAT_TYPE_SCENE, "Ganon Fight",               UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_HOUSE_OF_SKULLTULA,                 { STAT_TYPE_SCENE, "House of Skulltula",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_HYRULE_FIELD,                       { STAT_TYPE_SCENE, "Hyrule Field",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_KAKARIKO_VILLAGE,                   { STAT_TYPE_SCENE, "Kakariko Village",          UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GRAVEYARD,                          { STAT_TYPE_SCENE, "Graveyard",                 UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_ZORAS_RIVER,                        { STAT_TYPE_SCENE, "Zora's River",              UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_KOKIRI_FOREST,                      { STAT_TYPE_SCENE, "Kokiri Forest",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_SACRED_FOREST_MEADOW,               { STAT_TYPE_SCENE, "Sacred Forest Meadow",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_LAKE_HYLIA,                         { STAT_TYPE_SCENE, "Lake Hylia",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_ZORAS_DOMAIN,                       { STAT_TYPE_SCENE, "Zora's Domain",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_ZORAS_FOUNTAIN,                     { STAT_TYPE_SCENE, "Zora's Fountain",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GERUDO_VALLEY,                      { STAT_TYPE_SCENE, "Gerudo Valley",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_LOST_WOODS,                         { STAT_TYPE_SCENE, "Lost Woods",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_DESERT_COLOSSUS,                    { STAT_TYPE_SCENE, "Desert Colossus",           UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GERUDOS_FORTRESS,                   { STAT_TYPE_SCENE, "Gerudo's Fortress",         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_HAUNTED_WASTELAND,                  { STAT_TYPE_SCENE, "Haunted Wasteland",         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_HYRULE_CASTLE,                      { STAT_TYPE_SCENE, "Hyrule Castle",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_DEATH_MOUNTAIN_TRAIL,               { STAT_TYPE_SCENE, "Death Mountain Trail",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_DEATH_MOUNTAIN_CRATER,              { STAT_TYPE_SCENE, "Death Mountain Crater",     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_GORON_CITY,                         { STAT_TYPE_SCENE, "Goron City",                UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_LON_LON_RANCH,                      { STAT_TYPE_SCENE, "Lon Lon Ranch",             UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_OUTSIDE_GANONS_CASTLE,              { STAT_TYPE_SCENE, "Outside Ganon's Castle",    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            // Debug Only Scenes
            { SCENE_TEST01, 		                    { STAT_TYPE_SCENE, "Test Map", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_BESITU, 		                    { STAT_TYPE_SCENE, "Test Room", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_DEPTH_TEST, 	                    { STAT_TYPE_SCENE, "Depth Test", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_SYOTES, 		                    { STAT_TYPE_SCENE, "Stalfos Mini-Boss", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_SYOTES2, 		                    { STAT_TYPE_SCENE, "Stalfos Boss", 			    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_SUTARU, 		                    { STAT_TYPE_SCENE, "Dark Link", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_HAIRAL_NIWA2, 	                    { STAT_TYPE_SCENE, "Castle Maze (Broken)", 	    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_SASATEST, 		                    { STAT_TYPE_SCENE, "SRD Room", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { SCENE_TESTROOM, 		                    { STAT_TYPE_SCENE, "Chest Room", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
        }
    },
    { STAT_TYPE_ITEM, 
        {
            { ITEM_STICK,                           { STAT_TYPE_ITEM, "Deku Stick", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_NUT,                             { STAT_TYPE_ITEM, "Deku Nut", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOMB,                            { STAT_TYPE_ITEM, "Bombs", 			            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOW,                             { STAT_TYPE_ITEM, "Fairy Bow", 		            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_ARROW_FIRE,                      { STAT_TYPE_ITEM, "Fire Arrows", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_DINS_FIRE,                       { STAT_TYPE_ITEM, "Din's Fire", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SLINGSHOT,                       { STAT_TYPE_ITEM, "Slingshot", 		            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_OCARINA_FAIRY,                   { STAT_TYPE_ITEM, "Fairy Ocarina", 	            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_OCARINA_TIME,                    { STAT_TYPE_ITEM, "Ocarina of Time", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOMBCHU,                         { STAT_TYPE_ITEM, "Bombchus", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_HOOKSHOT,                        { STAT_TYPE_ITEM, "Hookshot", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_LONGSHOT,                        { STAT_TYPE_ITEM, "Longshot", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_ARROW_ICE,                       { STAT_TYPE_ITEM, "Ice Arrows", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_FARORES_WIND,                    { STAT_TYPE_ITEM, "Farore's Wind", 	            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOOMERANG,                       { STAT_TYPE_ITEM, "Boomerang", 		            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_LENS,                            { STAT_TYPE_ITEM, "Lens of Truth", 	            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BEAN,                            { STAT_TYPE_ITEM, "Magic Beans", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_HAMMER,                          { STAT_TYPE_ITEM, "Megaton Hammer", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_ARROW_LIGHT,                     { STAT_TYPE_ITEM, "Light Arrows", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_NAYRUS_LOVE,                     { STAT_TYPE_ITEM, "Nayru's Love", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOTTLE,                          { STAT_TYPE_ITEM, "Bottle", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_POTION_RED,                      { STAT_TYPE_ITEM, "Red Potion", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_POTION_GREEN,                    { STAT_TYPE_ITEM, "Green Potion", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_POTION_BLUE,                     { STAT_TYPE_ITEM, "Blue Potion", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_FAIRY,                           { STAT_TYPE_ITEM, "Fairy", 			            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_FISH,                            { STAT_TYPE_ITEM, "Fish", 				        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_MILK_BOTTLE,                     { STAT_TYPE_ITEM, "Milk (Full)", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_LETTER_RUTO,                     { STAT_TYPE_ITEM, "Ruto's Letter", 	            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BLUE_FIRE,                       { STAT_TYPE_ITEM, "Blue Fire", 		            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BUG,                             { STAT_TYPE_ITEM, "Bugs", 				        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BIG_POE,                         { STAT_TYPE_ITEM, "Big Poe", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_MILK_HALF,                       { STAT_TYPE_ITEM, "Milk (Half)", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_POE,                             { STAT_TYPE_ITEM, "Poe", 				        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_WEIRD_EGG,                       { STAT_TYPE_ITEM, "Weird Egg", 		            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_CHICKEN,                         { STAT_TYPE_ITEM, "Chicken", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_LETTER_ZELDA,                    { STAT_TYPE_ITEM, "Zelda's Letter", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_MASK_KEATON,                     { STAT_TYPE_ITEM, "Keaton Mask", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_MASK_SKULL,                      { STAT_TYPE_ITEM, "Skull Mask", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_MASK_SPOOKY,                     { STAT_TYPE_ITEM, "Spooky Mask", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_MASK_BUNNY,                      { STAT_TYPE_ITEM, "Bunny Hood", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_MASK_GORON,                      { STAT_TYPE_ITEM, "Goron Mask", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_MASK_ZORA,                       { STAT_TYPE_ITEM, "Zora Mask", 		            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_MASK_GERUDO,                     { STAT_TYPE_ITEM, "Gerudo Mask", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_MASK_TRUTH,                      { STAT_TYPE_ITEM, "Mask of Truth", 	            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SOLD_OUT,                        { STAT_TYPE_ITEM, "Sold Out", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_POCKET_EGG,                      { STAT_TYPE_ITEM, "Pocket Egg", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_POCKET_CUCCO,                    { STAT_TYPE_ITEM, "Pocket Cucco", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_COJIRO,                          { STAT_TYPE_ITEM, "Cojiro", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_ODD_MUSHROOM,                    { STAT_TYPE_ITEM, "Odd Mushroom", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_ODD_POTION,                      { STAT_TYPE_ITEM, "Odd Potion", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SAW,                             { STAT_TYPE_ITEM, "Poacher's Saw", 	            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SWORD_BROKEN,                    { STAT_TYPE_ITEM, "Broken Goron Sword",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_PRESCRIPTION,                    { STAT_TYPE_ITEM, "Prescription", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_FROG,                            { STAT_TYPE_ITEM, "Eyeball Frog", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_EYEDROPS,                        { STAT_TYPE_ITEM, "Eye Drops", 		            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_CLAIM_CHECK,                     { STAT_TYPE_ITEM, "Claim Check", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SWORD_KOKIRI,                    { STAT_TYPE_ITEM, "Kokiri Sword", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SWORD_MASTER,                    { STAT_TYPE_ITEM, "Master Sword", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SWORD_BGS,                       { STAT_TYPE_ITEM, "Biggoron's Sword", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SHIELD_DEKU,                     { STAT_TYPE_ITEM, "Deku Shield", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SHIELD_HYLIAN,                   { STAT_TYPE_ITEM, "Hylian Shield", 	            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SHIELD_MIRROR,                   { STAT_TYPE_ITEM, "Mirror Shield", 	            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_TUNIC_GORON,                     { STAT_TYPE_ITEM, "Goron Tunic", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_TUNIC_ZORA,                      { STAT_TYPE_ITEM, "Zora Tunic", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOOTS_IRON,                      { STAT_TYPE_ITEM, "Iron Boots", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOOTS_HOVER,                     { STAT_TYPE_ITEM, "Hover Boots", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BULLET_BAG_30,                   { STAT_TYPE_ITEM, "Bullet Bag", 			    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BULLET_BAG_40,                   { STAT_TYPE_ITEM, "Bigger Bullet Bag", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BULLET_BAG_50,                   { STAT_TYPE_ITEM, "Biggest Bullet Bag", 	    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_QUIVER_30,                       { STAT_TYPE_ITEM, "Quiver", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_QUIVER_40,                       { STAT_TYPE_ITEM, "Big Quiver", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_QUIVER_50,                       { STAT_TYPE_ITEM, "Biggest Quiver", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOMB_BAG_20,                     { STAT_TYPE_ITEM, "Bomb Bag", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOMB_BAG_30,                     { STAT_TYPE_ITEM, "Big Bomb Bag", 			    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOMB_BAG_40,                     { STAT_TYPE_ITEM, "Biggest Bomb Bag", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BRACELET,                        { STAT_TYPE_ITEM, "Goron's Bracelet", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_GAUNTLETS_SILVER,                { STAT_TYPE_ITEM, "Silver Gauntlets", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_GAUNTLETS_GOLD,                  { STAT_TYPE_ITEM, "Gold Gauntlets", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SCALE_SILVER,                    { STAT_TYPE_ITEM, "Silver Scale", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SCALE_GOLDEN,                    { STAT_TYPE_ITEM, "Gold Scale", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SWORD_KNIFE,                     { STAT_TYPE_ITEM, "Giant's Knife", 			    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_WALLET_ADULT,                    { STAT_TYPE_ITEM, "Adult's Wallet", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_WALLET_GIANT,                    { STAT_TYPE_ITEM, "Giant's Wallet", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_FISHING_POLE,                    { STAT_TYPE_ITEM, "Fishing Pole", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SONG_MINUET,                     { STAT_TYPE_ITEM, "Minuet of Forest", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Green) } },
            { ITEM_SONG_BOLERO,                     { STAT_TYPE_ITEM, "Bolero of Fire", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Red) } },
            { ITEM_SONG_SERENADE,                   { STAT_TYPE_ITEM, "Serenade of Water",          UIWidgets::ColorValues.at(UIWidgets::Colors::Blue) } },
            { ITEM_SONG_REQUIEM,                    { STAT_TYPE_ITEM, "Requiem of Spirit",          UIWidgets::ColorValues.at(UIWidgets::Colors::Orange) } },
            { ITEM_SONG_NOCTURNE,                   { STAT_TYPE_ITEM, "Nocturne of Shadow",         UIWidgets::ColorValues.at(UIWidgets::Colors::Purple) } },
            { ITEM_SONG_PRELUDE,                    { STAT_TYPE_ITEM, "Prelude of Light", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Yellow) } },
            { ITEM_SONG_LULLABY,                    { STAT_TYPE_ITEM, "Zelda's Lullaby", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Purple) } },
            { ITEM_SONG_EPONA,                      { STAT_TYPE_ITEM, "Epona's Song", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::Orange) } },
            { ITEM_SONG_SARIA,                      { STAT_TYPE_ITEM, "Saria's Song", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::Green) } },
            { ITEM_SONG_SUN,                        { STAT_TYPE_ITEM, "Sun's Song", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::Yellow) } },
            { ITEM_SONG_TIME,                       { STAT_TYPE_ITEM, "Song of Time", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::LightBlue) } },
            { ITEM_SONG_STORMS,                     { STAT_TYPE_ITEM, "Song of Storms", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Gray) } },
            { ITEM_MEDALLION_FOREST,                { STAT_TYPE_ITEM, "Forest Medallion", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Green) } },
            { ITEM_MEDALLION_FIRE,                  { STAT_TYPE_ITEM, "Fire Medallion", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Red) } },
            { ITEM_MEDALLION_WATER,                 { STAT_TYPE_ITEM, "Water Medallion", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Blue) } },
            { ITEM_MEDALLION_SPIRIT,                { STAT_TYPE_ITEM, "Spirit Medallion", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Orange) } },
            { ITEM_MEDALLION_SHADOW,                { STAT_TYPE_ITEM, "Shadow Medallion", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Purple) } },
            { ITEM_MEDALLION_LIGHT,                 { STAT_TYPE_ITEM, "Light Medallion", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Yellow) } },
            { ITEM_KOKIRI_EMERALD,                  { STAT_TYPE_ITEM, "Kokiri's Emerald", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Green) } },
            { ITEM_GORON_RUBY,                      { STAT_TYPE_ITEM, "Goron's Ruby", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::Red) } },
            { ITEM_ZORA_SAPPHIRE,                   { STAT_TYPE_ITEM, "Zora's Sapphire", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::Blue) } },
            { ITEM_STONE_OF_AGONY,                  { STAT_TYPE_ITEM, "Stone of Agony", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_GERUDO_CARD,                     { STAT_TYPE_ITEM, "Gerudo's Card", 	            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SKULL_TOKEN,                     { STAT_TYPE_ITEM, "Gold Skulltula Token", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_HEART_CONTAINER,                 { STAT_TYPE_ITEM, "Heart Container", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_HEART_PIECE,                     { STAT_TYPE_ITEM, "Piece of Heart", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_KEY_BOSS,                        { STAT_TYPE_ITEM, "Ganon's Boss Key", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_COMPASS,                         { STAT_TYPE_ITEM, "Compass", 					UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_DUNGEON_MAP,                     { STAT_TYPE_ITEM, "Dungeon Map", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_KEY_SMALL,                       { STAT_TYPE_ITEM, "Small Key", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_HEART_PIECE_2,                   { STAT_TYPE_ITEM, "Piece of Heart", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SINGLE_MAGIC,                    { STAT_TYPE_ITEM, "Magic", 			            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_DOUBLE_MAGIC,                    { STAT_TYPE_ITEM, "Double Magic", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_DOUBLE_DEFENSE,                  { STAT_TYPE_ITEM, "Double Defense", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_STICKS_5,                        { STAT_TYPE_ITEM, "Deku Stick", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_STICKS_10,                       { STAT_TYPE_ITEM, "Deku Stick", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_NUTS_5,                          { STAT_TYPE_ITEM, "Deku Nut", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_NUTS_10,                         { STAT_TYPE_ITEM, "Deku Nut", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOMBCHUS_5,                      { STAT_TYPE_ITEM, "Bombchus", 			        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOMBCHUS_20,                     { STAT_TYPE_ITEM, "Bombchus", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_STICK_UPGRADE_20,                { STAT_TYPE_ITEM, "Deku Stick Upgrade (20)",    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_STICK_UPGRADE_30,                { STAT_TYPE_ITEM, "Deku Stick Upgrade (30)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_NUT_UPGRADE_30,                  { STAT_TYPE_ITEM, "Deku Nut Upgrade (30)", 	    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_NUT_UPGRADE_40,                  { STAT_TYPE_ITEM, "Deku Nut Upgrade (30)", 	    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
        }
    },
    { STAT_TYPE_EVENT, 
        {
            { TIMESTAMP_DEFEAT_GOHMA,               { STAT_TYPE_EVENT, "Queen Gohma Defeated",   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { TIMESTAMP_DEFEAT_KING_DODONGO,        { STAT_TYPE_EVENT, "King Dodongo Defeated",  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { TIMESTAMP_DEFEAT_BARINADE,            { STAT_TYPE_EVENT, "Barinade Defeated",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { TIMESTAMP_DEFEAT_PHANTOM_GANON,       { STAT_TYPE_EVENT, "Phantom Ganon Defeated", UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { TIMESTAMP_DEFEAT_VOLVAGIA,            { STAT_TYPE_EVENT, "Volvagia Defeated",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { TIMESTAMP_DEFEAT_MORPHA,              { STAT_TYPE_EVENT, "Morpha Defeated",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { TIMESTAMP_DEFEAT_BONGO_BONGO,         { STAT_TYPE_EVENT, "Bongo Bongo Defeated",   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { TIMESTAMP_DEFEAT_TWINROVA,            { STAT_TYPE_EVENT, "Twinrova Defeated",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { TIMESTAMP_DEFEAT_GANONDORF,           { STAT_TYPE_EVENT, "Ganondorf Defeated",     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { TIMESTAMP_DEFEAT_GANON,               { STAT_TYPE_EVENT, "Ganon Defeated",         UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { TIMESTAMP_BOSSRUSH_FINISH,            { STAT_TYPE_EVENT, "Boss Rush Complete",     UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { TIMESTAMP_TRIFORCE_COMPLETED,         { STAT_TYPE_EVENT, "Triforce Complete",      UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
        }
    },
};

std::unordered_map<uint32_t, std::map<uint32_t, GameplayStatObject>> gameplayCountList = {
    { STAT_TYPE_ENEMY,
        {
            { COUNT_ENEMIES_DEFEATED_ANUBIS,               { STAT_TYPE_ENEMY, "Anubis", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_ARMOS,                { STAT_TYPE_ENEMY, "Armos", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_ARWING,               { STAT_TYPE_ENEMY, "Arwing", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_BARI,                 { STAT_TYPE_ENEMY, "Bari", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_BEAMOS,               { STAT_TYPE_ENEMY, "Biri", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_BIG_OCTO,             { STAT_TYPE_ENEMY, "Beamos", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_BIRI,                 { STAT_TYPE_ENEMY, "Big Octo", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_BUBBLE_GREEN,         { STAT_TYPE_ENEMY, "Bubble (Blue)", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_BUBBLE_BLUE,          { STAT_TYPE_ENEMY, "Bubble (Green)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_BUBBLE_WHITE,         { STAT_TYPE_ENEMY, "Bubble (Red)", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_BUBBLE_RED,           { STAT_TYPE_ENEMY, "Bubble (White)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_BUSINESS_SCRUB,       { STAT_TYPE_ENEMY, "Business Scrub", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_DARK_LINK,            { STAT_TYPE_ENEMY, "Dark Link", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_DEAD_HAND,            { STAT_TYPE_ENEMY, "Dead Hand", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_DEKU_BABA,            { STAT_TYPE_ENEMY, "Deku Baba", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_DEKU_BABA_BIG,        { STAT_TYPE_ENEMY, "Deku Baba (Big)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_DEKU_SCRUB,           { STAT_TYPE_ENEMY, "Deku Scrub", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_DINOLFOS,             { STAT_TYPE_ENEMY, "Dinolfos", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_DODONGO,              { STAT_TYPE_ENEMY, "Dodongo", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_DODONGO_BABY,         { STAT_TYPE_ENEMY, "Dodongo (Baby)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_DOOR_TRAP,            { STAT_TYPE_ENEMY, "Door Mimic", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_FLARE_DANCER,         { STAT_TYPE_ENEMY, "Flare Dancer", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_FLOORMASTER,          { STAT_TYPE_ENEMY, "Floormaster", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_FLYING_POT,           { STAT_TYPE_ENEMY, "Flying Floor Tile", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_FLOOR_TILE,           { STAT_TYPE_ENEMY, "Flying Pot", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_FREEZARD,             { STAT_TYPE_ENEMY, "Freezard", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_GERUDO_THIEF,         { STAT_TYPE_ENEMY, "Gerudo Thief", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_GIBDO,                { STAT_TYPE_ENEMY, "Gibdo", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_GOHMA_LARVA,          { STAT_TYPE_ENEMY, "Gohma Larva", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_GUAY,                 { STAT_TYPE_ENEMY, "Guay", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE,         { STAT_TYPE_ENEMY, "Iron Knuckle", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE_NABOORU, { STAT_TYPE_ENEMY, "Iron Knuckle (Nab)", UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_KEESE,                { STAT_TYPE_ENEMY, "Keese", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_KEESE_FIRE,           { STAT_TYPE_ENEMY, "Keese (Fire)", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_KEESE_ICE,            { STAT_TYPE_ENEMY, "Keese (Ice)", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_LEEVER,               { STAT_TYPE_ENEMY, "Leever", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_LEEVER_BIG,           { STAT_TYPE_ENEMY, "Leever (Big)", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_LIKE_LIKE,            { STAT_TYPE_ENEMY, "Like-Like", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_LIZALFOS,             { STAT_TYPE_ENEMY, "Lizalfos", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_MAD_SCRUB,            { STAT_TYPE_ENEMY, "Mad Scrub", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_MOBLIN,               { STAT_TYPE_ENEMY, "Moblin", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_MOBLIN_CLUB,          { STAT_TYPE_ENEMY, "Moblin (Club)", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_OCTOROK,              { STAT_TYPE_ENEMY, "Octorok", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_PARASITIC_TENTACLE,   { STAT_TYPE_ENEMY, "Parasitic Tentacle", UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_PEAHAT,               { STAT_TYPE_ENEMY, "Peahat", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_PEAHAT_LARVA,         { STAT_TYPE_ENEMY, "Peahat Larva", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_POE,                  { STAT_TYPE_ENEMY, "Poe", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_POE_BIG,              { STAT_TYPE_ENEMY, "Poe (Big)", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_POE_COMPOSER,         { STAT_TYPE_ENEMY, "Poe (Composer)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_POE_SISTERS,          { STAT_TYPE_ENEMY, "Poe Sisters", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_REDEAD,               { STAT_TYPE_ENEMY, "Redead", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_SHABOM,               { STAT_TYPE_ENEMY, "Shabom", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_SHELLBLADE,           { STAT_TYPE_ENEMY, "Shell Blade", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_SKULLTULA,            { STAT_TYPE_ENEMY, "Skull Kid", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_SKULLTULA_BIG,        { STAT_TYPE_ENEMY, "Skulltula", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_SKULLTULA_GOLD,       { STAT_TYPE_ENEMY, "Skulltula (Big)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_SKULLWALLTULA,        { STAT_TYPE_ENEMY, "Skulltula (Gold)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_SKULL_KID,            { STAT_TYPE_ENEMY, "Skullwalltula", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_SPIKE,                { STAT_TYPE_ENEMY, "Spike", 				UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_STALCHILD,            { STAT_TYPE_ENEMY, "Stalchild", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_STALFOS,              { STAT_TYPE_ENEMY, "Stalfos", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_STINGER,              { STAT_TYPE_ENEMY, "Stinger", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_TAILPASARAN,          { STAT_TYPE_ENEMY, "Tailpasaran", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_TEKTITE_BLUE,         { STAT_TYPE_ENEMY, "Tektite (Blue)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_TEKTITE_RED,          { STAT_TYPE_ENEMY, "Tektite (Red)", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_TORCH_SLUG,           { STAT_TYPE_ENEMY, "Torch Slug", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_WALLMASTER,           { STAT_TYPE_ENEMY, "Wallmaster", 		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_WITHERED_DEKU_BABA,   { STAT_TYPE_ENEMY, "Withered Deku Baba", UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_WOLFOS,               { STAT_TYPE_ENEMY, "Wolfos", 			UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ENEMIES_DEFEATED_WOLFOS_WHITE,         { STAT_TYPE_ENEMY, "Wolfos (White)", 	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
        }
    },
    { STAT_TYPE_PLAYER,
        {
            { ITEM_STICK,	                { STAT_TYPE_PLAYER, "Consumed - Deku Stick",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_NUT,		                { STAT_TYPE_PLAYER, "Consumed - Deku Nut", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOMB,		            { STAT_TYPE_PLAYER, "Consumed - Bomb", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOW,                     { STAT_TYPE_PLAYER, "Consumed - Arrow", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_SLINGSHOT,	            { STAT_TYPE_PLAYER, "Consumed - Seed", 		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BOMBCHU,	                { STAT_TYPE_PLAYER, "Consumed - Bombchu", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { ITEM_BEAN,		            { STAT_TYPE_PLAYER, "Consumed - Magic Bean",        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_A,	    { STAT_TYPE_PLAYER, "Pressed - A",				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_B,		{ STAT_TYPE_PLAYER, "Pressed - B", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_L,		{ STAT_TYPE_PLAYER, "Pressed - L", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_R,       { STAT_TYPE_PLAYER, "Pressed - R", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_Z,		{ STAT_TYPE_PLAYER, "Pressed - Z", 				    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_CUP,	    { STAT_TYPE_PLAYER, "Pressed - C Up", 			    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_CRIGHT,	{ STAT_TYPE_PLAYER, "Pressed - C Right",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_CDOWN,	{ STAT_TYPE_PLAYER, "Pressed - C Down",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_CLEFT,	{ STAT_TYPE_PLAYER, "Pressed - C Left", 		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_DUP,		{ STAT_TYPE_PLAYER, "Pressed - D-Pad Up", 		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_DRIGHT, 	{ STAT_TYPE_PLAYER, "Pressed - D-Pad Right", 	    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_DDOWN,	{ STAT_TYPE_PLAYER, "Pressed - D-Pad Down", 	    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_DLEFT,	{ STAT_TYPE_PLAYER, "Pressed - D-Pad Left", 	    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUTTON_PRESSES_START,	{ STAT_TYPE_PLAYER, "Pressed - Start",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_RUPEES_COLLECTED,	    { STAT_TYPE_PLAYER, "Collected - Rupees", 	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_RUPEES_SPENT,	        { STAT_TYPE_PLAYER, "Consumed - Rupees",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_CHESTS_OPENED,	        { STAT_TYPE_PLAYER, "Chests Opened",   		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_DAMAGE_TAKEN,	        { STAT_TYPE_PLAYER, "Damage Taken",   		        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ROLLS,	                { STAT_TYPE_PLAYER, "Action - Rolls",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BONKS,	                { STAT_TYPE_PLAYER, "Action - Bonks",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_PAUSES,	                { STAT_TYPE_PLAYER, "Action - Pauses",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_STEPS,	                { STAT_TYPE_PLAYER, "Action - Steps Taken",   	    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_POTS_BROKEN,	        { STAT_TYPE_PLAYER, "Pots Shattered",   	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BUSHES_CUT,	            { STAT_TYPE_PLAYER, "Lawns Mowed",   	            UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_SWORD_SWINGS,	        { STAT_TYPE_PLAYER, "Action - Sword Swings",   	    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_SIDEHOPS,	            { STAT_TYPE_PLAYER, "Action - Sidehops",   	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BACKFLIPS,	            { STAT_TYPE_PLAYER, "Action - Backflips",   	    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_TIME_BUNNY_HOOD,	    { STAT_TYPE_PLAYER, "Bunny Hood Time",   	        UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_PIECE_OF_HEART,	        { STAT_TYPE_PLAYER, "Collected - Piece of Heart",   UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_HEART_CONTAINER,	    { STAT_TYPE_PLAYER, "Collected - Heart Container",  UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_ICE_TRAPS,	            { STAT_TYPE_PLAYER, "Trap - Ice Traps",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BURN_TRAPS,	            { STAT_TYPE_PLAYER, "Trap - Burn Traps",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_SHOCK_TRAPS,	        { STAT_TYPE_PLAYER, "Trap - Shock Traps",   		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_KNOCK_TRAPS,	        { STAT_TYPE_PLAYER, "Trap - Knockback Traps",   	UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_SPEED_TRAPS,	        { STAT_TYPE_PLAYER, "Trap - Speed Traps",   		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_BOMB_TRAPS,	            { STAT_TYPE_PLAYER, "Trap - Bomb Traps",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_VOID_TRAPS,	            { STAT_TYPE_PLAYER, "Trap - Void Traps",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_AMMO_TRAPS,	            { STAT_TYPE_PLAYER, "Trap - Ammo Traps",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_KILL_TRAPS,	            { STAT_TYPE_PLAYER, "Trap - Kill Traps",   		    UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
            { COUNT_TELEPORT_TRAPS,	        { STAT_TYPE_PLAYER, "Trap - Teleport Traps",   		UIWidgets::ColorValues.at(UIWidgets::Colors::White) } },
        }
    },
};

static std::unordered_map<u16, u16> enemyIdToStatCount = {
    { ACTOR_EN_ANUBICE,     COUNT_ENEMIES_DEFEATED_ANUBIS },
    { ACTOR_EN_AM,          COUNT_ENEMIES_DEFEATED_ARMOS },
    { ACTOR_EN_CLEAR_TAG,   COUNT_ENEMIES_DEFEATED_ARWING },
    { ACTOR_EN_VALI,        COUNT_ENEMIES_DEFEATED_BARI },
    { ACTOR_EN_VM,          COUNT_ENEMIES_DEFEATED_BEAMOS },
    { ACTOR_EN_BIGOKUTA,    COUNT_ENEMIES_DEFEATED_BIG_OCTO },
    { ACTOR_EN_BILI,        COUNT_ENEMIES_DEFEATED_BIRI },
    { ACTOR_EN_DNS,         COUNT_ENEMIES_DEFEATED_BUSINESS_SCRUB },
    { ACTOR_EN_TORCH,       COUNT_ENEMIES_DEFEATED_DARK_LINK },
    { ACTOR_EN_DH,          COUNT_ENEMIES_DEFEATED_DEAD_HAND },
    { ACTOR_EN_HINTNUTS,    COUNT_ENEMIES_DEFEATED_DEKU_SCRUB },
    { ACTOR_EN_DODONGO,     COUNT_ENEMIES_DEFEATED_DODONGO },
    { ACTOR_EN_DODOJR,      COUNT_ENEMIES_DEFEATED_DODONGO_BABY },
    { ACTOR_DOOR_KILLER,    COUNT_ENEMIES_DEFEATED_DOOR_TRAP },
    { ACTOR_EN_FD,          COUNT_ENEMIES_DEFEATED_FLARE_DANCER },
    { ACTOR_EN_FLOORMAS,    COUNT_ENEMIES_DEFEATED_FLOORMASTER },
    { ACTOR_EN_TUBO_TRAP,   COUNT_ENEMIES_DEFEATED_FLYING_POT },
    { ACTOR_EN_YUKABYUN,    COUNT_ENEMIES_DEFEATED_FLOOR_TILE },
    { ACTOR_EN_FZ,          COUNT_ENEMIES_DEFEATED_FREEZARD },
    { ACTOR_EN_GELDB,       COUNT_ENEMIES_DEFEATED_GERUDO_THIEF },
    { ACTOR_EN_GOMA,        COUNT_ENEMIES_DEFEATED_GOHMA_LARVA },
    { ACTOR_EN_CROW,        COUNT_ENEMIES_DEFEATED_GUAY },
    { ACTOR_EN_RR,          COUNT_ENEMIES_DEFEATED_LIKE_LIKE },
    { ACTOR_EN_DEKUNUTS,    COUNT_ENEMIES_DEFEATED_MAD_SCRUB },
    { ACTOR_EN_OKUTA,       COUNT_ENEMIES_DEFEATED_OCTOROK },
    { ACTOR_EN_BA,          COUNT_ENEMIES_DEFEATED_PARASITIC_TENTACLE },
    { ACTOR_EN_PO_SISTERS,  COUNT_ENEMIES_DEFEATED_POE_SISTERS },
    { ACTOR_EN_BUBBLE,      COUNT_ENEMIES_DEFEATED_SHABOM },
    { ACTOR_EN_SB,          COUNT_ENEMIES_DEFEATED_SHELLBLADE },
    { ACTOR_EN_SKJ,         COUNT_ENEMIES_DEFEATED_SKULL_KID },
    { ACTOR_EN_NY,          COUNT_ENEMIES_DEFEATED_SPIKE },
    { ACTOR_EN_SKB,         COUNT_ENEMIES_DEFEATED_STALCHILD },
    { ACTOR_EN_TEST,        COUNT_ENEMIES_DEFEATED_STALFOS },
    { ACTOR_EN_WEIYER,      COUNT_ENEMIES_DEFEATED_STINGER },
    { ACTOR_EN_BW,          COUNT_ENEMIES_DEFEATED_TORCH_SLUG },
    { ACTOR_EN_WALLMAS,     COUNT_ENEMIES_DEFEATED_WALLMASTER },
    { ACTOR_EN_KAREBABA,    COUNT_ENEMIES_DEFEATED_WITHERED_DEKU_BABA },
};
// clang-format on

std::string formatIntGameplayStat(uint32_t value) {
    return fmt::format("{}", value);
}

std::string formatHexGameplayStat(uint32_t value) {
    return fmt::format("{:#x} ({:d})", value, value);
}

std::string formatHexOnlyGameplayStat(uint32_t value) {
    return fmt::format("{:#x}", value, value);
}

nlohmann::json GameplayStats_ObjectToJson(const GameplayStatObject& entry) {
    return nlohmann::json{
        { "entryType", entry.entryType },
        { "entryName", entry.entryName },
        { "entryTimestamp", entry.entryTimestamp },
    };
}

std::string GameplayStats_ResolveSceneByName(std::string sceneName) {
    if (sceneName == "Grotto") {
        switch (gPlayState->roomCtx.curRoom.num) {
            case 0:
                return "Generic Grotto";
            case 1:
                return "Lake Hylia Scrub Grotto";
            case 2:
                return "Redead Grotto";
            case 3:
                return "Cow Grotto";
            case 4:
                return "Scrub Trio";
            case 5:
                return "Flooded Grotto";
            case 6:
                return "Scrub Duo (Upgrade)";
            case 7:
                return "Wolfos Grotto";
            case 8:
                return "Hyrule Castle Storms Grotto";
            case 9:
                return "Scrub Duo";
            case 10:
                return "Tektite Grotto";
            case 11:
                return "Forest Stage";
            case 12:
                return "Webbed Grotto";
            case 13:
                return "Big Skulltula Grotto";
        };
    } else if (sceneName == "Dampe's Grave") {
        // Only the last room of Dampe's Grave (rm 6) is considered the windmill.
        return gPlayState->roomCtx.curRoom.num == 6 ? "Windmill" : "Dampe's Grave";
    }
    return sceneName;
}

GameplayStatObject GameplayStats_JsonToObject(const nlohmann::json& jsonEntry) {
    GameplayStatObject entry;

    entry.entryType = jsonEntry["entryType"];
    entry.entryName = jsonEntry["entryName"];
    entry.entryTimestamp = jsonEntry["entryTimestamp"];

    if (entry.entryType < STAT_TYPE_ENEMY) {
        for (auto& list : gameplayStatList.at(entry.entryType)) {
            auto check = list;
            if (list.second.entryName == entry.entryName) {
                entry.entryColor = list.second.entryColor;
                break;
            }
        }
    } else {
        for (auto& list : gameplayCountList.at(entry.entryType)) {
            auto check = list;
            if (list.second.entryName == entry.entryName) {
                entry.entryColor = list.second.entryColor;
                break;
            }
        }
    }

    return entry;
}

GameplayStatObject GameplayStats_GetObject(uint32_t entryId, uint32_t entryType) {
    if (isRandoItem && entryType == STAT_TYPE_ITEM) {
        GameplayStatObject randoObject;

        randoObject.entryType = STAT_TYPE_ITEM;
        randoObject.entryName = Rando::StaticData::GetItemTable().at(entryId).GetName().GetEnglish();
        randoObject.entryColor = UIWidgets::ColorValues.at(UIWidgets::Colors::White);

        return randoObject;
    }

    auto outerIt = gameplayStatList.find(entryType);
    if (outerIt == gameplayStatList.end()) {
        return {};
    }

    auto& innerMap = outerIt->second;
    auto innerIt = innerMap.find(entryId);
    if (innerIt == innerMap.end()) {
        return {};
    }

    return innerIt->second;
}

GameplayStatObject GameplayStats_GetCountObjectById(uint32_t countId, uint32_t countType) {
    auto outerIt = gameplayCountList.find(countType);
    if (outerIt == gameplayCountList.end()) {
        return {};
    }

    if (countType == STAT_TYPE_ENEMY) {
        if (enemyIdToStatCount.contains(countId)) {
            countId = enemyIdToStatCount.at(countId);
        } else {
            switch (countId) {

                default:
                    break;
            }
        }
    }

    auto& innerMap = outerIt->second;
    auto innerIt = innerMap.find(countId);
    if (innerIt == innerMap.end()) {
        return {};
    }

    return innerIt->second;
}

void GameplayStats_SortTimestamps(bool shouldSort) {
    if (!shouldSort) {
        std::sort(currentTimestamps.begin(), currentTimestamps.end(),
                  [](const GameplayStatObject& a, const GameplayStatObject& b) {
                      return a.entryTimestamp < b.entryTimestamp;
                  });
    } else {
        std::sort(currentTimestamps.begin(), currentTimestamps.end(),
                  [](const GameplayStatObject& a, const GameplayStatObject& b) {
                      return a.entryTimestamp > b.entryTimestamp;
                  });
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
        for (auto& timestamp : currentTimestamps) {
            listArray.push_back(GameplayStats_ObjectToJson(timestamp));
        }
        saveFile[std::to_string(gSaveContext.fileNum + 1)]["Timestamps"] = listArray;
        listArray.clear();

        for (auto& counts : currentCounts) {
            listArray.push_back(GameplayStats_ObjectToJson(counts));
        }
        saveFile[std::to_string(gSaveContext.fileNum + 1)]["Counts"] = listArray;
        listArray.clear();

        for (auto& scenes : currentSceneTimers) {
            listArray.push_back(GameplayStats_ObjectToJson(scenes));
        }
        saveFile[std::to_string(gSaveContext.fileNum + 1)]["Scenes"] = listArray;
    }

    if (action == STAT_ACTION_LOAD) {
        currentTimestamps.clear();
        currentCounts.clear();
        currentSceneTimers.clear();
        if (saveFile.contains(std::to_string(fileNum + 1))) {
            for (auto& load : saveFile[std::to_string(fileNum + 1)]["Timestamps"]) {
                currentTimestamps.push_back(GameplayStats_JsonToObject(load));
            }
            for (auto& load : saveFile[std::to_string(fileNum + 1)]["Counts"]) {
                currentCounts.push_back(GameplayStats_JsonToObject(load));
            }
            for (auto& load : saveFile[std::to_string(fileNum + 1)]["Scenes"]) {
                currentSceneTimers.push_back(GameplayStats_JsonToObject(load));
            }
        }
    }

    if (action == STAT_ACTION_DELETE) {
        currentTimestamps.clear();
        currentCounts.clear();
        currentSceneTimers.clear();
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

void GameplayStats_UpdateSceneTimer(GameplayStatObject sceneObject) {
    if (sceneObject.entryName == "") {
        return;
    }

    for (auto& scene : currentSceneTimers) {
        if (scene.entryName == sceneObject.entryName) {
            scene.entryTimestamp++;
            return;
        }
    }

    sceneObject.entryTimestamp = 1;
    currentSceneTimers.push_back(sceneObject);
}

void GameplayStats_AddCount(GameplayStatObject countObject) {
    if (countObject.entryName == "") {
        return;
    }

    if (countObject.entryTimestamp == 0) {
        countObject.entryTimestamp = 1;
    }

    for (auto& count : currentCounts) {
        if (count.entryName == countObject.entryName) {
            count.entryTimestamp += countObject.entryTimestamp;
            return;
        }
    }

    currentCounts.push_back(countObject);

    std::sort(currentCounts.begin(), currentCounts.end(),
              [](const GameplayStatObject& a, const GameplayStatObject& b) { return a.entryName < b.entryName; });
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
            timestampId = TIMESTAMP_DEFEAT_GANON;
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
        GameplayStats_AddTimestamp(GameplayStats_GetObject(timestampId, STAT_TYPE_EVENT));
    }
}

void GameplayStats_AddTimestamp(GameplayStatObject statObject) {
    bool fileInit = false;

    if (IS_RANDO && !gPlayState) {
        fileInit = true;
        statObject.entryTimestamp = 0;
    } else {
        statObject.entryTimestamp = GAMEPLAYSTAT_TOTAL_TIME;
    }

    if (statObject.entryType == STAT_TYPE_SCENE) {
        statObject.entryName = GameplayStats_ResolveSceneByName(statObject.entryName);
    }

    if (statObject.entryType != STAT_TYPE_ITEM) {
        auto it = std::find_if(currentTimestamps.begin(), currentTimestamps.end(), [&](const GameplayStatObject& obj) {
            return std::strcmp(obj.entryName.c_str(), statObject.entryName.c_str()) == 0;
        });

        if (it != currentTimestamps.end()) {
            return;
        }
    }

    currentTimestamps.push_back(statObject);

    if (fileInit) {
        GameplayStats_SaveFileActions(STAT_ACTION_SAVE, gSaveContext.fileNum + 1);
        fileInit = false;
    }
}

void DrawGameplayStatsOptionsTab() {
    if (ImGui::BeginTable("Options", 2)) {
        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Enable Gameplay Stats Tracking", CVAR_SETTING("Gameplaystats.Enable"),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR));

        ImGui::TableNextColumn();
        if (UIWidgets::CVarCheckbox("Latest Timestamps on Top", CVAR_GAMEPLAY_STATS("ReverseTimestamps"),
                                    UIWidgets::CheckboxOptions().Color(THEME_COLOR))) {
            GameplayStats_SortTimestamps(CVarGetInteger(CVAR_GAMEPLAY_STATS("ReverseTimestamps"), 0));
        }

        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("RTA Timing on New Files", CVAR_GAMEPLAY_STATS("RTATiming"),
                                UIWidgets::CheckboxOptions()
                                    .Tooltip("Timestamps are relative to starting timestamp rather than in game time, "
                                             "usually necessary for races/speedruns.\n\n"
                                             "Starting timestamp is on first non-C-up input after intro cutscene.\n\n"
                                             "NOTE: THIS NEEDS TO BE SET BEFORE CREATING A FILE TO TAKE EFFECT")
                                    .Color(THEME_COLOR));

        ImGui::EndTable();
    }
}

void GameplayStats_DrawSceneTimers() {
    if (ImGui::BeginTable("Scenes", 2)) {
        ImGui::TableSetupColumn("Scene Name");
        ImGui::TableSetupColumn("Time Spent");
        ImGui::TableHeadersRow();

        for (auto& entry : currentSceneTimers) {
            if (entry.entryColor == emptyColor) {
                entry.entryColor = UIWidgets::ColorValues.at(UIWidgets::Colors::White);
            }

            ImGui::TableNextColumn();
            ImGui::TextColored(entry.entryColor, entry.entryName.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(entry.entryColor, formatTimeDisplay(entry.entryTimestamp / 2).c_str());
        }

        ImGui::EndTable();
    }
}

void GameplayStats_DrawCounts(uint32_t typeIndex) {
    if (ImGui::BeginTable("Counts", 2)) {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        for (auto& entry : currentCounts) {
            if (entry.entryType != typeIndex) {
                continue;
            }

            if (entry.entryColor == emptyColor) {
                entry.entryColor = UIWidgets::ColorValues.at(UIWidgets::Colors::White);
            }

            ImGui::TableNextColumn();
            ImGui::TextColored(entry.entryColor, entry.entryName.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(entry.entryColor, entry.entryName == "Bunny Hood Time"
                                                     ? formatTimeDisplay(entry.entryTimestamp / 2).c_str()
                                                     : std::to_string(entry.entryTimestamp).c_str());
        }

        ImGui::EndTable();
    }
}

void GameplayStats_DrawTimeStamps() {
    float cursorY = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeight() * 0.5f);
    ImGui::Text("Filter by Type: ");
    ImGui::SameLine();
    ImGui::SetCursorPosY(cursorY);
    UIWidgets::Combobox("##TypeFilter", &typeIndex, statTypeNameMap,
                        UIWidgets::ComboboxOptions()
                            .Color(THEME_COLOR)
                            .LabelPosition(UIWidgets::LabelPositions::None)
                            .ComponentAlignment(UIWidgets::ComponentAlignments::Right));

    if (ImGui::BeginTable("Timestamps", 2)) {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Timestamp");
        ImGui::TableHeadersRow();

        for (auto& entry : currentTimestamps) {
            if (typeIndex != STAT_TYPE_ALL) {
                if (entry.entryType != typeIndex) {
                    continue;
                }
            }

            if (entry.entryColor == emptyColor) {
                entry.entryColor = UIWidgets::ColorValues.at(UIWidgets::Colors::White);
            }

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

    if (!gPlayState) {
        ImGui::Text("Load into a File first");
    } else {
        UIWidgets::PushStyleTabs(THEME_COLOR);
        if (ImGui::BeginTabBar("Gameplay Stats")) {
            if (ImGui::BeginTabItem("Timestamps")) {
                if (ImGui::BeginChild("Timestamps Window")) {
                    GameplayStats_DrawTimeStamps();
                    ImGui::EndChild();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Counts")) {
                if (ImGui::BeginChild("Counts Window")) {
                    if (ImGui::BeginTable("Counts Table", 2)) {
                        ImGui::TableSetupColumn("Item Counts");
                        ImGui::TableSetupColumn("Action Counts");

                        ImGui::TableNextColumn();
                        ImGui::SeparatorText("Item Counts");
                        GameplayStats_DrawCounts(STAT_TYPE_COLLECT);

                        ImGui::TableNextColumn();
                        ImGui::SeparatorText("Action Counts");
                        GameplayStats_DrawCounts(STAT_TYPE_PLAYER);

                        ImGui::EndTable();
                    }
                    ImGui::EndChild();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Enemy Kills")) {
                if (ImGui::BeginChild("Enemy Kills Window")) {
                    GameplayStats_DrawCounts(STAT_TYPE_ENEMY);
                    ImGui::EndChild();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Scene Timers")) {
                if (ImGui::BeginChild("Scene Timers Window")) {
                    GameplayStats_DrawSceneTimers();
                    ImGui::EndChild();
                }
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
        UIWidgets::PopStyleTabs();
    }
}

void InitStats(bool isDebug) {
    SohUtils::CopyStringToCharArray(gSaveContext.ship.stats.buildVersion, std::string((char*)gBuildVersion),
                                    ARRAY_COUNT(gSaveContext.ship.stats.buildVersion));
    gSaveContext.ship.stats.buildVersionMajor = gBuildVersionMajor;
    gSaveContext.ship.stats.buildVersionMinor = gBuildVersionMinor;
    gSaveContext.ship.stats.buildVersionPatch = gBuildVersionPatch;

    gSaveContext.ship.stats.rtaTiming = CVarGetInteger(CVAR_GAMEPLAY_STATS("RTATiming"), 0);
    gSaveContext.ship.stats.fileCreatedAt = 0;
    gSaveContext.ship.stats.playTimer = 0;
    gSaveContext.ship.stats.pauseTimer = 0;
}

void SaveStats(SaveContext* saveContext, int sectionID, bool fullSave) {
    SaveManager::Instance->SaveData("buildVersion", saveContext->ship.stats.buildVersion);
    SaveManager::Instance->SaveData("buildVersionMajor", saveContext->ship.stats.buildVersionMajor);
    SaveManager::Instance->SaveData("buildVersionMinor", saveContext->ship.stats.buildVersionMinor);
    SaveManager::Instance->SaveData("buildVersionPatch", saveContext->ship.stats.buildVersionPatch);

    SaveManager::Instance->SaveData("heartPieces", saveContext->ship.stats.heartPieces);
    SaveManager::Instance->SaveData("heartContainers", saveContext->ship.stats.heartContainers);
    SaveManager::Instance->SaveArray("dungeonKeys", ARRAY_COUNT(saveContext->ship.stats.dungeonKeys), [&](size_t i) {
        SaveManager::Instance->SaveData("", saveContext->ship.stats.dungeonKeys[i]);
    });
    SaveManager::Instance->SaveData("rtaTiming", saveContext->ship.stats.rtaTiming);
    SaveManager::Instance->SaveData("fileCreatedAt", saveContext->ship.stats.fileCreatedAt);
    SaveManager::Instance->SaveData("playTimer", saveContext->ship.stats.playTimer);
    SaveManager::Instance->SaveData("pauseTimer", saveContext->ship.stats.pauseTimer);
    // SaveManager::Instance->SaveArray(
    //     "itemTimestamps", ARRAY_COUNT(saveContext->ship.stats.itemTimestamp),
    //     [&](size_t i) { SaveManager::Instance->SaveData("", saveContext->ship.stats.itemTimestamp[i]); });
    // SaveManager::Instance->SaveArray(
    //     "sceneTimestamps", ARRAY_COUNT(saveContext->ship.stats.sceneTimestamps), [&](size_t i) {
    //         if (saveContext->ship.stats.sceneTimestamps[i].scene != 254 &&
    //             saveContext->ship.stats.sceneTimestamps[i].room != 254) {
    //             SaveManager::Instance->SaveStruct("", [&]() {
    //                 SaveManager::Instance->SaveData("scene", saveContext->ship.stats.sceneTimestamps[i].scene);
    //                 SaveManager::Instance->SaveData("room", saveContext->ship.stats.sceneTimestamps[i].room);
    //                 SaveManager::Instance->SaveData("sceneTime",
    //                 saveContext->ship.stats.sceneTimestamps[i].sceneTime);
    //                 SaveManager::Instance->SaveData("roomTime", saveContext->ship.stats.sceneTimestamps[i].roomTime);
    //                 SaveManager::Instance->SaveData("isRoom", saveContext->ship.stats.sceneTimestamps[i].isRoom);
    //             });
    //         }
    //     });
    SaveManager::Instance->SaveData("tsIdx", saveContext->ship.stats.tsIdx);
    SaveManager::Instance->SaveArray("counts", ARRAY_COUNT(saveContext->ship.stats.count), [&](size_t i) {
        SaveManager::Instance->SaveData("", saveContext->ship.stats.count[i]);
    });
    SaveManager::Instance->SaveArray(
        "scenesDiscovered", ARRAY_COUNT(saveContext->ship.stats.scenesDiscovered),
        [&](size_t i) { SaveManager::Instance->SaveData("", saveContext->ship.stats.scenesDiscovered[i]); });
    SaveManager::Instance->SaveArray(
        "entrancesDiscovered", ARRAY_COUNT(saveContext->ship.stats.entrancesDiscovered),
        [&](size_t i) { SaveManager::Instance->SaveData("", saveContext->ship.stats.entrancesDiscovered[i]); });
}

void LoadStatsVersion1() {
    SaveManager::Instance->LoadCharArray("buildVersion", gSaveContext.ship.stats.buildVersion,
                                         ARRAY_COUNT(gSaveContext.ship.stats.buildVersion));
    SaveManager::Instance->LoadData("buildVersionMajor", gSaveContext.ship.stats.buildVersionMajor);
    SaveManager::Instance->LoadData("buildVersionMinor", gSaveContext.ship.stats.buildVersionMinor);
    SaveManager::Instance->LoadData("buildVersionPatch", gSaveContext.ship.stats.buildVersionPatch);

    SaveManager::Instance->LoadData("heartPieces", gSaveContext.ship.stats.heartPieces);
    SaveManager::Instance->LoadData("heartContainers", gSaveContext.ship.stats.heartContainers);
    SaveManager::Instance->LoadArray("dungeonKeys", ARRAY_COUNT(gSaveContext.ship.stats.dungeonKeys), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.ship.stats.dungeonKeys[i]);
    });
    SaveManager::Instance->LoadData("rtaTiming", gSaveContext.ship.stats.rtaTiming);
    SaveManager::Instance->LoadData("fileCreatedAt", gSaveContext.ship.stats.fileCreatedAt);
    SaveManager::Instance->LoadData("playTimer", gSaveContext.ship.stats.playTimer);
    SaveManager::Instance->LoadData("pauseTimer", gSaveContext.ship.stats.pauseTimer);
    // SaveManager::Instance->LoadArray(
    //     "itemTimestamps", ARRAY_COUNT(gSaveContext.ship.stats.itemTimestamp),
    //     [](size_t i) { SaveManager::Instance->LoadData("", gSaveContext.ship.stats.itemTimestamp[i]); });
    // SaveManager::Instance->LoadArray(
    //     "sceneTimestamps", ARRAY_COUNT(gSaveContext.ship.stats.sceneTimestamps), [&](size_t i) {
    //         SaveManager::Instance->LoadStruct("", [&]() {
    //             int scene, room, sceneTime, roomTime, isRoom;
    //             SaveManager::Instance->LoadData("scene", scene);
    //             SaveManager::Instance->LoadData("room", room);
    //             SaveManager::Instance->LoadData("sceneTime", sceneTime);
    //             SaveManager::Instance->LoadData("roomTime", roomTime);
    //             SaveManager::Instance->LoadData("isRoom", isRoom);
    //             if (scene == 0 && room == 0 && sceneTime == 0 && roomTime == 0 && isRoom == 0) {
    //                 return;
    //             }
    //             gSaveContext.ship.stats.sceneTimestamps[i].scene = scene;
    //             gSaveContext.ship.stats.sceneTimestamps[i].room = room;
    //             gSaveContext.ship.stats.sceneTimestamps[i].sceneTime = sceneTime;
    //             gSaveContext.ship.stats.sceneTimestamps[i].roomTime = roomTime;
    //             gSaveContext.ship.stats.sceneTimestamps[i].isRoom = isRoom;
    //         });
    //     });
    SaveManager::Instance->LoadData("tsIdx", gSaveContext.ship.stats.tsIdx);
    SaveManager::Instance->LoadArray("counts", ARRAY_COUNT(gSaveContext.ship.stats.count), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.ship.stats.count[i]);
    });
    SaveManager::Instance->LoadArray(
        "scenesDiscovered", ARRAY_COUNT(gSaveContext.ship.stats.scenesDiscovered),
        [](size_t i) { SaveManager::Instance->LoadData("", gSaveContext.ship.stats.scenesDiscovered[i]); });
    SaveManager::Instance->LoadArray(
        "entrancesDiscovered", ARRAY_COUNT(gSaveContext.ship.stats.entrancesDiscovered),
        [](size_t i) { SaveManager::Instance->LoadData("", gSaveContext.ship.stats.entrancesDiscovered[i]); });
}

void GameplayStatsWindow::InitElement() {
    SaveManager::Instance->AddLoadFunction("sohStats", 1, LoadStatsVersion1);
    // Add main section save, no parent.
    SaveManager::Instance->AddSaveFunction("sohStats", 1, SaveStats, true, SECTION_PARENT_NONE);
    // Add subsections, parent of "sohStats". Not sure how to do this without the redundant references to "SaveStats".
    SaveManager::Instance->AddSaveFunction("entrances", 1, SaveStats, false, SECTION_ID_STATS);
    SaveManager::Instance->AddSaveFunction("scenes", 1, SaveStats, false, SECTION_ID_STATS);
    SaveManager::Instance->AddInitFunction(InitStats);
}

void RegisterGameplayStats() {
    if (!std::filesystem::exists(Ship::Context::GetPathRelativeToAppDirectory("SoHGameplayStats.json"))) {
        json initFile;
        std::ofstream file(Ship::Context::GetPathRelativeToAppDirectory("SoHGameplayStats.json"));
        file << initFile.dump(4);
        file.close();
    }

    COND_HOOK(OnEnemyDefeat, CVAR, [](void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);

        auto countObject = GameplayStats_GetCountObjectById(actor->id, STAT_TYPE_ENEMY);
        if (countObject.entryName == "") {
            switch (actor->id) {
                case ACTOR_EN_BB:
                    if (actor->params == ENBB_GREEN || actor->params == ENBB_GREEN_BIG) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_BUBBLE_GREEN, STAT_TYPE_ENEMY);
                    } else if (actor->params == ENBB_BLUE) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_BUBBLE_BLUE, STAT_TYPE_ENEMY);
                    } else if (actor->params == ENBB_WHITE) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_BUBBLE_WHITE, STAT_TYPE_ENEMY);
                    } else if (actor->params == ENBB_RED) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_BUBBLE_RED, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_DEKUBABA:
                    if (actor->params == DEKUBABA_BIG) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_DEKU_BABA_BIG, STAT_TYPE_ENEMY);
                    } else {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_DEKU_BABA, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_ZF:
                    if (actor->params == ENZF_TYPE_DINOLFOS) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_DINOLFOS, STAT_TYPE_ENEMY);
                    } else {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_LIZALFOS, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_RD:
                    if (actor->params >= -1) {
                        countObject = GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_REDEAD, STAT_TYPE_ENEMY);
                    } else {
                        countObject = GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_GIBDO, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_IK:
                    if (actor->params == 0) {
                        countObject = GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE_NABOORU,
                                                                       STAT_TYPE_ENEMY);
                    } else {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_FIREFLY:
                    if (actor->params == KEESE_NORMAL_FLY || actor->params == KEESE_NORMAL_PERCH) {
                        countObject = GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_KEESE, STAT_TYPE_ENEMY);
                    } else if (actor->params == KEESE_FIRE_FLY || actor->params == KEESE_FIRE_PERCH) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_KEESE_FIRE, STAT_TYPE_ENEMY);
                    } else if (actor->params == KEESE_ICE_FLY) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_KEESE_ICE, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_REEBA: {
                    EnReeba* reeba = (EnReeba*)actor;
                    if (reeba->isBig) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_LEEVER_BIG, STAT_TYPE_ENEMY);
                    } else {
                        countObject = GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_LEEVER, STAT_TYPE_ENEMY);
                    }
                } break;
                case ACTOR_EN_MB:
                    if (actor->params == 0) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_MOBLIN_CLUB, STAT_TYPE_ENEMY);
                    } else {
                        countObject = GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_MOBLIN, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_PEEHAT:
                    if (actor->params == PEAHAT_TYPE_LARVA) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_PEAHAT_LARVA, STAT_TYPE_ENEMY);
                    } else {
                        countObject = GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_PEAHAT, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_POH:
                    if (actor->params == EN_POH_FLAT || actor->params == EN_POH_SHARP) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_POE_COMPOSER, STAT_TYPE_ENEMY);
                    } else {
                        countObject = GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_POE, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_PO_FIELD:
                    if (actor->params == EN_PO_FIELD_BIG) {
                        countObject = GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_POE_BIG, STAT_TYPE_ENEMY);
                    } else {
                        countObject = GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_POE, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_ST:
                    if (actor->params == 1) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_SKULLTULA_BIG, STAT_TYPE_ENEMY);
                    } else {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_SKULLTULA, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_SW:
                    if (((actor->params & 0xE000) >> 0xD) != 0) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_SKULLTULA_GOLD, STAT_TYPE_ENEMY);
                    } else {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_SKULLWALLTULA, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_TP:
                    // Only count the head, otherwise each body segment will increment
                    if (actor->params == TAILPASARAN_HEAD) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_TAILPASARAN, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_TITE:
                    if (actor->params == TEKTITE_BLUE) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_TEKTITE_BLUE, STAT_TYPE_ENEMY);
                    } else {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_TEKTITE_RED, STAT_TYPE_ENEMY);
                    }
                    break;
                case ACTOR_EN_WF:
                    if (actor->params == WOLFOS_WHITE) {
                        countObject =
                            GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_WOLFOS_WHITE, STAT_TYPE_ENEMY);
                    } else {
                        countObject = GameplayStats_GetCountObjectById(COUNT_ENEMIES_DEFEATED_WOLFOS, STAT_TYPE_ENEMY);
                    }
                    break;
                default:
                    return;
            }
        }

        GameplayStats_AddCount(countObject);
    });
    COND_HOOK(OnItemReceive, CVAR, [](GetItemEntry itemEntry) {
        if (itemEntry.modIndex == MOD_RANDOMIZER) {
            if (itemEntry.itemId == RG_ICE_TRAP) {
                return;
            }
            isRandoItem = true;
        } else {
            isRandoItem = false;
        }

        auto statObject = GameplayStats_GetObject(itemEntry.itemId, STAT_TYPE_ITEM);
        if (statObject.entryName == "") {
            return;
        }

        if (isRandoItem) {
            if ((itemEntry.itemId >= RG_FOREST_TEMPLE_SMALL_KEY && itemEntry.itemId <= RG_TREASURE_GAME_SMALL_KEY) || itemEntry.itemId == RG_TRIFORCE_PIECE) {
                GameplayStatObject countObject = statObject;
                countObject.entryTimestamp = 1;
                countObject.entryType = STAT_TYPE_COLLECT;
                GameplayStats_AddCount(countObject);
            }
        }

        if (statObject.entryName == "Piece of Heart") {
            GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_PIECE_OF_HEART, STAT_TYPE_PLAYER));
            GameplayStats_AddCount(statObject);
        } else if (statObject.entryName == "Heart Container") {
            GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_HEART_CONTAINER, STAT_TYPE_PLAYER));
        } else {
            GameplayStats_AddTimestamp(statObject);
        }
    });
    COND_HOOK(OnBossDefeat, CVAR, [](void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);
        GameplayStats_GetTimestampByActorId(actor->id);
    });
    COND_HOOK(OnActorKill, CVAR, [](void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);
        if (actor->id == ACTOR_OBJ_TSUBO) {
            GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_POTS_BROKEN, STAT_TYPE_PLAYER));
        }
        if (actor->id == ACTOR_EN_KUSA) {
            GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUSHES_CUT, STAT_TYPE_PLAYER));
        }
    });
    COND_HOOK(OnActorUpdate, CVAR, [](void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);
        if (actor->id == ACTOR_EN_BOX) {
            EnBox* actor = static_cast<EnBox*>(refActor);

            if (actor->actionFunc != EnBox_Open) {
                return;
            }
            if (actor->skelanime.curFrame == 30.0f) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_CHESTS_OPENED, STAT_TYPE_PLAYER));
                return;
            }
        }
        if (actor->id == ACTOR_EN_KUSA) {
            EnKusa* actor = static_cast<EnKusa*>(refActor);
            // GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUSHES_CUT, STAT_TYPE_PLAYER));
        }
    });
    COND_HOOK(OnSceneInit, CVAR, [](int16_t sceneNum) {
        auto statObject = GameplayStats_GetObject((uint32_t)sceneNum, STAT_TYPE_SCENE);
        if (statObject.entryName == "") {
            return;
        }

        GameplayStats_AddTimestamp(statObject);
        GameplayStats_UpdateSceneTimer(statObject);
    });
    COND_HOOK(OnAmmoUsed, CVAR, [](s16 item, s16 ammoUsed) {
        if (item == ITEM_SEEDS) {
            item == ITEM_SLINGSHOT;
        }

        auto countObject = GameplayStats_GetCountObjectById(item, STAT_TYPE_PLAYER);
        if (countObject.entryName == "") {
            return;
        }

        if (countObject.entryName == "Consumed - Rupees" || countObject.entryName == "Collected - Rupees") {
            countObject.entryTimestamp = ammoUsed;
        }

        GameplayStats_AddCount(countObject);
    });
    COND_HOOK(OnPlayerUpdate, CVAR, []() {
        if (!gSaveContext.ship.stats.gameComplete) {
            Player* player = GET_PLAYER(gPlayState);
            Input* input = &gPlayState->state.input[0];

            if (CHECK_BTN_ALL(input[0].press.button, BTN_A)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_A, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_B)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_B, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_CUP)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_CUP, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_CRIGHT)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_CRIGHT, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_CLEFT)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_CLEFT, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_CDOWN)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_CDOWN, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_DUP)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_DUP, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_DRIGHT)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_DRIGHT, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_DDOWN)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_DDOWN, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_DLEFT)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_DLEFT, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_L)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_L, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_R)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_R, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_Z)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_Z, STAT_TYPE_PLAYER));
            }
            if (CHECK_BTN_ALL(input[0].press.button, BTN_START)) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BUTTON_PRESSES_START, STAT_TYPE_PLAYER));
            }

            if (player->actionFunc == Player_Action_Roll && !isRolling) {
                isRolling = true;
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_ROLLS, STAT_TYPE_PLAYER));
            } else if ((player->actionFunc != Player_Action_Roll) && isRolling) {
                isRolling = false;
            }

            if (!gSaveContext.ship.stats.gameComplete && !(player->stateFlags2 & PLAYER_STATE2_IDLE_FIDGET) &&
                !(player->stateFlags2 & PLAYER_STATE2_CRAWLING)) {
                if (player->stateFlags2 & PLAYER_STATE2_FOOTSTEP) {
                    GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_STEPS, STAT_TYPE_PLAYER));
                }
            }

            if (player->heldItemAction >= PLAYER_IA_SWORD_MASTER &&
                player->heldItemAction <= PLAYER_IA_SWORD_BIGGORON && player->meleeWeaponState == -1) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_SWORD_SWINGS, STAT_TYPE_PLAYER));
            }

            if (player->stateFlags2 & PLAYER_STATE2_HOPPING && !isHopping) {
                switch (player->controlStickDirections[player->controlStickDataIndex]) {
                    case 1:
                    case 3:
                        isHopping = true;
                        GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_SIDEHOPS, STAT_TYPE_PLAYER));
                        break;
                    case 2:
                        isHopping = true;
                        GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BACKFLIPS, STAT_TYPE_PLAYER));
                        break;
                    default:
                        break;
                }
            } else if (!(player->stateFlags2 & PLAYER_STATE2_HOPPING) && isHopping) {
                isHopping = false;
            }

            if (player->currentMask == PLAYER_MASK_BUNNY) {
                GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_TIME_BUNNY_HOOD, STAT_TYPE_PLAYER));
            }

            GameplayStats_UpdateSceneTimer(GameplayStats_GetObject(gPlayState->sceneNum, STAT_TYPE_SCENE));
        }
    });
    COND_HOOK(OnIceTrapReceived, CVAR, [](s16 item, s16 trapType) {
        GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_ICE_TRAPS + trapType, STAT_TYPE_PLAYER));
    });
    COND_HOOK(OnPlayerHealthChange, CVAR, [](int16_t amount) {
        auto countObject = GameplayStats_GetCountObjectById(COUNT_DAMAGE_TAKEN, STAT_TYPE_PLAYER);
        countObject.entryTimestamp = amount;

        GameplayStats_AddCount(countObject);
    });
    COND_HOOK(OnPlayerBonk, CVAR,
              []() { GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_BONKS, STAT_TYPE_PLAYER)); });
    COND_HOOK(OnKaleidoUpdate, CVAR, []() {
        if (!gPlayState) {
            return;
        }
        if (gPlayState->pauseCtx.state == 1) {
            GameplayStats_AddCount(GameplayStats_GetCountObjectById(COUNT_PAUSES, STAT_TYPE_PLAYER));
        }
    });
    COND_HOOK(OnDeleteFile, true, [](int32_t fileNum) { GameplayStats_SaveFileActions(STAT_ACTION_DELETE, fileNum); });
    COND_HOOK(OnLoadFile, true, [](int32_t fileNum) { GameplayStats_SaveFileActions(STAT_ACTION_LOAD, fileNum); });
    COND_HOOK(OnSaveFile, true, [](int32_t fileNum) { GameplayStats_SaveFileActions(STAT_ACTION_SAVE, fileNum); });
    COND_HOOK(OnPresentFileSelect, true, []() {
        currentTimestamps.clear();
        currentCounts.clear();
        currentSceneTimers.clear();
    });
}

static RegisterShipInitFunc initFunc(RegisterGameplayStats, { CVAR_NAME });
