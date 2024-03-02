/*
 *  IXSocketMbedTLS.cpp
 *  Author: Benjamin Sergeant, Max Weisel
 *  Copyright (c) 2019-2020 Machine Zone, Inc. All rights reserved.
 *
 *  Some code taken from
 *  https://github.com/rottor12/WsClientLib/blob/master/lib/src/WsClientLib.cpp
 *  and mini_client.c example from mbedtls
 */
#ifdef IXWEBSOCKET_USE_MBED_TLS

#include "IXSocketMbedTLS.h"

#include "IXNetSystem.h"
#include "IXSocket.h"
#include "IXSocketConnect.h"
#include <cstdint>
#include <string.h>

#ifdef _WIN32
// For manipulating the certificate store
#include <wincrypt.h>
#endif

namespace ix
{
    SocketMbedTLS::SocketMbedTLS(const SocketTLSOptions& tlsOptions, int fd)
        : Socket(fd)
        , _tlsOptions(tlsOptions)
    {
        initMBedTLS();
    }

    SocketMbedTLS::~SocketMbedTLS()
    {
        SocketMbedTLS::close();
    }

    void SocketMbedTLS::initMBedTLS()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        mbedtls_ssl_init(&_ssl);
        mbedtls_ssl_config_init(&_conf);
        mbedtls_ctr_drbg_init(&_ctr_drbg);
        mbedtls_entropy_init(&_entropy);
        mbedtls_x509_crt_init(&_cacert);
        mbedtls_x509_crt_init(&_cert);
        mbedtls_pk_init(&_pkey);
    }

    bool SocketMbedTLS::loadSystemCertificates(std::string& errorMsg)
    {
#ifdef _WIN32
        DWORD flags = CERT_STORE_READONLY_FLAG | CERT_STORE_OPEN_EXISTING_FLAG |
                      CERT_SYSTEM_STORE_CURRENT_USER;
        HCERTSTORE systemStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, flags, L"Root");

        if (!systemStore)
        {
            errorMsg = "CertOpenStore failed with ";
            errorMsg += std::to_string(GetLastError());
            return false;
        }

        PCCERT_CONTEXT certificateIterator = NULL;

        int certificateCount = 0;
        while (certificateIterator = CertEnumCertificatesInStore(systemStore, certificateIterator))
        {
            if (certificateIterator->dwCertEncodingType & X509_ASN_ENCODING)
            {
                int ret = mbedtls_x509_crt_parse(&_cacert,
                                                 certificateIterator->pbCertEncoded,
                                                 certificateIterator->cbCertEncoded);
                if (ret == 0)
                {
                    ++certificateCount;
                }
            }
        }

        CertFreeCertificateContext(certificateIterator);
        CertCloseStore(systemStore, 0);

        if (certificateCount == 0)
        {
            errorMsg = "No certificates found";
            return false;
        }

        return true;
#else
        // On macOS we can query the system cert location from the keychain
        // On Linux we could try to fetch some local files based on the distribution
        // On Android we could use JNI to get to the system certs
        return false;
