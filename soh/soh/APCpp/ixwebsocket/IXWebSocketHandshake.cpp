/*
 *  IXWebSocketHandshake.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#include "IXWebSocketHandshake.h"

#include "IXBase64.h"
#include "IXHttp.h"
#include "IXSocketConnect.h"
#include "IXStrCaseCompare.h"
#include "IXUrlParser.h"
#include "IXUserAgent.h"
#include "IXWebSocketHandshakeKeyGen.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <sstream>

namespace ix
{
    WebSocketHandshake::WebSocketHandshake(
        std::atomic<bool>& requestInitCancellation,
        std::unique_ptr<Socket>& socket,
        WebSocketPerMessageDeflatePtr& perMessageDeflate,
        WebSocketPerMessageDeflateOptions& perMessageDeflateOptions,
        std::atomic<bool>& enablePerMessageDeflate)
        : _requestInitCancellation(requestInitCancellation)
        , _socket(socket)
        , _perMessageDeflate(perMessageDeflate)
        , _perMessageDeflateOptions(perMessageDeflateOptions)
        , _enablePerMessageDeflate(enablePerMessageDeflate)
    {
    }

    bool WebSocketHandshake::insensitiveStringCompare(const std::string& a, const std::string& b)
    {
        return CaseInsensitiveLess::cmp(a, b) == 0;
    }

    std::string WebSocketHandshake::genRandomString(const int len)
    {
        std::string alphanum = "0123456789"
                               "ABCDEFGH"
                               "abcdefgh";

        std::random_device r;
        std::default_random_engine e1(r());
        std::uniform_int_distribution<int> dist(0, (int) alphanum.size() - 1);

        std::string s;
        s.resize(len);

        for (int i = 0; i < len; ++i)
        {
            int x = dist(e1);
            s[i] = alphanum[x];
        }

        return s;
    }

    WebSocketInitResult WebSocketHandshake::sendErrorResponse(int code, const std::string& reason)
    {
        std::stringstream ss;
        ss << "HTTP/1.1 ";
        ss << code;
        ss << " ";
        ss << reason;
        ss << "\r\n";
        ss << "Server: " << userAgent() << "\r\n";

        // Socket write can only be cancelled through a timeout here, not manually.
        static std::atomic<bool> requestInitCancellation(false);
        auto isCancellationRequested =
            makeCancellationRequestWithTimeout(1, requestInitCancellation);

        if (!_socket->writeBytes(ss.str(), isCancellationRequested))
        {
            return WebSocketInitResult(false, 500, "Timed out while sending error response");
        }

        return WebSocketInitResult(false, code, reason);
    }

    WebSocketInitResult WebSocketHandshake::clientHandshake(
        const std::string& url,
        const WebSocketHttpHeaders& extraHeaders,
        const std::string& protocol,
        const std::string& host,
        const std::string& path,
        int port,
        int timeoutSecs)
    {
        _requestInitCancellation = false;

        auto isCancellationRequested =
            makeCancellationRequestWithTimeout(timeoutSecs, _requestInitCancellation);

        std::string errMsg;
        bool success = _socket->connect(host, port, errMsg, isCancellationRequested);
        if (!success)
        {
            std::stringstream ss;
            ss << "Unable to connect to " << host << " on port " << port << ", error: " << errMsg;
            return WebSocketInitResult(false, 0, ss.str());
        }

        // Generate a random 16 bytes string and base64 encode it.
        //
        // See https://stackoverflow.com/questions/18265128/what-is-sec-websocket-key-for
        std::string secWebSocketKey = macaron::Base64::Encode(genRandomString(16));

        std::stringstream ss;
        ss << "GET " << path << " HTTP/1.1\r\n";
        ss << "Host: " << host << ":" << port << "\r\n";
        ss << "Upgrade: websocket\r\n";
        ss << "Connection: Upgrade\r\n";
        ss << "Sec-WebSocket-Version: 13\r\n";
        ss << "Sec-WebSocket-Key: " << secWebSocketKey << "\r\n";

        // User-Agent can be customized by users
        if (extraHeaders.find("User-Agent") == extraHeaders.end())
        {
            ss << "User-Agent: " << userAgent() << "\r\n";
        }

        // Set an origin header if missing
        if (extraHeaders.find("Origin") == extraHeaders.end())
        {
            ss << "Origin: " << protocol << "://" << host << ":" << port << "\r\n";
        }

        for (auto& it : extraHeaders)
        {
            ss << it.first << ": " << it.second << "\r\n";
        }

        if (_enablePerMessageDeflate)
        {
            ss << _perMessageDeflateOptions.generateHeader();
        }

        ss << "\r\n";

        if (!_socket->writeBytes(ss.str(), isCancellationRequested))
        {
            return WebSocketInitResult(
                false, 0, std::string("Failed sending GET request to ") + url);
        }

        // Read HTTP status line
        auto lineResult = _socket->readLine(isCancellationRequested);
        auto lineValid = lineResult.first;
        auto line = lineResult.second;

        if (!lineValid)
        {
            return WebSocketInitResult(
                false, 0, std::string("Failed reading HTTP status line from ") + url);
        }

        // Validate status
        auto statusLine = Http::parseStatusLine(line);
        std::string httpVersion = statusLine.first;
        int status = statusLine.second;

        // HTTP/1.0 is too old.
        if (httpVersion != "HTTP/1.1")
        {
            std::stringstream ss;
            ss << "Expecting HTTP/1.1, got " << httpVersion << ". "
               << "Rejecting connection to " << url << ", status: " << status
               << ", HTTP Status line: " << line;
            return WebSocketInitResult(false, status, ss.str());
        }

        auto result = parseHttpHeaders(_socket, isCancellationRequested);
        auto headersValid = result.first;
        auto headers = result.second;

        if (!headersValid)
        {
            return WebSocketInitResult(false, status, "Error parsing HTTP headers");
        }

        // We want an 101 HTTP status for websocket, otherwise it could be
        // a redirection (like 301)
        if (status != 101)
        {
            std::stringstream ss;
            ss << "Expecting status 101 (Switching Protocol), got " << status
               << " status connecting to " << url << ", HTTP Status line: " << line;

            return WebSocketInitResult(false, status, ss.str(), headers, path);
        }

        // Check the presence of the connection field
        if (headers.find("connection") == headers.end())
        {
            std::string errorMsg("Missing connection value");
            return WebSocketInitResult(false, status, errorMsg);
        }

        // Check the value of the connection field
        // Some websocket servers (Go/Gorilla?) send lowercase values for the
        // connection header, so do a case insensitive comparison
        //
        // See https://github.com/apache/thrift/commit/7c4bdf9914fcba6c89e0f69ae48b9675578f084a
        //
        if (!insensitiveStringCompare(headers["connection"], "Upgrade"))
        {
            std::stringstream ss;
            ss << "Invalid connection value: " << headers["connection"];
            return WebSocketInitResult(false, status, ss.str());
        }

        char output[29] = {};
        WebSocketHandshakeKeyGen::generate(secWebSocketKey, output);
        if (std::string(output) != headers["sec-websocket-accept"])
        {
            std::string errorMsg("Invalid Sec-WebSocket-Accept value");
            return WebSocketInitResult(false, status, errorMsg);
        }

        if (_enablePerMessageDeflate)
        {
            // Parse the server response. Does it support deflate ?
            std::string header = headers["sec-websocket-extensions"];
            WebSocketPerMessageDeflateOptions webSocketPerMessageDeflateOptions(header);

            // If the server does not support that extension, disable it.
            if (!webSocketPerMessageDeflateOptions.enabled())
            {
                _enablePerMessageDeflate = false;
            }
            // Otherwise try to initialize the deflate engine (zlib)
            else if (!_perMessageDeflate->init(webSocketPerMessageDeflateOptions))
            {
                return WebSocketInitResult(
                    false, 0, "Failed to initialize per message deflate engine");
            }
        }

        return WebSocketInitResult(true, status, "", headers, path);
    }

    WebSocketInitResult WebSocketHandshake::serverHandshake(int timeoutSecs,
                                                            bool enablePerMessageDeflate,
                                                            HttpRequestPtr request)
    {
        _requestInitCancellation = false;

        auto isCancellationRequested =
            makeCancellationRequestWithTimeout(timeoutSecs, _requestInitCancellation);

        std::string method;
        std::string uri;
        std::string httpVersion;

        if (request)
        {
            method = request->method;
            uri = request->uri;
            httpVersion = request->version;
        }
        else
        {
            // Read first line
            auto lineResult = _socket->readLine(isCancellationRequested);
            auto lineValid = lineResult.first;
            auto line = lineResult.second;

            if (!lineValid)
            {
                return sendErrorResponse(400, "Error reading HTTP request line");
            }

            // Validate request line (GET /foo HTTP/1.1\r\n)
            auto requestLine = Http::parseRequestLine(line);
            method = std::get<0>(requestLine);
            uri = std::get<1>(requestLine);
            httpVersion = std::get<2>(requestLine);
        }

        if (method != "GET")
        {
            return sendErrorResponse(400, "Invalid HTTP method, need GET, got " + method);
        }

        if (httpVersion != "HTTP/1.1")
        {
            return sendErrorResponse(400,
                                     "Invalid HTTP version, need HTTP/1.1, got: " + httpVersion);
        }

        WebSocketHttpHeaders headers;
        if (request)
        {
            headers = request->headers;
        }
        else
        {
            // Retrieve and validate HTTP headers
            auto result = parseHttpHeaders(_socket, isCancellationRequested);
            auto headersValid = result.first;
            headers = result.second;

            if (!headersValid)
            {
                return sendErrorResponse(400, "Error parsing HTTP headers");
            }
        }

        if (headers.find("sec-websocket-key") == headers.end())
        {
            return sendErrorResponse(400, "Missing Sec-WebSocket-Key value");
        }

        if (headers.find("upgrade") == headers.end())
        {
            return sendErrorResponse(400, "Missing Upgrade header");
        }

        if (!insensitiveStringCompare(headers["upgrade"], "WebSocket") &&
            headers["Upgrade"] != "keep-alive, Upgrade") // special case for firefox
        {
            return sendErrorResponse(400,
                                     "Invalid Upgrade header, "
                                     "need WebSocket, got " +
                                         headers["upgrade"]);
        }

        if (headers.find("sec-websocket-version") == headers.end())
        {
            return sendErrorResponse(400, "Missing Sec-WebSocket-Version value");
        }

        {
            std::stringstream ss;
            ss << headers["sec-websocket-version"];
            int version;
            ss >> version;

            if (version != 13)
            {
                return sendErrorResponse(400,
                                         "Invalid Sec-WebSocket-Version, "
                                         "need 13, got " +
                                             ss.str());
            }
        }

        char output[29] = {};
        WebSocketHandshakeKeyGen::generate(headers["sec-websocket-key"], output);

        std::stringstream ss;
        ss << "HTTP/1.1 101 Switching Protocols\r\n";
        ss << "Sec-WebSocket-Accept: " << std::string(output) << "\r\n";
        ss << "Upgrade: websocket\r\n";
        ss << "Connection: Upgrade\r\n";
        ss << "Server: " << userAgent() << "\r\n";

        // Parse the client headers. Does it support deflate ?
        std::string header = headers["sec-websocket-extensions"];
        WebSocketPerMessageDeflateOptions webSocketPerMessageDeflateOptions(header);

        // If the client has requested that extension,
        if (webSocketPerMessageDeflateOptions.enabled() && enablePerMessageDeflate)
        {
            _enablePerMessageDeflate = true;

            if (!_perMessageDeflate->init(webSocketPerMessageDeflateOptions))
            {
                return WebSocketInitResult(
                    false, 0, "Failed to initialize per message deflate engine");
            }
            ss << webSocketPerMessageDeflateOptions.generateHeader();
        }

        ss << "\r\n";

        if (!_socket->writeBytes(ss.str(), isCancellationRequested))
        {
            return WebSocketInitResult(
                false, 0, std::string("Failed sending response to remote end"));
        }

        return WebSocketInitResult(true, 200, "", headers, uri);
    }
} // namespace ix
