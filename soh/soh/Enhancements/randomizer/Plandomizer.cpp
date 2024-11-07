#include "Plandomizer.h"
#include "soh/UIWidgets.hpp"
#include "soh/util.h"
#include <vector>
#include "include/z64item.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#include <fstream>
#include <filesystem>

#include "soh/OTRGlobals.h"
#include "soh/ImGuiUtils.h"
#include "soh/Enhancements/randomizer/logic.h"
#include "soh/Enhancements/randomizer/randomizer_check_objects.h"

extern "C" {
    extern SaveContext gSaveContext;
    extern PlayState* gPlayState;
}

static int32_t itemIndexToRemove = -1;
static int32_t correctedItemID = -1;
static int32_t getTabID = TAB_HINTS;

Rando::Item temporaryItem;
std::string shortName = "";
std::string logTemp = "";
std::string lastLoadedSpoiler = "";
int32_t temporaryItemIndex = -1;
RandomizerCheckArea selectedArea = RCAREA_INVALID; 

ImVec4 itemTint = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
ImTextureID textureID;
ImVec2 textureUV0 = ImVec2( 0, 0 );
ImVec2 textureUV1 = ImVec2( 1, 1 );

char logID[15];
bool shouldPopup = false;
bool shouldTrapPopup = false;
bool shouldRemove = false;

namespace fs = std::filesystem;
std::vector<std::string> existingSeedList;

std::vector<int32_t> spoilerHash;
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
    { RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, "ITEM_KEY_SMALL" },
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
    { RG_PIECE_OF_HEART,            		"ITEM_HEART_PIECE" },
    { RG_HEART_CONTAINER,           		"ITEM_HEART_CONTAINER" },
    { RG_ICE_TRAP,                  		"ITEM_ICE_TRAP" },
    { RG_BOMBS_5,                   		"ITEM_BOMB" },
    { RG_BOMBS_10,                  		"ITEM_BOMB" },
    { RG_BOMBS_20,                  		"ITEM_BOMB" },
    { RG_BUY_BOMBS_10,              		"ITEM_BOMB" },
    { RG_BUY_BOMBS_20,              		"ITEM_BOMB" },
    { RG_DEKU_NUTS_5,               		"ITEM_NUT" },
    { RG_DEKU_NUTS_10,              		"ITEM_NUT" },
    { RG_BUY_DEKU_NUTS_5,           		"ITEM_NUT" },
    { RG_BOMBCHU_5,                 		"ITEM_BOMBCHU" },
    { RG_BOMBCHU_10,                		"ITEM_BOMBCHU" },
    { RG_BOMBCHU_20,                		"ITEM_BOMBCHU" },
    { RG_ARROWS_5,                  		"ITEM_ARROWS_SMALL" },
    { RG_ARROWS_10,                 		"ITEM_ARROWS_MEDIUM" },
    { RG_ARROWS_30,                 		"ITEM_ARROWS_LARGE" },
    { RG_TREASURE_GAME_HEART,       		"ITEM_HEART_PIECE" },
    { RG_DEKU_SEEDS_30,             		"ITEM_SEEDS" },
    { RG_SOLD_OUT,                  		"ITEM_SOLD_OUT" },
    { RG_TRIFORCE,                  		"ITEM_TRIFORCE" },
};

std::vector<std::string> hintPool = {
    "They say that %gGanondorf's Mom%w is going out with %ySqueak%w!",
	"They say that %gProxySaw%w is still fixing %yCaladius's Bugs%w...",
	"They say that %gItsHeckinPat%w is still just %yEyeballing it%w.",
	"They say that %gCaladius%w is working on %yV2%w of something.",
	"They say that %gdice%w is a funny name for a %ytaco%w.",
	"They say %g2Ship Rando%w is still blocked by %yV3%w...",
	"They say if you click your heels and say %gframebuffer%w 3 times, %yArchez%w appears!",
	"They say %gVB%w stands for %yVirtual Bananas%w... Probably.",
	"They say %gZeru%w is still routing his %yHundo%w.",
	"They say %gRaccoonCloud%w is still looking for his %yHover Boots%w.",
	"They say %gItsHeckinPat%w foreclosed on his %yHut%w.",
	"They say %gRaccoonCloud%w is part of the %yInner Circle%w.",
	"They say %gMoonlitxShadows%w is the %rleader%w of the %yDork Army%w.",
	"They say %gGanondorf%w hates the %yInternet%w.",
	"They say %gMido's House%w hoards %yTrash%w.",
	"They say %gSweettalking Ganondorf%w rewards %yHis Heart%w.",
	"They say %gaMannus%w said %yGo To Bed%w!",
	"They say %gCaladius%w is a %yPinhead%w!",
	"They say %gRaccoonCloud%w loves the %yIce Cavern%w.",
	"They say %gNo One%w should forget %yHover Scrub%w!",
	"They say %gMoonlitxShadows%w likes to %ySlide%w.",
	"They say that %gRaccoonCloud%w is still learning %yB1 Skip%w...",
	"They say that %gBackwalking%w should be %rBanned%w!",
	"They say that %gGoron's%w should always have %yLong Necks%w!",
	"They say that %gCaladius%w has a %ytendency to lose his shirt%w!",
	"They say that if your %rSkip keeps Failing%w, you're probably an %yESS Off%w.",
	"They say that %gLogic%w is just a %ySuggestion%w.",
	"They say there's %gAlways Logic%w in %yNo Logic%w...",
	"They said that %rFredomato%w has just %yone more push up%w to do!"
};

