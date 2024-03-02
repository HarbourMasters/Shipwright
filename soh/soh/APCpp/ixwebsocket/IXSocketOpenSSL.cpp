/*
 *  IXSocketOpenSSL.cpp
 *  Author: Benjamin Sergeant, Matt DeBoer, Max Weisel
 *  Copyright (c) 2017-2020 Machine Zone, Inc. All rights reserved.
 *
 *  Adapted from Satori SDK OpenSSL code.
 */
#ifdef IXWEBSOCKET_USE_OPEN_SSL

#include "IXSocketOpenSSL.h"

#include "IXSocketConnect.h"
#include "IXUniquePtr.h"
#include <cassert>
#include <errno.h>
#include <vector>
#ifdef _WIN32
#include <shlwapi.h>
#else
#include <fnmatch.h>
#endif
#if OPENSSL_VERSION_NUMBER < 0x10100000L
#include <openssl/x509v3.h>
#endif
#define socketerrno errno

#ifdef _WIN32
// For manipulating the certificate store
#include <wincrypt.h>
#endif

#ifdef _WIN32
namespace
{
    bool loadWindowsSystemCertificates(SSL_CTX* ssl, std::string& errorMsg)
    {
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
        X509_STORE* opensslStore = SSL_CTX_get_cert_store(ssl);

        int certificateCount = 0;
        while (certificateIterator = CertEnumCertificatesInStore(systemStore, certificateIterator))
        {
            X509* x509 = d2i_X509(NULL,
                                  (const unsigned char**) &certificateIterator->pbCertEncoded,
                                  certificateIterator->cbCertEncoded);

            if (x509)
            {
                if (X509_STORE_add_cert(opensslStore, x509) == 1)
                {
                    ++certificateCount;
                }

                X509_free(x509);
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
    }
} // namespace
#endif

namespace ix
{
    const std::string kDefaultCiphers =
        "ECDHE-ECDSA-AES128-GCM-SHA256 ECDHE-ECDSA-AES256-GCM-SHA384 ECDHE-ECDSA-AES128-SHA "
        "ECDHE-ECDSA-AES256-SHA ECDHE-ECDSA-AES128-SHA256 ECDHE-ECDSA-AES256-SHA384 "
        "ECDHE-RSA-AES128-GCM-SHA256 ECDHE-RSA-AES256-GCM-SHA384 ECDHE-RSA-AES128-SHA "
        "ECDHE-RSA-AES256-SHA ECDHE-RSA-AES128-SHA256 ECDHE-RSA-AES256-SHA384 "
        "DHE-RSA-AES128-GCM-SHA256 DHE-RSA-AES256-GCM-SHA384 DHE-RSA-AES128-SHA "
        "DHE-RSA-AES256-SHA DHE-RSA-AES128-SHA256 DHE-RSA-AES256-SHA256 AES128-SHA";

    std::atomic<bool> SocketOpenSSL::_openSSLInitializationSuccessful(false);
    std::once_flag SocketOpenSSL::_openSSLInitFlag;
    std::vector<std::unique_ptr<std::mutex>> openSSLMutexes;

    SocketOpenSSL::SocketOpenSSL(const SocketTLSOptions& tlsOptions, int fd)
        : Socket(fd)
        , _ssl_connection(nullptr)
        , _ssl_context(nullptr)
        , _tlsOptions(tlsOptions)
    {
        std::call_once(_openSSLInitFlag, &SocketOpenSSL::openSSLInitialize, this);
    }

    SocketOpenSSL::~SocketOpenSSL()
    {
        SocketOpenSSL::close();
    }

    void SocketOpenSSL::openSSLInitialize()
    {
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
        if (!OPENSSL_init_ssl(OPENSSL_INIT_LOAD_CONFIG, nullptr)) return;
#else
        (void) OPENSSL_config(nullptr);

        if (CRYPTO_get_locking_callback() == nullptr)
        {
            openSSLMutexes.clear();
            for (int i = 0; i < CRYPTO_num_locks(); ++i)
            {
                openSSLMutexes.push_back(ix::make_unique<std::mutex>());
            }
            CRYPTO_set_locking_callback(SocketOpenSSL::openSSLLockingCallback);
        }
#endif

        (void) OpenSSL_add_ssl_algorithms();
        (void) SSL_load_error_strings();

        _openSSLInitializationSuccessful = true;
    }

    void SocketOpenSSL::openSSLLockingCallback(int mode,
                                               int type,
                                               const char* /*file*/,
                                               int /*line*/)
    {
        if (mode & CRYPTO_LOCK)
        {
            openSSLMutexes[type]->lock();
        }
        else
        {
            openSSLMutexes[type]->unlock();
        }
    }

    std::string SocketOpenSSL::getSSLError(int ret)
    {
        unsigned long e;

        int err = SSL_get_error(_ssl_connection, ret);

        if (err == SSL_ERROR_WANT_CONNECT || err == SSL_ERROR_WANT_ACCEPT)
        {
            return "OpenSSL failed - connection failure";
        }
        else if (err == SSL_ERROR_WANT_X509_LOOKUP)
        {
            return "OpenSSL failed - x509 error";
        }
        else if (err == SSL_ERROR_SYSCALL)
        {
            e = ERR_get_error();
            if (e > 0)
            {
                std::string errMsg("OpenSSL failed - ");
                errMsg += ERR_error_string(e, nullptr);
                return errMsg;
            }
            else if (e == 0 && ret == 0)
            {
                return "OpenSSL failed - received early EOF";
            }
            else
            {
                return "OpenSSL failed - underlying BIO reported an I/O error";
            }
        }
        else if (err == SSL_ERROR_SSL)
        {
            e = ERR_get_error();
            std::string errMsg("OpenSSL failed - ");
            errMsg += ERR_error_string(e, nullptr);
            return errMsg;
        }
        else if (err == SSL_ERROR_NONE)
        {
            return "OpenSSL failed - err none";
        }
        else if (err == SSL_ERROR_ZERO_RETURN)
        {
            return "OpenSSL failed - err zero return";
        }
        else
        {
            return "OpenSSL failed - unknown error";
        }
    }

    SSL_CTX* SocketOpenSSL::openSSLCreateContext(std::string& errMsg)
    {
        const SSL_METHOD* method = SSLv23_client_method();
        if (method == nullptr)
        {
            errMsg = "SSLv23_client_method failure";
            return nullptr;
        }
        _ssl_method = method;

        SSL_CTX* ctx = SSL_CTX_new(_ssl_method);
        if (ctx)
        {
            SSL_CTX_set_mode(ctx,
                             SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

            int options = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_CIPHER_SERVER_PREFERENCE;

#ifdef SSL_OP_NO_TLSv1_3
            // (partially?) work around hang in openssl 1.1.1b, by disabling TLS V1.3
            // https://github.com/openssl/openssl/issues/7967
            options |= SSL_OP_NO_TLSv1_3;
#endif
            SSL_CTX_set_options(ctx, options);
        }
        return ctx;
    }

    bool SocketOpenSSL::openSSLAddCARootsFromString(const std::string roots)
    {
        // Create certificate store
        X509_STORE* certificate_store = SSL_CTX_get_cert_store(_ssl_context);
        if (certificate_store == nullptr) return false;

        // Configure to allow intermediate certs
        X509_STORE_set_flags(certificate_store,
                             X509_V_FLAG_TRUSTED_FIRST | X509_V_FLAG_PARTIAL_CHAIN);

        // Create a new buffer and populate it with the roots
        BIO* buffer = BIO_new_mem_buf((void*) roots.c_str(), static_cast<int>(roots.length()));
        if (buffer == nullptr) return false;

        // Read each root in the buffer and add to the certificate store
        bool success = true;
        size_t number_of_roots = 0;

        while (true)
        {
            // Read the next root in the buffer
            X509* root = PEM_read_bio_X509_AUX(buffer, nullptr, nullptr, (void*) "");
            if (root == nullptr)
            {
                // No more certs left in the buffer, we're done.
                ERR_clear_error();
                break;
            }

            // Try adding the root to the certificate store
            ERR_clear_error();
            if (!X509_STORE_add_cert(certificate_store, root))
            {
                // Failed to add. If the error is unrelated to the x509 lib or the cert already
                // exists, we're safe to continue.
                unsigned long error = ERR_get_error();
                if (ERR_GET_LIB(error) != ERR_LIB_X509 ||
                    ERR_GET_REASON(error) != X509_R_CERT_ALREADY_IN_HASH_TABLE)
                {
                    // Failed. Clean up and bail.
                    success = false;
                    X509_free(root);
                    break;
                }
            }

            // Clean up and loop
            X509_free(root);
            number_of_roots++;
        }

        // Clean up buffer
        BIO_free(buffer);

        // Make sure we loaded at least one certificate.
        if (number_of_roots == 0) success = false;

        return success;
    }

    /**
     * Check whether a hostname matches a pattern
     */
    bool SocketOpenSSL::checkHost(const std::string& host, const char* pattern)
    {
#ifdef _WIN32
        return PathMatchSpecA(host.c_str(), pattern);
#else
        return fnmatch(pattern, host.c_str(), 0) != FNM_NOMATCH;
#endif
    }

    bool SocketOpenSSL::openSSLCheckServerCert(SSL* ssl,
#if OPENSSL_VERSION_NUMBER < 0x10100000L
                                               const std::string& hostname,
#else
                                               const std::string& /* hostname */,
#endif
                                               std::string& errMsg)
    {
        X509* server_cert = SSL_get_peer_certificate(ssl);
        if (server_cert == nullptr)
        {
            errMsg = "OpenSSL failed - peer didn't present a X509 certificate.";
            return false;
        }

#if OPENSSL_VERSION_NUMBER < 0x10100000L
        // Check server name
        bool hostname_verifies_ok = false;
        STACK_OF(GENERAL_NAME)* san_names = (STACK_OF(GENERAL_NAME)*) X509_get_ext_d2i(
            (X509*) server_cert, NID_subject_alt_name, NULL, NULL);
        if (san_names)
        {
            for (int i = 0; i < sk_GENERAL_NAME_num(san_names); i++)
            {
                const GENERAL_NAME* sk_name = sk_GENERAL_NAME_value(san_names, i);
                if (sk_name->type == GEN_DNS)
                {
                    char* name = (char*) ASN1_STRING_data(sk_name->d.dNSName);
                    if ((size_t) ASN1_STRING_length(sk_name->d.dNSName) == strlen(name) &&
                        checkHost(hostname, name))
                    {
                        hostname_verifies_ok = true;
                        break;
                    }
                }
            }
        }
        sk_GENERAL_NAME_pop_free(san_names, GENERAL_NAME_free);

        if (!hostname_verifies_ok)
        {
            int cn_pos = X509_NAME_get_index_by_NID(
                X509_get_subject_name((X509*) server_cert), NID_commonName, -1);
            if (cn_pos >= 0)
            {
                X509_NAME_ENTRY* cn_entry =
                    X509_NAME_get_entry(X509_get_subject_name((X509*) server_cert), cn_pos);

                if (cn_entry != nullptr)
                {
                    ASN1_STRING* cn_asn1 = X509_NAME_ENTRY_get_data(cn_entry);
                    char* cn = (char*) ASN1_STRING_data(cn_asn1);

                    if ((size_t) ASN1_STRING_length(cn_asn1) == strlen(cn) &&
                        checkHost(hostname, cn))
                    {
                        hostname_verifies_ok = true;
                    }
                }
            }
        }

        if (!hostname_verifies_ok)
        {
            errMsg = "OpenSSL failed - certificate was issued for a different domain.";
            return false;
        }
#endif

        X509_free(server_cert);
        return true;
    }

    bool SocketOpenSSL::openSSLClientHandshake(const std::string& host,
                                               std::string& errMsg,
                                               const CancellationRequest& isCancellationRequested)
    {
        while (true)
        {
            if (_ssl_connection == nullptr || _ssl_context == nullptr)
            {
                return false;
            }

            if (isCancellationRequested())
            {
                errMsg = "Cancellation requested";
                return false;
            }

            ERR_clear_error();
            int connect_result = SSL_connect(_ssl_connection);
            if (connect_result == 1)
            {
                if (_tlsOptions.disable_hostname_validation)
                {
                    return true;
                }

                return openSSLCheckServerCert(_ssl_connection, host, errMsg);
            }
            int reason = SSL_get_error(_ssl_connection, connect_result);

            bool rc = false;
            if (reason == SSL_ERROR_WANT_READ || reason == SSL_ERROR_WANT_WRITE)
            {
                rc = true;
            }
            else
            {
                errMsg = getSSLError(connect_result);
                rc = false;
            }

            if (!rc)
            {
                return false;
            }
        }
    }

    bool SocketOpenSSL::openSSLServerHandshake(std::string& errMsg)
    {
        while (true)
        {
            if (_ssl_connection == nullptr || _ssl_context == nullptr)
            {
                return false;
            }

            ERR_clear_error();
            int accept_result = SSL_accept(_ssl_connection);
            if (accept_result == 1)
            {
                return true;
            }
            int reason = SSL_get_error(_ssl_connection, accept_result);

            bool rc = false;
            if (reason == SSL_ERROR_WANT_READ || reason == SSL_ERROR_WANT_WRITE)
            {
                rc = true;
            }
            else
            {
                errMsg = getSSLError(accept_result);
                rc = false;
            }

            if (!rc)
            {
                return false;
            }
        }
    }

    bool SocketOpenSSL::handleTLSOptions(std::string& errMsg)
    {
        ERR_clear_error();
        if (_tlsOptions.hasCertAndKey())
        {
            if (SSL_CTX_use_certificate_chain_file(_ssl_context, _tlsOptions.certFile.c_str()) != 1)
            {
                auto sslErr = ERR_get_error();
                errMsg = "OpenSSL failed - SSL_CTX_use_certificate_chain_file(\"" +
                         _tlsOptions.certFile + "\") failed: ";
                errMsg += ERR_error_string(sslErr, nullptr);
            }
            else if (SSL_CTX_use_PrivateKey_file(
                         _ssl_context, _tlsOptions.keyFile.c_str(), SSL_FILETYPE_PEM) != 1)
            {
                auto sslErr = ERR_get_error();
                errMsg = "OpenSSL failed - SSL_CTX_use_PrivateKey_file(\"" + _tlsOptions.keyFile +
                         "\") failed: ";
                errMsg += ERR_error_string(sslErr, nullptr);
            }
            else if (!SSL_CTX_check_private_key(_ssl_context))
            {
                auto sslErr = ERR_get_error();
                errMsg = "OpenSSL failed - cert/key mismatch(\"" + _tlsOptions.certFile + ", " +
                         _tlsOptions.keyFile + "\")";
                errMsg += ERR_error_string(sslErr, nullptr);
            }
        }

        ERR_clear_error();
        if (!_tlsOptions.isPeerVerifyDisabled())
        {
            if (_tlsOptions.isUsingSystemDefaults())
            {
#ifdef _WIN32
                if (!loadWindowsSystemCertificates(_ssl_context, errMsg))
                {
                    return false;
                }
#else
                if (SSL_CTX_set_default_verify_paths(_ssl_context) == 0)
                {
                    auto sslErr = ERR_get_error();
                    errMsg = "OpenSSL failed - SSL_CTX_default_verify_paths loading failed: ";
                    errMsg += ERR_error_string(sslErr, nullptr);
                    return false;
                }
#endif
            }
            else
            {
                if (_tlsOptions.isUsingInMemoryCAs())
                {
                    // Load from memory
                    openSSLAddCARootsFromString(_tlsOptions.caFile);
                }
                else
                {
                    if (SSL_CTX_load_verify_locations(
                            _ssl_context, _tlsOptions.caFile.c_str(), NULL) != 1)
                    {
                        auto sslErr = ERR_get_error();
                        errMsg = "OpenSSL failed - SSL_CTX_load_verify_locations(\"" +
                                 _tlsOptions.caFile + "\") failed: ";
                        errMsg += ERR_error_string(sslErr, nullptr);
                        return false;
                    }
                }
            }

            SSL_CTX_set_verify(_ssl_context,
                               SSL_VERIFY_PEER,
                               [](int preverify, X509_STORE_CTX*) -> int { return preverify; });
            SSL_CTX_set_verify_depth(_ssl_context, 4);
        }
        else
        {
            SSL_CTX_set_verify(_ssl_context, SSL_VERIFY_NONE, nullptr);
        }

        if (_tlsOptions.isUsingDefaultCiphers())
        {
            if (SSL_CTX_set_cipher_list(_ssl_context, kDefaultCiphers.c_str()) != 1)
            {
                auto sslErr = ERR_get_error();
                errMsg = "OpenSSL failed - SSL_CTX_set_cipher_list(\"" + kDefaultCiphers +
                         "\") failed: ";
                errMsg += ERR_error_string(sslErr, nullptr);
                return false;
            }
        }
        else if (SSL_CTX_set_cipher_list(_ssl_context, _tlsOptions.ciphers.c_str()) != 1)
        {
            auto sslErr = ERR_get_error();
            errMsg = "OpenSSL failed - SSL_CTX_set_cipher_list(\"" + _tlsOptions.ciphers +
                     "\") failed: ";
            errMsg += ERR_error_string(sslErr, nullptr);
            return false;
        }

        return true;
    }

    bool SocketOpenSSL::accept(std::string& errMsg)
    {
        bool handshakeSuccessful = false;
        {
            std::lock_guard<std::mutex> lock(_mutex);

            if (!_openSSLInitializationSuccessful)
            {
                errMsg = "OPENSSL_init_ssl failure";
                return false;
            }

            if (_sockfd == -1)
            {
                return false;
            }

            {
                const SSL_METHOD* method = SSLv23_server_method();
                if (method == nullptr)
                {
                    errMsg = "SSLv23_server_method failure";
                    _ssl_context = nullptr;
                }
                else
                {
                    _ssl_method = method;

                    _ssl_context = SSL_CTX_new(_ssl_method);
                    if (_ssl_context)
                    {
                        SSL_CTX_set_mode(_ssl_context, SSL_MODE_ENABLE_PARTIAL_WRITE);
                        SSL_CTX_set_mode(_ssl_context, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
                        SSL_CTX_set_options(_ssl_context,
                                            SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
                    }
                }
            }

            if (_ssl_context == nullptr)
            {
                return false;
            }

            ERR_clear_error();
            if (_tlsOptions.hasCertAndKey())
            {
                if (SSL_CTX_use_certificate_chain_file(_ssl_context,
                                                       _tlsOptions.certFile.c_str()) != 1)
                {
                    auto sslErr = ERR_get_error();
                    errMsg = "OpenSSL failed - SSL_CTX_use_certificate_chain_file(\"" +
                             _tlsOptions.certFile + "\") failed: ";
                    errMsg += ERR_error_string(sslErr, nullptr);
                }
                else if (SSL_CTX_use_PrivateKey_file(
                             _ssl_context, _tlsOptions.keyFile.c_str(), SSL_FILETYPE_PEM) != 1)
                {
                    auto sslErr = ERR_get_error();
                    errMsg = "OpenSSL failed - SSL_CTX_use_PrivateKey_file(\"" +
                             _tlsOptions.keyFile + "\") failed: ";
                    errMsg += ERR_error_string(sslErr, nullptr);
                }
            }


            ERR_clear_error();
            if (!_tlsOptions.isPeerVerifyDisabled())
            {
                if (_tlsOptions.isUsingSystemDefaults())
                {
                    if (SSL_CTX_set_default_verify_paths(_ssl_context) == 0)
                    {
                        auto sslErr = ERR_get_error();
                        errMsg = "OpenSSL failed - SSL_CTX_default_verify_paths loading failed: ";
                        errMsg += ERR_error_string(sslErr, nullptr);
                    }
                }
                else
                {
                    if (_tlsOptions.isUsingInMemoryCAs())
                    {
                        // Load from memory
                        openSSLAddCARootsFromString(_tlsOptions.caFile);
                    }
                    else
                    {
                        const char* root_ca_file = _tlsOptions.caFile.c_str();
                        STACK_OF(X509_NAME) * rootCAs;
                        rootCAs = SSL_load_client_CA_file(root_ca_file);
                        if (rootCAs == NULL)
                        {
                            auto sslErr = ERR_get_error();
                            errMsg = "OpenSSL failed - SSL_load_client_CA_file('" +
                                     _tlsOptions.caFile + "') failed: ";
                            errMsg += ERR_error_string(sslErr, nullptr);
                        }
                        else
                        {
                            SSL_CTX_set_client_CA_list(_ssl_context, rootCAs);
                            if (SSL_CTX_load_verify_locations(
                                    _ssl_context, root_ca_file, nullptr) != 1)
                            {
                                auto sslErr = ERR_get_error();
                                errMsg = "OpenSSL failed - SSL_CTX_load_verify_locations(\"" +
                                         _tlsOptions.caFile + "\") failed: ";
                                errMsg += ERR_error_string(sslErr, nullptr);
                            }
                        }
                    }
                }

                SSL_CTX_set_verify(
                    _ssl_context, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
                SSL_CTX_set_verify_depth(_ssl_context, 4);
            }
            else
            {
                SSL_CTX_set_verify(_ssl_context, SSL_VERIFY_NONE, nullptr);
            }
            if (_tlsOptions.isUsingDefaultCiphers())
            {
                if (SSL_CTX_set_cipher_list(_ssl_context, kDefaultCiphers.c_str()) != 1)
                {
                    return false;
                }
            }
            else if (SSL_CTX_set_cipher_list(_ssl_context, _tlsOptions.ciphers.c_str()) != 1)
            {
                return false;
            }

            _ssl_connection = SSL_new(_ssl_context);
            if (_ssl_connection == nullptr)
            {
                errMsg = "OpenSSL failed to connect";
                SSL_CTX_free(_ssl_context);
                _ssl_context = nullptr;
                return false;
            }

            SSL_set_ecdh_auto(_ssl_connection, 1);

            SSL_set_fd(_ssl_connection, _sockfd);

            handshakeSuccessful = openSSLServerHandshake(errMsg);
        }

        if (!handshakeSuccessful)
        {
            close();
            return false;
        }

        return true;
    }

    bool SocketOpenSSL::connect(const std::string& host,
                                int port,
                                std::string& errMsg,
                                const CancellationRequest& isCancellationRequested)
    {
        bool handshakeSuccessful = false;
        {
            std::lock_guard<std::mutex> lock(_mutex);

            if (!_openSSLInitializationSuccessful)
            {
                errMsg = "OPENSSL_init_ssl failure";
                return false;
            }

            _sockfd = SocketConnect::connect(host, port, errMsg, isCancellationRequested);
            if (_sockfd == -1) return false;

            _ssl_context = openSSLCreateContext(errMsg);
            if (_ssl_context == nullptr)
            {
                return false;
            }

            if (!handleTLSOptions(errMsg))
            {
                return false;
            }

            _ssl_connection = SSL_new(_ssl_context);
            if (_ssl_connection == nullptr)
            {
                errMsg = "OpenSSL failed to connect";
                SSL_CTX_free(_ssl_context);
                _ssl_context = nullptr;
                return false;
            }
            SSL_set_fd(_ssl_connection, _sockfd);

            // SNI support
            SSL_set_tlsext_host_name(_ssl_connection, host.c_str());

#if OPENSSL_VERSION_NUMBER >= 0x10002000L
            // Support for server name verification
            // (The docs say that this should work from 1.0.2, and is the default from
            // 1.1.0, but it does not. To be on the safe side, the manual test
            // below is enabled for all versions prior to 1.1.0.)
            if (!_tlsOptions.disable_hostname_validation)
            {
                X509_VERIFY_PARAM* param = SSL_get0_param(_ssl_connection);
                X509_VERIFY_PARAM_set1_host(param, host.c_str(), host.size());
            }
#endif
            handshakeSuccessful = openSSLClientHandshake(host, errMsg, isCancellationRequested);
        }

        if (!handshakeSuccessful)
        {
            close();
            return false;
        }

        return true;
    }

    void SocketOpenSSL::close()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_ssl_connection != nullptr)
        {
            SSL_free(_ssl_connection);
            _ssl_connection = nullptr;
        }
        if (_ssl_context != nullptr)
        {
            SSL_CTX_free(_ssl_context);
            _ssl_context = nullptr;
        }

        Socket::close();
    }

    ssize_t SocketOpenSSL::send(char* buf, size_t nbyte)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_ssl_connection == nullptr || _ssl_context == nullptr)
        {
            return 0;
        }

        ERR_clear_error();
        ssize_t write_result = SSL_write(_ssl_connection, buf, (int) nbyte);
        int reason = SSL_get_error(_ssl_connection, (int) write_result);

        if (reason == SSL_ERROR_NONE)
        {
            return write_result;
        }
        else if (reason == SSL_ERROR_WANT_READ || reason == SSL_ERROR_WANT_WRITE)
        {
            errno = EWOULDBLOCK;
            return -1;
        }
        else
        {
            return -1;
        }
    }

    ssize_t SocketOpenSSL::recv(void* buf, size_t nbyte)
    {
        while (true)
        {
            std::lock_guard<std::mutex> lock(_mutex);

            if (_ssl_connection == nullptr || _ssl_context == nullptr)
            {
                return 0;
            }

            ERR_clear_error();
            ssize_t read_result = SSL_read(_ssl_connection, buf, (int) nbyte);

            if (read_result > 0)
            {
                return read_result;
            }

            int reason = SSL_get_error(_ssl_connection, (int) read_result);

            if (reason == SSL_ERROR_WANT_READ || reason == SSL_ERROR_WANT_WRITE)
            {
                errno = EWOULDBLOCK;
            }
            return -1;
        }
    }

} // namespace ix

#endif // IXWEBSOCKET_USE_OPEN_SSL
