#include "ItemTracker.h"
#include "ItemTrackerSettings.h"

#include "2s2h/BenGui/UIWidgets.hpp"
#include "Rando/Rando.h"
#include "Rando/ActorBehavior/Souls.h"
#include "Rando/MiscBehavior/ClockShuffle.h"
#include "2s2h/BenPort.h"
#include "2s2h/ShipUtils.h"
#include <spdlog/fmt/fmt.h>
#include <algorithm>

extern "C" {
#include "z64save.h"
#include "variables.h"
#include <macros.h>
#include <functions.h>
#include "2s2h_assets.h"
#include "assets/archives/icon_item_static/icon_item_static_yar.h"
#include "overlays/actors/ovl_En_Si/z_en_si.h"
}

#include <fast/Fast3dGui.h>

namespace BenGui {
extern std::shared_ptr<ItemTrackerWindow> mItemTrackerWindow;
}

#define FORMAT_COUNT "{}/{}"

#define CVAR_NAME_VISIBILITY_MODE "gSettings.ItemTracker.VisibilityMode"
#define CVAR_NAME_VISIBILITY_BTN "gSettings.ItemTracker.VisibilityBtn"
#define CVAR_VISIBILITY_MODE CVarGetInteger(CVAR_NAME_VISIBILITY_MODE, ITEM_TRACKER_VISIBILITY_MODE_ALWAYS)
#define CVAR_VISIBILITY_BTN CVarGetInteger(CVAR_NAME_VISIBILITY_BTN, BTN_CUSTOM_MODIFIER1)

std::vector<TrackerGroup> itemTrackerGroups;
static bool sItemTrackerBtnState = false;

static bool IsTradeItemObtained(RandoItemId randoItemId) {
    ItemId itemId = Rando::StaticData::Items[randoItemId].itemId;
    if (INV_CONTENT(itemId) == itemId) {
        return true;
    }

    switch (randoItemId) {
        case RI_MOONS_TEAR:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR);
        case RI_DEED_LAND:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND);
        case RI_DEED_SWAMP:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP);
        case RI_DEED_MOUNTAIN:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN);
        case RI_DEED_OCEAN:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN);
        case RI_ROOM_KEY:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY);
        case RI_LETTER_TO_MAMA:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA);
        case RI_LETTER_TO_KAFEI:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI);
        case RI_PENDANT_OF_MEMORIES:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_PENDANT_OF_MEMORIES);
        default:
            return false;
    }
}

// When the file isn't loaded everythin is ocarinas, so we fall back to the first safe item
u32 GetVanillaItemIdForSlot(u32 slot) {
    bool isSaveLoaded = gPlayState != NULL && gSaveContext.gameMode == GAMEMODE_NORMAL;
    u32 vanillaItemId = isSaveLoaded ? gSaveContext.save.saveInfo.inventory.items[slot] : ITEM_NONE;

    if (vanillaItemId == ITEM_NONE || vanillaItemId >= ITEM_RECOVERY_HEART) {
        vanillaItemId = safeItemsForInventorySlot[slot][0];
    }

    return vanillaItemId;
}

