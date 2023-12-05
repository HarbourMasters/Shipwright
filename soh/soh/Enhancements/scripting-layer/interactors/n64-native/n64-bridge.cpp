#include "n64-bridge.h"

#include "soh/OTRGlobals.h"
#include "soh/Enhancements/scripting-layer/bridge.h"

extern "C" {
#include "z64.h"
extern PlayState* gPlayState;
}

enum class ControllerMode {
    kPress, kHold, kRelease
};

#define BIND_BTN(btn) GameBridge::Instance->BindField(#btn, btn, "N64");
#define BIND_MODE(mode) GameBridge::Instance->BindField(#mode, (int) ControllerMode::mode, "ControllerMode");

void N64Bridge::Initialize() {
    // Push button constants

    BIND_BTN(BTN_A)  BIND_BTN(BTN_B)
    BIND_BTN(BTN_L)  BIND_BTN(BTN_R)
    BIND_BTN(BTN_Z)  BIND_BTN(BTN_START)

    BIND_BTN(BTN_CUP)  BIND_BTN(BTN_CDOWN)
    BIND_BTN(BTN_CLEFT)  BIND_BTN(BTN_CRIGHT)

    BIND_BTN(BTN_DUP)    BIND_BTN(BTN_DDOWN)
    BIND_BTN(BTN_DLEFT)  BIND_BTN(BTN_DRIGHT)

    BIND_BTN(BTN_STICKUP)    BIND_BTN(BTN_STICKDOWN)
    BIND_BTN(BTN_STICKLEFT)  BIND_BTN(BTN_STICKRIGHT)

    BIND_BTN(BTN_VSTICKUP)   BIND_BTN(BTN_VSTICKDOWN)
    BIND_BTN(BTN_VSTICKLEFT) BIND_BTN(BTN_VSTICKRIGHT)

    BIND_BTN(BTN_MODIFIER1)  BIND_BTN(BTN_MODIFIER2)

    // Push controller modes
    BIND_MODE(kPress) BIND_MODE(kHold) BIND_MODE(kRelease)

    GameBridge::Instance->BindFunction("RawControllerInput", [](uintptr_t ctx, MethodCall* method) {
        auto type = (ControllerMode) method->GetArgument<int>(0);
        auto slot = method->GetArgument<int>(1);
        int  buttons = -1;

        if(gPlayState == nullptr) {
            method->error("No play state");
            return;
        }

        switch (type) {
            case ControllerMode::kPress:
                buttons = gPlayState->state.input[slot].press.button;
                break;
            case ControllerMode::kHold:
                buttons = gPlayState->state.input[slot].cur.button;
                break;
            case ControllerMode::kRelease:
                buttons = gPlayState->state.input[slot].rel.button;
                break;
            default:
                method->error("Invalid controller mode");
                return;
        }

        method->success(buttons);
    }, "N64");
}