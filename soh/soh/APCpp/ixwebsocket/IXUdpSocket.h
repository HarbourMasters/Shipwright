/*
 *  IXUdpSocket.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2020 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include <atomic>
#include <memory>
#include <string>

#ifdef _WIN32
#include <basetsd.h>
#ifdef _MSC_VER
typedef SSIZE_T ssize_t;
#endif
#endif

#include "IXNetSystem.h"

namespace ix
{
    class UdpSocket
    {
    public:
        UdpSocket(int fd = -1);
        ~UdpSocket();

        // Virtual methods
        bool init(const std::string& host, int port, std::string& errMsg);
        ssize_t sendto(const std::string& buffer);
        ssize_t recvfrom(char* buffer, size_t length);

        void close();

        static int getErrno();
        static bool isWaitNeeded();
        static void closeSocket(int fd);

    private:
        std::atomic<int> _sockfd;
        struct sockaddr_in _server;
    };
} // namespace ix
