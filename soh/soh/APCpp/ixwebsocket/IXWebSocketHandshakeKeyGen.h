// Copyright (c) 2016 Alex Hultman and contributors

// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.

// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:

// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgement in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#pragma once

#include <cstddef>
#include <cstdint>
#include <string.h>
#include <string>

class WebSocketHandshakeKeyGen
{
    template<int N, typename T>
    struct static_for
    {
        void operator()(uint32_t* a, uint32_t* b)
        {
            static_for<N - 1, T>()(a, b);
            T::template f<N - 1>(a, b);
        }
    };

    template<typename T>
    struct static_for<0, T>
    {
        void operator()(uint32_t* /*a*/, uint32_t* /*hash*/)
        {
        }
    };

    template<int state>
    struct Sha1Loop
    {
        static inline uint32_t rol(uint32_t value, size_t bits)
        {
            return (value << bits) | (value >> (32 - bits));
        }
        static inline uint32_t blk(uint32_t b[16], size_t i)
        {
            return rol(b[(i + 13) & 15] ^ b[(i + 8) & 15] ^ b[(i + 2) & 15] ^ b[i], 1);
        }

        template<int i>
        static inline void f(uint32_t* a, uint32_t* b)
        {
            switch (state)
            {
                case 1:
                    a[i % 5] +=
                        ((a[(3 + i) % 5] & (a[(2 + i) % 5] ^ a[(1 + i) % 5])) ^ a[(1 + i) % 5]) +
                        b[i] + 0x5a827999 + rol(a[(4 + i) % 5], 5);
                    a[(3 + i) % 5] = rol(a[(3 + i) % 5], 30);
                    break;
                case 2:
                    b[i] = blk(b, i);
                    a[(1 + i) % 5] +=
                        ((a[(4 + i) % 5] & (a[(3 + i) % 5] ^ a[(2 + i) % 5])) ^ a[(2 + i) % 5]) +
                        b[i] + 0x5a827999 + rol(a[(5 + i) % 5], 5);
                    a[(4 + i) % 5] = rol(a[(4 + i) % 5], 30);
                    break;
                case 3:
                    b[(i + 4) % 16] = blk(b, (i + 4) % 16);
                    a[i % 5] += (a[(3 + i) % 5] ^ a[(2 + i) % 5] ^ a[(1 + i) % 5]) +
                                b[(i + 4) % 16] + 0x6ed9eba1 + rol(a[(4 + i) % 5], 5);
                    a[(3 + i) % 5] = rol(a[(3 + i) % 5], 30);
                    break;
                case 4:
                    b[(i + 8) % 16] = blk(b, (i + 8) % 16);
                    a[i % 5] += (((a[(3 + i) % 5] | a[(2 + i) % 5]) & a[(1 + i) % 5]) |
                                 (a[(3 + i) % 5] & a[(2 + i) % 5])) +
                                b[(i + 8) % 16] + 0x8f1bbcdc + rol(a[(4 + i) % 5], 5);
                    a[(3 + i) % 5] = rol(a[(3 + i) % 5], 30);
                    break;
                case 5:
                    b[(i + 12) % 16] = blk(b, (i + 12) % 16);
                    a[i % 5] += (a[(3 + i) % 5] ^ a[(2 + i) % 5] ^ a[(1 + i) % 5]) +
                                b[(i + 12) % 16] + 0xca62c1d6 + rol(a[(4 + i) % 5], 5);
                    a[(3 + i) % 5] = rol(a[(3 + i) % 5], 30);
                    break;
                case 6: b[i] += a[4 - i];
            }
        }
    };

    static inline void sha1(uint32_t hash[5], uint32_t b[16])
    {
        uint32_t a[5] = {hash[4], hash[3], hash[2], hash[1], hash[0]};
        static_for<16, Sha1Loop<1>>()(a, b);
        static_for<4, Sha1Loop<2>>()(a, b);
        static_for<20, Sha1Loop<3>>()(a, b);
        static_for<20, Sha1Loop<4>>()(a, b);
        static_for<20, Sha1Loop<5>>()(a, b);
        static_for<5, Sha1Loop<6>>()(a, hash);
    }

    static inline void base64(unsigned char* src, char* dst)
    {
        const char* b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        for (int i = 0; i < 18; i += 3)
        {
            *dst++ = b64[(src[i] >> 2) & 63];
            *dst++ = b64[((src[i] & 3) << 4) | ((src[i + 1] & 240) >> 4)];
            *dst++ = b64[((src[i + 1] & 15) << 2) | ((src[i + 2] & 192) >> 6)];
            *dst++ = b64[src[i + 2] & 63];
        }
        *dst++ = b64[(src[18] >> 2) & 63];
        *dst++ = b64[((src[18] & 3) << 4) | ((src[19] & 240) >> 4)];
        *dst++ = b64[((src[19] & 15) << 2)];
        *dst++ = '=';
    }

public:
    static inline void generate(const std::string& inputStr, char output[28])
    {
        char input[25] = {};
        strncpy(input, inputStr.c_str(), 25 - 1);
        input[25 - 1] = '\0';

        uint32_t b_output[5] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};
        uint32_t b_input[16] = {0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0x32353845,
                                0x41464135,
                                0x2d453931,
                                0x342d3437,
                                0x44412d39,
                                0x3543412d,
                                0x43354142,
                                0x30444338,
                                0x35423131,
                                0x80000000};

        for (int i = 0; i < 6; i++)
        {
            b_input[i] = (input[4 * i + 3] & 0xff) | (input[4 * i + 2] & 0xff) << 8 |
                         (input[4 * i + 1] & 0xff) << 16 | (input[4 * i + 0] & 0xff) << 24;
        }
        sha1(b_output, b_input);
        uint32_t last_b[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 480};
        sha1(b_output, last_b);
        for (int i = 0; i < 5; i++)
        {
            uint32_t tmp = b_output[i];
            char* bytes = (char*) &b_output[i];
            bytes[3] = tmp & 0xff;
            bytes[2] = (tmp >> 8) & 0xff;
            bytes[1] = (tmp >> 16) & 0xff;
            bytes[0] = (tmp >> 24) & 0xff;
        }
        base64((unsigned char*) b_output, output);
    }
};
