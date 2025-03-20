#include "Plandomizer.h"
#include <soh/SohGui/SohGui.hpp>
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/util.h"
#include <vector>
#include "soh/Notification/Notification.h"
#include <soh_assets.h>
#include "soh/Enhancements/randomizer/3drando/hints.hpp"

#include <fstream>
#include <filesystem>

#include "soh/OTRGlobals.h"
#include "soh/SohGui/ImGuiUtils.h"
#include "soh/Enhancements/randomizer/logic.h"
#include "soh/Enhancements/randomizer/randomizer_check_objects.h"
#include "soh/Enhancements/randomizer/rando_hash.h"
#include "soh/Enhancements/randomizer/3drando/shops.hpp"

extern "C" {
    #include "include/z64item.h"
    #include "objects/gameplay_keep/gameplay_keep.h"
    extern SaveContext gSaveContext;
    extern PlayState* gPlayState;
}

const std::string randomizeButton = ICON_FA_RANDOM;

static int32_t correctedItemID = -1;
static int32_t getTabID = TAB_HINTS;

Rando::Item temporaryItem;
std::string shortName = "";
std::string logTemp = "";
std::string lastLoadedSpoiler = "";
int32_t temporaryItemIndex = -1;
RandomizerCheckArea selectedArea = RCAREA_INVALID; 

ImVec4 itemColor = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
ImTextureID textureID;
ImVec2 imageSize = ImVec2(32.0f, 32.0f);
float imagePadding = 2.0f;
ImVec2 textureUV0 = ImVec2( 0, 0 );
ImVec2 textureUV1 = ImVec2( 1, 1 );

bool shouldPopup = false;
bool shouldTrapPopup = false;
bool shouldRemove = false;

namespace fs = std::filesystem;
std::vector<std::string> existingSeedList;

std::vector<int32_t> spoilerHash;
std::vector<int32_t> plandoHash;
std::vector<SpoilerCheckObject> spoilerLogData;
std::vector<SpoilerCheckObject> plandoLogData;
std::vector<std::pair<Rando::Item, int32_t>> drawnItemsList;

std::vector<SpoilerHintObject> spoilerHintData;
std::vector<SpoilerHintObject> plandoHintData;

extern std::map<RandomizerCheckArea, std::string> rcAreaNames;

std::unordered_map<RandomizerGet, std::string> bossKeyShortNames = {
    { RG_FOREST_TEMPLE_BOSS_KEY,     "Frst" },
    { RG_FIRE_TEMPLE_BOSS_KEY,       "Fire" },
    { RG_WATER_TEMPLE_BOSS_KEY,      "Watr" },
    { RG_SPIRIT_TEMPLE_BOSS_KEY,     "Sprt" },
    { RG_SHADOW_TEMPLE_BOSS_KEY,     "Shdw" },
    { RG_GANONS_CASTLE_BOSS_KEY,     "Ganon" },
};

std::unordered_map<RandomizerGet, std::string> ocarinaButtonNames = {
    { RG_OCARINA_A_BUTTON,          "A" },
    { RG_OCARINA_C_UP_BUTTON,       "C-UP" },
    { RG_OCARINA_C_DOWN_BUTTON,     "C-DWN" },
    { RG_OCARINA_C_LEFT_BUTTON,     "C-LFT" },
    { RG_OCARINA_C_RIGHT_BUTTON,    "C-RHT" },
};

std::map<RandomizerGet, ImVec4> bossSoulColorMapping = {
    { RG_GOHMA_SOUL,          { 0.00f, 1.00f, 0.00f, 1.0f } },
    { RG_KING_DODONGO_SOUL,   { 1.00f, 0.00f, 0.39f, 1.0f } },
    { RG_BARINADE_SOUL,       { 0.20f, 1.00f, 1.00f, 1.0f } },
    { RG_PHANTOM_GANON_SOUL,  { 0.02f, 0.76f, 0.18f, 1.0f } },
    { RG_VOLVAGIA_SOUL,       { 0.93f, 0.37f, 0.37f, 1.0f } },
    { RG_MORPHA_SOUL,         { 0.33f, 0.71f, 0.87f, 1.0f } },
    { RG_BONGO_BONGO_SOUL,    { 0.49f, 0.06f, 0.69f, 1.0f } },
    { RG_TWINROVA_SOUL,       { 0.87f, 0.62f, 0.18f, 1.0f } },
    { RG_GANON_SOUL,          { 0.31f, 0.31f, 0.31f, 1.0f } }
};


std::vector<RandomizerGet> infiniteItemList = {
    RG_GREEN_RUPEE, RG_BLUE_RUPEE, RG_RED_RUPEE, RG_PURPLE_RUPEE, RG_HUGE_RUPEE,
    RG_ARROWS_5, RG_ARROWS_10, RG_ARROWS_30,
    RG_DEKU_STICK_1, RG_DEKU_SEEDS_30, RG_DEKU_NUTS_5, RG_DEKU_NUTS_10,
    RG_BOMBS_5, RG_BOMBS_10, RG_BOMBS_20, RG_BOMBCHU_5, RG_BOMBCHU_10, RG_BOMBCHU_20,
    RG_RECOVERY_HEART, RG_ICE_TRAP, RG_SOLD_OUT
};

