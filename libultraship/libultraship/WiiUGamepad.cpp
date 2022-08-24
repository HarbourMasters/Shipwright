#ifdef __WIIU__
#include "WiiUGamepad.h"
#include "ImGuiImpl.h"

#include "WiiUImpl.h"

namespace Ship {
    WiiUGamepad::WiiUGamepad() : Controller(), connected(true), rumblePatternStrength(1.0f) {
        memset(rumblePattern, 0xff, sizeof(rumblePattern));

        GUID = "WiiUGamepad";
    }

    bool WiiUGamepad::Open() {
        VPADReadError error;
        VPADStatus* status = Ship::WiiU::GetVPADStatus(&error);
        if (!status || error == VPAD_READ_INVALID_CONTROLLER) {
            Close();
            return false;
        }

        return true;
    }

    void WiiUGamepad::Close() {
        connected = false;

        for (int i = 0; i < MAXCONTROLLERS; i++) {
            getPressedButtons(i) = 0;
            getLeftStickX(i) = 0;
            getLeftStickY(i) = 0;
            getRightStickX(i) = 0;
            getRightStickY(i) = 0;
            getGyroX(i) = 0;
            getGyroY(i) = 0;
        }
    }

    void WiiUGamepad::ReadFromSource(int32_t virtualSlot) {
        auto profile = getProfile(virtualSlot);

        VPADReadError error;
        VPADStatus* status = Ship::WiiU::GetVPADStatus(&error);
        if (!status) {
            Close();
            return;
        }

        getPressedButtons(virtualSlot) = 0;
        getLeftStickX(virtualSlot) = 0;
        getLeftStickY(virtualSlot) = 0;
        getRightStickX(virtualSlot) = 0;
        getRightStickY(virtualSlot) = 0;
        getGyroX(virtualSlot) = 0;
        getGyroY(virtualSlot) = 0;

        if (error != VPAD_READ_SUCCESS) {
            return; 
        }

        int16_t stickX = 0;
        int16_t stickY = 0;
        int16_t camX = 0;
        int16_t camY = 0;

        for (uint32_t i = VPAD_BUTTON_SYNC; i <= VPAD_STICK_L_EMULATION_LEFT; i <<= 1) {
            if (profile->Mappings.contains(i)) {
                // check if the stick is mapped to an analog stick
                if (i >= VPAD_STICK_R_EMULATION_DOWN) {
                    float axisX = i >= VPAD_STICK_L_EMULATION_DOWN ? status->leftStick.x : status->rightStick.x;
                    float axisY = i >= VPAD_STICK_L_EMULATION_DOWN ? status->leftStick.y : status->rightStick.y;

                    if (profile->Mappings[i] == BTN_STICKRIGHT || profile->Mappings[i] == BTN_STICKLEFT) {
                        stickX = axisX * 85;
                        continue;
                    } else if (profile->Mappings[i] == BTN_STICKDOWN || profile->Mappings[i] == BTN_STICKUP) {
                        stickY = axisY * 85;
                        continue;
                    } else if (profile->Mappings[i] == BTN_VSTICKRIGHT || profile->Mappings[i] == BTN_VSTICKLEFT) {
                        camX = axisX * 85;
                        continue;
                    } else if (profile->Mappings[i] == BTN_VSTICKDOWN || profile->Mappings[i] == BTN_VSTICKUP) {
                        camY = axisY * 85;
                        continue;
                    }
                }

                if (status->hold & i) {
                    getPressedButtons(virtualSlot) |= profile->Mappings[i];
                }
            }
        }

        if (stickX || stickY) {
            NormalizeStickAxis(virtualSlot, stickX, stickY, profile->AxisDeadzones[0], false);
        }

        if (camX || camY) {
            NormalizeStickAxis(virtualSlot, camX, camY, profile->AxisDeadzones[2], true);
        }

        if (profile->UseGyro) {
            float gyroX = status->gyro.x * -8.0f;
            float gyroY = status->gyro.z * 8.0f;

            float gyro_drift_x = profile->GyroData[DRIFT_X] / 100.0f;
            float gyro_drift_y = profile->GyroData[DRIFT_Y] / 100.0f;
            const float gyro_sensitivity = profile->GyroData[GYRO_SENSITIVITY];

            if (gyro_drift_x == 0) {
                gyro_drift_x = gyroX;
            }

            if (gyro_drift_y == 0) {
                gyro_drift_y = gyroY;
            }

            profile->GyroData[DRIFT_X] = gyro_drift_x * 100.0f;
            profile->GyroData[DRIFT_Y] = gyro_drift_y * 100.0f;

            getGyroX(virtualSlot) = gyroX - gyro_drift_x;
            getGyroY(virtualSlot) = gyroY - gyro_drift_y;

            getGyroX(virtualSlot) *= gyro_sensitivity;
            getGyroY(virtualSlot) *= gyro_sensitivity;
        }
    }

