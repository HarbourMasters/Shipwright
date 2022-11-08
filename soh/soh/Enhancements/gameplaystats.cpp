#include "gameplaystats.h"

#include "libultraship/ImGuiImpl.h"

#include <map>
#include <string>
#include <vector>
#include <libultraship/Cvar.h>
#include <libultraship/Hooks.h>

extern "C" {
#include <z64.h>
#include "variables.h"
}

char itemDisplayName[ITEM_LAST_USED][23] = { "" };

typedef struct {
    char name[23];
    u32 time;
}ItemDisplayTimes;

ItemDisplayTimes itemStatDisplay[ITEM_LAST_USED];

void DisplayTimeHHMMSS(uint32_t timeInTenthsOfSeconds, const char* text) {

    uint32_t sec = timeInTenthsOfSeconds / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = timeInTenthsOfSeconds % 10;

    ImGui::Text(text);
    ImGui::SameLine();

    // Hack to keep the timers aligned and prevent them from shifting around
    // Put a leading zero in front of the seconds or minutes if they're less than 10
    if (mm < 10 && ss < 10) {
        ImGui::Text("%u:0%u:0%u.%u", hh, mm, ss, ds);
    }
    if (mm < 10 && ss >= 10) {
        ImGui::Text("%u:0%u:%u.%u", hh, mm, ss, ds);
    }
    if (mm >= 10 && ss < 10) {
        ImGui::Text("%u:%u:0%u.%u", hh, mm, ss, ds);
    }
    if (mm >= 10 && ss >= 10) {
        ImGui::Text("%u:%u:%u.%u", hh, mm, ss, ds);
    }
}

