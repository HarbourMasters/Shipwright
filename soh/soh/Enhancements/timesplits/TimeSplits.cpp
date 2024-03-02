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

extern "C" {
    extern SaveContext gSaveContext;
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
static std::string status;
static ImVec4 statusColor = COLOR_WHITE;
static uint32_t splitCurNum = 0;
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

using json = nlohmann::json;

std::vector<TimeSplitObject> splitObjects = {
 //	ID,	                    Item Name							    Image Name	
{	ITEM_STICK, 	 		"Deku Stick", 							"ITEM_STICK"},
{	ITEM_NUT,				"Deku Nut", 							"ITEM_NUT"},
{	ITEM_BOW, 	 			"Fairy Bow", 							"ITEM_BOW"},
{	ITEM_ARROW_FIRE, 		"Fire Arrow", 							"ITEM_ARROW_FIRE"},
{	ITEM_DINS_FIRE, 		"Din's Fire", 							"ITEM_DINS_FIRE"},
{	ITEM_SLINGSHOT, 		"Fairy Slingshot", 						"ITEM_SLINGSHOT"},
{	ITEM_OCARINA_FAIRY, 	"Fairy Ocarina", 						"ITEM_OCARINA_FAIRY"},
{	ITEM_OCARINA_TIME, 		"Ocarina of Time", 						"ITEM_OCARINA_TIME"},
{	ITEM_BOMBCHU, 	 		"Bombchu", 								"ITEM_BOMBCHU"},
{	ITEM_HOOKSHOT,  		"Hookshot", 							"ITEM_HOOKSHOT"},
{	ITEM_LONGSHOT,  		"Longshot", 							"ITEM_LONGSHOT"},
{	ITEM_ARROW_ICE,  		"Ice Arrow", 							"ITEM_ARROW_ICE"},
{	ITEM_FARORES_WIND,  	"Farore's Wind", 						"ITEM_FARORES_WIND"},
{	ITEM_BOOMERANG,  		"Boomerang", 							"ITEM_BOOMERANG"},
{	ITEM_LENS,  			"Lens of Truth", 						"ITEM_LENS"},
{	ITEM_BEAN,  			"Magic Bean", 							"ITEM_BEAN"},
{	ITEM_HAMMER,  			"Megaton Hammer", 						"ITEM_HAMMER"},
{	ITEM_ARROW_LIGHT,  		"Light Arrow", 							"ITEM_ARROW_LIGHT"},
{	ITEM_NAYRUS_LOVE,  		"Nayru's Love", 						"ITEM_NAYRUS_LOVE"},
{	ITEM_BOTTLE,  			"Empty Bottle", 						"ITEM_BOTTLE"},
{	ITEM_POTION_RED,  		"Red Potion", 							"ITEM_POTION_RED"},
{	ITEM_POTION_GREEN,  	"Green Potion", 						"ITEM_POTION_GREEN"},
{	ITEM_POTION_BLUE,  		"Blue Potion", 							"ITEM_POTION_BLUE"},
{	ITEM_FAIRY,  			"Bottled Fairy", 						"ITEM_FAIRY"},
{	ITEM_FISH,  			"Fish", 								"ITEM_FISH"},
{	ITEM_MILK_BOTTLE,  		"Lon Lon Milk & Bottle", 				"ITEM_MILK_BOTTLE"},
{	ITEM_LETTER_RUTO,  		"Ruto's Letter", 						"ITEM_LETTER_RUTO"},
{	ITEM_BLUE_FIRE,  		"Blue Fire", 							"ITEM_BLUE_FIRE"},
{	ITEM_BUG,  				"Bug", 									"ITEM_BUG"},
{	ITEM_BIG_POE,  			"Big Poe", 								"ITEM_BIG_POE"},
{	ITEM_POE,  				"Poe", 									"ITEM_POE"},
{	ITEM_WEIRD_EGG,  		"Weird Egg", 							"ITEM_WEIRD_EGG"},
{	ITEM_CHICKEN,  			"Chicken", 								"ITEM_CHICKEN"},
{	ITEM_LETTER_ZELDA,  	"Zelda's Letter", 						"ITEM_LETTER_ZELDA"},
{	ITEM_MASK_KEATON,  		"Keaton Mask", 							"ITEM_MASK_KEATON"},
{	ITEM_MASK_SKULL,  		"Skull Mask", 							"ITEM_MASK_SKULL"},
{	ITEM_MASK_SPOOKY,  		"Spooky Mask", 							"ITEM_MASK_SPOOKY"},
{	ITEM_MASK_BUNNY,  		"Bunny Hood", 							"ITEM_MASK_BUNNY"},
{	ITEM_MASK_GORON,  		"Goron Mask", 							"ITEM_MASK_GORON"},
{	ITEM_MASK_ZORA,  		"Zora Mask", 							"ITEM_MASK_ZORA"},
{	ITEM_MASK_GERUDO,  		"Gerudo Mask", 							"ITEM_MASK_GERUDO"},
{	ITEM_MASK_TRUTH,  		"Mask of Truth", 						"ITEM_MASK_TRUTH"},
{	ITEM_POCKET_EGG,  		"Pocket Egg", 							"ITEM_POCKET_EGG"},
{	ITEM_POCKET_CUCCO,  	"Pocket Cucco", 						"ITEM_POCKET_CUCCO"},
{	ITEM_COJIRO,  			"Cojiro", 								"ITEM_COJIRO"},
{	ITEM_ODD_MUSHROOM,  	"Odd Mushroom", 						"ITEM_ODD_MUSHROOM"},
{	ITEM_ODD_POTION,  		"Odd Potion", 							"ITEM_ODD_POTION"},
{	ITEM_SAW,  				"Poacher's Saw", 						"ITEM_SAW"},
{	ITEM_SWORD_BROKEN,  	"Goron's Sword (Broken)", 				"ITEM_SWORD_BROKEN"},
{	ITEM_PRESCRIPTION,  	"Prescription", 						"ITEM_PRESCRIPTION"},
{	ITEM_FROG,  			"Eyeball Frog", 						"ITEM_FROG"},
{	ITEM_EYEDROPS,  		"Eye Drops", 							"ITEM_EYEDROPS"},
{	ITEM_CLAIM_CHECK,  		"Claim Check",  						"ITEM_CLAIM_CHECK"},
{	ITEM_SWORD_KOKIRI,  	"Kokiri Sword", 						"ITEM_SWORD_KOKIRI"},
{	ITEM_SWORD_MASTER,  	"Master Sword", 						"ITEM_SWORD_MASTER"},
{	ITEM_SWORD_BGS,  		"Giant's Knife & Biggoron's Sword", 	"ITEM_SWORD_BGS"},
{	ITEM_SHIELD_DEKU,  		"Deku Shield", 							"ITEM_SHIELD_DEKU"},
{	ITEM_SHIELD_HYLIAN, 	"Hylian Shield", 						"ITEM_SHIELD_HYLIAN"},
{	ITEM_SHIELD_MIRROR, 	"Mirror Shield", 						"ITEM_SHIELD_MIRROR"},
{	ITEM_TUNIC_GORON,  		"Goron Tunic", 							"ITEM_TUNIC_GORON"},
{	ITEM_TUNIC_ZORA,  		"Zora Tunic", 							"ITEM_TUNIC_ZORA"},
{	ITEM_BOOTS_IRON,  		"Iron Boots", 							"ITEM_BOOTS_IRON"},
{	ITEM_BOOTS_HOVER,  		"Hover Boots", 							"ITEM_BOOTS_HOVER"},
{	ITEM_BULLET_BAG_30, 	"Bullet Bag (30)", 						"ITEM_BULLET_BAG_30"},
{	ITEM_BULLET_BAG_40, 	"Bullet Bag (40)", 						"ITEM_BULLET_BAG_40"},
{	ITEM_BULLET_BAG_50, 	"Bullet Bag (50)", 						"ITEM_BULLET_BAG_50"},
{	ITEM_QUIVER_30,  		"Quiver (30)", 							"ITEM_QUIVER_30"},
{	ITEM_QUIVER_40,  		"Big Quiver (40)", 						"ITEM_QUIVER_40"},
{	ITEM_QUIVER_50,  		"Biggest Quiver (50)", 					"ITEM_QUIVER_50"},
{	ITEM_BOMB_BAG_20,  		"Bomb Bag (20)", 						"ITEM_BOMB_BAG_20"},
{	ITEM_BOMB_BAG_30,  		"Big Bomb Bag (30)", 					"ITEM_BOMB_BAG_30"},
{	ITEM_BOMB_BAG_40,  		"Biggest Bomb Bag (40)", 				"ITEM_BOMB_BAG_40"},
{	ITEM_BRACELET,  		"Goron's Bracelet", 					"ITEM_BRACELET"},
{	ITEM_GAUNTLETS_SILVER,  "Silver Gauntlets", 					"ITEM_GAUNTLETS_SILVER"},
{	ITEM_GAUNTLETS_GOLD,  	"Golden Gauntlets", 					"ITEM_GAUNTLETS_GOLD"},
{	ITEM_SCALE_SILVER,  	"Silver Scale", 						"ITEM_SCALE_SILVER"},
{	ITEM_SCALE_GOLDEN,  	"Golden Scale", 						"ITEM_SCALE_GOLDEN"},
{	ITEM_SWORD_KNIFE,  		"Giant's Knife (Broken)", 				"ITEM_SWORD_KNIFE"},
{	ITEM_WALLET_ADULT,  	"Adult's Wallet", 						"ITEM_WALLET_ADULT"},
{	ITEM_WALLET_GIANT,  	"Giant's Wallet", 						"ITEM_WALLET_GIANT"},
{	ITEM_FISHING_POLE,  	"Fishing Pole", 						"ITEM_FISHING_POLE"},
{	ITEM_SONG_MINUET,  		"Minuet of Forest",						"QUEST_SONG_MINUET"},
{	ITEM_SONG_BOLERO,  		"Bolero of Fire", 						"QUEST_SONG_BOLERO"},
{	ITEM_SONG_SERENADE,  	"Serenade of Water", 					"QUEST_SONG_SERENADE"},
{	ITEM_SONG_REQUIEM,  	"Requiem of Spirit", 					"QUEST_SONG_REQUIEM"},
{	ITEM_SONG_NOCTURNE,  	"Nocturne of Shadow", 					"QUEST_SONG_NOCTURNE"},
{	ITEM_SONG_PRELUDE,  	"Prelude of Light", 					"QUEST_SONG_PRELUDE"},
{	ITEM_SONG_LULLABY,  	"Zelda's Lullaby", 						"QUEST_SONG_LULLABY"},
{	ITEM_SONG_EPONA,  		"Epona's Song", 						"QUEST_SONG_EPONA"},
{	ITEM_SONG_SARIA,  		"Saria's Song", 						"QUEST_SONG_SARIA"},
{	ITEM_SONG_SUN,  		"Sun's Song", 							"QUEST_SONG_SUN"},
{	ITEM_SONG_TIME, 		"Song of Time", 						"QUEST_SONG_TIME"},
{	ITEM_SONG_STORMS, 		"Song of Storms", 						"QUEST_SONG_STORMS"},
{	ITEM_MEDALLION_FOREST, 	"Forest Medallion", 					"QUEST_MEDALLION_FOREST"},
{	ITEM_MEDALLION_FIRE, 	"Fire Medallion", 						"QUEST_MEDALLION_FIRE"},
{	ITEM_MEDALLION_WATER, 	"Water Medallion", 						"QUEST_MEDALLION_WATER"},
{	ITEM_MEDALLION_SPIRIT, 	"Spirit Medallion", 					"QUEST_MEDALLION_SPIRIT"},
{	ITEM_MEDALLION_SHADOW, 	"Shadow Medallion", 					"QUEST_MEDALLION_SHADOW"},
{	ITEM_MEDALLION_LIGHT, 	"Light Medallion", 						"QUEST_MEDALLION_LIGHT"},
{	ITEM_KOKIRI_EMERALD, 	"Kokiri's Emerald", 					"QUEST_KOKIRI_EMERALD"},
{	ITEM_GORON_RUBY, 		"Goron's Ruby", 						"QUEST_GORON_RUBY"},
{	ITEM_ZORA_SAPPHIRE, 	"Zora's Sapphire", 						"QUEST_ZORA_SAPPHIRE"},
{	ITEM_STONE_OF_AGONY, 	"Stone of Agony", 						"QUEST_STONE_OF_AGONY"},
{	ITEM_GERUDO_CARD, 		"Gerudo's Card", 						"QUEST_GERUDO_CARD"},
{	ITEM_MAGIC_SMALL, 		"Magic Meter", 							"ITEM_MAGIC_SMALL"},
{	ITEM_MAGIC_LARGE, 		"Double Magic Meter", 				    "ITEM_MAGIC_LARGE"},
{	ITEM_DOUBLE_DEFENSE, 	"Double Defense", 						"ITEM_HEART_CONTAINER"},
{	ITEM_STICK_UPGRADE_20, 	"Deku Stick Upgrade (20)", 				"ITEM_STICK"}, 
{	ITEM_STICK_UPGRADE_30, 	"Deku Stick Upgrade (30)", 				"ITEM_STICK"},
{	ITEM_NUT_UPGRADE_30, 	"Deku Nut Upgrade (30)", 				"ITEM_NUT"},
{	ITEM_NUT_UPGRADE_40, 	"Deku Nut Upgrade (40)", 				"ITEM_NUT"},
{   657,                    "Queen Gohma",                          "SPECIAL_TRIFORCE_PIECE_WHITE"}, 
{   658,                    "King Dodongo",                         "SPECIAL_TRIFORCE_PIECE_WHITE"},
{   659,                    "Barinade",                             "SPECIAL_TRIFORCE_PIECE_WHITE"},
{   660,                    "Phantom Ganon",                        "SPECIAL_TRIFORCE_PIECE_WHITE"},
{   661,                    "Volvagia",                             "SPECIAL_TRIFORCE_PIECE_WHITE"},
{   662,                    "Morpha",                               "SPECIAL_TRIFORCE_PIECE_WHITE"},
{   663,                    "Bongo Bongo",                          "SPECIAL_TRIFORCE_PIECE_WHITE"},
{   664,                    "Twinrova",                             "SPECIAL_TRIFORCE_PIECE_WHITE"},  
{   665,                    "Ganondorf",                            "SPECIAL_TRIFORCE_PIECE_WHITE"},
{   666,                    "Ganon",                                "SPECIAL_TRIFORCE_PIECE_WHITE"},
};

std::string formatTimestampTimeSplit(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
}

void TimeSplitSplitsHandlerS(uint32_t itemID) {
    uint32_t loopCounter = 0;
    for (auto& str : splitItem) {
        if (itemID == splitItem[loopCounter]) {
            splitTime[loopCounter] = GAMEPLAYSTAT_TOTAL_TIME;
            splitStatus[loopCounter] = 1;
            if (splitTime[loopCounter] < splitBest[loopCounter]) {
                splitBest[loopCounter] = splitTime[loopCounter];
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
        if (itemEntry.itemId == splitItem[loopCounter]) {
            splitTime[loopCounter] = GAMEPLAYSTAT_TOTAL_TIME;
            splitStatus[loopCounter] = 1;
            if (splitTime[loopCounter] < splitBest[loopCounter]) {
                splitBest[loopCounter] = splitTime[loopCounter];
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
    if (ImGui::Button("Save List")) {
        std::ifstream existingFile("splitdata.json");
        json existingData;
        existingFile >> existingData;
        existingFile.close();

        existingData[textBuffer]["splitItem"] = splitItem;
        existingData[textBuffer]["splitTime"] = splitTime;
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
    ImGui::SameLine(0);
    if (ImGui::Button("Load List")) {
        std::ifstream file("splitdata.json");
        json j;

        if (file.is_open()) {
            file >> j;
            file.close();
        }
        if (j[textBuffer]["splitItem"].size() > 0) {
            splitItem.clear();
            splitTime.clear();
            splitPreviousBest.clear();
            splitBest.clear();
            splitStatus.clear();

            for (int i = 0; i < j[textBuffer]["splitItem"].size(); i++) {
                splitItem.push_back(0);
                splitTime.push_back(0);
                splitPreviousBest.push_back(0);
                splitBest.push_back(100000);
                splitStatus.push_back(0);
            }
            splitItem = j[textBuffer]["splitItem"].get<std::vector<uint32_t>>();
            splitTime = j[textBuffer]["splitTime"].get<std::vector<uint32_t>>();
            splitPreviousBest = j[textBuffer]["splitPreviousBest"].get<std::vector<uint32_t>>();
            colorChoice.x = j[textBuffer]["backgroundColor.r"];
            colorChoice.y = j[textBuffer]["backgroundColor.g"];
            colorChoice.z = j[textBuffer]["backgroundColor.b"];
            colorChoice.w = j[textBuffer]["backgroundColor.a"];
            file.close();
            status = "List has been loaded from Save Data";
            statusColor = COLOR_LIGHT_BLUE;
        } else {
            status = "List does not exist in Save Data";
            statusColor = COLOR_RED;
        }
        
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset List")) {
        splitItem.clear();
        splitTime.clear();
        splitStatus.clear();
        splitPreviousBest.clear();
        status = "List has been reset";
        statusColor = COLOR_RED;
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
            splitStatus.push_back(0);
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
    }
    ImGui::TextColored(statusColor, status.c_str());
    UIWidgets::PaddedSeparator();
}

void TimeSplitColorTint() {
    switch (itemNum) {
        case 657:
            pieceTint = COLOR_LIGHT_GREEN;
            break;
        case 658:
            pieceTint = COLOR_LIGHT_RED;
            break;
        case 659:
            pieceTint = COLOR_LIGHT_BLUE;
            break;
        case 660:
            pieceTint = COLOR_GREEN;
            break;
        case 661:
            pieceTint = COLOR_RED;
            break;
        case 662:
            pieceTint = COLOR_BLUE;
            break;
        case 663:
            pieceTint = COLOR_PURPLE;
            break;
        case 664:
            pieceTint = COLOR_ORANGE;
            break;
        case 665:
            pieceTint = COLOR_GREY;
            break;
        case 666:
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
    totalPreviousBest = 0;
    for (int i = 0; i < splitItem.size(); i++) {
        totalPreviousBest += splitPreviousBest[i];
    }
    ImGui::TextColored(COLOR_YELLOW, (splitAttempt).c_str());
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 0));
    ImGui::BeginTable("Splits", 5, ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable);
    ImGui::TableSetupColumn("Item Image", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, 27.0f);
    ImGui::TableSetupColumn("Item Name");
    ImGui::TableSetupColumn("Current Time", ImGuiTableColumnFlags_WidthFixed, 90.0f);
    ImGui::TableSetupColumn("+/-", ImGuiTableColumnFlags_WidthFixed, 80.0f);
    ImGui::TableSetupColumn("Prev. Best", ImGuiTableColumnFlags_WidthFixed, 90.0f);
    ImGui::TableHeadersRow();
    for (auto& str : splitItem) {
        ImGui::TableNextColumn();
        // Item Image
        for (const auto& obj : splitObjects) {
            if (obj.itemID == splitItem[loopCounter]) {
                itemNum = obj.itemID;
                itemImager = obj.itemImage;
                itemNamer = obj.itemName;
                if (obj.itemImage == "SPECIAL_TRIFORCE_PIECE_WHITE") {
                    TimeSplitColorTint();
                }
            }
        }
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::ImageButton(std::to_string(buttonID).c_str(), LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemImager),
                               ImVec2(26.0f, 26.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), pieceTint);
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
                }
                splitItem[loopCounter] = draggedItem;
                splitTime[loopCounter] = draggedTime;
                splitBest[loopCounter] = draggedBest;
                splitPreviousBest[loopCounter] = draggedPreviousBest;
                splitStatus[loopCounter] = draggedStatus;
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::TableNextColumn();
        // Item Name
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
        ImGui::AlignTextToFramePadding();
        ImGui::Text(itemNamer.c_str());
        ImGui::TableNextColumn();
        // Current Time
        if (splitTime[loopCounter] == 0) {
            ImGui::Text(formatTimestampTimeSplit(GAMEPLAYSTAT_TOTAL_TIME).c_str());
        } else {
            ImGui::Text(formatTimestampTimeSplit(splitTime[loopCounter]).c_str());
        }
        ImGui::TableNextColumn();
        // +/-
        if (splitStatus[loopCounter] == 0) {
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
        } else {
            if (splitPreviousBest[loopCounter] == 0) {
                ImGui::TextColored(COLOR_GREEN, formatTimestampTimeSplit(splitTime[loopCounter]).c_str());
            } else {
                if (splitTime[loopCounter] < splitPreviousBest[loopCounter]) {
                    ImGui::TextColored(COLOR_GREEN, formatTimestampTimeSplit(splitPreviousBest[loopCounter] - splitTime[loopCounter]).c_str());
                } else if (splitTime[loopCounter] == splitPreviousBest[loopCounter]) {
                    ImGui::TextColored(COLOR_WHITE, formatTimestampTimeSplit(splitTime[loopCounter] - splitPreviousBest[loopCounter]).c_str());
                } else if (splitTime[loopCounter] > splitPreviousBest[loopCounter]) {
                    ImGui::TextColored(COLOR_RED, formatTimestampTimeSplit(splitTime[loopCounter] - splitPreviousBest[loopCounter]).c_str());
                }
            }
        }
        ImGui::TableNextColumn();
        // Previous Best
        ImGui::Text(formatTimestampTimeSplit(splitPreviousBest[loopCounter]).c_str());
        ImGui::PopStyleVar();
        loopCounter++;
        buttonID++;
    }
    ImGui::EndTable();
    ImGui::PopStyleVar(1);
    UIWidgets::PaddedSeparator();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() -
                         ImGui::CalcTextSize(formatTimestampTimeSplit(GAMEPLAYSTAT_TOTAL_TIME).c_str()).x -
                         ImGui::CalcTextSize(formatTimestampTimeSplit(totalPreviousBest).c_str()).x - 60);
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

void InitializeSplitFile() {
    if (!std::filesystem::exists("splitdata.json")) {
        json j;
        j["splitItem"] = splitItem;
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

void DrawTimeSplitManageList() {
    ImGui::BeginTable("Items", 1);
    ImGui::TableNextColumn();
    uint32_t buttonID = 0;
    for (const auto& obj : splitObjects) {
        if (obj.itemImage == "SPECIAL_TRIFORCE_PIECE_WHITE") {
            itemNum = obj.itemID;
            TimeSplitColorTint();
        } else {
            pieceTint = { 1, 1, 1, 1 };
        }
        if (ImGui::ImageButton(std::to_string(buttonID).c_str(),
                               LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage),
                               ImVec2(24.0f, 24.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), pieceTint)) {
            splitItem.push_back(obj.itemID);
            splitTime.push_back(0);
            splitPreviousBest.push_back(0);
            splitBest.push_back(100000);
            splitStatus.push_back(0);
            statusColor = COLOR_GREEN;
            status = obj.itemName + std::string(" added to list");
        }
        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 8.0f));
        ImGui::AlignTextToFramePadding();
        ImGui::Text(obj.itemName);
        ImGui::PopStyleVar(1);
        ImGui::TableNextColumn();
        buttonID++;
    }
    ImGui::EndTable();
}

void TimeSplitWindow::DrawElement() {
    if (!initialized) {
        InitializeSplitFile();
        initialized = true;
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
        DrawTimeSplitManageList();
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    ImGui::End();
    ImGui::PopStyleColor(1);
}

void TimeSplitWindow::InitElement() {
    LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("SPECIAL_TRIFORCE_PIECE_WHITE", gWTriforcePieceTex, ImVec4(1, 1, 1, 1));
    //LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("DPAD_MINUS", gDPadMinusTex, ImVec4(1, 1, 1, 1));
    LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("DPAD_PLUS", gDPadTex, ImVec4(1, 1, 1, 1));
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
        TimeSplitSplitsHandler(itemEntry);
    });
}