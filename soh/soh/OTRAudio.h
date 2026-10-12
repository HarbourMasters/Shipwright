#pragma once
#include <thread>
#include <condition_variable>

static struct {
    std::thread thread;
    std::condition_variable cv_to_thread, cv_from_thread;
    std::mutex mutex;
    bool running;
    bool processing;
} audio;
