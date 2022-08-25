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
#include <SDL_video.h>
#else
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_video.h>
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

#define BindButton(btn, status) ImGui::Image(GetTextureByID(DefaultAssets[btn]->textureId), ImVec2(16.0f * scale, 16.0f * scale), ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, (status) ? 255 : 0));

#define TOGGLE_BTN ImGuiKey_F1
#define TOGGLE_PAD_BTN ImGuiKey_GamepadBack
#define HOOK(b) if(b) needs_save = true;
OSContPad* pads;

std::map<std::string, GameAsset*> DefaultAssets;
std::vector<std::string> emptyArgs;

namespace SohImGui {

    WindowImpl impl;
    ImGuiIO* io;
    std::shared_ptr<Console> console = std::make_shared<Console>();
    GameOverlay* overlay = new GameOverlay;
    InputEditor* controller = new InputEditor;
    static ImVector<ImRect> s_GroupPanelLabelStack;

    std::function<void(void)> clientDrawMenu;
    std::function<void(void)> clientSetupHooks;


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

    std::map<std::string, std::vector<std::string>> hiddenwindowCategories;
    std::map<std::string, std::vector<std::string>> windowCategories;
    std::map<std::string, CustomWindow> customWindows;

    void InitSettings() {
        clientSetupHooks();
        Ship::RegisterHook<Ship::GfxInit>([] {
            gfx_get_current_rendering_api()->set_texture_filter((FilteringMode)CVar_GetS32("gTextureFilter", FILTER_THREE_POINT));
            if (CVar_GetS32("gConsoleEnabled", 0)) {
                console->Open();
            } else {
                console->Close();
            }

            if (CVar_GetS32("gControllerConfigurationEnabled", 0)) {
                controller->Open();
            } else {
                controller->Close();
            }
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
        case Backend::SDL:
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

    void RenderCross(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz)
    {
        float thickness = ImMax(sz / 5.0f, 1.0f);
        sz -= thickness * 0.5f;
        pos += ImVec2(thickness * 0.25f, thickness * 0.25f);

        draw_list->PathLineTo(ImVec2(pos.x, pos.y));
        draw_list->PathLineTo(ImVec2(pos.x + sz, pos.y + sz));
        draw_list->PathStroke(col, 0, thickness);

        draw_list->PathLineTo(ImVec2(pos.x + sz, pos.y));
        draw_list->PathLineTo(ImVec2(pos.x, pos.y + sz));
        draw_list->PathStroke(col, 0, thickness);
    }

    bool CustomCheckbox(const char* label, bool* v, bool disabled, ImGuiCheckboxGraphics disabledGraphic) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

        const float square_sz = ImGui::GetFrameHeight();
        const ImVec2 pos = window->DC.CursorPos;
        const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
        ImGui::ItemSize(total_bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(total_bb, id))
        {
            IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
            return false;
        }

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
        if (pressed)
        {
            *v = !(*v);
            ImGui::MarkItemEdited(id);
        }

        const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
        ImGui::RenderNavHighlight(total_bb, id);
        ImGui::RenderFrame(check_bb.Min, check_bb.Max, ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
        ImU32 check_col = ImGui::GetColorU32(ImGuiCol_CheckMark);
        ImU32 cross_col = ImGui::GetColorU32(ImVec4(0.50f, 0.50f, 0.50f, 1.00f));
        bool mixed_value = (g.LastItemData.InFlags & ImGuiItemFlags_MixedValue) != 0;
        if (mixed_value)
        {
            // Undocumented tristate/mixed/indeterminate checkbox (#2644)
            // This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue supported by all widgets (not just checkbox)
            ImVec2 pad(ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)), ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)));
            window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
        }
        else if ((!disabled && *v) || (disabled && disabledGraphic == ImGuiCheckboxGraphics::Checkmark))
        {
            const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
            ImGui::RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
        }
        else if (disabled && disabledGraphic == ImGuiCheckboxGraphics::Cross) {
            const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
            RenderCross(window->DrawList, check_bb.Min + ImVec2(pad, pad), cross_col, square_sz - pad * 2.0f);
        }

        ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
        if (g.LogEnabled)
            ImGui::LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");
        if (label_size.x > 0.0f)
            ImGui::RenderText(label_pos, label);

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return pressed;
    }

    void EnhancementCheckbox(const char* text, const char* cvarName, bool disabled, const char* disabledTooltipText, ImGuiCheckboxGraphics disabledGraphic)
    {
        if (disabled) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        bool val = (bool)CVar_GetS32(cvarName, 0);
        if (CustomCheckbox(text, &val, disabled, disabledGraphic)) {
            CVar_SetS32(cvarName, val);
            needs_save = true;
        }

        if (disabled) {
            ImGui::PopStyleVar(1);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && disabledTooltipText != "") {
                ImGui::SetTooltip("%s", disabledTooltipText);
            }
            ImGui::PopItemFlag();
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
        Color_RGBA8 NewColors = {0,0,0,255};
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
            NewColors.r = ClampFloatToInt(colors->x * 255, 0, 255);
            NewColors.g = ClampFloatToInt(colors->y * 255, 0, 255);
            NewColors.b = ClampFloatToInt(colors->z * 255, 0, 255);
            CVar_SetRGBA(cvarName, NewColors);
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
            colors->x = defaultcolors.x;
            colors->y = defaultcolors.y;
            colors->z = defaultcolors.z;
            if (has_alpha) { colors->w = defaultcolors.w; };

            Color_RGBA8 colorsRGBA;
            colorsRGBA.r = defaultcolors.x;
            colorsRGBA.g = defaultcolors.y;
            colorsRGBA.b = defaultcolors.z;
            if (has_alpha) { colorsRGBA.a = defaultcolors.w; };

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
            DispatchConsoleCommand("reset");
        }
        #else
        if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
             ImGui::IsKeyDown(ImGuiKey_RightCtrl)) &&
             ImGui::IsKeyPressed(ImGuiKey_R, false)) {
            DispatchConsoleCommand("reset");
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

            clientDrawMenu();

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

    void RegisterMenuDrawMethod(std::function<void(void)> drawMethod) {
        clientDrawMenu = drawMethod;
    }

    void AddSetupHooksDelegate(std::function<void(void)> setupHooksMethod) {
        clientSetupHooks = setupHooksMethod;
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

    void Render() {
        ImGui::Render();
        ImGuiRenderDrawData(ImGui::GetDrawData());
        if (UseViewports()) {
            if (impl.backend == Backend::SDL) {
                SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
                SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();

                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();

                SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
            } else {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
        }
    }

    void CancelFrame() {
        ImGui::EndFrame();
        if (UseViewports()) {
            ImGui::UpdatePlatformWindows();
        }
    }

    void DrawSettings() {
        overlay->DrawSettings();
    }

    Backend WindowBackend() {
        return impl.backend;
    }

    float WindowRefreshRate() {
        return gfx_get_detected_hz();
    }

    std::pair<const char*, const char*>* GetAvailableRenderingBackends() {
        return backends;
    }

    std::pair<const char*, const char*> GetCurrentRenderingBackend() {
        return backends[lastBackendID];
    }

    void SetCurrentRenderingBackend(uint8_t index, std::pair<const char*, const char*> backend) {
        Window::GetInstance()->GetConfig()->setString("Window.GfxBackend", backend.first);
        lastBackendID = index;
    }

    const char** GetSupportedTextureFilters() {
        return filters;
    }

    void SetResolutionMultiplier(float multiplier) {
        gfx_current_dimensions.internal_mul = multiplier;
    }

    void SetMSAALevel(uint32_t value) {
        gfx_msaa_level = value;
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

    void EnableWindow(const std::string& name, bool isEnabled) {
        customWindows[name].enabled = isEnabled;
    }

    void ToggleInputEditorWindow(bool isOpen) {
        if (isOpen)
            controller->Open();
        else
            controller->Close();
    }

    void ToggleStatisticsWindow(bool isOpen) {
        statsWindowOpen = isOpen;
    }

    void ToggleConsoleWindow(bool isOpen) {
        if (isOpen)
            console->Open();
        else
            console->Close();
    }

    void DispatchConsoleCommand(const std::string& line) {
        console->Dispatch(line);
    }

    void RequestCvarSaveOnNextTick() {
        needs_save = true;
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

    void PaddedEnhancementCheckbox(const char* text, const char* cvarName, bool padTop, bool padBottom, bool disabled, const char* disabledTooltipText, ImGuiCheckboxGraphics disabledGraphic) {
        if (padTop) {
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
        }
        EnhancementCheckbox(text, cvarName, disabled, disabledTooltipText, disabledGraphic);
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
