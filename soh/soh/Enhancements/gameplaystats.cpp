extern "C" {
#include "gameplaystats.h"
}
#include "gameplaystatswindow.h"

#include "soh/SaveManager.h"
#include "functions.h"
#include "macros.h"
#include "../UIWidgets.hpp"

#include <vector>
#include <string>
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/enhancementTypes.h"

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
// Times are stored in gSaveContext.sohStats.itemTimestamp
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
    std::string buildVersion;
    SaveManager::Instance->LoadData("buildVersion", buildVersion);
    strncpy(gSaveContext.sohStats.buildVersion, buildVersion.c_str(), ARRAY_COUNT(gSaveContext.sohStats.buildVersion) - 1);
    gSaveContext.sohStats.buildVersion[ARRAY_COUNT(gSaveContext.sohStats.buildVersion) - 1] = 0;
    SaveManager::Instance->LoadData("buildVersionMajor", gSaveContext.sohStats.buildVersionMajor);
    SaveManager::Instance->LoadData("buildVersionMinor", gSaveContext.sohStats.buildVersionMinor);
    SaveManager::Instance->LoadData("buildVersionPatch", gSaveContext.sohStats.buildVersionPatch);

    SaveManager::Instance->LoadData("heartPieces", gSaveContext.sohStats.heartPieces);
    SaveManager::Instance->LoadData("heartContainers", gSaveContext.sohStats.heartContainers);
    SaveManager::Instance->LoadArray("dungeonKeys", ARRAY_COUNT(gSaveContext.sohStats.dungeonKeys), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.sohStats.dungeonKeys[i]);
    });
    SaveManager::Instance->LoadData("rtaTiming", gSaveContext.sohStats.rtaTiming);
    SaveManager::Instance->LoadData("fileCreatedAt", gSaveContext.sohStats.fileCreatedAt);
    SaveManager::Instance->LoadData("playTimer", gSaveContext.sohStats.playTimer);
    SaveManager::Instance->LoadData("pauseTimer", gSaveContext.sohStats.pauseTimer);
    SaveManager::Instance->LoadArray("itemTimestamps", ARRAY_COUNT(gSaveContext.sohStats.itemTimestamp), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.sohStats.itemTimestamp[i]);
    });
    SaveManager::Instance->LoadArray("sceneTimestamps", ARRAY_COUNT(gSaveContext.sohStats.sceneTimestamps), [&](size_t i) {
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
            gSaveContext.sohStats.sceneTimestamps[i].scene = scene;
            gSaveContext.sohStats.sceneTimestamps[i].room = room;
            gSaveContext.sohStats.sceneTimestamps[i].sceneTime = sceneTime;
            gSaveContext.sohStats.sceneTimestamps[i].roomTime = roomTime;
            gSaveContext.sohStats.sceneTimestamps[i].isRoom = isRoom;
        });
    });
    SaveManager::Instance->LoadData("tsIdx", gSaveContext.sohStats.tsIdx);
    SaveManager::Instance->LoadArray("counts", ARRAY_COUNT(gSaveContext.sohStats.count), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.sohStats.count[i]);
    });
    SaveManager::Instance->LoadArray("scenesDiscovered", ARRAY_COUNT(gSaveContext.sohStats.scenesDiscovered), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.sohStats.scenesDiscovered[i]);
    });
    SaveManager::Instance->LoadArray("entrancesDiscovered", ARRAY_COUNT(gSaveContext.sohStats.entrancesDiscovered), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.sohStats.entrancesDiscovered[i]);
    });
}

