#include "gameplaystats.h"

#include "ImGuiImpl.h"

#include <map>
#include <string>
#include <vector>
#include <Cvar.h>
#include <Hooks.h>

extern "C" {
#include <z64.h>
#include "variables.h"
}

char timestampDisplayName[TIMESTAMP_MAX][21] = { "" };

typedef struct {
    char name[21];
    u32 time;
}TimestampInfo;

TimestampInfo timestampDisplay[TIMESTAMP_MAX];

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

void SortChronological(TimestampInfo* arr, size_t len) {
    TimestampInfo temp;
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

void DisplayStat(const char* text, uint32_t value) {
    ImGui::Text(text);
    ImGui::SameLine();
    // Hack to keep the digits properly aligned in the column
           if (value < 10) {
        ImGui::Text("     %u", value);
    } else if (value < 100) {
        ImGui::Text("    %u", value);
    } else if (value < 1000) {
        ImGui::Text("   %u", value);
    } else if (value < 10000) {
        ImGui::Text("  %u", value);
    } else if (value < 100000) {
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

    ImGui::SetNextWindowSize(ImVec2(470, 550), ImGuiCond_Appearing);
    if (!ImGui::Begin("Gameplay Stats", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    u32 totalTimer = GAMEPLAYSTAT_TOTAL_TIME;

    DisplayTimeHHMMSS(totalTimer,     "Total game Time:     ");
    DisplayTimeHHMMSS(gSaveContext.gameplayStats.playTimer / 2,  "Gameplay Time:       ");
    DisplayTimeHHMMSS(gSaveContext.gameplayStats.pauseTimer / 3, "Pause Menu Time:     ");

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 8.0f, 8.0f });
    ImGui::BeginTable("gameStatsTable", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV);

    ImGui::TableSetupColumn("Timestamps", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableSetupColumn("Counts", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableHeadersRow();
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    // Initialize the array to match the one stored in gSaveContext
    for (int i = 0; i < TIMESTAMP_MAX; i++) {
        strcpy(timestampDisplay[i].name, timestampDisplayName[i]);
        timestampDisplay[i].time = gSaveContext.gameplayStats.timestamp[i];
    }

    // Sort it according to time obtained
    SortChronological(timestampDisplay, sizeof(timestampDisplay) / sizeof(timestampDisplay[0]));

    // Display it
    for (int i = 0; i < TIMESTAMP_MAX; i++) {
        // To be shown, the entry must have a time and a string for its display name
        if (timestampDisplay[i].time > 0 && strnlen(timestampDisplay[i].name, 23) > 1) {
            DisplayTimeHHMMSS(timestampDisplay[i].time, timestampDisplay[i].name);
        }
    }

    ImGui::TableNextColumn();

    DisplayStat("Rupees Collected:      ", gSaveContext.gameplayStats.count[COUNT_RUPEES_COLLECTED]);
    DisplayStat("Rupees Spent:          ", gSaveContext.gameplayStats.count[COUNT_RUPEES_SPENT]);
    DisplayStat("Damage Taken:          ", gSaveContext.gameplayStats.count[COUNT_DAMAGE_TAKEN]);
    DisplayStat("Sword Swings:          ", gSaveContext.gameplayStats.count[COUNT_SWORD_SWINGS]);
    DisplayStat("Steps Taken:           ", gSaveContext.gameplayStats.count[COUNT_STEPS]);
    DisplayStat("Rolls:                 ", gSaveContext.gameplayStats.count[COUNT_ROLLS]);
    DisplayStat("Bonks:                 ", gSaveContext.gameplayStats.count[COUNT_BONKS]);
    DisplayStat("Ice Traps:             ", gSaveContext.gameplayStats.count[COUNT_ICE_TRAPS]);
    DisplayStat("Pauses:                ", gSaveContext.gameplayStats.count[COUNT_PAUSES]);
    DisplayStat("Pots Smashed:          ", gSaveContext.gameplayStats.count[COUNT_POTS_BROKEN]);
    DisplayStat("Bushes Cut:            ", gSaveContext.gameplayStats.count[COUNT_BUSHES_CUT]);

    ImGui::PopStyleVar(1);
    ImGui::EndTable();

    ImGui::End();
}

void SetupDisplayNames() {
    
    // Only items in this list will have their times shown in the stat window
    strcpy(timestampDisplayName[ITEM_BOW],              "Fairy Bow:          ");
    strcpy(timestampDisplayName[ITEM_ARROW_FIRE],       "Fire Arrows:        ");
    strcpy(timestampDisplayName[ITEM_DINS_FIRE],        "Din's Fire:         ");
    strcpy(timestampDisplayName[ITEM_SLINGSHOT],        "Slingshot:          ");
    strcpy(timestampDisplayName[ITEM_OCARINA_FAIRY],    "Fairy Ocarina:      ");
    strcpy(timestampDisplayName[ITEM_OCARINA_TIME],     "Ocarina of Time:    ");
    strcpy(timestampDisplayName[ITEM_BOMBCHU],          "Bombchus:           ");
    strcpy(timestampDisplayName[ITEM_HOOKSHOT],         "Hookshot:           ");
    strcpy(timestampDisplayName[ITEM_LONGSHOT],         "Longshot:           ");
    strcpy(timestampDisplayName[ITEM_ARROW_ICE],        "Ice Arrows:         ");
    strcpy(timestampDisplayName[ITEM_FARORES_WIND],     "Farore's Wind:      ");
    strcpy(timestampDisplayName[ITEM_BOOMERANG],        "Boomerang:          ");
    strcpy(timestampDisplayName[ITEM_LENS],             "Lens of Truth:      ");
    strcpy(timestampDisplayName[ITEM_HAMMER],           "Megaton Hammer:     ");
    strcpy(timestampDisplayName[ITEM_ARROW_LIGHT],      "Light Arrows:       ");
    strcpy(timestampDisplayName[ITEM_BOTTLE],           "Bottle:             ");
    strcpy(timestampDisplayName[ITEM_LETTER_ZELDA],     "Zelda's Letter:     ");
    strcpy(timestampDisplayName[ITEM_SWORD_KOKIRI],     "Kokiri Sword:       ");
    strcpy(timestampDisplayName[ITEM_SWORD_MASTER],     "Master Sword:       ");
    strcpy(timestampDisplayName[ITEM_SWORD_BGS],        "Biggoron's Sword:   ");
    strcpy(timestampDisplayName[ITEM_SHIELD_DEKU],      "Deku Shield:        ");
    strcpy(timestampDisplayName[ITEM_SHIELD_HYLIAN],    "Hylian Shield:      ");
    strcpy(timestampDisplayName[ITEM_SHIELD_MIRROR],    "Mirror Shield:      ");
    strcpy(timestampDisplayName[ITEM_TUNIC_GORON],      "Goron Tunic:        ");
    strcpy(timestampDisplayName[ITEM_TUNIC_ZORA],       "Zora Tunic:         ");
    strcpy(timestampDisplayName[ITEM_BOOTS_IRON],       "Iron Boots:         ");
    strcpy(timestampDisplayName[ITEM_BOOTS_HOVER],      "Hover Boots:        ");
    strcpy(timestampDisplayName[ITEM_BOMB_BAG_20],      "Bomb Bag:           ");
    strcpy(timestampDisplayName[ITEM_BRACELET],         "Goron's Bracelet:   ");
    strcpy(timestampDisplayName[ITEM_GAUNTLETS_SILVER], "Silver Gauntlets:   ");
    strcpy(timestampDisplayName[ITEM_GAUNTLETS_GOLD],   "Gold Gauntlets:     ");
    strcpy(timestampDisplayName[ITEM_SCALE_SILVER],     "Silver Scale:       ");
    strcpy(timestampDisplayName[ITEM_SCALE_GOLDEN],     "Gold Scale:         ");
    strcpy(timestampDisplayName[ITEM_WALLET_ADULT],     "Adult's Wallet:     ");
    strcpy(timestampDisplayName[ITEM_WALLET_GIANT],     "Giant's Wallet:     ");
    strcpy(timestampDisplayName[ITEM_SONG_MINUET],      "Minuet of Forest:   ");
    strcpy(timestampDisplayName[ITEM_SONG_BOLERO],      "Bolero of Fire:     ");
    strcpy(timestampDisplayName[ITEM_SONG_SERENADE],    "Serenade of Water:  ");
    strcpy(timestampDisplayName[ITEM_SONG_REQUIEM],     "Requiem of Spirit:  ");
    strcpy(timestampDisplayName[ITEM_SONG_NOCTURNE],    "Nocturne of Shadow: ");
    strcpy(timestampDisplayName[ITEM_SONG_PRELUDE],     "Prelude of Light:   ");
    strcpy(timestampDisplayName[ITEM_SONG_LULLABY],     "Zelda's Lullaby:    ");
    strcpy(timestampDisplayName[ITEM_SONG_EPONA],       "Epona's Song:       ");
    strcpy(timestampDisplayName[ITEM_SONG_SARIA],       "Saria's Song:       ");
    strcpy(timestampDisplayName[ITEM_SONG_SUN],         "Sun's Song:         ");
    strcpy(timestampDisplayName[ITEM_SONG_TIME],        "Song of Time:       ");
    strcpy(timestampDisplayName[ITEM_SONG_STORMS],      "Song of Storms:     ");
    strcpy(timestampDisplayName[ITEM_MEDALLION_FOREST], "Forest Medallion:   ");
    strcpy(timestampDisplayName[ITEM_MEDALLION_FIRE],   "Fire Medallion:     ");
    strcpy(timestampDisplayName[ITEM_MEDALLION_WATER],  "Water Medallion:    ");
    strcpy(timestampDisplayName[ITEM_MEDALLION_SPIRIT], "Spirit Medallion:   ");
    strcpy(timestampDisplayName[ITEM_MEDALLION_SHADOW], "Shadow Medallion:   ");
    strcpy(timestampDisplayName[ITEM_MEDALLION_LIGHT],  "Light Medallion:    ");
    strcpy(timestampDisplayName[ITEM_KOKIRI_EMERALD],   "Kokiri's Emerald:   ");
    strcpy(timestampDisplayName[ITEM_GORON_RUBY],       "Goron's Ruby:       ");
    strcpy(timestampDisplayName[ITEM_ZORA_SAPPHIRE],    "Zora's Sapphire:    ");
    strcpy(timestampDisplayName[ITEM_SINGLE_MAGIC],     "Magic:              ");
    strcpy(timestampDisplayName[ITEM_DOUBLE_DEFENSE],   "Double Defense:     ");

    // Other events
    strcpy(timestampDisplayName[TIMESTAMP_DEFEAT_GOHMA],         "Gohma Defeated:     ");
    strcpy(timestampDisplayName[TIMESTAMP_DEFEAT_KING_DODONGO],  "KD Defeated:        ");
    strcpy(timestampDisplayName[TIMESTAMP_DEFEAT_BARINADE],      "Barinade Defeated:  ");
    strcpy(timestampDisplayName[TIMESTAMP_DEFEAT_PHANTOM_GANON], "PG Defeated:        ");
    strcpy(timestampDisplayName[TIMESTAMP_DEFEAT_VOLVAGIA],      "Volvagia Defeated:  ");
    strcpy(timestampDisplayName[TIMESTAMP_DEFEAT_MORPHA],        "Morpha Defeated:    ");
    strcpy(timestampDisplayName[TIMESTAMP_DEFEAT_BONGO_BONGO],   "Bongo Defeated:     ");
    strcpy(timestampDisplayName[TIMESTAMP_DEFEAT_TWINROVA],      "Twinrova Defeated:  ");
    strcpy(timestampDisplayName[TIMESTAMP_DEFEAT_GANONDORF],     "Ganondorf Defeated: ");
    strcpy(timestampDisplayName[TIMESTAMP_DEFEAT_GANON],         "Ganon Defeated:     ");

}

void InitStatTracker() {
    SohImGui::AddWindow("Enhancements", "Gameplay Stats", DrawStatsTracker);
    SetupDisplayNames();
}
