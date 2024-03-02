/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2012, 2013 <dhbaird@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 *  IXWebSocketTransport.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2019 Machine Zone, Inc. All rights reserved.
 */

//
// Adapted from https://github.com/dhbaird/easywsclient
//

#include "IXWebSocketTransport.h"

#include "IXSocketFactory.h"
#include "IXSocketTLSOptions.h"
#include "IXUniquePtr.h"
#include "IXUrlParser.h"
#include "IXUtf8Validator.h"
#include "IXWebSocketHandshake.h"
#include "IXWebSocketHttpHeaders.h"
#include <chrono>
#include <cstdarg>
#include <cstdlib>
#include <sstream>
#include <string.h>
#include <string>
#include <thread>
#include <vector>


namespace ix
{
    const int WebSocketTransport::kDefaultPingIntervalSecs(-1);
    const bool WebSocketTransport::kDefaultEnablePong(true);
    const int WebSocketTransport::kClosingMaximumWaitingDelayInMs(300);
    constexpr size_t WebSocketTransport::kChunkSize;

    WebSocketTransport::WebSocketTransport()
        : _useMask(true)
        , _blockingSend(false)
        , _receivedMessageCompressed(false)
        , _readyState(ReadyState::CLOSED)
        , _closeCode(WebSocketCloseConstants::kInternalErrorCode)
        , _closeWireSize(0)
        , _closeRemote(false)
        , _enablePerMessageDeflate(false)
        , _requestInitCancellation(false)
        , _closingTimePoint(std::chrono::steady_clock::now())
        , _enablePong(kDefaultEnablePong)
        , _pingIntervalSecs(kDefaultPingIntervalSecs)
        , _pongReceived(false)
        , _setCustomMessage(false)
        , _kPingMessage("ixwebsocket::heartbeat")
        , _pingType(SendMessageKind::Ping)
        , _pingCount(0)
        , _lastSendPingTimePoint(std::chrono::steady_clock::now())
    {
        setCloseReason(WebSocketCloseConstants::kInternalErrorMessage);
        _readbuf.resize(kChunkSize);
    }

    WebSocketTransport::~WebSocketTransport()
    {
        ;
    }

    void WebSocketTransport::configure(
        const WebSocketPerMessageDeflateOptions& perMessageDeflateOptions,
        const SocketTLSOptions& socketTLSOptions,
        bool enablePong,
        int pingIntervalSecs)
    {
        _perMessageDeflateOptions = perMessageDeflateOptions;
        _enablePerMessageDeflate = _perMessageDeflateOptions.enabled();
        _socketTLSOptions = socketTLSOptions;
        _enablePong = enablePong;
        _pingIntervalSecs = pingIntervalSecs;
    }

    // Client
    WebSocketInitResult WebSocketTransport::connectToUrl(const std::string& url,
                                                         const WebSocketHttpHeaders& headers,
                                                         int timeoutSecs)
    {
        std::lock_guard<std::mutex> lock(_socketMutex);

        std::string protocol, host, path, query;
        int port;
        std::string remoteUrl(url);

        WebSocketInitResult result;
        const int maxRedirections = 10;

        for (int i = 0; i < maxRedirections; ++i)
        {
            if (!UrlParser::parse(remoteUrl, protocol, host, path, query, port))
            {
                std::stringstream ss;
                ss << "Could not parse url: '" << url << "'";
                return WebSocketInitResult(false, 0, ss.str());
            }

            std::string errorMsg;
            bool tls = protocol == "wss";
            _socket = createSocket(tls, -1, errorMsg, _socketTLSOptions);
            _perMessageDeflate = ix::make_unique<WebSocketPerMessageDeflate>();

            if (!_socket)
            {
                return WebSocketInitResult(false, 0, errorMsg);
            }

            WebSocketHandshake webSocketHandshake(_requestInitCancellation,
                                                  _socket,
                                                  _perMessageDeflate,
                                                  _perMessageDeflateOptions,
                                                  _enablePerMessageDeflate);

            result = webSocketHandshake.clientHandshake(
                remoteUrl, headers, protocol, host, path, port, timeoutSecs);

            if (result.http_status >= 300 && result.http_status < 400)
            {
                auto it = result.headers.find("Location");
                if (it == result.headers.end())
                {
                    std::stringstream ss;
                    ss << "Missing Location Header for HTTP Redirect response. "
                       << "Rejecting connection to " << url << ", status: " << result.http_status;
                    result.errorStr = ss.str();
                    break;
                }

                remoteUrl = it->second;
                continue;
            }

            if (result.success)
            {
                setReadyState(ReadyState::OPEN);
            }
            return result;
        }

        return result;
    }

