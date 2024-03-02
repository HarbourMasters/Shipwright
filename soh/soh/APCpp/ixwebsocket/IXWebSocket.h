/*
 *  IXWebSocket.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2018 Machine Zone, Inc. All rights reserved.
 *
 *  WebSocket RFC
 *  https://tools.ietf.org/html/rfc6455
 */

#pragma once

#include "IXProgressCallback.h"
#include "IXSocketTLSOptions.h"
#include "IXWebSocketCloseConstants.h"
#include "IXWebSocketErrorInfo.h"
#include "IXWebSocketHttpHeaders.h"
#include "IXWebSocketMessage.h"
#include "IXWebSocketPerMessageDeflateOptions.h"
#include "IXWebSocketSendData.h"
#include "IXWebSocketSendInfo.h"
#include "IXWebSocketTransport.h"
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>

namespace ix
{
    // https://developer.mozilla.org/en-US/docs/Web/API/WebSocket#Ready_state_constants
    enum class ReadyState
    {
        Connecting = 0,
        Open = 1,
        Closing = 2,
        Closed = 3
    };

    using OnMessageCallback = std::function<void(const WebSocketMessagePtr&)>;

    using OnTrafficTrackerCallback = std::function<void(size_t size, bool incoming)>;

    class WebSocket
    {
    public:
        WebSocket();
        ~WebSocket();

        void setUrl(const std::string& url);

        // send extra headers in client handshake request
        void setExtraHeaders(const WebSocketHttpHeaders& headers);
        void setPerMessageDeflateOptions(
            const WebSocketPerMessageDeflateOptions& perMessageDeflateOptions);
        void setTLSOptions(const SocketTLSOptions& socketTLSOptions);
        void setPingMessage(const std::string& sendMessage,
                            SendMessageKind pingType = SendMessageKind::Ping);
        void setPingInterval(int pingIntervalSecs);
        void enablePong();
        void disablePong();
        void enablePerMessageDeflate();
        void disablePerMessageDeflate();
        void addSubProtocol(const std::string& subProtocol);
        void setHandshakeTimeout(int handshakeTimeoutSecs);

        // Run asynchronously, by calling start and stop.
        void start();

        // stop is synchronous
        void stop(uint16_t code = WebSocketCloseConstants::kNormalClosureCode,
                  const std::string& reason = WebSocketCloseConstants::kNormalClosureMessage);

        // Run in blocking mode, by connecting first manually, and then calling run.
        WebSocketInitResult connect(int timeoutSecs);
        void run();

        // send is in text mode by default
        WebSocketSendInfo send(const std::string& data,
                               bool binary = false,
                               const OnProgressCallback& onProgressCallback = nullptr);
        WebSocketSendInfo sendBinary(const std::string& data,
                                     const OnProgressCallback& onProgressCallback = nullptr);
        WebSocketSendInfo sendBinary(const IXWebSocketSendData& data,
                                     const OnProgressCallback& onProgressCallback = nullptr);
        // does not check for valid UTF-8 characters. Caller must check that.
        WebSocketSendInfo sendUtf8Text(const std::string& text,
                                       const OnProgressCallback& onProgressCallback = nullptr);
        // does not check for valid UTF-8 characters. Caller must check that.
        WebSocketSendInfo sendUtf8Text(const IXWebSocketSendData& text,
                                       const OnProgressCallback& onProgressCallback = nullptr);
        WebSocketSendInfo sendText(const std::string& text,
                                   const OnProgressCallback& onProgressCallback = nullptr);
        WebSocketSendInfo ping(const std::string& text,SendMessageKind pingType = SendMessageKind::Ping);

        void close(uint16_t code = WebSocketCloseConstants::kNormalClosureCode,
                   const std::string& reason = WebSocketCloseConstants::kNormalClosureMessage);

        void setOnMessageCallback(const OnMessageCallback& callback);
        bool isOnMessageCallbackRegistered() const;
        static void setTrafficTrackerCallback(const OnTrafficTrackerCallback& callback);
        static void resetTrafficTrackerCallback();

        ReadyState getReadyState() const;
        static std::string readyStateToString(ReadyState readyState);

        const std::string getUrl() const;
        const WebSocketPerMessageDeflateOptions getPerMessageDeflateOptions() const;
        const std::string getPingMessage() const;
        int getPingInterval() const;
        size_t bufferedAmount() const;

        void enableAutomaticReconnection();
        void disableAutomaticReconnection();
        bool isAutomaticReconnectionEnabled() const;
        void setMaxWaitBetweenReconnectionRetries(uint32_t maxWaitBetweenReconnectionRetries);
        void setMinWaitBetweenReconnectionRetries(uint32_t minWaitBetweenReconnectionRetries);
        uint32_t getMaxWaitBetweenReconnectionRetries() const;
        uint32_t getMinWaitBetweenReconnectionRetries() const;
        const std::vector<std::string>& getSubProtocols();

    private:
        WebSocketSendInfo sendMessage(const IXWebSocketSendData& message,
                                      SendMessageKind sendMessageKind,
                                      const OnProgressCallback& callback = nullptr);

        bool isConnected() const;
        bool isClosing() const;
        void checkConnection(bool firstConnectionAttempt);
        static void invokeTrafficTrackerCallback(size_t size, bool incoming);

        // Server
        WebSocketInitResult connectToSocket(std::unique_ptr<Socket>,
                                            int timeoutSecs,
                                            bool enablePerMessageDeflate,
                                            HttpRequestPtr request = nullptr);

        WebSocketTransport _ws;

        std::string _url;
        WebSocketHttpHeaders _extraHeaders;

        WebSocketPerMessageDeflateOptions _perMessageDeflateOptions;

        SocketTLSOptions _socketTLSOptions;

        mutable std::mutex _configMutex; // protect all config variables access

        OnMessageCallback _onMessageCallback;
        static OnTrafficTrackerCallback _onTrafficTrackerCallback;

        std::atomic<bool> _stop;
        std::thread _thread;
        std::mutex _writeMutex;

        // Automatic reconnection
        std::atomic<bool> _automaticReconnection;
        static const uint32_t kDefaultMaxWaitBetweenReconnectionRetries;
        static const uint32_t kDefaultMinWaitBetweenReconnectionRetries;
        uint32_t _maxWaitBetweenReconnectionRetries;
        uint32_t _minWaitBetweenReconnectionRetries;

        // Make the sleeping in the automatic reconnection cancellable
        std::mutex _sleepMutex;
        std::condition_variable _sleepCondition;

        std::atomic<int> _handshakeTimeoutSecs;
        static const int kDefaultHandShakeTimeoutSecs;

        // enable or disable PONG frame response to received PING frame
        bool _enablePong;
        static const bool kDefaultEnablePong;

        // Optional ping and pong timeout
        int _pingIntervalSecs;
        int _pingTimeoutSecs;
        std::string _pingMessage;
        SendMessageKind _pingType;
        static const int kDefaultPingIntervalSecs;
        static const int kDefaultPingTimeoutSecs;

        // Subprotocols
        std::vector<std::string> _subProtocols;

        friend class WebSocketServer;
    };
} // namespace ix
