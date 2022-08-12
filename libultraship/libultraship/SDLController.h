#pragma once
#include "Controller.h"
#include <SDL2/SDL.h>

namespace Ship {
	class SDLController : public Controller {
		public:
			inline static const char* AxisNames[] = {
				"Left Stick X",
				"Left Stick Y",
				"Right Stick X",
				"Right Stick Y",
				"Left Trigger",
				"Right Trigger",
				"Start Button"
			};

			SDLController(int32_t physicalSlot) : Controller(), Cont(nullptr), physicalSlot(physicalSlot) { }
			void ReadFromSource(int32_t virtualSlot) override;
			const std::string GetControllerName() override;
			const std::string GetButtonName(int32_t virtualSlot, int32_t n64Button) override;
			void WriteToSource(int32_t virtualSlot, ControllerCallback* controller) override;
			bool Connected() const override { return Cont != nullptr; }
			bool CanGyro() const override { return supportsGyro; }
			bool CanRumble() const override {
#if SDL_COMPILEDVERSION >= SDL_VERSIONNUM(2,0,18)
				return SDL_GameControllerHasRumble(Cont);
#endif
				return false;
			}

			bool Open();
			void ClearRawPress() override {}
			int32_t ReadRawPress() override;

		protected:
			void CreateDefaultBinding(int32_t virtualSlot) override;

		private:
			std::string ControllerName = "Unknown";
			SDL_GameController* Cont;
			int32_t physicalSlot;
			bool supportsGyro;
			void NormalizeStickAxis(SDL_GameControllerAxis axisX, SDL_GameControllerAxis axisY, int16_t axisThreshold, int32_t virtualSlot);
			bool Close();
	};
}
