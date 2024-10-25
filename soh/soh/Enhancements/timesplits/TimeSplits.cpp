#include "TimeSplits.h"
#include "soh/UIWidgets.hpp"
#include "soh/Enhancements/gameplaystats.h"
#include "soh/SaveManager.h"
#include "soh/util.h"
#include <vector>
#include "include/z64item.h"

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

using json = nlohmann::json;

static uint32_t splitBestTimeDisplay;
static int32_t popupID = -1;
static int32_t removeIndex = -1;
static uint32_t tableSize = 0;
static int skullTokenCount = 0;
static float timeSplitsWindowSize = 1.0f;

static ImVec4 windowColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
static ImVec4 splitStatusColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
static ImVec4 splitTimeColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
static ImVec4 activeSplitHighlight = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

static ImVec2 imageSize = ImVec2(38.0f, 38.0f);
static float imagePadding = 2.0f;

std::vector<std::string> keys;

char listNameBuf[25];
int dragSourceIndex = -1;
int dragTargetIndex = -1;

std::vector<SplitObject> splitList;
std::vector<SplitObject> emptyList;

std::vector<SplitObject> splitObjectList = {
    { SPLIT_ITEM, 	    ITEM_STICK, 	 		                "Deku Stick", 							"ITEM_STICK", 					COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_NUT,				                "Deku Nut", 							"ITEM_NUT", 					COLOR_WHITE },
    { SPLIT_ITEM,       ITEM_BOMB,                              "Bomb",                                 "ITEM_BOMB",                    COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_BOW, 	 			                "Fairy Bow", 							"ITEM_BOW", 					COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_ARROW_FIRE, 		                "Fire Arrow", 							"ITEM_ARROW_FIRE", 				COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_DINS_FIRE, 		                "Din's Fire", 							"ITEM_DINS_FIRE", 				COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_SLINGSHOT, 		                "Fairy Slingshot", 						"ITEM_SLINGSHOT", 				COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_OCARINA_FAIRY, 	                "Fairy Ocarina", 						"ITEM_OCARINA_FAIRY", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_OCARINA_TIME, 		                "Ocarina of Time", 						"ITEM_OCARINA_TIME", 			COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_BOMBCHU, 	 		                "Bombchu", 								"ITEM_BOMBCHU", 				COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_HOOKSHOT,  		                "Hookshot", 							"ITEM_HOOKSHOT", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_LONGSHOT,  		                "Longshot", 							"ITEM_LONGSHOT", 				COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_ARROW_ICE,  		                "Ice Arrow", 							"ITEM_ARROW_ICE", 				COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_FARORES_WIND,  	                "Farore's Wind", 						"ITEM_FARORES_WIND", 			COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_BOOMERANG,  		                "Boomerang", 							"ITEM_BOOMERANG", 				COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_LENS,  			                "Lens of Truth", 						"ITEM_LENS", 					COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_BEAN,  			                "Magic Bean", 							"ITEM_BEAN", 					COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_HAMMER,  			                "Megaton Hammer", 						"ITEM_HAMMER", 					COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_ARROW_LIGHT,  		                "Light Arrow", 							"ITEM_ARROW_LIGHT", 			COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_NAYRUS_LOVE,  		                "Nayru's Love", 						"ITEM_NAYRUS_LOVE", 			COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_BOTTLE,  			                "Empty Bottle", 						"ITEM_BOTTLE", 					COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_POTION_RED,  		                "Red Potion", 							"ITEM_POTION_RED", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_POTION_GREEN,  	                "Green Potion", 						"ITEM_POTION_GREEN", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_POTION_BLUE,  		                "Blue Potion", 							"ITEM_POTION_BLUE", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_FAIRY,  			                "Bottled Fairy", 						"ITEM_FAIRY", 					COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_FISH,  			                "Fish", 								"ITEM_FISH", 					COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_MILK_BOTTLE,  		                "Milk", 				                "ITEM_MILK_BOTTLE", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_LETTER_RUTO,  		                "Ruto's Letter", 						"ITEM_LETTER_RUTO", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_BLUE_FIRE,  		                "Blue Fire", 							"ITEM_BLUE_FIRE", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_BUG,  				                "Bug", 									"ITEM_BUG", 					COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_BIG_POE,  			                "Big Poe", 								"ITEM_BIG_POE", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_POE,  				                "Poe", 									"ITEM_POE", 					COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_WEIRD_EGG,  		                "Weird Egg", 							"ITEM_WEIRD_EGG", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_CHICKEN,  			                "Chicken", 								"ITEM_CHICKEN", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_LETTER_ZELDA,  	                "Zelda's Letter", 						"ITEM_LETTER_ZELDA", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_MASK_KEATON,  		                "Keaton Mask", 							"ITEM_MASK_KEATON", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_MASK_SKULL,  		                "Skull Mask", 							"ITEM_MASK_SKULL", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_MASK_SPOOKY,  		                "Spooky Mask", 							"ITEM_MASK_SPOOKY", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_MASK_BUNNY,  		                "Bunny Hood", 							"ITEM_MASK_BUNNY", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_MASK_GORON,  		                "Goron Mask", 							"ITEM_MASK_GORON", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_MASK_ZORA,  		                "Zora Mask", 							"ITEM_MASK_ZORA", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_MASK_GERUDO,  		                "Gerudo Mask", 							"ITEM_MASK_GERUDO", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_MASK_TRUTH,  		                "Mask of Truth", 						"ITEM_MASK_TRUTH", 				COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_POCKET_EGG,  		                "Pocket Egg", 							"ITEM_POCKET_EGG", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_POCKET_CUCCO,  	                "Pocket Cucco", 						"ITEM_POCKET_CUCCO", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_COJIRO,  			                "Cojiro", 								"ITEM_COJIRO", 					COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_ODD_MUSHROOM,  	                "Odd Mushroom", 						"ITEM_ODD_MUSHROOM", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_ODD_POTION,  		                "Odd Potion", 							"ITEM_ODD_POTION", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_SAW,  				                "Poacher's Saw", 						"ITEM_SAW", 					COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_SWORD_BROKEN,  	                "Goron's Sword (Broken)", 				"ITEM_SWORD_BROKEN", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_PRESCRIPTION,  	                "Prescription", 						"ITEM_PRESCRIPTION", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_FROG,  			                "Eyeball Frog", 						"ITEM_FROG", 					COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_EYEDROPS,  		                "Eye Drops", 							"ITEM_EYEDROPS", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_CLAIM_CHECK,  		                "Claim Check",  						"ITEM_CLAIM_CHECK", 			COLOR_WHITE },
    { SPLIT_EQUIPMENT, 	ITEM_SWORD_KOKIRI,  	                "Kokiri Sword", 						"ITEM_SWORD_KOKIRI", 			COLOR_WHITE },
    { SPLIT_EQUIPMENT, 	ITEM_SWORD_MASTER,  	                "Master Sword", 						"ITEM_SWORD_MASTER", 			COLOR_WHITE },
    { SPLIT_EQUIPMENT, 	ITEM_SWORD_BGS,  		                "Giant's Knife & Biggoron's Sword", 	"ITEM_SWORD_BGS", 				COLOR_WHITE },
    { SPLIT_EQUIPMENT, 	ITEM_SHIELD_DEKU,  		                "Deku Shield", 							"ITEM_SHIELD_DEKU", 			COLOR_WHITE },
    { SPLIT_EQUIPMENT, 	ITEM_SHIELD_HYLIAN, 	                "Hylian Shield", 						"ITEM_SHIELD_HYLIAN", 			COLOR_WHITE },
    { SPLIT_EQUIPMENT, 	ITEM_SHIELD_MIRROR, 	                "Mirror Shield", 						"ITEM_SHIELD_MIRROR", 			COLOR_WHITE },
    { SPLIT_EQUIPMENT, 	ITEM_TUNIC_GORON,  		                "Goron Tunic", 							"ITEM_TUNIC_GORON", 			COLOR_WHITE },
    { SPLIT_EQUIPMENT, 	ITEM_TUNIC_ZORA,  		                "Zora Tunic", 							"ITEM_TUNIC_ZORA", 				COLOR_WHITE },
    { SPLIT_EQUIPMENT, 	ITEM_BOOTS_IRON,  		                "Iron Boots", 							"ITEM_BOOTS_IRON", 				COLOR_WHITE },
    { SPLIT_EQUIPMENT, 	ITEM_BOOTS_HOVER,  		                "Hover Boots", 							"ITEM_BOOTS_HOVER", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_BULLET_BAG_30, 	                "Bullet Bag (30)", 						"ITEM_BULLET_BAG_30", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_BULLET_BAG_40, 	                "Bullet Bag (40)", 						"ITEM_BULLET_BAG_40", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_BULLET_BAG_50, 	                "Bullet Bag (50)", 						"ITEM_BULLET_BAG_50", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_QUIVER_30,  		                "Quiver (30)", 							"ITEM_QUIVER_30", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_QUIVER_40,  		                "Big Quiver (40)", 						"ITEM_QUIVER_40", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_QUIVER_50,  		                "Biggest Quiver (50)", 					"ITEM_QUIVER_50", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_BOMB_BAG_20,  		                "Bomb Bag (20)", 						"ITEM_BOMB_BAG_20", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_BOMB_BAG_30,  		                "Big Bomb Bag (30)", 					"ITEM_BOMB_BAG_30", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_BOMB_BAG_40,  		                "Biggest Bomb Bag (40)", 				"ITEM_BOMB_BAG_40", 			COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_BRACELET,  		                "Goron's Bracelet", 					"ITEM_BRACELET", 				COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_GAUNTLETS_SILVER,                  "Silver Gauntlets", 					"ITEM_GAUNTLETS_SILVER", 		COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_GAUNTLETS_GOLD,  	                "Golden Gauntlets", 					"ITEM_GAUNTLETS_GOLD", 			COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_SCALE_SILVER,  	                "Silver Scale", 						"ITEM_SCALE_SILVER", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_SCALE_GOLDEN,  	                "Golden Scale", 						"ITEM_SCALE_GOLDEN", 			COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_SWORD_KNIFE,  		                "Giant's Knife (Broken)", 				"ITEM_SWORD_KNIFE", 			COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_WALLET_ADULT,  	                "Adult's Wallet", 						"ITEM_WALLET_ADULT", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_WALLET_GIANT,  	                "Giant's Wallet", 						"ITEM_WALLET_GIANT", 			COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_FISHING_POLE,  	                "Fishing Pole", 						"ITEM_FISHING_POLE", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SONG_MINUET,  		                "Minuet of Forest",						"QUEST_SONG_MINUET", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SONG_BOLERO,  		                "Bolero of Fire", 						"QUEST_SONG_BOLERO", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SONG_SERENADE,  	                "Serenade of Water", 					"QUEST_SONG_SERENADE", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SONG_REQUIEM,  	                "Requiem of Spirit", 					"QUEST_SONG_REQUIEM", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SONG_NOCTURNE,  	                "Nocturne of Shadow", 					"QUEST_SONG_NOCTURNE", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SONG_PRELUDE,  	                "Prelude of Light", 					"QUEST_SONG_PRELUDE", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SONG_LULLABY,  	                "Zelda's Lullaby", 						"QUEST_SONG_LULLABY", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SONG_EPONA,  		                "Epona's Song", 						"QUEST_SONG_EPONA", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SONG_SARIA,  		                "Saria's Song", 						"QUEST_SONG_SARIA", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SONG_SUN,  		                "Sun's Song", 							"QUEST_SONG_SUN", 				COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SONG_TIME, 		                "Song of Time", 						"QUEST_SONG_TIME", 				COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SONG_STORMS, 		                "Song of Storms", 						"QUEST_SONG_STORMS", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_MEDALLION_FOREST, 	                "Forest Medallion", 					"QUEST_MEDALLION_FOREST", 		COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_MEDALLION_FIRE, 	                "Fire Medallion", 						"QUEST_MEDALLION_FIRE", 		COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_MEDALLION_WATER, 	                "Water Medallion", 						"QUEST_MEDALLION_WATER", 		COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_MEDALLION_SPIRIT, 	                "Spirit Medallion", 					"QUEST_MEDALLION_SPIRIT", 		COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_MEDALLION_SHADOW, 	                "Shadow Medallion", 					"QUEST_MEDALLION_SHADOW", 		COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_MEDALLION_LIGHT, 	                "Light Medallion", 						"QUEST_MEDALLION_LIGHT", 		COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_KOKIRI_EMERALD, 	                "Kokiri's Emerald", 					"QUEST_KOKIRI_EMERALD", 		COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_GORON_RUBY, 		                "Goron's Ruby", 						"QUEST_GORON_RUBY", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_ZORA_SAPPHIRE, 	                "Zora's Sapphire", 						"QUEST_ZORA_SAPPHIRE", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_STONE_OF_AGONY, 	                "Stone of Agony", 						"QUEST_STONE_OF_AGONY", 		COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_GERUDO_CARD, 		                "Gerudo's Card", 						"QUEST_GERUDO_CARD", 			COLOR_WHITE },
    { SPLIT_QUEST, 	    ITEM_SKULL_TOKEN,                       "Skulltula Token",                      "QUEST_SKULL_TOKEN", 			COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_SINGLE_MAGIC, 		                "Magic Meter", 							"ITEM_MAGIC_SMALL", 			COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_DOUBLE_MAGIC, 		                "Double Magic", 				        "ITEM_MAGIC_LARGE", 			COLOR_WHITE },
    { SPLIT_ITEM, 	    ITEM_DOUBLE_DEFENSE, 	                "Double Defense", 						"ITEM_HEART_CONTAINER", 		COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_STICK_UPGRADE_20, 	                "Deku Stick Upgrade (20)", 				"ITEM_STICK", 					COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_STICK_UPGRADE_30, 	                "Deku Stick Upgrade (30)", 				"ITEM_STICK", 					COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_NUT_UPGRADE_30, 	                "Deku Nut Upgrade (30)", 				"ITEM_NUT", 					COLOR_WHITE },
    { SPLIT_UPGRADE, 	ITEM_NUT_UPGRADE_40, 	                "Deku Nut Upgrade (40)", 				"ITEM_NUT", 					COLOR_WHITE },
    { SPLIT_BOSS, 	    ACTOR_BOSS_GOMA,                        "Queen Gohma",                          "SPECIAL_TRIFORCE_PIECE_WHITE",	COLOR_LIGHT_GREEN },
    { SPLIT_BOSS, 	    ACTOR_BOSS_DODONGO,                     "King Dodongo",                         "SPECIAL_TRIFORCE_PIECE_WHITE",	COLOR_LIGHT_RED },
    { SPLIT_BOSS, 	    ACTOR_BOSS_VA,                          "Barinade",                             "SPECIAL_TRIFORCE_PIECE_WHITE",	COLOR_LIGHT_BLUE },
    { SPLIT_BOSS, 	    ACTOR_BOSS_GANONDROF,                   "Phantom Ganon",                        "SPECIAL_TRIFORCE_PIECE_WHITE",	COLOR_GREEN },
    { SPLIT_BOSS, 	    ACTOR_BOSS_FD2,                         "Volvagia",                             "SPECIAL_TRIFORCE_PIECE_WHITE",	COLOR_RED },
    { SPLIT_BOSS, 	    ACTOR_BOSS_MO,                          "Morpha",                               "SPECIAL_TRIFORCE_PIECE_WHITE",	COLOR_BLUE },
    { SPLIT_BOSS, 	    ACTOR_BOSS_SST,                         "Bongo Bongo",                          "SPECIAL_TRIFORCE_PIECE_WHITE",	COLOR_PURPLE },
    { SPLIT_BOSS, 	    ACTOR_BOSS_TW,                          "Twinrova",                             "SPECIAL_TRIFORCE_PIECE_WHITE",	COLOR_ORANGE },
    { SPLIT_BOSS, 	    ACTOR_BOSS_GANON,                       "Ganondorf",                            "SPECIAL_TRIFORCE_PIECE_WHITE",	COLOR_GREY },
    { SPLIT_BOSS, 	    ACTOR_BOSS_GANON2,                      "Ganon",                                "SPECIAL_TRIFORCE_PIECE_WHITE",	COLOR_YELLOW },
    { SPLIT_ENTRANCE,	SCENE_DEKU_TREE,                        "Enter Deku Tree",                      "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_DODONGOS_CAVERN,                  "Enter Dodongos Cavern",                "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_JABU_JABU,                        "Enter Jabu Jabu's Belly",              "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_FOREST_TEMPLE,                    "Enter Forest Temple",                  "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_FIRE_TEMPLE,                      "Enter Fire Temple",                    "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_WATER_TEMPLE,                     "Enter Water Temple",                   "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_SPIRIT_TEMPLE,                    "Enter Spirit Temple",                  "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_SHADOW_TEMPLE,                    "Enter Shadow Temple",                  "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_BOTTOM_OF_THE_WELL,               "Enter Bottom of the Well",             "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_ICE_CAVERN,                       "Enter Ice Cavern",                     "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_GANONS_TOWER,                     "Enter Ganons Tower",                   "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_GERUDO_TRAINING_GROUND,           "Enter Gerudo Training Grounds",        "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_THIEVES_HIDEOUT,                  "Enter Thieves Hideout",                "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_INSIDE_GANONS_CASTLE,             "Enter Ganons Castle",                  "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_GANONS_TOWER_COLLAPSE_INTERIOR,   "Enter Tower Collapse Interior",        "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_ENTRANCE,	SCENE_INSIDE_GANONS_CASTLE_COLLAPSE,    "Enter Ganons Castle Collapse",         "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_MISC,	    SCENE_ZORAS_RIVER,                      "Lost Woods Escape",                    "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_MISC,	    SCENE_LOST_WOODS,                       "Forest Escape",                        "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
    { SPLIT_MISC,		SCENE_KAKARIKO_VILLAGE,                 "Watchtower Death",                     "SPECIAL_SPLIT_ENTRANCE", 		COLOR_WHITE },
};

std::map<uint32_t, std::vector<uint32_t>> popupList = {
    { ITEM_STICK,           { ITEM_STICK, ITEM_STICK_UPGRADE_20, ITEM_STICK_UPGRADE_30 } },
    { ITEM_NUT,             { ITEM_NUT, ITEM_NUT_UPGRADE_30, ITEM_NUT_UPGRADE_40 } },
    { ITEM_BOMB,            { ITEM_BOMB_BAG_20, ITEM_BOMB_BAG_30, ITEM_BOMB_BAG_40 } },
    { ITEM_BOW,             { ITEM_QUIVER_30, ITEM_QUIVER_40, ITEM_QUIVER_50 } },
    { ITEM_SLINGSHOT,       { ITEM_BULLET_BAG_30, ITEM_BULLET_BAG_40, ITEM_BULLET_BAG_50 } },
    { ITEM_OCARINA_FAIRY,   { ITEM_OCARINA_FAIRY, ITEM_OCARINA_TIME } },
    { ITEM_HOOKSHOT,        { ITEM_HOOKSHOT, ITEM_LONGSHOT } },
    { ITEM_BOTTLE,          { ITEM_BOTTLE, ITEM_POTION_RED, ITEM_POTION_GREEN, ITEM_POTION_BLUE,	
                              ITEM_FAIRY, ITEM_FISH, ITEM_MILK_BOTTLE, ITEM_LETTER_RUTO,
                              ITEM_BLUE_FIRE, ITEM_BUG, ITEM_BIG_POE, ITEM_POE } },
    { ITEM_WEIRD_EGG,       { ITEM_WEIRD_EGG, ITEM_CHICKEN, ITEM_LETTER_ZELDA, ITEM_MASK_KEATON, 
                              ITEM_MASK_SKULL, ITEM_MASK_SPOOKY, ITEM_MASK_BUNNY, ITEM_MASK_GORON,
                              ITEM_MASK_ZORA, ITEM_MASK_GERUDO, ITEM_MASK_TRUTH } },
    { ITEM_POCKET_EGG,      { ITEM_POCKET_EGG, ITEM_POCKET_CUCCO, ITEM_COJIRO, ITEM_ODD_MUSHROOM,
                              ITEM_ODD_POTION, ITEM_SAW, ITEM_SWORD_BROKEN, ITEM_PRESCRIPTION,
                              ITEM_FROG, ITEM_EYEDROPS, ITEM_CLAIM_CHECK } },
    { ITEM_BRACELET,        { ITEM_BRACELET, ITEM_GAUNTLETS_SILVER, ITEM_GAUNTLETS_GOLD } },
    { ITEM_SCALE_SILVER,    { ITEM_SCALE_SILVER, ITEM_SCALE_GOLDEN } },
    { ITEM_WALLET_ADULT,    { ITEM_WALLET_ADULT, ITEM_WALLET_GIANT } },
    { ITEM_SINGLE_MAGIC,    { ITEM_SINGLE_MAGIC, ITEM_DOUBLE_MAGIC } },
    { ITEM_SKULL_TOKEN,     { } }
};

std::string removeSpecialCharacters(const std::string& str) {
    std::string result;
    for (char ch : str) {
        // Only keep alphanumeric characters (letters and digits)
        if (std::isalnum(static_cast<unsigned char>(ch))) {
            result += ch;
        }
    }
    return result;
}

std::string formatTimestampTimeSplit(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
}

nlohmann::json ImVec4_to_json(const ImVec4& vec) {
    return nlohmann::json{
        {"x", vec.x},
        {"y", vec.y},
        {"z", vec.z},
        {"w", vec.w}
    };
}

ImVec4 json_to_ImVec4(const nlohmann::json& jsonVec) {
    return ImVec4(jsonVec["x"], jsonVec["y"], jsonVec["z"], jsonVec["w"]);
}

nlohmann::json SplitObject_to_json(const SplitObject& split) {
    return nlohmann::json{
        {"splitType", split.splitType},
        {"splitID", split.splitID},
        {"splitName", split.splitName},
        {"splitImage", split.splitImage},
        {"splitTint", ImVec4_to_json(split.splitTint)},
        {"splitTimeCurrent", split.splitTimeCurrent},
        {"splitTimeBest", split.splitTimeBest},
        {"splitTimePreviousBest", split.splitTimePreviousBest},
        {"splitTimeStatus", SPLIT_INACTIVE},
        {"splitSkullTokenCount", split.splitSkullTokenCount}
    };
}

SplitObject json_to_SplitObject(const nlohmann::json& jsonSplit) {
    SplitObject split;
    split.splitType = jsonSplit["splitType"];
    split.splitID = jsonSplit["splitID"];
    split.splitName = jsonSplit["splitName"].get<std::string>();
    split.splitImage = jsonSplit["splitImage"].get<std::string>();
    split.splitTint = json_to_ImVec4(jsonSplit["splitTint"]);
    split.splitTimeCurrent = jsonSplit["splitTimeCurrent"];
    split.splitTimeBest = jsonSplit["splitTimeBest"];
    split.splitTimePreviousBest = jsonSplit["splitTimePreviousBest"];
    split.splitTimeStatus = jsonSplit["splitTimeStatus"];
    split.splitSkullTokenCount = jsonSplit["splitSkullTokenCount"];
    return split;
}

void TimeSplitsGetImageSize(uint32_t item) {
    if (item >= ITEM_SONG_MINUET && item <= ITEM_SONG_STORMS) {
        imageSize = ImVec2(30.0f, 38.0f);
        imagePadding = 6.0f;
    } else {
        imageSize = ImVec2(38.0f, 38.0f);
        imagePadding = 2.0f;
    }
}

void TimeSplitsUpdateSplitStatus() {
    uint32_t index = 0;
    for (auto& data : splitList) {
        if (data.splitTimeStatus == SPLIT_INACTIVE || data.splitTimeStatus == SPLIT_ACTIVE) {
            data.splitTimeStatus = SPLIT_ACTIVE;
            break;
        }
        index++;
    }
    for (int i = index; i < splitList.size(); i++) {
        if (splitList[i].splitTimeStatus != SPLIT_ACTIVE && splitList[i].splitTimeStatus != SPLIT_COLLECTED) {
            splitList[i].splitTimeStatus = SPLIT_INACTIVE;
        }
    }
}

void HandleDragAndDrop(std::vector<SplitObject>& objectList, int targetIndex, const std::string& itemName, ImGuiDragDropFlags flags = ImGuiDragDropFlags_None) {
    if (ImGui::BeginDragDropSource(flags)) {
        ImGui::SetDragDropPayload("DragMove", &targetIndex, sizeof(uint32_t));
        ImGui::Text("Move %s", itemName.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragMove")) {
            IM_ASSERT(payload->DataSize == sizeof(uint32_t));
            dragSourceIndex = *(const int*)payload->Data;
            dragTargetIndex = targetIndex;
        }
        ImGui::EndDragDropTarget();
    }
}

void TimeSplitCompleteSplits() {
    gSaveContext.sohStats.itemTimestamp[TIMESTAMP_DEFEAT_GANON] = GAMEPLAYSTAT_TOTAL_TIME;
    gSaveContext.sohStats.gameComplete = true;
}

void TimeSplitsSkipSplit(uint32_t index) {
    splitList[index].splitTimeStatus = SPLIT_SKIPPED;
    if (index + 1 == splitList.size()) {
        TimeSplitCompleteSplits();
    } else {
        TimeSplitsUpdateSplitStatus();
    }
}

void TimeSplitsFileManagement(uint32_t action, const char* listEntry, std::vector<SplitObject> listData) {
    std::string filename = "timesplitdata.json";
    json saveFile;
    json listArray = nlohmann::json::array();

    std::ifstream inputFile(filename);
    if (inputFile.is_open()) {
        inputFile >> saveFile;
        inputFile.close();
    }

    if (action == ACTION_SAVE) {
        for (auto& data : listData) {
            listArray.push_back(SplitObject_to_json(data));
        }
        saveFile[listEntry] = listArray;

        // Update Save File on Disk
        std::ofstream outputFile(filename);
        if (outputFile.is_open()) {
            outputFile << saveFile.dump(4);
            outputFile.close();
        }
    }

    if (action == ACTION_LOAD) {
        if (saveFile.contains(listEntry)) {
            listArray = saveFile[listEntry];
            splitList.clear();

            for (auto& data : listArray) {
                splitList.push_back(json_to_SplitObject(data));
            }
            splitList[0].splitTimeStatus = SPLIT_ACTIVE;
        }
    }

    if (action == ACTION_UPDATE) {
        for (auto& update : listData) {
            if (update.splitTimeBest < update.splitTimePreviousBest) {
                update.splitTimePreviousBest = update.splitTimeBest;
            }
        }
    }

    if (action == ACTION_COLLECT) {
        keys.clear();
        for (auto& data : saveFile.items()) {
            keys.push_back(data.key());
        }
        if (keys.size() == 0) {
            keys.push_back("No Saved Lists");
        }
    }

    if (action == ACTION_DELETE) {
        if (saveFile.contains(listEntry)) {
            saveFile.erase(listEntry);

            std::ofstream outputFile(filename);
            if (outputFile.is_open()) {
                outputFile << saveFile.dump(4);
                outputFile.close();
            }
        }
    }
}

void TimeSplitsPopUpContext() {
    if ((popupID != -1) && ImGui::BeginPopup("TimeSplitsPopUp")) {
        if (popupID == ITEM_SKULL_TOKEN) {
            ImGui::BeginTable("Token Table", 2);
            ImGui::TableNextColumn();
            ImGui::ImageButton(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("QUEST_SKULL_TOKEN"),
                    ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1), 2.0f, ImVec4(0, 0, 0, 0));
            ImGui::TableNextColumn();
            ImGui::PushItemWidth(150.0f);
            ImGui::SliderInt("##count", &skullTokenCount, 0, 100, "%d Tokens");
            ImGui::PopItemWidth();
            if (ImGui::Button("Set Tokens")) {
                auto findID = std::find_if(splitObjectList.begin(), splitObjectList.end(), [&](const SplitObject& obj) { return obj.splitID == ITEM_SKULL_TOKEN; });
                SplitObject& buildTokenObject = *findID;
                std::string tokenStr = " (" + std::to_string(skullTokenCount) + ")";
                buildTokenObject.splitName += tokenStr.c_str();
                buildTokenObject.splitSkullTokenCount = skullTokenCount;

                splitList.push_back(buildTokenObject);
                TimeSplitsUpdateSplitStatus();
                ImGui::CloseCurrentPopup();
                popupID = -1;
            }
            ImGui::EndTable();
        }  else {
            int rowIndex = 0;
            for (auto item : popupList[popupID]) {
                auto findID = std::find_if(splitObjectList.begin(), splitObjectList.end(), [&](const SplitObject& obj) { return obj.splitID == item; });
                if (findID == splitObjectList.end()) {
                    continue;
                }

                SplitObject& popupObject = *findID;
                ImGui::BeginGroup();
                ImGui::PushID(popupObject.splitID);
                if (ImGui::ImageButton(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(popupObject.splitImage),
                ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1), 2, ImVec4(0, 0, 0, 0), popupObject.splitTint)) {
                    splitList.push_back(popupObject);
                    if (splitList.size() == 1) {
                        splitList[0].splitTimeStatus = SPLIT_ACTIVE;
                    } else {
                        splitList[splitList.size() - 1].splitTimeStatus = SPLIT_INACTIVE;
                    }
                    ImGui::CloseCurrentPopup();
                    popupID = -1;
                }
                ImGui::PopID();

                if (popupObject.splitType == SPLIT_UPGRADE) {
                    if (popupID <= ITEM_SLINGSHOT && popupID != -1) {
                        ImVec2 imageMin = ImGui::GetItemRectMin();
                        ImVec2 imageMax = ImGui::GetItemRectMax();
                        ImVec2 imageSize = ImVec2(imageMax.x - imageMin.x, imageMax.y - imageMin.y);
                        ImVec2 textPos = ImVec2(imageMax.x - ImGui::CalcTextSize("00").x - 5,
                                                imageMax.y - ImGui::CalcTextSize("00").y - 5);

                        ImGui::SetCursorScreenPos(textPos);
                        std::string upgSubstr = popupObject.splitName.substr(popupObject.splitName.size() - 4);
                        std::string upgOutput = removeSpecialCharacters(upgSubstr);
                        ImGui::Text(upgOutput.c_str());
                    }
                }
                ImGui::EndGroup();
                if (rowIndex != 5) {
                    ImGui::SameLine();
                }
                rowIndex++;
            }
        }
        ImGui::EndPopup();
    }
}

