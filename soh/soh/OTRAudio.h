#pragma once

static struct {
    std::condition_variable cv_to_thread, cv_from_thread;
    std::mutex mutex;
    bool initialized;
    bool processing;
} audio;
