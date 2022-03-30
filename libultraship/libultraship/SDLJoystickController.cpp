#include "SDLJoystickController.h"

#include "GameSettings.h"
#include "GlobalCtx2.h"
#include "spdlog/spdlog.h"
#include "stox.h"
#include "Window.h"

extern "C" uint8_t __osMaxControllers;
float gyroDriftX;
float gyroDriftY;

namespace Ship {


	SDLJoystickController::SDLJoystickController(int32_t dwControllerNumber) : SDLController(dwControllerNumber), Cont(nullptr), numButtons(0) {

	}

	SDLJoystickController::~SDLJoystickController() {
        Close();
	}

    bool SDLJoystickController::Open() {
        std::string ConfSection = GetConfSection();
        std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
        ConfigFile& Conf = *pConf.get();

        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            // This is for HID devices. So don't use the GameController API
            if (!SDL_IsGameController(i)) {
                // Get the GUID from SDL
                char GuidBuf[33];
                SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(i), GuidBuf, sizeof(GuidBuf));
                auto NewGuid = std::string(GuidBuf);

                // Invalid GUID read. Go to next.
                if (NewGuid.compare(INVALID_SDL_CONTROLLER_GUID) == 0) {
                    SPDLOG_ERROR("SDL Controller returned invalid guid");
                    continue;
                }

                // The GUID is in use, we want to use a different physical controller. Go to next.
                if (IsGuidInUse(NewGuid)) {
                    continue;
                }

                // If the GUID is blank from the config, OR if the config GUID matches, load the controller.
                if (Conf[ConfSection]["GUID"].compare("") == 0 || Conf[ConfSection]["GUID"].compare(INVALID_SDL_CONTROLLER_GUID) == 0 || Conf[ConfSection]["GUID"].compare(NewGuid) == 0) {
                    auto NewCont = SDL_JoystickOpen(i);

                    // We failed to load the controller. Go to next.
                    if (NewCont == nullptr) {
                        SPDLOG_ERROR("SDL Controller failed to open: ({})", SDL_GetError());
                        continue;
                    }

                    guid = NewGuid;
                    Cont = NewCont;
                    numButtons = SDL_JoystickNumButtons(Cont);

                    std::string BindingConfSection = GetBindingConfSection();
                    std::shared_ptr<ConfigFile> pBindingConf = GlobalCtx2::GetInstance()->GetConfig();
                    ConfigFile& BindingConf = *pBindingConf.get();

                    if (!BindingConf.has(BindingConfSection)) {
                        CreateDefaultBinding();
                    }

                    LoadBinding();

                    break;
                }
            }
        }

        return Cont != nullptr;
    }

    bool SDLJoystickController::Close() {
        if (Cont != nullptr) {
            SDL_JoystickClose(Cont);
        }
        Cont = nullptr;
        guid = "";
        ButtonMapping.clear();
        dwPressedButtons = 0;
        wStickX = 0;
        wStickY = 0;

        return true;
    }

    void SDLJoystickController::ReadFromSource() {
        std::string ConfSection = GetBindingConfSection();
        std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
        ConfigFile& Conf = *pConf.get();

        SDL_GameControllerUpdate();

        // If the controller is disconnected, close it.
        if (Cont != nullptr && !SDL_JoystickGetAttached(Cont)) {
            Close();
        }

        // Attempt to load the controller if it's not loaded
        if (Cont == nullptr) {
            // If we failed to load the controller, don't process it.
            if (!Open()) {
                return;
            }
        }

        for (int32_t i = 0; i < numButtons; i++) {
            if (ButtonMapping.contains(i)) {
                if (SDL_JoystickGetButton(Cont, i)) {
                    dwPressedButtons |= ButtonMapping[i];
                }
                else {
                    dwPressedButtons &= ~ButtonMapping[i];
                }
            }
        }

        int StickAxisX = SDL_CONTROLLER_AXIS_INVALID;
        int StickAxisY = SDL_CONTROLLER_AXIS_INVALID;
        int32_t StickDeadzone = 0;

        for (int32_t i = SDL_CONTROLLER_AXIS_LEFTX; i < SDL_CONTROLLER_AXIS_MAX; i++) {
            auto Axis = i;
            auto PosScancode = i + AXIS_SCANCODE_BIT;
            auto NegScancode = -PosScancode;
            auto AxisThreshold = ThresholdMapping[i];
            auto PosButton = ButtonMapping[PosScancode];
            auto NegButton = ButtonMapping[NegScancode];
            auto AxisValue = SDL_JoystickGetAxis(Cont, Axis);


            // If the axis is NOT mapped to the control stick.
            if (!(
                PosButton == BTN_STICKLEFT || PosButton == BTN_STICKRIGHT ||
                PosButton == BTN_STICKUP || PosButton == BTN_STICKDOWN ||
                NegButton == BTN_STICKLEFT || NegButton == BTN_STICKRIGHT ||
                NegButton == BTN_STICKUP || NegButton == BTN_STICKDOWN)) {
                if (AxisValue > AxisThreshold) {
                    dwPressedButtons |= PosButton;
                    dwPressedButtons &= ~NegButton;
                }
                else if (AxisValue < -AxisThreshold) {
                    dwPressedButtons &= ~PosButton;
                    dwPressedButtons |= NegButton;
                }
                else {
                    dwPressedButtons &= ~PosButton;
                    dwPressedButtons &= ~NegButton;
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
                auto AxisValueX = SDL_JoystickGetAxis(Cont, StickAxisX);
                auto AxisValueY = SDL_JoystickGetAxis(Cont, StickAxisY);
                NormalizeStickAxis(AxisValueX, AxisValueY, StickDeadzone);
            }
        }
    }

    void SDLJoystickController::WriteToSource(ControllerCallback* controller)
    {
        if (SDL_JoystickHasRumble(Cont)) {
            if (controller->rumble > 0) {
                SDL_JoystickRumble(Cont, 0xFFFF * Game::Settings.controller.rumble_strength, 0xFFFF * Game::Settings.controller.rumble_strength, 1);
            }
        }

        if (SDL_JoystickHasLED(Cont)) {
            if (controller->ledColor == 1) {
                SDL_JoystickSetLED(Cont, 255, 0, 0);
            }
            else {
                SDL_JoystickSetLED(Cont, 0, 255, 0);
            }
        }
    }

    void SDLJoystickController::CreateDefaultBinding() {
        std::string ConfSection = GetBindingConfSection();
        std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
        ConfigFile& Conf = *pConf.get();

        Conf[ConfSection][STR(BTN_CRIGHT)] = std::to_string(9);
        Conf[ConfSection][STR(BTN_CLEFT)] = std::to_string(8);
        Conf[ConfSection][STR(BTN_CDOWN)] = std::to_string(7);
        Conf[ConfSection][STR(BTN_CUP)] = std::to_string(6);
        Conf[ConfSection][STR(BTN_R)] = std::to_string(5);
        Conf[ConfSection][STR(BTN_L)] = std::to_string(4);
        Conf[ConfSection][STR(BTN_DRIGHT)] = std::to_string(13);
        Conf[ConfSection][STR(BTN_DLEFT)] = std::to_string(12);
        Conf[ConfSection][STR(BTN_DDOWN)] = std::to_string(11);
        Conf[ConfSection][STR(BTN_DUP)] = std::to_string(10);
        Conf[ConfSection][STR(BTN_START)] = std::to_string(3);
        Conf[ConfSection][STR(BTN_Z)] = std::to_string(2);
        Conf[ConfSection][STR(BTN_B)] = std::to_string(1);
        Conf[ConfSection][STR(BTN_A)] = std::to_string(0);
        Conf[ConfSection][STR(BTN_STICKRIGHT)] = std::to_string((SDL_CONTROLLER_AXIS_LEFTX + AXIS_SCANCODE_BIT));
        Conf[ConfSection][STR(BTN_STICKLEFT)] = std::to_string(-(SDL_CONTROLLER_AXIS_LEFTX + AXIS_SCANCODE_BIT));
        Conf[ConfSection][STR(BTN_STICKDOWN)] = std::to_string((SDL_CONTROLLER_AXIS_LEFTY + AXIS_SCANCODE_BIT));
        Conf[ConfSection][STR(BTN_STICKUP)] = std::to_string(-(SDL_CONTROLLER_AXIS_LEFTY + AXIS_SCANCODE_BIT));

        Conf[ConfSection][STR(SDL_CONTROLLER_AXIS_LEFTX) + "_threshold"] = std::to_string(16.0);
        Conf[ConfSection][STR(SDL_CONTROLLER_AXIS_LEFTY) + "_threshold"] = std::to_string(16.0);
        Conf[ConfSection][STR(SDL_CONTROLLER_AXIS_RIGHTX) + "_threshold"] = std::to_string(0x4000);
        Conf[ConfSection][STR(SDL_CONTROLLER_AXIS_RIGHTY) + "_threshold"] = std::to_string(0x4000);
        Conf[ConfSection][STR(SDL_CONTROLLER_AXIS_TRIGGERLEFT) + "_threshold"] = std::to_string(0x1E00);
        Conf[ConfSection][STR(SDL_CONTROLLER_AXIS_TRIGGERRIGHT) + "_threshold"] = std::to_string(0x1E00);

        Conf.Save();
    }

	std::string SDLJoystickController::GetControllerType() {
		return "Joystick";
	}
}