    // Server
    WebSocketInitResult WebSocketTransport::connectToSocket(std::unique_ptr<Socket> socket,
                                                            int timeoutSecs,
                                                            bool enablePerMessageDeflate,
                                                            HttpRequestPtr request)
    {
        std::lock_guard<std::mutex> lock(_socketMutex);

        // Server should not mask the data it sends to the client
        _useMask = false;
        _blockingSend = true;

        _socket = std::move(socket);
        _perMessageDeflate = ix::make_unique<WebSocketPerMessageDeflate>();

        WebSocketHandshake webSocketHandshake(_requestInitCancellation,
                                              _socket,
                                              _perMessageDeflate,
                                              _perMessageDeflateOptions,
                                              _enablePerMessageDeflate);

        auto result =
            webSocketHandshake.serverHandshake(timeoutSecs, enablePerMessageDeflate, request);
        if (result.success)
        {
            setReadyState(ReadyState::OPEN);
        }
        return result;
    }

    WebSocketTransport::ReadyState WebSocketTransport::getReadyState() const
    {
        return _readyState;
    }

    void WebSocketTransport::setReadyState(ReadyState readyState)
    {
        // No state change, return
        if (_readyState == readyState) return;

        if (readyState == ReadyState::CLOSED)
        {
            if (_onCloseCallback)
            {
                _onCloseCallback(_closeCode, getCloseReason(), _closeWireSize, _closeRemote);
            }
            setCloseReason(WebSocketCloseConstants::kInternalErrorMessage);
            _closeCode = WebSocketCloseConstants::kInternalErrorCode;
            _closeWireSize = 0;
            _closeRemote = false;
        }
        else if (readyState == ReadyState::OPEN)
        {
            initTimePointsAfterConnect();
            _pongReceived = false;
        }

        _readyState = readyState;
    }

    void WebSocketTransport::setOnCloseCallback(const OnCloseCallback& onCloseCallback)
    {
        _onCloseCallback = onCloseCallback;
    }

    void WebSocketTransport::initTimePointsAfterConnect()
    {
        {
            std::lock_guard<std::mutex> lock(_lastSendPingTimePointMutex);
            _lastSendPingTimePoint = std::chrono::steady_clock::now();
        }
    }

    // Only consider send PING time points for that computation.
    bool WebSocketTransport::pingIntervalExceeded()
    {
        if (_pingIntervalSecs <= 0) return false;

        std::lock_guard<std::mutex> lock(_lastSendPingTimePointMutex);
        auto now = std::chrono::steady_clock::now();
        return now - _lastSendPingTimePoint > std::chrono::seconds(_pingIntervalSecs);
    }

    void WebSocketTransport::setPingMessage(const std::string& message, SendMessageKind pingType)
    {
        _setCustomMessage = true;
        _kPingMessage = message;
        _pingType = pingType;
    }

    WebSocketSendInfo WebSocketTransport::sendHeartBeat(SendMessageKind pingMessage)
    {
        _pongReceived = false;
        std::stringstream ss;

        ss << _kPingMessage;
        if (!_setCustomMessage)
        {
            ss << "::" << _pingIntervalSecs << "s"
               << "::" << _pingCount++;
        }
        if (pingMessage == SendMessageKind::Ping)
        {
            return sendPing(ss.str());
        }
        else if (pingMessage == SendMessageKind::Binary)
        {
            WebSocketSendInfo info = sendBinary(ss.str(), nullptr);
            if (info.success)
            {
                std::lock_guard<std::mutex> lck(_lastSendPingTimePointMutex);
                _lastSendPingTimePoint = std::chrono::steady_clock::now();
            }
            return info;
        }
        else if (pingMessage == SendMessageKind::Text)
        {
            WebSocketSendInfo info = sendText(ss.str(), nullptr);
            if (info.success)
            {
                std::lock_guard<std::mutex> lck(_lastSendPingTimePointMutex);
                _lastSendPingTimePoint = std::chrono::steady_clock::now();
            }
            return info;
        }

        // unknow type ping message
        return {};
    }

    bool WebSocketTransport::closingDelayExceeded()
    {
        std::lock_guard<std::mutex> lock(_closingTimePointMutex);
        auto now = std::chrono::steady_clock::now();
        return now - _closingTimePoint > std::chrono::milliseconds(kClosingMaximumWaitingDelayInMs);
    }

