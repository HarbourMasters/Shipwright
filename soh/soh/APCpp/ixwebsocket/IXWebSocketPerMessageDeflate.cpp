/*
 * Copyright (c) 2015, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 Machine Zone, Inc. All rights reserved.
 *
 *  Adapted from websocketpp/extensions/permessage_deflate/enabled.hpp
 *  (same license as MZ: https://opensource.org/licenses/BSD-3-Clause)
 *
 *  - Reused zlib compression + decompression bits.
 *  - Refactored to have 2 class for compression and decompression, to allow multi-threading
 *    and make sure that _compressBuffer is not shared between threads.
 *  - Original code wasn't working for some reason, I had to add checks
 *    for the presence of the kEmptyUncompressedBlock at the end of buffer so that servers
 *    would start accepting receiving/decoding compressed messages. Original code was probably
 *    modifying the passed in buffers before processing in enabled.hpp ?
 *  - Added more documentation.
 *
 *  Per message Deflate RFC: https://tools.ietf.org/html/rfc7692
 *  Chrome websocket ->
 * https://github.com/chromium/chromium/tree/2ca8c5037021c9d2ecc00b787d58a31ed8fc8bcb/net/websockets
 *
 */

#include <cstdint>

#include "IXWebSocketPerMessageDeflate.h"

#include "IXUniquePtr.h"
#include "IXWebSocketPerMessageDeflateCodec.h"
#include "IXWebSocketPerMessageDeflateOptions.h"

namespace ix
{
    WebSocketPerMessageDeflate::WebSocketPerMessageDeflate()
        : _compressor(ix::make_unique<WebSocketPerMessageDeflateCompressor>())
        , _decompressor(ix::make_unique<WebSocketPerMessageDeflateDecompressor>())
    {
        ;
    }

    WebSocketPerMessageDeflate::~WebSocketPerMessageDeflate()
    {
        ;
    }

    bool WebSocketPerMessageDeflate::init(
        const WebSocketPerMessageDeflateOptions& perMessageDeflateOptions)
    {
        bool clientNoContextTakeover = perMessageDeflateOptions.getClientNoContextTakeover();

        uint8_t deflateBits = perMessageDeflateOptions.getClientMaxWindowBits();
        uint8_t inflateBits = perMessageDeflateOptions.getServerMaxWindowBits();

        return _compressor->init(deflateBits, clientNoContextTakeover) &&
               _decompressor->init(inflateBits, clientNoContextTakeover);
    }

    bool WebSocketPerMessageDeflate::compress(const IXWebSocketSendData& in, std::string& out)
    {
        return _compressor->compress(in, out);
    }

    bool WebSocketPerMessageDeflate::compress(const std::string& in, std::string& out)
    {
        return _compressor->compress(in, out);
    }

    bool WebSocketPerMessageDeflate::decompress(const std::string& in, std::string& out)
    {
        return _decompressor->decompress(in, out);
    }

} // namespace ix
