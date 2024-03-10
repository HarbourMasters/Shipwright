#include "TimeSplits.h"
#include "soh/UIWidgets.hpp"
#include "soh/Enhancements/randomizer/3drando/item_list.hpp"
#include "soh/Enhancements/gameplaystats.h"
#include "soh/SaveManager.h"
#include "soh/util.h"
#include <vector>
#include "include/z64item.h"

#include "nlohmann-json/include/nlohmann/json.hpp"
#include <fstream>
#include <filesystem>

#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/debugger/debugSaveEditor.h"
#include "soh_assets.h"
#include "assets/textures/parameter_static/parameter_static.h"

extern "C" {
    extern SaveContext gSaveContext;
    extern PlayState* gPlayState;
}

// ImVec4 Colors
#define COLOR_WHITE ImVec4(1.00f, 1.00f, 1.00f, 1.00f)
#define COLOR_LIGHT_RED ImVec4(1.0f, 0.05f, 0.0f, 1.0f)
#define COLOR_RED ImVec4(1.00f, 0.00f, 0.00f, 1.00f)
#define COLOR_LIGHT_GREEN ImVec4(0.52f, 1.0f, 0.23f, 1.0f)
#define COLOR_GREEN ImVec4(0.10f, 1.00f, 0.10f, 1.00f)
#define COLOR_BLUE ImVec4(0.00f, 0.33f, 1.00f, 1.00f)
#define COLOR_PURPLE ImVec4(0.54f, 0.19f, 0.89f, 1.00f)
#define COLOR_YELLOW ImVec4(1.00f, 1.00f, 0.00f, 1.00f)
#define COLOR_ORANGE ImVec4(1.00f, 0.67f, 0.11f, 1.00f)
#define COLOR_LIGHT_BLUE ImVec4(0.00f, 0.88f, 1.00f, 1.00f)
#define COLOR_GREY ImVec4(0.78f, 0.78f, 0.78f, 1.00f)

static std::vector<uint32_t> splitItem;
static std::vector<uint32_t> splitTime;
static std::vector<uint32_t> splitBest;
static std::vector<uint32_t> splitPreviousBest;
static std::vector<uint32_t> splitStatus;
static uint32_t splitTokens;
static std::string status;
static ImVec4 statusColor = COLOR_WHITE;
static uint32_t splitCurNum = 1;
std::string splitAttempt = "Attempt #: 1";
static std::string splitNumDisp = "Attempt #: ";
static ImVec4 colorChoice = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
static const char* backgroundColor;
static uint32_t itemNum;
static std::string itemImager;
static std::string itemNamer;
static uint32_t splitCounter;
static uint32_t totalPreviousBest;
char textBuffer[25];
static ImVec4 pieceTint;
static uint32_t uiScale;

using json = nlohmann::json;

