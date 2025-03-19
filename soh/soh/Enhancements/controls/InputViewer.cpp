#include "InputViewer.h"

#include "public/bridge/consolevariablebridge.h"
#include "libultraship/libultra/controller.h"
#include "Context.h"
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <cmath>

#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"

using namespace UIWidgets;

// Text colors
static Color_RGBA8 textColorDefault = { 255, 255, 255, 255 };
static Color_RGBA8 range1ColorDefault = { 255, 178, 0, 255 };
static Color_RGBA8 range2ColorDefault = { 0, 255, 0, 255 };

static std::unordered_map<int32_t, const char*> buttonOutlineOptions = 
    {{ BUTTON_OUTLINE_ALWAYS_SHOWN, "Always Shown" }, { BUTTON_OUTLINE_NOT_PRESSED, "Shown Only While Not Pressed" },
     { BUTTON_OUTLINE_PRESSED, "Shown Only While Pressed" }, { BUTTON_OUTLINE_ALWAYS_HIDDEN, "Always Hidden" }};
static std::unordered_map<int32_t, const char*> buttonOutlineOptionsVerbose = 
    {{ BUTTON_OUTLINE_ALWAYS_SHOWN, "Outline Always Shown" }, { BUTTON_OUTLINE_NOT_PRESSED, "Outline Shown Only While Not Pressed" },
     { BUTTON_OUTLINE_PRESSED, "Outline Shown Only While Pressed" }, { BUTTON_OUTLINE_ALWAYS_HIDDEN, "Outline Always Hidden" }};

