#include "SfxEditor.h"
#include "sequence.h"
#include <string>
#include <map>
#include <Cvar.h>
#include "../libultraship/ImGuiImpl.h"
#include <functions.h>

const std::map<u16, std::tuple<std::string, std::string, SeqType>> sequenceMap = {
  {NA_BGM_FIELD_LOGIC,      {"Hyrule Field", "NA_BGM_FIELD_LOGIC", SEQ_BGM_WORLD}},
  {NA_BGM_DUNGEON,          {"Dodongo's Cavern", "NA_BGM_DUNGEON", SEQ_BGM_WORLD}},
  {NA_BGM_KAKARIKO_ADULT,   {"Kakariko Village (Adult)", "NA_BGM_KAKARIKO_ADULT", SEQ_BGM_WORLD}},
  {NA_BGM_ENEMY,            {"Battle", "NA_BGM_ENEMY", SEQ_NOSHUFFLE}},
  {NA_BGM_BOSS,             {"Boss Battle", "NA_BGM_BOSS", SEQ_BGM_BATTLE}},
  {NA_BGM_INSIDE_DEKU_TREE, {"Inside the Deku Tree", "NA_BGM_INSIDE_DEKU_TREE", SEQ_BGM_WORLD}},
  {NA_BGM_MARKET,           {"Market", "NA_BGM_MARKET", SEQ_BGM_WORLD}},
  {NA_BGM_TITLE,            {"Title Theme", "NA_BGM_TITLE", SEQ_BGM_WORLD}},
  {NA_BGM_LINK_HOUSE,       {"House", "NA_BGM_LINK_HOUSE", SEQ_BGM_WORLD}},
  {NA_BGM_GAME_OVER,        {"Game Over", "NA_BGM_GAME_OVER", SEQ_FANFARE}},
  {NA_BGM_BOSS_CLEAR,       {"Boss Clear", "NA_BGM_BOSS_CLEAR", SEQ_FANFARE}},
  {NA_BGM_ITEM_GET,         {"Obtain Item", "NA_BGM_ITEM_GET", SEQ_FANFARE}},
  {NA_BGM_OPENING_GANON,    {"Enter Ganondorf", "NA_BGM_OPENING_GANON", SEQ_FANFARE}},
  {NA_BGM_HEART_GET,        {"Obtain Heart Container", "NA_BGM_HEART_GET", SEQ_FANFARE}},
  {NA_BGM_OCA_LIGHT,        {"Prelude of Light", "NA_BGM_OCA_LIGHT", SEQ_OCARINA}},
  {NA_BGM_JABU_JABU,        {"Inside Jabu-Jabu's Belly", "NA_BGM_JABU_JABU", SEQ_BGM_WORLD}},
  {NA_BGM_KAKARIKO_KID,     {"Kakariko Village (Child)", "NA_BGM_KAKARIKO_KID", SEQ_BGM_WORLD}},
  {NA_BGM_GREAT_FAIRY,      {"Great Fairy's Fountain", "NA_BGM_GREAT_FAIRY", SEQ_BGM_EVENT}},
  {NA_BGM_ZELDA_THEME,      {"Zelda's Theme", "NA_BGM_ZELDA_THEME", SEQ_BGM_EVENT}},
  {NA_BGM_FIRE_TEMPLE,      {"Fire Temple", "NA_BGM_FIRE_TEMPLE", SEQ_BGM_WORLD}},
  {NA_BGM_OPEN_TRE_BOX,     {"Open Treasure Chest", "NA_BGM_OPEN_TRE_BOX", SEQ_FANFARE}},
  {NA_BGM_FOREST_TEMPLE,    {"Forest Temple", "NA_BGM_FOREST_TEMPLE", SEQ_BGM_WORLD}},
  {NA_BGM_COURTYARD,        {"Hyrule Castle Courtyard", "NA_BGM_COURTYARD", SEQ_BGM_WORLD}},
  {NA_BGM_GANON_TOWER,      {"Ganondorf's Theme", "NA_BGM_GANON_TOWER", SEQ_BGM_WORLD}},
  {NA_BGM_LONLON,           {"Lon Lon Ranch", "NA_BGM_LONLON", SEQ_BGM_WORLD}},
  {NA_BGM_GORON_CITY,       {"Goron City", "NA_BGM_GORON_CITY", SEQ_BGM_WORLD}},
  {NA_BGM_SPIRITUAL_STONE,  {"Spiritual Stone Get", "NA_BGM_SPIRITUAL_STONE", SEQ_FANFARE}},
  {NA_BGM_OCA_BOLERO,       {"Bolero of Fire", "NA_BGM_OCA_BOLERO", SEQ_OCARINA}},
  {NA_BGM_OCA_MINUET,       {"Minuet of Forest", "NA_BGM_OCA_MINUET", SEQ_OCARINA}},
  {NA_BGM_OCA_SERENADE,     {"Serenade of Water", "NA_BGM_OCA_SERENADE", SEQ_OCARINA}},
  {NA_BGM_OCA_REQUIEM,      {"Requiem of Spirit", "NA_BGM_OCA_REQUIEM", SEQ_OCARINA}},
  {NA_BGM_OCA_NOCTURNE,     {"Nocturne of Shadow", "NA_BGM_OCA_NOCTURNE", SEQ_OCARINA}},
  {NA_BGM_MINI_BOSS,        {"Mini-Boss Battle", "NA_BGM_MINI_BOSS", SEQ_BGM_ERROR}},
  {NA_BGM_SMALL_ITEM_GET,   {"Obtain Small Item", "NA_BGM_SMALL_ITEM_GET", SEQ_FANFARE}},
  {NA_BGM_TEMPLE_OF_TIME,   {"Temple of Time", "NA_BGM_TEMPLE_OF_TIME", SEQ_BGM_WORLD}},
  {NA_BGM_EVENT_CLEAR,      {"Escape from Lon Lon Ranch", "NA_BGM_EVENT_CLEAR", SEQ_FANFARE}},
  {NA_BGM_KOKIRI,           {"Kokiri Forest", "NA_BGM_KOKIRI", SEQ_BGM_WORLD}},
  {NA_BGM_OCA_FAIRY_GET,    {"Obtain Fairy Ocarina", "NA_BGM_OCA_FAIRY_GET", SEQ_FANFARE}},
  {NA_BGM_SARIA_THEME,      {"Lost Woods", "NA_BGM_SARIA_THEME", SEQ_BGM_WORLD}},
  {NA_BGM_SPIRIT_TEMPLE,    {"Spirit Temple", "NA_BGM_SPIRIT_TEMPLE", SEQ_BGM_WORLD}},
  {NA_BGM_HORSE,            {"Horse Race", "NA_BGM_HORSE", SEQ_BGM_EVENT}},
  {NA_BGM_HORSE_GOAL,       {"Horse Race Goal", "NA_BGM_HORSE_GOAL", SEQ_FANFARE}},
  {NA_BGM_INGO,             {"Ingo's Theme", "NA_BGM_INGO", SEQ_BGM_WORLD}},
  {NA_BGM_MEDALLION_GET,    {"Obtain Medallion", "NA_BGM_MEDALLION_GET", SEQ_FANFARE}},
  {NA_BGM_OCA_SARIA,        {"Ocarina Saria's Song", "NA_BGM_OCA_SARIA", SEQ_OCARINA}},
  {NA_BGM_OCA_EPONA,        {"Ocarina Epona's Song", "NA_BGM_OCA_EPONA", SEQ_OCARINA}},
  {NA_BGM_OCA_ZELDA,        {"Ocarina Zelda's Lullaby", "NA_BGM_OCA_ZELDA", SEQ_OCARINA}},
  {NA_BGM_OCA_SUNS,         {"Ocarina Sun's Song", "NA_BGM_OCA_SUNS", SEQ_NOSHUFFLE}},
  {NA_BGM_OCA_TIME,         {"Ocarina Song of Time", "NA_BGM_OCA_TIME", SEQ_OCARINA}},
  {NA_BGM_OCA_STORM,        {"Ocarina Song of Storms", "NA_BGM_OCA_STORM", SEQ_OCARINA}},
  {NA_BGM_NAVI_OPENING,     {"Fairy Flying", "NA_BGM_NAVI_OPENING", SEQ_BGM_EVENT}},
  {NA_BGM_DEKU_TREE_CS,     {"Deku Tree", "NA_BGM_DEKU_TREE_CS", SEQ_BGM_EVENT}},
  {NA_BGM_WINDMILL,         {"Windmill Hut", "NA_BGM_WINDMILL", SEQ_BGM_WORLD}},
  {NA_BGM_HYRULE_CS,        {"Legend of Hyrule", "NA_BGM_HYRULE_CS", SEQ_NOSHUFFLE}},
  {NA_BGM_MINI_GAME,        {"Shooting Gallery", "NA_BGM_MINI_GAME", SEQ_BGM_EVENT}},
  {NA_BGM_SHEIK,            {"Sheik's Theme", "NA_BGM_SHEIK", SEQ_BGM_EVENT}},
  {NA_BGM_ZORA_DOMAIN,      {"Zora's Domain", "NA_BGM_ZORA_DOMAIN", SEQ_BGM_WORLD}},
  {NA_BGM_APPEAR,           {"Enter Zelda", "NA_BGM_APPEAR", SEQ_FANFARE}},
  {NA_BGM_ADULT_LINK,       {"Goodbye to Zelda", "NA_BGM_ADULT_LINK", SEQ_BGM_EVENT}},
  {NA_BGM_MASTER_SWORD,     {"Master Sword", "NA_BGM_MASTER_SWORD", SEQ_FANFARE}},
  {NA_BGM_INTRO_GANON,      {"Ganon Intro", "NA_BGM_INTRO_GANON", SEQ_BGM_EVENT}},
  {NA_BGM_SHOP,             {"Shop", "NA_BGM_SHOP", SEQ_BGM_WORLD}},
  {NA_BGM_CHAMBER_OF_SAGES, {"Chamber of the Sages", "NA_BGM_CHAMBER_OF_SAGES", SEQ_BGM_EVENT}},
  {NA_BGM_FILE_SELECT,      {"File Select", "NA_BGM_FILE_SELECT", SEQ_NOSHUFFLE}},
  {NA_BGM_ICE_CAVERN,       {"Ice Cavern", "NA_BGM_ICE_CAVERN", SEQ_BGM_WORLD}},
  {NA_BGM_DOOR_OF_TIME,     {"Open Door of Temple of Time", "NA_BGM_DOOR_OF_TIME", SEQ_NOSHUFFLE}},
  {NA_BGM_OWL,              {"Kaepora Gaebora's Theme", "NA_BGM_OWL", SEQ_BGM_EVENT}},
  {NA_BGM_SHADOW_TEMPLE,    {"Shadow Temple", "NA_BGM_SHADOW_TEMPLE", SEQ_BGM_WORLD}},
  {NA_BGM_WATER_TEMPLE,     {"Water Temple", "NA_BGM_WATER_TEMPLE", SEQ_BGM_WORLD}},
  {NA_BGM_BRIDGE_TO_GANONS, {"Ganon's Castle Bridge", "NA_BGM_BRIDGE_TO_GANONS", SEQ_NOSHUFFLE}},
  {NA_BGM_OCARINA_OF_TIME,  {"Ocarina of Time", "NA_BGM_OCARINA_OF_TIME", SEQ_NOSHUFFLE}},
  {NA_BGM_GERUDO_VALLEY,    {"Gerudo Valley", "NA_BGM_GERUDO_VALLEY", SEQ_BGM_WORLD}},
  {NA_BGM_POTION_SHOP,      {"Potion Shop", "NA_BGM_POTION_SHOP", SEQ_BGM_WORLD}},
  {NA_BGM_KOTAKE_KOUME,     {"Kotake & Koume's Theme", "NA_BGM_KOTAKE_KOUME", SEQ_BGM_EVENT}},
  {NA_BGM_ESCAPE,           {"Escape from Ganon's Castle", "NA_BGM_ESCAPE", SEQ_BGM_EVENT}},
  {NA_BGM_UNDERGROUND,      {"Ganon's Castle Under Ground", "NA_BGM_UNDERGROUND", SEQ_BGM_WORLD}},
  {NA_BGM_GANONDORF_BOSS,   {"Ganondorf Battle", "NA_BGM_GANONDORF_BOSS", SEQ_BGM_BATTLE}},
  {NA_BGM_GANON_BOSS,       {"Ganon Battle", "NA_BGM_GANON_BOSS", SEQ_BGM_BATTLE}},
  {NA_BGM_END_DEMO,         {"Seal of Six Sages", "NA_BGM_END_DEMO", SEQ_NOSHUFFLE}},
  {NA_BGM_STAFF_1,          {"End Credits I", "NA_BGM_STAFF_1", SEQ_NOSHUFFLE}},
  {NA_BGM_STAFF_2,          {"End Credits II", "NA_BGM_STAFF_2", SEQ_NOSHUFFLE}},
  {NA_BGM_STAFF_3,          {"End Credits III", "NA_BGM_STAFF_3", SEQ_NOSHUFFLE}},
  {NA_BGM_STAFF_4,          {"End Credits IV", "NA_BGM_STAFF_4", SEQ_NOSHUFFLE}},
  {NA_BGM_FIRE_BOSS,        {"King Dodongo & Volvagia Boss Battle", "NA_BGM_FIRE_BOSS", SEQ_BGM_BATTLE}},
  {NA_BGM_TIMED_MINI_GAME,  {"Mini-Game", "NA_BGM_TIMED_MINI_GAME", SEQ_BGM_ERROR}},
};

