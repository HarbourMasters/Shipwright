/*
 *  IXWebSocketCloseConstants.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#include "IXWebSocketCloseConstants.h"

namespace ix
{
    const uint16_t WebSocketCloseConstants::kNormalClosureCode(1000);
    const uint16_t WebSocketCloseConstants::kInternalErrorCode(1011);
    const uint16_t WebSocketCloseConstants::kAbnormalCloseCode(1006);
    const uint16_t WebSocketCloseConstants::kInvalidFramePayloadData(1007);
    const uint16_t WebSocketCloseConstants::kProtocolErrorCode(1002);
    const uint16_t WebSocketCloseConstants::kNoStatusCodeErrorCode(1005);

    const std::string WebSocketCloseConstants::kNormalClosureMessage("Normal closure");
    const std::string WebSocketCloseConstants::kInternalErrorMessage("Internal error");
    const std::string WebSocketCloseConstants::kAbnormalCloseMessage("Abnormal closure");
    const std::string WebSocketCloseConstants::kPingTimeoutMessage("Ping timeout");
    const std::string WebSocketCloseConstants::kProtocolErrorMessage("Protocol error");
    const std::string WebSocketCloseConstants::kNoStatusCodeErrorMessage("No status code");
    const std::string WebSocketCloseConstants::kProtocolErrorReservedBitUsed("Reserved bit used");
    const std::string WebSocketCloseConstants::kProtocolErrorPingPayloadOversized(
        "Ping reason control frame with payload length > 125 octets");
    const std::string WebSocketCloseConstants::kProtocolErrorCodeControlMessageFragmented(
        "Control message fragmented");
    const std::string WebSocketCloseConstants::kProtocolErrorCodeDataOpcodeOutOfSequence(
        "Fragmentation: data message out of sequence");
    const std::string WebSocketCloseConstants::kProtocolErrorCodeContinuationOpCodeOutOfSequence(
        "Fragmentation: continuation opcode out of sequence");
    const std::string WebSocketCloseConstants::kInvalidFramePayloadDataMessage(
        "Invalid frame payload data");
    const std::string WebSocketCloseConstants::kInvalidCloseCodeMessage("Invalid close code");
} // namespace ix
