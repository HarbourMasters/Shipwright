#include "ControlDeck.h"

#include "Window.h"
#include "Controller.h"
#include "DummyController.h"
#include "KeyboardController.h"
#include "SDLController.h"

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
			Window::ControllerApi->BindPhysicalDevice(std::make_shared<DeviceEntry>(
				SDL_GameControllerNameForIndex(i), "Unk", std::make_shared<SDLController>(0)
			));
		}
	}

	physicalDevices.push_back(std::make_shared<DeviceEntry>("Keyboard", "Keyboard", std::make_shared<KeyboardController>(0)));
	physicalDevices.push_back(std::make_shared<DeviceEntry>("Disconnected", "Unk", std::make_shared<DummyController>(0)));

	for (int i = 0; i < MAXCONTROLLERS; i++) {
		virtualDevices.push_back((int)physicalDevices.size() - 1);
	}
}

void Ship::ControlDeck::BindPhysicalDevice(const std::shared_ptr<DeviceEntry>& entry) {
	this->physicalDevices.push_back(entry);
}

void Ship::ControlDeck::SetPhysicalDevice(int slot, int deviceSlot) {
	int state = physicalDevices[deviceSlot]->Backend->Connected();
	virtualDevices[slot] = deviceSlot;
	*controllerBits |= state << slot;
}

void Ship::ControlDeck::WriteToPad(OSContPad* pad) const {
	for (size_t i = 0; i < virtualDevices.size(); i++) {
		physicalDevices[virtualDevices[i]]->Backend->Read(&pad[i]);
	}
}
