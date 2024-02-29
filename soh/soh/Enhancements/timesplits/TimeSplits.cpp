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
static std::string status = "";
static ImVec4 statusColor = COLOR_WHITE;
static uint32_t splitCurNum = 0;
std::string splitAttempt = "Attempt #: 1";
static std::string splitNumDisp = "Attempt #: ";
static ImVec4 colorChoice = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
static const char* backgroundColor;
static std::string itemImager;
static std::string itemNamer;
static uint32_t splitCounter;
static uint32_t totalPreviousBest;

using json = nlohmann::json;

std::vector<TimeSplitObject> splitObjects = {
 //	ID,	 Item Name							    Image Name	
{	0, 	 "Deku Stick", 							"ITEM_STICK"},
{	1, 	 "Deku Nut", 							"ITEM_NUT"},
{	3, 	 "Fairy Bow", 							"ITEM_BOW"},
{	4, 	 "Fire Arrow", 							"ITEM_ARROW_FIRE"},
{	5, 	 "Din's Fire", 							"ITEM_DINS_FIRE"},
{	6, 	 "Fairy Slingshot", 					"ITEM_SLINGSHOT"},
{	7, 	 "Fairy Ocarina", 						"ITEM_OCARINA_FAIRY"},
{	8, 	 "Ocarina of Time", 					"ITEM_OCARINA_TIME"},
{	9, 	 "Bombchu", 							"ITEM_BOMBCHU"},
{	10,  "Hookshot", 							"ITEM_HOOKSHOT"},
{	11,  "Longshot", 							"ITEM_LONGSHOT"},
{	12,  "Ice Arrow", 							"ITEM_ARROW_ICE"},
{	13,  "Farore's Wind", 						"ITEM_FARORES_WIND"},
{	14,  "Boomerang", 							"ITEM_BOOMERANG"},
{	15,  "Lens of Truth", 						"ITEM_LENS"},
{	16,  "Magic Bean", 							"ITEM_BEAN"},
{	17,  "Megaton Hammer", 						"ITEM_HAMMER"},
{	18,  "Light Arrow", 						"ITEM_ARROW_LIGHT"},
{	19,  "Nayru's Love", 						"ITEM_NAYRUS_LOVE"},
{	20,  "Empty Bottle", 						"ITEM_BOTTLE"},
{	21,  "Red Potion", 							"ITEM_POTION_RED"},
{	22,  "Green Potion", 						"ITEM_POTION_GREEN"},
{	23,  "Blue Potion", 						"ITEM_POTION_BLUE"},
{	24,  "Bottled Fairy", 						"ITEM_FAIRY"},
{	25,  "Fish", 								"ITEM_FISH"},
{	26,  "Lon Lon Milk & Bottle", 				"ITEM_MILK_BOTTLE"},
{	27,  "Ruto's Letter", 						"ITEM_LETTER_RUTO"},
{	28,  "Blue Fire", 							"ITEM_BLUE_FIRE"},
{	29,  "Bug", 								"ITEM_BUG"},
{	30,  "Big Poe", 							"ITEM_BIG_POE"},
{	32,  "Poe", 								"ITEM_POE"},
{	33,  "Weird Egg", 							"ITEM_WEIRD_EGG"},
{	34,  "Chicken", 							"ITEM_CHICKEN"},
{	35,  "Zelda's Letter", 						"ITEM_LETTER_ZELDA"},
{	36,  "Keaton Mask", 						"ITEM_MASK_KEATON"},
{	37,  "Skull Mask", 							"ITEM_MASK_SKULL"},
{	38,  "Spooky Mask", 						"ITEM_MASK_SPOOKY"},
{	39,  "Bunny Hood", 							"ITEM_MASK_BUNNY"},
{	40,  "Goron Mask", 							"ITEM_MASK_GORON"},
{	41,  "Zora Mask", 							"ITEM_MASK_ZORA"},
{	42,  "Gerudo Mask", 						"ITEM_MASK_GERUDO"},
{	43,  "Mask of Truth", 						"ITEM_MASK_TRUTH"},
{	45,  "Pocket Egg", 							"ITEM_POCKET_EGG"},
{	46,  "Pocket Cucco", 						"ITEM_POCKET_CUCCO"},
{	47,  "Cojiro", 								"ITEM_COJIRO"},
{	48,  "Odd Mushroom", 						"ITEM_ODD_MUSHROOM"},
{	49,  "Odd Potion", 							"ITEM_ODD_POTION"},
{	50,  "Poacher's Saw", 						"ITEM_SAW"},
{	51,  "Goron's Sword (Broken)", 				"ITEM_SWORD_BROKEN"},
{	52,  "Prescription", 						"ITEM_PRESCRIPTION"},
{	53,  "Eyeball Frog", 						"ITEM_FROG"},
{	54,  "Eye Drops", 							"ITEM_EYEDROPS"},
{	55,  "Claim Check",  						"ITEM_CLAIM_CHECK"},
{	59,  "Kokiri Sword", 						"ITEM_SWORD_KOKIRI"},
{	60,  "Master Sword", 						"ITEM_SWORD_MASTER"},
{	61,  "Giant's Knife & Biggoron's Sword", 	"ITEM_SWORD_BGS"},
{	62,  "Deku Shield", 						"ITEM_SHIELD_DEKU"},
{	63,  "Hylian Shield", 						"ITEM_SHIELD_HYLIAN"},
{	64,  "Mirror Shield", 						"ITEM_SHIELD_MIRROR"},
{	65,  "Kokiri Tunic", 						"ITEM_TUNIC_KOKIRI"},
{	66,  "Goron Tunic", 						"ITEM_TUNIC_GORON"},
{	67,  "Zora Tunic", 							"ITEM_TUNIC_ZORA"},
{	68,  "Kokiri Boots", 						"ITEM_BOOTS_KOKIRI"},
{	69,  "Iron Boots", 							"ITEM_BOOTS_IRON"},
{	70,  "Hover Boots", 						"ITEM_BOOTS_HOVER"},
{	71,  "Bullet Bag (30)", 					"ITEM_BULLET_BAG_30"},
{	72,  "Bullet Bag (40)", 					"ITEM_BULLET_BAG_40"},
{	73,  "Bullet Bag (50)", 					"ITEM_BULLET_BAG_50"},
{	74,  "Quiver (30)", 						"ITEM_QUIVER_30"},
{	75,  "Big Quiver (40)", 					"ITEM_QUIVER_40"},
{	76,  "Biggest Quiver (50)", 				"ITEM_QUIVER_50"},
{	77,  "Bomb Bag (20)", 						"ITEM_BOMB_BAG_20"},
{	78,  "Big Bomb Bag (30)", 					"ITEM_BOMB_BAG_30"},
{	79,  "Biggest Bomb Bag (40)", 				"ITEM_BOMB_BAG_40"},
{	80,  "Goron's Bracelet", 					"ITEM_BRACELET"},
{	81,  "Silver Gauntlets", 					"ITEM_GAUNTLETS_SILVER"},
{	82,  "Golden Gauntlets", 					"ITEM_GAUNTLETS_GOLD"},
{	83,  "Silver Scale", 						"ITEM_SCALE_SILVER"},
{	84,  "Golden Scale", 						"ITEM_SCALE_GOLDEN"},
{	85,  "Giant's Knife (Broken)", 				"ITEM_SWORD_KNIFE"},
{	86,  "Adult's Wallet", 						"ITEM_WALLET_ADULT"},
{	87,  "Giant's Wallet", 						"ITEM_WALLET_GIANT"},
{	89,  "Fishing Pole", 						"ITEM_FISHING_POLE"},
{	90,  "Minuet of Forest",					"QUEST_SONG_MINUET"},
{	91,  "Bolero of Fire", 						"QUEST_SONG_BOLERO"},
{	92,  "Serenade of Water", 					"QUEST_SONG_SERENADE"},
{	93,  "Requiem of Spirit", 					"QUEST_SONG_REQUIEM"},
{	94,  "Nocturne of Shadow", 					"QUEST_SONG_NOCTURNE"},
{	95,  "Prelude of Light", 					"QUEST_SONG_PRELUDE"},
{	96,  "Zelda's Lullaby", 					"QUEST_SONG_LULLABY"},
{	97,  "Epona's Song", 						"QUEST_SONG_EPONA"},
{	98,  "Saria's Song", 						"QUEST_SONG_SARIA"},
{	99,  "Sun's Song", 							"QUEST_SONG_SUN"},
{	100, "Song of Time", 						"QUEST_SONG_TIME"},
{	101, "Song of Storms", 						"QUEST_SONG_STORMS"},
{	102, "Forest Medallion", 					"QUEST_MEDALLION_FOREST"},
{	103, "Fire Medallion", 						"QUEST_MEDALLION_FIRE"},
{	104, "Water Medallion", 					"QUEST_MEDALLION_WATER"},
{	105, "Spirit Medallion", 					"QUEST_MEDALLION_SPIRIT"},
{	106, "Shadow Medallion", 					"QUEST_MEDALLION_SHADOW"},
{	107, "Light Medallion", 					"QUEST_MEDALLION_LIGHT"},
{	108, "Kokiri's Emerald", 					"QUEST_KOKIRI_EMERALD"},
{	109, "Goron's Ruby", 						"QUEST_GORON_RUBY"},
{	110, "Zora's Sapphire", 					"QUEST_ZORA_SAPPHIRE"},
{	111, "Stone of Agony", 						"QUEST_STONE_OF_AGONY"},
{	112, "Gerudo's Card", 						"QUEST_GERUDO_CARD"},
{	123, "Magic Meter", 						"ITEM_MAGIC_SMALL"},
{	124, "Double Magic Meter", 				    "ITEM_MAGIC_LARGE"},
{	125, "Double Defense", 						"ITEM_HEART_CONTAINER"},
{	152, "Deku Stick Upgrade (20)", 			"ITEM_STICK_UPGRADE_20"}, 
{	153, "Deku Stick Upgrade (30)", 			"ITEM_STICK_UPGRADE_30"},
{	154, "Deku Nut Upgrade (30)", 				"ITEM_NUT_UPGRADE_30"},
{	155, "Deku Nut Upgrade (40)", 				"ITEM_NUT_UPGRADE_40"},
{   657, "Queen Gohma",                         "RG_TRIFORCE_PIECE_GOHMA"},
{   658, "King Dodongo",                        "RG_TRIFORCE_PIECE_DODONGO"},
{   659, "Barinade",                            "RG_TRIFORCE_PIECE_BARINADE"},
{   660, "Phantom Ganon",                       "RG_TRIFORCE_PIECE_PHANTOM_GANON"},
{   661, "Volvagia",                            "RG_TRIFORCE_PIECE_VOLVAGIA"},
{   662, "Morpha",                              "RG_TRIFORCE_PIECE_MORPHA"},
{   663, "Bongo Bongo",                         "RG_TRIFORCE_PIECE_BONGO"},
{   664, "Twinrova",                            "RG_TRIFORCE_PIECE_TWINROVA"},  
{   665, "Ganondorf",                           "RG_TRIFORCE_PIECE_GANONDORF"},
{   666, "Ganon",                               "RG_TRIFORCE_PIECE"},
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
    if (ImGui::Button("Reset List")) {
        splitItem.clear();
        splitTime.clear();
        splitStatus.clear();
        splitPreviousBest.clear();
        status = "List has been reset";
        statusColor = COLOR_RED;
    }
    ImGui::SameLine(0);
    if (ImGui::Button("Save List")) {
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
        int itemSize = j["splitItem"].size();
        splitItem.clear();
        splitTime.clear();
        splitPreviousBest.clear();
        splitBest.clear();
        splitStatus.clear();

        for (int i = 0; i < itemSize; i++) {
            splitItem.push_back(0);
            splitTime.push_back(0);
            splitPreviousBest.push_back(0);
            splitBest.push_back(100000);
            splitStatus.push_back(0);
        }
        splitItem = j["splitItem"].get<std::vector<uint32_t>>();
        splitTime = j["splitTime"].get<std::vector<uint32_t>>();
        splitPreviousBest = j["splitPreviousBest"].get<std::vector<uint32_t>>();
        colorChoice.x = j["backgroundColor.r"];
        colorChoice.y = j["backgroundColor.g"];
        colorChoice.z = j["backgroundColor.b"];
        colorChoice.w = j["backgroundColor.a"];
        file.close();
        status = "List has been loaded from Save Data";
        statusColor = COLOR_LIGHT_BLUE;
    }
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
    ImGui::TextColored(statusColor, status.c_str());
    UIWidgets::PaddedSeparator();
}

