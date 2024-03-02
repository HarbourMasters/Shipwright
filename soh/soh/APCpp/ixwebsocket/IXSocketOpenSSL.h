/*
 *  IXSocketOpenSSL.h
 *  Author: Benjamin Sergeant, Matt DeBoer
 *  Copyright (c) 2017-2020 Machine Zone, Inc. All rights reserved.
 */
#ifdef IXWEBSOCKET_USE_OPEN_SSL

#pragma once

#include "IXCancellationRequest.h"
#include "IXSocket.h"
#include "IXSocketTLSOptions.h"
#include <mutex>
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/ssl.h>

namespace ix
{
    class SocketOpenSSL final : public Socket
    {
    public:
        SocketOpenSSL(const SocketTLSOptions& tlsOptions, int fd = -1);
        ~SocketOpenSSL();

        virtual bool accept(std::string& errMsg) final;

        virtual bool connect(const std::string& host,
                             int port,
                             std::string& errMsg,
                             const CancellationRequest& isCancellationRequested) final;
        virtual void close() final;

        virtual ssize_t send(char* buffer, size_t length) final;
        virtual ssize_t recv(void* buffer, size_t length) final;

    private:
        void openSSLInitialize();
        std::string getSSLError(int ret);
        SSL_CTX* openSSLCreateContext(std::string& errMsg);
        bool openSSLAddCARootsFromString(const std::string roots);
        bool openSSLClientHandshake(const std::string& hostname,
                                    std::string& errMsg,
                                    const CancellationRequest& isCancellationRequested);
        bool openSSLCheckServerCert(SSL* ssl, const std::string& hostname, std::string& errMsg);
        bool checkHost(const std::string& host, const char* pattern);
        bool handleTLSOptions(std::string& errMsg);
        bool openSSLServerHandshake(std::string& errMsg);

        // Required for OpenSSL < 1.1
        static void openSSLLockingCallback(int mode, int type, const char* /*file*/, int /*line*/);

        SSL* _ssl_connection;
        SSL_CTX* _ssl_context;
        const SSL_METHOD* _ssl_method;
        SocketTLSOptions _tlsOptions;

        mutable std::mutex _mutex; // OpenSSL routines are not thread-safe

        static std::once_flag _openSSLInitFlag;
        static std::atomic<bool> _openSSLInitializationSuccessful;
    };

} // namespace ix

#endif // IXWEBSOCKET_USE_OPEN_SSL
