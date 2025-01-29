#include "gameplaystats.h"
#include "gameplaystatswindow.h"

#include "soh/SaveManager.h"
#include "functions.h"
#include "macros.h"
#include "soh/cvar_prefixes.h"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/util.h"

#include <vector>
#include <string>
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include <z64.h>
#include "variables.h"
extern PlayState* gPlayState;
uint64_t GetUnixTimestamp();
}

const char* const sceneMappings[] = {
    "Inside the Deku Tree",
    "Dodongo's Cavern",
    "Inside Jabu-Jabu's Belly",
    "Forest Temple",
    "Fire Temple",
    "Water Temple",
    "Spirit Temple",
    "Shadow Temple",
    "Bottom of the Well",
    "Ice Cavern",
    "Ganon's Tower",
    "Gerudo Training Ground",
    "Theives' Hideout",
    "Inside Ganon's Castle",
    "Tower Collapse",
    "Castle Collapse",
    "Treasure Box Shop",
    "Gohma's Lair",
    "King Dodongo's Lair",
    "Barinade's Lair",
    "Phantom Ganon's Lair",
    "Volvagia's Lair",
    "Morpha's Lair",
    "Twinrova's Lair",
    "Bongo Bongo's Lair",
    "Ganondorf's Lair",
    "Ganon's Lair",
    "Market Entrance (Day)",
    "Market Entrance (Night)",
    "Market Entrance (Adult)",
    "Back Alley (Day)",
    "Back Alley (Night)",
    "Market (Day)",
    "Market (Night)",
    "Market (Adult)",
    "Outside ToT (Day)",
    "Outside ToT (Night)",
    "Outside ToT (Adult)",
    "Know-It-All Bros' House",
    "Twins' House",
    "Mido's House",
    "Saria's House",
    "Carpenter Boss's House",
    "Man in Green's House",
    "Bazaar",
    "Kokiri Shop",
    "Goron Shop",
    "Zora Shop",
    "Kakariko Potion Shop",
    "Market Potion Shop",
    "Bombchu Shop",
    "Happy Mask Shop",
    "Link's House",
    "Richard's House",
    "Stable",
    "Impa's House",
    "Lakeside Lab",
    "Carpenters' Tent",
    "Gravekeeper's Hut",
    "Great Fairy",
    "Fairy Fountain",
    "Great Fairy",
    "Grotto",
    "Redead Grave",
    "Fairy Fountain Grave",
    "Royal Family's Tomb",
    "Shooting Gallery",
    "Temple of Time",
    "Chamber of Sages",
    "Castle Maze (Day)",
    "Castle Maze (Night)",
    "Cutscene Map",
    "Dampe's Grave",
    "Fishing Pond",
    "Castle Courtyard",
    "Bombchu Bowling Alley",
    "Ranch House",
    "Guard House",
    "Granny's Potion Shop",
    "Ganon Fight",
    "House of Skulltula",
    "Hyrule Field",
    "Kakariko Village",
    "Graveyard",
    "Zora's River",
    "Kokiri Forest",
    "Sacred Forest Meadow",
    "Lake Hylia",
    "Zora's Domain",
    "Zora's Fountain",
    "Gerudo Valley",
    "Lost Woods",
    "Desert Colossus",
    "Gerudo's Fortress",
    "Haunted Wasteland",
    "Hyrule Castle",
    "Death Mountain Trail",
    "Death Mountain Crater",
    "Goron City",
    "Lon Lon Ranch",
    "Outside Ganon's Castle",
    //Debug Rooms
    "Test Map",
    "Test Room",
    "Depth Test",
    "Stalfos Mini-Boss",
    "Stalfos Boss",
    "Dark Link",
    "Castle Maze (Broken)",
    "SRD Room",
    "Chest Room",
};

