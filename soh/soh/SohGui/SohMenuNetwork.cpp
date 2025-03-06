#include "SohMenu.h"
#include <soh/Notification/Notification.h>
#include <soh/Network/Network.h>
#include "SohGui.hpp"
#include <soh/Network/Sail/Sail.h>

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets2;

void SohMenu::AddMenuNetwork() {
    // Add Network Menu
    AddMenuEntry("Network", CVAR_SETTING("Menu.NetworkSidebarSection"));

    // Sail
    WidgetPath path = { "Network", "Sail", SECTION_COLUMN_1 };
    AddSidebarEntry("Network", path.sidebarName, 3);

    AddWidget(path, "Sail is a networking protocol designed to facilitate remote "
                "control of the Ship of Harkinian client. It is intended to "
                "be utilized alongside a Sail server, for which we provide a "
                "few straightforward implementations on our GitHub. The current "
                "implementations available allow integration with Twitch chat "
                "and SAMMI Bot, feel free to contribute your own!\n"
                "\n"
                "Click this button to copy the link to the Sail Github "
                "page to your clipboard.", WIDGET_TEXT);
    AddWidget(path, ICON_FA_CLIPBOARD "##Sail", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) {
            ImGui::SetClipboardText("https://github.com/HarbourMasters/sail");
            Notification::Emit({
                .message = "Copied to clipboard",
            });
        })
        .Options(ButtonOptions()
            .Tooltip("https://github.com/HarbourMasters/sail")
        );
    AddWidget(path, "Host & Port", WIDGET_CUSTOM)
        .CustomFunction([](WidgetInfo& info) {
            ImGui::BeginDisabled(Sail::Instance->isEnabled);
            ImGui::Text("%s", info.name.c_str());
            CVarInputString("##Host", CVAR_REMOTE_SAIL("Host"), InputOptions().Color(THEME_COLOR).PlaceholderText("127.0.0.1").DefaultValue("127.0.0.1").Size(ImVec2(ImGui::GetFontSize() * 15, 0)).LabelPosition(LabelPosition::None));
            ImGui::SameLine();
            ImGui::Text(":");
            ImGui::SameLine();
            CVarInputInt("##Port", CVAR_REMOTE_SAIL("Port"), InputOptions().Color(THEME_COLOR).PlaceholderText("43384").DefaultValue("43384").Size(ImVec2(ImGui::GetFontSize() * 5, 0)).LabelPosition(LabelPosition::None));
            ImGui::EndDisabled();
        });
    AddWidget(path, "Enable", WIDGET_BUTTON)
        .PreFunc([](WidgetInfo& info) {
            std::string host = CVarGetString(CVAR_REMOTE_SAIL("Host"), "127.0.0.1");
            uint16_t port = CVarGetInteger(CVAR_REMOTE_SAIL("Port"), 43384);
            info.options->disabled = !(!SohUtils::IsStringEmpty(host) && port > 1024 && port < 65535);
            if (Sail::Instance->isEnabled) {
                info.name = "Disable";
            } else {
                info.name = "Enable";
            }
        })
        .Callback([](WidgetInfo& info) {
            if (Sail::Instance->isEnabled) {
                CVarClear(CVAR_REMOTE_SAIL("Enabled"));
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                Sail::Instance->Disable();
            } else {
                CVarSetInteger(CVAR_REMOTE_SAIL("Enabled"), 1);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
               Sail::Instance->Enable();
            }
        });
    AddWidget(path, "Connecting...", WIDGET_TEXT)
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = !Sail::Instance->isEnabled;
            if (Sail::Instance->isConnected) {
                info.name = "Connected";
            } else {
                info.name = "Connecting...";
            }
        });
}

} // namespace SohGui
