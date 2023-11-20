#include "SohInputEditorWindow.h"
#include <Utils/StringHelper.h>
#include "soh/OTRGlobals.h"
#include "../../UIWidgets.hpp"
#include "z64.h"
#ifndef __WIIU__
#include "controller/controldevice/controller/mapping/sdl/SDLAxisDirectionToButtonMapping.h"
#endif

#define SCALE_IMGUI_SIZE(value) ((value / 13.0f) * ImGui::GetFontSize())

SohInputEditorWindow::~SohInputEditorWindow() {
}

void SohInputEditorWindow::InitElement() {
    mGameInputBlockTimer = INT32_MAX;
    mMappingInputBlockTimer = INT32_MAX;
    mRumbleTimer = INT32_MAX;
    mRumbleMappingToTest = nullptr;
    mInputEditorPopupOpen = false;

    mButtonsBitmasks = { BTN_A, BTN_B, BTN_START, BTN_L, BTN_R, BTN_Z, BTN_CUP, BTN_CDOWN, BTN_CLEFT, BTN_CRIGHT };
    mDpadBitmasks = { BTN_DUP, BTN_DDOWN, BTN_DLEFT, BTN_DRIGHT };
    mModifierButtonsBitmasks = { BTN_MODIFIER1, BTN_MODIFIER2 };
}

#define INPUT_EDITOR_WINDOW_GAME_INPUT_BLOCK_ID 95237929
void SohInputEditorWindow::UpdateElement() {
    if (mRumbleTimer != INT32_MAX) {
        mRumbleTimer--;
        if (mRumbleMappingToTest != nullptr) {
            mRumbleMappingToTest->StartRumble();
        }
        if (mRumbleTimer <= 0) {
            if (mRumbleMappingToTest != nullptr) {
                mRumbleMappingToTest->StopRumble();
            }
            mRumbleTimer = INT32_MAX;
            mRumbleMappingToTest = nullptr;
        }
    }

    if (mInputEditorPopupOpen && ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId)) {
        LUS::Context::GetInstance()->GetControlDeck()->BlockGameInput(INPUT_EDITOR_WINDOW_GAME_INPUT_BLOCK_ID);

        // continue to block input for a third of a second after getting the mapping
        mGameInputBlockTimer = ImGui::GetIO().Framerate / 3;

        if (mMappingInputBlockTimer != INT32_MAX) {
            mMappingInputBlockTimer--;
            if (mMappingInputBlockTimer <= 0) {
                mMappingInputBlockTimer = INT32_MAX;
            }
        }

        LUS::Context::GetInstance()->GetWindow()->GetGui()->BlockImGuiGamepadNavigation();
    } else {
        if (mGameInputBlockTimer != INT32_MAX) {
            mGameInputBlockTimer--;
            if (mGameInputBlockTimer <= 0) {
                LUS::Context::GetInstance()->GetControlDeck()->UnblockGameInput(
                    INPUT_EDITOR_WINDOW_GAME_INPUT_BLOCK_ID);
                mGameInputBlockTimer = INT32_MAX;
            }
        }

        if (LUS::Context::GetInstance()->GetWindow()->GetGui()->ImGuiGamepadNavigationEnabled()) {
            mMappingInputBlockTimer = ImGui::GetIO().Framerate / 3;
        } else {
            mMappingInputBlockTimer = INT32_MAX;
        }

        LUS::Context::GetInstance()->GetWindow()->GetGui()->UnblockImGuiGamepadNavigation();
    }
}

void SohInputEditorWindow::DrawAnalogPreview(const char* label, ImVec2 stick, float deadzone, bool gyro) {
    ImGui::BeginChild(label, ImVec2(gyro ? SCALE_IMGUI_SIZE(78) : SCALE_IMGUI_SIZE(96), SCALE_IMGUI_SIZE(85)), false);
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + gyro ? SCALE_IMGUI_SIZE(10) : SCALE_IMGUI_SIZE(18),
                               ImGui::GetCursorPos().y + gyro ? SCALE_IMGUI_SIZE(10) : 0));
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    const ImVec2 cursorScreenPosition = ImGui::GetCursorScreenPos();

    // Draw the border box
    float borderSquareLeft = cursorScreenPosition.x + SCALE_IMGUI_SIZE(2.0f);
    float borderSquareTop = cursorScreenPosition.y + SCALE_IMGUI_SIZE(2.0f);
    float borderSquareSize = SCALE_IMGUI_SIZE(65.0f);
    drawList->AddRect(ImVec2(borderSquareLeft, borderSquareTop),
                      ImVec2(borderSquareLeft + borderSquareSize, borderSquareTop + borderSquareSize),
                      ImColor(100, 100, 100, 255), 0.0f, 0, 1.5f);

    // Draw the gate background
    float cardinalRadius = SCALE_IMGUI_SIZE(22.5f);
    float diagonalRadius = SCALE_IMGUI_SIZE(22.5f * (69.0f / 85.0f));

    ImVec2 joystickCenterpoint = ImVec2(cursorScreenPosition.x + cardinalRadius + SCALE_IMGUI_SIZE(12),
                                        cursorScreenPosition.y + cardinalRadius + SCALE_IMGUI_SIZE(11));
    drawList->AddQuadFilled(joystickCenterpoint,
                            ImVec2(joystickCenterpoint.x - diagonalRadius, joystickCenterpoint.y + diagonalRadius),
                            ImVec2(joystickCenterpoint.x, joystickCenterpoint.y + cardinalRadius),
                            ImVec2(joystickCenterpoint.x + diagonalRadius, joystickCenterpoint.y + diagonalRadius),
                            ImColor(130, 130, 130, 255));
    drawList->AddQuadFilled(joystickCenterpoint,
                            ImVec2(joystickCenterpoint.x + diagonalRadius, joystickCenterpoint.y + diagonalRadius),
                            ImVec2(joystickCenterpoint.x + cardinalRadius, joystickCenterpoint.y),
                            ImVec2(joystickCenterpoint.x + diagonalRadius, joystickCenterpoint.y - diagonalRadius),
                            ImColor(130, 130, 130, 255));
    drawList->AddQuadFilled(joystickCenterpoint,
                            ImVec2(joystickCenterpoint.x + diagonalRadius, joystickCenterpoint.y - diagonalRadius),
                            ImVec2(joystickCenterpoint.x, joystickCenterpoint.y - cardinalRadius),
                            ImVec2(joystickCenterpoint.x - diagonalRadius, joystickCenterpoint.y - diagonalRadius),
                            ImColor(130, 130, 130, 255));
    drawList->AddQuadFilled(joystickCenterpoint,
                            ImVec2(joystickCenterpoint.x - diagonalRadius, joystickCenterpoint.y - diagonalRadius),
                            ImVec2(joystickCenterpoint.x - cardinalRadius, joystickCenterpoint.y),
                            ImVec2(joystickCenterpoint.x - diagonalRadius, joystickCenterpoint.y + diagonalRadius),
                            ImColor(130, 130, 130, 255));

    // Draw the joystick position indicator
    ImVec2 joystickIndicatorDistanceFromCenter = ImVec2(0, 0);
    if ((stick.x * stick.x + stick.y * stick.y) > (deadzone * deadzone)) {
        joystickIndicatorDistanceFromCenter =
            ImVec2((stick.x * (cardinalRadius / 85.0f)), -(stick.y * (cardinalRadius / 85.0f)));
    }
    float indicatorRadius = SCALE_IMGUI_SIZE(5.0f);
    drawList->AddCircleFilled(ImVec2(joystickCenterpoint.x + joystickIndicatorDistanceFromCenter.x,
                                     joystickCenterpoint.y + joystickIndicatorDistanceFromCenter.y),
                              indicatorRadius, ImColor(34, 51, 76, 255), 7);

    if (!gyro) {
        ImGui::SetCursorPos(
            ImVec2(ImGui::GetCursorPos().x - SCALE_IMGUI_SIZE(8), ImGui::GetCursorPos().y + SCALE_IMGUI_SIZE(72)));
        ImGui::Text("X:%3d, Y:%3d", static_cast<int32_t>(stick.x), static_cast<int32_t>(stick.y));
    }
    ImGui::EndChild();
}

#define CHIP_COLOR_N64_GREY ImVec4(0.4f, 0.4f, 0.4f, 1.0f)
#define CHIP_COLOR_N64_BLUE ImVec4(0.176f, 0.176f, 0.5f, 1.0f)
#define CHIP_COLOR_N64_GREEN ImVec4(0.0f, 0.294f, 0.0f, 1.0f)
#define CHIP_COLOR_N64_YELLOW ImVec4(0.5f, 0.314f, 0.0f, 1.0f)
#define CHIP_COLOR_N64_RED ImVec4(0.392f, 0.0f, 0.0f, 1.0f)

#define BUTTON_COLOR_KEYBOARD_BEIGE ImVec4(0.651f, 0.482f, 0.357f, 0.5f)
#define BUTTON_COLOR_KEYBOARD_BEIGE_HOVERED ImVec4(0.651f, 0.482f, 0.357f, 1.0f)

#define BUTTON_COLOR_GAMEPAD_BLUE ImVec4(0.0f, 0.255f, 0.976f, 0.5f)
#define BUTTON_COLOR_GAMEPAD_BLUE_HOVERED ImVec4(0.0f, 0.255f, 0.976f, 1.0f)

#define BUTTON_COLOR_GAMEPAD_RED ImVec4(0.976f, 0.0f, 0.094f, 0.5f)
#define BUTTON_COLOR_GAMEPAD_RED_HOVERED ImVec4(0.976f, 0.0f, 0.094f, 1.0f)

#define BUTTON_COLOR_GAMEPAD_ORANGE ImVec4(0.976f, 0.376f, 0.0f, 0.5f)
#define BUTTON_COLOR_GAMEPAD_ORANGE_HOVERED ImVec4(0.976f, 0.376f, 0.0f, 1.0f)

#define BUTTON_COLOR_GAMEPAD_GREEN ImVec4(0.0f, 0.5f, 0.0f, 0.5f)
#define BUTTON_COLOR_GAMEPAD_GREEN_HOVERED ImVec4(0.0f, 0.5f, 0.0f, 1.0f)

#define BUTTON_COLOR_GAMEPAD_PURPLE ImVec4(0.431f, 0.369f, 0.706f, 0.5f)
#define BUTTON_COLOR_GAMEPAD_PURPLE_HOVERED ImVec4(0.431f, 0.369f, 0.706f, 1.0f)

