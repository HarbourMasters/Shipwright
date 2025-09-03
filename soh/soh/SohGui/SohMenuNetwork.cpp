#ifdef ENABLE_REMOTE_CONTROL
#include "SohMenu.h"
#include <soh/Notification/Notification.h>
#include <soh/Network/Network.h>
#include "SohGui.hpp"
#include "soh/OTRGlobals.h"
#include <soh/Network/Sail/Sail.h>
#include <soh/Network/CrowdControl/CrowdControl.h>
#include <soh/Network/Anchor/Anchor.h>

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets;

void SohMenu::AddMenuNetwork() {
    // Add Network Menu
    AddMenuEntry("Network", CVAR_SETTING("Menu.NetworkSidebarSection"));

    // Sail
    WidgetPath path = { "Network", "Sail", SECTION_COLUMN_1 };
    AddSidebarEntry("Network", path.sidebarName, 3);

    AddWidget(path,
              "Sail is a networking protocol designed to facilitate remote "
              "control of the Ship of Harkinian client. It is intended to "
              "be utilized alongside a Sail server, for which we provide a "
              "few straightforward implementations on our GitHub. The current "
              "implementations available allow integration with Twitch chat "
              "and SAMMI Bot, feel free to contribute your own!\n"
              "\n"
              "Click this button to copy the link to the Sail Github "
              "page to your clipboard.",
              WIDGET_TEXT);
    AddWidget(path, ICON_FA_CLIPBOARD "##Sail", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) {
            ImGui::SetClipboardText("https://github.com/HarbourMasters/sail");
            Notification::Emit({
                .message = "Copied to clipboard",
            });
        })
        .Options(ButtonOptions().Tooltip("https://github.com/HarbourMasters/sail"));
    AddWidget(path, "Host & Port", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        ImGui::BeginDisabled(Sail::Instance->isEnabled || CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
        ImGui::Text("%s", info.name.c_str());
        CVarInputString("##HostSail", CVAR_REMOTE_SAIL("Host"),
                        InputOptions()
                            .Color(THEME_COLOR)
                            .PlaceholderText("127.0.0.1")
                            .DefaultValue("127.0.0.1")
                            .Size(ImVec2(ImGui::GetFontSize() * 15, 0))
                            .LabelPosition(LabelPositions::None));
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        CVarInputInt("##PortSail", CVAR_REMOTE_SAIL("Port"),
                     InputOptions()
                         .Color(THEME_COLOR)
                         .PlaceholderText("43384")
                         .DefaultValue("43384")
                         .Size(ImVec2(ImGui::GetFontSize() * 5, 0))
                         .LabelPosition(LabelPositions::None));
        ImGui::EndDisabled();
    });
    AddWidget(path, "Enable##Sail", WIDGET_BUTTON)
        .PreFunc([](WidgetInfo& info) {
            std::string host = CVarGetString(CVAR_REMOTE_SAIL("Host"), "127.0.0.1");
            uint16_t port = CVarGetInteger(CVAR_REMOTE_SAIL("Port"), 43384);
            info.options->disabled = !(!SohUtils::IsStringEmpty(host) && port > 1024 && port < 65535);
            if (Sail::Instance->isEnabled) {
                info.name = "Disable##Sail";
            } else {
                info.name = "Enable##Sail";
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
    AddWidget(path, "Connecting...##Sail", WIDGET_TEXT).PreFunc([](WidgetInfo& info) {
        info.isHidden = !Sail::Instance->isEnabled;
        if (Sail::Instance->isConnected) {
            info.name = "Connected##Sail";
        } else {
            info.name = "Connecting...##Sail";
        }
    });

    path.sidebarName = "Crowd Control";
    AddSidebarEntry("Network", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "About Crowd Control", WIDGET_SEPARATOR_TEXT);
    AddWidget(path,
              "Crowd Control is a platform that allows viewers to interact "
              "with a streamer's game in real time.\n"
              "\n"
              "Please head over to www.crowdcontrol.live for more information!",
              WIDGET_TEXT);

    AddWidget(path, "Connect to Crowd Control", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Host & Port", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        ImGui::BeginDisabled(CrowdControl::Instance->isEnabled || CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
        ImGui::Text("%s", info.name.c_str());
        CVarInputString("##HostCrowdControl", CVAR_REMOTE_CROWD_CONTROL("Host"),
                        InputOptions()
                            .Color(THEME_COLOR)
                            .PlaceholderText("127.0.0.1")
                            .DefaultValue("127.0.0.1")
                            .Size(ImVec2(ImGui::GetFontSize() * 15, 0))
                            .LabelPosition(LabelPositions::None));
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        CVarInputInt("##PortCrowdControl", CVAR_REMOTE_CROWD_CONTROL("Port"),
                     InputOptions()
                         .Color(THEME_COLOR)
                         .PlaceholderText("43384")
                         .DefaultValue("43384")
                         .Size(ImVec2(ImGui::GetFontSize() * 5, 0))
                         .LabelPosition(LabelPositions::None));
        ImGui::EndDisabled();
    });
    AddWidget(path, "Enable##CrowdControl", WIDGET_BUTTON)
        .PreFunc([](WidgetInfo& info) {
            std::string host = CVarGetString(CVAR_REMOTE_CROWD_CONTROL("Host"), "127.0.0.1");
            uint16_t port = CVarGetInteger(CVAR_REMOTE_CROWD_CONTROL("Port"), 43384);
            info.options->disabled = !(!SohUtils::IsStringEmpty(host) && port > 1024 && port < 65535);
            if (CrowdControl::Instance->isEnabled) {
                info.name = "Disable##CrowdControl";
            } else {
                info.name = "Enable##CrowdControl";
            }
        })
        .Callback([](WidgetInfo& info) {
            if (CrowdControl::Instance->isEnabled) {
                CVarClear(CVAR_REMOTE_CROWD_CONTROL("Enabled"));
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                CrowdControl::Instance->Disable();
            } else {
                CVarSetInteger(CVAR_REMOTE_CROWD_CONTROL("Enabled"), 1);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                CrowdControl::Instance->Enable();
            }
        });
    AddWidget(path, "Connecting...", WIDGET_TEXT).PreFunc([](WidgetInfo& info) {
        info.isHidden = !CrowdControl::Instance->isEnabled;
        if (CrowdControl::Instance->isConnected) {
            info.name = "Connected";
        } else {
            info.name = "Connecting...";
        }
    });
    AddWidget(path, "Additional Settings", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Enemy Name Tags", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_REMOTE_CROWD_CONTROL("EnemyNameTags"))
        .RaceDisable(true)
        .Options(CheckboxOptions().Tooltip(
            "When viewers spawn enemies, the enemy will have a name tag above them with the viewer's name."));
    AddWidget(path, "Spawned Enemies Ignored Ingame", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_REMOTE_CROWD_CONTROL("SpawnedEnemiesIgnoredIngame"))
        .RaceDisable(true)
        .Options(CheckboxOptions().Tooltip("Enemies spawned by CrowdControl won't be considered for \"clear enemy "
                                           "rooms\", so they don't need to be killed to complete these rooms."));


    // Anchor
    path.sidebarName = "Online Multiplayer (Anchor)";
    AddSidebarEntry("Network", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "About Online Multiplayer (formerly known as Anchor)", WIDGET_SEPARATOR_TEXT);
    AddWidget(path,
              "Anchor is an option for Ship of Harkinian that allows "
              "multiple players to play within the same world to "
              "complete Ocarina of Time. This is supported for randomizers "
              "as well as vanilla playthroughs. Some bugs may occur, as this "
              "is still being developed.\n",
              WIDGET_TEXT);

    AddWidget(path, "Connect to a server", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Host & Port", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        ImGui::BeginDisabled(Anchor::Instance->isEnabled || CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
        ImGui::Text("%s", info.name.c_str());
        CVarInputString("##HostAnchor", CVAR_REMOTE_CROWD_CONTROL("Host"),
                        InputOptions()
                            .Color(THEME_COLOR)
                            .PlaceholderText("anchor.proxysaw.dev")
                            .DefaultValue("anchor.proxysaw.dev")
                            .Size(ImVec2(ImGui::GetFontSize() * 15, 0))
                            .LabelPosition(LabelPositions::None));
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        CVarInputInt("##PortAnchor", CVAR_REMOTE_CROWD_CONTROL("Port"),
                     InputOptions()
                         .Color(THEME_COLOR)
                         .PlaceholderText("43385")
                         .DefaultValue("43385")
                         .Size(ImVec2(ImGui::GetFontSize() * 5, 0))
                         .LabelPosition(LabelPositions::None));
        ImGui::EndDisabled();
    });
    AddWidget(path, "Enable##Anchor", WIDGET_BUTTON)
        .PreFunc([](WidgetInfo& info) {
            std::string host = CVarGetString(CVAR_REMOTE_CROWD_CONTROL("Host"), "anchor.proxysaw.dev");
            uint16_t port = CVarGetInteger(CVAR_REMOTE_CROWD_CONTROL("Port"), 43385);
            info.options->disabled = !(!SohUtils::IsStringEmpty(host) && port > 1024 && port < 65535);
            if (Anchor::Instance->isEnabled) {
                info.name = "Disable##Anchor";
            } else {
                info.name = "Enable##Anchor";
            }
        })
        .Callback([](WidgetInfo& info) {
            if (Anchor::Instance->isEnabled) {
                CVarClear(CVAR_REMOTE_CROWD_CONTROL("Enabled"));
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                Anchor::Instance->Disable();
            } else {
                CVarSetInteger(CVAR_REMOTE_CROWD_CONTROL("Enabled"), 1);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                Anchor::Instance->Enable();
            }
        });
    AddWidget(path, "Connecting...", WIDGET_TEXT).PreFunc([](WidgetInfo& info) {
        info.isHidden = !Anchor::Instance->isEnabled;
        if (Anchor::Instance->isConnected) {
            info.name = "Connected";
        } else {
            info.name = "Connecting...";
        }
    });
    /*AddWidget(path, "Additional Settings", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Enemy Name Tags", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_REMOTE_CROWD_CONTROL("EnemyNameTags"))
        .RaceDisable(true)
        .Options(CheckboxOptions().Tooltip(
            "When viewers spawn enemies, the enemy will have a name tag above them with the viewer's name."));
    AddWidget(path, "Spawned Enemies Ignored Ingame", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_REMOTE_CROWD_CONTROL("SpawnedEnemiesIgnoredIngame"))
        .RaceDisable(true)
        .Options(CheckboxOptions().Tooltip("Enemies spawned by CrowdControl won't be considered for \"clear enemy "
                                           "rooms\", so they don't need to be killed to complete these rooms."));*/
}

} // namespace SohGui
#endif