    WebSocketTransport::PollResult WebSocketTransport::poll()
    {
        if (_readyState == ReadyState::OPEN)
        {
            if (pingIntervalExceeded())
            {
                // If it is not a 'ping' message of ping type, there is no need to judge whether
                // pong will receive it
                if (_pingType == SendMessageKind::Ping && !_pongReceived)
                {
                    // ping response (PONG) exceeds the maximum delay, close the connection
                    close(WebSocketCloseConstants::kInternalErrorCode,
                          WebSocketCloseConstants::kPingTimeoutMessage);
                }
                else
                {
                    sendHeartBeat(_pingType);
                }
            }
        }

        // No timeout if state is not OPEN, otherwise computed
        // pingIntervalOrTimeoutGCD (equals to -1 if no ping and no ping timeout are set)
        int lastingTimeoutDelayInMs = (_readyState != ReadyState::OPEN) ? 0 : _pingIntervalSecs;

        if (_pingIntervalSecs > 0)
        {
            // compute lasting delay to wait for next ping / timeout, if at least one set
            auto now = std::chrono::steady_clock::now();
            int timeSinceLastPingMs = (int) std::chrono::duration_cast<std::chrono::milliseconds>(
                                          now - _lastSendPingTimePoint)
                                          .count();
            lastingTimeoutDelayInMs = (1000 * _pingIntervalSecs) - timeSinceLastPingMs;
        }

        // The platform may not have select interrupt capabilities, so wait with a small timeout
        if (lastingTimeoutDelayInMs <= 0 && !_socket->isWakeUpFromPollSupported())
        {
            lastingTimeoutDelayInMs = 20;
        }

        // If we are requesting a cancellation, pass in a positive and small timeout
        // to never poll forever without a timeout.
        if (_requestInitCancellation)
        {
            lastingTimeoutDelayInMs = 100;
        }

        // poll the socket
        PollResultType pollResult = _socket->isReadyToRead(lastingTimeoutDelayInMs);

        // Make sure we send all the buffered data
        // there can be a lot of it for large messages.
        if (pollResult == PollResultType::SendRequest)
        {
            if (!flushSendBuffer())
            {
                return PollResult::CannotFlushSendBuffer;
            }
        }
        else if (pollResult == PollResultType::ReadyForRead)
        {
            if (!receiveFromSocket())
            {
                return PollResult::AbnormalClose;
            }
        }
        else if (pollResult == PollResultType::Error)
        {
            closeSocket();
        }
        else if (pollResult == PollResultType::CloseRequest)
        {
            closeSocket();
        }

        if (_readyState == ReadyState::CLOSING && closingDelayExceeded())
        {
            _rxbuf.clear();
            // close code and reason were set when calling close()
            closeSocket();
            setReadyState(ReadyState::CLOSED);
        }

        return PollResult::Succeeded;
    }

    bool WebSocketTransport::isSendBufferEmpty() const
    {
        std::lock_guard<std::mutex> lock(_txbufMutex);
        return _txbuf.empty();
    }

    template<class Iterator>
    void WebSocketTransport::appendToSendBuffer(const std::vector<uint8_t>& header,
                                                Iterator begin,
                                                Iterator end,
                                                uint64_t message_size,
                                                uint8_t masking_key[4])
    {
        std::lock_guard<std::mutex> lock(_txbufMutex);

        _txbuf.insert(_txbuf.end(), header.begin(), header.end());
        _txbuf.insert(_txbuf.end(), begin, end);

        if (_useMask)
        {
            for (size_t i = 0; i != (size_t) message_size; ++i)
            {
                *(_txbuf.end() - (size_t) message_size + i) ^= masking_key[i & 0x3];
            }
        }
    }

    void WebSocketTransport::unmaskReceiveBuffer(const wsheader_type& ws)
    {
        if (ws.mask)
        {
            for (size_t j = 0; j != ws.N; ++j)
            {
                _rxbuf[j + ws.header_size] ^= ws.masking_key[j & 0x3];
            }
        }
    }

