#include "SDLController.h"

#include "spdlog/spdlog.h"
#include "Window.h"
#include <Utils/StringHelper.h>

#ifdef _MSC_VER
#define strdup _strdup
#endif

extern "C" uint8_t __osMaxControllers;

namespace Ship {

    SDLController::SDLController(int32_t physicalSlot) : Controller(), Cont(nullptr), physicalSlot(physicalSlot) {

    }

    bool SDLController::Open() {
        const auto NewCont = SDL_GameControllerOpen(physicalSlot);

        // We failed to load the controller. Go to next.
        if (NewCont == nullptr) {
            SPDLOG_ERROR("SDL Controller failed to open: ({})", SDL_GetError());
            return false;
        }

        supportsGyro = false;
        if (SDL_GameControllerHasSensor(NewCont, SDL_SENSOR_GYRO)) {
            SDL_GameControllerSetSensorEnabled(NewCont, SDL_SENSOR_GYRO, SDL_TRUE);
            supportsGyro = true;
        }

        char GuidBuf[33];
        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(physicalSlot), GuidBuf, sizeof(GuidBuf));
        Cont = NewCont;

#ifdef __SWITCH__
        GUID = StringHelper::Sprintf("%s:%d", GuidBuf, physicalSlot);
        ControllerName = StringHelper::Sprintf("%s #%d", SDL_GameControllerNameForIndex(physicalSlot), physicalSlot + 1);
#else
        GUID = std::string(GuidBuf);
        ControllerName = std::string(SDL_GameControllerNameForIndex(physicalSlot));
#endif
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

