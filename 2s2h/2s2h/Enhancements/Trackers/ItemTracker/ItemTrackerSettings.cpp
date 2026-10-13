#include "ItemTrackerSettings.h"
#include "ItemTracker.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "ShipUtils.h"
#include "ship/config/Config.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/BenPort.h"

namespace BenGui {
extern std::shared_ptr<ItemTrackerWindow> mItemTrackerWindow;
}

void ItemTrackerSettingsWindow::UpdateElement() {
}

#define WIDGET_COLOR UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5))
#define CVAR_NAME_VISIBILITY_MODE "gSettings.ItemTracker.VisibilityMode"
#define CVAR_NAME_VISIBILITY_BTN "gSettings.ItemTracker.VisibilityBtn"
#define CVAR_VISIBILITY_MODE CVarGetInteger(CVAR_NAME_VISIBILITY_MODE, ITEM_TRACKER_VISIBILITY_MODE_ALWAYS)
#define CVAR_VISIBILITY_BTN CVarGetInteger(CVAR_NAME_VISIBILITY_BTN, BTN_CUSTOM_MODIFIER1)

static const char* windowTypes[2] = { "Floating", "Window" };

static std::unordered_map<int32_t, const char*> sItemTrackerVisibilityModes = {
    { ITEM_TRACKER_VISIBILITY_MODE_ALWAYS, "Always" },
    { ITEM_TRACKER_VISIBILITY_MODE_ONLY_ON_PAUSE_MENU, "Only on Pause Menu" },
    { ITEM_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE, "Button Toggle" },
    { ITEM_TRACKER_VISIBILITY_MODE_BUTTON_HOLD, "Button Hold" },
};

std::vector<TrackerGroup> itemTrackerGroupsAvailable;
void SaveItemTrackerLayout();

std::vector<std::pair<TrackerItemType, u32>> GetItemsFromRange(TrackerItemType itemType, u32 start, u32 end) {
    std::vector<std::pair<TrackerItemType, u32>> items;
    for (u32 itemId = start; itemId <= end; itemId++) {
        items.push_back({ itemType, itemId });
    }
    return items;
}

// Some items don't have a 1:1 with rando items (eg ITEM_BOMB != RI_BOMBS_5) so we just handle them manually
static const std::unordered_map<u32, const char*> sVanillaItemNames = {
    { ITEM_BOMB, "Bombs" },
    { ITEM_BOMBCHU, "Bombchus" },
};

std::string GetItemTrackerItemName(TrackerItemType itemType, u32 itemId) {
    switch (itemType) {
        case TRACKER_ITEM_RANDO: {
            return Rando::StaticData::Items[(RandoItemId)itemId].name;
        } break;
        case TRACKER_ITEM_SLOT: {
            u32 vanillaItemId = GetVanillaItemIdForSlot(itemId);
            auto vanillaItemName = sVanillaItemNames.find(vanillaItemId);
            if (vanillaItemName != sVanillaItemNames.end()) {
                return vanillaItemName->second;
            }
            RandoItemId randoItemId = Rando::StaticData::GetItemIdFromVanillaItemId(vanillaItemId);
            return Rando::StaticData::Items[randoItemId].name;
        } break;
        case TRACKER_ITEM_SWORD: {
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_KOKIRI) {
                return Rando::StaticData::Items[RI_SWORD_KOKIRI].name;
            }
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_RAZOR) {
                return Rando::StaticData::Items[RI_SWORD_RAZOR].name;
            }
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_GILDED) {
                return Rando::StaticData::Items[RI_SWORD_GILDED].name;
            }
            return "Sword (None)";
        } break;
        case TRACKER_ITEM_SHIELD: {
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) == EQUIP_VALUE_SHIELD_HERO) {
                return Rando::StaticData::Items[RI_SHIELD_HERO].name;
            }
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) == EQUIP_VALUE_SHIELD_MIRROR) {
                return Rando::StaticData::Items[RI_SHIELD_MIRROR].name;
            }
            return "Shield (None)";
        } break;
        case TRACKER_ITEM_WALLET: {
            if (CUR_UPG_VALUE(UPG_WALLET) >= 3) {
                return Rando::StaticData::Items[RI_WALLET_TYCOON].name;
            }
            if (CUR_UPG_VALUE(UPG_WALLET) >= 2) {
                return Rando::StaticData::Items[RI_WALLET_GIANT].name;
            }
            if (CUR_UPG_VALUE(UPG_WALLET) >= 1) {
                return Rando::StaticData::Items[RI_WALLET_ADULT].name;
            }
            return "Wallet (None)";
        } break;
        case TRACKER_ITEM_MAGIC: {
            if (gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired) {
                return Rando::StaticData::Items[RI_DOUBLE_MAGIC].name;
            }
            if (gSaveContext.save.saveInfo.playerData.isMagicAcquired) {
                return Rando::StaticData::Items[RI_SINGLE_MAGIC].name;
            }
            return "Magic (None)";
        } break;
        default:
            break;
    }

    return "";
}

