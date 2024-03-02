/*
 *  IXWebSocketSendInfo.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 Machine Zone, Inc. All rights reserved.
 */

#pragma once

namespace ix
{
    struct WebSocketSendInfo
    {
        bool success;
        bool compressionError;
        size_t payloadSize;
        size_t wireSize;

        WebSocketSendInfo(bool s = false, bool c = false, size_t p = 0, size_t w = 0)
            : success(s)
            , compressionError(c)
            , payloadSize(p)
            , wireSize(w)
        {
            ;
        }
    };
} // namespace ix
