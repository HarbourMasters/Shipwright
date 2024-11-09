#include "tricks.h"


namespace Rando {
    const std::string& Tricks::GetRTAreaName(const RandomizerArea area) {
        return rtAreaNames.at(area);
    }

    bool Tricks::CheckRTTags(const std::map<Tag, bool> &showTag, const std::set<Tag> &rtTags) {
        if (rtTags.empty()) {
            return false;
        }
        for (auto rtTag : rtTags) {
            if (!showTag.at(rtTag)) {
                return false;
            }
        }
        return true;
    }

    std::string Tricks::GetRTTagName(const Tag tag) {
        return rtTagNames.at(tag);
    }

    ImVec4 Tricks::GetRTTagColor(const Tag tag) {
        switch(tag) {
            case Tag::NOVICE:
                return {0.09f,0.55f,0.37f,1.0f};
            case Tag::INTERMEDIATE:
                return {0.95f,0.52f,0.0f,1.0f};
            case Tag::ADVANCED:
                return {0.0f,0.29f,0.71f,1.0f};
            case Tag::EXPERT:
                return {0.53f,0.05f,0.14f,1.0f};
            case Tag::EXTREME:
                return {0.27f,0.0f,0.27f,1.0f};
            default:
                return {0.5f,0.5f,0.5f,1.0f};
            /*case RTTAG_LENS:
                return ImVec4(.f,.f,.f,1.0f);
            case RTTAG_BKSKIP:
                return ImVec4(.f,.f,.f,1.0f);
            case RTTAG_EXPERIMENTAL:
                return ImVec4(.f,.f,.f,1.0f);*/
        }
    }

    void Tricks::DrawTagChips(const std::set<Tag> &rtTags) {
        for (const auto rtTag : rtTags) {
            ImGui::SameLine();
            ImGui::BeginDisabled();
            ImGui::PushStyleColor(ImGuiCol_Button, GetRTTagColor(rtTag));
            ImGui::SmallButton(GetRTTagName(rtTag).c_str());
            ImGui::PopStyleColor();
            ImGui::EndDisabled();
        }
    }
}