struct DragDropPayload {
    TrackerItemType itemType;
    u32 itemId;
    int sourceGroupIndex;
    int sourceItemIndex;
    bool fromAddMode;
};

void DrawItemTrackerGroupPreview(TrackerGroup& group, bool addMode, int groupIndex) {
    static std::pair<TrackerItemType, u32> selectedItem = { TRACKER_ITEM_RANDO, 0 };

    if (ImGui::BeginChild(group.name.c_str(), ImVec2(0, 0),
                          ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX |
                              ImGuiChildFlags_AutoResizeY)) {
        if (ImGui::BeginTable(group.name.c_str(), group.columns)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

            // Use index-based loop to avoid iterator invalidation
            for (size_t itemIndex = 0; itemIndex < group.items.size(); itemIndex++) {
                auto& [itemType, itemId] = group.items[itemIndex];
                ImGui::TableNextColumn();
                ImGui::PushID((int)itemIndex);
                if (DrawItemTrackerSlot(itemType, itemId, 1.0f, true)) {
                    if (addMode) {
                        selectedItem = { itemType, itemId };
                        ImGui::OpenPopup("AddToGroup");
                    } else {
                        group.items.erase(
                            std::remove(group.items.begin(), group.items.end(), std::make_pair(itemType, itemId)),
                            group.items.end());
                        SaveItemTrackerLayout();
                    }
                }

                // Drag and drop functionality
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    DragDropPayload payload;
                    payload.itemType = itemType;
                    payload.itemId = itemId;
                    payload.sourceGroupIndex = groupIndex;
                    payload.sourceItemIndex = (int)itemIndex;
                    payload.fromAddMode = addMode;
                    ImGui::SetDragDropPayload("TRACKER_ITEM", &payload, sizeof(DragDropPayload));

                    // Display preview while dragging
                    DrawItemTrackerSlot(itemType, itemId, 0.5f, true);
                    ImGui::EndDragDropSource();
                }

                // Only allow dropping on removeMode slots
                if (!addMode && ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* imguiPayload = ImGui::AcceptDragDropPayload("TRACKER_ITEM")) {
                        DragDropPayload* payload = (DragDropPayload*)imguiPayload->Data;

                        if (payload->fromAddMode) {
                            // Dragging from addMode to removeMode: insert at this position
                            group.items.insert(group.items.begin() + itemIndex,
                                               std::make_pair(payload->itemType, payload->itemId));
                            SaveItemTrackerLayout();
                        } else {
                            // Dragging from removeMode to removeMode: move item
                            if (payload->sourceGroupIndex == groupIndex) {
                                // Moving within same group
                                if (payload->sourceItemIndex != (int)itemIndex) {
                                    auto item = group.items[payload->sourceItemIndex];
                                    group.items.erase(group.items.begin() + payload->sourceItemIndex);

                                    // Adjust target index if we removed an item before it
                                    int targetIndex = (int)itemIndex;
                                    if (payload->sourceItemIndex < (int)itemIndex) {
                                        targetIndex--;
                                    }
                                    group.items.insert(group.items.begin() + targetIndex, item);
                                    SaveItemTrackerLayout();
                                }
                            } else {
                                // Moving between different groups
                                auto item = std::make_pair(payload->itemType, payload->itemId);
                                itemTrackerGroups[payload->sourceGroupIndex].items.erase(
                                    itemTrackerGroups[payload->sourceGroupIndex].items.begin() +
                                    payload->sourceItemIndex);
                                group.items.insert(group.items.begin() + itemIndex, item);
                                SaveItemTrackerLayout();
                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                // Check for popup in the same ID scope
                if (addMode && ImGui::BeginPopup("AddToGroup")) {
                    UIWidgets::PushStyleMenuItem(WIDGET_COLOR);
                    ImGui::SeparatorText("Select group to add item to");
                    for (size_t i = 0; i < itemTrackerGroups.size(); i++) {
                        if (ImGui::Selectable(itemTrackerGroups[i].name.c_str(), false)) {
                            itemTrackerGroups[i].items.push_back(selectedItem);
                            SaveItemTrackerLayout();
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    UIWidgets::PopStyleMenuItem();
                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }
            ImGui::PopStyleVar(2);
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
}

bool DrawTrackerWindowOptions(int32_t windowIndex, TrackerGroup& group) {
    ImGui::PushID(windowIndex);
    int32_t columns = group.columns;
    float scale = group.scale;
    std::string trackerInputRename;
    if (windowIndex >= 0) {
        if (UIWidgets::InputString("##name", &trackerInputRename,
                                   UIWidgets::InputOptions()
                                       .LabelPosition(UIWidgets::LabelPosition::None)
                                       .Color(WIDGET_COLOR)
                                       .PlaceholderText(group.name)
                                       .DefaultValue(group.name))) {
            if (!trackerInputRename.empty() && trackerInputRename != group.name) {
                group.name = trackerInputRename;
                SaveItemTrackerLayout();
            }
        }
    }
    if (UIWidgets::SliderInt("Columns", &columns,
                             UIWidgets::IntSliderOptions()
                                 .Min(1)
                                 .Max(12)
                                 .DefaultValue(6)
                                 .LabelPosition(UIWidgets::LabelPosition::None)
                                 .Format("Columns: %i")
                                 .Color(WIDGET_COLOR))) {
        group.columns = columns;
        SaveItemTrackerLayout();
    }
    if (CVarGetInteger("gSettings.ItemTracker.WindowGroup", 0)) {
        if (UIWidgets::SliderFloat("Scale", &scale,
                                   UIWidgets::FloatSliderOptions()
                                       .Min(0.2f)
                                       .Max(3.0f)
                                       .DefaultValue(1.0f)
                                       .LabelPosition(UIWidgets::LabelPosition::None)
                                       .Format("Scale: %.1f")
                                       .Step(0.5f)
                                       .Color(WIDGET_COLOR))) {
            group.scale = scale;
            SaveItemTrackerLayout();
        }
    }
    if (UIWidgets::Button("Remove Group", { .color = UIWidgets::Colors::Red })) {
        itemTrackerGroups.erase(itemTrackerGroups.begin() + windowIndex);
        SaveItemTrackerLayout();
        ImGui::PopID();
        return true;
    }
    ImGui::PopID();
    return false;
}

void LoadAvailableWindows() {
    itemTrackerGroupsAvailable.clear();

    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Inventory",
        .columns = 6,
        .scale = 1.0f,
        .items = GetItemsFromRange(TRACKER_ITEM_SLOT, SLOT_OCARINA, SLOT_BOTTLE_6),
    });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Masks",
        .columns = 6,
        .scale = 1.0f,
        .items = GetItemsFromRange(TRACKER_ITEM_SLOT, SLOT_MASK_POSTMAN, SLOT_MASK_FIERCE_DEITY),
    });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Songs",
        .columns = 5,
        .scale = 1.0f,
        .items = {
            { TRACKER_ITEM_RANDO, RI_SONG_TIME },
            { TRACKER_ITEM_RANDO, RI_SONG_HEALING },
            { TRACKER_ITEM_RANDO, RI_SONG_EPONA },
            { TRACKER_ITEM_RANDO, RI_SONG_SOARING },
            { TRACKER_ITEM_RANDO, RI_SONG_STORMS },
            { TRACKER_ITEM_RANDO, RI_SONG_SONATA },
            { TRACKER_ITEM_RANDO, RI_SONG_LULLABY },
            { TRACKER_ITEM_RANDO, RI_SONG_NOVA },
            { TRACKER_ITEM_RANDO, RI_SONG_ELEGY },
            { TRACKER_ITEM_RANDO, RI_SONG_OATH },
            { TRACKER_ITEM_RANDO, RI_SONG_DOUBLE_TIME },
            { TRACKER_ITEM_RANDO, RI_SONG_INVERTED_TIME },
            { TRACKER_ITEM_RANDO, RI_SONG_SUN },
            { TRACKER_ITEM_RANDO, RI_SONG_SARIA },
        },
    });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Quest",
        .columns = 5,
        .scale = 1.0f,
        .items = {
            { TRACKER_ITEM_RANDO, RI_REMAINS_ODOLWA },
            { TRACKER_ITEM_RANDO, RI_REMAINS_GOHT },
            { TRACKER_ITEM_RANDO, RI_REMAINS_GYORG },
            { TRACKER_ITEM_RANDO, RI_REMAINS_TWINMOLD },
            { TRACKER_ITEM_RANDO, RI_BOMBERS_NOTEBOOK },
            { TRACKER_ITEM_SWORD, RI_PROGRESSIVE_SWORD },
            { TRACKER_ITEM_SHIELD, RI_SHIELD_HERO },
            { TRACKER_ITEM_MAGIC, RI_PROGRESSIVE_MAGIC },
            { TRACKER_ITEM_RANDO, RI_DOUBLE_DEFENSE },
            { TRACKER_ITEM_WALLET, RI_PROGRESSIVE_WALLET },
        },
    });

    // Ordered by the inventory slot each one shares, so the rows line up with the three trade slots
    // the Inventory group shows.
    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Trade Items",
        .columns = 5,
        .scale = 1.0f,
        .items = {
            { TRACKER_ITEM_RANDO, RI_MOONS_TEAR },
            { TRACKER_ITEM_RANDO, RI_DEED_LAND },
            { TRACKER_ITEM_RANDO, RI_DEED_SWAMP },
            { TRACKER_ITEM_RANDO, RI_DEED_MOUNTAIN },
            { TRACKER_ITEM_RANDO, RI_DEED_OCEAN },
            { TRACKER_ITEM_RANDO, RI_ROOM_KEY },
            { TRACKER_ITEM_RANDO, RI_LETTER_TO_MAMA },
            { TRACKER_ITEM_RANDO, RI_LETTER_TO_KAFEI },
            { TRACKER_ITEM_RANDO, RI_PENDANT_OF_MEMORIES },
        },
    });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Tokens",
        .columns = 2,
        .scale = 1.0f,
        .items = {
            { TRACKER_ITEM_RANDO, RI_GS_TOKEN_SWAMP },
            { TRACKER_ITEM_RANDO, RI_GS_TOKEN_OCEAN },
        },
    });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Stray Fairies",
        .columns = 5,
        .scale = 1.0f,
        .items = {
            { TRACKER_ITEM_RANDO, RI_CLOCK_TOWN_STRAY_FAIRY },
            { TRACKER_ITEM_RANDO, RI_WOODFALL_STRAY_FAIRY },
            { TRACKER_ITEM_RANDO, RI_SNOWHEAD_STRAY_FAIRY },
            { TRACKER_ITEM_RANDO, RI_GREAT_BAY_STRAY_FAIRY },
            { TRACKER_ITEM_RANDO, RI_STONE_TOWER_STRAY_FAIRY },
        },
    });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Dungeon",
        .columns = 4,
        .scale = 1.0f,
        .items = {
            { TRACKER_ITEM_RANDO, RI_WOODFALL_BOSS_KEY },
            { TRACKER_ITEM_RANDO, RI_SNOWHEAD_BOSS_KEY },
            { TRACKER_ITEM_RANDO, RI_GREAT_BAY_BOSS_KEY },
            { TRACKER_ITEM_RANDO, RI_STONE_TOWER_BOSS_KEY },
            { TRACKER_ITEM_RANDO, RI_WOODFALL_SMALL_KEY },
            { TRACKER_ITEM_RANDO, RI_SNOWHEAD_SMALL_KEY },
            { TRACKER_ITEM_RANDO, RI_GREAT_BAY_SMALL_KEY },
            { TRACKER_ITEM_RANDO, RI_STONE_TOWER_SMALL_KEY },
        },
    });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Frogs",
        .columns = 4,
        .scale = 1.0f,
        .items = GetItemsFromRange(TRACKER_ITEM_RANDO, RI_FROG_BLUE, RI_FROG_WHITE),
    });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Ocarina Buttons",
        .columns = 5,
        .scale = 1.0f,
        .items = GetItemsFromRange(TRACKER_ITEM_RANDO, RI_OCARINA_BUTTON_A, RI_OCARINA_BUTTON_C_UP),
    });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{ .name = "Boss Souls",
                                                       .columns = 5,
                                                       .scale = 1.0f,
                                                       .items = {
                                                           { TRACKER_ITEM_RANDO, RI_SOUL_BOSS_ODOLWA },
                                                           { TRACKER_ITEM_RANDO, RI_SOUL_BOSS_GOHT },
                                                           { TRACKER_ITEM_RANDO, RI_SOUL_BOSS_GYORG },
                                                           { TRACKER_ITEM_RANDO, RI_SOUL_BOSS_TWINMOLD },
                                                           { TRACKER_ITEM_RANDO, RI_SOUL_BOSS_MAJORA },
                                                       } });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Enemy Souls",
        .columns = 6,
        .scale = 1.0f,
        .items = GetItemsFromRange(TRACKER_ITEM_RANDO, RI_SOUL_ENEMY_ALIEN, RI_SOUL_ENEMY_WOLFOS),
    });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{ .name = "Owl Statues",
                                                       .columns = 5,
                                                       .scale = 1.0f,
                                                       .items = {
                                                           { TRACKER_ITEM_RANDO, RI_OWL_CLOCK_TOWN_SOUTH },
                                                           { TRACKER_ITEM_RANDO, RI_OWL_MILK_ROAD },
                                                           { TRACKER_ITEM_RANDO, RI_OWL_SOUTHERN_SWAMP },
                                                           { TRACKER_ITEM_RANDO, RI_OWL_WOODFALL },
                                                           { TRACKER_ITEM_RANDO, RI_OWL_MOUNTAIN_VILLAGE },
                                                           { TRACKER_ITEM_RANDO, RI_OWL_SNOWHEAD },
                                                           { TRACKER_ITEM_RANDO, RI_OWL_GREAT_BAY_COAST },
                                                           { TRACKER_ITEM_RANDO, RI_OWL_ZORA_CAPE },
                                                           { TRACKER_ITEM_RANDO, RI_OWL_IKANA_CANYON },
                                                           { TRACKER_ITEM_RANDO, RI_OWL_STONE_TOWER },
                                                       } });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Time",
        .columns = 6,
        .scale = 1.0f,
        .items = {
            { TRACKER_ITEM_RANDO, RI_TIME_DAY_1 },
            { TRACKER_ITEM_RANDO, RI_TIME_NIGHT_1 },
            { TRACKER_ITEM_RANDO, RI_TIME_DAY_2 },
            { TRACKER_ITEM_RANDO, RI_TIME_NIGHT_2 },
            { TRACKER_ITEM_RANDO, RI_TIME_DAY_3 },
            { TRACKER_ITEM_RANDO, RI_TIME_NIGHT_3 },
        },
    });

    itemTrackerGroupsAvailable.push_back(TrackerGroup{
        .name = "Misc",
        .columns = 6,
        .scale = 1.0f,
        .items = {
            { TRACKER_ITEM_RANDO, RI_TRIFORCE_PIECE },
            { TRACKER_ITEM_RANDO, RI_ABILITY_SWIM },
        },
    });
}

