/*
 *  IXHttpServer.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#include "IXHttpServer.h"

#include "IXGzipCodec.h"
#include "IXNetSystem.h"
#include "IXSocketConnect.h"
#include "IXUserAgent.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

namespace
{
    std::pair<bool, std::vector<uint8_t>> load(const std::string& path)
    {
        std::vector<uint8_t> memblock;

        std::ifstream file(path);
        if (!file.is_open()) return std::make_pair(false, memblock);

        file.seekg(0, file.end);
        std::streamoff size = file.tellg();
        file.seekg(0, file.beg);

        memblock.resize((size_t) size);
        file.read((char*) &memblock.front(), static_cast<std::streamsize>(size));

        return std::make_pair(true, memblock);
    }

    std::pair<bool, std::string> readAsString(const std::string& path)
    {
        auto res = load(path);
        auto vec = res.second;
        return std::make_pair(res.first, std::string(vec.begin(), vec.end()));
    }

    std::string response_head_file(const std::string& file_name){

        if (std::string::npos != file_name.find(".html") || std::string::npos != file_name.find(".htm"))
            return "text/html";
        else if (std::string::npos != file_name.find(".css"))
            return "text/css";
        else if (std::string::npos != file_name.find(".js") || std::string::npos != file_name.find(".mjs"))
            return "application/x-javascript";
        else if (std::string::npos != file_name.find(".ico"))
            return "image/x-icon";
        else if (std::string::npos != file_name.find(".png"))
            return "image/png";
        else if (std::string::npos != file_name.find(".jpg") || std::string::npos != file_name.find(".jpeg"))
            return "image/jpeg";
        else if (std::string::npos != file_name.find(".gif"))
            return "image/gif";
        else if (std::string::npos != file_name.find(".svg"))
            return "image/svg+xml";
        else
            return "application/octet-stream";
    }

} // namespace

namespace ix
{
    const int HttpServer::kDefaultTimeoutSecs(30);

    HttpServer::HttpServer(int port,
                           const std::string& host,
                           int backlog,
                           size_t maxConnections,
                           int addressFamily,
                           int timeoutSecs,
                           int handshakeTimeoutSecs)
        : WebSocketServer(port, host, backlog, maxConnections, handshakeTimeoutSecs, addressFamily)
        , _timeoutSecs(timeoutSecs)
    {
        setDefaultConnectionCallback();
    }

    void HttpServer::setOnConnectionCallback(const OnConnectionCallback& callback)
    {
        _onConnectionCallback = callback;
    }

    void HttpServer::handleConnection(std::unique_ptr<Socket> socket,
                                      std::shared_ptr<ConnectionState> connectionState)
    {
        auto ret = Http::parseRequest(socket, _timeoutSecs);
        // FIXME: handle errors in parseRequest

        if (std::get<0>(ret))
        {
            auto request = std::get<2>(ret);
            std::shared_ptr<ix::HttpResponse> response;
            if (request->headers["Upgrade"] == "websocket")
            {
                WebSocketServer::handleUpgrade(std::move(socket), connectionState, request);
            }
            else
            {
                auto response = _onConnectionCallback(request, connectionState);
                if (!Http::sendResponse(response, socket))
                {
                    logError("Cannot send response");
                }
            }
        }
        connectionState->setTerminated();
    }

    void HttpServer::setDefaultConnectionCallback()
    {
        setOnConnectionCallback(
            [this](HttpRequestPtr request,
                   std::shared_ptr<ConnectionState> connectionState) -> HttpResponsePtr
            {
                std::string uri(request->uri);
                if (uri.empty() || uri == "/")
                {
                    uri = "/index.html";
                }

                WebSocketHttpHeaders headers;
                headers["Server"] = userAgent();
                headers["Content-Type"] = response_head_file(uri);

                std::string path("." + uri);
                auto res = readAsString(path);
                bool found = res.first;
                if (!found)
                {
                    return std::make_shared<HttpResponse>(
                        404, "Not Found", HttpErrorCode::Ok, WebSocketHttpHeaders(), std::string());
                }

                std::string content = res.second;

#ifdef IXWEBSOCKET_USE_ZLIB
                std::string acceptEncoding = request->headers["Accept-encoding"];
                if (acceptEncoding == "*" || acceptEncoding.find("gzip") != std::string::npos)
                {
                    content = gzipCompress(content);
                    headers["Content-Encoding"] = "gzip";
                }
#endif

                // Log request
                std::stringstream ss;
                ss << connectionState->getRemoteIp() << ":" << connectionState->getRemotePort()
                   << " " << request->method << " " << request->headers["User-Agent"] << " "
                   << request->uri << " " << content.size();
                logInfo(ss.str());

                // FIXME: check extensions to set the content type
                // headers["Content-Type"] = "application/octet-stream";
                headers["Accept-Ranges"] = "none";

                for (auto&& it : request->headers)
                {
                    headers[it.first] = it.second;
                }

                return std::make_shared<HttpResponse>(
                    200, "OK", HttpErrorCode::Ok, headers, content);
            });
    }

    void HttpServer::makeRedirectServer(const std::string& redirectUrl)
    {
        //
        // See https://developer.mozilla.org/en-US/docs/Web/HTTP/Redirections
        //
        setOnConnectionCallback(
            [this, redirectUrl](HttpRequestPtr request,
                                std::shared_ptr<ConnectionState> connectionState) -> HttpResponsePtr
            {
                WebSocketHttpHeaders headers;
                headers["Server"] = userAgent();

                // Log request
                std::stringstream ss;
                ss << connectionState->getRemoteIp() << ":" << connectionState->getRemotePort()
                   << " " << request->method << " " << request->headers["User-Agent"] << " "
                   << request->uri;
                logInfo(ss.str());

                if (request->method == "POST")
                {
                    return std::make_shared<HttpResponse>(
                        200, "OK", HttpErrorCode::Ok, headers, std::string());
                }

                headers["Location"] = redirectUrl;

                return std::make_shared<HttpResponse>(
                    301, "OK", HttpErrorCode::Ok, headers, std::string());
            });
    }

    //
    // Display the client parameter and body on the console
    //
    void HttpServer::makeDebugServer()
    {
        setOnConnectionCallback(
            [this](HttpRequestPtr request,
                   std::shared_ptr<ConnectionState> connectionState) -> HttpResponsePtr
            {
                WebSocketHttpHeaders headers;
                headers["Server"] = userAgent();

                // Log request
                std::stringstream ss;
                ss << connectionState->getRemoteIp() << ":" << connectionState->getRemotePort()
                   << " " << request->method << " " << request->headers["User-Agent"] << " "
                   << request->uri;
                logInfo(ss.str());

                logInfo("== Headers == ");
                for (auto&& it : request->headers)
                {
                    std::ostringstream oss;
                    oss << it.first << ": " << it.second;
                    logInfo(oss.str());
                }
                logInfo("");

                logInfo("== Body == ");
                logInfo(request->body);
                logInfo("");

                return std::make_shared<HttpResponse>(
                    200, "OK", HttpErrorCode::Ok, headers, std::string("OK"));
            });
    }

    int HttpServer::getTimeoutSecs()
    {
        return _timeoutSecs;
    }

} // namespace ix
