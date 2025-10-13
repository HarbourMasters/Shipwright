#include "TimeSplits.h"
#include <libultraship/libultraship.h>
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include <fstream>
#include <filesystem>

#include "soh/Enhancements/randomizer/static_data.h"
#include "assets/textures/icon_item_static/icon_item_static.h"

using json = nlohmann::json;

extern "C" {
#include "variables.h"
uint64_t GetUnixTimestamp();
}

#define CVAR_NAME "gSettings.TimeSplits.Enable"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

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
        split.splitType = SPLIT_TYPE_NORMAL;
    }

    return split;
}

uint32_t GetCurrentActiveSplit(std::vector<TimesplitObject> list) {
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

TimesplitObject GetSplitObjectById(uint32_t itemId) {
    TimesplitObject splitObject;
    for (auto& list : splitObjectList) {
        if (list.splitId == itemId) {
            splitObject = list;
        }
    }
    return splitObject;
}

void HandlePopUpContext(uint32_t popupId) {
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
            SplitsPushImageButtonStyle();
            if (ImGui::ImageButton(
                    std::to_string(list).c_str(),
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(GetItemImageById(list)),
                    GetItemImageSizeById(list) * 1.5f, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                    GetItemColor(list))) {
                AddSplitEntryById(list);
                ImGui::CloseCurrentPopup();
                shouldPopUpOpen = false;
            }
            UIWidgets::Tooltip(GetSplitObjectById(list).splitName.c_str());
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
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("SPLIT_DRAG", &i, sizeof(size_t));
        ImGui::ImageButton(std::to_string(splitList[i].splitId).c_str(),
                           Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                               splitList[i].splitType == SPLIT_TYPE_NORMAL ? GetItemImageById(splitList[i].splitId)
                                                                           : "gPauseUnusedCursorTex"),
                           splitList[i].splitType == SPLIT_TYPE_NORMAL ? GetItemImageSizeById(splitList[i].splitId)
                                                                       : ImVec2(32.0f, 32.0f),
                           ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                           splitList[i].splitType == SPLIT_TYPE_NORMAL ? GetItemColor(splitList[i].splitId)
                                                                       : ImVec4(1, 1, 1, 1));
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
        gSaveContext.ship.stats.fileCreatedAt = GetUnixTimestamp();
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

void AddSplitEntryById(uint32_t itemId) {
    TimesplitObject splitObject = GetSplitObjectById(itemId);

    if (splitList.size() == 0) {
        splitObject.splitStatus = SPLIT_ACTIVE;
    }
    splitList.push_back(splitObject);
}

void RemoveSplitEntry(uint32_t splitId, uint32_t index) {
    uint32_t activeIndex = GetCurrentActiveSplit(splitList);

    if (activeIndex != -1) {
        if (splitList[activeIndex].splitId == splitId) {
            CheckSplitsCompleted(activeIndex);
        }
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

void UpdateSplitStatusBySceneId(uint32_t sceneId) {
    uint32_t activeIndex = GetCurrentActiveSplit(splitList);

    if (activeIndex == -1) {
        return;
    }

    if (splitList[activeIndex].splitType == SPLIT_TYPE_SCENE && splitList[activeIndex].splitId == sceneId) {
        splitList[activeIndex].splitCurrentTime = GetTotalTime();
        splitList[activeIndex].splitStatus = SPLIT_COMPLETE;

        if (activeIndex == splitList.size() - 1) {
            CheckSplitsCompleted(activeIndex);
        } else {
            splitList[activeIndex + 1].splitStatus = SPLIT_ACTIVE;
        }
    }
}

void UpdateSplitStatusById(uint32_t itemId) {
    uint32_t activeIndex = GetCurrentActiveSplit(splitList);

    if (activeIndex == -1) {
        return;
    }

    if (splitList[activeIndex].splitId == itemId) {
        splitList[activeIndex].splitCurrentTime = GetTotalTime();
        splitList[activeIndex].splitStatus = SPLIT_COMPLETE;

        if (activeIndex == splitList.size() - 1) {
            CheckSplitsCompleted(activeIndex);
        } else {
            splitList[activeIndex + 1].splitStatus = SPLIT_ACTIVE;
        }
    }
}

void GetSplitByActorId(int16_t actorId, uint32_t specialType = 0) {
    uint32_t activeIndex = GetCurrentActiveSplit(splitList);

    switch (actorId) {
        case ACTOR_BOSS_GOMA:
            UpdateSplitStatusById(SPLIT_KILLED_GOHMA);
            break;
        case ACTOR_BOSS_DODONGO:
            UpdateSplitStatusById(SPLIT_KILLED_KING_DODONGO);
            break;
        case ACTOR_BOSS_VA:
            UpdateSplitStatusById(SPLIT_KILLED_BARINADE);
            break;
        case ACTOR_BOSS_GANONDROF:
            UpdateSplitStatusById(SPLIT_KILLED_PHANTOM_GANON);
            break;
        case ACTOR_BOSS_FD2:
            UpdateSplitStatusById(SPLIT_KILLED_VOLVAGIA);
            break;
        case ACTOR_BOSS_MO:
            UpdateSplitStatusById(SPLIT_KILLED_MORPHA);
            break;
        case ACTOR_BOSS_SST:
            UpdateSplitStatusById(SPLIT_KILLED_BONGO_BONGO);
            break;
        case ACTOR_BOSS_TW:
            UpdateSplitStatusById(SPLIT_KILLED_TWINROVA);
            break;
        case ACTOR_BOSS_GANON:
            UpdateSplitStatusById(SPLIT_KILLED_GANONDORF);
            break;
        case ACTOR_BOSS_GANON2:
            UpdateSplitStatusById(SPLIT_KILLED_GANON);
            break;
        default:
            break;
    }

    if (activeIndex == -1) {
        return;
    }
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
            savedLists.push_back(data.key());
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
                // 4 is the old SPLIT_TYPE_BOSS which does not exist anymore
                if (items["splitType"] == 4) {
                    splitObject.splitId = ITEM_NONE;
                    for (auto& split : splitObjectList) {
                        if (split.splitName == items["splitName"].get<std::string>()) {
                            splitObject.splitId = split.splitId;
                            break;
                        }
                    }
                } else {
                    splitObject.splitId = items["splitID"];
                }

                splitObject.splitName = items["splitName"].get<std::string>();
                splitObject.splitCurrentTime = items["splitTimeCurrent"];
                splitObject.splitPreviousBest = items["splitTimePreviousBest"];
                splitObject.splitStatus = SPLIT_INACTIVE;
                // 5 is the old SPLIT_TYPE_ENTRANCE which is replaced by SPLIT_TYPE_SCENE
                if (items["splitType"] == 5) {
                    splitObject.splitType = SPLIT_TYPE_SCENE;
                } else {
                    splitObject.splitType = SPLIT_TYPE_NORMAL;
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
            if (itemEntry.itemId == RG_MAGIC_SINGLE) {
                itemEntry.itemId = SPLIT_SINGLE_MAGIC;
            }
            if (itemEntry.itemId == RG_MAGIC_DOUBLE) {
                itemEntry.itemId = SPLIT_DOUBLE_MAGIC;
            }
            if (itemEntry.itemId == RG_DOUBLE_DEFENSE) {
                itemEntry.itemId = SPLIT_DOUBLE_DEFENSE;
            }
        } else {
            if (itemEntry.itemId == ITEM_BOMBCHUS_20 || itemEntry.itemId == ITEM_BOMBCHUS_5) {
                itemEntry.itemId = ITEM_BOMBCHU;
            }
            if (itemEntry.itemId == ITEM_STICKS_5 || itemEntry.itemId == ITEM_STICKS_10) {
                itemEntry.itemId == ITEM_STICK;
            }
            if (itemEntry.itemId == ITEM_NUTS_5 || itemEntry.itemId == ITEM_NUTS_10) {
                itemEntry.itemId == ITEM_NUT;
            }
        }

        UpdateSplitStatusById((uint32_t)itemEntry.itemId);
    });

    COND_HOOK(OnPlayerBottleUpdate, CVAR, [](u8 item) { UpdateSplitStatusById((uint32_t)item); });
    COND_HOOK(OnBossDefeat, CVAR, [](void* refActor) {
        Actor* actor = (Actor*)refActor;
        GetSplitByActorId(actor->id);
    });

    COND_HOOK(OnSceneInit, CVAR, [](int16_t sceneNum) { UpdateSplitStatusBySceneId(sceneNum); });
}

static RegisterShipInitFunc initFunc(RegisterTimesplits, { CVAR_NAME });