const char* const countMappings[] = {
    "Anubis:",
    "Armos:",
    "Arwing:",
    "Bari:",
    "Biri:",
    "Beamos:",
    "Big Octo:",
    "Bubble (Blue):",
    "Bubble (Green):",
    "Bubble (Red):",
    "Bubble (White):",
    "Business Scrub:",
    "Dark Link:",
    "Dead Hand:",
    "Deku Baba:",
    "Deku Baba (Big):",
    "Deku Scrub:",
    "Dinolfos:",
    "Dodongo:",
    "Dodongo (Baby):",
    "Door Mimic:",
    "Flare Dancer:",
    "Floormaster:",
    "Flying Floor Tile:",
    "Flying Pot:",
    "Freezard:",
    "Gerudo Thief:",
    "Gibdo:",
    "Gohma Larva:",
    "Guay:",
    "Iron Knuckle:",
    "Iron Knuckle (Nab):",
    "Keese:",
    "Keese (Fire):",
    "Keese (Ice):",
    "Leever:",
    "Leever (Big):",
    "Like-Like:",
    "Lizalfos:",
    "Mad Scrub:",
    "Moblin:",
    "Moblin (Club):",
    "Octorok:",
    "Parasitic Tentacle:",
    "Peahat:",
    "Peahat Larva:",
    "Poe:",
    "Poe (Big):",
    "Poe (Composer):",
    "Poe Sisters:",
    "Redead:",
    "Shabom:",
    "Shellblade:",
    "Skull Kid:",
    "Skulltula:",
    "Skulltula (Big):",
    "Skulltula (Gold):",
    "Skullwalltula:",
    "Spike:",
    "Stalchild:",
    "Stalfos:",
    "Stinger:",
    "Tailpasaran:",
    "Tektite (Blue):",
    "Tektite (Red):",
    "Torch Slug:",
    "Wallmaster:",
    "Withered Deku Baba:",
    "Wolfos:",
    "Wolfos (White):",
    "Deku Sticks:",
    "Deku Nuts:",
    "Bombs:",
    "Arrows:",
    "Deku Seeds:",
    "Bombchus:",
    "Beans:",
    "A:",
    "B:",
    "L:",
    "R:",
    "Z:",
    "C-Up:",
    "C-Right:",
    "C-Down:",
    "C-Left:",
    "D-Up:",
    "D-Right:",
    "D-Down:",
    "D-Left:",
    "Start:",
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
// Times are stored in gSaveContext.ship.stats.itemTimestamp
TimestampInfo itemTimestampDisplay[TIMESTAMP_MAX];
TimestampInfo sceneTimestampDisplay[8191];
//std::vector<TimestampInfo> sceneTimestampDisplay;

std::string formatTimestampGameplayStat(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
}

std::string formatIntGameplayStat(uint32_t value) {
    return fmt::format("{}", value);
}

std::string formatHexGameplayStat(uint32_t value) {
    return fmt::format("{:#x} ({:d})", value, value);
}

std::string formatHexOnlyGameplayStat(uint32_t value) {
    return fmt::format("{:#x}", value, value);
}

extern "C" char* GameplayStats_GetCurrentTime() {
    std::string timeString = formatTimestampGameplayStat(GAMEPLAYSTAT_TOTAL_TIME).c_str();
    const size_t stringLength = timeString.length();
    char* timeChar = (char*)malloc(stringLength + 1); // We need to use malloc so we can free this from a C file.
    strcpy(timeChar, timeString.c_str());
    return timeChar;
}

void LoadStatsVersion1() {
    SaveManager::Instance->LoadCharArray("buildVersion", gSaveContext.ship.stats.buildVersion,
                                         ARRAY_COUNT(gSaveContext.ship.stats.buildVersion));
    SaveManager::Instance->LoadData("buildVersionMajor", gSaveContext.ship.stats.buildVersionMajor);
    SaveManager::Instance->LoadData("buildVersionMinor", gSaveContext.ship.stats.buildVersionMinor);
    SaveManager::Instance->LoadData("buildVersionPatch", gSaveContext.ship.stats.buildVersionPatch);

    SaveManager::Instance->LoadData("heartPieces", gSaveContext.ship.stats.heartPieces);
    SaveManager::Instance->LoadData("heartContainers", gSaveContext.ship.stats.heartContainers);
    SaveManager::Instance->LoadArray("dungeonKeys", ARRAY_COUNT(gSaveContext.ship.stats.dungeonKeys), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.ship.stats.dungeonKeys[i]);
    });
    SaveManager::Instance->LoadData("rtaTiming", gSaveContext.ship.stats.rtaTiming);
    SaveManager::Instance->LoadData("fileCreatedAt", gSaveContext.ship.stats.fileCreatedAt);
    SaveManager::Instance->LoadData("playTimer", gSaveContext.ship.stats.playTimer);
    SaveManager::Instance->LoadData("pauseTimer", gSaveContext.ship.stats.pauseTimer);
    SaveManager::Instance->LoadArray("itemTimestamps", ARRAY_COUNT(gSaveContext.ship.stats.itemTimestamp), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.ship.stats.itemTimestamp[i]);
    });
    SaveManager::Instance->LoadArray("sceneTimestamps", ARRAY_COUNT(gSaveContext.ship.stats.sceneTimestamps), [&](size_t i) {
        SaveManager::Instance->LoadStruct("", [&]() {
            int scene, room, sceneTime, roomTime, isRoom;
            SaveManager::Instance->LoadData("scene", scene);
            SaveManager::Instance->LoadData("room", room);
            SaveManager::Instance->LoadData("sceneTime", sceneTime);
            SaveManager::Instance->LoadData("roomTime", roomTime);
            SaveManager::Instance->LoadData("isRoom", isRoom);
            if (scene == 0 && room == 0 && sceneTime == 0 && roomTime == 0 && isRoom == 0) {
                return;
            }
            gSaveContext.ship.stats.sceneTimestamps[i].scene = scene;
            gSaveContext.ship.stats.sceneTimestamps[i].room = room;
            gSaveContext.ship.stats.sceneTimestamps[i].sceneTime = sceneTime;
            gSaveContext.ship.stats.sceneTimestamps[i].roomTime = roomTime;
            gSaveContext.ship.stats.sceneTimestamps[i].isRoom = isRoom;
        });
    });
    SaveManager::Instance->LoadData("tsIdx", gSaveContext.ship.stats.tsIdx);
    SaveManager::Instance->LoadArray("counts", ARRAY_COUNT(gSaveContext.ship.stats.count), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.ship.stats.count[i]);
    });
    SaveManager::Instance->LoadArray("scenesDiscovered", ARRAY_COUNT(gSaveContext.ship.stats.scenesDiscovered), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.ship.stats.scenesDiscovered[i]);
    });
    SaveManager::Instance->LoadArray("entrancesDiscovered", ARRAY_COUNT(gSaveContext.ship.stats.entrancesDiscovered), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.ship.stats.entrancesDiscovered[i]);
    });
}

