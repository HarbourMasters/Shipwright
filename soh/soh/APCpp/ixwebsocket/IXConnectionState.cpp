/*
 *  IXConnectionState.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#include "IXConnectionState.h"

namespace ix
{
    std::atomic<uint64_t> ConnectionState::_globalId(0);

    ConnectionState::ConnectionState()
        : _terminated(false)
    {
        computeId();
    }

    void ConnectionState::computeId()
    {
        _id = std::to_string(_globalId++);
    }

    const std::string& ConnectionState::getId() const
    {
        return _id;
    }

    std::shared_ptr<ConnectionState> ConnectionState::createConnectionState()
    {
        return std::make_shared<ConnectionState>();
    }

    void ConnectionState::setOnSetTerminatedCallback(const OnSetTerminatedCallback& callback)
    {
        _onSetTerminatedCallback = callback;
    }

    bool ConnectionState::isTerminated() const
    {
        return _terminated;
    }

    void ConnectionState::setTerminated()
    {
        _terminated = true;

        if (_onSetTerminatedCallback)
        {
            _onSetTerminatedCallback();
        }
    }

    const std::string& ConnectionState::getRemoteIp()
    {
        return _remoteIp;
    }

    int ConnectionState::getRemotePort()
    {
        return _remotePort;
    }

    void ConnectionState::setRemoteIp(const std::string& remoteIp)
    {
        _remoteIp = remoteIp;
    }

    void ConnectionState::setRemotePort(int remotePort)
    {
        _remotePort = remotePort;
    }
} // namespace ix
