#include "soh/SohGui/LocalizedWindow.h"
#include "soh/Enhancements/Lang/Lang.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "libultraship/libultra/controller.h"
#include <ship/Context.h>
#include <libultraship/controller/controldeck/ControlDeck.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <fast/Fast3dGui.h>

#include "InputViewer.h"
#include "soh/OTRGlobals.h"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"

using namespace UIWidgets;

// Text colors
static Color_RGBA8 textColorDefault = { 255, 255, 255, 255 };
static Color_RGBA8 range1ColorDefault = { 255, 178, 0, 255 };
static Color_RGBA8 range2ColorDefault = { 0, 255, 0, 255 };

static std::map<int32_t, const char*> buttonOutlineOptions = { { BUTTON_OUTLINE_ALWAYS_SHOWN, "Always Shown" },
                                                               { BUTTON_OUTLINE_NOT_PRESSED,
                                                                 "Shown Only While Not Pressed" },
                                                               { BUTTON_OUTLINE_PRESSED, "Shown Only While Pressed" },
                                                               { BUTTON_OUTLINE_ALWAYS_HIDDEN, "Always Hidden" } };
static std::map<int32_t, const char*> buttonOutlineOptionsVerbose = {
    { BUTTON_OUTLINE_ALWAYS_SHOWN, "Outline Always Shown" },
    { BUTTON_OUTLINE_NOT_PRESSED, "Outline Shown Only While Not Pressed" },
    { BUTTON_OUTLINE_PRESSED, "Outline Shown Only While Pressed" },
    { BUTTON_OUTLINE_ALWAYS_HIDDEN, "Outline Always Hidden" }
};

static std::map<int32_t, const char*> stickModeOptions = { { STICK_MODE_ALWAYS_SHOWN, "Always" },
                                                           { STICK_MODE_HIDDEN_IN_DEADZONE, "While In Use" },
                                                           { STICK_MODE_ALWAYS_HIDDEN, "Never" } };

InputViewer::~InputViewer() {
    SPDLOG_TRACE("destruct input viewer");
}

void InputViewer::RenderButton(std::string btnTexture, std::string btnOutlineTexture, int state, ImVec2 size,
                               int outlineMode) {
    const ImVec2 pos = ImGui::GetCursorPos();
    ImGui::SetNextItemAllowOverlap();
    // Render Outline based on settings
    if (outlineMode == BUTTON_OUTLINE_ALWAYS_SHOWN || (outlineMode == BUTTON_OUTLINE_NOT_PRESSED && !state) ||
        (outlineMode == BUTTON_OUTLINE_PRESSED && state)) {
        ImGui::Image(std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                         ->GetTextureByName(btnOutlineTexture),
                     size, ImVec2(0, 0), ImVec2(1.0f, 1.0f));
    }
    // Render button if pressed
    if (state) {
        ImGui::SetCursorPos(pos);
        ImGui::SetNextItemAllowOverlap();
        ImGui::Image(std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                         ->GetTextureByName(btnTexture),
                     size, ImVec2(0, 0), ImVec2(1.0f, 1.0f));
    }
}

void InputViewer::Draw() {
    if (!IsVisible()) {
        return;
    }
    DrawElement();
    // Sync up the IsVisible flag if it was changed by ImGui
    SyncVisibilityConsoleVariable();
}

