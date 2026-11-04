#pragma once
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

// One shared instance, defined in OTRGlobals.cpp: a header-local static would
// give each TU its own copy, and savestates.cpp locks audio.mutex to exclude
// the audio engine while copying the audio heap.
struct OTRAudioState {
    std::thread thread;
    std::condition_variable cv_to_thread, cv_from_thread;
    std::mutex mutex;
    bool running;
    bool in_frame;                                   // gfx thread is inside Graph_ProcessGfxCommands
    bool busy;                                       // audio thread is inside AudioMgr_CreateNextAudioBuffer
    bool primed;                                     // the engine has produced its first update
    bool produced_in_window;                         // the engine produced since this gfx frame window opened
    std::chrono::steady_clock::time_point last_tick; // last drainless-path production
};
extern OTRAudioState audio;

extern "C" void OTRAudio_Pump(void);