void SortChronological(ItemDisplayTimes* arr, size_t len) {
    ItemDisplayTimes temp;
    for (int i = 0; i < len; i++) {
        for (int j = 0; j + 1 < len - i; j++) {
            if (arr[j].time > arr[j + 1].time) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void DisplayStat(const char* text, uint16_t value) {
    ImGui::Text(text);
    ImGui::SameLine();
    // Hack to keep the digits properly aligned in the column
    if (value < 10) {
        ImGui::Text("    %u", value);
    } else if (value < 100) {
        ImGui::Text("   %u", value);
    } else if (value < 1000) {
        ImGui::Text("  %u", value);
    } else if (value < 10000) {
        ImGui::Text(" %u", value);
    } else {
        ImGui::Text("%u", value);
    }
}

void DrawStatsTracker(bool& open) {
    if (!open) {
        CVar_SetS32("gGameplayStatsEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(470, 600), ImGuiCond_Appearing);
    if (!ImGui::Begin("Gameplay Stats", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    DisplayTimeHHMMSS(gSaveContext.gameplayStats.totalTimer,     "Total game Time:     ");
    DisplayTimeHHMMSS(gSaveContext.gameplayStats.playTimer / 2,  "Gameplay Time:       ");
    DisplayTimeHHMMSS(gSaveContext.gameplayStats.pauseTimer / 3, "Pause Menu Time:     ");

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 8.0f, 8.0f });
    ImGui::BeginTable("gameStatsTable", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV);

    ImGui::TableSetupColumn("Time Obtained", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableSetupColumn("Counts", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableHeadersRow();
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    // Initialize the array to match the one stored in gSaveContext
    for (int i = 0; i < ITEM_LAST_USED; i++) {
        strcpy(itemStatDisplay[i].name, itemDisplayName[i]);
        itemStatDisplay[i].time = gSaveContext.gameplayStats.itemGetTime[i];
    }

    // Sort it according to time obtained
    SortChronological(itemStatDisplay, sizeof(itemStatDisplay) / sizeof(itemStatDisplay[0]));

    // Display it
    for (int i = 0; i < ITEM_LAST_USED; i++) {
        // To be shown, the item must have a time and a non-empty string for its display name
        if (itemStatDisplay[i].time > 0 && strnlen(itemStatDisplay[i].name, 23) > 1) {
            DisplayTimeHHMMSS(itemStatDisplay[i].time, itemStatDisplay[i].name);
        }
    }

    ImGui::TableNextColumn();

    DisplayStat("Rupees Collected:       ", gSaveContext.gameplayStats.rupeesCollected);
    DisplayStat("Rupees Spent:           ", gSaveContext.gameplayStats.rupeesSpent);
    DisplayStat("Damage Taken:           ", gSaveContext.gameplayStats.damageTaken);
    DisplayStat("Rolls:                  ", gSaveContext.gameplayStats.rollCount);
    DisplayStat("Bonks:                  ", gSaveContext.gameplayStats.bonkCount);
    DisplayStat("Ice Traps:              ", gSaveContext.gameplayStats.iceTrapCount);
    DisplayStat("Pauses:                 ", gSaveContext.gameplayStats.pauseCount);

    ImGui::PopStyleVar(1);
    ImGui::EndTable();

    ImGui::End();
}

void SetupDisplayNames() {
    
    // Only items in this list will have their times shown in the stat window
    strcpy(itemDisplayName[ITEM_BOW],              "Fairy Bow:          ");
    strcpy(itemDisplayName[ITEM_ARROW_FIRE],       "Fire Arrows:        ");
    strcpy(itemDisplayName[ITEM_DINS_FIRE],        "Din's Fire:         ");
    strcpy(itemDisplayName[ITEM_SLINGSHOT],        "Slingshot:          ");
    strcpy(itemDisplayName[ITEM_OCARINA_FAIRY],    "Fairy Ocarina:      ");
    strcpy(itemDisplayName[ITEM_OCARINA_TIME],     "Ocarina of Time:    ");
    strcpy(itemDisplayName[ITEM_BOMBCHU],          "Bombchus:           ");
    strcpy(itemDisplayName[ITEM_HOOKSHOT],         "Hookshot:           ");
    strcpy(itemDisplayName[ITEM_LONGSHOT],         "Longshot:           ");
    strcpy(itemDisplayName[ITEM_ARROW_ICE],        "Ice Arrows:         ");
    strcpy(itemDisplayName[ITEM_FARORES_WIND],     "Farore's Wind:      ");
    strcpy(itemDisplayName[ITEM_BOOMERANG],        "Boomerang:          ");
    strcpy(itemDisplayName[ITEM_LENS],             "Lens of Truth:      ");
    strcpy(itemDisplayName[ITEM_HAMMER],           "Megaton Hammer:     ");
    strcpy(itemDisplayName[ITEM_ARROW_LIGHT],      "Light Arrows:       ");
    strcpy(itemDisplayName[ITEM_BOTTLE],           "Bottle:             ");
    strcpy(itemDisplayName[ITEM_LETTER_ZELDA],     "Zelda's Letter:     ");
    strcpy(itemDisplayName[ITEM_SWORD_KOKIRI],     "Kokiri Sword:       ");
    strcpy(itemDisplayName[ITEM_SWORD_MASTER],     "Master Sword:       ");
    strcpy(itemDisplayName[ITEM_SWORD_BGS],        "Biggoron's Sword:   ");
    strcpy(itemDisplayName[ITEM_SHIELD_DEKU],      "Deku Shield:        ");
    strcpy(itemDisplayName[ITEM_SHIELD_HYLIAN],    "Hylian Shield:      ");
    strcpy(itemDisplayName[ITEM_SHIELD_MIRROR],    "Mirror Shield:      ");
    strcpy(itemDisplayName[ITEM_TUNIC_GORON],      "Goron Tunic:        ");
    strcpy(itemDisplayName[ITEM_TUNIC_ZORA],       "Zora Tunic:         ");
    strcpy(itemDisplayName[ITEM_BOOTS_IRON],       "Iron Boots:         ");
    strcpy(itemDisplayName[ITEM_BOOTS_HOVER],      "Hover Boots:        ");
    strcpy(itemDisplayName[ITEM_BOMB_BAG_20],      "Bomb Bag:           ");
    strcpy(itemDisplayName[ITEM_BRACELET],         "Goron's Bracelet:   ");
    strcpy(itemDisplayName[ITEM_GAUNTLETS_SILVER], "Silver Gauntlets:   ");
    strcpy(itemDisplayName[ITEM_GAUNTLETS_GOLD],   "Gold Gauntlets:     ");
    strcpy(itemDisplayName[ITEM_SCALE_SILVER],     "Silver Scale:       ");
    strcpy(itemDisplayName[ITEM_SCALE_GOLDEN],     "Gold Scale:         ");
    strcpy(itemDisplayName[ITEM_WALLET_ADULT],     "Adult's Wallet:     ");
    strcpy(itemDisplayName[ITEM_WALLET_GIANT],     "Giant's Wallet:     ");
    strcpy(itemDisplayName[ITEM_SONG_MINUET],      "Minuet of Forest:   ");
    strcpy(itemDisplayName[ITEM_SONG_BOLERO],      "Bolero of Fire:     ");
    strcpy(itemDisplayName[ITEM_SONG_SERENADE],    "Serenade of Water:  ");
    strcpy(itemDisplayName[ITEM_SONG_REQUIEM],     "Requiem of Spirit:  ");
    strcpy(itemDisplayName[ITEM_SONG_NOCTURNE],    "Nocturne of Shadow: ");
    strcpy(itemDisplayName[ITEM_SONG_PRELUDE],     "Prelude of Light:   ");
    strcpy(itemDisplayName[ITEM_SONG_LULLABY],     "Zelda's Lullaby:    ");
    strcpy(itemDisplayName[ITEM_SONG_EPONA],       "Epona's Song:       ");
    strcpy(itemDisplayName[ITEM_SONG_SARIA],       "Saria's Song:       ");
    strcpy(itemDisplayName[ITEM_SONG_SUN],         "Sun's Song:         ");
    strcpy(itemDisplayName[ITEM_SONG_TIME],        "Song of Time:       ");
    strcpy(itemDisplayName[ITEM_SONG_STORMS],      "Song of Storms:     ");
    strcpy(itemDisplayName[ITEM_MEDALLION_FOREST], "Forest Medallion:   ");
    strcpy(itemDisplayName[ITEM_MEDALLION_FIRE],   "Fire Medallion:     ");
    strcpy(itemDisplayName[ITEM_MEDALLION_WATER],  "Water Medallion:    ");
    strcpy(itemDisplayName[ITEM_MEDALLION_SPIRIT], "Spirit Medallion:   ");
    strcpy(itemDisplayName[ITEM_MEDALLION_SHADOW], "Shadow Medallion:   ");
    strcpy(itemDisplayName[ITEM_MEDALLION_LIGHT],  "Light Medallion:    ");
    strcpy(itemDisplayName[ITEM_KOKIRI_EMERALD],   "Kokiri's Emerald:   ");
    strcpy(itemDisplayName[ITEM_GORON_RUBY],       "Goron's Ruby:       ");
    strcpy(itemDisplayName[ITEM_ZORA_SAPPHIRE],    "Zora's Sapphire:    ");
    strcpy(itemDisplayName[ITEM_SINGLE_MAGIC],     "Magic:              ");
    strcpy(itemDisplayName[ITEM_DOUBLE_DEFENSE],   "Double Defense:     ");
}

void InitStatTracker() {
    SohImGui::AddWindow("Enhancements", "Gameplay Stats", DrawStatsTracker);
    SetupDisplayNames();
}