std::unordered_map<RandomizerGet, std::string> itemImageMap = {
    { RG_NONE,                      		"ITEM_SOLD_OUT" },
    { RG_KOKIRI_SWORD,              		"ITEM_SWORD_KOKIRI" },
    { RG_GIANTS_KNIFE,              		"ITEM_SWORD_KNIFE" },
    { RG_BIGGORON_SWORD,            		"ITEM_SWORD_BGS" },
    { RG_DEKU_SHIELD,               		"ITEM_SHIELD_DEKU" },
    { RG_HYLIAN_SHIELD,             		"ITEM_SHIELD_HYLIAN" },
    { RG_MIRROR_SHIELD,             		"ITEM_SHIELD_MIRROR" },
    { RG_GORON_TUNIC,               		"ITEM_TUNIC_GORON" },
    { RG_ZORA_TUNIC,                		"ITEM_TUNIC_ZORA" },
    { RG_IRON_BOOTS,                		"ITEM_BOOTS_IRON" },
    { RG_HOVER_BOOTS,               		"ITEM_BOOTS_HOVER" },
    { RG_BOOMERANG,                 		"ITEM_BOOMERANG" },
    { RG_LENS_OF_TRUTH,             		"ITEM_LENS" },
    { RG_MEGATON_HAMMER,            		"ITEM_HAMMER" },
    { RG_STONE_OF_AGONY,            		"ITEM_STONE_OF_AGONY" },
    { RG_DINS_FIRE,                 		"ITEM_DINS_FIRE" },
    { RG_FARORES_WIND,              		"ITEM_FARORES_WIND" },
    { RG_NAYRUS_LOVE,               		"ITEM_NAYRUS_LOVE" },
    { RG_FIRE_ARROWS,               		"ITEM_ARROW_FIRE" },
    { RG_ICE_ARROWS,                		"ITEM_ARROW_ICE" },
    { RG_LIGHT_ARROWS,              		"ITEM_ARROW_LIGHT" },
    { RG_GERUDO_MEMBERSHIP_CARD,    		"ITEM_GERUDO_CARD" },
    { RG_MAGIC_BEAN,                		"ITEM_BEAN" },
    { RG_MAGIC_BEAN_PACK,           		"ITEM_BEAN" },
    { RG_DOUBLE_DEFENSE,            		"ITEM_HEART_CONTAINER" },
    { RG_WEIRD_EGG,                 		"ITEM_WEIRD_EGG" },
    { RG_ZELDAS_LETTER,             		"ITEM_LETTER_ZELDA" },
    { RG_POCKET_EGG,                		"ITEM_POCKET_EGG" },
    { RG_COJIRO,                    		"ITEM_COJIRO" },
    { RG_ODD_MUSHROOM,              		"ITEM_ODD_MUSHROOM" },
    { RG_ODD_POTION,                		"ITEM_ODD_POTION" },
    { RG_POACHERS_SAW,              		"ITEM_SAW" },
    { RG_BROKEN_SWORD,              		"ITEM_SWORD_BROKEN" },
    { RG_PRESCRIPTION,              		"ITEM_PRESCRIPTION" },
    { RG_EYEBALL_FROG,              		"ITEM_FROG" },
    { RG_EYEDROPS,                  		"ITEM_EYEDROPS" },
    { RG_CLAIM_CHECK,               		"ITEM_CLAIM_CHECK" },
    { RG_GOLD_SKULLTULA_TOKEN,      		"ITEM_SKULL_TOKEN" },
    { RG_PROGRESSIVE_HOOKSHOT,      		"ITEM_HOOKSHOT" },
    { RG_PROGRESSIVE_STRENGTH,      		"ITEM_BRACELET" },
    { RG_PROGRESSIVE_BOMB_BAG,      		"ITEM_BOMB_BAG_30" },
    { RG_PROGRESSIVE_BOW,           		"ITEM_QUIVER_30" },
    { RG_PROGRESSIVE_SLINGSHOT,     		"ITEM_SLINGSHOT" },
    { RG_PROGRESSIVE_WALLET,        		"ITEM_WALLET_ADULT" },
    { RG_PROGRESSIVE_SCALE,         		"ITEM_SCALE_SILVER" },
    { RG_PROGRESSIVE_NUT_UPGRADE,   		"ITEM_NUT" },
    { RG_PROGRESSIVE_STICK_UPGRADE, 		"ITEM_STICK" },
    { RG_PROGRESSIVE_BOMBCHUS,      		"ITEM_BOMBCHU" },
    { RG_PROGRESSIVE_MAGIC_METER,   		"ITEM_MAGIC_SMALL" },
    { RG_MAGIC_SINGLE,              		"ITEM_MAGIC_SMALL" },
    { RG_MAGIC_DOUBLE,              		"ITEM_MAGIC_LARGE" },
    { RG_PROGRESSIVE_OCARINA,       		"ITEM_OCARINA_FAIRY" },
    { RG_PROGRESSIVE_GORONSWORD,    		"ITEM_SWORD_BGS" },
    { RG_EMPTY_BOTTLE,              		"ITEM_BOTTLE" },
    { RG_BOTTLE_WITH_MILK,          		"ITEM_MILK_BOTTLE" },
    { RG_BOTTLE_WITH_RED_POTION,    		"ITEM_POTION_RED" },
    { RG_BOTTLE_WITH_GREEN_POTION,  		"ITEM_POTION_GREEN" },
    { RG_BOTTLE_WITH_BLUE_POTION,   		"ITEM_POTION_BLUE" },
    { RG_BOTTLE_WITH_FAIRY,         		"ITEM_FAIRY" },
    { RG_BOTTLE_WITH_FISH,          		"ITEM_FISH" },
    { RG_BOTTLE_WITH_BLUE_FIRE,     		"ITEM_BLUE_FIRE" },
    { RG_BOTTLE_WITH_BUGS,          		"ITEM_BUG" },
    { RG_BOTTLE_WITH_POE,           		"ITEM_POE" },
    { RG_RUTOS_LETTER,              		"ITEM_LETTER_RUTO" },
    { RG_BOTTLE_WITH_BIG_POE,       		"ITEM_BIG_POE" },
    { RG_ZELDAS_LULLABY,            		"ITEM_SONG_LULLABY" },
    { RG_EPONAS_SONG,               		"ITEM_SONG_EPONA" },
    { RG_SARIAS_SONG,               		"ITEM_SONG_SARIA" },
    { RG_SUNS_SONG,                 		"ITEM_SONG_SUN" },
    { RG_SONG_OF_TIME,              		"ITEM_SONG_TIME" },
    { RG_SONG_OF_STORMS,            		"ITEM_SONG_STORMS" },
    { RG_MINUET_OF_FOREST,          		"ITEM_SONG_MINUET" },
    { RG_BOLERO_OF_FIRE,            		"ITEM_SONG_BOLERO" },
    { RG_SERENADE_OF_WATER,         		"ITEM_SONG_SERENADE" },
    { RG_REQUIEM_OF_SPIRIT,         		"ITEM_SONG_REQUIEM" },
    { RG_NOCTURNE_OF_SHADOW,        		"ITEM_SONG_NOCTURNE" },
    { RG_PRELUDE_OF_LIGHT,          		"ITEM_SONG_PRELUDE" },
    { RG_DEKU_TREE_MAP,             		"ITEM_DUNGEON_MAP" },
    { RG_DODONGOS_CAVERN_MAP,       		"ITEM_DUNGEON_MAP" },
    { RG_JABU_JABUS_BELLY_MAP,      		"ITEM_DUNGEON_MAP" },
    { RG_FOREST_TEMPLE_MAP,         		"ITEM_DUNGEON_MAP" },
    { RG_FIRE_TEMPLE_MAP,           		"ITEM_DUNGEON_MAP" },
    { RG_WATER_TEMPLE_MAP,          		"ITEM_DUNGEON_MAP" },
    { RG_SPIRIT_TEMPLE_MAP,         		"ITEM_DUNGEON_MAP" },
    { RG_SHADOW_TEMPLE_MAP,         		"ITEM_DUNGEON_MAP" },
    { RG_BOTTOM_OF_THE_WELL_MAP,    		"ITEM_DUNGEON_MAP" },
    { RG_ICE_CAVERN_MAP,            		"ITEM_DUNGEON_MAP" },
    { RG_DEKU_TREE_COMPASS,         		"ITEM_COMPASS" },
    { RG_DODONGOS_CAVERN_COMPASS,   		"ITEM_COMPASS" },
    { RG_JABU_JABUS_BELLY_COMPASS,  		"ITEM_COMPASS" },
    { RG_FOREST_TEMPLE_COMPASS,     		"ITEM_COMPASS" },
    { RG_FIRE_TEMPLE_COMPASS,       		"ITEM_COMPASS" },
    { RG_WATER_TEMPLE_COMPASS,      		"ITEM_COMPASS" },
    { RG_SPIRIT_TEMPLE_COMPASS,     		"ITEM_COMPASS" },
    { RG_SHADOW_TEMPLE_COMPASS,     		"ITEM_COMPASS" },
    { RG_BOTTOM_OF_THE_WELL_COMPASS,		"ITEM_COMPASS" },
    { RG_ICE_CAVERN_COMPASS,        		"ITEM_COMPASS" },
    { RG_FOREST_TEMPLE_BOSS_KEY,    		"ITEM_KEY_BOSS" },
    { RG_FIRE_TEMPLE_BOSS_KEY,      		"ITEM_KEY_BOSS" },
    { RG_WATER_TEMPLE_BOSS_KEY,     		"ITEM_KEY_BOSS" },
    { RG_SPIRIT_TEMPLE_BOSS_KEY,    		"ITEM_KEY_BOSS" },
    { RG_SHADOW_TEMPLE_BOSS_KEY,    		"ITEM_KEY_BOSS" },
    { RG_GANONS_CASTLE_BOSS_KEY,    		"ITEM_KEY_BOSS" },
    { RG_FOREST_TEMPLE_SMALL_KEY,   		"ITEM_KEY_SMALL" },
    { RG_FIRE_TEMPLE_SMALL_KEY,     		"ITEM_KEY_SMALL" },
    { RG_WATER_TEMPLE_SMALL_KEY,    		"ITEM_KEY_SMALL" },
    { RG_SPIRIT_TEMPLE_SMALL_KEY,   		"ITEM_KEY_SMALL" },
    { RG_SHADOW_TEMPLE_SMALL_KEY,   		"ITEM_KEY_SMALL" },
    { RG_BOTTOM_OF_THE_WELL_SMALL_KEY, 		"ITEM_KEY_SMALL" },
    { RG_GERUDO_TRAINING_GROUND_SMALL_KEY, "ITEM_KEY_SMALL" },
    { RG_GERUDO_FORTRESS_SMALL_KEY, 		"ITEM_KEY_SMALL" },
    { RG_GANONS_CASTLE_SMALL_KEY,   		"ITEM_KEY_SMALL" },
    { RG_TREASURE_GAME_SMALL_KEY,   		"ITEM_KEY_SMALL" },
    { RG_KOKIRI_EMERALD,            		"ITEM_KOKIRI_EMERALD" },
    { RG_GORON_RUBY,                		"ITEM_GORON_RUBY" },
    { RG_ZORA_SAPPHIRE,             		"ITEM_ZORA_SAPPHIRE" },
    { RG_FOREST_MEDALLION,          		"ITEM_MEDALLION_FOREST" },
    { RG_FIRE_MEDALLION,            		"ITEM_MEDALLION_FIRE" },
    { RG_WATER_MEDALLION,           		"ITEM_MEDALLION_WATER" },
    { RG_SPIRIT_MEDALLION,          		"ITEM_MEDALLION_SPIRIT" },
    { RG_SHADOW_MEDALLION,          		"ITEM_MEDALLION_SHADOW" },
    { RG_LIGHT_MEDALLION,           		"ITEM_MEDALLION_LIGHT" },
    { RG_RECOVERY_HEART,            		"ITEM_HEART_GRAYSCALE" },
    { RG_GREEN_RUPEE,               		"ITEM_RUPEE_GRAYSCALE" },
    { RG_GREG_RUPEE,                		"ITEM_RUPEE_GRAYSCALE" },
    { RG_BLUE_RUPEE,                		"ITEM_RUPEE_GRAYSCALE" },
    { RG_RED_RUPEE,                 		"ITEM_RUPEE_GRAYSCALE" },
    { RG_PURPLE_RUPEE,              		"ITEM_RUPEE_GRAYSCALE" },
    { RG_HUGE_RUPEE,                		"ITEM_RUPEE_GRAYSCALE" },
    { RG_TREASURE_GAME_GREEN_RUPEE,         "ITEM_RUPEE_GRAYSCALE" },
    { RG_PIECE_OF_HEART,            		"ITEM_HEART_PIECE" },
    { RG_HEART_CONTAINER,           		"ITEM_HEART_CONTAINER" },
    { RG_ICE_TRAP,                  		"ITEM_ICE_TRAP" },
    { RG_MILK,                              "ITEM_MILK_BOTTLE"},
    { RG_BOMBS_5,                   		"ITEM_BOMB" },
    { RG_BOMBS_10,                  		"ITEM_BOMB" },
    { RG_BOMBS_20,                  		"ITEM_BOMB" },
    { RG_BUY_BOMBS_525,              		"ITEM_BOMB" },
    { RG_BUY_BOMBS_535,              		"ITEM_BOMB" },
    { RG_BUY_BOMBS_10,              		"ITEM_BOMB" },
    { RG_BUY_BOMBS_20,              		"ITEM_BOMB" },
    { RG_BUY_BOMBS_30,                      "ITEM_BOMB" },
    { RG_DEKU_NUTS_5,               		"ITEM_NUT" },
    { RG_DEKU_NUTS_10,              		"ITEM_NUT" },
    { RG_BUY_DEKU_NUTS_5,           		"ITEM_NUT" },
    { RG_BUY_DEKU_NUTS_10,               	"ITEM_NUT" },
    { RG_BOMBCHU_5,                 		"ITEM_BOMBCHU" },
    { RG_BOMBCHU_10,                		"ITEM_BOMBCHU" },
    { RG_BOMBCHU_20,                		"ITEM_BOMBCHU" },
    { RG_BUY_BOMBCHUS_20,                 	"ITEM_BOMBCHU" },
    { RG_ARROWS_5,                  		"ITEM_ARROWS_SMALL" },
    { RG_BUY_ARROWS_10,                  	"ITEM_ARROWS_SMALL" },
    { RG_ARROWS_10,                 		"ITEM_ARROWS_MEDIUM" },
    { RG_BUY_ARROWS_30,                 	"ITEM_ARROWS_MEDIUM" },
    { RG_ARROWS_30,                 		"ITEM_ARROWS_LARGE" },
    { RG_BUY_ARROWS_50,                 	"ITEM_ARROWS_LARGE" },
    { RG_TREASURE_GAME_HEART,       		"ITEM_HEART_PIECE" },
    { RG_DEKU_SEEDS_30,             		"ITEM_SEEDS" },
    { RG_BUY_DEKU_SEEDS_30,             	"ITEM_SEEDS" },
    { RG_BUY_HEART,                         "ITEM_HEART_GRAYSCALE" },
    { RG_FISHING_POLE,             	        "ITEM_FISHING_POLE" },
    { RG_SOLD_OUT,                  		"ITEM_SOLD_OUT" },
    { RG_TRIFORCE_PIECE,                  	"TRIFORCE_PIECE" },
    { RG_SKELETON_KEY,                      "ITEM_KEY_SMALL" }
};

