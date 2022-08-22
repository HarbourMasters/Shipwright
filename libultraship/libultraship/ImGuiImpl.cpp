#include "ImGuiImpl.h"

#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <algorithm>
#include <vector>

#include <cstddef>
#include <PR/ultra64/types.h>
#include <PR/ultra64/sptask.h>
#include <PR/ultra64/pi.h>
#include <PR/ultra64/message.h>
#include "../../soh/include/z64audio.h"
#include "Archive.h"
#include "Console.h"
#include "Hooks.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Lib/ImGui/imgui_internal.h"
#include "ResourceMgr.h"
#include "Window.h"
#include "Cvar.h"
#include "GameOverlay.h"
#include "Texture.h"
#include "../Fast3D/gfx_pc.h"
#include "Lib/stb/stb_image.h"
#include "Lib/Fast3D/gfx_rendering_api.h"
#include "Lib/spdlog/include/spdlog/common.h"
#include "UltraController.h"

#ifdef __WIIU__
#include <gx2/registers.h> // GX2SetViewport / GX2SetScissor

#include "Lib/ImGui/backends/wiiu/imgui_impl_gx2.h"
#include "Lib/ImGui/backends/wiiu/imgui_impl_wiiu.h"

#include "Lib/Fast3D/gfx_wiiu.h"
#include "Lib/Fast3D/gfx_gx2.h"
#endif

#if __APPLE__
#include <SDL_hints.h>
#else
#include <SDL2/SDL_hints.h>
#endif

#ifdef __SWITCH__
#include "SwitchImpl.h"
#endif

#ifdef ENABLE_OPENGL
#include "Lib/ImGui/backends/imgui_impl_opengl3.h"
#include "Lib/ImGui/backends/imgui_impl_sdl.h"

#endif

#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
#include "Lib/ImGui/backends/imgui_impl_dx11.h"
#include "Lib/ImGui/backends/imgui_impl_win32.h"
#include <Windows.h>

IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif

using namespace Ship;
bool oldCursorState = true;

#define EXPERIMENTAL() \
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 50, 50, 255)); \
    InsertPadding(3.0f); \
    ImGui::Text("Experimental"); \
    ImGui::PopStyleColor(); \
    PaddedSeparator(false, true);
#define TOGGLE_BTN ImGuiKey_F1
#define TOGGLE_PAD_BTN ImGuiKey_GamepadBack
#define HOOK(b) if(b) needs_save = true;
OSContPad* pads;

std::map<std::string, GameAsset*> DefaultAssets;
std::vector<std::string> emptyArgs;

bool isBetaQuestEnabled = false;

enum SeqPlayers {
    /* 0 */ SEQ_BGM_MAIN,
    /* 1 */ SEQ_FANFARE,
    /* 2 */ SEQ_SFX,
    /* 3 */ SEQ_BGM_SUB,
    /* 4 */ SEQ_MAX
};

extern "C" {
    void enableBetaQuest() { isBetaQuestEnabled = true; }
    void disableBetaQuest() { isBetaQuestEnabled = false; }
}

namespace SohImGui {

    WindowImpl impl;
    ImGuiIO* io;
    std::shared_ptr<Console> console = std::make_shared<Console>();
    GameOverlay* overlay = new GameOverlay;
    InputEditor* controller = new InputEditor;
    static ImVector<ImRect> s_GroupPanelLabelStack;
    bool p_open = false;
    bool needs_save = false;
    int lastBackendID = 0;
    bool statsWindowOpen;

    const char* filters[3] = {
#ifdef __WIIU__
        "",
#else
        "Three-Point",
#endif
        "Linear",
        "None"
    };

    std::pair<const char*, const char*> backends[] = {
#ifdef _WIN32
        { "dx11", "DirectX" },
#endif
#ifndef __WIIU__
        { "sdl", "OpenGL" }
#else
        { "wiiu", "GX2" }
#endif
    };


    const char* powers[9] = {
        "Vanilla (1x)",
        "Double (2x)",
        "Quadruple (4x)",
        "Octuple (8x)",
        "Hexadecuple (16x)",
        "Duotrigintuple (32x)",
        "Quattuorsexagintuple (64x)",
        "Octoviginticentuple (128x)",
        "Hexaquinquagintiducentuple (256x)"
    };

    std::map<std::string, std::vector<std::string>> hiddenwindowCategories;
    std::map<std::string, std::vector<std::string>> windowCategories;
    std::map<std::string, CustomWindow> customWindows;

    void UpdateAudio() {
        Audio_SetGameVolume(SEQ_BGM_MAIN, CVar_GetFloat("gMainMusicVolume", 1));
        Audio_SetGameVolume(SEQ_BGM_SUB, CVar_GetFloat("gSubMusicVolume", 1));
        Audio_SetGameVolume(SEQ_FANFARE, CVar_GetFloat("gSFXMusicVolume", 1));
        Audio_SetGameVolume(SEQ_SFX, CVar_GetFloat("gFanfareVolume", 1));
    }

    void InitSettings() {
        Ship::RegisterHook<Ship::AudioInit>(UpdateAudio);
        Ship::RegisterHook<Ship::GfxInit>([] {
            gfx_get_current_rendering_api()->set_texture_filter((FilteringMode)CVar_GetS32("gTextureFilter", FILTER_THREE_POINT));
            if (CVar_GetS32("gConsoleEnabled", 0)) {
                console->Open();
            } else {
                console->Close();
            }
            SohImGui::controller->Opened = CVar_GetS32("gControllerConfigurationEnabled", 0);
            UpdateAudio();
        });
    }

    int GetBackendID(std::shared_ptr<Mercury> cfg) {
        std::string backend = cfg->getString("Window.GfxBackend");
        if (backend.empty()) {
            return 0;
        }

        for (size_t i = 0; i < (sizeof(backends) / sizeof(backends[0])); i++) {
            if(backend == backends[i].first) {
				return i;
			}
        }

        return 0;
    }

    int ClampFloatToInt(float value, int min, int max) {
        return fmin(fmax(value, min), max);
    }

    void Tooltip(const char* text) {
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", BreakTooltip(text, 60).c_str());
        }
    }

    void ImGuiWMInit() {
        switch (impl.backend) {
#ifdef __WIIU__
        case Backend::GX2:
            ImGui_ImplWiiU_Init();
            break;
#else
        case Backend::SDL:
            SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
            ImGui_ImplSDL2_InitForOpenGL(static_cast<SDL_Window*>(impl.sdl.window), impl.sdl.context);
            break;
#endif
#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
        case Backend::DX11:
            ImGui_ImplWin32_Init(impl.dx11.window);
            break;
#endif
        default:
            break;
        }

    }

    void ImGuiBackendInit() {
        switch (impl.backend) {
#ifdef __WIIU__
        case Backend::GX2:
            ImGui_ImplGX2_Init();
            break;
#else
        case Backend::SDL:
        #if defined(__APPLE__)
            ImGui_ImplOpenGL3_Init("#version 410 core");
        #else
            ImGui_ImplOpenGL3_Init("#version 120");
        #endif
            break;
#endif

#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
        case Backend::DX11:
            ImGui_ImplDX11_Init(static_cast<ID3D11Device*>(impl.dx11.device), static_cast<ID3D11DeviceContext*>(impl.dx11.device_context));
            break;
#endif
        default:
            break;
        }
    }

    void ImGuiProcessEvent(EventImpl event) {
        switch (impl.backend) {
#ifdef __WIIU__
        case Backend::GX2:
            if (!ImGui_ImplWiiU_ProcessInput((ImGui_ImplWiiU_ControllerInput*)event.gx2.input)) {
                
            }
            break;
#else
        case Backend::SDL:
            ImGui_ImplSDL2_ProcessEvent(static_cast<const SDL_Event*>(event.sdl.event));
            break;
#endif
#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
        case Backend::DX11:
            ImGui_ImplWin32_WndProcHandler(static_cast<HWND>(event.win32.handle), event.win32.msg, event.win32.wparam, event.win32.lparam);
            break;
#endif
        default:
            break;
        }
    }

    void ImGuiWMNewFrame() {
        switch (impl.backend) {
#ifdef __WIIU__
        case Backend::GX2:
            break;
#else
        case Backend::SDL:
            ImGui_ImplSDL2_NewFrame(static_cast<SDL_Window*>(impl.sdl.window));
            break;
#endif
#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
        case Backend::DX11:
            ImGui_ImplWin32_NewFrame();
            break;
#endif
        default:
            break;
        }
    }

    void ImGuiBackendNewFrame() {
        switch (impl.backend) {
#ifdef __WIIU__
        case Backend::GX2:
            io->DeltaTime = (float) frametime / 1000.0f / 1000.0f;
            ImGui_ImplGX2_NewFrame();
            break;
#else
        case Backend::SDL:
            ImGui_ImplOpenGL3_NewFrame();
            break;
#endif
#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
        case Backend::DX11:
            ImGui_ImplDX11_NewFrame();
            break;
#endif
        default:
            break;
        }
    }

    void ImGuiRenderDrawData(ImDrawData* data) {
        switch (impl.backend) {
#ifdef __WIIU__
        case Backend::GX2:
            ImGui_ImplGX2_RenderDrawData(data);

            // Reset viewport and scissor for drawing the keyboard
            GX2SetViewport(0.0f, 0.0f, io->DisplaySize.x, io->DisplaySize.y, 0.0f, 1.0f);
            GX2SetScissor(0, 0, io->DisplaySize.x, io->DisplaySize.y);
            ImGui_ImplWiiU_DrawKeyboardOverlay();
            break;
#else
        case Backend::SDL:
            ImGui_ImplOpenGL3_RenderDrawData(data);
            break;
#endif
#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
        case Backend::DX11:
            ImGui_ImplDX11_RenderDrawData(data);
            break;
#endif
        default:
            break;
        }
    }

    bool UseViewports() {
        switch (impl.backend) {
        case Backend::DX11:
            return true;
        default:
            return false;
        }
    }

    void ShowCursor(bool hide, Dialogues d) {
        if (d == Dialogues::dLoadSettings) {
            Window::GetInstance()->ShowCursor(hide);
            return;
        }

        if (d == Dialogues::dConsole && CVar_GetS32("gOpenMenuBar", 0)) {
            return;
        }
        if (!Window::GetInstance()->IsFullscreen()) {
            oldCursorState = false;
            return;
        }

        if (oldCursorState != hide) {
            oldCursorState = hide;
            Window::GetInstance()->ShowCursor(hide);
        }
    }

    void LoadTexture(const std::string& name, const std::string& path) {
        GfxRenderingAPI* api = gfx_get_current_rendering_api();
        const auto res = Window::GetInstance()->GetResourceManager()->LoadFile(path);

        const auto asset = new GameAsset{ api->new_texture() };
        uint8_t* img_data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(res->buffer.get()), res->dwBufferSize, &asset->width, &asset->height, nullptr, 4);

        if (img_data == nullptr) {
            std::cout << "Found error: " << stbi_failure_reason() << std::endl;
            return;
        }

        api->select_texture(0, asset->textureId);
        api->set_sampler_parameters(0, false, 0, 0);
        api->upload_texture(img_data, asset->width, asset->height);

        DefaultAssets[name] = asset;
        stbi_image_free(img_data);
    }

    void LoadPickersColors(ImVec4& ColorArray, const char* cvarname, const ImVec4& default_colors, bool has_alpha)
    {
        Color_RGBA8 defaultColors;
        defaultColors.r = default_colors.x;
        defaultColors.g = default_colors.y;
        defaultColors.b = default_colors.z;
        defaultColors.a = default_colors.w;

        Color_RGBA8 cvarColor = CVar_GetRGBA(cvarname, defaultColors);

        ColorArray.x = cvarColor.r / 255.0;
        ColorArray.y = cvarColor.g / 255.0;
        ColorArray.z = cvarColor.b / 255.0;
        ColorArray.w = cvarColor.a / 255.0;
    }

    void LoadResource(const std::string& name, const std::string& path, const ImVec4& tint) {
        GfxRenderingAPI* api = gfx_get_current_rendering_api();
        const auto res = static_cast<Ship::Texture*>(Window::GetInstance()->GetResourceManager()->LoadResource(path).get());

        std::vector<uint8_t> texBuffer;
        texBuffer.reserve(res->width * res->height * 4);

        switch (res->texType) {
        case Ship::TextureType::RGBA32bpp:
            texBuffer.assign(res->imageData, res->imageData + (res->width * res->height * 4));
            break;
        case Ship::TextureType::GrayscaleAlpha8bpp:
            for (int32_t i = 0; i < res->width * res->height; i++) {
                uint8_t ia = res->imageData[i];
                uint8_t color = ((ia >> 4) & 0xF) * 255 / 15;
                uint8_t alpha = (ia & 0xF) * 255 / 15;
                texBuffer.push_back(color);
                texBuffer.push_back(color);
                texBuffer.push_back(color);
                texBuffer.push_back(alpha);
            }
            break;
        default:
            // TODO convert other image types
            SPDLOG_WARN("SohImGui::LoadResource: Attempting to load unsupporting image type %s", path.c_str());
            return;
        }

        for (size_t pixel = 0; pixel < texBuffer.size() / 4; pixel++) {
            texBuffer[pixel * 4 + 0] *= tint.x;
            texBuffer[pixel * 4 + 1] *= tint.y;
            texBuffer[pixel * 4 + 2] *= tint.z;
            texBuffer[pixel * 4 + 3] *= tint.w;
        }

        const auto asset = new GameAsset{ api->new_texture() };

        api->select_texture(0, asset->textureId);
        api->set_sampler_parameters(0, false, 0, 0);
        api->upload_texture(texBuffer.data(), res->width, res->height);

        DefaultAssets[name] = asset;
    }

    void Init(WindowImpl window_impl) {
        CVar_Load();
        impl = window_impl;
        ImGuiContext* ctx = ImGui::CreateContext();
        ImGui::SetCurrentContext(ctx);
        io = &ImGui::GetIO();
        io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io->Fonts->AddFontDefault();
        statsWindowOpen = CVar_GetS32("gStatsEnabled", 0);
    #ifdef __SWITCH__
        Ship::Switch::SetupFont(io->Fonts);
    #endif

    #ifdef __WIIU__
        // Scale everything by 2 for the Wii U
        ImGui::GetStyle().ScaleAllSizes(2.0f);
        io->FontGlobalScale = 2.0f;

        // Setup display sizes
        io->DisplaySize.x = window_impl.gx2.width;
        io->DisplaySize.y =  window_impl.gx2.height;
    #endif

        lastBackendID = GetBackendID(Window::GetInstance()->GetConfig());
        if (CVar_GetS32("gOpenMenuBar", 0) != 1) {
            #if defined(__SWITCH__) || defined(__WIIU__)
            SohImGui::overlay->TextDrawNotification(30.0f, true, "Press - to access enhancements menu");
            #else
            SohImGui::overlay->TextDrawNotification(30.0f, true, "Press F1 to access enhancements menu");
            #endif
        }

        auto imguiIniPath = Ship::Window::GetPathRelativeToAppDirectory("imgui.ini");
        auto imguiLogPath = Ship::Window::GetPathRelativeToAppDirectory("imgui_log.txt");
        io->IniFilename = strcpy(new char[imguiIniPath.length() + 1], imguiIniPath.c_str());
        io->LogFilename = strcpy(new char[imguiLogPath.length() + 1], imguiLogPath.c_str());

        if (UseViewports()) {
            io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        }

        if (CVar_GetS32("gControlNav", 0) && CVar_GetS32("gOpenMenuBar", 0)) {
            io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_NavEnableKeyboard;
        } else {
            io->ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
        }

        console->Init();
        overlay->Init();
        controller->Init();
        ImGuiWMInit();
        ImGuiBackendInit();
    #ifdef __SWITCH__
        ImGui::GetStyle().ScaleAllSizes(2);
    #endif

        Ship::RegisterHook<Ship::GfxInit>([] {
            if (Window::GetInstance()->IsFullscreen())
                ShowCursor(CVar_GetS32("gOpenMenuBar", 0), Dialogues::dLoadSettings);

            LoadTexture("Game_Icon", "assets/ship_of_harkinian/icons/gSohIcon.png");
            LoadTexture("A-Btn", "assets/ship_of_harkinian/buttons/ABtn.png");
            LoadTexture("B-Btn", "assets/ship_of_harkinian/buttons/BBtn.png");
            LoadTexture("L-Btn", "assets/ship_of_harkinian/buttons/LBtn.png");
            LoadTexture("R-Btn", "assets/ship_of_harkinian/buttons/RBtn.png");
            LoadTexture("Z-Btn", "assets/ship_of_harkinian/buttons/ZBtn.png");
            LoadTexture("Start-Btn", "assets/ship_of_harkinian/buttons/StartBtn.png");
            LoadTexture("C-Left", "assets/ship_of_harkinian/buttons/CLeft.png");
            LoadTexture("C-Right", "assets/ship_of_harkinian/buttons/CRight.png");
            LoadTexture("C-Up", "assets/ship_of_harkinian/buttons/CUp.png");
            LoadTexture("C-Down", "assets/ship_of_harkinian/buttons/CDown.png");
        });

        Ship::RegisterHook<Ship::ControllerRead>([](OSContPad* cont_pad) {
            pads = cont_pad;
        });

        InitSettings();

        CVar_SetS32("gRandoGenerating", 0);
        CVar_SetS32("gNewSeedGenerated", 0);
        CVar_SetS32("gNewFileDropped", 0);
        CVar_SetString("gDroppedFile", "None");

    #ifdef __SWITCH__
        Switch::ApplyOverclock();
    #endif
    }

    void Update(EventImpl event) {
        if (needs_save) {
            CVar_Save();
            needs_save = false;
        }
        ImGuiProcessEvent(event);
    }

