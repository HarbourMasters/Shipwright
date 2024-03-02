/*
 *  IXSocketConnect.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 Machine Zone, Inc. All rights reserved.
 */

#include "IXSocketConnect.h"

#include "IXDNSLookup.h"
#include "IXNetSystem.h"
#include "IXSelectInterrupt.h"
#include "IXSocket.h"
#include "IXUniquePtr.h"
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

// Android needs extra headers for TCP_NODELAY and IPPROTO_TCP
#ifdef ANDROID
#include <linux/in.h>
#include <linux/tcp.h>
#endif
#include "IXSelectInterruptFactory.h"

namespace ix
{
    //
    // This function can be cancelled every 50 ms
    // This is important so that we don't block the main UI thread when shutting down a
    // connection which is already trying to reconnect, and can be blocked waiting for
    // ::connect to respond.
    //
    int SocketConnect::connectToAddress(const struct addrinfo* address,
                                        std::string& errMsg,
                                        const CancellationRequest& isCancellationRequested)
    {
        errMsg = "no error";

        socket_t fd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
        if (fd < 0)
        {
            errMsg = "Cannot create a socket";
            return -1;
        }

        // Set the socket to non blocking mode, so that slow responses cannot
        // block us for too long
        SocketConnect::configure(fd);

        int res = ::connect(fd, address->ai_addr, address->ai_addrlen);

        if (res == -1 && !Socket::isWaitNeeded())
        {
            errMsg = strerror(Socket::getErrno());
            Socket::closeSocket(fd);
            return -1;
        }

        for (;;)
        {
            if (isCancellationRequested && isCancellationRequested()) // Must handle timeout as well
            {
                Socket::closeSocket(fd);
                errMsg = "Cancelled";
                return -1;
            }

            int timeoutMs = 10;
            bool readyToRead = false;
            SelectInterruptPtr selectInterrupt = ix::createSelectInterrupt();
            PollResultType pollResult = Socket::poll(readyToRead, timeoutMs, fd, selectInterrupt);

            if (pollResult == PollResultType::Timeout)
            {
                continue;
            }
            else if (pollResult == PollResultType::Error)
            {
                Socket::closeSocket(fd);
                errMsg = std::string("Connect error: ") + strerror(Socket::getErrno());
                return -1;
            }
            else if (pollResult == PollResultType::ReadyForWrite)
            {
                return fd;
            }
            else
            {
                Socket::closeSocket(fd);
                errMsg = std::string("Connect error: ") + strerror(Socket::getErrno());
                return -1;
            }
        }
    }

    int SocketConnect::connect(const std::string& hostname,
                               int port,
                               std::string& errMsg,
                               const CancellationRequest& isCancellationRequested)
    {
        //
        // First do DNS resolution
        //
        auto dnsLookup = std::make_shared<DNSLookup>(hostname, port);
        auto res = dnsLookup->resolve(errMsg, isCancellationRequested);
        if (res == nullptr)
        {
            return -1;
        }

        int sockfd = -1;

        // iterate through the records to find a working peer
        struct addrinfo* address;
        for (address = res.get(); address != nullptr; address = address->ai_next)
        {
            //
            // Second try to connect to the remote host
            //
            sockfd = connectToAddress(address, errMsg, isCancellationRequested);
            if (sockfd != -1)
            {
                break;
            }
        }

        return sockfd;
    }

    // FIXME: configure is a terrible name
    void SocketConnect::configure(int sockfd)
    {
        // 1. disable Nagle's algorithm
        int flag = 1;
        setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag));

        // 2. make socket non blocking
#ifdef _WIN32
        unsigned long nonblocking = 1;
        ioctlsocket(sockfd, FIONBIO, &nonblocking);
#else
        fcntl(sockfd, F_SETFL, O_NONBLOCK); // make socket non blocking
#endif

        // 3. (apple) prevent SIGPIPE from being emitted when the remote end disconnect
#ifdef SO_NOSIGPIPE
        int value = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (void*) &value, sizeof(value));
#endif
    }
} // namespace ix
