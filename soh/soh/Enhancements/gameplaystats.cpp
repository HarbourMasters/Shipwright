extern "C" {
#include "gameplaystats.h"
}

#include "Gui.h"
#include "../UIWidgets.hpp"

#include <vector>
#include <string>
#include <libultraship/bridge.h>
#include <Hooks.h>

extern "C" {
#include <z64.h>
#include "variables.h"
extern PlayState* gPlayState;
}

const std::vector<std::string> sceneMappings = {
    {"Inside the Deku Tree"},
    {"Dodongo's Cavern"},
    {"Inside Jabu-Jabu's Belly"},
    {"Forest Temple"},
    {"Fire Temple"},
    {"Water Temple"},
    {"Spirit Temple"},
    {"Shadow Temple"},
    {"Bottom of the Well"},
    {"Ice Cavern"},
    {"Ganon's Tower"},
    {"Gerudo Training Ground"},
    {"Theives' Hideout"},
    {"Inside Ganon's Castle"},
    {"Tower Collapse"},
    {"Castle Collapse"},
    {"Treasure Box Shop"},
    {"Gohma's Lair"},
    {"King Dodongo's Lair"},
    {"Barinade's Lair"},
    {"Phantom Ganon's Lair"},
    {"Volvagia's Lair"},
    {"Morpha's Lair"},
    {"Twinrova's Lair"},
    {"Bongo Bongo's Lair"},
    {"Ganondorf's Lair"},
    {"Ganon's Lair"},
    {"Market Entrance (Day)"},
    {"Market Entrance (Night)"},
    {"Market Entrance (Adult)"},
    {"Back Alley (Day)"},
    {"Back Alley (Night)"},
    {"Market (Day)"},
    {"Market (Night)"},
    {"Market (Adult)"},
    {"Outside ToT (Day)"},
    {"Outside ToT (Night)"},
    {"Outside ToT (Adult)"},
    {"Know-It-All Bros' House"},
    {"Twins' House"},
    {"Mido's House"},
    {"Saria's House"},
    {"Carpenter Boss's House"},
    {"Man in Green's House"},
    {"Bazaar"},
    {"Kokiri Shop"},
    {"Goron Shop"},
    {"Zora Shop"},
    {"Kakariko Potion Shop"},
    {"Market Potion Shop"},
    {"Bombchu Shop"},
    {"Happy Mask Shop"},
    {"Link's House"},
    {"Richard's House"},
    {"Stable"},
    {"Impa's House"},
    {"Lakeside Lab"},
    {"Carpenters' Tent"},
    {"Gravekeeper's Hut"},
    {"Great Fairy"},
    {"Fairy Fountain"},
    {"Great Fairy"},
    {"Grotto"},
    {"Redead Grave"},
    {"Fairy Fountain Grave"},
    {"Royal Family's Tomb"},
    {"Shooting Gallery"},
    {"Temple of Time"},
    {"Chamber of Sages"},
    {"Castle Maze (Day)"},
    {"Castle Maze (Night)"},
    {"Cutscene Map"},
    {"Dampe's Grave"},
    {"Fishing Pond"},
    {"Castle Courtyard"},
    {"Bombchu Bowling Alley"},
    {"Ranch House"},
    {"Guard House"},
    {"Granny's Potion Shop"},
    {"Ganon Fight"},
    {"House of Skulltula"},
    {"Hyrule Field"},
    {"Kakariko Village"},
    {"Graveyard"},
    {"Zora's River"},
    {"Kokiri Forest"},
    {"Sacred Forest Meadow"},
    {"Lake Hylia"},
    {"Zora's Domain"},
    {"Zora's Fountain"},
    {"Gerudo Valley"},
    {"Lost Woods"},
    {"Desert Colossus"},
    {"Gerudo's Fortress"},
    {"Haunted Wasteland"},
    {"Hyrule Castle"},
    {"Death Mountain Trail"},
    {"Death Mountain Crater"},
    {"Goron City"},
    {"Lon Lon Ranch"},
    {"Outside Ganon's Castle"},
    //Debug Rooms
    {"Test Map"},
    {"Test Room"},
    {"Depth Test"},
    {"Stalfos Mini-Boss"},
    {"Stalfos Boss"},
    {"Dark Link"}, 
    {"Castle Maze (Broken)"},
    {"SRD Room"}, 
    {"Chest Room"}
};

