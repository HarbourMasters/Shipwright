#include "TimeSplits.h"
#include <libultraship/libultraship.h>
#include "soh/SohGui/UIWidgets.hpp"
#include "fast/Fast3dGui.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include <fstream>
#include <filesystem>
#include <iterator>

using json = nlohmann::json;

extern "C" {
#include "variables.h"
uint64_t GetUnixTimestamp();
}

#define CVAR_NAME "gSettings.TimeSplits.Enable"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

namespace TimeSplits {

typedef struct {
    uint32_t splitType;
    uint32_t splitId;
} LegacySplitId;

// Bosses and the magic upgrades used to be ids 256+ in the item space, saved lists still hold those.
#define LEGACY_SPLIT_ID_BASE 256
static const LegacySplitId legacySplitIds[] = {
    { SPLIT_TYPE_BOSS, ACTOR_BOSS_GOMA },     { SPLIT_TYPE_BOSS, ACTOR_BOSS_DODONGO },
    { SPLIT_TYPE_BOSS, ACTOR_BOSS_VA },       { SPLIT_TYPE_BOSS, ACTOR_BOSS_GANONDROF },
    { SPLIT_TYPE_BOSS, ACTOR_BOSS_FD2 },      { SPLIT_TYPE_BOSS, ACTOR_BOSS_MO },
    { SPLIT_TYPE_BOSS, ACTOR_BOSS_SST },      { SPLIT_TYPE_BOSS, ACTOR_BOSS_TW },
    { SPLIT_TYPE_BOSS, ACTOR_BOSS_GANON },    { SPLIT_TYPE_BOSS, ACTOR_BOSS_GANON2 },
    { SPLIT_TYPE_ITEM, ITEM_SINGLE_MAGIC },   { SPLIT_TYPE_ITEM, ITEM_DOUBLE_MAGIC },
    { SPLIT_TYPE_ITEM, ITEM_DOUBLE_DEFENSE },
};

nlohmann::json TimesplitObject_to_json(const TimesplitObject& split) {
    return nlohmann::json{
        { "splitId", split.splitId },
        { "splitName", split.splitName },
        { "splitCurrentTime", split.splitCurrentTime },
        { "splitPreviousBest", split.splitPreviousBest },
        { "splitStatus", SPLIT_INACTIVE },
        { "splitType", split.splitType },
    };
}

TimesplitObject json_to_TimesplitObject(const nlohmann::json& jsonSplit) {
    TimesplitObject split;
    split.splitId = jsonSplit["splitId"];
    split.splitName = jsonSplit["splitName"].get<std::string>();
    split.splitCurrentTime = jsonSplit["splitCurrentTime"];
    split.splitPreviousBest = jsonSplit["splitPreviousBest"];
    split.splitStatus = jsonSplit["splitStatus"];
    if (jsonSplit.contains("splitType")) {
        split.splitType = jsonSplit["splitType"];
    } else {
        split.splitType = SPLIT_TYPE_ITEM;
    }

    uint32_t legacyIndex = split.splitId - LEGACY_SPLIT_ID_BASE;
    if (split.splitType == SPLIT_TYPE_ITEM && split.splitId >= LEGACY_SPLIT_ID_BASE &&
        legacyIndex < std::size(legacySplitIds)) {
        split.splitType = legacySplitIds[legacyIndex].splitType;
        split.splitId = legacySplitIds[legacyIndex].splitId;
    }

    return split;
}

uint32_t GetCurrentActiveSplit(const std::vector<TimesplitObject>& list) {
    for (size_t i = 0; i < splitList.size(); i++) {
        if (splitList[i].splitStatus == SPLIT_ACTIVE) {
            return (uint32_t)i;
        }
    }
    return -1;
}

TimesplitObject GetSplitObjectBySceneId(uint32_t sceneId) {
    TimesplitObject splitObject;
    for (auto& list : sceneObjectList) {
        if (list.splitId == sceneId) {
            splitObject = list;
            splitObject.splitType = SPLIT_TYPE_SCENE;
            break;
        }
    }
    return splitObject;
}

TimesplitObject GetSplitObject(uint32_t splitType, uint32_t splitId) {
    TimesplitObject splitObject;
    for (auto& list : splitObjectList) {
        if (list.splitType == splitType && list.splitId == splitId) {
            splitObject = list;
        }
    }
    return splitObject;
}

void HandlePopUpContext(uint32_t popupId) {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    if (shouldPopUpOpen && ImGui::BeginPopup("ItemSubMenu")) {
        std::vector<uint32_t> itemList;

        for (auto& item : itemSubMenuList) {
            if (item.first == popupId) {
                itemList = item.second;
                break;
            }
        }

        if (itemList.size() == 0) {
            ImGui::EndPopup();
            return;
        }

        uint32_t slotIndex = 0;
        for (auto& list : itemList) {
            TimesplitObject split = GetSplitObject(SPLIT_TYPE_ITEM, list);
            SplitsPushImageButtonStyle();
            if (ImGui::ImageButton(std::to_string(list).c_str(), gui->GetTextureByName(GetSplitImage(split)),
                                   GetSplitImageSize(split) * 1.5f, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                                   GetSplitColor(split))) {
                AddSplitEntry(SPLIT_TYPE_ITEM, list);
                ImGui::CloseCurrentPopup();
                shouldPopUpOpen = false;
            }
            UIWidgets::Tooltip(split.splitName.c_str());
            SplitsPopImageButtonStyle();

            if (slotIndex == 4) {
                slotIndex = -1;
            } else {
                ImGui::SameLine();
            }
            slotIndex++;
        }
        ImGui::EndPopup();
    }
}

void HandleDragAndDrop(size_t i) {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("SPLIT_DRAG", &i, sizeof(size_t));
        ImGui::ImageButton(std::to_string(splitList[i].splitId).c_str(),
                           gui->GetTextureByName(GetSplitImage(splitList[i])), GetSplitImageSize(splitList[i]),
                           ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), GetSplitColor(splitList[i]));
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SPLIT_DRAG")) {
            size_t srcIndex = *(const size_t*)payload->Data;
            if (srcIndex != i && srcIndex < splitList.size()) {
                auto item = splitList[srcIndex];
                splitList.erase(splitList.begin() + srcIndex);

                if (srcIndex < i) {
                    i--;
                }

                splitList.insert(splitList.begin() + i, item);
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void CheckSplitsCompleted(uint32_t index) {
    if (index == splitList.size() - 1) {
        gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_TIMESPLITS_COMPLETED] = GetTotalTime();
        gSaveContext.ship.stats.gameComplete = true;
    } else {
        splitList[index + 1].splitStatus = SPLIT_ACTIVE;
    }
}

void AddSplitEntryBySceneId(uint32_t sceneId) {
    TimesplitObject splitObject = GetSplitObjectBySceneId(sceneId);

    if (splitList.size() == 0) {
        splitObject.splitStatus = SPLIT_ACTIVE;
    }
    splitList.push_back(splitObject);
}

void AddSplitEntry(uint32_t splitType, uint32_t splitId) {
    TimesplitObject splitObject = GetSplitObject(splitType, splitId);

    if (splitList.size() == 0) {
        splitObject.splitStatus = SPLIT_ACTIVE;
    }
    splitList.push_back(splitObject);
}

void RemoveSplitEntry(uint32_t index) {
    if (index == GetCurrentActiveSplit(splitList)) {
        CheckSplitsCompleted(index);
    }

    splitList.erase(splitList.begin() + index);
}

void SkipSplitEntry(uint32_t index) {
    if (splitList[index].splitStatus == SPLIT_ACTIVE) {
        CheckSplitsCompleted(index);
    }
    splitList[index].splitStatus = SPLIT_SKIPPED;
}

void UpdateSplitBests() {
    for (auto& splits : splitList) {
        if (splits.splitCurrentTime < splits.splitPreviousBest || splits.splitPreviousBest == 0) {
            splits.splitPreviousBest = splits.splitCurrentTime;
        }
    }
}

void UpdateSplitStatus(uint32_t splitType, uint32_t splitId) {
    uint32_t activeIndex = GetCurrentActiveSplit(splitList);

    if (activeIndex == UINT32_MAX) {
        return;
    }

    if (splitList[activeIndex].splitType == splitType && splitList[activeIndex].splitId == splitId) {
        splitList[activeIndex].splitCurrentTime = GetTotalTime();
        splitList[activeIndex].splitStatus = SPLIT_COMPLETE;
        CheckSplitsCompleted(activeIndex);
    }
}

static bool IsUpgradeSplitDone(const TimesplitObject& split) {
    if (split.splitType != SPLIT_TYPE_ITEM) {
        return false;
    }
    switch (split.splitId) {
        case ITEM_SINGLE_MAGIC:
            return gSaveContext.isMagicAcquired;
        case ITEM_DOUBLE_MAGIC:
            return gSaveContext.isDoubleMagicAcquired;
        case ITEM_DOUBLE_DEFENSE:
            return gSaveContext.isDoubleDefenseAcquired;
        default:
            return false;
    }
}

void UpdateSplitStatusByUpgrades() {
    uint32_t activeIndex = GetCurrentActiveSplit(splitList);

    if (activeIndex == UINT32_MAX || !IsUpgradeSplitDone(splitList[activeIndex])) {
        return;
    }

    UpdateSplitStatus(splitList[activeIndex].splitType, splitList[activeIndex].splitId);
}

void SplitLoadComparisonList() {
    std::string filename = Ship::Context::GetPathRelativeToAppDirectory("SoHTimeSplitData.json");
    json compareFile;
    json listArray = nlohmann::json::array();

    std::ifstream inputFile(filename);
    if (inputFile.is_open()) {
        inputFile >> compareFile;
        inputFile.close();
    }

    if (compareFile.contains(savedLists[comparedIndex])) {
        listArray = compareFile[savedLists[comparedIndex]];
        comparisonList.clear();

        for (auto& data : listArray) {
            comparisonList.push_back(json_to_TimesplitObject(data));
        }
    }
}

void SplitSaveFileAction(uint32_t action, std::string listName) {
    std::string filename = "";

    if (action == SPLIT_CONVERT) {
        filename = Ship::Context::GetPathRelativeToAppDirectory("timesplitdata.json");
    } else {
        filename = Ship::Context::GetPathRelativeToAppDirectory("SoHTimeSplitData.json");
    }

    json saveFile;
    json listArray = nlohmann::json::array();

    std::ifstream inputFile(filename);
    if (inputFile.is_open()) {
        inputFile >> saveFile;
        inputFile.close();
    }

    if (action == SPLIT_SAVE) {
        for (auto& data : splitList) {
            listArray.push_back(TimesplitObject_to_json(data));
        }
        saveFile[listName] = listArray;

        std::ofstream outputFile(filename);
        if (outputFile.is_open()) {
            outputFile << saveFile.dump(4);
            outputFile.close();
        }
    }

    if (action == SPLIT_LOAD) {
        if (saveFile.contains(listName)) {
            listArray = saveFile[listName];
            splitList.clear();

            for (auto& data : listArray) {
                splitList.push_back(json_to_TimesplitObject(data));
            }
            splitList[0].splitStatus = SPLIT_ACTIVE;
        }
    }

    if (action == SPLIT_RETRIEVE) {
        savedLists.clear();

        for (auto& data : saveFile.items()) {
            if (!data.key().empty()) {
                savedLists.push_back(data.key());
            }
        }
        if (savedLists.size() == 0) {
            savedLists.push_back("Create a List First");
        }
    }

    if (action == SPLIT_CONVERT) {
        for (auto& data : saveFile.items()) {
            splitList.clear();
            for (auto& items : data.value()) {
                if (items["splitName"].get<std::string>() == "Skulltula Token") {
                    continue;
                }

                TimesplitObject splitObject;
                splitObject.splitName = items["splitName"].get<std::string>();
                splitObject.splitCurrentTime = items["splitTimeCurrent"];
                splitObject.splitPreviousBest = items["splitTimePreviousBest"];
                splitObject.splitStatus = SPLIT_INACTIVE;

                // 4 is the old SPLIT_TYPE_BOSS which does not exist anymore
                if (items["splitType"] == 4) {
                    splitObject.splitId = ITEM_NONE;
                    splitObject.splitType = SPLIT_TYPE_ITEM;
                    for (auto& split : splitObjectList) {
                        if (split.splitName == splitObject.splitName) {
                            splitObject.splitId = split.splitId;
                            splitObject.splitType = split.splitType;
                            break;
                        }
                    }
                } else {
                    // 5 is the old SPLIT_TYPE_ENTRANCE which is replaced by SPLIT_TYPE_SCENE
                    splitObject.splitId = items["splitID"];
                    splitObject.splitType = items["splitType"] == 5 ? SPLIT_TYPE_SCENE : SPLIT_TYPE_ITEM;
                }
                splitList.push_back(splitObject);
            }
            SplitSaveFileAction(SPLIT_SAVE, data.key().c_str());
        }
        splitList.clear();
    }
}

void RegisterTimesplits() {
    if (!std::filesystem::exists(Ship::Context::GetPathRelativeToAppDirectory("SoHTimeSplitData.json"))) {
        json initFile;
        std::ofstream file(Ship::Context::GetPathRelativeToAppDirectory("SoHTimeSplitData.json"));
        file << initFile.dump(4);
        file.close();
    }

    // Handles Converting any old save data into the new format.
    if (std::filesystem::exists(Ship::Context::GetPathRelativeToAppDirectory("timesplitdata.json"))) {
        SplitSaveFileAction(SPLIT_CONVERT, "");
        std::filesystem::rename(Ship::Context::GetPathRelativeToAppDirectory("timesplitdata.json"),
                                Ship::Context::GetPathRelativeToAppDirectory("timesplitdata.backup"));
    }

    SplitSaveFileAction(SPLIT_RETRIEVE, "");

    COND_HOOK(OnItemReceive, CVAR, [](GetItemEntry itemEntry) {
        if (itemEntry.modIndex == MOD_RANDOMIZER) {
            return;
        }

        if (itemEntry.itemId == ITEM_BOMBCHUS_20 || itemEntry.itemId == ITEM_BOMBCHUS_5) {
            itemEntry.itemId = ITEM_BOMBCHU;
        }
        if (itemEntry.itemId == ITEM_STICKS_5 || itemEntry.itemId == ITEM_STICKS_10) {
            itemEntry.itemId = ITEM_STICK;
        }
        if (itemEntry.itemId == ITEM_NUTS_5 || itemEntry.itemId == ITEM_NUTS_10) {
            itemEntry.itemId = ITEM_NUT;
        }
        if (itemEntry.itemId == ITEM_SWORD_BGS) {
            if (gSaveContext.bgsFlag == 0) {
                itemEntry.itemId = ITEM_SWORD_KNIFE;
            }
        }

        UpdateSplitStatus(SPLIT_TYPE_ITEM, (uint32_t)itemEntry.itemId);
    });

    COND_HOOK(OnPlayerUpdate, CVAR, []() { UpdateSplitStatusByUpgrades(); });

    COND_HOOK(OnPlayerBottleUpdate, CVAR,
              [](int16_t contents) { UpdateSplitStatus(SPLIT_TYPE_ITEM, (uint32_t)contents); });
    COND_HOOK(OnBossDefeat, CVAR, [](void* refActor) {
        Actor* actor = (Actor*)refActor;
        UpdateSplitStatus(SPLIT_TYPE_BOSS, (uint32_t)actor->id);
    });

    COND_HOOK(OnSceneInit, CVAR, [](int16_t sceneNum) { UpdateSplitStatus(SPLIT_TYPE_SCENE, (uint32_t)sceneNum); });
}
} // namespace TimeSplits

static RegisterShipInitFunc initFunc(TimeSplits::RegisterTimesplits, { CVAR_NAME });