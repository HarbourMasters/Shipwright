#pragma once
#include <thread>
#include <condition_variable>

static struct {
    std::thread thread;
    std::condition_variable cv_to_thread;
    std::mutex mutex;
    std::atomic_bool running;
    std::atomic_bool processing;
} audio;
