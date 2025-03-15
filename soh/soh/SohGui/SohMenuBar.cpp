#include "SohMenuBar.h"
#include <imgui.h>
#include "regex"
#include "public/bridge/consolevariablebridge.h"
#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"
#include "include/z64audio.h"
#include "graphic/Fast3D/gfx_rendering_api.h"
#include "soh/OTRGlobals.h"
#include "soh/SaveManager.h"
#include "z64.h"
#include "soh/cvar_prefixes.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/presets.h"
#include "soh/Enhancements/mods.h"
#include "soh/Notification/Notification.h"
#include "soh/Enhancements/cosmetics/authenticGfxPatches.h"
#ifdef ENABLE_REMOTE_CONTROL
#include "soh/Network/CrowdControl/CrowdControl.h"
#include "soh/Network/Sail/Sail.h"
#endif


#include "soh/Enhancements/audio/AudioEditor.h"
#include "soh/Enhancements/controls/InputViewer.h"
#include "soh/Enhancements/cosmetics/CosmeticsEditor.h"
#include "soh/Enhancements/debugger/actorViewer.h"
#include "soh/Enhancements/debugger/colViewer.h"
#include "soh/Enhancements/debugger/debugSaveEditor.h"
#include "soh/Enhancements/debugger/hookDebugger.h"
#include "soh/Enhancements/debugger/dlViewer.h"
#include "soh/Enhancements/debugger/valueViewer.h"
#include "soh/Enhancements/gameplaystatswindow.h"
#include "soh/Enhancements/debugger/MessageViewer.h"
#include "soh/Enhancements/randomizer/randomizer_check_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_entrance_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_item_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_settings_window.h"
#include "soh/Enhancements/enemyrandomizer.h"
#include "soh/Enhancements/timesplits/TimeSplits.h"
#include "soh/Enhancements/randomizer/Plandomizer.h"
#include "soh/Enhancements/TimeDisplay/TimeDisplay.h"

// FA icons are kind of wonky, if they worked how I expected them to the "+ 2.0f" wouldn't be needed, but
// they don't work how I expect them to so I added that because it looked good when I eyeballed it
#define FA_ICON_BUTTON_FRAME_PADDING_X(icon) (((optionsButtonSize.x - ImGui::CalcTextSize(icon).x) / 2) + 2.0f)

extern bool isBetaQuestEnabled;

extern "C" PlayState* gPlayState;

std::string GetWindowButtonText(const char* text, bool menuOpen) {
    char buttonText[100] = "";
    if (menuOpen) {
        strcat(buttonText, ICON_FA_CHEVRON_RIGHT " ");
    }
    strcat(buttonText, text);
    if (!menuOpen) { strcat(buttonText, "  "); }
    return buttonText;
}

static std::unordered_map<Ship::WindowBackend, const char*> windowBackendNames = {
    { Ship::WindowBackend::FAST3D_DXGI_DX11, "DirectX" },
    { Ship::WindowBackend::FAST3D_SDL_OPENGL, "OpenGL" },
    { Ship::WindowBackend::FAST3D_SDL_METAL, "Metal" },
};

static const char* imguiScaleOptions[4] = { "Small", "Normal", "Large", "X-Large" };

    static const char* filters[3] = {
#ifdef __WIIU__
            "",
#else
            "Three-Point",
#endif
            "Linear", "None"
    };

extern "C" SaveContext gSaveContext;

namespace SohGui {

std::unordered_map<Ship::WindowBackend, const char*> availableWindowBackendsMap;
Ship::WindowBackend configWindowBackend;

extern std::shared_ptr<Ship::GuiWindow> mGfxDebuggerWindow;

void DrawSettingsMenu() {
    //if (ImGui::BeginMenu("Settings"))
    //{
    //    if (ImGui::BeginMenu("Graphics")) {
    //        ImGui::Text("ImGui Menu Scale");
    //        ImGui::SameLine();
    //        ImGui::TextColored({ 0.85f, 0.35f, 0.0f, 1.0f }, "(Experimental)");
    //        if (UIWidgets::EnhancementCombobox(CVAR_SETTING("ImGuiScale"), imguiScaleOptions, 1)) {
    //            OTRGlobals::Instance->ScaleImGui();
    //        }
    //        UIWidgets::Tooltip("Changes the scaling of the ImGui menu elements.");

    //        // Draw LUS settings menu (such as Overlays Text Font)
    //        Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGameOverlay()->DrawSettings();

    //        ImGui::EndMenu();
    //    }
    //    ImGui::EndMenu();
    //}
}

void SohMenuBar::InitElement() {
    
}

void SohMenuBar::DrawElement() {
    if (ImGui::BeginMenuBar()) {
        static ImVec2 sWindowPadding(8.0f, 8.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, sWindowPadding);

        DrawSettingsMenu();

        ImGui::SetCursorPosY(0.0f);

        ImGui::PopStyleVar(1);
        ImGui::EndMenuBar();
    }
}
} // namespace SohGui
