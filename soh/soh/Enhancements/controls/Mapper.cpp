#include "Mapper.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <utility>

#include <spdlog/spdlog.h>

#include <ship/Context.h>
#include <ship/controller/controldeck/ControlDeck.h>
#include <ship/resource/ResourceManager.h>
#include <ship/resource/archive/ArchiveManager.h>
#include <ship/utils/StringHelper.h>
#include <ship/window/Window.h>
#include <ship/window/gui/Gui.h>

#include "soh/OTRGlobals.h"
#include "soh/SohGui/SohGui.hpp"
#include "soh/SohGui/SohMenu.h"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/cvar_prefixes.h"

#include <fast/Fast3dGui.h>

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
}

namespace Mapper {

using namespace UIWidgets;

static const char* kUserMappingsFileName = "usergamepadmappings.txt";

static const char* kDiagramTextureName = "Gamepad-Mapper-Diagram";
static const char* kDiagramTexturePath = "textures/mapping-xbox.png";

// ---------------------------------------------------------------------------------------------------
// Binding metadata
// ---------------------------------------------------------------------------------------------------

static const char* kButtonNames[SDL_CONTROLLER_BUTTON_MAX] = {
    "A",
    "B",
    "X",
    "Y",
    "Back",
    "Guide",
    "Start",
    "Left Stick Click",
    "Right Stick Click",
    "Left Bumper",
    "Right Bumper",
    "D-Pad Up",
    "D-Pad Down",
    "D-Pad Left",
    "D-Pad Right",
    "Misc 1",
    "Paddle 1",
    "Paddle 2",
    "Paddle 3",
    "Paddle 4",
    "Touchpad",
};

static const char* kAxisNames[AXIS_BINDING_MAX] = {
    "Left Stick Left",   "Left Stick Right", "Left Stick Up",    "Left Stick Down", "Right Stick Left",
    "Right Stick Right", "Right Stick Up",   "Right Stick Down", "Left Trigger",    "Right Trigger",
};

const char* BindingName(int32_t binding) {
    if (binding < 0 || binding >= BINDING_COUNT) {
        return "?";
    }
    if (binding < SDL_CONTROLLER_BUTTON_MAX) {
        return kButtonNames[binding];
    }
    return kAxisNames[binding - SDL_CONTROLLER_BUTTON_MAX];
}

struct StickAxisPair {
    SDL_GameControllerAxis axis;
    int32_t negative;
    int32_t positive;
};

static const StickAxisPair kStickAxes[] = {
    { SDL_CONTROLLER_AXIS_LEFTX, AXIS_LEFTX_NEGATIVE, AXIS_LEFTX_POSITIVE },
    { SDL_CONTROLLER_AXIS_LEFTY, AXIS_LEFTY_NEGATIVE, AXIS_LEFTY_POSITIVE },
    { SDL_CONTROLLER_AXIS_RIGHTX, AXIS_RIGHTX_NEGATIVE, AXIS_RIGHTX_POSITIVE },
    { SDL_CONTROLLER_AXIS_RIGHTY, AXIS_RIGHTY_NEGATIVE, AXIS_RIGHTY_POSITIVE },
};

static const int32_t kCoreBindingOrder[] = {
    SDL_CONTROLLER_BUTTON_A,
    SDL_CONTROLLER_BUTTON_B,
    SDL_CONTROLLER_BUTTON_X,
    SDL_CONTROLLER_BUTTON_Y,
    SDL_CONTROLLER_BUTTON_BACK,
    SDL_CONTROLLER_BUTTON_START,
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
    SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
    SDL_CONTROLLER_BUTTON_LEFTSTICK,
    SDL_CONTROLLER_BUTTON_RIGHTSTICK,
    SDL_CONTROLLER_BUTTON_DPAD_UP,
    SDL_CONTROLLER_BUTTON_DPAD_DOWN,
    SDL_CONTROLLER_BUTTON_DPAD_LEFT,
    SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
    SDL_CONTROLLER_BUTTON_MAX + AXIS_LEFTX_NEGATIVE,
    SDL_CONTROLLER_BUTTON_MAX + AXIS_LEFTX_POSITIVE,
    SDL_CONTROLLER_BUTTON_MAX + AXIS_LEFTY_NEGATIVE,
    SDL_CONTROLLER_BUTTON_MAX + AXIS_LEFTY_POSITIVE,
    SDL_CONTROLLER_BUTTON_MAX + AXIS_RIGHTX_NEGATIVE,
    SDL_CONTROLLER_BUTTON_MAX + AXIS_RIGHTX_POSITIVE,
    SDL_CONTROLLER_BUTTON_MAX + AXIS_RIGHTY_NEGATIVE,
    SDL_CONTROLLER_BUTTON_MAX + AXIS_RIGHTY_POSITIVE,
    SDL_CONTROLLER_BUTTON_MAX + AXIS_TRIGGERLEFT,
    SDL_CONTROLLER_BUTTON_MAX + AXIS_TRIGGERRIGHT,
};

static const int32_t kExtraBindings[] = {
    SDL_CONTROLLER_BUTTON_GUIDE,    SDL_CONTROLLER_BUTTON_MISC1,   SDL_CONTROLLER_BUTTON_PADDLE1,
    SDL_CONTROLLER_BUTTON_PADDLE2,  SDL_CONTROLLER_BUTTON_PADDLE3, SDL_CONTROLLER_BUTTON_PADDLE4,
    SDL_CONTROLLER_BUTTON_TOUCHPAD,
};

// ---------------------------------------------------------------------------------------------------
// Binding helpers
// ---------------------------------------------------------------------------------------------------

static ExtendedBind NoBind() {
    ExtendedBind bind = {};
    bind.bindType = SDL_CONTROLLER_BINDTYPE_NONE;
    return bind;
}

void ClearBindings(ExtendedBind (&bindings)[BINDING_COUNT]) {
    for (int32_t i = 0; i < BINDING_COUNT; i++) {
        bindings[i] = NoBind();
    }
}

bool HasAnyBinding(const ExtendedBind (&bindings)[BINDING_COUNT]) {
    for (int32_t i = 0; i < BINDING_COUNT; i++) {
        if (bindings[i].bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
            return true;
        }
    }
    return false;
}

static bool IsFullRangeAxis(const ExtendedBind& bind) {
    if (bind.bindType != SDL_CONTROLLER_BINDTYPE_AXIS) {
        return false;
    }
    return (bind.value.axis.axisMin == SDL_JOYSTICK_AXIS_MIN && bind.value.axis.axisMax == SDL_JOYSTICK_AXIS_MAX) ||
           (bind.value.axis.axisMin == SDL_JOYSTICK_AXIS_MAX && bind.value.axis.axisMax == SDL_JOYSTICK_AXIS_MIN);
}

static bool MergeAxisBindings(ExtendedBind (&bindings)[BINDING_COUNT], int32_t negative, int32_t positive) {
    ExtendedBind& a = bindings[negative];
    ExtendedBind& b = bindings[positive];
    if (a.bindType == SDL_CONTROLLER_BINDTYPE_AXIS && b.bindType == SDL_CONTROLLER_BINDTYPE_AXIS &&
        a.value.axis.axis == b.value.axis.axis && a.value.axis.axisMin == b.value.axis.axisMin) {
        a.value.axis.axisMin = a.value.axis.axisMax;
        a.value.axis.axisMax = b.value.axis.axisMax;
        b = NoBind();
        return true;
    }
    return false;
}

static std::string EncodeBindElement(const ExtendedBind& bind) {
    switch (bind.bindType) {
        case SDL_CONTROLLER_BINDTYPE_BUTTON:
            return StringHelper::Sprintf("b%d", bind.value.button);
        case SDL_CONTROLLER_BINDTYPE_AXIS:
            if (bind.value.axis.axisMin == 0 && bind.value.axis.axisMax == SDL_JOYSTICK_AXIS_MIN) {
                return StringHelper::Sprintf("-a%d", bind.value.axis.axis);
            }
            if (bind.value.axis.axisMin == 0 && bind.value.axis.axisMax == SDL_JOYSTICK_AXIS_MAX) {
                return StringHelper::Sprintf("+a%d", bind.value.axis.axis);
            }
            return StringHelper::Sprintf("a%d%s", bind.value.axis.axis,
                                         bind.value.axis.axisMin > bind.value.axis.axisMax ? "~" : "");
        case SDL_CONTROLLER_BINDTYPE_HAT:
            return StringHelper::Sprintf("h%d.%d", bind.value.hat.hat, bind.value.hat.hatMask);
        default:
            return "";
    }
}

static const char* HatMaskName(int32_t mask) {
    switch (mask) {
        case SDL_HAT_UP:
            return "Up";
        case SDL_HAT_DOWN:
            return "Down";
        case SDL_HAT_LEFT:
            return "Left";
        case SDL_HAT_RIGHT:
            return "Right";
        case SDL_HAT_LEFTUP:
            return "Up-Left";
        case SDL_HAT_RIGHTUP:
            return "Up-Right";
        case SDL_HAT_LEFTDOWN:
            return "Down-Left";
        case SDL_HAT_RIGHTDOWN:
            return "Down-Right";
        default:
            return "?";
    }
}

std::string DescribeBind(const ExtendedBind& bind) {
    switch (bind.bindType) {
        case SDL_CONTROLLER_BINDTYPE_BUTTON:
            return StringHelper::Sprintf("Button %d", bind.value.button);
        case SDL_CONTROLLER_BINDTYPE_AXIS:
            if (IsFullRangeAxis(bind)) {
                return StringHelper::Sprintf("Axis %d%s", bind.value.axis.axis,
                                             bind.value.axis.axisMin > bind.value.axis.axisMax ? " inv" : "");
            }
            return StringHelper::Sprintf("Axis %d %s", bind.value.axis.axis,
                                         bind.value.axis.axisMax < bind.value.axis.axisMin ? "-" : "+");
        case SDL_CONTROLLER_BINDTYPE_HAT:
            return StringHelper::Sprintf("Hat %d %s", bind.value.hat.hat, HatMaskName(bind.value.hat.hatMask));
        default:
            return "";
    }
}

// ---------------------------------------------------------------------------------------------------
// Mapping string generation / parsing
// ---------------------------------------------------------------------------------------------------

static std::string SanitizeName(const std::string& name) {
    std::string result = name;
    std::replace(result.begin(), result.end(), ',', ' ');
    return result;
}

std::string GenerateMappingString(SDL_Joystick* joystick, const ExtendedBind (&bindings)[BINDING_COUNT],
                                  const std::string& nameOverride) {
    if (joystick == nullptr) {
        return "";
    }

    SDL_JoystickGUID guid = SDL_JoystickGetGUID(joystick);
    uint16_t crc = 0;
#if SDL_VERSION_ATLEAST(2, 26, 0)
    SDL_GetJoystickGUIDInfo(guid, nullptr, nullptr, nullptr, &crc);
    if (crc != 0) {
        guid.data[2] = 0;
        guid.data[3] = 0;
    }
#endif

    char guidString[33] = "";
    SDL_JoystickGetGUIDString(guid, guidString, sizeof(guidString));

    const char* rawName = SDL_JoystickName(joystick);
    std::string name = SanitizeName(nameOverride);
    if (name.find_first_not_of(' ') == std::string::npos) {
        name = SanitizeName(rawName != nullptr ? rawName : guidString);
    }

    std::string mapping = StringHelper::Sprintf("%s,%s,platform:%s,", guidString, name.c_str(), SDL_GetPlatform());
    if (crc != 0) {
        mapping += StringHelper::Sprintf("crc:%.4x,", crc);
    }

    ExtendedBind work[BINDING_COUNT];
    for (int32_t i = 0; i < BINDING_COUNT; i++) {
        work[i] = bindings[i];
    }

    for (int32_t i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        if (work[i].bindType == SDL_CONTROLLER_BINDTYPE_NONE) {
            continue;
        }
        const char* name = SDL_GameControllerGetStringForButton((SDL_GameControllerButton)i);
        if (name == nullptr) {
            continue;
        }
        mapping += StringHelper::Sprintf("%s:%s,", name, EncodeBindElement(work[i]).c_str());
    }

    for (const auto& stick : kStickAxes) {
        const int32_t negative = SDL_CONTROLLER_BUTTON_MAX + stick.negative;
        const int32_t positive = SDL_CONTROLLER_BUTTON_MAX + stick.positive;
        const char* name = SDL_GameControllerGetStringForAxis(stick.axis);
        if (name == nullptr) {
            continue;
        }

        MergeAxisBindings(work, negative, positive);

        if (work[negative].bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
            const char* prefix = IsFullRangeAxis(work[negative]) ? "" : "-";
            mapping += StringHelper::Sprintf("%s%s:%s,", prefix, name, EncodeBindElement(work[negative]).c_str());
        }
        if (work[positive].bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
            mapping += StringHelper::Sprintf("+%s:%s,", name, EncodeBindElement(work[positive]).c_str());
        }
    }

    const std::pair<int32_t, SDL_GameControllerAxis> triggers[] = {
        { SDL_CONTROLLER_BUTTON_MAX + AXIS_TRIGGERLEFT, SDL_CONTROLLER_AXIS_TRIGGERLEFT },
        { SDL_CONTROLLER_BUTTON_MAX + AXIS_TRIGGERRIGHT, SDL_CONTROLLER_AXIS_TRIGGERRIGHT },
    };
    for (const auto& trigger : triggers) {
        if (work[trigger.first].bindType == SDL_CONTROLLER_BINDTYPE_NONE) {
            continue;
        }
        const char* name = SDL_GameControllerGetStringForAxis(trigger.second);
        if (name == nullptr) {
            continue;
        }
        mapping += StringHelper::Sprintf("%s:%s,", name, EncodeBindElement(work[trigger.first]).c_str());
    }

    return mapping;
}

static int32_t BindingIndexForKey(const std::string& key) {
    std::string name = key;
    char sign = 0;
    if (!name.empty() && (name.front() == '+' || name.front() == '-')) {
        sign = name.front();
        name.erase(0, 1);
    }

    const SDL_GameControllerAxis axis = SDL_GameControllerGetAxisFromString(name.c_str());
    if (axis != SDL_CONTROLLER_AXIS_INVALID) {
        switch (axis) {
            case SDL_CONTROLLER_AXIS_LEFTX:
                return SDL_CONTROLLER_BUTTON_MAX + (sign == '+' ? AXIS_LEFTX_POSITIVE : AXIS_LEFTX_NEGATIVE);
            case SDL_CONTROLLER_AXIS_LEFTY:
                return SDL_CONTROLLER_BUTTON_MAX + (sign == '+' ? AXIS_LEFTY_POSITIVE : AXIS_LEFTY_NEGATIVE);
            case SDL_CONTROLLER_AXIS_RIGHTX:
                return SDL_CONTROLLER_BUTTON_MAX + (sign == '+' ? AXIS_RIGHTX_POSITIVE : AXIS_RIGHTX_NEGATIVE);
            case SDL_CONTROLLER_AXIS_RIGHTY:
                return SDL_CONTROLLER_BUTTON_MAX + (sign == '+' ? AXIS_RIGHTY_POSITIVE : AXIS_RIGHTY_NEGATIVE);
            case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                return SDL_CONTROLLER_BUTTON_MAX + AXIS_TRIGGERLEFT;
            case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                return SDL_CONTROLLER_BUTTON_MAX + AXIS_TRIGGERRIGHT;
            default:
                return -1;
        }
    }

    if (sign != 0) {
        return -1;
    }
    const SDL_GameControllerButton button = SDL_GameControllerGetButtonFromString(name.c_str());
    return button != SDL_CONTROLLER_BUTTON_INVALID ? (int32_t)button : -1;
}

static bool DecodeBindElement(const std::string& value, ExtendedBind& out) {
    std::string element = value;
    bool inverted = false;
    if (!element.empty() && element.back() == '~') {
        inverted = true;
        element.pop_back();
    }

    char sign = 0;
    if (!element.empty() && (element.front() == '+' || element.front() == '-')) {
        sign = element.front();
        element.erase(0, 1);
    }
    if (element.size() < 2) {
        return false;
    }

    out = {};
    out.committed = true;

    switch (element.front()) {
        case 'b':
            out.bindType = SDL_CONTROLLER_BINDTYPE_BUTTON;
            out.value.button = std::atoi(element.c_str() + 1);
            return true;
        case 'a':
            out.bindType = SDL_CONTROLLER_BINDTYPE_AXIS;
            out.value.axis.axis = std::atoi(element.c_str() + 1);
            if (sign == '-') {
                out.value.axis.axisMin = 0;
                out.value.axis.axisMax = SDL_JOYSTICK_AXIS_MIN;
            } else if (sign == '+') {
                out.value.axis.axisMin = 0;
                out.value.axis.axisMax = SDL_JOYSTICK_AXIS_MAX;
            } else {
                out.value.axis.axisMin = inverted ? SDL_JOYSTICK_AXIS_MAX : SDL_JOYSTICK_AXIS_MIN;
                out.value.axis.axisMax = inverted ? SDL_JOYSTICK_AXIS_MIN : SDL_JOYSTICK_AXIS_MAX;
            }
            return true;
        case 'h': {
            int32_t hat = 0;
            int32_t mask = 0;
            if (std::sscanf(element.c_str() + 1, "%d.%d", &hat, &mask) != 2) {
                return false;
            }
            out.bindType = SDL_CONTROLLER_BINDTYPE_HAT;
            out.value.hat.hat = hat;
            out.value.hat.hatMask = mask;
            return true;
        }
        default:
            out = NoBind();
            return false;
    }
}

int32_t ParseMappingString(const std::string& mapping, ExtendedBind (&bindings)[BINDING_COUNT]) {
    ClearBindings(bindings);

    int32_t recovered = 0;
    size_t start = 0;
    while (start <= mapping.size()) {
        const size_t end = mapping.find(',', start);
        const std::string field = mapping.substr(start, end == std::string::npos ? std::string::npos : end - start);
        start = end == std::string::npos ? mapping.size() + 1 : end + 1;

        const size_t colon = field.find(':');
        if (colon == std::string::npos) {
            continue;
        }

        const int32_t index = BindingIndexForKey(field.substr(0, colon));
        if (index < 0) {
            continue;
        }
        if (DecodeBindElement(field.substr(colon + 1), bindings[index])) {
            recovered++;
        }
    }

    for (const auto& stick : kStickAxes) {
        const int32_t negative = SDL_CONTROLLER_BUTTON_MAX + stick.negative;
        const int32_t positive = SDL_CONTROLLER_BUTTON_MAX + stick.positive;
        if (!IsFullRangeAxis(bindings[negative]) || bindings[positive].bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
            continue;
        }

        const bool axisInverted = bindings[negative].value.axis.axisMin > bindings[negative].value.axis.axisMax;
        bindings[positive] = bindings[negative];
        bindings[negative].value.axis.axisMin = 0;
        bindings[negative].value.axis.axisMax = axisInverted ? SDL_JOYSTICK_AXIS_MAX : SDL_JOYSTICK_AXIS_MIN;
        bindings[positive].value.axis.axisMin = 0;
        bindings[positive].value.axis.axisMax = axisInverted ? SDL_JOYSTICK_AXIS_MIN : SDL_JOYSTICK_AXIS_MAX;
    }

    return recovered;
}

// ---------------------------------------------------------------------------------------------------
// MappingSession
// ---------------------------------------------------------------------------------------------------

MappingSession::MappingSession()
    : mBindings{}, mCurrentStep(0), mPendingAdvanceTime(0), mActive(false), mComplete(false) {
    ClearBindings(mBindings);
}

void MappingSession::Start(SDL_Joystick* joystick, const std::vector<int32_t>& order) {
    mOrder = order;
    mCurrentStep = 0;
    mComplete = mOrder.empty();
    mActive = !mOrder.empty();
    mPendingAdvanceTime = 0;

    const int32_t axisCount = joystick != nullptr ? SDL_JoystickNumAxes(joystick) : 0;
    mAxisState.assign(axisCount > 0 ? (size_t)axisCount : 0, AxisState{});
    for (int32_t i = 0; i < axisCount; i++) {
        Sint16 initialValue = 0;
        mAxisState[i].moving = SDL_JoystickGetAxisInitialState(joystick, i, &initialValue) == SDL_TRUE;
        mAxisState[i].lastValue = initialValue;
        mAxisState[i].startingValue = initialValue;
        mAxisState[i].farthestValue = initialValue;
    }

    if (mActive) {
        mBindings[mOrder[0]] = NoBind();
    }
}

void MappingSession::Stop() {
    mActive = false;
    mComplete = false;
    mOrder.clear();
    mAxisState.clear();
    mPendingAdvanceTime = 0;
}

bool MappingSession::IsActive() const {
    return mActive;
}

bool MappingSession::IsComplete() const {
    return mComplete;
}

int32_t MappingSession::CurrentStep() const {
    return mCurrentStep;
}

int32_t MappingSession::StepCount() const {
    return (int32_t)mOrder.size();
}

int32_t MappingSession::CurrentBinding() const {
    if (mCurrentStep < 0 || mCurrentStep >= (int32_t)mOrder.size()) {
        return -1;
    }
    return mOrder[mCurrentStep];
}

void MappingSession::GoToStep(int32_t step) {
    if (step < 0) {
        return;
    }
    if (step >= (int32_t)mOrder.size()) {
        mComplete = true;
        mActive = false;
        return;
    }

    mCurrentStep = step;

    mBindings[mOrder[mCurrentStep]] = NoBind();
    for (auto& state : mAxisState) {
        state.farthestValue = state.startingValue;
    }
    mPendingAdvanceTime = 0;
}

void MappingSession::Advance() {
    GoToStep(mCurrentStep + 1);
}

void MappingSession::GoBack() {
    GoToStep(mCurrentStep - 1);
}

const ExtendedBind& MappingSession::GetBinding(int32_t index) const {
    return mBindings[index];
}

void MappingSession::SetBindings(const ExtendedBind (&bindings)[BINDING_COUNT]) {
    for (int32_t i = 0; i < BINDING_COUNT; i++) {
        mBindings[i] = bindings[i];
    }
}

void MappingSession::CopyBindingsTo(ExtendedBind (&out)[BINDING_COUNT]) const {
    for (int32_t i = 0; i < BINDING_COUNT; i++) {
        out[i] = mBindings[i];
    }
}

bool MappingSession::BindingContainsBinding(const ExtendedBind& a, const ExtendedBind& b) const {
    if (a.bindType != b.bindType) {
        return false;
    }

    if (a.bindType == SDL_CONTROLLER_BINDTYPE_AXIS) {
        if (a.value.axis.axis != b.value.axis.axis || !a.committed) {
            return false;
        }
        const int32_t minA = std::min(a.value.axis.axisMin, a.value.axis.axisMax);
        const int32_t maxA = std::max(a.value.axis.axisMin, a.value.axis.axisMax);
        const int32_t minB = std::min(b.value.axis.axisMin, b.value.axis.axisMax);
        const int32_t maxB = std::max(b.value.axis.axisMin, b.value.axis.axisMax);
        return minA <= minB && maxA >= maxB;
    }

    return std::memcmp(&a, &b, sizeof(a)) == 0;
}

void MappingSession::ConfigureBinding(const ExtendedBind& binding) {
    const int32_t currentElement = CurrentBinding();
    if (currentElement < 0) {
        return;
    }

    for (int32_t i = 0; i < BINDING_COUNT; i++) {
        if (BindingContainsBinding(mBindings[i], binding)) {
            return;
        }
    }

    ExtendedBind& current = mBindings[currentElement];
    if (current.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
        const bool nativeDPad =
            currentElement == SDL_CONTROLLER_BUTTON_DPAD_UP || currentElement == SDL_CONTROLLER_BUTTON_DPAD_DOWN ||
            currentElement == SDL_CONTROLLER_BUTTON_DPAD_LEFT || currentElement == SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
        if (nativeDPad && current.bindType == SDL_CONTROLLER_BINDTYPE_HAT) {
            return;
        }

        const bool nativeAxis = currentElement >= SDL_CONTROLLER_BUTTON_MAX;
        const bool currentIsAxis = current.bindType == SDL_CONTROLLER_BINDTYPE_AXIS;
        if (nativeAxis == currentIsAxis &&
            (binding.bindType != SDL_CONTROLLER_BINDTYPE_AXIS || binding.value.axis.axis != current.value.axis.axis)) {
            return;
        }
    }

    current = binding;
    mPendingAdvanceTime = binding.committed ? SDL_GetTicks() : 0;
}

void MappingSession::ProcessButtonDown(int32_t button) {
    if (!mActive) {
        return;
    }
    ExtendedBind binding = {};
    binding.bindType = SDL_CONTROLLER_BINDTYPE_BUTTON;
    binding.value.button = button;
    binding.committed = true;
    ConfigureBinding(binding);
}

void MappingSession::ProcessHatMotion(int32_t hat, int32_t value) {
    if (!mActive || value == SDL_HAT_CENTERED) {
        return;
    }
    ExtendedBind binding = {};
    binding.bindType = SDL_CONTROLLER_BINDTYPE_HAT;
    binding.value.hat.hat = hat;
    binding.value.hat.hatMask = value;
    binding.committed = true;
    ConfigureBinding(binding);
}

static int32_t StandardizeAxisValue(int32_t value) {
    if (value > SDL_JOYSTICK_AXIS_MAX / 2) {
        return SDL_JOYSTICK_AXIS_MAX;
    }
    if (value < SDL_JOYSTICK_AXIS_MIN / 2) {
        return SDL_JOYSTICK_AXIS_MIN;
    }
    return 0;
}

void MappingSession::ProcessAxisMotion(int32_t axis, int32_t value, SDL_Joystick* joystick) {
    if (!mActive || axis < 0 || axis >= (int32_t)mAxisState.size()) {
        return;
    }

    constexpr int32_t MAX_ALLOWED_JITTER = SDL_JOYSTICK_AXIS_MAX / 80;
    AxisState& state = mAxisState[axis];

    if (!state.moving) {
        Sint16 initialValue = 0;
        state.moving = SDL_JoystickGetAxisInitialState(joystick, axis, &initialValue) == SDL_TRUE;
        state.lastValue = value;
        state.startingValue = initialValue;
        state.farthestValue = initialValue;
    } else if (std::abs(value - state.lastValue) <= MAX_ALLOWED_JITTER) {
        return;
    } else {
        state.lastValue = value;
    }

    const int32_t currentDistance = std::abs(value - state.startingValue);
    int32_t farthestDistance = std::abs(state.farthestValue - state.startingValue);
    if (currentDistance > farthestDistance) {
        state.farthestValue = value;
        farthestDistance = std::abs(state.farthestValue - state.startingValue);
    }

    if (farthestDistance < AXIS_COMMIT_DISTANCE) {
        return;
    }

    ExtendedBind binding = {};
    binding.bindType = SDL_CONTROLLER_BINDTYPE_AXIS;
    binding.value.axis.axis = axis;
    binding.value.axis.axisMin = StandardizeAxisValue(state.startingValue);
    binding.value.axis.axisMax = StandardizeAxisValue(state.farthestValue);
    binding.committed = currentDistance <= AXIS_RETURN_DISTANCE;
    ConfigureBinding(binding);
}

void MappingSession::CheckPendingAdvance() {
    if (!mActive || mPendingAdvanceTime == 0) {
        return;
    }
    if (SDL_GetTicks() - mPendingAdvanceTime >= PENDING_ADVANCE_DELAY_MS) {
        Advance();
    }
}

// ---------------------------------------------------------------------------------------------------
// usergamepadmappings.txt
// ---------------------------------------------------------------------------------------------------

std::string GetUserMappingsPath() {
    return Ship::Context::GetPathRelativeToAppDirectory(kUserMappingsFileName);
}

static std::string StripCrcFromGuid(const std::string& guid) {
    if (guid.length() < 32) {
        return guid;
    }
    return guid.substr(0, 4) + "0000" + guid.substr(8);
}

static std::string MappingKey(const std::string& mapping) {
    const size_t comma = mapping.find(',');
    if (comma == std::string::npos || comma == 0) {
        return "";
    }
    std::string key = StripCrcFromGuid(mapping.substr(0, comma));

    const size_t crcPos = mapping.find(",crc:");
    if (crcPos != std::string::npos) {
        const size_t valueStart = crcPos + 1;
        size_t valueEnd = mapping.find(',', valueStart);
        if (valueEnd == std::string::npos) {
            valueEnd = mapping.length();
        }
        key += "," + mapping.substr(valueStart, valueEnd - valueStart);
    }
    return key;
}

static bool IsMappingLine(const std::string& line) {
    return !line.empty() && line.front() != '#';
}

static std::vector<std::string> ReadUserMappingLines() {
    std::vector<std::string> lines;
    std::ifstream file(GetUserMappingsPath());
    if (!file.is_open()) {
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }
    return lines;
}

static bool WriteUserMappingLines(const std::vector<std::string>& lines) {
    const std::string path = GetUserMappingsPath();
    const std::string tempPath = path + ".tmp";

    {
        std::ofstream file(tempPath, std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            SPDLOG_ERROR("Failed to open \"{}\" for writing", tempPath);
            return false;
        }
        file << "# SoH user gamepad mappings, written by the built-in gamepad mapper.\n";
        file << "# Loaded on top of gamecontrollerdb.txt, so entries here win.\n";
        for (const auto& line : lines) {
            file << line << "\n";
        }
        if (!file.good()) {
            SPDLOG_ERROR("Failed while writing \"{}\"", tempPath);
            return false;
        }
    }

    std::error_code error;
    std::filesystem::rename(tempPath, path, error);
    if (error) {
        SPDLOG_ERROR("Failed to move \"{}\" into place: {}", tempPath, error.message());
        std::filesystem::remove(tempPath, error);
        return false;
    }
    return true;
}

std::string FindUserMapping(const std::string& guid) {
    const std::string base = StripCrcFromGuid(guid);
    for (const auto& line : ReadUserMappingLines()) {
        if (!IsMappingLine(line)) {
            continue;
        }
        const size_t comma = line.find(',');
        if (comma != std::string::npos && StripCrcFromGuid(line.substr(0, comma)) == base) {
            return line;
        }
    }
    return "";
}

std::string MappingName(const std::string& mapping) {
    const size_t first = mapping.find(',');
    if (first == std::string::npos) {
        return "";
    }
    const size_t second = mapping.find(',', first + 1);
    return mapping.substr(first + 1, second == std::string::npos ? std::string::npos : second - first - 1);
}

bool LoadUserMappings() {
    const std::string path = GetUserMappingsPath();

    std::error_code error;
    if (!std::filesystem::exists(path, error)) {
        return false;
    }

    const int32_t added = SDL_GameControllerAddMappingsFromFile(path.c_str());
    if (added < 0) {
        SPDLOG_ERROR("Failed to add user gamepad mappings from \"{}\": {}", path, SDL_GetError());
        return false;
    }

    SPDLOG_INFO("Added {} user gamepad mapping(s) from \"{}\"", added, path);
    Ship::Context::GetRawInstance()
        ->GetControlDeck()
        ->GetConnectedPhysicalDeviceManager()
        ->RefreshConnectedSDLGamepads();
    return true;
}

bool SaveUserMapping(const std::string& mapping) {
    const std::string key = MappingKey(mapping);
    if (key.empty()) {
        SPDLOG_ERROR("Refusing to save a gamepad mapping with no GUID");
        return false;
    }

    std::vector<std::string> lines;
    for (const auto& line : ReadUserMappingLines()) {
        if (!IsMappingLine(line)) {
            continue;
        }
        if (MappingKey(line) == key) {
            continue;
        }
        lines.push_back(line);
    }
    lines.push_back(mapping);

    if (!WriteUserMappingLines(lines)) {
        return false;
    }

    if (SDL_GameControllerAddMapping(mapping.c_str()) < 0) {
        SPDLOG_ERROR("SDL rejected the generated mapping \"{}\": {}", mapping, SDL_GetError());
        return false;
    }

    Ship::Context::GetRawInstance()
        ->GetControlDeck()
        ->GetConnectedPhysicalDeviceManager()
        ->RefreshConnectedSDLGamepads();
    return true;
}

bool DeleteUserMapping(const std::string& guid) {
    const std::string base = StripCrcFromGuid(guid);

    std::vector<std::string> lines;
    bool found = false;
    for (const auto& line : ReadUserMappingLines()) {
        if (!IsMappingLine(line)) {
            continue;
        }
        const size_t comma = line.find(',');
        if (comma != std::string::npos && StripCrcFromGuid(line.substr(0, comma)) == base) {
            found = true;
            continue;
        }
        lines.push_back(line);
    }

    if (!found) {
        return false;
    }
    if (!WriteUserMappingLines(lines)) {
        return false;
    }

    Ship::Context::GetRawInstance()
        ->GetControlDeck()
        ->GetConnectedPhysicalDeviceManager()
        ->RefreshConnectedSDLGamepads();
    return true;
}

// ---------------------------------------------------------------------------------------------------
// Diagram layout
// ---------------------------------------------------------------------------------------------------

struct DiagramEntry {
    int32_t binding;
    ImVec2 hotspot;
    bool labelOnRight;
};

static constexpr float MAX_IMAGE_SPAN = 0.52f;
static constexpr float MIN_IMAGE_WIDTH = 360.0f;
static constexpr float FALLBACK_ART_ASPECT = 1850.0f / 1302.0f;
static constexpr ImU32 NAME_COLOR = IM_COL32(138, 144, 156, 255);

static const DiagramEntry kDiagram[] = {
    { SDL_CONTROLLER_BUTTON_MAX + AXIS_TRIGGERLEFT, ImVec2(0.081f, 0.110f), false },
    { SDL_CONTROLLER_BUTTON_LEFTSHOULDER, ImVec2(0.234f, 0.092f), false },
    { SDL_CONTROLLER_BUTTON_BACK, ImVec2(0.426f, 0.408f), false },
    { SDL_CONTROLLER_BUTTON_MAX + AXIS_LEFTY_NEGATIVE, ImVec2(0.241f, 0.315f), false },
    { SDL_CONTROLLER_BUTTON_MAX + AXIS_LEFTX_NEGATIVE, ImVec2(0.185f, 0.401f), false },
    { SDL_CONTROLLER_BUTTON_MAX + AXIS_LEFTX_POSITIVE, ImVec2(0.297f, 0.401f), false },
    { SDL_CONTROLLER_BUTTON_MAX + AXIS_LEFTY_POSITIVE, ImVec2(0.241f, 0.487f), false },
    { SDL_CONTROLLER_BUTTON_LEFTSTICK, ImVec2(0.241f, 0.401f), false },
    { SDL_CONTROLLER_BUTTON_DPAD_UP, ImVec2(0.368f, 0.540f), false },
    { SDL_CONTROLLER_BUTTON_DPAD_LEFT, ImVec2(0.310f, 0.614f), false },
    { SDL_CONTROLLER_BUTTON_DPAD_RIGHT, ImVec2(0.416f, 0.614f), false },
    { SDL_CONTROLLER_BUTTON_DPAD_DOWN, ImVec2(0.368f, 0.690f), false },

    { SDL_CONTROLLER_BUTTON_MAX + AXIS_TRIGGERRIGHT, ImVec2(0.919f, 0.110f), true },
    { SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, ImVec2(0.766f, 0.092f), true },
    { SDL_CONTROLLER_BUTTON_START, ImVec2(0.570f, 0.408f), true },
    { SDL_CONTROLLER_BUTTON_Y, ImVec2(0.752f, 0.328f), true },
    { SDL_CONTROLLER_BUTTON_X, ImVec2(0.689f, 0.412f), true },
    { SDL_CONTROLLER_BUTTON_B, ImVec2(0.816f, 0.412f), true },
    { SDL_CONTROLLER_BUTTON_A, ImVec2(0.752f, 0.497f), true },
    { SDL_CONTROLLER_BUTTON_MAX + AXIS_RIGHTY_NEGATIVE, ImVec2(0.628f, 0.518f), true },
    { SDL_CONTROLLER_BUTTON_MAX + AXIS_RIGHTX_NEGATIVE, ImVec2(0.570f, 0.595f), true },
    { SDL_CONTROLLER_BUTTON_MAX + AXIS_RIGHTX_POSITIVE, ImVec2(0.686f, 0.595f), true },
    { SDL_CONTROLLER_BUTTON_MAX + AXIS_RIGHTY_POSITIVE, ImVec2(0.628f, 0.682f), true },
    { SDL_CONTROLLER_BUTTON_RIGHTSTICK, ImVec2(0.628f, 0.595f), true },
};

// ---------------------------------------------------------------------------------------------------
// MapperWindow
// ---------------------------------------------------------------------------------------------------

#define MAPPER_WINDOW_GAME_INPUT_BLOCK_ID 95237930

static constexpr uint32_t DEVICE_SCAN_INTERVAL_MS = 500;
static constexpr uint32_t FILE_WATCH_INTERVAL_MS = 1000;

MapperWindow::~MapperWindow() {
    CloseJoystick();
}

void MapperWindow::InitElement() {
    ClearBindings(mBindings);
}

// Records the joystick's current digital state as the baseline for edge detection, so a button already
// held when a prompt appears has to be released and pressed again to count.
void MapperWindow::SnapshotDeviceState() {
    mPreviousButtons.clear();
    mPreviousHats.clear();
    if (mJoystick == nullptr) {
        return;
    }

    SDL_JoystickUpdate();
    for (int32_t i = 0; i < SDL_JoystickNumButtons(mJoystick); i++) {
        mPreviousButtons.push_back(SDL_JoystickGetButton(mJoystick, i));
    }
    for (int32_t i = 0; i < SDL_JoystickNumHats(mJoystick); i++) {
        mPreviousHats.push_back(SDL_JoystickGetHat(mJoystick, i));
    }
}

void MapperWindow::PollDeviceForSession() {
    if (mJoystick == nullptr) {
        return;
    }

    if (SDL_JoystickGetAttached(mJoystick) == SDL_FALSE) {
        EndSession(false);
        mStatusMessage = "Device disconnected during mapping.";
        return;
    }

    SDL_JoystickUpdate();

    const int32_t buttonCount = SDL_JoystickNumButtons(mJoystick);
    mPreviousButtons.resize((size_t)std::max(buttonCount, 0), 0);
    for (int32_t i = 0; i < buttonCount; i++) {
        const uint8_t state = SDL_JoystickGetButton(mJoystick, i);
        if (state != 0 && mPreviousButtons[i] == 0) {
            mSession.ProcessButtonDown(i);
        }
        mPreviousButtons[i] = state;
    }

    const int32_t hatCount = SDL_JoystickNumHats(mJoystick);
    mPreviousHats.resize((size_t)std::max(hatCount, 0), SDL_HAT_CENTERED);
    for (int32_t i = 0; i < hatCount; i++) {
        const uint8_t state = SDL_JoystickGetHat(mJoystick, i);
        if (state != mPreviousHats[i] && state != SDL_HAT_CENTERED) {
            mSession.ProcessHatMotion(i, state);
        }
        mPreviousHats[i] = state;
    }

    for (int32_t i = 0; i < SDL_JoystickNumAxes(mJoystick); i++) {
        mSession.ProcessAxisMotion(i, SDL_JoystickGetAxis(mJoystick, i), mJoystick);
    }
}

void MapperWindow::CloseJoystick() {
    if (mJoystick != nullptr) {
        SDL_JoystickClose(mJoystick);
        mJoystick = nullptr;
    }
    mPreviousButtons.clear();
    mPreviousHats.clear();
}

void MapperWindow::RefreshDeviceList() {
    const SDL_JoystickID previousInstanceId =
        mSelectedDevice >= 0 && mSelectedDevice < (int32_t)mDevices.size() ? mDevices[mSelectedDevice].instanceId : -1;

    mDevices.clear();

    const std::vector<std::string> userLines = ReadUserMappingLines();

    for (int32_t i = 0; i < SDL_NumJoysticks(); i++) {
        char guidString[33] = "";
        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(i), guidString, sizeof(guidString));

        DeviceInfo info = {};
        info.deviceIndex = i;
        info.instanceId = SDL_JoystickGetDeviceInstanceID(i);
        const char* name = SDL_JoystickNameForIndex(i);
        info.name = name != nullptr ? name : guidString;
        info.guid = guidString;
        info.hasMapping = SDL_IsGameController(i) == SDL_TRUE;
        info.hasUserMapping = false;

        const std::string base = StripCrcFromGuid(info.guid);
        for (const auto& line : userLines) {
            if (!IsMappingLine(line)) {
                continue;
            }
            const size_t comma = line.find(',');
            if (comma != std::string::npos && StripCrcFromGuid(line.substr(0, comma)) == base) {
                info.hasUserMapping = true;
                break;
            }
        }

        mDevices.push_back(info);
    }

