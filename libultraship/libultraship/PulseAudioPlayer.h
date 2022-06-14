#pragma once

#if defined(__linux__) || defined(__BSD__)

#include "AudioPlayer.h"
#include <pulse/pulseaudio.h>

namespace Ship {
	class PulseAudioPlayer : public AudioPlayer {
	public:
		PulseAudioPlayer() {}

		bool Init() override;
		int Buffered() override;
		int GetDesiredBuffered() override;
		void Play(const uint8_t* buff, uint32_t len) override;

	private:
		pa_context* m_Context = nullptr;
		pa_stream* m_Stream = nullptr;
		pa_mainloop* m_MainLoop = nullptr;
		bool m_WriteComplete = false;
		pa_buffer_attr m_Attr = {0};
	};
}
#endif