void SohInputEditorWindow::GetButtonColorsForLUSDeviceIndex(LUS::LUSDeviceIndex lusIndex, ImVec4& buttonColor,
                                                            ImVec4& buttonHoveredColor) {
    switch (lusIndex) {
        case LUS::LUSDeviceIndex::Keyboard:
            buttonColor = BUTTON_COLOR_KEYBOARD_BEIGE;
            buttonHoveredColor = BUTTON_COLOR_KEYBOARD_BEIGE_HOVERED;
            break;
        case LUS::LUSDeviceIndex::Blue:
            buttonColor = BUTTON_COLOR_GAMEPAD_BLUE;
            buttonHoveredColor = BUTTON_COLOR_GAMEPAD_BLUE_HOVERED;
            break;
        case LUS::LUSDeviceIndex::Red:
            buttonColor = BUTTON_COLOR_GAMEPAD_RED;
            buttonHoveredColor = BUTTON_COLOR_GAMEPAD_RED_HOVERED;
            break;
        case LUS::LUSDeviceIndex::Orange:
            buttonColor = BUTTON_COLOR_GAMEPAD_ORANGE;
            buttonHoveredColor = BUTTON_COLOR_GAMEPAD_ORANGE_HOVERED;
            break;
        case LUS::LUSDeviceIndex::Green:
            buttonColor = BUTTON_COLOR_GAMEPAD_GREEN;
            buttonHoveredColor = BUTTON_COLOR_GAMEPAD_GREEN_HOVERED;
            break;
        default:
            buttonColor = BUTTON_COLOR_GAMEPAD_PURPLE;
            buttonHoveredColor = BUTTON_COLOR_GAMEPAD_PURPLE_HOVERED;
    }
}

void SohInputEditorWindow::DrawInputChip(const char* buttonName, ImVec4 color = CHIP_COLOR_N64_GREY) {
    ImGui::BeginDisabled();
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::Button(buttonName, ImVec2(SCALE_IMGUI_SIZE(50.0f), 0));
    ImGui::PopStyleColor();
    ImGui::EndDisabled();
}