void InputViewer::DrawElement() {
    if (CVarGetInteger(CVAR_WINDOW("InputViewer"), 0)) {
        auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
        static bool sButtonTexturesLoaded = false;
        if (!sButtonTexturesLoaded) {
            gui->LoadTextureFromRawImage("Input-Viewer-Background", "textures/buttons/InputViewerBackground.png");
            gui->LoadTextureFromRawImage("A-Btn", "textures/buttons/ABtn.png");
            gui->LoadTextureFromRawImage("B-Btn", "textures/buttons/BBtn.png");
            gui->LoadTextureFromRawImage("L-Btn", "textures/buttons/LBtn.png");
            gui->LoadTextureFromRawImage("R-Btn", "textures/buttons/RBtn.png");
            gui->LoadTextureFromRawImage("Z-Btn", "textures/buttons/ZBtn.png");
            gui->LoadTextureFromRawImage("Start-Btn", "textures/buttons/StartBtn.png");
            gui->LoadTextureFromRawImage("C-Left", "textures/buttons/CLeft.png");
            gui->LoadTextureFromRawImage("C-Right", "textures/buttons/CRight.png");
            gui->LoadTextureFromRawImage("C-Up", "textures/buttons/CUp.png");
            gui->LoadTextureFromRawImage("C-Down", "textures/buttons/CDown.png");
            gui->LoadTextureFromRawImage("Analog-Stick", "textures/buttons/AnalogStick.png");
            gui->LoadTextureFromRawImage("Dpad-Left", "textures/buttons/DPadLeft.png");
            gui->LoadTextureFromRawImage("Dpad-Right", "textures/buttons/DPadRight.png");
            gui->LoadTextureFromRawImage("Dpad-Up", "textures/buttons/DPadUp.png");
            gui->LoadTextureFromRawImage("Dpad-Down", "textures/buttons/DPadDown.png");
            gui->LoadTextureFromRawImage("Modifier-1", "textures/buttons/Mod1.png");
            gui->LoadTextureFromRawImage("Modifier-2", "textures/buttons/Mod2.png");
            gui->LoadTextureFromRawImage("Right-Stick", "textures/buttons/RightStick.png");
            gui->LoadTextureFromRawImage("A-Btn Outline", "textures/buttons/ABtnOutline.png");
            gui->LoadTextureFromRawImage("B-Btn Outline", "textures/buttons/BBtnOutline.png");
            gui->LoadTextureFromRawImage("L-Btn Outline", "textures/buttons/LBtnOutline.png");
            gui->LoadTextureFromRawImage("R-Btn Outline", "textures/buttons/RBtnOutline.png");
            gui->LoadTextureFromRawImage("Z-Btn Outline", "textures/buttons/ZBtnOutline.png");
            gui->LoadTextureFromRawImage("Start-Btn Outline", "textures/buttons/StartBtnOutline.png");
            gui->LoadTextureFromRawImage("C-Left Outline", "textures/buttons/CLeftOutline.png");
            gui->LoadTextureFromRawImage("C-Right Outline", "textures/buttons/CRightOutline.png");
            gui->LoadTextureFromRawImage("C-Up Outline", "textures/buttons/CUpOutline.png");
            gui->LoadTextureFromRawImage("C-Down Outline", "textures/buttons/CDownOutline.png");
            gui->LoadTextureFromRawImage("Analog-Stick Outline", "textures/buttons/AnalogStickOutline.png");
            gui->LoadTextureFromRawImage("Dpad-Left Outline", "textures/buttons/DPadLeftOutline.png");
            gui->LoadTextureFromRawImage("Dpad-Right Outline", "textures/buttons/DPadRightOutline.png");
            gui->LoadTextureFromRawImage("Dpad-Up Outline", "textures/buttons/DPadUpOutline.png");
            gui->LoadTextureFromRawImage("Dpad-Down Outline", "textures/buttons/DPadDownOutline.png");
            gui->LoadTextureFromRawImage("Modifier-1 Outline", "textures/buttons/Mod1Outline.png");
            gui->LoadTextureFromRawImage("Modifier-2 Outline", "textures/buttons/Mod2Outline.png");
            gui->LoadTextureFromRawImage("Right-Stick Outline", "textures/buttons/RightStickOutline.png");
            sButtonTexturesLoaded = true;
        }

        ImVec2 mainPos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetMainViewport()->WorkSize;

#ifdef __WIIU__
        const float scale = CVarGetFloat(CVAR_INPUT_VIEWER("Scale"), 1.0f) * 2.0f;
#else
        const float scale = CVarGetFloat(CVAR_INPUT_VIEWER("Scale"), 1.0f);
#endif
        const int showAnalogAngles = CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Enabled"), 0);
        const int buttonOutlineMode =
            CVarGetInteger(CVAR_INPUT_VIEWER("ButtonOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED);
        const bool useGlobalOutlineMode = CVarGetInteger(CVAR_INPUT_VIEWER("UseGlobalButtonOutlineMode"), 1);

        ImVec2 bgSize = gui->GetTextureSize("Input-Viewer-Background");
        ImVec2 scaledBGSize = ImVec2(bgSize.x * scale, bgSize.y * scale);

        ImGui::SetNextWindowSize(
            ImVec2(scaledBGSize.x + 20, scaledBGSize.y +
                                            (showAnalogAngles ? ImGui::CalcTextSize("X").y : 0) * scale *
                                                CVarGetFloat(CVAR_INPUT_VIEWER("AnalogAngles.Scale"), 1.0f) +
                                            20));
        ImGui::SetNextWindowContentSize(
            ImVec2(scaledBGSize.x, scaledBGSize.y + (showAnalogAngles ? 15 : 0) * scale *
                                                        CVarGetFloat(CVAR_INPUT_VIEWER("AnalogAngles.Scale"), 1.0f)));
        ImGui::SetNextWindowPos(
            ImVec2(mainPos.x + size.x - scaledBGSize.x - 30, mainPos.y + size.y - scaledBGSize.y - 30),
            ImGuiCond_FirstUseEver);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

        OSContPad* pads =
            std::dynamic_pointer_cast<LUS::ControlDeck>(Ship::Context::GetRawInstance()->GetControlDeck())->GetPads();

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                                       ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground |
                                       ImGuiWindowFlags_NoFocusOnAppearing;

        if (!CVarGetInteger(CVAR_INPUT_VIEWER("EnableDragging"), 1)) {
            windowFlags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
        }

        if (pads != nullptr &&
            ImGui::Begin(SohGui::LocalizedWindowTitle("ui.text.enhancements.controls.inputviewer", "Input Viewer"),
                         nullptr, windowFlags)) {
            ImGui::SetCursorPos(ImVec2(10, 10));
            const ImVec2 aPos = ImGui::GetCursorPos();

            if (CVarGetInteger(CVAR_INPUT_VIEWER("ShowBackground"), 1)) {
                ImGui::SetNextItemAllowOverlap();
                // Background
                ImGui::Image(gui->GetTextureByName("Input-Viewer-Background"), scaledBGSize, ImVec2(0, 0),
                             ImVec2(1.0f, 1.0f));
            }

            // A/B
            if (CVarGetInteger(CVAR_INPUT_VIEWER("BBtn"), 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("B-Btn", "B-Btn Outline", pads[0].button & BTN_B, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("BBtnOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
            }
            if (CVarGetInteger(CVAR_INPUT_VIEWER("ABtn"), 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("A-Btn", "A-Btn Outline", pads[0].button & BTN_A, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("ABtnOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
            }

            // C buttons
            if (CVarGetInteger(CVAR_INPUT_VIEWER("CUp"), 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("C-Up", "C-Up Outline", pads[0].button & BTN_CUP, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("CUpOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
            }
            if (CVarGetInteger(CVAR_INPUT_VIEWER("CLeft"), 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("C-Left", "C-Left Outline", pads[0].button & BTN_CLEFT, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("CLeftOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
            }
            if (CVarGetInteger(CVAR_INPUT_VIEWER("CRight"), 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("C-Right", "C-Right Outline", pads[0].button & BTN_CRIGHT, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("CRightOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
            }
            if (CVarGetInteger(CVAR_INPUT_VIEWER("CDown"), 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("C-Down", "C-Down Outline", pads[0].button & BTN_CDOWN, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("CDownOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
            }

            // L/R/Z
            if (CVarGetInteger(CVAR_INPUT_VIEWER("LBtn"), 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("L-Btn", "L-Btn Outline", pads[0].button & BTN_L, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("LBtnOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
            }
            if (CVarGetInteger(CVAR_INPUT_VIEWER("RBtn"), 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("R-Btn", "R-Btn Outline", pads[0].button & BTN_R, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("RBtnOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
            }
            if (CVarGetInteger(CVAR_INPUT_VIEWER("ZBtn"), 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Z-Btn", "Z-Btn Outline", pads[0].button & BTN_Z, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("ZBtnOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
            }

            // Start
            if (CVarGetInteger(CVAR_INPUT_VIEWER("StartBtn"), 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Start-Btn", "Start-Btn Outline", pads[0].button & BTN_START, scaledBGSize,
                             useGlobalOutlineMode ? buttonOutlineMode
                                                  : CVarGetInteger(CVAR_INPUT_VIEWER("StartBtnOutlineMode"),
                                                                   BUTTON_OUTLINE_NOT_PRESSED));
            }

            // Dpad
            if (CVarGetInteger(CVAR_INPUT_VIEWER("Dpad"), 0)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Dpad-Left", "Dpad-Left Outline", pads[0].button & BTN_DLEFT, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("DpadOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Dpad-Right", "Dpad-Right Outline", pads[0].button & BTN_DRIGHT, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("DpadOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Dpad-Up", "Dpad-Up Outline", pads[0].button & BTN_DUP, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("DpadOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Dpad-Down", "Dpad-Down Outline", pads[0].button & BTN_DDOWN, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("DpadOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
            }

            // Modifier 1
            if (CVarGetInteger(CVAR_INPUT_VIEWER("Mod1"), 0)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Modifier-1", "Modifier-1 Outline", pads[0].button & BTN_CUSTOM_MODIFIER1, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("Mod1OutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
            }
            // Modifier 2
            if (CVarGetInteger(CVAR_INPUT_VIEWER("Mod2"), 0)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Modifier-2", "Modifier-2 Outline", pads[0].button & BTN_CUSTOM_MODIFIER2, scaledBGSize,
                             useGlobalOutlineMode
                                 ? buttonOutlineMode
                                 : CVarGetInteger(CVAR_INPUT_VIEWER("Mod2OutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
            }

            const bool analogStickIsInDeadzone = !pads[0].stick_x && !pads[0].stick_y;
            const bool rightStickIsInDeadzone = !pads[0].right_stick_x && !pads[0].right_stick_y;

            // Analog Stick
            const int analogOutlineMode =
                CVarGetInteger(CVAR_INPUT_VIEWER("AnalogStick.OutlineMode"), STICK_MODE_ALWAYS_SHOWN);
            const int32_t maxStickDistance = CVarGetInteger(CVAR_INPUT_VIEWER("AnalogStick.Movement"), 12);
            if (analogOutlineMode == STICK_MODE_ALWAYS_SHOWN ||
                (analogOutlineMode == STICK_MODE_HIDDEN_IN_DEADZONE && !analogStickIsInDeadzone)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                ImGui::Image(gui->GetTextureByName("Analog-Stick Outline"), scaledBGSize, ImVec2(0, 0),
                             ImVec2(1.0f, 1.0f));
            }
            const int analogStickMode =
                CVarGetInteger(CVAR_INPUT_VIEWER("AnalogStick.VisibilityMode"), STICK_MODE_ALWAYS_SHOWN);
            if (analogStickMode == STICK_MODE_ALWAYS_SHOWN ||
                (analogStickMode == STICK_MODE_HIDDEN_IN_DEADZONE && !analogStickIsInDeadzone)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(
                    ImVec2(aPos.x + maxStickDistance * ((float)(pads[0].stick_x) / MAX_AXIS_RANGE) * scale,
                           aPos.y - maxStickDistance * ((float)(pads[0].stick_y) / MAX_AXIS_RANGE) * scale));
                ImGui::Image(gui->GetTextureByName("Analog-Stick"), scaledBGSize, ImVec2(0, 0), ImVec2(1.0f, 1.0f));
            }

            // Right Stick
            const int32_t maxRightStickDistance = CVarGetInteger(CVAR_INPUT_VIEWER("RightStick.Movement"), 7);
            const int rightOutlineMode =
                CVarGetInteger(CVAR_INPUT_VIEWER("RightStick.OutlineMode"), STICK_MODE_ALWAYS_HIDDEN);
            if (rightOutlineMode == STICK_MODE_ALWAYS_SHOWN ||
                (rightOutlineMode == STICK_MODE_HIDDEN_IN_DEADZONE && !rightStickIsInDeadzone)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                ImGui::Image(gui->GetTextureByName("Right-Stick Outline"), scaledBGSize, ImVec2(0, 0),
                             ImVec2(1.0f, 1.0f));
            }
            const int rightStickMode =
                CVarGetInteger(CVAR_INPUT_VIEWER("RightStick.VisibilityMode"), STICK_MODE_ALWAYS_HIDDEN);
            if (rightStickMode == STICK_MODE_ALWAYS_SHOWN ||
                (rightStickMode == STICK_MODE_HIDDEN_IN_DEADZONE && !rightStickIsInDeadzone)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(
                    ImVec2(aPos.x + maxRightStickDistance * ((float)(pads[0].right_stick_x) / MAX_AXIS_RANGE) * scale,
                           aPos.y - maxRightStickDistance * ((float)(pads[0].right_stick_y) / MAX_AXIS_RANGE) * scale));
                ImGui::Image(gui->GetTextureByName("Right-Stick"), scaledBGSize, ImVec2(0, 0), ImVec2(1.0f, 1.0f));
            }

            // Analog stick angle text
            if (showAnalogAngles) {
                ImGui::SetCursorPos(
                    ImVec2(aPos.x + 10 + CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Offset"), 0) * scale,
                           scaledBGSize.y + aPos.y + 10));
                // Scale font with input viewer scale
                float oldFontScale = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= scale * CVarGetFloat(CVAR_INPUT_VIEWER("AnalogAngles.Scale"), 1.0f);
                ImGui::PushFont(ImGui::GetFont());

                // Calculate polar R coordinate from X and Y angles, squared to avoid sqrt
                const int32_t rSquared = pads[0].stick_x * pads[0].stick_x + pads[0].stick_y * pads[0].stick_y;

                // ESS range
                const int range1Min = CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Range1.Min"), 8);
                const int range1Max = CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Range1.Max"), 27);
                // Walking speed range
                const int range2Min = CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Range2.Min"), 27);
                const int range2Max = CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Range2.Max"), 62);

                // Push color based on angle ranges
                if (CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Range1.Enabled"), 0) &&
                    (rSquared >= (range1Min * range1Min)) && (rSquared < (range1Max * range1Max))) {
                    ImGui::PushStyleColor(
                        ImGuiCol_Text, VecFromRGBA8(CVarGetColor(CVAR_INPUT_VIEWER("AnalogAngles.Range1.Color.Value"),
                                                                 range1ColorDefault)));
                } else if (CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Range2.Enabled"), 0) &&
                           (rSquared >= (range2Min * range2Min)) && (rSquared < (range2Max * range2Max))) {
                    ImGui::PushStyleColor(
                        ImGuiCol_Text, VecFromRGBA8(CVarGetColor(CVAR_INPUT_VIEWER("AnalogAngles.Range2.Color.Value"),
                                                                 range2ColorDefault)));
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text,
                                          VecFromRGBA8(CVarGetColor(CVAR_INPUT_VIEWER("AnalogAngles.TextColor.Value"),
                                                                    textColorDefault)));
                }

                // Render text
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.controls.inputviewer.x_3d_y_3d", "X: %-3d  Y: %-3d"),
                            pads[0].stick_x, pads[0].stick_y);
                // Restore original color
                ImGui::PopStyleColor();
                // Restore original font scale
                ImGui::GetFont()->Scale = oldFontScale;
                ImGui::PopFont();
            }

            ImGui::End();
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }
}

InputViewerSettingsWindow::~InputViewerSettingsWindow() {
    SPDLOG_TRACE("destruct input viewer settings window");
}

void InputViewerSettingsWindow::DrawElement() {
    // gInputViewer.Scale
    CVarSliderFloat(
        Lang::UILabel("ui.text.enhancements.controls.inputviewer.input_viewer_scale_2f", "Input Viewer Scale: %.2f"),
        CVAR_INPUT_VIEWER("Scale"),
        FloatSliderOptions()
            .Color(THEME_COLOR)
            .DefaultValue(1.0f)
            .Min(0.1f)
            .Max(5.0f)
            .ShowButtons(true)
            .Tooltip(
                Lang::UIString("ui.text.enhancements.controls.inputviewer.sets_the_on_screen_size_of_the_input_viewer",
                               "Sets the on screen size of the input viewer")));

    // gInputViewer.EnableDragging
    CVarCheckbox(Lang::UILabel("ui.text.enhancements.controls.inputviewer.enable_dragging", "Enable Dragging"),
                 CVAR_INPUT_VIEWER("EnableDragging"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));

    UIWidgets::PaddedSeparator(true, true);

    // gInputViewer.ShowBackground
    CVarCheckbox(
        Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_background_layer", "Show Background Layer"),
        CVAR_INPUT_VIEWER("ShowBackground"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));

    UIWidgets::PaddedSeparator(true, true);

    PushStyleHeader(THEME_COLOR);
    if (ImGui::CollapsingHeader(Lang::UILabel("ui.text.enhancements.controls.inputviewer.buttons", "Buttons"))) {

        // gInputViewer.ButtonOutlineMode
        CVarCombobox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.button_outlines_backgrounds",
                          "Button Outlines/Backgrounds"),
            CVAR_INPUT_VIEWER("ButtonOutlineMode"), buttonOutlineOptions,
            UIWidgets::ComboboxOptions(
                ComboboxOptions({ { .disabled = !CVarGetInteger(CVAR_INPUT_VIEWER("UseGlobalButtonOutlineMode"), 1),
                                    .disabledTooltip = "Disabled because Global Button Outline is off" } })
                    .Color(THEME_COLOR)
                    .DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED)
                    .Tooltip(Lang::UIString(
                        "ui.text.enhancements.controls.inputviewer.sets_the_desired_visibility_behavior_for_the_button_"
                        "outline_background_layers_useful_for_custom_inpu",
                        "Sets the desired visibility behavior for the button outline/background layers. Useful for "
                        "custom input viewers.")))
                .TranslationScope("ui.text.enhancements.controls.inputviewer"));

        // gInputViewer.UseGlobalButtonOutlineMode
        CVarCheckbox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.use_for_all_buttons", "Use for all buttons"),
            CVAR_INPUT_VIEWER("UseGlobalButtonOutlineMode"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));

        UIWidgets::PaddedSeparator();

        bool useIndividualOutlines = !CVarGetInteger(CVAR_INPUT_VIEWER("UseGlobalButtonOutlineMode"), 1);

        // gInputViewer.ABtn
        CVarCheckbox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_a_button_layers", "Show A-Button Layers"),
            CVAR_INPUT_VIEWER("ABtn"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("ABtn"), 1)) {
            ImGui::Indent();
            CVarCombobox("##ABtnOutline", CVAR_INPUT_VIEWER("ABtnOutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }
        // gInputViewer.BBtn
        CVarCheckbox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_b_button_layers", "Show B-Button Layers"),
            CVAR_INPUT_VIEWER("BBtn"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("BBtn"), 1)) {
            ImGui::Indent();
            CVarCombobox("##BBtnOutline", CVAR_INPUT_VIEWER("BBtnOutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }
        // gInputViewer.CUp
        CVarCheckbox(Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_c_up_layers", "Show C-Up Layers"),
                     CVAR_INPUT_VIEWER("CUp"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("CUp"), 1)) {
            ImGui::Indent();
            CVarCombobox("##CUpOutline", CVAR_INPUT_VIEWER("CUpOutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }
        // gInputViewer.CRight
        CVarCheckbox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_c_right_layers", "Show C-Right Layers"),
            CVAR_INPUT_VIEWER("CRight"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("CRight"), 1)) {
            ImGui::Indent();
            CVarCombobox("##CRightOutline", CVAR_INPUT_VIEWER("CRightOutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }
        // gInputViewer.CDown
        CVarCheckbox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_c_down_layers", "Show C-Down Layers"),
            CVAR_INPUT_VIEWER("CDown"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("CDown"), 1)) {
            ImGui::Indent();
            CVarCombobox("##CDownOutline", CVAR_INPUT_VIEWER("CDownOutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }
        // gInputViewer.CLeft
        CVarCheckbox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_c_left_layers", "Show C-Left Layers"),
            CVAR_INPUT_VIEWER("CLeft"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("CLeft"), 1)) {
            ImGui::Indent();
            CVarCombobox("##CLeftOutline", CVAR_INPUT_VIEWER("CLeftOutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }
        // gInputViewer.LBtn
        CVarCheckbox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_l_button_layers", "Show L-Button Layers"),
            CVAR_INPUT_VIEWER("LBtn"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("LBtn"), 1)) {
            ImGui::Indent();
            CVarCombobox("##LBtnOutline", CVAR_INPUT_VIEWER("LBtnOutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }
        // gInputViewer.RBtn
        CVarCheckbox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_r_button_layers", "Show R-Button Layers"),
            CVAR_INPUT_VIEWER("RBtn"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("RBtn"), 1)) {
            ImGui::Indent();
            CVarCombobox("##RBtnOutline", CVAR_INPUT_VIEWER("RBtnOutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }
        // gInputViewer.ZBtn
        CVarCheckbox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_z_button_layers", "Show Z-Button Layers"),
            CVAR_INPUT_VIEWER("ZBtn"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("ZBtn"), 1)) {
            ImGui::Indent();
            CVarCombobox("##ZBtnOutline", CVAR_INPUT_VIEWER("ZBtnOutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }
        // gInputViewer.StartBtn
        CVarCheckbox(Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_start_button_layers",
                                   "Show Start Button Layers"),
                     CVAR_INPUT_VIEWER("StartBtn"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("StartBtn"), 1)) {
            ImGui::Indent();
            CVarCombobox("##StartBtnOutline", CVAR_INPUT_VIEWER("StartBtnOutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }
        // gInputViewer.Dpad
        CVarCheckbox(Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_d_pad_layers", "Show D-Pad Layers"),
                     CVAR_INPUT_VIEWER("Dpad"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(false));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("Dpad"), 0)) {
            ImGui::Indent();
            CVarCombobox("##DpadOutline", CVAR_INPUT_VIEWER("DpadOutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }
        // gInputViewer.Mod1
        CVarCheckbox(Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_modifier_button_1_layers",
                                   "Show Modifier Button 1 Layers"),
                     CVAR_INPUT_VIEWER("Mod1"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(false));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("Mod1"), 0)) {
            ImGui::Indent();
            CVarCombobox("##Mmod1Outline", CVAR_INPUT_VIEWER("Mod1OutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }
        // gInputViewer.Mod2
        CVarCheckbox(Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_modifier_button_2_layers",
                                   "Show Modifier Button 2 Layers"),
                     CVAR_INPUT_VIEWER("Mod2"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(false));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("Mod2"), 0)) {
            ImGui::Indent();
            CVarCombobox("##Mod2Outline", CVAR_INPUT_VIEWER("Mod2OutlineMode"), buttonOutlineOptionsVerbose,
                         UIWidgets::ComboboxOptions(
                             ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED))
                             .TranslationScope("ui.text.enhancements.controls.inputviewer"));
            ImGui::Unindent();
        }

        UIWidgets::PaddedSeparator(true, true);
    }

    if (ImGui::CollapsingHeader(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.analog_stick", "Analog Stick"))) {
        // gInputViewer.AnalogStick.VisibilityMode
        CVarCombobox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.analog_stick_visibility",
                          "Analog Stick Visibility"),
            CVAR_INPUT_VIEWER("AnalogStick.VisibilityMode"), stickModeOptions,
            UIWidgets::ComboboxOptions(
                ComboboxOptions()
                    .Color(THEME_COLOR)
                    .DefaultIndex(STICK_MODE_ALWAYS_SHOWN)
                    .Tooltip(Lang::UIString("ui.text.enhancements.controls.inputviewer.determines_the_conditions_under_"
                                            "which_the_moving_layer_of_the_analog_stick_texture_is_visible",
                                            "Determines the conditions under which the moving layer of the analog "
                                            "stick texture is visible.")))
                .TranslationScope("ui.text.enhancements.controls.inputviewer"));

        // gInputViewer.AnalogStick.OutlineMode
        CVarCombobox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.analog_stick_outline_background_visibility",
                          "Analog Stick Outline/Background Visibility"),
            CVAR_INPUT_VIEWER("AnalogStick.OutlineMode"), stickModeOptions,
            UIWidgets::ComboboxOptions(
                ComboboxOptions()
                    .Color(THEME_COLOR)
                    .DefaultIndex(STICK_MODE_ALWAYS_SHOWN)
                    .Tooltip(Lang::UIString("ui.text.enhancements.controls.inputviewer.determines_the_conditions_under_"
                                            "which_the_analog_stick_outline_background_texture_is_visible",
                                            "Determines the conditions under which the analog stick outline/background "
                                            "texture is visible.")))
                .TranslationScope("ui.text.enhancements.controls.inputviewer"));

        // gInputViewer.AnalogStick.Movement
        CVarSliderInt(Lang::UILabel("ui.text.enhancements.controls.inputviewer.analog_stick_movement_dpx",
                                    "Analog Stick Movement: %dpx"),
                      CVAR_INPUT_VIEWER("AnalogStick.Movement"),
                      IntSliderOptions()
                          .Color(THEME_COLOR)
                          .Min(0)
                          .Max(200)
                          .DefaultValue(12)
                          .ShowButtons(true)
                          .Tooltip(Lang::UIString(
                              "ui.text.enhancements.controls.inputviewer.sets_the_distance_to_move_the_analog_stick_in_"
                              "the_input_viewer_useful_for_custom_input_viewers",
                              "Sets the distance to move the analog stick in the input viewer. Useful for custom "
                              "input viewers.")));
        UIWidgets::PaddedSeparator(true, true);
    }

    if (ImGui::CollapsingHeader(Lang::UILabel("ui.text.enhancements.controls.inputviewer.additional_right_stick",
                                              "Additional (\"Right\") Stick"))) {
        // gInputViewer.RightStick.VisibilityMode
        CVarCombobox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.right_stick_visibility", "Right Stick Visibility"),
            CVAR_INPUT_VIEWER("RightStick.VisibilityMode"), stickModeOptions,
            UIWidgets::ComboboxOptions(
                ComboboxOptions()
                    .Color(THEME_COLOR)
                    .DefaultIndex(STICK_MODE_ALWAYS_HIDDEN)
                    .Tooltip(Lang::UIString("ui.text.enhancements.controls.inputviewer.determines_the_conditions_under_"
                                            "which_the_moving_layer_of_the_right_stick_texture_is_visible",
                                            "Determines the conditions under which the moving layer of the right stick "
                                            "texture is visible.")))
                .TranslationScope("ui.text.enhancements.controls.inputviewer"));

        // gInputViewer.RightStick.OutlineMode
        CVarCombobox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.right_stick_outline_background_visibility",
                          "Right Stick Outline/Background Visibility"),
            CVAR_INPUT_VIEWER("RightStick.OutlineMode"), stickModeOptions,
            UIWidgets::ComboboxOptions(
                ComboboxOptions()
                    .Color(THEME_COLOR)
                    .DefaultIndex(STICK_MODE_ALWAYS_HIDDEN)
                    .Tooltip(Lang::UIString("ui.text.enhancements.controls.inputviewer.determines_the_conditions_under_"
                                            "which_the_right_stick_outline_background_texture_is_visible",
                                            "Determines the conditions under which the right stick outline/background "
                                            "texture is visible.")))
                .TranslationScope("ui.text.enhancements.controls.inputviewer"));

        // gInputViewer.RightStick.Movement
        CVarSliderInt(Lang::UILabel("ui.text.enhancements.controls.inputviewer.right_stick_movement_dpx",
                                    "Right Stick Movement: %dpx"),
                      CVAR_INPUT_VIEWER("RightStick.Movement"),
                      IntSliderOptions()
                          .Color(THEME_COLOR)
                          .Min(0)
                          .Max(200)
                          .DefaultValue(7)
                          .ShowButtons(true)
                          .Tooltip(Lang::UIString("ui.text.enhancements.controls.inputviewer.sets_the_distance_to_move_"
                                                  "the_right_stick_in_the_input_viewer_useful_for_custom_input_viewers",
                                                  "Sets the distance to move the right stick in the input viewer. "
                                                  "Useful for custom input viewers.")));
        UIWidgets::PaddedSeparator(true, true);
    }

    if (ImGui::CollapsingHeader(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.analog_angle_values", "Analog Angle Values"))) {
        // gAnalogAngles
        CVarCheckbox(
            Lang::UILabel("ui.text.enhancements.controls.inputviewer.show_analog_stick_angle_values",
                          "Show Analog Stick Angle Values"),
            CVAR_INPUT_VIEWER("AnalogAngles.Enabled"),
            CheckboxOptions()
                .Color(THEME_COLOR)
                .Tooltip(Lang::UIString(
                    "ui.text.enhancements.controls.inputviewer.displays_analog_stick_angle_values_in_the_input_viewer",
                    "Displays analog stick angle values in the input viewer")));
        if (CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Enabled"), 0)) {
            // gInputViewer.AnalogAngles.TextColor
            CVarColorPicker(Lang::UILabel("ui.text.enhancements.controls.inputviewer.text_color", "Text Color"),
                            CVAR_INPUT_VIEWER("AnalogAngles.TextColor"), textColorDefault, true,
                            ColorPickerRandomButton | ColorPickerResetButton);
            // gAnalogAngleScale
            CVarSliderFloat(Lang::UILabel("ui.text.enhancements.controls.inputviewer.angle_text_scale_2f",
                                          "Angle Text Scale: %.2f%%"),
                            CVAR_INPUT_VIEWER("AnalogAngles.Scale"),
                            FloatSliderOptions()
                                .Color(THEME_COLOR)
                                .IsPercentage()
                                .Min(0.1f)
                                .Max(5.0f)
                                .DefaultValue(1.0f)
                                .ShowButtons(true));
            // gInputViewer.AnalogAngles.Offset
            CVarSliderInt(
                Lang::UILabel("ui.text.enhancements.controls.inputviewer.angle_text_offset_dpx",
                              "Angle Text Offset: %dpx"),
                CVAR_INPUT_VIEWER("AnalogAngles.Offset"),
                IntSliderOptions()
                    .Color(THEME_COLOR)
                    .Min(0)
                    .Max(400)
                    .DefaultValue(0)
                    .ShowButtons(true)
                    .Tooltip(Lang::UIString("ui.text.enhancements.controls.inputviewer.sets_the_distance_to_move_the_"
                                            "right_stick_in_the_input_viewer_useful_for_custom_input_viewers",
                                            "Sets the distance to move the right stick in the input viewer. Useful for "
                                            "custom input viewers.")));
            UIWidgets::PaddedSeparator(true, true);
            // gInputViewer.AnalogAngles.Range1.Enabled
            CVarCheckbox(
                Lang::UILabel("ui.text.enhancements.controls.inputviewer.highlight_ess_position",
                              "Highlight ESS Position"),
                CVAR_INPUT_VIEWER("AnalogAngles.Range1.Enabled"),
                CheckboxOptions()
                    .Color(THEME_COLOR)
                    .Tooltip(Lang::UIString(
                        "ui.text.enhancements.controls.inputviewer.highlights_the_angle_value_text_when_the_analog_"
                        "stick_is_in_ess_position_on_flat_ground",
                        "Highlights the angle value text when the analog stick is in ESS position (on flat ground)")));
            if (CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Range1.Enabled"), 0)) {
                // gInputViewer.AnalogAngles.Range1.Color
                CVarColorPicker(Lang::UILabel("ui.text.enhancements.controls.inputviewer.ess_color", "ESS Color"),
                                CVAR_INPUT_VIEWER("AnalogAngles.Range1.Color"), range1ColorDefault, true,
                                ColorPickerRandomButton | ColorPickerResetButton);
            }

            UIWidgets::PaddedSeparator(true, true);
            // gInputViewer.AnalogAngles.Range2.Enabled
            CVarCheckbox(Lang::UILabel("ui.text.enhancements.controls.inputviewer.highlight_walking_speed_angles",
                                       "Highlight Walking Speed Angles"),
                         CVAR_INPUT_VIEWER("AnalogAngles.Range2.Enabled"),
                         CheckboxOptions()
                             .Color(THEME_COLOR)
                             .Tooltip(Lang::UIString(
                                 "ui.text.enhancements.controls.inputviewer.highlights_the_angle_value_text_when_the_"
                                 "analog_stick_is_at_an_angle_that_would_produce_a_walking_sp",
                                 "Highlights the angle value text when the analog stick is at an angle that would "
                                 "produce a walking speed (on flat ground)\n\n"
                                 "Useful for 1.0 Empty Jumpslash Quick Put Away")));
            if (CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Range2.Enabled"), 0)) {
                // gInputViewer.AnalogAngles.Range2.Color
                CVarColorPicker(Lang::UILabel("ui.text.enhancements.controls.inputviewer.walking_speed_color",
                                              "Walking Speed Color"),
                                CVAR_INPUT_VIEWER("AnalogAngles.Range2.Color"), range2ColorDefault, true,
                                ColorPickerRandomButton | ColorPickerResetButton);
            }
        }
    }
    PopStyleHeader();
}