void TimeSplitsPostDragAndDrop() {
    if (dragTargetIndex != -1) {
        SplitObject tempSourceSplitObject = splitList[dragSourceIndex];
        if (tempSourceSplitObject.splitTimeStatus == SPLIT_ACTIVE) {
            tempSourceSplitObject.splitTimeStatus = SPLIT_INACTIVE;
        }
        if (splitList[dragTargetIndex].splitTimeStatus == SPLIT_ACTIVE) {
            splitList[dragTargetIndex].splitTimeStatus = SPLIT_INACTIVE;
        }

        splitList.erase(splitList.begin() + dragSourceIndex);
        splitList.insert(splitList.begin() + dragTargetIndex, tempSourceSplitObject);
        dragTargetIndex = -1;
        dragSourceIndex = -1;

        TimeSplitsUpdateSplitStatus();
    }
}

void TimeSplitsItemSplitEvent(uint32_t type, u8 item) {
    uint32_t index = 0;
    if (type <= SPLIT_QUEST) {
        if (item == ITEM_NUTS_5 || item == ITEM_NUTS_10) {
            item = ITEM_NUT;
        } else if (item == ITEM_STICKS_5 || item == ITEM_STICKS_10) {
            item = ITEM_STICK;
        }
        if (item == ITEM_SKULL_TOKEN) {
            auto it = std::find_if(splitList.begin(), splitList.end(), [item](const SplitObject& split) {
                if (split.splitSkullTokenCount == gSaveContext.inventory.gsTokens) {
                    return split.splitID == item;
                } else {
                    return split.splitID == ITEM_NONE;
                }
            });
            if (it == splitList.end()) {
                return;
            }
        }
    }
    if (type == SPLIT_ENTRANCE) {
        if ((item == SCENE_ZORAS_RIVER && gSaveContext.entranceIndex == ENTR_ZORAS_RIVER_4) ||
            (item == SCENE_LOST_WOODS &&
             (gSaveContext.entranceIndex == ENTR_LOST_WOODS_9 || gSaveContext.entranceIndex == ENTR_LOST_WOODS_0))) {
            type = SPLIT_MISC;
        }
    }
        
    for (auto& split : splitList) {
        if (split.splitType == type) {
            if (item == split.splitID) {
                if (split.splitTimeStatus == SPLIT_ACTIVE) {
                    split.splitTimeCurrent = GAMEPLAYSTAT_TOTAL_TIME;
                    split.splitTimeStatus = SPLIT_COLLECTED;
                    if (split.splitTimeBest > GAMEPLAYSTAT_TOTAL_TIME || split.splitTimeBest == 0) {
                        split.splitTimeBest = GAMEPLAYSTAT_TOTAL_TIME;
                    }
                    if (split.splitTimePreviousBest == 0) {
                        split.splitTimePreviousBest = GAMEPLAYSTAT_TOTAL_TIME;
                    }
                    if (index == splitList.size() - 1) {
                        TimeSplitCompleteSplits();
                    } else {
                        splitList[index + 1].splitTimeStatus = SPLIT_ACTIVE;
                    }
                }
            }
        }
        index++;
    }
}