#define BindButton(btn, status) ImGui::Image(GetTextureByID(DefaultAssets[btn]->textureId), ImVec2(16.0f * scale, 16.0f * scale), ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, (status) ? 255 : 0));

    void BindAudioSlider(const char* name, const char* key, float defaultValue, SeqPlayers playerId)
    {
        float value = CVar_GetFloat(key, defaultValue);

        ImGui::Text(name, static_cast<int>(100 * value));
        if (ImGui::SliderFloat((std::string("##") + key).c_str(), &value, 0.0f, 1.0f, "")) {
            const float volume = floorf(value * 100) / 100;
            CVar_SetFloat(key, volume);
            needs_save = true;
            Audio_SetGameVolume(playerId, volume);
        }
    }

    void EnhancementCombobox(const char* name, const char* ComboArray[], size_t arraySize, uint8_t FirstTimeValue = 0) {
        if (FirstTimeValue <= 0) {
            FirstTimeValue = 0;
        }
        uint8_t selected = CVar_GetS32(name, FirstTimeValue);
        uint8_t DefaultValue = selected;
        std::string comboName = std::string("##") + std::string(name);
        if (ImGui::BeginCombo(comboName.c_str(), ComboArray[DefaultValue])) {
            for (uint8_t i = 0; i < arraySize; i++) {
                if (strlen(ComboArray[i]) > 1) {
                    if (ImGui::Selectable(ComboArray[i], i == selected)) {
                        CVar_SetS32(name, i);
                        selected = i;
                        needs_save = true;
                    }
                }
            }
            ImGui::EndCombo();
        }
    }

    void EnhancementRadioButton(const char* text, const char* cvarName, int id) {
        /*Usage :
        EnhancementRadioButton("My Visible Name","gMyCVarName", MyID);
        First arg is the visible name of the Radio button
        Second is the cvar name where MyID will be saved.
        Note: the CVar name should be the same to each Buddies.
        Example :
            EnhancementRadioButton("English", "gLanguages", 0);
            EnhancementRadioButton("German", "gLanguages", 1);
            EnhancementRadioButton("French", "gLanguages", 2);
        */
        std::string make_invisible = "##";
        make_invisible += text;
        make_invisible += cvarName;

        int val = CVar_GetS32(cvarName, 0);
        if (ImGui::RadioButton(make_invisible.c_str(), id == val)) {
            CVar_SetS32(cvarName, id);
            needs_save = true;
        }
        ImGui::SameLine();
        ImGui::Text("%s", text);
    }

    void EnhancementCheckbox(const char* text, const char* cvarName)
    {
        bool val = (bool)CVar_GetS32(cvarName, 0);
        if (ImGui::Checkbox(text, &val)) {
            CVar_SetS32(cvarName, val);
            needs_save = true;
        }
    }

    void EnhancementButton(const char* text, const char* cvarName)
    {
        bool val = (bool)CVar_GetS32(cvarName, 0);
        if (ImGui::Button(text)) {
            CVar_SetS32(cvarName, !val);
            CVar_SetS32(cvarName, !val);
            needs_save = true;
        }
    }

    void EnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue, bool PlusMinusButton)
    {
        int val = CVar_GetS32(cvarName, defaultValue);
        ImGui::Text(text, val);
        if(PlusMinusButton) {
            std::string MinusBTNName = " - ##";
            MinusBTNName += cvarName;
            if (ImGui::Button(MinusBTNName.c_str())) {
                val--;
                CVar_SetS32(cvarName, val);
                needs_save = true;
            }
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
        }

        if (ImGui::SliderInt(id, &val, min, max, format))
        {
            CVar_SetS32(cvarName, val);
            needs_save = true;
        }

        if(PlusMinusButton) {
            std::string PlusBTNName = " + ##";
            PlusBTNName += cvarName;
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
            if (ImGui::Button(PlusBTNName.c_str())) {
                val++;
                CVar_SetS32(cvarName, val);
                needs_save = true;
            }
        }

        if (val < min)
        {
            val = min;
            CVar_SetS32(cvarName, val);
            needs_save = true;
        }

        if (val > max)
        {
            val = max;
            CVar_SetS32(cvarName, val);
            needs_save = true;
        }
    }

    void EnhancementSliderFloat(const char* text, const char* id, const char* cvarName, float min, float max, const char* format, float defaultValue, bool isPercentage, bool PlusMinusButton)
    {
        float val = CVar_GetFloat(cvarName, defaultValue);

        if (!isPercentage)
            ImGui::Text(text, val);
        else
            ImGui::Text(text, static_cast<int>(100 * val));

        InsertPadding();

        if(PlusMinusButton) {
            std::string MinusBTNName = " - ##";
            MinusBTNName += cvarName;
            if (ImGui::Button(MinusBTNName.c_str())) {
                if (!isPercentage)
                    val -= 0.1f;
                else
                    val -= 0.01f;
                CVar_SetFloat(cvarName, val);
                needs_save = true;
            }
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
        }
        if (PlusMinusButton) {
        #ifdef __WIIU__
            ImGui::PushItemWidth(ImGui::GetWindowSize().x - 79.0f * 2);
        #else
            ImGui::PushItemWidth(ImGui::GetWindowSize().x - 79.0f);
        #endif
        }
        if (ImGui::SliderFloat(id, &val, min, max, format))
        {
            CVar_SetFloat(cvarName, val);
            needs_save = true;
        }
        if (PlusMinusButton) {
            ImGui::PopItemWidth();
        }
        if(PlusMinusButton) {
            std::string PlusBTNName = " + ##";
            PlusBTNName += cvarName;
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
            if (ImGui::Button(PlusBTNName.c_str())) {
                if (!isPercentage)
                    val += 0.1f;
                else
                    val += 0.01f;
                CVar_SetFloat(cvarName, val);
                needs_save = true;
            }
        }

        if (val < min)
        {
            val = min;
            CVar_SetFloat(cvarName, val);
            needs_save = true;
        }

        if (val > max)
        {
            val = max;
            CVar_SetFloat(cvarName, val);
            needs_save = true;
        }
    }

    void EnhancementCombo(const std::string& name, const char* cvarName, const std::vector<std::string>& items, int defaultValue) {

        if (ImGui::BeginCombo(name.c_str(), items[static_cast<int>(CVar_GetS32(cvarName, defaultValue))].c_str())) {
            for (int settingIndex = 0; settingIndex < (int) items.size(); settingIndex++) {
                if (ImGui::Selectable(items[settingIndex].c_str())) {
                    CVar_SetS32(cvarName, settingIndex);
                    needs_save = true;

                }
            }
            ImGui::EndCombo();
        }
    }

    void RandomizeColor(const char* cvarName, ImVec4* colors) {
        std::string Cvar_Red = cvarName;
        Cvar_Red += "R";
        std::string Cvar_Green = cvarName;
        Cvar_Green += "G";
        std::string Cvar_Blue = cvarName;
        Cvar_Blue += "B";
        std::string Cvar_RBM = cvarName;
        Cvar_RBM += "RBM";
        std::string MakeInvisible = "##";
        MakeInvisible += cvarName;
        MakeInvisible += "Random";
        std::string FullName = "Random";
        FullName += MakeInvisible;
        if (ImGui::Button(FullName.c_str())) {
            s16 RND_R = rand() % (255 - 0);
            s16 RND_G = rand() % (255 - 0);
            s16 RND_B = rand() % (255 - 0);
            colors->x = (float)RND_R / 255;
            colors->y = (float)RND_G / 255;
            colors->z = (float)RND_B / 255;
            CVar_SetS32(Cvar_Red.c_str(), ClampFloatToInt(colors->x * 255, 0, 255));
            CVar_SetS32(Cvar_Green.c_str(), ClampFloatToInt(colors->y * 255, 0, 255));
            CVar_SetS32(Cvar_Blue.c_str(), ClampFloatToInt(colors->z * 255, 0, 255));
            CVar_SetS32(Cvar_RBM.c_str(), 0); //On click disable rainbow mode.
            needs_save = true;
        }
        Tooltip("Chooses a random color\nOverwrites previously chosen color");
    }

    void RainbowColor(const char* cvarName, ImVec4* colors) {
        std::string Cvar_RBM = cvarName;
        Cvar_RBM += "RBM";
        std::string MakeInvisible = "Rainbow";
        MakeInvisible += "##";
        MakeInvisible += cvarName;
        MakeInvisible += "Rainbow";

        EnhancementCheckbox(MakeInvisible.c_str(), Cvar_RBM.c_str());
        Tooltip("Cycles through colors on a timer\nOverwrites previously chosen color");
    }

    void ResetColor(const char* cvarName, ImVec4* colors, ImVec4 defaultcolors, bool has_alpha) {
        std::string Cvar_RBM = cvarName;
        Cvar_RBM += "RBM";
        std::string MakeInvisible = "Reset";
        MakeInvisible += "##";
        MakeInvisible += cvarName;
        MakeInvisible += "Reset";
        if (ImGui::Button(MakeInvisible.c_str())) {
            colors->x = defaultcolors.x / 255;
            colors->y = defaultcolors.y / 255;
            colors->z = defaultcolors.z / 255;
            if (has_alpha) { colors->w = defaultcolors.w / 255; };

            Color_RGBA8 colorsRGBA;
            colorsRGBA.r = defaultcolors.x / 255;
            colorsRGBA.g = defaultcolors.y / 255;
            colorsRGBA.b = defaultcolors.z / 255;
            if (has_alpha) { colorsRGBA.a = defaultcolors.w / 255; };

            CVar_SetRGBA(cvarName, colorsRGBA);
            CVar_SetS32(Cvar_RBM.c_str(), 0); //On click disable rainbow mode.
            needs_save = true;
        }
        Tooltip("Revert colors to the game's original colors (GameCube version)\nOverwrites previously chosen color");
    }

    void EnhancementColor(const char* text, const char* cvarName, ImVec4 ColorRGBA, ImVec4 default_colors, bool allow_rainbow, bool has_alpha, bool TitleSameLine) {
        LoadPickersColors(ColorRGBA, cvarName, default_colors, has_alpha);

        ImGuiColorEditFlags flags = ImGuiColorEditFlags_None;

        if (!TitleSameLine) {
            ImGui::Text("%s", text);
            flags = ImGuiColorEditFlags_NoLabel;
        }

        ImGui::PushID(cvarName);

        if (!has_alpha) {
            if (ImGui::ColorEdit3(text, (float*)&ColorRGBA, flags))
            {
                Color_RGBA8 colors;
                colors.r = ColorRGBA.x * 255.0;
                colors.g = ColorRGBA.y * 255.0;
                colors.b = ColorRGBA.z * 255.0;
                colors.a = ColorRGBA.w * 255.0;

                CVar_SetRGBA(cvarName, colors);
                needs_save = true;
            }
        }
        else
        {
            if (ImGui::ColorEdit4(text, (float*)&ColorRGBA, flags))
            {
                Color_RGBA8 colors;
                colors.r = ColorRGBA.x / 255;
                colors.g = ColorRGBA.y / 255;
                colors.b = ColorRGBA.z / 255;
                colors.a = ColorRGBA.w / 255;

                CVar_SetRGBA(cvarName, colors);
                needs_save = true;
            }
        }

        ImGui::PopID();

        //ImGui::SameLine(); // Removing that one to gain some width spacing on the HUD editor
        ImGui::PushItemWidth(-FLT_MIN);
        ResetColor(cvarName, &ColorRGBA, default_colors, has_alpha);
        ImGui::SameLine();
        RandomizeColor(cvarName, &ColorRGBA);
        if (allow_rainbow) {
            if (ImGui::GetContentRegionAvail().x > 185) {
                ImGui::SameLine();
            }
            RainbowColor(cvarName, &ColorRGBA);
        }
        ImGui::NewLine();
        ImGui::PopItemWidth();
    }

    void DrawMainMenuAndCalculateGameSize(void) {
        console->Update();
        ImGuiBackendNewFrame();
        ImGuiWMNewFrame();
        ImGui::NewFrame();

        const std::shared_ptr<Window> wnd = Window::GetInstance();
        const std::shared_ptr<Mercury> pConf = Window::GetInstance()->GetConfig();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize;
        if (CVar_GetS32("gOpenMenuBar", 0)) window_flags |= ImGuiWindowFlags_MenuBar;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(ImVec2((int) wnd->GetCurrentWidth(), (int) wnd->GetCurrentHeight()));
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::Begin("Main - Deck", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        ImVec2 top_left_pos = ImGui::GetWindowPos();

        const ImGuiID dockId = ImGui::GetID("main_dock");

        if (!ImGui::DockBuilderGetNode(dockId)) {
            ImGui::DockBuilderRemoveNode(dockId);
            ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_NoTabBar);

            ImGui::DockBuilderDockWindow("Main Game", dockId);

            ImGui::DockBuilderFinish(dockId);
        }

        ImGui::DockSpace(dockId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoDockingInCentralNode);

        if (ImGui::IsKeyPressed(TOGGLE_BTN) ||
           (ImGui::IsKeyPressed(TOGGLE_PAD_BTN) && CVar_GetS32("gControlNav", 0))) {
            bool menu_bar = CVar_GetS32("gOpenMenuBar", 0);
            CVar_SetS32("gOpenMenuBar", !menu_bar);
            needs_save = true;
            Window::GetInstance()->SetMenuBar(menu_bar);
            ShowCursor(menu_bar, Dialogues::dMenubar);
            Window::GetInstance()->GetControlDeck()->SaveControllerSettings();
            if (CVar_GetS32("gControlNav", 0) && CVar_GetS32("gOpenMenuBar", 0)) {
                io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_NavEnableKeyboard;
            } else {
                io->ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
            }
        }

        #if __APPLE__
        if ((ImGui::IsKeyDown(ImGuiKey_LeftSuper) ||
             ImGui::IsKeyDown(ImGuiKey_RightSuper)) &&
             ImGui::IsKeyPressed(ImGuiKey_R, false)) {
            console->Dispatch("reset");
        }
        #else
        if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
             ImGui::IsKeyDown(ImGuiKey_RightCtrl)) &&
             ImGui::IsKeyPressed(ImGuiKey_R, false)) {
            console->Dispatch("reset");
        }
        #endif

        if (ImGui::BeginMenuBar()) {
            if (DefaultAssets.contains("Game_Icon")) {
            #ifdef __SWITCH__
                ImVec2 iconSize = ImVec2(20.0f, 20.0f);
                float posScale = 1.0f;
            #elif defined(__WIIU__)
                ImVec2 iconSize = ImVec2(16.0f * 2, 16.0f * 2);
                float posScale = 2.0f;
            #else
                ImVec2 iconSize = ImVec2(16.0f, 16.0f);
                float posScale = 1.0f;
            #endif
                ImGui::SetCursorPos(ImVec2(5, 2.5f) * posScale);
                ImGui::Image(GetTextureByID(DefaultAssets["Game_Icon"]->textureId), iconSize);
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(25, 0) * posScale);
            }

            static ImVec2 windowPadding(8.0f, 8.0f);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, windowPadding);
            if (ImGui::BeginMenu("Shipwright")) {
                if (ImGui::MenuItem("Reset",
                    #if __APPLE__
                    "Command-R"
                    #else
                    "Ctrl+R"
                    #endif
                    )) {
                    console->Dispatch("reset");
                }
                ImGui::EndMenu();
            }

            ImGui::SetCursorPosY(0.0f);

            if (ImGui::BeginMenu("Settings"))
            {
                if (ImGui::BeginMenu("Audio")) {
                    EnhancementSliderFloat("Master Volume: %d %%", "##Master_Vol", "gGameMasterVolume", 0.0f, 1.0f, "", 1.0f, true);
                    InsertPadding();
                    BindAudioSlider("Main Music Volume: %d %%", "gMainMusicVolume", 1.0f, SEQ_BGM_MAIN);
                    InsertPadding();
                    BindAudioSlider("Sub Music Volume: %d %%", "gSubMusicVolume", 1.0f, SEQ_BGM_SUB);
                    InsertPadding();
                    BindAudioSlider("Sound Effects Volume: %d %%", "gSFXMusicVolume", 1.0f, SEQ_SFX);
                    InsertPadding();
                    BindAudioSlider("Fanfare Volume: %d %%", "gFanfareVolume", 1.0f, SEQ_FANFARE);

                    ImGui::EndMenu();
                }

                InsertPadding();

                if (ImGui::BeginMenu("Controller")) {
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 (12.0f, 6.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                    if (ImGui::Button(GetWindowButtonText("Controller Configuration", CVar_GetS32("gControllerConfigurationEnabled", 0)).c_str()))
                    {
                        bool currentValue = CVar_GetS32("gControllerConfigurationEnabled", 0);
                        CVar_SetS32("gControllerConfigurationEnabled", !currentValue);
                        needs_save = true;
                        controller->Opened = CVar_GetS32("gControllerConfigurationEnabled", 0);
                    }
                    ImGui::PopStyleColor(1);
                    ImGui::PopStyleVar(3);
                #ifndef __SWITCH__
                    PaddedEnhancementCheckbox("Use Controller Navigation", "gControlNav", true, false);
                    Tooltip("Allows controller navigation of the menu bar\nD-pad to move between items, A to select, and X to grab focus on the menu bar");
                #endif
                    PaddedEnhancementCheckbox("Show Inputs", "gInputEnabled", true, false);
                    Tooltip("Shows currently pressed inputs on the bottom right of the screen");
                    InsertPadding();
                    ImGui::PushItemWidth(ImGui::GetWindowSize().x - 20.0f);
                    EnhancementSliderFloat("Input Scale: %.1f", "##Input", "gInputScale", 1.0f, 3.0f, "", 1.0f, false);
                    Tooltip("Sets the on screen size of the displayed inputs from the Show Inputs setting");
                    ImGui::PopItemWidth();

                    ImGui::EndMenu();
                }

                InsertPadding();

                if (ImGui::BeginMenu("Graphics")) {
                #ifndef __APPLE__
                    EnhancementSliderFloat("Internal Resolution: %d %%", "##IMul", "gInternalResolution", 0.5f, 2.0f, "", 1.0f, true, true);
                    Tooltip("Multiplies your output resolution by the value inputted, as a more intensive but effective form of anti-aliasing");
                    gfx_current_dimensions.internal_mul = CVar_GetFloat("gInternalResolution", 1);
                #endif
                #ifndef __WIIU__
                    PaddedEnhancementSliderInt("MSAA: %d", "##IMSAA", "gMSAAValue", 1, 8, "", 1, false, true, false);
                    Tooltip("Activates multi-sample anti-aliasing when above 1x up to 8x for 8 samples for every pixel");
                    gfx_msaa_level = CVar_GetS32("gMSAAValue", 1);
                #endif

                    if (impl.backend == Backend::DX11)
                    {
                        const char* cvar = "gExtraLatencyThreshold";
                        int val = CVar_GetS32(cvar, 80);
                        val = MAX(MIN(val, 360), 0);
                        int fps = val;

                        InsertPadding();

                        if (fps == 0)
                        {
                            ImGui::Text("Jitter fix: Off");
                        }
                        else
                        {
                            ImGui::Text("Jitter fix: >= %d FPS", fps);
                        }

                        std::string MinusBTNELT = " - ##ExtraLatencyThreshold";
                        std::string PlusBTNELT = " + ##ExtraLatencyThreshold";
                        if (ImGui::Button(MinusBTNELT.c_str())) {
                            val--;
                            CVar_SetS32(cvar, val);
                            needs_save = true;
                        }
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                        ImGui::PushItemWidth(ImGui::GetWindowSize().x - 79.0f);
                        if (ImGui::SliderInt("##ExtraLatencyThreshold", &val, 0, 360, "", ImGuiSliderFlags_AlwaysClamp))
                        {
                            CVar_SetS32(cvar, val);
                            needs_save = true;
                        }
                        ImGui::PopItemWidth();
                        Tooltip("When Interpolation FPS setting is at least this threshold, add one frame of input lag (e.g. 16.6 ms for 60 FPS) in order to avoid jitter. This setting allows the CPU to work on one frame while GPU works on the previous frame.\nThis setting should be used when your computer is too slow to do CPU + GPU work in time.");

                        ImGui::SameLine();
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                        if (ImGui::Button(PlusBTNELT.c_str())) {
                            val++;
                            CVar_SetS32(cvar, val);
                            needs_save = true;
                        }

                        InsertPadding();
                    }

                    ImGui::Text("Renderer API (Needs reload)");
                    if (ImGui::BeginCombo("##RApi", backends[lastBackendID].second)) {
                        for (uint8_t i = 0; i < sizeof(backends) / sizeof(backends[0]); i++) {
                            if (ImGui::Selectable(backends[i].second, i == lastBackendID)) {
                                pConf->setString("Window.GfxBackend", backends[i].first);
                                lastBackendID = i;
                            }
                        }
                        ImGui::EndCombo();
                    }

                    EXPERIMENTAL();

                    ImGui::Text("Texture Filter (Needs reload)");
                    EnhancementCombobox("gTextureFilter", filters, 3, 0);

                    InsertPadding();

                    overlay->DrawSettings();

                    ImGui::EndMenu();
                }

                InsertPadding();

                if (ImGui::BeginMenu("Languages")) {
                    EnhancementRadioButton("English", "gLanguages", 0);
                    EnhancementRadioButton("German", "gLanguages", 1);
                    EnhancementRadioButton("French", "gLanguages", 2);
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            ImGui::SetCursorPosY(0.0f);

            if (ImGui::BeginMenu("Enhancements"))
            {

                const char* enhancementPresets[4] = { "Default", "Vanilla Plus", "Enhanced", "Randomizer"};
                PaddedText("Enhancement Presets", false, true);
                SohImGui::EnhancementCombobox("gSelectEnhancementPresets", enhancementPresets, 4, 0);
                Tooltip(
                    "Default - Set all enhancements to their default values. The true vanilla SoH experience.\n"
                    "\n"
                    "Vanilla Plus - Adds Quality of Life features that enhance your experience, but don't alter gameplay. Recommended for a first playthrough of OoT.\n"
                    "\n"
                    "Enhanced - The \"Vanilla Plus\" preset, but with more quality of life enhancements that might alter gameplay slightly. Recommended for returning players.\n"
                    "\n"
                    "Randomizer - The \"Enhanced\" preset, plus any other enhancements that are recommended for playing Randomizer."
                );

                InsertPadding();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));
                if (ImGui::Button("Apply Preset")) {
                    applyEnhancementPresets();
                    needs_save = true;
                }
                ImGui::PopStyleVar(1);

                PaddedSeparator();

                if (ImGui::BeginMenu("Controls")) {
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                    float availableWidth = ImGui::GetContentRegionAvail().x;
                    if (ImGui::Button(
                        GetWindowButtonText("Customize Game Controls", CVar_GetS32("gGameControlEditorEnabled", 0)).c_str(),
                        ImVec2(availableWidth, 0)
                    )) {
                        bool currentValue = CVar_GetS32("gGameControlEditorEnabled", 0);
                        CVar_SetS32("gGameControlEditorEnabled", !currentValue);
                        needs_save = true;
                        customWindows["Game Control Editor"].enabled = CVar_GetS32("gGameControlEditorEnabled", 0);
                    }
                    ImGui::PopStyleVar(3);
                    ImGui::PopStyleColor(1);

                    // TODO mutual exclusions -- There should be some system to prevent conclifting enhancements from being selected
                    PaddedEnhancementCheckbox("D-pad Support on Pause and File Select", "gDpadPauseName");
                    Tooltip("Enables Pause and File Select screen navigation with the D-pad\nIf used with D-pad as Equip Items, you must hold C-Up to equip instead of navigate");
                    PaddedEnhancementCheckbox("D-pad Support in Text Choice", "gDpadText", true, false);
                    PaddedEnhancementCheckbox("D-pad Support for Browsing Shop Items", "gDpadShop", true, false);
                    PaddedEnhancementCheckbox("D-pad as Equip Items", "gDpadEquips", true, false);
                    Tooltip("Allows the D-pad to be used as extra C buttons");
                    PaddedEnhancementCheckbox("Allow the cursor to be on any slot", "gPauseAnyCursor", true, false);
                    Tooltip("Allows the cursor on the pause menu to be over any slot\nSimilar to Rando and Spaceworld 97");
                    PaddedEnhancementCheckbox("Prevent Dropped Ocarina Inputs", "gDpadNoDropOcarinaInput", true, false);
                    Tooltip("Prevent dropping inputs when playing the ocarina quickly");
                    PaddedEnhancementCheckbox("Answer Navi Prompt with L Button", "gNaviOnL", true, false);
                    Tooltip("Speak to Navi with L but enter first-person camera with C-Up");
                    ImGui::EndMenu();
                }

                InsertPadding();

                if (ImGui::BeginMenu("Gameplay"))
                {
                    if (ImGui::BeginMenu("Time Savers"))
                    {
                        PaddedEnhancementSliderInt("Text Speed: %dx", "##TEXTSPEED", "gTextSpeed", 1, 5, "", 1, false, false, true);
                        PaddedEnhancementSliderInt("King Zora Speed: %dx", "##MWEEPSPEED", "gMweepSpeed", 1, 5, "", 1, false, false, true);
                        EnhancementSliderInt("Biggoron Forge Time: %d days", "##FORGETIME", "gForgeTime", 0, 3, "", 3);
                        Tooltip("Allows you to change the number of days it takes for Biggoron to forge the Biggoron Sword");
                        PaddedEnhancementSliderInt("Vine/Ladder Climb speed +%d", "##CLIMBSPEED", "gClimbSpeed", 0, 12, "", 0, false, false, true);
                        PaddedEnhancementSliderInt("Block pushing speed +%d", "##BLOCKSPEED", "gFasterBlockPush", 0, 5, "", 0, false, false, true);
                        PaddedEnhancementCheckbox("Faster Heavy Block Lift", "gFasterHeavyBlockLift", true, false);
                        Tooltip("Speeds up lifting silver rocks and obelisks");
                        PaddedEnhancementCheckbox("No Forced Navi", "gNoForcedNavi", true, false);
                        Tooltip("Prevent forced Navi conversations");
                        PaddedEnhancementCheckbox("No Skulltula Freeze", "gSkulltulaFreeze", true, false);
                        Tooltip("Stops the game from freezing the player when picking up Gold Skulltulas");
                        PaddedEnhancementCheckbox("MM Bunny Hood", "gMMBunnyHood", true, false);
                        Tooltip("Wearing the Bunny Hood grants a speed increase like in Majora's Mask");
                        PaddedEnhancementCheckbox("Fast Chests", "gFastChests", true, false);
                        Tooltip("Kick open every chest");
                        PaddedEnhancementCheckbox("Skip Pickup Messages", "gFastDrops", true, false);
                        Tooltip("Skip pickup messages for new consumable items and bottle swipes");
                        PaddedEnhancementCheckbox("Better Owl", "gBetterOwl", true, false);
                        Tooltip("The default response to Kaepora Gaebora is always that you understood what he said");
                        PaddedEnhancementCheckbox("Fast Ocarina Playback", "gFastOcarinaPlayback", true, false);
                        Tooltip("Skip the part where the Ocarina playback is called when you play a song");
                        PaddedEnhancementCheckbox("Instant Putaway", "gInstantPutaway", true, false);
                        Tooltip("Allow Link to put items away without having to wait around");
                        PaddedEnhancementCheckbox("Instant Boomerang Recall", "gFastBoomerang", true, false);
                        Tooltip("Instantly return the boomerang to Link by pressing its item button while it's in the air");
                        PaddedEnhancementCheckbox("Mask Select in Inventory", "gMaskSelect", true, false);
                        Tooltip("After completing the mask trading sub-quest, press A and any direction on the mask slot to change masks");
                        PaddedEnhancementCheckbox("Remember Save Location", "gRememberSaveLocation", true, false);
                        Tooltip("When loading a save, places Link at the last entrance he went through.\n"
                                "This doesn't work if the save was made in a grotto.");
                        ImGui::EndMenu();
                    }

                    InsertPadding();

                    if (ImGui::BeginMenu("Difficulty Options"))
                    {
                        ImGui::Text("Damage Multiplier");
                        EnhancementCombobox("gDamageMul", powers, 9, 0);
                        Tooltip(
                            "Modifies all sources of damage not affected by other sliders\n\
                            2x: Can survive all common attacks from the start of the game\n\
                            4x: Dies in 1 hit to any substantial attack from the start of the game\n\
                            8x: Can only survive trivial damage from the start of the game\n\
                            16x: Can survive all common attacks with max health without double defense\n\
                            32x: Can survive all common attacks with max health and double defense\n\
                            64x: Can survive trivial damage with max health without double defense\n\
                            128x: Can survive trivial damage with max health and double defense\n\
                            256x: Cannot survive damage"
                        );
                        PaddedText("Fall Damage Multiplier", true, false);
                        EnhancementCombobox("gFallDamageMul", powers, 8, 0);
                        Tooltip(
                            "Modifies all fall damage\n\
                            2x: Can survive all fall damage from the start of the game\n\
                            4x: Can only survive short fall damage from the start of the game\n\
                            8x: Cannot survive any fall damage from the start of the game\n\
                            16x: Can survive all fall damage with max health without double defense\n\
                            32x: Can survive all fall damage with max health and double defense\n\
                            64x: Can survive short fall damage with double defense\n\
                            128x: Cannot survive fall damage"
                        );
                        PaddedText("Void Damage Multiplier", true, false);
                        EnhancementCombobox("gVoidDamageMul", powers, 7, 0);
                        Tooltip(
                            "Modifies damage taken after falling into a void\n\
                            2x: Can survive void damage from the start of the game\n\
                            4x: Cannot survive void damage from the start of the game\n\
                            8x: Can survive void damage twice with max health without double defense\n\
                            16x: Can survive void damage with max health without double defense\n\
                            32x: Can survive void damage with max health and double defense\n\
                            64x: Cannot survive void damage"
                        );
                        PaddedEnhancementCheckbox("No Random Drops", "gNoRandomDrops", true, false);
                        Tooltip("Disables random drops, except from the Goron Pot, Dampe, and bosses");
                        PaddedEnhancementCheckbox("No Heart Drops", "gNoHeartDrops", true, false);
                        Tooltip("Disables heart drops, but not heart placements, like from a Deku Scrub running off\nThis simulates Hero Mode from other games in the series");
                        PaddedEnhancementCheckbox("Always Win Goron Pot", "gGoronPot", true, false);
                        Tooltip("Always get the heart piece/purple rupee from the spinning Goron pot");
                        InsertPadding();

                        if (ImGui::BeginMenu("Potion Values"))
                        {
                            EnhancementCheckbox("Change Red Potion Effect", "gRedPotionEffect");
                            Tooltip("Enable the following changes to the amount of health restored by Red Potions");
                            EnhancementSliderInt("Red Potion Health: %d", "##REDPOTIONHEALTH", "gRedPotionHealth", 1, 100, "", 0, true);
                            Tooltip("Changes the amount of health restored by Red Potions");
                            EnhancementCheckbox("Red Potion Percent Restore", "gRedPercentRestore");
                            Tooltip("Toggles from Red Potions restoring a fixed amount of health to a percent of the player's current max health");

                            ImGui::Separator();

                            EnhancementCheckbox("Change Green Potion Effect", "gGreenPotionEffect");
                            Tooltip("Enable the following changes to the amount of mana restored by Green Potions");
                            EnhancementSliderInt("Green Potion Mana: %d", "##GREENPOTIONMANA", "gGreenPotionMana", 1, 100, "", 0, true);
                            Tooltip("Changes the amount of mana restored by Green Potions, base max mana is 48, max upgraded mana is 96");
                            EnhancementCheckbox("Green Potion Percent Restore", "gGreenPercentRestore");
                            Tooltip("Toggles from Green Potions restoring a fixed amount of mana to a percent of the player's current max mana");

                            ImGui::Separator();

                            EnhancementCheckbox("Change Blue Potion Effects", "gBluePotionEffects");
                            Tooltip("Enable the following changes to the amount of health and mana restored by Blue Potions");
                            EnhancementSliderInt("Blue Potion Health: %d", "##BLUEPOTIONHEALTH", "gBluePotionHealth", 1, 100, "", 0, true);
                            Tooltip("Changes the amount of health restored by Blue Potions");
                            EnhancementCheckbox("Blue Potion Health Percent Restore", "gBlueHealthPercentRestore");
                            Tooltip("Toggles from Blue Potions restoring a fixed amount of health to a percent of the player's current max health");

                            ImGui::Separator();

                            EnhancementSliderInt("Blue Potion Mana: %d", "##BLUEPOTIONMANA", "gBluePotionMana", 1, 100, "", 0, true);
                            Tooltip("Changes the amount of mana restored by Blue Potions, base max mana is 48, max upgraded mana is 96");
                            EnhancementCheckbox("Blue Potion Mana Percent Restore", "gBlueManaPercentRestore");
                            Tooltip("Toggles from Blue Potions restoring a fixed amount of mana to a percent of the player's current max mana");

                            ImGui::Separator();

                            EnhancementCheckbox("Change Milk Effect", "gMilkEffect");
                            Tooltip("Enable the following changes to the amount of health restored by Milk");
                            EnhancementSliderInt("Milk Health: %d", "##MILKHEALTH", "gMilkHealth", 1, 100, "", 0, true);
                            Tooltip("Changes the amount of health restored by Milk");
                            EnhancementCheckbox("Milk Percent Restore", "gMilkPercentRestore");
                            Tooltip("Toggles from Milk restoring a fixed amount of health to a percent of the player's current max health");

                            ImGui::Separator();

                            EnhancementCheckbox("Separate Half Milk Effect", "gSeparateHalfMilkEffect");
                            Tooltip("Enable the following changes to the amount of health restored by Half Milk\nIf this is disabled, Half Milk will behave the same as Full Milk.");
                            EnhancementSliderInt("Half Milk Health: %d", "##HALFMILKHEALTH", "gHalfMilkHealth", 1, 100, "", 0, true);
                            Tooltip("Changes the amount of health restored by Half Milk");
                            EnhancementCheckbox("Half Milk Percent Restore", "gHalfMilkPercentRestore");
                            Tooltip("Toggles from Half Milk restoring a fixed amount of health to a percent of the player's current max health");
                            
                            ImGui::Separator();

                            EnhancementCheckbox("Change Fairy Effect", "gFairyEffect");
                            Tooltip("Enable the following changes to the amount of health restored by Fairies");
                            EnhancementSliderInt("Fairy: %d", "##FAIRYHEALTH", "gFairyHealth", 1, 100, "", 0, true);
                            Tooltip("Changes the amount of health restored by Fairies");
                            EnhancementCheckbox("Fairy Percent Restore", "gFairyPercentRestore");
                            Tooltip("Toggles from Fairies restoring a fixed amount of health to a percent of the player's current max health");
                            
                            ImGui::Separator();

                            EnhancementCheckbox("Change Fairy Revive Effect", "gFairyReviveEffect");
                            Tooltip("Enable the following changes to the amount of health restored by Fairy Revivals");
                            EnhancementSliderInt("Fairy Revival: %d", "##FAIRYREVIVEHEALTH", "gFairyReviveHealth", 1, 100, "", 0, true);
                            Tooltip("Changes the amount of health restored by Fairy Revivals");
                            EnhancementCheckbox("Fairy Revive Percent Restore", "gFairyRevivePercentRestore");
                            Tooltip("Toggles from Fairy Revivals restoring a fixed amount of health to a percent of the player's current max health");

                            ImGui::EndMenu();
                        }

                        InsertPadding();

                        if (ImGui::BeginMenu("Fishing")) {
                            EnhancementCheckbox("Instant Fishing", "gInstantFishing");
                            Tooltip("All fish will be caught instantly");
                            PaddedEnhancementCheckbox("Guarantee Bite", "gGuaranteeFishingBite", true, false);
                            Tooltip("When a line is stable, guarantee bite. Otherwise use default logic");
                            PaddedEnhancementSliderInt("Child Minimum Weight: %d", "##cMinimumWeight", "gChildMinimumWeightFish", 6, 10, "", 10, false, true, false);
                            Tooltip("The minimum weight for the unique fishing reward as a child");
                            PaddedEnhancementSliderInt("Adult Minimum Weight: %d", "##aMinimumWeight", "gAdultMinimumWeightFish", 8, 13, "", 13, false, true, false);
                            Tooltip("The minimum weight for the unique fishing reward as an adult");
                            ImGui::EndMenu();
                        }

                        ImGui::EndMenu();
                    }

                    InsertPadding();

                    if (ImGui::BeginMenu("Reduced Clutter"))
                    {
                        EnhancementCheckbox("Mute Low HP Alarm", "gLowHpAlarm");
                        Tooltip("Disable the low HP beeping sound");
                        PaddedEnhancementCheckbox("Minimal UI", "gMinimalUI", true, false);
                        Tooltip("Hides most of the UI when not needed\nNote: Doesn't activate until after loading a new scene");
                        PaddedEnhancementCheckbox("Disable Navi Call Audio", "gDisableNaviCallAudio", true, false);
                        Tooltip("Disables the voice audio when Navi calls you");

                        ImGui::EndMenu();
                    }

                    InsertPadding();

                    EnhancementCheckbox("Visual Stone of Agony", "gVisualAgony");
                    Tooltip("Displays an icon and plays a sound when Stone of Agony should be activated, for those without rumble");
                    PaddedEnhancementCheckbox("Assignable Tunics and Boots", "gAssignableTunicsAndBoots", true, false);
                    Tooltip("Allows equipping the tunic and boots to c-buttons");
                    PaddedEnhancementCheckbox("Equipment Toggle", "gEquipmentCanBeRemoved", true, false);
                    Tooltip("Allows equipment to be removed by toggling it off on\nthe equipment subscreen.");
                    PaddedEnhancementCheckbox("Link's Cow in Both Time Periods", "gCowOfTime", true, false);
                    Tooltip("Allows the Lon Lon Ranch obstacle course reward to be shared across time periods");
                    PaddedEnhancementCheckbox("Enable visible guard vision", "gGuardVision", true, false);
                    PaddedEnhancementCheckbox("Enable passage of time on file select", "gTimeFlowFileSelect", true, false);
                    PaddedEnhancementCheckbox("Count Golden Skulltulas", "gInjectSkulltulaCount", true, false);
                    Tooltip("Injects Golden Skulltula total count in pickup messages");
                    PaddedEnhancementCheckbox("Pull grave during the day", "gDayGravePull", true, false);
                    Tooltip("Allows graves to be pulled when child during the day");
                    ImGui::EndMenu();
                }

                InsertPadding();

                if (ImGui::BeginMenu("Graphics"))
                {
                    if (ImGui::BeginMenu("Animated Link in Pause Menu")) {
                        ImGui::Text("Rotation");
                        EnhancementRadioButton("Disabled", "gPauseLiveLinkRotation", 0);
                        EnhancementRadioButton("Rotate Link with D-pad", "gPauseLiveLinkRotation", 1);
                        Tooltip("Allow you to rotate Link on the Equipment menu with the D-pad\nUse D-pad Up or D-pad Down to reset Link's rotation");
                        EnhancementRadioButton("Rotate Link with C-buttons", "gPauseLiveLinkRotation", 2);
                        Tooltip("Allow you to rotate Link on the Equipment menu with the C-buttons\nUse C-Up or C-Down to reset Link's rotation");
                        EnhancementRadioButton("Rotate Link with Right Stick", "gPauseLiveLinkRotation", 3);
                        Tooltip("Allow you to rotate Link on the Equipment menu with the Right Stick\nYou can zoom in by pointing up and reset Link's rotation by pointing down");
                        if (CVar_GetS32("gPauseLiveLinkRotation", 0) != 0) {
                            EnhancementSliderInt("Rotation Speed: %d", "##MinRotationSpeed", "gPauseLiveLinkRotationSpeed", 1, 20, "");
                        }
                        PaddedSeparator();
                        ImGui::Text("Static loop");
                        EnhancementRadioButton("Disabled", "gPauseLiveLink", 0);
                        EnhancementRadioButton("Idle (standing)", "gPauseLiveLink", 1);
                        EnhancementRadioButton("Idle (look around)", "gPauseLiveLink", 2);
                        EnhancementRadioButton("Idle (belt)", "gPauseLiveLink", 3);
                        EnhancementRadioButton("Idle (shield)", "gPauseLiveLink", 4);
                        EnhancementRadioButton("Idle (test sword)", "gPauseLiveLink", 5);
                        EnhancementRadioButton("Idle (yawn)", "gPauseLiveLink", 6);
                        EnhancementRadioButton("Battle Stance", "gPauseLiveLink", 7);
                        EnhancementRadioButton("Walking (no shield)", "gPauseLiveLink", 8);
                        EnhancementRadioButton("Walking (holding shield)", "gPauseLiveLink", 9);
                        EnhancementRadioButton("Running (no shield)", "gPauseLiveLink", 10);
                        EnhancementRadioButton("Running (holding shield)", "gPauseLiveLink", 11);
                        EnhancementRadioButton("Hand on hip", "gPauseLiveLink", 12);
                        EnhancementRadioButton("Spin attack charge", "gPauseLiveLink", 13);
                        EnhancementRadioButton("Look at hand", "gPauseLiveLink", 14);
                        PaddedSeparator();
                        ImGui::Text("Randomize");
                        EnhancementRadioButton("Random", "gPauseLiveLink", 15);
                        Tooltip("Randomize the animation played each time you open the menu");
                        EnhancementRadioButton("Random cycle", "gPauseLiveLink", 16);
                        Tooltip("Randomize the animation played on the menu after a certain time");
                        EnhancementRadioButton("Random cycle (Idle)", "gPauseLiveLink", 17);
                        Tooltip("Randomize the animation played on the menu after a certain time (Idle animations only)");
                        if (CVar_GetS32("gPauseLiveLink", 0) >= 16) {
                            EnhancementSliderInt("Frame to wait: %d", "##MinFrameCount", "gMinFrameCount", 1, 1000, "", 0, true);
                        }

                        ImGui::EndMenu();
                    }
                    PaddedEnhancementCheckbox("N64 Mode", "gN64Mode", true, false);
                    Tooltip("Sets aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution");
                    PaddedEnhancementCheckbox("Enable 3D Dropped items/projectiles", "gNewDrops", true, false);
                    Tooltip("Change most 2D items and projectiles on the overworld to their 3D versions");
                    PaddedEnhancementCheckbox("Disable Black Bar Letterboxes", "gDisableBlackBars", true, false);
                    Tooltip("Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be minor visual glitches that were covered up by the black bars\nPlease disable this setting before reporting a bug");
                    PaddedEnhancementCheckbox("Dynamic Wallet Icon", "gDynamicWalletIcon", true, false);
                    Tooltip("Changes the rupee in the wallet icon to match the wallet size you currently have");
                    PaddedEnhancementCheckbox("Always show dungeon entrances", "gAlwaysShowDungeonMinimapIcon", true, false);
                    Tooltip("Always shows dungeon entrance icons on the minimap");

                    ImGui::EndMenu();
                }

                InsertPadding();

                if (ImGui::BeginMenu("Fixes"))
                {
                    EnhancementCheckbox("Fix L&R Pause menu", "gUniformLR");
                    Tooltip("Makes the L and R buttons in the pause menu the same color");
                    PaddedEnhancementCheckbox("Fix L&Z Page switch in Pause menu", "gNGCKaleidoSwitcher", true, false);
                    Tooltip("Makes L and R switch pages like on the GameCube\nZ opens the Debug Menu instead");
                    PaddedEnhancementCheckbox("Fix Dungeon entrances", "gFixDungeonMinimapIcon", true, false);
                    Tooltip("Removes the dungeon entrance icon on the top-left corner of the screen when no dungeon is present on the current map");
                    PaddedEnhancementCheckbox("Fix Two Handed idle animations", "gTwoHandedIdle", true, false);
                    Tooltip("Re-enables the two-handed idle animation, a seemingly finished animation that was disabled on accident in the original game");
                    PaddedEnhancementCheckbox("Fix the Gravedigging Tour Glitch", "gGravediggingTourFix", true, false);
                    Tooltip("Fixes a bug where the Gravedigging Tour Heart Piece disappears if the area reloads");
                    PaddedEnhancementCheckbox("Fix Deku Nut upgrade", "gDekuNutUpgradeFix", true, false);
                    Tooltip("Prevents the Forest Stage Deku Nut upgrade from becoming unobtainable after receiving the Poacher's Saw");
                    PaddedEnhancementCheckbox("Fix Navi text HUD position", "gNaviTextFix", true, false);
                    Tooltip("Correctly centers the Navi text prompt on the HUD's C-Up button");
                    PaddedEnhancementCheckbox("Fix Anubis fireballs", "gAnubisFix", true, false);
                    Tooltip("Make Anubis fireballs do fire damage when reflected back at them with the Mirror Shield");
                    PaddedEnhancementCheckbox("Fix Megaton Hammer crouch stab", "gCrouchStabHammerFix", true, false);
                    Tooltip("Make the Megaton Hammer's crouch stab able to destroy rocks without first swinging it normally");
                    if (CVar_GetS32("gCrouchStabHammerFix", 0) == 0) {
                        CVar_SetS32("gCrouchStabFix", 0);
                    } else {
                        PaddedEnhancementCheckbox("Remove power crouch stab", "gCrouchStabFix", true, false);
                        Tooltip("Make crouch stabbing always do the same damage as a regular slash");
                    }

                    ImGui::EndMenu();
                }

                InsertPadding();

                if (ImGui::BeginMenu("Restoration"))
                {
                    EnhancementCheckbox("Red Ganon blood", "gRedGanonBlood");
                    Tooltip("Restore the original red blood from NTSC 1.0/1.1. Disable for green blood");
                    PaddedEnhancementCheckbox("Fish while hovering", "gHoverFishing", true, false);
                    Tooltip("Restore a bug from NTSC 1.0 that allows casting the Fishing Rod while using the Hover Boots");
                    PaddedEnhancementCheckbox("N64 Weird Frames", "gN64WeirdFrames", true, false);
                    Tooltip("Restores N64 Weird Frames allowing weirdshots to behave the same as N64");
                    PaddedEnhancementCheckbox("Bombchus out of bounds", "gBombchusOOB", true, false);
                    Tooltip("Allows bombchus to explode out of bounds\nSimilar to GameCube and Wii VC");
                    PaddedEnhancementCheckbox("Restore old Gold Skulltula cutscene", "gGsCutscene", true, false);

                    ImGui::EndMenu();
                }

                PaddedEnhancementCheckbox("Autosave", "gAutosave", true, false);
                Tooltip("Automatically save the game every time a new area is entered or item is obtained\n"
                    "To disable saving when obtaining an item, manually set gAutosaveAllItems and gAutosaveMajorItems to 0\n"
                    "gAutosaveAllItems takes priority over gAutosaveMajorItems if both are set to 1\n"
                    "gAutosaveMajorItems excludes rupees and health/magic/ammo refills (but includes bombchus)");

                InsertPadding();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                static ImVec2 buttonSize(200.0f, 0.0f);
                if (ImGui::Button(GetWindowButtonText("Cosmetics Editor", CVar_GetS32("gCosmeticsEditorEnabled", 0)).c_str(), buttonSize))
                {
                    bool currentValue = CVar_GetS32("gCosmeticsEditorEnabled", 0);
                    CVar_SetS32("gCosmeticsEditorEnabled", !currentValue);
                    needs_save = true;
                    customWindows["Cosmetics Editor"].enabled = CVar_GetS32("gCosmeticsEditorEnabled", 0);
                }
                ImGui::PopStyleVar(3);
                ImGui::PopStyleColor(1);

                EXPERIMENTAL();

                const char* fps_cvar = "gInterpolationFPS";
                {
                #if defined(__SWITCH__) || defined(__WIIU__)
                    int minFps = 20;
                    int maxFps = 60;
                #else
                    int minFps = 20;
                    int maxFps = 360;
                #endif

                    int val = CVar_GetS32(fps_cvar, minFps);
                    val = MAX(MIN(val, maxFps), 20);

                #ifdef __WIIU__
                    // only support divisors of 60 on the Wii U
                    val = 60 / (60 / val);
                #endif

                    int fps = val;

                    if (fps == 20)
                    {
                        ImGui::Text("Frame interpolation: Off");
                    }
                    else
                    {
                        ImGui::Text("Frame interpolation: %d FPS", fps);
                    }

                    std::string MinusBTNFPSI = " - ##FPSInterpolation";
                    std::string PlusBTNFPSI = " + ##FPSInterpolation";
                    if (ImGui::Button(MinusBTNFPSI.c_str())) {
                    #ifdef __WIIU__
                        if (val >= 60) val = 30;
                        else val = 20;
                    #else
                        val--;
                    #endif
                        CVar_SetS32(fps_cvar, val);
                        needs_save = true;
                    }
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                #ifdef __WIIU__
                    ImGui::PushItemWidth(ImGui::GetWindowSize().x - 79.0f * 2);
                #else
                    ImGui::PushItemWidth(ImGui::GetWindowSize().x - 79.0f);
                #endif
                    if (ImGui::SliderInt("##FPSInterpolation", &val, minFps, maxFps, "", ImGuiSliderFlags_AlwaysClamp))
                    {
                    #ifdef __WIIU__
                        // only support divisors of 60 on the Wii U
                        val = 60 / (60 / val);
                    #endif
                        if (val > 360)
                        {
                            val = 360;
                        }
                        else if (val < 20)
                        {
                            val = 20;
                        }

                        CVar_SetS32(fps_cvar, val);
                        needs_save = true;
                    }
                    ImGui::PopItemWidth();
                    Tooltip("Interpolate extra frames to get smoother graphics\n"
                        "Set to match your monitor's refresh rate, or a divisor of it\n"
                        "A higher target FPS than your monitor's refresh rate will just waste resources, "
                        "and might give a worse result.\n"
                        "For consistent input lag, set this value and your monitor's refresh rate to a multiple of 20\n"
                        "Ctrl+Click for keyboard input");

                    ImGui::SameLine();
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                    if (ImGui::Button(PlusBTNFPSI.c_str())) {
                    #ifdef __WIIU__
                        if (val <= 20) val = 30;
                        else val = 60;
                    #else
                        val++;
                    #endif
                        CVar_SetS32(fps_cvar, val);
                        needs_save = true;
                    }
                }

                if (impl.backend == Backend::DX11)
                {
                    InsertPadding();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));
                    if (ImGui::Button("Match Refresh Rate"))
                    {
                        int hz = roundf(gfx_get_detected_hz());
                        if (hz >= 20 && hz <= 360)
                        {
                            CVar_SetS32(fps_cvar, hz);
                            needs_save = true;
                        }
                    }
                    ImGui::PopStyleVar(1);
                    InsertPadding();
                }
                EnhancementCheckbox("Disable LOD", "gDisableLOD");
                Tooltip("Turns off the Level of Detail setting, making models use their higher-poly variants at any distance");
                PaddedEnhancementCheckbox("Disable Draw Distance", "gDisableDrawDistance", true, false);
                Tooltip("Turns off the objects draw distance, making objects being visible from a longer range");
                if (CVar_GetS32("gDisableDrawDistance", 0) == 0) {
                    CVar_SetS32("gDisableKokiriDrawDistance", 0);
                } else if (CVar_GetS32("gDisableDrawDistance", 0) == 1) {
                    PaddedEnhancementCheckbox("Kokiri Draw Distance", "gDisableKokiriDrawDistance", true, false);
                    Tooltip("The Kokiri are mystical beings that fade into view when approached\nEnabling this will remove their draw distance");
                }
                PaddedEnhancementCheckbox("Skip Text", "gSkipText", true, false);
                Tooltip("Holding down B skips text\nKnown to cause a cutscene softlock in Water Temple\nSoftlock can be fixed by pressing D-Right in Debug mode");
                PaddedEnhancementCheckbox("Free Camera", "gFreeCamera", true, false);
                Tooltip("Enables camera control\nNote: You must remap C buttons off of the right stick in the controller config menu, and map the camera stick to the right stick.");

             #ifdef __SWITCH__
                InsertPadding();
                int slot = CVar_GetS32("gSwitchPerfMode", (int)SwitchProfiles::STOCK);
                ImGui::Text("Switch performance mode");
                if (ImGui::BeginCombo("##perf", SWITCH_CPU_PROFILES[slot])) {
                    for (int sId = 0; sId <= SwitchProfiles::POWERSAVINGM3; sId++) {
                        if (ImGui::Selectable(SWITCH_CPU_PROFILES[sId], sId == slot)) {
                            SPDLOG_INFO("Profile:: %s", SWITCH_CPU_PROFILES[sId]);
                            CVar_SetS32("gSwitchPerfMode", sId);
                            Switch::ApplyOverclock();
                            needs_save = true;
                        }

                    }
                    ImGui::EndCombo();
                }
             #endif

                ImGui::EndMenu();
            }

            ImGui::SetCursorPosY(0.0f);

            if (ImGui::BeginMenu("Cheats"))
            {
                if (ImGui::BeginMenu("Infinite...")) {
                    EnhancementCheckbox("Money", "gInfiniteMoney");
                    PaddedEnhancementCheckbox("Health", "gInfiniteHealth", true, false);
                    PaddedEnhancementCheckbox("Ammo", "gInfiniteAmmo", true, false);
                    PaddedEnhancementCheckbox("Magic", "gInfiniteMagic", true, false);
                    PaddedEnhancementCheckbox("Nayru's Love", "gInfiniteNayru", true, false);
                    PaddedEnhancementCheckbox("Epona Boost", "gInfiniteEpona", true, false);

                    ImGui::EndMenu();
                }

                PaddedEnhancementCheckbox("No Clip", "gNoClip", true, false);
                Tooltip("Allows you to walk through walls");
                PaddedEnhancementCheckbox("Climb Everything", "gClimbEverything", true, false);
                Tooltip("Makes every surface in the game climbable");
                PaddedEnhancementCheckbox("Moon Jump on L", "gMoonJumpOnL", true, false);
                Tooltip("Holding L makes you float into the air");
                PaddedEnhancementCheckbox("Super Tunic", "gSuperTunic", true, false);
                Tooltip("Makes every tunic have the effects of every other tunic");
                PaddedEnhancementCheckbox("Easy ISG", "gEzISG", true, false);
                Tooltip("Passive Infinite Sword Glitch\nIt makes your sword's swing effect and hitbox stay active indefinitely");
                PaddedEnhancementCheckbox("Unrestricted Items", "gNoRestrictItems", true, false);
                Tooltip("Allows you to use any item at any location");
                PaddedEnhancementCheckbox("Freeze Time", "gFreezeTime", true, false);
                Tooltip("Freezes the time of day");
                PaddedEnhancementCheckbox("Drops Don't Despawn", "gDropsDontDie", true, false);
                Tooltip("Drops from enemies, grass, etc. don't disappear after a set amount of time");
                PaddedEnhancementCheckbox("Fireproof Deku Shield", "gFireproofDekuShield", true, false);
                Tooltip("Prevents the Deku Shield from burning on contact with fire");
                PaddedEnhancementCheckbox("Shield with Two-Handed Weapons", "gShieldTwoHanded", true, false);
                Tooltip("This allows you to put up your shield with any two-handed weapon in hand except for Deku Sticks");
                PaddedEnhancementCheckbox("Time Sync", "gTimeSync", true, false);
                Tooltip("This syncs the ingame time with the real world time");

                {
                    static int32_t betaQuestEnabled = CVar_GetS32("gEnableBetaQuest", 0);
                    static int32_t lastBetaQuestEnabled = betaQuestEnabled;
                    static int32_t betaQuestWorld = CVar_GetS32("gBetaQuestWorld", 0xFFEF);
                    static int32_t lastBetaQuestWorld = betaQuestWorld;

                    if (!isBetaQuestEnabled) {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                    }

                    PaddedEnhancementCheckbox("Enable Beta Quest", "gEnableBetaQuest", true, false);
                    Tooltip("Turns on OoT Beta Quest. *WARNING* This will reset your game.");
                    betaQuestEnabled = CVar_GetS32("gEnableBetaQuest", 0);
                    if (betaQuestEnabled) {
                        if (betaQuestEnabled != lastBetaQuestEnabled) {
                            betaQuestWorld = 0;
                        }

                        ImGui::Text("Beta Quest World: %d", betaQuestWorld);

                        if (ImGui::Button(" - ##BetaQuest")) {
                            betaQuestWorld--;
                        }
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);

                        ImGui::SliderInt("##BetaQuest", &betaQuestWorld, 0, 8, "", ImGuiSliderFlags_AlwaysClamp);
                        Tooltip("Set the Beta Quest world to explore. *WARNING* Changing this will reset your game.\nCtrl+Click to type in a value.");

                        ImGui::SameLine();
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                        if (ImGui::Button(" + ##BetaQuest")) {
                            betaQuestWorld++;
                        }

                        if (betaQuestWorld > 8) {
                            betaQuestWorld = 8;
                        }
                        else if (betaQuestWorld < 0) {
                            betaQuestWorld = 0;
                        }
                    }
                    else {
                        lastBetaQuestWorld = betaQuestWorld = 0xFFEF;
                        CVar_SetS32("gBetaQuestWorld", betaQuestWorld);
                    }
                    if (betaQuestEnabled != lastBetaQuestEnabled || betaQuestWorld != lastBetaQuestWorld)
                    {
                        // Reset the game if the beta quest state or world changed because beta quest happens on redirecting the title screen cutscene.
                        lastBetaQuestEnabled = betaQuestEnabled;
                        lastBetaQuestWorld = betaQuestWorld;
                        CVar_SetS32("gEnableBetaQuest", betaQuestEnabled);
                        CVar_SetS32("gBetaQuestWorld", betaQuestWorld);

                        console->Dispatch("reset");

                        needs_save = true;
                    }

                    if (!isBetaQuestEnabled) {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar(1);
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::SetCursorPosY(0.0f);

            if (ImGui::BeginMenu("Developer Tools"))
            {
                EnhancementCheckbox("OoT Debug Mode", "gDebugEnabled");
                Tooltip("Enables Debug Mode, allowing you to select maps with L + R + Z, noclip with L + D-pad Right, and open the debug menu with L on the pause screen");
                PaddedEnhancementCheckbox("OoT Skulltula Debug", "gSkulltulaDebugEnabled", true, false);
                Tooltip("Enables Skulltula Debug, when moving the cursor in the menu above various map icons (boss key, compass, map screen locations, etc) will set the GS bits in that area.\nUSE WITH CAUTION AS IT DOES NOT UPDATE THE GS COUNT.");
                PaddedEnhancementCheckbox("Fast File Select", "gSkipLogoTitle", true, false);
                Tooltip("Load the game to the selected menu or file\n\"Zelda Map Select\" require debug mode else you will fallback to File choose menu\nUsing a file number that don't have save will create a save file only if you toggle on \"Create a new save if none ?\" else it will bring you to the File choose menu");
                if (CVar_GetS32("gSkipLogoTitle", 0)) {
                    const char* FastFileSelect[5] = {
                        "File N.1",
                        "File N.2",
                        "File N.3",
                        "File select",
                        "Zelda Map Select (require OoT Debug Mode)"
                    };
                    ImGui::Text("Loading :");
                    EnhancementCombobox("gSaveFileID", FastFileSelect, 5, 0);
                    PaddedEnhancementCheckbox("Create a new save if none", "gCreateNewSave", true, false);
                    Tooltip("Enable the creation of a new save file if none exist in the File number selected\nNo file name will be assigned please do in Save editor once you see the first text else your save file name will be named \"00000000\"\nIf disabled you will fall back in File select menu");
                };
                PaddedSeparator();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0,0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                static ImVec2 buttonSize(160.0f, 0.0f);
                if (ImGui::Button(GetWindowButtonText("Stats", CVar_GetS32("gStatsEnabled", 0)).c_str(), buttonSize))
                {
                    bool currentValue = CVar_GetS32("gStatsEnabled", 0);
                    CVar_SetS32("gStatsEnabled", !currentValue);
                    statsWindowOpen = true;
                    needs_save = true;
                }
                Tooltip("Shows the stats window, with your FPS and frametimes, and the OS you're playing on");
                InsertPadding();
                if (ImGui::Button(GetWindowButtonText("Console", CVar_GetS32("gConsoleEnabled", 0)).c_str(), buttonSize))
                {
                    bool currentValue = CVar_GetS32("gConsoleEnabled", 0);
                    CVar_SetS32("gConsoleEnabled", !currentValue);
                    needs_save = true;
                    if(CVar_GetS32("gConsoleEnabled", 0)){
                        console->Open();
                    } else {
                        console->Close();
                    }
                }
                Tooltip("Enables the console window, allowing you to input commands, type help for some examples");
                InsertPadding();
                if (ImGui::Button(GetWindowButtonText("Save Editor", CVar_GetS32("gSaveEditorEnabled", 0)).c_str(), buttonSize))
                {
                    bool currentValue = CVar_GetS32("gSaveEditorEnabled", 0);
                    CVar_SetS32("gSaveEditorEnabled", !currentValue);
                    needs_save = true;
                    customWindows["Save Editor"].enabled = CVar_GetS32("gSaveEditorEnabled", 0);
                }
                InsertPadding();
                if (ImGui::Button(GetWindowButtonText("Collision Viewer", CVar_GetS32("gCollisionViewerEnabled", 0)).c_str(), buttonSize))
                {
                    bool currentValue = CVar_GetS32("gCollisionViewerEnabled", 0);
                    CVar_SetS32("gCollisionViewerEnabled", !currentValue);
                    needs_save = true;
                    customWindows["Collision Viewer"].enabled = CVar_GetS32("gCollisionViewerEnabled", 0);
                }
                InsertPadding();
                if (ImGui::Button(GetWindowButtonText("Actor Viewer", CVar_GetS32("gActorViewerEnabled", 0)).c_str(), buttonSize))
                {
                    bool currentValue = CVar_GetS32("gActorViewerEnabled", 0);
                    CVar_SetS32("gActorViewerEnabled", !currentValue);
                    needs_save = true;
                    customWindows["Actor Viewer"].enabled = CVar_GetS32("gActorViewerEnabled", 0);
                }
                ImGui::PopStyleVar(3);
                ImGui::PopStyleColor(1);

                ImGui::EndMenu();
            }

            ImGui::SetCursorPosY(0.0f);

            if (ImGui::BeginMenu("Randomizer"))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                static ImVec2 buttonSize(200.0f, 0.0f);
                if (ImGui::Button(GetWindowButtonText("Randomizer Settings", CVar_GetS32("gRandomizerSettingsEnabled", 0)).c_str(), buttonSize))
                {
                    bool currentValue = CVar_GetS32("gRandomizerSettingsEnabled", 0);
                    CVar_SetS32("gRandomizerSettingsEnabled", !currentValue);
                    needs_save = true;
                    customWindows["Randomizer Settings"].enabled = CVar_GetS32("gRandomizerSettingsEnabled", 0);
                }
                InsertPadding();
                if (ImGui::Button(GetWindowButtonText("Item Tracker", CVar_GetS32("gItemTrackerEnabled", 0)).c_str(), buttonSize))
                {
                    bool currentValue = CVar_GetS32("gItemTrackerEnabled", 0);
                    CVar_SetS32("gItemTrackerEnabled", !currentValue);
                    needs_save = true;
                    customWindows["Item Tracker"].enabled = CVar_GetS32("gItemTrackerEnabled", 0);
                }
                ImGui::PopStyleVar(3);
                ImGui::PopStyleColor(1);

                ImGui::EndMenu();
            }

            ImGui::PopStyleVar(1);
            ImGui::EndMenuBar();
        }

        ImGui::End();

        for (const auto& category : hiddenwindowCategories) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::SetNextWindowSize(ImVec2 (0,0));
            ImGuiWindowFlags HiddenWndFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavInputs |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoDecoration;
            ImGui::Begin(category.first.c_str(), nullptr, HiddenWndFlags);
            ImGui::End();
            ImGui::PopStyleColor();
        }

        if (CVar_GetS32("gStatsEnabled", 0)) {
            if (!statsWindowOpen) {
                CVar_SetS32("gStatsEnabled", 0);
            }
            const float framerate = ImGui::GetIO().Framerate;
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Debug Stats", &statsWindowOpen, ImGuiWindowFlags_NoFocusOnAppearing);

#if defined(_WIN32)
            ImGui::Text("Platform: Windows");
#elif defined(__APPLE__)
            ImGui::Text("Platform: macOS");
#elif defined(__SWITCH__)
            ImGui::Text("Platform: Nintendo Switch");
#elif defined(__WIIU__)
            ImGui::Text("Platform: Nintendo Wii U");
#elif defined(__linux__)
            ImGui::Text("Platform: Linux");
#else
            ImGui::Text("Platform: Unknown");
#endif
            ImGui::Text("Status: %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
            ImGui::End();
            ImGui::PopStyleColor();
        }

        console->Draw();
        controller->DrawHud();

        for (auto& windowIter : customWindows) {
            CustomWindow& window = windowIter.second;
            if (window.drawFunc != nullptr) {
                window.drawFunc(window.enabled);
            }
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
        ImGui::Begin("Main Game", nullptr, flags);
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor();

        ImVec2 main_pos = ImGui::GetWindowPos();
        main_pos.x -= top_left_pos.x;
        main_pos.y -= top_left_pos.y;
        ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 pos = ImVec2(0, 0);
        gfx_current_dimensions.width = (uint32_t)(size.x * gfx_current_dimensions.internal_mul);
        gfx_current_dimensions.height = (uint32_t)(size.y * gfx_current_dimensions.internal_mul);
        gfx_current_game_window_viewport.x = (int16_t)main_pos.x;
        gfx_current_game_window_viewport.y = (int16_t)main_pos.y;
        gfx_current_game_window_viewport.width = (int16_t)size.x;
        gfx_current_game_window_viewport.height = (int16_t)size.y;

        if (CVar_GetS32("gN64Mode", 0))
        {
            gfx_current_dimensions.width = 320;
            gfx_current_dimensions.height = 240;
            const int sw = size.y * 320 / 240;
            gfx_current_game_window_viewport.x += ((int)size.x - sw) / 2;
            gfx_current_game_window_viewport.width = sw;
            pos = ImVec2(size.x / 2 - sw / 2, 0);
            size = ImVec2(sw, size.y);
        }

        overlay->Draw();
    }

    void DrawFramebufferAndGameInput(void) {
        const ImVec2 main_pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 pos = ImVec2(0, 0);
        if (CVar_GetS32("gN64Mode", 0)) {
            const float sw = size.y * 320.0f / 240.0f;
            pos = ImVec2(size.x / 2 - sw / 2, 0);
            size = ImVec2(sw, size.y);
        }
        if (gfxFramebuffer) {
            //ImGui::ImageSimple(reinterpret_cast<ImTextureID>(gfxFramebuffer), pos, size);
            ImGui::SetCursorPos(pos);
            ImGui::Image(reinterpret_cast<ImTextureID>(gfxFramebuffer), size);
        }

        ImGui::End();

        const float scale = CVar_GetFloat("gInputScale", 1.0f);
        ImVec2 BtnPos = ImVec2(160 * scale, 85 * scale);

        if (CVar_GetS32("gInputEnabled", 0)) {
            ImGui::SetNextWindowSize(BtnPos);
            ImGui::SetNextWindowPos(ImVec2(main_pos.x + size.x - BtnPos.x - 20, main_pos.y + size.y - BtnPos.y - 20));

            if (pads != nullptr && ImGui::Begin("Game Buttons", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground)) {
                ImGui::SetCursorPosY(32 * scale);

                ImGui::BeginGroup();
                const ImVec2 cPos = ImGui::GetCursorPos();
                ImGui::SetCursorPos(ImVec2(cPos.x + 10 * scale, cPos.y - 20 * scale));
                BindButton("L-Btn", pads[0].button & BTN_L);
                ImGui::SetCursorPos(ImVec2(cPos.x + 16 * scale, cPos.y));
                BindButton("C-Up", pads[0].button & BTN_CUP);
                ImGui::SetCursorPos(ImVec2(cPos.x, cPos.y + 16 * scale));
                BindButton("C-Left", pads[0].button & BTN_CLEFT);
                ImGui::SetCursorPos(ImVec2(cPos.x + 32 * scale, cPos.y + 16 * scale));
                BindButton("C-Right", pads[0].button & BTN_CRIGHT);
                ImGui::SetCursorPos(ImVec2(cPos.x + 16 * scale, cPos.y + 32 * scale));
                BindButton("C-Down", pads[0].button & BTN_CDOWN);
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                const ImVec2 sPos = ImGui::GetCursorPos();
                ImGui::SetCursorPos(ImVec2(sPos.x + 21, sPos.y - 20 * scale));
                BindButton("Z-Btn", pads[0].button & BTN_Z);
                ImGui::SetCursorPos(ImVec2(sPos.x + 22, sPos.y + 16 * scale));
                BindButton("Start-Btn", pads[0].button & BTN_START);
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                const ImVec2 bPos = ImGui::GetCursorPos();
                ImGui::SetCursorPos(ImVec2(bPos.x + 20 * scale, bPos.y - 20 * scale));
                BindButton("R-Btn", pads[0].button & BTN_R);
                ImGui::SetCursorPos(ImVec2(bPos.x + 12 * scale, bPos.y + 8 * scale));
                BindButton("B-Btn", pads[0].button & BTN_B);
                ImGui::SetCursorPos(ImVec2(bPos.x + 28 * scale, bPos.y + 24 * scale));
                BindButton("A-Btn", pads[0].button & BTN_A);
                ImGui::EndGroup();

                ImGui::End();
            }
        }
    }

    void applyEnhancementPresets(void) {

        switch (CVar_GetS32("gSelectEnhancementPresets", 0)) {
            // Default
        case 0:
            applyEnhancementPresetDefault();
            break;

            // Vanilla Plus
        case 1:
            applyEnhancementPresetDefault();
            applyEnhancementPresetVanillaPlus();
            break;

            // Enhanced
        case 2:
            applyEnhancementPresetDefault();
            applyEnhancementPresetVanillaPlus();
            applyEnhancementPresetEnhanced();
            break;

            // Randomizer
        case 3:
            applyEnhancementPresetDefault();
            applyEnhancementPresetVanillaPlus();
            applyEnhancementPresetEnhanced();
            applyEnhancementPresetRandomizer();
            break;
        }
    }

    void applyEnhancementPresetDefault(void) {
        // D-pad Support on Pause and File Select
        CVar_SetS32("gDpadPauseName", 0);
        // D-pad Support in Ocarina and Text Choice
        CVar_SetS32("gDpadOcarinaText", 0);
        // D-pad Support for Browsing Shop Items
        CVar_SetS32("gDpadShop", 0);
        // D-pad as Equip Items
        CVar_SetS32("gDpadEquips", 0);
        // Allow the cursor to be on any slot
        CVar_SetS32("gPauseAnyCursor", 0);
        // Prevent Dropped Ocarina Inputs
        CVar_SetS32("gDpadNoDropOcarinaInput", 0);
        // Answer Navi Prompt with L Button
        CVar_SetS32("gNaviOnL", 0);

        // Text Speed (1 to 5)
        CVar_SetS32("gTextSpeed", 1);
        // King Zora Speed (1 to 5)
        CVar_SetS32("gMweepSpeed", 1);
        // Biggoron Forge Time (0 to 3)
        CVar_SetS32("gForgeTime", 3);
        // Vine/Ladder Climb speed (+0 to +12)
        CVar_SetS32("gClimbSpeed", 0);
        // Faster Block Push (+0 to +5)
        CVar_SetS32("gFasterBlockPush", 0);
        // Faster Heavy Block Lift
        CVar_SetS32("gFasterHeavyBlockLift", 0);
        // No Forced Navi
        CVar_SetS32("gNoForcedNavi", 0);
        // No Skulltula Freeze
        CVar_SetS32("gSkulltulaFreeze", 0);
        // MM Bunny Hood
        CVar_SetS32("gMMBunnyHood", 0);
        // Fast Chests
        CVar_SetS32("gFastChests", 0);
        // Fast Drops
        CVar_SetS32("gFastDrops", 0);
        // Better Owl
        CVar_SetS32("gBetterOwl", 0);
        // Fast Ocarina Playback
        CVar_SetS32("gFastOcarinaPlayback", 0);
        // Instant Putaway
        CVar_SetS32("gInstantPutaway", 0);
        // Instant Boomerang Recall
        CVar_SetS32("gFastBoomerang", 0);
        // Mask Select in Inventory
        CVar_SetS32("gMaskSelect", 0);
        // Remember Save Location
        CVar_SetS32("gRememberSaveLocation", 0);

        // Damage Multiplier (0 to 8)
        CVar_SetS32("gDamageMul", 0);
        // Fall Damage Multiplier (0 to 7)
        CVar_SetS32("gFallDamageMul", 0);
        // Void Damage Multiplier (0 to 6)
        CVar_SetS32("gVoidDamageMul", 0);
        // No Random Drops
        CVar_SetS32("gNoRandomDrops", 0);
        // No Heart Drops
        CVar_SetS32("gNoHeartDrops", 0);
        // Always Win Goron Pot
        CVar_SetS32("gGoronPot", 0);

        // Change Red Potion Effect
        CVar_SetS32("gRedPotionEffect", 0);
        // Red Potion Health (1 to 100)
        CVar_SetS32("gRedPotionHealth", 1);
        // Red Potion Percent Restore
        CVar_SetS32("gRedPercentRestore", 0);
        // Change Green Potion Effect
        CVar_SetS32("gGreenPotionEffect", 0);
        // Green Potion Mana (1 to 100)
        CVar_SetS32("gGreenPotionMana", 1);
        // Green Potion Percent Restore
        CVar_SetS32("gGreenPercentRestore", 0);
        // Change Blue Potion Effects
        CVar_SetS32("gBluePotionEffects", 0);
        // Blue Potion Health (1 to 100)
        CVar_SetS32("gBluePotionHealth", 1);
        // Blue Potion Health Percent Restore
        CVar_SetS32("gBlueHealthPercentRestore", 0);
        // Blue Potion Mana (1 to 100)
        CVar_SetS32("gBluePotionMana", 1);
        // Blue Potion Mana Percent Restore
        CVar_SetS32("gBlueManaPercentRestore", 0);
        // Change Milk Effect
        CVar_SetS32("gMilkEffect", 0);
        // Milk Health (1 to 100)
        CVar_SetS32("gMilkHealth", 1);
        // Milk Percent Restore
        CVar_SetS32("gMilkPercentRestore", 0);
        // Separate Half Milk Effect
        CVar_SetS32("gSeparateHalfMilkEffect", 0);
        // Half Milk Health (1 to 100)
        CVar_SetS32("gHalfMilkHealth", 0);
        // Half Milk Percent Restore
        CVar_SetS32("gHalfMilkPercentRestore", 0);
        // Change Fairy Effect
        CVar_SetS32("gFairyEffect", 0);
        // Fairy (1 to 100)
        CVar_SetS32("gFairyHealth", 1);
        // Fairy Percent Restore
        CVar_SetS32("gFairyPercentRestore", 0);
        // Change Fairy Revive Effect
        CVar_SetS32("gFairyReviveEffect", 0);
        // Fairy Revival (1 to 100)
        CVar_SetS32("gFairyReviveHealth", 1);
        // Fairy Revive Percent Restore
        CVar_SetS32("gFairyRevivePercentRestore", 0);

        // Instant Fishing
        CVar_SetS32("gInstantFishing", 0);
        // Guarantee Bite
        CVar_SetS32("gGuaranteeFishingBite", 0);
        // Child Minimum Weight (6 to 10)
        CVar_SetS32("gChildMinimumWeightFish", 10);
        // Adult Minimum Weight (8 to 13)
        CVar_SetS32("gAdultMinimumWeightFish", 13);

        // Mute Low HP Alarm
        CVar_SetS32("gLowHpAlarm", 0);
        // Minimal UI
        CVar_SetS32("gMinimalUI", 0);
        // Disable Navi Call Audio
        CVar_SetS32("gDisableNaviCallAudio", 0);

        // Visual Stone of Agony
        CVar_SetS32("gVisualAgony", 0);
        // Assignable Tunics and Boots
        CVar_SetS32("gAssignableTunicsAndBoots", 0);
        // Equipment Toggle
        CVar_SetS32("gEquipmentCanBeRemoved", 0);
        // Link's Cow in Both Time Periods
        CVar_SetS32("gCowOfTime", 0);
        // Enable visible guard vision
        CVar_SetS32("gGuardVision", 0);
        // Enable passage of time on file select
        CVar_SetS32("gTimeFlowFileSelect", 0);
        // Count Golden Skulltulas
        CVar_SetS32("gInjectSkulltulaCount", 0);
        // Pull grave during the day
        CVar_SetS32("gDayGravePull", 0);

        // Rotate link (0 to 2)
        CVar_SetS32("gPauseLiveLinkRotation", 0);
        // Pause link animation (0 to 16)
        CVar_SetS32("gPauseLiveLink", 0);
        // Frames to wait
        CVar_SetS32("gMinFrameCount", 1);

        // N64 Mode
        CVar_SetS32("gN64Mode", 0);
        // Enable 3D Dropped items/projectiles
        CVar_SetS32("gNewDrops", 0);
        // Disable Black Bar Letterboxes
        CVar_SetS32("gDisableBlackBars", 0);
        // Dynamic Wallet Icon
        CVar_SetS32("gDynamicWalletIcon", 0);
        // Always show dungeon entrances
        CVar_SetS32("gAlwaysShowDungeonMinimapIcon", 0);

        // Fix L&R Pause menu
        CVar_SetS32("gUniformLR", 0);
        // Fix L&Z Page switch in Pause menu
        CVar_SetS32("gNGCKaleidoSwitcher", 0);
        // Fix Dungeon entrances
        CVar_SetS32("gFixDungeonMinimapIcon", 0);
        // Fix Two Handed idle animations
        CVar_SetS32("gTwoHandedIdle", 0);
        // Fix the Gravedigging Tour Glitch
        CVar_SetS32("gGravediggingTourFix", 0);
        // Fix Deku Nut upgrade
        CVar_SetS32("gDekuNutUpgradeFix", 0);
        // Fix Navi text HUD position
        CVar_SetS32("gNaviTextFix", 0);
        // Fix Anubis fireballs
        CVar_SetS32("gAnubisFix", 0);
        // Fix Megaton Hammer crouch stab
        CVar_SetS32("gCrouchStabHammerFix", 0);
        // Fix all crouch stab
        CVar_SetS32("gCrouchStabFix", 0);

        // Red Ganon blood
        CVar_SetS32("gRedGanonBlood", 0);
        // Fish while hovering
        CVar_SetS32("gHoverFishing", 0);
        // N64 Weird Frames
        CVar_SetS32("gN64WeirdFrames", 0);
        // Bombchus out of bounds
        CVar_SetS32("gBombchusOOB", 0);

        CVar_SetS32("gGsCutscene", 0);
        // Autosave
        CVar_SetS32("gAutosave", 0);
    }

    void applyEnhancementPresetVanillaPlus(void) {
        // D-pad Support in Ocarina and Text Choice
        CVar_SetS32("gDpadOcarinaText", 1);
        // D-pad Support for Browsing Shop Items
        CVar_SetS32("gDpadShop", 1);
        // D-pad as Equip Items
        CVar_SetS32("gDpadEquips", 1);
        // Prevent Dropped Ocarina Inputs
        CVar_SetS32("gDpadNoDropOcarinaInput", 1);

        // Text Speed (1 to 5)
        CVar_SetS32("gTextSpeed", 5);
        // King Zora Speed (1 to 5)
        CVar_SetS32("gMweepSpeed", 2);
        // Faster Block Push (+0 to +5)
        CVar_SetS32("gFasterBlockPush", 5);
        // Better Owl
        CVar_SetS32("gBetterOwl", 1);

        // Assignable Tunics and Boots
        CVar_SetS32("gAssignableTunicsAndBoots", 1);
        // Enable passage of time on file select
        CVar_SetS32("gTimeFlowFileSelect", 1);
        // Count Golden Skulltulas
        CVar_SetS32("gInjectSkulltulaCount", 1);

        // Pause link animation (0 to 16)
        CVar_SetS32("gPauseLiveLink", 1);

        // Dynamic Wallet Icon
        CVar_SetS32("gDynamicWalletIcon", 1);
        // Always show dungeon entrances
        CVar_SetS32("gAlwaysShowDungeonMinimapIcon", 1);

        // Fix L&R Pause menu
        CVar_SetS32("gUniformLR", 1);
        // Fix Dungeon entrances
        CVar_SetS32("gFixDungeonMinimapIcon", 1);
        // Fix Two Handed idle animations
        CVar_SetS32("gTwoHandedIdle", 1);
        // Fix the Gravedigging Tour Glitch
        CVar_SetS32("gGravediggingTourFix", 1);
        // Fix Deku Nut upgrade
        CVar_SetS32("gDekuNutUpgradeFix", 1);

        // Red Ganon blood
        CVar_SetS32("gRedGanonBlood", 1);
        // Fish while hovering
        CVar_SetS32("gHoverFishing", 1);
        // N64 Weird Frames
        CVar_SetS32("gN64WeirdFrames", 1);
        // Bombchus out of bounds
        CVar_SetS32("gBombchusOOB", 1);
    }

    void applyEnhancementPresetEnhanced(void) {
        // King Zora Speed (1 to 5)
        CVar_SetS32("gMweepSpeed", 5);
        // Biggoron Forge Time (0 to 3)
        CVar_SetS32("gForgeTime", 0);
        // Vine/Ladder Climb speed (+0 to +12)
        CVar_SetS32("gClimbSpeed", 1);
        // Faster Heavy Block Lift
        CVar_SetS32("gFasterHeavyBlockLift", 1);
        // No Forced Navi
        CVar_SetS32("gNoForcedNavi", 1);
        // No Skulltula Freeze
        CVar_SetS32("gSkulltulaFreeze", 1);
        // MM Bunny Hood
        CVar_SetS32("gMMBunnyHood", 1);
        // Fast Chests
        CVar_SetS32("gFastChests", 1);
        // Fast Drops
        CVar_SetS32("gFastDrops", 1);
        // Fast Ocarina Playback
        CVar_SetS32("gFastOcarinaPlayback", 1);
        // Instant Putaway
        CVar_SetS32("gInstantPutaway", 1);
        // Instant Boomerang Recall
        CVar_SetS32("gFastBoomerang", 1);
        // Mask Select in Inventory
        CVar_SetS32("gMaskSelect", 1);

        // Disable Navi Call Audio
        CVar_SetS32("gDisableNaviCallAudio", 1);

        // Equipment Toggle
        CVar_SetS32("gEquipmentCanBeRemoved", 1);
        // Count Golden Skulltulas
        CVar_SetS32("gInjectSkulltulaCount", 1);

        // Enable 3D Dropped items/projectiles
        CVar_SetS32("gNewDrops", 1);

        // Fix Anubis fireballs
        CVar_SetS32("gAnubisFix", 1);
    }

    void applyEnhancementPresetRandomizer(void) {
        // Allow the cursor to be on any slot
        CVar_SetS32("gPauseAnyCursor", 1);

        // Instant Fishing
        CVar_SetS32("gInstantFishing", 1);
        // Guarantee Bite
        CVar_SetS32("gGuaranteeFishingBite", 1);
        // Child Minimum Weight (6 to 10)
        CVar_SetS32("gChildMinimumWeightFish", 6);
        // Adult Minimum Weight (8 to 13)
        CVar_SetS32("gAdultMinimumWeightFish", 8);

        // Visual Stone of Agony
        CVar_SetS32("gVisualAgony", 1);
        // Pull grave during the day
        CVar_SetS32("gDayGravePull", 1);

        // Pause link animation (0 to 16)
        CVar_SetS32("gPauseLiveLink", 16);
        // Frames to wait
        CVar_SetS32("gMinFrameCount", 200);
    }

    void Render() {
        ImGui::Render();
        ImGuiRenderDrawData(ImGui::GetDrawData());
        if (UseViewports()) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void CancelFrame() {
        ImGui::EndFrame();
        if (UseViewports()) {
            ImGui::UpdatePlatformWindows();
        }
    }

    void BindCmd(const std::string& cmd, CommandEntry entry) {
        console->AddCommand(cmd, entry);
    }

    void AddWindow(const std::string& category, const std::string& name, WindowDrawFunc drawFunc, bool isEnabled, bool isHidden) {
        if (customWindows.contains(name)) {
            SPDLOG_ERROR("SohImGui::AddWindow: Attempting to add duplicate window name %s", name.c_str());
            return;
        }

        customWindows[name] = {
            .enabled = isEnabled,
            .drawFunc = drawFunc
        };

        if (isHidden) {
            hiddenwindowCategories[category].emplace_back(name);
        } else {
            windowCategories[category].emplace_back(name);
        }
    }

    ImTextureID GetTextureByName(const std::string& name) {
        return GetTextureByID(DefaultAssets[name]->textureId);
    }

    ImTextureID GetTextureByID(int id) {
#ifdef ENABLE_DX11
        if (impl.backend == Backend::DX11)
        {
            ImTextureID gfx_d3d11_get_texture_by_id(int id);
            return gfx_d3d11_get_texture_by_id(id);
        }
#endif
#ifdef __WIIU__
        if (impl.backend == Backend::GX2)
        {
            return gfx_gx2_texture_for_imgui(id);
        }
#endif

        return reinterpret_cast<ImTextureID>(id);
    }

    void BeginGroupPanel(const char* name, const ImVec2& size)
    {
        ImGui::BeginGroup();

        // auto cursorPos = ImGui::GetCursorScreenPos();
        auto itemSpacing = ImGui::GetStyle().ItemSpacing;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        auto frameHeight = ImGui::GetFrameHeight();
        ImGui::BeginGroup();

        ImVec2 effectiveSize = size;
        if (size.x < 0.0f)
            effectiveSize.x = ImGui::GetContentRegionAvail().x;
        else
            effectiveSize.x = size.x;
        ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::TextUnformatted(name);
        auto labelMin = ImGui::GetItemRectMin();
        auto labelMax = ImGui::GetItemRectMax();
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
        ImGui::BeginGroup();

        //ImGui::GetWindowDrawList()->AddRect(labelMin, labelMax, IM_COL32(255, 0, 255, 255));

        ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
        ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->WorkRect.Max.x -= frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->InnerRect.Max.x -= frameHeight * 0.5f;
#else
        ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
        ImGui::GetCurrentWindow()->Size.x -= frameHeight;

        auto itemWidth = ImGui::CalcItemWidth();
        ImGui::PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));
        s_GroupPanelLabelStack.push_back(ImRect(labelMin, labelMax));
    }

    void EndGroupPanel(float minHeight) {
        ImGui::PopItemWidth();

        auto itemSpacing = ImGui::GetStyle().ItemSpacing;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        auto frameHeight = ImGui::GetFrameHeight();

        ImGui::EndGroup();

        //ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

        ImGui::EndGroup();

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::Dummy(ImVec2(0.0, std::max(frameHeight - frameHeight * 0.5f - itemSpacing.y, minHeight)));

        ImGui::EndGroup();

        auto itemMin = ImGui::GetItemRectMin();
        auto itemMax = ImGui::GetItemRectMax();
        //ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

        auto labelRect = s_GroupPanelLabelStack.back();
        s_GroupPanelLabelStack.pop_back();

        ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
        ImRect frameRect = ImRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f));
        labelRect.Min.x -= itemSpacing.x;
        labelRect.Max.x += itemSpacing.x;
        for (int i = 0; i < 4; ++i)
        {
            switch (i)
            {
                // left half-plane
            case 0: ImGui::PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(labelRect.Min.x, FLT_MAX), true); break;
                // right half-plane
            case 1: ImGui::PushClipRect(ImVec2(labelRect.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true); break;
                // top
            case 2: ImGui::PushClipRect(ImVec2(labelRect.Min.x, -FLT_MAX), ImVec2(labelRect.Max.x, labelRect.Min.y), true); break;
                // bottom
            case 3: ImGui::PushClipRect(ImVec2(labelRect.Min.x, labelRect.Max.y), ImVec2(labelRect.Max.x, FLT_MAX), true); break;
            }

            ImGui::GetWindowDrawList()->AddRect(
                frameRect.Min, frameRect.Max,
                ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)),
                halfFrame.x);

            ImGui::PopClipRect();
        }

        ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
        ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->WorkRect.Max.x += frameHeight * 0.5f;
        ImGui::GetCurrentWindow()->InnerRect.Max.x += frameHeight * 0.5f;
