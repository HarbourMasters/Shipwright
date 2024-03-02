/*
 *  IXSocketMbedTLS.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019-2020 Machine Zone, Inc. All rights reserved.
 */
#ifdef IXWEBSOCKET_USE_MBED_TLS

#pragma once

#include "IXSocket.h"
#include "IXSocketTLSOptions.h"
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>
#include <mbedtls/entropy.h>
#include <mbedtls/error.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/platform.h>
#include <mbedtls/x509.h>
#include <mbedtls/x509_crt.h>
#include <mutex>

namespace ix
{
    class SocketMbedTLS final : public Socket
    {
    public:
        SocketMbedTLS(const SocketTLSOptions& tlsOptions, int fd = -1);
        ~SocketMbedTLS();

        virtual bool accept(std::string& errMsg) final;

        virtual bool connect(const std::string& host,
                             int port,
                             std::string& errMsg,
                             const CancellationRequest& isCancellationRequested) final;
        virtual void close() final;

        virtual ssize_t send(char* buffer, size_t length) final;
        virtual ssize_t recv(void* buffer, size_t length) final;

    private:
        mbedtls_ssl_context _ssl;
        mbedtls_ssl_config _conf;
        mbedtls_entropy_context _entropy;
        mbedtls_ctr_drbg_context _ctr_drbg;
        mbedtls_x509_crt _cacert;
        mbedtls_x509_crt _cert;
        mbedtls_pk_context _pkey;

        std::mutex _mutex;
        SocketTLSOptions _tlsOptions;

        bool init(const std::string& host, bool isClient, std::string& errMsg);
        void initMBedTLS();
        bool loadSystemCertificates(std::string& errMsg);
    };

} // namespace ix

#endif // IXWEBSOCKET_USE_MBED_TLS
