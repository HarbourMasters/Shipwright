#pragma once

#include <memory>
#include <map>
#include <string>
#include <optional>
#include "stdint.h"
#include "UltraController.h"
#include "ControllerAttachment.h"

#define EXTENDED_SCANCODE_BIT (1 << 8)
#define AXIS_SCANCODE_BIT (1 << 9)

namespace Ship {
	class Controller {

		public:
			Controller(int32_t dwControllerNumber);

			void Read(OSContPad* pad);
			virtual void ReadFromSource() = 0;
			virtual void WriteToSource(ControllerCallback* controller) = 0;
			virtual bool Connected() const = 0;
			virtual bool CanRumble() const = 0;
			bool isRumbling;

			void SetButtonMapping(const std::string& szButtonName, int32_t dwScancode);
			std::shared_ptr<ControllerAttachment> GetAttachment() { return Attachment; }
			int32_t GetControllerNumber() { return dwControllerNumber; }

			virtual bool HasPadConf() const = 0;
			virtual std::optional<std::string> GetPadConfSection() = 0;

		protected:
			int32_t dwPressedButtons;
			std::map<int32_t, int32_t> ButtonMapping;
			int8_t wStickX;
			int8_t wStickY;
			float wGyroX;
			float wGyroY;
			
			virtual std::string GetControllerType() = 0;
			virtual std::string GetConfSection() = 0;
			virtual std::string GetBindingConfSection() = 0;
			void LoadBinding();

		private:
			std::shared_ptr<ControllerAttachment> Attachment;
			int32_t dwControllerNumber;
	};
}