    //
    // http://tools.ietf.org/html/rfc6455#section-5.2  Base Framing Protocol
    //
    //  0                   1                   2                   3
    //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    // +-+-+-+-+-------+-+-------------+-------------------------------+
    // |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
    // |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
    // |N|V|V|V|       |S|             |   (if payload len==126/127)   |
    // | |1|2|3|       |K|             |                               |
    // +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
    // |     Extended payload length continued, if payload len == 127  |
    // + - - - - - - - - - - - - - - - +-------------------------------+
    // |                               |Masking-key, if MASK set to 1  |
    // +-------------------------------+-------------------------------+
    // | Masking-key (continued)       |          Payload Data         |
    // +-------------------------------- - - - - - - - - - - - - - - - +
    // :                     Payload Data continued ...                :
    // + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
    // |                     Payload Data continued ...                |
    // +---------------------------------------------------------------+
    //
    void WebSocketTransport::dispatch(WebSocketTransport::PollResult pollResult,
                                      const OnMessageCallback& onMessageCallback)
    {
        while (true)
        {
            wsheader_type ws;
            if (_rxbuf.size() < 2) break;                /* Need at least 2 */
            const uint8_t* data = (uint8_t*) &_rxbuf[0]; // peek, but don't consume
            ws.fin = (data[0] & 0x80) == 0x80;
            ws.rsv1 = (data[0] & 0x40) == 0x40;
            ws.rsv2 = (data[0] & 0x20) == 0x20;
            ws.rsv3 = (data[0] & 0x10) == 0x10;
            ws.opcode = (wsheader_type::opcode_type)(data[0] & 0x0f);
            ws.mask = (data[1] & 0x80) == 0x80;
            ws.N0 = (data[1] & 0x7f);
            ws.header_size =
                2 + (ws.N0 == 126 ? 2 : 0) + (ws.N0 == 127 ? 8 : 0) + (ws.mask ? 4 : 0);
            if (_rxbuf.size() < ws.header_size) break; /* Need: ws.header_size - _rxbuf.size() */

            if ((ws.rsv1 && !_enablePerMessageDeflate) || ws.rsv2 || ws.rsv3)
            {
                close(WebSocketCloseConstants::kProtocolErrorCode,
                      WebSocketCloseConstants::kProtocolErrorReservedBitUsed,
                      _rxbuf.size());
                return;
            }

            //
            // Calculate payload length:
            // 0-125 mean the payload is that long.
            // 126 means that the following two bytes indicate the length,
            // 127 means the next 8 bytes indicate the length.
            //
            int i = 0;
            if (ws.N0 < 126)
            {
                ws.N = ws.N0;
                i = 2;
            }
            else if (ws.N0 == 126)
            {
                ws.N = 0;
                ws.N |= ((uint64_t) data[2]) << 8;
                ws.N |= ((uint64_t) data[3]) << 0;
                i = 4;
            }
            else if (ws.N0 == 127)
            {
                ws.N = 0;
                ws.N |= ((uint64_t) data[2]) << 56;
                ws.N |= ((uint64_t) data[3]) << 48;
                ws.N |= ((uint64_t) data[4]) << 40;
                ws.N |= ((uint64_t) data[5]) << 32;
                ws.N |= ((uint64_t) data[6]) << 24;
                ws.N |= ((uint64_t) data[7]) << 16;
                ws.N |= ((uint64_t) data[8]) << 8;
                ws.N |= ((uint64_t) data[9]) << 0;
                i = 10;
            }
            else
            {
                // invalid payload length according to the spec. bail out
                return;
            }

            if (ws.mask)
            {
                ws.masking_key[0] = ((uint8_t) data[i + 0]) << 0;
                ws.masking_key[1] = ((uint8_t) data[i + 1]) << 0;
                ws.masking_key[2] = ((uint8_t) data[i + 2]) << 0;
                ws.masking_key[3] = ((uint8_t) data[i + 3]) << 0;
            }
            else
            {
                ws.masking_key[0] = 0;
                ws.masking_key[1] = 0;
                ws.masking_key[2] = 0;
                ws.masking_key[3] = 0;
            }

            // Prevent integer overflow in the next conditional
            const uint64_t maxFrameSize(1ULL << 63);
            if (ws.N > maxFrameSize)
            {
                return;
            }

            if (_rxbuf.size() < ws.header_size + ws.N)
            {
                return; /* Need: ws.header_size+ws.N - _rxbuf.size() */
            }

            if (!ws.fin && (ws.opcode == wsheader_type::PING || ws.opcode == wsheader_type::PONG ||
                            ws.opcode == wsheader_type::CLOSE))
            {
                // Control messages should not be fragmented
                close(WebSocketCloseConstants::kProtocolErrorCode,
                      WebSocketCloseConstants::kProtocolErrorCodeControlMessageFragmented);
                return;
            }

            unmaskReceiveBuffer(ws);
            std::string frameData(_rxbuf.begin() + ws.header_size,
                                  _rxbuf.begin() + ws.header_size + (size_t) ws.N);

            // We got a whole message, now do something with it:
            if (ws.opcode == wsheader_type::TEXT_FRAME ||
                ws.opcode == wsheader_type::BINARY_FRAME ||
                ws.opcode == wsheader_type::CONTINUATION)
            {
                if (ws.opcode != wsheader_type::CONTINUATION)
                {
                    _fragmentedMessageKind = (ws.opcode == wsheader_type::TEXT_FRAME)
                                                 ? MessageKind::MSG_TEXT
                                                 : MessageKind::MSG_BINARY;

                    _receivedMessageCompressed = _enablePerMessageDeflate && ws.rsv1;

                    // Continuation message needs to follow a non-fin TEXT or BINARY message
                    if (!_chunks.empty())
                    {
                        close(WebSocketCloseConstants::kProtocolErrorCode,
                              WebSocketCloseConstants::kProtocolErrorCodeDataOpcodeOutOfSequence);
                    }
                }
                else if (_chunks.empty())
                {
                    // Continuation message need to follow a non-fin TEXT or BINARY message
                    close(
                        WebSocketCloseConstants::kProtocolErrorCode,
                        WebSocketCloseConstants::kProtocolErrorCodeContinuationOpCodeOutOfSequence);
                }

                //
                // Usual case. Small unfragmented messages
                //
                if (ws.fin && _chunks.empty())
                {
                    emitMessage(_fragmentedMessageKind,
                                frameData,
                                _receivedMessageCompressed,
                                onMessageCallback);

                    _receivedMessageCompressed = false;
                }
                else
                {
                    //
                    // Add intermediary message to our chunk list.
                    // We use a chunk list instead of a big buffer because resizing
                    // large buffer can be very costly when we need to re-allocate
                    // the internal buffer which is slow and can let the internal OS
                    // receive buffer fill out.
                    //
                    _chunks.emplace_back(frameData);

                    if (ws.fin)
                    {
                        emitMessage(_fragmentedMessageKind,
                                    getMergedChunks(),
                                    _receivedMessageCompressed,
                                    onMessageCallback);

                        _chunks.clear();
                        _receivedMessageCompressed = false;
                    }
                    else
                    {
                        emitMessage(MessageKind::FRAGMENT, std::string(), false, onMessageCallback);
                    }
                }
            }
            else if (ws.opcode == wsheader_type::PING)
            {
                // too large
                if (frameData.size() > 125)
                {
                    // Unexpected frame type
                    close(WebSocketCloseConstants::kProtocolErrorCode,
                          WebSocketCloseConstants::kProtocolErrorPingPayloadOversized);
                    return;
                }

                if (_enablePong)
                {
                    // Reply back right away
                    bool compress = false;
                    sendData(wsheader_type::PONG, frameData, compress);
                }

                emitMessage(MessageKind::PING, frameData, false, onMessageCallback);
            }
            else if (ws.opcode == wsheader_type::PONG)
            {
                _pongReceived = true;
                emitMessage(MessageKind::PONG, frameData, false, onMessageCallback);
            }
            else if (ws.opcode == wsheader_type::CLOSE)
            {
                std::string reason;
                uint16_t code = 0;

                if (ws.N >= 2)
                {
                    // Extract the close code first, available as the first 2 bytes
                    code |= ((uint64_t) _rxbuf[ws.header_size]) << 8;
                    code |= ((uint64_t) _rxbuf[ws.header_size + 1]) << 0;

                    // Get the reason.
                    if (ws.N > 2)
                    {
                        reason = frameData.substr(2, frameData.size());
                    }

                    // Validate that the reason is proper utf-8. Autobahn 7.5.1
                    if (!validateUtf8(reason))
                    {
                        code = WebSocketCloseConstants::kInvalidFramePayloadData;
                        reason = WebSocketCloseConstants::kInvalidFramePayloadDataMessage;
                    }

                    //
                    // Validate close codes. Autobahn 7.9.*
                    // 1014, 1015 are debattable. The firefox MSDN has a description for them.
                    // Full list of status code and status range is defined in the dedicated
                    // RFC section at https://tools.ietf.org/html/rfc6455#page-45
                    //
                    if (code < 1000 || code == 1004 || code == 1006 || (code > 1013 && code < 3000))
                    {
                        // build up an error message containing the bad error code
                        std::stringstream ss;
                        ss << WebSocketCloseConstants::kInvalidCloseCodeMessage << ": " << code;
                        reason = ss.str();

                        code = WebSocketCloseConstants::kProtocolErrorCode;
                    }
                }
                else
                {
                    // no close code received
                    code = WebSocketCloseConstants::kNoStatusCodeErrorCode;
                    reason = WebSocketCloseConstants::kNoStatusCodeErrorMessage;
                }

                // We receive a CLOSE frame from remote and are NOT the ones who triggered the close
                if (_readyState != ReadyState::CLOSING)
                {
                    // send back the CLOSE frame
                    setReadyState(ReadyState::CLOSING);
                    sendCloseFrame(code, reason);

                    wakeUpFromPoll(SelectInterrupt::kCloseRequest);

                    bool remote = true;
                    closeSocketAndSwitchToClosedState(code, reason, _rxbuf.size(), remote);
                }
                else
                {
                    // we got the CLOSE frame answer from our close, so we can close the connection
                    // if the code/reason are the same
                    bool identicalReason = _closeCode == code && getCloseReason() == reason;

                    if (identicalReason)
                    {
                        bool remote = false;
                        closeSocketAndSwitchToClosedState(code, reason, _rxbuf.size(), remote);
                    }
                }
            }
            else
            {
                // Unexpected frame type
                close(WebSocketCloseConstants::kProtocolErrorCode,
                      WebSocketCloseConstants::kProtocolErrorMessage,
                      _rxbuf.size());
            }

            // Erase the message that has been processed from the input/read buffer
            _rxbuf.erase(_rxbuf.begin(), _rxbuf.begin() + ws.header_size + (size_t) ws.N);
        }

        // if an abnormal closure was raised in poll, and nothing else triggered a CLOSED state in
        // the received and processed data then close the connection
        if (pollResult != PollResult::Succeeded)
        {
            _rxbuf.clear();

            // if we previously closed the connection (CLOSING state), then set state to CLOSED
            // (code/reason were set before)
            if (_readyState == ReadyState::CLOSING)
            {
                closeSocket();
                setReadyState(ReadyState::CLOSED);
            }
            // if we weren't closing, then close using abnormal close code and message
            else if (_readyState != ReadyState::CLOSED)
            {
                closeSocketAndSwitchToClosedState(WebSocketCloseConstants::kAbnormalCloseCode,
                                                  WebSocketCloseConstants::kAbnormalCloseMessage,
                                                  0,
                                                  false);
            }
        }
    }

