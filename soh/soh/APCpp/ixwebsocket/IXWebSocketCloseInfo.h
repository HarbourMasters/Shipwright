/*
 *  IXWebSocketCloseInfo.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2019 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <string>

namespace ix
{
    struct WebSocketCloseInfo
    {
        uint16_t code;
        std::string reason;
        bool remote;

        WebSocketCloseInfo(uint16_t c = 0, const std::string& r = std::string(), bool rem = false)
            : code(c)
            , reason(r)
            , remote(rem)
        {
            ;
        }
    };
} // namespace ix
