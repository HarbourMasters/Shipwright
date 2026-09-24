#include <libultraship/libultraship.h>
#include <libultraship/controller/controldeck/ControlDeck.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/SohGui/MenuTypes.h"
#include "soh/SohGui/SohMenu.h"
#include "soh/SohGui/SohGui.hpp"
#include "soh/SohGui/UIWidgets.hpp"

extern "C" {
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "macros.h"
#include "libultraship/libultra/controller.h"
extern PlayState* gPlayState;
void LinkAnimation_PlayLoopSetSpeed(PlayState* play, SkelAnime* skelAnime, LinkAnimationHeader* animation,
                                    f32 playSpeed);
void LinkAnimation_PlayOnceSetSpeed(PlayState* play, SkelAnime* skelAnime, LinkAnimationHeader* animation,
                                    f32 playSpeed);
}

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
} // namespace SohGui

struct EmoteSlot {
    std::string animationPath;
    bool playOnce;
    float speed;
    std::string displayName;
};

static EmoteSlot emoteSlots[4] = {
    { "", false, 1.0f, "C-Up" },
    { "", false, 1.0f, "C-Right" },
    { "", false, 1.0f, "C-Down" },
    { "", false, 1.0f, "C-Left" },
};

static std::vector<std::string> availableAnimations;

static char animationSearchString[64] = "";
static int16_t animationSearchDebounceFrames = -1;
static bool doAnimationSearch = false;
static bool wheelActive = false;
static EmoteSlot targetSlot;
static std::string activeAnimation = "";

// CVars for persistence
#define CVAR_EMOTE(slot, field) ("gEmoteWheel.Slot" + std::to_string(slot) + "." + field).c_str()

void LoadEmoteConfiguration() {
    for (int i = 0; i < 4; i++) {
        std::string animPath = CVarGetString(CVAR_EMOTE(i, "Animation"), emoteSlots[i].animationPath.c_str());
        if (!animPath.empty()) {
            emoteSlots[i].animationPath = animPath;
        }
        emoteSlots[i].playOnce = CVarGetInteger(CVAR_EMOTE(i, "PlayOnce"), emoteSlots[i].playOnce ? 1 : 0);
        emoteSlots[i].speed = CVarGetFloat(CVAR_EMOTE(i, "Speed"), emoteSlots[i].speed);
    }
}

void SaveEmoteConfiguration() {
    for (int i = 0; i < 4; i++) {
        CVarSetString(CVAR_EMOTE(i, "Animation"), emoteSlots[i].animationPath.c_str());
        CVarSetInteger(CVAR_EMOTE(i, "PlayOnce"), emoteSlots[i].playOnce ? 1 : 0);
        CVarSetFloat(CVAR_EMOTE(i, "Speed"), emoteSlots[i].speed);
    }
    CVarSave();
}

void LoadAvailableAnimations() {
    availableAnimations.clear();

    // Get all available animations
    auto result = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->ListFiles(
        "*gPlayerAnim_link*" + std::string(animationSearchString) + "*");

    availableAnimations.clear();
    for (const auto& path : *result.get()) {
        availableAnimations.push_back("__OTR__" + path);
    }
}

