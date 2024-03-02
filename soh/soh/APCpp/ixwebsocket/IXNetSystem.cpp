/*
 *  IXNetSystem.cpp
 *  Author: Korchynskyi Dmytro
 *  Copyright (c) 2019 Machine Zone. All rights reserved.
 */

#include "IXNetSystem.h"
#include <cstdint>
#include <cstdio>
#ifdef _WIN32
#ifndef EAFNOSUPPORT
  #define EAFNOSUPPORT 102
#endif
#ifndef ENOSPC
  #define ENOSPC 28
#endif
#include <vector>
#endif

namespace ix
{
    bool initNetSystem()
    {
#ifdef _WIN32
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        // Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
        wVersionRequested = MAKEWORD(2, 2);
        err = WSAStartup(wVersionRequested, &wsaData);

        return err == 0;
#else
        return true;
#endif
    }

    bool uninitNetSystem()
    {
#ifdef _WIN32
        int err = WSACleanup();
        return err == 0;
#else
        return true;
#endif
    }

#ifdef _WIN32
    struct WSAEvent
    {
    public:
        WSAEvent(struct pollfd* fd)
            : _fd(fd)
        {
            _event = WSACreateEvent();
        }

        WSAEvent(WSAEvent&& source) noexcept
        {
            _event = source._event;
            source._event = WSA_INVALID_EVENT; // invalidate the event in the source
            _fd = source._fd;
        }

        ~WSAEvent()
        {
            if (_event != WSA_INVALID_EVENT)
            {
                // We must deselect the networkevents from the socket event. Otherwise the
                // socket will report states that aren't there.
                if (_fd != nullptr && _fd->fd != -1)
                    WSAEventSelect(_fd->fd, _event, 0);
                WSACloseEvent(_event);
            }
        }

        operator HANDLE()
        {
            return _event;
        }

        operator struct pollfd*()
        {
            return _fd;
        }

    private:
        HANDLE _event;
        struct pollfd* _fd;
    };
#endif