void SaveStats(SaveContext* saveContext, int sectionID, bool fullSave) {
    SaveManager::Instance->SaveData("buildVersion", saveContext->ship.stats.buildVersion);
    SaveManager::Instance->SaveData("buildVersionMajor", saveContext->ship.stats.buildVersionMajor);
    SaveManager::Instance->SaveData("buildVersionMinor", saveContext->ship.stats.buildVersionMinor);
    SaveManager::Instance->SaveData("buildVersionPatch", saveContext->ship.stats.buildVersionPatch);

    SaveManager::Instance->SaveData("heartPieces", saveContext->ship.stats.heartPieces);
    SaveManager::Instance->SaveData("heartContainers", saveContext->ship.stats.heartContainers);
    SaveManager::Instance->SaveArray("dungeonKeys", ARRAY_COUNT(saveContext->ship.stats.dungeonKeys), [&](size_t i) {
        SaveManager::Instance->SaveData("", saveContext->ship.stats.dungeonKeys[i]);
    });
    SaveManager::Instance->SaveData("rtaTiming", saveContext->ship.stats.rtaTiming);
    SaveManager::Instance->SaveData("fileCreatedAt", saveContext->ship.stats.fileCreatedAt);
    SaveManager::Instance->SaveData("playTimer", saveContext->ship.stats.playTimer);
    SaveManager::Instance->SaveData("pauseTimer", saveContext->ship.stats.pauseTimer);
    SaveManager::Instance->SaveArray("itemTimestamps", ARRAY_COUNT(saveContext->ship.stats.itemTimestamp), [&](size_t i) {
        SaveManager::Instance->SaveData("", saveContext->ship.stats.itemTimestamp[i]);
    });
    SaveManager::Instance->SaveArray("sceneTimestamps", ARRAY_COUNT(saveContext->ship.stats.sceneTimestamps), [&](size_t i) {
        if (saveContext->ship.stats.sceneTimestamps[i].scene != 254 && saveContext->ship.stats.sceneTimestamps[i].room != 254) {
            SaveManager::Instance->SaveStruct("", [&]() {
                SaveManager::Instance->SaveData("scene", saveContext->ship.stats.sceneTimestamps[i].scene);
                SaveManager::Instance->SaveData("room", saveContext->ship.stats.sceneTimestamps[i].room);
                SaveManager::Instance->SaveData("sceneTime", saveContext->ship.stats.sceneTimestamps[i].sceneTime);
                SaveManager::Instance->SaveData("roomTime", saveContext->ship.stats.sceneTimestamps[i].roomTime);
                SaveManager::Instance->SaveData("isRoom", saveContext->ship.stats.sceneTimestamps[i].isRoom);
            });
        }
    });
    SaveManager::Instance->SaveData("tsIdx", saveContext->ship.stats.tsIdx);
    SaveManager::Instance->SaveArray("counts", ARRAY_COUNT(saveContext->ship.stats.count), [&](size_t i) {
        SaveManager::Instance->SaveData("", saveContext->ship.stats.count[i]);
    });
    SaveManager::Instance->SaveArray("scenesDiscovered", ARRAY_COUNT(saveContext->ship.stats.scenesDiscovered), [&](size_t i) {
        SaveManager::Instance->SaveData("", saveContext->ship.stats.scenesDiscovered[i]);
    });
    SaveManager::Instance->SaveArray("entrancesDiscovered", ARRAY_COUNT(saveContext->ship.stats.entrancesDiscovered), [&](size_t i) {
        SaveManager::Instance->SaveData("", saveContext->ship.stats.entrancesDiscovered[i]);
    });
}

