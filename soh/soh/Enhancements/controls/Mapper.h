#pragma once

// Built-in SDL gamepad mapper.
//
// Produces `gamecontrollerdb.txt`-style mapping strings for a raw SDL joystick and persists them to
// `usergamepadmappings.txt` in the app directory. That file is loaded at boot (and whenever it changes on
// disk) on top of the bundled `gamecontrollerdb.txt`, so a user-authored mapping always wins.
//
// The binding state machine (axis commit/return distances, binding override rules, mapping string layout)
// is adapted from https://github.com/General-Arcade/sdl2-gamepad-tool (MIT).

#include <cstdint>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <imgui.h>
#include <ship/window/gui/GuiWindow.h>

namespace Mapper {
enum AxisBinding {
    AXIS_LEFTX_NEGATIVE,
    AXIS_LEFTX_POSITIVE,
    AXIS_LEFTY_NEGATIVE,
    AXIS_LEFTY_POSITIVE,
    AXIS_RIGHTX_NEGATIVE,
    AXIS_RIGHTX_POSITIVE,
    AXIS_RIGHTY_NEGATIVE,
    AXIS_RIGHTY_POSITIVE,
    AXIS_TRIGGERLEFT,
    AXIS_TRIGGERRIGHT,
    AXIS_BINDING_MAX,
};

constexpr int32_t BINDING_COUNT = SDL_CONTROLLER_BUTTON_MAX + AXIS_BINDING_MAX;

struct ExtendedBind {
    SDL_GameControllerBindType bindType;
    union {
        int32_t button;

        struct {
            int32_t axis;
            int32_t axisMin;
            int32_t axisMax;
        } axis;

        struct {
            int32_t hat;
            int32_t hatMask;
        } hat;
    } value;
    bool committed;
};

struct AxisState {
    bool moving;
    int32_t lastValue;
    int32_t startingValue;
    int32_t farthestValue;
};

class MappingSession {
  public:
    static constexpr int32_t AXIS_COMMIT_DISTANCE = 16000;
    static constexpr int32_t AXIS_RETURN_DISTANCE = 10000;
    static constexpr uint32_t PENDING_ADVANCE_DELAY_MS = 100;

    MappingSession();

    void Start(SDL_Joystick* joystick, const std::vector<int32_t>& order);
    void Stop();

    bool IsActive() const;
    bool IsComplete() const;

    int32_t CurrentStep() const;
    int32_t StepCount() const;
    int32_t CurrentBinding() const;

    void GoToStep(int32_t step);
    void Advance();
    void GoBack();

    void ProcessButtonDown(int32_t button);
    void ProcessHatMotion(int32_t hat, int32_t value);
    void ProcessAxisMotion(int32_t axis, int32_t value, SDL_Joystick* joystick);

    void CheckPendingAdvance();

    const ExtendedBind& GetBinding(int32_t index) const;
    void SetBindings(const ExtendedBind (&bindings)[BINDING_COUNT]);
    void CopyBindingsTo(ExtendedBind (&out)[BINDING_COUNT]) const;

  private:
    void ConfigureBinding(const ExtendedBind& binding);
    bool BindingContainsBinding(const ExtendedBind& a, const ExtendedBind& b) const;

    ExtendedBind mBindings[BINDING_COUNT];
    std::vector<int32_t> mOrder;
    std::vector<AxisState> mAxisState;
    int32_t mCurrentStep;
    uint32_t mPendingAdvanceTime;
    bool mActive;
    bool mComplete;
};

void ClearBindings(ExtendedBind (&bindings)[BINDING_COUNT]);
bool HasAnyBinding(const ExtendedBind (&bindings)[BINDING_COUNT]);
std::string GenerateMappingString(SDL_Joystick* joystick, const ExtendedBind (&bindings)[BINDING_COUNT],
                                  const std::string& nameOverride = "");
int32_t ParseMappingString(const std::string& mapping, ExtendedBind (&bindings)[BINDING_COUNT]);
std::string DescribeBind(const ExtendedBind& bind);
const char* BindingName(int32_t binding);
std::string GetUserMappingsPath();
std::string FindUserMapping(const std::string& guid);
std::string MappingName(const std::string& mapping);
bool LoadUserMappings();
bool SaveUserMapping(const std::string& mapping);
bool DeleteUserMapping(const std::string& guid);

class MapperWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    ~MapperWindow();

  protected:
    void InitElement() override;
    void UpdateElement() override;
    void DrawElement() override;

  private:
    struct DeviceInfo {
        int32_t deviceIndex;
        SDL_JoystickID instanceId;
        std::string name;
        std::string guid;
        bool hasMapping;
        bool hasUserMapping;
    };

    void RefreshDeviceList();
    void SelectDevice(int32_t listIndex);
    void CloseJoystick();
    void LoadBindingsForDevice();

    void StartSession(const std::vector<int32_t>& order);
    void EndSession(bool applyResult);

    void PollDeviceForSession();
    void SnapshotDeviceState();

    void DrawDeviceSelector();
    void DrawDiagram();
    void DrawSessionPrompt();
    void DrawActionButtons();
    void DrawMappingStringPreview();

    bool IsBindingLive(int32_t binding) const;

    std::vector<DeviceInfo> mDevices;
    int32_t mSelectedDevice = -1;
    SDL_Joystick* mJoystick = nullptr;

    ExtendedBind mBindings[BINDING_COUNT] = {};
    MappingSession mSession;
    bool mDirty = false;
    bool mIncludeExtraButtons = false;
    std::string mStatusMessage;
    std::string mCustomName;

    std::vector<uint8_t> mPreviousButtons;
    std::vector<uint8_t> mPreviousHats;

    int32_t mGameInputBlockTimer = INT32_MAX;
    uint32_t mLastDeviceScanTicks = 0;
    uint64_t mLastUserFileWriteTime = 0;
    uint32_t mLastUserFileCheckTicks = 0;
};

} // namespace Mapper
