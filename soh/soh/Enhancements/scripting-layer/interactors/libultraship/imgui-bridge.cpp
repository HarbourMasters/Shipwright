#include "imgui-bridge.h"
#include <extern/ImGui/imgui.h>
#include "soh/Enhancements/scripting-layer/bridge.h"

#define BIND_FLAG(flag) BIND_FIELD(#flag, (int) flag)

void ImGuiBridge::Initialize() {
    const auto entries = {
        BIND_FLAG(ImGuiWindowFlags_None),
        BIND_FLAG(ImGuiWindowFlags_NoTitleBar),
        BIND_FLAG(ImGuiWindowFlags_NoResize),
        BIND_FLAG(ImGuiWindowFlags_NoMove),
        BIND_FLAG(ImGuiWindowFlags_NoScrollbar),
        BIND_FLAG(ImGuiWindowFlags_NoScrollWithMouse),
        BIND_FLAG(ImGuiWindowFlags_NoCollapse),
        BIND_FLAG(ImGuiWindowFlags_AlwaysAutoResize),
        BIND_FLAG(ImGuiWindowFlags_NoBackground),
        BIND_FLAG(ImGuiWindowFlags_NoSavedSettings),
        BIND_FLAG(ImGuiWindowFlags_NoMouseInputs),
        BIND_FLAG(ImGuiWindowFlags_MenuBar),
        BIND_FLAG(ImGuiWindowFlags_HorizontalScrollbar),
        BIND_FLAG(ImGuiWindowFlags_NoFocusOnAppearing),
        BIND_FLAG(ImGuiWindowFlags_NoBringToFrontOnFocus),
        BIND_FLAG(ImGuiWindowFlags_AlwaysVerticalScrollbar),
        BIND_FLAG(ImGuiWindowFlags_AlwaysHorizontalScrollbar),
        BIND_FLAG(ImGuiWindowFlags_AlwaysUseWindowPadding),
        BIND_FLAG(ImGuiWindowFlags_NoNavInputs),
        BIND_FLAG(ImGuiWindowFlags_NoNavFocus),
        BIND_FLAG(ImGuiWindowFlags_UnsavedDocument),
        BIND_FLAG(ImGuiWindowFlags_NoNav),
        BIND_FLAG(ImGuiWindowFlags_NoDecoration),
        BIND_FLAG(ImGuiWindowFlags_NoInputs),
        BIND_FLAG(ImGuiWindowFlags_NavFlattened),
        BIND_FLAG(ImGuiWindowFlags_ChildWindow),
        BIND_FLAG(ImGuiWindowFlags_Tooltip),
        BIND_FLAG(ImGuiWindowFlags_Popup),
        BIND_FLAG(ImGuiWindowFlags_Modal),
        BIND_FLAG(ImGuiWindowFlags_ChildMenu),
        BIND_FLAG(ImGuiButtonFlags_None),
        BIND_FLAG(ImGuiButtonFlags_MouseButtonLeft),
        BIND_FLAG(ImGuiButtonFlags_MouseButtonRight),
        BIND_FLAG(ImGuiButtonFlags_MouseButtonMiddle),
        BIND_FLAG(ImGuiButtonFlags_MouseButtonMask_),
        BIND_FLAG(ImGuiButtonFlags_MouseButtonDefault_),
        BIND_FUNCTION("Begin", [](uintptr_t, MethodCall* method) {
            const auto name = method->GetArgument<std::string>(0);
            const auto flags = method->GetArgument<int>(1);
            const auto result = ImGui::Begin(name.c_str(), nullptr, flags);
            method->success(result);
        }),
        BIND_FUNCTION("End", [](uintptr_t, MethodCall* method) {
            ImGui::End();
            method->success();
        }),
        BIND_FUNCTION("BeginChild", [](uintptr_t, MethodCall* method) {
            const auto name = method->GetArgument<std::string>(0);
            const auto size = method->GetArgument<ImVec2>(1);
            const auto border = method->GetArgument<bool>(2);
            const auto flags = method->GetArgument<int>(3);
            const auto result = ImGui::BeginChild(name.c_str(), size, border, flags);
            method->success(result);
        }),
        BIND_FUNCTION("EndChild", [](uintptr_t, MethodCall* method) {
            ImGui::EndChild();
            method->success();
        }),
        BIND_FUNCTION("BeginGroup", [](uintptr_t, MethodCall* method) {
            ImGui::BeginGroup();
            method->success();
        }),
        BIND_FUNCTION("EndGroup", [](uintptr_t, MethodCall* method) {
            ImGui::EndGroup();
            method->success();
        }),
        BIND_FUNCTION("BeginPopupContextItem", [](uintptr_t, MethodCall* method) {
            const auto name = method->GetArgument<std::string>(0);
            const auto flags = method->GetArgument<int>(1);
            const auto result = ImGui::BeginPopupContextItem(name.c_str(), flags);
            method->success(result);
        }),
        BIND_FUNCTION("BeginPopupContextWindow", [](uintptr_t, MethodCall* method) {
            const auto name = method->GetArgument<std::string>(0);
            const auto flags = method->GetArgument<int>(1);
            const auto result = ImGui::BeginPopupContextWindow(name.c_str(), flags);
            method->success(result);
        }),
        BIND_FUNCTION("BeginPopupContextVoid", [](uintptr_t, MethodCall* method) {
            const auto name = method->GetArgument<std::string>(0);
            const auto flags = method->GetArgument<int>(1);
            const auto result = ImGui::BeginPopupContextVoid(name.c_str(), flags);
            method->success(result);
        }),
        BIND_FUNCTION("BeginPopupModal", [](uintptr_t, MethodCall* method) {
            const auto name = method->GetArgument<std::string>(0);
            auto open = method->GetArgument<bool>(1);
            const auto flags = method->GetArgument<int>(2);
            const auto result = ImGui::BeginPopupModal(name.c_str(), &open, flags);
            method->success(result);
        }),
        BIND_FUNCTION("EndPopup", [](uintptr_t, MethodCall* method) {
            ImGui::EndPopup();
            method->success();
        }),
        BIND_FUNCTION("BeginPopup", [](uintptr_t, MethodCall* method) {
            const auto name = method->GetArgument<std::string>(0);
            const auto flags = method->GetArgument<int>(1);
            const auto result = ImGui::BeginPopup(name.c_str(), flags);
            method->success(result);
        }),
        BIND_FUNCTION("OpenPopup", [](uintptr_t, MethodCall* method) {
            const auto name = method->GetArgument<std::string>(0);
            const auto flags = method->GetArgument<int>(1);
            ImGui::OpenPopup(name.c_str(), flags);
            method->success();
        }),
        BIND_FUNCTION("CloseCurrentPopup", [](uintptr_t, MethodCall* method) {
            ImGui::CloseCurrentPopup();
            method->success();
        }),
        BIND_FUNCTION("BeginTooltip", [](uintptr_t, MethodCall* method) {
            ImGui::BeginTooltip();
            method->success();
        }),
        BIND_FUNCTION("EndTooltip", [](uintptr_t, MethodCall* method) {
            ImGui::EndTooltip();
            method->success();
        }),
        BIND_FUNCTION("SetTooltip", [](uintptr_t, MethodCall* method) {
            const auto text = method->GetArgument<std::string>(0);
            ImGui::SetTooltip(text.c_str());
            method->success();
        }),
        BIND_FUNCTION("IsItemHovered", [](uintptr_t, MethodCall* method) {
            const auto flags = method->GetArgument<int>(0);
            const auto result = ImGui::IsItemHovered(flags);
            method->success(result);
        }),
        BIND_FUNCTION("IsItemActive", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsItemActive();
            method->success(result);
        }),
        BIND_FUNCTION("IsItemFocused", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsItemFocused();
            method->success(result);
        }),
        BIND_FUNCTION("IsItemClicked", [](uintptr_t, MethodCall* method) {
            const auto button = method->GetArgument<int>(0);
            const auto result = ImGui::IsItemClicked(button);
            method->success(result);
        }),
        BIND_FUNCTION("IsItemVisible", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsItemVisible();
            method->success(result);
        }),
        BIND_FUNCTION("IsItemEdited", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsItemEdited();
            method->success(result);
        }),
        BIND_FUNCTION("IsItemActivated", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsItemActivated();
            method->success(result);
        }),
        BIND_FUNCTION("IsItemDeactivated", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsItemDeactivated();
            method->success(result);
        }),
        BIND_FUNCTION("IsItemDeactivatedAfterEdit", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsItemDeactivatedAfterEdit();
            method->success(result);
        }),
        BIND_FUNCTION("IsItemToggledOpen", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsItemToggledOpen();
            method->success(result);
        }),
        BIND_FUNCTION("IsAnyItemHovered", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsAnyItemHovered();
            method->success(result);
        }),
        BIND_FUNCTION("IsAnyItemActive", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsAnyItemActive();
            method->success(result);
        }),
        BIND_FUNCTION("IsAnyItemFocused", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsAnyItemFocused();
            method->success(result);
        }),
        BIND_FUNCTION("GetItemRectMin", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetItemRectMin();
            method->success(result);
        }),
        BIND_FUNCTION("GetItemRectMax", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetItemRectMax();
            method->success(result);
        }),
        BIND_FUNCTION("GetItemRectSize", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetItemRectSize();
            method->success(result);
        }),
        BIND_FUNCTION("BeginChildFrame", [](uintptr_t, MethodCall* method) {
            const auto id = method->GetArgument<int>(0);
            const auto size = method->GetArgument<ImVec2>(1);
            const auto flags = method->GetArgument<int>(2);
            const auto result = ImGui::BeginChildFrame(id, size, flags);
            method->success(result);
        }),
        BIND_FUNCTION("EndChildFrame", [](uintptr_t, MethodCall* method) {
            ImGui::EndChildFrame();
            method->success();
        }),
        BIND_FUNCTION("CalcItemWidth", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::CalcItemWidth();
            method->success(result);
        }),
        BIND_FUNCTION("PushItemWidth", [](uintptr_t, MethodCall* method) {
            const auto width = method->GetArgument<float>(0);
            ImGui::PushItemWidth(width);
            method->success();
        }),
        BIND_FUNCTION("PopItemWidth", [](uintptr_t, MethodCall* method) {
            ImGui::PopItemWidth();
            method->success();
        }),
        BIND_FUNCTION("SetNextItemWidth", [](uintptr_t, MethodCall* method) {
            const auto width = method->GetArgument<float>(0);
            ImGui::SetNextItemWidth(width);
            method->success();
        }),
        BIND_FUNCTION("GetContentRegionAvail", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetContentRegionAvail();
            method->success(result);
        }),
        BIND_FUNCTION("GetContentRegionMax", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetContentRegionMax();
            method->success(result);
        }),
        BIND_FUNCTION("GetWindowContentRegionMin", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetWindowContentRegionMin();
            method->success(result);
        }),
        BIND_FUNCTION("GetWindowContentRegionMax", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetWindowContentRegionMax();
            method->success(result);
        }),
        BIND_FUNCTION("GetWindowContentRegionWidth", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetWindowContentRegionWidth();
            method->success(result);
        }),
        BIND_FUNCTION("GetWindowDrawList", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetWindowDrawList();
            method->success(result);
        }),
        BIND_FUNCTION("GetWindowPos", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetWindowPos();
            method->success(result);
        }),
        BIND_FUNCTION("GetWindowSize", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetWindowSize();
            method->success(result);
        }),
        BIND_FUNCTION("GetWindowWidth", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetWindowWidth();
            method->success(result);
        }),
        BIND_FUNCTION("GetWindowHeight", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::GetWindowHeight();
            method->success(result);
        }),
        BIND_FUNCTION("IsWindowCollapsed", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsWindowCollapsed();
            method->success(result);
        }),
        BIND_FUNCTION("IsWindowAppearing", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::IsWindowAppearing();
            method->success(result);
        }),
        BIND_FUNCTION("SetWindowFontScale", [](uintptr_t, MethodCall* method) {
            const auto scale = method->GetArgument<float>(0);
            ImGui::SetWindowFontScale(scale);
            method->success();
        }),
        BIND_FUNCTION("SetNextWindowPos", [](uintptr_t, MethodCall* method) {
            const auto pos = method->GetArgument<ImVec2>(0);
            const auto cond = method->GetArgument<int>(1);
            const auto pivot = method->GetArgument<ImVec2>(2);
            ImGui::SetNextWindowPos(pos, cond, pivot);
            method->success();
        }),
        BIND_FUNCTION("SetNextWindowSize", [](uintptr_t, MethodCall* method) {
            const auto size = method->GetArgument<ImVec2>(0);
            const auto cond = method->GetArgument<int>(1);
            ImGui::SetNextWindowSize(size, cond);
            method->success();
        }),
        BIND_FUNCTION("BeginTabBar", [](uintptr_t, MethodCall* method) {
            const auto name = method->GetArgument<std::string>(0);
            const auto flags = method->GetArgument<int>(1);
            const auto result = ImGui::BeginTabBar(name.c_str(), flags);
            method->success(result);
        }),
        BIND_FUNCTION("EndTabBar", [](uintptr_t, MethodCall* method) {
            ImGui::EndTabBar();
            method->success();
        }),
        BIND_FUNCTION("BeginTabItem", [](uintptr_t, MethodCall* method) {
            const auto name = method->GetArgument<std::string>(0);
            auto open = method->GetArgument<bool>(1);
            const auto flags = method->GetArgument<int>(2);
            const auto result = ImGui::BeginTabItem(name.c_str(), &open, flags);
            method->success(result);
        }),
        BIND_FUNCTION("EndTabItem", [](uintptr_t, MethodCall* method) {
            ImGui::EndTabItem();
            method->success();
        }),
        BIND_FUNCTION("BeginMenuBar", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::BeginMenuBar();
            method->success(result);
        }),
        BIND_FUNCTION("EndMenuBar", [](uintptr_t, MethodCall* method) {
            ImGui::EndMenuBar();
            method->success();
        }),
        BIND_FUNCTION("BeginMainMenuBar", [](uintptr_t, MethodCall* method) {
            const auto result = ImGui::BeginMainMenuBar();
            method->success(result);
        }),
        BIND_FUNCTION("EndMainMenuBar", [](uintptr_t, MethodCall* method) {
            ImGui::EndMainMenuBar();
            method->success();
        }),
        BIND_FUNCTION("BeginMenu", [](uintptr_t, MethodCall* method) {
            const auto name = method->GetArgument<std::string>(0);
            const auto enabled = method->GetArgument<bool>(1);
            const auto result = ImGui::BeginMenu(name.c_str(), enabled);
            method->success(result);
        }),
        BIND_FUNCTION("EndMenu", [](uintptr_t, MethodCall* method) {
            ImGui::EndMenu();
            method->success();
        }),
        BIND_FUNCTION("MenuItem", [](uintptr_t, MethodCall* method) {
            const auto label = method->GetArgument<std::string>(0);
            const auto shortcut = method->GetArgument<std::string>(1);
            const auto selected = method->GetArgument<bool>(2);
            const auto enabled = method->GetArgument<bool>(3);
            const auto result = ImGui::MenuItem(label.c_str(), shortcut.c_str(), selected, enabled);
            method->success(result);
        }),
        BIND_FUNCTION("Text", [](uintptr_t, MethodCall* method) {
            const auto fmt = method->GetArgument<std::string>(0);
            ImGui::Text(fmt.c_str());
            method->success();
        }),
        BIND_FUNCTION("TextColored", [](uintptr_t, MethodCall* method) {
            const auto color = method->GetArgument<ImVec4>(0);
            const auto fmt = method->GetArgument<std::string>(1);
            ImGui::TextColored(color, fmt.c_str());
            method->success();
        }),
        BIND_FUNCTION("TextDisabled", [](uintptr_t, MethodCall* method) {
            const auto fmt = method->GetArgument<std::string>(0);
            ImGui::TextDisabled(fmt.c_str());
            method->success();
        }),
        BIND_FUNCTION("TextWrapped", [](uintptr_t, MethodCall* method) {
            const auto fmt = method->GetArgument<std::string>(0);
            ImGui::TextWrapped(fmt.c_str());
            method->success();
        }),
        BIND_FUNCTION("LabelText", [](uintptr_t, MethodCall* method) {
            const auto label = method->GetArgument<std::string>(0);
            const auto fmt = method->GetArgument<std::string>(1);
            ImGui::LabelText(label.c_str(), fmt.c_str());
            method->success();
        }),
        BIND_FUNCTION("BulletText", [](uintptr_t, MethodCall* method) {
            const auto fmt = method->GetArgument<std::string>(0);
            ImGui::BulletText(fmt.c_str());
            method->success();
        }),
        BIND_FUNCTION("Button", [](uintptr_t, MethodCall* method) {
            const auto count = method->ArgumentCount();
            if(count == 1) {
                const auto label = method->GetArgument<std::string>(0);
                const auto result = ImGui::Button(label.c_str());
                method->success(result);
            } else if(count == 3) {
                const auto label = method->GetArgument<std::string>(0);
                const auto width = method->GetArgument<float>(1);
                const auto height = method->GetArgument<float>(2);
                const auto result = ImGui::Button(label.c_str(), ImVec2(width, height));
                method->success(result);
            }
        }),
        BIND_FUNCTION("SmallButton", [](uintptr_t, MethodCall* method) {
            const auto label = method->GetArgument<std::string>(0);
            const auto result = ImGui::SmallButton(label.c_str());
            method->success(result);
        }),
        BIND_FUNCTION("InvisibleButton", [](uintptr_t, MethodCall* method) {
            const auto label = method->GetArgument<std::string>(0);
            const auto size = method->GetArgument<ImVec2>(1);
            const auto result = ImGui::InvisibleButton(label.c_str(), size);
            method->success(result);
        }),
        BIND_FUNCTION("ArrowButton", [](uintptr_t, MethodCall* method) {
            const auto label = method->GetArgument<std::string>(0);
            const auto dir = method->GetArgument<int>(1);
            const auto result = ImGui::ArrowButton(label.c_str(), dir);
            method->success(result);
        }),
        BIND_FUNCTION("SameLine", [](uintptr_t, MethodCall* method) {
            ImGui::SameLine();
            method->success();
        }),
    };

    GameBridge::Instance->Register(entries, "ImGui");
}