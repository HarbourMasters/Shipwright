/*
 *  IXNetSystem.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone. All rights reserved.
 */

#pragma once

#include <cstdint>

#ifdef __FreeBSD__
#include <sys/types.h>
#endif

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <ws2tcpip.h>
#include <winsock2.h>
#include <basetsd.h>
#include <io.h>
#include <ws2def.h>
#include <cerrno>

#undef EWOULDBLOCK
#undef EAGAIN
#undef EINPROGRESS
#undef EBADF
#undef EINVAL

// map to WSA error codes
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EAGAIN WSATRY_AGAIN
#define EINPROGRESS WSAEINPROGRESS
#define EBADF WSAEBADF
#define EINVAL WSAEINVAL

// Define our own poll on Windows, as a wrapper on top of select
typedef unsigned long int nfds_t;

// pollfd is not defined by some versions of mingw64 since _WIN32_WINNT is too low
#if _WIN32_WINNT < 0x0600
struct pollfd
{
    int fd;        /* file descriptor */
    short events;  /* requested events */
    short revents; /* returned events */
};

#define POLLIN 0x001   /* There is data to read.  */
#define POLLOUT 0x004  /* Writing now will not block.  */
#define POLLERR 0x008  /* Error condition.  */
#define POLLHUP 0x010  /* Hung up.  */
#define POLLNVAL 0x020 /* Invalid polling request.  */
#endif

#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#endif

namespace ix
{
#ifdef _WIN32
    typedef SOCKET socket_t;
#else
    typedef int socket_t;
#endif

    bool initNetSystem();
    bool uninitNetSystem();

    int poll(struct pollfd* fds, nfds_t nfds, int timeout, void** event);

    const char* inet_ntop(int af, const void* src, char* dst, socklen_t size);
    int inet_pton(int af, const char* src, void* dst);

    unsigned short network_to_host_short(unsigned short value);
} // namespace ix
