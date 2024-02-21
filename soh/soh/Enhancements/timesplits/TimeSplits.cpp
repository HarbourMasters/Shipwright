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
std::string splitAttempt = "Attempt #: 0";
static std::string splitNumDisp = "Attempt #: ";
static ImVec4 colorChoice = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
static const char* backgroundColor;
static std::string itemImager;

using json = nlohmann::json;

std::vector<TimeSplitObject> splitObjects = {
    			//	ID,	 Item Name							    Image Name	
TimeSplitObject(	0, 	 "Deku Stick", 							"ITEM_STICK"),
TimeSplitObject(	1, 	 "Deku Nut", 							"ITEM_NUT"),
TimeSplitObject(	3, 	 "Fairy Bow", 							"ITEM_BOW"),
TimeSplitObject(	4, 	 "Fire Arrow", 							"ITEM_ARROW_FIRE"),
TimeSplitObject(	5, 	 "Din's Fire", 							"ITEM_DINS_FIRE"),
TimeSplitObject(	6, 	 "Fairy Slingshot", 					"ITEM_SLINGSHOT"),
TimeSplitObject(	7, 	 "Fairy Ocarina", 						"ITEM_OCARINA_FAIRY"),
TimeSplitObject(	8, 	 "Ocarina of Time", 					"ITEM_OCARINA_TIME"),
TimeSplitObject(	9, 	 "Bombchu", 							"ITEM_BOMBCHU"),
TimeSplitObject(	10,  "Hookshot", 							"ITEM_HOOKSHOT"),
TimeSplitObject(	11,  "Longshot", 							"ITEM_LONGSHOT"),
TimeSplitObject(	12,  "Ice Arrow", 							"ITEM_ARROW_ICE"),
TimeSplitObject(	13,  "Farore's Wind", 						"ITEM_FARORES_WIND"),
TimeSplitObject(	14,  "Boomerang", 							"ITEM_BOOMERANG"),
TimeSplitObject(	15,  "Lens of Truth", 						"ITEM_LENS"),
TimeSplitObject(	16,  "Magic Bean", 							"ITEM_BEAN"),
TimeSplitObject(	17,  "Megaton Hammer", 						"ITEM_HAMMER"),
TimeSplitObject(	18,  "Light Arrow", 						"ITEM_ARROW_LIGHT"),
TimeSplitObject(	19,  "Nayru's Love", 						"ITEM_NAYRUS_LOVE"),
TimeSplitObject(	20,  "Empty Bottle", 						"ITEM_BOTTLE"),
TimeSplitObject(	21,  "Red Potion", 							"ITEM_POTION_RED"),
TimeSplitObject(	22,  "Green Potion", 						"ITEM_POTION_GREEN"),
TimeSplitObject(	23,  "Blue Potion", 						"ITEM_POTION_BLUE"),
TimeSplitObject(	24,  "Bottled Fairy", 						"ITEM_FAIRY"),
TimeSplitObject(	25,  "Fish", 								"ITEM_FISH"),
TimeSplitObject(	26,  "Lon Lon Milk & Bottle", 				"ITEM_MILK_BOTTLE"),
TimeSplitObject(	27,  "Ruto's Letter", 						"ITEM_LETTER_RUTO"),
TimeSplitObject(	28,  "Blue Fire", 							"ITEM_BLUE_FIRE"),
TimeSplitObject(	29,  "Bug", 								"ITEM_BUG"),
TimeSplitObject(	30,  "Big Poe", 							"ITEM_BIG_POE"),
TimeSplitObject(	32,  "Poe", 								"ITEM_POE"),
TimeSplitObject(	33,  "Weird Egg", 							"ITEM_WEIRD_EGG"),
TimeSplitObject(	34,  "Chicken", 							"ITEM_CHICKEN"),
TimeSplitObject(	35,  "Zelda's Letter", 						"ITEM_LETTER_ZELDA"),
TimeSplitObject(	36,  "Keaton Mask", 						"ITEM_MASK_KEATON"),
TimeSplitObject(	37,  "Skull Mask", 							"ITEM_MASK_SKULL"),
TimeSplitObject(	38,  "Spooky Mask", 						"ITEM_MASK_SPOOKY"),
TimeSplitObject(	39,  "Bunny Hood", 							"ITEM_MASK_BUNNY"),
TimeSplitObject(	40,  "Goron Mask", 							"ITEM_MASK_GORON"),
TimeSplitObject(	41,  "Zora Mask", 							"ITEM_MASK_ZORA"),
TimeSplitObject(	42,  "Gerudo Mask", 						"ITEM_MASK_GERUDO"),
TimeSplitObject(	43,  "Mask of Truth", 						"ITEM_MASK_TRUTH"),
TimeSplitObject(	45,  "Pocket Egg", 							"ITEM_POCKET_EGG"),
TimeSplitObject(	46,  "Pocket Cucco", 						"ITEM_POCKET_CUCCO"),
TimeSplitObject(	47,  "Cojiro", 								"ITEM_COJIRO"),
TimeSplitObject(	48,  "Odd Mushroom", 						"ITEM_ODD_MUSHROOM"),
TimeSplitObject(	49,  "Odd Potion", 							"ITEM_ODD_POTION"),
TimeSplitObject(	50,  "Poacher's Saw", 						"ITEM_SAW"),
TimeSplitObject(	51,  "Goron's Sword (Broken)", 				"ITEM_SWORD_BROKEN"),
TimeSplitObject(	52,  "Prescription", 						"ITEM_PRESCRIPTION"),
TimeSplitObject(	53,  "Eyeball Frog", 						"ITEM_FROG"),
TimeSplitObject(	54,  "Eye Drops", 							"ITEM_EYEDROPS"),
TimeSplitObject(	55,  "Claim Check",  						"ITEM_CLAIM_CHECK"),
TimeSplitObject(	59,  "Kokiri Sword", 						"ITEM_SWORD_KOKIRI"),
TimeSplitObject(	60,  "Master Sword", 						"ITEM_SWORD_MASTER"),
TimeSplitObject(	61,  "Giant's Knife & Biggoron's Sword", 	"ITEM_SWORD_BGS"),
TimeSplitObject(	62,  "Deku Shield", 						"ITEM_SHIELD_DEKU"),
TimeSplitObject(	63,  "Hylian Shield", 						"ITEM_SHIELD_HYLIAN"),
TimeSplitObject(	64,  "Mirror Shield", 						"ITEM_SHIELD_MIRROR"),
TimeSplitObject(	65,  "Kokiri Tunic", 						"ITEM_TUNIC_KOKIRI"),
TimeSplitObject(	66,  "Goron Tunic", 						"ITEM_TUNIC_GORON"),
TimeSplitObject(	67,  "Zora Tunic", 							"ITEM_TUNIC_ZORA"),
TimeSplitObject(	68,  "Kokiri Boots", 						"ITEM_BOOTS_KOKIRI"),
TimeSplitObject(	69,  "Iron Boots", 							"ITEM_BOOTS_IRON"),
TimeSplitObject(	70,  "Hover Boots", 						"ITEM_BOOTS_HOVER"),
TimeSplitObject(	71,  "Bullet Bag (30)", 					"ITEM_BULLET_BAG_30"),
TimeSplitObject(	72,  "Bullet Bag (40)", 					"ITEM_BULLET_BAG_40"),
TimeSplitObject(	73,  "Bullet Bag (50)", 					"ITEM_BULLET_BAG_50"),
TimeSplitObject(	74,  "Quiver (30)", 						"ITEM_QUIVER_30"),
TimeSplitObject(	75,  "Big Quiver (40)", 					"ITEM_QUIVER_40"),
TimeSplitObject(	76,  "Biggest Quiver (50)", 				"ITEM_QUIVER_50"),
TimeSplitObject(	77,  "Bomb Bag (20)", 						"ITEM_BOMB_BAG_20"),
TimeSplitObject(	78,  "Big Bomb Bag (30)", 					"ITEM_BOMB_BAG_30"),
TimeSplitObject(	79,  "Biggest Bomb Bag (40)", 				"ITEM_BOMB_BAG_40"),
TimeSplitObject(	80,  "Goron's Bracelet", 					"ITEM_BRACELET"),
TimeSplitObject(	81,  "Silver Gauntlets", 					"ITEM_GAUNTLETS_SILVER"),
TimeSplitObject(	82,  "Golden Gauntlets", 					"ITEM_GAUNTLETS_GOLD"),
TimeSplitObject(	83,  "Silver Scale", 						"ITEM_SCALE_SILVER"),
TimeSplitObject(	84,  "Golden Scale", 						"ITEM_SCALE_GOLDEN"),
TimeSplitObject(	85,  "Giant's Knife (Broken)", 				"ITEM_SWORD_KNIFE"),
TimeSplitObject(	86,  "Adult's Wallet", 						"ITEM_WALLET_ADULT"),
TimeSplitObject(	87,  "Giant's Wallet", 						"ITEM_WALLET_GIANT"),
TimeSplitObject(	89,  "Fishing Pole", 						"ITEM_FISHING_POLE"),
TimeSplitObject(	90,  "Minuet of Forest",					"QUEST_SONG_MINUET"),
TimeSplitObject(	91,  "Bolero of Fire", 						"QUEST_SONG_BOLERO"),
TimeSplitObject(	92,  "Serenade of Water", 					"QUEST_SONG_SERENADE"),
TimeSplitObject(	93,  "Requiem of Spirit", 					"QUEST_SONG_REQUIEM"),
TimeSplitObject(	94,  "Nocturne of Shadow", 					"QUEST_SONG_NOCTURNE"),
TimeSplitObject(	95,  "Prelude of Light", 					"QUEST_SONG_PRELUDE"),
TimeSplitObject(	96,  "Zelda's Lullaby", 					"QUEST_SONG_LULLABY"),
TimeSplitObject(	97,  "Epona's Song", 						"QUEST_SONG_EPONA"),
TimeSplitObject(	98,  "Saria's Song", 						"QUEST_SONG_SARIA"),
TimeSplitObject(	99,  "Sun's Song", 							"QUEST_SONG_SUN"),
TimeSplitObject(	100, "Song of Time", 						"QUEST_SONG_TIME"),
TimeSplitObject(	101, "Song of Storms", 						"QUEST_SONG_STORMS"),
TimeSplitObject(	102, "Forest Medallion", 					"QUEST_MEDALLION_FOREST"),
TimeSplitObject(	103, "Fire Medallion", 						"QUEST_MEDALLION_FIRE"),
TimeSplitObject(	104, "Water Medallion", 					"QUEST_MEDALLION_WATER"),
TimeSplitObject(	105, "Spirit Medallion", 					"QUEST_MEDALLION_SPIRIT"),
TimeSplitObject(	106, "Shadow Medallion", 					"QUEST_MEDALLION_SHADOW"),
TimeSplitObject(	107, "Light Medallion", 					"QUEST_MEDALLION_LIGHT"),
TimeSplitObject(	108, "Kokiri's Emerald", 					"QUEST_KOKIRI_EMERALD"),
TimeSplitObject(	109, "Goron's Ruby", 						"QUEST_GORON_RUBY"),
TimeSplitObject(	110, "Zora's Sapphire", 					"QUEST_ZORA_SAPPHIRE"),
TimeSplitObject(	111, "Stone of Agony", 						"QUEST_STONE_OF_AGONY"),
TimeSplitObject(	112, "Gerudo's Card", 						"QUEST_GERUDO_CARD"),
TimeSplitObject(	123, "Magic Meter", 						"ITEM_MAGIC_SMALL"),
TimeSplitObject(	124, "Double Magic Meter", 				    "ITEM_MAGIC_LARGE"),
TimeSplitObject(	125, "Double Defense", 						"ITEM_HEART_CONTAINER"),
TimeSplitObject(	152, "Deku Stick Upgrade (20)", 			"ITEM_STICK"),
TimeSplitObject(	153, "Deku Stick Upgrade (30)", 			"ITEM_STICK"),
TimeSplitObject(	154, "Deku Nut Upgrade (30)", 				"ITEM_NUT"),
TimeSplitObject(	155, "Deku Nut Upgrade (40)", 				"ITEM_NUT")
};

