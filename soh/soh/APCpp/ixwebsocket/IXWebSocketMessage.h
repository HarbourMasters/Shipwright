/*
 *  IXWebSocketMessage.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2019 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include "IXWebSocketCloseInfo.h"
#include "IXWebSocketErrorInfo.h"
#include "IXWebSocketMessageType.h"
#include "IXWebSocketOpenInfo.h"
#include <memory>
#include <string>

namespace ix
{
    struct WebSocketMessage
    {
        WebSocketMessageType type;
        const std::string& str;
        size_t wireSize;
        WebSocketErrorInfo errorInfo;
        WebSocketOpenInfo openInfo;
        WebSocketCloseInfo closeInfo;
        bool binary;

        WebSocketMessage(WebSocketMessageType t,
                         const std::string& s,
                         size_t w,
                         WebSocketErrorInfo e,
                         WebSocketOpenInfo o,
                         WebSocketCloseInfo c,
                         bool b = false)
            : type(t)
            , str(s)
            , wireSize(w)
            , errorInfo(e)
            , openInfo(o)
            , closeInfo(c)
            , binary(b)
        {
            ;
        }

        /**
         * @brief Deleted overload to prevent binding `str` to a temporary, which would cause
         * undefined behavior since class members don't extend lifetime beyond the constructor call.
         */
        WebSocketMessage(WebSocketMessageType t,
                         std::string&& s,
                         size_t w,
                         WebSocketErrorInfo e,
                         WebSocketOpenInfo o,
                         WebSocketCloseInfo c,
                         bool b = false) = delete;
    };

    using WebSocketMessagePtr = std::unique_ptr<WebSocketMessage>;
} // namespace ix