static std::unordered_map<int32_t, const char*> stickModeOptions = 
    {{ STICK_MODE_ALWAYS_SHOWN, "Always" }, { STICK_MODE_HIDDEN_IN_DEADZONE, "While In Use" }, { STICK_MODE_ALWAYS_HIDDEN, "Never" }};

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
        ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(btnOutlineTexture), size,
                     ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
    }
    // Render button if pressed
    if (state) {
        ImGui::SetCursorPos(pos);
        ImGui::SetNextItemAllowOverlap();
        ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(btnTexture), size,
                     ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
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
        static bool sButtonTexturesLoaded = false;
        if (!sButtonTexturesLoaded) {
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage(
                "Input-Viewer-Background", "textures/buttons/InputViewerBackground.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("A-Btn", "textures/buttons/ABtn.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("B-Btn", "textures/buttons/BBtn.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("L-Btn", "textures/buttons/LBtn.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("R-Btn", "textures/buttons/RBtn.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Z-Btn", "textures/buttons/ZBtn.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Start-Btn",
                "textures/buttons/StartBtn.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Left", "textures/buttons/CLeft.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Right", "textures/buttons/CRight.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Up", "textures/buttons/CUp.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Down", "textures/buttons/CDown.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Analog-Stick",
                "textures/buttons/AnalogStick.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Left",
                "textures/buttons/DPadLeft.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Right",
                "textures/buttons/DPadRight.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Up", "textures/buttons/DPadUp.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Down",
                "textures/buttons/DPadDown.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Modifier-1", "textures/buttons/Mod1.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Modifier-2", "textures/buttons/Mod2.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Right-Stick",
                "textures/buttons/RightStick.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("A-Btn Outline",
                "textures/buttons/ABtnOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("B-Btn Outline",
                "textures/buttons/BBtnOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("L-Btn Outline",
                "textures/buttons/LBtnOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("R-Btn Outline",
                "textures/buttons/RBtnOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Z-Btn Outline",
                "textures/buttons/ZBtnOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Start-Btn Outline",
                "textures/buttons/StartBtnOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Left Outline",
                "textures/buttons/CLeftOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Right Outline",
                "textures/buttons/CRightOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Up Outline",
                "textures/buttons/CUpOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Down Outline",
                "textures/buttons/CDownOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Analog-Stick Outline",
                "textures/buttons/AnalogStickOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Left Outline",
                "textures/buttons/DPadLeftOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Right Outline",
                "textures/buttons/DPadRightOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Up Outline",
                "textures/buttons/DPadUpOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Down Outline",
                "textures/buttons/DPadDownOutline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Modifier-1 Outline",
                "textures/buttons/Mod1Outline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Modifier-2 Outline",
                "textures/buttons/Mod2Outline.png");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Right-Stick Outline",
                "textures/buttons/RightStickOutline.png");
            sButtonTexturesLoaded = true;
        }

        ImVec2 mainPos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetContentRegionAvail();

#ifdef __WIIU__
        const float scale = CVarGetFloat(CVAR_INPUT_VIEWER("Scale"), 1.0f) * 2.0f;
#else
        const float scale = CVarGetFloat(CVAR_INPUT_VIEWER("Scale"), 1.0f);
#endif
        const int showAnalogAngles = CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Enabled"), 0);
        const int buttonOutlineMode = CVarGetInteger(CVAR_INPUT_VIEWER("ButtonOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED);
        const bool useGlobalOutlineMode = CVarGetInteger(CVAR_INPUT_VIEWER("UseGlobalButtonOutlineMode"), 1);

        ImVec2 bgSize = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureSize("Input-Viewer-Background");
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

        OSContPad* pads = std::dynamic_pointer_cast<LUS::ControlDeck>(Ship::Context::GetInstance()->GetControlDeck())->GetPads();

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoFocusOnAppearing;

        if (!CVarGetInteger(CVAR_INPUT_VIEWER("EnableDragging"), 1)) {
            windowFlags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
        }

        if (pads != nullptr && ImGui::Begin("Input Viewer", nullptr, windowFlags)) {
            ImGui::SetCursorPos(ImVec2(10, 10));
            const ImVec2 aPos = ImGui::GetCursorPos();

            if (CVarGetInteger(CVAR_INPUT_VIEWER("ShowBackground"), 1)) {
                ImGui::SetNextItemAllowOverlap();
                // Background
                ImGui::Image(
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Input-Viewer-Background"),
                    scaledBGSize, ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
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
                    useGlobalOutlineMode
                    ? buttonOutlineMode
                    : CVarGetInteger(CVAR_INPUT_VIEWER("StartBtnOutlineMode"), BUTTON_OUTLINE_NOT_PRESSED));
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
            const float maxStickDistance = CVarGetInteger(CVAR_INPUT_VIEWER("AnalogStick.Movement"), 12);
            if (analogOutlineMode == STICK_MODE_ALWAYS_SHOWN ||
                (analogOutlineMode == STICK_MODE_HIDDEN_IN_DEADZONE && !analogStickIsInDeadzone)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                ImGui::Image(
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Analog-Stick Outline"),
                    scaledBGSize, ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
            }
            const int analogStickMode =
                CVarGetInteger(CVAR_INPUT_VIEWER("AnalogStick.VisibilityMode"), STICK_MODE_ALWAYS_SHOWN);
            if (analogStickMode == STICK_MODE_ALWAYS_SHOWN ||
                (analogStickMode == STICK_MODE_HIDDEN_IN_DEADZONE && !analogStickIsInDeadzone)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(
                    ImVec2(aPos.x + maxStickDistance * ((float)(pads[0].stick_x) / MAX_AXIS_RANGE) * scale,
                        aPos.y - maxStickDistance * ((float)(pads[0].stick_y) / MAX_AXIS_RANGE) * scale));
                ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Analog-Stick"),
                    scaledBGSize, ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
            }

            // Right Stick
            const float maxRightStickDistance = CVarGetInteger(CVAR_INPUT_VIEWER("RightStick.Movement"), 7);
            const int rightOutlineMode =
                CVarGetInteger(CVAR_INPUT_VIEWER("RightStick.OutlineMode"), STICK_MODE_ALWAYS_HIDDEN);
            if (rightOutlineMode == STICK_MODE_ALWAYS_SHOWN ||
                (rightOutlineMode == STICK_MODE_HIDDEN_IN_DEADZONE && !rightStickIsInDeadzone)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                ImGui::Image(
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Right-Stick Outline"),
                    scaledBGSize, ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
            }
            const int rightStickMode =
                CVarGetInteger(CVAR_INPUT_VIEWER("RightStick.VisibilityMode"), STICK_MODE_ALWAYS_HIDDEN);
            if (rightStickMode == STICK_MODE_ALWAYS_SHOWN ||
                (rightStickMode == STICK_MODE_HIDDEN_IN_DEADZONE && !rightStickIsInDeadzone)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(
                    ImVec2(aPos.x + maxRightStickDistance * ((float)(pads[0].right_stick_x) / MAX_AXIS_RANGE) * scale,
                        aPos.y - maxRightStickDistance * ((float)(pads[0].right_stick_y) / MAX_AXIS_RANGE) * scale));
                ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Right-Stick"),
                    scaledBGSize, ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
            }

            // Analog stick angle text
            if (showAnalogAngles) {
                ImGui::SetCursorPos(ImVec2(aPos.x + 10 + CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Offset"), 0) * scale,
                    scaledBGSize.y + aPos.y + 10));
                // Scale font with input viewer scale
                float oldFontScale = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= scale * CVarGetFloat(CVAR_INPUT_VIEWER("AnalogAngles.Scale"), 1.0f);
                ImGui::PushFont(ImGui::GetFont());

                // Calculate polar R coordinate from X and Y angles, squared to avoid sqrt
                const float rSquared = pads[0].stick_x * pads[0].stick_x + pads[0].stick_y * pads[0].stick_y;

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
                        ImGuiCol_Text,
                        VecFromRGBA8(CVarGetColor(CVAR_INPUT_VIEWER("AnalogAngles.Range1.Color.Value"), range1ColorDefault)));
                }
                else if (CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Range2.Enabled"), 0) &&
                    (rSquared >= (range2Min * range2Min)) && (rSquared < (range2Max * range2Max))) {
                    ImGui::PushStyleColor(
                        ImGuiCol_Text,
                        VecFromRGBA8(CVarGetColor(CVAR_INPUT_VIEWER("AnalogAngles.Range2.Color.Value"), range2ColorDefault)));
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_Text, VecFromRGBA8(CVarGetColor(CVAR_INPUT_VIEWER("AnalogAngles.TextColor.Value"),
                        textColorDefault)));
                }

                // Render text
                ImGui::Text("X: %-3d  Y: %-3d", pads[0].stick_x, pads[0].stick_y);
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
    CVarSliderFloat("Input Viewer Scale: %.2f", CVAR_INPUT_VIEWER("Scale"),
        FloatSliderOptions().Color(THEME_COLOR).DefaultValue(1.0f).Min(0.1f).Max(5.0f).ShowButtons(true).Tooltip("Sets the on screen size of the input viewer"));

    // gInputViewer.EnableDragging
    CVarCheckbox("Enable Dragging", CVAR_INPUT_VIEWER("EnableDragging"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));

    UIWidgets::PaddedSeparator(true, true);

    // gInputViewer.ShowBackground
    CVarCheckbox("Show Background Layer", CVAR_INPUT_VIEWER("ShowBackground"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));

    UIWidgets::PaddedSeparator(true, true);

    PushStyleHeader(THEME_COLOR);
    if (ImGui::CollapsingHeader("Buttons")) {

        // gInputViewer.ButtonOutlineMode
        CVarCombobox("Button Outlines/Backgrounds", CVAR_INPUT_VIEWER("ButtonOutlineMode"), buttonOutlineOptions,
            ComboboxOptions({{ .disabled = !CVarGetInteger(CVAR_INPUT_VIEWER("UseGlobalButtonOutlineMode"), 1), .disabledTooltip = "Disabled because Global Button Outline is off" }})
            .Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED)
            .Tooltip("Sets the desired visibility behavior for the button outline/background layers. Useful for "
                "custom input viewers."));

        // gInputViewer.UseGlobalButtonOutlineMode
        CVarCheckbox("Use for all buttons", CVAR_INPUT_VIEWER("UseGlobalButtonOutlineMode"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));

        UIWidgets::PaddedSeparator();

        bool useIndividualOutlines = !CVarGetInteger(CVAR_INPUT_VIEWER("UseGlobalButtonOutlineMode"), 1);

        // gInputViewer.ABtn
        CVarCheckbox("Show A-Button Layers", CVAR_INPUT_VIEWER("ABtn"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("ABtn"), 1)) {
            ImGui::Indent();
            CVarCombobox("##ABtnOutline", CVAR_INPUT_VIEWER("ABtnOutlineMode"), buttonOutlineOptionsVerbose, 
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }
        // gInputViewer.BBtn
        CVarCheckbox("Show B-Button Layers", CVAR_INPUT_VIEWER("BBtn"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("BBtn"), 1)) {
            ImGui::Indent();
            CVarCombobox("##BBtnOutline", CVAR_INPUT_VIEWER("BBtnOutlineMode"), buttonOutlineOptionsVerbose,
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }
        // gInputViewer.CUp
        CVarCheckbox("Show C-Up Layers", CVAR_INPUT_VIEWER("CUp"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("CUp"), 1)) {
            ImGui::Indent();
            CVarCombobox("##CUpOutline", CVAR_INPUT_VIEWER("CUpOutlineMode"), buttonOutlineOptionsVerbose,
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }
        // gInputViewer.CRight
        CVarCheckbox("Show C-Right Layers", CVAR_INPUT_VIEWER("CRight"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("CRight"), 1)) {
            ImGui::Indent();
            CVarCombobox("##CRightOutline", CVAR_INPUT_VIEWER("CRightOutlineMode"), buttonOutlineOptionsVerbose,
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }
        // gInputViewer.CDown
        CVarCheckbox("Show C-Down Layers", CVAR_INPUT_VIEWER("CDown"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("CDown"), 1)) {
            ImGui::Indent();
            CVarCombobox("##CDownOutline", CVAR_INPUT_VIEWER("CDownOutlineMode"), buttonOutlineOptionsVerbose,
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }
        // gInputViewer.CLeft
        CVarCheckbox("Show C-Left Layers", CVAR_INPUT_VIEWER("CLeft"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("CLeft"), 1)) {
            ImGui::Indent();
            CVarCombobox("##CLeftOutline", CVAR_INPUT_VIEWER("CLeftOutlineMode"), buttonOutlineOptionsVerbose,
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }
        // gInputViewer.LBtn
        CVarCheckbox("Show L-Button Layers", CVAR_INPUT_VIEWER("LBtn"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("LBtn"), 1)) {
            ImGui::Indent();
            CVarCombobox("##LBtnOutline", CVAR_INPUT_VIEWER("LBtnOutlineMode"), buttonOutlineOptionsVerbose,
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }
        // gInputViewer.RBtn
        CVarCheckbox("Show R-Button Layers", CVAR_INPUT_VIEWER("RBtn"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("RBtn"), 1)) {
            ImGui::Indent();
            CVarCombobox("##RBtnOutline", CVAR_INPUT_VIEWER("RBtnOutlineMode"), buttonOutlineOptionsVerbose,
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }
        // gInputViewer.ZBtn
        CVarCheckbox("Show Z-Button Layers", CVAR_INPUT_VIEWER("ZBtn"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("ZBtn"), 1)) {
            ImGui::Indent();
            CVarCombobox("##ZBtnOutline", CVAR_INPUT_VIEWER("ZBtnOutlineMode"), buttonOutlineOptionsVerbose,
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }
        // gInputViewer.StartBtn
        CVarCheckbox("Show Start Button Layers", CVAR_INPUT_VIEWER("StartBtn"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("StartBtn"), 1)) {
            ImGui::Indent();
            CVarCombobox("##StartBtnOutline", CVAR_INPUT_VIEWER("StartBtnOutlineMode"), buttonOutlineOptionsVerbose,
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }
        // gInputViewer.Dpad
        CVarCheckbox("Show D-Pad Layers", CVAR_INPUT_VIEWER("Dpad"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("Dpad"), 0)) {
            ImGui::Indent();
            CVarCombobox("##DpadOutline", CVAR_INPUT_VIEWER("DpadOutlineMode"), buttonOutlineOptionsVerbose,
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }
        // gInputViewer.Mod1
        CVarCheckbox("Show Modifier Button 1 Layers", CVAR_INPUT_VIEWER("Mod1"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("Mod1"), 0)) {
            ImGui::Indent();
            CVarCombobox("##Mmod1Outline", CVAR_INPUT_VIEWER("Mod1OutlineMode"), buttonOutlineOptionsVerbose,
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }
        // gInputViewer.Mod2
        CVarCheckbox("Show Modifier Button 2 Layers", CVAR_INPUT_VIEWER("Mod2"), CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        if (useIndividualOutlines && CVarGetInteger(CVAR_INPUT_VIEWER("Mod2"), 0)) {
            ImGui::Indent();
            CVarCombobox("##Mod2Outline", CVAR_INPUT_VIEWER("Mod2OutlineMode"), buttonOutlineOptionsVerbose,
                ComboboxOptions().Color(THEME_COLOR).DefaultIndex(BUTTON_OUTLINE_NOT_PRESSED));
            ImGui::Unindent();
        }

        UIWidgets:PaddedSeparator(true, true);
    }

    if (ImGui::CollapsingHeader("Analog Stick")) {
        // gInputViewer.AnalogStick.VisibilityMode
        CVarCombobox("Analog Stick Visibility", CVAR_INPUT_VIEWER("AnalogStick.VisibilityMode"), stickModeOptions,
            ComboboxOptions().Color(THEME_COLOR).DefaultIndex(STICK_MODE_ALWAYS_SHOWN)
            .Tooltip("Determines the conditions under which the moving layer of the analog stick texture is visible."));

        // gInputViewer.AnalogStick.OutlineMode
        CVarCombobox("Analog Stick Outline/Background Visibility", CVAR_INPUT_VIEWER("AnalogStick.OutlineMode"), stickModeOptions,
            ComboboxOptions().Color(THEME_COLOR).DefaultIndex(STICK_MODE_ALWAYS_SHOWN)
            .Tooltip("Determines the conditions under which the analog stick outline/background texture is visible."));

        // gInputViewer.AnalogStick.Movement
        CVarSliderInt("Analog Stick Movement: %dpx", CVAR_INPUT_VIEWER("AnalogStick.Movement"), IntSliderOptions().Color(THEME_COLOR).Min(0).Max(200).DefaultValue(12).ShowButtons(true)
            .Tooltip("Sets the distance to move the analog stick in the input viewer. Useful for custom input viewers."));
        UIWidgets::PaddedSeparator(true, true);
    }

    if (ImGui::CollapsingHeader("Additional (\"Right\") Stick")) {
        // gInputViewer.RightStick.VisibilityMode
        CVarCombobox("Right Stick Visibility", CVAR_INPUT_VIEWER("RightStick.VisibilityMode"), stickModeOptions,
            ComboboxOptions().Color(THEME_COLOR).DefaultIndex(STICK_MODE_ALWAYS_SHOWN)
            .Tooltip("Determines the conditions under which the moving layer of the right stick texture is visible."));

        // gInputViewer.RightStick.OutlineMode
        CVarCombobox("Right Stick Outline/Background Visibility", CVAR_INPUT_VIEWER("RightStick.OutlineMode"), stickModeOptions,
            ComboboxOptions().Color(THEME_COLOR).DefaultIndex(STICK_MODE_ALWAYS_SHOWN)
            .Tooltip("Determines the conditions under which the right stick outline/background texture is visible."));

        // gInputViewer.RightStick.Movement
        CVarSliderInt("Right Stick Movement: %dpx", CVAR_INPUT_VIEWER("RightStick.Movement"), IntSliderOptions().Color(THEME_COLOR).Min(0).Max(200).DefaultValue(7).ShowButtons(true)
            .Tooltip("Sets the distance to move the right stick in the input viewer. Useful for custom input viewers."));
        UIWidgets::PaddedSeparator(true, true);
    }

    if (ImGui::CollapsingHeader("Analog Angle Values")) {
        // gAnalogAngles
        CVarCheckbox("Show Analog Stick Angle Values", CVAR_INPUT_VIEWER("AnalogAngles.Enabled"), CheckboxOptions().Color(THEME_COLOR)
            .Tooltip("Displays analog stick angle values in the input viewer"));
        if (CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Enabled"), 0)) {
            // gInputViewer.AnalogAngles.TextColor
            CVarColorPicker("Text Color", CVAR_INPUT_VIEWER("AnalogAngles.TextColor"), textColorDefault, 
                            true, ColorPickerRandomButton | ColorPickerResetButton);
            // gAnalogAngleScale
            CVarSliderFloat("Angle Text Scale: %.2f%%", CVAR_INPUT_VIEWER("AnalogAngles.Scale"), 
                FloatSliderOptions().Color(THEME_COLOR).IsPercentage().Min(0.1f).Max(5.0f).DefaultValue(1.0f).ShowButtons(true));
            // gInputViewer.AnalogAngles.Offset
            CVarSliderInt("Angle Text Offset: %dpx", CVAR_INPUT_VIEWER("AnalogAngles.Offset"), IntSliderOptions().Color(THEME_COLOR).Min(0).Max(400).DefaultValue(0).ShowButtons(true)
                .Tooltip("Sets the distance to move the right stick in the input viewer. Useful for custom input viewers."));
            UIWidgets::PaddedSeparator(true, true);
            // gInputViewer.AnalogAngles.Range1.Enabled
            CVarCheckbox("Highlight ESS Position", CVAR_INPUT_VIEWER("AnalogAngles.Range1.Enabled"), CheckboxOptions().Color(THEME_COLOR)
                .Tooltip("Highlights the angle value text when the analog stick is in ESS position (on flat ground)"));
            if (CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Range1.Enabled"), 0)) {
                // gInputViewer.AnalogAngles.Range1.Color
                CVarColorPicker("ESS Color", CVAR_INPUT_VIEWER("AnalogAngles.Range1.Color"), range1ColorDefault,
                    true, ColorPickerRandomButton | ColorPickerResetButton);
            }

            UIWidgets::PaddedSeparator(true, true);
            // gInputViewer.AnalogAngles.Range2.Enabled
            CVarCheckbox("Highlight Walking Speed Angles", CVAR_INPUT_VIEWER("AnalogAngles.Range2.Enabled"), CheckboxOptions().Color(THEME_COLOR)
                .Tooltip("Highlights the angle value text when the analog stick is at an angle that would "
                                "produce a walking speed (on flat ground)\n\n"
                    "Useful for 1.0 Empty Jumpslash Quick Put Away"));
            if (CVarGetInteger(CVAR_INPUT_VIEWER("AnalogAngles.Range2.Enabled"), 0)) {
                // gInputViewer.AnalogAngles.Range2.Color
                CVarColorPicker("Walking Speed Color", CVAR_INPUT_VIEWER("AnalogAngles.Range2.Color"), range2ColorDefault, 
                                true, ColorPickerRandomButton | ColorPickerResetButton);
            }
        }
    }
    PopStyleHeader();
}
