#pragma once
#include "SDLController.h"


namespace Ship {
	class SDLGamepadController : public SDLController {
		public:
			SDLGamepadController(int32_t dwControllerNumber);
			~SDLGamepadController();

			void ReadFromSource();
			void WriteToSource(ControllerCallback* controller);


		protected:
			std::string GetControllerType();
			void CreateDefaultBinding();

		private:
			SDL_GameController* Cont;

			bool Open();
			bool Close();
	};
}