    for (int32_t i = 0; i < (int32_t)mDevices.size(); i++) {
        if (mDevices[i].instanceId == previousInstanceId) {
            mSelectedDevice = i;
            return;
        }
    }

    SelectDevice(mDevices.empty() ? -1 : 0);
}

void MapperWindow::SelectDevice(int32_t listIndex) {
    if (mSession.IsActive()) {
        EndSession(false);
    }

    CloseJoystick();
    ClearBindings(mBindings);
    mDirty = false;
    mStatusMessage.clear();
    mSelectedDevice = listIndex;

    if (listIndex < 0 || listIndex >= (int32_t)mDevices.size()) {
        return;
    }

    mJoystick = SDL_JoystickOpen(mDevices[listIndex].deviceIndex);
    if (mJoystick == nullptr) {
        mStatusMessage = StringHelper::Sprintf("Could not open device: %s", SDL_GetError());
        return;
    }

    LoadBindingsForDevice();
}

void MapperWindow::LoadBindingsForDevice() {
    ClearBindings(mBindings);
    mCustomName.clear();
    if (mJoystick == nullptr) {
        return;
    }

    char guidString[33] = "";
    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(mJoystick), guidString, sizeof(guidString));

    const std::string userMapping = FindUserMapping(guidString);
    if (!userMapping.empty()) {
        ParseMappingString(userMapping, mBindings);
        mCustomName = MappingName(userMapping);
        return;
    }

    const char* joystickName = SDL_JoystickName(mJoystick);
    mCustomName = joystickName != nullptr ? joystickName : "";

    char* existing = SDL_GameControllerMappingForGUID(SDL_JoystickGetGUID(mJoystick));
    if (existing == nullptr) {
        return;
    }
    ParseMappingString(existing, mBindings);
    SDL_free(existing);
}