void Draw_SfxTab(const std::string& tabKey, const std::map<u16, std::tuple<std::string, std::string, SeqType>>& map, SeqType type) {
    if (ImGui::Button("Reset All")) {
        for (const auto& [defaultValue, nameAndStorageKeySuffix] : map) {
            const auto& [name, storageKeySuffix, seqType] = nameAndStorageKeySuffix;
            if (seqType == type) {
                const std::string storageKey = "gSfxEditor_" + storageKeySuffix;
                CVar_SetS32(storageKey.c_str(), defaultValue);
            }
        }
        SohImGui::needs_save = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Randomize All")) {
        std::vector<u16> values;
        for (const auto& [value, nameAndStorageKeySuffix] : map) {
            const auto& [name, storageKeySuffix, seqType] = nameAndStorageKeySuffix;

            if (seqType == type) {
                values.push_back(value);
            }
        }
        for (const auto& [defaultValue, nameAndStorageKeySuffix] : map) {
            const auto& [name, storageKeySuffix, seqType] = nameAndStorageKeySuffix;
            const std::string storageKey = "gSfxEditor_" + storageKeySuffix;
            if (seqType == type) {
                const int randomIndex = rand() % values.size();
                const int randomValue = values[randomIndex];
                values.erase(values.begin() + randomIndex);
                CVar_SetS32(storageKey.c_str(), randomValue);
            }
        }
        SohImGui::needs_save = true;
    }

    ImGui::BeginTable(tabKey.c_str(), 3, ImGuiTableFlags_SizingFixedFit);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 110.0f);
    for (const auto& [defaultValue, nameAndStorageKeySuffix] : map) {
        const auto& [name, storageKeySuffix, seqType] = nameAndStorageKeySuffix;

        if (seqType != type) {
            continue;
        }

        const std::string storageKey = "gSfxEditor_" + storageKeySuffix;
        const std::string hiddenKey = "##" + storageKey;
        const std::string stopButton = " Stop  " + hiddenKey;
        const std::string previewButton = "Preview" + hiddenKey;
        const int currentValue = CVar_GetS32(storageKey.c_str(), defaultValue);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text(name.c_str());
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::BeginCombo(hiddenKey.c_str(), std::get<0>(map.at(static_cast<u16>(currentValue))).c_str())) {
            for (const auto& [value, nameAndStorageKeySuffix] : map) {
                const auto& [name, storageKeySuffix, seqType] = nameAndStorageKeySuffix;
                if (seqType != type) {
                    continue;
                }

                if (ImGui::Selectable(std::get<0>(nameAndStorageKeySuffix).c_str())) {
                    CVar_SetS32(storageKey.c_str(), static_cast<int>(value));
                    SohImGui::needs_save = true;
                }
            }

            ImGui::EndCombo();
        }
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        if (CVar_GetS32("gSfxEditor_playing", 0) == currentValue) {
            if (ImGui::Button(stopButton.c_str())) {
                func_800F5C2C();
                CVar_SetS32("gSfxEditor_playing", 0);
                SohImGui::needs_save = true;
            }
        } else {
            if (ImGui::Button(previewButton.c_str())) {
                if  (CVar_GetS32("gSfxEditor_playing", 0) != 0) {
                    func_800F5C2C();
                    CVar_SetS32("gSfxEditor_playing", 0);
                    SohImGui::needs_save = true;
                } else {
                    // TODO: Cant do both here, so have to click preview button twice
                    func_800F5ACC(currentValue);
                    CVar_SetS32("gSfxEditor_playing", currentValue);
                    SohImGui::needs_save = true;
                }
            }
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::Button("Reset")) {
            CVar_SetS32(storageKey.c_str(), defaultValue);
            SohImGui::needs_save = true;
        }
    }
    ImGui::EndTable();
}