TrackerImageObject GetImageObject(TrackerItemType itemType, u32 itemId) {
    bool isSaveLoaded = gPlayState != NULL && gSaveContext.gameMode == GAMEMODE_NORMAL;
    bool itemObtained = false;
    TrackerImageObject trackerImageObject = {
        .textureColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        .textureDimensions = ImVec2(ITEM_TEXTURE_SIZE, ITEM_TEXTURE_SIZE),
    };

    switch (itemType) {
        case TRACKER_ITEM_RANDO: {
            RandoItemId randoItemId = (RandoItemId)itemId;
            switch (randoItemId) {
                case RI_GS_TOKEN_SWAMP: {
                    itemObtained = Inventory_GetSkullTokenCount(SCENE_KINSTA1) > 0;
                } break;
                case RI_GS_TOKEN_OCEAN: {
                    itemObtained = Inventory_GetSkullTokenCount(SCENE_KINDAN2) > 0;
                } break;
                case RI_WOODFALL_SMALL_KEY: {
                    itemObtained = DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE) > 0;
                } break;
                case RI_SNOWHEAD_SMALL_KEY: {
                    itemObtained = DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE) > 0;
                } break;
                case RI_GREAT_BAY_SMALL_KEY: {
                    itemObtained = DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE) > 0;
                } break;
                case RI_STONE_TOWER_SMALL_KEY: {
                    itemObtained = DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE) > 0;
                } break;
                case RI_WOODFALL_STRAY_FAIRY: {
                    itemObtained =
                        gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE] > 0;
                } break;
                case RI_SNOWHEAD_STRAY_FAIRY: {
                    itemObtained =
                        gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE] > 0;
                } break;
                case RI_GREAT_BAY_STRAY_FAIRY: {
                    itemObtained =
                        gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE] > 0;
                } break;
                case RI_STONE_TOWER_STRAY_FAIRY: {
                    itemObtained =
                        gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE] > 0;
                } break;
                case RI_TRIFORCE_PIECE: {
                    itemObtained = gSaveContext.save.shipSaveInfo.rando.foundTriforcePieces > 0;
                } break;
                case RI_SONG_SARIA: {
                    itemObtained = gSaveContext.save.shipSaveInfo.rando.sariaHintsAvailable > 0;
                } break;
                case RI_MOONS_TEAR:
                case RI_DEED_LAND:
                case RI_DEED_SWAMP:
                case RI_DEED_MOUNTAIN:
                case RI_DEED_OCEAN:
                case RI_ROOM_KEY:
                case RI_LETTER_TO_MAMA:
                case RI_LETTER_TO_KAFEI:
                case RI_PENDANT_OF_MEMORIES: {
                    itemObtained = IsTradeItemObtained(randoItemId);
                } break;
                default: {
                    itemObtained = !Rando::IsItemObtainable(randoItemId);
                } break;
            }

            trackerImageObject.textureColor = Ship_GetRandoItemColorTint(randoItemId);
            const char* texturePath = Rando::StaticData::GetIconTexturePath(randoItemId);
            if (texturePath != nullptr) {
                trackerImageObject.textureId =
                    std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                        ->GetTextureByName(texturePath);
            }
            if (randoItemId >= RI_OWL_CLOCK_TOWN_SOUTH && randoItemId <= RI_OWL_ZORA_CAPE) {
                trackerImageObject.textureDimensions.y = 24.0f;
            } else if (randoItemId >= RI_SONG_DOUBLE_TIME && randoItemId <= RI_SONG_TIME) {
                trackerImageObject.textureDimensions.x = 32.0f;
            }
        } break;
        case TRACKER_ITEM_SLOT: {
            itemObtained = gSaveContext.save.saveInfo.inventory.items[itemId] != ITEM_NONE;
            auto vanillaItemId = GetVanillaItemIdForSlot(itemId);

            trackerImageObject.textureId =
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName((const char*)gItemIcons[vanillaItemId]);
        } break;
        case TRACKER_ITEM_SWORD: {
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) > EQUIP_VALUE_SWORD_NONE) {
                itemObtained = true;
            }
            int vanillaItemId = ITEM_SWORD_KOKIRI;
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) > EQUIP_VALUE_SWORD_KOKIRI) {
                vanillaItemId = ITEM_SWORD_KOKIRI + GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) - EQUIP_VALUE_SWORD_KOKIRI;
            }

            trackerImageObject.textureId =
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName((const char*)gItemIcons[vanillaItemId]);
        } break;
        case TRACKER_ITEM_SHIELD: {
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) > EQUIP_VALUE_SHIELD_NONE) {
                itemObtained = true;
            }
            int vanillaItemId = ITEM_SHIELD_HERO;
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) > EQUIP_VALUE_SHIELD_HERO) {
                vanillaItemId = ITEM_SHIELD_MIRROR;
            }

            trackerImageObject.textureId =
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName((const char*)gItemIcons[vanillaItemId]);
        } break;
        case TRACKER_ITEM_WALLET: {
            if (CUR_UPG_VALUE(UPG_WALLET) >= 1) {
                itemObtained = true;
            }
            auto vanillaItemId = ITEM_WALLET_ADULT;
            if (CUR_UPG_VALUE(UPG_WALLET) >= 2) {
                vanillaItemId = ITEM_WALLET_GIANT;
            }

            trackerImageObject.textureId =
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName((const char*)gItemIcons[vanillaItemId]);
        } break;
        case TRACKER_ITEM_MAGIC: {
            if (gSaveContext.save.saveInfo.playerData.isMagicAcquired) {
                itemObtained = true;
            }
            auto vanillaItemId = ITEM_MAGIC_JAR_SMALL;
            if (gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired) {
                vanillaItemId = ITEM_MAGIC_JAR_BIG;
            }

            trackerImageObject.textureId =
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName((const char*)gItemIcons[vanillaItemId]);
        } break;
        default:
            break;
    }

    if (!isSaveLoaded) {
        itemObtained = false;
    }

    trackerImageObject.textureColor.w = itemObtained ? 1.0f : 0.4f;
    return trackerImageObject;
}