void MapperWindow::StartSession(const std::vector<int32_t>& order) {
    if (mJoystick == nullptr || order.empty()) {
        return;
    }

    mStatusMessage.clear();
    mSession.SetBindings(mBindings);
    mSession.Start(mJoystick, order);
    SnapshotDeviceState();

    Ship::Context::GetRawInstance()->GetControlDeck()->BlockGameInput(MAPPER_WINDOW_GAME_INPUT_BLOCK_ID);
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->BlockGamepadNavigation();
}

void MapperWindow::EndSession(bool applyResult) {
    if (applyResult) {
        mSession.CopyBindingsTo(mBindings);
        mDirty = true;
    }
    mSession.Stop();
    mPreviousButtons.clear();
    mPreviousHats.clear();

    mGameInputBlockTimer = 20;
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->UnblockGamepadNavigation();
}

void MapperWindow::UpdateElement() {
    const uint32_t now = SDL_GetTicks();

    if (now - mLastUserFileCheckTicks >= FILE_WATCH_INTERVAL_MS || mLastUserFileCheckTicks == 0) {
        mLastUserFileCheckTicks = now;

        std::error_code error;
        const std::filesystem::path path = GetUserMappingsPath();
        if (std::filesystem::exists(path, error)) {
            const auto writeTime = std::filesystem::last_write_time(path, error);
            const uint64_t stamp = (uint64_t)writeTime.time_since_epoch().count();
            if (!error && stamp != mLastUserFileWriteTime) {
                const bool firstLoad = mLastUserFileWriteTime == 0;
                mLastUserFileWriteTime = stamp;
                LoadUserMappings();
                if (!firstLoad && !mSession.IsActive() && !mDirty) {
                    LoadBindingsForDevice();
                }
            }
        } else {
            mLastUserFileWriteTime = 0;
        }
    }

    if (mGameInputBlockTimer != INT32_MAX && !mSession.IsActive()) {
        mGameInputBlockTimer--;
        if (mGameInputBlockTimer <= 0) {
            Ship::Context::GetRawInstance()->GetControlDeck()->UnblockGameInput(MAPPER_WINDOW_GAME_INPUT_BLOCK_ID);
            mGameInputBlockTimer = INT32_MAX;
        }
    }

    if (!IsVisible()) {
        if (mSession.IsActive()) {
            EndSession(false);
        }
        if (mJoystick != nullptr) {
            CloseJoystick();
            mSelectedDevice = -1;
            mDevices.clear();
        }
        return;
    }

    if (!mSession.IsActive() && (mDevices.empty() || now - mLastDeviceScanTicks >= DEVICE_SCAN_INTERVAL_MS)) {
        mLastDeviceScanTicks = now;
        RefreshDeviceList();
    }

    if (mSession.IsActive()) {
        Ship::Context::GetRawInstance()->GetControlDeck()->BlockGameInput(MAPPER_WINDOW_GAME_INPUT_BLOCK_ID);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->BlockGamepadNavigation();

        PollDeviceForSession();
        mSession.CheckPendingAdvance();

        if (mSession.IsComplete()) {
            EndSession(true);
            mStatusMessage = "Mapping captured. Check the diagram, then Save Mapping.";
        }
    }
}

