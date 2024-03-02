/*
 *  IXConnectionState.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace ix
{
    using OnSetTerminatedCallback = std::function<void()>;

    class ConnectionState
    {
    public:
        ConnectionState();
        virtual ~ConnectionState() = default;

        virtual void computeId();
        virtual const std::string& getId() const;

        void setTerminated();
        bool isTerminated() const;

        const std::string& getRemoteIp();
        int getRemotePort();

        static std::shared_ptr<ConnectionState> createConnectionState();

    private:
        void setOnSetTerminatedCallback(const OnSetTerminatedCallback& callback);

        void setRemoteIp(const std::string& remoteIp);
        void setRemotePort(int remotePort);

    protected:
        std::atomic<bool> _terminated;
        std::string _id;
        OnSetTerminatedCallback _onSetTerminatedCallback;

        static std::atomic<uint64_t> _globalId;

        std::string _remoteIp;
        int _remotePort;

        friend class SocketServer;
    };
} // namespace ix