void TimeSplitsSplitBestTimeDisplay(SplitObject split) {
    activeSplitHighlight = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    if (split.splitTimeStatus == SPLIT_ACTIVE) {
        if (GAMEPLAYSTAT_TOTAL_TIME > split.splitTimePreviousBest) {
            splitTimeColor = COLOR_RED;
            splitBestTimeDisplay = (GAMEPLAYSTAT_TOTAL_TIME - split.splitTimePreviousBest);
        }
        if (GAMEPLAYSTAT_TOTAL_TIME == split.splitTimePreviousBest) {
            splitTimeColor = COLOR_WHITE;
            splitBestTimeDisplay = GAMEPLAYSTAT_TOTAL_TIME;
        }
        if (GAMEPLAYSTAT_TOTAL_TIME < split.splitTimePreviousBest) {
            splitTimeColor = COLOR_GREEN;
            splitBestTimeDisplay = (split.splitTimePreviousBest - GAMEPLAYSTAT_TOTAL_TIME);
        }
        activeSplitHighlight = COLOR_LIGHT_BLUE;
    }
    if (split.splitTimeStatus == SPLIT_INACTIVE) {
        splitTimeColor = COLOR_WHITE;
        splitBestTimeDisplay = split.splitTimeBest;
    }
    if (split.splitTimeStatus == SPLIT_COLLECTED) {
        if (split.splitTimeCurrent > split.splitTimePreviousBest) {
            splitTimeColor = COLOR_RED;
            splitBestTimeDisplay = (split.splitTimeCurrent - split.splitTimePreviousBest);
        }
        if (split.splitTimeCurrent == split.splitTimePreviousBest) {
            splitTimeColor = COLOR_WHITE;
            splitBestTimeDisplay = split.splitTimeCurrent;
        }
        if (split.splitTimeCurrent < split.splitTimePreviousBest) {
            splitTimeColor = COLOR_GREEN;
            splitBestTimeDisplay = (split.splitTimePreviousBest - split.splitTimeCurrent);
        }
    }
}

