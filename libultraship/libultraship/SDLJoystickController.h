#pragma once
#include "SDLController.h"
#include <SDL2/SDL.h>

namespace Ship {
	class SDLJoystickController : public SDLController {
		public:
			SDLJoystickController(int32_t dwControllerNumber);
			~SDLJoystickController();

			void ReadFromSource();
			void WriteToSource(ControllerCallback* controller);

		protected:
			std::string GetControllerType();
			void CreateDefaultBinding();

		private:
			SDL_Joystick* Cont;
			int numButtons;

			bool Open();
			bool Close();
	};
}