void ApplyDefaultItemPreset() {
    itemTrackerGroups.clear();

    std::set<std::string> defaultGroups = {
        "Inventory", "Masks", "Songs", "Quest", "Tokens", "Stray Fairies", "Dungeon",
    };

    for (auto& group : itemTrackerGroupsAvailable) {
        if (defaultGroups.count(group.name)) {
            if (group.name == "Songs") {
                // Limit songs to first 10 in default preset
                TrackerGroup limitedGroup = group;
                limitedGroup.items =
                    std::vector<std::pair<TrackerItemType, u32>>(group.items.begin(), group.items.begin() + 10);
                itemTrackerGroups.push_back(limitedGroup);
                continue;
            }

            itemTrackerGroups.push_back(group);
        }
    }
}

void SaveItemTrackerLayout() {
    auto itemTrackerLayout = nlohmann::json::array();
    for (auto& group : itemTrackerGroups) {
        auto groupJson = nlohmann::json::object({
            { "name", group.name },
            { "columns", group.columns },
            { "scale", group.scale },
            { "items", group.items },
        });
        itemTrackerLayout.push_back(groupJson);
    }
    Ship::Context::GetRawInstance()->GetConfig()->SetBlock("CVars.ItemTrackerLayout", itemTrackerLayout);
    Ship::Context::GetRawInstance()->GetConfig()->Save();
}