void TimeSplitsDrawSplitsList() {
    uint32_t dragIndex = 0;
    ImGui::BeginChild("SplitTable", ImVec2(0.0f, ImGui::GetWindowHeight() - 128.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 0));
    ImGui::BeginTable("Splits", 5, ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable);
    ImGui::TableSetupColumn("Item Image", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, 34.0f);
    ImGui::TableSetupColumn("Item Name");
    ImGui::TableSetupColumn("Current Time");
    ImGui::TableSetupColumn("+/-");
    ImGui::TableSetupColumn("Prev. Best");
    ImGui::TableHeadersRow();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

    for (auto& split : splitList) {
        ImGui::TableNextColumn();
        TimeSplitsSplitBestTimeDisplay(split);
        
        ImGui::PushID(split.splitID);
        if (split.splitTimeStatus == SPLIT_ACTIVE) {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(47, 79, 90, 255));
        }
        TimeSplitsGetImageSize(split.splitID);
        if (ImGui::ImageButton(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(split.splitImage),
                               imageSize, ImVec2(0, 0), ImVec2(1, 1), imagePadding, ImVec4(0, 0, 0, 0), split.splitTint)) {
            TimeSplitsSkipSplit(dragIndex);
        }
        HandleDragAndDrop(splitList, dragIndex, split.splitName);
        ImGui::TableNextColumn();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
        ImGui::AlignTextToFramePadding();
        ImGui::TextWrapped(split.splitName.c_str());
        ImGui::TableNextColumn();
        // Current Time
        ImGui::Text((split.splitTimeStatus == SPLIT_ACTIVE) 
            ? formatTimestampTimeSplit(GAMEPLAYSTAT_TOTAL_TIME).c_str() : (split.splitTimeStatus == SPLIT_COLLECTED) 
            ? formatTimestampTimeSplit(split.splitTimeCurrent).c_str() : "--:--:-");
        ImGui::TableNextColumn();
        // +/- Difference
        ImGui::TextColored(splitTimeColor, formatTimestampTimeSplit(splitBestTimeDisplay).c_str());
        ImGui::TableNextColumn();
        // Previous Best
        ImGui::Text((split.splitTimePreviousBest != 0) ? formatTimestampTimeSplit(split.splitTimePreviousBest).c_str() : "--:--:-");
        ImGui::PopID();
        ImGui::PopStyleVar(1);

        dragIndex++;
    }

    TimeSplitsPostDragAndDrop();

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(1);
    ImGui::EndTable();
    ImGui::EndChild();
}

