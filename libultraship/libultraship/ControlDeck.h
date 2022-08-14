#pragma once

#include "Controller.h"
#include <vector>

namespace Ship {

	class ControlDeck {
	public:
		void Init(uint8_t* controllerBits);
		void ScanPhysicalDevices();
		void WriteToPad(OSContPad* pad) const;
		void LoadControllerSettings();
		void SaveControllerSettings();
		void SetPhysicalDevice(int slot, int deviceSlot);
		std::shared_ptr<Ship::Controller> GetPhysicalDevice(int deviceSlot);
		std::shared_ptr<Ship::Controller> GetPhysicalDeviceFromVirtualSlot(int slot);
		size_t GetNumPhysicalDevices();
		int GetVirtualDevice(int slot);
		size_t GetNumVirtualDevices();
		uint8_t* GetControllerBits();
	private:
		std::vector<int> virtualDevices = {};
		std::vector<std::shared_ptr<Controller>> physicalDevices = {};
		uint8_t* controllerBits = nullptr;
	};
}