std::vector<TimeSplitObject> splitObjects = {
     //	ID,	                                    Item Name							    Image Name	
    {	ITEM_STICK, 	 		                "Deku Stick", 							"ITEM_STICK"},
    {	ITEM_NUT,				                "Deku Nut", 							"ITEM_NUT"},
    {	ITEM_BOW, 	 			                "Fairy Bow", 							"ITEM_BOW"},
    {	ITEM_ARROW_FIRE, 		                "Fire Arrow", 							"ITEM_ARROW_FIRE"},
    {	ITEM_DINS_FIRE, 		                "Din's Fire", 							"ITEM_DINS_FIRE"},
    {	ITEM_SLINGSHOT, 		                "Fairy Slingshot", 						"ITEM_SLINGSHOT"},
    {	ITEM_OCARINA_FAIRY, 	                "Fairy Ocarina", 						"ITEM_OCARINA_FAIRY"},
    {	ITEM_OCARINA_TIME, 		                "Ocarina of Time", 						"ITEM_OCARINA_TIME"},
    {	ITEM_BOMBCHU, 	 		                "Bombchu", 								"ITEM_BOMBCHU"},
    {	ITEM_HOOKSHOT,  		                "Hookshot", 							"ITEM_HOOKSHOT"},
    {	ITEM_LONGSHOT,  		                "Longshot", 							"ITEM_LONGSHOT"},
    {	ITEM_ARROW_ICE,  		                "Ice Arrow", 							"ITEM_ARROW_ICE"},
    {	ITEM_FARORES_WIND,  	                "Farore's Wind", 						"ITEM_FARORES_WIND"},
    {	ITEM_BOOMERANG,  		                "Boomerang", 							"ITEM_BOOMERANG"},
    {	ITEM_LENS,  			                "Lens of Truth", 						"ITEM_LENS"},
    {	ITEM_BEAN,  			                "Magic Bean", 							"ITEM_BEAN"},
    {	ITEM_HAMMER,  			                "Megaton Hammer", 						"ITEM_HAMMER"},
    {	ITEM_ARROW_LIGHT,  		                "Light Arrow", 							"ITEM_ARROW_LIGHT"},
    {	ITEM_NAYRUS_LOVE,  		                "Nayru's Love", 						"ITEM_NAYRUS_LOVE"},
    {	ITEM_BOTTLE,  			                "Empty Bottle", 						"ITEM_BOTTLE"},
    {	ITEM_POTION_RED,  		                "Red Potion", 							"ITEM_POTION_RED"},
    {	ITEM_POTION_GREEN,  	                "Green Potion", 						"ITEM_POTION_GREEN"},
    {	ITEM_POTION_BLUE,  		                "Blue Potion", 							"ITEM_POTION_BLUE"},
    {	ITEM_FAIRY,  			                "Bottled Fairy", 						"ITEM_FAIRY"},
    {	ITEM_FISH,  			                "Fish", 								"ITEM_FISH"},
    {	ITEM_MILK_BOTTLE,  		                "Milk", 				                "ITEM_MILK_BOTTLE"},
    {	ITEM_LETTER_RUTO,  		                "Ruto's Letter", 						"ITEM_LETTER_RUTO"},
    {	ITEM_BLUE_FIRE,  		                "Blue Fire", 							"ITEM_BLUE_FIRE"},
    {	ITEM_BUG,  				                "Bug", 									"ITEM_BUG"},
    {	ITEM_BIG_POE,  			                "Big Poe", 								"ITEM_BIG_POE"},
    {	ITEM_POE,  				                "Poe", 									"ITEM_POE"},
    {	ITEM_WEIRD_EGG,  		                "Weird Egg", 							"ITEM_WEIRD_EGG"},
    {	ITEM_CHICKEN,  			                "Chicken", 								"ITEM_CHICKEN"},
    {	ITEM_LETTER_ZELDA,  	                "Zelda's Letter", 						"ITEM_LETTER_ZELDA"},
    {	ITEM_MASK_KEATON,  		                "Keaton Mask", 							"ITEM_MASK_KEATON"},
    {	ITEM_MASK_SKULL,  		                "Skull Mask", 							"ITEM_MASK_SKULL"},
    {	ITEM_MASK_SPOOKY,  		                "Spooky Mask", 							"ITEM_MASK_SPOOKY"},
    {	ITEM_MASK_BUNNY,  		                "Bunny Hood", 							"ITEM_MASK_BUNNY"},
    {	ITEM_MASK_GORON,  		                "Goron Mask", 							"ITEM_MASK_GORON"},
    {	ITEM_MASK_ZORA,  		                "Zora Mask", 							"ITEM_MASK_ZORA"},
    {	ITEM_MASK_GERUDO,  		                "Gerudo Mask", 							"ITEM_MASK_GERUDO"},
    {	ITEM_MASK_TRUTH,  		                "Mask of Truth", 						"ITEM_MASK_TRUTH"},
    {	ITEM_POCKET_EGG,  		                "Pocket Egg", 							"ITEM_POCKET_EGG"},
    {	ITEM_POCKET_CUCCO,  	                "Pocket Cucco", 						"ITEM_POCKET_CUCCO"},
    {	ITEM_COJIRO,  			                "Cojiro", 								"ITEM_COJIRO"},
    {	ITEM_ODD_MUSHROOM,  	                "Odd Mushroom", 						"ITEM_ODD_MUSHROOM"},
    {	ITEM_ODD_POTION,  		                "Odd Potion", 							"ITEM_ODD_POTION"},
    {	ITEM_SAW,  				                "Poacher's Saw", 						"ITEM_SAW"},
    {	ITEM_SWORD_BROKEN,  	                "Goron's Sword (Broken)", 				"ITEM_SWORD_BROKEN"},
    {	ITEM_PRESCRIPTION,  	                "Prescription", 						"ITEM_PRESCRIPTION"},
    {	ITEM_FROG,  			                "Eyeball Frog", 						"ITEM_FROG"},
    {	ITEM_EYEDROPS,  		                "Eye Drops", 							"ITEM_EYEDROPS"},
    {	ITEM_CLAIM_CHECK,  		                "Claim Check",  						"ITEM_CLAIM_CHECK"},
    {	ITEM_SWORD_KOKIRI,  	                "Kokiri Sword", 						"ITEM_SWORD_KOKIRI"},
    {	ITEM_SWORD_MASTER,  	                "Master Sword", 						"ITEM_SWORD_MASTER"},
    {	ITEM_SWORD_BGS,  		                "Giant's Knife & Biggoron's Sword", 	"ITEM_SWORD_BGS"},
    {	ITEM_SHIELD_DEKU,  		                "Deku Shield", 							"ITEM_SHIELD_DEKU"},
    {	ITEM_SHIELD_HYLIAN, 	                "Hylian Shield", 						"ITEM_SHIELD_HYLIAN"},
    {	ITEM_SHIELD_MIRROR, 	                "Mirror Shield", 						"ITEM_SHIELD_MIRROR"},
    {	ITEM_TUNIC_GORON,  		                "Goron Tunic", 							"ITEM_TUNIC_GORON"},
    {	ITEM_TUNIC_ZORA,  		                "Zora Tunic", 							"ITEM_TUNIC_ZORA"},
    {	ITEM_BOOTS_IRON,  		                "Iron Boots", 							"ITEM_BOOTS_IRON"},
    {	ITEM_BOOTS_HOVER,  		                "Hover Boots", 							"ITEM_BOOTS_HOVER"},
    {	ITEM_BULLET_BAG_30, 	                "Bullet Bag (30)", 						"ITEM_BULLET_BAG_30"},
    {	ITEM_BULLET_BAG_40, 	                "Bullet Bag (40)", 						"ITEM_BULLET_BAG_40"},
    {	ITEM_BULLET_BAG_50, 	                "Bullet Bag (50)", 						"ITEM_BULLET_BAG_50"},
    {	ITEM_QUIVER_30,  		                "Quiver (30)", 							"ITEM_QUIVER_30"},
    {	ITEM_QUIVER_40,  		                "Big Quiver (40)", 						"ITEM_QUIVER_40"},
    {	ITEM_QUIVER_50,  		                "Biggest Quiver (50)", 					"ITEM_QUIVER_50"},
    {	ITEM_BOMB_BAG_20,  		                "Bomb Bag (20)", 						"ITEM_BOMB_BAG_20"},
    {	ITEM_BOMB_BAG_30,  		                "Big Bomb Bag (30)", 					"ITEM_BOMB_BAG_30"},
    {	ITEM_BOMB_BAG_40,  		                "Biggest Bomb Bag (40)", 				"ITEM_BOMB_BAG_40"},
    {	ITEM_BRACELET,  		                "Goron's Bracelet", 					"ITEM_BRACELET"},
    {	ITEM_GAUNTLETS_SILVER,                  "Silver Gauntlets", 					"ITEM_GAUNTLETS_SILVER"},
    {	ITEM_GAUNTLETS_GOLD,  	                "Golden Gauntlets", 					"ITEM_GAUNTLETS_GOLD"},
    {	ITEM_SCALE_SILVER,  	                "Silver Scale", 						"ITEM_SCALE_SILVER"},
    {	ITEM_SCALE_GOLDEN,  	                "Golden Scale", 						"ITEM_SCALE_GOLDEN"},
    {	ITEM_SWORD_KNIFE,  		                "Giant's Knife (Broken)", 				"ITEM_SWORD_KNIFE"},
    {	ITEM_WALLET_ADULT,  	                "Adult's Wallet", 						"ITEM_WALLET_ADULT"},
    {	ITEM_WALLET_GIANT,  	                "Giant's Wallet", 						"ITEM_WALLET_GIANT"},
    {	ITEM_FISHING_POLE,  	                "Fishing Pole", 						"ITEM_FISHING_POLE"},
    {	ITEM_SONG_MINUET,  		                "Minuet of Forest",						"QUEST_SONG_MINUET"},
    {	ITEM_SONG_BOLERO,  		                "Bolero of Fire", 						"QUEST_SONG_BOLERO"},
    {	ITEM_SONG_SERENADE,  	                "Serenade of Water", 					"QUEST_SONG_SERENADE"},
    {	ITEM_SONG_REQUIEM,  	                "Requiem of Spirit", 					"QUEST_SONG_REQUIEM"},
    {	ITEM_SONG_NOCTURNE,  	                "Nocturne of Shadow", 					"QUEST_SONG_NOCTURNE"},
    {	ITEM_SONG_PRELUDE,  	                "Prelude of Light", 					"QUEST_SONG_PRELUDE"},
    {	ITEM_SONG_LULLABY,  	                "Zelda's Lullaby", 						"QUEST_SONG_LULLABY"},
    {	ITEM_SONG_EPONA,  		                "Epona's Song", 						"QUEST_SONG_EPONA"},
    {	ITEM_SONG_SARIA,  		                "Saria's Song", 						"QUEST_SONG_SARIA"},
    {	ITEM_SONG_SUN,  		                "Sun's Song", 							"QUEST_SONG_SUN"},
    {	ITEM_SONG_TIME, 		                "Song of Time", 						"QUEST_SONG_TIME"},
    {	ITEM_SONG_STORMS, 		                "Song of Storms", 						"QUEST_SONG_STORMS"},
    {	ITEM_MEDALLION_FOREST, 	                "Forest Medallion", 					"QUEST_MEDALLION_FOREST"},
    {	ITEM_MEDALLION_FIRE, 	                "Fire Medallion", 						"QUEST_MEDALLION_FIRE"},
    {	ITEM_MEDALLION_WATER, 	                "Water Medallion", 						"QUEST_MEDALLION_WATER"},
    {	ITEM_MEDALLION_SPIRIT, 	                "Spirit Medallion", 					"QUEST_MEDALLION_SPIRIT"},
    {	ITEM_MEDALLION_SHADOW, 	                "Shadow Medallion", 					"QUEST_MEDALLION_SHADOW"},
    {	ITEM_MEDALLION_LIGHT, 	                "Light Medallion", 						"QUEST_MEDALLION_LIGHT"},
    {	ITEM_KOKIRI_EMERALD, 	                "Kokiri's Emerald", 					"QUEST_KOKIRI_EMERALD"},
    {	ITEM_GORON_RUBY, 		                "Goron's Ruby", 						"QUEST_GORON_RUBY"},
    {	ITEM_ZORA_SAPPHIRE, 	                "Zora's Sapphire", 						"QUEST_ZORA_SAPPHIRE"},
    {	ITEM_STONE_OF_AGONY, 	                "Stone of Agony", 						"QUEST_STONE_OF_AGONY"},
    {	ITEM_GERUDO_CARD, 		                "Gerudo's Card", 						"QUEST_GERUDO_CARD"},
    {   ITEM_SKULL_TOKEN,                       "Skulltula Token",                      "QUEST_SKULL_TOKEN"},
    {	ITEM_MAGIC_SMALL, 		                "Magic Meter", 							"ITEM_MAGIC_SMALL"},
    {	ITEM_MAGIC_LARGE, 		                "Double Magic", 				        "ITEM_MAGIC_LARGE"},
    {	ITEM_DOUBLE_DEFENSE, 	                "Double Defense", 						"ITEM_HEART_CONTAINER"},
    {	ITEM_STICK_UPGRADE_20, 	                "Deku Stick Upgrade (20)", 				"ITEM_STICK"}, 
    {	ITEM_STICK_UPGRADE_30, 	                "Deku Stick Upgrade (30)", 				"ITEM_STICK"},
    {	ITEM_NUT_UPGRADE_30, 	                "Deku Nut Upgrade (30)", 				"ITEM_NUT"},
    {	ITEM_NUT_UPGRADE_40, 	                "Deku Nut Upgrade (40)", 				"ITEM_NUT"},
    {   BOSS_QUEEN_GOHMA,                       "Queen Gohma",                          "SPECIAL_TRIFORCE_PIECE_WHITE"}, 
    {   BOSS_KING_DODONGO,                      "King Dodongo",                         "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_BARINADE,                          "Barinade",                             "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_PHANTOM_GANON,                     "Phantom Ganon",                        "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_VOLVAGIA,                          "Volvagia",                             "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_MORPHA,                            "Morpha",                               "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_BONGO_BONGO,                       "Bongo Bongo",                          "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_TWINROVA,                          "Twinrova",                             "SPECIAL_TRIFORCE_PIECE_WHITE"},  
    {   BOSS_GANONDORF,                         "Ganondorf",                            "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_GANON,                             "Ganon",                                "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   ENTER_DEKU_TREE,                        "Enter Deku Tree",                      "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_DODONGOS_CAVERN,                  "Enter Dodongos Cavern",                "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_JABU_JABU,                        "Enter Jabu Jabu's Belly",              "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_FOREST_TEMPLE,                    "Enter Forest Temple",                  "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_FIRE_TEMPLE,                      "Enter Fire Temple",                    "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_WATER_TEMPLE,                     "Enter Water Temple",                   "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_SPIRIT_TEMPLE,                    "Enter Spirit Temple",                  "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_SHADOW_TEMPLE,                    "Enter Shadow Temple",                  "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_BOTTOM_OF_THE_WELL,               "Enter Bottom of the Well",             "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_ICE_CAVERN,                       "Enter Ice Cavern",                     "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_GANONS_TOWER,                     "Enter Ganons Tower",                   "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_GERUDO_TRAINING_GROUND,           "Enter Gerudo Training Grounds",        "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_THIEVES_HIDEOUT,                  "Enter Thieves Hideout",                "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_INSIDE_GANONS_CASTLE,             "Enter Ganons Castle",                  "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_GANONS_TOWER_COLLAPSE_INTERIOR,   "Enter Tower Collapse Interior",        "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_INSIDE_GANONS_CASTLE_COLLAPSE,    "Enter Ganons Castle Collapse",         "SPECIAL_SPLIT_ENTRANCE"},
    {   ESCAPE_LOST_WOODS,                      "Lost Woods Escape",                    "SPECIAL_SPLIT_ENTRANCE"},
    {   ESCAPE_KOKIRI_FOREST,                   "Forest Escape",                        "SPECIAL_SPLIT_ENTRANCE"},
};

std::vector<TimeSplitObject> equipmentObjects = {
    {	ITEM_SWORD_KOKIRI,  	"Kokiri Sword", 						"ITEM_SWORD_KOKIRI"},
    {	ITEM_SHIELD_DEKU,  		"Deku Shield", 							"ITEM_SHIELD_DEKU"},
    {	ITEM_BRACELET,  		"Goron's Bracelet", 	                "ITEM_BRACELET"},
    {	ITEM_SCALE_SILVER,  	"Silver Scale", 		                "ITEM_SCALE_SILVER"},
    {	ITEM_SWORD_MASTER,  	"Master Sword", 						"ITEM_SWORD_MASTER"},
    {	ITEM_SHIELD_HYLIAN, 	"Hylian Shield", 						"ITEM_SHIELD_HYLIAN"},
    {	ITEM_TUNIC_GORON,  		"Goron Tunic", 							"ITEM_TUNIC_GORON"},
    {	ITEM_BOOTS_IRON,  		"Iron Boots", 							"ITEM_BOOTS_IRON"},
    {	ITEM_SWORD_BGS,  		"Giant's Knife & Biggoron's Sword", 	"ITEM_SWORD_BGS"},
    {	ITEM_SHIELD_MIRROR, 	"Mirror Shield", 						"ITEM_SHIELD_MIRROR"},
    {	ITEM_TUNIC_ZORA,  		"Zora Tunic", 							"ITEM_TUNIC_ZORA"},
    {	ITEM_BOOTS_HOVER,  		"Hover Boots", 							"ITEM_BOOTS_HOVER"},
};

std::vector<TimeSplitObject> questObjects = {
    {	ITEM_STONE_OF_AGONY, 	"Stone of Agony", 		"QUEST_STONE_OF_AGONY"},
    {	ITEM_MEDALLION_LIGHT, 	"Light Medallion", 		"QUEST_MEDALLION_LIGHT"},
    {	ITEM_SONG_LULLABY,  	"Zelda's Lullaby", 		"QUEST_SONG_LULLABY"},
    {	ITEM_SONG_MINUET,  		"Minuet of Forest",		"QUEST_SONG_MINUET"},
    {	ITEM_GERUDO_CARD, 		"Gerudo's Card", 		"QUEST_GERUDO_CARD"},
    {	ITEM_MEDALLION_FOREST, 	"Forest Medallion", 	"QUEST_MEDALLION_FOREST"},
    {	ITEM_SONG_EPONA,  		"Epona's Song", 		"QUEST_SONG_EPONA"},
    {	ITEM_SONG_BOLERO,  		"Bolero of Fire", 		"QUEST_SONG_BOLERO"},
    {   ITEM_SKULL_TOKEN,       "Skulltula Token",      "QUEST_SKULL_TOKEN"},
    {	ITEM_MEDALLION_FIRE, 	"Fire Medallion", 		"QUEST_MEDALLION_FIRE"},
    {	ITEM_SONG_SARIA,  		"Saria's Song", 		"QUEST_SONG_SARIA"},
    {	ITEM_SONG_SERENADE,  	"Serenade of Water", 	"QUEST_SONG_SERENADE"},
    {	ITEM_KOKIRI_EMERALD, 	"Kokiri's Emerald", 	"QUEST_KOKIRI_EMERALD"},
    {	ITEM_MEDALLION_WATER, 	"Water Medallion", 		"QUEST_MEDALLION_WATER"},
    {	ITEM_SONG_SUN,  		"Sun's Song", 			"QUEST_SONG_SUN"},
    {	ITEM_SONG_REQUIEM,  	"Requiem of Spirit", 	"QUEST_SONG_REQUIEM"},
    {	ITEM_GORON_RUBY, 		"Goron's Ruby", 		"QUEST_GORON_RUBY"},
    {	ITEM_MEDALLION_SPIRIT, 	"Spirit Medallion", 	"QUEST_MEDALLION_SPIRIT"},
    {	ITEM_SONG_TIME, 		"Song of Time", 		"QUEST_SONG_TIME"},
    {	ITEM_SONG_NOCTURNE,  	"Nocturne of Shadow", 	"QUEST_SONG_NOCTURNE"},
    {	ITEM_ZORA_SAPPHIRE, 	"Zora's Sapphire", 		"QUEST_ZORA_SAPPHIRE"},
    {	ITEM_MEDALLION_SHADOW, 	"Shadow Medallion", 	"QUEST_MEDALLION_SHADOW"},
    {	ITEM_SONG_STORMS, 		"Song of Storms", 		"QUEST_SONG_STORMS"},
    {	ITEM_SONG_PRELUDE,  	"Prelude of Light", 	"QUEST_SONG_PRELUDE"},
};

std::vector<TimeSplitObject> inventoryObjects = {
    {	ITEM_STICK, 	 		"Deku Stick", 		"ITEM_STICK"},
    {	ITEM_SLINGSHOT, 		"Fairy Slingshot", 	"ITEM_SLINGSHOT"},
    {	ITEM_BOOMERANG,  		"Boomerang", 		"ITEM_BOOMERANG"},
    {	ITEM_BOTTLE,  			"Empty Bottle", 	"ITEM_BOTTLE"},
    {	ITEM_NUT,				"Deku Nut", 		"ITEM_NUT"},
    {	ITEM_OCARINA_FAIRY, 	"Fairy Ocarina", 	"ITEM_OCARINA_FAIRY"},
    {	ITEM_LENS,  			"Lens of Truth", 	"ITEM_LENS"},
    {	ITEM_DOUBLE_DEFENSE, 	"Double Defense", 	"ITEM_HEART_CONTAINER"},
    {	ITEM_BOMB,  		    "Bombs", 			"ITEM_BOMB"},
    {	ITEM_BOMBCHU, 	 		"Bombchu", 			"ITEM_BOMBCHU"},
    {	ITEM_BEAN,  			"Magic Bean", 		"ITEM_BEAN"},
    {	ITEM_MAGIC_SMALL, 		"Magic Meter", 		"ITEM_MAGIC_SMALL"},
    {	ITEM_BOW, 	 			"Fairy Bow", 		"ITEM_BOW"},
    {	ITEM_HOOKSHOT,  		"Hookshot", 		"ITEM_HOOKSHOT"},
    {	ITEM_HAMMER,  			"Megaton Hammer", 	"ITEM_HAMMER"},
    {	ITEM_WALLET_ADULT,  	"Adult's Wallet", 	"ITEM_WALLET_ADULT"},
    {	ITEM_ARROW_FIRE, 		"Fire Arrow", 		"ITEM_ARROW_FIRE"},
    {	ITEM_ARROW_ICE,  		"Ice Arrow", 		"ITEM_ARROW_ICE"},
    {	ITEM_ARROW_LIGHT,  		"Light Arrow", 		"ITEM_ARROW_LIGHT"},
    {	ITEM_POCKET_EGG,  		"Pocket Egg", 		"ITEM_POCKET_EGG"},
    {	ITEM_DINS_FIRE, 		"Din's Fire", 		"ITEM_DINS_FIRE"},
    {	ITEM_FARORES_WIND,  	"Farore's Wind", 	"ITEM_FARORES_WIND"},
    {	ITEM_NAYRUS_LOVE,  		"Nayru's Love", 	"ITEM_NAYRUS_LOVE"},
    {	ITEM_WEIRD_EGG,  		"Weird Egg", 		"ITEM_WEIRD_EGG"},
};

std::vector<TimeSplitObject> bossObjects = {
    {   BOSS_QUEEN_GOHMA,       "Queen Gohma",      "SPECIAL_TRIFORCE_PIECE_WHITE"}, 
    {   BOSS_KING_DODONGO,      "King Dodongo",     "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_BARINADE,          "Barinade",         "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_PHANTOM_GANON,     "Phantom Ganon",    "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_VOLVAGIA,          "Volvagia",         "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_MORPHA,            "Morpha",           "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_BONGO_BONGO,       "Bongo Bongo",      "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_TWINROVA,          "Twinrova",         "SPECIAL_TRIFORCE_PIECE_WHITE"},  
    {   BOSS_GANONDORF,         "Ganondorf",        "SPECIAL_TRIFORCE_PIECE_WHITE"},
    {   BOSS_GANON,             "Ganon",            "SPECIAL_TRIFORCE_PIECE_WHITE"},
};

std::vector<TimeSplitObject> sceneObjects = {
    {   ENTER_DEKU_TREE,                        "Enter Deku Tree",                  "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_DODONGOS_CAVERN,                  "Enter Dodongos Cavern",            "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_JABU_JABU,                        "Enter Jabu Jabu's Belly",          "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_FOREST_TEMPLE,                    "Enter Forest Temple",              "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_FIRE_TEMPLE,                      "Enter Fire Temple",                "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_WATER_TEMPLE,                     "Enter Water Temple",               "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_SPIRIT_TEMPLE,                    "Enter Spirit Temple",              "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_SHADOW_TEMPLE,                    "Enter Shadow Temple",              "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_BOTTOM_OF_THE_WELL,               "Enter Bottom of the Well",         "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_ICE_CAVERN,                       "Enter Ice Cavern",                 "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_GANONS_TOWER,                     "Enter Ganons Tower",               "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_GERUDO_TRAINING_GROUND,           "Enter Gerudo Training Grounds",    "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_THIEVES_HIDEOUT,                  "Enter Thieves Hideout",            "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_INSIDE_GANONS_CASTLE,             "Enter Ganons Castle",              "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_GANONS_TOWER_COLLAPSE_INTERIOR,   "Enter Tower Collapse Interior",    "SPECIAL_SPLIT_ENTRANCE"},
    {   ENTER_INSIDE_GANONS_CASTLE_COLLAPSE,    "Enter Ganons Castle Collapse",     "SPECIAL_SPLIT_ENTRANCE"},
    {   ESCAPE_LOST_WOODS,                      "Lost Woods Escape",                "SPECIAL_SPLIT_ENTRANCE"},
    {   ESCAPE_KOKIRI_FOREST,                   "Forest Escape",                    "SPECIAL_SPLIT_ENTRANCE"},
};

std::string formatTimestampTimeSplit(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
}

void TimeSplitCompleteSplits() {
    gSaveContext.sohStats.itemTimestamp[TIMESTAMP_DEFEAT_GANON] = GAMEPLAYSTAT_TOTAL_TIME;
    gSaveContext.sohStats.gameComplete = true;
}

void TimeSplitSplitsHandlerS(uint32_t itemID) {
    uint32_t loopCounter = 0;
    for (auto& str : splitItem) {
        if (itemID == splitItem[loopCounter]) {
            if (splitStatus[loopCounter] == 2) {
                splitTime[loopCounter] = GAMEPLAYSTAT_TOTAL_TIME;
                splitStatus[loopCounter] = 1;
                if (loopCounter < (splitItem.size() - 1)) {
                    for (int i = loopCounter; i < splitItem.size(); i++) {
                        if (splitStatus[i] == 0) {
                            splitStatus[i] = 2;
                            i = splitItem.size() - 1;
                        }
                        if (splitStatus[splitItem.size() - 1] == 1) {
                            TimeSplitCompleteSplits();
                        }
                    }
                }
                if (splitTime[loopCounter] < splitBest[loopCounter]) {
                    splitBest[loopCounter] = splitTime[loopCounter];
                }
                if (loopCounter == (splitItem.size() - 1)) {
                    TimeSplitCompleteSplits();
                }
            }
        }
        loopCounter++;
    }
}

void TimeSplitSceneSplitHandler(uint32_t entrance) {
    uint32_t loopCounter = 0;
    if (entrance == ESCAPE_KOKIRI_FOREST && gSaveContext.cutsceneIndex != 0xfff0) {
        return;
    } 
    if (entrance != ESCAPE_LOST_WOODS && entrance != ESCAPE_KOKIRI_FOREST) {
        entrance = gPlayState->sceneNum + ENTER_DEKU_TREE;
    }
    for (auto& str : splitItem) {
        if (entrance == splitItem[loopCounter]) {
            if (splitStatus[loopCounter] == 2) {
                splitTime[loopCounter] = GAMEPLAYSTAT_TOTAL_TIME;
                splitStatus[loopCounter] = 1;
                if (loopCounter < (splitItem.size() - 1)) {
                    for (int i = loopCounter; i < splitItem.size(); i++) {
                        if (splitStatus[i] == 0) {
                            splitStatus[i] = 2;
                            i = splitItem.size() - 1;
                        }
                        if (splitStatus[splitItem.size() - 1] == 1) {
                            TimeSplitCompleteSplits();
                        }
                    }
                }
                if (splitTime[loopCounter] < splitBest[loopCounter]) {
                    splitBest[loopCounter] = splitTime[loopCounter];
                }
                if (loopCounter == (splitItem.size() - 1)) {
                    TimeSplitCompleteSplits();
                }
            }
        }
        loopCounter++;
    } 
}

void TimeSplitSplitsHandler(GetItemEntry itemEntry) {
    if (itemEntry.modIndex != 0) {
        return;
    }
    uint32_t loopCounter = 0;
    for (auto& str : splitItem) {
        if (itemEntry.itemId == ITEM_NUTS_5 || itemEntry.itemId == ITEM_NUTS_10) {
            itemEntry.itemId = ITEM_NUT;
        } else if (itemEntry.itemId == ITEM_STICKS_5 || itemEntry.itemId == ITEM_STICKS_10) {
            itemEntry.itemId == ITEM_STICK;
        } else if (itemEntry.itemId == ITEM_SKULL_TOKEN) {
            if (gSaveContext.inventory.gsTokens != splitTokens) {
                return;
            }
        }
        if (itemEntry.itemId == splitItem[loopCounter]) {
            if (splitStatus[loopCounter] == 2) {
                splitTime[loopCounter] = GAMEPLAYSTAT_TOTAL_TIME;
                splitStatus[loopCounter] = 1;
                if (loopCounter < (splitItem.size() - 1)) {
                    for (int i = loopCounter; i < splitItem.size(); i++) {
                        if (splitStatus[i] == 0) {
                            splitStatus[i] = 2;
                            i = splitItem.size() - 1;
                        }
                        if (splitStatus[splitItem.size() - 1] == 1) {
                            TimeSplitCompleteSplits();
                        }
                    }
                }
                if (splitTime[loopCounter] < splitBest[loopCounter]) {
                    splitBest[loopCounter] = splitTime[loopCounter];
                }
                if (loopCounter == (splitItem.size() - 1)) {
                    TimeSplitCompleteSplits();
                }
            }
        }
        loopCounter++;
    }
}

void DrawTimeSplitOptions() {
    ImGui::Text("List Name");
    ImGui::SameLine(0);
    ImGui::PushItemWidth(165.0f);
    ImGui::InputText("##List Name", textBuffer, sizeof(textBuffer));
    ImGui::PopItemWidth();
    ImGui::SameLine();    
    if (ImGui::Button("Save List")) {
        std::ifstream existingFile("splitdata.json");
        json existingData;
        existingFile >> existingData;
        existingFile.close();

        existingData[textBuffer]["splitItem"] = splitItem;
        existingData[textBuffer]["splitTime"] = splitTime;
        existingData[textBuffer]["splitTokens"] = splitTokens;
        existingData[textBuffer]["splitPreviousBest"] = splitPreviousBest;
        existingData[textBuffer]["backgroundColor.r"] = colorChoice.x;
        existingData[textBuffer]["backgroundColor.g"] = colorChoice.y;
        existingData[textBuffer]["backgroundColor.b"] = colorChoice.z;
        existingData[textBuffer]["backgroundColor.a"] = colorChoice.w;

        std::ofstream newFile("splitdata.json");
        newFile << existingData.dump(4);
        newFile.close();
        status = "List has been saved to disk";
        statusColor = COLOR_LIGHT_BLUE;
    }
    UIWidgets::Spacer(0);
    ImGui::Text("Saved Lists");
    ImGui::SameLine();
    std::ifstream lists("splitdata.json");
    json comboList;
    lists >> comboList;
    std::vector<std::string> keys;
    for (auto& entry : comboList.items()) {
        keys.push_back(entry.key());
    }
    static uint32_t selectedItem = 0;
    static std::string listItem = keys[0];
    ImGui::PushItemWidth(151.0f);
    if (ImGui::BeginCombo("##lists", keys[selectedItem].c_str())) {
        for (int i = 0; i < keys.size(); i++) {
            bool isSelected = (selectedItem == i);
            if (ImGui::Selectable(keys[i].c_str(), isSelected)) {
                selectedItem = i;
                listItem = keys[i].c_str();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Load List")) {
        std::ifstream file("splitdata.json");
        json j;

        if (file.is_open()) {
            file >> j;
            file.close();
        }
        if (j[listItem]["splitItem"].size() > 0) {
            splitItem.clear();
            splitTime.clear();
            splitPreviousBest.clear();
            splitBest.clear();
            splitStatus.clear();

            for (int i = 0; i < j[listItem]["splitItem"].size(); i++) {
                splitItem.push_back(0);
                splitTime.push_back(0);
                splitPreviousBest.push_back(0);
                splitBest.push_back(100000);
                if (i == 0) {
                    splitStatus.push_back(2);
                } else {
                    splitStatus.push_back(0);
                }
            }
            splitItem = j[listItem]["splitItem"].get<std::vector<uint32_t>>();
            splitTime = j[listItem]["splitTime"].get<std::vector<uint32_t>>();
            splitTokens = j[listItem]["splitTokens"];
            splitPreviousBest = j[listItem]["splitPreviousBest"].get<std::vector<uint32_t>>();
            colorChoice.x = j[listItem]["backgroundColor.r"];
            colorChoice.y = j[listItem]["backgroundColor.g"];
            colorChoice.z = j[listItem]["backgroundColor.b"];
            colorChoice.w = j[listItem]["backgroundColor.a"];
            file.close();
            status = "List has been loaded from Save Data";
            statusColor = COLOR_LIGHT_BLUE;
        } else {
            status = "List does not exist in Save Data";
            statusColor = COLOR_RED;
        }
        
    }
    UIWidgets::Spacer(0);
    ImGui::Text("Background Color");
    if (ImGui::ColorEdit4("Background Color", (float*)&colorChoice, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        Color_RGBA8 color;
        color.r = colorChoice.x;
        color.g = colorChoice.y;
        color.b = colorChoice.z;
        color.a = colorChoice.w;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        colorChoice = { 0.0f, 0.0f, 0.0f, 1.0f };
    }
    UIWidgets::Spacer(0);
    if (ImGui::Button("New Attempt")) {
        splitStatus.clear();
        splitTime.clear();
        for (int i = 0; i < splitItem.size(); i++) {
            if (i == 0)  {
                splitStatus.push_back(2);
            } else {
                splitStatus.push_back(0);
            }
            splitTime.push_back(0);
        }
        splitCurNum++;
        std::stringstream ss;
        ss << splitCurNum;
        splitAttempt = (splitNumDisp).c_str() + ss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Update Splits")) {
        for (int i = 0; i < splitItem.size(); i++) {
            if (splitPreviousBest[i] == 0) {
                splitPreviousBest[i] = splitBest[i];
            } else if (splitPreviousBest[i] > splitBest[i]) {
                splitPreviousBest[i] = splitBest[i];
            }
        }
        status = "Splits updated";
        statusColor = COLOR_GREEN;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset List")) {
        splitItem.clear();
        splitTime.clear();
        splitTokens = 0;
        splitStatus.clear();
        splitPreviousBest.clear();
        status = "List has been reset";
        statusColor = COLOR_RED;
    }
    UIWidgets::EnhancementCheckbox("Enable Removals", "gTimeSplit.EnableEdits", false);
    ImGui::TextColored(statusColor, status.c_str());
    UIWidgets::PaddedSeparator();
}

void TimeSplitColorTint() {
    switch (itemNum) {
        case BOSS_QUEEN_GOHMA:
            pieceTint = COLOR_LIGHT_GREEN;
            break;
        case BOSS_KING_DODONGO:
            pieceTint = COLOR_LIGHT_RED;
            break;
        case BOSS_BARINADE:
            pieceTint = COLOR_LIGHT_BLUE;
            break;
        case BOSS_PHANTOM_GANON:
            pieceTint = COLOR_GREEN;
            break;
        case BOSS_VOLVAGIA:
            pieceTint = COLOR_RED;
            break;
        case BOSS_MORPHA:
            pieceTint = COLOR_BLUE;
            break;
        case BOSS_BONGO_BONGO:
            pieceTint = COLOR_PURPLE;
            break;
        case BOSS_TWINROVA:
            pieceTint = COLOR_ORANGE;
            break;
        case BOSS_GANONDORF:
            pieceTint = COLOR_GREY;
            break;
        case BOSS_GANON:
            pieceTint = COLOR_YELLOW;
            break;
        default:
            pieceTint = COLOR_WHITE;
            break;
    }
}

void DrawTimeSplitSplits(){
    uint32_t loopCounter = 0;
    uint32_t buttonID = 0;
    if (ImGui::CollapsingHeader("Time Splitter")) {
        DrawTimeSplitOptions();
    }
    //List Removals
    if (CVarGetInteger("gTimeSplit.EnableEdits", 0)) {
        ImGui::BeginTable("Remove Entries", 4);
        ImGui::TableSetupColumn("Item Image", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, (28.0f * uiScale));
        ImGui::TableSetupColumn("Item Name");
        ImGui::TableSetupColumn("Current Best", ImGuiTableColumnFlags_WidthFixed, (90.0f * uiScale));
        ImGui::TableSetupColumn("Prev. Best", ImGuiTableColumnFlags_WidthFixed, (90.0f * uiScale));
        ImGui::TableHeadersRow();
        ImGui::TableNextColumn();
        for (int i = 0; i < splitItem.size(); i++) {
            for (const auto& obj : splitObjects) {
                if (obj.itemID == splitItem[i]) {
                    itemNum = obj.itemID;
                    itemImager = obj.itemImage;
                    itemNamer = obj.itemName;
                    TimeSplitColorTint();
                }
            }
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.0f, 0.0f, 1.0f));
            // Item Image (Removal)
            if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemImager),
                               ImVec2(26.0f * uiScale, 26.0f *uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), pieceTint)) {
                uint32_t currentStatus = splitStatus[i];
                splitItem.erase(splitItem.begin() + i);
                splitTime.erase(splitTime.begin() + i);
                splitPreviousBest.erase(splitPreviousBest.begin() + i);
                splitBest.erase(splitBest.begin() + i);
                splitStatus.erase(splitStatus.begin() + i);
                splitStatus[i] = currentStatus;
            }
            ImGui::TableNextColumn();
            // Item Name (Removal)
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f * uiScale));
            ImGui::AlignTextToFramePadding();
            if (itemNum == ITEM_SKULL_TOKEN) {
                std::string skullText = itemNamer.c_str();
                skullText += " (";
                skullText += std::to_string(splitTokens).c_str();
                skullText += ")";
                ImGui::TextWrapped(skullText.c_str());
            } else {
                ImGui::TextWrapped(itemNamer.c_str());
            }
            ImGui::TableNextColumn();
            if (splitBest[i] != 100000) {
                if (ImGui::Button(std::string(formatTimestampTimeSplit(splitBest[i])).c_str(),
                                  ImVec2(85.0f, 30.0f))) {
                    splitBest[i] = 100000;
                }
            } else {
                ImGui::TextColored(COLOR_RED, "No Data");
            }
            ImGui::TableNextColumn();
            // Previous Best (Removal)
            if (splitPreviousBest[i] != 0) {
                if (ImGui::Button(std::string(formatTimestampTimeSplit(splitPreviousBest[i])).c_str(), ImVec2(85.0f, 30.0f))) {
                    splitPreviousBest[i] = 0;
                }
            } else {
                ImGui::TextColored(COLOR_RED, "No Data");
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    } else {
        totalPreviousBest = 0;
        for (int i = 0; i < splitItem.size(); i++) {
            totalPreviousBest += splitPreviousBest[i];
        }
        ImGui::TextColored(COLOR_YELLOW, (splitAttempt).c_str());
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 0));
        ImGui::BeginTable("Splits", 5, ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable);
        ImGui::TableSetupColumn("Item Image", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, (27.0f * uiScale));
        ImGui::TableSetupColumn("Item Name");
        ImGui::TableSetupColumn("Current Time", ImGuiTableColumnFlags_WidthFixed, (90.0f * uiScale));
        ImGui::TableSetupColumn("+/-", ImGuiTableColumnFlags_WidthFixed, (80.0f * uiScale));
        ImGui::TableSetupColumn("Prev. Best", ImGuiTableColumnFlags_WidthFixed, (90.0f * uiScale));
        ImGui::TableHeadersRow();

        for (auto& str : splitItem) {
            ImGui::TableNextColumn();
            for (const auto& obj : splitObjects) {
                if (obj.itemID == splitItem[loopCounter]) {
                    itemNum = obj.itemID;
                    itemImager = obj.itemImage;
                    itemNamer = obj.itemName;
                    TimeSplitColorTint();
                }
            }
            // Item Image
            if (splitStatus[buttonID] == 2) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(47, 79, 90, 255));
            }
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
            ImGui::ImageButton(std::to_string(buttonID).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemImager),
                                   ImVec2(26.0f * uiScale, 26.0f *uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), pieceTint);
            ImGui::PopStyleColor();
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("DragMove", &buttonID, sizeof(uint32_t));

                ImGui::Text("Move %s", itemNamer.c_str());
                ImGui::EndDragDropSource();
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragMove")) {
                    IM_ASSERT(payload->DataSize == sizeof(uint32_t));
                    int nextIndex = *(const int*)payload->Data;
                    uint32_t draggedItem = splitItem[nextIndex];
                    uint32_t draggedTime = splitTime[nextIndex];
                    uint32_t draggedBest = splitBest[nextIndex];
                    uint32_t draggedPreviousBest = splitPreviousBest[nextIndex];
                    uint32_t draggedStatus = splitStatus[nextIndex];
                    if (loopCounter < nextIndex) {
                        int v = 0;
                        int o = nextIndex - v - 1;
                        for (int i = loopCounter + 1; i < nextIndex + 1; ++i) {
                            splitItem[nextIndex - v] = splitItem[o];
                            splitTime[nextIndex - v] = splitTime[o];
                            splitBest[nextIndex - v] = splitBest[o];
                            splitPreviousBest[nextIndex - v] = splitPreviousBest[o];
                            splitStatus[nextIndex - v] = splitStatus[o];
                            v++;
                            o--;
                        }
                        splitStatus[loopCounter] = 3;
                    } else {
                        int v = 0;
                        int o = nextIndex + v + 1;
                        for (int i = loopCounter - 1; i > nextIndex - 1; i--) {
                            splitItem[nextIndex + v] = splitItem[o];
                            splitTime[nextIndex + v] = splitTime[o];
                            splitBest[nextIndex + v] = splitBest[o];
                            splitPreviousBest[nextIndex + v] = splitPreviousBest[o];
                            splitStatus[nextIndex + v] = splitStatus[o];
                            v++;
                            o++;
                        }
                        splitStatus[loopCounter] = 0;
                    }
                    splitItem[loopCounter] = draggedItem;
                    splitTime[loopCounter] = draggedTime;
                    splitBest[loopCounter] = draggedBest;
                    splitPreviousBest[loopCounter] = draggedPreviousBest;
                    splitStatus[loopCounter] = draggedStatus;
                    for (int i = 0; i < splitItem.size(); i++) {
                        if (splitStatus[i] != 1) {
                            splitStatus[i] = 2;
                            for (int v = i + 1; v < splitItem.size(); v++) {
                                if (splitStatus[v] != 1){
                                    splitStatus[v] = 0;
                                }
                            }
                            i = splitItem.size() - 1;
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::TableNextColumn();
            // Item Name
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f * uiScale));
            ImGui::AlignTextToFramePadding();
            if (itemNum == ITEM_SKULL_TOKEN) {
                std::string skullText = itemNamer.c_str();
                skullText += " (";
                skullText += std::to_string(splitTokens).c_str();
                skullText += ")";
                ImGui::TextWrapped(skullText.c_str());
            } else {
                ImGui::TextWrapped(itemNamer.c_str());
            }
            ImGui::TableNextColumn();
            if (splitStatus[loopCounter] == 2) {
                // Current Time (Active)
                if (splitTime[loopCounter] == 0) {
                    ImGui::Text(formatTimestampTimeSplit(GAMEPLAYSTAT_TOTAL_TIME).c_str());
                } else {
                    ImGui::Text(formatTimestampTimeSplit(splitTime[loopCounter]).c_str());
                }
                ImGui::TableNextColumn();
                // +/- (active)
                if (splitPreviousBest[loopCounter] == 0) {
                    ImGui::TextColored(COLOR_WHITE, formatTimestampTimeSplit(GAMEPLAYSTAT_TOTAL_TIME).c_str());
                } else {
                    if (GAMEPLAYSTAT_TOTAL_TIME < splitPreviousBest[loopCounter]) {
                        ImGui::TextColored(COLOR_LIGHT_GREEN, formatTimestampTimeSplit(splitPreviousBest[loopCounter] - GAMEPLAYSTAT_TOTAL_TIME).c_str());
                    } else if (GAMEPLAYSTAT_TOTAL_TIME == splitPreviousBest[loopCounter]) {
                        ImGui::TextColored(COLOR_WHITE, formatTimestampTimeSplit(splitPreviousBest[loopCounter]).c_str());
                    } else if (GAMEPLAYSTAT_TOTAL_TIME > splitPreviousBest[loopCounter]) {
                        ImGui::TextColored(COLOR_LIGHT_RED, formatTimestampTimeSplit(GAMEPLAYSTAT_TOTAL_TIME - splitPreviousBest[loopCounter]).c_str());
                    }
                }
                ImGui::TableNextColumn();
                // Previous Best (Active)
                ImGui::Text(formatTimestampTimeSplit(splitPreviousBest[loopCounter]).c_str());
            } else if (splitStatus[loopCounter] == 1) {
                // Current (Collected)
                ImGui::Text(formatTimestampTimeSplit(splitTime[loopCounter]).c_str());
                ImGui::TableNextColumn();
                // +/- (Collected)
                if (splitPreviousBest[loopCounter] == 0) {
                    ImGui::TextColored(COLOR_WHITE, formatTimestampTimeSplit(splitTime[loopCounter]).c_str());
                } else {
                    if (splitTime[loopCounter] < splitPreviousBest[loopCounter]) {
                        ImGui::TextColored(COLOR_GREEN, formatTimestampTimeSplit(splitPreviousBest[loopCounter] - splitTime[loopCounter]).c_str());
                    } else if (splitTime[loopCounter] == splitPreviousBest[loopCounter]) {
                        ImGui::TextColored(COLOR_WHITE, formatTimestampTimeSplit(splitTime[loopCounter] - splitPreviousBest[loopCounter]).c_str());
                    } else if (splitTime[loopCounter] > splitPreviousBest[loopCounter]) {
                        ImGui::TextColored(COLOR_RED, formatTimestampTimeSplit(splitTime[loopCounter] - splitPreviousBest[loopCounter]).c_str());
                    }
                }
                ImGui::TableNextColumn();
                // Previous Best (Collected)
                ImGui::Text(formatTimestampTimeSplit(splitPreviousBest[loopCounter]).c_str());
            } else {
                // Current Time (Inactive)
                ImGui::Text("-:--:--.-");
                ImGui::TableNextColumn();
                // +/- (Inactive)
                ImGui::Text("-:--:--.-");
                ImGui::TableNextColumn();
                // Previous Best (Inactive)
                ImGui::Text("-:--:--.-");
            }
            ImGui::PopStyleVar();
            loopCounter++;
            buttonID++;
        }
        ImGui::EndTable();
        ImGui::PopStyleVar();
        UIWidgets::PaddedSeparator();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() -
                             ImGui::CalcTextSize(formatTimestampTimeSplit(GAMEPLAYSTAT_TOTAL_TIME).c_str()).x -
                             ImGui::CalcTextSize(formatTimestampTimeSplit(totalPreviousBest).c_str()).x - (60 * uiScale));
        ImGui::Text(formatTimestampTimeSplit(GAMEPLAYSTAT_TOTAL_TIME).c_str());
        ImGui::SameLine();
        ImGui::Text(" / ");
        ImGui::SameLine();
        if (splitItem.size() > 0) {
            uint32_t lastIndex = splitItem.size() - 1;
            ImGui::Text(formatTimestampTimeSplit(splitTime[lastIndex]).c_str());
        } else {
            ImGui::Text(formatTimestampTimeSplit(totalPreviousBest).c_str());
        }
    }
}

