#include "soh/Enhancements/Lang/Lang.h"
#include <ship/Context.h>

#include "Anchor.h"
#include "soh/SohGui/SohGui.hpp"
#include "soh/SohGui/SohMenu.h"
#include "soh/util.h"
#include "soh/SohGui/UIWidgets.hpp"

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
extern std::shared_ptr<AnchorRoomWindow> mAnchorRoomWindow;
} // namespace SohGui

static const char* pvpModes[3] = { "Off", "On", "On + Friendly Fire" };
static std::vector<const char*> teleportModes = { "None", "Team Only", "All" };
static std::vector<const char*> showLocationsModes = { "None", "Team Only", "All" };

void AnchorMainMenu(WidgetInfo& info) {
    auto anchor = Anchor::Instance;

    std::string host = CVarGetString(CVAR_REMOTE_ANCHOR("Host"), "anchor.hm64.org");
    uint16_t port = CVarGetInteger(CVAR_REMOTE_ANCHOR("Port"), 43383);
    std::string anchorTeamId = CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");
    std::string anchorRoomId = CVarGetString(CVAR_REMOTE_ANCHOR("RoomId"), "");
    std::string anchorName = CVarGetString(CVAR_REMOTE_ANCHOR("Name"), "");
    bool isFormValid = !SohUtils::IsStringEmpty(host) && port > 1024 && port < 65535 &&
                       !SohUtils::IsStringEmpty(anchorRoomId) && !SohUtils::IsStringEmpty(anchorName);

    ImGui::SeparatorText(Lang::UIString("ui.text.network.anchor.menu.connection_settings", "Connection Settings"));

    ImGui::BeginDisabled(anchor->isEnabled);
    ImGui::TextUnformatted(Lang::UIString("ui.text.network.anchor.menu.host_port", "Host & Port"));
    if (UIWidgets::InputString("##Host", &host,
                               UIWidgets::InputOptions()
                                   .Size(ImGui::GetContentRegionAvail() -
                                         ImVec2((ImGui::GetFontSize() * 5 + ImGui::GetStyle().ItemSpacing.x), 0))
                                   .Color(THEME_COLOR))) {
        CVarSetString(CVAR_REMOTE_ANCHOR("Host"), host.c_str());
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    ImGui::SameLine();
    UIWidgets::PushStyleInput(THEME_COLOR);
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 5);
    if (ImGui::InputScalar("##Port", ImGuiDataType_U16, &port)) {
        CVarSetInteger(CVAR_REMOTE_ANCHOR("Port"), port);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    UIWidgets::PopStyleInput();

    ImGui::TextUnformatted(Lang::UIString("ui.text.network.anchor.menu.name_color", "Name & Color"));
    static Color_RGBA8 defaultColor = { 100, 255, 100, 255 };
    UIWidgets::CVarColorPicker("##Color", CVAR_REMOTE_ANCHOR("Color"), defaultColor);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (UIWidgets::InputString("##Name", &anchorName, UIWidgets::InputOptions().Color(THEME_COLOR))) {
        CVarSetString(CVAR_REMOTE_ANCHOR("Name"), anchorName.c_str());
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    ImGui::TextUnformatted(Lang::UIString("ui.text.network.anchor.menu.room_id", "Room ID"));
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (UIWidgets::InputString("##RoomId", &anchorRoomId,
                               UIWidgets::InputOptions().IsSecret(anchor->isEnabled).Color(THEME_COLOR))) {
        CVarSetString(CVAR_REMOTE_ANCHOR("RoomId"), anchorRoomId.c_str());
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    ImGui::TextUnformatted(
        Lang::UIString("ui.text.network.anchor.menu.team_id_items_flags_shared", "Team ID (Items & Flags Shared)"));
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (UIWidgets::InputString("##TeamId", &anchorTeamId, UIWidgets::InputOptions().Color(THEME_COLOR))) {
        CVarSetString(CVAR_REMOTE_ANCHOR("TeamId"), anchorTeamId.c_str());
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    ImGui::Spacing();

    if (UIWidgets::Button(Lang::UILabel("ui.text.network.anchor.menu.restore_defaults", "Restore Defaults"),
                          UIWidgets::ButtonOptions()
                              .Size(ImVec2(ImGui::GetContentRegionAvail().x / 2, 0))
                              .Color(UIWidgets::Colors::Red))) {
        CVarSetString(CVAR_REMOTE_ANCHOR("Host"), "anchor.hm64.org");
        CVarSetInteger(CVAR_REMOTE_ANCHOR("Port"), 43383);
        CVarSetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");
        CVarSetString(CVAR_REMOTE_ANCHOR("RoomId"), "");
        CVarSetString(CVAR_REMOTE_ANCHOR("Name"), "");
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    ImGui::SameLine();

    if (UIWidgets::Button(
            Lang::UILabel("ui.text.network.anchor.menu.global_room", "Global Room"),
            UIWidgets::ButtonOptions()
                .Color(UIWidgets::Colors::Blue)
                .Tooltip(Lang::UIString("ui.text.network.anchor.menu.always_online_public_room_so_you_don_t_have_to_"
                                        "experience_hyrule_alone_pvp_and_syncing_are_disabled",
                                        "Always-online public room so you don't have to experience "
                                        "Hyrule alone. PVP and syncing are disabled.")))) {
        CVarSetString(CVAR_REMOTE_ANCHOR("Host"), "anchor.hm64.org");
        CVarSetInteger(CVAR_REMOTE_ANCHOR("Port"), 43383);
        CVarSetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");
        CVarSetString(CVAR_REMOTE_ANCHOR("RoomId"), "soh-global");
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    ImGui::EndDisabled();

    ImGui::Spacing();

    ImGui::BeginDisabled(!isFormValid);
    const char* buttonLabel = anchor->isEnabled ? "Disable" : "Enable";
    UIWidgets::PushStyleButton(anchor->isEnabled ? UIWidgets::ColorValues.at(UIWidgets::Colors::Red)
                                                 : UIWidgets::ColorValues.at(UIWidgets::Colors::Green));
    if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {
        if (anchor->isEnabled) {
            CVarClear(CVAR_REMOTE_ANCHOR("Enabled"));
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            anchor->Disable();
        } else {
            CVarSetInteger(CVAR_REMOTE_ANCHOR("Enabled"), 1);
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            anchor->Enable();
        }
    }
    UIWidgets::PopStyleButton();
    ImGui::EndDisabled();
    ImGui::Spacing();

    if (!anchor->isEnabled) {
        return;
    }

    if (!anchor->isConnected) {
        ImGui::TextUnformatted(Lang::UIString("ui.text.network.anchor.menu.connecting", "Connecting..."));
        return;
    }

    ImGui::SeparatorText(Lang::UIString("ui.text.network.anchor.menu.current_room", "Current Room"));
    ImGui::Text(Lang::UIFormat("ui.text.network.anchor.menu.s_connected", "%s Connected"), ICON_FA_CHECK);

    UIWidgets::PushStyleButton(THEME_COLOR);
    if (ImGui::Button(Lang::UILabel("ui.text.network.anchor.menu.request_team_state", "Request Team State"))) {
        anchor->SendPacket_RequestTeamState();
    }
    UIWidgets::Tooltip(Lang::UIString(
        "ui.text.network.anchor.menu.try_this_if_you_are_missing_items_or_flags_that_your_team_members_have_collected",
        "Try this if you are missing items or flags that your team members have collected"));
    UIWidgets::PopStyleButton();

    ImGui::SameLine();

    UIWidgets::WindowButton(
        Lang::UILabel("ui.text.network.anchor.menu.toggle_anchor_room_window", "Toggle Anchor Room Window"),
        CVAR_WINDOW("AnchorRoom"), SohGui::mAnchorRoomWindow);

    ImGui::Spacing();

    bool hideLocations = Anchor::Instance->roomState.showLocationsMode == 0;
    ImGui::BeginDisabled(hideLocations);
    UIWidgets::CVarCheckbox(
        Lang::UILabel("ui.text.network.anchor.menu.show_other_players_on_minimap", "Show Other Players on Minimap"),
        CVAR_REMOTE_ANCHOR("ShowOtherPlayersOnMinimap"),
        UIWidgets::CheckboxOptions()
            .Color(THEME_COLOR)
            .DefaultValue(true)
            .Tooltip(!hideLocations
                         ? "Other players will appear on the minimap in areas where you have the compass. "
                           "Visibility is restricted according to the Show Locations mode for the room."
                         : "Cannot show other players because the room's Show Locations mode is set to None."));
    ImGui::EndDisabled();

    ImGui::Spacing();

    if (!SohGui::mAnchorRoomWindow->IsVisible()) {
        SohGui::mAnchorRoomWindow->DrawElement();
    }
}

void AnchorAdminMenu(WidgetInfo& info) {
    auto anchor = Anchor::Instance;
    bool isGlobalRoom = (std::string("soh-global") == CVarGetString(CVAR_REMOTE_ANCHOR("RoomId"), ""));

    if (!anchor->isEnabled || !anchor->isConnected || anchor->roomState.ownerClientId != anchor->ownClientId ||
        isGlobalRoom) {
        return;
    }

    ImGui::SeparatorText(
        Lang::UIString("ui.text.network.anchor.menu.room_settings_admin_only", "Room Settings (Admin Only)"));

    UIWidgets::PushStyleButton(THEME_COLOR);
    if (ImGui::Button(Lang::UILabel("ui.text.network.anchor.menu.clear_all_team_state", "Clear All Team State"))) {
        std::set<std::string> teams;
        for (auto& [clientId, client] : Anchor::Instance->clients) {
            teams.insert(client.teamId);
        }
        for (auto& team : teams) {
            anchor->SendPacket_ClearTeamState(team);
        }
    }
    UIWidgets::PopStyleButton();

    if (UIWidgets::CVarCombobox(Lang::UILabel("ui.text.network.anchor.menu.pvp_mode", "PvP Mode:"),
                                CVAR_REMOTE_ANCHOR("RoomSettings.PvpMode"), pvpModes,
                                UIWidgets::ComboboxOptions(UIWidgets::ComboboxOptions()
                                                               .DefaultIndex(1)
                                                               .LabelPosition(UIWidgets::LabelPositions::Above)
                                                               .Color(THEME_COLOR))
                                    .TranslationScope("ui.text.network.anchor.menu"))) {
        anchor->SendPacket_UpdateRoomState();
    }
    if (UIWidgets::CVarCombobox(Lang::UILabel("ui.text.network.anchor.menu.show_locations_for", "Show Locations For:"),
                                CVAR_REMOTE_ANCHOR("RoomSettings.ShowLocationsMode"), showLocationsModes,
                                UIWidgets::ComboboxOptions(UIWidgets::ComboboxOptions()
                                                               .DefaultIndex(1)
                                                               .LabelPosition(UIWidgets::LabelPositions::Above)
                                                               .Color(THEME_COLOR))
                                    .TranslationScope("ui.text.network.anchor.menu"))) {
        anchor->SendPacket_UpdateRoomState();
    }
    if (UIWidgets::CVarCombobox(
            Lang::UILabel("ui.text.network.anchor.menu.allow_teleporting_to", "Allow Teleporting To:"),
            CVAR_REMOTE_ANCHOR("RoomSettings.TeleportMode"), teleportModes,
            UIWidgets::ComboboxOptions(UIWidgets::ComboboxOptions()
                                           .DefaultIndex(1)
                                           .LabelPosition(UIWidgets::LabelPositions::Above)
                                           .Color(THEME_COLOR))
                .TranslationScope("ui.text.network.anchor.menu"))) {
        anchor->SendPacket_UpdateRoomState();
    }
    if (UIWidgets::CVarCheckbox(Lang::UILabel("ui.text.network.anchor.menu.sync_items_flags", "Sync Items & Flags"),
                                CVAR_REMOTE_ANCHOR("RoomSettings.SyncItemsAndFlags"),
                                UIWidgets::CheckboxOptions().DefaultValue(true).Color(THEME_COLOR))) {
        anchor->SendPacket_UpdateRoomState();
    }
}

void AnchorInstructionsMenu(WidgetInfo& info) {
    auto anchor = Anchor::Instance;

    ImGui::SeparatorText(Lang::UIString("ui.text.network.anchor.menu.usage_instructions", "Usage Instructions"));

    ImGui::TextWrapped(
        "%s",
        Lang::UIString("ui.text.network.anchor.menu.1_all_players_involved_should_start_at_the_file_select_screen",
                       "1. All players involved should start at the file select screen"));

    ImGui::TextWrapped(
        "%s",
        Lang::UIString("ui.text.network.anchor.menu.2_come_up_with_a_unique_room_id_this_is_basically_your_password_"
                       "and_enter_it_along_with_your_desired",
                       "2. Come up with a unique Room ID (this is basically your password) and enter it, along with "
                       "your desired player name and team ID and click Enable"));

    ImGui::TextWrapped(
        "%s",
        Lang::UIString("ui.text.network.anchor.menu.3_the_host_should_configure_the_randomizer_settings_and_generate_a_"
                       "seed_then_share_the_newly_generat",
                       "3. The host should configure the randomizer settings and generate a seed, then share the newly "
                       "generated JSON spoiler file with other players."));

    ImGui::TextWrapped(
        "%s",
        Lang::UIString("ui.text.network.anchor.menu.4_all_players_should_load_the_same_json_spoiler_file_drag_it_into_"
                       "soh_window_make_sure_seed_icons_ma",
                       "4. All players should load the same JSON spoiler file (drag it into SoH window), make sure "
                       "seed icons match, then create a new file."));

    ImGui::TextWrapped(
        "%s",
        Lang::UIString("ui.text.network.anchor.menu.5_all_players_should_now_load_into_their_game_important_if_using_"
                       "an_existing_save_seed_ensure_the_pl",
                       "5. All players should now load into their game. IMPORTANT! If using an existing save/seed "
                       "ensure the player with the most progress loads the file first."));

    ImGui::TextWrapped(
        "%s",
        Lang::UIString("ui.text.network.anchor.menu.6_after_everyone_has_loaded_in_verify_on_the_network_tab_that_it_"
                       "doesn_t_warn_about_anyone_being_on_",
                       "6. After everyone has loaded in, verify on the network tab that it doesn't warn about anyone "
                       "being on a wrong version or seed."));

    ImGui::Spacing();

    ImGui::TextWrapped(
        "%s",
        Lang::UIString(
            "ui.text.network.anchor.menu.note_team_id_is_used_to_group_players_together_in_the_same_team_sharing_items_"
            "and_flags_make_sure_al",
            "Note: Team ID is used to group players together in the same team, sharing items and flags. Make sure all "
            "players who want to share progress use the same Team ID. All players with the same Team ID should be "
            "using "
            "the same randomizer seed, while players on different teams can use different seeds."));
}

void RegisterAnchorMenu() {
    WidgetPath path = { "Network", "Anchor", SECTION_COLUMN_1 };
    SohGui::mSohMenu->AddWidget(path, "AnchorMainMenu", WIDGET_CUSTOM)
        .Translation("ui.panels.menu.anchormainmenu")
        .CustomFunction(AnchorMainMenu)
        .HideInSearch(true);
    path.column = SECTION_COLUMN_2;
    SohGui::mSohMenu->AddWidget(path, "AnchorAdminMenu", WIDGET_CUSTOM)
        .Translation("ui.panels.menu.anchoradminmenu")
        .CustomFunction(AnchorAdminMenu)
        .HideInSearch(true);
    SohGui::mSohMenu->AddWidget(path, "AnchorInstructionsMenu", WIDGET_CUSTOM)
        .Translation("ui.panels.menu.anchorinstructionsmenu")
        .CustomFunction(AnchorInstructionsMenu)
        .HideInSearch(true);
}

static RegisterMenuInitFunc menuInitFunc(RegisterAnchorMenu);