bool MapperWindow::IsBindingLive(int32_t binding) const {
    if (mJoystick == nullptr || binding < 0 || binding >= BINDING_COUNT) {
        return false;
    }

    const ExtendedBind& bind = mSession.IsActive() ? mSession.GetBinding(binding) : mBindings[binding];

    switch (bind.bindType) {
        case SDL_CONTROLLER_BINDTYPE_BUTTON:
            return SDL_JoystickGetButton(mJoystick, bind.value.button) != 0;
        case SDL_CONTROLLER_BINDTYPE_AXIS: {
            const int32_t value = SDL_JoystickGetAxis(mJoystick, bind.value.axis.axis);
            if (IsFullRangeAxis(bind)) {
                Sint16 rest = 0;
                SDL_JoystickGetAxisInitialState(mJoystick, bind.value.axis.axis, &rest);
                return std::abs(value - rest) > SDL_JOYSTICK_AXIS_MAX / 2;
            }
            // A half-axis counts as active once the reading is most of the way toward axisMax.
            const int32_t threshold = bind.value.axis.axisMax / 2;
            return bind.value.axis.axisMax < 0 ? value <= threshold : value >= threshold;
        }
        case SDL_CONTROLLER_BINDTYPE_HAT:
            return (SDL_JoystickGetHat(mJoystick, bind.value.hat.hat) & bind.value.hat.hatMask) ==
                   bind.value.hat.hatMask;
        default:
            return false;
    }
}