void DrawTimeSplitSplits(){
    if (ImGui::CollapsingHeader("Time Splitter")) {
        DrawTimeSplitOptions();
    }

    totalPreviousBest = 0;
    for (int i = 0; i < splitItem.size(); i++) {
        totalPreviousBest += splitPreviousBest[i];
    }

    uint32_t loopCounter = 0;
    uint32_t buttonID = 0;
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
                itemImager = obj.itemImage;
                itemNamer = obj.itemName;
            }
        }
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        if (LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemImager)  == nullptr) {
            ImGui::Button(std::to_string(buttonID).c_str());
            buttonID++;
        } else {
            ImGui::ImageButton(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemImager), ImVec2(26.0f, 26.0f), ImVec2(0, 0), ImVec2(1, 1));
        }
        ImGui::PopStyleColor();
        
        if (ImGui::IsItemActive()) {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImGuiCol_HeaderHovered));
            static ImVec2 mousePos = ImGui::GetMousePos();
            float curPos = ImGui::GetMousePos().y;
            if (ImGui::IsMouseDragging(0)) {
                ImVec2 dragDelta = ImGui::GetMouseDragDelta(0);
                int direction = (dragDelta.y < 0.0f) ? -1 : 1;
                int nextIndex = loopCounter + direction;
                if (ImGui::GetMousePos().y >= mousePos.y + 27.4f || ImGui::GetMousePos().y <= mousePos.y - 27.4f) {
                    ImGui::ResetMouseDragDelta(0);
                    mousePos = ImGui::GetMousePos();
                    if (nextIndex >= 0 && nextIndex < splitItem.size()) {
                        uint32_t itemTemp = splitItem[loopCounter];
                        uint32_t timeTemp = splitTime[loopCounter];
                        uint32_t bestTemp = splitBest[loopCounter];
                        uint32_t prevTemp = splitPreviousBest[loopCounter];
                        uint32_t statusTemp = splitStatus[loopCounter];
                        splitItem[loopCounter] = splitItem[nextIndex];
                        splitTime[loopCounter] = splitTime[nextIndex];
                        splitBest[loopCounter] = splitBest[nextIndex];
                        splitPreviousBest[loopCounter] = splitPreviousBest[nextIndex];
                        splitStatus[loopCounter] = splitStatus[nextIndex];
                        splitItem[nextIndex] = itemTemp;
                        splitTime[nextIndex] = timeTemp;
                        splitBest[nextIndex] = bestTemp;
                        splitPreviousBest[nextIndex] - prevTemp;
                        splitStatus[nextIndex] = statusTemp;
                    }
                }
            }
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
    }
    ImGui::EndTable();
    ImGui::PopStyleVar(1);
    
    ImGui::Text(formatTimestampTimeSplit(GAMEPLAYSTAT_TOTAL_TIME).c_str());
    ImGui::SameLine();
    ImGui::Text(" / ");
    ImGui::SameLine();
    ImGui::Text(formatTimestampTimeSplit(totalPreviousBest).c_str());
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
        
        if (LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage) == nullptr) {
            if (ImGui::Button(std::to_string(buttonID).c_str())) {
                splitItem.push_back(obj.itemID);
                splitTime.push_back(0);
                splitPreviousBest.push_back(0);
                splitBest.push_back(100000);
                splitStatus.push_back(0);
                statusColor = COLOR_GREEN;
                status = obj.itemName + std::string(" added to list");
            }
            buttonID++;
        } else {
            if (ImGui::ImageButton(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage), ImVec2(24.0f, 24.0f), ImVec2(0, 0), ImVec2(1, 1))) {
                splitItem.push_back(obj.itemID);
                splitTime.push_back(0);
                splitPreviousBest.push_back(0);
                splitBest.push_back(100000);
                splitStatus.push_back(0);
                statusColor = COLOR_GREEN;
                status = obj.itemName + std::string(" added to list");
        }
        
        }
        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 8.0f));
        ImGui::AlignTextToFramePadding();
        ImGui::Text(obj.itemName);
        ImGui::PopStyleVar(1);
        ImGui::TableNextColumn();
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
        status = "";
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
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
        TimeSplitSplitsHandler(itemEntry);
    });
}