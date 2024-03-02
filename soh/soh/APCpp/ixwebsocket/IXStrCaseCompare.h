/*
 *  IXStrCaseCompare.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2020 Machine Zone. All rights reserved.
 */

#pragma once

#include <string>

namespace ix
{
    struct CaseInsensitiveLess
    {
        // Case Insensitive compare_less binary function
        struct NocaseCompare
        {
            bool operator()(const unsigned char& c1, const unsigned char& c2) const;
        };

        static bool cmp(const std::string& s1, const std::string& s2);

        bool operator()(const std::string& s1, const std::string& s2) const;
    };
} // namespace ix