// ---------------------------------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------------------------------

void MapperWindow::DrawDeviceSelector() {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Device");
    ImGui::SameLine();

    std::string preview = "No joysticks detected";
    if (mSelectedDevice >= 0 && mSelectedDevice < (int32_t)mDevices.size()) {
        preview = mDevices[mSelectedDevice].name;
    }

    ImGui::BeginDisabled(mSession.IsActive() || mDevices.empty());
    PushStyleCombobox(THEME_COLOR);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
    if (ImGui::BeginCombo("##mapperDevice", preview.c_str())) {
        for (int32_t i = 0; i < (int32_t)mDevices.size(); i++) {
            const DeviceInfo& device = mDevices[i];
            const char* status = device.hasUserMapping ? " [user mapping]" : (device.hasMapping ? "" : " [unmapped]");
            if (ImGui::Selectable(StringHelper::Sprintf("%s%s##dev%d", device.name.c_str(), status, i).c_str(),
                                  i == mSelectedDevice)) {
                SelectDevice(i);
            }
        }
        ImGui::EndCombo();
    }
    PopStyleCombobox();
    ImGui::EndDisabled();

    if (mSelectedDevice < 0 || mSelectedDevice >= (int32_t)mDevices.size()) {
        ImGui::TextDisabled("Connect a gamepad to get started.");
        return;
    }

    const DeviceInfo& device = mDevices[mSelectedDevice];
    ImGui::TextDisabled("GUID %s", device.guid.c_str());
    if (mJoystick != nullptr) {
        ImGui::SameLine();
        ImGui::TextDisabled("| %d buttons, %d axes, %d hats", SDL_JoystickNumButtons(mJoystick),
                            SDL_JoystickNumAxes(mJoystick), SDL_JoystickNumHats(mJoystick));
    }

    if (!device.hasMapping) {
        ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.2f, 1.0f),
                           "This device has no SDL mapping, so the game cannot see it yet. Map it below.");
    } else if (device.hasUserMapping) {
        ImGui::TextColored(ImVec4(0.4f, 0.85f, 0.4f, 1.0f), "Using a mapping from %s.", kUserMappingsFileName);
    }
}

