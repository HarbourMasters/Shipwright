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
#include "functions.h"
#include "macros.h"
extern GlobalContext* gGlobalCtx;

#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
}

void DisplayTimeHHMMSS(uint32_t timeInTenthsOfSeconds, const char* text) {

    if (timeInTenthsOfSeconds < 1) {
        return;
    }

    uint32_t sec = timeInTenthsOfSeconds / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = timeInTenthsOfSeconds % 10;

    ImGui::Text(text);
    ImGui::SameLine();

    if (mm < 10 && ss < 10) {
        ImGui::Text(" %d:0%d:0%d.%d", hh, mm, ss, ds);
    }
    if (mm < 10 && ss >= 10) {
        ImGui::Text(" %d:0%d:%d.%d", hh, mm, ss, ds);
    }
    if (mm >= 10 && ss < 10) {
        ImGui::Text(" %d:%d:0%d.%d", hh, mm, ss, ds);
    }
    if (mm >= 10 && ss >= 10) {
        ImGui::Text(" %d:%d:%d.%d", hh, mm, ss, ds);
    }
}

void DrawStatsTracker(bool& open) {
    if (!open) {
        CVar_SetS32("gGameplayStatsEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Randomizer Stats", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    DisplayTimeHHMMSS(gSaveContext.gamePlayStats.totalTimer,     "Total game Time: ");
    DisplayTimeHHMMSS(gSaveContext.gamePlayStats.playTimer / 2,  "Gameplay Time:   ");
    DisplayTimeHHMMSS(gSaveContext.gamePlayStats.pauseTimer / 3, "Pause Menu Time: ");

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 8.0f, 8.0f });
    ImGui::BeginTable("randoStatsTable", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV);

    ImGui::TableSetupColumn("Time Found", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableSetupColumn("Collection", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableHeadersRow();
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    DisplayTimeHHMMSS(gSaveContext.gamePlayStats.itemGetTime[ITEM_SWORD_KOKIRI], "Kokiri Sword:        ");
    DisplayTimeHHMMSS(gSaveContext.gamePlayStats.itemGetTime[ITEM_BOOMERANG],    "Boomerang:           ");
    DisplayTimeHHMMSS(gSaveContext.gamePlayStats.itemGetTime[ITEM_SONG_LULLABY], "Zelda's Lullaby:     ");
    DisplayTimeHHMMSS(gSaveContext.gamePlayStats.itemGetTime[ITEM_SONG_SARIA],   "Saria's Song:        ");
    DisplayTimeHHMMSS(gSaveContext.gamePlayStats.itemGetTime[ITEM_BOMB_BAG_20],  "Bomb Bag:            ");
    DisplayTimeHHMMSS(gSaveContext.gamePlayStats.itemGetTime[ITEM_SINGLE_MAGIC], "Magic:               ");
    DisplayTimeHHMMSS(gSaveContext.gamePlayStats.itemGetTime[ITEM_BOTTLE],       "Bottle:              ");

    ImGui::TableNextColumn();

    ImGui::Text("Rupees Collected:        %d", gSaveContext.gamePlayStats.rupeesCollected);
    ImGui::Text("Rupees Spent:            %d", gSaveContext.gamePlayStats.rupeesSpent);
    ImGui::Text("Songs Found:             %d", gSaveContext.gamePlayStats.songsFound);
    ImGui::Text("Ice Traps:               %d", gSaveContext.gamePlayStats.iceTrapCount);

    ImGui::PopStyleVar(1);
    ImGui::EndTable();

    ImGui::End();
}

void InitStatTracker() {
    SohImGui::AddWindow("Enhancements", "Gameplay Stats", DrawStatsTracker);
}