/*
 *  IXGetFreePort.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone. All rights reserved.
 */

// Using inet_addr will trigger an error on uwp without this
// FIXME: use a different api
#ifdef _WIN32
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#endif

#include "IXGetFreePort.h"

#include "IXNetSystem.h"
#include "IXSocket.h"
#include <random>
#include <string>

namespace ix
{
    int getAnyFreePortRandom()
    {
        std::random_device rd;
        std::uniform_int_distribution<int> dist(1024 + 1, 65535);

        return dist(rd);
    }

    int getAnyFreePort()
    {
        socket_t sockfd;
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            return getAnyFreePortRandom();
        }

        int enable = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &enable, sizeof(enable)) < 0)
        {
            return getAnyFreePortRandom();
        }

        // Bind to port 0. This is the standard way to get a free port.
        struct sockaddr_in server; // server address information
        server.sin_family = AF_INET;
        server.sin_port = htons(0);
        server.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (bind(sockfd, (struct sockaddr*) &server, sizeof(server)) < 0)
        {
            Socket::closeSocket(sockfd);
            return getAnyFreePortRandom();
        }

        struct sockaddr_in sa; // server address information
        socklen_t len = sizeof(sa);
        if (getsockname(sockfd, (struct sockaddr*) &sa, &len) < 0)
        {
            Socket::closeSocket(sockfd);
            return getAnyFreePortRandom();
        }

        int port = ntohs(sa.sin_port);
        Socket::closeSocket(sockfd);

        return port;
    }

    int getFreePort()
    {
        while (true)
        {
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
            int port = getAnyFreePortRandom();
#else
            int port = getAnyFreePort();
#endif
#else
            int port = getAnyFreePort();
#endif
            //
            // Only port above 1024 can be used by non root users, but for some
            // reason I got port 7 returned with macOS when binding on port 0...
            //
            if (port > 1024)
            {
                return port;
            }
        }

        return -1;
    }
} // namespace ix
