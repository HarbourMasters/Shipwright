#pragma once

#include "soh/SohGui/UIWidgets2.hpp"
#ifndef TRICKS_H
#define TRICKS_H

#include <imgui.h>
#include "randomizerTypes.h"

#include <map>
#include <set>
#include <string>

namespace Rando {
    class Tricks {
        public:
            enum class Tag {
                NOVICE,
                INTERMEDIATE,
                ADVANCED,
                EXPERT,
                EXTREME,
                EXPERIMENTAL,
                GLITCH,
            };

            static const std::string& GetAreaName(RandomizerArea area);
            static bool CheckTags(const std::map<Tag, bool>& showTag, const std::set<Tag>& rtTags);
            static const std::string GetTagName(Tag tag);
            static const ImVec4 GetTextColor(Tag tag);
            static UIWidgets2::Colors GetTagColor(Tag tag);
            static void DrawTagChips(const std::set<Tag>& rtTags, std::string trickName = "");
    };
}

#endif //TRICKS_H