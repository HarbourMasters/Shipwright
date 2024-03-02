/*
 *  IXGzipCodec.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2020 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include <string>

namespace ix
{
    std::string gzipCompress(const std::string& str);
    bool gzipDecompress(const std::string& in, std::string& out);
} // namespace ix
