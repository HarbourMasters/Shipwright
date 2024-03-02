/*
 *  IXSelectInterruptEvent.cpp
 */

//
// On Windows we use a Windows Event to wake up ix::poll() (WSAWaitForMultipleEvents).
// And on any other platform that doesn't support pipe file descriptors we
// emulate the interrupt event by using a short timeout with ix::poll() and
// read from the SelectInterrupt. (see Socket::poll() "Emulation mode")
//
#include <algorithm>
#include "IXSelectInterruptEvent.h"

namespace ix
{
    SelectInterruptEvent::SelectInterruptEvent()
    {
#ifdef _WIN32
        _event = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif
    }

    SelectInterruptEvent::~SelectInterruptEvent()
    {
#ifdef _WIN32
        CloseHandle(_event);
#endif
    }

    bool SelectInterruptEvent::init(std::string& /*errorMsg*/)
    {
        return true;
    }

    bool SelectInterruptEvent::notify(uint64_t value)
    {
        std::lock_guard<std::mutex> lock(_valuesMutex);

        // WebSocket implementation detail: We only need one of the values in the queue
        if (std::find(_values.begin(), _values.end(), value) == _values.end())
            _values.push_back(value);
#ifdef _WIN32
        SetEvent(_event); // wake up
#endif
        return true;
    }

    uint64_t SelectInterruptEvent::read()
    {
        std::lock_guard<std::mutex> lock(_valuesMutex);

        if (_values.size() > 0)
        {
            uint64_t value = _values.front();
            _values.pop_front();
#ifdef _WIN32
            // signal the event if there is still data in the queue
            if (_values.size() == 0)
                ResetEvent(_event);
#endif
            return value;
        }
        return 0;
    }

    bool SelectInterruptEvent::clear()
    {
        std::lock_guard<std::mutex> lock(_valuesMutex);
        _values.clear();
#ifdef _WIN32
        ResetEvent(_event);
#endif
        return true;
    }

    void* SelectInterruptEvent::getEvent() const
    {
#ifdef _WIN32
        return reinterpret_cast<void*>(_event);
#else
        return nullptr;
#endif
    }

} // namespace ix