    std::string WebSocketTransport::getMergedChunks() const
    {
        size_t length = 0;
        for (auto&& chunk : _chunks)
        {
            length += chunk.size();
        }

        std::string msg;
        msg.reserve(length);

        for (auto&& chunk : _chunks)
        {
            msg += chunk;
        }

        return msg;
    }

    void WebSocketTransport::emitMessage(MessageKind messageKind,
                                         const std::string& message,
                                         bool compressedMessage,
                                         const OnMessageCallback& onMessageCallback)
    {
        size_t wireSize = message.size();

        // When the RSV1 bit is 1 it means the message is compressed
        if (compressedMessage && messageKind != MessageKind::FRAGMENT)
        {
            bool success = _perMessageDeflate->decompress(message, _decompressedMessage);

            if (messageKind == MessageKind::MSG_TEXT && !validateUtf8(_decompressedMessage))
            {
                close(WebSocketCloseConstants::kInvalidFramePayloadData,
                      WebSocketCloseConstants::kInvalidFramePayloadDataMessage);
            }
            else
            {
                onMessageCallback(_decompressedMessage, wireSize, !success, messageKind);
            }
        }
        else
        {
            if (messageKind == MessageKind::MSG_TEXT && !validateUtf8(message))
            {
                close(WebSocketCloseConstants::kInvalidFramePayloadData,
                      WebSocketCloseConstants::kInvalidFramePayloadDataMessage);
            }
            else
            {
                onMessageCallback(message, wireSize, false, messageKind);
            }
        }
    }