    //
    // That function could 'return WSAPoll(pfd, nfds, timeout);'
    // but WSAPoll is said to have weird behaviors on the internet
    // (the curl folks have had problems with it).
    //
    // So we make it a select wrapper
    //
    // UPDATE: WSAPoll was fixed in Windows 10 Version 2004
    //
    // The optional "event" is set to nullptr if it wasn't signaled.
    int poll(struct pollfd* fds, nfds_t nfds, int timeout, void** event)
    {
#ifdef _WIN32

        if (event && *event)
        {
            HANDLE interruptEvent = reinterpret_cast<HANDLE>(*event);
            *event = nullptr; // the event wasn't signaled yet

            if (nfds < 0 || nfds >= MAXIMUM_WAIT_OBJECTS - 1)
            {
                WSASetLastError(WSAEINVAL);
                return SOCKET_ERROR;
            }

            std::vector<WSAEvent> socketEvents;
            std::vector<HANDLE> handles;
            // put the interrupt event as first element, making it highest priority
            handles.push_back(interruptEvent);

            // create the WSAEvents for the sockets
            for (nfds_t i = 0; i < nfds; ++i)
            {
                struct pollfd* fd = &fds[i];
                fd->revents = 0;
                if (fd->fd >= 0)
                {
                    // create WSAEvent and add it to the vectors
                    socketEvents.push_back(std::move(WSAEvent(fd)));
                    HANDLE handle = socketEvents.back();
                    if (handle == WSA_INVALID_EVENT)
                    {
                        WSASetLastError(WSAENOBUFS);
                        return SOCKET_ERROR;
                    }
                    handles.push_back(handle);

                    // mapping
                    long networkEvents = 0;
                    if (fd->events & (POLLIN                               )) networkEvents |= FD_READ  | FD_ACCEPT;
                    if (fd->events & (POLLOUT /*| POLLWRNORM | POLLWRBAND*/)) networkEvents |= FD_WRITE | FD_CONNECT;
                    //if (fd->events & (POLLPRI | POLLRDBAND               )) networkEvents |= FD_OOB;

                    if (WSAEventSelect(fd->fd, handle, networkEvents) != 0)
                    {
                        fd->revents = POLLNVAL;
                        socketEvents.pop_back();
                        handles.pop_back();
                    }
                }
            }

            DWORD n = WSAWaitForMultipleEvents(handles.size(), handles.data(), FALSE, timeout != -1 ? static_cast<DWORD>(timeout) : WSA_INFINITE, FALSE);

            if (n == WSA_WAIT_FAILED) return SOCKET_ERROR;
            if (n == WSA_WAIT_TIMEOUT) return 0;
            if (n == WSA_WAIT_EVENT_0)
            {
                // the interrupt event was signaled
                *event = reinterpret_cast<void*>(interruptEvent);
                return 1;
            }

            int handleIndex = n - WSA_WAIT_EVENT_0;
            int socketIndex = handleIndex - 1;

            WSANETWORKEVENTS netEvents;
            int count = 0;
            // WSAWaitForMultipleEvents returns the index of the first signaled event. And to emulate WSAPoll()
            // all the signaled events must be processed.
            while (socketIndex < socketEvents.size())
            {
                struct pollfd* fd = socketEvents[socketIndex];

                memset(&netEvents, 0, sizeof(netEvents));
                if (WSAEnumNetworkEvents(fd->fd, socketEvents[socketIndex], &netEvents) != 0)
                {
                    fd->revents = POLLERR;
                }
                else if (netEvents.lNetworkEvents != 0)
                {
                    // mapping
                    if (netEvents.lNetworkEvents & (FD_READ  | FD_ACCEPT | FD_OOB)) fd->revents |= POLLIN;
                    if (netEvents.lNetworkEvents & (FD_WRITE | FD_CONNECT        )) fd->revents |= POLLOUT;

                    for (int i = 0; i < FD_MAX_EVENTS; ++i)
                    {
                        if (netEvents.iErrorCode[i] != 0)
                        {
                            fd->revents |= POLLERR;
                            break;
                        }
                    }

                    if (fd->revents != 0)
                    {
                        // only signaled sockets count
                        count++;
                    }
                }
                socketIndex++;
            }

            return count;
        }
        else
        {
            if (event && *event) *event = nullptr;

            socket_t maxfd = 0;
            fd_set readfds, writefds, errorfds;
            FD_ZERO(&readfds);
            FD_ZERO(&writefds);
            FD_ZERO(&errorfds);

            for (nfds_t i = 0; i < nfds; ++i)
            {
                struct pollfd* fd = &fds[i];

                if (fd->fd > maxfd)
                {
                    maxfd = fd->fd;
                }
                if ((fd->events & POLLIN))
                {
                    FD_SET(fd->fd, &readfds);
                }
                if ((fd->events & POLLOUT))
                {
                    FD_SET(fd->fd, &writefds);
                }
                if ((fd->events & POLLERR))
                {
                    FD_SET(fd->fd, &errorfds);
                }
            }

            struct timeval tv;
            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout % 1000) * 1000;

            int ret = select(maxfd + 1, &readfds, &writefds, &errorfds, timeout != -1 ? &tv : NULL);

            if (ret < 0)
            {
                return ret;
            }

            for (nfds_t i = 0; i < nfds; ++i)
            {
                struct pollfd* fd = &fds[i];
                fd->revents = 0;

                if (FD_ISSET(fd->fd, &readfds))
                {
                    fd->revents |= POLLIN;
                }
                if (FD_ISSET(fd->fd, &writefds))
                {
                    fd->revents |= POLLOUT;
                }
                if (FD_ISSET(fd->fd, &errorfds))
                {
                    fd->revents |= POLLERR;
                }
            }
            return ret;
        }
#else
        if (event && *event) *event = nullptr;

        //
        // It was reported that on Android poll can fail and return -1 with
        // errno == EINTR, which should be a temp error and should typically
        // be handled by retrying in a loop.
        // Maybe we need to put all syscall / C functions in
        // a new IXSysCalls.cpp and wrap them all.
        //
        // The style from libuv is as such.
        //
        int ret = -1;
        do
        {
            ret = ::poll(fds, nfds, timeout);
        } while (ret == -1 && errno == EINTR);

