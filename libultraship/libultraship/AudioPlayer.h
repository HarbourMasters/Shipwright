#pragma once
#include "stdint.h"

namespace Ship {
	class AudioPlayer {

	public:
		AudioPlayer() {  };

		virtual bool Init(void) = 0;
		virtual int Buffered(void) = 0;
		virtual int GetDesiredBuffered(void) = 0;
		virtual void Play(const uint8_t* buf, uint32_t len) = 0;
		constexpr int GetSampleRate() const { return 44100; }
	};
}

#if !_MSC_VER  && !__APPLE__
#endif
#ifdef _WIN32
#include "WasapiAudioPlayer.h"
#elif defined(__linux)
#include "PulseAudioPlayer.h"
#else
#include "SDLAudioPlayer.h"
#endif
