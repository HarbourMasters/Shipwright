/*
 *  IXWebSocketPerMessageDeflateCodec.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018-2019 Machine Zone, Inc. All rights reserved.
 */

#include "IXWebSocketPerMessageDeflateCodec.h"

#include "IXWebSocketPerMessageDeflateOptions.h"
#include <cassert>
#include <string.h>

namespace
{
    // The passed in size (4) is important, without it the string litteral
    // is treated as a char* and the null termination (\x00) makes it
    // look like an empty string.
    const std::string kEmptyUncompressedBlock = std::string("\x00\x00\xff\xff", 4);
} // namespace

namespace ix
{
    //
    // Compressor
    //
    WebSocketPerMessageDeflateCompressor::WebSocketPerMessageDeflateCompressor()
    {
#ifdef IXWEBSOCKET_USE_ZLIB
        memset(&_deflateState, 0, sizeof(_deflateState));

        _deflateState.zalloc = Z_NULL;
        _deflateState.zfree = Z_NULL;
        _deflateState.opaque = Z_NULL;
#endif
    }

    WebSocketPerMessageDeflateCompressor::~WebSocketPerMessageDeflateCompressor()
    {
#ifdef IXWEBSOCKET_USE_ZLIB
        deflateEnd(&_deflateState);
#endif
    }

    bool WebSocketPerMessageDeflateCompressor::init(uint8_t deflateBits,
                                                    bool clientNoContextTakeOver)
    {
#ifdef IXWEBSOCKET_USE_ZLIB
        int ret = deflateInit2(&_deflateState,
                               Z_DEFAULT_COMPRESSION,
                               Z_DEFLATED,
                               -1 * deflateBits,
                               4, // memory level 1-9
                               Z_DEFAULT_STRATEGY);

        if (ret != Z_OK) return false;

        _flush = (clientNoContextTakeOver) ? Z_FULL_FLUSH : Z_SYNC_FLUSH;

        return true;
#else
        return false;
#endif
    }

    template<typename T>
    bool WebSocketPerMessageDeflateCompressor::endsWithEmptyUnCompressedBlock(const T& value)
    {
        if (kEmptyUncompressedBlock.size() > value.size()) return false;
        auto N = value.size();
        return value[N - 1] == kEmptyUncompressedBlock[3] &&
               value[N - 2] == kEmptyUncompressedBlock[2] &&
               value[N - 3] == kEmptyUncompressedBlock[1] &&
               value[N - 4] == kEmptyUncompressedBlock[0];
    }

    bool WebSocketPerMessageDeflateCompressor::compress(const std::string& in, std::string& out)
    {
        return compressData(in, out);
    }

    bool WebSocketPerMessageDeflateCompressor::compress(const IXWebSocketSendData& in,
                                                        std::string& out)
    {
        return compressData(in, out);
    }

    bool WebSocketPerMessageDeflateCompressor::compress(const std::string& in,
                                                        std::vector<uint8_t>& out)
    {
        return compressData(in, out);
    }

    bool WebSocketPerMessageDeflateCompressor::compress(const std::vector<uint8_t>& in,
                                                        std::string& out)
    {
        return compressData(in, out);
    }

    bool WebSocketPerMessageDeflateCompressor::compress(const std::vector<uint8_t>& in,
                                                        std::vector<uint8_t>& out)
    {
        return compressData(in, out);
    }

