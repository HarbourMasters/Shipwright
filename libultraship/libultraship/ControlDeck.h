#pragma once

#include "Controller.h"
#include <vector>
#include <string>

namespace Ship {

	class ControlDeck {
	public:
		std::vector<int> virtualDevices;
		std::vector<std::shared_ptr<Controller>> physicalDevices = {};
		void Init(uint8_t* controllerBits);
		void ScanPhysicalDevices();
		void WriteToPad(OSContPad* pad) const;
		void LoadControllerSettings();
		void SaveControllerSettings();
		void SetPhysicalDevice(int slot, int deviceSlot);
	};
}