std::string GetItemCounts(TrackerItemType itemType, u32 itemId) {
    std::string countStr = "";

    switch (itemType) {
        case TRACKER_ITEM_RANDO: {
            switch (itemId) {
                case RI_TRIFORCE_PIECE: {
                    if (IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_TRIFORCE_PIECES]) {
                        auto max = RANDO_SAVE_OPTIONS[RO_TRIFORCE_PIECES_REQUIRED];
                        countStr = fmt::format(FORMAT_COUNT, gSaveContext.save.shipSaveInfo.rando.foundTriforcePieces,
                                               max > 999 ? "1k" : std::to_string(max));
                    }
                } break;
                case RI_SONG_SARIA: {
                    auto count = gSaveContext.save.shipSaveInfo.rando.sariaHintsAvailable;
                    if (count > 1) {
                        countStr = std::to_string(count);
                    }
                } break;
                case RI_GS_TOKEN_OCEAN:
                case RI_GS_TOKEN_SWAMP: {
                    auto max =
                        IS_RANDO ? RANDO_SAVE_OPTIONS[RO_SKULLTULA_TOKENS_REQUIRED] : SPIDER_HOUSE_TOKENS_REQUIRED;
                    auto count =
                        Inventory_GetSkullTokenCount(itemId == RI_GS_TOKEN_SWAMP ? SCENE_KINSTA1 : SCENE_KINDAN2);
                    countStr = fmt::format(FORMAT_COUNT, count, max);
                } break;
                case RI_WOODFALL_STRAY_FAIRY: {
                    auto max = IS_RANDO ? RANDO_SAVE_OPTIONS[RO_STRAY_FAIRIES_REQUIRED] : STRAY_FAIRY_SCATTERED_TOTAL;
                    auto count = gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE];
                    countStr = fmt::format(FORMAT_COUNT, count, max);
                } break;
                case RI_SNOWHEAD_STRAY_FAIRY: {
                    auto max = IS_RANDO ? RANDO_SAVE_OPTIONS[RO_STRAY_FAIRIES_REQUIRED] : STRAY_FAIRY_SCATTERED_TOTAL;
                    auto count = gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE];
                    countStr = fmt::format(FORMAT_COUNT, count, max);
                } break;
                case RI_GREAT_BAY_STRAY_FAIRY: {
                    auto max = IS_RANDO ? RANDO_SAVE_OPTIONS[RO_STRAY_FAIRIES_REQUIRED] : STRAY_FAIRY_SCATTERED_TOTAL;
                    auto count =
                        gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE];
                    countStr = fmt::format(FORMAT_COUNT, count, max);
                } break;
                case RI_STONE_TOWER_STRAY_FAIRY: {
                    auto max = IS_RANDO ? RANDO_SAVE_OPTIONS[RO_STRAY_FAIRIES_REQUIRED] : STRAY_FAIRY_SCATTERED_TOTAL;
                    auto count =
                        gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE];
                    countStr = fmt::format(FORMAT_COUNT, count, max);
                } break;
                case RI_WOODFALL_SMALL_KEY: {
                    auto count = DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE);
                    if (count > 0)
                        countStr = fmt::format(FORMAT_COUNT, count, 1);
                } break;
                case RI_SNOWHEAD_SMALL_KEY: {
                    auto count = DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE);
                    if (count > 0)
                        countStr = fmt::format(FORMAT_COUNT, count, 3);
                } break;
                case RI_GREAT_BAY_SMALL_KEY: {
                    auto count = DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE);
                    if (count > 0)
                        countStr = fmt::format(FORMAT_COUNT, count, 1);
                } break;
                case RI_STONE_TOWER_SMALL_KEY: {
                    auto count = DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE);
                    if (count > 0)
                        countStr = fmt::format(FORMAT_COUNT, count, 4);
                } break;
                default:
                    break;
            }
        } break;
        case TRACKER_ITEM_SLOT: {
            switch (itemId) {
                case SLOT_BOW:
                case SLOT_BOMB:
                case SLOT_BOMBCHU:
                case SLOT_DEKU_STICK:
                case SLOT_DEKU_NUT:
                case SLOT_MAGIC_BEANS:
                case SLOT_POWDER_KEG: {
                    auto count = gSaveContext.save.saveInfo.inventory.ammo[itemId];
                    if (count > 0)
                        countStr = std::to_string(count);
                } break;
                default:
                    break;
            }
        } break;
        case TRACKER_ITEM_WALLET: {
            countStr = std::to_string(CUR_CAPACITY(UPG_WALLET));
        } break;
        default:
            break;
    }

    return countStr;
}

