/*
 *  IXWebSocketPerMessageDeflateCodec.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018-2019 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#ifdef IXWEBSOCKET_USE_ZLIB
#include "zlib.h"
#endif
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include "IXWebSocketSendData.h"

namespace ix
{
    class WebSocketPerMessageDeflateCompressor
    {
    public:
        WebSocketPerMessageDeflateCompressor();
        ~WebSocketPerMessageDeflateCompressor();

        bool init(uint8_t deflateBits, bool clientNoContextTakeOver);
        bool compress(const IXWebSocketSendData& in, std::string& out);
        bool compress(const std::string& in, std::string& out);
        bool compress(const std::string& in, std::vector<uint8_t>& out);
        bool compress(const std::vector<uint8_t>& in, std::string& out);
        bool compress(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);

    private:
        template<typename T, typename S>
        bool compressData(const T& in, S& out);
        template<typename T>
        bool endsWithEmptyUnCompressedBlock(const T& value);

        int _flush;
        std::array<unsigned char, 1 << 14> _compressBuffer;

#ifdef IXWEBSOCKET_USE_ZLIB
        z_stream _deflateState;
#endif
    };

    class WebSocketPerMessageDeflateDecompressor
    {
    public:
        WebSocketPerMessageDeflateDecompressor();
        ~WebSocketPerMessageDeflateDecompressor();

        bool init(uint8_t inflateBits, bool clientNoContextTakeOver);
        bool decompress(const std::string& in, std::string& out);

    private:
        int _flush;
        std::array<unsigned char, 1 << 14> _compressBuffer;

#ifdef IXWEBSOCKET_USE_ZLIB
        z_stream _inflateState;
#endif
    };

} // namespace ix
