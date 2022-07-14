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
            supportsGyro = true;
        }

        char GuidBuf[33];
        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(physicalSlot), GuidBuf, sizeof(GuidBuf));
        GUID = std::string(GuidBuf);
        Cont = NewCont;
        wCamX = 0;
        wCamY = 0;

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


    void SDLController::NormalizeStickAxis(int16_t wAxisValueX, int16_t wAxisValueY, int16_t wAxisThreshold, bool isRightStick, float sensitivity) {
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
            auto slope = ay / ax;
            auto edgex = copysign(85.0 / (abs(slope) + 16.0 / 69.0), ax);
            auto edgey = copysign(std::min(abs(edgex * slope), 85.0 / (1.0 / abs(slope) + 16.0 / 69.0)), ay);
            edgex = edgey / slope;

            auto scale = sqrt(edgex * edgex + edgey * edgey) / 85.0;
            ax *= scale;
            ay *= scale;
        }

        if (!isRightStick) {
            wStickX = +ax;
            wStickY = -ay;
        } else {
            wCamX = +ax * sensitivity;
            wCamY = -ay * sensitivity;
        }
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
            const auto AxisValue = SDL_GameControllerGetAxis(Cont, Axis) / 32767.0f;

            if (AxisValue < -0.7f) {
                return -(Axis + AXIS_SCANCODE_BIT);
            }

            if (AxisValue > 0.7f) {
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

        if (supportsGyro && profile.UseGyro) {

            float gyroData[3];
            SDL_GameControllerGetSensorData(Cont, SDL_SENSOR_GYRO, gyroData, 3);

            float gyro_drift_x = profile.Thresholds[DRIFT_X] / 100.0f;
            float gyro_drift_y = profile.Thresholds[DRIFT_Y] / 100.0f;
            const float gyro_sensitivity = profile.Thresholds[GYRO_SENSITIVITY];

            if (gyro_drift_x == 0) {
                gyro_drift_x = gyroData[0];
            }

            if (gyro_drift_y == 0) {
                gyro_drift_y = gyroData[1];
            }

            profile.Thresholds[DRIFT_X] = gyro_drift_x * 100.0f;
            profile.Thresholds[DRIFT_Y] = gyro_drift_y * 100.0f;

            wGyroX = gyroData[0] - gyro_drift_x;
            wGyroY = gyroData[1] - gyro_drift_y;

            wGyroX *= gyro_sensitivity;
            wGyroY *= gyro_sensitivity;
        }

        dwPressedButtons[slot] = 0;

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

        SDL_GameControllerAxis LStickAxisX = SDL_CONTROLLER_AXIS_INVALID;
        SDL_GameControllerAxis LStickAxisY = SDL_CONTROLLER_AXIS_INVALID;
        int32_t LStickDeadzone = 0;

        SDL_GameControllerAxis RStickAxisX = SDL_CONTROLLER_AXIS_INVALID;
        SDL_GameControllerAxis RStickAxisY = SDL_CONTROLLER_AXIS_INVALID;
        int32_t RStickDeadzone = 0;

        for (int32_t i = SDL_CONTROLLER_AXIS_LEFTX; i < SDL_CONTROLLER_AXIS_MAX; i++) {
            const auto Axis = static_cast<SDL_GameControllerAxis>(i);
            const auto PosScancode = i + AXIS_SCANCODE_BIT;
            const auto NegScancode = -PosScancode;
            const auto AxisThreshold = static_cast<int>(profile.Thresholds[SDLAxisToThreshold(i)]);
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
                    if (LStickAxisX != SDL_CONTROLLER_AXIS_INVALID && LStickAxisX != Axis) {
                        SPDLOG_TRACE("Invalid PosStickX configured. Neg was {} and Pos is {}", LStickAxisX, Axis);
                    }

                    if (LStickDeadzone != 0 && LStickDeadzone != AxisThreshold) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Up/Down was {} and Left/Right is {}", LStickDeadzone, AxisThreshold);
                    }

                    LStickDeadzone = AxisThreshold;
                    LStickAxisX = Axis;
                }

                if (PosButton == BTN_STICKUP || PosButton == BTN_STICKDOWN) {
                    if (LStickAxisY != SDL_CONTROLLER_AXIS_INVALID && LStickAxisY != Axis) {
                        SPDLOG_TRACE("Invalid PosStickY configured. Neg was {} and Pos is {}", LStickAxisY, Axis);
                    }

                    if (LStickDeadzone != 0 && LStickDeadzone != AxisThreshold) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Left/Right was {} and Up/Down is {}", LStickDeadzone, AxisThreshold);
                    }

                    LStickDeadzone = AxisThreshold;
                    LStickAxisY = Axis;
                }

                if (NegButton == BTN_STICKLEFT || NegButton == BTN_STICKRIGHT) {
                    if (LStickAxisX != SDL_CONTROLLER_AXIS_INVALID && LStickAxisX != Axis) {
                        SPDLOG_TRACE("Invalid NegStickX configured. Pos was {} and Neg is {}", LStickAxisX, Axis);
                    }

                    if (LStickDeadzone != 0 && LStickDeadzone != AxisThreshold) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Left/Right was {} and Up/Down is {}", LStickDeadzone, AxisThreshold);
                    }

                    LStickDeadzone = AxisThreshold;
                    LStickAxisX = Axis;
                }

                if (NegButton == BTN_STICKUP || NegButton == BTN_STICKDOWN) {
                    if (LStickAxisY != SDL_CONTROLLER_AXIS_INVALID && LStickAxisY != Axis) {
                        SPDLOG_TRACE("Invalid NegStickY configured. Pos was {} and Neg is {}", LStickAxisY, Axis);
                    }

                    if (LStickDeadzone != 0 && LStickDeadzone != AxisThreshold) {
                        SPDLOG_TRACE("Invalid Deadzone misconfigured. Left/Right was {} and Up/Down is {}", LStickDeadzone, AxisThreshold);
                    }

                    LStickDeadzone = AxisThreshold;
                    LStickAxisY = Axis;
                }
            }

            if (LStickAxisX != SDL_CONTROLLER_AXIS_INVALID && LStickAxisY != SDL_CONTROLLER_AXIS_INVALID) {
                const auto AxisValueX = SDL_GameControllerGetAxis(Cont, LStickAxisX);
                const auto AxisValueY = SDL_GameControllerGetAxis(Cont, LStickAxisY);
                NormalizeStickAxis(AxisValueX, AxisValueY, LStickDeadzone, false, profile.Thresholds[SENSITIVITY]);
            }

            // Right Stick
            // If the axis is NOT mapped to the control stick.
            if (!(
                PosButton == BTN_VSTICKLEFT || PosButton == BTN_VSTICKRIGHT ||
                PosButton == BTN_VSTICKUP || PosButton == BTN_VSTICKDOWN ||
                NegButton == BTN_VSTICKLEFT || NegButton == BTN_VSTICKRIGHT ||
                NegButton == BTN_VSTICKUP || NegButton == BTN_VSTICKDOWN)) {

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

            } else {
                if (PosButton == BTN_VSTICKLEFT || PosButton == BTN_VSTICKRIGHT) {
                    if (RStickAxisX != SDL_CONTROLLER_AXIS_INVALID && RStickAxisX != Axis) {
                        SPDLOG_TRACE("Invalid PosStickX configured. Neg was {} and Pos is {}", RStickAxisX, Axis);
                    }

                    if (RStickDeadzone != 0 && RStickDeadzone != AxisThreshold) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Up/Down was {} and Left/Right is {}", RStickDeadzone, AxisThreshold);
                    }

                    RStickDeadzone = AxisThreshold;
                    RStickAxisX = Axis;
                }

                if (PosButton == BTN_VSTICKUP || PosButton == BTN_VSTICKDOWN) {
                    if (RStickAxisY != SDL_CONTROLLER_AXIS_INVALID && RStickAxisY != Axis) {
                        SPDLOG_TRACE("Invalid PosStickY configured. Neg was {} and Pos is {}", RStickAxisY, Axis);
                    }

                    if (RStickDeadzone != 0 && RStickDeadzone != AxisThreshold) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Left/Right was {} and Up/Down is {}", RStickDeadzone, AxisThreshold);
                    }

                    RStickDeadzone = AxisThreshold;
                    RStickAxisY = Axis;
                }

                if (NegButton == BTN_VSTICKLEFT || NegButton == BTN_VSTICKRIGHT) {
                    if (RStickAxisX != SDL_CONTROLLER_AXIS_INVALID && RStickAxisX != Axis) {
                        SPDLOG_TRACE("Invalid NegStickX configured. Pos was {} and Neg is {}", RStickAxisX, Axis);
                    }

                    if (RStickDeadzone != 0 && RStickDeadzone != AxisThreshold) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Left/Right was {} and Up/Down is {}", RStickDeadzone, AxisThreshold);
                    }

                    RStickDeadzone = AxisThreshold;
                    RStickAxisX = Axis;
                }

                if (NegButton == BTN_VSTICKUP || NegButton == BTN_VSTICKDOWN) {
                    if (RStickAxisY != SDL_CONTROLLER_AXIS_INVALID && RStickAxisY != Axis) {
                        SPDLOG_TRACE("Invalid NegStickY configured. Pos was {} and Neg is {}", RStickAxisY, Axis);
                    }

                    if (RStickDeadzone != 0 && RStickDeadzone != AxisThreshold) {
                        SPDLOG_TRACE("Invalid Deadzone misconfigured. Left/Right was {} and Up/Down is {}", RStickDeadzone, AxisThreshold);
                    }

                    RStickDeadzone = AxisThreshold;
                    RStickAxisY = Axis;
                }
            }

            if (RStickAxisX != SDL_CONTROLLER_AXIS_INVALID && RStickAxisY != SDL_CONTROLLER_AXIS_INVALID) {
                const auto AxisValueX = SDL_GameControllerGetAxis(Cont, RStickAxisX);
                const auto AxisValueY = SDL_GameControllerGetAxis(Cont, RStickAxisY);
                NormalizeStickAxis(AxisValueX, AxisValueY, RStickDeadzone, true, profile.Thresholds[SENSITIVITY]);
            }
        }
    }

    void SDLController::WriteToSource(int32_t slot, ControllerCallback* controller)
    {
        if (CanRumble() && profiles[slot].UseRumble) {
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
        std::map<int32_t, int32_t>& Mappings = profiles[slot].Mappings;
        const auto find = std::find_if(Mappings.begin(), Mappings.end(), [n64Button](const std::pair<int32_t, int32_t>& pair) {
            return pair.second == n64Button;
        });

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
        profile.Mappings.clear();

        profile.UseRumble = true;
        profile.RumbleStrength = 1.0f;
        profile.UseGyro = false;
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
        profile.Thresholds[LEFT_STICK] = 16.0f;
        profile.Thresholds[RIGHT_STICK] = 16.0f;
        profile.Thresholds[LEFT_TRIGGER] = 0x1E00;
        profile.Thresholds[RIGHT_TRIGGER] = 0x1E00;
        profile.Thresholds[DRIFT_X] = 0.0f;
        profile.Thresholds[DRIFT_Y] = 0.0f;
        profile.Thresholds[SENSITIVITY] = 16.0f;
        profile.Thresholds[GYRO_SENSITIVITY] = 1.0f;
    }
}
