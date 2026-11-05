#pragma once

#include "soh/Enhancements/Lang/Lang.h"
#include <ship/window/gui/GuiWindow.h>
#include <imgui_internal.h>

namespace SohGui {

inline const char* LocalizedWindowTitle(const char* scope, const std::string& canonical,
                                        const char* fallbackScope = nullptr) {
    const auto visible = canonical.substr(0, canonical.find("##"));
    if (fallbackScope && std::string_view(Lang::UIVariant(scope, visible.c_str())) == visible)
        scope = fallbackScope;
    const char* title = Lang::UILabelVariant(scope, canonical.c_str());
    const ImGuiID oldId = ImHashStr(canonical.c_str());
    const ImGuiID newId = ImHashStr(title);
    if (oldId != newId && ImGui::FindWindowByID(newId) == nullptr) {
        auto* oldSettings = ImGui::FindWindowSettingsByID(oldId);
        if (oldSettings && !oldSettings->WantDelete && ImGui::FindWindowSettingsByID(newId) == nullptr) {
            // Creating a settings entry can relocate the storage; copy first
            // and reacquire the old entry after adding the new canonical ### ID.
            const ImGuiWindowSettings saved = *oldSettings;
            auto* settings = ImGui::CreateNewWindowSettings(title);
            *settings = saved;
            settings->ID = newId;
            settings->WantApply = true;
            ImGui::FindWindowSettingsByID(oldId)->WantDelete = true;
#ifdef IMGUI_HAS_DOCK
            for (auto& entry : GImGui->DockContext.Nodes.Data) {
                auto* node = static_cast<ImGuiDockNode*>(entry.val_p);
                if (node && node->SelectedTabId == oldId)
                    node->SelectedTabId = newId;
                if (node && node->TabBar) {
                    if (node->TabBar->SelectedTabId == oldId)
                        node->TabBar->SelectedTabId = newId;
                    if (node->TabBar->NextSelectedTabId == oldId)
                        node->TabBar->NextSelectedTabId = newId;
                    if (node->TabBar->VisibleTabId == oldId)
                        node->TabBar->VisibleTabId = newId;
                }
            }
#endif
        }
    }
    return title;
}

// Application window captions use the UI catalog. GuiWindow's canonical name
// remains unchanged for registry lookup and visibility CVars.
class LocalizedWindow : public Ship::GuiWindow {
  public:
    LocalizedWindow(const std::string& cvar, const std::string& name, ImVec2 size = ImVec2(-1, -1), uint32_t flags = 0)
        : Ship::GuiWindow(cvar, name, size, flags), originalSize(size), windowFlags(flags) {
    }
    LocalizedWindow(const std::string& cvar, bool visible, const std::string& name, ImVec2 size = ImVec2(-1, -1),
                    uint32_t flags = 0)
        : Ship::GuiWindow(cvar, visible, name, size, flags), originalSize(size), windowFlags(flags) {
    }
    LocalizedWindow() = default;

    void Draw() override {
        if (!IsVisible())
            return;
        if (originalSize.x != -1 || originalSize.y != -1)
            ImGui::SetNextWindowSize(originalSize, ImGuiCond_FirstUseEver);
        const auto canonical = GetName();
        if (ImGui::Begin(LocalizedWindowTitle("ui.text.sohgui.sohgui", canonical), &mIsVisible, windowFlags))
            DrawElement();
        ImGui::End();
        SyncVisibilityConsoleVariable();
    }

  private:
    ImVec2 originalSize = ImVec2(-1, -1);
    uint32_t windowFlags = 0;
};

} // namespace SohGui
