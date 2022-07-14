#pragma once

#include <map>
#include <memory>
#include <string>
#include "stdint.h"
#include "UltraController.h"
#include "ControllerAttachment.h"
#include <vector>
#include <unordered_map>

#define EXTENDED_SCANCODE_BIT (1 << 8)
#define AXIS_SCANCODE_BIT (1 << 9)

namespace Ship {

	enum ControllerThresholds {
		LEFT_STICK = 1,
		RIGHT_STICK = 2,
		LEFT_TRIGGER = 3,
		RIGHT_TRIGGER = 4,
		DRIFT_X = 5,
		DRIFT_Y = 6,
		SENSITIVITY = 7,
		GYRO_SENSITIVITY = 8
	};

	struct DeviceProfile {
		bool UseRumble = false;
		bool UseGyro = false;
		float RumbleStrength = 1.0f;
		std::unordered_map<ControllerThresholds, float> Thresholds;
		std::map<int32_t, int32_t> Mappings;
	};

	class Controller {
		public:
			virtual ~Controller() = default;
			Controller();
			void Read(OSContPad* pad, int32_t slot);
			virtual void ReadFromSource(int32_t slot) = 0;
			virtual void WriteToSource(int32_t slot, ControllerCallback* controller) = 0;
			virtual bool Connected() const = 0;
			virtual bool CanRumble() const = 0;
			virtual bool CanGyro() const = 0;
			virtual void CreateDefaultBinding(int32_t slot) = 0;
			bool isRumbling;
			std::vector<DeviceProfile> profiles;

			virtual void ClearRawPress() = 0;
			virtual int32_t ReadRawPress() = 0;
			void SetButtonMapping(int slot, int32_t n64Button, int32_t dwScancode);
			std::shared_ptr<ControllerAttachment> GetAttachment() { return Attachment; }

			std::string GetGuid() { return GUID; }
			virtual const char* GetButtonName(int slot, int n64Button) = 0;
			virtual const char* GetControllerName() = 0;

			int8_t wStickX;
			int8_t wStickY;
			float wGyroX;
			float wGyroY;
			float wCamX;
			float wCamY;

		protected:
			std::vector<int32_t> dwPressedButtons;
			std::string GUID;

			void LoadBinding();

		private:
			std::shared_ptr<ControllerAttachment> Attachment;
	};

	struct ControllerEntry {
		uint8_t* controllerBits;
		Controller* entryIO;
	};

}
