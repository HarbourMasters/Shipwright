/*
 *  IXSetThreadName.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 2020 Machine Zone, Inc. All rights reserved.
 */
#include "IXSetThreadName.h"

// unix systems
#if defined(__APPLE__) || defined(__linux__) || defined(BSD)
#include <pthread.h>
#endif

// freebsd needs this header as well
#if defined(BSD)
#include <pthread_np.h>
#endif

// Windows
#ifdef _WIN32
#include <windows.h>
#endif

namespace ix
{
#ifdef _WIN32
    const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType;     // Must be 0x1000.
        LPCSTR szName;    // Pointer to name (in user addr space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags;    // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

    void SetThreadName(DWORD dwThreadID, const char* threadName)
    {
#ifndef __GNUC__
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = threadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags = 0;

        __try
        {
            RaiseException(
                MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*) &info);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
#endif
    }
#endif

    void setThreadName(const std::string& name)
    {
#if defined(__APPLE__)
        //
        // Apple reserves 16 bytes for its thread names
        // Notice that the Apple version of pthread_setname_np
        // does not take a pthread_t argument
        //
        pthread_setname_np(name.substr(0, 63).c_str());
#elif defined(__linux__)
        //
        // Linux only reserves 16 bytes for its thread names
        // See prctl and PR_SET_NAME property in
        // http://man7.org/linux/man-pages/man2/prctl.2.html
        //
        pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
#elif defined(_WIN32)
        SetThreadName(-1, name.c_str());
#elif defined(BSD)
        pthread_set_name_np(pthread_self(), name.substr(0, 15).c_str());
#else
        // ... assert here ?
#endif
    }
} // namespace ix