void MapperWindow::DrawDiagram() {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    const bool haveArt = gui != nullptr && gui->HasTextureByName(kDiagramTextureName);

    float artAspect = FALLBACK_ART_ASPECT;
    if (haveArt) {
        const ImVec2 artSize = gui->GetTextureSize(kDiagramTextureName);
        if (artSize.x > 0.0f && artSize.y > 0.0f) {
            artAspect = artSize.x / artSize.y;
        }
    }

    std::string values[IM_ARRAYSIZE(kDiagram)];
    int32_t rowsPerGutter[2] = { 0, 0 };
    float widestName = 0.0f;
    float widestValue = 0.0f;
    for (int32_t i = 0; i < (int32_t)IM_ARRAYSIZE(kDiagram); i++) {
        const DiagramEntry& entry = kDiagram[i];
        const ExtendedBind& bind = mSession.IsActive() ? mSession.GetBinding(entry.binding) : mBindings[entry.binding];
        values[i] = bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE ? DescribeBind(bind) : "-";
        widestValue = std::max(widestValue, ImGui::CalcTextSize(values[i].c_str()).x);
        widestName = std::max(widestName, ImGui::CalcTextSize(BindingName(entry.binding)).x);
        rowsPerGutter[entry.labelOnRight ? 1 : 0]++;
    }
    const int32_t maxRows = std::max(rowsPerGutter[0], rowsPerGutter[1]);

    const float valueColumnWidth = std::max(widestValue, ImGui::GetFontSize() * 6.0f);

    const float valueGap = ImGui::CalcTextSize("  ").x;

    const float gutterWidth = widestName + valueColumnWidth + valueGap + ImGui::GetFontSize() * 3.0f;
    const float width = std::max(ImGui::GetContentRegionAvail().x, gutterWidth * 2.0f + MIN_IMAGE_WIDTH);
    const float imageWidth = std::min(width - gutterWidth * 2.0f, width * MAX_IMAGE_SPAN);
    const float imageHeight = imageWidth / artAspect;
    const float rowHeight = ImGui::GetTextLineHeightWithSpacing() * 1.35f;
    const float height = std::max(imageHeight, rowHeight * maxRows);

    const ImVec2 origin = ImGui::GetCursorScreenPos();
    const ImVec2 canvas(width, height);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    const ImVec2 imageMin(origin.x + (width - imageWidth) * 0.5f, origin.y + (height - imageHeight) * 0.5f);
    const ImVec2 imageMax(imageMin.x + imageWidth, imageMin.y + imageHeight);

    const auto toArt = [&](const ImVec2& uv) {
        return ImVec2(imageMin.x + uv.x * imageWidth, imageMin.y + uv.y * imageHeight);
    };

    if (haveArt) {
        drawList->AddImage(gui->GetTextureByName(kDiagramTextureName), imageMin, imageMax);
    } else {
        drawList->AddRectFilled(imageMin, imageMax, IM_COL32(40, 44, 52, 255), 12.0f);
        drawList->AddRect(imageMin, imageMax, IM_COL32(90, 96, 110, 255), 12.0f, 0, 2.0f);
        const char* placeholder = "controller image";
        const ImVec2 textSize = ImGui::CalcTextSize(placeholder);
        drawList->AddText(
            ImVec2((imageMin.x + imageMax.x - textSize.x) * 0.5f, (imageMin.y + imageMax.y - textSize.y) * 0.5f),
            IM_COL32(120, 126, 140, 255), placeholder);
    }

    const int32_t currentBinding = mSession.IsActive() ? mSession.CurrentBinding() : -1;
    const float pulse = 0.55f + 0.45f * std::sin((float)ImGui::GetTime() * 6.0f);

    int32_t rowUsed[2] = { 0, 0 };
    for (int32_t i = 0; i < (int32_t)IM_ARRAYSIZE(kDiagram); i++) {
        const DiagramEntry& entry = kDiagram[i];
        const ExtendedBind& bind = mSession.IsActive() ? mSession.GetBinding(entry.binding) : mBindings[entry.binding];
        const bool bound = bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE;
        const bool active = entry.binding == currentBinding;
        const bool live = IsBindingLive(entry.binding);

        const bool onRight = entry.labelOnRight;
        const float lineHeight = ImGui::GetTextLineHeight();
        const ImVec2 valueSize = ImGui::CalcTextSize(values[i].c_str());

        const int32_t gutter = onRight ? 1 : 0;
        const int32_t row = rowUsed[gutter]++;
        const float rowCenterY = origin.y + (row + 0.5f) * (height / rowsPerGutter[gutter]);

        const float run = ImGui::GetFontSize();
        const float elbowX =
            onRight ? imageMax.x + ImGui::GetFontSize() * 0.6f : imageMin.x - ImGui::GetFontSize() * 0.6f;
        const float valueEdgeX = onRight ? elbowX + run + valueGap : elbowX - run - valueGap;

        const ImVec2 cellMin(onRight ? valueEdgeX : valueEdgeX - valueColumnWidth,
                             rowCenterY - lineHeight * 0.5f - 2.0f);
        const ImVec2 cellMax(cellMin.x + valueColumnWidth, cellMin.y + lineHeight + 4.0f);

        ImGui::SetCursorScreenPos(cellMin);
        ImGui::InvisibleButton(StringHelper::Sprintf("##mapperRow%d", i).c_str(),
                               ImVec2(cellMax.x - cellMin.x, cellMax.y - cellMin.y),
                               ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        const bool hovered = ImGui::IsItemHovered();
        if (hovered && !mSession.IsActive() && mJoystick != nullptr) {
            Tooltip("Click to map this control. Right-click to clear it.");
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                StartSession({ entry.binding });
            } else if (ImGui::IsItemClicked(ImGuiMouseButton_Right) && bound) {
                mBindings[entry.binding] = NoBind();
                mDirty = true;
            }
        }

        ImU32 color;
        if (active) {
            color = IM_COL32(255, (int32_t)(200.0f * pulse), 60, 255);
        } else if (live) {
            color = IM_COL32(110, 220, 110, 255);
        } else if (bound) {
            color = IM_COL32(215, 220, 230, 255);
        } else {
            color = IM_COL32(130, 135, 145, 255);
        }

        if (hovered) {
            drawList->AddRectFilled(cellMin, cellMax, IM_COL32(255, 255, 255, 24), 3.0f);
        }

        const ImVec2 hotspot = toArt(entry.hotspot);
        const float lineStartX = onRight ? cellMin.x - valueGap : cellMax.x + valueGap;
        const ImU32 lineColor = active ? color : (bound ? IM_COL32(150, 156, 168, 255) : IM_COL32(90, 94, 104, 255));
        drawList->AddLine(ImVec2(lineStartX, rowCenterY), ImVec2(elbowX, rowCenterY), lineColor, 1.5f);
        drawList->AddLine(ImVec2(elbowX, rowCenterY), hotspot, lineColor, 1.5f);
        drawList->AddCircleFilled(hotspot, active || live ? 5.0f : 3.5f, color);

        drawList->AddText(ImVec2(onRight ? cellMin.x : cellMax.x - valueSize.x, rowCenterY - valueSize.y * 0.5f), color,
                          values[i].c_str());

        const char* name = BindingName(entry.binding);
        const ImVec2 nameSize = ImGui::CalcTextSize(name);
        drawList->AddText(
            ImVec2(onRight ? origin.x + width - 4.0f - nameSize.x : origin.x + 4.0f, rowCenterY - nameSize.y * 0.5f),
            NAME_COLOR, name);
    }

    ImGui::SetCursorScreenPos(ImVec2(origin.x, origin.y + canvas.y));
    ImGui::Dummy(ImVec2(1.0f, 0.0f));

    if (!mIncludeExtraButtons) {
        return;
    }

    ImGui::SeparatorText("Extra buttons");
    for (int32_t i = 0; i < (int32_t)IM_ARRAYSIZE(kExtraBindings); i++) {
        const int32_t binding = kExtraBindings[i];
        const ExtendedBind& bind = mSession.IsActive() ? mSession.GetBinding(binding) : mBindings[binding];
        const bool bound = bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE;
        const bool active = mSession.IsActive() && mSession.CurrentBinding() == binding;

        ImGui::BeginDisabled(mSession.IsActive() || mJoystick == nullptr);
        PushStyleButton(active ? Colors::Orange : THEME_COLOR);
        if (ImGui::Button(StringHelper::Sprintf("%s: %s##extra%d", BindingName(binding),
                                                bound ? DescribeBind(bind).c_str() : "-", i)
                              .c_str())) {
            StartSession({ binding });
        }
        PopStyleButton();
        ImGui::EndDisabled();
        if (i % 3 != 2 && i != (int32_t)IM_ARRAYSIZE(kExtraBindings) - 1) {
            ImGui::SameLine();
        }
    }
}

