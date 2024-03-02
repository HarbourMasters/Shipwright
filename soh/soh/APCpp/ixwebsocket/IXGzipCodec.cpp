/*
 *  IXGzipCodec.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2020 Machine Zone, Inc. All rights reserved.
 */

#include "IXGzipCodec.h"

#include "IXBench.h"
#include <array>
#include <string.h>

#ifdef IXWEBSOCKET_USE_ZLIB
#include <zlib.h>
#endif

namespace ix
{
    std::string gzipCompress(const std::string& str)
    {
#ifndef IXWEBSOCKET_USE_ZLIB
        return std::string();
#else
        z_stream zs; // z_stream is zlib's control structure
        memset(&zs, 0, sizeof(zs));

        // deflateInit2 configure the file format: request gzip instead of deflate
        const int windowBits = 15;
        const int GZIP_ENCODING = 16;

        deflateInit2(&zs,
                     Z_DEFAULT_COMPRESSION,
                     Z_DEFLATED,
                     windowBits | GZIP_ENCODING,
                     8,
                     Z_DEFAULT_STRATEGY);

        zs.next_in = (Bytef*) str.data();
        zs.avail_in = (uInt) str.size(); // set the z_stream's input

        int ret;
        char outbuffer[32768];
        std::string outstring;

        // retrieve the compressed bytes blockwise
        do
        {
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
            zs.avail_out = sizeof(outbuffer);

            ret = deflate(&zs, Z_FINISH);

            if (outstring.size() < zs.total_out)
            {
                // append the block to the output string
                outstring.append(outbuffer, zs.total_out - outstring.size());
            }
        } while (ret == Z_OK);

        deflateEnd(&zs);

        return outstring;
#endif // IXWEBSOCKET_USE_ZLIB
    }

#ifdef IXWEBSOCKET_USE_DEFLATE
    static uint32_t loadDecompressedGzipSize(const uint8_t* p)
    {
        return ((uint32_t) p[0] << 0) | ((uint32_t) p[1] << 8) | ((uint32_t) p[2] << 16) |
               ((uint32_t) p[3] << 24);
    }
#endif

    bool gzipDecompress(const std::string& in, std::string& out)
    {
#ifndef IXWEBSOCKET_USE_ZLIB
        return false;
#else
        z_stream inflateState;
        memset(&inflateState, 0, sizeof(inflateState));

        inflateState.zalloc = Z_NULL;
        inflateState.zfree = Z_NULL;
        inflateState.opaque = Z_NULL;
        inflateState.avail_in = 0;
        inflateState.next_in = Z_NULL;

        if (inflateInit2(&inflateState, 16 + MAX_WBITS) != Z_OK)
        {
            return false;
        }

        inflateState.avail_in = (uInt) in.size();
        inflateState.next_in = (unsigned char*) (const_cast<char*>(in.data()));

        const int kBufferSize = 1 << 14;
        std::array<unsigned char, kBufferSize> compressBuffer;

        do
        {
            inflateState.avail_out = (uInt) kBufferSize;
            inflateState.next_out = &compressBuffer.front();

            int ret = inflate(&inflateState, Z_SYNC_FLUSH);

            if (ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR)
            {
                inflateEnd(&inflateState);
                return false;
            }

            out.append(reinterpret_cast<char*>(&compressBuffer.front()),
                       kBufferSize - inflateState.avail_out);
        } while (inflateState.avail_out == 0);

        inflateEnd(&inflateState);
        return true;
#endif // IXWEBSOCKET_USE_ZLIB
    }
} // namespace ix
