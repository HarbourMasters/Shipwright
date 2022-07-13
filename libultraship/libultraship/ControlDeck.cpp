#include "ControlDeck.h"

#include "Window.h"
#include "Controller.h"
#include "DummyController.h"
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
	physicalDevices.push_back(std::make_shared<DummyController>());

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

const char* GetN64ButtonName(int32_t btn) {
	switch(btn) {
	case BTN_CRIGHT:
		return "BTN_CRIGHT";
	case BTN_CLEFT:
		return "BTN_CLEFT";
	case BTN_CDOWN:
		return "BTN_CDOWN";
	case BTN_CUP:
		return "BTN_CUP";
	case BTN_R:
		return "BTN_R";
	case BTN_L:
		return "BTN_L";
	case BTN_DRIGHT:
		return "BTN_DRIGHT";
	case BTN_DLEFT:
		return "BTN_DLEFT";
	case BTN_DDOWN:
		return "BTN_DDOWN";
	case BTN_DUP:
		return "BTN_DUP";
	case BTN_START:
		return "BTN_START";
	case BTN_Z:
		return "BTN_Z";
	case BTN_B:
		return "BTN_B";
	case BTN_A:
		return "BTN_A";
	case BTN_STICKLEFT:
		return "BTN_STICKLEFT";
	case BTN_STICKRIGHT:
		return "BTN_STICKRIGHT";
	case BTN_STICKDOWN:
		return "BTN_STICKDOWN";
	case BTN_STICKUP:
		return "BTN_STICKUP";
	case BTN_VSTICKLEFT:
		return "BTN_VSTICKLEFT";
	case BTN_VSTICKRIGHT:
		return "BTN_VSTICKRIGHT";
	case BTN_VSTICKDOWN:
		return "BTN_VSTICKDOWN";
	case BTN_VSTICKUP:
		return "BTN_VSTICKUP";
	}

	return "NONE";
}

int32_t GetN64ButtonFromName(std::string Name) {
	if(Name == "BTN_CRIGHT")
		return BTN_CRIGHT;
	if(Name == "BTN_CLEFT")
		return BTN_CLEFT;
	if(Name == "BTN_CDOWN")
		return BTN_CDOWN;
	if(Name == "BTN_CUP")
		return BTN_CUP;
	if(Name == "BTN_R")
		return BTN_R;
	if(Name == "BTN_L")
		return BTN_L;
	if(Name == "BTN_DRIGHT")
		return BTN_DRIGHT;
	if(Name == "BTN_DLEFT")
		return BTN_DLEFT;
	if(Name == "BTN_DDOWN")
		return BTN_DDOWN;
	if(Name == "BTN_DUP")
		return BTN_DUP;
	if(Name == "BTN_START")
		return BTN_START;
	if(Name == "BTN_Z")
		return BTN_Z;
	if(Name == "BTN_B")
		return BTN_B;
	if(Name == "BTN_A")
		return BTN_A;
	if(Name == "BTN_STICKLEFT")
		return BTN_STICKLEFT;
	if(Name == "BTN_STICKRIGHT")
		return BTN_STICKRIGHT;
	if(Name == "BTN_STICKDOWN")
		return BTN_STICKDOWN;
	if(Name == "BTN_STICKUP")
		return BTN_STICKUP;
	if(Name == "BTN_VSTICKLEFT")
		return BTN_VSTICKLEFT;
	if(Name == "BTN_VSTICKRIGHT")
		return BTN_VSTICKRIGHT;
	if(Name == "BTN_VSTICKDOWN")
		return BTN_VSTICKDOWN;
	if(Name == "BTN_VSTICKUP")
		return BTN_VSTICKUP;
	return -1;
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
			profile.GyroThresholds.clear();
			profile.UseRumble = Config->getBool(NESTED("Rumble.Enabled", ""));
			profile.RumbleStrength = Config->getBool(NESTED("Rumble.Strength", ""));
			profile.UseGyro = Config->getBool(NESTED("Gyro.Enabled", ""));

			for (auto const& val : rawProfile["Gyro"]["Thresholds"].items()) {
				profile.GyroThresholds[std::stoi(val.key())] = val.value();
			}

			for (auto const& val : rawProfile["Thresholds"].items()) {
				profile.Thresholds[static_cast<ControllerThresholds>(std::stoi(val.key()))] = val.value();
			}

			for (auto const& val : rawProfile["Mappings"].items()) {
				device->SetButtonMapping(slot, GetN64ButtonFromName(val.key()), val.value());
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

			for (auto const& [key, val] : profile.GyroThresholds) {
				Config->setInt(NESTED("Gyro.Thresholds.%d", key), val);
			}

			for (auto const& [key, val] : profile.Thresholds) {
				Config->setInt(NESTED("Thresholds.%d", key), val);
			}

			for (auto const& [key, val] : profile.Mappings) {
				Config->setInt(NESTED("Mappings.%s", GetN64ButtonName(val)), key);
			}

			slot++;
		}
	}

	Config->save();
}