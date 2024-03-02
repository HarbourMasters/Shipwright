/*
 *  IXHttpClient.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#include "IXHttpClient.h"

#include "IXGzipCodec.h"
#include "IXSocketFactory.h"
#include "IXUrlParser.h"
#include "IXUserAgent.h"
#include "IXWebSocketHttpHeaders.h"
#include <assert.h>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <random>
#include <sstream>
#include <vector>

namespace ix
{
    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods
    const std::string HttpClient::kPost = "POST";
    const std::string HttpClient::kGet = "GET";
    const std::string HttpClient::kHead = "HEAD";
    const std::string HttpClient::kDelete = "DELETE";
    const std::string HttpClient::kPut = "PUT";
    const std::string HttpClient::kPatch = "PATCH";

    HttpClient::HttpClient(bool async)
        : _async(async)
        , _stop(false)
        , _forceBody(false)
    {
        if (!_async) return;

        _thread = std::thread(&HttpClient::run, this);
    }

    HttpClient::~HttpClient()
    {
        if (!_thread.joinable()) return;

        _stop = true;
        _condition.notify_one();
        _thread.join();
    }

    void HttpClient::setTLSOptions(const SocketTLSOptions& tlsOptions)
    {
        _tlsOptions = tlsOptions;
    }

    void HttpClient::setForceBody(bool value)
    {
        _forceBody = value;
    }

    HttpRequestArgsPtr HttpClient::createRequest(const std::string& url, const std::string& verb)
    {
        auto request = std::make_shared<HttpRequestArgs>();
        request->url = url;
        request->verb = verb;
        return request;
    }

    bool HttpClient::performRequest(HttpRequestArgsPtr args,
                                    const OnResponseCallback& onResponseCallback)
    {
        assert(_async && "HttpClient needs its async parameter set to true "
                         "in order to call performRequest");
        if (!_async) return false;

        // Enqueue the task
        {
            // acquire lock
            std::unique_lock<std::mutex> lock(_queueMutex);

            // add the task
            _queue.push(std::make_pair(args, onResponseCallback));
        } // release lock

        // wake up one thread
        _condition.notify_one();

        return true;
    }

    void HttpClient::run()
    {
        while (true)
        {
            HttpRequestArgsPtr args;
            OnResponseCallback onResponseCallback;

            {
                std::unique_lock<std::mutex> lock(_queueMutex);

                while (!_stop && _queue.empty())
                {
                    _condition.wait(lock);
                }

                if (_stop) return;

                auto p = _queue.front();
                _queue.pop();

                args = p.first;
                onResponseCallback = p.second;
            }

            if (_stop) return;

            HttpResponsePtr response = request(args->url, args->verb, args->body, args);
            onResponseCallback(response);

            if (_stop) return;
        }
    }

    HttpResponsePtr HttpClient::request(const std::string& url,
                                        const std::string& verb,
                                        const std::string& body,
                                        HttpRequestArgsPtr args,
                                        int redirects)
    {
        // We only have one socket connection, so we cannot
        // make multiple requests concurrently.
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        uint64_t uploadSize = 0;
        uint64_t downloadSize = 0;
        int code = 0;
        WebSocketHttpHeaders headers;
        std::string payload;
        std::string description;

        std::string protocol, host, path, query;
        int port;
        bool isProtocolDefaultPort;

        if (!UrlParser::parse(url, protocol, host, path, query, port, isProtocolDefaultPort))
        {
            std::stringstream ss;
            ss << "Cannot parse url: " << url;
            return std::make_shared<HttpResponse>(code,
                                                  description,
                                                  HttpErrorCode::UrlMalformed,
                                                  headers,
                                                  payload,
                                                  ss.str(),
                                                  uploadSize,
                                                  downloadSize);
        }

        bool tls = protocol == "https";
        std::string errorMsg;
        _socket = createSocket(tls, -1, errorMsg, _tlsOptions);

        if (!_socket)
        {
            return std::make_shared<HttpResponse>(code,
                                                  description,
                                                  HttpErrorCode::CannotCreateSocket,
                                                  headers,
                                                  payload,
                                                  errorMsg,
                                                  uploadSize,
                                                  downloadSize);
        }

        // Build request string
        std::stringstream ss;
        ss << verb << " " << path << " HTTP/1.1\r\n";
        ss << "Host: " << host;
        if (!isProtocolDefaultPort)
        {
            ss << ":" << port;
        }
        ss << "\r\n";

#ifdef IXWEBSOCKET_USE_ZLIB
        if (args->compress && !args->onChunkCallback)
        {
            ss << "Accept-Encoding: gzip"
               << "\r\n";
        }
#endif

        // Append extra headers
        for (auto&& it : args->extraHeaders)
        {
            ss << it.first << ": " << it.second << "\r\n";
        }

        // Set a default Accept header if none is present
        if (args->extraHeaders.find("Accept") == args->extraHeaders.end())
        {
            ss << "Accept: */*"
               << "\r\n";
        }

        // Set a default User agent if none is present
        if (args->extraHeaders.find("User-Agent") == args->extraHeaders.end())
        {
            ss << "User-Agent: " << userAgent() << "\r\n";
        }

        // Set an origin header if missing
        if (args->extraHeaders.find("Origin") == args->extraHeaders.end())
        {
            ss << "Origin: " << protocol << "://" << host << ":" << port << "\r\n";
        }

        if (verb == kPost || verb == kPut || verb == kPatch || _forceBody)
        {
            // Set request compression header
#ifdef IXWEBSOCKET_USE_ZLIB
            if (args->compressRequest)
            {
                ss << "Content-Encoding: gzip"
                   << "\r\n";
            }
#endif

            ss << "Content-Length: " << body.size() << "\r\n";

            // Set default Content-Type if unspecified
            if (args->extraHeaders.find("Content-Type") == args->extraHeaders.end())
            {
                if (args->multipartBoundary.empty())
                {
                    ss << "Content-Type: application/x-www-form-urlencoded"
                       << "\r\n";
                }
                else
                {
                    ss << "Content-Type: multipart/form-data; boundary=" << args->multipartBoundary
                       << "\r\n";
                }
            }
            ss << "\r\n";
            ss << body;
        }
        else
        {
            ss << "\r\n";
        }

        std::string req(ss.str());
        std::string errMsg;

        // Make a cancellation object dealing with connection timeout
        auto cancelled = makeCancellationRequestWithTimeout(args->connectTimeout, args->cancel);

        auto isCancellationRequested = [&]() {
            return cancelled() || _stop;
        };

        bool success = _socket->connect(host, port, errMsg, isCancellationRequested);
        if (!success)
        {
            auto errorCode = args->cancel ? HttpErrorCode::Cancelled : HttpErrorCode::CannotConnect;
            std::stringstream ss;
            ss << "Cannot connect to url: " << url << " / error : " << errMsg;
            return std::make_shared<HttpResponse>(code,
                                                  description,
                                                  errorCode,
                                                  headers,
                                                  payload,
                                                  ss.str(),
                                                  uploadSize,
                                                  downloadSize);
        }

        // Make a new cancellation object dealing with transfer timeout
        cancelled = makeCancellationRequestWithTimeout(args->transferTimeout, args->cancel);

        if (args->verbose)
        {
            std::stringstream ss;
            ss << "Sending " << verb << " request "
               << "to " << host << ":" << port << std::endl
               << "request size: " << req.size() << " bytes" << std::endl
               << "=============" << std::endl
               << req << "=============" << std::endl
               << std::endl;

            log(ss.str(), args);
        }

        if (!_socket->writeBytes(req, isCancellationRequested))
        {
            auto errorCode = args->cancel ? HttpErrorCode::Cancelled : HttpErrorCode::SendError;
            std::string errorMsg("Cannot send request");
            return std::make_shared<HttpResponse>(code,
                                                  description,
                                                  errorCode,
                                                  headers,
                                                  payload,
                                                  errorMsg,
                                                  uploadSize,
                                                  downloadSize);
        }

        uploadSize = req.size();

        auto lineResult = _socket->readLine(isCancellationRequested);
        auto lineValid = lineResult.first;
        auto line = lineResult.second;

        if (!lineValid)
        {
            auto errorCode = args->cancel ? HttpErrorCode::Cancelled : HttpErrorCode::CannotReadStatusLine;
            std::string errorMsg("Cannot retrieve status line");
            return std::make_shared<HttpResponse>(code,
                                                  description,
                                                  errorCode,
                                                  headers,
                                                  payload,
                                                  errorMsg,
                                                  uploadSize,
                                                  downloadSize);
        }

        if (args->verbose)
        {
            std::stringstream ss;
            ss << "Status line " << line;
            log(ss.str(), args);
        }

        if (sscanf(line.c_str(), "HTTP/1.1 %d", &code) != 1)
        {
            std::string errorMsg("Cannot parse response code from status line");
            return std::make_shared<HttpResponse>(code,
                                                  description,
                                                  HttpErrorCode::MissingStatus,
                                                  headers,
                                                  payload,
                                                  errorMsg,
                                                  uploadSize,
                                                  downloadSize);
        }

        auto result = parseHttpHeaders(_socket, isCancellationRequested);
        auto headersValid = result.first;
        headers = result.second;

        if (!headersValid)
        {
            auto errorCode = args->cancel ? HttpErrorCode::Cancelled : HttpErrorCode::HeaderParsingError;
            std::string errorMsg("Cannot parse http headers");
            return std::make_shared<HttpResponse>(code,
                                                  description,
                                                  errorCode,
                                                  headers,
                                                  payload,
                                                  errorMsg,
                                                  uploadSize,
                                                  downloadSize);
        }

        // Redirect ?
        if ((code >= 301 && code <= 308) && args->followRedirects)
        {
            if (headers.find("Location") == headers.end())
            {
                std::string errorMsg("Missing location header for redirect");
                return std::make_shared<HttpResponse>(code,
                                                      description,
                                                      HttpErrorCode::MissingLocation,
                                                      headers,
                                                      payload,
                                                      errorMsg,
                                                      uploadSize,
                                                      downloadSize);
            }

            if (redirects >= args->maxRedirects)
            {
                std::stringstream ss;
                ss << "Too many redirects: " << redirects;
                return std::make_shared<HttpResponse>(code,
                                                      description,
                                                      HttpErrorCode::TooManyRedirects,
                                                      headers,
                                                      payload,
                                                      ss.str(),
                                                      uploadSize,
                                                      downloadSize);
            }

            // Recurse
            std::string location = headers["Location"];
            return request(location, verb, body, args, redirects + 1);
        }

        if (verb == "HEAD")
        {
            return std::make_shared<HttpResponse>(code,
                                                  description,
                                                  HttpErrorCode::Ok,
                                                  headers,
                                                  payload,
                                                  std::string(),
                                                  uploadSize,
                                                  downloadSize);
        }

        // Parse response:
        if (headers.find("Content-Length") != headers.end())
        {
            ssize_t contentLength = -1;
            ss.str("");
            ss << headers["Content-Length"];
            ss >> contentLength;

            auto chunkResult = _socket->readBytes(contentLength,
                                                  args->onProgressCallback,
                                                  args->onChunkCallback,
                                                  isCancellationRequested);
            if (!chunkResult.first)
            {
                auto errorCode = args->cancel ? HttpErrorCode::Cancelled : HttpErrorCode::ChunkReadError;
                errorMsg = "Cannot read chunk";
                return std::make_shared<HttpResponse>(code,
                                                      description,
                                                      errorCode,
                                                      headers,
                                                      payload,
                                                      errorMsg,
                                                      uploadSize,
                                                      downloadSize);
            }

            if (!args->onChunkCallback)
            {
                payload.reserve(contentLength);
                payload += chunkResult.second;
            }
        }
        else if (headers.find("Transfer-Encoding") != headers.end() &&
                 headers["Transfer-Encoding"] == "chunked")
        {
            std::stringstream ss;

            while (true)
            {
                auto errorCode = args->cancel ? HttpErrorCode::Cancelled : HttpErrorCode::ChunkReadError;
                lineResult = _socket->readLine(isCancellationRequested);
                line = lineResult.second;

                if (!lineResult.first)
                {
                    return std::make_shared<HttpResponse>(code,
                                                          description,
                                                          errorCode,
                                                          headers,
                                                          payload,
                                                          errorMsg,
                                                          uploadSize,
                                                          downloadSize);
                }

                uint64_t chunkSize;
                ss.str("");
                ss << std::hex << line;
                ss >> chunkSize;

                if (args->verbose)
                {
                    std::stringstream oss;
                    oss << "Reading " << chunkSize << " bytes" << std::endl;
                    log(oss.str(), args);
                }

                // Read a chunk
                auto chunkResult = _socket->readBytes((size_t) chunkSize,
                                                      args->onProgressCallback,
                                                      args->onChunkCallback,
                                                      isCancellationRequested);
                if (!chunkResult.first)
                {
                    auto errorCode = args->cancel ? HttpErrorCode::Cancelled : HttpErrorCode::ChunkReadError;
                    errorMsg = "Cannot read chunk";
                    return std::make_shared<HttpResponse>(code,
                                                          description,
                                                          errorCode,
                                                          headers,
                                                          payload,
                                                          errorMsg,
                                                          uploadSize,
                                                          downloadSize);
                }

                if (!args->onChunkCallback)
                {
                    payload.reserve(payload.size() + (size_t) chunkSize);
                    payload += chunkResult.second;
                }

                // Read the line that terminates the chunk (\r\n)
                lineResult = _socket->readLine(isCancellationRequested);

                if (!lineResult.first)
                {
                    auto errorCode = args->cancel ? HttpErrorCode::Cancelled : HttpErrorCode::ChunkReadError;
                    return std::make_shared<HttpResponse>(code,
                                                          description,
                                                          errorCode,
                                                          headers,
                                                          payload,
                                                          errorMsg,
                                                          uploadSize,
                                                          downloadSize);
                }

                if (chunkSize == 0) break;
            }
        }
        else if (code == 204)
        {
            ; // 204 is NoContent response code
        }
        else
        {
            std::string errorMsg("Cannot read http body");
            return std::make_shared<HttpResponse>(code,
                                                  description,
                                                  HttpErrorCode::CannotReadBody,
                                                  headers,
                                                  payload,
                                                  errorMsg,
                                                  uploadSize,
                                                  downloadSize);
        }

        downloadSize = payload.size();

        // If the content was compressed with gzip, decode it
        if (headers["Content-Encoding"] == "gzip")
        {
#ifdef IXWEBSOCKET_USE_ZLIB
            std::string decompressedPayload;
            if (!gzipDecompress(payload, decompressedPayload))
            {
                std::string errorMsg("Error decompressing payload");
                return std::make_shared<HttpResponse>(code,
                                                      description,
                                                      HttpErrorCode::Gzip,
                                                      headers,
                                                      payload,
                                                      errorMsg,
                                                      uploadSize,
                                                      downloadSize);
            }
            payload = decompressedPayload;
#else
            std::string errorMsg("ixwebsocket was not compiled with gzip support on");
            return std::make_shared<HttpResponse>(code,
                                                  description,
                                                  HttpErrorCode::Gzip,
                                                  headers,
                                                  payload,
                                                  errorMsg,
                                                  uploadSize,
                                                  downloadSize);
#endif
        }

        return std::make_shared<HttpResponse>(code,
                                              description,
                                              HttpErrorCode::Ok,
                                              headers,
                                              payload,
                                              std::string(),
                                              uploadSize,
                                              downloadSize);
    }

    HttpResponsePtr HttpClient::get(const std::string& url, HttpRequestArgsPtr args)
    {
        return request(url, kGet, std::string(), args);
    }

    HttpResponsePtr HttpClient::head(const std::string& url, HttpRequestArgsPtr args)
    {
        return request(url, kHead, std::string(), args);
    }

    HttpResponsePtr HttpClient::Delete(const std::string& url, HttpRequestArgsPtr args)
    {
        return request(url, kDelete, std::string(), args);
    }

    HttpResponsePtr HttpClient::request(const std::string& url,
                                        const std::string& verb,
                                        const HttpParameters& httpParameters,
                                        const HttpFormDataParameters& httpFormDataParameters,
                                        HttpRequestArgsPtr args)
    {
        std::string body;

        if (httpFormDataParameters.empty())
        {
            body = serializeHttpParameters(httpParameters);
        }
        else
        {
            std::string multipartBoundary = generateMultipartBoundary();
            args->multipartBoundary = multipartBoundary;
            body = serializeHttpFormDataParameters(
                multipartBoundary, httpFormDataParameters, httpParameters);
        }

#ifdef IXWEBSOCKET_USE_ZLIB
        if (args->compressRequest)
        {
            body = gzipCompress(body);
        }
#endif

        return request(url, verb, body, args);
    }

    HttpResponsePtr HttpClient::post(const std::string& url,
                                     const HttpParameters& httpParameters,
                                     const HttpFormDataParameters& httpFormDataParameters,
                                     HttpRequestArgsPtr args)
    {
        return request(url, kPost, httpParameters, httpFormDataParameters, args);
    }

    HttpResponsePtr HttpClient::post(const std::string& url,
                                     const std::string& body,
                                     HttpRequestArgsPtr args)
    {
        return request(url, kPost, body, args);
    }

    HttpResponsePtr HttpClient::put(const std::string& url,
                                    const HttpParameters& httpParameters,
                                    const HttpFormDataParameters& httpFormDataParameters,
                                    HttpRequestArgsPtr args)
    {
        return request(url, kPut, httpParameters, httpFormDataParameters, args);
    }

    HttpResponsePtr HttpClient::put(const std::string& url,
                                    const std::string& body,
                                    const HttpRequestArgsPtr args)
    {
        return request(url, kPut, body, args);
    }

    HttpResponsePtr HttpClient::patch(const std::string& url,
                                      const HttpParameters& httpParameters,
                                      const HttpFormDataParameters& httpFormDataParameters,
                                      HttpRequestArgsPtr args)
    {
        return request(url, kPatch, httpParameters, httpFormDataParameters, args);
    }

    HttpResponsePtr HttpClient::patch(const std::string& url,
                                      const std::string& body,
                                      const HttpRequestArgsPtr args)
    {
        return request(url, kPatch, body, args);
    }

    std::string HttpClient::urlEncode(const std::string& value)
    {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i)
        {
            std::string::value_type c = (*i);

            // Keep alphanumeric and other accepted characters intact
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
            {
                escaped << c;
                continue;
            }

            // Any other characters are percent-encoded
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int((unsigned char) c);
            escaped << std::nouppercase;
        }

        return escaped.str();
    }

    std::string HttpClient::serializeHttpParameters(const HttpParameters& httpParameters)
    {
        std::stringstream ss;
        size_t count = httpParameters.size();
        size_t i = 0;

        for (auto&& it : httpParameters)
        {
            ss << urlEncode(it.first) << "=" << urlEncode(it.second);

            if (i++ < (count - 1))
            {
                ss << "&";
            }
        }
        return ss.str();
    }

    std::string HttpClient::serializeHttpFormDataParameters(
        const std::string& multipartBoundary,
        const HttpFormDataParameters& httpFormDataParameters,
        const HttpParameters& httpParameters)
    {
        //
        // --AaB03x
        // Content-Disposition: form-data; name="submit-name"

        // Larry
        // --AaB03x
        // Content-Disposition: form-data; name="foo.txt"; filename="file1.txt"
        // Content-Type: text/plain

        // ... contents of file1.txt ...
        // --AaB03x--
        //
        std::stringstream ss;

        for (auto&& it : httpFormDataParameters)
        {
            ss << "--" << multipartBoundary << "\r\n"
               << "Content-Disposition:"
               << " form-data; name=\"" << it.first << "\";"
               << " filename=\"" << it.first << "\""
               << "\r\n"
               << "Content-Type: application/octet-stream"
               << "\r\n"
               << "\r\n"
               << it.second << "\r\n";
        }

        for (auto&& it : httpParameters)
        {
            ss << "--" << multipartBoundary << "\r\n"
               << "Content-Disposition:"
               << " form-data; name=\"" << it.first << "\";"
               << "\r\n"
               << "\r\n"
               << it.second << "\r\n";
        }

        ss << "--" << multipartBoundary << "--\r\n";

        return ss.str();
    }

    void HttpClient::log(const std::string& msg, HttpRequestArgsPtr args)
    {
        if (args->logger)
        {
            args->logger(msg);
        }
    }

    std::string HttpClient::generateMultipartBoundary()
    {
        std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

        static std::random_device rd;
        static std::mt19937 generator(rd());

        std::shuffle(str.begin(), str.end(), generator);

        return str;
    }
} // namespace ix
