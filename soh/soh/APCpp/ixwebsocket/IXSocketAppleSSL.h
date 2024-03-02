/*
 *  IXSocketAppleSSL.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2020 Machine Zone, Inc. All rights reserved.
 */
#ifdef IXWEBSOCKET_USE_SECURE_TRANSPORT

#pragma once

#include "IXCancellationRequest.h"
#include "IXSocket.h"
#include "IXSocketTLSOptions.h"
#include <Security/SecureTransport.h>
#include <Security/Security.h>
#include <mutex>

namespace ix
{
    class SocketAppleSSL final : public Socket
    {
    public:
        SocketAppleSSL(const SocketTLSOptions& tlsOptions, int fd = -1);
        ~SocketAppleSSL();

        virtual bool accept(std::string& errMsg) final;

        virtual bool connect(const std::string& host,
                             int port,
                             std::string& errMsg,
                             const CancellationRequest& isCancellationRequested) final;
        virtual void close() final;

        virtual ssize_t send(char* buffer, size_t length) final;
        virtual ssize_t recv(void* buffer, size_t length) final;

    private:
        static std::string getSSLErrorDescription(OSStatus status);
        static OSStatus writeToSocket(SSLConnectionRef connection, const void* data, size_t* len);
        static OSStatus readFromSocket(SSLConnectionRef connection, void* data, size_t* len);

        OSStatus tlsHandShake(std::string& errMsg,
                              const CancellationRequest& isCancellationRequested);

        SSLContextRef _sslContext;
        mutable std::mutex _mutex; // AppleSSL routines are not thread-safe

        SocketTLSOptions _tlsOptions;
    };

} // namespace ix

#endif // IXWEBSOCKET_USE_SECURE_TRANSPORT
