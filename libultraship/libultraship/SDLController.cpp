#include "SDLController.h"
#include "GlobalCtx2.h"
#include "spdlog/spdlog.h"
#include "Window.h"
#include <Utils/StringHelper.h>

extern "C" uint8_t __osMaxControllers;

namespace Ship {

    bool SDLController::Open() {

        const auto NewCont = SDL_GameControllerOpen(physicalSlot);

        // We failed to load the controller. Go to next.
        if (NewCont == nullptr) {
            SPDLOG_ERROR("SDL Controller failed to open: ({})", SDL_GetError());
            return false;
        }

        if (SDL_GameControllerHasSensor(NewCont, SDL_SENSOR_GYRO)) {
            SDL_GameControllerSetSensorEnabled(NewCont, SDL_SENSOR_GYRO, SDL_TRUE);
        }

        char GuidBuf[33];
        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(physicalSlot), GuidBuf, sizeof(GuidBuf));
        GUID = std::string(GuidBuf);
        Cont = NewCont;

        return true;
    }

    bool SDLController::Close() {
        if (Cont != nullptr && SDL_WasInit(SDL_INIT_GAMECONTROLLER)) {
            if (CanRumble()) {
                SDL_GameControllerRumble(Cont, 0, 0, 0);
            }
            SDL_GameControllerClose(Cont);
        }
        Cont = nullptr;
        wStickX = 0;
        wStickY = 0;

        return true;
    }


    void SDLController::NormalizeStickAxis(int16_t wAxisValueX, int16_t wAxisValueY, int16_t wAxisThreshold) {
        //scale {-32768 ... +32767} to {-84 ... +84}
        auto ax = wAxisValueX * 85.0 / 32767.0;
        auto ay = wAxisValueY * 85.0 / 32767.0;

        //create scaled circular dead-zone in range {-15 ... +15}
        auto len = sqrt(ax * ax + ay * ay);
        if (len < wAxisThreshold) {
            len = 0;
        }
        else if (len > 85.0) {
            len = 85.0 / len;
        }
        else {
            len = (len - wAxisThreshold) * 85.0 / (85.0 - wAxisThreshold) / len;
        }
        ax *= len;
        ay *= len;

        //bound diagonals to an octagonal range {-68 ... +68}
        if (ax != 0.0 && ay != 0.0) {
            const auto slope = ay / ax;
            auto edge_x = copysign(85.0 / (abs(slope) + 16.0 / 69.0), ax);
            const auto edge_y = copysign(std::min(abs(edge_x * slope), 85.0 / (1.0 / abs(slope) + 16.0 / 69.0)), ay);
            edge_x = edge_y / slope;

            const auto scale = sqrt(edge_x * edge_x + edge_y * edge_y) / 85.0;
            ax *= scale;
            ay *= scale;
        }

        wStickX = +ax;
        wStickY = -ay;
    }

    int32_t SDLController::ReadRawPress() {
        SDL_GameControllerUpdate();

        for (int32_t i = SDL_CONTROLLER_BUTTON_A; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
            if (SDL_GameControllerGetButton(Cont, static_cast<SDL_GameControllerButton>(i))) {
                return i;
            }
        }

        for (int32_t i = SDL_CONTROLLER_AXIS_LEFTX; i < SDL_CONTROLLER_AXIS_MAX; i++) {
	        const auto Axis = static_cast<SDL_GameControllerAxis>(i);
            const auto AxisValue = SDL_GameControllerGetAxis(Cont, Axis) / 32767;

            if(AxisValue < 0) {
                return -(Axis + AXIS_SCANCODE_BIT);
            }

            if (AxisValue > 0) {
                return (Axis + AXIS_SCANCODE_BIT);
            }
        }

        return -1;
    }

    ControllerThresholds SDLAxisToThreshold( uint32_t axis ){
        switch(axis){
	        case SDL_CONTROLLER_AXIS_LEFTX:
	        case SDL_CONTROLLER_AXIS_LEFTY:
	            return LEFT_STICK;
	        case SDL_CONTROLLER_AXIS_RIGHTX:
	        case SDL_CONTROLLER_AXIS_RIGHTY:
	            return RIGHT_STICK;
	        case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
	            return LEFT_TRIGGER;
	        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
	            return RIGHT_TRIGGER;
            default: return DRIFT_X;
        }
    }

    bool isBindToButton(int32_t btn) {
	    switch(btn) {
		    case BTN_A:
	        case BTN_B:
	        case BTN_L:
	        case BTN_R:
		    case BTN_Z:
	        case BTN_STICKUP:
	        case BTN_STICKDOWN:
	        case BTN_STICKLEFT:
	        case BTN_STICKRIGHT:
	        case BTN_CUP:
	        case BTN_CDOWN:
	        case BTN_CLEFT:
		    case BTN_CRIGHT:
	        case BTN_DUP:
	        case BTN_DDOWN:
	        case BTN_DLEFT:
	        case BTN_DRIGHT:
	            return true;
	    }
        return false;
    }

    void SDLController::ReadFromSource(int32_t slot) {
        DeviceProfile& profile = profiles[slot];

        SDL_GameControllerUpdate();

        // If the controller is disconnected, close it.
        if (Cont != nullptr && !SDL_GameControllerGetAttached(Cont)) {
            Close();
        }

        // Attempt to load the controller if it's not loaded
        if (Cont == nullptr) {
            // If we failed to load the controller, don't process it.
            if (!Open()) {
                return;
            }
        }

        if (SDL_GameControllerHasSensor(Cont, SDL_SENSOR_GYRO)) {

            float gyroData[3];
            SDL_GameControllerGetSensorData(Cont, SDL_SENSOR_GYRO, gyroData, 3);

            float gyro_drift_x = profile.GyroThresholds[DRIFT_X];
            float gyro_drift_y = profile.GyroThresholds[DRIFT_Y];
            const float gyro_sensitivity = profile.GyroThresholds[SENSITIVITY];

            if (gyro_drift_x == 0) {
                gyro_drift_x = gyroData[0];
            }

            if (gyro_drift_y == 0) {
                gyro_drift_y = gyroData[1];
            }

            profile.GyroThresholds[DRIFT_X] = gyro_drift_x;
            profile.GyroThresholds[DRIFT_Y] = gyro_drift_y;

            wGyroX = gyroData[0] - gyro_drift_x;
            wGyroY = gyroData[1] - gyro_drift_y;

            wGyroX *= gyro_sensitivity;
            wGyroY *= gyro_sensitivity;
        }

        for (int32_t i = SDL_CONTROLLER_BUTTON_A; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
            if (profile.Mappings.contains(i)) {
                if (SDL_GameControllerGetButton(Cont, static_cast<SDL_GameControllerButton>(i))) {
                    dwPressedButtons[slot] |= profile.Mappings[i];
                }
                else {
                    dwPressedButtons[slot] &= ~profile.Mappings[i];
                }
            }
        }

        SDL_GameControllerAxis StickAxisX = SDL_CONTROLLER_AXIS_INVALID;
        SDL_GameControllerAxis StickAxisY = SDL_CONTROLLER_AXIS_INVALID;
        int32_t StickDeadzone = 0;

        for (int32_t i = SDL_CONTROLLER_AXIS_LEFTX; i < SDL_CONTROLLER_AXIS_MAX; i++) {
            const auto Axis = static_cast<SDL_GameControllerAxis>(i);
            const auto PosScancode = i + AXIS_SCANCODE_BIT;
            const auto NegScancode = -PosScancode;
            const auto AxisThreshold = profile.Thresholds[SDLAxisToThreshold(i)];
            const auto PosButton = profile.Mappings[PosScancode];
            const auto NegButton = profile.Mappings[NegScancode];
            const auto AxisValue = SDL_GameControllerGetAxis(Cont, Axis);

#ifdef TARGET_WEB
            // Firefox has a bug: https://bugzilla.mozilla.org/show_bug.cgi?id=1606562
            // It sets down y to 32768.0f / 32767.0f, which is greater than the allowed 1.0f,
            // which SDL then converts to a int16_t by multiplying by 32767.0f, which overflows into -32768.
            // Maximum up will hence never become -32768 with the current version of SDL2,
            // so this workaround should be safe in compliant browsers.
            if (AxisValue == -32768) {
                AxisValue = 32767;
            }
#endif

            // If the axis is NOT mapped to the control stick.
            if (!(
                PosButton == BTN_STICKLEFT || PosButton == BTN_STICKRIGHT ||
                PosButton == BTN_STICKUP || PosButton == BTN_STICKDOWN ||
                NegButton == BTN_STICKLEFT || NegButton == BTN_STICKRIGHT ||
                NegButton == BTN_STICKUP || NegButton == BTN_STICKDOWN)) {

                if (AxisValue > 0x1E00) {
                    dwPressedButtons[slot] |= PosButton;
                    dwPressedButtons[slot] &= ~NegButton;
                }
                else if (AxisValue < -0x1E00) {
                    dwPressedButtons[slot] &= ~PosButton;
                    dwPressedButtons[slot] |= NegButton;
                }
                else {
                    dwPressedButtons[slot] &= ~PosButton;
                    dwPressedButtons[slot] &= ~NegButton;
                }
            }
            else {
                if (PosButton == BTN_STICKLEFT || PosButton == BTN_STICKRIGHT) {
                    if (StickAxisX != SDL_CONTROLLER_AXIS_INVALID && StickAxisX != Axis) {
                        SPDLOG_TRACE("Invalid PosStickX configured. Neg was {} and Pos is {}", StickAxisX, Axis);
                    }

                    if (StickDeadzone != 0 && StickDeadzone != AxisThreshold) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Up/Down was {} and Left/Right is {}", StickDeadzone, AxisThreshold);
                    }

                    StickDeadzone = AxisThreshold;
                    StickAxisX = Axis;
                }

                if (PosButton == BTN_STICKUP || PosButton == BTN_STICKDOWN) {
                    if (StickAxisY != SDL_CONTROLLER_AXIS_INVALID && StickAxisY != Axis) {
                        SPDLOG_TRACE("Invalid PosStickY configured. Neg was {} and Pos is {}", StickAxisY, Axis);
                    }

                    if (StickDeadzone != 0 && StickDeadzone != AxisThreshold) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Left/Right was {} and Up/Down is {}", StickDeadzone, AxisThreshold);
                    }

                    StickDeadzone = AxisThreshold;
                    StickAxisY = Axis;
                }

                if (NegButton == BTN_STICKLEFT || NegButton == BTN_STICKRIGHT) {
                    if (StickAxisX != SDL_CONTROLLER_AXIS_INVALID && StickAxisX != Axis) {
                        SPDLOG_TRACE("Invalid NegStickX configured. Pos was {} and Neg is {}", StickAxisX, Axis);
                    }

                    if (StickDeadzone != 0 && StickDeadzone != AxisThreshold) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Left/Right was {} and Up/Down is {}", StickDeadzone, AxisThreshold);
                    }

                    StickDeadzone = AxisThreshold;
                    StickAxisX = Axis;
                }

                if (NegButton == BTN_STICKUP || NegButton == BTN_STICKDOWN) {
                    if (StickAxisY != SDL_CONTROLLER_AXIS_INVALID && StickAxisY != Axis) {
                        SPDLOG_TRACE("Invalid NegStickY configured. Pos was {} and Neg is {}", StickAxisY, Axis);
                    }

                    if (StickDeadzone != 0 && StickDeadzone != AxisThreshold) {
                        SPDLOG_TRACE("Invalid Deadzone misconfigured. Left/Right was {} and Up/Down is {}", StickDeadzone, AxisThreshold);
                    }

                    StickDeadzone = AxisThreshold;
                    StickAxisY = Axis;
                }
            }

            if (StickAxisX != SDL_CONTROLLER_AXIS_INVALID && StickAxisY != SDL_CONTROLLER_AXIS_INVALID) {
                const auto AxisValueX = SDL_GameControllerGetAxis(Cont, StickAxisX);
                const auto AxisValueY = SDL_GameControllerGetAxis(Cont, StickAxisY);
                NormalizeStickAxis(AxisValueX, AxisValueY, StickDeadzone);
            }
        }
    }

    void SDLController::WriteToSource(int32_t slot, ControllerCallback* controller)
    {
        if (CanRumble()) {
            if (controller->rumble > 0) {
                float rumble_strength = profiles[slot].RumbleStrength;
                SDL_GameControllerRumble(Cont, 0xFFFF * rumble_strength, 0xFFFF * rumble_strength, 0);
            }
            else {
                SDL_GameControllerRumble(Cont, 0, 0, 0);
            }
        }

        if (SDL_GameControllerHasLED(Cont)) {
            switch (controller->ledColor) {
            case 0:
                SDL_JoystickSetLED(SDL_GameControllerGetJoystick(Cont), 255, 0, 0);
                break;
            case 1:
                SDL_JoystickSetLED(SDL_GameControllerGetJoystick(Cont), 0x1E, 0x69, 0x1B);
                break;
            case 2:
                SDL_JoystickSetLED(SDL_GameControllerGetJoystick(Cont), 0x64, 0x14, 0x00);
                break;
            case 3:
                SDL_JoystickSetLED(SDL_GameControllerGetJoystick(Cont), 0x00, 0x3C, 0x64);
                break;
            }
        }
    }

    const char* AxisNames[] = {
        "Left Stick X",
        "Left Stick Y",
        "Right Stick X",
        "Right Stick Y",
        "Left Trigger",
        "Right Trigger",
        "Start Button"
    };

    char buffer[50];
    const char* SDLController::GetButtonName(int slot, int n64Button) {
        std::unordered_map<int32_t, int32_t>& Mappings = profiles[slot].Mappings;
        const auto find = std::ranges::find_if(Mappings, [n64Button](const std::pair<int32_t, int32_t>& bin) { return bin.second == n64Button; });

        if (find == Mappings.end()) return "Unknown";

        int btn = abs(find->first);

        if(btn >= AXIS_SCANCODE_BIT) {
            btn -= AXIS_SCANCODE_BIT;

        	snprintf(buffer, sizeof(buffer), "%s%s", AxisNames[btn], find->first > 0 ? "+" : "-");
            return buffer;
        }

        snprintf(buffer, sizeof(buffer), "Button %d", btn);
        return buffer;
    }

    const char* SDLController::GetControllerName() {
        return SDL_GameControllerNameForIndex(physicalSlot);
    }

    void SDLController::CreateDefaultBinding(int32_t slot) {
        DeviceProfile& profile = profiles[slot];

        profile.Mappings[  SDL_CONTROLLER_AXIS_RIGHTX | AXIS_SCANCODE_BIT]  = BTN_CRIGHT;
        profile.Mappings[-(SDL_CONTROLLER_AXIS_RIGHTX | AXIS_SCANCODE_BIT)] = BTN_CLEFT;
        profile.Mappings[  SDL_CONTROLLER_AXIS_RIGHTY | AXIS_SCANCODE_BIT]  = BTN_CDOWN;
        profile.Mappings[-(SDL_CONTROLLER_AXIS_RIGHTY | AXIS_SCANCODE_BIT)] = BTN_CUP;
        profile.Mappings[SDL_CONTROLLER_AXIS_TRIGGERRIGHT + AXIS_SCANCODE_BIT] = BTN_R;
        profile.Mappings[SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = BTN_L;
        profile.Mappings[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = BTN_DRIGHT;
        profile.Mappings[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = BTN_DLEFT;
        profile.Mappings[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = BTN_DDOWN;
        profile.Mappings[SDL_CONTROLLER_BUTTON_DPAD_UP] = BTN_DUP;
        profile.Mappings[SDL_CONTROLLER_BUTTON_START] = BTN_START;
        profile.Mappings[SDL_CONTROLLER_AXIS_TRIGGERLEFT + AXIS_SCANCODE_BIT] = BTN_Z;
        profile.Mappings[SDL_CONTROLLER_BUTTON_B] = BTN_B;
        profile.Mappings[SDL_CONTROLLER_BUTTON_A] = BTN_A;
        profile.Mappings[(SDL_CONTROLLER_AXIS_LEFTX + AXIS_SCANCODE_BIT)] = BTN_STICKRIGHT;
        profile.Mappings[-(SDL_CONTROLLER_AXIS_LEFTX + AXIS_SCANCODE_BIT)] = BTN_STICKLEFT;
        profile.Mappings[SDL_CONTROLLER_AXIS_LEFTY + AXIS_SCANCODE_BIT] = BTN_STICKDOWN;
        profile.Mappings[-(SDL_CONTROLLER_AXIS_LEFTY + AXIS_SCANCODE_BIT)] = BTN_STICKUP;
        profile.Thresholds[LEFT_STICK] = 16.0;
        profile.Thresholds[RIGHT_STICK] = 16.0;
        profile.Thresholds[LEFT_TRIGGER] = 0x1E00;
        profile.Thresholds[RIGHT_TRIGGER] = 0x1E00;
    }
}