// Choose a different font for the font size otherwise it gets scaled and blurry.
static ImFont* GetItemCountFont(float fontSize) {
    if (fontSize >= 22.0f) {
        return OTRGlobals::Instance->fontMonoLargest;
    }
    if (fontSize >= 18.0f) {
        return OTRGlobals::Instance->fontMonoLarger;
    }
    return OTRGlobals::Instance->fontMono;
}

void DrawItemCounts(TrackerItemType itemType, u32 itemId, ImVec2 cellMin, ImVec2 cellSize, float scale) {
    std::string itemCount = GetItemCounts(itemType, itemId);

    if (itemCount.empty()) {
        return;
    }

    float fontSize = std::max(cellSize.x * 0.44f, 12.0f) * ImGui::GetIO().FontGlobalScale;
    ImFont* font = GetItemCountFont(fontSize);
    ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, itemCount.c_str());

    float pad = 2.0f * scale;
    ImVec2 textPos(cellMin.x + cellSize.x - textSize.x - pad, cellMin.y + cellSize.y - textSize.y - pad);

    static const ImVec2 outlineDirections[] = { { -1, -1 }, { 0, -1 }, { 1, -1 }, { -1, 0 },
                                                { 1, 0 },   { -1, 1 }, { 0, 1 },  { 1, 1 } };

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float outline = std::max(1.0f, fontSize / 12.0f);
    for (const ImVec2& direction : outlineDirections) {
        drawList->AddText(font, fontSize, textPos + direction * outline, IM_COL32(0, 0, 0, 255), itemCount.c_str());
    }
    drawList->AddText(font, fontSize, textPos, IM_COL32(255, 255, 255, 255), itemCount.c_str());
}

