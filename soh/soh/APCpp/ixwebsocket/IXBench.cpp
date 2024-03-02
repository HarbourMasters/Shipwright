/*
 *  IXBench.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2020 Machine Zone, Inc. All rights reserved.
 */

#include "IXBench.h"

#include <iostream>

namespace ix
{
    Bench::Bench(const std::string& description)
        : _description(description)
    {
        reset();
    }

    Bench::~Bench()
    {
        if (!_reported)
        {
            report();
        }
    }

    void Bench::reset()
    {
        _start = std::chrono::high_resolution_clock::now();
        _reported = false;
    }

    void Bench::report()
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - _start);

        _duration = microseconds.count();
        std::cerr << _description << " completed in " << _duration << " us" << std::endl;

        setReported();
    }

    void Bench::record()
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - _start);

        _duration = microseconds.count();
    }

    void Bench::setReported()
    {
        _reported = true;
    }

    uint64_t Bench::getDuration() const
    {
        return _duration;
    }
} // namespace ix
