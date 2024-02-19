#include "InputViewer.h"

#include "public/bridge/consolevariablebridge.h"
#include "libultraship/libultra/controller.h"
#include "Context.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <ImGui/imgui.h>
#include <spdlog/spdlog.h>
#include <cmath>

#include "../../UIWidgets.hpp"

// Text colors
static ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
static ImVec4 range1Color = ImVec4(1.0f, 0.7f, 0, 1.0f);
static ImVec4 range2Color = ImVec4(0, 1.0f, 0, 1.0f);

static const char* buttonOutlineOptions[4] = { "Always Shown", "Shown Only While Not Pressed",
                                               "Shown Only While Pressed", "Always Hidden" };

static const char* stickModeOptions[3] = { "Always", "While In Use", "Never" };

static Color_RGBA8 vec2Color(ImVec4 vec) {
    Color_RGBA8 color;
    color.r = vec.x * 255.0;
    color.g = vec.y * 255.0;
    color.b = vec.z * 255.0;
    color.a = vec.w * 255.0;
    return color;
}

static ImVec4 color2Vec(Color_RGBA8 color) {
    return ImVec4(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
}

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
        ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(btnOutlineTexture), size,
                     ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
    }
    // Render button if pressed
    if (state) {
        ImGui::SetCursorPos(pos);
        ImGui::SetNextItemAllowOverlap();
        ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(btnTexture), size,
                     ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
    }
}

