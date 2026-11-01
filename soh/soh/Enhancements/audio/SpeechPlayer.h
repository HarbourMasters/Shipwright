#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace SOH {

// Where spoken text goes once the speech engine has been asked for samples rather
// than for sound. One utterance plays at a time, either mixed into the game's own
// output or, when the player asks for it, on a device of its own - so speech can go
// to a headset while the music stays on the speakers.
//
// Play() and Stop() run on the speech thread and hand an utterance over; Mix() runs
// on the audio thread and never blocks on either.
class SpeechPlayer {
  public:
    static SpeechPlayer& Instance();

    // Mono samples at the engine's own rate. Replaces whatever is playing, which is
    // how the speech backends interrupt themselves.
    void Play(std::vector<int16_t>&& samples, int32_t rate);
    void Stop();

    // Cheap enough for the audio thread to gate its stock path on. False while speech
    // is on its own device, where it adds nothing to the game's output.
    [[nodiscard("the answer decides whether the mix has to run at all")]] bool IsPlaying() const noexcept;

    // Audio thread: adds `frames` of interleaved stereo into `buffer`, resampling from
    // the engine's rate to `outRate`. Adds nothing when idle or routed away.
    void Mix(int16_t* buffer, uint32_t frames, int32_t outRate);

    // Opens or closes the player's own output device, by name as the system reports it;
    // empty asks for the default. Game thread.
    //
    // Only a device the game is not already playing on becomes a stream of its own:
    // everything opened on one device is folded back together before the system sees
    // it. On the game's own device speech still escapes the game's volume, but it
    // cannot be routed away from the music.
    void SetSeparateOutput(bool separate, const std::string& device);

  private:
    SpeechPlayer() = default;

    static void Feed(void* self, uint8_t* stream, int length);
    void MixLocked(int16_t* buffer, uint32_t frames, int32_t outRate);

    std::mutex mHandoff;
    std::shared_ptr<std::vector<int16_t>> mPending;
    // Freeing the outgoing utterance on the audio thread would put an allocator call
    // in the mix; the next Play() or Stop() drops it on the speech thread instead.
    std::shared_ptr<std::vector<int16_t>> mRetired;
    int32_t mPendingRate = 0;
    bool mHasPending = false;

    // Mixing thread only, past the handoff.
    std::shared_ptr<std::vector<int16_t>> mCurrent;
    int32_t mRate = 0;
    double mCursor = 0.0;

    std::atomic<bool> mActive{ false };

    // Shared by both mixing threads so a routing change cannot leave them running over
    // the same cursor; only one of them is ever active for longer than that.
    std::mutex mMixing;
    std::atomic<bool> mSeparate{ false };
    uint32_t mDeviceId = 0;
    std::string mDevice;
};

} // namespace SOH