void LoadItemTrackerConfig() {
    auto allConfig = Ship::Context::GetRawInstance()->GetConfig()->GetNestedJson();

    // Verify that the config has CVars.ItemTrackerLayout and its an array
    if (allConfig.find("CVars") != allConfig.end() && allConfig["CVars"].is_object() &&
        allConfig["CVars"].find("ItemTrackerLayout") != allConfig["CVars"].end() &&
        allConfig["CVars"]["ItemTrackerLayout"].is_array()) {

        itemTrackerGroups.clear();
        auto& itemTrackerLayout = allConfig["CVars"]["ItemTrackerLayout"];
        for (auto& group : itemTrackerLayout) {
            std::vector<std::pair<TrackerItemType, u32>> items;

            // Manually parse items array
            if (group.contains("items") && group["items"].is_array()) {
                for (auto& item : group["items"]) {
                    if (item.is_array() && item.size() >= 2) {
                        items.push_back({ static_cast<TrackerItemType>(item[0].get<int>()), item[1].get<u32>() });
                    }
                }
            }

            itemTrackerGroups.push_back(TrackerGroup{
                .name = group.value("name", std::string("")),
                .columns = group.value("columns", (u8)6),
                .scale = group.value("scale", 1.0f),
                .items = items,
            });
        }
    }
}

