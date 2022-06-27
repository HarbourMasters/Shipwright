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

namespace SohImGui {

    WindowImpl impl;
    ImGuiIO* io;
    Console* console = new Console;
    GameOverlay* overlay = new GameOverlay;
    bool p_open = false;
    bool needs_save = false;
    ImVec4 hearts_colors;
    ImVec4 hearts_dd_colors;
    ImVec4 a_btn_colors;
    ImVec4 b_btn_colors;
    ImVec4 c_btn_colors;
    ImVec4 start_btn_colors;
    ImVec4 magic_border_colors;
    ImVec4 magic_remaining_colors;
    ImVec4 magic_use_colors;
    ImVec4 minimap_colors;
    ImVec4 rupee_colors;
    ImVec4 smolekey_colors;
    ImVec4 fileselect_colors;
    ImVec4 fileselect_text_colors;
    ImVec4 kokiri_col;
    ImVec4 goron_col;
    ImVec4 zora_col;
    ImVec4 navi_idle_i_col;
    ImVec4 navi_idle_o_col;
    ImVec4 navi_npc_i_col;
    ImVec4 navi_npc_o_col;
    ImVec4 navi_enemy_i_col;
    ImVec4 navi_enemy_o_col;
    ImVec4 navi_prop_i_col;
    ImVec4 navi_prop_o_col;

    const char* RainbowColorCvarList[] = {
        //This is the list of possible CVars that has rainbow effect.
            "gTunic_Kokiri_","gTunic_Goron_","gTunic_Zora_",
            "gCCHeartsPrim","gDDCCHeartsPrim",
            "gCCABtnPrim","gCCBBtnPrim","gCCCBtnPrim","gCCStartBtnPrim",
            "gCCMagicBorderPrim","gCCMagicPrim","gCCMagicUsePrim",
            "gCCMinimapPrim","gCCRupeePrim","gCCKeysPrim",
            "gCCFileChoosePrim", "gCCFileChooseTextPrim"
    };

    const char* filters[3] = {
        "Three-Point",
        "Linear",
        "None"
    };

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

    void LoadRainbowColor() {
        u8 arrayLength = sizeof(RainbowColorCvarList) / sizeof(*RainbowColorCvarList);
        for (u8 s = 0; s < arrayLength; s++) {
            std::string cvarName = RainbowColorCvarList[s];
            std::string Cvar_Red = cvarName;
            Cvar_Red += "R";
            std::string Cvar_Green = cvarName;
            Cvar_Green += "G";
            std::string Cvar_Blue = cvarName;
            Cvar_Blue += "B";
            std::string Cvar_RBM = cvarName;
            Cvar_RBM += "RBM";
            std::string RBM_HUE = cvarName;
            RBM_HUE += "Hue";
            f32 Canon = 10.f * s;
            ImVec4 NewColor;
            const f32 deltaTime = 1.0f / ImGui::GetIO().Framerate;
            f32 hue = CVar_GetFloat(RBM_HUE.c_str(), 0.0f);
            f32 newHue = hue + CVar_GetS32("gColorRainbowSpeed", 1) * 36.0f * deltaTime;
            if (newHue >= 360)
                newHue = 0;
            CVar_SetFloat(RBM_HUE.c_str(), newHue);
            f32 current_hue = CVar_GetFloat(RBM_HUE.c_str(), 0);
            u8 i = current_hue / 60 + 1;
            u8 a = (-current_hue / 60.0f + i) * 255;
            u8 b = (current_hue / 60.0f + (1 - i)) * 255;

            switch (i) {
            case 1: NewColor.x = 255; NewColor.y = b; NewColor.z = 0; break;
            case 2: NewColor.x = a; NewColor.y = 255; NewColor.z = 0; break;
            case 3: NewColor.x = 0; NewColor.y = 255; NewColor.z = b; break;
            case 4: NewColor.x = 0; NewColor.y = a; NewColor.z = 255; break;
            case 5: NewColor.x = b; NewColor.y = 0; NewColor.z = 255; break;
            case 6: NewColor.x = 255; NewColor.y = 0; NewColor.z = a; break;
            }

            if (CVar_GetS32(Cvar_RBM.c_str(), 0) != 0) {
                CVar_SetS32(Cvar_Red.c_str(), ClampFloatToInt(NewColor.x, 0, 255));
                CVar_SetS32(Cvar_Green.c_str(), ClampFloatToInt(NewColor.y, 0, 255));
                CVar_SetS32(Cvar_Blue.c_str(), ClampFloatToInt(NewColor.z, 0, 255));
            }
        }
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

        if (UseViewports()) {
            io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        }
        console->Init();
        overlay->Init();
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

        for (const auto& [i, controllers] : Ship::Window::Controllers)
        {
            CVar_SetFloat(StringHelper::Sprintf("gCont%i_GyroDriftX", i).c_str(), 0);
            CVar_SetFloat(StringHelper::Sprintf("gCont%i_GyroDriftY", i).c_str(), 0);
            needs_save = true;
        }

        ModInternal::RegisterHook<ModInternal::ControllerRead>([](OSContPad* cont_pad) {
            pads = cont_pad;
            });
        Game::InitSettings();
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

    void EnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format)
    {
        int val = CVar_GetS32(cvarName, 0);

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
        Tooltip("Clicking this button will make a random color for the colors at it's right.\nPrevious color will be overwrite and will not be recoverable");
    }