std::string GetAnimationDisplayName(const std::string& path) {
    // Extract a friendly name from the path
    size_t lastSlash = path.find_last_of('/');
    std::string name = (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;

    // Remove common prefixes
    if (name.find("gPlayerAnim_link") == 0) {
        name = name.substr(17);
    }

    return name;
}

int GetSelectedSlotFromCButtons(uint16_t buttons) {
    if (buttons & BTN_CUP) {
        return 0;
    } else if (buttons & BTN_CRIGHT) {
        return 1;
    } else if (buttons & BTN_CDOWN) {
        return 2;
    } else if (buttons & BTN_CLEFT) {
        return 3;
    }
    return -1;
}

void RegisterEmote() {
    LoadAvailableAnimations();
    LoadEmoteConfiguration();

    COND_HOOK(OnPlayerUpdate, CVarGetInteger("gEmoteWheel.Enabled", 0), []() {
        Player* player = GET_PLAYER(gPlayState);

        if (player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM || player->stateFlags1 & PLAYER_STATE1_CLIMBING_LADDER ||
            player->stateFlags1 & PLAYER_STATE1_IN_WATER || player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS) {
            return;
        }

        if (targetSlot.animationPath != "") {
            activeAnimation = targetSlot.animationPath;
            if (targetSlot.playOnce) {
                LinkAnimation_PlayOnceSetSpeed(gPlayState, &player->skelAnime,
                                               (LinkAnimationHeader*)activeAnimation.c_str(), targetSlot.speed);
            } else {
                LinkAnimation_PlayLoopSetSpeed(gPlayState, &player->skelAnime,
                                               (LinkAnimationHeader*)activeAnimation.c_str(), targetSlot.speed);
            }
            targetSlot.animationPath = "";
        }
    });

    COND_HOOK(OnGameStateMainStart, CVarGetInteger("gEmoteWheel.Enabled", 0), []() {
        if (!gPlayState)
            return;

        Player* player = GET_PLAYER(gPlayState);
        if (!player)
            return;

        Input* input = &gPlayState->state.input[0];

        if (input->press.button & BTN_L) {
            input->press.button &= ~BTN_L; // Capture
            wheelActive = true;
        }

        if (!(input->cur.button & BTN_L) && wheelActive) {
            wheelActive = false;
        }

        if (wheelActive) {
            auto slot = GetSelectedSlotFromCButtons(input->cur.button);
            if (slot >= 0 && slot < 4) {
                targetSlot = emoteSlots[slot];
            }
            input->press.button &= ~BTN_CUP & ~BTN_CRIGHT & ~BTN_CDOWN & ~BTN_CLEFT; // Capture
        }
    });
}

void DrawEmoteConfiguration(WidgetInfo& info) {
    ImGui::Text("Hold L button and press a C-button to select an emote");
    UIWidgets::CVarCheckbox("Enabled", "gEmoteWheel.Enabled");

    if (!CVarGetInteger("gEmoteWheel.Enabled", 0)) {
        return;
    }

    UIWidgets::PushStyleInput(THEME_COLOR);

    if (ImGui::InputText("Search Animations", animationSearchString, ARRAY_COUNT(animationSearchString))) {
        doAnimationSearch = true;
        animationSearchDebounceFrames = 30;
    }
    UIWidgets::PopStyleInput();

    if (doAnimationSearch) {
        if (animationSearchDebounceFrames == 0) {
            doAnimationSearch = false;
            LoadAvailableAnimations();
        }

        animationSearchDebounceFrames--;
    }

    ImGui::Separator();
    ImGui::Spacing();

    const char* slotNames[] = { "C-Up", "C-Right", "C-Down", "C-Left" };

    for (int i = 0; i < 4; i++) {
        ImGui::PushID(i);

        ImGui::Text("%s Slot", slotNames[i]);

        std::string currentAnimName = GetAnimationDisplayName(emoteSlots[i].animationPath);
        UIWidgets::PushStyleCombobox();
        if (ImGui::BeginCombo("Animation", currentAnimName.c_str())) {
            for (const auto& animPath : availableAnimations) {
                bool isSelected = (emoteSlots[i].animationPath == animPath);
                std::string displayName = GetAnimationDisplayName(animPath);
                if (ImGui::Selectable(displayName.c_str(), isSelected)) {
                    emoteSlots[i].animationPath = animPath;
                    SaveEmoteConfiguration();
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        UIWidgets::PopStyleCombobox();

        // Play once checkbox
        bool playOnce = emoteSlots[i].playOnce;
        UIWidgets::PushStyleCheckbox();
        if (ImGui::Checkbox("Play Once", &playOnce)) {
            emoteSlots[i].playOnce = playOnce;
            SaveEmoteConfiguration();
        }
        UIWidgets::PopStyleCheckbox();

        // Speed slider
        float speed = emoteSlots[i].speed;
        ImGui::SetNextItemWidth(200.0f);
        UIWidgets::PushStyleSlider();
        if (ImGui::SliderFloat("Speed", &speed, 0.1f, 3.0f, "%.2f")) {
            emoteSlots[i].speed = speed;
            SaveEmoteConfiguration();
        }
        UIWidgets::PopStyleSlider();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PopID();
    }

    if (UIWidgets::Button("Reset to Defaults", { .size = UIWidgets::Sizes::Inline })) {
        emoteSlots[0] = { "", false, 1.0f, "Up" };
        emoteSlots[1] = { "", false, 1.0f, "Right" };
        emoteSlots[2] = { "", false, 1.0f, "Down" };
        emoteSlots[3] = { "", false, 1.0f, "Left" };
        SaveEmoteConfiguration();
    }
}

void RegisterEmoteWidgets() {
    SohGui::mSohMenu->AddSidebarEntry("Settings", "Emotes", 1);
    WidgetPath path = { "Settings", "Emotes", SECTION_COLUMN_1 };
    SohGui::mSohMenu->AddWidget(path, "Emote Wheel Config", WIDGET_CUSTOM)
        .CustomFunction(DrawEmoteConfiguration)
        .HideInSearch(true);
}

static RegisterMenuInitFunc menuInitFunc(RegisterEmoteWidgets);
static RegisterShipInitFunc initFunc(RegisterEmote, { "gEmoteWheel.Enabled" });
