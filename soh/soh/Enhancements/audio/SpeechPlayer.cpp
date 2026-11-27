#include "SpeechPlayer.h"

#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstring>
#include <utility>

namespace SOH {

namespace {

// The game's mix and the player's own device are both interleaved stereo.
constexpr uint8_t kChannels = 2;
constexpr int32_t kDeviceRate = 48000;

// Catmull-Rom between the two samples either side of the cursor. The ends repeat
// rather than reading past the buffer.
[[nodiscard]] float Interpolate(const std::vector<int16_t>& samples, int64_t index, float t) {
    const auto at = [&samples](int64_t i) {
        const int64_t clamped = std::clamp(i, int64_t(0), int64_t(samples.size()) - 1);
        return float(samples.at(size_t(clamped))) / 32768.0f;
    };

    const float p0 = at(index - 1);
    const float p1 = at(index);
    const float p2 = at(index + 1);
    const float p3 = at(index + 2);

    return p1 +
           0.5f * t * ((p2 - p0) + t * ((2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) + t * (3.0f * (p1 - p2) + p3 - p0)));
}

[[nodiscard]] int16_t AddSaturating(int16_t to, float value) {
    const float sum = float(to) + value * 32767.0f;
    return int16_t(std::clamp(sum, -32768.0f, 32767.0f));
}

} // namespace

SpeechPlayer& SpeechPlayer::Instance() {
    static SpeechPlayer instance;
    return instance;
}

void SpeechPlayer::Play(std::vector<int16_t>&& samples, int32_t rate) {
    if (samples.empty() || rate <= 0) {
        return;
    }

    auto utterance = std::make_shared<std::vector<int16_t>>(std::move(samples));
    std::lock_guard<std::mutex> guard(mHandoff);
    mRetired.reset();
    mPending = std::move(utterance);
    mPendingRate = rate;
    mHasPending = true;
    mActive.store(true, std::memory_order_relaxed);
}

void SpeechPlayer::Stop() {
    std::lock_guard<std::mutex> guard(mHandoff);
    mRetired.reset();
    mPending.reset();
    mPendingRate = 0;
    mHasPending = true;
    mActive.store(false, std::memory_order_relaxed);
}

bool SpeechPlayer::IsPlaying() const noexcept {
    return !mSeparate.load(std::memory_order_relaxed) && mActive.load(std::memory_order_relaxed);
}

void SpeechPlayer::MixLocked(int16_t* buffer, uint32_t frames, int32_t outRate) {
    // A missed pickup costs one batch of latency, which is cheaper than letting the
    // speech thread stall the mix while it hands an utterance over.
    if (mHandoff.try_lock()) {
        if (mHasPending) {
            mRetired = std::move(mCurrent);
            mCurrent = std::move(mPending);
            mRate = mPendingRate;
            mPending.reset();
            mHasPending = false;
            mCursor = 0.0;
        }
        mHandoff.unlock();
    }

    if (mCurrent == nullptr || outRate <= 0 || mRate <= 0) {
        return;
    }

    const std::vector<int16_t>& samples = *mCurrent;
    const double step = double(mRate) / double(outRate);

    double cursor = mCursor;
    for (uint32_t f = 0; f < frames && cursor < double(samples.size()); f++) {
        const auto index = int64_t(cursor);
        const float value = Interpolate(samples, index, float(cursor - double(index)));
        int16_t* frame = &buffer[f * kChannels];
        for (uint8_t c = 0; c < kChannels; c++) {
            frame[c] = AddSaturating(frame[c], value);
        }
        cursor += step;
    }
    mCursor = cursor;

    if (cursor >= double(samples.size())) {
        mActive.store(false, std::memory_order_relaxed);
    }
}

void SpeechPlayer::Mix(int16_t* buffer, uint32_t frames, int32_t outRate) {
    // Two relaxed loads is all an idle game pays for this per batch of samples.
    if (mSeparate.load(std::memory_order_relaxed) || !mActive.load(std::memory_order_relaxed)) {
        return;
    }
    // Held only against the instant a routing change is switching threads over.
    if (mMixing.try_lock()) {
        MixLocked(buffer, frames, outRate);
        mMixing.unlock();
    }
}

void SpeechPlayer::Feed(void* self, uint8_t* stream, int length) {
    std::memset(stream, 0, size_t(length));

    auto& player = *static_cast<SpeechPlayer*>(self);
    if (player.mMixing.try_lock()) {
        const auto frames = uint32_t(size_t(length) / (kChannels * sizeof(int16_t)));
        player.MixLocked(reinterpret_cast<int16_t*>(stream), frames, kDeviceRate);
        player.mMixing.unlock();
    }
}

void SpeechPlayer::SetSeparateOutput(bool separate, const std::string& device) {
    if (separate && mDeviceId != 0 && device == mDevice) {
        return;
    }

    if (mDeviceId != 0) {
        // Ordered so the callback cannot be mixing while its device goes away.
        const SDL_AudioDeviceID id = mDeviceId;
        mDeviceId = 0;
        mSeparate.store(false, std::memory_order_relaxed);
        SDL_CloseAudioDevice(id);
    }
    if (!separate) {
        return;
    }

    // Refcounted, and the game's own audio may not have brought this up yet.
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        SPDLOG_WARN("Speech could not start its own output ({}); it stays in the game mix", SDL_GetError());
        return;
    }

    SDL_AudioSpec want{};
    want.freq = kDeviceRate;
    want.format = AUDIO_S16SYS;
    want.channels = kChannels;
    want.samples = 1024;
    want.callback = &SpeechPlayer::Feed;
    want.userdata = this;

    SDL_AudioSpec have{};
    mDeviceId = SDL_OpenAudioDevice(device.empty() ? nullptr : device.c_str(), 0, &want, &have, 0);
    if (mDeviceId == 0) {
        SPDLOG_WARN("Speech could not open output device \"{}\" ({}); it stays in the game mix", device,
                    SDL_GetError());
        return;
    }

    mDevice = device;
    SDL_PauseAudioDevice(mDeviceId, 0);
    mSeparate.store(true, std::memory_order_relaxed);
}

} // namespace SOH