    unsigned WebSocketTransport::getRandomUnsigned()
    {
        auto now = std::chrono::system_clock::now();
        auto seconds =
            std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        return static_cast<unsigned>(seconds);
    }

    WebSocketSendInfo WebSocketTransport::sendData(wsheader_type::opcode_type type,
                                                   const IXWebSocketSendData& message,
                                                   bool compress,
                                                   const OnProgressCallback& onProgressCallback)
    {
        if (_readyState != ReadyState::OPEN && _readyState != ReadyState::CLOSING)
        {
            return WebSocketSendInfo(false);
        }

        size_t payloadSize = message.size();
        size_t wireSize = message.size();
        bool compressionError = false;

        auto message_begin = message.cbegin();
        auto message_end = message.cend();

        if (compress)
        {
            if (!_perMessageDeflate->compress(message, _compressedMessage))
            {
                bool success = false;
                compressionError = true;
                payloadSize = 0;
                wireSize = 0;
                return WebSocketSendInfo(success, compressionError, payloadSize, wireSize);
            }
            compressionError = false;
            wireSize = _compressedMessage.size();

            IXWebSocketSendData compressedSendData(_compressedMessage);
            message_begin = compressedSendData.cbegin();
            message_end = compressedSendData.cend();
        }

        {
            std::lock_guard<std::mutex> lock(_txbufMutex);
            _txbuf.reserve(wireSize);
        }

        bool success = true;

        // Common case for most message. No fragmentation required.
        if (wireSize < kChunkSize)
        {
            success = sendFragment(type, true, message_begin, message_end, compress);

            if (onProgressCallback)
            {
                onProgressCallback(0, 1);
            }
        }
        else
        {
            //
            // Large messages need to be fragmented
            //
            // Rules:
            // First message needs to specify a proper type (BINARY or TEXT)
            // Intermediary and last messages need to be of type CONTINUATION
            // Last message must set the fin byte.
            //
            auto steps = wireSize / kChunkSize;

            auto begin = message_begin;
            auto end = message_end;

            for (uint64_t i = 0; i < steps; ++i)
            {
                bool firstStep = i == 0;
                bool lastStep = (i + 1) == steps;
                bool fin = lastStep;

                end = begin + kChunkSize;
                if (lastStep)
                {
                    end = message_end;
                }

                auto opcodeType = type;
                if (!firstStep)
                {
                    opcodeType = wsheader_type::CONTINUATION;
                }

                // Send message
                if (!sendFragment(opcodeType, fin, begin, end, compress))
                {
                    return WebSocketSendInfo(false);
                }

                if (onProgressCallback && !onProgressCallback((int) i, (int) steps))
                {
                    break;
                }

                begin += kChunkSize;
            }
        }

        // Request to flush the send buffer on the background thread if it isn't empty
        if (!isSendBufferEmpty())
        {
            wakeUpFromPoll(SelectInterrupt::kSendRequest);

            // FIXME: we should have a timeout when sending large messages: see #131
            if (_blockingSend && !flushSendBuffer())
            {
                success = false;
            }
        }

        return WebSocketSendInfo(success, compressionError, payloadSize, wireSize);
    }

