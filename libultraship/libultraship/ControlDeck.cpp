#include "ControlDeck.h"

#include "Window.h"
#include "Controller.h"
#include "DummyController.h"
#include <Utils/StringHelper.h>
#include "Cvar.h"

#ifndef __WIIU__
#include "KeyboardController.h"
#include "SDLController.h"
#else
#include "WiiUGamepad.h"
#include "WiiUController.h"
#endif

namespace Ship {

    void ControlDeck::Init(uint8_t* bits) {
        ScanPhysicalDevices();
        controllerBits = bits;
    }

    void ControlDeck::ScanPhysicalDevices() {

        virtualDevices.clear();
        physicalDevices.clear();

#ifndef __WIIU__
        for (int32_t i = 0; i < SDL_NumJoysticks(); i++) {
            if (SDL_IsGameController(i)) {
                auto sdl = std::make_shared<SDLController>(i);
                sdl->Open();
                physicalDevices.push_back(sdl);
            }
        }

        physicalDevices.push_back(std::make_shared<DummyController>("Auto", "Auto", true));
        physicalDevices.push_back(std::make_shared<KeyboardController>());
#else
        physicalDevices.push_back(std::make_shared<DummyController>("Auto", "Auto", true));

        auto gamepad = std::make_shared<Ship::WiiUGamepad>();
        gamepad->Open();
        physicalDevices.push_back(gamepad);

        for (int32_t i = 0; i < 4; i++) {
            auto controller = std::make_shared<Ship::WiiUController>((WPADChan) i);
            controller->Open();
            physicalDevices.push_back(controller);
        }
#endif

        physicalDevices.push_back(std::make_shared<DummyController>("Disconnected", "None", false));

        for (const auto device : physicalDevices) {
            for (int32_t i = 0; i < MAXCONTROLLERS; i++) {
                device->CreateDefaultBinding(i);
            }
        }

        for (int32_t i = 0; i < MAXCONTROLLERS; i++) {
            virtualDevices.push_back(i == 0 ? 0 : static_cast<int>(physicalDevices.size()) - 1);
        }

        LoadControllerSettings();
    }

    void ControlDeck::SetPhysicalDevice(int32_t slot, int32_t deviceSlot) {
        const std::shared_ptr<Controller> backend = physicalDevices[deviceSlot];
        virtualDevices[slot] = deviceSlot;
        *controllerBits |= (backend->Connected()) << slot;
    }

	void ControlDeck::WriteToPad(OSContPad* pad) const {
		bool shouldBlockGameInput = CVar_GetS32("gOpenMenuBar", 0) && CVar_GetS32("gControlNav", 0);

		for (size_t i = 0; i < virtualDevices.size(); i++) {
			const std::shared_ptr<Controller> backend = physicalDevices[virtualDevices[i]];
			if (backend->GetGuid() == "Auto") {
				for (const auto& device : physicalDevices) {
					if(shouldBlockGameInput && device->GetGuid() != "Keyboard") {
						continue;
					}
					device->Read(&pad[i], i);
				}
				continue;
			}
			if(shouldBlockGameInput && backend->GetGuid() != "Keyboard") {
				continue;
			}
			backend->Read(&pad[i], i);
		}
	}

#define NESTED(key, ...) StringHelper::Sprintf("Controllers.%s.Slot_%d." key, device->GetGuid().c_str(), virtualSlot, __VA_ARGS__)