void InputViewer::DrawElement() {
    if (CVarGetInteger("gOpenWindows.InputViewer", 0)) {
        static bool sButtonTexturesLoaded = false;
        if (!sButtonTexturesLoaded) {
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage(
                "Input-Viewer-Background", "textures/buttons/InputViewerBackground.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("A-Btn", "textures/buttons/ABtn.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("B-Btn", "textures/buttons/BBtn.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("L-Btn", "textures/buttons/LBtn.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("R-Btn", "textures/buttons/RBtn.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Z-Btn", "textures/buttons/ZBtn.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Start-Btn",
                                                                            "textures/buttons/StartBtn.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Left", "textures/buttons/CLeft.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Right", "textures/buttons/CRight.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Up", "textures/buttons/CUp.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Down", "textures/buttons/CDown.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Analog-Stick",
                                                                            "textures/buttons/AnalogStick.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Left",
                                                                            "textures/buttons/DPadLeft.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Right",
                                                                            "textures/buttons/DPadRight.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Up", "textures/buttons/DPadUp.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Down",
                                                                            "textures/buttons/DPadDown.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Right-Stick",
                                                                            "textures/buttons/RightStick.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("A-Btn Outline",
                                                                            "textures/buttons/ABtnOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("B-Btn Outline",
                                                                            "textures/buttons/BBtnOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("L-Btn Outline",
                                                                            "textures/buttons/LBtnOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("R-Btn Outline",
                                                                            "textures/buttons/RBtnOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Z-Btn Outline",
                                                                            "textures/buttons/ZBtnOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Start-Btn Outline",
                                                                            "textures/buttons/StartBtnOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Left Outline",
                                                                            "textures/buttons/CLeftOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Right Outline",
                                                                            "textures/buttons/CRightOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Up Outline",
                                                                            "textures/buttons/CUpOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("C-Down Outline",
                                                                            "textures/buttons/CDownOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Analog-Stick Outline",
                                                                            "textures/buttons/AnalogStickOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Left Outline",
                                                                            "textures/buttons/DPadLeftOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Right Outline",
                                                                            "textures/buttons/DPadRightOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Up Outline",
                                                                            "textures/buttons/DPadUpOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Dpad-Down Outline",
                                                                            "textures/buttons/DPadDownOutline.png");
            LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Right-Stick Outline",
                                                                            "textures/buttons/RightStickOutline.png");
            sButtonTexturesLoaded = true;
        }

        ImVec2 mainPos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetContentRegionAvail();

#ifdef __WIIU__
        const float scale = CVarGetFloat("gInputViewer.Scale", 1.0f) * 2.0f;
#else
        const float scale = CVarGetFloat("gInputViewer.Scale", 1.0f);
#endif
        const int showAnalogAngles = CVarGetInteger("gInputViewer.AnalogAngles.Enabled", 0);
        const int buttonOutlineMode = CVarGetInteger("gInputViewer.ButtonOutlineMode", BUTTON_OUTLINE_NOT_PRESSED);

        ImVec2 bgSize = LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureSize("Input-Viewer-Background");
        ImVec2 scaledBGSize = ImVec2(bgSize.x * scale, bgSize.y * scale);

        ImGui::SetNextWindowSize(ImVec2(
            scaledBGSize.x + 20,
            scaledBGSize.y +
                (showAnalogAngles ? ImGui::CalcTextSize("X").y : 0) * scale * CVarGetFloat("gInputViewer.AnalogAngles.Scale", 1.0f) + 20));
        ImGui::SetNextWindowContentSize(
            ImVec2(scaledBGSize.x, scaledBGSize.y + (showAnalogAngles ? 15 : 0) * scale *
                                                          CVarGetFloat("gInputViewer.AnalogAngles.Scale", 1.0f)));
        ImGui::SetNextWindowPos(
            ImVec2(mainPos.x + size.x - scaledBGSize.x - 30, mainPos.y + size.y - scaledBGSize.y - 30),
            ImGuiCond_FirstUseEver);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

        OSContPad* pads = LUS::Context::GetInstance()->GetControlDeck()->GetPads();

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                                       ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground |
                                       ImGuiWindowFlags_NoFocusOnAppearing;

        if (!CVarGetInteger("gInputViewer.EnableDragging", 1)) {
            windowFlags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
        }

        if (pads != nullptr && ImGui::Begin("Input Viewer", nullptr, windowFlags)) {
            ImGui::SetCursorPos(ImVec2(10, 10));
            const ImVec2 aPos = ImGui::GetCursorPos();

            if (CVarGetInteger("gInputViewer.ShowBackground", 1)) {
                ImGui::SetNextItemAllowOverlap();
                // Background
                ImGui::Image(
                    LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Input-Viewer-Background"),
                    scaledBGSize, ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
            }

            // A/B
            if (CVarGetInteger("gInputViewer.BBtn", 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("B-Btn", "B-Btn Outline", pads[0].button & BTN_B, scaledBGSize, buttonOutlineMode);
            }
            if (CVarGetInteger("gInputViewer.ABtn", 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("A-Btn", "A-Btn Outline", pads[0].button & BTN_A, scaledBGSize, buttonOutlineMode);
            }

            // C buttons
            if (CVarGetInteger("gInputViewer.CUp", 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("C-Up", "C-Up Outline", pads[0].button & BTN_CUP, scaledBGSize, buttonOutlineMode);
            }
            if (CVarGetInteger("gInputViewer.CLeft", 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("C-Left", "C-Left Outline", pads[0].button & BTN_CLEFT, scaledBGSize, buttonOutlineMode);
            }
            if (CVarGetInteger("gInputViewer.CRight", 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("C-Right", "C-Right Outline", pads[0].button & BTN_CRIGHT, scaledBGSize,
                             buttonOutlineMode);
            }
            if (CVarGetInteger("gInputViewer.CDown", 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("C-Down", "C-Down Outline", pads[0].button & BTN_CDOWN, scaledBGSize, buttonOutlineMode);
            }

            // L/R/Z
            if (CVarGetInteger("gInputViewer.LBtn", 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("L-Btn", "L-Btn Outline", pads[0].button & BTN_L, scaledBGSize, buttonOutlineMode);
            }
            if (CVarGetInteger("gInputViewer.RBtn", 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("R-Btn", "R-Btn Outline", pads[0].button & BTN_R, scaledBGSize, buttonOutlineMode);
            }
            if (CVarGetInteger("gInputViewer.ZBtn", 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Z-Btn", "Z-Btn Outline", pads[0].button & BTN_Z, scaledBGSize, buttonOutlineMode);
            }

            // Start
            if (CVarGetInteger("gInputViewer.StartBtn", 1)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Start-Btn", "Start-Btn Outline", pads[0].button & BTN_START, scaledBGSize,
                             buttonOutlineMode);
            }

            // Dpad
            if (CVarGetInteger("gInputViewer.Dpad", 0)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Dpad-Left", "Dpad-Left Outline", pads[0].button & BTN_DLEFT, scaledBGSize,
                             buttonOutlineMode);
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Dpad-Right", "Dpad-Right Outline", pads[0].button & BTN_DRIGHT, scaledBGSize,
                             buttonOutlineMode);
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Dpad-Up", "Dpad-Up Outline", pads[0].button & BTN_DUP, scaledBGSize, buttonOutlineMode);
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                RenderButton("Dpad-Down", "Dpad-Down Outline", pads[0].button & BTN_DDOWN, scaledBGSize,
                             buttonOutlineMode);
            }

            const bool analogStickIsInDeadzone = !pads[0].stick_x && !pads[0].stick_y;
            const bool rightStickIsInDeadzone = !pads[0].right_stick_x && !pads[0].right_stick_y;

            // Analog Stick
            const int analogOutlineMode =
                CVarGetInteger("gInputViewer.AnalogStick.OutlineMode", STICK_MODE_ALWAYS_SHOWN);
            const float maxStickDistance = CVarGetInteger("gInputViewer.AnalogStick.Movement", 12);
            if (analogOutlineMode == STICK_MODE_ALWAYS_SHOWN ||
                (analogOutlineMode == STICK_MODE_HIDDEN_IN_DEADZONE && !analogStickIsInDeadzone)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                ImGui::Image(
                    LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Analog-Stick Outline"),
                    scaledBGSize, ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
            }
            const int analogStickMode =
                CVarGetInteger("gInputViewer.AnalogStick.VisibilityMode", STICK_MODE_ALWAYS_SHOWN);
            if (analogStickMode == STICK_MODE_ALWAYS_SHOWN ||
                (analogStickMode == STICK_MODE_HIDDEN_IN_DEADZONE && !analogStickIsInDeadzone)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(
                    ImVec2(aPos.x + maxStickDistance * ((float)(pads[0].stick_x) / MAX_AXIS_RANGE) * scale,
                           aPos.y - maxStickDistance * ((float)(pads[0].stick_y) / MAX_AXIS_RANGE) * scale));
                ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Analog-Stick"),
                             scaledBGSize, ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
            }

            // Right Stick
            const float maxRightStickDistance = CVarGetInteger("gInputViewer.RightStick.Movement", 7);
            const int rightOutlineMode =
                CVarGetInteger("gInputViewer.RightStick.OutlineMode", STICK_MODE_ALWAYS_HIDDEN);
            if (rightOutlineMode == STICK_MODE_ALWAYS_SHOWN ||
                (rightOutlineMode == STICK_MODE_HIDDEN_IN_DEADZONE && !rightStickIsInDeadzone)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(aPos);
                ImGui::Image(
                    LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Right-Stick Outline"),
                    scaledBGSize, ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
            }
            const int rightStickMode =
                CVarGetInteger("gInputViewer.RightStick.VisibilityMode", STICK_MODE_ALWAYS_HIDDEN);
            if (rightStickMode == STICK_MODE_ALWAYS_SHOWN ||
                (rightStickMode == STICK_MODE_HIDDEN_IN_DEADZONE && !rightStickIsInDeadzone)) {
                ImGui::SetNextItemAllowOverlap();
                ImGui::SetCursorPos(
                    ImVec2(aPos.x + maxRightStickDistance * ((float)(pads[0].right_stick_x) / MAX_AXIS_RANGE) * scale,
                           aPos.y - maxRightStickDistance * ((float)(pads[0].right_stick_y) / MAX_AXIS_RANGE) * scale));
                ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Right-Stick"),
                             scaledBGSize, ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, 255));
            }

            // Analog stick angle text
            if (showAnalogAngles) {
                ImGui::SetCursorPos(ImVec2(aPos.x + 10 + CVarGetInteger("gInputViewer.AnalogAngles.Offset", 0) * scale,
                                           scaledBGSize.y + aPos.y + 10));
                // Scale font with input viewer scale
                float oldFontScale = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= scale * CVarGetFloat("gInputViewer.AnalogAngles.Scale", 1.0f);
                ImGui::PushFont(ImGui::GetFont());

                // Calculate polar R coordinate from X and Y angles, squared to avoid sqrt
                const float rSquared = pads[0].stick_x * pads[0].stick_x + pads[0].stick_y * pads[0].stick_y;

                // ESS range
                const int range1Min = CVarGetInteger("gInputViewer.AnalogAngles.Range1.Min", 8);
                const int range1Max = CVarGetInteger("gInputViewer.AnalogAngles.Range1.Max", 27);
                // Walking speed range
                const int range2Min = CVarGetInteger("gInputViewer.AnalogAngles.Range2.Min", 27);
                const int range2Max = CVarGetInteger("gInputViewer.AnalogAngles.Range2.Max", 62);

                // Push color based on angle ranges
                if (CVarGetInteger("gInputViewer.AnalogAngles.Range1.Enabled", 0) &&
                    (rSquared >= (range1Min * range1Min)) && (rSquared < (range1Max * range1Max))) {
                    ImGui::PushStyleColor(
                        ImGuiCol_Text,
                        color2Vec(CVarGetColor("gInputViewer.AnalogAngles.Range1.Color", vec2Color(range1Color))));
                } else if (CVarGetInteger("gInputViewer.AnalogAngles.Range2.Enabled", 0) &&
                           (rSquared >= (range2Min * range2Min)) && (rSquared < (range2Max * range2Max))) {
                    ImGui::PushStyleColor(
                        ImGuiCol_Text,
                        color2Vec(CVarGetColor("gInputViewer.AnalogAngles.Range2.Color", vec2Color(range2Color))));
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, color2Vec(CVarGetColor("gInputViewer.AnalogAngles.TextColor",
                                                                                vec2Color(textColor))));
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
    ImGui::SetNextWindowSize(ImVec2(450, 525), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Input Viewer Settings", &mIsVisible)) {

        // gInputViewer.Scale
        UIWidgets::EnhancementSliderFloat("Input Viewer Scale: %.2f", "##Input", "gInputViewer.Scale", 0.1f, 5.0f, "",
                                          1.0f, false, true);
        UIWidgets::Tooltip("Sets the on screen size of the input viewer");

        // gInputViewer.EnableDragging
        UIWidgets::EnhancementCheckbox("Enable Dragging", "gInputViewer.EnableDragging", false, "",
                                       UIWidgets::CheckboxGraphics::Checkmark, true);

        UIWidgets::PaddedSeparator(true, true);

        // gInputViewer.ShowBackground
        UIWidgets::EnhancementCheckbox("Show Background Layer", "gInputViewer.ShowBackground", false, "",
                                       UIWidgets::CheckboxGraphics::Checkmark, true);

        UIWidgets::PaddedSeparator(true, true);

        if (ImGui::CollapsingHeader("Buttons")) {
            // gInputViewer.ABtn
            UIWidgets::EnhancementCheckbox("Show A-Button Layers", "gInputViewer.ABtn", false, "",
                                           UIWidgets::CheckboxGraphics::Checkmark, true);
            // gInputViewer.BBtn
            UIWidgets::EnhancementCheckbox("Show B-Button Layers", "gInputViewer.BBtn", false, "",
                                           UIWidgets::CheckboxGraphics::Checkmark, true);
            // gInputViewer.CUp
            UIWidgets::EnhancementCheckbox("Show C-Up Layers", "gInputViewer.CUp", false, "",
                                           UIWidgets::CheckboxGraphics::Checkmark, true);
            // gInputViewer.CRight
            UIWidgets::EnhancementCheckbox("Show C-Right Layers", "gInputViewer.CRight", false, "",
                                           UIWidgets::CheckboxGraphics::Checkmark, true);
            // gInputViewer.CDown
            UIWidgets::EnhancementCheckbox("Show C-Down Layers", "gInputViewer.CDown", false, "",
                                           UIWidgets::CheckboxGraphics::Checkmark, true);
            // gInputViewer.CLeft
            UIWidgets::EnhancementCheckbox("Show C-Left Layers", "gInputViewer.CLeft", false, "",
                                           UIWidgets::CheckboxGraphics::Checkmark, true);
            // gInputViewer.LBtn
            UIWidgets::EnhancementCheckbox("Show L-Button Layers", "gInputViewer.LBtn", false, "",
                                           UIWidgets::CheckboxGraphics::Checkmark, true);
            // gInputViewer.RBtn
            UIWidgets::EnhancementCheckbox("Show R-Button Layers", "gInputViewer.RBtn", false, "",
                                           UIWidgets::CheckboxGraphics::Checkmark, true);
            // gInputViewer.ZBtn
            UIWidgets::EnhancementCheckbox("Show Z-Button Layers", "gInputViewer.ZBtn", false, "",
                                           UIWidgets::CheckboxGraphics::Checkmark, true);
            // gInputViewer.StartBtn
            UIWidgets::EnhancementCheckbox("Show Start Button Layers", "gInputViewer.StartBtn", false, "",
                                           UIWidgets::CheckboxGraphics::Checkmark, true);
            // gInputViewer.Dpad
            UIWidgets::EnhancementCheckbox("Show D-Pad Layers", "gInputViewer.Dpad", false, "",
                                           UIWidgets::CheckboxGraphics::Checkmark, false);

            // gInputViewer.ButtonOutlineMode
            UIWidgets::PaddedText("Button Outlines/Backgrounds", true, false);
            UIWidgets::EnhancementCombobox("gInputViewer.ButtonOutlineMode", buttonOutlineOptions,
                                           BUTTON_OUTLINE_NOT_PRESSED);
            UIWidgets::Tooltip(
                "Sets the desired visibility behavior for the button outline/background layers. Useful for "
                "custom input viewers.");

            UIWidgets::PaddedSeparator(true, true);
        }

        if (ImGui::CollapsingHeader("Analog Stick")) {
            // gInputViewer.AnalogStick.VisibilityMode
            UIWidgets::PaddedText("Analog Stick Visibility", true, false);
            UIWidgets::EnhancementCombobox("gInputViewer.AnalogStick.VisibilityMode", stickModeOptions,
                                           STICK_MODE_ALWAYS_SHOWN);
            UIWidgets::Tooltip(
                "Determines the conditions under which the moving layer of the analog stick texture is visible.");

            // gInputViewer.AnalogStick.OutlineMode
            UIWidgets::PaddedText("Analog Stick Outline/Background Visibility", true, false);
            UIWidgets::EnhancementCombobox("gInputViewer.AnalogStick.OutlineMode", stickModeOptions,
                                           STICK_MODE_ALWAYS_SHOWN);
            UIWidgets::Tooltip(
                "Determines the conditions under which the analog stick outline/background texture is visible.");

            // gInputViewer.AnalogStick.Movement
            UIWidgets::EnhancementSliderInt("Analog Stick Movement: %dpx", "##AnalogMovement",
                                            "gInputViewer.AnalogStick.Movement", 0, 200, "", 12, true);
            UIWidgets::Tooltip(
                "Sets the distance to move the analog stick in the input viewer. Useful for custom input viewers.");
            UIWidgets::PaddedSeparator(true, true);
        }

        if (ImGui::CollapsingHeader("Additional (\"Right\") Stick")) {
            // gInputViewer.RightStick.VisibilityMode
            UIWidgets::PaddedText("Right Stick Visibility", true, false);
            UIWidgets::EnhancementCombobox("gInputViewer.RightStick.VisibilityMode", stickModeOptions,
                                           STICK_MODE_HIDDEN_IN_DEADZONE);
            UIWidgets::Tooltip(
                "Determines the conditions under which the moving layer of the right stick texture is visible.");

            // gInputViewer.RightStick.OutlineMode
            UIWidgets::PaddedText("Right Stick Outline/Background Visibility", true, false);
            UIWidgets::EnhancementCombobox("gInputViewer.RightStick.OutlineMode", stickModeOptions,
                                           STICK_MODE_HIDDEN_IN_DEADZONE);
            UIWidgets::Tooltip(
                "Determines the conditions under which the right stick outline/background texture is visible.");

            // gInputViewer.RightStick.Movement
            UIWidgets::EnhancementSliderInt("Right Stick Movement: %dpx", "##RightMovement",
                                            "gInputViewer.RightStick.Movement", 0, 200, "", 7, true);
            UIWidgets::Tooltip(
                "Sets the distance to move the right stick in the input viewer. Useful for custom input viewers.");
            UIWidgets::PaddedSeparator(true, true);
        }

        if (ImGui::CollapsingHeader("Analog Angle Values")) {
            // gAnalogAngles
            UIWidgets::EnhancementCheckbox("Show Analog Stick Angle Values", "gInputViewer.AnalogAngles.Enabled");
            UIWidgets::Tooltip("Displays analog stick angle values in the input viewer");
            if (CVarGetInteger("gInputViewer.AnalogAngles.Enabled", 0)) {
                // gInputViewer.AnalogAngles.TextColor
                if (ImGui::ColorEdit4("Text Color", (float*)&textColor)) {
                    CVarSetColor("gInputViewer.AnalogAngles.TextColor", vec2Color(textColor));
                }
                // gAnalogAngleScale
                UIWidgets::EnhancementSliderFloat("Angle Text Scale: %.2f%%", "##AnalogAngleScale",
                                                  "gInputViewer.AnalogAngles.Scale", 0.1f, 5.0f, "", 1.0f, true, true);
                // gInputViewer.AnalogAngles.Offset
                UIWidgets::EnhancementSliderInt("Angle Text Offset: %dpx", "##AnalogAngleOffset",
                                                "gInputViewer.AnalogAngles.Offset", 0, 400, "", 0, true);
                UIWidgets::PaddedSeparator(true, true);
                // gInputViewer.AnalogAngles.Range1.Enabled
                UIWidgets::EnhancementCheckbox("Highlight ESS Position", "gInputViewer.AnalogAngles.Range1.Enabled");
                UIWidgets::Tooltip(
                    "Highlights the angle value text when the analog stick is in ESS position (on flat ground)");
                if (CVarGetInteger("gInputViewer.AnalogAngles.Range1.Enabled", 0)) {
                    // gInputViewer.AnalogAngles.Range1.Color
                    if (ImGui::ColorEdit4("ESS Color", (float*)&range1Color)) {
                        CVarSetColor("gInputViewer.AnalogAngles.Range1.Color", vec2Color(range1Color));
                    }
                }

                UIWidgets::PaddedSeparator(true, true);
                // gInputViewer.AnalogAngles.Range2.Enabled
                UIWidgets::EnhancementCheckbox("Highlight Walking Speed Angles",
                                               "gInputViewer.AnalogAngles.Range2.Enabled");
                UIWidgets::Tooltip("Highlights the angle value text when the analog stick is at an angle that would "
                                   "produce a walking speed (on flat ground)\n\n"
                                   "Useful for 1.0 Empty Jumpslash Quick Put Away");
                if (CVarGetInteger("gInputViewer.AnalogAngles.Range2.Enabled", 0)) {
                    // gInputViewer.AnalogAngles.Range2.Color
                    if (ImGui::ColorEdit4("Walking Speed Color", (float*)&range2Color)) {
                        CVarSetColor("gInputViewer.AnalogAngles.Range2.Color", vec2Color(range2Color));
                    }
                }
            }
        }

        ImGui::End();
    }
}