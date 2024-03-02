/*
 *  IXHttpClient.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include "IXHttp.h"
#include "IXSocket.h"
#include "IXSocketTLSOptions.h"
#include "IXWebSocketHttpHeaders.h"
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace ix
{
    class HttpClient
    {
    public:
        HttpClient(bool async = false);
        ~HttpClient();

        HttpResponsePtr get(const std::string& url, HttpRequestArgsPtr args);
        HttpResponsePtr head(const std::string& url, HttpRequestArgsPtr args);
        HttpResponsePtr Delete(const std::string& url, HttpRequestArgsPtr args);

        HttpResponsePtr post(const std::string& url,
                             const HttpParameters& httpParameters,
                             const HttpFormDataParameters& httpFormDataParameters,
                             HttpRequestArgsPtr args);
        HttpResponsePtr post(const std::string& url,
                             const std::string& body,
                             HttpRequestArgsPtr args);

        HttpResponsePtr put(const std::string& url,
                            const HttpParameters& httpParameters,
                            const HttpFormDataParameters& httpFormDataParameters,
                            HttpRequestArgsPtr args);
        HttpResponsePtr put(const std::string& url,
                            const std::string& body,
                            HttpRequestArgsPtr args);

        HttpResponsePtr patch(const std::string& url,
                              const HttpParameters& httpParameters,
                              const HttpFormDataParameters& httpFormDataParameters,
                              HttpRequestArgsPtr args);
        HttpResponsePtr patch(const std::string& url,
                              const std::string& body,
                              HttpRequestArgsPtr args);

        HttpResponsePtr request(const std::string& url,
                                const std::string& verb,
                                const std::string& body,
                                HttpRequestArgsPtr args,
                                int redirects = 0);

        HttpResponsePtr request(const std::string& url,
                                const std::string& verb,
                                const HttpParameters& httpParameters,
                                const HttpFormDataParameters& httpFormDataParameters,
                                HttpRequestArgsPtr args);

        void setForceBody(bool value);

        // Async API
        HttpRequestArgsPtr createRequest(const std::string& url = std::string(),
                                         const std::string& verb = HttpClient::kGet);

        bool performRequest(HttpRequestArgsPtr request,
                            const OnResponseCallback& onResponseCallback);

        // TLS
        void setTLSOptions(const SocketTLSOptions& tlsOptions);

        std::string serializeHttpParameters(const HttpParameters& httpParameters);

        std::string serializeHttpFormDataParameters(
            const std::string& multipartBoundary,
            const HttpFormDataParameters& httpFormDataParameters,
            const HttpParameters& httpParameters = HttpParameters());

        std::string generateMultipartBoundary();

        std::string urlEncode(const std::string& value);

        const static std::string kPost;
        const static std::string kGet;
        const static std::string kHead;
        const static std::string kDelete;
        const static std::string kPut;
        const static std::string kPatch;

    private:
        void log(const std::string& msg, HttpRequestArgsPtr args);

        // Async API background thread runner
        void run();
        // Async API
        bool _async;
        std::queue<std::pair<HttpRequestArgsPtr, OnResponseCallback>> _queue;
        mutable std::mutex _queueMutex;
        std::condition_variable _condition;
        std::atomic<bool> _stop;
        std::thread _thread;

        std::unique_ptr<Socket> _socket;
        std::recursive_mutex _mutex; // to protect accessing the _socket (only one socket per
                                     // client) the mutex needs to be recursive as this function
                                     // might be called recursively to follow HTTP redirections

        SocketTLSOptions _tlsOptions;

        bool _forceBody;
    };
} // namespace ix