    void WiiUGamepad::WriteToSource(int32_t virtualSlot, ControllerCallback* controller) {
        auto profile = getProfile(virtualSlot);

        if (profile->UseRumble) {
            int patternSize = sizeof(rumblePattern) * 8;

            // update rumble pattern if strength changed
            if (rumblePatternStrength != profile->RumbleStrength) {
                rumblePatternStrength = profile->RumbleStrength;
                if (rumblePatternStrength > 1.0f) {
                    rumblePatternStrength = 1.0f;
                } else if (rumblePatternStrength < 0.0f) {
                    rumblePatternStrength = 0.0f;
                }

                memset(rumblePattern, 0, sizeof(rumblePattern));

                // distribute wanted amount of bits equally in pattern
                float scale = (rumblePatternStrength * (1.0f - 0.3f)) + 0.3f;
                int bitcnt = patternSize * scale;
                for (int i = 0; i < bitcnt; i++) {
                    int bitpos = ((i * patternSize) / bitcnt) % patternSize;
                    rumblePattern[bitpos / 8] |= 1 << (bitpos % 8);
                }
            }

            VPADControlMotor(VPAD_CHAN_0, rumblePattern, controller->rumble ? patternSize : 0);
        }
    }

    void WiiUGamepad::ClearRawPress() {
        // Clear already triggered buttons
        VPADReadError error;
        VPADStatus* status = Ship::WiiU::GetVPADStatus(&error);
        if (status) {
            status->trigger = 0;
        }
    }

    int32_t WiiUGamepad::ReadRawPress() {
        VPADReadError error;
        VPADStatus* status = Ship::WiiU::GetVPADStatus(&error);
        if (!status || error != VPAD_READ_SUCCESS) {
            return -1;
        }

        for (uint32_t i = VPAD_BUTTON_SYNC; i <= VPAD_BUTTON_STICK_L; i <<= 1) {
            if (status->trigger & i) {
                return i;
            }
        }

        if (status->leftStick.x > 0.7f) {
            return VPAD_STICK_L_EMULATION_RIGHT;
        }
        if (status->leftStick.x < -0.7f) {
            return VPAD_STICK_L_EMULATION_LEFT;
        }
        if (status->leftStick.y > 0.7f) {
            return VPAD_STICK_L_EMULATION_UP;
        }
        if (status->leftStick.y < -0.7f) {
            return VPAD_STICK_L_EMULATION_DOWN;
        }

        if (status->rightStick.x > 0.7f) {
            return VPAD_STICK_R_EMULATION_RIGHT;
        }
        if (status->rightStick.x < -0.7f) {
            return VPAD_STICK_R_EMULATION_LEFT;
        }
        if (status->rightStick.y > 0.7f) {
            return VPAD_STICK_R_EMULATION_UP;
        }
        if (status->rightStick.y < -0.7f) {
            return VPAD_STICK_R_EMULATION_DOWN;
        }

        return -1;
    }

    const std::string WiiUGamepad::GetButtonName(int32_t virtualSlot, int n64Button) {
        std::map<int32_t, int32_t>& Mappings = getProfile(virtualSlot)->Mappings;
        const auto find = std::find_if(Mappings.begin(), Mappings.end(), [n64Button](const std::pair<int32_t, int32_t>& pair) {
            return pair.second == n64Button;
        });

        if (find == Mappings.end()) return "Unknown";

        uint32_t btn = find->first;
        switch (btn) {
            case VPAD_BUTTON_A: return "A";
            case VPAD_BUTTON_B: return "B";
            case VPAD_BUTTON_X: return "X";
            case VPAD_BUTTON_Y: return "Y";
            case VPAD_BUTTON_LEFT: return "D-pad Left";
            case VPAD_BUTTON_RIGHT: return "D-pad Right";
            case VPAD_BUTTON_UP: return "D-pad Up";
            case VPAD_BUTTON_DOWN: return "D-pad Down";
            case VPAD_BUTTON_ZL: return "ZL";
            case VPAD_BUTTON_ZR: return "ZR";
            case VPAD_BUTTON_L: return "L";
            case VPAD_BUTTON_R: return "R";
            case VPAD_BUTTON_PLUS: return "+ (START)";
            case VPAD_BUTTON_MINUS: return "- (SELECT)";
            case VPAD_BUTTON_STICK_R: return "Stick Button R";
            case VPAD_BUTTON_STICK_L: return "Stick Button L";
            case VPAD_STICK_R_EMULATION_LEFT: return "Right Stick Left";
            case VPAD_STICK_R_EMULATION_RIGHT: return "Right Stick Right";
            case VPAD_STICK_R_EMULATION_UP: return "Right Stick Up";
            case VPAD_STICK_R_EMULATION_DOWN: return "Right Stick Down";
            case VPAD_STICK_L_EMULATION_LEFT: return "Left Stick Left";
            case VPAD_STICK_L_EMULATION_RIGHT: return "Left Stick Right";
            case VPAD_STICK_L_EMULATION_UP: return "Left Stick Up";
            case VPAD_STICK_L_EMULATION_DOWN: return "Left Stick Down";
        }

        return "Unknown";
    }