#define COLOR_WHITE      ImVec4(1.00f, 1.00f, 1.00f, 1.00f)
#define COLOR_RED        ImVec4(1.00f, 0.00f, 0.00f, 1.00f)
#define COLOR_GREEN      ImVec4(0.10f, 1.00f, 0.10f, 1.00f)
#define COLOR_BLUE       ImVec4(0.00f, 0.33f, 1.00f, 1.00f)
#define COLOR_PURPLE     ImVec4(0.54f, 0.19f, 0.89f, 1.00f)
#define COLOR_YELLOW     ImVec4(1.00f, 1.00f, 0.00f, 1.00f)
#define COLOR_ORANGE     ImVec4(1.00f, 0.67f, 0.11f, 1.00f)
#define COLOR_LIGHT_BLUE ImVec4(0.00f, 0.88f, 1.00f, 1.00f)
#define COLOR_GREY       ImVec4(0.78f, 0.78f, 0.78f, 1.00f)

char itemTimestampDisplayName[TIMESTAMP_MAX][21] = { "" };
ImVec4 itemTimestampDisplayColor[TIMESTAMP_MAX];

typedef struct {
    char name[40];
    u32 time;
    ImVec4 color;
    bool isRoom;
}TimestampInfo;

// Timestamps are an array of structs, each with a name, time, and color
// Names and colors are set up at the bottom of this file
// Times are stored in gSaveContext.sohStats.itemTimestamp
TimestampInfo itemTimestampDisplay[TIMESTAMP_MAX];
TimestampInfo sceneTimestampDisplay[8191];
//std::vector<TimestampInfo> sceneTimestampDisplay;

void DisplayTimeHHMMSS(uint32_t timeInTenthsOfSeconds, std::string text, ImVec4 color) {

    uint32_t sec = timeInTenthsOfSeconds / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = timeInTenthsOfSeconds % 10;

    ImGui::PushStyleColor(ImGuiCol_Text, color);

    std::string padded = fmt::format("{:<40}", text);
    ImGui::Text(padded.c_str());
    ImGui::SameLine();
    ImGui::Text("%2u:%02u:%02u.%u", hh, mm, ss, ds);
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
    ImGui::Text("%7u", value);
}

void DisplayStatIfNonZero(const char* text, uint32_t value) {
    if (value > 0) {
        DisplayStat(text, value);
    }
    return;
}

std::string ResolveSceneID(int sceneID, int roomID){
    std::string scene = "";
    if (sceneID == SCENE_KAKUSIANA) {
        switch (roomID) {
            case 0:
                scene = "Generic Grotto";
                break;
            case 1:
                scene = "Lake Hylia Scrub Grotto";
                break;
            case 2:
                scene = "Redead Grotto";
                break;
            case 3:
                scene = "Cow Grotto";
                break;
            case 4:
                scene = "Scrub Trio";
                break;
            case 5:
                scene = "Flooded Grotto";
                break;
            case 6:
                scene = "Scrub Duo (Upgrade)";
                break;
            case 7:
                scene = "Wolfos Grotto";
                break;
            case 8:
                scene = "Hyrule Castle Storms Grotto";
                break;
            case 9:
                scene = "Scrub Duo";
                break;
            case 10:
                scene = "Tektite Grotto";
                break;
            case 11:
                scene = "Forest Stage";
                break;
            case 12:
                scene = "Webbed Grotto";
                break;
            case 13:
                scene = "Big Skulltula Grotto";
                break;
            default:
                scene = "???";
        };
    } else if (sceneID == SCENE_HAKASITARELAY) {
        //Only the last room of Dampe's Grave (rm 6) is considered the windmill
        scene = roomID == 6 ? "Windmill" : "Dampe's Grave";
    } else if (sceneID < SCENE_ID_MAX) {
        scene = sceneMappings[sceneID];
    } else {
        scene = "???";
    }
    return scene;
}

