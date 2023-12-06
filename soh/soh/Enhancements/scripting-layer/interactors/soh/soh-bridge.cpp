#include "soh-bridge.h"
#include "soh/UIWidgets.hpp"
#include "soh/Enhancements/scripting-layer/bridge.h"
#include "soh/Enhancements/scripting-layer/types/hostfunction.h"

#include <libultraship/libultraship.h>

void BindUIWidgets(){

    const auto entries = {
        BIND_FUNCTION("WrappedText", [](uintptr_t, MethodCall* method) {
            const auto text = method->GetArgument<std::string>(0);
            const auto charactersPerLine = method->GetArgument<int>(1);
            const auto result = UIWidgets::WrappedText(text, charactersPerLine);
            method->success(std::string(result));
        }),
        BIND_FUNCTION("SetLastItemHoverText", [](uintptr_t, MethodCall* method) {
            const auto text = method->GetArgument<std::string>(0);
            UIWidgets::SetLastItemHoverText(text);
            method->success();
        }),
        BIND_FUNCTION("InsertHelpHoverText", [](uintptr_t, MethodCall* method) {
            const auto text = method->GetArgument<std::string>(0);
            UIWidgets::InsertHelpHoverText(text);
            method->success();
        }),
        BIND_FUNCTION("Tooltip", [](uintptr_t, MethodCall* method) {
            const auto text = method->GetArgument<std::string>(0);
            UIWidgets::Tooltip(text.c_str());
            method->success();
        }),
        BIND_FUNCTION("Spacer", [](uintptr_t, MethodCall* method) {
            const auto height = method->GetArgument<float>(0);
            UIWidgets::Spacer(height);
            method->success();
        }),
    };

    GameBridge::Instance->Register(entries, "UIWidgets");
}

void BindSohImgui(){
    const auto entries = {
        BIND_FUNCTION("AddWindow", [](uintptr_t ctx, MethodCall* method) {
            const auto category = method->GetArgument<std::string>(0);
            const auto name = method->GetArgument<std::string>(1);
            const auto callback = method->GetArgument<HostFunction*>(2);
            // SohImGui::AddWindow(category, name, [callback](bool& open){
            //     callback->execute(open);
            // });
            method->success();
        }),
        BIND_FUNCTION("RemoveWindow", [](uintptr_t ctx, MethodCall* method) {
            const auto name = method->GetArgument<std::string>(0);
            // SohImGui::RemoveWindow(name);
            method->success();
        }),
    };

    GameBridge::Instance->Register(entries, "SohImGui");
}

void SOHBridge::Initialize() {
    BindUIWidgets();
    // BindSohImgui();
}