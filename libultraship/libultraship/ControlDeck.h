#pragma once

#include "Controller.h"
#include <vector>
#include "Lib/Mercury/Mercury.h"

namespace Ship {

	class ControlDeck {
	public:
		void Init(uint8_t* controllerBits);
		void ScanPhysicalDevices();
		void WriteToPad(OSContPad* pad) const;
		void LoadControllerSettings();
		void SaveControllerSettings();
		void SetPhysicalDevice(int32_t slot, int32_t deviceSlot);
		std::shared_ptr<Controller> GetPhysicalDevice(int32_t deviceSlot);
		std::shared_ptr<Controller> GetPhysicalDeviceFromVirtualSlot(int32_t slot);
		size_t GetNumPhysicalDevices();
		int32_t GetVirtualDevice(int32_t slot);
		size_t GetNumVirtualDevices();
		uint8_t* GetControllerBits();
	private:
		std::vector<int> virtualDevices = {};
		std::vector<std::shared_ptr<Controller>> physicalDevices = {};
		uint8_t* controllerBits = nullptr;
	};
}
