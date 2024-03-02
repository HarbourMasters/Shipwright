/*
 *  IXWebSocketPerMessageDeflateOptions.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <string>

namespace ix
{
    class WebSocketPerMessageDeflateOptions
    {
    public:
        WebSocketPerMessageDeflateOptions(
            bool enabled = false,
            bool clientNoContextTakeover = false,
            bool serverNoContextTakeover = false,
            uint8_t clientMaxWindowBits = kDefaultClientMaxWindowBits,
            uint8_t serverMaxWindowBits = kDefaultServerMaxWindowBits);

        WebSocketPerMessageDeflateOptions(std::string extension);

        std::string generateHeader();
        bool enabled() const;
        bool getClientNoContextTakeover() const;
        bool getServerNoContextTakeover() const;
        uint8_t getServerMaxWindowBits() const;
        uint8_t getClientMaxWindowBits() const;

        static bool startsWith(const std::string& str, const std::string& start);
        static std::string removeSpaces(const std::string& str);

        static uint8_t const kDefaultClientMaxWindowBits;
        static uint8_t const kDefaultServerMaxWindowBits;

    private:
        bool _enabled;
        bool _clientNoContextTakeover;
        bool _serverNoContextTakeover;
        uint8_t _clientMaxWindowBits;
        uint8_t _serverMaxWindowBits;

        void sanitizeClientMaxWindowBits();
    };
} // namespace ix
