#include "SohInputEditorWindow.h"
#include <utils/StringHelper.h>
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
    mModifierButtonsBitmasks = { BTN_CUSTOM_MODIFIER1, BTN_CUSTOM_MODIFIER2 };
    mCustomOcarinaButtonsBitmasks = {
        BTN_CUSTOM_OCARINA_NOTE_D4,
        BTN_CUSTOM_OCARINA_NOTE_F4,
        BTN_CUSTOM_OCARINA_NOTE_A4,
        BTN_CUSTOM_OCARINA_NOTE_B4,
        BTN_CUSTOM_OCARINA_NOTE_D5
    };

    addButtonName(BTN_A,		"A");
    addButtonName(BTN_B,		"B");
    addButtonName(BTN_CUP,		"C Up");
    addButtonName(BTN_CDOWN,	"C Down");
    addButtonName(BTN_CLEFT,	"C Left");
    addButtonName(BTN_CRIGHT,	"C Right");
    addButtonName(BTN_L,		"L");
    addButtonName(BTN_Z,		"Z");
    addButtonName(BTN_R,		"R");
    addButtonName(BTN_START,	"Start");
    addButtonName(BTN_DUP,		"D-pad up");
    addButtonName(BTN_DDOWN,	"D-pad down");
    addButtonName(BTN_DLEFT,	"D-pad left");
    addButtonName(BTN_DRIGHT,	"D-pad right");
    addButtonName(0,			"None");

    mDeviceIndexVisiblity.clear();
    mDeviceIndexVisiblity[Ship::ShipDeviceIndex::Keyboard] = true;
    mDeviceIndexVisiblity[Ship::ShipDeviceIndex::Blue] = true;
    for (auto index = 1; index < Ship::ShipDeviceIndex::Max; index++) {
        mDeviceIndexVisiblity[static_cast<Ship::ShipDeviceIndex>(index)] = false;
    }
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
        Ship::Context::GetInstance()->GetControlDeck()->BlockGameInput(INPUT_EDITOR_WINDOW_GAME_INPUT_BLOCK_ID);

        // continue to block input for a third of a second after getting the mapping
        mGameInputBlockTimer = ImGui::GetIO().Framerate / 3;

        if (mMappingInputBlockTimer != INT32_MAX) {
            mMappingInputBlockTimer--;
            if (mMappingInputBlockTimer <= 0) {
                mMappingInputBlockTimer = INT32_MAX;
            }
        }

        Ship::Context::GetInstance()->GetWindow()->GetGui()->BlockImGuiGamepadNavigation();
    } else {
        if (mGameInputBlockTimer != INT32_MAX) {
            mGameInputBlockTimer--;
            if (mGameInputBlockTimer <= 0) {
                Ship::Context::GetInstance()->GetControlDeck()->UnblockGameInput(
                    INPUT_EDITOR_WINDOW_GAME_INPUT_BLOCK_ID);
                mGameInputBlockTimer = INT32_MAX;
            }
        }

        if (Ship::Context::GetInstance()->GetWindow()->GetGui()->ImGuiGamepadNavigationEnabled()) {
            mMappingInputBlockTimer = ImGui::GetIO().Framerate / 3;
        } else {
            mMappingInputBlockTimer = INT32_MAX;
        }

        Ship::Context::GetInstance()->GetWindow()->GetGui()->UnblockImGuiGamepadNavigation();
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

void SohInputEditorWindow::GetButtonColorsForLUSDeviceIndex(Ship::ShipDeviceIndex lusIndex, ImVec4& buttonColor,
                                                            ImVec4& buttonHoveredColor) {
    switch (lusIndex) {
        case Ship::ShipDeviceIndex::Keyboard:
            buttonColor = BUTTON_COLOR_KEYBOARD_BEIGE;
            buttonHoveredColor = BUTTON_COLOR_KEYBOARD_BEIGE_HOVERED;
            break;
        case Ship::ShipDeviceIndex::Blue:
            buttonColor = BUTTON_COLOR_GAMEPAD_BLUE;
            buttonHoveredColor = BUTTON_COLOR_GAMEPAD_BLUE_HOVERED;
            break;
        case Ship::ShipDeviceIndex::Red:
            buttonColor = BUTTON_COLOR_GAMEPAD_RED;
            buttonHoveredColor = BUTTON_COLOR_GAMEPAD_RED_HOVERED;
            break;
        case Ship::ShipDeviceIndex::Orange:
            buttonColor = BUTTON_COLOR_GAMEPAD_ORANGE;
            buttonHoveredColor = BUTTON_COLOR_GAMEPAD_ORANGE_HOVERED;
            break;
        case Ship::ShipDeviceIndex::Green:
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

void SohInputEditorWindow::DrawButtonLineAddMappingButton(uint8_t port, N64ButtonMask bitmask) {
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
        if (mMappingInputBlockTimer == INT32_MAX && Ship::Context::GetInstance()
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

void SohInputEditorWindow::DrawButtonLineEditMappingButton(uint8_t port, N64ButtonMask bitmask, std::string id) {
    auto mapping = Ship::Context::GetInstance()
                       ->GetControlDeck()
                       ->GetControllerByPort(port)
                       ->GetButton(bitmask)
                       ->GetButtonMappingById(id);
    if (mapping == nullptr) {
        return;
    }
    if (!mDeviceIndexVisiblity[mapping->GetShipDeviceIndex()]) {
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
    GetButtonColorsForLUSDeviceIndex(mapping->GetShipDeviceIndex(), buttonColor, buttonHoveredColor);
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
        ImGui::SetTooltip("%s", mapping->GetPhysicalDeviceName().c_str());
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
        if (mMappingInputBlockTimer == INT32_MAX && Ship::Context::GetInstance()
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
    auto sdlAxisDirectionToButtonMapping = std::dynamic_pointer_cast<Ship::SDLAxisDirectionToButtonMapping>(mapping);
    auto indexMapping = Ship::Context::GetInstance()
                            ->GetControlDeck()
                            ->GetDeviceIndexMappingManager()
                            ->GetDeviceIndexMappingFromShipDeviceIndex(mapping->GetShipDeviceIndex());
    auto sdlIndexMapping = std::dynamic_pointer_cast<Ship::ShipDeviceIndexToSDLDeviceIndexMapping>(indexMapping);

    if (sdlIndexMapping != nullptr && sdlAxisDirectionToButtonMapping != nullptr) {
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
        auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        GetButtonColorsForLUSDeviceIndex(mapping->GetShipDeviceIndex(), buttonColor, buttonHoveredColor);
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
        Ship::Context::GetInstance()
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

void SohInputEditorWindow::DrawButtonLine(const char* buttonName, uint8_t port, N64ButtonMask bitmask,
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
                                                                  Ship::Direction direction) {
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
        if (stick == Ship::LEFT) {
            if (mMappingInputBlockTimer == INT32_MAX &&
                Ship::Context::GetInstance()
                    ->GetControlDeck()
                    ->GetControllerByPort(port)
                    ->GetLeftStick()
                    ->AddOrEditAxisDirectionMappingFromRawPress(direction, "")) {
                mInputEditorPopupOpen = false;
                ImGui::CloseCurrentPopup();
            }
        } else {
            if (mMappingInputBlockTimer == INT32_MAX &&
                Ship::Context::GetInstance()
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
                                                                   Ship::Direction direction, std::string id) {
    std::shared_ptr<Ship::ControllerAxisDirectionMapping> mapping = nullptr;
    if (stick == Ship::LEFT) {
        mapping = Ship::Context::GetInstance()
                      ->GetControlDeck()
                      ->GetControllerByPort(port)
                      ->GetLeftStick()
                      ->GetAxisDirectionMappingById(direction, id);
    } else {
        mapping = Ship::Context::GetInstance()
                      ->GetControlDeck()
                      ->GetControllerByPort(port)
                      ->GetRightStick()
                      ->GetAxisDirectionMappingById(direction, id);
    }

    if (mapping == nullptr) {
        return;
    }
    if (!mDeviceIndexVisiblity[mapping->GetShipDeviceIndex()]) {
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
    GetButtonColorsForLUSDeviceIndex(mapping->GetShipDeviceIndex(), buttonColor, buttonHoveredColor);
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
        ImGui::SetTooltip("%s", mapping->GetPhysicalDeviceName().c_str());
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

        if (stick == Ship::LEFT) {
            if (mMappingInputBlockTimer == INT32_MAX &&
                Ship::Context::GetInstance()
                    ->GetControlDeck()
                    ->GetControllerByPort(port)
                    ->GetLeftStick()
                    ->AddOrEditAxisDirectionMappingFromRawPress(direction, id)) {
                mInputEditorPopupOpen = false;
                ImGui::CloseCurrentPopup();
            }
        } else {
            if (mMappingInputBlockTimer == INT32_MAX &&
                Ship::Context::GetInstance()
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
        if (stick == Ship::LEFT) {
            Ship::Context::GetInstance()
                ->GetControlDeck()
                ->GetControllerByPort(port)
                ->GetLeftStick()
                ->ClearAxisDirectionMapping(direction, id);
        } else {
            Ship::Context::GetInstance()
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
                                                  Ship::Direction direction, ImVec4 color = CHIP_COLOR_N64_GREY) {
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
    std::shared_ptr<Ship::ControllerStick> controllerStick = nullptr;
    if (stick == Ship::LEFT) {
        controllerStick = Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetLeftStick();
    } else {
        controllerStick = Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetRightStick();
    }
    controllerStick->Process(sX, sY);
    DrawAnalogPreview(StringHelper::Sprintf("##AnalogPreview%d", id).c_str(), ImVec2(sX, sY));

    ImGui::SameLine();
    ImGui::BeginGroup();
    DrawStickDirectionLine(ICON_FA_ARROW_UP, port, stick, Ship::UP, color);
    DrawStickDirectionLine(ICON_FA_ARROW_DOWN, port, stick, Ship::DOWN, color);
    DrawStickDirectionLine(ICON_FA_ARROW_LEFT, port, stick, Ship::LEFT, color);
    DrawStickDirectionLine(ICON_FA_ARROW_RIGHT, port, stick, Ship::RIGHT, color);
    ImGui::EndGroup();
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode(StringHelper::Sprintf("Analog Stick Options##%d", id).c_str())) {
        ImGui::Text("Sensitivity:");

        int32_t sensitivityPercentage = controllerStick->GetSensitivityPercentage();
        if (sensitivityPercentage == 0) {
            ImGui::BeginDisabled();
        }
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button(StringHelper::Sprintf("-##Sensitivity%d", id).c_str())) {
            controllerStick->SetSensitivity(sensitivityPercentage - 1);
        }
        ImGui::PopButtonRepeat();
        if (sensitivityPercentage == 0) {
            ImGui::EndDisabled();
        }
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::SetNextItemWidth(SCALE_IMGUI_SIZE(160.0f));
        if (ImGui::SliderInt(StringHelper::Sprintf("##Sensitivity%d", id).c_str(), &sensitivityPercentage, 0, 200, "%d%%",
                             ImGuiSliderFlags_AlwaysClamp)) {
            controllerStick->SetSensitivity(sensitivityPercentage);
        }
        ImGui::SameLine(0.0f, 0.0f);
        if (sensitivityPercentage == 200) {
            ImGui::BeginDisabled();
        }
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button(StringHelper::Sprintf("+##Sensitivity%d", id).c_str())) {
            controllerStick->SetSensitivity(sensitivityPercentage + 1);
        }
        ImGui::PopButtonRepeat();
        if (sensitivityPercentage == 200) {
            ImGui::EndDisabled();
        }
        if (!controllerStick->SensitivityIsDefault()) {
            ImGui::SameLine();
            if (ImGui::Button(StringHelper::Sprintf("Reset to Default###resetStickSensitivity%d", id).c_str())) {
                controllerStick->ResetSensitivityToDefault();
            }
        }

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
         Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetAllButtons()) {
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
         { std::make_pair<uint8_t, std::shared_ptr<Ship::ControllerStick>>(
               Ship::LEFT, Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetLeftStick()),
           std::make_pair<uint8_t, std::shared_ptr<Ship::ControllerStick>>(
               Ship::RIGHT,
               Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetRightStick()) }) {
        for (auto direction : { Ship::LEFT, Ship::RIGHT, Ship::UP, Ship::DOWN }) {
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
        Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetRumble()->ClearRumbleMapping(id);
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

        if (mMappingInputBlockTimer == INT32_MAX && Ship::Context::GetInstance()
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
    for (auto [id, mapping] : Ship::Context::GetInstance()
                                  ->GetControlDeck()
                                  ->GetControllerByPort(port)
                                  ->GetRumble()
                                  ->GetAllRumbleMappings()) {
        ImGui::AlignTextToFramePadding();
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        GetButtonColorsForLUSDeviceIndex(mapping->GetShipDeviceIndex(), buttonColor, buttonHoveredColor);
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
        Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetLED()->ClearLEDMapping(id);
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

        if (mMappingInputBlockTimer == INT32_MAX && Ship::Context::GetInstance()
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

void SohInputEditorWindow::DrawLEDSection(uint8_t port) {
    for (auto [id, mapping] :
         Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetLED()->GetAllLEDMappings()) {
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
                UIWidgets::EnhancementCombobox(CVAR_SETTING("LEDColorSource"), ledSources, LED_SOURCE_TUNIC_ORIGINAL);
                UIWidgets::Tooltip("Health\n- Red when health critical (13-20% depending on max health)\n- Yellow when "
                                   "health < 40%. Green otherwise.\n\n"
                                   "Tunics: colors will mirror currently equipped tunic, whether original or the current "
                                   "values in Cosmetics Editor.\n\n"
                                   "Custom: single, solid color");
                if (CVarGetInteger(CVAR_SETTING("LEDColorSource"), 1) == LED_SOURCE_CUSTOM) {
                    UIWidgets::Spacer(3);
                    auto port1Color = CVarGetColor24(CVAR_SETTING("LEDPort1Color"), { 255, 255, 255 });
                    ImVec4 colorVec = { port1Color.r / 255.0f, port1Color.g / 255.0f, port1Color.b / 255.0f, 1.0f };
                    if (ImGui::ColorEdit3("", (float*)&colorVec,
                                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
                        Color_RGB8 color;
                        color.r = colorVec.x * 255.0;
                        color.g = colorVec.y * 255.0;
                        color.b = colorVec.z * 255.0;

                        CVarSetColor24(CVAR_SETTING("LEDPort1Color"), color);
                        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                    }
                    ImGui::SameLine();
                    ImGui::Text("Custom Color");
                }
                UIWidgets::PaddedEnhancementSliderFloat("Brightness: %.1f %%", "##LED_Brightness", CVAR_SETTING("LEDBrightness"), 0.0f,
                                                        1.0f, "", 1.0f, true, true);
                UIWidgets::Tooltip("Sets the brightness of controller LEDs. 0% brightness = LEDs off.");
                UIWidgets::PaddedEnhancementCheckbox(
                    "Critical Health Override", CVAR_SETTING("LEDCriticalOverride"), true, true,
                    CVarGetInteger(CVAR_SETTING("LEDColorSource"), LED_SOURCE_TUNIC_ORIGINAL) == LED_SOURCE_HEALTH,
                    "Override redundant for health source.", UIWidgets::CheckboxGraphics::Cross, true);
                UIWidgets::Tooltip("Shows red color when health is critical, otherwise displays according to color source.");
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
        Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetGyro()->ClearGyroMapping();
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

        if (mMappingInputBlockTimer == INT32_MAX && Ship::Context::GetInstance()
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
        Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(port)->GetGyro()->GetGyroMapping();
    if (mapping != nullptr) {
        auto id = mapping->GetGyroMappingId();
        ImGui::AlignTextToFramePadding();
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        ImGui::BulletText("%s", mapping->GetPhysicalDeviceName().c_str());
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

void SohInputEditorWindow::DrawButtonDeviceIcons(uint8_t portIndex, std::set<N64ButtonMask> bitmasks) {
    std::set<Ship::ShipDeviceIndex> allLusDeviceIndices;
    allLusDeviceIndices.insert(Ship::ShipDeviceIndex::Keyboard);
    for (auto [lusIndex, mapping] : Ship::Context::GetInstance()
                                        ->GetControlDeck()
                                        ->GetDeviceIndexMappingManager()
                                        ->GetAllDeviceIndexMappingsFromConfig()) {
        allLusDeviceIndices.insert(lusIndex);
    }

    std::vector<std::pair<Ship::ShipDeviceIndex, bool>> lusDeviceIndiciesWithMappings;
    for (auto lusIndex : allLusDeviceIndices) {
        for (auto [bitmask, button] :
             Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->GetAllButtons()) {
            if (!bitmasks.contains(bitmask)) {
                continue;
            }

            if (button->HasMappingsForShipDeviceIndex(lusIndex)) {
                for (auto [id, mapping] : button->GetAllButtonMappings()) {
                    if (mapping->GetShipDeviceIndex() == lusIndex) {
                        lusDeviceIndiciesWithMappings.push_back(
                            std::pair<Ship::ShipDeviceIndex, bool>(lusIndex, mapping->PhysicalDeviceIsConnected()));
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
        if (lusIndex == Ship::ShipDeviceIndex::Keyboard) {
            ImGui::SmallButton(ICON_FA_KEYBOARD_O);
        } else {
            ImGui::SmallButton(connected ? ICON_FA_GAMEPAD : ICON_FA_CHAIN_BROKEN);
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
}

void SohInputEditorWindow::DrawAnalogStickDeviceIcons(uint8_t portIndex, Ship::Stick stick) {
    std::set<Ship::ShipDeviceIndex> allLusDeviceIndices;
    allLusDeviceIndices.insert(Ship::ShipDeviceIndex::Keyboard);
    for (auto [lusIndex, mapping] : Ship::Context::GetInstance()
                                        ->GetControlDeck()
                                        ->GetDeviceIndexMappingManager()
                                        ->GetAllDeviceIndexMappingsFromConfig()) {
        allLusDeviceIndices.insert(lusIndex);
    }

    std::vector<std::pair<Ship::ShipDeviceIndex, bool>> lusDeviceIndiciesWithMappings;
    for (auto lusIndex : allLusDeviceIndices) {
        auto controllerStick =
            stick == Ship::Stick::LEFT_STICK
                ? Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->GetLeftStick()
                : Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->GetRightStick();
        if (controllerStick->HasMappingsForShipDeviceIndex(lusIndex)) {
            for (auto [direction, mappings] : controllerStick->GetAllAxisDirectionMappings()) {
                bool foundMapping = false;
                for (auto [id, mapping] : mappings) {
                    if (mapping->GetShipDeviceIndex() == lusIndex) {
                        foundMapping = true;
                        lusDeviceIndiciesWithMappings.push_back(
                            std::pair<Ship::ShipDeviceIndex, bool>(lusIndex, mapping->PhysicalDeviceIsConnected()));
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
        if (lusIndex == Ship::ShipDeviceIndex::Keyboard) {
            ImGui::SmallButton(ICON_FA_KEYBOARD_O);
        } else {
            ImGui::SmallButton(connected ? ICON_FA_GAMEPAD : ICON_FA_CHAIN_BROKEN);
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
}

void SohInputEditorWindow::DrawRumbleDeviceIcons(uint8_t portIndex) {
    std::set<Ship::ShipDeviceIndex> allLusDeviceIndices;
    for (auto [lusIndex, mapping] : Ship::Context::GetInstance()
                                        ->GetControlDeck()
                                        ->GetDeviceIndexMappingManager()
                                        ->GetAllDeviceIndexMappingsFromConfig()) {
        allLusDeviceIndices.insert(lusIndex);
    }

    std::vector<std::pair<Ship::ShipDeviceIndex, bool>> lusDeviceIndiciesWithMappings;
    for (auto lusIndex : allLusDeviceIndices) {
        if (Ship::Context::GetInstance()
                ->GetControlDeck()
                ->GetControllerByPort(portIndex)
                ->GetRumble()
                ->HasMappingsForShipDeviceIndex(lusIndex)) {
            for (auto [id, mapping] : Ship::Context::GetInstance()
                                          ->GetControlDeck()
                                          ->GetControllerByPort(portIndex)
                                          ->GetRumble()
                                          ->GetAllRumbleMappings()) {
                if (mapping->GetShipDeviceIndex() == lusIndex) {
                    lusDeviceIndiciesWithMappings.push_back(
                        std::pair<Ship::ShipDeviceIndex, bool>(lusIndex, mapping->PhysicalDeviceIsConnected()));
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
        Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->GetGyro()->GetGyroMapping();
    if (mapping == nullptr) {
        return;
    }

    auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
    GetButtonColorsForLUSDeviceIndex(mapping->GetShipDeviceIndex(), buttonColor, buttonHoveredColor);
    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
    ImGui::SameLine();
    ImGui::SmallButton(mapping->PhysicalDeviceIsConnected() ? ICON_FA_GAMEPAD : ICON_FA_CHAIN_BROKEN);
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}

void SohInputEditorWindow::DrawLEDDeviceIcons(uint8_t portIndex) {
    std::set<Ship::ShipDeviceIndex> allLusDeviceIndices;
    for (auto [lusIndex, mapping] : Ship::Context::GetInstance()
                                        ->GetControlDeck()
                                        ->GetDeviceIndexMappingManager()
                                        ->GetAllDeviceIndexMappingsFromConfig()) {
        allLusDeviceIndices.insert(lusIndex);
    }

    std::vector<std::pair<Ship::ShipDeviceIndex, bool>> lusDeviceIndiciesWithMappings;
    for (auto lusIndex : allLusDeviceIndices) {
        if (Ship::Context::GetInstance()
                ->GetControlDeck()
                ->GetControllerByPort(portIndex)
                ->GetRumble()
                ->HasMappingsForShipDeviceIndex(lusIndex)) {
            for (auto [id, mapping] : Ship::Context::GetInstance()
                                          ->GetControlDeck()
                                          ->GetControllerByPort(portIndex)
                                          ->GetLED()
                                          ->GetAllLEDMappings()) {
                if (mapping->GetShipDeviceIndex() == lusIndex) {
                    lusDeviceIndiciesWithMappings.push_back(
                        std::pair<Ship::ShipDeviceIndex, bool>(lusIndex, mapping->PhysicalDeviceIsConnected()));
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

const ImGuiTableFlags PANEL_TABLE_FLAGS =
    ImGuiTableFlags_BordersH |
    ImGuiTableFlags_BordersV;
const ImGuiTableColumnFlags PANEL_TABLE_COLUMN_FLAGS =
    ImGuiTableColumnFlags_IndentEnable |
    ImGuiTableColumnFlags_NoSort;

namespace TableHelper {
    void InitHeader(bool has_header = true) {
        if (has_header) {
            ImGui::TableHeadersRow();
        }
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding(); //This is to adjust Vertical pos of item in a cell to be normlized.
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    }

    void NextCol() {
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    }

    void NextLine() {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    }
}

void SohInputEditorWindow::addButtonName(N64ButtonMask mask, const char* name) {
    buttons.push_back(std::make_pair(mask, name));
    buttonNames[mask] = std::prev(buttons.end());
}

// Draw a button mapping setting consisting of a padded label and button dropdown.
// excludedButtons indicates which buttons are unavailable to choose from.
void SohInputEditorWindow::DrawMapping(CustomButtonMap& mapping, float labelWidth, N64ButtonMask excludedButtons) {
    N64ButtonMask currentButton = CVarGetInteger(mapping.cVarName, mapping.defaultBtn);

    const char* preview;
    if (buttonNames.contains(currentButton)) {
        preview = buttonNames[currentButton]->second;
    } else {
        preview = "Unknown";
    }

    UIWidgets::Spacer(0);
    ImVec2 cursorPos = ImGui::GetCursorPos();
    ImVec2 textSize = ImGui::CalcTextSize(mapping.label);
    ImGui::SetCursorPosY(cursorPos.y + textSize.y / 4);
    ImGui::SetCursorPosX(cursorPos.x + abs(textSize.x - labelWidth));
    ImGui::Text("%s", mapping.label);
    ImGui::SameLine();
    ImGui::SetCursorPosY(cursorPos.y);

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
        if (ImGui::BeginCombo(StringHelper::Sprintf("##%s", mapping.cVarName).c_str(), preview)) {
        for (auto i = buttons.begin(); i != buttons.end(); i++) {
            if ((i->first & excludedButtons) != 0) {
                continue;
            }
            if (ImGui::Selectable(i->second, i->first == currentButton)) {
                CVarSetInteger(mapping.cVarName, i->first);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            }
        }
        ImGui::EndCombo();
    }
    UIWidgets::Spacer(0);
}

void SohInputEditorWindow::DrawOcarinaControlPanel() {
    ImVec2 cursor = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(cursor.x + 24, cursor.y + 5));

    UIWidgets::EnhancementCheckbox("Customize Ocarina Controls", CVAR_SETTING("CustomOcarina.Enabled"));

    if (!CVarGetInteger(CVAR_SETTING("CustomOcarina.Enabled"), 0)) {
        ImGui::BeginDisabled();
    }

    ImGui::AlignTextToFramePadding();
    ImGui::BulletText("Notes");
    DrawButtonLine("A (D4)", 0, BTN_CUSTOM_OCARINA_NOTE_D4);
    DrawButtonLine(ICON_FA_ARROW_DOWN " (F4)", 0, BTN_CUSTOM_OCARINA_NOTE_F4);
    DrawButtonLine(ICON_FA_ARROW_RIGHT " (A4)", 0, BTN_CUSTOM_OCARINA_NOTE_A4);
    DrawButtonLine(ICON_FA_ARROW_LEFT " (B4)", 0, BTN_CUSTOM_OCARINA_NOTE_B4);
    DrawButtonLine(ICON_FA_ARROW_UP " (D5)", 0, BTN_CUSTOM_OCARINA_NOTE_D5);

    ImGui::AlignTextToFramePadding();
    ImGui::BulletText("Disable song detection");
    DrawButtonLine(ICON_FA_BAN, 0, BTN_CUSTOM_OCARINA_DISABLE_SONGS);

    ImGui::AlignTextToFramePadding();
    ImGui::BulletText("Pitch");
    DrawButtonLine(ICON_FA_ARROW_UP, 0, BTN_CUSTOM_OCARINA_PITCH_UP);
    DrawButtonLine(ICON_FA_ARROW_DOWN, 0, BTN_CUSTOM_OCARINA_PITCH_DOWN);

    if (!CVarGetInteger(CVAR_SETTING("CustomOcarina.Enabled"), 0)) {
        ImGui::EndDisabled();
    }
}

void SohInputEditorWindow::DrawCameraControlPanel() {
    ImVec2 cursor = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(cursor.x + 5, cursor.y + 5));
    Ship::GuiWindow::BeginGroupPanel("Aiming/First-Person Camera", ImGui::GetContentRegionAvail());
    UIWidgets::PaddedEnhancementCheckbox("Right Stick Aiming", CVAR_SETTING("Controls.RightStickAim"));
    UIWidgets::Tooltip("Allows for aiming with the right stick in:\n-First-Person/C-Up view\n-Weapon Aiming");
    if (CVarGetInteger(CVAR_SETTING("Controls.RightStickAim"), 0)) {
        UIWidgets::PaddedEnhancementCheckbox("Allow moving while in first person mode", CVAR_SETTING("MoveInFirstPerson"));
        UIWidgets::Tooltip("Changes the left stick to move the player while in first person mode");
    }
    UIWidgets::PaddedEnhancementCheckbox("Invert Aiming X Axis", CVAR_SETTING("Controls.InvertAimingXAxis"));
    UIWidgets::Tooltip("Inverts the Camera X Axis in:\n-First-Person/C-Up view\n-Weapon Aiming");
    UIWidgets::PaddedEnhancementCheckbox("Invert Aiming Y Axis", CVAR_SETTING("Controls.InvertAimingYAxis"), true, true, false, "", UIWidgets::CheckboxGraphics::Cross, true);
    UIWidgets::Tooltip("Inverts the Camera Y Axis in:\n-First-Person/C-Up view\n-Weapon Aiming");
    UIWidgets::PaddedEnhancementCheckbox("Invert Shield Aiming Y Axis", CVAR_SETTING("Controls.InvertShieldAimingYAxis"), true, true, false, "", UIWidgets::CheckboxGraphics::Cross, true);
    UIWidgets::Tooltip("Inverts the Shield Aiming Y Axis");
    UIWidgets::PaddedEnhancementCheckbox("Invert Shield Aiming X Axis", CVAR_SETTING("Controls.InvertShieldAimingYAxis"));
    UIWidgets::Tooltip("Inverts the Shield Aiming X Axis");
    UIWidgets::PaddedEnhancementCheckbox("Invert Z-Weapon Aiming Y Axis", CVAR_SETTING("Controls.InvertZAimingYAxis"), true, true, false, "", UIWidgets::CheckboxGraphics::Cross, true);
    UIWidgets::Tooltip("Inverts the Camera Y Axis in:\n-Z-Weapon Aiming");
    UIWidgets::PaddedEnhancementCheckbox("Disable Auto-Centering in First-Person View", CVAR_SETTING("DisableFirstPersonAutoCenterView"));
    UIWidgets::Tooltip("Prevents the C-Up view from auto-centering, allowing for Gyro Aiming");
    if (UIWidgets::PaddedEnhancementCheckbox("Enable Custom Aiming/First-Person sensitivity", CVAR_SETTING("FirstPersonCameraSensitivity.Enabled"), true, false)) {
        if (!CVarGetInteger(CVAR_SETTING("FirstPersonCameraSensitivity.Enabled"), 0)) {
            CVarClear(CVAR_SETTING("FirstPersonCameraSensitivity.X"));
            CVarClear(CVAR_SETTING("FirstPersonCameraSensitivity.Y"));
        }
    }
    if (CVarGetInteger(CVAR_SETTING("FirstPersonCameraSensitivity.Enabled"), 0)) {
        UIWidgets::EnhancementSliderFloat("Aiming/First-Person Horizontal Sensitivity: %.0f %%", "##FirstPersonSensitivity Horizontal",
                                            CVAR_SETTING("FirstPersonCameraSensitivity.X"), 0.01f, 5.0f, "", 1.0f, true);
        UIWidgets::EnhancementSliderFloat("Aiming/First-Person Vertical Sensitivity: %.0f %%", "##FirstPersonSensitivity Vertical",
                                          CVAR_SETTING("FirstPersonCameraSensitivity.Y"), 0.01f, 5.0f, "", 1.0f, true);
    }
    UIWidgets::Spacer(0);
    Ship::GuiWindow::EndGroupPanel(0);

    UIWidgets::Spacer(0);
    cursor = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(cursor.x + 5, cursor.y + 5));
    Ship::GuiWindow::BeginGroupPanel("Third-Person Camera", ImGui::GetContentRegionAvail());

    UIWidgets::PaddedEnhancementCheckbox("Free Look", CVAR_SETTING("FreeLook.Enabled"));
    UIWidgets::Tooltip("Enables free look camera control\nNote: You must remap C buttons off of the right stick in the "
                            "controller config menu, and map the camera stick to the right stick.");
    UIWidgets::PaddedEnhancementCheckbox("Invert Camera X Axis", CVAR_SETTING("FreeLook.InvertXAxis"));
    UIWidgets::Tooltip("Inverts the Camera X Axis in:\n-Free look");
    UIWidgets::PaddedEnhancementCheckbox("Invert Camera Y Axis", CVAR_SETTING("FreeLook.InvertYAxis"), true, true, false, "", UIWidgets::CheckboxGraphics::Cross, true);
    UIWidgets::Tooltip("Inverts the Camera Y Axis in:\n-Free look");
    UIWidgets::Spacer(0);
    UIWidgets::PaddedEnhancementSliderFloat("Third-Person Horizontal Sensitivity: %.0f %%", "##ThirdPersonSensitivity Horizontal",
                                            CVAR_SETTING("FreeLook.CameraSensitivity.X"), 0.01f, 5.0f, "", 1.0f, true, true, false, true);
    UIWidgets::PaddedEnhancementSliderFloat("Third-Person Vertical Sensitivity: %.0f %%", "##ThirdPersonSensitivity Vertical",
                                            CVAR_SETTING("FreeLook.CameraSensitivity.Y"), 0.01f, 5.0f, "", 1.0f, true, true, false, true);
    UIWidgets::PaddedEnhancementSliderInt("Camera Distance: %d", "##CamDist",
                                    CVAR_SETTING("FreeLook.MaxCameraDistance"), 100, 900, "", 185, true, false, true);
    UIWidgets::PaddedEnhancementSliderInt("Camera Transition Speed: %d", "##CamTranSpeed",
                                    CVAR_SETTING("FreeLook.TransitionSpeed"), 0, 900, "", 25, true, false, true);
    Ship::GuiWindow::EndGroupPanel(0);
}

void SohInputEditorWindow::DrawDpadControlPanel() {
    ImVec2 cursor = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(cursor.x + 5, cursor.y + 5));
    Ship::GuiWindow::BeginGroupPanel("D-Pad Options", ImGui::GetContentRegionAvail());
    UIWidgets::PaddedEnhancementCheckbox("D-pad Support on Pause Screen", CVAR_SETTING("DPadOnPause"));
    UIWidgets::Tooltip("Navigate Pause with the D-pad\nIf used with \"D-pad as Equip Items\", you must hold C-Up to equip instead of navigate");
    UIWidgets::PaddedEnhancementCheckbox("D-pad Support in Text Boxes", CVAR_SETTING("DpadInText"));
    UIWidgets::Tooltip("Navigate choices in text boxes, shop item selection, and the file select / name entry screens with the D-pad");

    if (!CVarGetInteger(CVAR_SETTING("DPadOnPause"), 0) && !CVarGetInteger(CVAR_SETTING("DpadInText"), 0)) {
        ImGui::BeginDisabled();
    }

    UIWidgets::PaddedEnhancementCheckbox("D-pad hold change", CVAR_SETTING("DpadHoldChange"), true, true, false, "", UIWidgets::CheckboxGraphics::Cross, true);
    UIWidgets::Tooltip("The cursor will only move a single space no matter how long a D-pad direction is held");

    if (!CVarGetInteger(CVAR_SETTING("DPadOnPause"), 0) && !CVarGetInteger(CVAR_SETTING("DpadInText"), 0)) {
        ImGui::EndDisabled();
    }

    Ship::GuiWindow::EndGroupPanel(0);
}

void SohInputEditorWindow::DrawDeviceVisibilityButtons() {
    std::map<Ship::ShipDeviceIndex, std::pair<std::string, int32_t>> indexMappings;
    for (auto [lusIndex, mapping] : Ship::Context::GetInstance()
                                        ->GetControlDeck()
                                        ->GetDeviceIndexMappingManager()
                                        ->GetAllDeviceIndexMappingsFromConfig()) {
        auto sdlIndexMapping = std::static_pointer_cast<Ship::ShipDeviceIndexToSDLDeviceIndexMapping>(mapping);
        if (sdlIndexMapping == nullptr) {
            continue;
        }

        indexMappings[lusIndex] = { sdlIndexMapping->GetSDLControllerName(), -1 };
    }

    for (auto [lusIndex, mapping] : Ship::Context::GetInstance()
                                        ->GetControlDeck()
                                        ->GetDeviceIndexMappingManager()
                                        ->GetAllDeviceIndexMappings()) {
        auto sdlIndexMapping = std::static_pointer_cast<Ship::ShipDeviceIndexToSDLDeviceIndexMapping>(mapping);
        if (sdlIndexMapping == nullptr) {
            continue;
        }

        indexMappings[lusIndex] = { sdlIndexMapping->GetSDLControllerName(), sdlIndexMapping->GetSDLDeviceIndex() };
    }

    auto keyboardButtonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    auto keyboardButtonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
    GetButtonColorsForLUSDeviceIndex(Ship::ShipDeviceIndex::Keyboard, keyboardButtonColor, keyboardButtonHoveredColor);
    ImGui::PushStyleColor(ImGuiCol_Button, keyboardButtonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, keyboardButtonHoveredColor);
    bool keyboardVisible = mDeviceIndexVisiblity[Ship::ShipDeviceIndex::Keyboard];
    if(ImGui::Button(
        StringHelper::Sprintf("%s %s Keyboard", keyboardVisible ? ICON_FA_EYE : ICON_FA_EYE_SLASH, ICON_FA_KEYBOARD_O)
            .c_str())) {
        mDeviceIndexVisiblity[Ship::ShipDeviceIndex::Keyboard] = !keyboardVisible;
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();


    for (auto [lusIndex, info] : indexMappings) {
        auto [name, sdlIndex] = info;
        bool connected = sdlIndex != -1;

        auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        auto buttonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        GetButtonColorsForLUSDeviceIndex(lusIndex, buttonColor, buttonHoveredColor);

        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
        bool visible = mDeviceIndexVisiblity[lusIndex];
        if(ImGui::Button(
            StringHelper::Sprintf("%s %s %s (%s)", visible ? ICON_FA_EYE : ICON_FA_EYE_SLASH, connected ? ICON_FA_GAMEPAD : ICON_FA_CHAIN_BROKEN, name.c_str(),
                                    connected ? StringHelper::Sprintf("SDL %d", sdlIndex).c_str() : "Disconnected")
                .c_str())) {
            mDeviceIndexVisiblity[lusIndex] = !visible;
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
}

void SohInputEditorWindow::DrawLinkTab() {
    uint8_t portIndex = 0;
    if (ImGui::BeginTabItem(StringHelper::Sprintf("Link (P1)###port%d", portIndex).c_str())) {
        DrawClearAllButton(portIndex);
        DrawSetDefaultsButton(portIndex);
        DrawDeviceVisibilityButtons();

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
            DrawAnalogStickDeviceIcons(portIndex, Ship::LEFT_STICK);
            DrawStickSection(portIndex, Ship::LEFT, 0);
        } else {
            DrawAnalogStickDeviceIcons(portIndex, Ship::LEFT_STICK);
        }

        if (ImGui::CollapsingHeader("Additional (\"Right\") Stick")) {
            DrawAnalogStickDeviceIcons(portIndex, Ship::RIGHT_STICK);
            DrawStickSection(portIndex, Ship::RIGHT, 1, CHIP_COLOR_N64_YELLOW);
        } else {
            DrawAnalogStickDeviceIcons(portIndex, Ship::RIGHT_STICK);
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
            DrawButtonLine("M1", portIndex, BTN_CUSTOM_MODIFIER1);
            DrawButtonLine("M2", portIndex, BTN_CUSTOM_MODIFIER2);

            ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
            UIWidgets::PaddedEnhancementCheckbox("Enable speed modifiers", CVAR_SETTING("WalkModifier.Enabled"), true, false);
            UIWidgets::Tooltip("Hold the assigned button to change the maximum walking or swimming speed");
            if (CVarGetInteger(CVAR_SETTING("WalkModifier.Enabled"), 0)) {
                UIWidgets::Spacer(5);
                Ship::GuiWindow::BeginGroupPanel("Speed Modifier", ImGui::GetContentRegionAvail());
                UIWidgets::PaddedEnhancementCheckbox("Toggle modifier instead of holding",
                                                     CVAR_SETTING("WalkModifier.SpeedToggle"), true, false);
                Ship::GuiWindow::BeginGroupPanel("Walk Modifier", ImGui::GetContentRegionAvail());
                UIWidgets::PaddedEnhancementCheckbox("Don't affect jump distance/velocity",
                                                     CVAR_SETTING("WalkModifier.DoesntChangeJump"), true, false);
                UIWidgets::PaddedEnhancementSliderFloat("Walk Modifier 1: %.0f %%", "##WalkMod1",
                                                        CVAR_SETTING("WalkModifier.Mapping1"), 0.0f, 5.0f, "", 1.0f,
                                                        true, true, false, true);
                UIWidgets::PaddedEnhancementSliderFloat("Walk Modifier 2: %.0f %%", "##WalkMod2",
                                                        CVAR_SETTING("WalkModifier.Mapping2"), 0.0f, 5.0f, "", 1.0f,
                                                        true, true, false, true);
                Ship::GuiWindow::EndGroupPanel(0);
                Ship::GuiWindow::BeginGroupPanel("Swim Modifier", ImGui::GetContentRegionAvail());
                UIWidgets::PaddedEnhancementSliderFloat("Swim Modifier 1: %.0f %%", "##SwimMod1",
                                                        CVAR_SETTING("WalkModifier.SwimMapping1"), 0.0f, 5.0f, "", 1.0f,
                                                        true, true, false, true);
                UIWidgets::PaddedEnhancementSliderFloat("Swim Modifier 2: %.0f %%", "##SwimMod2",
                                                        CVAR_SETTING("WalkModifier.SwimMapping2"), 0.0f, 5.0f, "", 1.0f,
                                                        true, true, false, true);
                Ship::GuiWindow::EndGroupPanel(0);
                Ship::GuiWindow::EndGroupPanel(0);
            }
            ImGui::EndDisabled();
        } else {
            DrawButtonDeviceIcons(portIndex, mModifierButtonsBitmasks);
        }

        if (ImGui::CollapsingHeader("Ocarina Controls")) {
            DrawButtonDeviceIcons(portIndex, mCustomOcarinaButtonsBitmasks);
            DrawOcarinaControlPanel();
        } else {
            DrawButtonDeviceIcons(portIndex, mCustomOcarinaButtonsBitmasks);
        }

        if (ImGui::CollapsingHeader("Camera Controls")) {
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            DrawCameraControlPanel();
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.133f, 0.133f, 0.133f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        }

        if (ImGui::CollapsingHeader("D-Pad Controls")) {
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            DrawDpadControlPanel();
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.133f, 0.133f, 0.133f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::EndTabItem();
    }
}

void SohInputEditorWindow::DrawIvanTab() {
    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugEnabled"), 0)) {
        DrawDebugPortTab(1, "Ivan (P2)");
        return;
    }

    uint8_t portIndex = 1;
    if (ImGui::BeginTabItem(StringHelper::Sprintf("Ivan (P2)###port%d", portIndex).c_str())) {
        DrawClearAllButton(portIndex);
        DrawSetDefaultsButton(portIndex);
        DrawDeviceVisibilityButtons();

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
            DrawAnalogStickDeviceIcons(portIndex, Ship::LEFT_STICK);
            DrawStickSection(portIndex, Ship::LEFT, 0);
        } else {
            DrawAnalogStickDeviceIcons(portIndex, Ship::LEFT_STICK);
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
        DrawDeviceVisibilityButtons();

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
            DrawAnalogStickDeviceIcons(portIndex, Ship::LEFT_STICK);
            DrawStickSection(portIndex, Ship::LEFT, 0);
        } else {
            DrawAnalogStickDeviceIcons(portIndex, Ship::LEFT_STICK);
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
            Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->ClearAllMappings();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void SohInputEditorWindow::DrawSetDefaultsButton(uint8_t portIndex) {
    ImGui::SameLine();
    auto popupId = StringHelper::Sprintf("setDefaultsPopup##%d", portIndex);
    if (ImGui::Button(StringHelper::Sprintf("Set Defaults##%d", portIndex).c_str(),
                      ImVec2(ImGui::CalcTextSize("Set Defaults") * 2))) {
        ImGui::OpenPopup(popupId.c_str());
    }

    if (ImGui::BeginPopup(popupId.c_str())) {
        std::map<Ship::ShipDeviceIndex, std::pair<std::string, int32_t>> indexMappings;
        for (auto [lusIndex, mapping] : Ship::Context::GetInstance()
                                            ->GetControlDeck()
                                            ->GetDeviceIndexMappingManager()
                                            ->GetAllDeviceIndexMappings()) {
            auto sdlIndexMapping = std::static_pointer_cast<Ship::ShipDeviceIndexToSDLDeviceIndexMapping>(mapping);
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
                Ship::Context::GetInstance()
                    ->GetControlDeck()
                    ->GetControllerByPort(portIndex)
                    ->ClearAllMappingsForDevice(Ship::ShipDeviceIndex::Keyboard);
                Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->AddDefaultMappings(
                    Ship::ShipDeviceIndex::Keyboard);
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
                    Ship::Context::GetInstance()
                        ->GetControlDeck()
                        ->GetControllerByPort(portIndex)
                        ->ClearAllMappingsForDevice(lusIndex);
                    Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(portIndex)->AddDefaultMappings(
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

void SohInputEditorWindow::DrawElement() {
    ImGui::BeginTabBar("##ControllerConfigPortTabs");
    DrawLinkTab();
    DrawIvanTab();
    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugEnabled"), 0)) {
        DrawDebugPortTab(2);
        DrawDebugPortTab(3);
    }
    ImGui::EndTabBar();
}
