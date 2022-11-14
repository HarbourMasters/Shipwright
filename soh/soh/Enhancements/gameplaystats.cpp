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
        if (timestampDisplay[i].time > 0 && strnlen(timestampDisplay[i].name, 21) > 1) {
            DisplayTimeHHMMSS(timestampDisplay[i].time, timestampDisplay[i].name);
        }
    }

    ImGui::TableNextColumn();

    for (int i = COUNT_ENEMIES_DEFEATED_ANUBIS; i <= COUNT_ENEMIES_DEFEATED_WOLFOS; i++) {
        enemiesDefeated += gSaveContext.gameplayStats.count[i];
    }
    for (int i = COUNT_AMMO_USED_STICK; i <= COUNT_AMMO_USED_BEAN; i++) {
        ammoUsed += gSaveContext.gameplayStats.count[i];
    }

    DisplayStat("Enemies Defeated:      ", enemiesDefeated);
    if (enemiesDefeated > 0) {
        if (ImGui::TreeNode("Enemy Details...")) {

            DisplayStatIfNonZero("Anubis:            ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_ANUBIS]);
            DisplayStatIfNonZero("Armos:             ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_ARMOS]);
            DisplayStatIfNonZero("Bari:              ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_BARI]);
            DisplayStatIfNonZero("Biri:              ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_BIRI]);
            DisplayStatIfNonZero("Beamos:            ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_BEAMOS]);
            DisplayStatIfNonZero("Big Octo:          ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_BIG_OCTO]);
            DisplayStatIfNonZero("Bubble (Blue):     ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_BLUE]);
            DisplayStatIfNonZero("Bubble (Green):    ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_GREEN]);
            DisplayStatIfNonZero("Bubble (Red):      ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_RED]);
            DisplayStatIfNonZero("Bubble (White):    ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_WHITE]);
            DisplayStatIfNonZero("Business Scrub:    ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_BUSINESS_SCRUB]);
            DisplayStatIfNonZero("Dark Link:         ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_DARK_LINK]);
            DisplayStatIfNonZero("Dead Hand:         ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_DEAD_HAND]);
            DisplayStatIfNonZero("Deku Baba:         ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_DEKU_BABA]);
            DisplayStatIfNonZero("Deku Baba (Big):   ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_DEKU_BABA_BIG]);
            DisplayStatIfNonZero("Deku Scrub:        ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_DEKU_SCRUB]);
            DisplayStatIfNonZero("Dinolfos:          ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_DINOLFOS]);
            DisplayStatIfNonZero("Dodongo:           ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_DODONGO]);
            DisplayStatIfNonZero("Dodongo (Baby):    ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_DODONGO_BABY]);
            DisplayStatIfNonZero("Door Mimic:        ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_DOOR_TRAP]);
            DisplayStatIfNonZero("Flare Dancer:      ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_FLARE_DANCER]);
            DisplayStatIfNonZero("Floormaster:       ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_FLOORMASTER]);
            DisplayStatIfNonZero("Flying Floor Tile: ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_FLOOR_TILE]);
            DisplayStatIfNonZero("Flying Pot:        ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_FLYING_POT]);
            DisplayStatIfNonZero("Freezard:          ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_FREEZARD]);
            DisplayStatIfNonZero("Gibdo:             ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_GIBDO]);
            DisplayStatIfNonZero("Gohma Larva:       ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_GOHMA_LARVA]);
            DisplayStatIfNonZero("Guay:              ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_GUAY]);
            DisplayStatIfNonZero("Iron Knuckle:      ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE]);
            DisplayStatIfNonZero("Iron Knuckle (Nab):", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE_NABOORU]);
            DisplayStatIfNonZero("Keese:             ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_KEESE]);
            DisplayStatIfNonZero("Keese (Fire):      ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_KEESE_FIRE]);
            DisplayStatIfNonZero("Keese (Ice):       ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_KEESE_ICE]);
            DisplayStatIfNonZero("Leever:            ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_LEEVER]);
            DisplayStatIfNonZero("Leever (Big):      ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_LEEVER_BIG]);
            DisplayStatIfNonZero("Like-Like:         ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_LIKE_LIKE]);
            DisplayStatIfNonZero("Lizalfos:          ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_LIZALFOS]);
            DisplayStatIfNonZero("Mad Scrub:         ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_MAD_SCRUB]);
            DisplayStatIfNonZero("Moblin:            ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_MOBLIN]);
            DisplayStatIfNonZero("Moblin (Club):     ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_MOBLIN_CLUB]);
            DisplayStatIfNonZero("Octorok:           ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_OCTOROK]);
            DisplayStatIfNonZero("Parasitic Tentacle:", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_PARASITIC_TENTACLE]);
            DisplayStatIfNonZero("Peahat:            ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_PEAHAT]);
            DisplayStatIfNonZero("Peahat Larva:      ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_PEAHAT_LARVA]);
            DisplayStatIfNonZero("Poe:               ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_POE]);
            DisplayStatIfNonZero("Poe (Big):         ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_POE_BIG]);
            DisplayStatIfNonZero("Poe (Composer):    ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_POE_COMPOSER]);
            DisplayStatIfNonZero("Poe Sisters:       ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_POE_SISTERS]);
            DisplayStatIfNonZero("Redead:            ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_REDEAD]);
            DisplayStatIfNonZero("Shabom:            ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_SHABOM]);
            DisplayStatIfNonZero("Shellblade:        ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_SHELLBLADE]);
            DisplayStatIfNonZero("Skull Kid:         ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_SKULL_KID]);
            DisplayStatIfNonZero("Skulltula:         ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA]);
            DisplayStatIfNonZero("Skulltula (Big):   ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA_BIG]);
            DisplayStatIfNonZero("Skulltula (Gold):  ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA_GOLD]);
            DisplayStatIfNonZero("Skullwalltula:     ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_SKULLWALLTULA]);
            DisplayStatIfNonZero("Spike:             ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_SPIKE]);
            DisplayStatIfNonZero("Stalchild:         ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_STALCHILD]);
            DisplayStatIfNonZero("Stalfos:           ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_STALFOS]);
            DisplayStatIfNonZero("Stinger:           ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_STINGER]);
            DisplayStatIfNonZero("Tailpasaran:       ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_TAILPASARAN]);
            DisplayStatIfNonZero("Tektite (Blue):    ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_TEKTITE_BLUE]);
            DisplayStatIfNonZero("Tektite (Red):     ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_TEKTITE_RED]);
            DisplayStatIfNonZero("Torch Slug:        ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_TORCH_SLUG]);
            DisplayStatIfNonZero("Wallmaster:        ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_WALLMASTER]);
            DisplayStatIfNonZero("Withered Deku Baba:", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_WITHERED_DEKU_BABA]);
            DisplayStatIfNonZero("Wolfos:            ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_WOLFOS]);
            DisplayStatIfNonZero("Wolfos (White):    ", gSaveContext.gameplayStats.count[COUNT_ENEMIES_DEFEATED_WOLFOS_WHITE]);
        
        ImGui::NewLine();
        ImGui::TreePop();
        }
    }
    
    DisplayStat("Rupees Collected:      ", gSaveContext.gameplayStats.count[COUNT_RUPEES_COLLECTED]);
    DisplayStat("Rupees Spent:          ", gSaveContext.gameplayStats.count[COUNT_RUPEES_SPENT]);
    DisplayStat("Chests Opened:         ", gSaveContext.gameplayStats.count[COUNT_CHESTS_OPENED]);

    DisplayStat("Ammo Used:             ", ammoUsed);
    if (ammoUsed > 0) {
        if (ImGui::TreeNode("Ammo Details...")) {

            DisplayStatIfNonZero("Deku Sticks:       ", gSaveContext.gameplayStats.count[COUNT_AMMO_USED_STICK]);
            DisplayStatIfNonZero("Deku Nuts:         ", gSaveContext.gameplayStats.count[COUNT_AMMO_USED_NUT]);
            DisplayStatIfNonZero("Deku Seeds:        ", gSaveContext.gameplayStats.count[COUNT_AMMO_USED_SEED]);
            DisplayStatIfNonZero("Bombs:             ", gSaveContext.gameplayStats.count[COUNT_AMMO_USED_BOMB]);
            DisplayStatIfNonZero("Bombchus:          ", gSaveContext.gameplayStats.count[COUNT_AMMO_USED_BOMBCHU]);
            DisplayStatIfNonZero("Arrows:            ", gSaveContext.gameplayStats.count[COUNT_AMMO_USED_ARROW]);
            DisplayStatIfNonZero("Beans:             ", gSaveContext.gameplayStats.count[COUNT_AMMO_USED_BEAN]);

            ImGui::NewLine();
            ImGui::TreePop();
        }
    }

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