#include "SohModals.h"
#include <imgui.h>
#include <vector>
#include <string>
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"
#include "OTRGlobals.h"
#include "z64.h"

extern "C" PlayState* gPlayState;
struct SohModal {
    std::string title_;
    std::string message_;
    std::string button1_;
    std::string button2_;
    std::function<void()> button1callback_;
    std::function<void()> button2callback_;
};
std::vector<SohModal> modals;

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
        if (!ImGui::IsPopupOpen(curModal.title_.c_str())) {
            ImGui::OpenPopup(curModal.title_.c_str());
        }
        if (ImGui::BeginPopupModal(curModal.title_.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::Text(curModal.message_.c_str());
            if (ImGui::Button(curModal.button1_.c_str())) {
                if (curModal.button1callback_ != nullptr) {
                    curModal.button1callback_();
                }
                ImGui::CloseCurrentPopup();
                modals.erase(modals.begin());
            }
            ImGui::SameLine();
            if (curModal.button2_ != "") {
                if (ImGui::Button(curModal.button2_.c_str())) {
                    if (curModal.button2callback_ != nullptr) {
                        curModal.button2callback_();
                    }
                    ImGui::CloseCurrentPopup();
                    modals.erase(modals.begin());
                }
            }
        }
        ImGui::EndPopup();
    }
}

void SohModalWindow::RegisterPopup(std::string title, std::string message, std::string button1, std::string button2, std::function<void()> button1callback, std::function<void()> button2callback) {
    modals.push_back({ title, message, button1, button2, button1callback, button2callback });
}