        return true;
    }

    void SDLController::NormalizeStickAxis(SDL_GameControllerAxis axisX, SDL_GameControllerAxis axisY, int16_t axisThreshold, int32_t virtualSlot) {
        auto profile = getProfile(virtualSlot);

        const auto axisValueX = SDL_GameControllerGetAxis(Cont, axisX);
        const auto axisValueY = SDL_GameControllerGetAxis(Cont, axisY);

        //scale {-32768 ... +32767} to {-84 ... +84}
        auto ax = axisValueX * 85.0f / 32767.0f;
        auto ay = axisValueY * 85.0f / 32767.0f;

        //create scaled circular dead-zone in range {-15 ... +15}
        auto len = sqrt(ax * ax + ay * ay);
        if (len < axisThreshold) {
            len = 0.0f;
        }
        else if (len > 85.0) {
            len = 85.0f / len;
        }
        else {
            len = (len - axisThreshold) * 85.0f / (85.0f - axisThreshold) / len;
        }
        ax *= len;
        ay *= len;

        //bound diagonals to an octagonal range {-68 ... +68}
        if (ax != 0.0f && ay != 0.0f) {
            auto slope = ay / ax;
            auto edgex = copysign(85.0f / (abs(slope) + 16.0f / 69.0f), ax);
            auto edgey = copysign(std::min(abs(edgex * slope), 85.0f / (1.0f / abs(slope) + 16.0f / 69.0f)), ay);
            edgex = edgey / slope;

            auto scale = sqrt(edgex * edgex + edgey * edgey) / 85.0f;
            ax *= scale;
            ay *= scale;
        }

        if (axisX == SDL_CONTROLLER_AXIS_LEFTX) {
            getLeftStickX(virtualSlot) = +ax;
            getLeftStickY(virtualSlot) = -ay;
        } else if (axisX == SDL_CONTROLLER_AXIS_RIGHTX) {
            getRightStickX(virtualSlot) = +ax;
            getRightStickY(virtualSlot) = -ay;
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

    void SDLController::ReadFromSource(int32_t virtualSlot) {
        auto profile = getProfile(virtualSlot);

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

        if (supportsGyro && profile->UseGyro) {

            float gyroData[3];
            SDL_GameControllerGetSensorData(Cont, SDL_SENSOR_GYRO, gyroData, 3);

            float gyro_drift_x = profile->GyroData[DRIFT_X] / 100.0f;
            float gyro_drift_y = profile->GyroData[DRIFT_Y] / 100.0f;
            const float gyro_sensitivity = profile->GyroData[GYRO_SENSITIVITY];

            if (gyro_drift_x == 0) {
                gyro_drift_x = gyroData[0];
            }

            if (gyro_drift_y == 0) {
                gyro_drift_y = gyroData[1];
            }

            profile->GyroData[DRIFT_X] = gyro_drift_x * 100.0f;
            profile->GyroData[DRIFT_Y] = gyro_drift_y * 100.0f;

            getGyroX(virtualSlot) = gyroData[0] - gyro_drift_x;
            getGyroY(virtualSlot) = gyroData[1] - gyro_drift_y;

            getGyroX(virtualSlot) *= gyro_sensitivity;
            getGyroY(virtualSlot) *= gyro_sensitivity;
        }
        else {
            getGyroX(virtualSlot) = 0;
            getGyroY(virtualSlot) = 0;
        }

        getPressedButtons(virtualSlot) = 0;

        for (int32_t i = SDL_CONTROLLER_BUTTON_A; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
            if (profile->Mappings.contains(i)) {
                if (SDL_GameControllerGetButton(Cont, static_cast<SDL_GameControllerButton>(i))) {
                    getPressedButtons(virtualSlot) |= profile->Mappings[i];
                }
                else {
                    getPressedButtons(virtualSlot) &= ~profile->Mappings[i];
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
            const auto PosScancode = i | AXIS_SCANCODE_BIT;
            const auto NegScancode = -PosScancode;
            const auto AxisDeadzone = profile->AxisDeadzones[i];
            const auto AxisMinimumPress = profile->AxisMinimumPress[i];
            const auto PosButton = profile->Mappings[PosScancode];
            const auto NegButton = profile->Mappings[NegScancode];
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
                NegButton == BTN_STICKUP || NegButton == BTN_STICKDOWN ||
                PosButton == BTN_VSTICKLEFT || PosButton == BTN_VSTICKRIGHT ||
                PosButton == BTN_VSTICKUP || PosButton == BTN_VSTICKDOWN ||
                NegButton == BTN_VSTICKLEFT || NegButton == BTN_VSTICKRIGHT ||
                NegButton == BTN_VSTICKUP || NegButton == BTN_VSTICKDOWN)) {

                // The axis is being treated as a "button"
                if (AxisValue > AxisMinimumPress) {
                    getPressedButtons(virtualSlot) |= PosButton;
                    getPressedButtons(virtualSlot) &= ~NegButton;
                }
                else if (AxisValue < -AxisMinimumPress) {
                    getPressedButtons(virtualSlot) &= ~PosButton;
                    getPressedButtons(virtualSlot) |= NegButton;
                }
                else {
                    getPressedButtons(virtualSlot) &= ~PosButton;
                    getPressedButtons(virtualSlot) &= ~NegButton;
                }
            } else {
                // The axis is being treated as a "stick"

                // Left stick
                if (PosButton == BTN_STICKLEFT || PosButton == BTN_STICKRIGHT) {
                    if (LStickAxisX != SDL_CONTROLLER_AXIS_INVALID && LStickAxisX != Axis) {
                        SPDLOG_TRACE("Invalid PosStickX configured. Neg was {} and Pos is {}", LStickAxisX, Axis);
                    }

                    if (LStickDeadzone != 0 && LStickDeadzone != AxisDeadzone) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Up/Down was {} and Left/Right is {}", LStickDeadzone, AxisDeadzone);
                    }

                    LStickDeadzone = AxisDeadzone;
                    LStickAxisX = Axis;
                }

                if (PosButton == BTN_STICKUP || PosButton == BTN_STICKDOWN) {
                    if (LStickAxisY != SDL_CONTROLLER_AXIS_INVALID && LStickAxisY != Axis) {
                        SPDLOG_TRACE("Invalid PosStickY configured. Neg was {} and Pos is {}", LStickAxisY, Axis);
                    }

                    if (LStickDeadzone != 0 && LStickDeadzone != AxisDeadzone) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Left/Right was {} and Up/Down is {}", LStickDeadzone, AxisDeadzone);
                    }

                    LStickDeadzone = AxisDeadzone;
                    LStickAxisY = Axis;
                }

                if (NegButton == BTN_STICKLEFT || NegButton == BTN_STICKRIGHT) {
                    if (LStickAxisX != SDL_CONTROLLER_AXIS_INVALID && LStickAxisX != Axis) {
                        SPDLOG_TRACE("Invalid NegStickX configured. Pos was {} and Neg is {}", LStickAxisX, Axis);
                    }

                    if (LStickDeadzone != 0 && LStickDeadzone != AxisDeadzone) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Left/Right was {} and Up/Down is {}", LStickDeadzone, AxisDeadzone);
                    }

                    LStickDeadzone = AxisDeadzone;
                    LStickAxisX = Axis;
                }

                if (NegButton == BTN_STICKUP || NegButton == BTN_STICKDOWN) {
                    if (LStickAxisY != SDL_CONTROLLER_AXIS_INVALID && LStickAxisY != Axis) {
                        SPDLOG_TRACE("Invalid NegStickY configured. Pos was {} and Neg is {}", LStickAxisY, Axis);
                    }

                    if (LStickDeadzone != 0 && LStickDeadzone != AxisDeadzone) {
                        SPDLOG_TRACE("Invalid Deadzone misconfigured. Left/Right was {} and Up/Down is {}", LStickDeadzone, AxisDeadzone);
                    }

                    LStickDeadzone = AxisDeadzone;
                    LStickAxisY = Axis;
                }

                // Right Stick
                if (PosButton == BTN_VSTICKLEFT || PosButton == BTN_VSTICKRIGHT) {
                    if (RStickAxisX != SDL_CONTROLLER_AXIS_INVALID && RStickAxisX != Axis) {
                        SPDLOG_TRACE("Invalid PosStickX configured. Neg was {} and Pos is {}", RStickAxisX, Axis);
                    }

                    if (RStickDeadzone != 0 && RStickDeadzone != AxisDeadzone) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Up/Down was {} and Left/Right is {}", RStickDeadzone, AxisDeadzone);
                    }

                    RStickDeadzone = AxisDeadzone;
                    RStickAxisX = Axis;
                }

                if (PosButton == BTN_VSTICKUP || PosButton == BTN_VSTICKDOWN) {
                    if (RStickAxisY != SDL_CONTROLLER_AXIS_INVALID && RStickAxisY != Axis) {
                        SPDLOG_TRACE("Invalid PosStickY configured. Neg was {} and Pos is {}", RStickAxisY, Axis);
                    }

                    if (RStickDeadzone != 0 && RStickDeadzone != AxisDeadzone) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Left/Right was {} and Up/Down is {}", RStickDeadzone, AxisDeadzone);
                    }

                    RStickDeadzone = AxisDeadzone;
                    RStickAxisY = Axis;
                }

                if (NegButton == BTN_VSTICKLEFT || NegButton == BTN_VSTICKRIGHT) {
                    if (RStickAxisX != SDL_CONTROLLER_AXIS_INVALID && RStickAxisX != Axis) {
                        SPDLOG_TRACE("Invalid NegStickX configured. Pos was {} and Neg is {}", RStickAxisX, Axis);
                    }

                    if (RStickDeadzone != 0 && RStickDeadzone != AxisDeadzone) {
                        SPDLOG_TRACE("Invalid Deadzone configured. Left/Right was {} and Up/Down is {}", RStickDeadzone, AxisDeadzone);
                    }

                    RStickDeadzone = AxisDeadzone;
                    RStickAxisX = Axis;
                }

                if (NegButton == BTN_VSTICKUP || NegButton == BTN_VSTICKDOWN) {
                    if (RStickAxisY != SDL_CONTROLLER_AXIS_INVALID && RStickAxisY != Axis) {
                        SPDLOG_TRACE("Invalid NegStickY configured. Pos was {} and Neg is {}", RStickAxisY, Axis);
                    }

                    if (RStickDeadzone != 0 && RStickDeadzone != AxisDeadzone) {
                        SPDLOG_TRACE("Invalid Deadzone misconfigured. Left/Right was {} and Up/Down is {}", RStickDeadzone, AxisDeadzone);
                    }

                    RStickDeadzone = AxisDeadzone;
                    RStickAxisY = Axis;
                }
            }
        }

        if (LStickAxisX != SDL_CONTROLLER_AXIS_INVALID && LStickAxisY != SDL_CONTROLLER_AXIS_INVALID) {
            NormalizeStickAxis(LStickAxisX, LStickAxisY, LStickDeadzone, virtualSlot);
        }

        if (RStickAxisX != SDL_CONTROLLER_AXIS_INVALID && RStickAxisY != SDL_CONTROLLER_AXIS_INVALID) {
            NormalizeStickAxis(RStickAxisX, RStickAxisY, RStickDeadzone, virtualSlot);
        }
    }

    void SDLController::WriteToSource(int32_t virtualSlot, ControllerCallback* controller)
    {
        if (CanRumble() && getProfile(virtualSlot)->UseRumble) {
            if (controller->rumble > 0) {
                float rumble_strength = getProfile(virtualSlot)->RumbleStrength;
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
    
    const std::string SDLController::GetButtonName(int32_t virtualSlot, int32_t n64Button) {
        char buffer[50];
        std::map<int32_t, int32_t>& Mappings = getProfile(virtualSlot)->Mappings;

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

    const std::string SDLController::GetControllerName() {
        return ControllerName;
    }

    void SDLController::CreateDefaultBinding(int32_t virtualSlot) {
        auto profile = getProfile(virtualSlot);
        profile->Mappings.clear();
        profile->AxisDeadzones.clear();
        profile->AxisMinimumPress.clear();
        profile->GyroData.clear();

        profile->Version = DEVICE_PROFILE_CURRENT_VERSION;
        profile->UseRumble = true;
        profile->RumbleStrength = 1.0f;
        profile->UseGyro = false;

        profile->Mappings[SDL_CONTROLLER_AXIS_RIGHTX | AXIS_SCANCODE_BIT] = BTN_CRIGHT;
        profile->Mappings[-(SDL_CONTROLLER_AXIS_RIGHTX | AXIS_SCANCODE_BIT)] = BTN_CLEFT;
        profile->Mappings[SDL_CONTROLLER_AXIS_RIGHTY | AXIS_SCANCODE_BIT] = BTN_CDOWN;
        profile->Mappings[-(SDL_CONTROLLER_AXIS_RIGHTY | AXIS_SCANCODE_BIT)] = BTN_CUP;
        profile->Mappings[SDL_CONTROLLER_AXIS_LEFTX | AXIS_SCANCODE_BIT] = BTN_STICKRIGHT;
        profile->Mappings[-(SDL_CONTROLLER_AXIS_LEFTX | AXIS_SCANCODE_BIT)] = BTN_STICKLEFT;
        profile->Mappings[SDL_CONTROLLER_AXIS_LEFTY | AXIS_SCANCODE_BIT] = BTN_STICKDOWN;
        profile->Mappings[-(SDL_CONTROLLER_AXIS_LEFTY | AXIS_SCANCODE_BIT)] = BTN_STICKUP;
        profile->Mappings[SDL_CONTROLLER_AXIS_TRIGGERRIGHT | AXIS_SCANCODE_BIT] = BTN_R;
        profile->Mappings[SDL_CONTROLLER_AXIS_TRIGGERLEFT | AXIS_SCANCODE_BIT] = BTN_Z;
        profile->Mappings[SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = BTN_L;
        profile->Mappings[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = BTN_DRIGHT;
        profile->Mappings[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = BTN_DLEFT;
        profile->Mappings[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = BTN_DDOWN;
        profile->Mappings[SDL_CONTROLLER_BUTTON_DPAD_UP] = BTN_DUP;
        profile->Mappings[SDL_CONTROLLER_BUTTON_START] = BTN_START;
        profile->Mappings[SDL_CONTROLLER_BUTTON_B] = BTN_B;
        profile->Mappings[SDL_CONTROLLER_BUTTON_A] = BTN_A;
        profile->Mappings[SDL_CONTROLLER_BUTTON_LEFTSTICK] = BTN_MODIFIER1;
        profile->Mappings[SDL_CONTROLLER_BUTTON_RIGHTSTICK] = BTN_MODIFIER2;

        for (int32_t i = SDL_CONTROLLER_AXIS_LEFTX; i < SDL_CONTROLLER_AXIS_MAX; i++) {
            profile->AxisDeadzones[i] = 16.0f;
            profile->AxisMinimumPress[i] = 7680.0f;
        }

        profile->GyroData[DRIFT_X] = 0.0f;
        profile->GyroData[DRIFT_Y] = 0.0f;
        profile->GyroData[GYRO_SENSITIVITY] = 1.0f;
    }

    bool SDLController::Connected() const {
	    return Cont != nullptr;
    }

    bool SDLController::CanGyro() const {
	    return supportsGyro;
    }

    bool SDLController::CanRumble() const {
#if SDL_COMPILEDVERSION >= SDL_VERSIONNUM(2,0,18)
        return SDL_GameControllerHasRumble(Cont);
#endif
        return false;
    }

    void SDLController::ClearRawPress() {
	    
    }
}
