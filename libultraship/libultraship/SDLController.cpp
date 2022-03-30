#include "SDLController.h"

#include "GameSettings.h"
#include "GlobalCtx2.h"
#include "spdlog/spdlog.h"
#include "stox.h"
#include "Window.h"

extern "C" uint8_t __osMaxControllers;

namespace Ship {

	SDLController::SDLController(int32_t dwControllerNumber) : Controller(dwControllerNumber), guid(INVALID_SDL_CONTROLLER_GUID) {

	}

	SDLController::~SDLController() {
	}

    bool SDLController::IsGuidInUse(const std::string& guid) {
        // Check if the GUID is loaded in any other controller;
        for (size_t i = 0; i < __osMaxControllers; i++) {
            for (size_t j = 0; j < Window::Controllers[i].size(); j++) {
	            SDLController* OtherCont = dynamic_cast<SDLController*>(Window::Controllers[i][j].get());

	            if (OtherCont != nullptr && OtherCont->GetGuid().compare(guid) == 0) {
	                return true;
	            }
			}
        }

        return false;
    }

    void SDLController::LoadAxisThresholds() {
        std::string ConfSection = GetBindingConfSection();
        std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
        ConfigFile& Conf = *pConf.get();

        ThresholdMapping[SDL_CONTROLLER_AXIS_LEFTX] = Ship::stoi(Conf[ConfSection][STR(SDL_CONTROLLER_AXIS_LEFTX) + "_threshold"]);
        ThresholdMapping[SDL_CONTROLLER_AXIS_LEFTY] = Ship::stoi(Conf[ConfSection][STR(SDL_CONTROLLER_AXIS_LEFTY) + "_threshold"]);
        ThresholdMapping[SDL_CONTROLLER_AXIS_RIGHTX] = Ship::stoi(Conf[ConfSection][STR(SDL_CONTROLLER_AXIS_RIGHTX) + "_threshold"]);
        ThresholdMapping[SDL_CONTROLLER_AXIS_RIGHTY] = Ship::stoi(Conf[ConfSection][STR(SDL_CONTROLLER_AXIS_RIGHTY) + "_threshold"]);
        ThresholdMapping[SDL_CONTROLLER_AXIS_TRIGGERLEFT] = Ship::stoi(Conf[ConfSection][STR(SDL_CONTROLLER_AXIS_TRIGGERLEFT) + "_threshold"]);
        ThresholdMapping[SDL_CONTROLLER_AXIS_TRIGGERRIGHT] = Ship::stoi(Conf[ConfSection][STR(SDL_CONTROLLER_AXIS_TRIGGERRIGHT) + "_threshold"]);
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
            auto slope = ay / ax;
            auto edgex = copysign(85.0 / (abs(slope) + 16.0 / 69.0), ax);
            auto edgey = copysign(std::min(abs(edgex * slope), 85.0 / (1.0 / abs(slope) + 16.0 / 69.0)), ay);
            edgex = edgey / slope;

            auto scale = sqrt(edgex * edgex + edgey * edgey) / 85.0;
            ax *= scale;
            ay *= scale;
        }

        wStickX = +ax;
        wStickY = -ay;
    }

    void SDLController::SetButtonMapping(const std::string& szButtonName, int32_t dwScancode) {
        if (guid.compare(INVALID_SDL_CONTROLLER_GUID)) {
            return;
        }

        Controller::SetButtonMapping(szButtonName, dwScancode);
    }

	std::string SDLController::GetControllerType() {
		return "Base";
	}

    std::string SDLController::GetConfSection() {
        return GetControllerType() + " CONTROLLER " + std::to_string(GetControllerNumber() + 1);
    }

    std::string SDLController::GetBindingConfSection() {
        return GetControllerType() + " CONTROLLER BINDING " + guid;
    }
}