bool DrawItemTrackerSlot(TrackerItemType itemType, u32 itemId, float scale, bool clickable) {
    ImVec2 cellSize(ITEM_TEXTURE_SIZE * scale, ITEM_TEXTURE_SIZE * scale);

    TrackerImageObject imageObject = GetImageObject(itemType, itemId);

    // Create an invisible button for layout + clicks
    bool clicked = false;
    if (clickable) {
        imageObject.textureColor.w = 1.0f; // force full opacity for clickable items
        clicked = ImGui::InvisibleButton(std::to_string(itemId).c_str(), cellSize);
    } else {
        ImGui::Dummy(cellSize);
    }

    // Draw image centered inside the cell
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();

    if (clickable) {
        bool hovered = ImGui::IsItemHovered();
        bool held = ImGui::IsItemActive();

        ImU32 bg;
        if (held) {
            bg = IM_COL32(0, 0, 0, 60); // pressed
        } else if (hovered) {
            bg = IM_COL32(255, 255, 255, 20); // hover
        } else {
            bg = IM_COL32(0, 0, 0, 0); // idle subtle
        }

        ImGui::GetWindowDrawList()->AddRectFilled(p0, p1, bg, 4.0f); // small rounding
    }

    float aspect = imageObject.textureDimensions.x / imageObject.textureDimensions.y;

    // fit to height
    ImVec2 drawSize(cellSize.y * aspect, cellSize.y);

    // clamp if too wide
    if (drawSize.x > cellSize.x) {
        drawSize.x = cellSize.x;
        drawSize.y = cellSize.x / aspect;
    }

    ImVec2 offset((cellSize.x - drawSize.x) * 0.5f, (cellSize.y - drawSize.y) * 0.5f);

    // Draw a glow behind skulltula tokens
    if (itemType == TRACKER_ITEM_RANDO && (itemId == RI_GS_TOKEN_SWAMP || itemId == RI_GS_TOKEN_OCEAN)) {
        ImVec4 tintColor = ImVec4(25.0f / 255.0f, 251.0f / 255.0f, 0.0f, imageObject.textureColor.w); // Swamp tint
        if (itemId == RI_GS_TOKEN_OCEAN) {
            tintColor = ImVec4(0.0f, 209.0f / 256.0f, 231.0f / 256.0f, imageObject.textureColor.w); // Ocean tint
        }
        auto textureId =
            std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                ->GetTextureByName(gMagicArrowEquipEffectTex);

        // Draw texture behind the actual item icon
        ImGui::GetWindowDrawList()->AddImage(textureId, p0 + offset - ImVec2(8.0f, 8.0f),
                                             p0 + offset + drawSize + ImVec2(8.0f, 8.0f), ImVec2(0, 0), ImVec2(1, 1),
                                             ImGui::GetColorU32(tintColor));
    }

    if (itemType == TRACKER_ITEM_RANDO && itemId >= RI_SOUL_BOSS_GOHT && itemId <= RI_SOUL_BOSS_TWINMOLD) {
        ImVec4 tintColor =
            ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, imageObject.textureColor.w); // Swamp tint
        auto textureId =
            std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                ->GetTextureByName(gMagicArrowEquipEffectTex);

        // Draw texture behind the actual item icon
        ImGui::GetWindowDrawList()->AddImage(textureId, p0 + offset - ImVec2(8.0f, 8.0f),
                                             p0 + offset + drawSize + ImVec2(8.0f, 8.0f), ImVec2(0, 0), ImVec2(1, 1),
                                             ImGui::GetColorU32(tintColor));
    }

    if (imageObject.textureId != nullptr) {
        ImGui::GetWindowDrawList()->AddImage(imageObject.textureId, p0 + offset, p0 + offset + drawSize, ImVec2(0, 0),
                                             ImVec2(1, 1), ImGui::GetColorU32(imageObject.textureColor));
    }
    auto itemName = GetItemTrackerItemName(itemType, itemId);
    if (!itemName.empty()) {
        UIWidgets::Tooltip(itemName.c_str());
    }

    if (CVarGetInteger("gSettings.ItemTracker.ItemCounts", 1)) {
        DrawItemCounts(itemType, itemId, p0, cellSize, scale);
    }

    return clicked;
}

