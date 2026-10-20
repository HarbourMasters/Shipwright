#pragma once
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <functional>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>

// Ripped sfx are stored as mono Ogg Vorbis at the game's output rate. Storing raw PCM
// made accessibility.o2r roughly 7x larger for no benefit; nothing ever read a wav header.
#define A11Y_SFX_SAMPLE_RATE 32000
// VBR quality. These are short, already heavily processed sounds, so a low setting is plenty.
#define A11Y_SFX_VORBIS_QUALITY 0.2f
// Bumped whenever the stored format changes, so a stale archive is detected instead of
// silently producing no sound.
#define A11Y_SFX_FORMAT_VERSION "vorbis1"
#define A11Y_SFX_FORMAT_PATH "accessibility/audio/format"

bool AccessibleSfx_EncodeVorbis(const int16_t* mono, size_t frames, std::vector<uint8_t>& out);
bool AccessibleSfx_DecodeVorbis(const uint8_t* data, size_t size, std::vector<int16_t>& out);

// Runs sfx decodes off the game thread. A sound that isn't decoded yet simply doesn't play,
// which is preferable to stalling a frame on it.
class AccessibleSfxDecoder {
    std::deque<std::function<void()>> jobs;
    std::thread thread;
    std::mutex mtx;
    std::condition_variable cv;
    bool running;

    void runThread();

  public:
    AccessibleSfxDecoder();
    ~AccessibleSfxDecoder();
    void enqueue(std::function<void()> job);
};
