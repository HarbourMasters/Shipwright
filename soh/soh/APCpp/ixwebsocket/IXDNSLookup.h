/*
 *  IXDNSLookup.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 Machine Zone, Inc. All rights reserved.
 *
 *  Resolve a hostname+port to a struct addrinfo obtained with getaddrinfo
 *  Does this in a background thread so that it can be cancelled, since
 *  getaddrinfo is a blocking call, and we don't want to block the main thread on Mobile.
 */

#pragma once

#include "IXCancellationRequest.h"
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <set>
#include <string>

struct addrinfo;

namespace ix
{
    class DNSLookup : public std::enable_shared_from_this<DNSLookup>
    {
    public:
        using AddrInfoPtr = std::shared_ptr<addrinfo>;
        DNSLookup(const std::string& hostname, int port, int64_t wait = DNSLookup::kDefaultWait);
        ~DNSLookup() = default;

        AddrInfoPtr resolve(std::string& errMsg,
                                 const CancellationRequest& isCancellationRequested,
                                 bool cancellable = true);

    private:
        AddrInfoPtr resolveCancellable(std::string& errMsg,
                                            const CancellationRequest& isCancellationRequested);
        AddrInfoPtr resolveUnCancellable(std::string& errMsg,
                                              const CancellationRequest& isCancellationRequested);

        AddrInfoPtr getAddrInfo(const std::string& hostname,
                                            int port,
                                            std::string& errMsg);

        void run(std::weak_ptr<DNSLookup> self, std::string hostname, int port); // thread runner

        void setErrMsg(const std::string& errMsg);
        const std::string& getErrMsg();

        void setRes(AddrInfoPtr addr);
        AddrInfoPtr getRes();

        std::string _hostname;
        int _port;
        int64_t _wait;
        const static int64_t kDefaultWait;

        AddrInfoPtr _res;
        std::mutex _resMutex;

        std::string _errMsg;
        std::mutex _errMsgMutex;

        std::atomic<bool> _done;
    };
} // namespace ix
