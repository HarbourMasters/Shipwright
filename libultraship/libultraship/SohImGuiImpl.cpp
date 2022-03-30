#include "SohImGuiImpl.h"

#include <iostream>
#include <utility>

#include "Archive.h"
#include "Environment.h"
#include "GameSettings.h"
#include "SohConsole.h"
#include "SohHooks.h"
#include "Lib/ImGui/imgui_internal.h"
#include "GlobalCtx2.h"
#include "ResourceMgr.h"
#include "TextureMod.h"
#include "Window.h"
#include "Cvar.h"
#include "../Fast3D/gfx_pc.h"
#include "Lib/stb/stb_image.h"
#include "Lib/Fast3D/gfx_rendering_api.h"
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

#define TOGGLE_BTN ImGuiKey_F1
#define HOOK(b) if(b) needs_save = true;
OSContPad* pads;

std::map<std::string, GameAsset*> DefaultAssets;

namespace SohImGui {

    WindowImpl impl;
    ImGuiIO* io;
    Console* console = new Console;
    bool p_open = false;
    bool needs_save = false;

    void ImGuiWMInit() {
        switch (impl.backend) {
        case Backend::SDL:
            ImGui_ImplSDL2_InitForOpenGL(static_cast<SDL_Window*>(impl.sdl.window), impl.sdl.context);
            break;
        case Backend::DX11:
            ImGui_ImplWin32_Init(impl.dx11.window);
            break;
        }
    }

    void ImGuiBackendInit() {
        switch (impl.backend) {
        case Backend::SDL:
            ImGui_ImplOpenGL3_Init("#version 120");
            break;
        case Backend::DX11:
            ImGui_ImplDX11_Init(static_cast<ID3D11Device*>(impl.dx11.device), static_cast<ID3D11DeviceContext*>(impl.dx11.device_context));
            break;
        }
    }

    void ImGuiProcessEvent(EventImpl event) {
        switch (impl.backend) {
        case Backend::SDL:
            ImGui_ImplSDL2_ProcessEvent(static_cast<const SDL_Event*>(event.sdl.event));
            break;
        case Backend::DX11:
            ImGui_ImplWin32_WndProcHandler(static_cast<HWND>(event.win32.handle), event.win32.msg, event.win32.wparam, event.win32.lparam);
            break;
        }
    }

    void ImGuiWMNewFrame() {
        switch (impl.backend) {
        case Backend::SDL:
            ImGui_ImplSDL2_NewFrame(static_cast<SDL_Window*>(impl.sdl.window));
            break;
        case Backend::DX11:
            ImGui_ImplWin32_NewFrame();
            break;
        }
    }

    void ImGuiBackendNewFrame() {
        switch (impl.backend) {
        case Backend::SDL:
            ImGui_ImplOpenGL3_NewFrame();
            break;
        case Backend::DX11:
            ImGui_ImplDX11_NewFrame();
            break;
        }
    }

    void ImGuiRenderDrawData(ImDrawData* data) {
        switch (impl.backend) {
        case Backend::SDL:
            ImGui_ImplOpenGL3_RenderDrawData(data);
            break;
        case Backend::DX11:
            ImGui_ImplDX11_RenderDrawData(data);
            break;
        }
    }

    bool UseInternalRes() {
        switch (impl.backend) {
        case Backend::SDL:
            return true;
        }
        return false;
    }

    bool UseViewports() {
        switch (impl.backend) {
        case Backend::DX11:
            return true;
        }
        return false;
    }

