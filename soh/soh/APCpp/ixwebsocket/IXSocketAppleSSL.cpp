/*
 *  IXSocketAppleSSL.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2020 Machine Zone, Inc. All rights reserved.
 *
 *  Adapted from Satori SDK Apple SSL code.
 */
#ifdef IXWEBSOCKET_USE_SECURE_TRANSPORT

#include "IXSocketAppleSSL.h"

#include "IXSocketConnect.h"
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#define socketerrno errno

#include <Security/SecureTransport.h>

namespace ix
{
    SocketAppleSSL::SocketAppleSSL(const SocketTLSOptions& tlsOptions, int fd)
        : Socket(fd)
        , _sslContext(nullptr)
        , _tlsOptions(tlsOptions)
    {
        ;
    }

    SocketAppleSSL::~SocketAppleSSL()
    {
        SocketAppleSSL::close();
    }

    std::string SocketAppleSSL::getSSLErrorDescription(OSStatus status)
    {
        std::string errMsg("Unknown SSL error.");

        CFErrorRef error = CFErrorCreate(kCFAllocatorDefault, kCFErrorDomainOSStatus, status, NULL);
        if (error)
        {
            CFStringRef message = CFErrorCopyDescription(error);
            if (message)
            {
                char localBuffer[128];
                Boolean success;
                success = CFStringGetCString(message, localBuffer, 128, kCFStringEncodingUTF8);
                if (success)
                {
                    errMsg = localBuffer;
                }
                CFRelease(message);
            }
            CFRelease(error);
        }

        return errMsg;
    }

    OSStatus SocketAppleSSL::readFromSocket(SSLConnectionRef connection, void* data, size_t* len)
    {
        int fd = (int) (long) connection;
        if (fd < 0) return errSSLInternal;

        assert(data != nullptr);
        assert(len != nullptr);

        size_t requested_sz = *len;

        ssize_t status = read(fd, data, requested_sz);

        if (status > 0)
        {
            *len = (size_t) status;
            if (requested_sz > *len)
            {
                return errSSLWouldBlock;
            }
            else
            {
                return noErr;
            }
        }
        else if (status == 0)
        {
            *len = 0;
            return errSSLClosedGraceful;
        }
        else
        {
            *len = 0;
            switch (errno)
            {
                case ENOENT: return errSSLClosedGraceful;

                case EAGAIN: return errSSLWouldBlock; // EWOULDBLOCK is a define for EAGAIN on osx
                case EINPROGRESS: return errSSLWouldBlock;

                case ECONNRESET: return errSSLClosedAbort;

                default: return errSecIO;
            }
        }
    }

    OSStatus SocketAppleSSL::writeToSocket(SSLConnectionRef connection,
                                           const void* data,
                                           size_t* len)
    {
        int fd = (int) (long) connection;
        if (fd < 0) return errSSLInternal;

        assert(data != nullptr);
        assert(len != nullptr);

        size_t to_write_sz = *len;
        ssize_t status = write(fd, data, to_write_sz);

        if (status > 0)
        {
            *len = (size_t) status;
            if (to_write_sz > *len)
            {
                return errSSLWouldBlock;
            }
            else
            {
                return noErr;
            }
        }
        else if (status == 0)
        {
            *len = 0;
            return errSSLClosedGraceful;
        }
        else
        {
            *len = 0;
            switch (errno)
            {
                case ENOENT: return errSSLClosedGraceful;

                case EAGAIN: return errSSLWouldBlock; // EWOULDBLOCK is a define for EAGAIN on osx
                case EINPROGRESS: return errSSLWouldBlock;

                case ECONNRESET: return errSSLClosedAbort;

                default: return errSecIO;
            }
        }
    }


    bool SocketAppleSSL::accept(std::string& errMsg)
    {
        errMsg = "TLS not supported yet in server mode with apple ssl backend";
        return false;
    }

