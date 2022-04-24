#pragma once
#include "Controller.h"
#include <SDL2/SDL.h>

#define INVALID_SDL_CONTROLLER_GUID (std::string("00000000000000000000000000000000"))

namespace Ship {
	class SDLController : public Controller {
		public:
			SDLController(int32_t dwControllerNumber);
			~SDLController();

			void ReadFromSource();
			void WriteToSource(ControllerCallback* controller);
			bool CanRumble() const { return SDL_GameControllerHasRumble(Cont); }

			std::string GetGuid() { return guid; };

			std::optional<std::string> GetPadConfSection();
			bool HasPadConf() { return true; };

		protected:
			std::string GetControllerType();
			void SetButtonMapping(const std::string& szButtonName, int32_t dwScancode);
			std::string GetConfSection();
			std::string GetBindingConfSection();
			void CreateDefaultBinding();
			void CreateDefaultPadConf();
			static bool IsGuidInUse(const std::string& guid);

		private:
			std::string guid;
			SDL_GameController* Cont;
			std::map<int32_t, int16_t> ThresholdMapping;

			void LoadAxisThresholds();
			void NormalizeStickAxis(int16_t wAxisValueX, int16_t wAxisValueY, int16_t wAxisThreshold);
			bool Open();
			bool Close();
	};
}
