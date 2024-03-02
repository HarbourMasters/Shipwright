/*
 *  IXSocketFactory.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#include "IXSocketFactory.h"

#include "IXUniquePtr.h"
#ifdef IXWEBSOCKET_USE_TLS

#ifdef IXWEBSOCKET_USE_MBED_TLS
#include "IXSocketMbedTLS.h"
#elif defined(IXWEBSOCKET_USE_OPEN_SSL)
#include "IXSocketOpenSSL.h"
#elif __APPLE__
#include "IXSocketAppleSSL.h"
#endif

#else

#include "IXSocket.h"

#endif

namespace ix
{
    std::unique_ptr<Socket> createSocket(bool tls,
                                         int fd,
                                         std::string& errorMsg,
                                         const SocketTLSOptions& tlsOptions)
    {
        (void) tlsOptions;
        errorMsg.clear();
        std::unique_ptr<Socket> socket;

        if (!tls)
        {
            socket = ix::make_unique<Socket>(fd);
        }
        else
        {
#ifdef IXWEBSOCKET_USE_TLS
#if defined(IXWEBSOCKET_USE_MBED_TLS)
            socket = ix::make_unique<SocketMbedTLS>(tlsOptions, fd);
#elif defined(IXWEBSOCKET_USE_OPEN_SSL)
            socket = ix::make_unique<SocketOpenSSL>(tlsOptions, fd);
#elif defined(__APPLE__)
            socket = ix::make_unique<SocketAppleSSL>(tlsOptions, fd);
#endif
#else
            errorMsg = "TLS support is not enabled on this platform.";
            return nullptr;
#endif
        }

        if (!socket->init(errorMsg))
        {
            socket.reset();
        }

        return socket;
    }
} // namespace ix