#else
        ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
        ImGui::GetCurrentWindow()->Size.x += frameHeight;

        InsertPadding();

        ImGui::EndGroup();
    }

    // Automatically add newlines to break up tooltips longer than a specified number of characters
    // Manually included newlines will still be respected and reset the line length
    // Default line length is 60 characters
    std::string BreakTooltip(const char* text, int lineLength) {
        std::string newText(text);
        const int tipLength = newText.length();
        int lastSpace = -1;
        int currentLineLength = 0;
        for (int currentCharacter = 0; currentCharacter < tipLength; currentCharacter++) {
            if (newText[currentCharacter] == '\n') {
                currentLineLength = 0;
                lastSpace = -1;
                continue;
            }
            else if (newText[currentCharacter] == ' ') {
                lastSpace = currentCharacter;
            }

            if ((currentLineLength >= lineLength) && (lastSpace >= 0)) {
                newText[lastSpace] = '\n';
                currentLineLength = currentCharacter - lastSpace - 1;
                lastSpace = -1;
            }
            currentLineLength++;
        }
        return newText;
    }

    std::string BreakTooltip(const std::string& text, int lineLength) {
        return BreakTooltip(text.c_str(), lineLength);
    }

    void InsertPadding(float extraVerticalPadding) {
        ImGui::Dummy(ImVec2(0.0f, extraVerticalPadding));
    }

    void PaddedSeparator(bool padTop, bool padBottom, float extraVerticalTopPadding, float extraVerticalBottomPadding) {
        if (padTop) {
            ImGui::Dummy(ImVec2(0.0f, extraVerticalTopPadding));
        }
        ImGui::Separator();
        if (padBottom) {
            ImGui::Dummy(ImVec2(0.0f, extraVerticalBottomPadding));
        }
    }

    void PaddedEnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue, bool PlusMinusButton, bool padTop, bool padBottom) {
        if (padTop) {
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
        }
        EnhancementSliderInt(text, id, cvarName, min, max, format, defaultValue, PlusMinusButton);
        if (padBottom) {
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
        }
    }

    void PaddedEnhancementCheckbox(const char* text, const char* cvarName, bool padTop, bool padBottom) {
        if (padTop) {
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
        }
        EnhancementCheckbox(text, cvarName);
        if (padBottom) {
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
        }
    }

    void PaddedText(const char* text, bool padTop, bool padBottom) {
        if (padTop) {
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
        }
        ImGui::Text("%s", text);
        if (padBottom) {
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
        }
    }

    std::string GetWindowButtonText(const char* text, bool menuOpen) {
        char buttonText[100] = "";
        if(menuOpen) { strcat(buttonText,"> "); }
        strcat(buttonText, text);
        if (!menuOpen) { strcat(buttonText, "  "); }
        return buttonText;
    }
}
