#include "SohMenu.h"
#include "soh/SohGui/SohGui.hpp"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/ImGuiUtils.h"
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/settings.h"

#include <vector>
#include <fast/Fast3dGui.h>
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "z64.h"
}

namespace SohGui {

using namespace UIWidgets;

static constexpr float kIconSize = 48.0f;
static const ImVec2 kSongSize = ImVec2(32.0f, 48.0f);

static std::shared_ptr<Fast::Fast3dGui> GetFast3dGui() {
    return std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
}

static void SaveStartingItemCVars() {
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

// Toggleable item icon (lit when the starting CVar is set, faded when not), mirroring
// DrawQuestItemButton in the save editor. The CVar name and tooltip come from the Option itself.
static void StartingItemToggle(RandomizerSettingKey rsk, uint32_t itemId, bool forceFaded = false) {
    Rando::Option& option = Rando::Settings::GetInstance()->GetOption(rsk);
    const char* cvar = option.GetCVarName().c_str();
    const ItemMapEntry& entry = itemMapping[itemId];
    bool on = !forceFaded && CVarGetInteger(cvar, 0) != 0;

    ImGui::PushID(static_cast<int32_t>(rsk));
    PushStyleButton(Colors::DarkGray);
    if (ImGui::ImageButton(entry.name.c_str(), GetFast3dGui()->GetTextureByName(on ? entry.name : entry.nameFaded),
                           ImVec2(kIconSize, kIconSize), ImVec2(0, 0), ImVec2(1, 1))) {
        CVarSetInteger(cvar, on ? 0 : 1);
        SaveStartingItemCVars();
    }
    PopStyleButton();
    Tooltip(option.GetName().c_str());
    ImGui::PopID();
}

// Tiered item icon with a popup picker (None + one icon per tier), mirroring DrawUpgradeIcon.
// `itemIds[i]` is the icon shown for CVar value `i + 1`; value 0 shows itemIds[0] faded.
static void StartingItemTiered(RandomizerSettingKey rsk, const std::vector<uint32_t>& itemIds) {
    Rando::Option& option = Rando::Settings::GetInstance()->GetOption(rsk);
    const char* cvar = option.GetCVarName().c_str();
    int32_t value = CVarGetInteger(cvar, 0);

    ImGui::PushID(static_cast<int32_t>(rsk));
    const ItemMapEntry& displayEntry = itemMapping[itemIds[value > 0 ? value - 1 : 0]];
    bool lit = value > 0;
    PushStyleButton(Colors::DarkGray);
    if (ImGui::ImageButton(displayEntry.name.c_str(),
                           GetFast3dGui()->GetTextureByName(lit ? displayEntry.name : displayEntry.nameFaded),
                           ImVec2(kIconSize, kIconSize), ImVec2(0, 0), ImVec2(1, 1))) {
        ImGui::OpenPopup("tieredPicker");
    }
    PopStyleButton();
    Tooltip(option.GetName().c_str());

    if (ImGui::BeginPopup("tieredPicker")) {
        PushStyleButton(Colors::DarkGray);
        if (ImGui::Button("##none", ImVec2(kIconSize, kIconSize) + ImGui::GetStyle().FramePadding * 2)) {
            CVarSetInteger(cvar, 0);
            SaveStartingItemCVars();
            ImGui::CloseCurrentPopup();
        }
        PopStyleButton();
        Tooltip("None");
        for (size_t i = 0; i < itemIds.size(); i++) {
            ImGui::SameLine();
            const ItemMapEntry& entry = itemMapping[itemIds[i]];
            PushStyleButton(Colors::DarkGray);
            if (ImGui::ImageButton(entry.name.c_str(), GetFast3dGui()->GetTextureByName(entry.name),
                                   ImVec2(kIconSize, kIconSize), ImVec2(0, 0), ImVec2(1, 1))) {
                CVarSetInteger(cvar, static_cast<int32_t>(i) + 1);
                SaveStartingItemCVars();
                ImGui::CloseCurrentPopup();
            }
            PopStyleButton();
            Tooltip(option.GetOptionText(i + 1).c_str());
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

// Toggleable 32x48 song icon, mirroring the songMapping loop in DrawQuestStatusTab.
static void StartingSongToggle(RandomizerSettingKey rsk, QuestItem song) {
    Rando::Option& option = Rando::Settings::GetInstance()->GetOption(rsk);
    const char* cvar = option.GetCVarName().c_str();
    const SongMapEntry& entry = songMapping[song];
    bool on = CVarGetInteger(cvar, 0) != 0;

    ImGui::PushID(static_cast<int32_t>(rsk));
    PushStyleButton(Colors::DarkGray);
    if (ImGui::ImageButton(entry.name.c_str(), GetFast3dGui()->GetTextureByName(on ? entry.name : entry.nameFaded),
                           kSongSize, ImVec2(0, 0), ImVec2(1, 1))) {
        CVarSetInteger(cvar, on ? 0 : 1);
        SaveStartingItemCVars();
    }
    PopStyleButton();
    Tooltip(option.GetName().c_str());
    ImGui::PopID();
}

// Item icon followed by a count slider, like the ammo rows in DrawInventoryTab. The slider
// mirrors the Option's own semantics: the CVar stores the option index and the displayed
// number comes from the option text (matching how WIDGET_CVAR_SLIDER_INT renders these).
static void StartingItemCount(RandomizerSettingKey rsk, uint32_t itemId) {
    Rando::Option& option = Rando::Settings::GetInstance()->GetOption(rsk);
    const char* cvar = option.GetCVarName().c_str();
    const ItemMapEntry& entry = itemMapping[itemId];

    ImGui::PushID(static_cast<int32_t>(rsk));
    ImGui::Image(GetFast3dGui()->GetTextureByName(entry.name), ImVec2(kIconSize, kIconSize));
    ImGui::SameLine();
    int32_t value = CVarGetInteger(cvar, option.GetMenuOptionDefault());
    CVarSliderInt(option.GetName().c_str(), cvar,
                  IntSliderOptions()
                      .Color(THEME_COLOR)
                      .Min(0)
                      .Max(static_cast<int32_t>(option.GetOptionCount()) - 1)
                      .DefaultValue(option.GetMenuOptionDefault())
                      .Format(option.GetOptionText(value).c_str())
                      .LabelPosition(LabelPositions::Near));
    ImGui::PopID();
}

// Plain combobox bound to an Option's CVar, with option text pulled from the Option.
static void StartingItemCombobox(RandomizerSettingKey rsk) {
    Rando::Option& option = Rando::Settings::GetInstance()->GetOption(rsk);
    std::vector<const char*> entries;
    for (size_t i = 0; i < option.GetOptionCount(); i++) {
        entries.push_back(option.GetOptionText(i).c_str());
    }
    CVarCombobox(option.GetName().c_str(), option.GetCVarName().c_str(), entries,
                 ComboboxOptions().Color(THEME_COLOR).LabelPosition(LabelPositions::Above));
}

void DrawStartingItemsMenu(WidgetInfo& info) {
    bool generating = CVarGetInteger(CVAR_GENERAL("RandoGenerating"), 0);
    bool disableEditingRandoSettings = generating || CVarGetInteger(CVAR_GENERAL("OnFileSelectNameEntry"), 0);
    ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0) || disableEditingRandoSettings);

    ImGui::SeparatorText("Equipment");
    StartingItemToggle(RSK_STARTING_KOKIRI_SWORD, ITEM_SWORD_KOKIRI);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_MASTER_SWORD, ITEM_SWORD_MASTER);
    ImGui::SameLine();
    StartingItemTiered(RSK_STARTING_BIGGORON_SWORD, { ITEM_SWORD_KNIFE, ITEM_SWORD_BGS });
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_DEKU_SHIELD, ITEM_SHIELD_DEKU);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_HYLIAN_SHIELD, ITEM_SHIELD_HYLIAN);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_MIRROR_SHIELD, ITEM_SHIELD_MIRROR);

    StartingItemToggle(RSK_STARTING_GORON_TUNIC, ITEM_TUNIC_GORON);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_ZORA_TUNIC, ITEM_TUNIC_ZORA);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_IRON_BOOTS, ITEM_BOOTS_IRON);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_HOVER_BOOTS, ITEM_BOOTS_HOVER);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_STONE_OF_AGONY, ITEM_STONE_OF_AGONY);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_GERUDO_CARD, ITEM_GERUDO_CARD);

    // Starting Strength/Scale have no effect when Grab/Swim are shuffled; the generator
    // force-disables them (settings.cpp), so gray them out to match.
    bool grabShuffled =
        CVarGetInteger(Rando::Settings::GetInstance()->GetOption(RSK_SHUFFLE_GRAB).GetCVarName().c_str(), 0) != 0;
    ImGui::BeginDisabled(grabShuffled);
    StartingItemTiered(RSK_STARTING_STRENGTH, { ITEM_BRACELET, ITEM_GAUNTLETS_SILVER, ITEM_GAUNTLETS_GOLD });
    ImGui::EndDisabled();
    if (grabShuffled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Disabled because Shuffle Grab is on.");
    }
    ImGui::SameLine();
    bool swimShuffled =
        CVarGetInteger(Rando::Settings::GetInstance()->GetOption(RSK_SHUFFLE_SWIM).GetCVarName().c_str(), 0) != 0;
    ImGui::BeginDisabled(swimShuffled);
    StartingItemTiered(RSK_STARTING_SCALE, { ITEM_SCALE_SILVER, ITEM_SCALE_GOLDEN });
    ImGui::EndDisabled();
    if (swimShuffled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Disabled because Shuffle Swim is on.");
    }
    ImGui::SameLine();
    StartingItemTiered(RSK_STARTING_MAGIC_METER, { ITEM_MAGIC_SMALL, ITEM_MAGIC_LARGE });
    ImGui::SameLine();
    StartingItemTiered(RSK_STARTING_WALLET, { ITEM_WALLET_ADULT, ITEM_WALLET_GIANT });

    ImGui::SeparatorText("Items");
    bool stickBagShuffled =
        CVarGetInteger(Rando::Settings::GetInstance()->GetOption(RSK_SHUFFLE_DEKU_STICK_BAG).GetCVarName().c_str(),
                       0) != 0;
    ImGui::BeginDisabled(stickBagShuffled);
    StartingItemToggle(RSK_STARTING_STICKS, ITEM_STICK, stickBagShuffled);
    ImGui::EndDisabled();
    if (stickBagShuffled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Disabled because the Deku Stick Bag is being shuffled.");
    }
    ImGui::SameLine();
    bool nutBagShuffled =
        CVarGetInteger(Rando::Settings::GetInstance()->GetOption(RSK_SHUFFLE_DEKU_NUT_BAG).GetCVarName().c_str(), 0) !=
        0;
    ImGui::BeginDisabled(nutBagShuffled);
    StartingItemToggle(RSK_STARTING_NUTS, ITEM_NUT, nutBagShuffled);
    ImGui::EndDisabled();
    if (nutBagShuffled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Disabled because the Deku Nut Bag is being shuffled.");
    }
    ImGui::SameLine();
    StartingItemTiered(RSK_STARTING_BOMB_BAG, { ITEM_BOMB_BAG_20, ITEM_BOMB_BAG_30, ITEM_BOMB_BAG_40 });
    ImGui::SameLine();
    StartingItemTiered(RSK_STARTING_BOW, { ITEM_QUIVER_30, ITEM_QUIVER_40, ITEM_QUIVER_50 });
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_FIRE_ARROWS, ITEM_ARROW_FIRE);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_DINS_FIRE, ITEM_DINS_FIRE);

    StartingItemTiered(RSK_STARTING_SLINGSHOT, { ITEM_BULLET_BAG_30, ITEM_BULLET_BAG_40, ITEM_BULLET_BAG_50 });
    ImGui::SameLine();
    StartingItemTiered(RSK_STARTING_OCARINA, { ITEM_OCARINA_FAIRY, ITEM_OCARINA_TIME });
    ImGui::SameLine();
    // The bombchu bag start option only makes sense when the bombchu bag setting enables bombchu bags;
    // single-bag mode is a simple toggle, progressive mode is tiered, and None disables it entirely.
    {
        Rando::Option& bombchuOption = Rando::Settings::GetInstance()->GetOption(RSK_BOMBCHU_BAG);
        int32_t bombchuMode = CVarGetInteger(bombchuOption.GetCVarName().c_str(), RO_BOMBCHU_BAG_NONE);
        if (bombchuMode == RO_BOMBCHU_BAG_PROGRESSIVE) {
            StartingItemTiered(RSK_STARTING_BOMBCHU_BAG, { ITEM_BOMBCHU, ITEM_BOMBCHU, ITEM_BOMBCHU });
        } else if (bombchuMode == RO_BOMBCHU_BAG_SINGLE) {
            StartingItemToggle(RSK_STARTING_BOMBCHU_BAG, ITEM_BOMBCHU);
        } else {
            ImGui::BeginDisabled();
            StartingItemToggle(RSK_STARTING_BOMBCHU_BAG, ITEM_BOMBCHU, true);
            ImGui::EndDisabled();
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Enable the Bombchu Bag setting to start with bombchus.");
            }
        }
    }
    ImGui::SameLine();
    StartingItemTiered(RSK_STARTING_HOOKSHOT, { ITEM_HOOKSHOT, ITEM_LONGSHOT });
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_ICE_ARROWS, ITEM_ARROW_ICE);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_FARORES_WIND, ITEM_FARORES_WIND);

    StartingItemToggle(RSK_STARTING_BOOMERANG, ITEM_BOOMERANG);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_LENS_OF_TRUTH, ITEM_LENS);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_BEANS, ITEM_BEAN);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_MEGATON_HAMMER, ITEM_HAMMER);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_LIGHT_ARROWS, ITEM_ARROW_LIGHT);
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_NAYRUS_LOVE, ITEM_NAYRUS_LOVE);

    StartingItemTiered(RSK_STARTING_BOTTLE_1, { ITEM_BOTTLE, ITEM_BIG_POE, ITEM_LETTER_RUTO });
    ImGui::SameLine();
    StartingItemTiered(RSK_STARTING_BOTTLE_2, { ITEM_BOTTLE, ITEM_BIG_POE });
    ImGui::SameLine();
    StartingItemTiered(RSK_STARTING_BOTTLE_3, { ITEM_BOTTLE, ITEM_BIG_POE });
    ImGui::SameLine();
    StartingItemTiered(RSK_STARTING_BOTTLE_4, { ITEM_BOTTLE, ITEM_BIG_POE });
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_CLAIM_CHECK, ITEM_CLAIM_CHECK);
    ImGui::SameLine();
    bool weirdEggShuffled =
        CVarGetInteger(Rando::Settings::GetInstance()->GetOption(RSK_SHUFFLE_WEIRD_EGG).GetCVarName().c_str(), 0) != 0;
    ImGui::BeginDisabled(!weirdEggShuffled);
    StartingItemToggle(RSK_STARTING_WEIRD_EGG, ITEM_WEIRD_EGG, !weirdEggShuffled);
    ImGui::EndDisabled();
    if (!weirdEggShuffled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Enable Shuffle Weird Egg to start with the Weird Egg.");
    }
    ImGui::SameLine();
    StartingItemToggle(RSK_STARTING_BUNNY_HOOD, ITEM_MASK_BUNNY);

    ImGui::SeparatorText("Songs");
    StartingSongToggle(RSK_STARTING_ZELDAS_LULLABY, QUEST_SONG_LULLABY);
    ImGui::SameLine();
    StartingSongToggle(RSK_STARTING_EPONAS_SONG, QUEST_SONG_EPONA);
    ImGui::SameLine();
    StartingSongToggle(RSK_STARTING_SARIAS_SONG, QUEST_SONG_SARIA);
    ImGui::SameLine();
    StartingSongToggle(RSK_STARTING_SUNS_SONG, QUEST_SONG_SUN);
    ImGui::SameLine();
    StartingSongToggle(RSK_STARTING_SONG_OF_TIME, QUEST_SONG_TIME);
    ImGui::SameLine();
    StartingSongToggle(RSK_STARTING_SONG_OF_STORMS, QUEST_SONG_STORMS);

    StartingSongToggle(RSK_STARTING_MINUET_OF_FOREST, QUEST_SONG_MINUET);
    ImGui::SameLine();
    StartingSongToggle(RSK_STARTING_BOLERO_OF_FIRE, QUEST_SONG_BOLERO);
    ImGui::SameLine();
    StartingSongToggle(RSK_STARTING_SERENADE_OF_WATER, QUEST_SONG_SERENADE);
    ImGui::SameLine();
    StartingSongToggle(RSK_STARTING_REQUIEM_OF_SPIRIT, QUEST_SONG_REQUIEM);
    ImGui::SameLine();
    StartingSongToggle(RSK_STARTING_NOCTURNE_OF_SHADOW, QUEST_SONG_NOCTURNE);
    ImGui::SameLine();
    StartingSongToggle(RSK_STARTING_PRELUDE_OF_LIGHT, QUEST_SONG_PRELUDE);

    ImGui::SeparatorText("Other");
    StartingItemCombobox(RSK_LINKS_POCKET);
    // The reward type only applies when Link's Pocket grants a dungeon reward.
    if (CVarGetInteger(Rando::Settings::GetInstance()->GetOption(RSK_LINKS_POCKET).GetCVarName().c_str(),
                       RO_LINKS_POCKET_DUNGEON_REWARD) == RO_LINKS_POCKET_DUNGEON_REWARD) {
        StartingItemCombobox(RSK_LINKS_POCKET_REWARD);
    }
    StartingItemCount(RSK_STARTING_SKULLTULA_TOKEN, ITEM_SKULL_TOKEN);
    StartingItemCount(RSK_STARTING_HEARTS, ITEM_HEART_CONTAINER);

    ImGui::EndDisabled();
}

} // namespace SohGui