std::string formatTimestampTimeSplit(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
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
        std::ofstream file("SplitofHarkinian.json");
        file << j.dump(4);
        file.close();
        status = "List has been saved to disk";
        statusColor = COLOR_LIGHT_BLUE;
    }
    ImGui::SameLine(0);
    if (ImGui::Button("Load List")) {
        std::ifstream file("SplitofHarkinian.json");
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
    if (ImGui::ColorEdit4("Background Color", (float*)&colorChoice,
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
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

    uint32_t loopCounter = 0;
    ImGui::TextColored(COLOR_YELLOW, (splitAttempt).c_str());

    ImGui::BeginTable("Splits", 5, ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable);
    ImGui::TableSetupColumn("Item Image", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, 22.0f);
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
            }
        }
        ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemImager), ImVec2(24.0f, 24.0f), ImVec2(0, 0), ImVec2(1, 1));
        ImGui::TableNextColumn();
        // Item Name
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
        ImGui::AlignTextToFramePadding();
        ImGui::Text(SohUtils::itemNames[splitItem[loopCounter]].c_str());
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
        ImGui::PopStyleVar();
        ImGui::TableNextColumn();
        // Previous Best
        ImGui::Text(formatTimestampTimeSplit(splitPreviousBest[loopCounter]).c_str());
    
        loopCounter++;
    }
    ImGui::EndTable();
}

