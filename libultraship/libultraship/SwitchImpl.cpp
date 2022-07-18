#ifdef __SWITCH__
#include "SwitchImpl.h"
#include <switch.h>
#include <SDL2/SDL.h>
#include "Lib/ImGui/imgui.h"
#include "Lib/ImGui/imgui_internal.h"
#include "SwitchPerformanceProfiles.h"
#include "Cvar.h"
#include "Hooks.h"

extern "C" s32 CVar_GetS32(const char* name, s32 defaultValue);
extern "C" void CVar_SetS32(const char* name, s32 value);

#define DOCKED_MODE 1
#define HANDHELD_MODE 0

static int MouseX, MouseY, Prev_TouchCount;
static int WaitFramesToUpdate;
static HidTouchScreenState TouchState = {0};
static SwkbdConfig kbd;
static PadState pad;
static float internalMultiplier = 1.0f;
static AppletHookCookie applet_hook_cookie;
static bool isRunning = true;
static bool hasFocus  = true;

void InitKeyboard(){
    Result rc = 0;
    char tmpoutstr[16] = {0};
    rc = swkbdCreate(&kbd, 0);
    if (R_SUCCEEDED(rc))
        swkbdConfigMakePresetDefault(&kbd);
}

// TODO: Fully Implement Keyboard

void UpdateKeyboard() {
    ImGuiIO* io = &ImGui::GetIO();
    int length = 512;
    char* message;

    if(WaitFramesToUpdate > 0)
        WaitFramesToUpdate--;

    if(WaitFramesToUpdate){
        ImGui::ClearActiveID();
        free(message);
    }

    if(io->WantTextInput && !WaitFramesToUpdate){
        message = (char*)malloc(length);
        ImGuiInputTextState* state = ImGui::GetInputTextState(ImGui::GetActiveID());
        if(!state->InitialTextA.empty()){
            swkbdConfigSetInitialText(&kbd, state->InitialTextA.Data);
        }

        Result rc = swkbdShow(&kbd, message, length);

        if(R_SUCCEEDED(rc)){
            state->ClearText();
            state->OverwriteData = &message[0];
        }

        WaitFramesToUpdate = 2;
        io->WantTextInput = false;
    }
}

void Ship::Switch::ApplyOverclock(void) {
    SwitchProfiles perfMode = (SwitchProfiles) CVar_GetS32("gSwitchPerfMode", (int) Ship::STOCK);

    if (perfMode >= 0 && perfMode <= Ship::POWERSAVINGM3) {
        if (hosversionBefore(8, 0, 0)) {
            pcvSetClockRate(PcvModule_CpuBus, SWITCH_CPU_SPEEDS_VALUES[ perfMode ]);
        } else {
            ClkrstSession session = {0};
            clkrstOpenSession(&session, PcvModuleId_CpuBus, 3);
            clkrstSetClockRate(&session, SWITCH_CPU_SPEEDS_VALUES[ perfMode ]);
            clkrstCloseSession(&session);
        }
   }
}

static void on_applet_hook(AppletHookType hook, void *param) {
   AppletFocusState focus_state;

   /* Exit request */
   switch (hook) {
        case AppletHookType_OnExitRequest:
            isRunning = false;
            break;

         /* Focus state*/
        case AppletHookType_OnFocusState:
            focus_state = appletGetFocusState();
            hasFocus = focus_state == AppletFocusState_InFocus;

            if (!hasFocus) {
                if (hosversionBefore(8, 0, 0)) {
                    pcvSetClockRate(PcvModule_CpuBus, 1020000000);
                } else {
                    ClkrstSession session = {0};
                    clkrstOpenSession(&session, PcvModuleId_CpuBus, 3);
                    clkrstSetClockRate(&session, 1020000000);
                    clkrstCloseSession(&session);
                }
            } else
                Ship::Switch::ApplyOverclock();
            break;

         /* Performance mode */
        case AppletHookType_OnPerformanceMode:
            Ship::Switch::ApplyOverclock();
            break;
        default: break;
   }
}

void Ship::Switch::Init(){
    appletInitializeGamePlayRecording();
#ifdef DEBUG
    socketInitializeDefault();
    nxlinkStdio();
#endif
    appletSetGamePlayRecordingState(true);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    hidInitializeTouchScreen();
    InitKeyboard();
    appletHook(&applet_hook_cookie, on_applet_hook, NULL);
    appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
}

void PushSDLEvent(Uint32 type, Uint8 button){
    SDL_Event event;
    event.type = type;
    event.button.button = button;
    SDL_PushEvent(&event);
}

void Ship::Switch::Update(){
    UpdateKeyboard();
    padUpdate(&pad);
    uint64_t kDown = padGetButtonsDown(&pad);

    if (kDown & HidNpadButton_Minus){
        CVar_SetS32("gOpenMenuBar", !CVar_GetS32("gOpenMenuBar", 0));
        ModInternal::ExecuteHooks<ModInternal::RequestConfigSave>();
    }

    if (hidGetTouchScreenStates(&TouchState, 1)) {
        if (TouchState.count != Prev_TouchCount) {
            Prev_TouchCount = TouchState.count;
            PushSDLEvent(SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT);
            PushSDLEvent(SDL_MOUSEBUTTONUP, SDL_BUTTON_RIGHT);
        }

        for(s32 i = 0; i < TouchState.count; i++) {
            MouseX = TouchState.touches[0].x * internalMultiplier;
            MouseY = TouchState.touches[0].y * internalMultiplier;
        }

        if(TouchState.count == 1) {
            PushSDLEvent(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT);
            return;
        }

        if(TouchState.count > 1) {
            PushSDLEvent(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_RIGHT);
        }
    }
}

void Ship::Switch::Exit(){
    socketExit();
    appletSetGamePlayRecordingState(false);
}

bool Ship::Switch::IsRunning(){
    return isRunning;
}

void Ship::Switch::GetDisplaySize(int *width, int *height) {
    switch (appletGetOperationMode()) {
        case DOCKED_MODE:
            *width  = 1920;
            *height = 1080;
            break;
        case HANDHELD_MODE:
            *width  = 1280 * internalMultiplier;
            *height = 720  * internalMultiplier;
            break;
    }
}

void Ship::Switch::GetTouchPosition(int *touchX, int *touchY) {
    *touchX = MouseX;
    *touchY = MouseY;
}

float Ship::Switch::GetDPI(){
    switch (appletGetOperationMode()) {
        case DOCKED_MODE:
            return 56.48480f;
        case HANDHELD_MODE:
            return 236.8717f;
    }
}
#endif