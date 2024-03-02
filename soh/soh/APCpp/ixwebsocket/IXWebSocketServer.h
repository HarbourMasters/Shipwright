/*
 *  IXWebSocketServer.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include "IXSocketServer.h"
#include "IXWebSocket.h"
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <utility> // pair

namespace ix
{
    class WebSocketServer : public SocketServer
    {
    public:
        using OnConnectionCallback =
            std::function<void(std::weak_ptr<WebSocket>, std::shared_ptr<ConnectionState>)>;

        using OnClientMessageCallback = std::function<void(
            std::shared_ptr<ConnectionState>, WebSocket&, const WebSocketMessagePtr&)>;

        WebSocketServer(int port = SocketServer::kDefaultPort,
                        const std::string& host = SocketServer::kDefaultHost,
                        int backlog = SocketServer::kDefaultTcpBacklog,
                        size_t maxConnections = SocketServer::kDefaultMaxConnections,
                        int handshakeTimeoutSecs = WebSocketServer::kDefaultHandShakeTimeoutSecs,
                        int addressFamily = SocketServer::kDefaultAddressFamily);
        virtual ~WebSocketServer();
        virtual void stop() final;

        void enablePong();
        void disablePong();
        void disablePerMessageDeflate();

        void setOnConnectionCallback(const OnConnectionCallback& callback);
        void setOnClientMessageCallback(const OnClientMessageCallback& callback);

        // Get all the connected clients
        std::set<std::shared_ptr<WebSocket>> getClients();

        void makeBroadcastServer();
        bool listenAndStart();

        const static int kDefaultHandShakeTimeoutSecs;

        int getHandshakeTimeoutSecs();
        bool isPongEnabled();
        bool isPerMessageDeflateEnabled();

    private:
        // Member variables
        int _handshakeTimeoutSecs;
        bool _enablePong;
        bool _enablePerMessageDeflate;

        OnConnectionCallback _onConnectionCallback;
        OnClientMessageCallback _onClientMessageCallback;

        std::mutex _clientsMutex;
        std::set<std::shared_ptr<WebSocket>> _clients;

        const static bool kDefaultEnablePong;

        // Methods
        virtual void handleConnection(std::unique_ptr<Socket> socket,
                                      std::shared_ptr<ConnectionState> connectionState);
        virtual size_t getConnectedClientsCount() final;

    protected:
        void handleUpgrade(std::unique_ptr<Socket> socket,
                           std::shared_ptr<ConnectionState> connectionState,
                           HttpRequestPtr request = nullptr);
    };
} // namespace ix