Rando::Item plandomizerRandoRetrieveItem(RandomizerGet randoGetItem) {
    auto randoGetItemEntry = Rando::StaticData::RetrieveItem(randoGetItem);
    return randoGetItemEntry;
}

void PlandoPushImageButtonStyle(){
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
}

void PlandoPopImageButtonStyle(){
    ImGui::PopStyleColor(3);
}

ImVec4 plandomizerGetItemColor(Rando::Item randoItem) {
    itemColor = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
    if (randoItem.GetItemType() == ITEMTYPE_SMALLKEY || randoItem.GetItemType() == ITEMTYPE_FORTRESS_SMALLKEY
        || randoItem.GetItemType() == ITEMTYPE_BOSSKEY) {
        if (randoItem.GetRandomizerGet() == RG_FOREST_TEMPLE_SMALL_KEY || 
            randoItem.GetRandomizerGet() == RG_FOREST_TEMPLE_KEY_RING) {
            itemColor = ImVec4( 0.02f, 0.76f, 0.18f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_FIRE_TEMPLE_SMALL_KEY || 
            randoItem.GetRandomizerGet() == RG_FIRE_TEMPLE_KEY_RING) {
            itemColor = ImVec4( 0.93f, 0.37f, 0.37f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_WATER_TEMPLE_SMALL_KEY || 
            randoItem.GetRandomizerGet() == RG_WATER_TEMPLE_KEY_RING) {
            itemColor = ImVec4( 0.33f, 0.71f, 0.87f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_SPIRIT_TEMPLE_SMALL_KEY || 
            randoItem.GetRandomizerGet() == RG_SPIRIT_TEMPLE_KEY_RING) {
            itemColor = ImVec4( 0.87f, 0.62f, 0.18f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_SHADOW_TEMPLE_SMALL_KEY || 
            randoItem.GetRandomizerGet() == RG_SHADOW_TEMPLE_KEY_RING) {
            itemColor = ImVec4( 0.49f, 0.06f, 0.69f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_BOTTOM_OF_THE_WELL_SMALL_KEY || 
            randoItem.GetRandomizerGet() == RG_BOTTOM_OF_THE_WELL_KEY_RING) {
            itemColor = ImVec4( 0.89f, 0.43f, 1.0f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_GERUDO_TRAINING_GROUND_SMALL_KEY || 
            randoItem.GetRandomizerGet() == RG_GERUDO_TRAINING_GROUND_KEY_RING) {
            itemColor = ImVec4( 1.0f, 1.0f, 0, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_GERUDO_FORTRESS_SMALL_KEY || 
            randoItem.GetRandomizerGet() == RG_GERUDO_FORTRESS_KEY_RING) {
            itemColor = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_GANONS_CASTLE_SMALL_KEY || 
            randoItem.GetRandomizerGet() == RG_GANONS_CASTLE_KEY_RING) {
            itemColor = ImVec4( 0.5f, 0.5f, 0.5f, 1.0f );
        }
        return itemColor;
    }
    if (randoItem.GetItemType() == ITEMTYPE_SONG) {
        uint32_t questID = Rando::Logic::RandoGetToQuestItem[randoItem.GetRandomizerGet()];
        textureID = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(songMapping.at((QuestItem)questID).name);
        itemColor = songMapping.at((QuestItem)questID).color;
        imageSize = ImVec2(24.0f, 32.0f);
        imagePadding = 6.0f;
        return itemColor;
    }
    if (randoItem.GetRandomizerGet() >= RG_GREEN_RUPEE && randoItem.GetRandomizerGet() <= RG_HUGE_RUPEE) {
        if (randoItem.GetRandomizerGet() == RG_GREG_RUPEE || randoItem.GetRandomizerGet() == RG_GREEN_RUPEE 
            || randoItem.GetRandomizerGet() == RG_TREASURE_GAME_GREEN_RUPEE) {
            itemColor = ImVec4( 0.02f, 0.76f, 0.18f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_BLUE_RUPEE) {
            itemColor = ImVec4( 0.33f, 0.71f, 0.87f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_RED_RUPEE) {
            itemColor = ImVec4( 0.93f, 0.37f, 0.37f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_PURPLE_RUPEE) {
            itemColor = ImVec4( 0.89f, 0.43f, 1.0f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_HUGE_RUPEE) {
            itemColor = ImVec4( 1.0f, 1.0f, 0, 1.0f );
        }
        return itemColor;
    }

    if (randoItem.GetRandomizerGet() >= RG_GOHMA_SOUL && randoItem.GetRandomizerGet() <= RG_GANON_SOUL) {
        itemColor = bossSoulColorMapping.at(randoItem.GetRandomizerGet());
    }
    
    return itemColor;
}

std::string plandomizerHintsTooltip() {
    std::string hintTootip;
    hintTootip = 
        "The following options are available:\n"
        "- Use \\n to create New Lines.\n"
        "- Use %g to change the text color to Green,\n"
        " - %r for Red, %y for Yellow, and %w for White\n"
        "   can also be used as color examples.";

    return hintTootip;
}

std::string extractNumberInParentheses(const std::string& text) {
    size_t start = text.find('(');
    size_t end = text.find(')');

    if (start != std::string::npos && end != std::string::npos && start < end) {
        return text.substr(start + 1, end - start - 1);
    }
    return "";
}

void PlandomizerPopulateSeedList() {
    existingSeedList.clear();
    auto spoilerPath = Ship::Context::GetPathRelativeToAppDirectory("Randomizer");

    if (std::filesystem::exists(spoilerPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(spoilerPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                existingSeedList.push_back(entry.path().stem().string());
            }
        }
    }
}

void PlandomizerItemImageCorrection(Rando::Item randoItem) {
    textureID = 0;
    imageSize = ImVec2( 32.0f, 32.0f );
    imagePadding = 2.0f;
    textureUV0 = ImVec2( 0, 0 );
    textureUV1 = ImVec2( 1, 1 );
    

    itemColor = plandomizerGetItemColor(randoItem);

    if (randoItem.GetItemType() == ITEMTYPE_SMALLKEY || randoItem.GetItemType() == ITEMTYPE_FORTRESS_SMALLKEY) {
        textureID = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("ITEM_KEY_SMALL");
        return;
    }
    if (randoItem.GetItemType() == ITEMTYPE_BOSSKEY) {
        textureID = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("ITEM_KEY_BOSS");
        return;
    }

    for (auto& map : itemImageMap) {
        if (map.first == randoItem.GetRandomizerGet()) {
            textureID = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(map.second.c_str());
            if (map.second.find("ITEM_ARROWS") != std::string::npos) {
                textureUV0 = ImVec2( 0, 1 );
                textureUV1 = ImVec2( 1, 0 );
            }
            if (map.second == "ITEM_TRIFORCE" || map.first == RG_SKELETON_KEY) {
                textureUV0 = ImVec2( 1, 1 );
                textureUV1 = ImVec2( 0, 0 );
            }
            break;
        }
    }

    if (randoItem.GetRandomizerGet() >= RG_GOHMA_SOUL && randoItem.GetRandomizerGet() <= RG_GANON_SOUL) {
        textureID = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("BOSS_SOUL");
    }

    if (randoItem.GetRandomizerGet() >= RG_OCARINA_A_BUTTON && randoItem.GetRandomizerGet() <= RG_OCARINA_C_RIGHT_BUTTON) {
        textureID = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("ITEM_OCARINA_TIME");
    }

    if (textureID == 0) {
        textureID = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[randoItem.GetGIEntry()->itemId].name);
    }
}

void PlandomizerRandomizeHint(int32_t status, int32_t index) {
    if (status == HINT_SINGLE) {
        plandoHintData[index].hintText = Rando::StaticData::hintTextTable[GetRandomJunkHint()].GetHintMessage().GetForCurrentLanguage(MF_ENCODE);
    } else {
        for (auto& hint : plandoHintData) {
            hint.hintText = Rando::StaticData::hintTextTable[GetRandomJunkHint()].GetHintMessage().GetForCurrentLanguage(MF_ENCODE);
        }
    }
}

void PlandomizerRemoveAllHints() {
    if (plandoHintData.size() > 0) {
        for (auto& remove : plandoHintData) {
            remove.hintText.clear();
        }
    }
}

void PlandomizerSortDrawnItems() {
    std::sort(drawnItemsList.begin(), drawnItemsList.end(),
      [](const auto& a, const auto& b) {
        auto typeA = a.first.GetItemType();
        auto typeB = b.first.GetItemType();
        if (typeA != typeB){
          return typeA < typeB;
        }
        return a.first.GetRandomizerGet() < b.first.GetRandomizerGet();
      });
}

void PlandomizerRemoveAllItems() {
    if (drawnItemsList.size() == 1) {
        drawnItemsList.clear();
    }
    for (auto& remove : plandoLogData) {
        if (std::find(infiniteItemList.begin(), infiniteItemList.end(), remove.checkRewardItem.GetRandomizerGet()) == infiniteItemList.end()) {
            bool itemExists = false;
            for (auto& itemToCheck : drawnItemsList) {
                if (itemToCheck.first.GetRandomizerGet() == remove.checkRewardItem.GetRandomizerGet()) {
                    itemToCheck.second += 1;
                    itemExists = true;
                    break;
                }
            }
            if (!itemExists) {
                drawnItemsList.push_back(std::make_pair(remove.checkRewardItem, 1));
            }
        }
        remove.checkRewardItem = plandomizerRandoRetrieveItem(RG_SOLD_OUT);
    }
    PlandomizerSortDrawnItems();
}

void PlandomizerRemoveFromItemList(Rando::Item randoItem) {
    if (std::find(infiniteItemList.begin(), infiniteItemList.end(), randoItem.GetRandomizerGet()) == infiniteItemList.end()) {
        uint32_t index = 0;
        for (auto& itemToCheck : drawnItemsList) {
            if (itemToCheck.first.GetRandomizerGet() == randoItem.GetRandomizerGet()) {
                if (shouldRemove) {
                    drawnItemsList.erase(drawnItemsList.begin() + index);
                    break;
                } else {
                    itemToCheck.second -= 1;
                }
            }
            index++;
        }
        shouldRemove = false;
    }
    PlandomizerSortDrawnItems();
}

void PlandomizerAddToItemList(Rando::Item randoItem) {
    if (std::find(infiniteItemList.begin(), infiniteItemList.end(), randoItem.GetRandomizerGet()) == infiniteItemList.end()) {
        bool itemExists = false;
        for (auto& itemToCheck : drawnItemsList) {
            if (itemToCheck.first.GetRandomizerGet() == randoItem.GetRandomizerGet()) {
                itemToCheck.second += 1;
                itemExists = true;
                break;
            }
        }
        
        if (!itemExists) {
            drawnItemsList.push_back(std::make_pair(randoItem, 1));
        }
    }
    PlandomizerSortDrawnItems();
}

void PlandomizerSaveSpoilerLog() {
    nlohmann::json spoilerSave;
    std::string filename = lastLoadedSpoiler;

    std::ifstream inputFile(filename);
    if (inputFile.is_open()) {
        inputFile >> spoilerSave;
        inputFile.close();
    }

    spoilerSave["file_hash"] = {
        plandoHash[0], plandoHash[1], plandoHash[2], plandoHash[3], plandoHash[4]
    };

    for (auto& import : plandoHintData) {
        spoilerSave["Gossip Stone Hints"][import.hintName] = {
            { "type", import.hintType.c_str() },
            { "message", import.hintText.c_str() }
        };
    }
    
    for (auto& import : plandoLogData) {
        if (import.checkRewardItem.GetRandomizerGet() == RG_ICE_TRAP) {
            spoilerSave["locations"][import.checkName] = {
                { "item", import.checkRewardItem.GetName().english },
                { "model", import.iceTrapModel.GetName().english },
                { "trickName", import.iceTrapName.c_str() }
            };
            if (import.shopPrice > -1) {
                spoilerSave["locations"][import.checkName]["price"] = import.shopPrice;
            }
        } else if (import.shopPrice > -1) {
            spoilerSave["locations"][import.checkName] = {
                { "item", import.checkRewardItem.GetName().english },
                { "price", import.shopPrice }
            };
        } else {
            spoilerSave["locations"][import.checkName] = import.checkRewardItem.GetName().english;
        }
    }

    std::ofstream outputFile(filename);
    if (outputFile.is_open()) {
        outputFile << spoilerSave.dump(4);
        outputFile.close();
    }    
}

void PlandomizerLoadSpoilerLog(std::string logFile) {
    spoilerHash.clear();
    plandoHash.clear();
    spoilerLogData.clear();
    plandoLogData.clear();
    spoilerHintData.clear();
    plandoHintData.clear();
    drawnItemsList.clear();

    nlohmann::json spoilerLogInput;
    auto spoilerPath = Ship::Context::GetPathRelativeToAppDirectory("Randomizer");
    std::string spoilerStr = spoilerPath + "/" + logFile.c_str() + ".json";

    if (!std::filesystem::exists(spoilerStr)) {
        return;
    }

    std::ifstream file(spoilerStr);

    if (file.is_open()) {
        try {
            file >> spoilerLogInput;
            file.close();

            if (spoilerLogInput.contains("file_hash")) {
                auto hash = spoilerLogInput["file_hash"];
                for (auto& load : hash) {
                    spoilerHash.push_back(load);
                    plandoHash.push_back(load);
                }
            }

            if (spoilerLogInput.contains("Gossip Stone Hints")) {
                auto hints = spoilerLogInput["Gossip Stone Hints"];
                for (auto& [key, value] : hints.items()) {
                    SpoilerHintObject hintObject;
                    hintObject.hintName = key.c_str();
                    hintObject.hintType = "Hardcoded Message";
                    hintObject.hintText = value["message"];

                    spoilerHintData.push_back(hintObject);
                    plandoHintData.push_back(hintObject);
                }
            }

            if (spoilerLogInput.contains("locations")) {
                auto locations = spoilerLogInput["locations"];
                for (auto& [key, value] : locations.items()) {
                    if (key == "Ganon" || key == "Completed Triforce") {
                        continue;
                    }
                    SpoilerCheckObject checkObject;
                    checkObject.checkName = key;
                    auto type = value;
                    if (value.is_object()) {
                        checkObject.checkRewardItem = plandomizerRandoRetrieveItem(Rando::StaticData::itemNameToEnum[value["item"]]);
                        if (value["price"].is_number()) {
                            checkObject.shopPrice = value["price"];
                        } else {
                            checkObject.shopPrice = -1;
                        }
                        if (checkObject.checkRewardItem.GetRandomizerGet() == RG_ICE_TRAP) {
                            checkObject.iceTrapModel = plandomizerRandoRetrieveItem(Rando::StaticData::itemNameToEnum[value["model"]]);
                            checkObject.iceTrapName = value["trickName"];
                        }
                    } else {
                        checkObject.checkRewardItem = plandomizerRandoRetrieveItem(Rando::StaticData::itemNameToEnum[value.get<std::string>()]);
                        checkObject.shopPrice = -1;
                        if (checkObject.shopPrice == -1 
                            && checkObject.checkRewardItem.GetName().english.find("Buy") != std::string::npos) {
                            checkObject.shopPrice = checkObject.checkRewardItem.GetPrice();
                        }
                    }
                    spoilerLogData.push_back(checkObject);
                    plandoLogData.push_back(checkObject);
                    PlandomizerAddToItemList(plandomizerRandoRetrieveItem(RG_SOLD_OUT));
                }
            }
        } catch (nlohmann::json::parse_error& e) {
            Notification::Emit({ .message = "Invalid Spoiler Log Format", .remainingTime = 10.0f });
        }
    }
    lastLoadedSpoiler = spoilerStr;
}

void PlandomizerOverlayText(std::pair<Rando::Item, uint32_t> drawObject ) {
    // Overlay the item count text on the existing button
    ImVec2 imageMin = ImGui::GetItemRectMin();
    ImVec2 imageMax = ImGui::GetItemRectMax();
    ImVec2 textPos = ImVec2(imageMax.x - ImGui::CalcTextSize(std::to_string(drawObject.second).c_str()).x - 2,
                            imageMax.y - ImGui::CalcTextSize(std::to_string(drawObject.second).c_str()).y - 2);

    ImGui::SetCursorScreenPos(textPos);
    ImGui::Text(std::to_string(drawObject.second).c_str());

    // Overlay item info
    if (drawObject.first.GetRandomizerGet() >= RG_PROGRESSIVE_HOOKSHOT && 
        drawObject.first.GetRandomizerGet() <= RG_PROGRESSIVE_GORONSWORD) {
        textPos = ImVec2(imageMin.x + 2, imageMin.y + 2);

        ImGui::SetCursorScreenPos(textPos);
        ImGui::Text("+");
    }
    if (extractNumberInParentheses(drawObject.first.GetName().english.c_str()) != "" && 
        extractNumberInParentheses(drawObject.first.GetName().english.c_str()) != "WINNER" && 
        extractNumberInParentheses(drawObject.first.GetName().english.c_str()) != "LOSER") {
        textPos = ImVec2(imageMin.x + 2, imageMin.y + 2);

        ImGui::SetCursorScreenPos(textPos);
        std::string overlayText = "+";
        overlayText += extractNumberInParentheses(drawObject.first.GetName().english.c_str());
        ImGui::Text(overlayText.c_str());
    }
    if (drawObject.first.GetRandomizerGet() >= RG_FOREST_TEMPLE_BOSS_KEY && 
        drawObject.first.GetRandomizerGet() <= RG_GANONS_CASTLE_BOSS_KEY) {
        textPos = ImVec2(imageMin.x + 1, imageMin.y + 1);
        ImGui::SetCursorScreenPos(textPos);
        shortName = "";
        for (auto& name : bossKeyShortNames) {
            if (name.first == drawObject.first.GetRandomizerGet()) {
                shortName = name.second;
                break;
            }
        }
        ImGui::Text(shortName.c_str());
    }
    if (drawObject.first.GetRandomizerGet() >= RG_OCARINA_A_BUTTON &&
        drawObject.first.GetRandomizerGet() <= RG_OCARINA_C_RIGHT_BUTTON) {
        textPos = ImVec2(imageMin.x + 1, imageMin.y + 1);
        ImGui::SetCursorScreenPos(textPos);
        shortName = "";
        for (auto& name : ocarinaButtonNames) {
            if (name.first == drawObject.first.GetRandomizerGet()) {
                shortName = name.second;
                break;
            }
        }
        ImGui::Text(shortName.c_str());
    }
}

void PlandomizerDrawItemPopup(uint32_t index) {
    if (shouldPopup && ImGui::BeginPopup("ItemList")) {
        PlandoPushImageButtonStyle();
        ImGui::SeparatorText("Resources");
        ImGui::BeginTable("Infinite Item Table", 7);
        for (auto& item : infiniteItemList) {
            ImGui::PushID(item);
            ImGui::TableNextColumn();
            PlandomizerItemImageCorrection(plandomizerRandoRetrieveItem(item));
            auto name = plandomizerRandoRetrieveItem(item).GetName().english;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(imagePadding, imagePadding));
            auto ret = ImGui::ImageButton(name.c_str(), textureID, imageSize, textureUV0, textureUV1, ImVec4(0, 0, 0, 0), itemColor);
            ImGui::PopStyleVar();
            if (ret) {
                if (std::find(infiniteItemList.begin(), infiniteItemList.end(), plandoLogData[index].checkRewardItem.GetRandomizerGet()) == infiniteItemList.end()) {
                    PlandomizerAddToItemList(plandoLogData[index].checkRewardItem);
                }
                plandoLogData[index].checkRewardItem = plandomizerRandoRetrieveItem(item);
                ImGui::CloseCurrentPopup();
            }
            UIWidgets::Tooltip(name.c_str());
            PlandomizerOverlayText(std::make_pair(plandomizerRandoRetrieveItem(item), 1));
            ImGui::PopID();
        }
        
        
        ImGui::EndTable();
        ImGui::SeparatorText("Spoiler Log Rewards");
        ImGui::BeginTable("Item Button Table", 8);
        uint32_t itemIndex = 0;

        bool isClicked = false;
        for (auto& drawSlots : drawnItemsList) {
            ImGui::TableNextColumn();
            ImGui::BeginGroup();
            ImGui::PushID(itemIndex);
            auto itemToDraw = drawSlots.first;
            PlandomizerItemImageCorrection(drawSlots.first);
            auto name = drawSlots.first.GetName().english;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(imagePadding, imagePadding));
            auto ret = ImGui::ImageButton(name.c_str(), textureID, imageSize, textureUV0, textureUV1, ImVec4(0, 0, 0, 0), itemColor);
            ImGui::PopStyleVar();
            if (ret) {
                if (itemToDraw.GetRandomizerGet() >= RG_PROGRESSIVE_HOOKSHOT && 
                    itemToDraw.GetRandomizerGet() <= RG_PROGRESSIVE_GORONSWORD) {
                    plandoLogData[index].checkRewardItem = drawSlots.first;
                } else {
                    plandoLogData[index].checkRewardItem = itemToDraw;
                }
                temporaryItemIndex = itemIndex;
                if (drawSlots.second == 1) {
                    shouldRemove = true;
                }
                isClicked = true;
                ImGui::CloseCurrentPopup();
            }
            if (!isClicked) {
                UIWidgets::Tooltip(name.c_str());
            }
            ImGui::PopID();

            PlandomizerOverlayText(drawSlots);

            ImGui::EndGroup();
            itemIndex++;
        }
        if (isClicked) {
            PlandomizerRemoveFromItemList(drawnItemsList[temporaryItemIndex].first);
            PlandomizerAddToItemList(temporaryItem);
        }
        PlandoPopImageButtonStyle();
        ImGui::EndTable();
        ImGui::EndPopup();
    }
}

void PlandomizerDrawIceTrapPopUp(uint32_t index) {
    if (shouldTrapPopup && ImGui::BeginPopup("TrapList")) {
        ImGui::BeginTable("Ice Trap Table", 8);
        PlandoPushImageButtonStyle();
        for (auto& items : itemImageMap) {
            if (items.first == RG_ICE_TRAP) {
                continue;
            }
            ImGui::TableNextColumn();
            ImGui::PushID(items.first);
            auto name = Rando::StaticData::RetrieveItem(items.first).GetName().english;
            PlandomizerItemImageCorrection(Rando::StaticData::RetrieveItem(items.first));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(imagePadding, imagePadding));
            auto ret = ImGui::ImageButton(name.c_str(), textureID, imageSize, textureUV0, textureUV1, ImVec4(0, 0, 0, 0), itemColor); 
            ImGui::PopStyleVar();
            if (ret) {
                plandoLogData[index].iceTrapModel = Rando::StaticData::RetrieveItem(items.first);
                ImGui::CloseCurrentPopup();
            };
            UIWidgets::Tooltip(name.c_str());

            auto itemObject = Rando::StaticData::RetrieveItem(items.first);
            PlandomizerOverlayText(std::make_pair(itemObject, 1));

            ImGui::PopID();
        }
        PlandoPopImageButtonStyle();
        ImGui::EndTable();
        ImGui::EndPopup();
    }
}

void PlandomizerDrawItemSlots(uint32_t index) {
    ImGui::PushID(index);
    PlandoPushImageButtonStyle();
    PlandomizerItemImageCorrection(plandoLogData[index].checkRewardItem);
    auto name = plandoLogData[index].checkRewardItem.GetName().english;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(imagePadding, imagePadding));
    auto ret = ImGui::ImageButton(name.c_str(), textureID, imageSize, textureUV0, textureUV1, ImVec4(0, 0, 0, 0), itemColor);
    ImGui::PopStyleVar();
    if (ret) {
        shouldPopup = true;
        temporaryItem = plandoLogData[index].checkRewardItem;
        ImGui::OpenPopup("ItemList");
    };
    PlandoPopImageButtonStyle();
    UIWidgets::Tooltip(name.c_str());
    PlandomizerOverlayText(std::make_pair(plandoLogData[index].checkRewardItem, 1));
    PlandomizerDrawItemPopup(index);
    ImGui::PopID();
}

void PlandomizerDrawShopSlider(uint32_t index) {
    ImGui::PushID(index);
    UIWidgets::SliderInt("Price:", &plandoLogData[index].shopPrice, UIWidgets::IntSliderOptions()
        .Color(THEME_COLOR).Format("%d Rupees").Min(0).Max(999).LabelPosition(UIWidgets::LabelPositions::Near)
        .ComponentAlignment(UIWidgets::ComponentAlignments::Right).Size(UIWidgets::Sizes::Inline));
    ImGui::PopID();
}

void PlandomizerDrawIceTrapSetup(uint32_t index) {
    std::string trapTextInput = plandoLogData[index].iceTrapName.c_str();

    ImGui::PushID(index);
    ImGui::BeginTable("IceTrap", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInner);
    ImGui::TableSetupColumn("Model", ImGuiTableColumnFlags_WidthFixed, 36.0f);
    ImGui::TableSetupColumn("Trap Options");
    ImGui::TableHeadersRow();

    ImGui::TableNextColumn();
    PlandomizerItemImageCorrection(plandoLogData[index].iceTrapModel);
    PlandoPushImageButtonStyle();
    auto name = plandoLogData[index].iceTrapModel.GetName().english;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(imagePadding, imagePadding));
    auto ret = ImGui::ImageButton(name.c_str(), textureID, imageSize, textureUV0, textureUV1, ImVec4(0, 0, 0, 0), itemColor);
    ImGui::PopStyleVar();
    if (ret) {
        shouldTrapPopup = true;
        ImGui::OpenPopup("TrapList");
    };
    PlandoPopImageButtonStyle();
    UIWidgets::Tooltip(name.c_str());
    PlandomizerDrawIceTrapPopUp(index);
    ImGui::SameLine();
    ImGui::TableNextColumn();
    ImGui::Text("Name: ");
    ImGui::SameLine();
    if (plandoLogData[index].iceTrapModel.GetRandomizerGet() != RG_NONE && 
        plandoLogData[index].iceTrapModel.GetRandomizerGet() != RG_SOLD_OUT) {
        if (UIWidgets::Button(randomizeButton.c_str(), UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(UIWidgets::Sizes::Inline).Padding(ImVec2(10.f, 6.f)))) {
            plandoLogData[index].iceTrapName = 
                GetIceTrapName(plandoLogData[index].iceTrapModel.GetRandomizerGet()).GetForLanguage(CVarGetInteger(CVAR_SETTING("Languages"), 0)).c_str();
        }
        ImGui::SameLine();
    }
    if (UIWidgets::InputString("##TrapName", &trapTextInput, UIWidgets::InputOptions().Color(THEME_COLOR).LabelPosition(UIWidgets::LabelPositions::None))) {
            plandoLogData[index].iceTrapName = trapTextInput.c_str();
        }
    
    if (plandoLogData[index].shopPrice >= 0) {
        PlandomizerDrawShopSlider(index);
    }
    ImGui::EndTable();
    
    ImGui::PopID();
}
static std::unordered_map<RandomizerCheckArea, const char*> rcAreaNameMap = {
    { RCAREA_KOKIRI_FOREST, "Kokiri Forest" },
    { RCAREA_LOST_WOODS, "Lost Woods" },
    { RCAREA_SACRED_FOREST_MEADOW, "Sacred Forest Meadow" },
    { RCAREA_HYRULE_FIELD, "Hyrule Field" },
    { RCAREA_LAKE_HYLIA, "Lake Hylia" },
    { RCAREA_GERUDO_VALLEY, "Gerudo Valley" },
    { RCAREA_GERUDO_FORTRESS, "Gerudo Fortress" },
    { RCAREA_WASTELAND, "Haunted Wasteland" },
    { RCAREA_DESERT_COLOSSUS, "Desert Colossus" },
    { RCAREA_MARKET, "Hyrule Market" },
    { RCAREA_HYRULE_CASTLE, "Hyrule Castle" },
    { RCAREA_KAKARIKO_VILLAGE, "Kakariko Village" },
    { RCAREA_GRAVEYARD, "Graveyard" },
    { RCAREA_DEATH_MOUNTAIN_TRAIL, "Death Mountain Trail" },
    { RCAREA_GORON_CITY, "Goron City" },
    { RCAREA_DEATH_MOUNTAIN_CRATER, "Death Mountain Crater" },
    { RCAREA_ZORAS_RIVER, "Zora's River" },
    { RCAREA_ZORAS_DOMAIN, "Zora's Domain" },
    { RCAREA_ZORAS_FOUNTAIN, "Zora's Fountain" },
    { RCAREA_LON_LON_RANCH, "Lon Lon Ranch" },
    { RCAREA_DEKU_TREE, "Deku Tree" },
    { RCAREA_DODONGOS_CAVERN, "Dodongo's Cavern" },
    { RCAREA_JABU_JABUS_BELLY, "Jabu Jabu's Belly" },
    { RCAREA_FOREST_TEMPLE, "Forest Temple" },
    { RCAREA_FIRE_TEMPLE, "Fire Temple" },
    { RCAREA_WATER_TEMPLE, "Water Temple" },
    { RCAREA_SPIRIT_TEMPLE, "Spirit Temple" },
    { RCAREA_SHADOW_TEMPLE, "Shadow Temple" },
    { RCAREA_BOTTOM_OF_THE_WELL, "Bottom of the Well" },
    { RCAREA_ICE_CAVERN, "Ice Cavern" },
    { RCAREA_GERUDO_TRAINING_GROUND, "Gerudo Training Ground" },
    { RCAREA_GANONS_CASTLE, "Ganon's Castle" },
    { RCAREA_INVALID, "All" },
};
void PlandomizerDrawOptions() {
    if (ImGui::BeginTable("LoadSpoiler", 2)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextColumn();
        ImGui::SeparatorText("Load/Save Spoiler Log");
        PlandomizerPopulateSeedList();
        static size_t selectedList = 0;
        if (existingSeedList.size() != 0) {
            UIWidgets::Combobox("##JsonFiles", &selectedList, existingSeedList, UIWidgets::ComboboxOptions().Color(THEME_COLOR).LabelPosition(UIWidgets::LabelPositions::None));
        }
        else {
            ImGui::Text("No Spoiler Logs found.");
        }
        ImGui::BeginDisabled(existingSeedList.empty());
        if (UIWidgets::Button("Load", UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(UIWidgets::Sizes::Inline))) {
            logTemp = existingSeedList[selectedList].c_str();
            PlandomizerLoadSpoilerLog(logTemp.c_str());
        }
        ImGui::EndDisabled();
        ImGui::BeginDisabled(spoilerLogData.empty());
        ImGui::SameLine();
        if (UIWidgets::Button("Save", UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(UIWidgets::Sizes::Inline))) {
            PlandomizerSaveSpoilerLog();
        }
        ImGui::EndDisabled();

        ImGui::TableNextColumn();
        ImGui::SeparatorText("Current Seed Hash");
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * 0.5f) - (34.0f * 5.0f));
        if (spoilerLogData.size() > 0) {
            if (ImGui::BeginTable("HashIcons", 5)) {
                for (int i = 0; i < 5; i++) {
                    ImGui::TableSetupColumn("Icon", ImGuiTableColumnFlags_WidthFixed, 34.0f);
                }
                ImGui::TableNextColumn();

                size_t index = 0;
                PlandoPushImageButtonStyle();
                for (auto& hash : plandoHash) {
                    ImGui::PushID(index);
                    textureID = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(gSeedTextures[hash].tex);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
                    auto upRet = ImGui::ImageButton("HASH_ARROW_UP", Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("HASH_ARROW_UP"),
                                                  ImVec2(35.0f, 18.0f), ImVec2(1, 1), ImVec2(0, 0), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
                    ImGui::PopStyleVar();
                    if (upRet) {
                        if (hash + 1 >= gSeedTextures.size()) {
                            hash = 0;
                        }
                        else {
                            hash++;
                        }
                    }
                    ImGui::Image(textureID, ImVec2(35.0f, 35.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
                    auto downRet = ImGui::ImageButton("HASH_ARROW_DWN", Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("HASH_ARROW_DWN"),
                                                  ImVec2(35.0f, 18.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
                    ImGui::PopStyleVar();
                    if (downRet) {
                        if (hash == 0) {
                            hash = gSeedTextures.size() - 1;
                        }
                        else {
                            hash--;
                        }
                    }
                    if (index != spoilerHash.size() - 1) {
                        ImGui::TableNextColumn();
                    }
                    ImGui::PopID();
                    index++;
                }
                PlandoPopImageButtonStyle();
                ImGui::EndTable();
            }
        }
        else {
            ImGui::Text("No Spoiler Log Loaded");
        }
        ImGui::EndTable();
    }
    
    ImGui::SeparatorText("Options");
    if (plandoLogData.size() == 0) {
        ImGui::Text("Please Load Spoiler Data...");
        return;
    }

    if (getTabID == TAB_HINTS) {
        if (UIWidgets::Button("Clear All Hints", UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(UIWidgets::Sizes::Inline))) {
            PlandomizerRemoveAllHints();
        }
        ImGui::SameLine();
        if (UIWidgets::Button("Randomize All Hints", UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(UIWidgets::Sizes::Inline))) {
            PlandomizerRandomizeHint(HINT_ALL, 0);
        }
    }
    if (getTabID == TAB_LOCATIONS) {
        if (plandoLogData.size() > 0) {
            UIWidgets::Combobox("Filter by Area:##AreaFilter", &selectedArea, rcAreaNameMap, UIWidgets::ComboboxOptions().Color(THEME_COLOR).LabelPosition(UIWidgets::LabelPositions::Near).ComponentAlignment(UIWidgets::ComponentAlignments::Right));
            ImGui::SameLine();
            if (UIWidgets::Button("Empty All Rewards", UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(UIWidgets::Sizes::Inline).Padding(ImVec2(10.f, 6.f)))) {
                PlandomizerRemoveAllItems();
            }
        }
    }
}

void PlandomizerDrawHintsWindow() {
    uint32_t index = 0;
    std::string hintInputText;

    ImGui::BeginChild("Hints");
    if (ImGui::BeginTable("Hints Window", 1, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Hint Entries");
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (auto& hintData : spoilerHintData) {
            ImGui::PushID(index);
            ImGui::TableNextColumn();
            ImGui::SeparatorText(hintData.hintName.c_str());
            ImGui::Text("Current Hint: ");
            ImGui::SameLine();
            ImGui::TextWrapped(hintData.hintText.c_str());

            if (spoilerHintData.size() > 0) {
                hintInputText = plandoHintData[index].hintText.c_str();
            }
            ImGui::Text("New Hint:     ");
            ImGui::SameLine();
            if (UIWidgets::Button(randomizeButton.c_str(), UIWidgets::ButtonOptions().Color(THEME_COLOR).Padding(ImVec2(10.f, 6.f)).Size(UIWidgets::Sizes::Inline).Tooltip("Randomize Hint"))) {
                PlandomizerRandomizeHint(HINT_SINGLE, index);
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
            if (UIWidgets::InputString("##HintMessage", &hintInputText, UIWidgets::InputOptions().Color(THEME_COLOR).LabelPosition(UIWidgets::LabelPositions::None).Tooltip(plandomizerHintsTooltip().c_str()))) {
                plandoHintData[index].hintText = hintInputText.c_str();
            }
            index++;
            ImGui::PopID();
        }

        ImGui::EndTable();
    }
    ImGui::EndChild();
}

void PlandomizerDrawLocationsWindow(RandomizerCheckArea rcArea) {
    uint32_t index = 0;
    ImGui::BeginChild("Locations");
    if (ImGui::BeginTable("Locations Window", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Spoiler Log Check Name", ImGuiTableColumnFlags_WidthFixed, 250.0f);
        ImGui::TableSetupColumn("Spoiler Log Reward", ImGuiTableColumnFlags_WidthFixed, 190.0f);
        ImGui::TableSetupColumn("New Reward", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, 34.0f);
        ImGui::TableSetupColumn("Additional Options");
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (auto& spoilerData : spoilerLogData) {
            auto checkID = Rando::StaticData::locationNameToEnum[spoilerData.checkName];
            auto randoArea = Rando::StaticData::GetLocation(checkID)->GetArea();
            if (rcArea == RCAREA_INVALID || rcArea == randoArea) {
                ImGui::TableNextColumn();
                ImGui::TextWrapped(spoilerData.checkName.c_str());
                ImGui::TableNextColumn();
                ImGui::TextWrapped(spoilerData.checkRewardItem.GetName().english.c_str());
                ImGui::TableNextColumn();
                PlandomizerDrawItemSlots(index);
                if (plandoLogData[index].checkRewardItem.GetRandomizerGet() == RG_ICE_TRAP) {
                    ImGui::TableNextColumn();
                    PlandomizerDrawIceTrapSetup(index);
                }
                else if (spoilerData.shopPrice != -1) {
                    ImGui::TableNextColumn();
                    ImGui::BeginTable("Shops", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInner);
                    ImGui::TableSetupColumn("Shop Price");
                    ImGui::TableHeadersRow();
                    ImGui::TableNextColumn();
                    PlandomizerDrawShopSlider(index);
                    ImGui::EndTable();
                }
                else {
                    ImGui::TableNextColumn();
                }
            }
            index++;
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
}

void PlandomizerDrawSpoilerTable() {
    ImGui::BeginChild("Main");
    UIWidgets::PushStyleTabs(THEME_COLOR);
    if (ImGui::BeginTabBar("Check Tabs")) {
        if (ImGui::BeginTabItem("Gossip Stones")) {
            getTabID = TAB_HINTS;
            PlandomizerDrawHintsWindow();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Locations")) {
            getTabID = TAB_LOCATIONS;
            PlandomizerDrawLocationsWindow(selectedArea);
            ImGui::EndTabItem();
        }
    }
    ImGui::EndTabBar();
    UIWidgets::PopStyleTabs();
    ImGui::EndChild();
}

void PlandomizerWindow::DrawElement() {
    PlandomizerDrawOptions();
    UIWidgets::Separator(true, true, 0.f, 0.f);
    PlandomizerDrawSpoilerTable();
}

void PlandomizerWindow::InitElement() {
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("ITEM_RUPEE_GRAYSCALE", gRupeeCounterIconTex, ImVec4(1, 1, 1, 1));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("ITEM_HEART_GRAYSCALE", gHeartFullTex, ImVec4(0.87f, 0.10f, 0.10f, 1));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("ITEM_SEEDS", gItemIconDekuSeedsTex, ImVec4( 1, 1, 1, 1 ));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("ITEM_ARROWS_SMALL", gDropArrows1Tex, ImVec4( 1, 1, 1, 1 ));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("ITEM_ARROWS_MEDIUM", gDropArrows2Tex, ImVec4( 1, 1, 1, 1 ));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("ITEM_ARROWS_LARGE", gDropArrows3Tex, ImVec4( 1, 1, 1, 1 ));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("ITEM_ICE_TRAP", gMagicArrowEquipEffectTex, ImVec4( 1, 1, 1, 1 ));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("HASH_ARROW_UP", gEmptyCDownArrowTex, ImVec4( 1, 1, 1, 1 ));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("HASH_ARROW_DWN", gEmptyCDownArrowTex, ImVec4( 1, 1, 1, 1 ));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("BOSS_SOUL", gBossSoulTex, ImVec4(1, 1, 1, 1));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("TRIFORCE_PIECE", gTriforcePieceTex, ImVec4(1, 1, 1, 1));
}
