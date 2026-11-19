#include "soh/Enhancements/Lang/Lang.h"
#include <ship/Context.h>

#include "SohMenu.h"
#include <soh/Notification/Notification.h>
#include "SohGui.hpp"
#include "soh/OTRGlobals.h"
#include "soh/util.h"
#include <soh/Network/Sail/Sail.h>
#include <soh/Network/CrowdControl/CrowdControl.h>
#include "soh/SohGui/UIWidgets.hpp"

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets;

void SohMenu::AddMenuNetwork() {
    // Add Network Menu
    AddMenuEntry("Network", CVAR_SETTING("Menu.NetworkSidebarSection"));
    WidgetPath path;

    // Sail
    path = { "Network", "Sail", SECTION_COLUMN_1 };
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
              WIDGET_TEXT)
        .Translation("ui.panels.sohmenunetwork.sail_is_a_networking_protocol_designed_to_facilitate_remote_control_of_"
                     "the_ship_of_harkinian_client__label");
    AddWidget(path, ICON_FA_CLIPBOARD "##Sail", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) {
            ImGui::SetClipboardText("https://github.com/HarbourMasters/sail");
            Notification::Emit({
                .message = Lang::UIString("ui.text.sohgui.sohmenunetwork.copied_to_clipboard", "Copied to clipboard"),
            });
        })
        .Options(ButtonOptions().Tooltip("https://github.com/HarbourMasters/sail"));
    AddWidget(path, "Host & Port", WIDGET_CUSTOM)
        .Translation("ui.panels.sohmenunetwork.host_port")
        .CustomFunction([](WidgetInfo& info) {
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
        .Translation("ui.panels.sohmenunetwork.enable")
        .PreFunc([](WidgetInfo& info) {
            std::string host = CVarGetString(CVAR_REMOTE_SAIL("Host"), "127.0.0.1");
            uint16_t port = CVarGetInteger(CVAR_REMOTE_SAIL("Port"), 43384);
            info.options->disabled = !(!SohUtils::IsStringEmpty(host) && port > 1024 && port < 65535);
            if (Sail::Instance->isEnabled) {
                info.name = "Disable##Sail";
                info.Translation("ui.text.sohgui.sohmenunetwork.disable");
            } else {
                info.name = "Enable##Sail";
                info.Translation("ui.panels.sohmenunetwork.enable");
            }
        })
        .Callback([](WidgetInfo& info) {
            if (Sail::Instance->isEnabled) {
                CVarClear(CVAR_REMOTE_SAIL("Enabled"));
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                Sail::Instance->Disable();
            } else {
                CVarSetInteger(CVAR_REMOTE_SAIL("Enabled"), 1);
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                Sail::Instance->Enable();
            }
        });
    AddWidget(path, "Connecting...##Sail", WIDGET_TEXT)
        .Translation("ui.panels.sohmenunetwork.connecting")
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = !Sail::Instance->isEnabled;
            if (Sail::Instance->isConnected) {
                info.name = "Connected##Sail";
                info.Translation("ui.text.sohgui.sohmenunetwork.connected");
            } else {
                info.name = "Connecting...##Sail";
                info.Translation("ui.panels.sohmenunetwork.connecting");
            }
        });

    path.sidebarName = "Crowd Control";
    AddSidebarEntry("Network", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "About Crowd Control", WIDGET_SEPARATOR_TEXT)
        .Translation("ui.panels.sohmenunetwork.about_crowd_control");
    AddWidget(path,
              "Crowd Control is a platform that allows viewers to interact "
              "with a streamer's game in real time.\n"
              "\n"
              "Please head over to www.crowdcontrol.live for more information!",
              WIDGET_TEXT)
        .Translation("ui.panels.sohmenunetwork.crowd_control_is_a_platform_that_allows_viewers_to_interact_with_a_"
                     "streamer_s_game_in_real_time_plea");

    AddWidget(path, "Connect to Crowd Control", WIDGET_SEPARATOR_TEXT)
        .Translation("ui.panels.sohmenunetwork.connect_to_crowd_control");
    AddWidget(path, "Host & Port", WIDGET_CUSTOM)
        .Translation("ui.panels.sohmenunetwork.host_port")
        .CustomFunction([](WidgetInfo& info) {
            ImGui::BeginDisabled(CrowdControl::Instance->isEnabled ||
                                 CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
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
        .Translation("ui.panels.sohmenunetwork.enable")
        .PreFunc([](WidgetInfo& info) {
            std::string host = CVarGetString(CVAR_REMOTE_CROWD_CONTROL("Host"), "127.0.0.1");
            uint16_t port = CVarGetInteger(CVAR_REMOTE_CROWD_CONTROL("Port"), 43384);
            info.options->disabled = !(!SohUtils::IsStringEmpty(host) && port > 1024 && port < 65535);
            if (CrowdControl::Instance->isEnabled) {
                info.name = "Disable##CrowdControl";
                info.Translation("ui.text.sohgui.sohmenunetwork.disable");
            } else {
                info.name = "Enable##CrowdControl";
                info.Translation("ui.panels.sohmenunetwork.enable");
            }
        })
        .Callback([](WidgetInfo& info) {
            if (CrowdControl::Instance->isEnabled) {
                CVarClear(CVAR_REMOTE_CROWD_CONTROL("Enabled"));
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                CrowdControl::Instance->Disable();
            } else {
                CVarSetInteger(CVAR_REMOTE_CROWD_CONTROL("Enabled"), 1);
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                CrowdControl::Instance->Enable();
            }
        });
    AddWidget(path, "Connecting...", WIDGET_TEXT)
        .Translation("ui.panels.sohmenunetwork.connecting")
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = !CrowdControl::Instance->isEnabled;
            if (CrowdControl::Instance->isConnected) {
                info.name = "Connected";
                info.Translation("ui.text.sohgui.sohmenunetwork.connected");
            } else {
                info.name = "Connecting...";
                info.Translation("ui.panels.sohmenunetwork.connecting");
            }
        });
    AddWidget(path, "Additional Settings", WIDGET_SEPARATOR_TEXT)
        .Translation("ui.panels.sohmenunetwork.additional_settings");
    AddWidget(path, "Enemy Name Tags", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_REMOTE_CROWD_CONTROL("EnemyNameTags"))
        .RaceDisable(true)
        .Options(CheckboxOptions().Tooltip(Lang::UIString(
            "ui.widgets.gRemote.CrowdControl.EnemyNameTags.description",
            "When viewers spawn enemies, the enemy will have a name tag above them with the viewer's name.")));
    AddWidget(path, "Spawned Enemies Ignored Ingame", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_REMOTE_CROWD_CONTROL("SpawnedEnemiesIgnoredIngame"))
        .RaceDisable(true)
        .Options(CheckboxOptions().Tooltip(
            Lang::UIString("ui.widgets.gRemote.CrowdControl.SpawnedEnemiesIgnoredIngame.description",
                           "Enemies spawned by CrowdControl won't be considered for \"clear enemy "
                           "rooms\", so they don't need to be killed to complete these rooms.")));
    path.sidebarName = "Anchor";
    AddSidebarEntry("Network", path.sidebarName, 2);
}

} // namespace SohGui