    void RainbowColor(const char* cvarName, ImVec4* colors) {
        std::string Cvar_RBM = cvarName;
        Cvar_RBM += "RBM";
        std::string MakeInvisible = "Rainbow";
        MakeInvisible += "##";
        MakeInvisible += cvarName;
        MakeInvisible += "Rainbow";

        EnhancementCheckbox(MakeInvisible.c_str(), Cvar_RBM.c_str());
        Tooltip("Clicking this button will make color cycling\nPrevious color will be overwrite and will not be recoverable");
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
        Tooltip("Clicking this button will to the game original color (GameCube version)\nPrevious color will be overwrite and will not be recoverable");
    }

    void EnhancementColor(const char* text, const char* cvarName, ImVec4 ColorRGBA, ImVec4 default_colors, bool allow_rainbow, bool has_alpha, bool TitleSameLine) {
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
        }
        else {
            if (ImGui::ColorEdit4(text, (float*)&ColorRGBA, flags)) {
                CVar_SetS32(Cvar_Red.c_str(), ClampFloatToInt(ColorRGBA.x * 255, 0, 255));
                CVar_SetS32(Cvar_Green.c_str(), ClampFloatToInt(ColorRGBA.y * 255, 0, 255));
                CVar_SetS32(Cvar_Blue.c_str(), ClampFloatToInt(ColorRGBA.z * 255, 0, 255));
                CVar_SetS32(Cvar_Alpha.c_str(), ClampFloatToInt(ColorRGBA.w * 255, 0, 255));
                needs_save = true;
            }

        }
        //ImGui::SameLine(); // Removing that one to gain some width spacing on the HUD editor
        ImGui::NewLine();
        ImGui::PushItemWidth(-FLT_MIN);
        ResetColor(cvarName, &ColorRGBA, default_colors, has_alpha);
        ImGui::SameLine();
        RandomizeColor(cvarName, &ColorRGBA);
        if (allow_rainbow) {
            
            if (ImGui::GetWindowSize().x > 560) {
                ImGui::SameLine();
            }
            else {
                ImGui::NewLine();
            }
            RainbowColor(cvarName, &ColorRGBA);
        }
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
            ImGuiWindowFlags_NoResize;
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

            ImGui::DockBuilderDockWindow("OoT Master Quest", dockId);

