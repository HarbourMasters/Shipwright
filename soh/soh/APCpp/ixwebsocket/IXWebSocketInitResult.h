/*
 *  IXWebSocketInitResult.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include "IXWebSocketHttpHeaders.h"

namespace ix
{
    struct WebSocketInitResult
    {
        bool success;
        int http_status;
        std::string errorStr;
        WebSocketHttpHeaders headers;
        std::string uri;
        std::string protocol;

        WebSocketInitResult(bool s = false,
                            int status = 0,
                            const std::string& e = std::string(),
                            WebSocketHttpHeaders h = WebSocketHttpHeaders(),
                            const std::string& u = std::string())
        {
            success = s;
            http_status = status;
            errorStr = e;
            headers = h;
            uri = u;
            protocol = h["Sec-WebSocket-Protocol"];
        }
    };
} // namespace ix
