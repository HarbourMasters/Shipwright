#pragma once
#include "AudioPlayer.h"
#if __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

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
