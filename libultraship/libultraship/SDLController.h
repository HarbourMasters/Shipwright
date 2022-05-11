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
			bool Connected() const { return Cont != nullptr; }
			bool CanRumble() const {
#if SDL_COMPILEDVERSION >= SDL_VERSIONNUM(2,0,18)
				return SDL_GameControllerHasRumble(Cont);
#endif
				return true;
			}

			std::string GetGuid() { return guid; };

			bool HasPadConf() const { return true; }
			std::optional<std::string> GetPadConfSection();

		protected:
			std::string GetControllerType();
			void SetButtonMapping(const std::string& szButtonName, int32_t dwScancode);
			std::string GetConfSection();
			std::string GetBindingConfSection();
			void CreateDefaultBinding();
			void CreateDefaultPadConf();
			static bool IsGuidInUse(const std::string& guid);

		private:
			SDL_GameController* Cont;
			std::string guid;
			std::map<int32_t, int16_t> ThresholdMapping;

			void LoadAxisThresholds();
			void NormalizeStickAxis(int16_t wAxisValueX, int16_t wAxisValueY, int16_t wAxisThreshold);
			bool Open();
			bool Close();
	};
}