            ImGui::DockBuilderFinish(dockId);
        }

        ImGui::DockSpace(dockId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; 
        if ((ImGui::IsKeyPressed(TOGGLE_BTN)) || (ImGui::IsKeyDown(TOGGLE_PAD_BTN))) {
            bool menu_bar = CVar_GetS32("gOpenMenuBar", 0);
            CVar_SetS32("gOpenMenuBar", !menu_bar);
            needs_save = true;
            GlobalCtx2::GetInstance()->GetWindow()->dwMenubar = menu_bar;
            ShowCursor(menu_bar, Dialogues::dMenubar);
        }

        if (ImGui::BeginMenuBar()) {
            if (DefaultAssets.contains("Game_Icon")) {
                ImGui::SetCursorPos(ImVec2(5, 2.5f));
                ImGui::Image(GetTextureByID(DefaultAssets["Game_Icon"]->textureId), ImVec2(16.0f, 16.0f));
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(25, 0));
            }
            ImGui::Text("Shipwright");
            ImGui::Separator();

            if (ImGui::BeginMenu("Audio")) {
                EnhancementSliderFloat("Master Volume: %d %%", "##Master_Vol", "gGameMasterVolume", 0.0f, 1.0f, "", 1.0f, true);

                BindAudioSlider("Main Music Volume: %d %%", "gMainMusicVolume", 1.0f, SEQ_BGM_MAIN);
                BindAudioSlider("Sub Music Volume: %d %%", "gSubMusicVolume", 1.0f, SEQ_BGM_SUB);
                BindAudioSlider("Sound Effects Volume: %d %%", "gSFXMusicVolume", 1.0f, SEQ_SFX);
                BindAudioSlider("Fanfare Volume: %d %%", "gFanfareVolume", 1.0f, SEQ_FANFARE);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Controller"))
	    {
                // TODO mutual exclusions -- gDpadEquips and gDpadPauseName cause conflicts, but nothing stops a user from selecting both
                // There should be some system to prevent conclifting enhancements from being selected
                EnhancementCheckbox("D-pad Support on Pause and File Select", "gDpadPauseName");
                EnhancementCheckbox("D-pad Support in Ocarina and Text Choice", "gDpadOcarinaText");
                EnhancementCheckbox("D-pad Support for Browsing Shop Items", "gDpadShop");
                EnhancementCheckbox("D-pad as Equip Items", "gDpadEquips");

		ImGui::Separator();

                EnhancementCheckbox("Show Inputs", "gInputEnabled");
                Tooltip("Shows currently pressed inputs on the bottom right of the screen");
                EnhancementCheckbox("Rumble Enabled", "gRumbleEnabled");

                EnhancementSliderFloat("Input Scale: %.1f", "##Input", "gInputScale", 1.0f, 3.0f, "", 1.0f, false);
                Tooltip("Sets the on screen size of the displayed inputs from Show Inputs");  

		ImGui::Separator();  
		    
                for (const auto& [i, controllers] : Ship::Window::Controllers)
                {
                    bool hasPad = std::find_if(controllers.begin(), controllers.end(), [](const auto& c) {
                        return c->HasPadConf() && c->Connected();
                        }) != controllers.end();

                        if (!hasPad) continue;

                        auto menuLabel = "Controller " + std::to_string(i + 1);
                        if (ImGui::BeginMenu(menuLabel.c_str()))
                        {
                            EnhancementSliderFloat("Gyro Sensitivity: %d %%", "##GYROSCOPE", StringHelper::Sprintf("gCont%i_GyroSensitivity", i).c_str(), 0.0f, 1.0f, "", 1.0f, true);

                            if (ImGui::Button("Recalibrate Gyro"))
                            {
                                CVar_SetFloat(StringHelper::Sprintf("gCont%i_GyroDriftX", i).c_str(), 0);
                                CVar_SetFloat(StringHelper::Sprintf("gCont%i_GyroDriftY", i).c_str(), 0);
                                needs_save = true;
                            }

                            ImGui::Separator();

                            EnhancementSliderFloat("Rumble Strength: %d %%", "##RUMBLE", StringHelper::Sprintf("gCont%i_RumbleStrength", i).c_str(), 0.0f, 1.0f, "", 1.0f, true);

                            ImGui::EndMenu();
                        }
                        ImGui::Separator();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Graphics"))
            {
                EnhancementSliderInt("Internal Resolution: %dx", "##IMul", "gInternalResolution", 1, 8, "");
                Tooltip("Increases the render resolution of the game, up to 8x your output resolution,\nas a more intensive but effective form of anti-aliasing");
                gfx_current_dimensions.internal_mul = CVar_GetS32("gInternalResolution", 1);
                EnhancementSliderInt("MSAA: %d", "##IMSAA", "gMSAAValue", 1, 8, "");
                Tooltip("Activates anti-aliasing when above 1, up to 8x for 8 samples for every pixel");
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
                        "add one frame of input lag (e.g. 16.6 ms for 60 FPS) in order to avoid jitter.\n"
                        "This setting allows the CPU to work on one frame while GPU works on the previous frame.\n"
                        "This setting should be used when your computer is too slow to do CPU + GPU work in time.");
                }

                EXPERIMENTAL();
                ImGui::Text("Texture Filter (Needs reload)");
                EnhancementCombobox("gTextureFilter", filters, 3, 0);
                GfxRenderingAPI* gapi = gfx_get_current_rendering_api();
                gapi->set_texture_filter((FilteringMode)CVar_GetS32("gTextureFilter", 0));
                overlay->DrawSettings();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Languages")) {
                EnhancementRadioButton("English", "gLanguages", 0);
                EnhancementRadioButton("German", "gLanguages", 1);
                EnhancementRadioButton("French", "gLanguages", 2);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Enhancements"))
            {
                if (ImGui::BeginMenu("Gameplay"))
                {
                    EnhancementSliderInt("Text Speed: %dx", "##TEXTSPEED", "gTextSpeed", 1, 5, "");
                    EnhancementSliderInt("King Zora Speed: %dx", "##WEEPSPEED", "gMweepSpeed", 1, 5, "");
                    EnhancementSliderInt("Biggoron Forge Time: %d days", "##FORGETIME", "gForgeTime", 0, 3, "");
                    Tooltip("Allows you to change the number of days it takes for Biggoron to forge the Biggoron Sword");
                    EnhancementSliderInt("Vine/Ladder Climb speed +%d", "##CLIMBSPEED", "gClimbSpeed", 0, 12, "");
                    EnhancementSliderInt("Damage Multiplier %dx", "##DAMAGEMUL", "gDamageMul", 1, 4, "");
                    Tooltip("Modifies all sources of damage not affected by other sliders");
                    EnhancementSliderInt("Fall Damage Multiplier %dx", "##FALLDAMAGEMUL", "gFallDamageMul", 1, 4, "");
                    Tooltip("Modifies all fall damage");
                    EnhancementSliderInt("Void Damage Multiplier %dx", "##VOIDDAMAGEMUL", "gVoidDamageMul", 1, 4, "");
                    Tooltip("Modifies all void out damage");

                    EnhancementCheckbox("Skip Text", "gSkipText");
                    Tooltip("Holding down B skips text");
                    EnhancementCheckbox("Mute Low HP Alarm", "gLowHpAlarm");
                    Tooltip("Disable the low HP beeping sound");
                    EnhancementCheckbox("Minimal UI", "gMinimalUI");
                    Tooltip("Hides most of the UI when not needed");
                    EnhancementCheckbox("Visual Stone of Agony", "gVisualAgony");
                    Tooltip("Displays an icon and plays a sound when Stone of Agony should be activated, for those without rumble");
                    EnhancementCheckbox("Faster Block Push", "gFasterBlockPush");
                    EnhancementCheckbox("Assignable Tunics and Boots", "gAssignableTunicsAndBoots");
                    Tooltip("Allows equipping the tunic and boots to c-buttons");
                    EnhancementCheckbox("MM Bunny Hood", "gMMBunnyHood");
                    Tooltip("Wearing the Bunny Hood grants a speed increase like in Majora's Mask");
                    EnhancementCheckbox("No Forced Navi", "gNoForcedNavi");
                    Tooltip("Prevent forced Navi conversations");
                    EnhancementCheckbox("No Skulltula Freeze", "gSkulltulaFreeze");
                    Tooltip("Stops the game from freezing the player when picking up Gold Skulltulas");
                    EnhancementCheckbox("Disable Navi Call Audio", "gDisableNaviCallAudio");
                    Tooltip("Disables the voice audio when Navi calls you");
                    EnhancementCheckbox("Fast Chests", "gFastChests");
                    Tooltip("Kick open every chest");
                    EnhancementCheckbox("Fast Drops", "gFastDrops");
                    Tooltip("Skip first-time pickup messages for consumable items");
                    EnhancementCheckbox("Better Owl", "gBetterOwl");
                    Tooltip("The default response to Kaepora Gaebora is always that you understood what he said");
                    EnhancementCheckbox("Link's Cow in Both Time Periods", "gCowOfTime");
                    Tooltip("Allows the Lon Lon Ranch obstacle course reward to be shared across time periods");
                    EnhancementCheckbox("Enable visible guard vision", "gGuardVision");
                    EnhancementCheckbox("Enable passage of time on file select", "gTimeFlowFileSelect");
                    EnhancementCheckbox("Allow the cursor to be on any slot", "gPauseAnyCursor");
                    Tooltip("Allows the cursor on the pause menu to be over any slot. Similar to Rando and Spaceworld 97");
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
                    Tooltip("Change most 2D items & projectiles to their a 3D version");
                    EnhancementCheckbox("Disable Black Bar Letterboxes", "gDisableBlackBars");
                    Tooltip("Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be minor visual glitches that were covered up by the black bars\nPlease disable this setting before reporting a bug");
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
                    Tooltip("Enabling it make L and R be your page switch like on Gamecube\nZ become the button to open Debug Menu");
                    EnhancementCheckbox("Fix Dungeon entrances", "gFixDungeonMinimapIcon");
                    Tooltip("Show dungeon entrances icon only when it should be");
                    EnhancementCheckbox("Fix Two Handed idle animations", "gTwoHandedIdle");
                    Tooltip("Makes two handed idle animation play, a seemingly finished animation that was disabled on accident in the original game");
                    EnhancementCheckbox("Fix the Gravedigging Tour Glitch", "gGravediggingTourFix");
                    Tooltip("Fixes a bug where you can permanently miss the Gravedigging Tour Heart Piece");
                    EnhancementCheckbox("Fix Deku Nut upgrade", "gDekuNutUpgradeFix");
                    Tooltip("Prevents the Forest Stage Deku Nut upgrade from becoming unobtainable after receiving the Poacher's Saw");
                    EnhancementCheckbox("Fix Navi text HUD position", "gNaviTextFix");
                    Tooltip("Correctly centers the Navi text prompt on the HUD's C-Up button");
                    EnhancementCheckbox("Fix Anubis fireballs", "gAnubisFix");
                    Tooltip("Make Anubis fireballs do fire damage when reflected back at them with the Mirror Shield");

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Restoration"))
                {
                    EnhancementCheckbox("Red Ganon blood", "gRedGanonBlood");
                    Tooltip("Restore the original red blood from NTSC 1.0/1.1. Disable for green blood");
                    EnhancementCheckbox("Fish while hovering", "gHoverFishing");
                    Tooltip("Restore a bug from NTSC 1.0 that allows casting the Fishing Rod while using the Hover Boots");

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

                    Tooltip("Interpolate extra frames to get smoother graphics.\n"
                        "Set to match your monitor's refresh rate, or a divisor of it.\n"
                        "A higher target FPS than your monitor's refresh rate will just waste resources,\n"
                        "and might give a worse result.\n"
                        "For consistent input lag, set this value and your monitor's refresh rate to a multiple of 20.\n"
                        "Ctrl+Click for keyboard input.");
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
                Tooltip("Turns off the level of detail setting, making models always use their higher poly variants");
                EnhancementCheckbox("Disable Draw Distance", "gDisableDrawDistance");
                Tooltip("Turns off the objects draw distance, making objects being visible from a longer range");
                if (CVar_GetS32("gDisableDrawDistance", 0) == 0) {
                    CVar_SetS32("gDisableKokiriDrawDistance", 0);
                } else if (CVar_GetS32("gDisableDrawDistance", 0) == 1) {
                    EnhancementCheckbox("Kokiri Draw Distance", "gDisableKokiriDrawDistance");
                    Tooltip("Kokiris are mystical being that appear from a certain distance\nEnable this will remove their draw distance");
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Cosmetics"))  {
                EnhancementCheckbox("Cosmetics editor", "gCosmeticEditor");
                Tooltip("Edit Navi and Link's Tunics color.");
                EnhancementCheckbox("HUD Margins editor", "gUseMargins");
                EnhancementRadioButton("N64 interface", "gHudColors", 0);
                Tooltip("Change interface color to N64 style.");
                EnhancementRadioButton("GameCube interface", "gHudColors", 1);
                Tooltip("Change interface color to GameCube style.");
                EnhancementRadioButton("Custom interface", "gHudColors", 2);
                Tooltip("Change interface color to your own made style.");
                if (CVar_GetS32("gHudColors", 1) == 2) {
                    EnhancementCheckbox("Interface editor", "gColorsEditor");
                    Tooltip("Edit the colors used for your own interface");
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Cheats"))
            {
                if (ImGui::BeginMenu("Infinite...")) {
                    EnhancementCheckbox("Money", "gInfiniteMoney");
                    EnhancementCheckbox("Health", "gInfiniteHealth");
                    EnhancementCheckbox("Ammo", "gInfiniteAmmo");
                    EnhancementCheckbox("Magic", "gInfiniteMagic");
                    EnhancementCheckbox("Nayru's Love", "gInfiniteNayru");

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
                Tooltip("Automatically activates the Infinite Sword glitch, making you constantly swing your sword");
                EnhancementCheckbox("Unrestricted Items", "gNoRestrictItems");
                Tooltip("Allows you to use any item at any location");
                EnhancementCheckbox("Freeze Time", "gFreezeTime");
                Tooltip("Freezes the time of day");
                EnhancementCheckbox("Drops Don't Despawn", "gDropsDontDie");
                Tooltip("Drops from enemies, grass, etc. don't disappear after a set amount of time");
                EnhancementCheckbox("Fireproof Deku Shield", "gFireproofDekuShield");
                Tooltip("Prevents the Deku Shield from burning on contact with fire");
                EnhancementCheckbox("Shield with Two-Handed Weapons", "gShieldTwoHanded");
                Tooltip("Allows Link to shield normally with two-handed swords and the Megaton Hammer");

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Developer Tools"))
            {
                EnhancementCheckbox("OoT Debug Mode", "gDebugEnabled");
                Tooltip("Enables Debug Mode, allowing you to select maps with L + R + Z, noclip with L + D-pad Right,\nand open the debug menu with L on the pause screen");
                EnhancementCheckbox("Fast File Select", "gSkipLogoTitle");
                Tooltip("Directly load the game to selected slot bellow\nUse slot number 4 to load directly in Zelda Map Select\n(Do not require debug menu but you will be unable to save there)\n(you can also load Zelda map select with Debug mod + slot 0).\nWith Slot : 0 you can go directly in File Select menu\nAttention, Loading an empty save will result in crash");
                if (CVar_GetS32("gSkipLogoTitle", 0)) {
                    EnhancementSliderInt("Loading %d", "##SaveFileID", "gSaveFileID", 0, 4, "");
                }
                ImGui::Separator();
                EnhancementCheckbox("Stats", "gStatsEnabled");
                Tooltip("Shows the stats window, with your FPS and frametimes, and the OS you're playing on");
                EnhancementCheckbox("Console", "gConsoleEnabled");
                Tooltip("Enables the console window, allowing you to input commands, type help for some examples");
                console->opened = CVar_GetS32("gConsoleEnabled", 0);

                ImGui::EndMenu();
            }

            bool Margins_isOpen = CVar_GetS32("gUseMargins", 0);
            bool Cosmetics_isOpen = CVar_GetS32("gCosmeticEditor", 0);
            bool Interface_isOpen = CVar_GetS32("gColorsEditor", 0);

            if (Margins_isOpen) {
                if (!Margins_isOpen) {
                    CVar_SetS32("gHUDMargins", 0);
                    return;
                }
                ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
                ImGui::Begin("Margins Editor", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
                if (ImGui::BeginTabBar("Margins Editor", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
                    if (ImGui::BeginTabItem("Interface margins")) {
                        EnhancementCheckbox("Use margins", "gHUDMargins");
                        Tooltip("Enable/Disable custom margins. \nIf disabled you will have original margins");
                        EnhancementSliderInt("Top : %dx", "##UIMARGINT", "gHUDMargin_T", -20, 20, "");
                        EnhancementSliderInt("Left: %dx", "##UIMARGINL", "gHUDMargin_L", -25, 25, "");
                        EnhancementSliderInt("Right: %dx", "##UIMARGINR", "gHUDMargin_R", -25, 25, "");
                        EnhancementSliderInt("Bottom: %dx", "##UIMARGINB", "gHUDMargin_B", -20, 20, "");
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::End();
            }
            if (Cosmetics_isOpen) {
                if (!Cosmetics_isOpen) {
                    CVar_SetS32("gCosmeticEditor", 0);
                    return;
                }
                ImGui::SetNextWindowSize(ImVec2(500, 627), ImGuiCond_FirstUseEver);
                ImGui::Begin("Cosmetics Editor", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
                if (ImGui::BeginTabBar("Cosmetics Editor", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
                    if (ImGui::BeginTabItem("Navi")) {
                        EnhancementCheckbox("Custom colors for Navi", "gUseNaviCol");
                        Tooltip("Enable/Disable custom Navi's colors. \nIf disabled you will have original colors for Navi.\nColors are refreshed when Navi goes back in your pockets.");
                        EnhancementColor("Navi Idle Inner", "gNavi_Idle_Inner_", navi_idle_i_col, ImVec4(255, 255, 255, 255), false);
                        Tooltip("Inner color for Navi (idle flying around)");
                        EnhancementColor("Navi Idle Outer", "gNavi_Idle_Outer_", navi_idle_o_col, ImVec4(0, 0, 255, 255), false);
                        Tooltip("Outer color for Navi (idle flying around)");
                        ImGui::Separator();
                        EnhancementColor("Navi NPC Inner", "gNavi_NPC_Inner_", navi_npc_i_col, ImVec4(150, 150, 255, 255), false);
                        Tooltip("Inner color for Navi (when Navi fly around NPCs)");
                        EnhancementColor("Navi NPC Outer", "gNavi_NPC_Outer_", navi_npc_o_col, ImVec4(150, 150, 255, 255), false);
                        Tooltip("Outer color for Navi (when Navi fly around NPCs)");
                        ImGui::Separator();
                        EnhancementColor("Navi Enemy Inner", "gNavi_Enemy_Inner_", navi_enemy_i_col, ImVec4(255, 255, 0, 255), false);
                        Tooltip("Inner color for Navi (when Navi fly around Enemies or Bosses)");
                        EnhancementColor("Navi Enemy Outer", "gNavi_Enemy_Outer_", navi_enemy_o_col, ImVec4(220, 155, 0, 255), false);
                        Tooltip("Outer color for Navi (when Navi fly around Enemies or Bosses)");
                        ImGui::Separator();
                        EnhancementColor("Navi Prop Inner", "gNavi_Prop_Inner_", navi_prop_i_col, ImVec4(0, 255, 0, 255), false);
                        Tooltip("Inner color for Navi (when Navi fly around props (signs etc))");
                        EnhancementColor("Navi Prop Outer", "gNavi_Prop_Outer_", navi_prop_o_col, ImVec4(0, 255, 0, 255), false);
                        Tooltip("Outer color for Navi (when Navi fly around props (signs etc))");
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Tunics")) {
                        EnhancementCheckbox("Custom colors on tunics", "gUseTunicsCol");
                        Tooltip("Enable/Disable custom Link's tunics colors. \nIf disabled you will have original colors for Link's tunics.");
                        EnhancementColor("Kokiri Tunic", "gTunic_Kokiri_", kokiri_col, ImVec4(30, 105, 27, 255));
                        ImGui::Separator();
                        EnhancementColor("Goron Tunic", "gTunic_Goron_", goron_col, ImVec4(100, 20, 0, 255));
                        ImGui::Separator();
                        EnhancementColor("Zora Tunic", "gTunic_Zora_", zora_col, ImVec4(0, 60, 100, 255));
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::End();
            }
            if (Interface_isOpen) {
                if (!Interface_isOpen) {
                    CVar_SetS32("gColorsEditor", 0);
                    return;
                }
                ImGui::SetNextWindowSize(ImVec2(215, 627), ImGuiCond_FirstUseEver);
                ImGui::Begin("Interface Editor", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
                if (ImGui::BeginTabBar("Interface Editor", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
                    if (ImGui::BeginTabItem("HUD")) {
                        if (ImGui::BeginTable("tableInterfaceEditor", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                            ImGui::TableSetupColumn("Hearts", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                            ImGui::TableSetupColumn("Magic Bar", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                            ImGui::TableSetupColumn("Buttons", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                            ImGui::TableHeadersRow();
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            // COLUMN 1.1 - HEARTS
                            ImGui::PushItemWidth(-FLT_MIN);
                            EnhancementColor("Hearts inner", "gCCHeartsPrim", hearts_colors, ImVec4(255, 70, 50, 255));
                            Tooltip("Hearts inner color (red in original)\nAffect both Normal Hearts and the ones in Double Defense");
                            ImGui::Separator();
                            EnhancementColor("Hearts double def", "gDDCCHeartsPrim", hearts_dd_colors, ImVec4(255, 255, 255, 255));
                            Tooltip("Hearts outline color (white in original)\nAffect Double Defense outline only.");
                            ImGui::PopItemWidth();
                            ImGui::Separator();
                            if (ImGui::BeginTable("tableMisc", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                                ImGui::TableSetupColumn("Misc", ImGuiTableColumnFlags_WidthStretch, 600.0f);
                                ImGui::TableHeadersRow();
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                // COLUMN 1.2 - MISC
                                ImGui::PushItemWidth(-FLT_MIN);
                                EnhancementColor("Minimap color", "gCCMinimapPrim", minimap_colors, ImVec4(0, 255, 255, 255));
                                Tooltip("Affect the Dungeon and Overworld minimaps.");
                                ImGui::Separator();
                                EnhancementColor("Rupee icon color", "gCCRupeePrim", rupee_colors, ImVec4(120, 120, 120, 255));
                                Tooltip("Affect the Rupee icon on interface\nGreen by default.");
                                ImGui::Separator();
                                EnhancementColor("Small Keys icon color", "gCCKeysPrim", smolekey_colors, ImVec4(200, 230, 255, 255));
                                Tooltip("Affect the Small keys icon on interface\nGray by default.");
                                ImGui::PopItemWidth();
                                ImGui::EndTable();
                            }
                            ImGui::TableNextColumn();
                            // COLUMN 2 - MAGIC BAR
                            ImGui::PushItemWidth(-FLT_MIN);
                            EnhancementColor("Magic bar borders", "gCCMagicBorderPrim", magic_border_colors, ImVec4(255, 255, 255, 255));
                            Tooltip("Affect the border of the magic bar when being used\nWhite flash in original game.");
                            ImGui::Separator();
                            EnhancementColor("Magic bar main color", "gCCMagicPrim", magic_remaining_colors, ImVec4(0, 200, 0, 255));
                            Tooltip("Affect the magic bar color\nGreen in original game.");
                            ImGui::Separator();
                            EnhancementColor("Magic bar being used", "gCCMagicUsePrim", magic_use_colors, ImVec4(250, 250, 0, 255));
                            Tooltip("Affect the magic bar when being used\nYellow in original game.");
                            ImGui::Separator();
                            ImGui::PopItemWidth();
                            ImGui::TableNextColumn();
                            // COLUMN 3 - BUTTON
                            ImGui::PushItemWidth(-FLT_MIN);
                            EnhancementColor("A Buttons", "gCCABtnPrim", a_btn_colors, ImVec4(90, 90, 255, 255));
                            Tooltip("A Buttons colors (Green in original GameCube)\nAffect A buttons colors on interface, in shops, messages boxes, ocarina notes and inventory cursors.");
                            ImGui::Separator();
                            EnhancementColor("B Buttons", "gCCBBtnPrim", b_btn_colors, ImVec4(0, 150, 0, 255));
                            Tooltip("B Button colors (Red in original GameCube)\nAffect B button colors on interface");
                            ImGui::Separator();
                            EnhancementColor("C Buttons", "gCCCBtnPrim", c_btn_colors, ImVec4(255, 160, 0, 255));
                            Tooltip("C Buttons colors (Yellowish / Oranges in originals)\nAffect C buttons colors on interface, in inventory and ocarina notes");
                            ImGui::Separator();
                            EnhancementColor("Start Buttons", "gCCStartBtnPrim", start_btn_colors, ImVec4(120, 120, 120, 255));
                            Tooltip("Start Button colors (gray in GameCube)\nAffect Start button colors in inventory");
                            ImGui::Separator();
                            ImGui::PopItemWidth();
                            ImGui::EndTable();
                            ImGui::EndTabItem();
                        }
                    }

                    if (ImGui::BeginTabItem("File Choose")) {
                        EnhancementColor("File Choose color", "gCCFileChoosePrim", fileselect_colors, ImVec4(100, 150, 255, 255));
                        Tooltip("Affect the File Select.");
                        ImGui::Separator();
                        EnhancementColor("Bottom text color", "gCCFileChooseTextPrim", fileselect_text_colors, ImVec4(0, 100, 255, 255));
                        Tooltip("Affect the File Select.");
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::End();
            }

            for (const auto& category : windowCategories) {
                if (ImGui::BeginMenu(category.first.c_str())) {
                    for (const std::string& name : category.second) {
                        std::string varName(name);
                        varName.erase(std::ranges::remove_if(varName, isspace).begin(), varName.end());
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
        ImGui::Begin("OoT Master Quest", nullptr, flags);
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
        //Placed here so it does the rainbow effects even if menu is not on.
        LoadRainbowColor();
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

    void AddWindow(const std::string& category, const std::string& name, WindowDrawFunc drawFunc) {
        if (customWindows.contains(name)) {
            SPDLOG_ERROR("SohImGui::AddWindow: Attempting to add duplicate window name %s", name.c_str());
            return;
        }

        customWindows[name] = {
            .enabled = false,
            .drawFunc = drawFunc
        };

        windowCategories[category].emplace_back(name);
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
}