#endif
    }

    bool SocketMbedTLS::init(const std::string& host, bool isClient, std::string& errMsg)
    {
        initMBedTLS();
        std::lock_guard<std::mutex> lock(_mutex);

        const char* pers = "IXSocketMbedTLS";

        if (mbedtls_ctr_drbg_seed(&_ctr_drbg,
                                  mbedtls_entropy_func,
                                  &_entropy,
                                  (const unsigned char*) pers,
                                  strlen(pers)) != 0)
        {
            errMsg = "Setting entropy seed failed";
            return false;
        }

        if (mbedtls_ssl_config_defaults(&_conf,
                                        (isClient) ? MBEDTLS_SSL_IS_CLIENT : MBEDTLS_SSL_IS_SERVER,
                                        MBEDTLS_SSL_TRANSPORT_STREAM,
                                        MBEDTLS_SSL_PRESET_DEFAULT) != 0)
        {
            errMsg = "Setting config default failed";
            return false;
        }

        mbedtls_ssl_conf_rng(&_conf, mbedtls_ctr_drbg_random, &_ctr_drbg);

        if (_tlsOptions.hasCertAndKey())
        {
            if (mbedtls_x509_crt_parse_file(&_cert, _tlsOptions.certFile.c_str()) < 0)
            {
                errMsg = "Cannot parse cert file '" + _tlsOptions.certFile + "'";
                return false;
            }
#ifdef IXWEBSOCKET_USE_MBED_TLS_MIN_VERSION_3
            if (mbedtls_pk_parse_keyfile(&_pkey, _tlsOptions.keyFile.c_str(), "", mbedtls_ctr_drbg_random, &_ctr_drbg) < 0)
#else
            if (mbedtls_pk_parse_keyfile(&_pkey, _tlsOptions.keyFile.c_str(), "") < 0)
#endif
            {
                errMsg = "Cannot parse key file '" + _tlsOptions.keyFile + "'";
                return false;
            }
            if (mbedtls_ssl_conf_own_cert(&_conf, &_cert, &_pkey) < 0)
            {
                errMsg = "Problem configuring cert '" + _tlsOptions.certFile + "'";
                return false;
            }
        }

        if (_tlsOptions.isPeerVerifyDisabled())
        {
            mbedtls_ssl_conf_authmode(&_conf, MBEDTLS_SSL_VERIFY_NONE);
        }
        else
        {
            // FIXME: should we call mbedtls_ssl_conf_verify ?
            mbedtls_ssl_conf_authmode(&_conf, MBEDTLS_SSL_VERIFY_REQUIRED);

            if (_tlsOptions.isUsingSystemDefaults())
            {
                if (!loadSystemCertificates(errMsg))
                {
                    return false;
                }
            }
            else
            {
                if (_tlsOptions.isUsingInMemoryCAs())
                {
                    const char* buffer = _tlsOptions.caFile.c_str();
                    size_t bufferSize =
                        _tlsOptions.caFile.size() + 1; // Needs to include null terminating
                                                       // character otherwise mbedtls will fail.
                    if (mbedtls_x509_crt_parse(
                            &_cacert, (const unsigned char*) buffer, bufferSize) < 0)
                    {
                        errMsg = "Cannot parse CA from memory.";
                        return false;
                    }
                }
                else if (mbedtls_x509_crt_parse_file(&_cacert, _tlsOptions.caFile.c_str()) < 0)
                {
                    errMsg = "Cannot parse CA file '" + _tlsOptions.caFile + "'";
                    return false;
                }
            }

            mbedtls_ssl_conf_ca_chain(&_conf, &_cacert, NULL);
        }

        if (mbedtls_ssl_setup(&_ssl, &_conf) != 0)
        {
            errMsg = "SSL setup failed";
            return false;
        }

        if (!_tlsOptions.disable_hostname_validation)
        {
            if (!host.empty() && mbedtls_ssl_set_hostname(&_ssl, host.c_str()) != 0)
            {
                errMsg = "SNI setup failed";
                return false;
            }
        }

        return true;
    }

    bool SocketMbedTLS::accept(std::string& errMsg)
    {
        bool isClient = false;
        bool initialized = init(std::string(), isClient, errMsg);
        if (!initialized)
        {
            close();
            return false;
        }

        mbedtls_ssl_set_bio(&_ssl, &_sockfd, mbedtls_net_send, mbedtls_net_recv, NULL);

        int res;
        do
        {
            std::lock_guard<std::mutex> lock(_mutex);
            res = mbedtls_ssl_handshake(&_ssl);
        } while (res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE);

        if (res != 0)
        {
            char buf[256];
            mbedtls_strerror(res, buf, sizeof(buf));

            errMsg = "error in handshake : ";
            errMsg += buf;

            if (res == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED)
            {
                char verifyBuf[512];
                uint32_t flags = mbedtls_ssl_get_verify_result(&_ssl);

                mbedtls_x509_crt_verify_info(verifyBuf, sizeof(verifyBuf), "  ! ", flags);
                errMsg += " : ";
                errMsg += verifyBuf;
            }

            close();
            return false;
        }

        return true;
    }

    bool SocketMbedTLS::connect(const std::string& host,
                                int port,
                                std::string& errMsg,
                                const CancellationRequest& isCancellationRequested)
    {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _sockfd = SocketConnect::connect(host, port, errMsg, isCancellationRequested);
            if (_sockfd == -1) return false;
        }

        bool isClient = true;
        bool initialized = init(host, isClient, errMsg);
        if (!initialized)
        {
            close();
            return false;
        }

        mbedtls_ssl_set_bio(&_ssl, &_sockfd, mbedtls_net_send, mbedtls_net_recv, NULL);

        int res;
        do
        {
            {
                std::lock_guard<std::mutex> lock(_mutex);
                res = mbedtls_ssl_handshake(&_ssl);
            }

            if (isCancellationRequested())
            {
                errMsg = "Cancellation requested";
                close();
                return false;
            }
        } while (res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE);

        if (res != 0)
        {
            char buf[256];
            mbedtls_strerror(res, buf, sizeof(buf));

            errMsg = "error in handshake : ";
            errMsg += buf;

            close();
            return false;
        }

        return true;
    }

    void SocketMbedTLS::close()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        mbedtls_ssl_free(&_ssl);
        mbedtls_ssl_config_free(&_conf);
        mbedtls_ctr_drbg_free(&_ctr_drbg);
        mbedtls_entropy_free(&_entropy);
        mbedtls_x509_crt_free(&_cacert);
        mbedtls_x509_crt_free(&_cert);

        Socket::close();
    }

    ssize_t SocketMbedTLS::send(char* buf, size_t nbyte)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        ssize_t res = mbedtls_ssl_write(&_ssl, (unsigned char*) buf, nbyte);

        if (res > 0)
        {
            return res;
        }
        else if (res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            errno = EWOULDBLOCK;
            return -1;
        }
        else
        {
            return -1;
        }
    }

    ssize_t SocketMbedTLS::recv(void* buf, size_t nbyte)
    {
        while (true)
        {
            std::lock_guard<std::mutex> lock(_mutex);

            ssize_t res = mbedtls_ssl_read(&_ssl, (unsigned char*) buf, (int) nbyte);

            if (res > 0)
            {
                return res;
            }

            if (res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                errno = EWOULDBLOCK;
            }
            return -1;
        }
    }

} // namespace ix

#endif // IXWEBSOCKET_USE_MBED_TLS