void InitializeSplitFile() {
    if (!std::filesystem::exists("splitdata.json")) {
        json j;
        j["splitItem"] = splitItem;
        j["splitTokens"] = splitTokens;
        j["splitTime"] = splitTime;
        j["splitPreviousBest"] = splitPreviousBest;
        j["backgroundColor.r"] = colorChoice.x;
        j["backgroundColor.g"] = colorChoice.y;
        j["backgroundColor.b"] = colorChoice.z;
        j["backgroundColor.a"] = colorChoice.w;
        std::ofstream file("splitdata.json");
        file << j.dump(4);
        file.close();
    }
}

static bool initialized = false;

void TimeSplitAddToSplits(uint32_t itemToAdd) {
    std::string itemStatus;
    for (const auto& obj : splitObjects) {
        if (obj.itemID == itemToAdd) {
            itemStatus = obj.itemName;
        }
    }
    splitItem.push_back(itemToAdd);
    splitTime.push_back(0);
    splitPreviousBest.push_back(0);
    splitBest.push_back(100000);
    if (splitItem.size() == 1) {
        splitStatus.push_back(2);
    } else {
        splitStatus.push_back(0);
    }
    statusColor = COLOR_GREEN;
    status = itemStatus + std::string(" added to list");
}

void DrawTimeSplitListManager() {
    if (ImGui::CollapsingHeader("Equipment & Quest Status")) {
        ImGui::BeginTable("Equipment", 3);
        for (int i = 0; i < 3; i++) {
            ImGui::TableSetupColumn(std::to_string(i).c_str(), ImGuiTableColumnFlags_WidthFixed, (35.0f * uiScale));
        }
        ImGui::TableNextColumn();
        for (int i = 0; i < 12; i++) {
            if (i == 4 || i == 8) {
                ImGui::TableNextColumn();
            }
            if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(equipmentObjects[i].itemImage),
                                ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                if (equipmentObjects[i].itemID == ITEM_BRACELET) {
                    ImGui::OpenPopup("Strength");
                } else if (equipmentObjects[i].itemID == ITEM_SCALE_SILVER) {
                    ImGui::OpenPopup("Scale");
                } else {
                    TimeSplitAddToSplits(equipmentObjects[i].itemID);
                }
            }
        }
        if (ImGui::BeginPopupContextItem("Strength")) {
            for (int i = ITEM_BRACELET; i <= ITEM_GAUNTLETS_GOLD; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupContextItem("Scale")) {
            for (int i = ITEM_SCALE_SILVER; i <= ITEM_SCALE_GOLDEN; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                    }
                }
            }
            ImGui::EndPopup();
        }
        ImGui::EndTable();
        ImGui::SameLine(129.0f * uiScale);

        ImGui::BeginTable("Quest", 6);
        for (int i = 0; i < 6; i++) {
            ImGui::TableSetupColumn(std::to_string(i).c_str(), ImGuiTableColumnFlags_WidthFixed, (35.0f * uiScale));
        }
        ImGui::TableNextColumn();
        for (int i = 0; i < 24; i++) {
            if (i == 4 || i == 8 || i == 12 || i == 16 || i == 20) {
                ImGui::TableNextColumn();
            }
            if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(questObjects[i].itemImage),
                                ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                if (questObjects[i].itemID == ITEM_SKULL_TOKEN) {
                    ImGui::OpenPopup("Skull Count");
                } else {
                    TimeSplitAddToSplits(questObjects[i].itemID);
                }
            }
        }
        if (ImGui::BeginPopupContextItem("Skull Count")) {
            UIWidgets::PaddedEnhancementSliderInt("Skulltula Count: %d Tokens", "##splitToken", "gTimeSplit.SkullTokens", 0, 100, "", 0, true, true, false);
            if (ImGui::Button("Set", ImVec2(30.0f, 20.0f))) {
                splitTokens = CVarGetInteger("gTimeSplit.SkullTokens", 0);
                TimeSplitAddToSplits(ITEM_SKULL_TOKEN);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::EndTable();
    }
    if (ImGui::CollapsingHeader("Inventory")) {
        ImGui::BeginTable("Inventory", 6);
        for (int i = 0; i < 6; i++) {
            ImGui::TableSetupColumn(std::to_string(i).c_str(), ImGuiTableColumnFlags_WidthFixed, (35.0f * uiScale));
        }
        ImGui::TableNextColumn();
        for (int i = 0; i < 24; i++) {
            if (i == 4 || i == 8 || i == 12 || i == 16 || i == 20) {
                ImGui::TableNextColumn();
            }
            if (ImGui::ImageButton(std::to_string(i).c_str(),
                                   LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(inventoryObjects[i].itemImage),
                                   ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                if (inventoryObjects[i].itemID == ITEM_STICK) {
                    //ImGui::OpenPopup("Stick");
                    TimeSplitAddToSplits(inventoryObjects[i].itemID);
                } else if (inventoryObjects[i].itemID == ITEM_SLINGSHOT) {
                    ImGui::OpenPopup("Slingshot");
                } else if (inventoryObjects[i].itemID == ITEM_NUT) {
                    //ImGui::OpenPopup("Nut");
                    TimeSplitAddToSplits(inventoryObjects[i].itemID);
                } else if (inventoryObjects[i].itemID == ITEM_OCARINA_FAIRY) {
                    ImGui::OpenPopup("Ocarina");
                } else if (inventoryObjects[i].itemID == ITEM_BOMB) {
                    ImGui::OpenPopup("Bomb");
                } else if (inventoryObjects[i].itemID == ITEM_BOW) {
                    ImGui::OpenPopup("Bow");
                } else if (inventoryObjects[i].itemID == ITEM_HOOKSHOT) {
                    ImGui::OpenPopup("Hookshot");
                } else if (inventoryObjects[i].itemID == ITEM_BOTTLE) {
                    ImGui::OpenPopup("Bottles");
                } else if (inventoryObjects[i].itemID == ITEM_MAGIC_SMALL) {
                    ImGui::OpenPopup("Magic");
                } else if (inventoryObjects[i].itemID == ITEM_WALLET_ADULT) {
                    ImGui::OpenPopup("Wallet");
                } else if (inventoryObjects[i].itemID == ITEM_POCKET_EGG) {
                    ImGui::OpenPopup("AdultTrade");
                } else if (inventoryObjects[i].itemID == ITEM_WEIRD_EGG) {
                    ImGui::OpenPopup("ChildTrade");
                } else {
                    TimeSplitAddToSplits(inventoryObjects[i].itemID);
                }
            }
        }
        if (ImGui::BeginPopupContextItem("Stick")) {
            for (int i = ITEM_STICK; i <= ITEM_STICK_UPGRADE_30; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                    }
                }
                if (i == ITEM_STICK) {
                    i = (ITEM_STICK_UPGRADE_20 - 1);
                }
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupContextItem("Slingshot")) {
            for (int i = ITEM_SLINGSHOT; i <= ITEM_BULLET_BAG_50; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                    }
                }
                if (i == ITEM_SLINGSHOT) {
                    i = (ITEM_BULLET_BAG_40 - 1);
                }
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupContextItem("Nut")) {
            for (int i = ITEM_NUT; i <= ITEM_NUT_UPGRADE_40; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                    }
                }
                if (i == ITEM_NUT) {
                    i = (ITEM_NUT_UPGRADE_30 - 1);
                }
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupContextItem("Ocarina")) {
            for (int i = ITEM_OCARINA_FAIRY; i <= ITEM_OCARINA_TIME; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupContextItem("Bomb")) {
            for (int i = ITEM_BOMB_BAG_20; i <= ITEM_BOMB_BAG_40; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupContextItem("Bow")) {
            for (int i = ITEM_BOW; i <= ITEM_QUIVER_50; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                    }
                }
                if (i == ITEM_BOW) {
                    i = (ITEM_QUIVER_40 - 1);
                }
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupContextItem("Hookshot")) {
            for (int i = ITEM_HOOKSHOT; i <= ITEM_LONGSHOT; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupContextItem("Bottles")) {
            for (int i = ITEM_BOTTLE; i <= ITEM_POE; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        if (i != ITEM_FISH) {
                            ImGui::SameLine();
                        }
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupContextItem("Magic")) {
            for (int i = ITEM_MAGIC_SMALL; i <= ITEM_MAGIC_LARGE; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupContextItem("Wallet")) {
            for (int i = ITEM_WALLET_ADULT; i <= ITEM_WALLET_GIANT; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupContextItem("AdultTrade")) {
            for (int i = ITEM_POCKET_EGG; i <= ITEM_CLAIM_CHECK; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        if (i != ITEM_SAW) {
                            ImGui::SameLine();
                        }
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupContextItem("ChildTrade")) {
            for (int i = ITEM_WEIRD_EGG; i <= ITEM_MASK_TRUTH; i++) {
                for (auto& obj : splitObjects) {
                    if (obj.itemID == i) {
                        if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                            TimeSplitAddToSplits(i);
                            ImGui::CloseCurrentPopup();
                        }
                        if (i != ITEM_MASK_SPOOKY) {
                            ImGui::SameLine();
                        }
                    }
                }
            }
            ImGui::EndPopup();
        }
        ImGui::EndTable();
    }
    // SPACE SAVE FOR MISC ITEMS (12 SLOTS/3 COLUMNS x4)
    if (ImGui::CollapsingHeader("Bosses")) {
        ImGui::BeginTable("Bosses", 2);
        ImGui::TableSetupColumn("Boss1", ImGuiTableColumnFlags_WidthFixed, (150.0f * uiScale));
        ImGui::TableSetupColumn("Boss2", ImGuiTableColumnFlags_WidthFixed, (150.0f * uiScale));
        ImGui::TableNextColumn();
        for (int i = 0; i < 10; i++) {
            if (i == 5) {
                ImGui::TableNextColumn();
            }
            itemNum = bossObjects[i].itemID;
            TimeSplitColorTint();
            if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(bossObjects[i].itemImage),
                                ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), pieceTint)) {
                TimeSplitAddToSplits(bossObjects[i].itemID);
            }
            ImGui::SameLine();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
            ImGui::AlignTextToFramePadding();
            ImGui::Text(bossObjects[i].itemName);
            ImGui::PopStyleVar(1);
        }
        ImGui::EndTable();
    }
    if (ImGui::CollapsingHeader("Scenes")) {
        if (ImGui::CollapsingHeader("Dungeon Entrances")) {
            ImGui::BeginTable("Dungeons", 2);
            ImGui::TableSetupColumn("Dungeon1", ImGuiTableColumnFlags_WidthFixed, (220.0f * uiScale));
            ImGui::TableSetupColumn("Dungeon2", ImGuiTableColumnFlags_WidthFixed, (220.0f * uiScale));
            ImGui::TableNextColumn();
            for (int i = 0; i < 16; i++) {
                if (i == 8) {
                    ImGui::TableNextColumn();
                }
                //itemNum = sceneObjects[i].itemID;
                //TimeSplitColorTint();
                if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(sceneObjects[i].itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                    TimeSplitAddToSplits(sceneObjects[i].itemID);
                }
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
                ImGui::AlignTextToFramePadding();
                ImGui::TextWrapped(sceneObjects[i].itemName);
                ImGui::PopStyleVar(1);
            }
            ImGui::EndTable();
        }
        if (ImGui::CollapsingHeader("Overworld Entrances")) {
            ImGui::BeginTable("Overworld", 1);
            ImGui::TableSetupColumn("Overworld1", ImGuiTableColumnFlags_WidthFixed, (220.0f * uiScale));
            ImGui::TableNextColumn();
            for (int i = 16; i <= 17; i++) {
                //itemNum = obj.itemID;
                //TimeSplitColorTint();
                if (ImGui::ImageButton(std::to_string(i).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(sceneObjects[i].itemImage),
                                       ImVec2(32.0f * uiScale, 32.0f * uiScale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
                    TimeSplitAddToSplits(sceneObjects[i].itemID);
                }
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
                ImGui::AlignTextToFramePadding();
                ImGui::TextWrapped(sceneObjects[i].itemName);
                ImGui::PopStyleVar(1);
            }
            ImGui::EndTable();
        }
    }
}

void TimeSplitWindow::DrawElement() {
    if (!initialized) {
        InitializeSplitFile();
        initialized = true;
    }

    if (CVarGetInteger("gImGuiScale", 0)) {
        uiScale = 1;
    } else {
        uiScale = CVarGetInteger("gImGuiScale", 0);
    }
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg, colorChoice);
    if (!ImGui::Begin("Time Splitter Window", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        ImGui::PopStyleColor(1);
        return;
    }
    ImGui::BeginTabBar("Split Tabs");
    if (ImGui::BeginTabItem("Splits")) {
        DrawTimeSplitSplits();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Manage List")) {
        ImGui::TextColored(statusColor, status.c_str());
        UIWidgets::PaddedSeparator();
        ImGui::BeginChild("Add Items");
        DrawTimeSplitListManager();
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    ImGui::End();
    ImGui::PopStyleColor();
}

void TimeSplitWindow::InitElement() {
    LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("SPECIAL_TRIFORCE_PIECE_WHITE", gWTriforcePieceTex, ImVec4(1, 1, 1, 1));
    LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("SPECIAL_SPLIT_ENTRANCE", gSplitEntranceTex, ImVec4(1, 1, 1, 1));
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
        TimeSplitSplitsHandler(itemEntry);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneSpawnActors>([]() {
        TimeSplitSceneSplitHandler(gSaveContext.entranceIndex);
    });
}