void GameplayStatsRow(const char* label, const std::string& value, ImVec4 color = COLOR_WHITE) {
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("%s", label);
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize(value.c_str()).x - 8.0f));
    ImGui::Text("%s", value.c_str());
    ImGui::PopStyleColor();
}

bool compareTimestampInfoByTime(const TimestampInfo& a, const TimestampInfo& b) {
    return CVarGetInteger(CVAR_ENHANCEMENT("GameplayStats.ReverseTimestamps"), 0) ? a.time > b.time : a.time < b.time;
}

const char* ResolveSceneID(int sceneID, int roomID){
    if (sceneID == SCENE_GROTTOS) {
        switch (roomID) {
            case 0:
                return "Generic Grotto";
            case 1:
                return "Lake Hylia Scrub Grotto";
            case 2:
                return "Redead Grotto";
            case 3:
                return "Cow Grotto";
            case 4:
                return "Scrub Trio";
            case 5:
                return "Flooded Grotto";
            case 6:
                return "Scrub Duo (Upgrade)";
            case 7:
                return "Wolfos Grotto";
            case 8:
                return "Hyrule Castle Storms Grotto";
            case 9:
                return "Scrub Duo";
            case 10:
                return "Tektite Grotto";
            case 11:
                return "Forest Stage";
            case 12:
                return "Webbed Grotto";
            case 13:
                return "Big Skulltula Grotto";
        };
    } else if (sceneID == SCENE_WINDMILL_AND_DAMPES_GRAVE) {
        //Only the last room of Dampe's Grave (rm 6) is considered the windmill
        return roomID == 6 ? "Windmill" : "Dampe's Grave";
    } else if (sceneID < SCENE_ID_MAX) {
        return sceneMappings[sceneID];
    }

    return "???";
}

void DrawGameplayStatsHeader() {
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.0f, 4.0f });
    ImGui::BeginTable("gameplayStatsHeader", 1, ImGuiTableFlags_BordersOuter);
    ImGui::TableSetupColumn("stat", ImGuiTableColumnFlags_WidthStretch);
    //if tag is empty (not a release build)
    if (gGitCommitTag[0] == 0) {
        GameplayStatsRow("Git Branch:", (char*)gGitBranch);
        GameplayStatsRow("Git Commit Hash:", (char*)gGitCommitHash);
    } else {
        GameplayStatsRow("Build Version:", (char*)gBuildVersion);
    }
    if (gSaveContext.ship.stats.rtaTiming) {
        GameplayStatsRow("Total Time (RTA):", formatTimestampGameplayStat(GAMEPLAYSTAT_TOTAL_TIME), gSaveContext.ship.stats.gameComplete ? COLOR_GREEN : COLOR_WHITE);
    } else {
        GameplayStatsRow("Total Game Time:", formatTimestampGameplayStat(GAMEPLAYSTAT_TOTAL_TIME), gSaveContext.ship.stats.gameComplete ? COLOR_GREEN : COLOR_WHITE);
    }
    if (CVarGetInteger(CVAR_ENHANCEMENT("GameplayStats.ShowAdditionalTimers"), 0)) { // !Only display total game time
        GameplayStatsRow("Gameplay Time:", formatTimestampGameplayStat(gSaveContext.ship.stats.playTimer / 2), COLOR_GREY);
        GameplayStatsRow("Pause Menu Time:", formatTimestampGameplayStat(gSaveContext.ship.stats.pauseTimer / 3), COLOR_GREY);
        GameplayStatsRow("Time in scene:", formatTimestampGameplayStat(gSaveContext.ship.stats.sceneTimer / 2), COLOR_LIGHT_BLUE);
        GameplayStatsRow("Time in room:", formatTimestampGameplayStat(gSaveContext.ship.stats.roomTimer / 2), COLOR_LIGHT_BLUE);
    }
    if (gPlayState != NULL && CVarGetInteger(CVAR_ENHANCEMENT("GameplayStats.ShowDebugInfo"), 0)) { // && display debug info
        GameplayStatsRow("play->sceneNum:", formatHexGameplayStat(gPlayState->sceneNum), COLOR_YELLOW);
        GameplayStatsRow("gSaveContext.entranceIndex:", formatHexGameplayStat(gSaveContext.entranceIndex), COLOR_YELLOW);
        GameplayStatsRow("gSaveContext.cutsceneIndex:", formatHexOnlyGameplayStat(gSaveContext.cutsceneIndex), COLOR_YELLOW);
        GameplayStatsRow("play->roomCtx.curRoom.num:", formatIntGameplayStat(gPlayState->roomCtx.curRoom.num), COLOR_YELLOW);
    }
    ImGui::EndTable();
    ImGui::PopStyleVar(1);
}