    template<class Iterator>
    bool WebSocketTransport::sendFragment(wsheader_type::opcode_type type,
                                          bool fin,
                                          Iterator message_begin,
                                          Iterator message_end,
                                          bool compress)
    {
        uint64_t message_size = static_cast<uint64_t>(message_end - message_begin);

        unsigned x = getRandomUnsigned();
        uint8_t masking_key[4] = {};
        masking_key[0] = (x >> 24);
        masking_key[1] = (x >> 16) & 0xff;
        masking_key[2] = (x >> 8) & 0xff;
        masking_key[3] = (x) &0xff;

        std::vector<uint8_t> header;
        header.assign(2 + (message_size >= 126 ? 2 : 0) + (message_size >= 65536 ? 6 : 0) +
                          (_useMask ? 4 : 0),
                      0);
        header[0] = type;

        // The fin bit indicate that this is the last fragment. Fin is French for end.
        if (fin)
        {
            header[0] |= 0x80;
        }

        // The rsv1 bit indicate that the frame is compressed
        // continuation opcodes should not set it. Autobahn 12.2.10 and others 12.X
        if (compress && type != wsheader_type::CONTINUATION)
        {
            header[0] |= 0x40;
        }

        if (message_size < 126)
        {
            header[1] = (message_size & 0xff) | (_useMask ? 0x80 : 0);

            if (_useMask)
            {
                header[2] = masking_key[0];
                header[3] = masking_key[1];
                header[4] = masking_key[2];
                header[5] = masking_key[3];
            }
        }
        else if (message_size < 65536)
        {
            header[1] = 126 | (_useMask ? 0x80 : 0);
            header[2] = (message_size >> 8) & 0xff;
            header[3] = (message_size >> 0) & 0xff;

            if (_useMask)
            {
                header[4] = masking_key[0];
                header[5] = masking_key[1];
                header[6] = masking_key[2];
                header[7] = masking_key[3];
            }
        }
        else
        { // TODO: run coverage testing here
            header[1] = 127 | (_useMask ? 0x80 : 0);
            header[2] = (message_size >> 56) & 0xff;
            header[3] = (message_size >> 48) & 0xff;
            header[4] = (message_size >> 40) & 0xff;
            header[5] = (message_size >> 32) & 0xff;
            header[6] = (message_size >> 24) & 0xff;
            header[7] = (message_size >> 16) & 0xff;
            header[8] = (message_size >> 8) & 0xff;
            header[9] = (message_size >> 0) & 0xff;

            if (_useMask)
            {
                header[10] = masking_key[0];
                header[11] = masking_key[1];
                header[12] = masking_key[2];
                header[13] = masking_key[3];
            }
        }

        // _txbuf will keep growing until it can be transmitted over the socket:
        appendToSendBuffer(header, message_begin, message_end, message_size, masking_key);

        // Now actually send this data
        return sendOnSocket();
    }

    WebSocketSendInfo WebSocketTransport::sendPing(const IXWebSocketSendData& message)
    {
        bool compress = false;
        WebSocketSendInfo info = sendData(wsheader_type::PING, message, compress);

        if (info.success)
        {
            std::lock_guard<std::mutex> lck(_lastSendPingTimePointMutex);
            _lastSendPingTimePoint = std::chrono::steady_clock::now();
        }

        return info;
    }

    WebSocketSendInfo WebSocketTransport::sendBinary(const IXWebSocketSendData& message,
                                                     const OnProgressCallback& onProgressCallback)

    {
        return sendData(
            wsheader_type::BINARY_FRAME, message, _enablePerMessageDeflate, onProgressCallback);
    }

    WebSocketSendInfo WebSocketTransport::sendText(const IXWebSocketSendData& message,
                                                   const OnProgressCallback& onProgressCallback)

    {
        return sendData(
            wsheader_type::TEXT_FRAME, message, _enablePerMessageDeflate, onProgressCallback);
    }