    void ControlDeck::LoadControllerSettings() {
        std::shared_ptr<Mercury> Config = Window::GetInstance()->GetConfig();

        for (auto const& val : Config->rjson["Controllers"]["Deck"].items()) {
            int32_t slot = std::stoi(val.key().substr(5));

            for (size_t dev = 0; dev < physicalDevices.size(); dev++) {
                std::string guid = physicalDevices[dev]->GetGuid();
                if (guid != val.value()) continue;

                virtualDevices[slot] = dev;
            }
        }

        for (size_t i = 0; i < virtualDevices.size(); i++) {
            std::shared_ptr<Controller> backend = physicalDevices[virtualDevices[i]];
            Config->setString(StringHelper::Sprintf("Controllers.Deck.Slot_%d", (int)i), backend->GetGuid());
        }

        for (const auto device : physicalDevices) {

            std::string guid = device->GetGuid();

            for (int32_t virtualSlot = 0; virtualSlot < MAXCONTROLLERS; virtualSlot++) {

                if (!(Config->rjson["Controllers"].contains(guid) && Config->rjson["Controllers"][guid].contains(StringHelper::Sprintf("Slot_%d", virtualSlot)))) continue;

                auto profile = device->getProfile(virtualSlot);
                auto rawProfile = Config->rjson["Controllers"][guid][StringHelper::Sprintf("Slot_%d", virtualSlot)];

                profile->Mappings.clear();
                profile->AxisDeadzones.clear();
                profile->AxisDeadzones.clear();
                profile->GyroData.clear();

                profile->Version = Config->getInt(NESTED("Version", ""), DEVICE_PROFILE_VERSION_V0);

                switch (profile->Version) {

                case DEVICE_PROFILE_VERSION_V0:

                    // Load up defaults for the things we can't load.
                    device->CreateDefaultBinding(virtualSlot);

                    profile->UseRumble = Config->getBool(NESTED("Rumble.Enabled", ""));
                    profile->RumbleStrength = Config->getFloat(NESTED("Rumble.Strength", ""));
                    profile->UseGyro = Config->getBool(NESTED("Gyro.Enabled", ""));

                    for (auto const& val : rawProfile["Mappings"].items()) {
                        device->SetButtonMapping(virtualSlot, std::stoi(val.key().substr(4)), val.value());
                    }

                    break;

                case DEVICE_PROFILE_VERSION_V1:
                    profile->UseRumble = Config->getBool(NESTED("Rumble.Enabled", ""));
                    profile->RumbleStrength = Config->getFloat(NESTED("Rumble.Strength", ""));
                    profile->UseGyro = Config->getBool(NESTED("Gyro.Enabled", ""));

                    for (auto const& val : rawProfile["AxisDeadzones"].items()) {
                        profile->AxisDeadzones[std::stoi(val.key())] = val.value();
                    }

                    for (auto const& val : rawProfile["AxisMinimumPress"].items()) {
                        profile->AxisMinimumPress[std::stoi(val.key())] = val.value();
                    }

                    for (auto const& val : rawProfile["GyroData"].items()) {
                        profile->GyroData[std::stoi(val.key())] = val.value();
                    }

                    for (auto const& val : rawProfile["Mappings"].items()) {
                        device->SetButtonMapping(virtualSlot, std::stoi(val.key().substr(4)), val.value());
                    }

                    break;

                // Version is invalid.
                default:
                    device->CreateDefaultBinding(virtualSlot);
                    break;
                }
            }
        }
    }

    void ControlDeck::SaveControllerSettings() {
        std::shared_ptr<Mercury> Config = Window::GetInstance()->GetConfig();

        for (size_t i = 0; i < virtualDevices.size(); i++) {
            std::shared_ptr<Controller> backend = physicalDevices[virtualDevices[i]];
            Config->setString(StringHelper::Sprintf("Controllers.Deck.Slot_%d", (int)i), backend->GetGuid());
        }

        for (const auto& device : physicalDevices) {

            int32_t virtualSlot = 0;
            std::string guid = device->GetGuid();

            for (int32_t virtualSlot = 0; virtualSlot < MAXCONTROLLERS; virtualSlot++) {
                auto profile = device->getProfile(virtualSlot);

                if (!device->Connected()) continue;

                auto  rawProfile = Config->rjson["Controllers"][guid][StringHelper::Sprintf("Slot_%d", virtualSlot)];
                Config->setInt(NESTED("Version", ""), profile->Version);
                Config->setBool(NESTED("Rumble.Enabled", ""), profile->UseRumble);
                Config->setFloat(NESTED("Rumble.Strength", ""), profile->RumbleStrength);
                Config->setBool(NESTED("Gyro.Enabled", ""), profile->UseGyro);

                for (auto const& val : rawProfile["Mappings"].items()) {
                    Config->setInt(NESTED("Mappings.%s", val.key().c_str()), -1);
                }

                for (auto const& [key, val] : profile->AxisDeadzones) {
                    Config->setFloat(NESTED("AxisDeadzones.%d", key), val);
                }

                for (auto const& [key, val] : profile->AxisMinimumPress) {
                    Config->setFloat(NESTED("AxisMinimumPress.%d", key), val);
                }

                for (auto const& [key, val] : profile->GyroData) {
                    Config->setFloat(NESTED("GyroData.%d", key), val);
                }

                for (auto const& [key, val] : profile->Mappings) {
                    Config->setInt(NESTED("Mappings.BTN_%d", val), key);
                }

                virtualSlot++;
            }
        }

        Config->save();
    }

    std::shared_ptr<Controller> ControlDeck::GetPhysicalDevice(int32_t deviceSlot) {
        return physicalDevices[deviceSlot];
    }

    size_t ControlDeck::GetNumPhysicalDevices() {
        return physicalDevices.size();
    }

    int32_t ControlDeck::GetVirtualDevice(int32_t slot) {
        return virtualDevices[slot];
    }

    size_t ControlDeck::GetNumVirtualDevices() {
        return virtualDevices.size();
    }

    std::shared_ptr<Controller> ControlDeck::GetPhysicalDeviceFromVirtualSlot(int32_t slot) {
        return GetPhysicalDevice(GetVirtualDevice(slot));
    }

    uint8_t* ControlDeck::GetControllerBits() {
        return controllerBits;
    }

}