#include "SDLAudioPlayer.h"
#include "spdlog/spdlog.h"

namespace Ship {
    bool SDLAudioPlayer::Init(void) {
        if (SDL_Init(SDL_INIT_AUDIO) != 0) {
            SPDLOG_ERROR("SDL init error: %s\n", SDL_GetError());
            return false;
        }
        SDL_AudioSpec want, have;
        SDL_zero(want);
        want.freq = this->GetSampleRate();
        want.format = AUDIO_S16;
        want.channels = 2;
        want.samples = 1024;
        want.callback = NULL;
        Device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
        if (Device == 0) {
            SPDLOG_ERROR("SDL_OpenAudio error: {}", SDL_GetError());
            return false;
        }
        SDL_PauseAudioDevice(Device, 0);
        return true;
    }

    int SDLAudioPlayer::Buffered(void) {
        // 4 is sizeof(int16_t) * num_channels (2 for stereo)
        return SDL_GetQueuedAudioSize(Device) / 4;
    }

    int SDLAudioPlayer::GetDesiredBuffered(void) {
        return 2480;
    }

    void SDLAudioPlayer::Play(const uint8_t* Buffer, uint32_t BufferLen) {
        if (Buffered() < 6000) {
            // Don't fill the audio buffer too much in case this happens
            SDL_QueueAudio(Device, Buffer, BufferLen);
        }
    }
}