void DrawGameplayStatsTimestampsTab() {
    // Set up the array of item timestamps and then sort it chronologically
    for (int i = 0; i < TIMESTAMP_MAX; i++) {
        strcpy(itemTimestampDisplay[i].name, itemTimestampDisplayName[i]);
        itemTimestampDisplay[i].time = gSaveContext.ship.stats.itemTimestamp[i];
        itemTimestampDisplay[i].color = itemTimestampDisplayColor[i];
    }

    std::sort(itemTimestampDisplay, itemTimestampDisplay + TIMESTAMP_MAX, compareTimestampInfoByTime);

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.0f, 4.0f });
    ImGui::BeginTable("gameplayStatsTimestamps", 1, ImGuiTableFlags_BordersOuter);
    ImGui::TableSetupColumn("stat", ImGuiTableColumnFlags_WidthStretch);
    for (int i = 0; i < TIMESTAMP_MAX; i++) {
        // To be shown, the entry must have a non-zero time and a string for its display name
        if (itemTimestampDisplay[i].time > 0 && strnlen(itemTimestampDisplay[i].name, 21) > 1) {
            GameplayStatsRow(itemTimestampDisplay[i].name, formatTimestampGameplayStat(itemTimestampDisplay[i].time), itemTimestampDisplay[i].color);
        }
    }
    ImGui::EndTable();
    ImGui::PopStyleVar(1);

}

