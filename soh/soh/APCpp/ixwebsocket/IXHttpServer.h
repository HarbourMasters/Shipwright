/*
 *  IXHttpServer.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include "IXHttp.h"
#include "IXWebSocket.h"
#include "IXWebSocketServer.h"
#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <utility> // pair

namespace ix
{
    class HttpServer final : public WebSocketServer
    {
    public:
        using OnConnectionCallback =
            std::function<HttpResponsePtr(HttpRequestPtr, std::shared_ptr<ConnectionState>)>;

        HttpServer(int port = SocketServer::kDefaultPort,
                   const std::string& host = SocketServer::kDefaultHost,
                   int backlog = SocketServer::kDefaultTcpBacklog,
                   size_t maxConnections = SocketServer::kDefaultMaxConnections,
                   int addressFamily = SocketServer::kDefaultAddressFamily,
                   int timeoutSecs = HttpServer::kDefaultTimeoutSecs,
                   int handshakeTimeoutSecs = WebSocketServer::kDefaultHandShakeTimeoutSecs);

        void setOnConnectionCallback(const OnConnectionCallback& callback);

        void makeRedirectServer(const std::string& redirectUrl);

        void makeDebugServer();

        int getTimeoutSecs();

    private:
        // Member variables
        OnConnectionCallback _onConnectionCallback;

        const static int kDefaultTimeoutSecs;
        int _timeoutSecs;

        // Methods
        virtual void handleConnection(std::unique_ptr<Socket>,
                                      std::shared_ptr<ConnectionState> connectionState) final;

        void setDefaultConnectionCallback();
    };
} // namespace ix