void MapperWindow::DrawSessionPrompt() {
    if (!mSession.IsActive()) {
        return;
    }

    const int32_t binding = mSession.CurrentBinding();
    const ImGuiStyle& style = ImGui::GetStyle();

    PushStyleButton(THEME_COLOR);
    const float buttonHeight = ImGui::GetFrameHeight();
    PopStyleButton();

    const float promptHeight =
        style.WindowPadding.y * 2.0f + ImGui::GetTextLineHeightWithSpacing() * 2.0f + buttonHeight;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16f, 0.14f, 0.06f, 1.0f));
    ImGui::BeginChild("##mapperPrompt", ImVec2(0.0f, promptHeight), true, ImGuiWindowFlags_NoScrollbar);

    ImGui::Text("Press or move: %s", binding >= 0 ? BindingName(binding) : "?");
    if (mSession.StepCount() > 1) {
        ImGui::SameLine();
        ImGui::TextDisabled("(%d / %d)", mSession.CurrentStep() + 1, mSession.StepCount());
    }
    ImGui::TextDisabled("Sticks and triggers: push all the way, then let go. Space skips, Escape cancels.");

    PushStyleButton(THEME_COLOR);
    const bool skip = ImGui::Button("Skip") || ImGui::IsKeyPressed(ImGuiKey_Space, false);
    ImGui::SameLine();
    ImGui::BeginDisabled(mSession.CurrentStep() == 0);
    const bool back = ImGui::Button("Back");
    ImGui::EndDisabled();
    ImGui::SameLine();
    const bool cancel = ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape, false);
    PopStyleButton();

    ImGui::EndChild();
    ImGui::PopStyleColor();

    if (cancel) {
        EndSession(false);
        mStatusMessage = "Mapping cancelled; nothing was changed.";
    } else if (back) {
        mSession.GoBack();
    } else if (skip) {
        mSession.Advance();
        if (mSession.IsComplete()) {
            EndSession(true);
            mStatusMessage = "Mapping captured. Check the diagram, then Save Mapping.";
        }
    }
}

