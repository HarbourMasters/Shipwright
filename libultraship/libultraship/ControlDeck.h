#pragma once

#include "Controller.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace Ship {

	struct DeviceEntry {
		const char* Name;
		std::string Guid;
		std::shared_ptr<Controller> Backend;
		DeviceProfile Mappings;
	};

	class ControlDeck {
	public:
		std::vector<int> virtualDevices;
		std::vector<std::shared_ptr<DeviceEntry>>  physicalDevices = {};
		void Init(uint8_t* controllerBits);
		void ScanPhysicalDevices();
		void WriteToPad(OSContPad* pad) const;
		void SetPhysicalDevice(int slot, int deviceSlot);
		void BindPhysicalDevice(const std::shared_ptr<DeviceEntry>& entry);
	};
}
