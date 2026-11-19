#include "SohModals.h"
#include <imgui.h>
#include <ship/window/Window.h>
#include <vector>
#include <string>
#include "UIWidgets.hpp"
#include "SohGui.hpp"
#include "soh/Enhancements/Lang/Lang.h"

extern "C" PlayState* gPlayState;
struct SohModal {
    std::string title_;
    std::string message_;
    std::string button1_;
    std::string button2_;
    std::function<void()> button1callback_;
    std::function<void()> button2callback_;
    std::string translationKey_;
};
std::vector<SohModal> modals;

bool closePopup = false;

void SohModalWindow::Draw() {
    if (!IsVisible()) {
        return;
    }
    DrawElement();
    // Sync up the IsVisible flag if it was changed by ImGui
    SyncVisibilityConsoleVariable();
}

void SohModalWindow::DrawElement() {
    if (modals.size() > 0) {
        SohModal curModal = modals.at(0);
        if (!curModal.translationKey_.empty()) {
            const auto& key = curModal.translationKey_;
            curModal.title_ = Lang::TranslateOrFallback(key + ".title", Lang::UIVariant(key, curModal.title_.c_str())) +
                              "###" + modals.at(0).title_;
            curModal.message_ =
                Lang::TranslateOrFallback(key + ".message", Lang::UIVariant(key, curModal.message_.c_str()));
            curModal.button1_ =
                Lang::TranslateOrFallback(key + ".button1", Lang::UIVariant(key, curModal.button1_.c_str()));
            curModal.button2_ =
                Lang::TranslateOrFallback(key + ".button2", Lang::UIVariant(key, curModal.button2_.c_str()));
        }
        if (!ImGui::IsPopupOpen(curModal.title_.c_str())) {
            ImGui::OpenPopup(curModal.title_.c_str());
        }
        if (closePopup) {
            ImGui::CloseCurrentPopup();
            modals.erase(modals.begin());
            closePopup = false;
        }
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal(curModal.title_.c_str(), NULL,
                                   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                       ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::Text("%s", curModal.message_.c_str());
            UIWidgets::PushStyleButton(THEME_COLOR);
            if (ImGui::Button(curModal.button1_.c_str())) {
                if (curModal.button1callback_ != nullptr) {
                    curModal.button1callback_();
                }
                ImGui::CloseCurrentPopup();
                modals.erase(modals.begin());
            }
            UIWidgets::PopStyleButton();
            if (curModal.button2_ != "") {
                ImGui::SameLine();
                UIWidgets::PushStyleButton(THEME_COLOR);
                if (ImGui::Button(curModal.button2_.c_str())) {
                    if (curModal.button2callback_ != nullptr) {
                        curModal.button2callback_();
                    }
                    ImGui::CloseCurrentPopup();
                    modals.erase(modals.begin());
                }
                UIWidgets::PopStyleButton();
            }
            ImGui::EndPopup();
        }
    }
}

void SohModalWindow::RegisterPopup(std::string title, std::string message, std::string button1, std::string button2,
                                   std::function<void()> button1callback, std::function<void()> button2callback,
                                   std::string translationKey) {
    modals.push_back({ title, message, button1, button2, button1callback, button2callback, translationKey });
}

size_t SohModalWindow::PopupsQueued() {
    return modals.size();
}

bool SohModalWindow::IsPopupOpen(std::string title) {
    return !modals.empty() && modals.at(0).title_ == title;
}

void SohModalWindow::DismissPopup() {
    closePopup = true;
}