void InitializeSplitFile() {
    if (!std::filesystem::exists("SplitofHarkinian.json")) {
        json j;
        j["splitItem"] = splitItem;
        j["splitTime"] = splitTime;
        j["splitPreviousBest"] = splitPreviousBest;
        j["backgroundColor.r"] = colorChoice.x;
        j["backgroundColor.g"] = colorChoice.y;
        j["backgroundColor.b"] = colorChoice.z;
        j["backgroundColor.a"] = colorChoice.w;
        std::ofstream file("SplitofHarkinian.json");
        file << j.dump(4);
        file.close();
    }
}

static bool initialized = false;

void DrawTimeSplitManageList() {
    uint32_t itemLoop = 0;
    ImGui::BeginTable("Items", 1);
    ImGui::TableNextColumn();
    for (const auto& obj : splitObjects) {
        if (ImGui::ImageButton(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(obj.itemImage), ImVec2(24.0f, 24.0f), ImVec2(0, 0), ImVec2(1, 1))) {
            uint32_t itemReference = obj.itemID;
            splitItem.push_back(itemReference);
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
        ImGui::PopStyleVar();
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
        ImGui::PopStyleColor();
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
        ImGui::BeginChild("Split List");
        DrawTimeSplitManageList();
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    ImGui::End();
    ImGui::PopStyleColor();
}

void TimeSplitWindow::InitElement() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
        TimeSplitSplitsHandler(itemEntry);
    });
}