void MapperWindow::DrawActionButtons() {
    const bool haveDevice = mJoystick != nullptr;
    const bool busy = mSession.IsActive();

    char nameBuffer[128] = "";
    SDL_strlcpy(nameBuffer, mCustomName.c_str(), sizeof(nameBuffer));
    ImGui::BeginDisabled(busy || !haveDevice);
    PushStyleInput(THEME_COLOR);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.45f);
    if (ImGui::InputText("Controller Name", nameBuffer, sizeof(nameBuffer))) {
        mCustomName = nameBuffer;
        mDirty = true;
    }
    PopStyleInput();
    ImGui::EndDisabled();
    Tooltip("Name saved into usergamepadmappings.txt for this device. Commas are replaced with spaces.");

    ImGui::BeginDisabled(busy || !haveDevice);
    PushStyleButton(THEME_COLOR);
    if (ImGui::Button("Map All")) {
        std::vector<int32_t> order(std::begin(kCoreBindingOrder), std::end(kCoreBindingOrder));
        if (mIncludeExtraButtons) {
            order.insert(order.end(), std::begin(kExtraBindings), std::end(kExtraBindings));
        }
        StartSession(order);
    }
    PopStyleButton();
    ImGui::EndDisabled();
    Tooltip(haveDevice ? "Walks every control on the pad, asking for one input at a time." : "Select a device first.");

    ImGui::SameLine();
    ImGui::BeginDisabled(busy || !haveDevice || !mDirty);
    PushStyleButton(THEME_COLOR);
    if (ImGui::Button("Save Mapping")) {
        char guidString[33] = "";
        SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(mJoystick), guidString, sizeof(guidString));

        bool ok;
        if (!HasAnyBinding(mBindings)) {
            ok = DeleteUserMapping(guidString);
            mStatusMessage =
                ok ? StringHelper::Sprintf("Removed from %s. SDL keeps the device registered until you restart.",
                                           kUserMappingsFileName)
                   : "Nothing to remove; this device has no saved mapping.";
            ok = true;
        } else {
            const std::string mapping = GenerateMappingString(mJoystick, mBindings, mCustomName);
            ok = !mapping.empty() && SaveUserMapping(mapping);
            mStatusMessage = ok ? StringHelper::Sprintf("Saved to %s.", kUserMappingsFileName)
                                : "Failed to save the mapping; see the log for details.";
        }

        if (ok) {
            mDirty = false;
            std::error_code error;
            const auto writeTime = std::filesystem::last_write_time(GetUserMappingsPath(), error);
            if (!error) {
                mLastUserFileWriteTime = (uint64_t)writeTime.time_since_epoch().count();
            }
            RefreshDeviceList();
        }
    }
    PopStyleButton();
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::BeginDisabled(busy || !haveDevice);
    PushStyleButton(THEME_COLOR);
    if (ImGui::Button("Revert")) {
        LoadBindingsForDevice();
        mDirty = false;
        mStatusMessage = "Reverted to the mapping SDL is currently using.";
    }
    PopStyleButton();
    ImGui::EndDisabled();
    Tooltip("Discards unsaved changes and reloads the mapping SDL is using for this device.");

    ImGui::SameLine();
    ImGui::BeginDisabled(busy || !haveDevice);
    PushStyleButton(THEME_COLOR);
    if (ImGui::Button("Clear All Bindings")) {
        ClearBindings(mBindings);
        mDirty = true;
    }
    PopStyleButton();
    ImGui::EndDisabled();

    const bool haveUserMapping =
        mSelectedDevice >= 0 && mSelectedDevice < (int32_t)mDevices.size() && mDevices[mSelectedDevice].hasUserMapping;

    ImGui::BeginDisabled(busy || !haveUserMapping);
    PushStyleButton(Colors::DarkRed);
    if (ImGui::Button("Delete Saved Mapping")) {
        if (DeleteUserMapping(mDevices[mSelectedDevice].guid)) {
            mStatusMessage = StringHelper::Sprintf("Removed from %s. Restart to fall back to gamecontrollerdb.txt.",
                                                   kUserMappingsFileName);
            RefreshDeviceList();
        } else {
            mStatusMessage = "No saved mapping to remove.";
        }
    }
    PopStyleButton();
    ImGui::EndDisabled();
    Tooltip("SDL keeps the mapping in memory until the game restarts, so the device stays usable this session.");

    ImGui::SameLine();
    ImGui::BeginDisabled(busy);
    PushStyleButton(THEME_COLOR);
    if (ImGui::Button("Reload From File")) {
        if (LoadUserMappings()) {
            LoadBindingsForDevice();
            mDirty = false;
            RefreshDeviceList();
            mStatusMessage = StringHelper::Sprintf("Reloaded %s.", kUserMappingsFileName);
        } else {
            mStatusMessage = StringHelper::Sprintf("No %s to load.", kUserMappingsFileName);
        }
    }
    PopStyleButton();
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::BeginDisabled(busy);
    Checkbox("Include extra buttons", &mIncludeExtraButtons,
             CheckboxOptions()
                 .Color(THEME_COLOR)
                 .Tooltip("Adds Guide, Misc and paddle/touchpad buttons - which most pads do not have - to the "
                          "diagram and the Map All walk."));
    ImGui::EndDisabled();
}

void MapperWindow::DrawMappingStringPreview() {
    if (mJoystick == nullptr || !ImGui::CollapsingHeader("Mapping string")) {
        return;
    }

    const std::string mapping = GenerateMappingString(mJoystick, mBindings, mCustomName);
    ImGui::TextWrapped("%s", mapping.c_str());
    PushStyleButton(THEME_COLOR);
    if (ImGui::Button("Copy to Clipboard")) {
        ImGui::SetClipboardText(mapping.c_str());
        mStatusMessage = "Mapping string copied to the clipboard.";
    }
    PopStyleButton();
}

void MapperWindow::DrawElement() {
    static bool sDiagramTextureRequested = false;
    if (!sDiagramTextureRequested) {
        sDiagramTextureRequested = true;
        auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
        auto archives = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager();
        if (gui != nullptr && archives != nullptr && archives->HasFile(kDiagramTexturePath)) {
            gui->LoadTextureFromRawImage(kDiagramTextureName, kDiagramTexturePath);
        }
    }

    ImGui::PushFont(OTRGlobals::Instance->fontMonoLarger);

    DrawDeviceSelector();
    Separator();
    DrawDiagram();
    DrawSessionPrompt();
    Separator();
    DrawActionButtons();

    if (mDirty) {
        ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.2f, 1.0f), "Unsaved changes.");
    }
    if (!mStatusMessage.empty()) {
        ImGui::TextWrapped("%s", mStatusMessage.c_str());
    }

    DrawMappingStringPreview();

    ImGui::PopFont();
}

// ---------------------------------------------------------------------------------------------------
// Menu registration
// ---------------------------------------------------------------------------------------------------

static void RegisterMapperWidgets() {
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->AddGuiWindow(
        std::make_shared<MapperWindow>(CVAR_WINDOW("GamepadMapper"), "Gamepad Mapper", ImVec2(1280, 820)));
    WidgetPath path = { "Settings", "Controls", SECTION_COLUMN_2 };
    SohGui::mSohMenu->AddWidget(path, "Gamepad Mapper", WIDGET_SEPARATOR_TEXT);
    SohGui::mSohMenu->AddWidget(path, "Open Gamepad Mapper", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("GamepadMapper"))
        .RaceDisable(false)
        .WindowName("Gamepad Mapper")
        .HideInSearch(true)
        .Options(WindowButtonOptions()
                     .Tooltip("Build an SDL mapping for a gamepad the game does not recognize, or fix one that is "
                              "mapped wrong. Saves to usergamepadmappings.txt.")
                     .EmbedWindow(false));
}

static RegisterMenuInitFunc menuInitFunc(RegisterMapperWidgets);

} // namespace Mapper
