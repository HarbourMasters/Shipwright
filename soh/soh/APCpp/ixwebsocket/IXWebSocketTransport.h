/*
 *  IXWebSocketTransport.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2018 Machine Zone, Inc. All rights reserved.
 */

#pragma once

//
// Adapted from https://github.com/dhbaird/easywsclient
//

#include "IXCancellationRequest.h"
#include "IXProgressCallback.h"
#include "IXSocketTLSOptions.h"
#include "IXWebSocketCloseConstants.h"
#include "IXWebSocketHandshake.h"
#include "IXWebSocketHttpHeaders.h"
#include "IXWebSocketPerMessageDeflate.h"
#include "IXWebSocketPerMessageDeflateOptions.h"
#include "IXWebSocketSendData.h"
#include "IXWebSocketSendInfo.h"
#include <atomic>
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace ix
{
    class Socket;

    enum class SendMessageKind
    {
        Text,
        Binary,
        Ping
    };

    class WebSocketTransport
    {
    public:
        enum class ReadyState
        {
            CLOSING,
            CLOSED,
            CONNECTING,
            OPEN
        };

        enum class MessageKind
        {
            MSG_TEXT,
            MSG_BINARY,
            PING,
            PONG,
            FRAGMENT
        };

        enum class PollResult
        {
            Succeeded,
            AbnormalClose,
            CannotFlushSendBuffer
        };

        using OnMessageCallback =
            std::function<void(const std::string&, size_t, bool, MessageKind)>;
        using OnCloseCallback = std::function<void(uint16_t, const std::string&, size_t, bool)>;

        WebSocketTransport();
        ~WebSocketTransport();

        void configure(const WebSocketPerMessageDeflateOptions& perMessageDeflateOptions,
                       const SocketTLSOptions& socketTLSOptions,
                       bool enablePong,
                       int pingIntervalSecs);

        // Client
        WebSocketInitResult connectToUrl(const std::string& url,
                                         const WebSocketHttpHeaders& headers,
                                         int timeoutSecs);

        // Server
        WebSocketInitResult connectToSocket(std::unique_ptr<Socket> socket,
                                            int timeoutSecs,
                                            bool enablePerMessageDeflate,
                                            HttpRequestPtr request = nullptr);

        PollResult poll();
        WebSocketSendInfo sendBinary(const IXWebSocketSendData& message,
                                     const OnProgressCallback& onProgressCallback);
        WebSocketSendInfo sendText(const IXWebSocketSendData& message,
                                   const OnProgressCallback& onProgressCallback);
        WebSocketSendInfo sendPing(const IXWebSocketSendData& message);

        void close(uint16_t code = WebSocketCloseConstants::kNormalClosureCode,
                   const std::string& reason = WebSocketCloseConstants::kNormalClosureMessage,
                   size_t closeWireSize = 0,
                   bool remote = false);

        void closeSocket();

        ReadyState getReadyState() const;
        void setReadyState(ReadyState readyState);
        void setOnCloseCallback(const OnCloseCallback& onCloseCallback);
        void dispatch(PollResult pollResult, const OnMessageCallback& onMessageCallback);
        size_t bufferedAmount() const;

        // set ping heartbeat message
        void setPingMessage(const std::string& message, SendMessageKind pingType);

        // internal
        // send any type of ping packet, not only 'ping' type
        WebSocketSendInfo sendHeartBeat(SendMessageKind pingType);

    private:
        std::string _url;

        struct wsheader_type
        {
            unsigned header_size;
            bool fin;
            bool rsv1;
            bool rsv2;
            bool rsv3;
            bool mask;
            enum opcode_type
            {
                CONTINUATION = 0x0,
                TEXT_FRAME = 0x1,
                BINARY_FRAME = 0x2,
                CLOSE = 8,
                PING = 9,
                PONG = 0xa,
            } opcode;
            int N0;
            uint64_t N;
            uint8_t masking_key[4];
        };

        // Tells whether we should mask the data we send.
        // client should mask but server should not
        std::atomic<bool> _useMask;

        // Tells whether we should flush the send buffer before
        // saying that a send is complete. This is the mode for server code.
        std::atomic<bool> _blockingSend;

        // Buffer for reading from our socket. That buffer is never resized.
        std::vector<uint8_t> _readbuf;

        // Contains all messages that were fetched in the last socket read.
        // This could be a mix of control messages (Close, Ping, etc...) and
        // data messages. That buffer is resized
        std::vector<uint8_t> _rxbuf;

        // Contains all messages that are waiting to be sent
        std::vector<uint8_t> _txbuf;
        mutable std::mutex _txbufMutex;

        // Hold fragments for multi-fragments messages in a list. We support receiving very large
        // messages (tested messages up to 700M) and we cannot put them in a single
        // buffer that is resized, as this operation can be slow when a buffer has its
        // size increased 2 fold, while appending to a list has a fixed cost.
        std::list<std::string> _chunks;

        // Record the message kind (will be TEXT or BINARY) for a fragmented
        // message, present in the first chunk, since the final chunk will be a
        // CONTINUATION opcode and doesn't tell the full message kind
        MessageKind _fragmentedMessageKind;

        // Ditto for whether a message is compressed
        bool _receivedMessageCompressed;

        // Fragments are 32K long
        static constexpr size_t kChunkSize = 1 << 15;

        // Underlying TCP socket
        std::unique_ptr<Socket> _socket;
        std::mutex _socketMutex;

        // Hold the state of the connection (OPEN, CLOSED, etc...)
        std::atomic<ReadyState> _readyState;

        OnCloseCallback _onCloseCallback;
        std::string _closeReason;
        mutable std::mutex _closeReasonMutex;
        std::atomic<uint16_t> _closeCode;
        std::atomic<size_t> _closeWireSize;
        std::atomic<bool> _closeRemote;

        // Data used for Per Message Deflate compression (with zlib)
        WebSocketPerMessageDeflatePtr _perMessageDeflate;
        WebSocketPerMessageDeflateOptions _perMessageDeflateOptions;
        std::atomic<bool> _enablePerMessageDeflate;

        std::string _decompressedMessage;
        std::string _compressedMessage;

        // Used to control TLS connection behavior
        SocketTLSOptions _socketTLSOptions;

        // Used to cancel dns lookup + socket connect + http upgrade
        std::atomic<bool> _requestInitCancellation;

        mutable std::mutex _closingTimePointMutex;
        std::chrono::time_point<std::chrono::steady_clock> _closingTimePoint;
        static const int kClosingMaximumWaitingDelayInMs;

        // enable auto response to ping
        std::atomic<bool> _enablePong;
        static const bool kDefaultEnablePong;

        // Optional ping and pong timeout
        int _pingIntervalSecs;
        std::atomic<bool> _pongReceived;

        static const int kDefaultPingIntervalSecs;

        bool _setCustomMessage;
        std::string _kPingMessage;
        SendMessageKind _pingType;
        std::atomic<uint64_t> _pingCount;

        // We record when ping are being sent so that we can know when to send the next one
        mutable std::mutex _lastSendPingTimePointMutex;
        std::chrono::time_point<std::chrono::steady_clock> _lastSendPingTimePoint;

        // If this function returns true, it is time to send a new ping
        bool pingIntervalExceeded();
        void initTimePointsAfterConnect();

        // after calling close(), if no CLOSE frame answer is received back from the remote, we
        // should close the connexion
        bool closingDelayExceeded();

        void sendCloseFrame(uint16_t code, const std::string& reason);

        void closeSocketAndSwitchToClosedState(uint16_t code,
                                               const std::string& reason,
                                               size_t closeWireSize,
                                               bool remote);

        bool wakeUpFromPoll(uint64_t wakeUpCode);

        bool flushSendBuffer();
        bool sendOnSocket();
        bool receiveFromSocket();

        WebSocketSendInfo sendData(wsheader_type::opcode_type type,
                                   const IXWebSocketSendData& message,
                                   bool compress,
                                   const OnProgressCallback& onProgressCallback = nullptr);

        template<class Iterator>
        bool sendFragment(
            wsheader_type::opcode_type type, bool fin, Iterator begin, Iterator end, bool compress);

        void emitMessage(MessageKind messageKind,
                         const std::string& message,
                         bool compressedMessage,
                         const OnMessageCallback& onMessageCallback);

        bool isSendBufferEmpty() const;

        template<class Iterator>
        void appendToSendBuffer(const std::vector<uint8_t>& header,
                                Iterator begin,
                                Iterator end,
                                uint64_t message_size,
                                uint8_t masking_key[4]);

        unsigned getRandomUnsigned();
        void unmaskReceiveBuffer(const wsheader_type& ws);

        std::string getMergedChunks() const;

        void setCloseReason(const std::string& reason);
        const std::string& getCloseReason() const;
    };
} // namespace ix
