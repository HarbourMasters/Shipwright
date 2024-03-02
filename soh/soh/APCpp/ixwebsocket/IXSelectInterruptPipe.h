/*
 *  IXSelectInterruptPipe.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018-2019 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include "IXSelectInterrupt.h"
#include <cstdint>
#include <mutex>
#include <stdint.h>
#include <string>

namespace ix
{
    class SelectInterruptPipe final : public SelectInterrupt
    {
    public:
        SelectInterruptPipe();
        virtual ~SelectInterruptPipe();

        bool init(std::string& errorMsg) final;

        bool notify(uint64_t value) final;
        bool clear() final;
        uint64_t read() final;
        int getFd() const final;

    private:
        // Store file descriptors used by the communication pipe. Communication
        // happens between a control thread and a background thread, which is
        // blocked on select.
        int _fildes[2];
        mutable std::mutex _fildesMutex;

        // Used to identify the read/write idx
        static const int kPipeReadIndex;
        static const int kPipeWriteIndex;
    };
} // namespace ix