        return ret;
#endif
    }

    //
    // mingw does not have inet_ntop, which were taken as is from the musl C library.
    //
    const char* inet_ntop(int af, const void* a0, char* s, socklen_t l)
    {
#if defined(_WIN32) && defined(__GNUC__)
        const unsigned char* a = (const unsigned char*) a0;
        int i, j, max, best;
        char buf[100];

        switch (af)
        {
            case AF_INET:
                if (snprintf(s, l, "%d.%d.%d.%d", a[0], a[1], a[2], a[3]) < l) return s;
                break;
            case AF_INET6:
                if (memcmp(a, "\0\0\0\0\0\0\0\0\0\0\377\377", 12))
                    snprintf(buf,
                             sizeof buf,
                             "%x:%x:%x:%x:%x:%x:%x:%x",
                             256 * a[0] + a[1],
                             256 * a[2] + a[3],
                             256 * a[4] + a[5],
                             256 * a[6] + a[7],
                             256 * a[8] + a[9],
                             256 * a[10] + a[11],
                             256 * a[12] + a[13],
                             256 * a[14] + a[15]);
                else
                    snprintf(buf,
                             sizeof buf,
                             "%x:%x:%x:%x:%x:%x:%d.%d.%d.%d",
                             256 * a[0] + a[1],
                             256 * a[2] + a[3],
                             256 * a[4] + a[5],
                             256 * a[6] + a[7],
                             256 * a[8] + a[9],
                             256 * a[10] + a[11],
                             a[12],
                             a[13],
                             a[14],
                             a[15]);
                /* Replace longest /(^0|:)[:0]{2,}/ with "::" */
                for (i = best = 0, max = 2; buf[i]; i++)
                {
                    if (i && buf[i] != ':') continue;
                    j = strspn(buf + i, ":0");
                    if (j > max) best = i, max = j;
                }
                if (max > 3)
                {
                    buf[best] = buf[best + 1] = ':';
                    memmove(buf + best + 2, buf + best + max, i - best - max + 1);
                }
                if (strlen(buf) < l)
                {
                    strcpy(s, buf);
                    return s;
                }
                break;
            default: errno = EAFNOSUPPORT; return 0;
        }
        errno = ENOSPC;
        return 0;
#else
        return ::inet_ntop(af, a0, s, l);
#endif
    }

#if defined(_WIN32) && defined(__GNUC__)
    static int hexval(unsigned c)
    {
        if (c - '0' < 10) return c - '0';
        c |= 32;
        if (c - 'a' < 6) return c - 'a' + 10;
        return -1;
    }
#endif

    //
    // mingw does not have inet_pton, which were taken as is from the musl C library.
    //
    int inet_pton(int af, const char* s, void* a0)
    {
#if defined(_WIN32) && defined(__GNUC__)
        uint16_t ip[8];
        unsigned char* a = (unsigned char*) a0;
        int i, j, v, d, brk = -1, need_v4 = 0;

        if (af == AF_INET)
        {
            for (i = 0; i < 4; i++)
            {
                for (v = j = 0; j < 3 && isdigit(s[j]); j++)
                    v = 10 * v + s[j] - '0';
                if (j == 0 || (j > 1 && s[0] == '0') || v > 255) return 0;
                a[i] = v;
                if (s[j] == 0 && i == 3) return 1;
                if (s[j] != '.') return 0;
                s += j + 1;
            }
            return 0;
        }
        else if (af != AF_INET6)
        {
            errno = EAFNOSUPPORT;
            return -1;
        }

        if (*s == ':' && *++s != ':') return 0;

        for (i = 0;; i++)
        {
            if (s[0] == ':' && brk < 0)
            {
                brk = i;
                ip[i & 7] = 0;
                if (!*++s) break;
                if (i == 7) return 0;
                continue;
            }
            for (v = j = 0; j < 4 && (d = hexval(s[j])) >= 0; j++)
                v = 16 * v + d;
            if (j == 0) return 0;
            ip[i & 7] = v;
            if (!s[j] && (brk >= 0 || i == 7)) break;
            if (i == 7) return 0;
            if (s[j] != ':')
            {
                if (s[j] != '.' || (i < 6 && brk < 0)) return 0;
                need_v4 = 1;
                i++;
                break;
            }
            s += j + 1;
        }
        if (brk >= 0)
        {
            memmove(ip + brk + 7 - i, ip + brk, 2 * (i + 1 - brk));
            for (j = 0; j < 7 - i; j++)
                ip[brk + j] = 0;
        }
        for (j = 0; j < 8; j++)
        {
            *a++ = ip[j] >> 8;
            *a++ = ip[j];
        }
        if (need_v4 && inet_pton(AF_INET, (const char*) s, a - 4) <= 0) return 0;
        return 1;
#else
        return ::inet_pton(af, s, a0);
#endif
    }

    // Convert network bytes to host bytes. Copied from the ASIO library
    unsigned short network_to_host_short(unsigned short value)
    {
        #if defined(_WIN32)
          unsigned char* value_p = reinterpret_cast<unsigned char*>(&value);
          unsigned short result = (static_cast<unsigned short>(value_p[0]) << 8)
            | static_cast<unsigned short>(value_p[1]);
          return result;
        #else // defined(_WIN32)
          return ntohs(value);
        #endif // defined(_WIN32)
    }

} // namespace ix
