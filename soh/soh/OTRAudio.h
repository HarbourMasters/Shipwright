#pragma once
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

// Defined once in OTRGlobals.cpp: a header-local static gave each TU its own
// copy, so savestates.cpp locked a mutex the audio thread never used.
struct OTRAudioState {
    std::thread thread;
    std::condition_variable cv_to_thread;
    std::mutex mutex;
    bool running;
    bool in_frame;
    bool primed;
    bool produced_in_window;
    std::chrono::steady_clock::time_point last_drainless_update;
};
extern OTRAudioState audio;

extern "C" void SOH_Audio_Pump();