    template<typename T, typename S>
    bool WebSocketPerMessageDeflateCompressor::compressData(const T& in, S& out)
    {
#ifdef IXWEBSOCKET_USE_ZLIB
        //
        // 7.2.1.  Compression
        //
        //    An endpoint uses the following algorithm to compress a message.
        //
        //    1.  Compress all the octets of the payload of the message using
        //        DEFLATE.
        //
        //    2.  If the resulting data does not end with an empty DEFLATE block
        //        with no compression (the "BTYPE" bits are set to 00), append an
        //        empty DEFLATE block with no compression to the tail end.
        //
        //    3.  Remove 4 octets (that are 0x00 0x00 0xff 0xff) from the tail end.
        //        After this step, the last octet of the compressed data contains
        //        (possibly part of) the DEFLATE header bits with the "BTYPE" bits
        //        set to 00.
        //
        size_t output;

        // Clear output
        out.clear();

        if (in.empty())
        {
            // See issue #167
            // The normal buffer size should be 6 but
            // we remove the 4 octets from the tail (#4)
            uint8_t buf[2] = {0x02, 0x00};
            out.push_back(buf[0]);
            out.push_back(buf[1]);

            return true;
        }

        _deflateState.avail_in = (uInt) in.size();
        _deflateState.next_in = (Bytef*) in.data();

        do
        {
            // Output to local buffer
            _deflateState.avail_out = (uInt) _compressBuffer.size();
            _deflateState.next_out = &_compressBuffer.front();

            deflate(&_deflateState, _flush);

            output = _compressBuffer.size() - _deflateState.avail_out;

            out.insert(out.end(), _compressBuffer.begin(), _compressBuffer.begin() + output);
        } while (_deflateState.avail_out == 0);

        if (endsWithEmptyUnCompressedBlock(out))
        {
            out.resize(out.size() - 4);
        }

        return true;
#else
        return false;
#endif
    }

    //
    // Decompressor
    //
    WebSocketPerMessageDeflateDecompressor::WebSocketPerMessageDeflateDecompressor()
    {
#ifdef IXWEBSOCKET_USE_ZLIB
        memset(&_inflateState, 0, sizeof(_inflateState));

        _inflateState.zalloc = Z_NULL;
        _inflateState.zfree = Z_NULL;
        _inflateState.opaque = Z_NULL;
        _inflateState.avail_in = 0;
        _inflateState.next_in = Z_NULL;
#endif
    }

    WebSocketPerMessageDeflateDecompressor::~WebSocketPerMessageDeflateDecompressor()
    {
#ifdef IXWEBSOCKET_USE_ZLIB
        inflateEnd(&_inflateState);
#endif
    }

    bool WebSocketPerMessageDeflateDecompressor::init(uint8_t inflateBits,
                                                      bool clientNoContextTakeOver)
    {
#ifdef IXWEBSOCKET_USE_ZLIB
        int ret = inflateInit2(&_inflateState, -1 * inflateBits);

        if (ret != Z_OK) return false;

        _flush = (clientNoContextTakeOver) ? Z_FULL_FLUSH : Z_SYNC_FLUSH;

        return true;
#else
        return false;
#endif
    }

    bool WebSocketPerMessageDeflateDecompressor::decompress(const std::string& in, std::string& out)
    {
#ifdef IXWEBSOCKET_USE_ZLIB
        //
        // 7.2.2.  Decompression
        //
        //    An endpoint uses the following algorithm to decompress a message.
        //
        //    1.  Append 4 octets of 0x00 0x00 0xff 0xff to the tail end of the
        //        payload of the message.
        //
        //    2.  Decompress the resulting data using DEFLATE.
        //
        std::string inFixed(in);
        inFixed += kEmptyUncompressedBlock;

        _inflateState.avail_in = (uInt) inFixed.size();
        _inflateState.next_in = (unsigned char*) (const_cast<char*>(inFixed.data()));

        // Clear output
        out.clear();

        do
        {
            _inflateState.avail_out = (uInt) _compressBuffer.size();
            _inflateState.next_out = &_compressBuffer.front();

            int ret = inflate(&_inflateState, Z_SYNC_FLUSH);

            if (ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR)
            {
                return false; // zlib error
            }

            out.append(reinterpret_cast<char*>(&_compressBuffer.front()),
                       _compressBuffer.size() - _inflateState.avail_out);
        } while (_inflateState.avail_out == 0);

        return true;
#else
        return false;
#endif
    }
} // namespace ix
