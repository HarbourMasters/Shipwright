#include "gameplaystats.h"

#include "ImGuiImpl.h"
#include "../UIWidgets.hpp"

#include <map>
#include <string>
#include <Cvar.h>
#include <Hooks.h>

extern "C" {
#include <z64.h>
#include "variables.h"
}

#define COLOR_WHITE      ImVec4(1.00f, 1.00f, 1.00f, 1.00f)
#define COLOR_RED        ImVec4(1.00f, 0.00f, 0.00f, 1.00f)
#define COLOR_GREEN      ImVec4(0.10f, 1.00f, 0.10f, 1.00f)
#define COLOR_BLUE       ImVec4(0.00f, 0.33f, 1.00f, 1.00f)
#define COLOR_PURPLE     ImVec4(0.54f, 0.19f, 0.89f, 1.00f)
#define COLOR_YELLOW     ImVec4(1.00f, 1.00f, 0.00f, 1.00f)
#define COLOR_ORANGE     ImVec4(1.00f, 0.67f, 0.11f, 1.00f)
#define COLOR_LIGHT_BLUE ImVec4(0.00f, 0.88f, 1.00f, 1.00f)
#define COLOR_GREY       ImVec4(0.78f, 0.78f, 0.78f, 1.00f)

char timestampDisplayName[TIMESTAMP_MAX][21] = { "" };
ImVec4 timestampDisplayColor[TIMESTAMP_MAX];

typedef struct {
    char name[21];
    u32 time;
    ImVec4 color;
}TimestampInfo;

// Timestamps are an array of structs, each with a name, time, and color
// Names and colors are set up at the bottom of this file
// Times are stored in gSaveContext.sohStats.timestamp
TimestampInfo timestampDisplay[TIMESTAMP_MAX];