void DrawTrackerOptions() {
    if (ImGui::BeginTable("OptionsTable", 3)) {
        ImGui::TableNextColumn();
        if (CVarGetInteger("gWindows.ItemTracker", 0)) {
            UIWidgets::WindowButton("Disable Item Tracker", "gWindows.ItemTracker", BenGui::mItemTrackerWindow,
                                    { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red });
        } else {
            UIWidgets::WindowButton("Enable Item Tracker", "gWindows.ItemTracker", BenGui::mItemTrackerWindow,
                                    { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Green });
        }

        UIWidgets::CVarCheckbox("Split Window Groups", "gSettings.ItemTracker.WindowGroup");
        UIWidgets::CVarCheckbox("Show Item Counts", "gSettings.ItemTracker.ItemCounts",
                                UIWidgets::CheckboxOptions().DefaultValue(true));
        ImGui::TextWrapped(
            "Click or drag & drop individual items, or use the corner buttons to add/remove entire groups.");
        ImGui::TableNextColumn();

        UIWidgets::CVarCombobox("Window Type", "gSettings.ItemTracker.WindowType", windowTypes,
                                UIWidgets::ComboboxOptions()
                                    .ComponentAlignment(UIWidgets::ComponentAlignment::Right)
                                    .LabelPosition(UIWidgets::LabelPosition::Far));
        UIWidgets::CVarCombobox("Visibility", CVAR_NAME_VISIBILITY_MODE, &sItemTrackerVisibilityModes,
                                UIWidgets::ComboboxOptions()
                                    .DefaultIndex(ITEM_TRACKER_VISIBILITY_MODE_ALWAYS)
                                    .ComponentAlignment(UIWidgets::ComponentAlignment::Right)
                                    .LabelPosition(UIWidgets::LabelPosition::Far));
        if (CVAR_VISIBILITY_MODE == ITEM_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE ||
            CVAR_VISIBILITY_MODE == ITEM_TRACKER_VISIBILITY_MODE_BUTTON_HOLD) {
            UIWidgets::CVarBtnSelector("Button Combination:", CVAR_NAME_VISIBILITY_BTN,
                                       UIWidgets::BtnSelectorOptions().DefaultValue(BTN_CUSTOM_MODIFIER1));
        }

        ImGui::TableNextColumn();

        if (UIWidgets::Button("Restore Default Groups", { .color = UIWidgets::Colors::Gray })) {
            ApplyDefaultItemPreset();
            SaveItemTrackerLayout();
        }

        if (!CVarGetInteger("gSettings.ItemTracker.WindowGroup", 0)) {
            UIWidgets::CVarSliderFloat("Scale", "gSettings.ItemTracker.Scale",
                                       UIWidgets::FloatSliderOptions()
                                           .Min(0.2f)
                                           .Max(3.0f)
                                           .DefaultValue(1.0f)
                                           .LabelPosition(UIWidgets::LabelPosition::None)
                                           .Format("Scale: %.1f")
                                           .Step(0.5f)
                                           .Color(WIDGET_COLOR));
        }

        UIWidgets::CVarSliderFloat("Opacity", "gSettings.ItemTracker.Opacity",
                                   UIWidgets::FloatSliderOptions()
                                       .Min(0.0f)
                                       .Max(1.0f)
                                       .DefaultValue(0.5f)
                                       .Format("Opacity: %.1f")
                                       .LabelPosition(UIWidgets::LabelPosition::None)
                                       .Step(0.1f)
                                       .Color(WIDGET_COLOR));
        static std::string trackerInputName;
        UIWidgets::InputString("##Group Name", &trackerInputName,
                               UIWidgets::InputOptions()
                                   .LabelPosition(UIWidgets::LabelPosition::None)
                                   .Color(WIDGET_COLOR)
                                   .PlaceholderText("Enter new group name")
                                   .Size({ ImGui::GetContentRegionAvail().x - 45.0f, 0 }));
        ImGui::SameLine();
        if (UIWidgets::Button(ICON_FA_PLUS, { .color = UIWidgets::Colors::Green })) {
            itemTrackerGroups.push_back(TrackerGroup{
                .name = trackerInputName.c_str(),
                .columns = 6,
                .scale = 1.0f,
            });
            SaveItemTrackerLayout();
            trackerInputName.clear();
        }

        ImGui::EndTable();
    }
}

