#include "ImGuiImpl.h"

#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <algorithm>
#include <vector>

#include "Archive.h"
#include "Environment.h"
#include "GameSettings.h"
#include "Console.h"
#include "Hooks.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Lib/ImGui/imgui_internal.h"
#include "GlobalCtx2.h"
#include "ResourceMgr.h"
#include "TextureMod.h"
#include "Window.h"
#include "Cvar.h"
#include "GameOverlay.h"
#include "Texture.h"
#include "../Fast3D/gfx_pc.h"
#include "Lib/stb/stb_image.h"
#include "Lib/Fast3D/gfx_rendering_api.h"
#include "Lib/spdlog/include/spdlog/common.h"
#include "Utils/StringHelper.h"

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
    ImGui::Text("Experimental"); \
    ImGui::PopStyleColor(); \
    ImGui::Separator();
#define TOGGLE_BTN ImGuiKey_F1
#define TOGGLE_PAD_BTN ImGuiKey_GamepadBack
#define HOOK(b) if(b) needs_save = true;
OSContPad* pads;

std::map<std::string, GameAsset*> DefaultAssets;
std::vector<std::string> emptyArgs;

namespace SohImGui {

    WindowImpl impl;
    ImGuiIO* io;
    Console* console = new Console;
    GameOverlay* overlay = new GameOverlay;
    InputEditor* controller = new InputEditor;
    static ImVector<ImRect> s_GroupPanelLabelStack;
    bool p_open = false;
    bool needs_save = false;

