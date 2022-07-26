#pragma once
#include "AudioPlayer.h"
#include <SDL2/SDL.h>

namespace Ship {
	class SDLAudioPlayer : public AudioPlayer {
	public:
		SDLAudioPlayer() {  };

		bool Init(void);
		int Buffered(void);
		int GetDesiredBuffered(void);
		void Play(const uint8_t* Buffer, uint32_t BufferLen);

	private:
		SDL_AudioDeviceID Device;
	};
}
