/*
 *  IXWebSocketMessageType.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2019 Machine Zone, Inc. All rights reserved.
 */

#pragma once

namespace ix
{
    enum class WebSocketMessageType
    {
        Message = 0,
        Open = 1,
        Close = 2,
        Error = 3,
        Ping = 4,
        Pong = 5,
        Fragment = 6
    };
}