void SaveStats(SaveContext* saveContext, int sectionID, bool fullSave) {
    SaveManager::Instance->SaveData("buildVersion", saveContext->sohStats.buildVersion);
    SaveManager::Instance->SaveData("buildVersionMajor", saveContext->sohStats.buildVersionMajor);
    SaveManager::Instance->SaveData("buildVersionMinor", saveContext->sohStats.buildVersionMinor);
    SaveManager::Instance->SaveData("buildVersionPatch", saveContext->sohStats.buildVersionPatch);

    SaveManager::Instance->SaveData("heartPieces", saveContext->sohStats.heartPieces);
    SaveManager::Instance->SaveData("heartContainers", saveContext->sohStats.heartContainers);
    SaveManager::Instance->SaveArray("dungeonKeys", ARRAY_COUNT(saveContext->sohStats.dungeonKeys), [&](size_t i) {
        SaveManager::Instance->SaveData("", saveContext->sohStats.dungeonKeys[i]);
    });
    SaveManager::Instance->SaveData("rtaTiming", saveContext->sohStats.rtaTiming);
    SaveManager::Instance->SaveData("fileCreatedAt", saveContext->sohStats.fileCreatedAt);
    SaveManager::Instance->SaveData("playTimer", saveContext->sohStats.playTimer);
    SaveManager::Instance->SaveData("pauseTimer", saveContext->sohStats.pauseTimer);
    SaveManager::Instance->SaveArray("itemTimestamps", ARRAY_COUNT(saveContext->sohStats.itemTimestamp), [&](size_t i) {
        SaveManager::Instance->SaveData("", saveContext->sohStats.itemTimestamp[i]);
    });
    SaveManager::Instance->SaveArray("sceneTimestamps", ARRAY_COUNT(saveContext->sohStats.sceneTimestamps), [&](size_t i) {
        if (saveContext->sohStats.sceneTimestamps[i].scene != 254 && saveContext->sohStats.sceneTimestamps[i].room != 254) {
            SaveManager::Instance->SaveStruct("", [&]() {
                SaveManager::Instance->SaveData("scene", saveContext->sohStats.sceneTimestamps[i].scene);
                SaveManager::Instance->SaveData("room", saveContext->sohStats.sceneTimestamps[i].room);
                SaveManager::Instance->SaveData("sceneTime", saveContext->sohStats.sceneTimestamps[i].sceneTime);
                SaveManager::Instance->SaveData("roomTime", saveContext->sohStats.sceneTimestamps[i].roomTime);
                SaveManager::Instance->SaveData("isRoom", saveContext->sohStats.sceneTimestamps[i].isRoom);
            });
        }
    });
    SaveManager::Instance->SaveData("tsIdx", saveContext->sohStats.tsIdx);
    SaveManager::Instance->SaveArray("counts", ARRAY_COUNT(saveContext->sohStats.count), [&](size_t i) {
        SaveManager::Instance->SaveData("", saveContext->sohStats.count[i]);
    });
    SaveManager::Instance->SaveArray("scenesDiscovered", ARRAY_COUNT(saveContext->sohStats.scenesDiscovered), [&](size_t i) {
        SaveManager::Instance->SaveData("", saveContext->sohStats.scenesDiscovered[i]);
    });
    SaveManager::Instance->SaveArray("entrancesDiscovered", ARRAY_COUNT(saveContext->sohStats.entrancesDiscovered), [&](size_t i) {
        SaveManager::Instance->SaveData("", saveContext->sohStats.entrancesDiscovered[i]);
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
    return CVarGetInteger("gGameplayStats.TimestampsReverse", 0) ? a.time > b.time : a.time < b.time;
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
    GameplayStatsRow("Build Version:", (char*) gBuildVersion);
    if (gSaveContext.sohStats.rtaTiming) {
        GameplayStatsRow("Total Time (RTA):", formatTimestampGameplayStat(GAMEPLAYSTAT_TOTAL_TIME), gSaveContext.sohStats.gameComplete ? COLOR_GREEN : COLOR_WHITE);
    } else {
        GameplayStatsRow("Total Game Time:", formatTimestampGameplayStat(GAMEPLAYSTAT_TOTAL_TIME), gSaveContext.sohStats.gameComplete ? COLOR_GREEN : COLOR_WHITE);
    }
    if (CVarGetInteger("gGameplayStats.ShowAdditionalTimers", 0)) { // !Only display total game time
        GameplayStatsRow("Gameplay Time:", formatTimestampGameplayStat(gSaveContext.sohStats.playTimer / 2), COLOR_GREY);
        GameplayStatsRow("Pause Menu Time:", formatTimestampGameplayStat(gSaveContext.sohStats.pauseTimer / 3), COLOR_GREY);
        GameplayStatsRow("Time in scene:", formatTimestampGameplayStat(gSaveContext.sohStats.sceneTimer / 2), COLOR_LIGHT_BLUE);
        GameplayStatsRow("Time in room:", formatTimestampGameplayStat(gSaveContext.sohStats.roomTimer / 2), COLOR_LIGHT_BLUE);
    }
    if (gPlayState != NULL && CVarGetInteger("gGameplayStats.ShowDebugInfo", 0)) { // && display debug info
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
        itemTimestampDisplay[i].time = gSaveContext.sohStats.itemTimestamp[i];
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

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.0f, 4.0f });
    ImGui::BeginTable("gameplayStatsCounts", 1, ImGuiTableFlags_BordersOuter);
    ImGui::TableSetupColumn("stat", ImGuiTableColumnFlags_WidthStretch);
    GameplayStatsRow("Enemies Defeated:", formatIntGameplayStat(enemiesDefeated));
    if (enemiesDefeated > 0) {
        ImGui::TableNextRow(); ImGui::TableNextColumn();
        if (ImGui::TreeNodeEx("Enemy Details...", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
            for (int i = COUNT_ENEMIES_DEFEATED_ANUBIS; i <= COUNT_ENEMIES_DEFEATED_WOLFOS; i++) {
                if (i == COUNT_ENEMIES_DEFEATED_FLOORMASTER) {
                    GameplayStatsRow(countMappings[i], formatIntGameplayStat(gSaveContext.sohStats.count[i] / 3));
                } else {
                    GameplayStatsRow(countMappings[i], formatIntGameplayStat(gSaveContext.sohStats.count[i]));
                }
            }
        }
    }
    GameplayStatsRow("Rupees Collected:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_RUPEES_COLLECTED]));
    UIWidgets::Tooltip("Includes rupees collected with a full wallet.");
    GameplayStatsRow("Rupees Spent:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_RUPEES_SPENT]));
    GameplayStatsRow("Chests Opened:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_CHESTS_OPENED]));
    GameplayStatsRow("Ammo Used:", formatIntGameplayStat(ammoUsed));
    if (ammoUsed > 0) {
        ImGui::TableNextRow(); ImGui::TableNextColumn();
        if (ImGui::TreeNodeEx("Ammo Details...", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
            for (int i = COUNT_AMMO_USED_STICK; i <= COUNT_AMMO_USED_BEAN; i++) {
                GameplayStatsRow(countMappings[i], formatIntGameplayStat(gSaveContext.sohStats.count[i]));
            }
        }
    }
    GameplayStatsRow("Damage Taken:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_DAMAGE_TAKEN]));
    GameplayStatsRow("Sword Swings:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_SWORD_SWINGS]));
    GameplayStatsRow("Steps Taken:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_STEPS]));
    // If using MM Bunny Hood enhancement, show how long it's been equipped (not counting pause time)
    if (CVarGetInteger("gMMBunnyHood", BUNNY_HOOD_VANILLA) != BUNNY_HOOD_VANILLA || gSaveContext.sohStats.count[COUNT_TIME_BUNNY_HOOD] > 0) {
        GameplayStatsRow("Bunny Hood Time:", formatTimestampGameplayStat(gSaveContext.sohStats.count[COUNT_TIME_BUNNY_HOOD] / 2));
    }
    GameplayStatsRow("Rolls:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_ROLLS]));
    GameplayStatsRow("Bonks:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_BONKS]));
    GameplayStatsRow("Sidehops:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_SIDEHOPS]));
    GameplayStatsRow("Backflips:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_BACKFLIPS]));
    GameplayStatsRow("Ice Traps:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_ICE_TRAPS]));
    GameplayStatsRow("Pauses:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_PAUSES]));
    GameplayStatsRow("Pots Smashed:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_POTS_BROKEN]));
    GameplayStatsRow("Bushes Cut:", formatIntGameplayStat(gSaveContext.sohStats.count[COUNT_BUSHES_CUT]));
    GameplayStatsRow("Buttons Pressed:", formatIntGameplayStat(buttonPresses));
    if (buttonPresses > 0) {
        ImGui::TableNextRow(); ImGui::TableNextColumn();
        if (ImGui::TreeNodeEx("Buttons...", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
            for (int i = COUNT_BUTTON_PRESSES_A; i <= COUNT_BUTTON_PRESSES_START; i++) {
                GameplayStatsRow(countMappings[i], formatIntGameplayStat(gSaveContext.sohStats.count[i]));
            }
        }
    }
    ImGui::EndTable();
    ImGui::PopStyleVar(1);
}

void DrawGameplayStatsBreakdownTab() {
    for (int i = 0; i < gSaveContext.sohStats.tsIdx; i++) {
        std::string sceneName = ResolveSceneID(gSaveContext.sohStats.sceneTimestamps[i].scene, gSaveContext.sohStats.sceneTimestamps[i].room);
        std::string name;
        if (CVarGetInteger("gGameplayStats.RoomBreakdown", 0) && gSaveContext.sohStats.sceneTimestamps[i].scene != SCENE_GROTTOS) {
            name = fmt::format("{:s} Room {:d}", sceneName, gSaveContext.sohStats.sceneTimestamps[i].room);    
        } else {
            name = sceneName;
        }
        strcpy(sceneTimestampDisplay[i].name, name.c_str());
        sceneTimestampDisplay[i].time = CVarGetInteger("gGameplayStats.RoomBreakdown", 0) ? 
            gSaveContext.sohStats.sceneTimestamps[i].roomTime : gSaveContext.sohStats.sceneTimestamps[i].sceneTime;
        sceneTimestampDisplay[i].color = COLOR_GREY;
        sceneTimestampDisplay[i].isRoom = gSaveContext.sohStats.sceneTimestamps[i].isRoom;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.0f, 4.0f });
    ImGui::BeginTable("gameplayStatsCounts", 1, ImGuiTableFlags_BordersOuter);
    ImGui::TableSetupColumn("stat", ImGuiTableColumnFlags_WidthStretch);
    for (int i = 0; i < gSaveContext.sohStats.tsIdx; i++) {
        TimestampInfo tsInfo = sceneTimestampDisplay[i];
        bool canShow = !tsInfo.isRoom || CVarGetInteger("gGameplayStats.RoomBreakdown", 0);
        if (tsInfo.time > 0 && strnlen(tsInfo.name, 40) > 1 && canShow) {
            GameplayStatsRow(tsInfo.name, formatTimestampGameplayStat(tsInfo.time), tsInfo.color);
        }
    }
    std::string toPass;
    if (CVarGetInteger("gGameplayStats.RoomBreakdown", 0) && gSaveContext.sohStats.sceneNum != SCENE_GROTTOS) {
        toPass = fmt::format("{:s} Room {:d}", ResolveSceneID(gSaveContext.sohStats.sceneNum, gSaveContext.sohStats.roomNum), gSaveContext.sohStats.roomNum);
    } else {
        toPass = ResolveSceneID(gSaveContext.sohStats.sceneNum, gSaveContext.sohStats.roomNum);
    }
    GameplayStatsRow(toPass.c_str(), formatTimestampGameplayStat(CURRENT_MODE_TIMER / 2));
    ImGui::EndTable();
    ImGui::PopStyleVar(1);
}

void DrawGameplayStatsOptionsTab() {
    UIWidgets::PaddedEnhancementCheckbox("Show in-game total timer", "gGameplayStats.ShowIngameTimer", true, false);
    UIWidgets::InsertHelpHoverText("Keep track of the timer as an in-game HUD element. The position of the timer can be changed in the Cosmetics Editor.");
    UIWidgets::PaddedEnhancementCheckbox("Show latest timestamps on top", "gGameplayStats.TimestampsReverse", true, false);
    UIWidgets::PaddedEnhancementCheckbox("Room Breakdown", "gGameplayStats.RoomBreakdown", true, false);
    ImGui::SameLine();
    UIWidgets::InsertHelpHoverText("Allows a more in-depth perspective of time spent in a certain map.");   
    UIWidgets::PaddedEnhancementCheckbox("RTA Timing on new files", "gGameplayStats.RTATiming", true, false);
    ImGui::SameLine();
    UIWidgets::InsertHelpHoverText(
        "Timestamps are relative to starting timestamp rather than in game time, usually necessary for races/speedruns.\n\n"
        "Starting timestamp is on first non-c-up input after intro cutscene.\n\n"
        "NOTE: THIS NEEDS TO BE SET BEFORE CREATING A FILE TO TAKE EFFECT"
    );   
    UIWidgets::PaddedEnhancementCheckbox("Show additional detail timers", "gGameplayStats.ShowAdditionalTimers", true, false);
    UIWidgets::PaddedEnhancementCheckbox("Show Debug Info", "gGameplayStats.ShowDebugInfo");
}

void GameplayStatsWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(480, 550), ImGuiCond_Appearing);
    if (!ImGui::Begin("Gameplay Stats", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

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

    ImGui::End();
}
void InitStats(bool isDebug) {
    gSaveContext.sohStats.heartPieces = isDebug ? 8 : 0;
    gSaveContext.sohStats.heartContainers = isDebug ? 8 : 0;
    for (int dungeon = 0; dungeon < ARRAY_COUNT(gSaveContext.sohStats.dungeonKeys); dungeon++) {
        gSaveContext.sohStats.dungeonKeys[dungeon] = isDebug ? 8 : 0;
    }
    gSaveContext.sohStats.rtaTiming = CVarGetInteger("gGameplayStats.RTATiming", 0);
    gSaveContext.sohStats.fileCreatedAt = 0;
    gSaveContext.sohStats.playTimer = 0;
    gSaveContext.sohStats.pauseTimer = 0;
    for (int timestamp = 0; timestamp < ARRAY_COUNT(gSaveContext.sohStats.itemTimestamp); timestamp++) {
        gSaveContext.sohStats.itemTimestamp[timestamp] = 0;
    }
    for (int timestamp = 0; timestamp < ARRAY_COUNT(gSaveContext.sohStats.sceneTimestamps); timestamp++) {
        gSaveContext.sohStats.sceneTimestamps[timestamp].sceneTime = 0;
        gSaveContext.sohStats.sceneTimestamps[timestamp].roomTime = 0;
        gSaveContext.sohStats.sceneTimestamps[timestamp].scene = 254;
        gSaveContext.sohStats.sceneTimestamps[timestamp].room = 254;
        gSaveContext.sohStats.sceneTimestamps[timestamp].isRoom = 0;
    }
    gSaveContext.sohStats.tsIdx = 0;
    for (int count = 0; count < ARRAY_COUNT(gSaveContext.sohStats.count); count++) {
        gSaveContext.sohStats.count[count] = 0;
    }
    gSaveContext.sohStats.gameComplete = false;
    for (int scenesIdx = 0; scenesIdx < ARRAY_COUNT(gSaveContext.sohStats.scenesDiscovered); scenesIdx++) {
        gSaveContext.sohStats.scenesDiscovered[scenesIdx] = 0;
    }
    for (int entrancesIdx = 0; entrancesIdx < ARRAY_COUNT(gSaveContext.sohStats.entrancesDiscovered); entrancesIdx++) {
        gSaveContext.sohStats.entrancesDiscovered[entrancesIdx] = 0;
    }

    strncpy(gSaveContext.sohStats.buildVersion, (const char*) gBuildVersion, sizeof(gSaveContext.sohStats.buildVersion) - 1);
    gSaveContext.sohStats.buildVersion[sizeof(gSaveContext.sohStats.buildVersion) - 1] = 0;
    gSaveContext.sohStats.buildVersionMajor = gBuildVersionMajor;
    gSaveContext.sohStats.buildVersionMinor = gBuildVersionMinor;
    gSaveContext.sohStats.buildVersionPatch = gBuildVersionPatch;
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