Rando::Item plandomizerRandoRetrieveItem(RandomizerGet randoGetItem) {
    auto randoGetItemEntry = Rando::StaticData::RetrieveItem(randoGetItem);
    return randoGetItemEntry;
}

ImVec4 plandomizerGetItemTint(Rando::Item randoItem) {
    itemTint = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
    if (randoItem.GetItemType() == ITEMTYPE_SMALLKEY || randoItem.GetItemType() == ITEMTYPE_FORTRESS_SMALLKEY
        || randoItem.GetItemType() == ITEMTYPE_BOSSKEY) {
        if (randoItem.GetRandomizerGet() == RG_FOREST_TEMPLE_SMALL_KEY) {
            itemTint = ImVec4( 0.02f, 0.76f, 0.18f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_FIRE_TEMPLE_SMALL_KEY) {
            itemTint = ImVec4( 0.93f, 0.37f, 0.37f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_WATER_TEMPLE_SMALL_KEY) {
            itemTint = ImVec4( 0.33f, 0.71f, 0.87f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_SPIRIT_TEMPLE_SMALL_KEY) {
            itemTint = ImVec4( 0.87f, 0.62f, 0.18f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_SHADOW_TEMPLE_SMALL_KEY) {
            itemTint = ImVec4( 0.49f, 0.06f, 0.69f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_BOTTOM_OF_THE_WELL_SMALL_KEY) {
            itemTint = ImVec4( 0.89f, 0.43f, 1.0f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY) {
            itemTint = ImVec4( 1.0f, 1.0f, 0, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_GERUDO_FORTRESS_SMALL_KEY) {
            itemTint = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_GANONS_CASTLE_SMALL_KEY) {
            itemTint = ImVec4( 0.5f, 0.5f, 0.5f, 1.0f );
        }
        return itemTint;
    }
    if (randoItem.GetItemType() == ITEMTYPE_SONG) {
        uint32_t questID = Rando::Logic::RandoGetToQuestItem[randoItem.GetRandomizerGet()];
        textureID = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(songMapping.at((QuestItem)questID).name);
        itemTint = songMapping.at((QuestItem)questID).color;
        return itemTint;
    }
    if (randoItem.GetRandomizerGet() >= RG_GREEN_RUPEE && randoItem.GetRandomizerGet() <= RG_HUGE_RUPEE) {
        if (randoItem.GetRandomizerGet() == RG_GREG_RUPEE || randoItem.GetRandomizerGet() == RG_GREEN_RUPEE) {
            itemTint = ImVec4( 0.02f, 0.76f, 0.18f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_BLUE_RUPEE) {
            itemTint = ImVec4( 0.33f, 0.71f, 0.87f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_RED_RUPEE) {
            itemTint = ImVec4( 0.93f, 0.37f, 0.37f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_PURPLE_RUPEE) {
            itemTint = ImVec4( 0.89f, 0.43f, 1.0f, 1.0f );
        } else if (randoItem.GetRandomizerGet() == RG_HUGE_RUPEE) {
            itemTint = ImVec4( 1.0f, 1.0f, 0, 1.0f );
        }
        return itemTint;
    }
    
    return itemTint;
}

void PlandomizerPopulateSeedList() {
    existingSeedList.clear();

    for (const auto& entry : fs::directory_iterator("Randomizer")) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            existingSeedList.push_back(entry.path().stem().string());
        }
    }
}

void PlandomizerItemImageCorrection(Rando::Item randoItem) {
    textureID = 0;
    textureUV0 = ImVec2( 0, 0 );
    textureUV1 = ImVec2( 1, 1 );

    itemTint = plandomizerGetItemTint(randoItem);

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
            if (map.second.find("ITEM_TRIFORCE") != std::string::npos) {
                textureUV0 = ImVec2( 1, 1 );
                textureUV1 = ImVec2( 0, 0 );
            }
            break;
        }
    }

    if (textureID == 0) {
        textureID = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[randoItem.GetGIEntry()->itemId].name);
    }
}

void PlandomizerRandomizeHint(int32_t status, int32_t index) {
    switch (status) {
        case HINT_SINGLE:
            plandoHintData[index].hintText = "";
            break;
        case HINT_ALL:
            for (auto& hint : plandoHintData) {
                int32_t roll = (rand() % hintPool.size());
                hint.hintText = hintPool[roll].c_str();
            }
            break;
        default:
            break;
    }
}

void PlandomizerRemoveAllHints() {
    if (plandoHintData.size() > 0) {
        for (auto& remove : plandoHintData) {
            remove.hintText.clear();
        }
    }
}

void PlandomizerRemoveAllItems() {
    if (drawnItemsList.size() == 1) {
        drawnItemsList.clear();
    }
    for (auto& remove : plandoLogData) {
        if (remove.checkRewardItem != plandomizerRandoRetrieveItem(RG_SOLD_OUT)) {
            bool itemExists = false;
            for (auto& itemToCheck : drawnItemsList) {
                if (itemToCheck.first == remove.checkRewardItem) {
                    itemToCheck.second += 1;
                    itemExists = true;
                    break;
                }
            }
            if (!itemExists) {
                drawnItemsList.push_back(std::make_pair(remove.checkRewardItem, 1));
            }
            remove.checkRewardItem = plandomizerRandoRetrieveItem(RG_SOLD_OUT);
        }
    }
}

void PlandomizerRemoveFromItemList(Rando::Item randoItem) {
    uint32_t index = 0;
    for (auto& itemToCheck : drawnItemsList) {
        if (itemToCheck.first == randoItem) {
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
    itemIndexToRemove = -1;
}

void PlandomizerAddToItemList(Rando::Item randoItem) {
    bool itemExists = false;
    for (auto& itemToCheck : drawnItemsList) {
        if (itemToCheck.first == randoItem) {
            itemToCheck.second += 1;
            itemExists = true;
            break;
        }
    }
    if (!itemExists) {
        drawnItemsList.push_back(std::make_pair(randoItem, 1));
    }
}

void PlandomizerSaveSpoilerLog() {
    nlohmann::json spoilerSave;
    std::string filename = lastLoadedSpoiler;

    std::ifstream inputFile(filename);
    if (inputFile.is_open()) {
        inputFile >> spoilerSave;
        inputFile.close();
    }

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
    spoilerLogData.clear();
    plandoLogData.clear();
    spoilerHintData.clear();
    plandoHintData.clear();
    drawnItemsList.clear();

    nlohmann::json spoilerLogInput;
    std::string spoilerStr = "./Randomizer/";
    spoilerStr += logFile.c_str();
    spoilerStr += ".json";

    if (!std::filesystem::exists(spoilerStr)) {
        return;
    }

    std::ifstream file(spoilerStr);

    if (file.is_open()) {
        file >> spoilerLogInput;
        file.close();

        if (spoilerLogInput.contains("file_hash")) {
            auto hash = spoilerLogInput["file_hash"];
            for (auto& load : hash) {
                spoilerHash.push_back(load);
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
}

void PlandomizerDrawItemPopup(uint32_t index) {
    if (shouldPopup && ImGui::BeginPopup("ItemList")) {
        std::sort(drawnItemsList.begin(), drawnItemsList.end(),
              [](const auto& a, const auto& b) {
                auto typeA = a.first.GetItemType();
                auto typeB = b.first.GetItemType();
                  return typeA < typeB;
              });
        ImGui::BeginTable("Item Button Table", 8);
        uint32_t itemIndex = 0;

        bool isClicked = false;
        for (auto& drawSlots : drawnItemsList) {
            ImGui::TableNextColumn();
            ImGui::BeginGroup();
            ImGui::PushID(itemIndex);
            auto itemToDraw = drawSlots.first;
            PlandomizerItemImageCorrection(drawSlots.first);
            if (ImGui::ImageButton(textureID,
                    ImVec2(32.0f, 32.0f), textureUV0, textureUV1, 2.0f, ImVec4(0, 0, 0, 0), itemTint)) {
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
                UIWidgets::Tooltip(drawSlots.first.GetName().english.c_str());
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
        ImGui::EndTable();
        ImGui::EndPopup();
    }
}

void PlandomizerDrawIceTrapPopUp(uint32_t index) {
    if (shouldTrapPopup && ImGui::BeginPopup("TrapList")) {
        ImGui::BeginTable("Ice Trap Table", 8);
        for (auto& items : itemImageMap) {
            ImGui::TableNextColumn();
            ImGui::PushID(items.first);
            PlandomizerItemImageCorrection(Rando::StaticData::RetrieveItem(items.first));
            if (ImGui::ImageButton(textureID, ImVec2(32.0f, 32.0f), textureUV0, textureUV1, 2.0f, ImVec4(0, 0, 0, 0), itemTint)) {
                plandoLogData[index].iceTrapModel = Rando::StaticData::RetrieveItem(items.first);
                ImGui::CloseCurrentPopup();
            };
            UIWidgets::Tooltip(Rando::StaticData::RetrieveItem(items.first).GetName().english.c_str());

            auto itemObject = Rando::StaticData::RetrieveItem(items.first);
            PlandomizerOverlayText(std::make_pair(itemObject, 1));

            ImGui::PopID();
        }
        ImGui::EndTable();
        ImGui::EndPopup();
    }
}

void PlandomizerDrawItemSlots(uint32_t index) {
    ImGui::PushID(index);
    PlandomizerItemImageCorrection(plandoLogData[index].checkRewardItem);
    if (ImGui::ImageButton(textureID, ImVec2(32.0f, 32.0f), textureUV0, textureUV1, 2.0f, ImVec4(0, 0, 0, 0), itemTint)) {
        shouldPopup = true;
        temporaryItem = plandoLogData[index].checkRewardItem;
        ImGui::OpenPopup("ItemList");
    };
    UIWidgets::Tooltip(plandoLogData[index].checkRewardItem.GetName().english.c_str());
    PlandomizerDrawItemPopup(index);
    ImGui::PopID();
}

void PlandomizerDrawShopSlider(uint32_t index) {
    ImGui::PushID(index);
    ImGui::PushItemWidth(190.0f);
    UIWidgets::Spacer(3.0f);
    ImGui::SliderInt("", &plandoLogData[index].shopPrice, 0, 999, "%d Rupees");
    ImGui::PopItemWidth();
    ImGui::PopID();
}

void PlandomizerDrawIceTrapSetup(uint32_t index) {
    char trapTextBuf[35];
    std::strncpy(trapTextBuf, plandoLogData[index].iceTrapName.c_str(), sizeof(trapTextBuf) - 1);

    ImGui::PushID(index);
    PlandomizerItemImageCorrection(plandoLogData[index].iceTrapModel);
    if (ImGui::ImageButton(textureID, ImVec2(32.0f, 32.0f), textureUV0, textureUV1, 2.0f, ImVec4(0, 0, 0, 0), itemTint)) {
        shouldTrapPopup = true;
        ImGui::OpenPopup("TrapList");
    };
    UIWidgets::Tooltip(plandoLogData[index].iceTrapModel.GetName().english.c_str());
    PlandomizerDrawIceTrapPopUp(index);
    ImGui::SameLine();

    
    if (ImGui::InputText("##TrapName", trapTextBuf, 25)) {
        std::string trapName = trapTextBuf;
        plandoLogData[index].iceTrapName = trapName.c_str();
    }

    if (plandoLogData[index].shopPrice > -1) {
        PlandomizerDrawShopSlider(index);
    }
    
    ImGui::PopID();
}

void PlandomizerDrawOptions() {
    ImGui::BeginTable("LoadSpoiler", 2);
    ImGui::TableNextColumn();
    ImGui::SeparatorText("Load/Save Spoiler Log");
    //ImGui::InputText("##logID", logID, 15);
    PlandomizerPopulateSeedList();
    static int32_t selectedList = 0;
    if (ImGui::BeginCombo("##JsonFiles", existingSeedList[selectedList].c_str())) {
        for (size_t i = 0; i < existingSeedList.size(); i++) {
            bool isSelected = (selectedList == i);
            if (ImGui::Selectable(existingSeedList[i].c_str(), isSelected)) {
                selectedList = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::Button("Load")) {
        logTemp = existingSeedList[selectedList].c_str();
        PlandomizerLoadSpoilerLog(logTemp.c_str());
    }
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        PlandomizerSaveSpoilerLog();
    }

    ImGui::TableNextColumn();
    ImGui::SeparatorText("Current Seed Hash");
    if (spoilerLogData.size() > 0) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * 0.5f) - (32.0f * 4.0f));
        int32_t index = 0;
        for (auto& hash : spoilerHash) {
            int32_t roll = rand() % itemMapping.size();
            textureID = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[roll].name);
            ImGui::Image(textureID, ImVec2(32.0f, 32.0f));
            if (index != spoilerHash.size() - 1) {
                ImGui::SameLine();
            }
            index++;
        }
    } else {
        ImGui::Text("No Spoiler Log Loaded");
    }
    ImGui::EndTable();
    
    ImGui::SeparatorText("Options");
    if (plandoLogData.size() == 0) {
        ImGui::Text("Please Load Spoiler Data...");
        return;
    }

    if (getTabID == TAB_HINTS) {
        if (ImGui::Button("Clear All Hints")) {
            PlandomizerRemoveAllHints();
        }
        ImGui::SameLine();
        if (ImGui::Button("Randomize All Hints")) {
            PlandomizerRandomizeHint(HINT_ALL, 0);
        }
    }
    if (getTabID == TAB_LOCATIONS) {
        if (plandoLogData.size() > 0) {
            const char* comboLabel = rcAreaNames[selectedArea].c_str();
            if (selectedArea == RCAREA_INVALID) {
                comboLabel = "All";
            }
            ImGui::PushItemWidth(300.0f);
            if (ImGui::BeginCombo("Filter by Area", comboLabel)) {
                for (const auto& [area, name] : rcAreaNames) {
                    bool isSelected = (selectedArea == area);

                    const char* displayName = name.c_str();
                    if (area == RCAREA_INVALID) {
                        displayName = "All";
                    }
                    if (ImGui::Selectable(displayName, isSelected)) {
                        selectedArea = area;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
    
            ImGui::SameLine();
            if (ImGui::Button("Empty All Rewards")) {
                PlandomizerRemoveAllItems();
            }
        }
    }
}

void PlandomizerDrawHintsWindow() {
    uint32_t index = 0;
    char hintTextBuf[400];

    ImGui::BeginChild("Hints");
    ImGui::BeginTable("Hints Window", 1, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY);
    ImGui::TableSetupColumn("Hint Entries");
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableHeadersRow();
    
    for (auto& hintData : spoilerHintData) {
        ImGui::PushID(index);
        ImGui::TableNextColumn();
        ImGui::SeparatorText(hintData.hintName.c_str());
        ImGui::Text("Default Hint: ");
        ImGui::SameLine();
        ImGui::TextWrapped(hintData.hintText.c_str());

        if (spoilerHintData.size() > 0) {
            std::strncpy(hintTextBuf, plandoHintData[index].hintText.c_str(), sizeof(hintTextBuf) - 1);
        }
        ImGui::Text("New Hint:     ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
        if (ImGui::InputText("##HintMessage", hintTextBuf, 400)) {
            std::string hintMessage = hintTextBuf;
            plandoHintData[index].hintText = hintMessage.c_str();
        }
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * 0.5f) - 75.0f);
        if (ImGui::Button("Randomize Hint")) {
            PlandomizerRandomizeHint(HINT_SINGLE, index);
        }
        
        index++;
        ImGui::PopID();
    }

    ImGui::EndTable();
    ImGui::EndChild();
}

void PlandomizerDrawLocationsWindow(RandomizerCheckArea rcArea) {
    uint32_t index = 0;
    ImGui::BeginChild("Locations");
    ImGui::BeginTable("Locations Window", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY);
    ImGui::TableSetupColumn("Spoiler Log Check Name", ImGuiTableColumnFlags_WidthFixed, 250.0f);
    ImGui::TableSetupColumn("Default Reward", ImGuiTableColumnFlags_WidthFixed, 190.0f);
    ImGui::TableSetupColumn("New Reward", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, 34.0f);
    ImGui::TableSetupColumn("Additional Options");
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableHeadersRow();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

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
            } else if (spoilerData.shopPrice != -1) {
                ImGui::TableNextColumn();
                PlandomizerDrawShopSlider(index);
            } else {
                ImGui::TableNextColumn();
            }
        }
        index++;
    }
    ImGui::PopStyleColor(3);
    ImGui::EndTable();
    ImGui::EndChild();
}

void PlandomizerDrawSpoilerTable() {
    ImGui::BeginChild("Main");
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
    ImGui::EndChild();
}

void PlandomizerWindow::DrawElement() {
    PlandomizerDrawOptions();
    UIWidgets::PaddedSeparator();
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
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("ITEM_TRIFORCE", gEmptyCDownArrowTex, ImVec4( 1, 1, 0, 1 ));
}