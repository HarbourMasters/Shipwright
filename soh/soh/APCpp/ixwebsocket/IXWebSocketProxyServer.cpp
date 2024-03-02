/*
 *  IXWebSocketProxyServer.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 Machine Zone, Inc. All rights reserved.
 */

#include "IXWebSocketProxyServer.h"

#include "IXWebSocketServer.h"
#include <sstream>

namespace ix
{
    class ProxyConnectionState : public ix::ConnectionState
    {
    public:
        ProxyConnectionState()
            : _connected(false)
        {
        }

        ix::WebSocket& webSocket()
        {
            return _serverWebSocket;
        }

        bool isConnected()
        {
            return _connected;
        }

        void setConnected()
        {
            _connected = true;
        }

    private:
        ix::WebSocket _serverWebSocket;
        bool _connected;
    };

    int websocket_proxy_server_main(int port,
                                    const std::string& hostname,
                                    const ix::SocketTLSOptions& tlsOptions,
                                    const std::string& remoteUrl,
                                    const RemoteUrlsMapping& remoteUrlsMapping,
                                    bool /*verbose*/)
    {
        ix::WebSocketServer server(port, hostname);
        server.setTLSOptions(tlsOptions);

        auto factory = []() -> std::shared_ptr<ix::ConnectionState> {
            return std::make_shared<ProxyConnectionState>();
        };
        server.setConnectionStateFactory(factory);

        server.setOnConnectionCallback(
            [remoteUrl, remoteUrlsMapping](std::weak_ptr<ix::WebSocket> webSocket,
                                           std::shared_ptr<ConnectionState> connectionState) {
                auto state = std::dynamic_pointer_cast<ProxyConnectionState>(connectionState);
                auto remoteIp = connectionState->getRemoteIp();

                // Server connection
                state->webSocket().setOnMessageCallback(
                    [webSocket, state, remoteIp](const WebSocketMessagePtr& msg) {
                        if (msg->type == ix::WebSocketMessageType::Close)
                        {
                            state->setTerminated();
                        }
                        else if (msg->type == ix::WebSocketMessageType::Message)
                        {
                            auto ws = webSocket.lock();
                            if (ws)
                            {
                                ws->send(msg->str, msg->binary);
                            }
                        }
                    });

                // Client connection
                auto ws = webSocket.lock();
                if (ws)
                {
                    ws->setOnMessageCallback([state, remoteUrl, remoteUrlsMapping](
                                                 const WebSocketMessagePtr& msg) {
                        if (msg->type == ix::WebSocketMessageType::Open)
                        {
                            // Connect to the 'real' server
                            std::string url(remoteUrl);

                            // maybe we want a different url based on the mapping
                            std::string host = msg->openInfo.headers["Host"];
                            auto it = remoteUrlsMapping.find(host);
                            if (it != remoteUrlsMapping.end())
                            {
                                url = it->second;
                            }

                            // append the uri to form the full url
                            // (say ws://localhost:1234/foo/?bar=baz)
                            url += msg->openInfo.uri;

                            state->webSocket().setUrl(url);
                            state->webSocket().disableAutomaticReconnection();
                            state->webSocket().start();

                            // we should sleep here for a bit until we've established the
                            // connection with the remote server
                            while (state->webSocket().getReadyState() != ReadyState::Open)
                            {
                                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                            }
                        }
                        else if (msg->type == ix::WebSocketMessageType::Close)
                        {
                            state->webSocket().close(msg->closeInfo.code, msg->closeInfo.reason);
                        }
                        else if (msg->type == ix::WebSocketMessageType::Message)
                        {
                            state->webSocket().send(msg->str, msg->binary);
                        }
                    });
                }
            });

        auto res = server.listen();
        if (!res.first)
        {
            return 1;
        }

        server.start();
        server.wait();

        return 0;
    }
} // namespace ix