    void SohImGui::ShowCursor(bool hide, Dialogues d) {
        if (d == Dialogues::dLoadSettings) {
            GlobalCtx2::GetInstance()->GetWindow()->ShowCursor(hide);
            return;
        }

        if (d == Dialogues::dConsole && Game::Settings.debug.menu_bar) {
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

    void LoadTexture(std::string name, std::string path) {
        GfxRenderingAPI* api = gfx_get_current_rendering_api();
        const auto res = GlobalCtx2::GetInstance()->GetResourceManager()->LoadFile(normalize(path));

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

    void Init(WindowImpl window_impl) {
        impl = window_impl;
        Game::LoadSettings();
        ImGuiContext* ctx = ImGui::CreateContext();
        ImGui::SetCurrentContext(ctx);
        io = &ImGui::GetIO();
        io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        if (UseViewports()) {
            io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        }
        console->Init();
        ImGuiWMInit();
        ImGuiBackendInit();

        ModInternal::registerHookListener({ GFX_INIT, [](const HookEvent ev) {

            if (GlobalCtx2::GetInstance()->GetWindow()->IsFullscreen())
                ShowCursor(Game::Settings.debug.menu_bar, Dialogues::dLoadSettings);

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
        } });

        ModInternal::registerHookListener({ CONTROLLER_READ, [](const HookEvent ev) {
            pads = static_cast<OSContPad*>(ev->baseArgs["cont_pad"]);
        } });
        Game::InitSettings();
    }

    void Update(EventImpl event) {
        if (needs_save) {
            Game::SaveSettings();
            needs_save = false;
        }
        ImGuiProcessEvent(event);
    }

#define BindButton(btn, status) ImGui::Image(impl.backend == Backend::DX11 ? GetTextureByID(DefaultAssets[btn]->textureId) : (ImTextureID)(DefaultAssets[btn]->textureId), ImVec2(16.0f * scale, 16.0f * scale), ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, (status) ? 255 : 0));

    void BindAudioSlider(const char* name, const char* key, float* value, SeqPlayers playerId) {
        ImGui::Text(name, static_cast<int>(100 * *(value)));
        if (ImGui::SliderFloat((std::string("##") + key).c_str(), value, 0.0f, 1.0f, "")) {
            const float volume = floorf(*(value) * 100) / 100;
            CVar_SetFloat(const_cast<char*>(key), volume);
            needs_save = true;
            Game::SetSeqPlayerVolume(playerId, volume);
        }
    }

    void Draw() {

        console->Update();
        ImGuiBackendNewFrame();
        ImGuiWMNewFrame();
        ImGui::NewFrame();

        const std::shared_ptr<Window> wnd = GlobalCtx2::GetInstance()->GetWindow();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize;
        if (UseViewports()) {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }
        if (Game::Settings.debug.menu_bar) window_flags |= ImGuiWindowFlags_MenuBar;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(ImVec2(wnd->GetCurrentWidth(), wnd->GetCurrentHeight()));
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Main - Deck", nullptr, window_flags);
        ImGui::PopStyleVar();

        const ImGuiID dockId = ImGui::GetID("main_dock");

        if (!ImGui::DockBuilderGetNode(dockId)) {
            ImGui::DockBuilderRemoveNode(dockId);
            ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_NoTabBar);

            ImGui::DockBuilderDockWindow("OoT Master Quest", dockId);

            ImGui::DockBuilderFinish(dockId);
        }

        ImGui::DockSpace(dockId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        if (ImGui::IsKeyPressed(TOGGLE_BTN)) {
            Game::Settings.debug.menu_bar = !Game::Settings.debug.menu_bar;
            needs_save = true;
            GlobalCtx2::GetInstance()->GetWindow()->dwMenubar = Game::Settings.debug.menu_bar;
            ShowCursor(Game::Settings.debug.menu_bar, Dialogues::dMenubar);
        }

        if (ImGui::BeginMenuBar()) {
            if (DefaultAssets.contains("Game_Icon")) {
                ImGui::SetCursorPos(ImVec2(5, 2.5f));
                ImGui::Image(impl.backend == Backend::DX11 ? GetTextureByID(DefaultAssets["Game_Icon"]->textureId) : reinterpret_cast<ImTextureID>(DefaultAssets["Game_Icon"]->textureId), ImVec2(16.0f, 16.0f));
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(25, 0));
            }
            ImGui::Text("Shipwright");
            ImGui::Separator();

            if (ImGui::BeginMenu("Audio")) {
                const float volume = Game::Settings.audio.master;
                ImGui::Text("Master Volume: %d %%", static_cast<int>(100 * volume));
                if (ImGui::SliderFloat("##Master_Vol", &Game::Settings.audio.master, 0.0f, 1.0f, "")) {
                    CVar_SetFloat(const_cast<char*>("gGameMasterVolume"), volume);
                    needs_save = true;
                }

                BindAudioSlider("Main Music Volume: %d %%", "gMainMusicVolume", &Game::Settings.audio.music_main, SEQ_BGM_MAIN);
                BindAudioSlider("Sub Music Volume: %d %%", "gSubMusicVolume", &Game::Settings.audio.music_sub, SEQ_BGM_SUB);
                BindAudioSlider("Sound Effects Volume: %d %%", "gSFXMusicVolume", &Game::Settings.audio.sfx, SEQ_SFX);
                BindAudioSlider("Fanfare Volume: %d %%", "gFanfareVolume", &Game::Settings.audio.fanfare, SEQ_FANFARE);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Controller")) {
                ImGui::Text("Gyro Sensitivity: %d %%", static_cast<int>(100 * Game::Settings.controller.gyro_sensitivity));
                if (ImGui::SliderFloat("##GYROSCOPE", &Game::Settings.controller.gyro_sensitivity, 0.0f, 1.0f, "")) {
                    needs_save = true;
                }

                if (ImGui::Button("Recalibrate Gyro")) {
                    Game::Settings.controller.gyroDriftX = 0;
                    Game::Settings.controller.gyroDriftY = 0;
                }

                ImGui::Separator();

                ImGui::Text("Rumble Strength: %d %%", static_cast<int>(100 * Game::Settings.controller.rumble_strength));
                if (ImGui::SliderFloat("##RUMBLE", &Game::Settings.controller.rumble_strength, 0.0f, 1.0f, "")) {
                    needs_save = true;
                }

                if (ImGui::Checkbox("Show Inputs", &Game::Settings.controller.input_enabled)) {
                    needs_save = true;
                }

                ImGui::Text("Input Scale: %.1f", Game::Settings.controller.input_scale);
                if (ImGui::SliderFloat("##Input", &Game::Settings.controller.input_scale, 1.0f, 3.0f, "")) {
                    needs_save = true;
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Enhancements")) {

                ImGui::Text("Gameplay");
                ImGui::Separator();

                if (ImGui::Checkbox("Fast Text", &Game::Settings.enhancements.fast_text)) {
                    CVar_SetS32(const_cast<char*>("gFastText"), Game::Settings.enhancements.fast_text);
                    needs_save = true;
                }

                ImGui::Text("Graphics");
                ImGui::Separator();

                if (UseInternalRes()) {
                    HOOK(ImGui::Checkbox("N64 Mode", &Game::Settings.debug.n64mode));
                }

                if (ImGui::Checkbox("Animated Link in Pause Menu", &Game::Settings.enhancements.animated_pause_menu)) {
                    CVar_SetS32(const_cast<char*>("gPauseLiveLink"), Game::Settings.enhancements.animated_pause_menu);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Disable LOD", &Game::Settings.enhancements.disable_lod)) {
                    CVar_SetS32(const_cast<char*>("gDisableLOD"), Game::Settings.enhancements.disable_lod);
                    needs_save = true;
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Developer Tools")) {
                HOOK(ImGui::MenuItem("Stats", nullptr, &Game::Settings.debug.soh));
                HOOK(ImGui::MenuItem("Console", nullptr, &console->opened));
                
                ImGui::Text("Debug");
                ImGui::Separator();
                
                if (ImGui::Checkbox("Debug Mode", &Game::Settings.cheats.debug_mode)) {
                    CVar_SetS32(const_cast<char*>("gDebugEnabled"), Game::Settings.cheats.debug_mode);
                    needs_save = true;
                }

                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Cheats")) {
                if (ImGui::Checkbox("Infinite Money", &Game::Settings.cheats.infinite_money)) {
                    CVar_SetS32(const_cast<char*>("gInfiniteMoney"), Game::Settings.cheats.infinite_money);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Infinite Health", &Game::Settings.cheats.infinite_health)) {
                    CVar_SetS32(const_cast<char*>("gInfiniteHealth"), Game::Settings.cheats.infinite_health);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Infinite Ammo", &Game::Settings.cheats.infinite_ammo)) {
                    CVar_SetS32(const_cast<char*>("gInfiniteAmmo"), Game::Settings.cheats.infinite_ammo);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Infinite Magic", &Game::Settings.cheats.infinite_magic)) {
                    CVar_SetS32(const_cast<char*>("gInfiniteMagic"), Game::Settings.cheats.infinite_magic);
                    needs_save = true;
                }
                
                if (ImGui::Checkbox("No Clip", &Game::Settings.cheats.no_clip)) {
                    CVar_SetS32(const_cast<char*>("gNoClip"), Game::Settings.cheats.no_clip);
                    needs_save = true;
                }
                
                if (ImGui::Checkbox("Climb Everything", &Game::Settings.cheats.climb_everything)) {
                    CVar_SetS32(const_cast<char*>("gClimbEverything"), Game::Settings.cheats.climb_everything);
                    needs_save = true;
                }
                
                if (ImGui::Checkbox("Moon Jump on L", &Game::Settings.cheats.moon_jump_on_l)) {
                    CVar_SetS32(const_cast<char*>("gMoonJumpOnL"), Game::Settings.cheats.moon_jump_on_l);
                    needs_save = true;
                }
                
                if (ImGui::Checkbox("Super Tunic", &Game::Settings.cheats.super_tunic)) {
                    CVar_SetS32(const_cast<char*>("gSuperTunic"), Game::Settings.cheats.super_tunic);
                    needs_save = true;
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        if (UseViewports()) {
            flags |= ImGuiWindowFlags_NoBackground;
        }
        ImGui::Begin("OoT Master Quest", nullptr, flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        ImVec2 main_pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 pos = ImVec2(0, 0);
        gfx_current_dimensions.width = size.x * gfx_current_dimensions.internal_mul;
        gfx_current_dimensions.height = size.y * gfx_current_dimensions.internal_mul;
        if (UseInternalRes()) {
            if (Game::Settings.debug.n64mode) {
                gfx_current_dimensions.width = 320;
                gfx_current_dimensions.height = 240;
                const int sw = size.y * 320 / 240;
                pos = ImVec2(size.x / 2 - sw / 2, 0);
                size = ImVec2(sw, size.y);
            }
        }

        if (UseInternalRes()) {
            int fbuf = std::stoi(SohUtils::getEnvironmentVar("framebuffer"));
            ImGui::ImageRotated(reinterpret_cast<ImTextureID>(fbuf), pos, size, 0.0f);
        }
        ImGui::End();

        if (Game::Settings.debug.soh) {
            const float framerate = ImGui::GetIO().Framerate;
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Debug Stats", nullptr, ImGuiWindowFlags_None);

            ImGui::Text("Platform: Windows");
            ImGui::Text("Status: %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
            if (UseInternalRes()) {
                ImGui::Text("Internal Resolution:");
                ImGui::SliderInt("Mul", reinterpret_cast<int*>(&gfx_current_dimensions.internal_mul), 1, 8);
            }
            ImGui::End();
            ImGui::PopStyleColor();
        }

        const float scale = Game::Settings.controller.input_scale;
        ImVec2 BtnPos = ImVec2(160 * scale, 85 * scale);

        if(Game::Settings.controller.input_enabled) {
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

        console->Draw();

        ImGui::Render();
        ImGuiRenderDrawData(ImGui::GetDrawData());
        if (UseViewports()) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void BindCmd(const std::string& cmd, CommandEntry entry) {
        console->Commands[cmd] = std::move(entry);
    }
}