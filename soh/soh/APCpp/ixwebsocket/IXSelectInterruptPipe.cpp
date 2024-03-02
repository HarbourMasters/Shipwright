/*
 *  IXSelectInterruptPipe.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018-2019 Machine Zone, Inc. All rights reserved.
 */

//
// On UNIX we use pipes to wake up select. There is no way to do that
// on Windows so this file is compiled out on Windows.
//
#ifndef _WIN32

#include "IXSelectInterruptPipe.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sstream>
#include <string.h> // for strerror
#include <unistd.h> // for write

namespace ix
{
    // File descriptor at index 0 in _fildes is the read end of the pipe
    // File descriptor at index 1 in _fildes is the write end of the pipe
    const int SelectInterruptPipe::kPipeReadIndex = 0;
    const int SelectInterruptPipe::kPipeWriteIndex = 1;

    SelectInterruptPipe::SelectInterruptPipe()
    {
        _fildes[kPipeReadIndex] = -1;
        _fildes[kPipeWriteIndex] = -1;
    }

    SelectInterruptPipe::~SelectInterruptPipe()
    {
        ::close(_fildes[kPipeReadIndex]);
        ::close(_fildes[kPipeWriteIndex]);
        _fildes[kPipeReadIndex] = -1;
        _fildes[kPipeWriteIndex] = -1;
    }

    bool SelectInterruptPipe::init(std::string& errorMsg)
    {
        std::lock_guard<std::mutex> lock(_fildesMutex);

        // calling init twice is a programming error
        assert(_fildes[kPipeReadIndex] == -1);
        assert(_fildes[kPipeWriteIndex] == -1);

        if (pipe(_fildes) < 0)
        {
            std::stringstream ss;
            ss << "SelectInterruptPipe::init() failed in pipe() call"
               << " : " << strerror(errno);
            errorMsg = ss.str();
            return false;
        }

        if (fcntl(_fildes[kPipeReadIndex], F_SETFL, O_NONBLOCK) == -1)
        {
            std::stringstream ss;
            ss << "SelectInterruptPipe::init() failed in fcntl(..., O_NONBLOCK) call"
               << " : " << strerror(errno);
            errorMsg = ss.str();

            _fildes[kPipeReadIndex] = -1;
            _fildes[kPipeWriteIndex] = -1;
            return false;
        }

        if (fcntl(_fildes[kPipeWriteIndex], F_SETFL, O_NONBLOCK) == -1)
        {
            std::stringstream ss;
            ss << "SelectInterruptPipe::init() failed in fcntl(..., O_NONBLOCK) call"
               << " : " << strerror(errno);
            errorMsg = ss.str();

            _fildes[kPipeReadIndex] = -1;
            _fildes[kPipeWriteIndex] = -1;
            return false;
        }

#ifdef F_SETNOSIGPIPE
        if (fcntl(_fildes[kPipeWriteIndex], F_SETNOSIGPIPE, 1) == -1)
        {
            std::stringstream ss;
            ss << "SelectInterruptPipe::init() failed in fcntl(.... F_SETNOSIGPIPE) call"
               << " : " << strerror(errno);
            errorMsg = ss.str();

            _fildes[kPipeReadIndex] = -1;
            _fildes[kPipeWriteIndex] = -1;
            return false;
        }

        if (fcntl(_fildes[kPipeWriteIndex], F_SETNOSIGPIPE, 1) == -1)
        {
            std::stringstream ss;
            ss << "SelectInterruptPipe::init() failed in fcntl(..., F_SETNOSIGPIPE) call"
               << " : " << strerror(errno);
            errorMsg = ss.str();

            _fildes[kPipeReadIndex] = -1;
            _fildes[kPipeWriteIndex] = -1;
            return false;
        }
#endif

        return true;
    }

    bool SelectInterruptPipe::notify(uint64_t value)
    {
        std::lock_guard<std::mutex> lock(_fildesMutex);

        int fd = _fildes[kPipeWriteIndex];
        if (fd == -1) return false;

        ssize_t ret = -1;
        do
        {
            ret = ::write(fd, &value, sizeof(value));
        } while (ret == -1 && errno == EINTR);

        // we should write 8 bytes for an uint64_t
        return ret == 8;
    }

    // TODO: return max uint64_t for errors ?
    uint64_t SelectInterruptPipe::read()
    {
        std::lock_guard<std::mutex> lock(_fildesMutex);

        int fd = _fildes[kPipeReadIndex];

        uint64_t value = 0;

        ssize_t ret = -1;
        do
        {
            ret = ::read(fd, &value, sizeof(value));
        } while (ret == -1 && errno == EINTR);

        return value;
    }

    bool SelectInterruptPipe::clear()
    {
        return true;
    }

    int SelectInterruptPipe::getFd() const
    {
        std::lock_guard<std::mutex> lock(_fildesMutex);

        return _fildes[kPipeReadIndex];
    }
} // namespace ix

#endif // !_WIN32
