/*
 *  IXUserAgent.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2019 Machine Zone, Inc. All rights reserved.
 */

#include "IXUserAgent.h"

#include "IXWebSocketVersion.h"
#include <sstream>
#ifdef IXWEBSOCKET_USE_ZLIB
#include <zlib.h>
#endif

// Platform name
#if defined(_WIN32)
#define PLATFORM_NAME "windows" // Windows
#elif defined(_WIN64)
#define PLATFORM_NAME "windows" // Windows
#elif defined(__CYGWIN__) && !defined(_WIN32)
#define PLATFORM_NAME "windows" // Windows (Cygwin POSIX under Microsoft Window)
#elif defined(__ANDROID__)
#define PLATFORM_NAME "android" // Android (implies Linux, so it must come first)
#elif defined(__linux__)
#define PLATFORM_NAME "linux" // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos and other
#elif defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
#include <sys/param.h>
#if defined(BSD)
#define PLATFORM_NAME "bsd" // FreeBSD, NetBSD, OpenBSD, DragonFly BSD
#endif
#elif defined(__hpux)
#define PLATFORM_NAME "hp-ux" // HP-UX
#elif defined(_AIX)
#define PLATFORM_NAME "aix"                   // IBM AIX
#elif defined(__APPLE__) && defined(__MACH__) // Apple OSX and iOS (Darwin)
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
#define PLATFORM_NAME "ios" // Apple iOS
#elif TARGET_OS_IPHONE == 1
#define PLATFORM_NAME "ios" // Apple iOS
#elif TARGET_OS_MAC == 1
#define PLATFORM_NAME "macos" // Apple OSX
#endif
#elif defined(__sun) && defined(__SVR4)
#define PLATFORM_NAME "solaris" // Oracle Solaris, Open Indiana
#else
#define PLATFORM_NAME "unknown platform"
#endif

// SSL
#ifdef IXWEBSOCKET_USE_MBED_TLS
#include <mbedtls/version.h>
#elif defined(IXWEBSOCKET_USE_OPEN_SSL)
#include <openssl/opensslv.h>
#endif

namespace ix
{
    std::string userAgent()
    {
        std::stringstream ss;

        // IXWebSocket Version
        ss << "ixwebsocket/" << IX_WEBSOCKET_VERSION;

        // Platform
        ss << " " << PLATFORM_NAME;

        // TLS
#ifdef IXWEBSOCKET_USE_TLS
#ifdef IXWEBSOCKET_USE_MBED_TLS
        ss << " ssl/mbedtls " << MBEDTLS_VERSION_STRING;
#elif defined(IXWEBSOCKET_USE_OPEN_SSL)
        ss << " ssl/OpenSSL " << OPENSSL_VERSION_TEXT;
#elif __APPLE__
        ss << " ssl/SecureTransport";
#endif
#else
        ss << " nossl";
#endif

#ifdef IXWEBSOCKET_USE_ZLIB
        // Zlib version
        ss << " zlib " << ZLIB_VERSION;
#endif

        return ss.str();
    }
} // namespace ix
