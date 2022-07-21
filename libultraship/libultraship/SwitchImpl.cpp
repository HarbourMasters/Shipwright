#ifdef __SWITCH__
#include "SwitchImpl.h"
#include <switch.h>
#include <SDL2/SDL.h>
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
static AppletHookCookie applet_hook_cookie;
static bool isRunning = true;
static bool hasFocus  = true;

void InitKeyboard(){
    Result rc = 0;
    rc = swkbdCreate(&kbd, 0);
    if (R_SUCCEEDED(rc))
        swkbdConfigMakePresetDefault(&kbd);
}

// TODO: Fully Implement Keyboard

void UpdateKeyboard() {
    ImGuiIO* io = &ImGui::GetIO();
    int length = 512;
    char* message = nullptr;

    if(WaitFramesToUpdate > 0)
        WaitFramesToUpdate--;

    if(WaitFramesToUpdate){
        ImGui::ClearActiveID();
        if(message != nullptr)
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
            state->OverwriteData = message;
            state->OnKeyPressed(ImGuiKey_Enter);
        }

        WaitFramesToUpdate = 2;
        io->WantTextInput = false;
    }
}

void Ship::Switch::ApplyOverclock(void) {
    SwitchProfiles perfMode = (SwitchProfiles) CVar_GetS32("gSwitchPerfMode", (int) Ship::MAXIMUM);

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
                    pcvSetClockRate(PcvModule_CpuBus, SWITCH_CPU_SPEEDS_VALUES[ Ship::STOCK ]);
                } else {
                    ClkrstSession session = {0};
                    clkrstOpenSession(&session, PcvModuleId_CpuBus, 3);
                    clkrstSetClockRate(&session, SWITCH_CPU_SPEEDS_VALUES[ Ship::STOCK ]);
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

const char* RandomTexts[] = {
    "Psst, don't forget to blame melon",
    "I knew it, we have the right amount of O",
    "Who told Kenix that he is a developer?",
    "Welcome to *** wooooorld!",
    "Welcome to ***** wooooorld!",
    "Pot sanity when ?",
    "Why are you acting so random?",
    "Can't forget my ship sails in the morning",
    "Do you want 2 or 7 of those?",
    "Im not gonna play that, its not accurate damnit!",
    "Lamp oil, rope, bombs you want it, it's yours my friend as long as you have enough rubies",
    "You can build it yourself",
    "Descargar para android",
    "Made with <3 by the Harbour Masters!",
    "They say that Kenix is not a developer",
    "Squadala we're off",
    "They say one once saw an equals not get set equals",
    "This is the port all true gamers dock at"
    "Enhancements? Times Savers? Cheats? You want them? They're yours my friend!",
    "They say you gotta have the BIIIIG salad",
    "They say Louis stopped working on the imports so he can focus on the exports",
    "They say that the harbour masters loves a game with the right amount of 'o'",
    "They say ZAPD is good software",
    "You can't play your port on the bathroom? thats lame",
    "They say their 60fps mode is broken"
};

void DetectAppletMode(){

    AppletType at = appletGetAppletType();

    if (at == AppletType_Application || at == AppletType_SystemApplication){
        return;
    }

    consoleInit(NULL);

    printf("\x1b[2;2HDon't launch from the album directly, damnit!");
    printf("\x1b[4;2HRelaunch the Ship on app mode.");
    printf("\x1b[5;2HPress R when opening a game to launch the HBMenu.");

    srand(time(0));
    const char* text = RandomTexts[rand() % 25];
    printf("\x1b[44;2H%s.", text);

    while(appletMainLoop()){
        consoleUpdate(NULL);
    }

    consoleExit(NULL);
}

void Ship::Switch::Init(){
    DetectAppletMode();
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
    if (!hosversionBefore(8, 0, 0)) {
        clkrstInitialize();
    }
}

void Ship::Switch::Exit(){
#ifdef DEBUG
    socketExit();
#endif
    clkrstExit();
    appletSetGamePlayRecordingState(false);
}

bool Ship::Switch::IsRunning(){
    return isRunning;
}

void Ship::Switch::SetupFont(ImFontAtlas* fonts) {
    plInitialize(PlServiceType_System);
    static PlFontData stdFontData, extFontData;

    PlFontData fonts_std;
    PlFontData fonts_ext;
    
    plGetSharedFontByType(&fonts_std, PlSharedFontType_Standard);
    plGetSharedFontByType(&fonts_ext, PlSharedFontType_NintendoExt);

    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;

    strcpy(config.Name, "Nintendo Standard");
    fonts->AddFontFromMemoryTTF (fonts_std.address, fonts_std.size, 24.0f, &config, fonts->GetGlyphRangesCyrillic());

    strcpy(config.Name, "Nintendo Ext");
    static const ImWchar ranges[] =
        {
            0xE000, 0xE06B,
            0xE070, 0xE07E,
            0xE080, 0xE099,
            0xE0A0, 0xE0BA,
            0xE0C0, 0xE0D6,
            0xE0E0, 0xE0F5,
            0xE100, 0xE105,
            0xE110, 0xE116,
            0xE121, 0xE12C,
            0xE130, 0xE13C,
            0xE140, 0xE14D,
            0xE150, 0xE153,
            0,
        };

    fonts->AddFontFromMemoryTTF (fonts_ext.address, fonts_ext.size, 24.0f, &config, ranges);
    fonts->Build ();

    plExit();
}

void Ship::Switch::GetDisplaySize(int *width, int *height) {
    switch (appletGetOperationMode()) {
        case DOCKED_MODE:
            *width  = 1920;
            *height = 1080;
            break;
        case HANDHELD_MODE:
            *width  = 1280;
            *height = 720;
            break;
    }
}
#endif