void DrawGameplayStatsCountsTab() {
    u32 enemiesDefeated = 0;
    u32 ammoUsed = 0;
    u32 buttonPresses = 0;

    // Sum of all enemies defeated
    for (int i = COUNT_ENEMIES_DEFEATED_ANUBIS; i <= COUNT_ENEMIES_DEFEATED_WOLFOS; i++) {
        if (i == COUNT_ENEMIES_DEFEATED_FLOORMASTER) {
            // Special case: You must kill 3 mini Floormasters for it count as one defeated Floormaster
            enemiesDefeated += gSaveContext.ship.stats.count[i] / 3;
        } else {
            enemiesDefeated += gSaveContext.ship.stats.count[i];
        }
    }
    // Sum of all ammo used
    for (int i = COUNT_AMMO_USED_STICK; i <= COUNT_AMMO_USED_BEAN; i++) {
        ammoUsed += gSaveContext.ship.stats.count[i];
    }
    // Sum of all button presses
    for (int i = COUNT_BUTTON_PRESSES_A; i <= COUNT_BUTTON_PRESSES_START; i++) {
        buttonPresses += gSaveContext.ship.stats.count[i];
    }

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.0f, 4.0f });
    ImGui::BeginTable("gameplayStatsCounts", 1, ImGuiTableFlags_BordersOuter);
    ImGui::TableSetupColumn("stat", ImGuiTableColumnFlags_WidthStretch);
    GameplayStatsRow("Enemies Defeated:", formatIntGameplayStat(enemiesDefeated));
    if (enemiesDefeated > 0) {
        ImGui::TableNextRow(); ImGui::TableNextColumn();
        if (ImGui::TreeNodeEx("Enemy Details...", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
            for (int i = COUNT_ENEMIES_DEFEATED_ANUBIS; i <= COUNT_ENEMIES_DEFEATED_WOLFOS; i++) {
                if (i == COUNT_ENEMIES_DEFEATED_FLOORMASTER) {
                    GameplayStatsRow(countMappings[i], formatIntGameplayStat(gSaveContext.ship.stats.count[i] / 3));
                } else {
                    GameplayStatsRow(countMappings[i], formatIntGameplayStat(gSaveContext.ship.stats.count[i]));
                }
            }
        }
    }
    GameplayStatsRow("Rupees Collected:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_RUPEES_COLLECTED]));
    UIWidgets::Tooltip("Includes rupees collected with a full wallet.");
    GameplayStatsRow("Rupees Spent:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_RUPEES_SPENT]));
    GameplayStatsRow("Chests Opened:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_CHESTS_OPENED]));
    GameplayStatsRow("Ammo Used:", formatIntGameplayStat(ammoUsed));
    if (ammoUsed > 0) {
        ImGui::TableNextRow(); ImGui::TableNextColumn();
        if (ImGui::TreeNodeEx("Ammo Details...", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
            for (int i = COUNT_AMMO_USED_STICK; i <= COUNT_AMMO_USED_BEAN; i++) {
                GameplayStatsRow(countMappings[i], formatIntGameplayStat(gSaveContext.ship.stats.count[i]));
            }
        }
    }
    GameplayStatsRow("Damage Taken:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_DAMAGE_TAKEN]));
    GameplayStatsRow("Sword Swings:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_SWORD_SWINGS]));
    GameplayStatsRow("Steps Taken:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_STEPS]));
    // If using MM Bunny Hood enhancement, show how long it's been equipped (not counting pause time)
    if (CVarGetInteger(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_VANILLA) != BUNNY_HOOD_VANILLA || gSaveContext.ship.stats.count[COUNT_TIME_BUNNY_HOOD] > 0) {
        GameplayStatsRow("Bunny Hood Time:", formatTimestampGameplayStat(gSaveContext.ship.stats.count[COUNT_TIME_BUNNY_HOOD] / 2));
    }
    GameplayStatsRow("Rolls:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_ROLLS]));
    GameplayStatsRow("Bonks:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_BONKS]));
    GameplayStatsRow("Sidehops:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_SIDEHOPS]));
    GameplayStatsRow("Backflips:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_BACKFLIPS]));
    GameplayStatsRow("Ice Traps:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_ICE_TRAPS]));
    GameplayStatsRow("Pauses:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_PAUSES]));
    GameplayStatsRow("Pots Smashed:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_POTS_BROKEN]));
    GameplayStatsRow("Bushes Cut:", formatIntGameplayStat(gSaveContext.ship.stats.count[COUNT_BUSHES_CUT]));
    GameplayStatsRow("Buttons Pressed:", formatIntGameplayStat(buttonPresses));
    if (buttonPresses > 0) {
        ImGui::TableNextRow(); ImGui::TableNextColumn();
        if (ImGui::TreeNodeEx("Buttons...", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
            for (int i = COUNT_BUTTON_PRESSES_A; i <= COUNT_BUTTON_PRESSES_START; i++) {
                GameplayStatsRow(countMappings[i], formatIntGameplayStat(gSaveContext.ship.stats.count[i]));
            }
        }
    }
    ImGui::EndTable();
    ImGui::PopStyleVar(1);
}

void DrawGameplayStatsBreakdownTab() {
    for (int i = 0; i < gSaveContext.ship.stats.tsIdx; i++) {
        std::string sceneName = ResolveSceneID(gSaveContext.ship.stats.sceneTimestamps[i].scene, gSaveContext.ship.stats.sceneTimestamps[i].room);
        std::string name;
        if (CVarGetInteger(CVAR_ENHANCEMENT("GameplayStats.RoomBreakdown"), 0) && gSaveContext.ship.stats.sceneTimestamps[i].scene != SCENE_GROTTOS) {
            name = fmt::format("{:s} Room {:d}", sceneName, gSaveContext.ship.stats.sceneTimestamps[i].room);
        } else {
            name = sceneName;
        }
        strcpy(sceneTimestampDisplay[i].name, name.c_str());
        sceneTimestampDisplay[i].time = CVarGetInteger(CVAR_ENHANCEMENT("GameplayStats.RoomBreakdown"), 0) ?
            gSaveContext.ship.stats.sceneTimestamps[i].roomTime : gSaveContext.ship.stats.sceneTimestamps[i].sceneTime;
        sceneTimestampDisplay[i].color = COLOR_GREY;
        sceneTimestampDisplay[i].isRoom = gSaveContext.ship.stats.sceneTimestamps[i].isRoom;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.0f, 4.0f });
    ImGui::BeginTable("gameplayStatsCounts", 1, ImGuiTableFlags_BordersOuter);
    ImGui::TableSetupColumn("stat", ImGuiTableColumnFlags_WidthStretch);
    for (int i = 0; i < gSaveContext.ship.stats.tsIdx; i++) {
        TimestampInfo tsInfo = sceneTimestampDisplay[i];
        bool canShow = !tsInfo.isRoom || CVarGetInteger(CVAR_ENHANCEMENT("GameplayStats.RoomBreakdown"), 0);
        if (tsInfo.time > 0 && strnlen(tsInfo.name, 40) > 1 && canShow) {
            GameplayStatsRow(tsInfo.name, formatTimestampGameplayStat(tsInfo.time), tsInfo.color);
        }
    }
    std::string toPass;
    if (CVarGetInteger(CVAR_ENHANCEMENT("GameplayStats.RoomBreakdown"), 0) && gSaveContext.ship.stats.sceneNum != SCENE_GROTTOS) {
        toPass = fmt::format("{:s} Room {:d}", ResolveSceneID(gSaveContext.ship.stats.sceneNum, gSaveContext.ship.stats.roomNum), gSaveContext.ship.stats.roomNum);
    } else {
        toPass = ResolveSceneID(gSaveContext.ship.stats.sceneNum, gSaveContext.ship.stats.roomNum);
    }
    GameplayStatsRow(toPass.c_str(), formatTimestampGameplayStat(CURRENT_MODE_TIMER / 2));
    ImGui::EndTable();
    ImGui::PopStyleVar(1);
}

void DrawGameplayStatsOptionsTab() {
    UIWidgets::PaddedEnhancementCheckbox("Show in-game total timer", CVAR_ENHANCEMENT("GameplayStats.ShowIngameTimer"), true, false);
    UIWidgets::InsertHelpHoverText("Keep track of the timer as an in-game HUD element. The position of the timer can be changed in the Cosmetics Editor.");
    UIWidgets::PaddedEnhancementCheckbox("Show latest timestamps on top", CVAR_ENHANCEMENT("GameplayStats.ReverseTimestamps"), true, false);
    UIWidgets::PaddedEnhancementCheckbox("Room Breakdown", CVAR_ENHANCEMENT("GameplayStats.RoomBreakdown"), true, false);
    ImGui::SameLine();
    UIWidgets::InsertHelpHoverText("Allows a more in-depth perspective of time spent in a certain map.");
    UIWidgets::PaddedEnhancementCheckbox("RTA Timing on new files", CVAR_ENHANCEMENT("GameplayStats.RTATiming"), true, false);
    ImGui::SameLine();
    UIWidgets::InsertHelpHoverText(
        "Timestamps are relative to starting timestamp rather than in game time, usually necessary for races/speedruns.\n\n"
        "Starting timestamp is on first non-c-up input after intro cutscene.\n\n"
        "NOTE: THIS NEEDS TO BE SET BEFORE CREATING A FILE TO TAKE EFFECT"
    );
    UIWidgets::PaddedEnhancementCheckbox("Show additional detail timers", CVAR_ENHANCEMENT("GameplayStats.ShowAdditionalTimers"), true, false);
    UIWidgets::PaddedEnhancementCheckbox("Show Debug Info", CVAR_ENHANCEMENT("GameplayStats.ShowDebugInfo"));
}

void GameplayStatsWindow::DrawElement() {
    DrawGameplayStatsHeader();

    if (ImGui::BeginTabBar("Stats", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Timestamps")) {
            DrawGameplayStatsTimestampsTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Counts")) {
            DrawGameplayStatsCountsTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Breakdown")) {
            DrawGameplayStatsBreakdownTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Options")) {
            DrawGameplayStatsOptionsTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::Text("Note: Gameplay stats are saved to the current file and will be\nlost if you quit without saving.");
}
void InitStats(bool isDebug) {
    gSaveContext.ship.stats.heartPieces = isDebug ? 8 : 0;
    gSaveContext.ship.stats.heartContainers = isDebug ? 8 : 0;
    for (int dungeon = 0; dungeon < ARRAY_COUNT(gSaveContext.ship.stats.dungeonKeys); dungeon++) {
        gSaveContext.ship.stats.dungeonKeys[dungeon] = isDebug ? 8 : 0;
    }
    gSaveContext.ship.stats.rtaTiming = CVarGetInteger(CVAR_ENHANCEMENT("GameplayStats.RTATiming"), 0);
    gSaveContext.ship.stats.fileCreatedAt = 0;
    gSaveContext.ship.stats.playTimer = 0;
    gSaveContext.ship.stats.pauseTimer = 0;
    for (int timestamp = 0; timestamp < ARRAY_COUNT(gSaveContext.ship.stats.itemTimestamp); timestamp++) {
        gSaveContext.ship.stats.itemTimestamp[timestamp] = 0;
    }
    for (int timestamp = 0; timestamp < ARRAY_COUNT(gSaveContext.ship.stats.sceneTimestamps); timestamp++) {
        gSaveContext.ship.stats.sceneTimestamps[timestamp].sceneTime = 0;
        gSaveContext.ship.stats.sceneTimestamps[timestamp].roomTime = 0;
        gSaveContext.ship.stats.sceneTimestamps[timestamp].scene = 254;
        gSaveContext.ship.stats.sceneTimestamps[timestamp].room = 254;
        gSaveContext.ship.stats.sceneTimestamps[timestamp].isRoom = 0;
    }
    gSaveContext.ship.stats.tsIdx = 0;
    for (int count = 0; count < ARRAY_COUNT(gSaveContext.ship.stats.count); count++) {
        gSaveContext.ship.stats.count[count] = 0;
    }
    gSaveContext.ship.stats.gameComplete = false;
    for (int scenesIdx = 0; scenesIdx < ARRAY_COUNT(gSaveContext.ship.stats.scenesDiscovered); scenesIdx++) {
        gSaveContext.ship.stats.scenesDiscovered[scenesIdx] = 0;
    }
    for (int entrancesIdx = 0; entrancesIdx < ARRAY_COUNT(gSaveContext.ship.stats.entrancesDiscovered); entrancesIdx++) {
        gSaveContext.ship.stats.entrancesDiscovered[entrancesIdx] = 0;
    }

    SohUtils::CopyStringToCharArray(gSaveContext.ship.stats.buildVersion, std::string((char*)gBuildVersion),
                                    ARRAY_COUNT(gSaveContext.ship.stats.buildVersion));
    gSaveContext.ship.stats.buildVersionMajor = gBuildVersionMajor;
    gSaveContext.ship.stats.buildVersionMinor = gBuildVersionMinor;
    gSaveContext.ship.stats.buildVersionPatch = gBuildVersionPatch;
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
    strcpy(itemTimestampDisplayName[TIMESTAMP_BOSSRUSH_FINISH],      "Boss Rush Finished: ");
    strcpy(itemTimestampDisplayName[TIMESTAMP_FOUND_GREG],           "Greg Found:         ");
    strcpy(itemTimestampDisplayName[TIMESTAMP_TRIFORCE_COMPLETED],   "Triforce Completed: ");
}

void SetupDisplayColors() {
    for (int i = 0; i < TIMESTAMP_MAX; i++) {
        switch (i) {
            case ITEM_SONG_MINUET:
            case ITEM_KOKIRI_EMERALD:
            case ITEM_SONG_SARIA:
            case ITEM_MEDALLION_FOREST:
            case TIMESTAMP_DEFEAT_GOHMA:
            case TIMESTAMP_DEFEAT_PHANTOM_GANON:
            case TIMESTAMP_FOUND_GREG:
                itemTimestampDisplayColor[i] = COLOR_GREEN;
                break;
            case ITEM_SONG_BOLERO:
            case ITEM_GORON_RUBY:
            case ITEM_MEDALLION_FIRE:
            case TIMESTAMP_DEFEAT_KING_DODONGO:
            case TIMESTAMP_DEFEAT_VOLVAGIA:
                itemTimestampDisplayColor[i] = COLOR_RED;
                break;
            case ITEM_SONG_SERENADE:
            case ITEM_ZORA_SAPPHIRE:
            case ITEM_MEDALLION_WATER:
            case TIMESTAMP_DEFEAT_BARINADE:
            case TIMESTAMP_DEFEAT_MORPHA:
                itemTimestampDisplayColor[i] = COLOR_BLUE;
                break;
            case ITEM_SONG_LULLABY:
            case ITEM_SONG_NOCTURNE:
            case ITEM_MEDALLION_SHADOW:
            case TIMESTAMP_DEFEAT_BONGO_BONGO:
                itemTimestampDisplayColor[i] = COLOR_PURPLE;
                break;
            case ITEM_SONG_EPONA:
            case ITEM_SONG_REQUIEM:
            case ITEM_MEDALLION_SPIRIT:
            case TIMESTAMP_DEFEAT_TWINROVA:
                itemTimestampDisplayColor[i] = COLOR_ORANGE;
                break;
            case ITEM_SONG_SUN:
            case ITEM_SONG_PRELUDE:
            case ITEM_MEDALLION_LIGHT:
            case ITEM_ARROW_LIGHT:
            case TIMESTAMP_DEFEAT_GANONDORF:
            case TIMESTAMP_DEFEAT_GANON:
            case TIMESTAMP_TRIFORCE_COMPLETED:
                itemTimestampDisplayColor[i] = COLOR_YELLOW;
                break;
            case ITEM_SONG_STORMS:
                itemTimestampDisplayColor[i] = COLOR_GREY;
                break;
            case ITEM_SONG_TIME:
            case TIMESTAMP_BOSSRUSH_FINISH:
                itemTimestampDisplayColor[i] = COLOR_LIGHT_BLUE;
                break;
            default:
                itemTimestampDisplayColor[i] = COLOR_WHITE;
                break;
        }
    }
}

void GameplayStatsWindow::InitElement() {
    SetupDisplayNames();
    SetupDisplayColors();

    SaveManager::Instance->AddLoadFunction("sohStats", 1, LoadStatsVersion1);
    // Add main section save, no parent
    SaveManager::Instance->AddSaveFunction("sohStats", 1, SaveStats, true, SECTION_PARENT_NONE);
    // Add subsections, parent of "sohStats". Not sure how to do this without the redundant references to "SaveStats"
    SaveManager::Instance->AddSaveFunction("entrances", 1, SaveStats, false, SECTION_ID_STATS);
    SaveManager::Instance->AddSaveFunction("scenes", 1, SaveStats, false, SECTION_ID_STATS);
    SaveManager::Instance->AddInitFunction(InitStats);
}
