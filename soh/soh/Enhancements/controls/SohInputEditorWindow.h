#pragma once

#include "stdint.h"
#include <libultraship/libultraship.h>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <set>
#include <list>

typedef CONTROLLERBUTTONS_T N64ButtonMask;

typedef struct {
    const char* label;
    const char* cVarName;
    N64ButtonMask defaultBtn;
} CustomButtonMap;

class SohInputEditorWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    ~SohInputEditorWindow();

    void DrawButton(const char* label, int32_t n64Btn, int32_t currentPort, int32_t* btnReading);

    void DrawInputChip(const char* buttonName, ImVec4 color);
    void DrawAnalogPreview(const char* label, ImVec2 stick, float deadzone = 0, bool gyro = false);
    void DrawControllerSchema();
    bool TestingRumble();

  protected:
    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;

  private:
    void DrawStickDirectionLine(const char* axisDirectionName, uint8_t port, uint8_t stick, Ship::Direction direction,
                                ImVec4 color);
    void DrawButtonLine(const char* buttonName, uint8_t port, N64ButtonMask bitmask, ImVec4 color);
    void DrawButtonLineEditMappingButton(uint8_t port, N64ButtonMask bitmask, std::string id);
    void DrawButtonLineAddMappingButton(uint8_t port, N64ButtonMask bitmask);

    void DrawStickDirectionLineEditMappingButton(uint8_t port, uint8_t stick, Ship::Direction direction, std::string id);
    void DrawStickDirectionLineAddMappingButton(uint8_t port, uint8_t stick, Ship::Direction direction);
    void DrawStickSection(uint8_t port, uint8_t stick, int32_t id, ImVec4 color);

    void DrawRumbleSection(uint8_t port);
    void DrawRemoveRumbleMappingButton(uint8_t port, std::string id);
    void DrawAddRumbleMappingButton(uint8_t port);

    void DrawLEDSection(uint8_t port);
    void DrawRemoveLEDMappingButton(uint8_t port, std::string id);
    void DrawAddLEDMappingButton(uint8_t port);

    void DrawGyroSection(uint8_t port);
    void DrawRemoveGyroMappingButton(uint8_t port, std::string id);
    void DrawAddGyroMappingButton(uint8_t port);

    // Used together for an incomplete linked hash map implementation in order to
    // map button masks to their names and original mapping on N64
    std::list<std::pair<N64ButtonMask, const char*>> buttons;
    std::unordered_map<N64ButtonMask, decltype(buttons)::iterator> buttonNames;
    void addButtonName(N64ButtonMask mask, const char* name);
    void DrawMapping(CustomButtonMap& mapping, float labelWidth, N64ButtonMask excludedButtons);
    void DrawOcarinaControlPanel();
    void DrawCameraControlPanel();
    void DrawDpadControlPanel();

    int32_t mGameInputBlockTimer;
    int32_t mMappingInputBlockTimer;
    int32_t mRumbleTimer;
    std::shared_ptr<Ship::ControllerRumbleMapping> mRumbleMappingToTest;

    // mBitmaskToMappingIds[port][bitmask] = { id0, id1, ... }
    std::unordered_map<uint8_t, std::unordered_map<N64ButtonMask, std::vector<std::string>>> mBitmaskToMappingIds;

    // mStickDirectionToMappingIds[port][stick][direction] = { id0, id1, ... }
    std::unordered_map<uint8_t,
                       std::unordered_map<uint8_t, std::unordered_map<Ship::Direction, std::vector<std::string>>>>
        mStickDirectionToMappingIds;

    void UpdateBitmaskToMappingIds(uint8_t port);
    void UpdateStickDirectionToMappingIds(uint8_t port);

    void GetButtonColorsForLUSDeviceIndex(Ship::ShipDeviceIndex lusIndex, ImVec4& buttonColor,
                                          ImVec4& buttonHoveredColor);
    void DrawLinkTab();
    void DrawIvanTab();
    void DrawDebugPortTab(uint8_t portIndex, std::string customName = "");
    std::set<N64ButtonMask> mButtonsBitmasks;
    std::set<N64ButtonMask> mDpadBitmasks;
    std::set<N64ButtonMask> mModifierButtonsBitmasks;
    std::set<N64ButtonMask> mCustomOcarinaButtonsBitmasks;
    void DrawButtonDeviceIcons(uint8_t portIndex, std::set<N64ButtonMask> bitmasks);
    void DrawAnalogStickDeviceIcons(uint8_t portIndex, Ship::Stick stick);
    void DrawRumbleDeviceIcons(uint8_t portIndex);
    void DrawGyroDeviceIcons(uint8_t portIndex);
    void DrawLEDDeviceIcons(uint8_t portIndex);
    bool mInputEditorPopupOpen;
    void DrawSetDefaultsButton(uint8_t portIndex);
    void DrawClearAllButton(uint8_t portIndex);

    std::map<Ship::ShipDeviceIndex, bool> mDeviceIndexVisiblity;
    void DrawDeviceVisibilityButtons();
};
