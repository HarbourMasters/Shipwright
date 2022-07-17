#include "ControlDeck.h"

#include "Window.h"
#include "Controller.h"
#include "DisconnectedController.h"
#include "KeyboardController.h"
#include "SDLController.h"
#include <Utils/StringHelper.h>

uint8_t* controllerBits;

void Ship::ControlDeck::Init(uint8_t* bits) {
	ScanPhysicalDevices();
	controllerBits = bits;
}

void Ship::ControlDeck::ScanPhysicalDevices() {

	virtualDevices.clear();
	physicalDevices.clear();

	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			auto sdl = std::make_shared<SDLController>(i);
			sdl->Open();
			physicalDevices.push_back(sdl);
		}
	}

	physicalDevices.push_back(std::make_shared<KeyboardController>());
	physicalDevices.push_back(std::make_shared<DisconnectedController>());

	for (const auto& device : physicalDevices) {
		for (int i = 0; i < MAXCONTROLLERS; i++) {
			device->CreateDefaultBinding(i);
		}
	}

	for (int i = 0; i < MAXCONTROLLERS; i++) {
		virtualDevices.push_back(i == 0 ? 0 : static_cast<int>(physicalDevices.size()) - 1);
	}

	LoadControllerSettings();
}

void Ship::ControlDeck::SetPhysicalDevice(int slot, int deviceSlot) {
	const std::shared_ptr<Controller> backend = physicalDevices[deviceSlot];
	virtualDevices[slot] = deviceSlot;
	*controllerBits |= (backend->Connected()) << slot;
}

void Ship::ControlDeck::WriteToPad(OSContPad* pad) const {
	for (size_t i = 0; i < virtualDevices.size(); i++) {
		physicalDevices[virtualDevices[i]]->Read(&pad[i], i);
	}
}

#define NESTED(key, ...) StringHelper::Sprintf("Controllers.%s.Slot_%d." key, device->GetGuid().c_str(), slot, __VA_ARGS__)

void Ship::ControlDeck::LoadControllerSettings() {
	std::shared_ptr<Mercury> Config = GlobalCtx2::GetInstance()->GetConfig();

	for (auto const& val : Config->rjson["Controllers"]["Deck"].items()) {
		int slot = std::stoi(val.key().substr(5));

		for (size_t dev = 0; dev < physicalDevices.size(); dev++) {
			std::string guid = physicalDevices[dev]->GetGuid();
			if(guid != val.value()) continue;

			virtualDevices[slot] = dev;
		}
	}

	for (size_t i = 0; i < virtualDevices.size(); i++) {
		std::shared_ptr<Controller> backend = physicalDevices[virtualDevices[i]];
		Config->setString(StringHelper::Sprintf("Controllers.Deck.Slot_%d", (int)i), backend->GetGuid());
	}

	for (const auto& device : physicalDevices) {

		std::string guid = device->GetGuid();

		for (int slot = 0; slot < MAXCONTROLLERS; slot++) {

			if (!(Config->rjson["Controllers"].contains(guid) && Config->rjson["Controllers"][guid].contains(StringHelper::Sprintf("Slot_%d", slot)))) continue;

			auto& profile = device->profiles[slot];
			auto  rawProfile = Config->rjson["Controllers"][guid][StringHelper::Sprintf("Slot_%d", slot)];

			profile.Mappings.clear();
			profile.Thresholds.clear();
			profile.UseRumble = Config->getBool(NESTED("Rumble.Enabled", ""));
			profile.RumbleStrength = Config->getFloat(NESTED("Rumble.Strength", ""));
			profile.UseGyro = Config->getBool(NESTED("Gyro.Enabled", ""));

			for (auto const& val : rawProfile["Thresholds"].items()) {
				profile.Thresholds[static_cast<ControllerThresholds>(std::stoi(val.key()))] = val.value();
			}

			for (auto const& val : rawProfile["Mappings"].items()) {
				device->SetButtonMapping(slot, std::stoi(val.key().substr(4)), val.value());
			}
		}
	}
}

void Ship::ControlDeck::SaveControllerSettings() {
	std::shared_ptr<Mercury> Config = GlobalCtx2::GetInstance()->GetConfig();

	for (size_t i = 0; i < virtualDevices.size(); i++) {
		std::shared_ptr<Controller> backend = physicalDevices[virtualDevices[i]];
		Config->setString(StringHelper::Sprintf("Controllers.Deck.Slot_%d", (int)i), backend->GetGuid());
	}

	for (const auto& device : physicalDevices) {

		int slot = 0;
		std::string guid = device->GetGuid();

		for (const auto& profile : device->profiles) {

			if (!device->Connected()) continue;

			auto  rawProfile = Config->rjson["Controllers"][guid][StringHelper::Sprintf("Slot_%d", slot)];
			Config->setBool(NESTED("Rumble.Enabled", ""), profile.UseRumble);
			Config->setFloat(NESTED("Rumble.Strength", ""), profile.RumbleStrength);
			Config->setBool(NESTED("Gyro.Enabled", ""), profile.UseGyro);

			for (auto const& val : rawProfile["Mappings"].items()) {
				Config->setInt(NESTED("Mappings.%s", val.key().c_str()), -1);
			}

			for (auto const& [key, val] : profile.Thresholds) {
				Config->setFloat(NESTED("Thresholds.%d", key), val);
			}

			for (auto const& [key, val] : profile.Mappings) {
				Config->setInt(NESTED("Mappings.BTN_%d", val), key);
			}

			slot++;
		}
	}

	Config->save();
}