void DisplayTimeHHMMSS(uint32_t timeInTenthsOfSeconds, const char* text, ImVec4 color) {

    uint32_t sec = timeInTenthsOfSeconds / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = timeInTenthsOfSeconds % 10;

    ImGui::PushStyleColor(ImGuiCol_Text, color);

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

    ImGui::PopStyleColor();
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

void DisplayStatIfNonZero(const char* text, uint32_t value) {
    if (value > 0) {
        DisplayStat(text, value);
    }
    return;
}

void DrawStatsTracker(bool& open) {
    if (!open) {
        CVar_SetS32("gGameplayStatsEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(480, 550), ImGuiCond_Appearing);
    if (!ImGui::Begin("Gameplay Stats", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    u32 totalTimer = GAMEPLAYSTAT_TOTAL_TIME;
    u32 enemiesDefeated = 0;
    u32 ammoUsed = 0;
    u32 buttonPresses = 0;

    // Sum of all enemies defeated
    for (int i = COUNT_ENEMIES_DEFEATED_ANUBIS; i <= COUNT_ENEMIES_DEFEATED_WOLFOS; i++) {
        if (i == COUNT_ENEMIES_DEFEATED_FLOORMASTER) {
            // Special case: You must kill 3 mini Floormasters for it count as one defeated Floormaster
            enemiesDefeated += gSaveContext.sohStats.count[i] / 3;
        } else {
            enemiesDefeated += gSaveContext.sohStats.count[i];
        }
    }
    // Sum of all ammo used
    for (int i = COUNT_AMMO_USED_STICK; i <= COUNT_AMMO_USED_BEAN; i++) {
        ammoUsed += gSaveContext.sohStats.count[i];
    }
    // Sum of all button presses
    for (int i = COUNT_BUTTON_PRESSES_A; i <= COUNT_BUTTON_PRESSES_START; i++) {
        buttonPresses += gSaveContext.sohStats.count[i];
    }
    // Set up the array of timestamps and then sort it chronologically
    for (int i = 0; i < TIMESTAMP_MAX; i++) {
        strcpy(timestampDisplay[i].name, timestampDisplayName[i]);
        timestampDisplay[i].time = gSaveContext.sohStats.timestamp[i];
        timestampDisplay[i].color = timestampDisplayColor[i];
    }
    SortChronological(timestampDisplay, sizeof(timestampDisplay) / sizeof(timestampDisplay[0]));

    
    // Begin drawing the table and showing the stats

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 8.0f, 8.0f });
    ImGui::BeginTable("timers", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV);
    ImGui::TableSetupColumn("Timers", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableNextColumn();

    DisplayTimeHHMMSS(totalTimer, "Total Game Time:    ", COLOR_WHITE);
    UIWidgets::Tooltip("Timer accuracy may be affected by game performance and loading.");
    DisplayTimeHHMMSS(gSaveContext.sohStats.playTimer / 2, "Gameplay Time:      ", COLOR_WHITE);
    UIWidgets::Tooltip("Timer accuracy may be affected by game performance and loading.");
    DisplayTimeHHMMSS(gSaveContext.sohStats.pauseTimer / 3, "Pause Menu Time:    ", COLOR_WHITE);

    ImGui::PopStyleVar(1);
    ImGui::EndTable();

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 8.0f, 8.0f });
    ImGui::BeginTable("gameStatsTable", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV);

    ImGui::TableSetupColumn("Timestamps", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableSetupColumn("Counts", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableHeadersRow();
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    // Display chronological timestamps of items obtained and bosses defeated
    for (int i = 0; i < TIMESTAMP_MAX; i++) {
        // To be shown, the entry must have a non-zero time and a string for its display name
        if (timestampDisplay[i].time > 0 && strnlen(timestampDisplay[i].name, 21) > 1) {
            DisplayTimeHHMMSS(timestampDisplay[i].time, timestampDisplay[i].name, timestampDisplay[i].color);
        }
    }

    ImGui::TableNextColumn();

    DisplayStat("Enemies Defeated:      ", enemiesDefeated);
    // Show breakdown of enemies defeated in a tree. Only show counts for enemies if they've been defeated at least once.
    if (enemiesDefeated > 0) {
        if (ImGui::TreeNode("Enemy Details...")) {

            DisplayStatIfNonZero("Anubis:             ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_ANUBIS]);
            DisplayStatIfNonZero("Armos:              ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_ARMOS]);
            DisplayStatIfNonZero("Bari:               ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BARI]);
            DisplayStatIfNonZero("Biri:               ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BIRI]);
            DisplayStatIfNonZero("Beamos:             ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BEAMOS]);
            DisplayStatIfNonZero("Big Octo:           ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BIG_OCTO]);
            DisplayStatIfNonZero("Bubble (Blue):      ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_BLUE]);
            DisplayStatIfNonZero("Bubble (Green):     ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_GREEN]);
            DisplayStatIfNonZero("Bubble (Red):       ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_RED]);
            DisplayStatIfNonZero("Bubble (White):     ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_WHITE]);
            DisplayStatIfNonZero("Business Scrub:     ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BUSINESS_SCRUB]);
            DisplayStatIfNonZero("Dark Link:          ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DARK_LINK]);
            DisplayStatIfNonZero("Dead Hand:          ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DEAD_HAND]);
            DisplayStatIfNonZero("Deku Baba:          ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DEKU_BABA]);
            DisplayStatIfNonZero("Deku Baba (Big):    ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DEKU_BABA_BIG]);
            DisplayStatIfNonZero("Deku Scrub:         ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DEKU_SCRUB]);
            DisplayStatIfNonZero("Dinolfos:           ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DINOLFOS]);
            DisplayStatIfNonZero("Dodongo:            ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DODONGO]);
            DisplayStatIfNonZero("Dodongo (Baby):     ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DODONGO_BABY]);
            DisplayStatIfNonZero("Door Mimic:         ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DOOR_TRAP]);
            DisplayStatIfNonZero("Flare Dancer:       ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_FLARE_DANCER]);
            DisplayStatIfNonZero("Floormaster:        ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_FLOORMASTER]/3);
            DisplayStatIfNonZero("Flying Floor Tile:  ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_FLOOR_TILE]);
            DisplayStatIfNonZero("Flying Pot:         ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_FLYING_POT]);
            DisplayStatIfNonZero("Freezard:           ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_FREEZARD]);
            DisplayStatIfNonZero("Gerudo Thief:       ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_GERUDO_THIEF]);
            DisplayStatIfNonZero("Gibdo:              ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_GIBDO]);
            DisplayStatIfNonZero("Gohma Larva:        ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_GOHMA_LARVA]);
            DisplayStatIfNonZero("Guay:               ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_GUAY]);
            DisplayStatIfNonZero("Iron Knuckle:       ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE]);
            DisplayStatIfNonZero("Iron Knuckle (Nab): ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE_NABOORU]);
            DisplayStatIfNonZero("Keese:              ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_KEESE]);
            DisplayStatIfNonZero("Keese (Fire):       ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_KEESE_FIRE]);
            DisplayStatIfNonZero("Keese (Ice):        ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_KEESE_ICE]);
            DisplayStatIfNonZero("Leever:             ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_LEEVER]);
            DisplayStatIfNonZero("Leever (Big):       ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_LEEVER_BIG]);
            DisplayStatIfNonZero("Like-Like:          ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_LIKE_LIKE]);
            DisplayStatIfNonZero("Lizalfos:           ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_LIZALFOS]);
            DisplayStatIfNonZero("Mad Scrub:          ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_MAD_SCRUB]);
            DisplayStatIfNonZero("Moblin:             ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_MOBLIN]);
            DisplayStatIfNonZero("Moblin (Club):      ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_MOBLIN_CLUB]);
            DisplayStatIfNonZero("Octorok:            ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_OCTOROK]);
            DisplayStatIfNonZero("Parasitic Tentacle: ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_PARASITIC_TENTACLE]);
            DisplayStatIfNonZero("Peahat:             ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_PEAHAT]);
            DisplayStatIfNonZero("Peahat Larva:       ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_PEAHAT_LARVA]);
            DisplayStatIfNonZero("Poe:                ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_POE]);
            DisplayStatIfNonZero("Poe (Big):          ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_POE_BIG]);
            DisplayStatIfNonZero("Poe (Composer):     ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_POE_COMPOSER]);
            DisplayStatIfNonZero("Poe Sisters:        ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_POE_SISTERS]);
            DisplayStatIfNonZero("Redead:             ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_REDEAD]);
            DisplayStatIfNonZero("Shabom:             ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SHABOM]);
            DisplayStatIfNonZero("Shellblade:         ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SHELLBLADE]);
            DisplayStatIfNonZero("Skull Kid:          ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SKULL_KID]);
            DisplayStatIfNonZero("Skulltula:          ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA]);
            DisplayStatIfNonZero("Skulltula (Big):    ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA_BIG]);
            DisplayStatIfNonZero("Skulltula (Gold):   ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA_GOLD]);
            DisplayStatIfNonZero("Skullwalltula:      ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SKULLWALLTULA]);
            DisplayStatIfNonZero("Spike:              ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SPIKE]);
            DisplayStatIfNonZero("Stalchild:          ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_STALCHILD]);
            DisplayStatIfNonZero("Stalfos:            ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_STALFOS]);
            DisplayStatIfNonZero("Stinger:            ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_STINGER]);
            DisplayStatIfNonZero("Tailpasaran:        ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_TAILPASARAN]);
            DisplayStatIfNonZero("Tektite (Blue):     ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_TEKTITE_BLUE]);
            DisplayStatIfNonZero("Tektite (Red):      ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_TEKTITE_RED]);
            DisplayStatIfNonZero("Torch Slug:         ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_TORCH_SLUG]);
            DisplayStatIfNonZero("Wallmaster:         ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_WALLMASTER]);
            DisplayStatIfNonZero("Withered Deku Baba: ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_WITHERED_DEKU_BABA]);
            DisplayStatIfNonZero("Wolfos:             ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_WOLFOS]);
            DisplayStatIfNonZero("Wolfos (White):     ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_WOLFOS_WHITE]);
        
        ImGui::NewLine();
        ImGui::TreePop();
        }
    }
    
    DisplayStat("Rupees Collected:      ", gSaveContext.sohStats.count[COUNT_RUPEES_COLLECTED]);
    UIWidgets::Tooltip("Includes rupees collected with a full wallet.");
    DisplayStat("Rupees Spent:          ", gSaveContext.sohStats.count[COUNT_RUPEES_SPENT]);
    DisplayStat("Chests Opened:         ", gSaveContext.sohStats.count[COUNT_CHESTS_OPENED]);

    DisplayStat("Ammo Used:             ", ammoUsed);
    // Show breakdown of ammo used in a collapsible tree. Only show ammo types if they've been used at least once.
    if (ammoUsed > 0) {
        if (ImGui::TreeNode("Ammo Details...")) {

            DisplayStatIfNonZero("Deku Sticks:        ", gSaveContext.sohStats.count[COUNT_AMMO_USED_STICK]);
            DisplayStatIfNonZero("Deku Nuts:          ", gSaveContext.sohStats.count[COUNT_AMMO_USED_NUT]);
            DisplayStatIfNonZero("Deku Seeds:         ", gSaveContext.sohStats.count[COUNT_AMMO_USED_SEED]);
            DisplayStatIfNonZero("Bombs:              ", gSaveContext.sohStats.count[COUNT_AMMO_USED_BOMB]);
            DisplayStatIfNonZero("Bombchus:           ", gSaveContext.sohStats.count[COUNT_AMMO_USED_BOMBCHU]);
            DisplayStatIfNonZero("Arrows:             ", gSaveContext.sohStats.count[COUNT_AMMO_USED_ARROW]);
            DisplayStatIfNonZero("Beans:              ", gSaveContext.sohStats.count[COUNT_AMMO_USED_BEAN]);

            ImGui::NewLine();
            ImGui::TreePop();
        }
    }

    DisplayStat("Damage Taken:          ", gSaveContext.sohStats.count[COUNT_DAMAGE_TAKEN]);
    DisplayStat("Sword Swings:          ", gSaveContext.sohStats.count[COUNT_SWORD_SWINGS]);
    DisplayStat("Steps Taken:           ", gSaveContext.sohStats.count[COUNT_STEPS]);
    DisplayStat("Rolls:                 ", gSaveContext.sohStats.count[COUNT_ROLLS]);
    DisplayStat("Bonks:                 ", gSaveContext.sohStats.count[COUNT_BONKS]);
    DisplayStat("Ice Traps:             ", gSaveContext.sohStats.count[COUNT_ICE_TRAPS]);
    DisplayStat("Pauses:                ", gSaveContext.sohStats.count[COUNT_PAUSES]);
    DisplayStat("Pots Smashed:          ", gSaveContext.sohStats.count[COUNT_POTS_BROKEN]);
    DisplayStat("Bushes Cut:            ", gSaveContext.sohStats.count[COUNT_BUSHES_CUT]);

    DisplayStat("Buttons Pressed:       ", buttonPresses);
    // Show breakdown of ammo used in a collapsible tree. Only show ammo types if they've been used at least once.
    if (buttonPresses > 0) {
        if (ImGui::TreeNode("Buttons...")) {

            DisplayStatIfNonZero("A:                  ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_A]);
            DisplayStatIfNonZero("B:                  ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_B]);
            DisplayStatIfNonZero("L:                  ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_L]);
            DisplayStatIfNonZero("R:                  ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_R]);
            DisplayStatIfNonZero("Z:                  ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_Z]);
            DisplayStatIfNonZero("C-Up:               ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_CUP]);
            DisplayStatIfNonZero("C-Right:            ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_CRIGHT]);
            DisplayStatIfNonZero("C-Down:             ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_CDOWN]);
            DisplayStatIfNonZero("C-Left:             ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_CLEFT]);
            DisplayStatIfNonZero("D-Up:               ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_DUP]);
            DisplayStatIfNonZero("D-Right:            ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_DRIGHT]);
            DisplayStatIfNonZero("D-Down:             ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_DDOWN]);
            DisplayStatIfNonZero("D-Left:             ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_DLEFT]);
            DisplayStatIfNonZero("Start:              ", gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_START]);

            ImGui::NewLine();
            ImGui::TreePop();
        }
    }

    ImGui::PopStyleVar(1);
    ImGui::EndTable();

    ImGui::Text("Note: Gameplay stats are saved to the current file and will be\nlost if you quit without saving.");

    ImGui::End();
}

// Entries listed here will have a timestamp shown in the stat window
void SetupDisplayNames() {
    // To add a timestamp for an item or event, add it to this list and ensure
    // it has a corresponding entry in the enum (see gameplaystats.h)

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

void SetupDisplayColors() {
    for (int i = 0; i < TIMESTAMP_MAX; i++) {
        switch (i) {
            case ITEM_SONG_MINUET:
            case ITEM_KOKIRI_EMERALD:
            case ITEM_SONG_SARIA:
            case ITEM_MEDALLION_FOREST:
                timestampDisplayColor[i] = COLOR_GREEN;
                break;
            case ITEM_SONG_BOLERO:
            case ITEM_GORON_RUBY:
            case ITEM_MEDALLION_FIRE:
                timestampDisplayColor[i] = COLOR_RED;
                break;
            case ITEM_SONG_SERENADE:
            case ITEM_ZORA_SAPPHIRE:
            case ITEM_MEDALLION_WATER:
                timestampDisplayColor[i] = COLOR_BLUE;
                break;
            case ITEM_SONG_LULLABY:
            case ITEM_SONG_NOCTURNE:
            case ITEM_MEDALLION_SHADOW:
                timestampDisplayColor[i] = COLOR_PURPLE;
                break;
            case ITEM_SONG_EPONA:
            case ITEM_SONG_REQUIEM:
            case ITEM_MEDALLION_SPIRIT:
                timestampDisplayColor[i] = COLOR_ORANGE;
                break;
            case ITEM_SONG_SUN:
            case ITEM_SONG_PRELUDE:
            case ITEM_MEDALLION_LIGHT:
            case ITEM_ARROW_LIGHT:
                timestampDisplayColor[i] = COLOR_YELLOW;
                break;
            case ITEM_SONG_STORMS:
                timestampDisplayColor[i] = COLOR_GREY;
                break;
            case ITEM_SONG_TIME:
                timestampDisplayColor[i] = COLOR_LIGHT_BLUE;
                break;
            default:
                timestampDisplayColor[i] = COLOR_WHITE;
                break;
        }
    }
}

void InitStatTracker() {
    SohImGui::AddWindow("Enhancements", "Gameplay Stats", DrawStatsTracker);
    SetupDisplayNames();
    SetupDisplayColors();
}