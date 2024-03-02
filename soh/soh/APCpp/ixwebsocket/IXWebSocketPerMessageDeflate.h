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
 */

#pragma once

#include <memory>
#include <string>
#include "IXWebSocketSendData.h"

namespace ix
{
    class WebSocketPerMessageDeflateOptions;
    class WebSocketPerMessageDeflateCompressor;
    class WebSocketPerMessageDeflateDecompressor;

    class WebSocketPerMessageDeflate
    {
    public:
        WebSocketPerMessageDeflate();
        ~WebSocketPerMessageDeflate();

        bool init(const WebSocketPerMessageDeflateOptions& perMessageDeflateOptions);
        bool compress(const IXWebSocketSendData& in, std::string& out);
        bool compress(const std::string& in, std::string& out);
        bool decompress(const std::string& in, std::string& out);

    private:
        std::unique_ptr<WebSocketPerMessageDeflateCompressor> _compressor;
        std::unique_ptr<WebSocketPerMessageDeflateDecompressor> _decompressor;
    };

    using WebSocketPerMessageDeflatePtr = std::unique_ptr<WebSocketPerMessageDeflate>;
} // namespace ix
