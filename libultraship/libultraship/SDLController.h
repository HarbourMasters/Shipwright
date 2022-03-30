#pragma once
#include "Controller.h"
#include <SDL2/SDL.h>

#define INVALID_SDL_CONTROLLER_GUID (std::string("00000000000000000000000000000000"))

namespace Ship {
	class SDLController : public Controller {
		public:
			SDLController(int32_t dwControllerNumber);
			~SDLController();

			std::string GetGuid() { return guid; };

		protected:
			std::string GetControllerType();
			void SetButtonMapping(const std::string& szButtonName, int32_t dwScancode);
			std::string GetConfSection();
			std::string GetBindingConfSection();
			static bool IsGuidInUse(const std::string& guid);
			void LoadAxisThresholds();
			void NormalizeStickAxis(int16_t wAxisValueX, int16_t wAxisValueY, int16_t wAxisThreshold);

			std::string guid;
			std::map<int32_t, int16_t> ThresholdMapping;

		private:
	};
}
