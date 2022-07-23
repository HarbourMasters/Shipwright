#pragma once
#include "Controller.h"
#if __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

namespace Ship {
	class SDLController : public Controller {
		public:
			SDLController(int slot) : Controller(), Cont(nullptr), physicalSlot(slot) { }
			void ReadFromSource(int32_t slot) override;
			const char* GetControllerName() override;
			const char* GetButtonName(int slot, int n64Button) override;
			void WriteToSource(int32_t slot, ControllerCallback* controller) override;
			bool Connected() const override { return Cont != nullptr; }
			bool CanGyro() const override { return supportsGyro; }
			bool CanRumble() const override {
#if SDL_COMPILEDVERSION >= SDL_VERSIONNUM(2,0,18)
				return SDL_GameControllerHasRumble(Cont);
#endif
				return true;
			}

			bool Open();
			void ClearRawPress() override {}
			int32_t ReadRawPress() override;

		protected:
			void CreateDefaultBinding(int32_t slot) override;

		private:
			SDL_GameController* Cont;
			int physicalSlot;
			bool supportsGyro;
			void NormalizeStickAxis(int16_t wAxisValueX, int16_t wAxisValueY, int16_t wAxisThreshold, bool isRightStick, float sensitivity);
			bool Close();
	};
}