void TimeSplitsGetTableSize(uint32_t type) {
    switch (type) {
        case SPLIT_ITEM:
        case SPLIT_QUEST:
            tableSize = 6;
            break;
        case SPLIT_EQUIPMENT:
            tableSize = 3;
            break;
        default:
            tableSize = 2;
            break;
    }
}

void TimeSplitsDrawItemList(uint32_t type) {
    TimeSplitsGetTableSize(type);

    ImGui::BeginChild("Item Child");
    ImGui::BeginTable("Item List", tableSize);
    for (int i = 0; i < tableSize; i++) {
        if (i == 0) {
            ImGui::TableSetupColumn("Item Image", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, 39.0f);
        } else {
            if (type > SPLIT_QUEST) {
                ImGui::TableSetupColumn("Item Name");
            } else {
                ImGui::TableSetupColumn(std::to_string(i).c_str(), ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, 39.0f);
            }
        }
    }

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

    for (auto& split : splitObjectList) {
        if (split.splitType == type) {
            ImGui::TableNextColumn();
            ImGui::PushID(split.splitID);
            TimeSplitsGetImageSize(split.splitID);
            if (ImGui::ImageButton(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(split.splitImage),
                               imageSize, ImVec2(0, 0), ImVec2(1, 1), imagePadding, ImVec4(0, 0, 0, 0), split.splitTint)) {
                
                if (popupList.contains(split.splitID) && (split.splitType < SPLIT_BOSS)) {
                    popupID = split.splitID;
                    ImGui::OpenPopup("TimeSplitsPopUp");
                } else {
                    splitList.push_back(split);
                    

                    if (splitList.size() == 1) {
                        splitList[0].splitTimeStatus = SPLIT_ACTIVE;
                    } else {
                        splitList[splitList.size() - 1].splitTimeStatus = SPLIT_INACTIVE;
                    }
                }
            }

            TimeSplitsPopUpContext();
            ImGui::PopID();
            
            if (type > SPLIT_QUEST) {
                ImGui::TableNextColumn();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 7.0f));
                ImGui::AlignTextToFramePadding();
                ImGui::Text(split.splitName.c_str());
                ImGui::PopStyleVar(1);
            }
            
        }
    }
    ImGui::PopStyleColor(3);
    ImGui::EndTable();
    ImGui::EndChild();
}

