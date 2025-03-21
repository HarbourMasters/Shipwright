
#include "Notification.h"
#include <libultraship/libultraship.h>
#include "soh/OTRGlobals.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
}

namespace Notification {

static uint32_t nextId = 0;
static std::vector<Options> notifications = {};

void Window::Draw() {
    auto vp = ImGui::GetMainViewport();

    const float margin = 30.0f;
    const float padding = 10.0f;

    int position = CVarGetInteger(CVAR_SETTING("Notifications.Position"), 3);

    // Top Left
    ImVec2 basePosition;
    switch (position) {
        case 0: // Top Left
            basePosition = ImVec2(vp->Pos.x + margin, vp->Pos.y + margin);
            break;  
        case 1: // Top Right
            basePosition = ImVec2(vp->Pos.x + vp->Size.x - margin, vp->Pos.y + margin);
            break;
        case 2: // Bottom Left
            basePosition = ImVec2(vp->Pos.x + margin, vp->Pos.y + vp->Size.y - margin);
            break;
        case 3: // Bottom Right
            basePosition = ImVec2(vp->Pos.x + vp->Size.x - margin, vp->Pos.y + vp->Size.y - margin);
            break;
        case 4: // Hidden
            return;
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, CVarGetFloat(CVAR_SETTING("Notifications.BgOpacity"), 0.5f)));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    for (int index = 0; index < notifications.size(); ++index) {
        auto& notification = notifications[index];
        int inverseIndex = -ABS(index - (notifications.size() - 1));

        ImGui::SetNextWindowViewport(vp->ID);
        if (notification.remainingTime < 4.0f) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (notification.remainingTime - 1) / 3.0f);
        } else {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
        }

        ImGui::Begin(("notification#" + std::to_string(notification.id)).c_str(), nullptr,
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar
        );

        ImGui::SetWindowFontScale(CVarGetFloat(CVAR_SETTING("Notifications.Size"), 1.8f)); // Make this adjustable

        ImVec2 notificationPos;
        switch (position) {
            case 0: // Top Left
                notificationPos = ImVec2(basePosition.x, basePosition.y + ((ImGui::GetWindowSize().y + padding) * inverseIndex));
                break;
            case 1: // Top Right
                notificationPos = ImVec2(basePosition.x - ImGui::GetWindowSize().x, basePosition.y + ((ImGui::GetWindowSize().y + padding) * inverseIndex));
                break;
            case 2: // Bottom Left
                notificationPos = ImVec2(basePosition.x, basePosition.y - ((ImGui::GetWindowSize().y + padding) * (inverseIndex + 1)));
                break;
            case 3: // Bottom Right
                notificationPos = ImVec2(basePosition.x - ImGui::GetWindowSize().x, basePosition.y - ((ImGui::GetWindowSize().y + padding) * (inverseIndex + 1)));
                break;
        }

        ImGui::SetWindowPos(notificationPos);

        if (notification.itemIcon != nullptr) {
            ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(notification.itemIcon), ImVec2(24, 24));
            ImGui::SameLine();
        }
        if (!notification.prefix.empty()) {
            ImGui::TextColored(notification.prefixColor, "%s", notification.prefix.c_str());
            ImGui::SameLine();
        }
        ImGui::TextColored(notification.messageColor, "%s", notification.message.c_str());
        if (!notification.suffix.empty()) {
            ImGui::SameLine();
            ImGui::TextColored(notification.suffixColor, "%s", notification.suffix.c_str());
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}


void Window::UpdateElement() {
    for (int index = 0; index < notifications.size(); ++index) {
        auto& notification = notifications[index];

        // decrement remainingTime
        notification.remainingTime -= ImGui::GetIO().DeltaTime;

        // remove notification if it has expired
        if (notification.remainingTime <= 0) {
            notifications.erase(notifications.begin() + index);
            --index;
        }
    }
}

void Emit(Options notification) {
    notification.id = nextId++;
    if (notification.remainingTime == 0.0f) {
        notification.remainingTime = CVarGetFloat(CVAR_SETTING("Notifications.Duration"), 10.0f);
    }
    notifications.push_back(notification);
    Audio_PlaySoundGeneral(NA_SE_SY_METRONOME, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

} // namespace Notification
