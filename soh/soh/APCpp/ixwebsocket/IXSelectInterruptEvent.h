/*
 *  IXSelectInterruptEvent.h
 */

#pragma once

#include "IXSelectInterrupt.h"
#include <cstdint>
#include <mutex>
#include <string>
#include <deque>
#ifdef _WIN32
#include <windows.h>
#endif

namespace ix
{
    class SelectInterruptEvent final : public SelectInterrupt
    {
    public:
        SelectInterruptEvent();
        virtual ~SelectInterruptEvent();

        bool init(std::string& /*errorMsg*/) final;

        bool notify(uint64_t value) final;
        bool clear() final;
        uint64_t read() final;
        void* getEvent() const final;
    private:
        // contains every value only once, new values are inserted at the begin, nu
        std::deque<uint64_t> _values;
        std::mutex _valuesMutex;
#ifdef _WIN32
        // Windows Event to wake up the socket poll
        HANDLE _event;
#endif
    };
} // namespace ix