void DrawTrackerAvailableGroups() {
    if (ImGui::BeginChild("Available Groups")) {
        int groupIndex = 0;
        for (auto& group : itemTrackerGroupsAvailable) {
            ImGui::PushID(groupIndex);
            ImGui::SeparatorText(group.name.c_str());
            ImGui::SameLine(ImGui::GetContentRegionMax().x - (ImGui::CalcTextSize("aaa").x * 1.5f));
            if (UIWidgets::Button(ICON_FA_PLUS, { .size = ImVec2(0, 0), .color = UIWidgets::Colors::Green })) {
                itemTrackerGroups.push_back(group);
                SaveItemTrackerLayout();
            }
            DrawItemTrackerGroupPreview(group, true, groupIndex);
            ImGui::PopID();
            groupIndex++;
        }
    }
    ImGui::EndChild();
}

void DrawTrackerActiveGroups() {
    if (ImGui::BeginChild("Active Groups")) {
        int groupIndex = 0;
        for (auto& group : itemTrackerGroups) {
            ImGui::PushID(groupIndex);
            ImGui::SeparatorText(group.name.c_str());

            ImGui::SameLine(ImGui::GetContentRegionMax().x - (ImGui::CalcTextSize("aaaaaaaaa").x * 1.5f));
            if (UIWidgets::Button(ICON_FA_CHEVRON_UP, { .size = ImVec2(0, 0), .color = WIDGET_COLOR })) {
                if (groupIndex > 0) {
                    std::swap(itemTrackerGroups[groupIndex], itemTrackerGroups[groupIndex - 1]);
                    SaveItemTrackerLayout();
                }
            }
            ImGui::SameLine();
            if (UIWidgets::Button(ICON_FA_CHEVRON_DOWN, { .size = ImVec2(0, 0), .color = WIDGET_COLOR })) {
                if (groupIndex < static_cast<int>(itemTrackerGroups.size()) - 1) {
                    std::swap(itemTrackerGroups[groupIndex], itemTrackerGroups[groupIndex + 1]);
                    SaveItemTrackerLayout();
                }
            }
            ImGui::SameLine();
            if (UIWidgets::Button(ICON_FA_ELLIPSIS_H, { .size = ImVec2(0, 0), .color = UIWidgets::Colors::Gray })) {
                ImGui::OpenPopup("GroupOptions");
            }
            ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Always);
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x - 400, ImGui::GetItemRectMin().y),
                                    ImGuiCond_Always);
            if (ImGui::BeginPopup("GroupOptions")) {
                if (DrawTrackerWindowOptions(groupIndex, group)) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            DrawItemTrackerGroupPreview(group, false, groupIndex);
            ImGui::PopID();
            groupIndex++;
        }
    }
    ImGui::EndChild();
}

void ItemTrackerSettingsWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(733, 472), ImGuiCond_FirstUseEver);
    if (ImGui::BeginChild("Item Tracker Settings")) {
        DrawTrackerOptions();

        UIWidgets::PushStyleTabs(WIDGET_COLOR);
        if (ImGui::BeginTable("TrackerTabs", 2)) {
            ImGui::TableNextColumn();
            if (ImGui::BeginChild("TrackerChild")) {
                if (ImGui::BeginTabBar("TrackerTabs")) {
                    if (ImGui::BeginTabItem("Available Groups")) {
                        DrawTrackerAvailableGroups();
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }
            ImGui::EndChild();

            ImGui::TableNextColumn();
            if (ImGui::BeginChild("WindowChild")) {
                if (ImGui::BeginTabBar("WindowTab")) {
                    if (ImGui::BeginTabItem("Active Groups")) {
                        DrawTrackerActiveGroups();
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }
            ImGui::EndChild();
            ImGui::EndTable();
        }
        UIWidgets::PopStyleTabs();
    }
    ImGui::EndChild();
}

void ItemTrackerSettingsWindow::InitElement() {
    LoadAvailableWindows();
    ApplyDefaultItemPreset();
    LoadItemTrackerConfig();
}
