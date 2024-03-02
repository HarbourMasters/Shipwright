/*
 *  IXUuid.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 Machine Zone. All rights reserved.
 */

/**
 * Generate a random uuid similar to the uuid python module
 *
 * >>> import uuid
 * >>> uuid.uuid4().hex
 * 'bec08155b37d4050a1f3c3fa0276bf12'
 *
 * Code adapted from https://github.com/r-lyeh-archived/sole
 */

#include "IXUuid.h"

#include <cstdint>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>


namespace ix
{
    class Uuid
    {
    public:
        Uuid();
        std::string toString() const;

    private:
        uint64_t _ab;
        uint64_t _cd;
    };

    Uuid::Uuid()
    {
        static std::random_device rd;
        static std::uniform_int_distribution<uint64_t> dist(0, (uint64_t)(~0));

        _ab = dist(rd);
        _cd = dist(rd);

        _ab = (_ab & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
        _cd = (_cd & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;
    }

    std::string Uuid::toString() const
    {
        std::stringstream ss;
        ss << std::hex << std::nouppercase << std::setfill('0');

        uint32_t a = (_ab >> 32);
        uint32_t b = (_ab & 0xFFFFFFFF);
        uint32_t c = (_cd >> 32);
        uint32_t d = (_cd & 0xFFFFFFFF);

        ss << std::setw(8) << (a);
        ss << std::setw(4) << (b >> 16);
        ss << std::setw(4) << (b & 0xFFFF);
        ss << std::setw(4) << (c >> 16);
        ss << std::setw(4) << (c & 0xFFFF);
        ss << std::setw(8) << d;

        return ss.str();
    }

    std::string uuid4()
    {
        Uuid id;
        return id.toString();
    }
} // namespace ix
