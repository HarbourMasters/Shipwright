/*
 *  IXWebSocketCloseConstants.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <string>

namespace ix
{
    struct WebSocketCloseConstants
    {
        static const uint16_t kNormalClosureCode;
        static const uint16_t kInternalErrorCode;
        static const uint16_t kAbnormalCloseCode;
        static const uint16_t kProtocolErrorCode;
        static const uint16_t kNoStatusCodeErrorCode;
        static const uint16_t kInvalidFramePayloadData;

        static const std::string kNormalClosureMessage;
        static const std::string kInternalErrorMessage;
        static const std::string kAbnormalCloseMessage;
        static const std::string kPingTimeoutMessage;
        static const std::string kProtocolErrorMessage;
        static const std::string kNoStatusCodeErrorMessage;
        static const std::string kProtocolErrorReservedBitUsed;
        static const std::string kProtocolErrorPingPayloadOversized;
        static const std::string kProtocolErrorCodeControlMessageFragmented;
        static const std::string kProtocolErrorCodeDataOpcodeOutOfSequence;
        static const std::string kProtocolErrorCodeContinuationOpCodeOutOfSequence;
        static const std::string kInvalidFramePayloadDataMessage;
        static const std::string kInvalidCloseCodeMessage;
    };
} // namespace ix