    bool WebSocketTransport::sendOnSocket()
    {
        std::lock_guard<std::mutex> lock(_txbufMutex);

        while (_txbuf.size())
        {
            ssize_t ret = 0;
            {
                std::lock_guard<std::mutex> lock(_socketMutex);
                ret = _socket->send((char*) &_txbuf[0], _txbuf.size());
            }

            if (ret < 0 && Socket::isWaitNeeded())
            {
                break;
            }
            else if (ret <= 0)
            {
                closeSocket();
                if (_readyState != ReadyState::CLOSING)
                {
                    setReadyState(ReadyState::CLOSED);
                }
                return false;
            }
            else
            {
                _txbuf.erase(_txbuf.begin(), _txbuf.begin() + ret);
            }
        }

        return true;
    }

    bool WebSocketTransport::receiveFromSocket()
    {
        while (true)
        {
            ssize_t ret = _socket->recv((char*) &_readbuf[0], _readbuf.size());

            if (ret < 0 && Socket::isWaitNeeded())
            {
                break;
            }
            else if (ret <= 0)
            {
                // if there are received data pending to be processed, then delay the abnormal
                // closure to after dispatch (other close code/reason could be read from the
                // buffer)

                closeSocket();
                return false;
            }
            else
            {
                _rxbuf.insert(_rxbuf.end(), _readbuf.begin(), _readbuf.begin() + ret);
            }
        }

        return true;
    }

    void WebSocketTransport::sendCloseFrame(uint16_t code, const std::string& reason)
    {
        bool compress = false;

        // if a status is set/was read
        if (code != WebSocketCloseConstants::kNoStatusCodeErrorCode)
        {
            // See list of close events here:
            // https://developer.mozilla.org/en-US/docs/Web/API/CloseEvent
            std::string closure {(char) (code >> 8), (char) (code & 0xff)};

            // copy reason after code
            closure.append(reason);

            sendData(wsheader_type::CLOSE, closure, compress);
        }
        else
        {
            // no close code/reason set
            sendData(wsheader_type::CLOSE, std::string(""), compress);
        }
    }

    void WebSocketTransport::closeSocket()
    {
        std::lock_guard<std::mutex> lock(_socketMutex);
        _socket->close();
    }

    bool WebSocketTransport::wakeUpFromPoll(uint64_t wakeUpCode)
    {
        std::lock_guard<std::mutex> lock(_socketMutex);
        return _socket->wakeUpFromPoll(wakeUpCode);
    }

    void WebSocketTransport::closeSocketAndSwitchToClosedState(uint16_t code,
                                                               const std::string& reason,
                                                               size_t closeWireSize,
                                                               bool remote)
    {
        closeSocket();

        setCloseReason(reason);
        _closeCode = code;
        _closeWireSize = closeWireSize;
        _closeRemote = remote;

        setReadyState(ReadyState::CLOSED);
        _requestInitCancellation = false;
    }

    void WebSocketTransport::close(uint16_t code,
                                   const std::string& reason,
                                   size_t closeWireSize,
                                   bool remote)
    {
        _requestInitCancellation = true;

        if (_readyState == ReadyState::CLOSING || _readyState == ReadyState::CLOSED) return;

        if (closeWireSize == 0)
        {
            closeWireSize = reason.size();
        }

        setCloseReason(reason);
        _closeCode = code;
        _closeWireSize = closeWireSize;
        _closeRemote = remote;

        {
            std::lock_guard<std::mutex> lock(_closingTimePointMutex);
            _closingTimePoint = std::chrono::steady_clock::now();
        }
        setReadyState(ReadyState::CLOSING);

        sendCloseFrame(code, reason);

        // wake up the poll, but do not close yet
        wakeUpFromPoll(SelectInterrupt::kSendRequest);
    }

    size_t WebSocketTransport::bufferedAmount() const
    {
        std::lock_guard<std::mutex> lock(_txbufMutex);
        return _txbuf.size();
    }

    bool WebSocketTransport::flushSendBuffer()
    {
        while (!isSendBufferEmpty() && !_requestInitCancellation)
        {
            // Wait with a 10ms timeout until the socket is ready to write.
            // This way we are not busy looping
            PollResultType result = _socket->isReadyToWrite(10);

            if (result == PollResultType::Error)
            {
                closeSocket();
                setReadyState(ReadyState::CLOSED);
                return false;
            }
            else if (result == PollResultType::ReadyForWrite)
            {
                if (!sendOnSocket())
                {
                    return false;
                }
            }
        }

        return true;
    }

    void WebSocketTransport::setCloseReason(const std::string& reason)
    {
        std::lock_guard<std::mutex> lock(_closeReasonMutex);
        _closeReason = reason;
    }

    const std::string& WebSocketTransport::getCloseReason() const
    {
        std::lock_guard<std::mutex> lock(_closeReasonMutex);
        return _closeReason;
    }
} // namespace ix