void DrawItemTrackerGroup(TrackerGroup& trackerGroup) {
    int columns = trackerGroup.columns;
    if (trackerGroup.items.size() < trackerGroup.columns) {
        columns = trackerGroup.items.size();
    }

    float scale = CVarGetInteger("gSettings.ItemTracker.WindowGroup", 0)
                      ? trackerGroup.scale
                      : CVarGetFloat("gSettings.ItemTracker.Scale", 1.0f);

    if (ImGui::BeginTable(trackerGroup.name.c_str(), columns)) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        for (auto& [itemType, itemId] : trackerGroup.items) {
            ImGui::TableNextColumn();
            DrawItemTrackerSlot(itemType, itemId, scale, false);
        }

        ImGui::PopStyleVar(2);
        ImGui::EndTable();
    }
}

void ItemTrackerWindow::Draw() {
    if (!IsVisible()) {
        return;
    }

    if (CVAR_VISIBILITY_MODE == ITEM_TRACKER_VISIBILITY_MODE_ONLY_ON_PAUSE_MENU &&
        (!gPlayState || !gPlayState->pauseCtx.state)) {
        return;
    }

    if ((CVAR_VISIBILITY_MODE == ITEM_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE ||
         CVAR_VISIBILITY_MODE == ITEM_TRACKER_VISIBILITY_MODE_BUTTON_HOLD) &&
        !sItemTrackerBtnState) {
        return;
    }

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav |
                                   ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

    if (!CVarGetInteger("gSettings.ItemTracker.WindowType", 0)) {
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking;
    }
    bool shouldWindowSplit = CVarGetInteger("gSettings.ItemTracker.WindowGroup", 0);

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    // Adjust opacity
    ImVec4 windowBg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
    windowBg.w = CVarGetFloat("gSettings.ItemTracker.Opacity", 0.5f);

    bool singleWindowOpen = false;
    if (!shouldWindowSplit) {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, windowBg);
        singleWindowOpen = ImGui::Begin("Item Tracker", nullptr, windowFlags);
    }

    uint32_t index = 0;
    for (auto& group : itemTrackerGroups) {
        if (group.items.empty()) {
            index++;
            continue;
        }

        bool isWindowOpen = false;

        if (shouldWindowSplit) {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, windowBg);
            std::string name = std::string(group.name) + "##" + std::to_string(index);
            isWindowOpen = ImGui::Begin(name.c_str(), nullptr, windowFlags);
        } else {
            isWindowOpen = singleWindowOpen;
        }

        if (isWindowOpen) {
            DrawItemTrackerGroup(group);
        }

        if (shouldWindowSplit) {
            ImGui::PopStyleColor(1);
            ImGui::End();
        }

        index++;
    }
    if (!shouldWindowSplit) {
        ImGui::PopStyleColor(1);
        ImGui::End();
    }

    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(1);
}

void ItemTrackerWindow::InitElement() {
}

void ItemTrackerWindow::DrawElement() {
}

static RegisterShipInitFunc initFunc(
    []() {
        COND_HOOK(OnGameStateMainStart, CVAR_VISIBILITY_MODE >= ITEM_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE, []() {
            Input* input = CONTROLLER1(gGameState);

            if (CVAR_VISIBILITY_MODE == ITEM_TRACKER_VISIBILITY_MODE_BUTTON_HOLD) {
                if (CHECK_BTN_ALL(input->cur.button, CVAR_VISIBILITY_BTN)) {
                    sItemTrackerBtnState = true;
                } else {
                    sItemTrackerBtnState = false;
                }
            } else {
                if (CHECK_BTN_ALL(input->cur.button, CVAR_VISIBILITY_BTN) &&
                    CHECK_BTN_ANY(input->press.button, CVAR_VISIBILITY_BTN)) {
                    sItemTrackerBtnState = !sItemTrackerBtnState;
                }
            }
        });
    },
    { CVAR_NAME_VISIBILITY_MODE });