void TimeSplitsUpdateWindowSize() {
    timeSplitsWindowSize = CVarGetFloat(CVAR_ENHANCEMENT("TimeSplits.WindowSize"), 0);
    if (timeSplitsWindowSize < 1.0f) {
        timeSplitsWindowSize = 1.0f;
    }
}

void TimeSplitsDrawOptionsMenu() {
    ImGui::SeparatorText("Window Options");
    if (ImGui::ColorEdit4("Background Color", (float*)&windowColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        Color_RGBA8 color;
        color.r = windowColor.x;
        color.g = windowColor.y;
        color.b = windowColor.z;
        color.a = windowColor.w;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        windowColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    if (UIWidgets::PaddedEnhancementSliderFloat("Window Size: %.1fx", "##windowSize",
        CVAR_ENHANCEMENT("TimeSplits.WindowSize"), 1.0f, 3.0f, "", 1.0f, false, true, true, false)) {
        TimeSplitsUpdateWindowSize();
    }

    ImGui::SeparatorText("Split List Management");

    ImGui::Text("New List Name: ");
    ImGui::PushItemWidth(150.0f);
    ImGui::InputText("##listName", listNameBuf, 25);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Create List")) {
        TimeSplitsFileManagement(ACTION_SAVE, listNameBuf, splitList);
    }
    UIWidgets::PaddedSeparator();

    TimeSplitsFileManagement(ACTION_COLLECT, "", emptyList);
    static uint32_t selectedItem = 0;
    static std::string listItem = keys[0];
    ImGui::Text("Select List to Load: ");
    ImGui::PushItemWidth(150.0f);
    if (ImGui::BeginCombo("##listEntries", keys[selectedItem].c_str())) {
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
        TimeSplitsFileManagement(ACTION_LOAD, keys[selectedItem].c_str(), emptyList);
    }
    ImGui::SameLine();
    if (ImGui::Button("Save List")) {
        TimeSplitsFileManagement(ACTION_SAVE, keys[selectedItem].c_str(), splitList);
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete List")) {
        TimeSplitsFileManagement(ACTION_DELETE, keys[selectedItem].c_str(), emptyList);
    }
    UIWidgets::PaddedSeparator();

    if (ImGui::Button("New Attempt")) {
        for (auto& data : splitList) {
            data.splitTimeStatus = SPLIT_INACTIVE;
        }
        splitList[0].splitTimeStatus = SPLIT_ACTIVE;
    }
    ImGui::SameLine();
    if (ImGui::Button("Update Splits")) {
        TimeSplitsFileManagement(ACTION_UPDATE, keys[selectedItem].c_str(), splitList);
    }
}

void TimeSplitsRemoveSplitEntry(uint32_t index) {
    if (removeIndex != -1) {
        splitList.erase(splitList.begin() + index);
        removeIndex = -1;
    }
}

void TimeSplitsDrawManageList() {
    uint32_t index = 0;
    ImGui::BeginTable("List Management", 2, ImGuiTableFlags_BordersInnerV);
    ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed, 60.0f);
    ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_NoHeaderLabel);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

    ImGui::TableNextColumn();
    ImGui::BeginTabBar("List Preview");
    if (ImGui::BeginTabItem("Preview")) {
        ImGui::BeginChild("PreviewChild");
        for (auto& data : splitList) {
            float availableWidth = ImGui::GetContentRegionAvail().x;
            float imageWidth = 38.0f; // Width of your image button
            float offsetX = (availableWidth - imageWidth) * 0.5f; // Centering offset

            if (offsetX > 0.0f) {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX); // Apply the offset to center
            }
            TimeSplitsGetImageSize(data.splitID);
            if (ImGui::ImageButton(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(data.splitImage),
                                   imageSize, ImVec2(0, 0), ImVec2(1, 1), imagePadding, ImVec4(0, 0, 0, 0), data.splitTint)) {
                removeIndex = index;
            }
            HandleDragAndDrop(splitList, index, splitList[index].splitName);
            index++;
        }
        TimeSplitsRemoveSplitEntry(removeIndex);
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    
    ImGui::PopStyleColor(3);
    ImGui::TableNextColumn();
    ImGui::BeginTabBar("List Options");
    if (ImGui::BeginTabItem("Equipment")) {
        TimeSplitsDrawItemList(SPLIT_EQUIPMENT);
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Inventory")) {
        TimeSplitsDrawItemList(SPLIT_ITEM);
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Quest")) {
        TimeSplitsDrawItemList(SPLIT_QUEST);
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Entrances")) {
        TimeSplitsDrawItemList(SPLIT_ENTRANCE);
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Bosses")) {
        TimeSplitsDrawItemList(SPLIT_BOSS);
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Miscellaneous")) {
        TimeSplitsDrawItemList(SPLIT_MISC);
        ImGui::EndTabItem();
    }

    TimeSplitsPostDragAndDrop();

    ImGui::EndTabBar();
    ImGui::EndTable();
}