extern "C" u16 getReplacementSeq(u16 seqId) {
    if (sequenceMap.find(seqId) == sequenceMap.end()) {
        return seqId;
    }

    const auto& [name, storageKeySuffix, seqType] = sequenceMap.at(seqId);
    const std::string storageKey = "gSfxEditor_" + storageKeySuffix;
    const int replacementSeq = CVar_GetS32(storageKey.c_str(), seqId);

    return static_cast<u16>(replacementSeq);
}

void DrawSfxEditor(bool& open) {
    if (!open) {
        CVar_SetS32("gSfxEditor", 0);
        return;
    }
    ImGui::SetNextWindowSize(ImVec2(465, 430), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("SFX Editor", &open)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("SfxContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Background Music")) {
            Draw_SfxTab("backgroundMusic", sequenceMap, SEQ_BGM_WORLD);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Fanfares")) {
            Draw_SfxTab("fanfares", sequenceMap, SEQ_FANFARE);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Events")) {
            Draw_SfxTab("event", sequenceMap, SEQ_BGM_EVENT);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Battle Music")) {
            Draw_SfxTab("event", sequenceMap, SEQ_BGM_BATTLE);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Ocarina")) {
            Draw_SfxTab("event", sequenceMap, SEQ_OCARINA);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void InitSfxEditor() {
    //Draw the bar in the menu.
    SohImGui::AddWindow("Enhancements", "SFX Editor", DrawSfxEditor);
}
