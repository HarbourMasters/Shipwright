#pragma once
#include <thread>

static struct {
    std::thread thread;
    std::mutex mutex;
    bool running;
} audio;
