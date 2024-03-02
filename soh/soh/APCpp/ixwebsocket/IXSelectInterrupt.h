/*
 *  IXSelectInterrupt.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace ix
{
    class SelectInterrupt
    {
    public:
        SelectInterrupt();
        virtual ~SelectInterrupt();

        virtual bool init(std::string& errorMsg);

        virtual bool notify(uint64_t value);
        virtual bool clear();
        virtual uint64_t read();
        virtual int getFd() const;
        virtual void* getEvent() const;

        // Used as special codes for pipe communication
        static const uint64_t kSendRequest;
        static const uint64_t kCloseRequest;
    };

    using SelectInterruptPtr = std::unique_ptr<SelectInterrupt>;
} // namespace ix