    const char* filters[3] = {
        "Three-Point",
        "Linear",
        "None"
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

    int ClampFloatToInt(float value, int min, int max) {
        return fmin(fmax(value, min), max);
    }

    void Tooltip(const char* text) {
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", text);
    }

    void ImGuiWMInit() {
        switch (impl.backend) {
        case Backend::SDL:
            ImGui_ImplSDL2_InitForOpenGL(static_cast<SDL_Window*>(impl.sdl.window), impl.sdl.context);
            break;
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
        case Backend::SDL:
        #if defined(__APPLE__)
            ImGui_ImplOpenGL3_Init("#version 410 core");
        #else
            ImGui_ImplOpenGL3_Init("#version 120");
        #endif
            break;

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
        case Backend::SDL:
            ImGui_ImplSDL2_ProcessEvent(static_cast<const SDL_Event*>(event.sdl.event));
            break;
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
        case Backend::SDL:
            ImGui_ImplSDL2_NewFrame(static_cast<SDL_Window*>(impl.sdl.window));
            break;
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
        case Backend::SDL:
            ImGui_ImplOpenGL3_NewFrame();
            break;
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
        case Backend::SDL:
            ImGui_ImplOpenGL3_RenderDrawData(data);
            break;
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
            GlobalCtx2::GetInstance()->GetWindow()->ShowCursor(hide);
            return;
        }

        if (d == Dialogues::dConsole && CVar_GetS32("gOpenMenuBar", 0)) {
            return;
        }
        if (!GlobalCtx2::GetInstance()->GetWindow()->IsFullscreen()) {
            oldCursorState = false;
            return;
        }

        if (oldCursorState != hide) {
            oldCursorState = hide;
            GlobalCtx2::GetInstance()->GetWindow()->ShowCursor(hide);
        }
    }

    void LoadTexture(const std::string& name, const std::string& path) {
        GfxRenderingAPI* api = gfx_get_current_rendering_api();
        const auto res = GlobalCtx2::GetInstance()->GetResourceManager()->LoadFile(path);

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

    void LoadPickersColors(ImVec4& ColorArray, const char* cvarname, const ImVec4& default_colors, bool has_alpha) {
        std::string Cvar_Red = cvarname;
        Cvar_Red += "R";
        std::string Cvar_Green = cvarname;
        Cvar_Green += "G";
        std::string Cvar_Blue = cvarname;
        Cvar_Blue += "B";
        std::string Cvar_Alpha = cvarname;
        Cvar_Alpha += "A";

        ColorArray.x = (float)CVar_GetS32(Cvar_Red.c_str(), default_colors.x) / 255;
        ColorArray.y = (float)CVar_GetS32(Cvar_Green.c_str(), default_colors.y) / 255;
        ColorArray.z = (float)CVar_GetS32(Cvar_Blue.c_str(), default_colors.z) / 255;
        if (has_alpha) {
            ColorArray.w = (float)CVar_GetS32(Cvar_Alpha.c_str(), default_colors.w) / 255;
        }
    }

    void LoadResource(const std::string& name, const std::string& path, const ImVec4& tint) {
        GfxRenderingAPI* api = gfx_get_current_rendering_api();
        const auto res = static_cast<Ship::Texture*>(GlobalCtx2::GetInstance()->GetResourceManager()->LoadResource(path).get());

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
        Game::LoadSettings();
        impl = window_impl;
        ImGuiContext* ctx = ImGui::CreateContext();
        ImGui::SetCurrentContext(ctx);
        io = &ImGui::GetIO();
        io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io->Fonts->AddFontDefault();
        if (CVar_GetS32("gOpenMenuBar", 0) != 1) {
            SohImGui::overlay->TextDrawNotification(30.0f, true, "Press F1 to access enhancements menu");
        }

        auto imguiIniPath = Ship::GlobalCtx2::GetPathRelativeToAppDirectory("imgui.ini");
        auto imguiLogPath = Ship::GlobalCtx2::GetPathRelativeToAppDirectory("imgui_log.txt");
        io->IniFilename = strcpy(new char[imguiIniPath.length() + 1], imguiIniPath.c_str());
        io->LogFilename = strcpy(new char[imguiLogPath.length() + 1], imguiLogPath.c_str());

        if (UseViewports()) {
            io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        }
        console->Init();
        overlay->Init();
        controller->Init();
        ImGuiWMInit();
        ImGuiBackendInit();

        ModInternal::RegisterHook<ModInternal::GfxInit>([] {
            if (GlobalCtx2::GetInstance()->GetWindow()->IsFullscreen())
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

        ModInternal::RegisterHook<ModInternal::ControllerRead>([](OSContPad* cont_pad) {
            pads = cont_pad;
        });

        Game::InitSettings();

        CVar_SetS32("gRandoGenerating", 0);
        CVar_SetS32("gNewSeedGenerated", 0);
        CVar_SetS32("gNewFileDropped", 0);
        CVar_SetString("gDroppedFile", "None");
        // Game::SaveSettings();
    }

    void Update(EventImpl event) {
        if (needs_save) {
            Game::SaveSettings();
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
            Game::SetSeqPlayerVolume(playerId, volume);
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

    void EnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue)
    {
        int val = CVar_GetS32(cvarName, defaultValue);

        ImGui::Text(text, val);

        if (ImGui::SliderInt(id, &val, min, max, format))
        {
            CVar_SetS32(cvarName, val);
            needs_save = true;
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

    void EnhancementSliderFloat(const char* text, const char* id, const char* cvarName, float min, float max, const char* format, float defaultValue, bool isPercentage)
    {
        float val = CVar_GetFloat(cvarName, defaultValue);

        if (!isPercentage)
            ImGui::Text(text, val);
        else
            ImGui::Text(text, static_cast<int>(100 * val));

        if (ImGui::SliderFloat(id, &val, min, max, format))
        {
            CVar_SetFloat(cvarName, val);
            needs_save = true;
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
        std::string Cvar_Red = cvarName;
        Cvar_Red += "R";
        std::string Cvar_Green = cvarName;
        Cvar_Green += "G";
        std::string Cvar_Blue = cvarName;
        Cvar_Blue += "B";
        std::string Cvar_Alpha = cvarName;
        Cvar_Alpha += "A";
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
            CVar_SetS32(Cvar_Red.c_str(), ClampFloatToInt(colors->x * 255, 0, 255));
            CVar_SetS32(Cvar_Green.c_str(), ClampFloatToInt(colors->y * 255, 0, 255));
            CVar_SetS32(Cvar_Blue.c_str(), ClampFloatToInt(colors->z * 255, 0, 255));
            if (has_alpha) { CVar_SetS32(Cvar_Alpha.c_str(), ClampFloatToInt(colors->w * 255, 0, 255)); };
            CVar_SetS32(Cvar_RBM.c_str(), 0); //On click disable rainbow mode.
            needs_save = true;
        }
        Tooltip("Revert colors to the game's original colors (GameCube version)\nOverwrites previously chosen color");
    }

    void EnhancementColor(const char* text, const char* cvarName, ImVec4 ColorRGBA, ImVec4 default_colors, bool allow_rainbow, bool has_alpha, bool TitleSameLine) {
        //This will be moved to external cosmetics ed
	std::string Cvar_Red = cvarName;
        Cvar_Red += "R";
        std::string Cvar_Green = cvarName;
        Cvar_Green += "G";
        std::string Cvar_Blue = cvarName;
        Cvar_Blue += "B";
        std::string Cvar_Alpha = cvarName;
        Cvar_Alpha += "A";
        std::string Cvar_RBM = cvarName;
        Cvar_RBM += "RBM";

        LoadPickersColors(ColorRGBA, cvarName, default_colors, has_alpha);
        ImGuiColorEditFlags flags = ImGuiColorEditFlags_None;

        if (!TitleSameLine) {
            ImGui::Text("%s", text);
            flags = ImGuiColorEditFlags_NoLabel;
        }
        if (!has_alpha) {
            if (ImGui::ColorEdit3(text, (float*)&ColorRGBA, flags)) {
                CVar_SetS32(Cvar_Red.c_str(), ClampFloatToInt(ColorRGBA.x * 255, 0, 255));
                CVar_SetS32(Cvar_Green.c_str(), ClampFloatToInt(ColorRGBA.y * 255, 0, 255));
                CVar_SetS32(Cvar_Blue.c_str(), ClampFloatToInt(ColorRGBA.z * 255, 0, 255));
                needs_save = true;
            }
        } else {
            if (ImGui::ColorEdit4(text, (float*)&ColorRGBA, flags)) {
                CVar_SetS32(Cvar_Red.c_str(), ClampFloatToInt(ColorRGBA.x * 255, 0, 255));
                CVar_SetS32(Cvar_Green.c_str(), ClampFloatToInt(ColorRGBA.y * 255, 0, 255));
                CVar_SetS32(Cvar_Blue.c_str(), ClampFloatToInt(ColorRGBA.z * 255, 0, 255));
                CVar_SetS32(Cvar_Alpha.c_str(), ClampFloatToInt(ColorRGBA.w * 255, 0, 255));
                needs_save = true;
            }

        }
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

        const std::shared_ptr<Window> wnd = GlobalCtx2::GetInstance()->GetWindow();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize;
        if (CVar_GetS32("gOpenMenuBar", 0)) window_flags |= ImGuiWindowFlags_MenuBar;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(ImVec2(wnd->GetCurrentWidth(), wnd->GetCurrentHeight()));
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

        if (ImGui::IsKeyPressed(TOGGLE_BTN)) {
            bool menu_bar = CVar_GetS32("gOpenMenuBar", 0);
            CVar_SetS32("gOpenMenuBar", !menu_bar);
            needs_save = true;
            GlobalCtx2::GetInstance()->GetWindow()->dwMenubar = menu_bar;
            ShowCursor(menu_bar, Dialogues::dMenubar);
            GlobalCtx2::GetInstance()->GetWindow()->GetControlDeck()->SaveControllerSettings();
            if (CVar_GetS32("gControlNav", 0)) {
                if (CVar_GetS32("gOpenMenuBar", 0)) {
                    io->ConfigFlags |=ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_NavEnableKeyboard;
                } else {
                    io->ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
                }
            } else {
                io->ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
            }
        }

        #if __APPLE__
        if ((ImGui::IsKeyDown(ImGuiKey_LeftSuper) ||
             ImGui::IsKeyDown(ImGuiKey_RightSuper)) &&
             ImGui::IsKeyPressed(ImGuiKey_R, false)) {
            console->Commands["reset"].handler(emptyArgs);
        }
        #else
        if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
             ImGui::IsKeyDown(ImGuiKey_RightCtrl)) &&
             ImGui::IsKeyPressed(ImGuiKey_R, false)) {
            console->Commands["reset"].handler(emptyArgs);
        }
        #endif

        if (ImGui::BeginMenuBar()) {
            if (DefaultAssets.contains("Game_Icon")) {
                ImGui::SetCursorPos(ImVec2(5, 2.5f));
                ImGui::Image(GetTextureByID(DefaultAssets["Game_Icon"]->textureId), ImVec2(16.0f, 16.0f));
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(25, 0));
            }

            if (ImGui::BeginMenu("Shipwright")) {
                if (ImGui::MenuItem("Reset",
                    #if __APPLE__
                    "Command-R"
                    #else
                    "Ctrl+R"
                    #endif
                    )) {
                    console->Commands["reset"].handler(emptyArgs);
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();

            ImGui::SetCursorPosY(0.0f);

            if (ImGui::BeginMenu("Audio")) {
                EnhancementSliderFloat("Master Volume: %d %%", "##Master_Vol", "gGameMasterVolume", 0.0f, 1.0f, "", 1.0f, true);

                BindAudioSlider("Main Music Volume: %d %%", "gMainMusicVolume", 1.0f, SEQ_BGM_MAIN);
                BindAudioSlider("Sub Music Volume: %d %%", "gSubMusicVolume", 1.0f, SEQ_BGM_SUB);
                BindAudioSlider("Sound Effects Volume: %d %%", "gSFXMusicVolume", 1.0f, SEQ_SFX);
                BindAudioSlider("Fanfare Volume: %d %%", "gFanfareVolume", 1.0f, SEQ_FANFARE);

                ImGui::EndMenu();
            }

            ImGui::SetCursorPosY(0.0f);

            if (ImGui::BeginMenu("Controller"))
            {
                EnhancementCheckbox("Use Controller Navigation", "gControlNav");
                Tooltip("Allows controller navigation of the menu bar\nD-pad to move between items, A to select, and X to grab focus on the menu bar");

                EnhancementCheckbox("Controller Configuration", "gControllerConfigurationEnabled");
                controller->Opened = CVar_GetS32("gControllerConfigurationEnabled", 0);

                ImGui::Separator();

                // TODO mutual exclusions -- There should be some system to prevent conclifting enhancements from being selected
                EnhancementCheckbox("D-pad Support on Pause and File Select", "gDpadPauseName");
                Tooltip("Enables Pause and File Select screen navigation with the D-pad\nIf used with D-pad as Equip Items, you must hold C-Up\nto equip instead of navigate");
                EnhancementCheckbox("D-pad Support in Ocarina and Text Choice", "gDpadOcarinaText");
                EnhancementCheckbox("D-pad Support for Browsing Shop Items", "gDpadShop");
                EnhancementCheckbox("D-pad as Equip Items", "gDpadEquips");
                Tooltip("Allows the D-pad to be used as extra C buttons");
                ImGui::Separator();

                EnhancementCheckbox("Show Inputs", "gInputEnabled");
                Tooltip("Shows currently pressed inputs on the bottom right of the screen");

                EnhancementSliderFloat("Input Scale: %.1f", "##Input", "gInputScale", 1.0f, 3.0f, "", 1.0f, false);
                Tooltip("Sets the on screen size of the displayed inputs from the Show Inputs setting");

                ImGui::EndMenu();
            }

            ImGui::SetCursorPosY(0.0f);

            if (ImGui::BeginMenu("Graphics"))
            {
                EnhancementSliderFloat("Internal Resolution: %d %%", "##IMul", "gInternalResolution", 0.5f, 2.0f, "", 1.0f, true);
                Tooltip("Multiplies your output resolution by the value inputted,\nas a more intensive but effective form of anti-aliasing");
                gfx_current_dimensions.internal_mul = CVar_GetFloat("gInternalResolution", 1);
                EnhancementSliderInt("MSAA: %d", "##IMSAA", "gMSAAValue", 1, 8, "");
                Tooltip("Activates multi-sample anti-aliasing when above 1x\nup to 8x for 8 samples for every pixel");
                gfx_msaa_level = CVar_GetS32("gMSAAValue", 1);

                if (impl.backend == Backend::DX11)
                {
                    const char* cvar = "gExtraLatencyThreshold";
                    int val = CVar_GetS32(cvar, 80);
                    val = MAX(MIN(val, 250), 0);
                    int fps = val;

                    if (fps == 0)
                    {
                        ImGui::Text("Jitter fix: Off");
                    }
                    else
                    {
                        ImGui::Text("Jitter fix: >= %d FPS", fps);
                    }

                    if (ImGui::SliderInt("##ExtraLatencyThreshold", &val, 0, 250, "", ImGuiSliderFlags_AlwaysClamp))
                    {
                        CVar_SetS32(cvar, val);
                        needs_save = true;
                    }

                    Tooltip("When Interpolation FPS setting is at least this threshold,\n"
                        "add one frame of input lag (e.g. 16.6 ms for 60 FPS)\n"
                        "in order to avoid jitter.This setting allows the CPU\n"
                        "to work on one frame while GPU works on the previous frame.\n"
                        "This setting should be used when your computer is too slow\n"
                        "to do CPU + GPU work in time.");
                }

                EXPERIMENTAL();
                ImGui::Text("Texture Filter (Needs reload)");
                EnhancementCombobox("gTextureFilter", filters, 3, 0);
                GfxRenderingAPI* gapi = gfx_get_current_rendering_api();
                gapi->set_texture_filter((FilteringMode)CVar_GetS32("gTextureFilter", 0));
                overlay->DrawSettings();
                ImGui::EndMenu();
            }

            ImGui::SetCursorPosY(0.0f);

            if (ImGui::BeginMenu("Languages")) {
                EnhancementRadioButton("English", "gLanguages", 0);
                EnhancementRadioButton("German", "gLanguages", 1);
                EnhancementRadioButton("French", "gLanguages", 2);
                ImGui::EndMenu();
            }

            ImGui::SetCursorPosY(0.0f);

            if (ImGui::BeginMenu("Enhancements"))
            {
                if (ImGui::BeginMenu("Gameplay"))
                {
                    if (ImGui::BeginMenu("Time Savers"))
                    {
                        EnhancementSliderInt("Text Speed: %dx", "##TEXTSPEED", "gTextSpeed", 1, 5, "");
                        EnhancementSliderInt("King Zora Speed: %dx", "##MWEEPSPEED", "gMweepSpeed", 1, 5, "");
                        EnhancementSliderInt("Biggoron Forge Time: %d days", "##FORGETIME", "gForgeTime", 0, 3, "");
                        Tooltip("Allows you to change the number of days it takes for\nBiggoron to forge the Biggoron Sword");
                        EnhancementSliderInt("Vine/Ladder Climb speed +%d", "##CLIMBSPEED", "gClimbSpeed", 0, 12, "");

                        EnhancementCheckbox("Faster Block Push", "gFasterBlockPush");
                        EnhancementCheckbox("No Forced Navi", "gNoForcedNavi");
                        Tooltip("Prevent forced Navi conversations");
                        EnhancementCheckbox("No Skulltula Freeze", "gSkulltulaFreeze");
                        Tooltip("Stops the game from freezing the player\nwhen picking up Gold Skulltulas");
                        EnhancementCheckbox("MM Bunny Hood", "gMMBunnyHood");
                        Tooltip("Wearing the Bunny Hood grants a speed\nincrease like in Majora's Mask");
                        EnhancementCheckbox("Fast Chests", "gFastChests");
                        Tooltip("Kick open every chest");
                        EnhancementCheckbox("Fast Drops", "gFastDrops");
                        Tooltip("Skip first-time pickup messages for consumable items");
                        EnhancementCheckbox("Better Owl", "gBetterOwl");
                        Tooltip("The default response to Kaepora Gaebora is\nalways that you understood what he said");
                        EnhancementCheckbox("Fast Ocarina Playback", "gFastOcarinaPlayback");
                        Tooltip("Skip the part where the Ocarina playback is called when you play\na song");

                        ImGui::EndMenu();
                    }

                    if (ImGui::BeginMenu("Difficulty Options"))
                    {
                        ImGui::Text("Damage Multiplier");
                        EnhancementCombobox("gDamageMul", powers, 9, 0);
                        Tooltip("Modifies all sources of damage not affected by other sliders\n\
2x: Can survive all common attacks from the start of the game\n\
4x: Dies in 1 hit to any substantial attack from the start of the game\n\
8x: Can only survive trivial damage from the start of the game\n\
16x: Can survive all common attacks with max health without double defense\n\
32x: Can survive all common attacks with max health and double defense\n\
64x: Can survive trivial damage with max health without double defense\n\
128x: Can survive trivial damage with max health and double defense\n\
256x: Cannot survive damage");
                        ImGui::Text("Fall Damage Multiplier");
                        EnhancementCombobox("gFallDamageMul", powers, 8, 0);
                        Tooltip("Modifies all fall damage\n\
2x: Can survive all fall damage from the start of the game\n\
4x: Can only survive short fall damage from the start of the game\n\
8x: Cannot survive any fall damage from the start of the game\n\
16x: Can survive all fall damage with max health without double defense\n\
32x: Can survive all fall damage with max health and double defense\n\
64x: Can survive short fall damage with double defense\n\
128x: Cannot survive fall damage");
                        ImGui::Text("Void Damage Multiplier");
                        EnhancementCombobox("gVoidDamageMul", powers, 7, 0);
                        Tooltip("Modifies damage taken after falling into a void\n\
2x: Can survive void damage from the start of the game\n\
4x: Cannot survive void damage from the start of the game\n\
8x: Can survive void damage twice with max health without double defense\n\
16x: Can survive void damage with max health without double defense\n\
32x: Can survive void damage with max health and double defense\n\
64x: Cannot survive void damage");

                        EnhancementCheckbox("No Random Drops", "gNoRandomDrops");
                        Tooltip("Disables random drops, except from the Goron Pot, Dampe, and bosses");
                        EnhancementCheckbox("No Heart Drops", "gNoHeartDrops");
                        Tooltip("Disables heart drops, but not heart placements, like from a Deku Scrub running off\nThis simulates Hero Mode from other games in the series");

                        if (ImGui::BeginMenu("Potion Values"))
                        {
                            EnhancementCheckbox("Change Red Potion Effect", "gRedPotionEffect");
                            Tooltip("Enable the following changes to the amount of health restored by Red Potions");
                            EnhancementSliderInt("Red Potion Health: %d", "##REDPOTIONHEALTH", "gRedPotionHealth", 1, 100, "");
                            Tooltip("Changes the amount of health restored by Red Potions");
                            EnhancementCheckbox("Red Potion Percent Restore", "gRedPercentRestore");
                            Tooltip("Toggles from Red Potions restoring a fixed amount of health to a percent of the player's current max health");

                            EnhancementCheckbox("Change Green Potion Effect", "gGreenPotionEffect");
                            Tooltip("Enable the following changes to the amount of mana restored by Green Potions");
                            EnhancementSliderInt("Green Potion Mana: %d", "##GREENPOTIONMANA", "gGreenPotionMana", 1, 100, "");
                            Tooltip("Changes the amount of mana restored by Green Potions, base max mana is 48, max upgraded mana is 96");
                            EnhancementCheckbox("Green Potion Percent Restore", "gGreenPercentRestore");
                            Tooltip("Toggles from Green Potions restoring a fixed amount of mana to a percent of the player's current max mana");

                            EnhancementCheckbox("Change Blue Potion Effects", "gBluePotionEffects");
                            Tooltip("Enable the following changes to the amount of health and mana restored by Blue Potions");
                            EnhancementSliderInt("Blue Potion Health: %d", "##BLUEPOTIONHEALTH", "gBluePotionHealth", 1, 100, "");
                            Tooltip("Changes the amount of health restored by Blue Potions");
                            EnhancementCheckbox("Blue Potion Health Percent Restore", "gBlueHealthPercentRestore");
                            Tooltip("Toggles from Blue Potions restoring a fixed amount of health to a percent of the player's current max health");

                            EnhancementSliderInt("Blue Potion Mana: %d", "##BLUEPOTIONMANA", "gBluePotionMana", 1, 100, "");
                            Tooltip("Changes the amount of mana restored by Blue Potions, base max mana is 48, max upgraded mana is 96");
                            EnhancementCheckbox("Blue Potion Mana Percent Restore", "gBlueManaPercentRestore");
                            Tooltip("Toggles from Blue Potions restoring a fixed amount of mana to a percent of the player's current max mana");

                            EnhancementCheckbox("Change Milk Effect", "gMilkEffect");
                            Tooltip("Enable the following changes to the amount of health restored by Milk");
                            EnhancementSliderInt("Milk Health: %d", "##MILKHEALTH", "gMilkHealth", 1, 100, "");
                            Tooltip("Changes the amount of health restored by Milk");
                            EnhancementCheckbox("Milk Percent Restore", "gMilkPercentRestore");
                            Tooltip("Toggles from Milk restoring a fixed amount of health to a percent of the player's current max health");

                            EnhancementCheckbox("Separate Half Milk Effect", "gSeparateHalfMilkEffect");
                            Tooltip("Enable the following changes to the amount of health restored by Half Milk\nIf this is disabled, Half Milk will behave the same as Full Milk.");
                            EnhancementSliderInt("Half Milk Health: %d", "##HALFMILKHEALTH", "gHalfMilkHealth", 1, 100, "");
                            Tooltip("Changes the amount of health restored by Half Milk");
                            EnhancementCheckbox("Half Milk Percent Restore", "gHalfMilkPercentRestore");
                            Tooltip("Toggles from Half Milk restoring a fixed amount of health to a percent of the player's current max health");

                            EnhancementCheckbox("Change Fairy Effect", "gFairyEffect");
                            Tooltip("Enable the following changes to the amount of health restored by Fairies");
                            EnhancementSliderInt("Fairy: %d", "##FAIRYHEALTH", "gFairyHealth", 1, 100, "");
                            Tooltip("Changes the amount of health restored by Fairies");
                            EnhancementCheckbox("Fairy Percent Restore", "gFairyPercentRestore");
                            Tooltip("Toggles from Fairies restoring a fixed amount of health to a percent of the player's current max health");

                            EnhancementCheckbox("Change Fairy Revive Effect", "gFairyReviveEffect");
                            Tooltip("Enable the following changes to the amount of health restored by Fairy Revivals");
                            EnhancementSliderInt("Fairy Revival: %d", "##FAIRYREVIVEHEALTH", "gFairyReviveHealth", 1, 100, "");
                            Tooltip("Changes the amount of health restored by Fairy Revivals");
                            EnhancementCheckbox("Fairy Revive Percent Restore", "gFairyRevivePercentRestore");
                            Tooltip("Toggles from Fairy Revivals restoring a fixed amount of health to a percent of the player's current max health");

                            ImGui::EndMenu();
                        }

                        if (ImGui::BeginMenu("Fishing")) {
                            EnhancementCheckbox("Instant Fishing", "gInstantFishing");
                            Tooltip("All fish will be caught instantly");
                            EnhancementCheckbox("Guarantee Bite", "gGuaranteeFishingBite");
                            Tooltip("When a line is stable, guarantee bite. Otherwise use default logic");
                            EnhancementSliderInt("Child Minimum Weight: %d", "##cMinimumWeight", "gChildMinimumWeightFish", 6, 10, "", 10);
                            Tooltip("The minimum weight for the unique fishing reward as a child");
                            EnhancementSliderInt("Adult Minimum Weight: %d", "##aMinimumWeight", "gAdultMinimumWeightFish", 8, 13, "", 13);
                            Tooltip("The minimum weight for the unique fishing reward as an adult");
                            ImGui::EndMenu();
                        }

                        ImGui::EndMenu();
                    }

                    if (ImGui::BeginMenu("Reduced Clutter"))
                    {
                        EnhancementCheckbox("Mute Low HP Alarm", "gLowHpAlarm");
                        Tooltip("Disable the low HP beeping sound");
                        EnhancementCheckbox("Minimal UI", "gMinimalUI");
                        Tooltip("Hides most of the UI when not needed\nNote: Doesn't activate until after loading a new scene");
                        EnhancementCheckbox("Disable Navi Call Audio", "gDisableNaviCallAudio");
                        Tooltip("Disables the voice audio when Navi calls you");

                        ImGui::EndMenu();
                    }

                    EnhancementCheckbox("Visual Stone of Agony", "gVisualAgony");
                    Tooltip("Displays an icon and plays a sound when Stone of Agony\nshould be activated, for those without rumble");
                    EnhancementCheckbox("Assignable Tunics and Boots", "gAssignableTunicsAndBoots");
                    Tooltip("Allows equipping the tunic and boots to c-buttons");
                    EnhancementCheckbox("Link's Cow in Both Time Periods", "gCowOfTime");
                    Tooltip("Allows the Lon Lon Ranch obstacle course reward to be\nshared across time periods");
                    EnhancementCheckbox("Enable visible guard vision", "gGuardVision");
                    EnhancementCheckbox("Enable passage of time on file select", "gTimeFlowFileSelect");
                    EnhancementCheckbox("Allow the cursor to be on any slot", "gPauseAnyCursor");
                    Tooltip("Allows the cursor on the pause menu to be over any slot\nSimilar to Rando and Spaceworld 97");
                    EnhancementCheckbox("Count Golden Skulltulas", "gInjectSkulltulaCount");
                    Tooltip("Injects Golden Skulltula total count in pickup messages");
                    EnhancementCheckbox("Pull grave during the day", "gDayGravePull");
                    Tooltip("Allows graves to be pulled when child during the day");
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Graphics"))
                {
                    if (ImGui::BeginMenu("Animated Link in Pause Menu")) {
                        ImGui::Text("Rotation");
                        EnhancementRadioButton("Disabled", "gPauseLiveLinkRotation", 0);
                        EnhancementRadioButton("Rotate Link with D-pad", "gPauseLiveLinkRotation", 1);
                        Tooltip("Allow you to rotate Link on the Equipment menu with the DPAD\nUse DPAD-Up or DPAD-Down to reset Link's rotation");
                        EnhancementRadioButton("Rotate Link with C-buttons", "gPauseLiveLinkRotation", 2);
                        Tooltip("Allow you to rotate Link on the Equipment menu with the C-buttons\nUse C-Up or C-Down to reset Link's rotation");

                        if (CVar_GetS32("gPauseLiveLinkRotation", 0) != 0) {
                            EnhancementSliderInt("Rotation Speed: %d", "##MinRotationSpeed", "gPauseLiveLinkRotationSpeed", 1, 20, "");
                        }
                        ImGui::Separator();
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
                        ImGui::Separator();
                        ImGui::Text("Randomize");
                        EnhancementRadioButton("Random", "gPauseLiveLink", 15);
                        Tooltip("Randomize the animation played each time you open the menu");
                        EnhancementRadioButton("Random cycle", "gPauseLiveLink", 16);
                        Tooltip("Randomize the animation played on the menu after a certain time");
                        if (CVar_GetS32("gPauseLiveLink", 0) >= 16) {
                            EnhancementSliderInt("Frame to wait: %d", "##MinFrameCount", "gMinFrameCount", 1, 1000, "");
                        }

                        ImGui::EndMenu();
                    }
                    EnhancementCheckbox("N64 Mode", "gN64Mode");
                    Tooltip("Sets aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution");
                    EnhancementCheckbox("Enable 3D Dropped items/projectiles", "gNewDrops");
                    Tooltip("Change most 2D items and projectiles on the overworld to their 3D versions");
                    EnhancementCheckbox("Disable Black Bar Letterboxes", "gDisableBlackBars");
                    Tooltip("Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be minor visual glitches that\nwere covered up by the black bars\nPlease disable this setting before reporting a bug");
                    EnhancementCheckbox("Dynamic Wallet Icon", "gDynamicWalletIcon");
                    Tooltip("Changes the rupee in the wallet icon to match the wallet size you currently have");
                    EnhancementCheckbox("Always show dungeon entrances", "gAlwaysShowDungeonMinimapIcon");
                    Tooltip("Always shows dungeon entrance icons on the minimap");

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Fixes"))
                {
                    EnhancementCheckbox("Fix L&R Pause menu", "gUniformLR");
                    Tooltip("Makes the L and R buttons in the pause menu the same color");
                    EnhancementCheckbox("Fix L&Z Page switch in Pause menu", "gNGCKaleidoSwitcher");
                    Tooltip("Makes L and R switch pages like on the GameCube\nZ opens the Debug Menu instead");
                    EnhancementCheckbox("Fix Dungeon entrances", "gFixDungeonMinimapIcon");
                    Tooltip("Removes the dungeon entrance icon on the top-left corner\nof the screen when no dungeon is present on the current map");
                    EnhancementCheckbox("Fix Two Handed idle animations", "gTwoHandedIdle");
                    Tooltip("Re-enables the two-handed idle animation, a seemingly\nfinished animation that was disabled on accident in the original game");
                    EnhancementCheckbox("Fix the Gravedigging Tour Glitch", "gGravediggingTourFix");
                    Tooltip("Fixes a bug where the Gravedigging Tour Heart\nPiece disappears if the area reloads");
                    EnhancementCheckbox("Fix Deku Nut upgrade", "gDekuNutUpgradeFix");
                    Tooltip("Prevents the Forest Stage Deku Nut upgrade from\nbecoming unobtainable after receiving the Poacher's Saw");
                    EnhancementCheckbox("Fix Navi text HUD position", "gNaviTextFix");
                    Tooltip("Correctly centers the Navi text prompt on the HUD's C-Up button");
                    EnhancementCheckbox("Fix Anubis fireballs", "gAnubisFix");
                    Tooltip("Make Anubis fireballs do fire damage when reflected\nback at them with the Mirror Shield");

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Restoration"))
                {
                    EnhancementCheckbox("Red Ganon blood", "gRedGanonBlood");
                    Tooltip("Restore the original red blood from NTSC 1.0/1.1. Disable for green blood");
                    EnhancementCheckbox("Fish while hovering", "gHoverFishing");
                    Tooltip("Restore a bug from NTSC 1.0 that allows casting\nthe Fishing Rod while using the Hover Boots");
                    EnhancementCheckbox("N64 Weird Frames", "gN64WeirdFrames");
                    Tooltip("Restores N64 Weird Frames allowing weirdshots to behave the same as N64");
                    EnhancementCheckbox("Bombchus out of bounds", "gBombchusOOB");
                    Tooltip("Allows bombchus to explode out of bounds\nSimilar to GameCube and Wii VC");

                    ImGui::EndMenu();
                }

                EXPERIMENTAL();

                const char* fps_cvar = "gInterpolationFPS";
                {
                    int val = CVar_GetS32(fps_cvar, 20);
                    val = MAX(MIN(val, 250), 20);
                    int fps = val;

                    if (fps == 20)
                    {
                        ImGui::Text("Frame interpolation: Off");
                    }
                    else
                    {
                        ImGui::Text("Frame interpolation: %d FPS", fps);
                    }

                    if (ImGui::SliderInt("##FPSInterpolation", &val, 20, 250, "", ImGuiSliderFlags_AlwaysClamp))
                    {
                        CVar_SetS32(fps_cvar, val);
                        needs_save = true;
                    }

                    Tooltip("Interpolate extra frames to get smoother graphics\n"
                        "Set to match your monitor's refresh rate, or a divisor of it\n"
                        "A higher target FPS than your monitor's refresh rate will just waste resources,\n"
                        "and might give a worse result.\n"
                        "For consistent input lag, set this value and your monitor's refresh rate to a multiple of 20\n"
                        "Ctrl+Click for keyboard input");
                }
                if (impl.backend == Backend::DX11)
                {
                    if (ImGui::Button("Match Refresh Rate"))
                    {
                        int hz = roundf(gfx_get_detected_hz());
                        if (hz >= 20 && hz <= 250)
                        {
                            CVar_SetS32(fps_cvar, hz);
                            needs_save = true;
                        }
                    }
                }
                EnhancementCheckbox("Disable LOD", "gDisableLOD");
                Tooltip("Turns off the Level of Detail setting, making models use their higher-poly variants at any distance");
                EnhancementCheckbox("Disable Draw Distance", "gDisableDrawDistance");
                Tooltip("Turns off the objects draw distance,\nmaking objects being visible from a longer range");
                if (CVar_GetS32("gDisableDrawDistance", 0) == 0) {
                    CVar_SetS32("gDisableKokiriDrawDistance", 0);
                } else if (CVar_GetS32("gDisableDrawDistance", 0) == 1) {
                    EnhancementCheckbox("Kokiri Draw Distance", "gDisableKokiriDrawDistance");
                    Tooltip("The Kokiri are mystical beings that fade into view when approached\nEnabling this will remove their draw distance");
                }
                EnhancementCheckbox("Skip Text", "gSkipText");
                Tooltip("Holding down B skips text\nKnown to cause a cutscene softlock in Water Temple\nSoftlock can be fixed by pressing D-Right in Debug mode");

                EnhancementCheckbox("Free Camera", "gFreeCamera");

                ImGui::EndMenu();
            }

            ImGui::SetCursorPosY(0.0f);

            if (ImGui::BeginMenu("Cheats"))
            {
                if (ImGui::BeginMenu("Infinite...")) {
                    EnhancementCheckbox("Money", "gInfiniteMoney");
                    EnhancementCheckbox("Health", "gInfiniteHealth");
                    EnhancementCheckbox("Ammo", "gInfiniteAmmo");
                    EnhancementCheckbox("Magic", "gInfiniteMagic");
                    EnhancementCheckbox("Nayru's Love", "gInfiniteNayru");
                    EnhancementCheckbox("Epona Boost", "gInfiniteEpona");

                    ImGui::EndMenu();
                }

                EnhancementCheckbox("No Clip", "gNoClip");
                Tooltip("Allows you to walk through walls");
                EnhancementCheckbox("Climb Everything", "gClimbEverything");
                Tooltip("Makes every surface in the game climbable");
                EnhancementCheckbox("Moon Jump on L", "gMoonJumpOnL");
                Tooltip("Holding L makes you float into the air");
                EnhancementCheckbox("Super Tunic", "gSuperTunic");
                Tooltip("Makes every tunic have the effects of every other tunic");
                EnhancementCheckbox("Easy ISG", "gEzISG");
                Tooltip("Passive Infinite Sword Glitch\nIt makes your sword's swing effect and hitbox stay active indefinitely");
                EnhancementCheckbox("Unrestricted Items", "gNoRestrictItems");
                Tooltip("Allows you to use any item at any location");
                EnhancementCheckbox("Freeze Time", "gFreezeTime");
                Tooltip("Freezes the time of day");
                EnhancementCheckbox("Drops Don't Despawn", "gDropsDontDie");
                Tooltip("Drops from enemies, grass, etc. don't disappear after a set amount of time");
                EnhancementCheckbox("Fireproof Deku Shield", "gFireproofDekuShield");
                Tooltip("Prevents the Deku Shield from burning on contact with fire");
                EnhancementCheckbox("Shield with Two-Handed Weapons", "gShieldTwoHanded");
                Tooltip("This allows you to put up your shield with any two-handed weapon in hand\nexcept for Deku Sticks");

                ImGui::EndMenu();
            }

            ImGui::SetCursorPosY(0.0f);

            if (ImGui::BeginMenu("Developer Tools"))
            {
                EnhancementCheckbox("OoT Debug Mode", "gDebugEnabled");
                Tooltip("Enables Debug Mode, allowing you to select maps with L + R + Z, noclip with L + D-pad Right,\nand open the debug menu with L on the pause screen");
                EnhancementCheckbox("Fast File Select", "gSkipLogoTitle");
                Tooltip("Load the game to the selected slot below upon launch\nUse slot number 4 to load directly into the game's internal Map Select\n(Does not require the Debug Menu, but you will be unable to save there\nYou can also load the Map Select with OoT Debug Mode + slot 0)\nWith slot 0 you can directly go to the File Select menu\nAttention: loading an empty save file will result in a crash");
                if (CVar_GetS32("gSkipLogoTitle", 0)) {
                    EnhancementSliderInt("Loading %d", "##SaveFileID", "gSaveFileID", 0, 4, "");
                }
                ImGui::Separator();
                EnhancementCheckbox("Stats", "gStatsEnabled");
                Tooltip("Shows the stats window, with your FPS and frametimes,\nand the OS you're playing on");
                EnhancementCheckbox("Console", "gConsoleEnabled");
                Tooltip("Enables the console window, allowing you to input commands,\ntype help for some examples");
                console->opened = CVar_GetS32("gConsoleEnabled", 0);

                ImGui::EndMenu();
            }

            for (const auto& category : windowCategories) {
                ImGui::SetCursorPosY(0.0f);
                if (ImGui::BeginMenu(category.first.c_str())) {
                    for (const std::string& name : category.second) {
                        std::string varName(name);
                        varName.erase(std::remove_if(varName.begin(), varName.end(), [](unsigned char x) { return std::isspace(x); }), varName.end());
                        std::string toggleName = "g" + varName + "Enabled";

                        EnhancementCheckbox(name.c_str(), toggleName.c_str());
                        customWindows[name].enabled = CVar_GetS32(toggleName.c_str(), 0);
                    }
                    ImGui::EndMenu();
                }
            }

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
            const float framerate = ImGui::GetIO().Framerate;
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Debug Stats", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);

#ifdef _WIN32
            ImGui::Text("Platform: Windows");
#elif __APPLE__
            ImGui::Text("Platform: macOS");
#else
            ImGui::Text("Platform: Linux");
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
        std::string fb_str = SohUtils::getEnvironmentVar("framebuffer");
        if (!fb_str.empty()) {
            uintptr_t fbuf = (uintptr_t)std::stoull(fb_str);
            //ImGui::ImageSimple(reinterpret_cast<ImTextureID>(fbuf), pos, size);
            ImGui::SetCursorPos(pos);
            ImGui::Image(reinterpret_cast<ImTextureID>(fbuf), size);
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
        console->Commands[cmd] = std::move(entry);
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

        ImGui::Dummy(ImVec2(0.0f, 0.0f));

        ImGui::EndGroup();
    }
}