    OSStatus SocketAppleSSL::tlsHandShake(std::string& errMsg,
                                          const CancellationRequest& isCancellationRequested)
    {
        OSStatus status;

        do
        {
            status = SSLHandshake(_sslContext);

            // Interrupt the handshake
            if (isCancellationRequested())
            {
                errMsg = "Cancellation requested";
                return errSSLInternal;
            }
        } while (status == errSSLWouldBlock || status == errSSLServerAuthCompleted);

        return status;
    }

    // No wait support
    bool SocketAppleSSL::connect(const std::string& host,
                                 int port,
                                 std::string& errMsg,
                                 const CancellationRequest& isCancellationRequested)
    {
        OSStatus status;
        {
            std::lock_guard<std::mutex> lock(_mutex);

            _sockfd = SocketConnect::connect(host, port, errMsg, isCancellationRequested);
            if (_sockfd == -1) return false;

            _sslContext = SSLCreateContext(kCFAllocatorDefault, kSSLClientSide, kSSLStreamType);

            SSLSetIOFuncs(
                _sslContext, SocketAppleSSL::readFromSocket, SocketAppleSSL::writeToSocket);
            SSLSetConnection(_sslContext, (SSLConnectionRef)(long) _sockfd);
            SSLSetProtocolVersionMin(_sslContext, kTLSProtocol12);

            if (!_tlsOptions.disable_hostname_validation)
                SSLSetPeerDomainName(_sslContext, host.c_str(), host.size());

            if (_tlsOptions.isPeerVerifyDisabled())
            {
                Boolean option(1);
                SSLSetSessionOption(_sslContext, kSSLSessionOptionBreakOnServerAuth, option);

                status = tlsHandShake(errMsg, isCancellationRequested);

                if (status == errSSLServerAuthCompleted)
                {
                    // proceed with the handshake
                    status = tlsHandShake(errMsg, isCancellationRequested);
                }
            }
            else
            {
                status = tlsHandShake(errMsg, isCancellationRequested);
            }
        }

        if (status != noErr)
        {
            errMsg = getSSLErrorDescription(status);
            close();
            return false;
        }

        return true;
    }

    void SocketAppleSSL::close()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_sslContext == nullptr) return;

        SSLClose(_sslContext);
        CFRelease(_sslContext);
        _sslContext = nullptr;

        Socket::close();
    }

    ssize_t SocketAppleSSL::send(char* buf, size_t nbyte)
    {
        OSStatus status = errSSLWouldBlock;
        while (status == errSSLWouldBlock)
        {
            size_t processed = 0;
            std::lock_guard<std::mutex> lock(_mutex);
            status = SSLWrite(_sslContext, buf, nbyte, &processed);

            if (processed > 0) return (ssize_t) processed;

            // The connection was reset, inform the caller that this
            // Socket should close
            if (status == errSSLClosedGraceful || status == errSSLClosedNoNotify ||
                status == errSSLClosedAbort)
            {
                errno = ECONNRESET;
                return -1;
            }

            if (status == errSSLWouldBlock)
            {
                errno = EWOULDBLOCK;
                return -1;
            }
        }
        return -1;
    }

    // No wait support
    ssize_t SocketAppleSSL::recv(void* buf, size_t nbyte)
    {
        OSStatus status = errSSLWouldBlock;
        while (status == errSSLWouldBlock)
        {
            size_t processed = 0;
            std::lock_guard<std::mutex> lock(_mutex);
            status = SSLRead(_sslContext, buf, nbyte, &processed);

            if (processed > 0) return (ssize_t) processed;

            // The connection was reset, inform the caller that this
            // Socket should close
            if (status == errSSLClosedGraceful || status == errSSLClosedNoNotify ||
                status == errSSLClosedAbort)
            {
                errno = ECONNRESET;
                return -1;
            }

            if (status == errSSLWouldBlock)
            {
                errno = EWOULDBLOCK;
                return -1;
            }
        }
        return -1;
    }

} // namespace ix

#endif // IXWEBSOCKET_USE_SECURE_TRANSPORT