void InitializeSplitDataFile() {
    if (!std::filesystem::exists("timesplitdata.json")) {
        json j;
        std::ofstream file("timesplitdata.json");
        file << j.dump(4);
        file.close();
    }
}

void TimeSplitWindow::Draw() {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, windowColor);
    GuiWindow::Draw();
    ImGui::PopStyleColor();
}

static bool initialized = false;

void TimeSplitWindow::DrawElement() {
    ImGui::SetWindowFontScale(timeSplitsWindowSize);
    if (!initialized) {
        InitializeSplitDataFile();
        initialized = true;
    }
    
    if (ImGui::BeginTabBar("Split Tabs")) {
        if (ImGui::BeginTabItem("Splits")) {
            TimeSplitsDrawSplitsList();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Manage List")) {
            TimeSplitsDrawManageList();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Options")) {
            TimeSplitsDrawOptionsMenu();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void TimeSplitWindow::InitElement() {
    TimeSplitsUpdateWindowSize();

    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("SPECIAL_TRIFORCE_PIECE_WHITE", gWTriforcePieceTex, ImVec4(1, 1, 1, 1));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("SPECIAL_SPLIT_ENTRANCE", gSplitEntranceTex, ImVec4(1, 1, 1, 1));
    
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnTimestamp>([](u8 item) {
        if (item != ITEM_SKULL_TOKEN) {
            uint32_t tempType = SPLIT_ITEM;
            for (auto& data : splitList) {
                if (data.splitID == item) {
                    tempType = data.splitType;
                    break;
                }
            }
            TimeSplitsItemSplitEvent(tempType, item);
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
        GetItemEntry testItem = itemEntry;
        if (itemEntry.itemId == ITEM_SKULL_TOKEN || itemEntry.itemId == ITEM_BOTTLE || itemEntry.itemId == ITEM_POE 
            || itemEntry.itemId == ITEM_BIG_POE) {
            uint32_t tempType = SPLIT_ITEM;
            for (auto& data : splitList) {
                if (data.splitID == itemEntry.itemId) {
                    tempType = data.splitType;
                    break;
                }
            }
            TimeSplitsItemSplitEvent(tempType, itemEntry.itemId);
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerBottleUpdate>([](int16_t contents) {
        TimeSplitsItemSplitEvent(SPLIT_UPGRADE, contents);
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnBossDefeat>([](void* refActor) {
        Actor* bossActor = (Actor*)refActor;
        TimeSplitsItemSplitEvent(SPLIT_BOSS, bossActor->id);
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) {
        if (gPlayState->sceneNum != SCENE_KAKARIKO_VILLAGE) {
            TimeSplitsItemSplitEvent(SPLIT_ENTRANCE, sceneNum);
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerHealthChange>([](int16_t amount) {
        if (gPlayState->sceneNum == SCENE_KAKARIKO_VILLAGE) {
            Player* player = GET_PLAYER(gPlayState);
            if (player->fallDistance > 500 && gSaveContext.health <= 0) {
                TimeSplitsItemSplitEvent(SPLIT_MISC, gPlayState->sceneNum);
            }
        }
    });
}