void DrawStatsTracker(bool& open) {
    if (!open) {
        if (CVarGetInteger("gGameplayStatsEnabled", 0)) {
            CVarClear("gGameplayStatsEnabled");
            LUS::RequestCvarSaveOnNextTick();
        }
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
    // Set up the array of item timestamps and then sort it chronologically
    for (int i = 0; i < TIMESTAMP_MAX; i++) {
        strcpy(itemTimestampDisplay[i].name, itemTimestampDisplayName[i]);
        itemTimestampDisplay[i].time = gSaveContext.sohStats.itemTimestamp[i];
        itemTimestampDisplay[i].color = itemTimestampDisplayColor[i];
    }
   
    for (int i = 0; i < gSaveContext.sohStats.tsIdx; i++) {
        std::string sceneName = ResolveSceneID(gSaveContext.sohStats.sceneTimestamps[i].scene, gSaveContext.sohStats.sceneTimestamps[i].room);
        std::string name;
        if (CVarGetInteger("gGameplayStatRoomBreakdown", 0) && gSaveContext.sohStats.sceneTimestamps[i].scene != SCENE_KAKUSIANA) {
            name = fmt::format("{:s} Room {:d}", sceneName, gSaveContext.sohStats.sceneTimestamps[i].room);    
        } else {
            name = sceneName;
        }
        strcpy(sceneTimestampDisplay[i].name, name.c_str());
        sceneTimestampDisplay[i].time = CVarGetInteger("gGameplayStatRoomBreakdown", 0) ? 
            gSaveContext.sohStats.sceneTimestamps[i].roomTime : gSaveContext.sohStats.sceneTimestamps[i].sceneTime;
        sceneTimestampDisplay[i].color = COLOR_GREY;
        sceneTimestampDisplay[i].isRoom = gSaveContext.sohStats.sceneTimestamps[i].isRoom;
    }

    SortChronological(itemTimestampDisplay, sizeof(itemTimestampDisplay) / sizeof(itemTimestampDisplay[0]));

    
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
    DisplayTimeHHMMSS(gSaveContext.sohStats.sceneTimer / 2, "Time in scene:      ", COLOR_LIGHT_BLUE);
    UIWidgets::Tooltip("Timer accuracy may be affected by game performance and loading.");
    DisplayTimeHHMMSS(gSaveContext.sohStats.roomTimer / 2, "Time in room:       ", COLOR_LIGHT_BLUE);
    UIWidgets::Tooltip("Timer accuracy may be affected by game performance and loading.");
    ImGui::Text("Current room: %d", gSaveContext.sohStats.roomNum);

    ImGui::PopStyleVar(1);
    ImGui::EndTable();

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 8.0f, 8.0f });
    if (ImGui::BeginTabBar("Stats", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Timestamps")) {
            // Display chronological timestamps of items obtained and bosses defeated
            for (int i = 0; i < TIMESTAMP_MAX; i++) {
                // To be shown, the entry must have a non-zero time and a string for its display name
                if (itemTimestampDisplay[i].time > 0 && strnlen(itemTimestampDisplay[i].name, 21) > 1) {
                    DisplayTimeHHMMSS(itemTimestampDisplay[i].time, itemTimestampDisplay[i].name, itemTimestampDisplay[i].color);
                }
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Counts")) {
            DisplayStat("Enemies Defeated:      ", enemiesDefeated);
            // Show breakdown of enemies defeated in a tree. Only show counts for enemies if they've been defeated at least once.
            if (enemiesDefeated > 0) {
                if (ImGui::TreeNode("Enemy Details...")) {
                    DisplayStatIfNonZero("Anubis:             ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_ANUBIS]);
                    DisplayStatIfNonZero("Armos:              ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_ARMOS]);
                    DisplayStatIfNonZero("Arwing:             ", gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_ARWING]);
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
            // If using MM Bunny Hood enhancement, show how long it's been equipped (not counting pause time)
            if (CVarGetInteger("gMMBunnyHood", 0) || gSaveContext.sohStats.count[COUNT_TIME_BUNNY_HOOD] > 0) {
                DisplayTimeHHMMSS(gSaveContext.sohStats.count[COUNT_TIME_BUNNY_HOOD] / 2, "Bunny Hood Time:    ", COLOR_WHITE);
            }
            DisplayStat("Rolls:                 ", gSaveContext.sohStats.count[COUNT_ROLLS]);
            DisplayStat("Bonks:                 ", gSaveContext.sohStats.count[COUNT_BONKS]);
            DisplayStat("Sidehops:              ", gSaveContext.sohStats.count[COUNT_SIDEHOPS]);
            DisplayStat("Backflips:             ", gSaveContext.sohStats.count[COUNT_BACKFLIPS]);
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
        ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Breakdown")) {
            UIWidgets::PaddedEnhancementCheckbox("Room Breakdown", "gGameplayStatRoomBreakdown");
            ImGui::SameLine();
            UIWidgets::InsertHelpHoverText("Allows a more in-depth perspective of time spent in a certain map.");
            if (gPlayState == NULL) {
                ImGui::Text("Waiting for file load...");
            } else {
                for (int i = 0; i < gSaveContext.sohStats.tsIdx; i++) {
                    TimestampInfo tsInfo = sceneTimestampDisplay[i];
                    bool canShow = !tsInfo.isRoom || CVarGetInteger("gGameplayStatRoomBreakdown", 0);
                    if (tsInfo.time > 0 && strnlen(tsInfo.name, 40) > 1 && canShow) {
                        DisplayTimeHHMMSS(tsInfo.time, tsInfo.name, tsInfo.color);
                    }
                }
                std::string toPass;
                if (CVarGetInteger("gGameplayStatRoomBreakdown", 0) && gSaveContext.sohStats.sceneNum != SCENE_KAKUSIANA) {
                    toPass = fmt::format("{:s} Room {:d}", ResolveSceneID(gSaveContext.sohStats.sceneNum, gSaveContext.sohStats.roomNum), gSaveContext.sohStats.roomNum);
                } else {
                    toPass = ResolveSceneID(gSaveContext.sohStats.sceneNum, gSaveContext.sohStats.roomNum);
                }
                DisplayTimeHHMMSS(CURRENT_MODE_TIMER / 2, toPass.c_str(), COLOR_WHITE);
            }
            ImGui::EndTabItem();
        }
    ImGui::EndTabBar();
    }
    ImGui::PopStyleVar(1);
    ImGui::Text("Note: Gameplay stats are saved to the current file and will be\nlost if you quit without saving.");

    ImGui::End();
}

// Entries listed here will have a timestamp shown in the stat window
void SetupDisplayNames() {
    // To add a timestamp for an item or event, add it to this list and ensure
    // it has a corresponding entry in the enum (see gameplaystats.h)

    strcpy(itemTimestampDisplayName[ITEM_BOW],              "Fairy Bow:          ");
    strcpy(itemTimestampDisplayName[ITEM_ARROW_FIRE],       "Fire Arrows:        ");
    strcpy(itemTimestampDisplayName[ITEM_DINS_FIRE],        "Din's Fire:         ");
    strcpy(itemTimestampDisplayName[ITEM_SLINGSHOT],        "Slingshot:          ");
    strcpy(itemTimestampDisplayName[ITEM_OCARINA_FAIRY],    "Fairy Ocarina:      ");
    strcpy(itemTimestampDisplayName[ITEM_OCARINA_TIME],     "Ocarina of Time:    ");
    strcpy(itemTimestampDisplayName[ITEM_BOMBCHU],          "Bombchus:           ");
    strcpy(itemTimestampDisplayName[ITEM_HOOKSHOT],         "Hookshot:           ");
    strcpy(itemTimestampDisplayName[ITEM_LONGSHOT],         "Longshot:           ");
    strcpy(itemTimestampDisplayName[ITEM_ARROW_ICE],        "Ice Arrows:         ");
    strcpy(itemTimestampDisplayName[ITEM_FARORES_WIND],     "Farore's Wind:      ");
    strcpy(itemTimestampDisplayName[ITEM_BOOMERANG],        "Boomerang:          ");
    strcpy(itemTimestampDisplayName[ITEM_LENS],             "Lens of Truth:      ");
    strcpy(itemTimestampDisplayName[ITEM_HAMMER],           "Megaton Hammer:     ");
    strcpy(itemTimestampDisplayName[ITEM_ARROW_LIGHT],      "Light Arrows:       ");
    strcpy(itemTimestampDisplayName[ITEM_BOTTLE],           "Bottle:             ");
    strcpy(itemTimestampDisplayName[ITEM_LETTER_ZELDA],     "Zelda's Letter:     ");
    strcpy(itemTimestampDisplayName[ITEM_SWORD_KOKIRI],     "Kokiri Sword:       ");
    strcpy(itemTimestampDisplayName[ITEM_SWORD_MASTER],     "Master Sword:       ");
    strcpy(itemTimestampDisplayName[ITEM_SWORD_BGS],        "Biggoron's Sword:   ");
    strcpy(itemTimestampDisplayName[ITEM_SHIELD_DEKU],      "Deku Shield:        ");
    strcpy(itemTimestampDisplayName[ITEM_SHIELD_HYLIAN],    "Hylian Shield:      ");
    strcpy(itemTimestampDisplayName[ITEM_SHIELD_MIRROR],    "Mirror Shield:      ");
    strcpy(itemTimestampDisplayName[ITEM_TUNIC_GORON],      "Goron Tunic:        ");
    strcpy(itemTimestampDisplayName[ITEM_TUNIC_ZORA],       "Zora Tunic:         ");
    strcpy(itemTimestampDisplayName[ITEM_BOOTS_IRON],       "Iron Boots:         ");
    strcpy(itemTimestampDisplayName[ITEM_BOOTS_HOVER],      "Hover Boots:        ");
    strcpy(itemTimestampDisplayName[ITEM_BOMB_BAG_20],      "Bomb Bag:           ");
    strcpy(itemTimestampDisplayName[ITEM_BRACELET],         "Goron's Bracelet:   ");
    strcpy(itemTimestampDisplayName[ITEM_GAUNTLETS_SILVER], "Silver Gauntlets:   ");
    strcpy(itemTimestampDisplayName[ITEM_GAUNTLETS_GOLD],   "Gold Gauntlets:     ");
    strcpy(itemTimestampDisplayName[ITEM_SCALE_SILVER],     "Silver Scale:       ");
    strcpy(itemTimestampDisplayName[ITEM_SCALE_GOLDEN],     "Gold Scale:         ");
    strcpy(itemTimestampDisplayName[ITEM_WALLET_ADULT],     "Adult's Wallet:     ");
    strcpy(itemTimestampDisplayName[ITEM_WALLET_GIANT],     "Giant's Wallet:     ");
    strcpy(itemTimestampDisplayName[ITEM_WEIRD_EGG],        "Weird Egg:          ");
    strcpy(itemTimestampDisplayName[ITEM_GERUDO_CARD],      "Gerudo's Card:      ");
    strcpy(itemTimestampDisplayName[ITEM_COJIRO],           "Cojiro:             ");
    strcpy(itemTimestampDisplayName[ITEM_POCKET_EGG],       "Pocket Egg:         ");
    strcpy(itemTimestampDisplayName[ITEM_MASK_SKULL],       "Skull Mask:         ");
    strcpy(itemTimestampDisplayName[ITEM_MASK_SPOOKY],      "Spooky Mask:        ");
    strcpy(itemTimestampDisplayName[ITEM_MASK_KEATON],      "Keaton Mask:        ");
    strcpy(itemTimestampDisplayName[ITEM_MASK_BUNNY],       "Bunny Hood:         ");
    strcpy(itemTimestampDisplayName[ITEM_ODD_MUSHROOM],     "Odd Mushroom:       ");
    strcpy(itemTimestampDisplayName[ITEM_ODD_POTION],       "Odd Potion:         ");
    strcpy(itemTimestampDisplayName[ITEM_SAW],              "Poacher's Saw:      ");
    strcpy(itemTimestampDisplayName[ITEM_SWORD_BROKEN],     "Broken Goron Sword: ");
    strcpy(itemTimestampDisplayName[ITEM_PRESCRIPTION],     "Prescription:       ");
    strcpy(itemTimestampDisplayName[ITEM_FROG],             "Eyeball Frog:       ");
    strcpy(itemTimestampDisplayName[ITEM_EYEDROPS],         "Eye Drops:          ");
    strcpy(itemTimestampDisplayName[ITEM_CLAIM_CHECK],      "Claim Check:        ");
    strcpy(itemTimestampDisplayName[ITEM_SONG_MINUET],      "Minuet of Forest:   ");
    strcpy(itemTimestampDisplayName[ITEM_SONG_BOLERO],      "Bolero of Fire:     ");
    strcpy(itemTimestampDisplayName[ITEM_SONG_SERENADE],    "Serenade of Water:  ");
    strcpy(itemTimestampDisplayName[ITEM_SONG_REQUIEM],     "Requiem of Spirit:  ");
    strcpy(itemTimestampDisplayName[ITEM_SONG_NOCTURNE],    "Nocturne of Shadow: ");
    strcpy(itemTimestampDisplayName[ITEM_SONG_PRELUDE],     "Prelude of Light:   ");
    strcpy(itemTimestampDisplayName[ITEM_SONG_LULLABY],     "Zelda's Lullaby:    ");
    strcpy(itemTimestampDisplayName[ITEM_SONG_EPONA],       "Epona's Song:       ");
    strcpy(itemTimestampDisplayName[ITEM_SONG_SARIA],       "Saria's Song:       ");
    strcpy(itemTimestampDisplayName[ITEM_SONG_SUN],         "Sun's Song:         ");
    strcpy(itemTimestampDisplayName[ITEM_SONG_TIME],        "Song of Time:       ");
    strcpy(itemTimestampDisplayName[ITEM_SONG_STORMS],      "Song of Storms:     ");
    strcpy(itemTimestampDisplayName[ITEM_MEDALLION_FOREST], "Forest Medallion:   ");
    strcpy(itemTimestampDisplayName[ITEM_MEDALLION_FIRE],   "Fire Medallion:     ");
    strcpy(itemTimestampDisplayName[ITEM_MEDALLION_WATER],  "Water Medallion:    ");
    strcpy(itemTimestampDisplayName[ITEM_MEDALLION_SPIRIT], "Spirit Medallion:   ");
    strcpy(itemTimestampDisplayName[ITEM_MEDALLION_SHADOW], "Shadow Medallion:   ");
    strcpy(itemTimestampDisplayName[ITEM_MEDALLION_LIGHT],  "Light Medallion:    ");
    strcpy(itemTimestampDisplayName[ITEM_KOKIRI_EMERALD],   "Kokiri's Emerald:   ");
    strcpy(itemTimestampDisplayName[ITEM_GORON_RUBY],       "Goron's Ruby:       ");
    strcpy(itemTimestampDisplayName[ITEM_ZORA_SAPPHIRE],    "Zora's Sapphire:    ");
    strcpy(itemTimestampDisplayName[ITEM_KEY_BOSS],         "Ganon's Boss Key:   ");
    strcpy(itemTimestampDisplayName[ITEM_SINGLE_MAGIC],     "Magic:              ");
    strcpy(itemTimestampDisplayName[ITEM_DOUBLE_DEFENSE],   "Double Defense:     ");

    // Other events
    strcpy(itemTimestampDisplayName[TIMESTAMP_DEFEAT_GOHMA],         "Gohma Defeated:     ");
    strcpy(itemTimestampDisplayName[TIMESTAMP_DEFEAT_KING_DODONGO],  "KD Defeated:        ");
    strcpy(itemTimestampDisplayName[TIMESTAMP_DEFEAT_BARINADE],      "Barinade Defeated:  ");
    strcpy(itemTimestampDisplayName[TIMESTAMP_DEFEAT_PHANTOM_GANON], "PG Defeated:        ");
    strcpy(itemTimestampDisplayName[TIMESTAMP_DEFEAT_VOLVAGIA],      "Volvagia Defeated:  ");
    strcpy(itemTimestampDisplayName[TIMESTAMP_DEFEAT_MORPHA],        "Morpha Defeated:    ");
    strcpy(itemTimestampDisplayName[TIMESTAMP_DEFEAT_BONGO_BONGO],   "Bongo Defeated:     ");
    strcpy(itemTimestampDisplayName[TIMESTAMP_DEFEAT_TWINROVA],      "Twinrova Defeated:  ");
    strcpy(itemTimestampDisplayName[TIMESTAMP_DEFEAT_GANONDORF],     "Ganondorf Defeated: ");
    strcpy(itemTimestampDisplayName[TIMESTAMP_DEFEAT_GANON],         "Ganon Defeated:     ");
}

void SetupDisplayColors() {
    for (int i = 0; i < TIMESTAMP_MAX; i++) {
        switch (i) {
            case ITEM_SONG_MINUET:
            case ITEM_KOKIRI_EMERALD:
            case ITEM_SONG_SARIA:
            case ITEM_MEDALLION_FOREST:
            case TIMESTAMP_FOUND_GREG:
                itemTimestampDisplayColor[i] = COLOR_GREEN;
                break;
            case ITEM_SONG_BOLERO:
            case ITEM_GORON_RUBY:
            case ITEM_MEDALLION_FIRE:
                itemTimestampDisplayColor[i] = COLOR_RED;
                break;
            case ITEM_SONG_SERENADE:
            case ITEM_ZORA_SAPPHIRE:
            case ITEM_MEDALLION_WATER:
                itemTimestampDisplayColor[i] = COLOR_BLUE;
                break;
            case ITEM_SONG_LULLABY:
            case ITEM_SONG_NOCTURNE:
            case ITEM_MEDALLION_SHADOW:
                itemTimestampDisplayColor[i] = COLOR_PURPLE;
                break;
            case ITEM_SONG_EPONA:
            case ITEM_SONG_REQUIEM:
            case ITEM_MEDALLION_SPIRIT:
                itemTimestampDisplayColor[i] = COLOR_ORANGE;
                break;
            case ITEM_SONG_SUN:
            case ITEM_SONG_PRELUDE:
            case ITEM_MEDALLION_LIGHT:
            case ITEM_ARROW_LIGHT:
                itemTimestampDisplayColor[i] = COLOR_YELLOW;
                break;
            case ITEM_SONG_STORMS:
                itemTimestampDisplayColor[i] = COLOR_GREY;
                break;
            case ITEM_SONG_TIME:
                itemTimestampDisplayColor[i] = COLOR_LIGHT_BLUE;
                break;
            default:
                itemTimestampDisplayColor[i] = COLOR_WHITE;
                break;
        }
    }
}

extern "C" void InitStatTracker() {
    LUS::Context::GetInstance()->GetWindow()->GetGui()->AddWindow("Enhancements", "Gameplay Stats", DrawStatsTracker,
                    CVarGetInteger("gGameplayStatsEnabled", 0) == 1);
    SetupDisplayNames();
    SetupDisplayColors();
}