void SohInputEditorWindow::DrawButtonLineAddMappingButton(uint8_t port, uint16_t bitmask) {
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
    auto popupId = StringHelper::Sprintf("addButtonMappingPopup##%d-%d", port, bitmask);
    if (ImGui::Button(StringHelper::Sprintf("%s###addButtonMappingButton%d-%d", ICON_FA_PLUS, port, bitmask).c_str(),
                      ImVec2(SCALE_IMGUI_SIZE(20.0f), 0.0f))) {
        ImGui::OpenPopup(popupId.c_str());
    };
    ImGui::PopStyleVar();

    if (ImGui::BeginPopup(popupId.c_str())) {
        mInputEditorPopupOpen = true;
        ImGui::Text("Press any button,\nmove any axis,\nor press any key\nto add mapping");
        if (ImGui::Button("Cancel")) {
            mInputEditorPopupOpen = false;
            ImGui::CloseCurrentPopup();
        }
        // todo: figure out why optional params (using id = "" in the definition) wasn't working
        if (mMappingInputBlockTimer == INT32_MAX && LUS::Context::GetInstance()
                                                        ->GetControlDeck()
                                                        ->GetControllerByPort(port)
                                                        ->GetButton(bitmask)
                                                        ->AddOrEditButtonMappingFromRawPress(bitmask, "")) {
            mInputEditorPopupOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void SohInputEditorWindow::DrawButtonLineEditMappingButton(uint8_t port, uint16_t bitmask, std::string id) {
    auto mapping = LUS::Context::GetInstance()
                       ->GetControlDeck()
                       ->GetControllerByPort(port)
                       ->GetButton(bitmask)
                       ->GetButtonMappingById(id);
    if (mapping == nullptr) {
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    std::string icon = "";
    switch (mapping->GetMappingType()) {
        case MAPPING_TYPE_GAMEPAD:
            icon = ICON_FA_GAMEPAD;
            break;
        case MAPPING_TYPE_KEYBOARD:
            icon = ICON_FA_KEYBOARD_O;
            break;
        case MAPPING_TYPE_UNKNOWN:
            icon = ICON_FA_BUG;
            break;
    }
    auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
    auto physicalInputDisplayName =
        StringHelper::Sprintf("%s %s", icon.c_str(), mapping->GetPhysicalInputName().c_str());
    GetButtonColorsForLUSDeviceIndex(mapping->GetLUSDeviceIndex(), buttonColor, buttonHoveredColor);
    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
    auto popupId = StringHelper::Sprintf("editButtonMappingPopup##%s", id.c_str());
    if (ImGui::Button(
            StringHelper::Sprintf("%s###editButtonMappingButton%s", physicalInputDisplayName.c_str(), id.c_str())
                .c_str(),
            ImVec2(ImGui::CalcTextSize(physicalInputDisplayName.c_str()).x + SCALE_IMGUI_SIZE(12.0f), 0.0f))) {
        ImGui::OpenPopup(popupId.c_str());
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay)) {
        ImGui::SetTooltip(mapping->GetPhysicalDeviceName().c_str());
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    if (ImGui::BeginPopup(popupId.c_str())) {
        mInputEditorPopupOpen = true;
        ImGui::Text("Press any button,\nmove any axis,\nor press any key\nto edit mapping");
        if (ImGui::Button("Cancel")) {
            mInputEditorPopupOpen = false;
            ImGui::CloseCurrentPopup();
        }
        if (mMappingInputBlockTimer == INT32_MAX && LUS::Context::GetInstance()
                                                        ->GetControlDeck()
                                                        ->GetControllerByPort(port)
                                                        ->GetButton(bitmask)
                                                        ->AddOrEditButtonMappingFromRawPress(bitmask, id)) {
            mInputEditorPopupOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    ImGui::SameLine(0, 0);

#ifndef __WIIU__
    auto sdlAxisDirectionToButtonMapping = std::dynamic_pointer_cast<LUS::SDLAxisDirectionToButtonMapping>(mapping);
    auto indexMapping = LUS::Context::GetInstance()
                            ->GetControlDeck()
                            ->GetDeviceIndexMappingManager()
                            ->GetDeviceIndexMappingFromLUSDeviceIndex(mapping->GetLUSDeviceIndex());
    auto sdlIndexMapping = std::dynamic_pointer_cast<LUS::LUSDeviceIndexToSDLDeviceIndexMapping>(indexMapping);

    if (sdlIndexMapping != nullptr && sdlAxisDirectionToButtonMapping != nullptr) {
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
        auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        GetButtonColorsForLUSDeviceIndex(mapping->GetLUSDeviceIndex(), buttonColor, buttonHoveredColor);
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
        auto popupId = StringHelper::Sprintf("editAxisThresholdPopup##%s", id.c_str());
        if (ImGui::Button(StringHelper::Sprintf("%s###editAxisThresholdButton%s", ICON_FA_COG, id.c_str()).c_str(),
                          ImVec2(ImGui::CalcTextSize(ICON_FA_COG).x + SCALE_IMGUI_SIZE(10.0f), 0.0f))) {
            ImGui::OpenPopup(popupId.c_str());
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay)) {
            ImGui::SetTooltip("Edit axis threshold");
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        if (ImGui::BeginPopup(popupId.c_str())) {
            mInputEditorPopupOpen = true;
            ImGui::Text("Axis Threshold\n\nThe extent to which the joystick\nmust be moved or the trigger\npressed to "
                        "initiate the assigned\nbutton action.\n\n");

            if (sdlAxisDirectionToButtonMapping->AxisIsStick()) {
                ImGui::Text("Stick axis threshold:");

                int32_t stickAxisThreshold = sdlIndexMapping->GetStickAxisThresholdPercentage();
                if (stickAxisThreshold == 0) {
                    ImGui::BeginDisabled();
                }
                ImGui::PushButtonRepeat(true);
                if (ImGui::Button(StringHelper::Sprintf("-##Stick Axis Threshold%s", id.c_str()).c_str())) {
                    sdlIndexMapping->SetStickAxisThresholdPercentage(stickAxisThreshold - 1);
                    sdlIndexMapping->SaveToConfig();
                }
                ImGui::PopButtonRepeat();
                if (stickAxisThreshold == 0) {
                    ImGui::EndDisabled();
                }
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::SetNextItemWidth(SCALE_IMGUI_SIZE(160.0f));
                if (ImGui::SliderInt(StringHelper::Sprintf("##Stick Axis Threshold%s", id.c_str()).c_str(),
                                     &stickAxisThreshold, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp)) {
                    sdlIndexMapping->SetStickAxisThresholdPercentage(stickAxisThreshold);
                    sdlIndexMapping->SaveToConfig();
                }
                ImGui::SameLine(0.0f, 0.0f);
                if (stickAxisThreshold == 100) {
                    ImGui::BeginDisabled();
                }
                ImGui::PushButtonRepeat(true);
                if (ImGui::Button(StringHelper::Sprintf("+##Stick Axis Threshold%s", id.c_str()).c_str())) {
                    sdlIndexMapping->SetStickAxisThresholdPercentage(stickAxisThreshold + 1);
                    sdlIndexMapping->SaveToConfig();
                }
                ImGui::PopButtonRepeat();
                if (stickAxisThreshold == 100) {
                    ImGui::EndDisabled();
                }
            }

            if (sdlAxisDirectionToButtonMapping->AxisIsTrigger()) {
                ImGui::Text("Trigger axis threshold:");

                int32_t triggerAxisThreshold = sdlIndexMapping->GetTriggerAxisThresholdPercentage();
                if (triggerAxisThreshold == 0) {
                    ImGui::BeginDisabled();
                }
                ImGui::PushButtonRepeat(true);
                if (ImGui::Button(StringHelper::Sprintf("-##Trigger Axis Threshold%s", id.c_str()).c_str())) {
                    sdlIndexMapping->SetTriggerAxisThresholdPercentage(triggerAxisThreshold - 1);
                    sdlIndexMapping->SaveToConfig();
                }
                ImGui::PopButtonRepeat();
                if (triggerAxisThreshold == 0) {
                    ImGui::EndDisabled();
                }
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::SetNextItemWidth(SCALE_IMGUI_SIZE(160.0f));
                if (ImGui::SliderInt(StringHelper::Sprintf("##Trigger Axis Threshold%s", id.c_str()).c_str(),
                                     &triggerAxisThreshold, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp)) {
                    sdlIndexMapping->SetTriggerAxisThresholdPercentage(triggerAxisThreshold);
                    sdlIndexMapping->SaveToConfig();
                }
                ImGui::SameLine(0.0f, 0.0f);
                if (triggerAxisThreshold == 100) {
                    ImGui::BeginDisabled();
                }
                ImGui::PushButtonRepeat(true);
                if (ImGui::Button(StringHelper::Sprintf("+##Trigger Axis Threshold%s", id.c_str()).c_str())) {
                    sdlIndexMapping->SetTriggerAxisThresholdPercentage(triggerAxisThreshold + 1);
                    sdlIndexMapping->SaveToConfig();
                }
                ImGui::PopButtonRepeat();
                if (triggerAxisThreshold == 100) {
                    ImGui::EndDisabled();
                }
            }

            if (ImGui::Button("Close")) {
                mInputEditorPopupOpen = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::PopStyleVar();
        ImGui::SameLine(0, 0);
    }
#endif

    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
    if (ImGui::Button(StringHelper::Sprintf("%s###removeButtonMappingButton%s", ICON_FA_TIMES, id.c_str()).c_str(),
                      ImVec2(ImGui::CalcTextSize(ICON_FA_TIMES).x + SCALE_IMGUI_SIZE(10.0f), 0.0f))) {
        LUS::Context::GetInstance()
            ->GetControlDeck()
            ->GetControllerByPort(port)
            ->GetButton(bitmask)
            ->ClearButtonMapping(id);
    };
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    ImGui::SameLine(0, SCALE_IMGUI_SIZE(4.0f));
}

void SohInputEditorWindow::DrawButtonLine(const char* buttonName, uint8_t port, uint16_t bitmask,
                                          ImVec4 color = CHIP_COLOR_N64_GREY) {
    ImGui::NewLine();
    ImGui::SameLine(SCALE_IMGUI_SIZE(32.0f));
    DrawInputChip(buttonName, color);
    ImGui::SameLine(SCALE_IMGUI_SIZE(86.0f));
    for (auto id : mBitmaskToMappingIds[port][bitmask]) {
        DrawButtonLineEditMappingButton(port, bitmask, id);
    }
    DrawButtonLineAddMappingButton(port, bitmask);
}

void SohInputEditorWindow::DrawStickDirectionLineAddMappingButton(uint8_t port, uint8_t stick,
                                                                  LUS::Direction direction) {
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
    auto popupId = StringHelper::Sprintf("addStickDirectionMappingPopup##%d-%d-%d", port, stick, direction);
    if (ImGui::Button(
            StringHelper::Sprintf("%s###addStickDirectionMappingButton%d-%d-%d", ICON_FA_PLUS, port, stick, direction)
                .c_str(),
            ImVec2(SCALE_IMGUI_SIZE(20.0f), 0.0f))) {
        ImGui::OpenPopup(popupId.c_str());
    };
    ImGui::PopStyleVar();

    if (ImGui::BeginPopup(popupId.c_str())) {
        mInputEditorPopupOpen = true;
        ImGui::Text("Press any button,\nmove any axis,\nor press any key\nto add mapping");
        if (ImGui::Button("Cancel")) {
            mInputEditorPopupOpen = false;
            ImGui::CloseCurrentPopup();
        }
        if (stick == LUS::LEFT) {
            if (mMappingInputBlockTimer == INT32_MAX &&
                LUS::Context::GetInstance()
                    ->GetControlDeck()
                    ->GetControllerByPort(port)
                    ->GetLeftStick()
                    ->AddOrEditAxisDirectionMappingFromRawPress(direction, "")) {
                mInputEditorPopupOpen = false;
                ImGui::CloseCurrentPopup();
            }
        } else {
            if (mMappingInputBlockTimer == INT32_MAX &&
                LUS::Context::GetInstance()
                    ->GetControlDeck()
                    ->GetControllerByPort(port)
                    ->GetRightStick()
                    ->AddOrEditAxisDirectionMappingFromRawPress(direction, "")) {
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
}

void SohInputEditorWindow::DrawStickDirectionLineEditMappingButton(uint8_t port, uint8_t stick,
                                                                   LUS::Direction direction, std::string id) {
    std::shared_ptr<LUS::ControllerAxisDirectionMapping> mapping = nullptr;
    if (stick == LUS::LEFT) {
        mapping = LUS::Context::GetInstance()
                      ->GetControlDeck()
                      ->GetControllerByPort(port)
                      ->GetLeftStick()
                      ->GetAxisDirectionMappingById(direction, id);
    } else {
        mapping = LUS::Context::GetInstance()
                      ->GetControlDeck()
                      ->GetControllerByPort(port)
                      ->GetRightStick()
                      ->GetAxisDirectionMappingById(direction, id);
    }

    if (mapping == nullptr) {
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    std::string icon = "";
    switch (mapping->GetMappingType()) {
        case MAPPING_TYPE_GAMEPAD:
            icon = ICON_FA_GAMEPAD;
            break;
        case MAPPING_TYPE_KEYBOARD:
            icon = ICON_FA_KEYBOARD_O;
            break;
        case MAPPING_TYPE_UNKNOWN:
            icon = ICON_FA_BUG;
            break;
    }
    auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
    auto physicalInputDisplayName =
        StringHelper::Sprintf("%s %s", icon.c_str(), mapping->GetPhysicalInputName().c_str());
    GetButtonColorsForLUSDeviceIndex(mapping->GetLUSDeviceIndex(), buttonColor, buttonHoveredColor);
    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
    auto popupId = StringHelper::Sprintf("editStickDirectionMappingPopup##%s", id.c_str());
    if (ImGui::Button(
            StringHelper::Sprintf("%s###editStickDirectionMappingButton%s", physicalInputDisplayName.c_str(),
                                  id.c_str())
                .c_str(),
            ImVec2(ImGui::CalcTextSize(physicalInputDisplayName.c_str()).x + SCALE_IMGUI_SIZE(12.0f), 0.0f))) {
        ImGui::OpenPopup(popupId.c_str());
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay)) {
        ImGui::SetTooltip(mapping->GetPhysicalDeviceName().c_str());
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    if (ImGui::BeginPopup(popupId.c_str())) {
        mInputEditorPopupOpen = true;
        ImGui::Text("Press any button,\nmove any axis,\nor press any key\nto edit mapping");
        if (ImGui::Button("Cancel")) {
            mInputEditorPopupOpen = false;
            ImGui::CloseCurrentPopup();
        }

        if (stick == LUS::LEFT) {
            if (mMappingInputBlockTimer == INT32_MAX &&
                LUS::Context::GetInstance()
                    ->GetControlDeck()
                    ->GetControllerByPort(port)
                    ->GetLeftStick()
                    ->AddOrEditAxisDirectionMappingFromRawPress(direction, id)) {
                mInputEditorPopupOpen = false;
                ImGui::CloseCurrentPopup();
            }
        } else {
            if (mMappingInputBlockTimer == INT32_MAX &&
                LUS::Context::GetInstance()
                    ->GetControlDeck()
                    ->GetControllerByPort(port)
                    ->GetRightStick()
                    ->AddOrEditAxisDirectionMappingFromRawPress(direction, id)) {
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    ImGui::SameLine(0, 0);
    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
    if (ImGui::Button(
            StringHelper::Sprintf("%s###removeStickDirectionMappingButton%s", ICON_FA_TIMES, id.c_str()).c_str(),
            ImVec2(ImGui::CalcTextSize(ICON_FA_TIMES).x + SCALE_IMGUI_SIZE(10.0f), 0.0f))) {
        if (stick == LUS::LEFT) {
            LUS::Context::GetInstance()
                ->GetControlDeck()
                ->GetControllerByPort(port)
                ->GetLeftStick()
                ->ClearAxisDirectionMapping(direction, id);
        } else {
            LUS::Context::GetInstance()
                ->GetControlDeck()
                ->GetControllerByPort(port)
                ->GetRightStick()
                ->ClearAxisDirectionMapping(direction, id);
        }
    };
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::SameLine(0, SCALE_IMGUI_SIZE(4.0f));
}

void SohInputEditorWindow::DrawStickDirectionLine(const char* axisDirectionName, uint8_t port, uint8_t stick,
                                                  LUS::Direction direction, ImVec4 color = CHIP_COLOR_N64_GREY) {
    ImGui::NewLine();
    ImGui::SameLine();
    ImGui::BeginDisabled();
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::Button(axisDirectionName, ImVec2(SCALE_IMGUI_SIZE(26.0f), 0));
    ImGui::PopStyleColor();
    ImGui::EndDisabled();
    ImGui::SameLine(0.0f, SCALE_IMGUI_SIZE(4.0f));
    for (auto id : mStickDirectionToMappingIds[port][stick][direction]) {
        DrawStickDirectionLineEditMappingButton(port, stick, direction, id);
    }
    DrawStickDirectionLineAddMappingButton(port, stick, direction);
}

void SohInputEditorWindow::DrawStickSection(uint8_t port, uint8_t stick, int32_t id,
                                            ImVec4 color = CHIP_COLOR_N64_GREY) {
    static int8_t sX, sY;
    std::shared_ptr<LUS::ControllerStick> controllerStick = nullptr;
    if (stick == LUS::LEFT) {
        controllerStick = LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetLeftStick();
    } else {
        controllerStick = LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetRightStick();
    }
    controllerStick->Process(sX, sY);
    DrawAnalogPreview(StringHelper::Sprintf("##AnalogPreview%d", id).c_str(), ImVec2(sX, sY));

    ImGui::SameLine();
    ImGui::BeginGroup();
    DrawStickDirectionLine(ICON_FA_ARROW_UP, port, stick, LUS::UP, color);
    DrawStickDirectionLine(ICON_FA_ARROW_DOWN, port, stick, LUS::DOWN, color);
    DrawStickDirectionLine(ICON_FA_ARROW_LEFT, port, stick, LUS::LEFT, color);
    DrawStickDirectionLine(ICON_FA_ARROW_RIGHT, port, stick, LUS::RIGHT, color);
    ImGui::EndGroup();
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode(StringHelper::Sprintf("Analog Stick Options##%d", id).c_str())) {
        ImGui::Text("Deadzone:");

        int32_t deadzonePercentage = controllerStick->GetDeadzonePercentage();
        if (deadzonePercentage == 0) {
            ImGui::BeginDisabled();
        }
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button(StringHelper::Sprintf("-##Deadzone%d", id).c_str())) {
            controllerStick->SetDeadzone(deadzonePercentage - 1);
        }
        ImGui::PopButtonRepeat();
        if (deadzonePercentage == 0) {
            ImGui::EndDisabled();
        }
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::SetNextItemWidth(SCALE_IMGUI_SIZE(160.0f));
        if (ImGui::SliderInt(StringHelper::Sprintf("##Deadzone%d", id).c_str(), &deadzonePercentage, 0, 100, "%d%%",
                             ImGuiSliderFlags_AlwaysClamp)) {
            controllerStick->SetDeadzone(deadzonePercentage);
        }
        ImGui::SameLine(0.0f, 0.0f);
        if (deadzonePercentage == 100) {
            ImGui::BeginDisabled();
        }
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button(StringHelper::Sprintf("+##Deadzone%d", id).c_str())) {
            controllerStick->SetDeadzone(deadzonePercentage + 1);
        }
        ImGui::PopButtonRepeat();
        if (deadzonePercentage == 100) {
            ImGui::EndDisabled();
        }
        if (!controllerStick->DeadzoneIsDefault()) {
            ImGui::SameLine();
            if (ImGui::Button(StringHelper::Sprintf("Reset to Default###resetStickDeadzone%d", id).c_str())) {
                controllerStick->ResetDeadzoneToDefault();
            }
        }

        ImGui::Text("Notch Snap Angle:");
        int32_t notchSnapAngle = controllerStick->GetNotchSnapAngle();
        if (notchSnapAngle == 0) {
            ImGui::BeginDisabled();
        }
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button(StringHelper::Sprintf("-##NotchProximityThreshold%d", id).c_str())) {
            controllerStick->SetNotchSnapAngle(notchSnapAngle - 1);
        }
        ImGui::PopButtonRepeat();
        if (notchSnapAngle == 0) {
            ImGui::EndDisabled();
        }
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::SetNextItemWidth(SCALE_IMGUI_SIZE(160.0f));
        if (ImGui::SliderInt(StringHelper::Sprintf("##NotchProximityThreshold%d", id).c_str(), &notchSnapAngle, 0, 45,
                             "%d°", ImGuiSliderFlags_AlwaysClamp)) {
            controllerStick->SetNotchSnapAngle(notchSnapAngle);
        }
        ImGui::SameLine(0.0f, 0.0f);
        if (notchSnapAngle == 45) {
            ImGui::BeginDisabled();
        }
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button(StringHelper::Sprintf("+##NotchProximityThreshold%d", id).c_str())) {
            controllerStick->SetNotchSnapAngle(notchSnapAngle + 1);
        }
        ImGui::PopButtonRepeat();
        if (notchSnapAngle == 45) {
            ImGui::EndDisabled();
        }
        if (!controllerStick->NotchSnapAngleIsDefault()) {
            ImGui::SameLine();
            if (ImGui::Button(StringHelper::Sprintf("Reset to Default###resetStickSnap%d", id).c_str())) {
                controllerStick->ResetNotchSnapAngleToDefault();
            }
        }

        ImGui::TreePop();
    }
}

void SohInputEditorWindow::UpdateBitmaskToMappingIds(uint8_t port) {
    // todo: do we need this now that ControllerButton exists?

    for (auto [bitmask, button] :
         LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetAllButtons()) {
        for (auto [id, mapping] : button->GetAllButtonMappings()) {
            // using a vector here instead of a set because i want newly added mappings
            // to go to the end of the list instead of autosorting
            if (std::find(mBitmaskToMappingIds[port][bitmask].begin(), mBitmaskToMappingIds[port][bitmask].end(), id) ==
                mBitmaskToMappingIds[port][bitmask].end()) {
                mBitmaskToMappingIds[port][bitmask].push_back(id);
            }
        }
    }
}

void SohInputEditorWindow::UpdateStickDirectionToMappingIds(uint8_t port) {
    // todo: do we need this?
    for (auto stick :
         { std::make_pair<uint8_t, std::shared_ptr<LUS::ControllerStick>>(
               LUS::LEFT, LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetLeftStick()),
           std::make_pair<uint8_t, std::shared_ptr<LUS::ControllerStick>>(
               LUS::RIGHT,
               LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetRightStick()) }) {
        for (auto direction : { LUS::LEFT, LUS::RIGHT, LUS::UP, LUS::DOWN }) {
            for (auto [id, mapping] : stick.second->GetAllAxisDirectionMappingByDirection(direction)) {
                // using a vector here instead of a set because i want newly added mappings
                // to go to the end of the list instead of autosorting
                if (std::find(mStickDirectionToMappingIds[port][stick.first][direction].begin(),
                              mStickDirectionToMappingIds[port][stick.first][direction].end(),
                              id) == mStickDirectionToMappingIds[port][stick.first][direction].end()) {
                    mStickDirectionToMappingIds[port][stick.first][direction].push_back(id);
                }
            }
        }
    }
}

void SohInputEditorWindow::DrawRemoveRumbleMappingButton(uint8_t port, std::string id) {
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
    if (ImGui::Button(StringHelper::Sprintf("%s###removeRumbleMapping%s", ICON_FA_TIMES, id.c_str()).c_str(),
                      ImVec2(SCALE_IMGUI_SIZE(20.0f), SCALE_IMGUI_SIZE(20.0f)))) {
        LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetRumble()->ClearRumbleMapping(id);
    }
    ImGui::PopStyleVar();
}

void SohInputEditorWindow::DrawAddRumbleMappingButton(uint8_t port) {
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
    auto popupId = StringHelper::Sprintf("addRumbleMappingPopup##%d", port);
    if (ImGui::Button(StringHelper::Sprintf("%s###addRumbleMapping%d", ICON_FA_PLUS, port).c_str(),
                      ImVec2(SCALE_IMGUI_SIZE(20.0f), SCALE_IMGUI_SIZE(20.0f)))) {
        ImGui::OpenPopup(popupId.c_str());
    }
    ImGui::PopStyleVar();

    if (ImGui::BeginPopup(popupId.c_str())) {
        mInputEditorPopupOpen = true;
        ImGui::Text("Press any button\nor move any axis\nto add rumble device");
        if (ImGui::Button("Cancel")) {
            mInputEditorPopupOpen = false;
            ImGui::CloseCurrentPopup();
        }

        if (mMappingInputBlockTimer == INT32_MAX && LUS::Context::GetInstance()
                                                        ->GetControlDeck()
                                                        ->GetControllerByPort(port)
                                                        ->GetRumble()
                                                        ->AddRumbleMappingFromRawPress()) {
            mInputEditorPopupOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

bool SohInputEditorWindow::TestingRumble() {
    return mRumbleTimer != INT32_MAX;
}

void SohInputEditorWindow::DrawRumbleSection(uint8_t port) {
    for (auto [id, mapping] : LUS::Context::GetInstance()
                                  ->GetControlDeck()
                                  ->GetControllerByPort(port)
                                  ->GetRumble()
                                  ->GetAllRumbleMappings()) {
        ImGui::AlignTextToFramePadding();
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        GetButtonColorsForLUSDeviceIndex(mapping->GetLUSDeviceIndex(), buttonColor, buttonHoveredColor);
        // begin hackaround https://github.com/ocornut/imgui/issues/282#issuecomment-123763192
        // spaces to have background color for text in a tree node
        std::string spaces = "";
        for (size_t i = 0; i < mapping->GetPhysicalDeviceName().length(); i++) {
            spaces += " ";
        }
        auto open = ImGui::TreeNode(StringHelper::Sprintf("%s###Rumble%s", spaces.c_str(), id.c_str()).c_str());
        ImGui::SameLine();
        ImGui::SetCursorPosX(SCALE_IMGUI_SIZE(30.0f));
        // end hackaround

        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        ImGui::Button(mapping->GetPhysicalDeviceName().c_str());
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();

        DrawRemoveRumbleMappingButton(port, id);
        ImGui::SameLine();
        if (ImGui::Button(
                StringHelper::Sprintf("%s###rumbleTestButton%s", TestingRumble() ? "Stop" : "Test", id.c_str())
                    .c_str())) {
            if (mRumbleTimer != INT32_MAX) {
                mRumbleTimer = INT32_MAX;
                mRumbleMappingToTest->StopRumble();
                mRumbleMappingToTest = nullptr;
            } else {
                mRumbleTimer = ImGui::GetIO().Framerate;
                mRumbleMappingToTest = mapping;
            }
        }
        if (open) {
            ImGui::Text("Small Motor Intensity:");

            int32_t smallMotorIntensity = mapping->GetHighFrequencyIntensityPercentage();
            if (smallMotorIntensity == 0) {
                ImGui::BeginDisabled();
            }
            ImGui::PushButtonRepeat(true);
            if (ImGui::Button(StringHelper::Sprintf("-##Small Motor Intensity%s", id.c_str()).c_str())) {
                mapping->SetHighFrequencyIntensity(smallMotorIntensity - 1);
                mapping->SaveToConfig();
            }
            ImGui::PopButtonRepeat();
            if (smallMotorIntensity == 0) {
                ImGui::EndDisabled();
            }
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::SetNextItemWidth(SCALE_IMGUI_SIZE(160.0f));
            if (ImGui::SliderInt(StringHelper::Sprintf("##Small Motor Intensity%s", id.c_str()).c_str(),
                                 &smallMotorIntensity, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp)) {
                mapping->SetHighFrequencyIntensity(smallMotorIntensity);
                mapping->SaveToConfig();
            }
            ImGui::SameLine(0.0f, 0.0f);
            if (smallMotorIntensity == 100) {
                ImGui::BeginDisabled();
            }
            ImGui::PushButtonRepeat(true);
            if (ImGui::Button(StringHelper::Sprintf("+##Small Motor Intensity%s", id.c_str()).c_str())) {
                mapping->SetHighFrequencyIntensity(smallMotorIntensity + 1);
                mapping->SaveToConfig();
            }
            ImGui::PopButtonRepeat();
            if (smallMotorIntensity == 100) {
                ImGui::EndDisabled();
            }
            if (!mapping->HighFrequencyIntensityIsDefault()) {
                ImGui::SameLine();
                if (ImGui::Button(StringHelper::Sprintf("Reset to Default###resetHighFrequencyIntensity%s", id.c_str())
                                      .c_str())) {
                    mapping->ResetHighFrequencyIntensityToDefault();
                }
            }

            ImGui::Text("Large Motor Intensity:");

            int32_t largeMotorIntensity = mapping->GetLowFrequencyIntensityPercentage();
            if (largeMotorIntensity == 0) {
                ImGui::BeginDisabled();
            }
            ImGui::PushButtonRepeat(true);
            if (ImGui::Button(StringHelper::Sprintf("-##Large Motor Intensity%s", id.c_str()).c_str())) {
                mapping->SetLowFrequencyIntensity(largeMotorIntensity - 1);
                mapping->SaveToConfig();
            }
            ImGui::PopButtonRepeat();
            if (largeMotorIntensity == 0) {
                ImGui::EndDisabled();
            }
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::SetNextItemWidth(SCALE_IMGUI_SIZE(160.0f));
            if (ImGui::SliderInt(StringHelper::Sprintf("##Large Motor Intensity%s", id.c_str()).c_str(),
                                 &largeMotorIntensity, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp)) {
                mapping->SetLowFrequencyIntensity(largeMotorIntensity);
                mapping->SaveToConfig();
            }
            ImGui::SameLine(0.0f, 0.0f);
            if (largeMotorIntensity == 100) {
                ImGui::BeginDisabled();
            }
            ImGui::PushButtonRepeat(true);
            if (ImGui::Button(StringHelper::Sprintf("+##Large Motor Intensity%s", id.c_str()).c_str())) {
                mapping->SetLowFrequencyIntensity(largeMotorIntensity + 1);
                mapping->SaveToConfig();
            }
            ImGui::PopButtonRepeat();
            if (largeMotorIntensity == 100) {
                ImGui::EndDisabled();
            }
            if (!mapping->LowFrequencyIntensityIsDefault()) {
                ImGui::SameLine();
                if (ImGui::Button(
                        StringHelper::Sprintf("Reset to Default###resetLowFrequencyIntensity%s", id.c_str()).c_str())) {
                    mapping->ResetLowFrequencyIntensityToDefault();
                }
            }
            ImGui::Dummy(ImVec2(0, SCALE_IMGUI_SIZE(20)));

            ImGui::TreePop();
        }
    }

    ImGui::AlignTextToFramePadding();
    ImGui::BulletText("Add rumble device");
    DrawAddRumbleMappingButton(port);
}

void SohInputEditorWindow::DrawRemoveLEDMappingButton(uint8_t port, std::string id) {
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
    if (ImGui::Button(StringHelper::Sprintf("%s###removeLEDMapping%s", ICON_FA_TIMES, id.c_str()).c_str(),
                      ImVec2(SCALE_IMGUI_SIZE(20.0f), SCALE_IMGUI_SIZE(20.0f)))) {
        LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetLED()->ClearLEDMapping(id);
    }
    ImGui::PopStyleVar();
}

void SohInputEditorWindow::DrawAddLEDMappingButton(uint8_t port) {
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
    auto popupId = StringHelper::Sprintf("addLEDMappingPopup##%d", port);
    if (ImGui::Button(StringHelper::Sprintf("%s###addLEDMapping%d", ICON_FA_PLUS, port).c_str(),
                      ImVec2(SCALE_IMGUI_SIZE(20.0f), SCALE_IMGUI_SIZE(20.0f)))) {
        ImGui::OpenPopup(popupId.c_str());
    }
    ImGui::PopStyleVar();

    if (ImGui::BeginPopup(popupId.c_str())) {
        mInputEditorPopupOpen = true;
        ImGui::Text("Press any button\nor move any axis\nto add LED device");
        if (ImGui::Button("Cancel")) {
            mInputEditorPopupOpen = false;
            ImGui::CloseCurrentPopup();
        }

        if (mMappingInputBlockTimer == INT32_MAX && LUS::Context::GetInstance()
                                                        ->GetControlDeck()
                                                        ->GetControllerByPort(port)
                                                        ->GetLED()
                                                        ->AddLEDMappingFromRawPress()) {
            mInputEditorPopupOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void SohInputEditorWindow::DrawHelpIcon(const std::string& helptext) {
    // place the ? button to the most of the right side of the cell it is using.
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - SCALE_IMGUI_SIZE(22));
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - SCALE_IMGUI_SIZE(15));
    ImGui::SmallButton("?");
    UIWidgets::Tooltip(helptext.c_str());
}

void SohInputEditorWindow::DrawLEDSection(uint8_t port) {
    for (auto [id, mapping] :
         LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetLED()->GetAllLEDMappings()) {
        ImGui::AlignTextToFramePadding();
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        auto open = ImGui::TreeNode(
            StringHelper::Sprintf("%s##LED%s", mapping->GetPhysicalDeviceName().c_str(), id.c_str()).c_str());
        DrawRemoveLEDMappingButton(port, id);
        if (open) {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("LED Color:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(SCALE_IMGUI_SIZE(80.0f));
            int32_t colorSource = mapping->GetColorSource();
            if (ImGui::Combo(StringHelper::Sprintf("###ledColorSource%s", mapping->GetLEDMappingId().c_str()).c_str(),
                             &colorSource, "Off\0Set\0Game\0\0")) {
                mapping->SetColorSource(colorSource);
            };
            if (mapping->GetColorSource() == LED_COLOR_SOURCE_SET) {
                ImGui::SameLine();
                ImVec4 color = { mapping->GetSavedColor().r / 255.0f, mapping->GetSavedColor().g / 255.0f,
                                 mapping->GetSavedColor().b / 255.0f, 1.0f };
                if (ImGui::ColorEdit3(
                        StringHelper::Sprintf("###ledSavedColor%s", mapping->GetLEDMappingId().c_str()).c_str(),
                        (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
                    mapping->SetSavedColor(
                        Color_RGB8({ static_cast<uint8_t>(color.x * 255.0), static_cast<uint8_t>(color.y * 255.0),
                                     static_cast<uint8_t>(color.z * 255.0) }));
                }
            }
            // todo: clean this up, probably just hardcode to LED_COLOR_SOURCE_GAME and use SoH options only here
            if (mapping->GetColorSource() == LED_COLOR_SOURCE_GAME) {
                static const char* ledSources[] = {
                    "Original Tunic Colors",          "Cosmetics Tunic Colors",          "Health Colors",
                    "Original Navi Targeting Colors", "Cosmetics Navi Targeting Colors", "Custom"
                };
                UIWidgets::PaddedText("Source");
                UIWidgets::EnhancementCombobox("gLedColorSource", ledSources, LED_SOURCE_TUNIC_ORIGINAL);
                DrawHelpIcon("Health\n- Red when health critical (13-20% depending on max health)\n- Yellow when "
                             "health < 40%. Green otherwise.\n\n"
                             "Tunics: colors will mirror currently equipped tunic, whether original or the current "
                             "values in Cosmetics Editor.\n\n"
                             "Custom: single, solid color");
                if (CVarGetInteger("gLedColorSource", 1) == LED_SOURCE_CUSTOM) {
                    UIWidgets::Spacer(3);
                    auto port1Color = CVarGetColor24("gLedPort1Color", { 255, 255, 255 });
                    ImVec4 colorVec = { port1Color.r / 255.0f, port1Color.g / 255.0f, port1Color.b / 255.0f, 1.0f };
                    if (ImGui::ColorEdit3("", (float*)&colorVec,
                                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
                        Color_RGB8 color;
                        color.r = colorVec.x * 255.0;
                        color.g = colorVec.y * 255.0;
                        color.b = colorVec.z * 255.0;

                        CVarSetColor24("gLedPort1Color", color);
                        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                    }
                    ImGui::SameLine();
                    ImGui::Text("Custom Color");
                }
                UIWidgets::PaddedEnhancementSliderFloat("Brightness: %d%%", "##LED_Brightness", "gLedBrightness", 0.0f,
                                                        1.0f, "", 1.0f, true, true);
                DrawHelpIcon("Sets the brightness of controller LEDs. 0% brightness = LEDs off.");
                UIWidgets::PaddedEnhancementCheckbox(
                    "Critical Health Override", "gLedCriticalOverride", true, true,
                    CVarGetInteger("gLedColorSource", LED_SOURCE_TUNIC_ORIGINAL) == LED_SOURCE_HEALTH,
                    "Override redundant for health source.", UIWidgets::CheckboxGraphics::Cross, true);
                DrawHelpIcon("Shows red color when health is critical, otherwise displays according to color source.");
            }
            ImGui::TreePop();
        }
    }

    ImGui::AlignTextToFramePadding();
    ImGui::BulletText("Add LED device");
    DrawAddLEDMappingButton(port);
}

void SohInputEditorWindow::DrawRemoveGyroMappingButton(uint8_t port, std::string id) {
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
    if (ImGui::Button(StringHelper::Sprintf("%s###removeGyroMapping%s", ICON_FA_TIMES, id.c_str()).c_str(),
                      ImVec2(SCALE_IMGUI_SIZE(20.0f), SCALE_IMGUI_SIZE(20.0f)))) {
        LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetGyro()->ClearGyroMapping();
    }
    ImGui::PopStyleVar();
}

void SohInputEditorWindow::DrawAddGyroMappingButton(uint8_t port) {
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
    auto popupId = StringHelper::Sprintf("addGyroMappingPopup##%d", port);
    if (ImGui::Button(StringHelper::Sprintf("%s###addGyroMapping%d", ICON_FA_PLUS, port).c_str(),
                      ImVec2(SCALE_IMGUI_SIZE(20.0f), SCALE_IMGUI_SIZE(20.0f)))) {
        ImGui::OpenPopup(popupId.c_str());
    }
    ImGui::PopStyleVar();

    if (ImGui::BeginPopup(popupId.c_str())) {
        mInputEditorPopupOpen = true;
        ImGui::Text("Press any button\nor move any axis\nto add gyro device");
        if (ImGui::Button("Cancel")) {
            mInputEditorPopupOpen = false;
            ImGui::CloseCurrentPopup();
        }

        if (mMappingInputBlockTimer == INT32_MAX && LUS::Context::GetInstance()
                                                        ->GetControlDeck()
                                                        ->GetControllerByPort(port)
                                                        ->GetGyro()
                                                        ->SetGyroMappingFromRawPress()) {
            mInputEditorPopupOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void SohInputEditorWindow::DrawGyroSection(uint8_t port) {
    auto mapping =
        LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetGyro()->GetGyroMapping();
    if (mapping != nullptr) {
        auto id = mapping->GetGyroMappingId();
        ImGui::AlignTextToFramePadding();
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        ImGui::BulletText(mapping->GetPhysicalDeviceName().c_str());
        DrawRemoveGyroMappingButton(port, id);

        static float sPitch, sYaw = 0.0f;
        mapping->UpdatePad(sPitch, sYaw);

        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - SCALE_IMGUI_SIZE(8)));
        // to find a reasonable scaling factor gyro values
        // I tried to find the maximum value reported by shaking
        // a PS5 controller as hard as I could without worrying about breaking it
        // the max I found for both pitch and yaw was ~21
        // the preview window expects values in an n64 analog stick range (-85 to 85)
        // so I decided to multiply these by 85/21
        DrawAnalogPreview(StringHelper::Sprintf("###GyroPreview%s", id.c_str()).c_str(),
                          ImVec2(sYaw * (85.0f / 21.0f), sPitch * (85.0f / 21.0f)), 0.0f, true);
        ImGui::SameLine();
        ImGui::SetCursorPos(
            ImVec2(ImGui::GetCursorPos().x + SCALE_IMGUI_SIZE(8), ImGui::GetCursorPos().y + SCALE_IMGUI_SIZE(8)));

        ImGui::BeginGroup();
        ImGui::Text("Sensitivity:");

        int32_t sensitivity = mapping->GetSensitivityPercent();
        if (sensitivity == 0) {
            ImGui::BeginDisabled();
        }
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button(StringHelper::Sprintf("-##GyroSensitivity%s", id.c_str()).c_str())) {
            mapping->SetSensitivity(sensitivity - 1);
            mapping->SaveToConfig();
        }
        ImGui::PopButtonRepeat();
        if (sensitivity == 0) {
            ImGui::EndDisabled();
        }
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::SetNextItemWidth(SCALE_IMGUI_SIZE(160.0f));
        if (ImGui::SliderInt(StringHelper::Sprintf("##GyroSensitivity%s", id.c_str()).c_str(), &sensitivity, 0, 100,
                             "%d%%", ImGuiSliderFlags_AlwaysClamp)) {
            mapping->SetSensitivity(sensitivity);
            mapping->SaveToConfig();
        }
        ImGui::SameLine(0.0f, 0.0f);
        if (sensitivity == 100) {
            ImGui::BeginDisabled();
        }
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button(StringHelper::Sprintf("+##GyroSensitivity%s", id.c_str()).c_str())) {
            mapping->SetSensitivity(sensitivity + 1);
            mapping->SaveToConfig();
        }
        ImGui::PopButtonRepeat();
        if (sensitivity == 100) {
            ImGui::EndDisabled();
        }

        if (!mapping->SensitivityIsDefault()) {
            ImGui::SameLine();
            if (ImGui::Button(StringHelper::Sprintf("Reset to Default###resetGyroSensitivity%s", id.c_str()).c_str())) {
                mapping->ResetSensitivityToDefault();
            }
        }

        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + SCALE_IMGUI_SIZE(8)));
        if (ImGui::Button("Recalibrate")) {
            mapping->Recalibrate();
            mapping->SaveToConfig();
        }
        ImGui::EndGroup();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - SCALE_IMGUI_SIZE(8)));
    } else {
        ImGui::AlignTextToFramePadding();
        ImGui::BulletText("Add gyro device");
        DrawAddGyroMappingButton(port);
    }
}

void SohInputEditorWindow::DrawButtonDeviceIcons(uint8_t portIndex, std::set<uint16_t> bitmasks) {
    std::set<LUS::LUSDeviceIndex> allLusDeviceIndices;
    allLusDeviceIndices.insert(LUS::LUSDeviceIndex::Keyboard);
    for (auto [lusIndex, mapping] : LUS::Context::GetInstance()
                                        ->GetControlDeck()
                                        ->GetDeviceIndexMappingManager()
                                        ->GetAllDeviceIndexMappingsFromConfig()) {
        allLusDeviceIndices.insert(lusIndex);
    }

    std::vector<std::pair<LUS::LUSDeviceIndex, bool>> lusDeviceIndiciesWithMappings;
    for (auto lusIndex : allLusDeviceIndices) {
        for (auto [bitmask, button] :
             LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->GetAllButtons()) {
            if (!bitmasks.contains(bitmask)) {
                continue;
            }

            if (button->HasMappingsForLUSDeviceIndex(lusIndex)) {
                for (auto [id, mapping] : button->GetAllButtonMappings()) {
                    if (mapping->GetLUSDeviceIndex() == lusIndex) {
                        lusDeviceIndiciesWithMappings.push_back(
                            std::pair<LUS::LUSDeviceIndex, bool>(lusIndex, mapping->PhysicalDeviceIsConnected()));
                        break;
                    }
                }
                break;
            }
        }
    }

    for (auto [lusIndex, connected] : lusDeviceIndiciesWithMappings) {
        auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        GetButtonColorsForLUSDeviceIndex(lusIndex, buttonColor, buttonHoveredColor);
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
        ImGui::SameLine();
        if (lusIndex == LUS::LUSDeviceIndex::Keyboard) {
            ImGui::SmallButton(ICON_FA_KEYBOARD_O);
        } else {
            ImGui::SmallButton(connected ? ICON_FA_GAMEPAD : ICON_FA_CHAIN_BROKEN);
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
}

void SohInputEditorWindow::DrawAnalogStickDeviceIcons(uint8_t portIndex, LUS::Stick stick) {
    std::set<LUS::LUSDeviceIndex> allLusDeviceIndices;
    allLusDeviceIndices.insert(LUS::LUSDeviceIndex::Keyboard);
    for (auto [lusIndex, mapping] : LUS::Context::GetInstance()
                                        ->GetControlDeck()
                                        ->GetDeviceIndexMappingManager()
                                        ->GetAllDeviceIndexMappingsFromConfig()) {
        allLusDeviceIndices.insert(lusIndex);
    }

    std::vector<std::pair<LUS::LUSDeviceIndex, bool>> lusDeviceIndiciesWithMappings;
    for (auto lusIndex : allLusDeviceIndices) {
        auto controllerStick =
            stick == LUS::Stick::LEFT_STICK
                ? LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->GetLeftStick()
                : LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->GetRightStick();
        if (controllerStick->HasMappingsForLUSDeviceIndex(lusIndex)) {
            for (auto [direction, mappings] : controllerStick->GetAllAxisDirectionMappings()) {
                bool foundMapping = false;
                for (auto [id, mapping] : mappings) {
                    if (mapping->GetLUSDeviceIndex() == lusIndex) {
                        foundMapping = true;
                        lusDeviceIndiciesWithMappings.push_back(
                            std::pair<LUS::LUSDeviceIndex, bool>(lusIndex, mapping->PhysicalDeviceIsConnected()));
                        break;
                    }
                }
                if (foundMapping) {
                    break;
                }
            }
        }
    }

    for (auto [lusIndex, connected] : lusDeviceIndiciesWithMappings) {
        auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        GetButtonColorsForLUSDeviceIndex(lusIndex, buttonColor, buttonHoveredColor);
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
        ImGui::SameLine();
        if (lusIndex == LUS::LUSDeviceIndex::Keyboard) {
            ImGui::SmallButton(ICON_FA_KEYBOARD_O);
        } else {
            ImGui::SmallButton(connected ? ICON_FA_GAMEPAD : ICON_FA_CHAIN_BROKEN);
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
}

void SohInputEditorWindow::DrawRumbleDeviceIcons(uint8_t portIndex) {
    std::set<LUS::LUSDeviceIndex> allLusDeviceIndices;
    for (auto [lusIndex, mapping] : LUS::Context::GetInstance()
                                        ->GetControlDeck()
                                        ->GetDeviceIndexMappingManager()
                                        ->GetAllDeviceIndexMappingsFromConfig()) {
        allLusDeviceIndices.insert(lusIndex);
    }

    std::vector<std::pair<LUS::LUSDeviceIndex, bool>> lusDeviceIndiciesWithMappings;
    for (auto lusIndex : allLusDeviceIndices) {
        if (LUS::Context::GetInstance()
                ->GetControlDeck()
                ->GetControllerByPort(portIndex)
                ->GetRumble()
                ->HasMappingsForLUSDeviceIndex(lusIndex)) {
            for (auto [id, mapping] : LUS::Context::GetInstance()
                                          ->GetControlDeck()
                                          ->GetControllerByPort(portIndex)
                                          ->GetRumble()
                                          ->GetAllRumbleMappings()) {
                if (mapping->GetLUSDeviceIndex() == lusIndex) {
                    lusDeviceIndiciesWithMappings.push_back(
                        std::pair<LUS::LUSDeviceIndex, bool>(lusIndex, mapping->PhysicalDeviceIsConnected()));
                    break;
                }
            }
        }
    }

    for (auto [lusIndex, connected] : lusDeviceIndiciesWithMappings) {
        auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        GetButtonColorsForLUSDeviceIndex(lusIndex, buttonColor, buttonHoveredColor);
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
        ImGui::SameLine();
        ImGui::SmallButton(connected ? ICON_FA_GAMEPAD : ICON_FA_CHAIN_BROKEN);
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
}

void SohInputEditorWindow::DrawGyroDeviceIcons(uint8_t portIndex) {
    auto mapping =
        LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->GetGyro()->GetGyroMapping();
    if (mapping == nullptr) {
        return;
    }

    auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
    GetButtonColorsForLUSDeviceIndex(mapping->GetLUSDeviceIndex(), buttonColor, buttonHoveredColor);
    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
    ImGui::SameLine();
    ImGui::SmallButton(mapping->PhysicalDeviceIsConnected() ? ICON_FA_GAMEPAD : ICON_FA_CHAIN_BROKEN);
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}

void SohInputEditorWindow::DrawLEDDeviceIcons(uint8_t portIndex) {
    std::set<LUS::LUSDeviceIndex> allLusDeviceIndices;
    for (auto [lusIndex, mapping] : LUS::Context::GetInstance()
                                        ->GetControlDeck()
                                        ->GetDeviceIndexMappingManager()
                                        ->GetAllDeviceIndexMappingsFromConfig()) {
        allLusDeviceIndices.insert(lusIndex);
    }

    std::vector<std::pair<LUS::LUSDeviceIndex, bool>> lusDeviceIndiciesWithMappings;
    for (auto lusIndex : allLusDeviceIndices) {
        if (LUS::Context::GetInstance()
                ->GetControlDeck()
                ->GetControllerByPort(portIndex)
                ->GetRumble()
                ->HasMappingsForLUSDeviceIndex(lusIndex)) {
            for (auto [id, mapping] : LUS::Context::GetInstance()
                                          ->GetControlDeck()
                                          ->GetControllerByPort(portIndex)
                                          ->GetLED()
                                          ->GetAllLEDMappings()) {
                if (mapping->GetLUSDeviceIndex() == lusIndex) {
                    lusDeviceIndiciesWithMappings.push_back(
                        std::pair<LUS::LUSDeviceIndex, bool>(lusIndex, mapping->PhysicalDeviceIsConnected()));
                    break;
                }
            }
        }
    }

    for (auto [lusIndex, connected] : lusDeviceIndiciesWithMappings) {
        auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        GetButtonColorsForLUSDeviceIndex(lusIndex, buttonColor, buttonHoveredColor);
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
        ImGui::SameLine();
        ImGui::SmallButton(connected ? ICON_FA_GAMEPAD : ICON_FA_CHAIN_BROKEN);
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
}

void SohInputEditorWindow::DrawLinkTab() {
    uint8_t portIndex = 0;
    if (ImGui::BeginTabItem(StringHelper::Sprintf("Link (P1)###port%d", portIndex).c_str())) {
        DrawClearAllButton(portIndex);
        DrawSetDefaultsButton(portIndex);

        UpdateBitmaskToMappingIds(portIndex);
        UpdateStickDirectionToMappingIds(portIndex);

        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.133f, 0.133f, 0.133f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

        if (ImGui::CollapsingHeader("Buttons", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawButtonDeviceIcons(portIndex, mButtonsBitmasks);
            DrawButtonLine("A", portIndex, BTN_A, CHIP_COLOR_N64_BLUE);
            DrawButtonLine("B", portIndex, BTN_B, CHIP_COLOR_N64_GREEN);
            DrawButtonLine("Start", portIndex, BTN_START, CHIP_COLOR_N64_RED);
            DrawButtonLine("L", portIndex, BTN_L);
            DrawButtonLine("R", portIndex, BTN_R);
            DrawButtonLine("Z", portIndex, BTN_Z);
            DrawButtonLine(StringHelper::Sprintf("C %s", ICON_FA_ARROW_UP).c_str(), portIndex, BTN_CUP,
                           CHIP_COLOR_N64_YELLOW);
            DrawButtonLine(StringHelper::Sprintf("C %s", ICON_FA_ARROW_DOWN).c_str(), portIndex, BTN_CDOWN,
                           CHIP_COLOR_N64_YELLOW);
            DrawButtonLine(StringHelper::Sprintf("C %s", ICON_FA_ARROW_LEFT).c_str(), portIndex, BTN_CLEFT,
                           CHIP_COLOR_N64_YELLOW);
            DrawButtonLine(StringHelper::Sprintf("C %s", ICON_FA_ARROW_RIGHT).c_str(), portIndex, BTN_CRIGHT,
                           CHIP_COLOR_N64_YELLOW);
        } else {
            DrawButtonDeviceIcons(portIndex, mButtonsBitmasks);
        }

        if (ImGui::CollapsingHeader("D-Pad", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawButtonDeviceIcons(portIndex, mDpadBitmasks);
            DrawButtonLine(StringHelper::Sprintf("%s", ICON_FA_ARROW_UP).c_str(), portIndex, BTN_DUP);
            DrawButtonLine(StringHelper::Sprintf("%s", ICON_FA_ARROW_DOWN).c_str(), portIndex, BTN_DDOWN);
            DrawButtonLine(StringHelper::Sprintf("%s", ICON_FA_ARROW_LEFT).c_str(), portIndex, BTN_DLEFT);
            DrawButtonLine(StringHelper::Sprintf("%s", ICON_FA_ARROW_RIGHT).c_str(), portIndex, BTN_DRIGHT);
        } else {
            DrawButtonDeviceIcons(portIndex, mDpadBitmasks);
        }

        if (ImGui::CollapsingHeader("Analog Stick", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawAnalogStickDeviceIcons(portIndex, LUS::LEFT_STICK);
            DrawStickSection(portIndex, LUS::LEFT, 0);
        } else {
            DrawAnalogStickDeviceIcons(portIndex, LUS::LEFT_STICK);
        }

        if (ImGui::CollapsingHeader("Additional (\"Right\") Stick")) {
            DrawAnalogStickDeviceIcons(portIndex, LUS::RIGHT_STICK);
            DrawStickSection(portIndex, LUS::RIGHT, 1, CHIP_COLOR_N64_YELLOW);
        } else {
            DrawAnalogStickDeviceIcons(portIndex, LUS::RIGHT_STICK);
        }

        if (ImGui::CollapsingHeader("Rumble")) {
            DrawRumbleDeviceIcons(portIndex);
            DrawRumbleSection(portIndex);
        } else {
            DrawRumbleDeviceIcons(portIndex);
        }

        if (ImGui::CollapsingHeader("Gyro")) {
            DrawGyroDeviceIcons(portIndex);
            DrawGyroSection(portIndex);
        } else {
            DrawGyroDeviceIcons(portIndex);
        }

        if (ImGui::CollapsingHeader("LEDs")) {
            DrawLEDDeviceIcons(portIndex);
            DrawLEDSection(portIndex);
        } else {
            DrawLEDDeviceIcons(portIndex);
        }

        if (ImGui::CollapsingHeader("Modifier Buttons")) {
            DrawButtonDeviceIcons(portIndex, mModifierButtonsBitmasks);
            DrawButtonLine("M1", portIndex, BTN_MODIFIER1);
            DrawButtonLine("M2", portIndex, BTN_MODIFIER2);
        } else {
            DrawButtonDeviceIcons(portIndex, mModifierButtonsBitmasks);
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::EndTabItem();
    }
}

void SohInputEditorWindow::DrawIvanTab() {
    if (CVarGetInteger("gDebugEnabled", 0)) {
        DrawDebugPortTab(1, "Ivan (P2)");
        return;
    }

    uint8_t portIndex = 1;
    if (ImGui::BeginTabItem(StringHelper::Sprintf("Ivan (P2)###port%d", portIndex).c_str())) {
        DrawClearAllButton(portIndex);
        DrawSetDefaultsButton(portIndex);

        UpdateBitmaskToMappingIds(portIndex);
        UpdateStickDirectionToMappingIds(portIndex);

        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.133f, 0.133f, 0.133f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

        if (ImGui::CollapsingHeader("Buttons", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawButtonDeviceIcons(portIndex, mButtonsBitmasks);
            DrawButtonLine("A", portIndex, BTN_A, CHIP_COLOR_N64_BLUE);
            DrawButtonLine("B", portIndex, BTN_B, CHIP_COLOR_N64_GREEN);
            DrawButtonLine("Z", portIndex, BTN_Z);
            DrawButtonLine(StringHelper::Sprintf("C %s", ICON_FA_ARROW_UP).c_str(), portIndex, BTN_CUP,
                           CHIP_COLOR_N64_YELLOW);
            DrawButtonLine(StringHelper::Sprintf("C %s", ICON_FA_ARROW_DOWN).c_str(), portIndex, BTN_CDOWN,
                           CHIP_COLOR_N64_YELLOW);
            DrawButtonLine(StringHelper::Sprintf("C %s", ICON_FA_ARROW_LEFT).c_str(), portIndex, BTN_CLEFT,
                           CHIP_COLOR_N64_YELLOW);
            DrawButtonLine(StringHelper::Sprintf("C %s", ICON_FA_ARROW_RIGHT).c_str(), portIndex, BTN_CRIGHT,
                           CHIP_COLOR_N64_YELLOW);
        } else {
            DrawButtonDeviceIcons(portIndex, mButtonsBitmasks);
        }

        if (ImGui::CollapsingHeader("D-Pad", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawButtonDeviceIcons(portIndex, mDpadBitmasks);
            DrawButtonLine(StringHelper::Sprintf("%s", ICON_FA_ARROW_UP).c_str(), portIndex, BTN_DUP);
            DrawButtonLine(StringHelper::Sprintf("%s", ICON_FA_ARROW_DOWN).c_str(), portIndex, BTN_DDOWN);
            DrawButtonLine(StringHelper::Sprintf("%s", ICON_FA_ARROW_LEFT).c_str(), portIndex, BTN_DLEFT);
            DrawButtonLine(StringHelper::Sprintf("%s", ICON_FA_ARROW_RIGHT).c_str(), portIndex, BTN_DRIGHT);
        } else {
            DrawButtonDeviceIcons(portIndex, mDpadBitmasks);
        }

        if (ImGui::CollapsingHeader("Analog Stick", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawAnalogStickDeviceIcons(portIndex, LUS::LEFT_STICK);
            DrawStickSection(portIndex, LUS::LEFT, 0);
        } else {
            DrawAnalogStickDeviceIcons(portIndex, LUS::LEFT_STICK);
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::EndTabItem();
    }
}

void SohInputEditorWindow::DrawDebugPortTab(uint8_t portIndex, std::string customName) {
    if (ImGui::BeginTabItem(customName == ""
                                ? StringHelper::Sprintf("Port %d###port%d", portIndex + 1, portIndex).c_str()
                                : customName.c_str())) {
        DrawClearAllButton(portIndex);
        DrawSetDefaultsButton(portIndex);

        UpdateBitmaskToMappingIds(portIndex);
        UpdateStickDirectionToMappingIds(portIndex);

        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.133f, 0.133f, 0.133f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

        if (ImGui::CollapsingHeader("Buttons", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawButtonDeviceIcons(portIndex, mButtonsBitmasks);
            DrawButtonLine("A", portIndex, BTN_A, CHIP_COLOR_N64_BLUE);
            DrawButtonLine("B", portIndex, BTN_B, CHIP_COLOR_N64_GREEN);
            DrawButtonLine("Start", portIndex, BTN_START, CHIP_COLOR_N64_RED);
            DrawButtonLine("L", portIndex, BTN_L);
            DrawButtonLine("R", portIndex, BTN_R);
            DrawButtonLine("Z", portIndex, BTN_Z);
            DrawButtonLine(StringHelper::Sprintf("C %s", ICON_FA_ARROW_UP).c_str(), portIndex, BTN_CUP,
                           CHIP_COLOR_N64_YELLOW);
            DrawButtonLine(StringHelper::Sprintf("C %s", ICON_FA_ARROW_DOWN).c_str(), portIndex, BTN_CDOWN,
                           CHIP_COLOR_N64_YELLOW);
            DrawButtonLine(StringHelper::Sprintf("C %s", ICON_FA_ARROW_LEFT).c_str(), portIndex, BTN_CLEFT,
                           CHIP_COLOR_N64_YELLOW);
            DrawButtonLine(StringHelper::Sprintf("C %s", ICON_FA_ARROW_RIGHT).c_str(), portIndex, BTN_CRIGHT,
                           CHIP_COLOR_N64_YELLOW);
        } else {
            DrawButtonDeviceIcons(portIndex, mButtonsBitmasks);
        }

        if (ImGui::CollapsingHeader("D-Pad", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawButtonDeviceIcons(portIndex, mDpadBitmasks);
            DrawButtonLine(StringHelper::Sprintf("%s", ICON_FA_ARROW_UP).c_str(), portIndex, BTN_DUP);
            DrawButtonLine(StringHelper::Sprintf("%s", ICON_FA_ARROW_DOWN).c_str(), portIndex, BTN_DDOWN);
            DrawButtonLine(StringHelper::Sprintf("%s", ICON_FA_ARROW_LEFT).c_str(), portIndex, BTN_DLEFT);
            DrawButtonLine(StringHelper::Sprintf("%s", ICON_FA_ARROW_RIGHT).c_str(), portIndex, BTN_DRIGHT);
        } else {
            DrawButtonDeviceIcons(portIndex, mDpadBitmasks);
        }

        if (ImGui::CollapsingHeader("Analog Stick", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawAnalogStickDeviceIcons(portIndex, LUS::LEFT_STICK);
            DrawStickSection(portIndex, LUS::LEFT, 0);
        } else {
            DrawAnalogStickDeviceIcons(portIndex, LUS::LEFT_STICK);
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::EndTabItem();
    }
}

void SohInputEditorWindow::DrawClearAllButton(uint8_t portIndex) {
    if (ImGui::Button("Clear All", ImGui::CalcTextSize("Clear All") * 2)) {
        ImGui::OpenPopup("Clear All##clearAllPopup");
    }
    if (ImGui::BeginPopupModal("Clear All##clearAllPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("This will clear all mappings for port %d.\n\nContinue?", portIndex + 1);
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Clear All")) {
            LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->ClearAllMappings();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

#ifdef __WIIU__
void SohInputEditorWindow::DrawSetDefaultsButton(uint8_t portIndex) {
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
    auto popupId = StringHelper::Sprintf("setDefaultsPopup##%d", portIndex);
    if (ImGui::Button(StringHelper::Sprintf("Set defaults...##%d", portIndex).c_str())) {
        ImGui::OpenPopup(popupId.c_str());
    }
    ImGui::PopStyleVar();

    if (ImGui::BeginPopup(popupId.c_str())) {
        std::map<LUS::LUSDeviceIndex, std::pair<std::string, int32_t>> indexMappings;
        for (auto [lusIndex, mapping] : LUS::Context::GetInstance()
                                            ->GetControlDeck()
                                            ->GetDeviceIndexMappingManager()
                                            ->GetAllDeviceIndexMappings()) {
            auto wiiuIndexMapping = std::static_pointer_cast<LUS::LUSDeviceIndexToWiiUDeviceIndexMapping>(mapping);
            if (wiiuIndexMapping == nullptr) {
                continue;
            }

            indexMappings[lusIndex] = { wiiuIndexMapping->GetWiiUControllerName(),
                                        wiiuIndexMapping->IsWiiUGamepad() ? INT32_MAX
                                                                          : wiiuIndexMapping->GetDeviceChannel() };
        }

        bool shouldClose = false;
        for (auto [lusIndex, info] : indexMappings) {
            auto [name, wiiuChannel] = info;
            bool isGamepad = wiiuChannel == INT32_MAX;

            auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
            auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
            GetButtonColorsForLUSDeviceIndex(lusIndex, buttonColor, buttonHoveredColor);
            ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);

            auto fancyName = StringHelper::Sprintf(
                "%s%s", name.c_str(), isGamepad ? "" : StringHelper::Sprintf(" (%d)", wiiuChannel).c_str());
            if (ImGui::Button(StringHelper::Sprintf("%s %s", ICON_FA_GAMEPAD, fancyName.c_str()).c_str())) {
                ImGui::OpenPopup(StringHelper::Sprintf("Set Defaults for %s", name.c_str()).c_str());
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            if (ImGui::BeginPopupModal(StringHelper::Sprintf("Set Defaults for %s", name.c_str()).c_str(), NULL,
                                       ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("This will clear all existing mappings for\n%s on port %d.\n\nContinue?", fancyName.c_str(),
                            portIndex + 1);
                if (ImGui::Button("Cancel")) {
                    shouldClose = true;
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("Set defaults")) {
                    LUS::Context::GetInstance()
                        ->GetControlDeck()
                        ->GetControllerByPort(portIndex)
                        ->ClearAllMappingsForDevice(lusIndex);
                    LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->AddDefaultMappings(
                        lusIndex);
                    shouldClose = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        if (ImGui::Button("Cancel") || shouldClose) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void SohInputEditorWindow::DrawDevicesTab() {
    if (ImGui::BeginTabItem("Devices")) {
        std::map<LUS::LUSDeviceIndex, std::pair<std::string, int32_t>> indexMappings;
        for (auto [lusIndex, mapping] : LUS::Context::GetInstance()
                                            ->GetControlDeck()
                                            ->GetDeviceIndexMappingManager()
                                            ->GetAllDeviceIndexMappingsFromConfig()) {
            auto wiiuIndexMapping = std::static_pointer_cast<LUS::LUSDeviceIndexToWiiUDeviceIndexMapping>(mapping);
            if (wiiuIndexMapping == nullptr) {
                continue;
            }

            indexMappings[lusIndex] = { wiiuIndexMapping->GetWiiUControllerName(), -1 };
        }

        for (auto [lusIndex, mapping] : LUS::Context::GetInstance()
                                            ->GetControlDeck()
                                            ->GetDeviceIndexMappingManager()
                                            ->GetAllDeviceIndexMappings()) {
            auto wiiuIndexMapping = std::static_pointer_cast<LUS::LUSDeviceIndexToWiiUDeviceIndexMapping>(mapping);
            if (wiiuIndexMapping == nullptr) {
                continue;
            }

            indexMappings[lusIndex] = { wiiuIndexMapping->GetWiiUControllerName(),
                                        wiiuIndexMapping->IsWiiUGamepad() ? INT32_MAX
                                                                          : wiiuIndexMapping->GetDeviceChannel() };
        }

        for (auto [lusIndex, info] : indexMappings) {
            auto [name, wiiuChannel] = info;
            bool connected = wiiuChannel != -1;
            bool isGamepad = wiiuChannel == INT32_MAX;

            auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
            auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
            GetButtonColorsForLUSDeviceIndex(lusIndex, buttonColor, buttonHoveredColor);
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
            ImGui::Button(StringHelper::Sprintf("%s %s%s", connected ? ICON_FA_GAMEPAD : ICON_FA_CHAIN_BROKEN,
                                                name.c_str(),
                                                !connected  ? " (Disconnected)"
                                                : isGamepad ? ""
                                                            : StringHelper::Sprintf(" (%d)", wiiuChannel).c_str())
                              .c_str());
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        }

        ImGui::EndTabItem();
    }
}
#else
void SohInputEditorWindow::DrawSetDefaultsButton(uint8_t portIndex) {
    ImGui::SameLine();
    auto popupId = StringHelper::Sprintf("setDefaultsPopup##%d", portIndex);
    if (ImGui::Button(StringHelper::Sprintf("Set Defaults##%d", portIndex).c_str(),
                      ImVec2(ImGui::CalcTextSize("Set Defaults") * 2))) {
        ImGui::OpenPopup(popupId.c_str());
    }

    if (ImGui::BeginPopup(popupId.c_str())) {
        std::map<LUS::LUSDeviceIndex, std::pair<std::string, int32_t>> indexMappings;
        for (auto [lusIndex, mapping] : LUS::Context::GetInstance()
                                            ->GetControlDeck()
                                            ->GetDeviceIndexMappingManager()
                                            ->GetAllDeviceIndexMappings()) {
            auto sdlIndexMapping = std::static_pointer_cast<LUS::LUSDeviceIndexToSDLDeviceIndexMapping>(mapping);
            if (sdlIndexMapping == nullptr) {
                continue;
            }

            indexMappings[lusIndex] = { sdlIndexMapping->GetSDLControllerName(), sdlIndexMapping->GetSDLDeviceIndex() };
        }

        bool shouldClose = false;
        ImGui::PushStyleColor(ImGuiCol_Button, BUTTON_COLOR_KEYBOARD_BEIGE);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BUTTON_COLOR_KEYBOARD_BEIGE_HOVERED);
        if (ImGui::Button(StringHelper::Sprintf("%s Keyboard", ICON_FA_KEYBOARD_O).c_str())) {
            ImGui::OpenPopup("Set Defaults for Keyboard");
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        if (ImGui::BeginPopupModal("Set Defaults for Keyboard", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("This will clear all existing mappings for\nKeyboard on port %d.\n\nContinue?", portIndex + 1);
            if (ImGui::Button("Cancel")) {
                shouldClose = true;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Set defaults")) {
                LUS::Context::GetInstance()
                    ->GetControlDeck()
                    ->GetControllerByPort(portIndex)
                    ->ClearAllMappingsForDevice(LUS::LUSDeviceIndex::Keyboard);
                LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->AddDefaultMappings(
                    LUS::LUSDeviceIndex::Keyboard);
                shouldClose = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        for (auto [lusIndex, info] : indexMappings) {
            auto [name, sdlIndex] = info;

            auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
            auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
            GetButtonColorsForLUSDeviceIndex(lusIndex, buttonColor, buttonHoveredColor);
            ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
            if (ImGui::Button(StringHelper::Sprintf("%s %s (%s)", ICON_FA_GAMEPAD, name.c_str(),
                                                    StringHelper::Sprintf("SDL %d", sdlIndex).c_str())
                                  .c_str())) {
                ImGui::OpenPopup(StringHelper::Sprintf("Set Defaults for %s", name.c_str()).c_str());
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            if (ImGui::BeginPopupModal(StringHelper::Sprintf("Set Defaults for %s", name.c_str()).c_str(), NULL,
                                       ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("This will clear all existing mappings for\n%s (SDL %d) on port %d.\n\nContinue?",
                            name.c_str(), sdlIndex, portIndex + 1);
                if (ImGui::Button("Cancel")) {
                    shouldClose = true;
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("Set defaults")) {
                    LUS::Context::GetInstance()
                        ->GetControlDeck()
                        ->GetControllerByPort(portIndex)
                        ->ClearAllMappingsForDevice(lusIndex);
                    LUS::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->AddDefaultMappings(
                        lusIndex);
                    shouldClose = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        if (ImGui::Button("Cancel") || shouldClose) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void SohInputEditorWindow::DrawDevicesTab() {
    if (ImGui::BeginTabItem("Devices")) {
        std::map<LUS::LUSDeviceIndex, std::pair<std::string, int32_t>> indexMappings;
        for (auto [lusIndex, mapping] : LUS::Context::GetInstance()
                                            ->GetControlDeck()
                                            ->GetDeviceIndexMappingManager()
                                            ->GetAllDeviceIndexMappingsFromConfig()) {
            auto sdlIndexMapping = std::static_pointer_cast<LUS::LUSDeviceIndexToSDLDeviceIndexMapping>(mapping);
            if (sdlIndexMapping == nullptr) {
                continue;
            }

            indexMappings[lusIndex] = { sdlIndexMapping->GetSDLControllerName(), -1 };
        }

        for (auto [lusIndex, mapping] : LUS::Context::GetInstance()
                                            ->GetControlDeck()
                                            ->GetDeviceIndexMappingManager()
                                            ->GetAllDeviceIndexMappings()) {
            auto sdlIndexMapping = std::static_pointer_cast<LUS::LUSDeviceIndexToSDLDeviceIndexMapping>(mapping);
            if (sdlIndexMapping == nullptr) {
                continue;
            }

            indexMappings[lusIndex] = { sdlIndexMapping->GetSDLControllerName(), sdlIndexMapping->GetSDLDeviceIndex() };
        }

        for (auto [lusIndex, info] : indexMappings) {
            auto [name, sdlIndex] = info;
            bool connected = sdlIndex != -1;

            auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
            auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
            GetButtonColorsForLUSDeviceIndex(lusIndex, buttonColor, buttonHoveredColor);
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
            ImGui::Button(
                StringHelper::Sprintf("%s %s (%s)", connected ? ICON_FA_GAMEPAD : ICON_FA_CHAIN_BROKEN, name.c_str(),
                                      connected ? StringHelper::Sprintf("SDL %d", sdlIndex).c_str() : "Disconnected")
                    .c_str());
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        }

        ImGui::EndTabItem();
    }
}
#endif

void SohInputEditorWindow::DrawElement() {
    ImGui::Begin("Controller Configuration###sohControllerConfigWindowV1", &mIsVisible);
    ImGui::BeginTabBar("##ControllerConfigPortTabs");
    DrawLinkTab();
    DrawIvanTab();
    if (CVarGetInteger("gDebugEnabled", 0)) {
        DrawDebugPortTab(2);
        DrawDebugPortTab(3);
    }
    DrawDevicesTab();
    ImGui::EndTabBar();
    ImGui::End();
}
