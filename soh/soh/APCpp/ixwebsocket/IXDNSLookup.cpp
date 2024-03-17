/*
 *  IXDNSLookup.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 Machine Zone, Inc. All rights reserved.
 */

//
// On Windows Universal Platform (uwp), gai_strerror defaults behavior is to returns wchar_t
// which is different from all other platforms. We want the non unicode version.
// See https://github.com/microsoft/vcpkg/pull/11030
// We could do this in IXNetSystem.cpp but so far we are only using gai_strerror in here.
//
#ifdef _UNICODE
#undef _UNICODE
#endif
#ifdef UNICODE
#undef UNICODE
#endif

#include "IXDNSLookup.h"

#include "IXNetSystem.h"
#include <chrono>
#include <string.h>
#include <thread>
#include <utility>

// mingw build quirks
#if defined(_WIN32) && defined(__GNUC__)
#define AI_NUMERICSERV NI_NUMERICSERV
#define AI_ADDRCONFIG LUP_ADDRCONFIG
#endif

namespace ix
{
    const int64_t DNSLookup::kDefaultWait = 1; // ms

    DNSLookup::DNSLookup(const std::string& hostname, int port, int64_t wait)
        : _hostname(hostname)
        , _port(port)
        , _wait(wait)
        , _res(nullptr)
        , _done(false)
    {
        ;
    }

    DNSLookup::AddrInfoPtr DNSLookup::getAddrInfo(const std::string& hostname,
                                            int port,
                                            std::string& errMsg)
    {
        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_flags = AI_ADDRCONFIG | AI_NUMERICSERV;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        std::string sport = std::to_string(port);

        struct addrinfo* res;
        int getaddrinfo_result = getaddrinfo(hostname.c_str(), sport.c_str(), &hints, &res);
        if (getaddrinfo_result)
        {
            errMsg = gai_strerror(getaddrinfo_result);
            res = nullptr;
        }
        return AddrInfoPtr{ res, freeaddrinfo };
    }

    DNSLookup::AddrInfoPtr DNSLookup::resolve(std::string& errMsg,
                                        const CancellationRequest& isCancellationRequested,
                                        bool cancellable)
    {
        return cancellable ? resolveCancellable(errMsg, isCancellationRequested)
                           : resolveUnCancellable(errMsg, isCancellationRequested);
    }

    DNSLookup::AddrInfoPtr DNSLookup::resolveUnCancellable(
        std::string& errMsg, const CancellationRequest& isCancellationRequested)
    {
        errMsg = "no error";

        // Maybe a cancellation request got in before the background thread terminated ?
        if (isCancellationRequested())
        {
            errMsg = "cancellation requested";
            return nullptr;
        }

        return getAddrInfo(_hostname, _port, errMsg);
    }

    DNSLookup::AddrInfoPtr DNSLookup::resolveCancellable(
        std::string& errMsg, const CancellationRequest& isCancellationRequested)
    {
        errMsg = "no error";

        // Can only be called once, otherwise we would have to manage a pool
        // of background thread which is overkill for our usage.
        if (_done)
        {
            return nullptr; // programming error, create a second DNSLookup instance
                            // if you need a second lookup.
        }

        //
        // Good resource on thread forced termination
        // https://www.bo-yang.net/2017/11/19/cpp-kill-detached-thread
        //
        auto ptr = shared_from_this();
        std::weak_ptr<DNSLookup> self(ptr);

        int port = _port;
        std::string hostname(_hostname);

        // We make the background thread doing the work a shared pointer
        // instead of a member variable, because it can keep running when
        // this object goes out of scope, in case of cancellation
        auto t = std::make_shared<std::thread>(&DNSLookup::run, this, self, hostname, port);
        t->detach();

        while (!_done)
        {
            // Wait for 1 milliseconds, to see if the bg thread has terminated.
            // We do not use a condition variable to wait, as destroying this one
            // if the bg thread is alive can cause undefined behavior.
            std::this_thread::sleep_for(std::chrono::milliseconds(_wait));

            // Were we cancelled ?
            if (isCancellationRequested())
            {
                errMsg = "cancellation requested";
                return nullptr;
            }
        }

        // Maybe a cancellation request got in before the bg terminated ?
        if (isCancellationRequested())
        {
            errMsg = "cancellation requested";
            return nullptr;
        }

        errMsg = getErrMsg();
        return getRes();
    }

    void DNSLookup::run(std::weak_ptr<DNSLookup> self,
                        std::string hostname,
                        int port) // thread runner
    {
        // We don't want to read or write into members variables of an object that could be
        // gone, so we use temporary variables (res) or we pass in by copy everything that
        // getAddrInfo needs to work.
        std::string errMsg;
        auto res = getAddrInfo(hostname, port, errMsg);

        if (auto lock = self.lock())
        {
            // Copy result into the member variables
            setRes(res);
            setErrMsg(errMsg);

            _done = true;
        }
    }

    void DNSLookup::setErrMsg(const std::string& errMsg)
    {
        std::lock_guard<std::mutex> lock(_errMsgMutex);
        _errMsg = errMsg;
    }

    const std::string& DNSLookup::getErrMsg()
    {
        std::lock_guard<std::mutex> lock(_errMsgMutex);
        return _errMsg;
    }

    void DNSLookup::setRes(DNSLookup::AddrInfoPtr addr)
    {
        std::lock_guard<std::mutex> lock(_resMutex);
        _res = std::move(addr);
    }

    DNSLookup::AddrInfoPtr DNSLookup::getRes()
    {
        std::lock_guard<std::mutex> lock(_resMutex);
        return _res;
    }
} // namespace ix
