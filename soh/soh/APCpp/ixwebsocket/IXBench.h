/*
 *  IXBench.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2020 Machine Zone, Inc. All rights reserved.
 */
#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace ix
{
    class Bench
    {
    public:
        Bench(const std::string& description);
        ~Bench();

        void reset();
        void record();
        void report();
        void setReported();
        uint64_t getDuration() const;

    private:
        std::string _description;
        std::chrono::time_point<std::chrono::high_resolution_clock> _start;
        uint64_t _duration;
        bool _reported;
    };
} // namespace ix