    const std::string WiiUGamepad::GetControllerName() {
        return Connected() ? "Wii U GamePad" : "Wii U GamePad (Disconnected)";
    }

    void WiiUGamepad::NormalizeStickAxis(int32_t virtualSlot, float x, float y, uint16_t threshold, bool isRightStick) {
        auto profile = getProfile(virtualSlot);

        //create scaled circular dead-zone in range {-15 ... +15}
        auto len = sqrt(x * x + y * y);
        if (len < threshold) {
            len = 0;
        }
        else if (len > 85.0) {
            len = 85.0 / len;
        }
        else {
            len = (len - threshold) * 85.0 / (85.0 - threshold) / len;
        }
        x *= len;
        y *= len;

        //bound diagonals to an octagonal range {-68 ... +68}
        if (x != 0.0 && y != 0.0) {
            auto slope = y / x;
            auto edgex = copysign(85.0 / (fabs(slope) + 16.0 / 69.0), x);
            auto edgey = copysign(std::min(fabs(edgex * slope), 85.0 / (1.0 / fabs(slope) + 16.0 / 69.0)), y);
            edgex = edgey / slope;

            auto scale = sqrt(edgex * edgex + edgey * edgey) / 85.0;
            x *= scale;
            y *= scale;
        }

        if (isRightStick) {
            getRightStickX(virtualSlot) = x;
            getRightStickY(virtualSlot) = y;
        } else {
            getLeftStickX(virtualSlot) = x;
            getLeftStickY(virtualSlot) = y;
        }
    }

    void WiiUGamepad::CreateDefaultBinding(int32_t virtualSlot) {
        auto profile = getProfile(virtualSlot);
        profile->Mappings.clear();

        profile->Version = DEVICE_PROFILE_CURRENT_VERSION;
        profile->UseRumble = true;
        profile->RumbleStrength = 1.0f;
        profile->UseGyro = false;

        profile->Mappings[VPAD_STICK_R_EMULATION_RIGHT] = BTN_CRIGHT;
        profile->Mappings[VPAD_STICK_R_EMULATION_LEFT] = BTN_CLEFT;
        profile->Mappings[VPAD_STICK_R_EMULATION_DOWN] = BTN_CDOWN;
        profile->Mappings[VPAD_STICK_R_EMULATION_UP] = BTN_CUP;
        profile->Mappings[VPAD_BUTTON_ZR] = BTN_R;
        profile->Mappings[VPAD_BUTTON_L] = BTN_L;
        profile->Mappings[VPAD_BUTTON_RIGHT] = BTN_DRIGHT;
        profile->Mappings[VPAD_BUTTON_LEFT] = BTN_DLEFT;
        profile->Mappings[VPAD_BUTTON_DOWN] = BTN_DDOWN;
        profile->Mappings[VPAD_BUTTON_UP] = BTN_DUP;
        profile->Mappings[VPAD_BUTTON_PLUS] = BTN_START;
        profile->Mappings[VPAD_BUTTON_ZL] = BTN_Z;
        profile->Mappings[VPAD_BUTTON_B] = BTN_B;
        profile->Mappings[VPAD_BUTTON_A] = BTN_A;
        profile->Mappings[VPAD_STICK_L_EMULATION_RIGHT] = BTN_STICKRIGHT;
        profile->Mappings[VPAD_STICK_L_EMULATION_LEFT] = BTN_STICKLEFT;
        profile->Mappings[VPAD_STICK_L_EMULATION_DOWN] = BTN_STICKDOWN;
        profile->Mappings[VPAD_STICK_L_EMULATION_UP] = BTN_STICKUP;

        for (int i = 0; i < 4; i++) {
            profile->AxisDeadzones[i] = 0.0f;
            profile->AxisMinimumPress[i] = 7680.0f;
        }

        profile->GyroData[GYRO_SENSITIVITY] = 1.0f;
        profile->GyroData[DRIFT_X] = 0.0f;
        profile->GyroData[DRIFT_Y] = 0.0f;
    }
}
#endif
