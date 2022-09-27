#include "ImGuiImpl.h"

#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <algorithm>
#include <vector>

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

    bool needs_save = false;
    int lastRenderingBackendID = 0;
    int lastAudioBackendID = 0;
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

    std::vector<std::pair<const char*, const char*>> renderingBackends = {
#ifdef _WIN32
        { "dx11", "DirectX" },
#endif
#ifndef __WIIU__
        { "sdl", "OpenGL" }
#else
        { "wiiu", "GX2" }
#endif
    };

    std::vector<std::pair<const char*, const char*>> audioBackends = {
#ifdef _WIN32
        { "wasapi", "Windows Audio Session API" },
#endif
#if defined(__linux)
        { "pulse", "PulseAudio" },
#endif
        { "sdl", "SDL Audio" }
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

    void PopulateBackendIds(std::shared_ptr<Mercury> cfg) {
        std::string renderingBackend = cfg->getString("Window.GfxBackend");
        if (renderingBackend.empty()) {
            lastRenderingBackendID = 0;
        } else {
            for (size_t i = 0; i < renderingBackends.size(); i++) {
                if(renderingBackend == renderingBackends[i].first) {
                    lastRenderingBackendID = i;
                    break;
                }
            }
        }

        std::string audioBackend = cfg->getString("Window.AudioBackend");
        if (audioBackend.empty()) {
            lastAudioBackendID = 0;
        } else {
            for (size_t i = 0; i < audioBackends.size(); i++) {
                if(audioBackend == audioBackends[i].first) {
                    lastAudioBackendID = i;
                    break;
                }
            }
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

    // MARK: - Public API

    void Init(WindowImpl window_impl) {
        CVar_Load();
        impl = window_impl;
        ImGuiContext* ctx = ImGui::CreateContext();
        ImGui::SetCurrentContext(ctx);
        io = &ImGui::GetIO();
        io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io->Fonts->AddFontDefault();
        statsWindowOpen = CVar_GetS32("gStatsEnabled", 0);
        CVar_RegisterS32("gRandomizeRupeeNames", 1);
        CVar_RegisterS32("gRandoRelevantNavi", 1);
        CVar_RegisterS32("gRandoMatchKeyColors", 1);
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

        PopulateBackendIds(Window::GetInstance()->GetConfig());

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
            io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
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
                io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
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

    std::vector<std::pair<const char*, const char*>> GetAvailableRenderingBackends() {
        return renderingBackends;
    }

    std::vector<std::pair<const char*, const char*>> GetAvailableAudioBackends() {
        return audioBackends;
    }

    std::pair<const char*, const char*> GetCurrentRenderingBackend() {
        return renderingBackends[lastRenderingBackendID];
    }

    std::pair<const char*, const char*> GetCurrentAudioBackend() {
        return audioBackends[lastAudioBackendID];
    }

    void SetCurrentRenderingBackend(uint8_t index, std::pair<const char*, const char*> backend) {
        Window::GetInstance()->GetConfig()->setString("Window.GfxBackend", backend.first);
        lastRenderingBackendID = index;
    }

    void SetCurrentAudioBackend(uint8_t index, std::pair<const char*, const char*> backend) {
        Window::GetInstance()->GetConfig()->setString("Window.AudioBackend", backend.first);
        lastAudioBackendID = index;
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

    Ship::GameOverlay* GetGameOverlay() {
        return overlay;
    }

    Ship::InputEditor* GetInputEditor() {
        return controller;
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

    std::shared_ptr<Ship::Console> GetConsole() {
        return console;
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

    void BeginGroupPanel(const char* name, const ImVec2& size) {
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
