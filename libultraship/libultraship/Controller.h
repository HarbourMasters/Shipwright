#pragma once

#include <map>
#include <memory>
#include <string>
#include <cstdint>
#include "UltraController.h"
#include "ControllerAttachment.h"
#include <unordered_map>

#define EXTENDED_SCANCODE_BIT (1 << 8)
#define AXIS_SCANCODE_BIT (1 << 9)

namespace Ship {
	enum GyroData {
		DRIFT_X,
		DRIFT_Y,
		GYRO_SENSITIVITY
	};

	enum DeviceProfileVersion {
		DEVICE_PROFILE_VERSION_V0 = 0,
		DEVICE_PROFILE_VERSION_V1 = 1
	};

	#define DEVICE_PROFILE_CURRENT_VERSION DEVICE_PROFILE_VERSION_V1

	struct DeviceProfile {
		int32_t Version = 0;
		bool UseRumble = false;
		bool UseGyro = false;
		float RumbleStrength = 1.0f;
		std::unordered_map<int32_t, float> AxisDeadzones;
		std::unordered_map<int32_t, float> AxisMinimumPress;
		std::unordered_map<int32_t, float> GyroData;
		std::map<int32_t, int32_t> Mappings;
	};

	class Controller {
	public:
		virtual ~Controller() = default;
		Controller();
		void Read(OSContPad* pad, int32_t virtualSlot);
		virtual void ReadFromSource(int32_t virtualSlot) = 0;
		virtual void WriteToSource(int32_t virtualSlot, ControllerCallback* controller) = 0;
		virtual bool Connected() const = 0;
		virtual bool CanRumble() const = 0;
		virtual bool CanGyro() const = 0;
		virtual void CreateDefaultBinding(int32_t virtualSlot) = 0;
		virtual void ClearRawPress() = 0;
		virtual int32_t ReadRawPress() = 0;
		void SetButtonMapping(int32_t virtualSlot, int32_t n64Button, int32_t dwScancode);
		std::shared_ptr<ControllerAttachment> GetAttachment() { return Attachment; }
		int8_t& getLeftStickX(int32_t virtualSlot);
		int8_t& getLeftStickY(int32_t virtualSlot);
		int8_t& getRightStickX(int32_t virtualSlot);
		int8_t& getRightStickY(int32_t virtualSlot);
		int32_t& getPressedButtons(int32_t virtualSlot);
		float& getGyroX(int32_t virtualSlot);
		float& getGyroY(int32_t virtualSlot);
		std::shared_ptr<DeviceProfile> getProfile(int32_t virtualSlot);
		bool IsRumbling() { return isRumbling; }
		std::string GetGuid() { return GUID; }
		virtual const std::string GetButtonName(int32_t virtualSlot, int32_t n64Button) = 0;
		virtual const std::string GetControllerName() = 0;

		protected:
			std::shared_ptr<ControllerAttachment> Attachment;
			std::string GUID;
			bool isRumbling;
			
			void LoadBinding();

		private:
			struct Buttons {
				int32_t pressedButtons = 0;
				int8_t leftStickX = 0;
				int8_t leftStickY = 0;
				int8_t rightStickX = 0;
				int8_t rightStickY = 0;
				float gyroX = 0.0f;
				float gyroY = 0.0f;
			};

			std::unordered_map<int32_t, std::shared_ptr<DeviceProfile>> profiles;
			std::unordered_map<int32_t, std::shared_ptr<Buttons>> ButtonData = {};
	};
}
