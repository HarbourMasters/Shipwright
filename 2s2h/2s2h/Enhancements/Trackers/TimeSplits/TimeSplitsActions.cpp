#include "Timesplits.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include <ship/Context.h>
#include <ship/window/Window.h>
#include "2s2h/BenGui/UIWidgets.hpp"
#include <fstream>

#include "assets/archives/icon_item_static/icon_item_static_yar.h"

using json = nlohmann::json;

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Bg_Dy_Yoseizo/z_bg_dy_yoseizo.h"
uint64_t GetUnixTimestamp();
}

#include <fast/Fast3dGui.h>

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
                    std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                        ->GetTextureByName(GetItemImageById(list)),
                    GetItemImageSizeById(list) * 1.5f, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                    Ship_GetItemColorTint(list))) {
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
        ImGui::ImageButton(
            std::to_string(splitList[i].splitId).c_str(),
            std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                ->GetTextureByName(splitList[i].splitType == SPLIT_TYPE_NORMAL ? GetItemImageById(splitList[i].splitId)
                                                                               : gPauseUnusedCursorTex),
            splitList[i].splitType == SPLIT_TYPE_NORMAL ? GetItemImageSizeById(splitList[i].splitId)
                                                        : ImVec2(32.0f, 32.0f),
            ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
            splitList[i].splitType == SPLIT_TYPE_NORMAL ? Ship_GetItemColorTint(splitList[i].splitId)
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
        gSaveContext.save.shipSaveInfo.fileCompletedAt = GetUnixTimestamp();
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
        splitList[activeIndex].splitCurrentTime =
            ((GetUnixTimestamp() - gSaveContext.save.shipSaveInfo.fileCreatedAt) / 100);
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
        splitList[activeIndex].splitCurrentTime =
            ((GetUnixTimestamp() - gSaveContext.save.shipSaveInfo.fileCreatedAt) / 100);
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
        case ACTOR_BOSS_01:
            UpdateSplitStatusById(SPLIT_KILLED_ODOLWA);
            break;
        case ACTOR_BOSS_02:
            UpdateSplitStatusById(SPLIT_KILLED_TWINMOLD);
            break;
        case ACTOR_BOSS_03:
            UpdateSplitStatusById(SPLIT_KILLED_GYORG);
            break;
        case ACTOR_BOSS_07:
            UpdateSplitStatusById(SPLIT_KILLED_MAJORA);
            break;
        case ACTOR_BOSS_HAKUGIN:
            UpdateSplitStatusById(SPLIT_KILLED_GOHT);
            break;
        case ACTOR_BG_DY_YOSEIZO:
        case ACTOR_EN_ELFGRP:
            if (specialType == GREAT_FAIRY_TYPE_MAGIC) {
                if (gSaveContext.save.saveInfo.playerData.isMagicAcquired != true) {
                    UpdateSplitStatusById(SPLIT_SINGLE_MAGIC);
                }
            } else if (specialType == GREAT_FAIRY_TYPE_WISDOM) {
                if (gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired != true) {
                    UpdateSplitStatusById(SPLIT_DOUBLE_MAGIC);
                }
            } else if (specialType == GREAT_FAIRY_TYPE_COURAGE) {
                if (gSaveContext.save.saveInfo.playerData.doubleDefense != true) {
                    UpdateSplitStatusById(SPLIT_DOUBLE_DEFENSE);
                }
            }
            break;
        default:
            break;
    }

    if (activeIndex == -1) {
        return;
    }
}

void SplitLoadComparisonList() {
    std::string filename = Ship::Context::GetPathRelativeToAppDirectory("2S2HTimeSplitData.json");
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
    std::string filename = Ship::Context::GetPathRelativeToAppDirectory("2S2HTimeSplitData.json");
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
}

void RegisterTimesplits() {
    SplitSaveFileAction(SPLIT_RETRIEVE, "");

    COND_HOOK(OnItemGive, CVAR, [](u8 item) {
        if (item == ITEM_HEART_PIECE_2) {
            item = ITEM_HEART_PIECE;
        }
        if (item == ITEM_LONGSHOT) {
            item = ITEM_POTION_RED;
        }
        if (item >= ITEM_BOMBCHUS_20 && item <= ITEM_BOMBCHUS_5) {
            item = ITEM_BOMBCHU;
        }

        UpdateSplitStatusById((uint32_t)item);
    });

    COND_HOOK(OnBottleContentsUpdate, CVAR, [](u8 item) { UpdateSplitStatusById((uint32_t)item); });
    COND_HOOK(OnBossDefeated, CVAR, [](int16_t actorId) { GetSplitByActorId(actorId); });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_GREAT_FAIRY, CVAR, {
        Actor* actor = va_arg(args, Actor*);

        GetSplitByActorId(actor->id, GREAT_FAIRY_GET_TYPE(actor));
    });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_STRAY_FAIRY_MANAGER, CVAR, {
        Actor* actor = va_arg(args, Actor*);

        GetSplitByActorId(actor->id, GREAT_FAIRY_GET_TYPE(actor));
    });
    COND_HOOK(OnSceneInit, CVAR, [](s8 sceneId, s8 spawnNum) { UpdateSplitStatusBySceneId(sceneId); });
}

static RegisterShipInitFunc initFunc(RegisterTimesplits, { CVAR_NAME });
