#ifdef __SWITCH__
#include "SwitchImpl.h"
#include <switch.h>
#include <SDL2/SDL.h>
#include "SwitchPerformanceProfiles.h"
#include "Cvar.h"
#include "Hooks.h"

extern "C" s32 CVar_GetS32(const char* name, s32 defaultValue);
extern "C" void CVar_SetS32(const char* name, s32 value);

#define DOCKED_MODE 1
#define HANDHELD_MODE 0

static AppletHookCookie applet_hook_cookie;
static bool isRunning = true;
static bool hasFocus  = true;

void DetectAppletMode();

static void on_applet_hook(AppletHookType hook, void *param);

void Ship::Switch::Init(SwitchPhase phase){
    switch(phase){
        case PreInitPhase:
            DetectAppletMode();
            break;
        case PostInitPhase:
            appletInitializeGamePlayRecording();
        #ifdef DEBUG
            socketInitializeDefault();
            nxlinkStdio();
        #endif
            appletSetGamePlayRecordingState(true);
            appletHook(&applet_hook_cookie, on_applet_hook, NULL);
            appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
            if (!hosversionBefore(8, 0, 0)) {
                clkrstInitialize();
            }
            break;
    }
}

void Ship::Switch::Exit(){
#ifdef DEBUG
    socketExit();
#endif
    clkrstExit();
    appletSetGamePlayRecordingState(false);
}

void Ship::Switch::SetupFont(ImFontAtlas* fonts) {
    Result rInit = plInitialize(PlServiceType_System);

    if (!R_SUCCEEDED(rInit)) {
        return;
    }

    static PlFontData stdFontData, extFontData;

    PlFontData fonts_std;
    PlFontData fonts_ext;

    Result rStd  = plGetSharedFontByType(&fonts_std, PlSharedFontType_Standard);
    Result rNext = plGetSharedFontByType(&fonts_ext, PlSharedFontType_NintendoExt);

    if(!R_SUCCEEDED(rStd) || !R_SUCCEEDED(rNext)) result;

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
            *width  = 1280;
            *height = 720;
            break;
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

struct Star {
    SDL_Rect* rect;
    float speed;
    int   layer;
    int8_t color[3];
};

std::vector<Star*> stars;

void Ship::Switch::PrintErrorMessageToScreen(const char *str, ...) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return;

    int width, height;
    Uint64 now, last, deltaTime;
    GetDisplaySize(&width, &height);

    SDL_Window* win        = SDL_CreateWindow("Switch-Error", 0, 0, width, height, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);

    int layers = 9;

    for(int layer = 0; layer < layers; layer++) {
        for(int i = 0; i < 100; i++) {
            srand(time(0));

            int brightness = 50 - layer * (rand() * 5);
            SDL_Rect rect;
            rect.x = rand() % width;
            rect.y = i*i;
            rect.w = rand() % 20;
            rect.h = rand() % 20;

            stars.push_back(new Star{
                &rect,
                0.03f,
                layer,
                {
                    120 + brightness,
                    120 + brightness,
                    120 + brightness
                }
            });
        }
    }

    while(appletMainLoop()){
        SDL_Event e;
		if ( SDL_PollEvent(&e) ) {
			if (e.type == SDL_QUIT)
				break;
		}

        last = now;
        now = SDL_GetPerformanceCounter();

        deltaTime = (double)((now - last) * 1000 / (double) SDL_GetPerformanceFrequency() );

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

        for(int i = 0; i < stars.size(); i++){
            Star* star = stars[i];

            if(star->rect->x >= width){
                star->rect->x = -star->rect->w;
            }

            star->rect->x += 1;
            // star->rect->y += (cos((star->rect->x / star->layer)) * .9f ) / 16;
            SDL_SetRenderDrawColor(renderer, star->color[0], star->color[1], star->color[2], 255);
            SDL_RenderFillRect( renderer, star->rect );
        }

		SDL_RenderPresent(renderer);
		SDL_Delay(0);
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
    "Psst, don't forget to blame Melon",
    "Potsanity when?",
    "Why are you acting so random?",
    "Enough! My ship sails in the morning",
    "Do you want 2 or 7 of those?",
    "Lamp oil, rope, bombs you want it, it's yours my friend as long as you have enough rupees",
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
    "They say ZAPD is good software",
};

void DetectAppletMode() {
    AppletType at = appletGetAppletType();
    if (at == AppletType_Application || at == AppletType_SystemApplication)
        return;

    srand(time(0));
    Ship::Switch::PrintErrorMessageToScreen(
        "\x1b[2;2HYou've launched the Ship while in Applet mode."
        "\x1b[4;2HPlease relaunch while in full-memory mode."
        "\x1b[5;2HHold R when opening any game to enter HBMenu."
        "\x1b[44;2H%s."
    , RandomTexts[rand() % 25]);
}

void Ship::Switch::ThrowMissingOTR(std::string OTRPath){
    Ship::Switch::PrintErrorMessageToScreen(
        "\x1b[2;2HYou've launched the Ship without the OTR file."
        "\x1b[4;2HPlease relaunch making sure %s exists."
        "\x1b[44;2H%s."
    , OTRPath.c_str(), RandomTexts[rand() % 25]);
}
#endif