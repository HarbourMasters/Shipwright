#include "SohImGuiImpl.h"

#include <iostream>
#include <map>
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
    float kokiri_col[3] = { 0.118f, 0.41f, 0.106f };
    float goron_col[3] = { 0.392f, 0.078f, 0.0f };
    float zora_col[3] = { 0.0f, 0.235f, 0.392f };

    float navi_idle_i_col[3] = { 0.0f, 0.0f, 0.0f };
    float navi_idle_o_col[3] = { 0.0f, 0.0f, 0.0f };

    float navi_npc_i_col[3] = { 0.0f, 0.0f, 0.0f };
    float navi_npc_o_col[3] = { 0.0f, 0.0f, 0.0f };

    float navi_enemy_i_col[3] = { 0.0f, 0.0f, 0.0f };
    float navi_enemy_o_col[3] = { 0.0f, 0.0f, 0.0f };

    float navi_prop_i_col[3] = { 0.0f, 0.0f, 0.0f };
    float navi_prop_o_col[3] = { 0.0f, 0.0f, 0.0f };

    std::map<std::string, std::vector<std::string>> windowCategories;
    std::map<std::string, CustomWindow> customWindows;

    void ImGuiWMInit() {
        switch (impl.backend) {
        case Backend::SDL:
            ImGui_ImplSDL2_InitForOpenGL(static_cast<SDL_Window*>(impl.sdl.window), impl.sdl.context);
            break;
        case Backend::DX11:
            ImGui_ImplWin32_Init(impl.dx11.window);
            break;
        }
        kokiri_col[0] = std::clamp((float) CVar_GetS32((char*)"gTunic_Kokiri_Red", 30)/255, 0.0f, 1.0f);
        kokiri_col[1] = std::clamp((float)CVar_GetS32((char*)"gTunic_Kokiri_Green", 105) / 255, 0.0f, 1.0f);
        kokiri_col[2] = std::clamp((float)CVar_GetS32((char*)"gTunic_Kokiri_Blue", 27) / 255, 0.0f, 1.0f);

        goron_col[0] = std::clamp((float)CVar_GetS32((char*)"gTunic_Goron_Red", 100) / 255, 0.0f, 1.0f);
        goron_col[1] = std::clamp((float)CVar_GetS32((char*)"gTunic_Goron_Green", 20) / 255, 0.0f, 1.0f);
        goron_col[2] = std::clamp((float)CVar_GetS32((char*)"gTunic_Goron_Blue", 0) / 255, 0.0f, 1.0f);
        
        zora_col[0] = std::clamp((float)CVar_GetS32((char*)"gTunic_Zora_Red", 0) / 255, 0.0f, 1.0f);
        zora_col[1] = std::clamp((float)CVar_GetS32((char*)"gTunic_Zora_Green", 60) / 255, 0.0f, 1.0f);
        zora_col[2] = std::clamp((float)CVar_GetS32((char*)"gTunic_Zora_Blue", 100) / 255, 0.0f, 1.0f);

        navi_idle_i_col[0] = std::clamp((float)CVar_GetS32((char*)"gNavi_Idle_Inner_Red", 0) / 255, 0.0f, 1.0f);
        navi_idle_i_col[1] = std::clamp((float)CVar_GetS32((char*)"gNavi_Idle_Inner_Green", 0) / 255, 0.0f, 1.0f);
        navi_idle_i_col[2] = std::clamp((float)CVar_GetS32((char*)"gNavi_Idle_Inner_Blue", 0) / 255, 0.0f, 1.0f);

        navi_idle_o_col[0] = std::clamp((float)CVar_GetS32((char*)"gNavi_Idle_Outer_Red", 0) / 255, 0.0f, 1.0f);
        navi_idle_o_col[1] = std::clamp((float)CVar_GetS32((char*)"gNavi_Idle_Outer_Green", 0) / 255, 0.0f, 1.0f);
        navi_idle_o_col[2] = std::clamp((float)CVar_GetS32((char*)"gNavi_Idle_Outer_Blue", 0) / 255, 0.0f, 1.0f);

        navi_npc_i_col[0] = std::clamp((float)CVar_GetS32((char*)"gNavi_NPC_Inner_Red", 0) / 255, 0.0f, 1.0f);
        navi_npc_i_col[1] = std::clamp((float)CVar_GetS32((char*)"gNavi_NPC_Inner_Green", 0) / 255, 0.0f, 1.0f);
        navi_npc_i_col[2] = std::clamp((float)CVar_GetS32((char*)"gNavi_NPC_Inner_Blue", 0) / 255, 0.0f, 1.0f);

        navi_npc_o_col[0] = std::clamp((float)CVar_GetS32((char*)"gNavi_NPC_Outer_Red", 0) / 255, 0.0f, 1.0f);
        navi_npc_o_col[1] = std::clamp((float)CVar_GetS32((char*)"gNavi_NPC_Outer_Green", 0) / 255, 0.0f, 1.0f);
        navi_npc_o_col[2] = std::clamp((float)CVar_GetS32((char*)"gNavi_NPC_Outer_Blue", 0) / 255, 0.0f, 1.0f);

        navi_enemy_i_col[0] = std::clamp((float)CVar_GetS32((char*)"gNavi_Enemy_Inner_Red", 0) / 255, 0.0f, 1.0f);
        navi_enemy_i_col[1] = std::clamp((float)CVar_GetS32((char*)"gNavi_Enemy_Inner_Green", 0) / 255, 0.0f, 1.0f);
        navi_enemy_i_col[2] = std::clamp((float)CVar_GetS32((char*)"gNavi_Enemy_Inner_Blue", 0) / 255, 0.0f, 1.0f);

        navi_enemy_o_col[0] = std::clamp((float)CVar_GetS32((char*)"gNavi_Enemy_Outer_Red", 0) / 255, 0.0f, 1.0f);
        navi_enemy_o_col[1] = std::clamp((float)CVar_GetS32((char*)"gNavi_Enemy_Outer_Green", 0) / 255, 0.0f, 1.0f);
        navi_enemy_o_col[2] = std::clamp((float)CVar_GetS32((char*)"gNavi_Enemy_Outer_Blue", 0) / 255, 0.0f, 1.0f);

        navi_prop_i_col[0] = std::clamp((float)CVar_GetS32((char*)"gNavi_Prop_Inner_Red", 0) / 255, 0.0f, 1.0f);
        navi_prop_i_col[1] = std::clamp((float)CVar_GetS32((char*)"gNavi_Prop_Inner_Green", 0) / 255, 0.0f, 1.0f);
        navi_prop_i_col[2] = std::clamp((float)CVar_GetS32((char*)"gNavi_Prop_Inner_Blue", 0) / 255, 0.0f, 1.0f);

        navi_prop_o_col[0] = std::clamp((float)CVar_GetS32((char*)"gNavi_Prop_Outer_Red", 0) / 255, 0.0f, 1.0f);
        navi_prop_o_col[1] = std::clamp((float)CVar_GetS32((char*)"gNavi_Prop_Outer_Green", 0) / 255, 0.0f, 1.0f);
        navi_prop_o_col[2] = std::clamp((float)CVar_GetS32((char*)"gNavi_Prop_Outer_Blue", 0) / 255, 0.0f, 1.0f);
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

    void LoadTexture(const std::string& name, const std::string& path) {
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

    void LoadResource(const std::string& name, const std::string& path, const ImVec4& tint) {
        GfxRenderingAPI* api = gfx_get_current_rendering_api();
        const auto res = static_cast<Ship::Texture*>(GlobalCtx2::GetInstance()->GetResourceManager()->LoadResource(normalize(path)).get());

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

#define BindButton(btn, status) ImGui::Image(GetTextureByID(DefaultAssets[btn]->textureId), ImVec2(16.0f * scale, 16.0f * scale), ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(255, 255, 255, (status) ? 255 : 0));

    void BindAudioSlider(const char* name, const char* key, float* value, SeqPlayers playerId) {
        ImGui::Text(name, static_cast<int>(100 * *(value)));
        if (ImGui::SliderFloat((std::string("##") + key).c_str(), value, 0.0f, 1.0f, "")) {
            const float volume = floorf(*(value) * 100) / 100;
            CVar_SetFloat(key, volume);
            needs_save = true;
            Game::SetSeqPlayerVolume(playerId, volume);
        }
    }

    void DrawMainMenuAndCalculateGameSize() {
        console->Update();
        ImGuiBackendNewFrame();
        ImGuiWMNewFrame();
        ImGui::NewFrame();

        const std::shared_ptr<Window> wnd = GlobalCtx2::GetInstance()->GetWindow();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize;
        if (Game::Settings.debug.menu_bar) window_flags |= ImGuiWindowFlags_MenuBar;

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

        if (ImGui::IsKeyPressed(TOGGLE_BTN)) {
            Game::Settings.debug.menu_bar = !Game::Settings.debug.menu_bar;
            needs_save = true;
            GlobalCtx2::GetInstance()->GetWindow()->dwMenubar = Game::Settings.debug.menu_bar;
            ShowCursor(Game::Settings.debug.menu_bar, Dialogues::dMenubar);
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
                const float volume = Game::Settings.audio.master;
                ImGui::Text("Master Volume: %d %%", static_cast<int>(100 * volume));
                if (ImGui::SliderFloat("##Master_Vol", &Game::Settings.audio.master, 0.0f, 1.0f, "")) {
                    CVar_SetFloat("gGameMasterVolume", volume);
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

                ImGui::Separator();

                if (ImGui::Checkbox("Dpad Support on Pause and File Select", &Game::Settings.controller.dpad_pause_name)) {
                    CVar_SetS32("gDpadPauseName", Game::Settings.controller.dpad_pause_name);
                    needs_save = true;
                }

                if (ImGui::Checkbox("DPad Support in Ocarina and Text Choice", &Game::Settings.controller.dpad_ocarina_text)) {
                    CVar_SetS32("gDpadOcarinaText", Game::Settings.controller.dpad_ocarina_text);
                    needs_save = true;
                }

                if (ImGui::Checkbox("DPad Support for Browsing Shop Items", &Game::Settings.controller.dpad_shop)) {
                    CVar_SetS32("gDpadShop", Game::Settings.controller.dpad_shop);
                    needs_save = true;
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Enhancements")) {

                ImGui::Text("Gameplay");
                ImGui::Separator();

                ImGui::Text("Text Speed: %dx", Game::Settings.enhancements.text_speed);
                if (ImGui::SliderInt("##TEXTSPEED", &Game::Settings.enhancements.text_speed, 1, 5, "")) {
                    CVar_SetS32("gTextSpeed", Game::Settings.enhancements.text_speed);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Skip Text", &Game::Settings.enhancements.skip_text)) {
                    CVar_SetS32("gSkipText", Game::Settings.enhancements.skip_text);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Minimal UI", &Game::Settings.enhancements.minimal_ui)) {
                    CVar_SetS32("gMinimalUI", Game::Settings.enhancements.minimal_ui);
                    needs_save = true;
                }

                if (ImGui::Checkbox("MM Bunny Hood", &Game::Settings.enhancements.mm_bunny_hood)) {
                    CVar_SetS32("gMMBunnyHood", Game::Settings.enhancements.mm_bunny_hood);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Visual Stone of Agony", &Game::Settings.enhancements.visualagony)) {
                    CVar_SetS32("gVisualAgony", Game::Settings.enhancements.visualagony);
                    needs_save = true;
                }

                ImGui::Text("Graphics");
                ImGui::Separator();

                HOOK(ImGui::Checkbox("N64 Mode", &Game::Settings.debug.n64mode));

                if (ImGui::Checkbox("Animated Link in Pause Menu", &Game::Settings.enhancements.animated_pause_menu)) {
                    CVar_SetS32("gPauseLiveLink", Game::Settings.enhancements.animated_pause_menu);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Disable LOD", &Game::Settings.enhancements.disable_lod)) {
                    CVar_SetS32("gDisableLOD", Game::Settings.enhancements.disable_lod);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Enable 3D Dropped items", &Game::Settings.enhancements.newdrops)) {
                    CVar_SetS32("gNewDrops", Game::Settings.enhancements.newdrops);
                    needs_save = true;
                }
              
                if (ImGui::Checkbox("Dynamic Wallet Icon", &Game::Settings.enhancements.dynamic_wallet_icon)) {
                    CVar_SetS32(const_cast<char*>("gDynamicWalletIcon"), Game::Settings.enhancements.dynamic_wallet_icon);

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
                    CVar_SetS32("gDebugEnabled", Game::Settings.cheats.debug_mode);
                    needs_save = true;
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Graphics")) {
                HOOK(ImGui::MenuItem("Anti-aliasing", nullptr, &Game::Settings.graphics.show));
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Cheats")) {
                if (ImGui::BeginMenu("Infinite...")) {
                    if (ImGui::Checkbox("Money", &Game::Settings.cheats.infinite_money)) {
                        CVar_SetS32("gInfiniteMoney", Game::Settings.cheats.infinite_money);
                        needs_save = true;
                    }

                    if (ImGui::Checkbox("Health", &Game::Settings.cheats.infinite_health)) {
                        CVar_SetS32("gInfiniteHealth", Game::Settings.cheats.infinite_health);
                        needs_save = true;
                    }

                    if (ImGui::Checkbox("Ammo", &Game::Settings.cheats.infinite_ammo)) {
                        CVar_SetS32("gInfiniteAmmo", Game::Settings.cheats.infinite_ammo);
                        needs_save = true;
                    }

                    if (ImGui::Checkbox("Magic", &Game::Settings.cheats.infinite_magic)) {
                        CVar_SetS32("gInfiniteMagic", Game::Settings.cheats.infinite_magic);
                        needs_save = true;
                    }

                    if (ImGui::Checkbox("Nayru's Love", &Game::Settings.cheats.infinite_nayru)) {
                        CVar_SetS32("gInfiniteNayru", Game::Settings.cheats.infinite_nayru);
                        needs_save = true;
                    }

                ImGui::EndMenu();
                }

                if (ImGui::Checkbox("No Clip", &Game::Settings.cheats.no_clip)) {
                    CVar_SetS32("gNoClip", Game::Settings.cheats.no_clip);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Climb Everything", &Game::Settings.cheats.climb_everything)) {
                    CVar_SetS32("gClimbEverything", Game::Settings.cheats.climb_everything);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Moon Jump on L", &Game::Settings.cheats.moon_jump_on_l)) {
                    CVar_SetS32("gMoonJumpOnL", Game::Settings.cheats.moon_jump_on_l);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Super Tunic", &Game::Settings.cheats.super_tunic)) {
                    CVar_SetS32("gSuperTunic", Game::Settings.cheats.super_tunic);
                    needs_save = true;
                }


            if (ImGui::BeginMenu("Cosmetics")) {
                ImGui::Text("Tunics");
                ImGui::Separator();
                if (ImGui::ColorEdit3("Kokiri Tunic", kokiri_col)) {
                    Game::Settings.cosmetic.tunic_kokiri_red = (int) (kokiri_col[0] * 255);
                    Game::Settings.cosmetic.tunic_kokiri_green = (int) (kokiri_col[1] * 255);
                    Game::Settings.cosmetic.tunic_kokiri_blue = (int) (kokiri_col[2] * 255);
                    CVar_SetS32(const_cast<char*>("gTunic_Kokiri_Red"), Game::Settings.cosmetic.tunic_kokiri_red);
                    CVar_SetS32(const_cast<char*>("gTunic_Kokiri_Green"), Game::Settings.cosmetic.tunic_kokiri_green);
                    CVar_SetS32(const_cast<char*>("gTunic_Kokiri_Blue"), Game::Settings.cosmetic.tunic_kokiri_blue);
                    needs_save = true;
                }
                if (ImGui::ColorEdit3("Goron Tunic", goron_col)) {
                    Game::Settings.cosmetic.tunic_goron_red = (int)(goron_col[0] * 255);
                    Game::Settings.cosmetic.tunic_goron_green = (int)(goron_col[1] * 255);
                    Game::Settings.cosmetic.tunic_goron_blue = (int)(goron_col[2] * 255);
                    CVar_SetS32(const_cast<char*>("gTunic_Goron_Red"), Game::Settings.cosmetic.tunic_goron_red);
                    CVar_SetS32(const_cast<char*>("gTunic_Goron_Green"), Game::Settings.cosmetic.tunic_goron_green);
                    CVar_SetS32(const_cast<char*>("gTunic_Goron_Blue"), Game::Settings.cosmetic.tunic_goron_blue);
                    needs_save = true;
                }
                if (ImGui::ColorEdit3("Zora Tunic", zora_col)) {
                    Game::Settings.cosmetic.tunic_zora_red = (int)(zora_col[0] * 255);
                    Game::Settings.cosmetic.tunic_zora_green = (int)(zora_col[1] * 255);
                    Game::Settings.cosmetic.tunic_zora_blue = (int)(zora_col[2] * 255);
                    CVar_SetS32(const_cast<char*>("gTunic_Zora_Red"), Game::Settings.cosmetic.tunic_zora_red);
                    CVar_SetS32(const_cast<char*>("gTunic_Zora_Green"), Game::Settings.cosmetic.tunic_zora_green);
                    CVar_SetS32(const_cast<char*>("gTunic_Zora_Blue"), Game::Settings.cosmetic.tunic_zora_blue);
                    needs_save = true;
                }
                ImGui::Text("Navi");
                ImGui::Separator();
                if (ImGui::ColorEdit3("Navi Idle Inner", navi_idle_i_col)) {
                    Game::Settings.cosmetic.navi_idle_inner_red = (int)(navi_idle_i_col[0] * 255);
                    Game::Settings.cosmetic.navi_idle_inner_green = (int)(navi_idle_i_col[1] * 255);
                    Game::Settings.cosmetic.navi_idle_inner_blue = (int)(navi_idle_i_col[2] * 255);
                    CVar_SetS32(const_cast<char*>("gNavi_Idle_Inner_Red"), Game::Settings.cosmetic.navi_idle_inner_red);
                    CVar_SetS32(const_cast<char*>("gNavi_Idle_Inner_Green"), Game::Settings.cosmetic.navi_idle_inner_green);
                    CVar_SetS32(const_cast<char*>("gNavi_Idle_Inner_Blue"), Game::Settings.cosmetic.navi_idle_inner_blue);
                    needs_save = true;
                }

                if (ImGui::ColorEdit3("Navi Idle Outer", navi_idle_o_col)) {
                    Game::Settings.cosmetic.navi_idle_outer_red = (int)(navi_idle_o_col[0] * 255);
                    Game::Settings.cosmetic.navi_idle_outer_green = (int)(navi_idle_o_col[1] * 255);
                    Game::Settings.cosmetic.navi_idle_outer_blue = (int)(navi_idle_o_col[2] * 255);
                    CVar_SetS32(const_cast<char*>("gNavi_Idle_Outer_Red"), Game::Settings.cosmetic.navi_idle_outer_red);
                    CVar_SetS32(const_cast<char*>("gNavi_Idle_Outer_Green"), Game::Settings.cosmetic.navi_idle_outer_green);
                    CVar_SetS32(const_cast<char*>("gNavi_Idle_Outer_Blue"), Game::Settings.cosmetic.navi_idle_outer_blue);
                    needs_save = true;
                }

                if (ImGui::ColorEdit3("Navi NPC Inner", navi_npc_i_col)) {
                    Game::Settings.cosmetic.navi_npc_inner_red = (int)(navi_npc_i_col[0] * 255);
                    Game::Settings.cosmetic.navi_npc_inner_green = (int)(navi_npc_i_col[1] * 255);
                    Game::Settings.cosmetic.navi_npc_inner_blue = (int)(navi_npc_i_col[2] * 255);
                    CVar_SetS32(const_cast<char*>("gNavi_NPC_Inner_Red"), Game::Settings.cosmetic.navi_npc_inner_red);
                    CVar_SetS32(const_cast<char*>("gNavi_NPC_Inner_Green"), Game::Settings.cosmetic.navi_npc_inner_green);
                    CVar_SetS32(const_cast<char*>("gNavi_NPC_Inner_Blue"), Game::Settings.cosmetic.navi_npc_inner_blue);
                    needs_save = true;
                }

                if (ImGui::ColorEdit3("Navi NPC Outer", navi_npc_o_col)) {
                    Game::Settings.cosmetic.navi_npc_outer_red = (int)(navi_npc_o_col[0] * 255);
                    Game::Settings.cosmetic.navi_npc_outer_green = (int)(navi_npc_o_col[1] * 255);
                    Game::Settings.cosmetic.navi_npc_outer_blue = (int)(navi_npc_o_col[2] * 255);
                    CVar_SetS32(const_cast<char*>("gNavi_NPC_Outer_Red"), Game::Settings.cosmetic.navi_npc_outer_red);
                    CVar_SetS32(const_cast<char*>("gNavi_NPC_Outer_Green"), Game::Settings.cosmetic.navi_npc_outer_green);
                    CVar_SetS32(const_cast<char*>("gNavi_NPC_Outer_Blue"), Game::Settings.cosmetic.navi_npc_outer_blue);
                    needs_save = true;
                }

                if (ImGui::ColorEdit3("Navi Enemy Inner", navi_enemy_i_col)) {
                    Game::Settings.cosmetic.navi_enemy_inner_red = (int)(navi_enemy_i_col[0] * 255);
                    Game::Settings.cosmetic.navi_enemy_inner_green = (int)(navi_enemy_i_col[1] * 255);
                    Game::Settings.cosmetic.navi_enemy_inner_blue = (int)(navi_enemy_i_col[2] * 255);
                    CVar_SetS32(const_cast<char*>("gNavi_Enemy_Inner_Red"), Game::Settings.cosmetic.navi_enemy_inner_red);
                    CVar_SetS32(const_cast<char*>("gNavi_Enemy_Inner_Green"), Game::Settings.cosmetic.navi_enemy_inner_green);
                    CVar_SetS32(const_cast<char*>("gNavi_Enemy_Inner_Blue"), Game::Settings.cosmetic.navi_enemy_inner_blue);
                    needs_save = true;
                }

                if (ImGui::ColorEdit3("Navi Enemy Outer", navi_enemy_o_col)) {
                    Game::Settings.cosmetic.navi_enemy_outer_red = (int)(navi_enemy_o_col[0] * 255);
                    Game::Settings.cosmetic.navi_enemy_outer_green = (int)(navi_enemy_o_col[1] * 255);
                    Game::Settings.cosmetic.navi_enemy_outer_blue = (int)(navi_enemy_o_col[2] * 255);
                    CVar_SetS32(const_cast<char*>("gNavi_Enemy_Outer_Red"), Game::Settings.cosmetic.navi_enemy_outer_red);
                    CVar_SetS32(const_cast<char*>("gNavi_Enemy_Outer_Green"), Game::Settings.cosmetic.navi_enemy_outer_green);
                    CVar_SetS32(const_cast<char*>("gNavi_Enemy_Outer_Blue"), Game::Settings.cosmetic.navi_enemy_outer_blue);
                    needs_save = true;
                }

                if (ImGui::ColorEdit3("Navi Prop Inner", navi_prop_i_col)) {
                    Game::Settings.cosmetic.navi_prop_inner_red = (int)(navi_prop_i_col[0] * 255);
                    Game::Settings.cosmetic.navi_prop_inner_green = (int)(navi_prop_i_col[1] * 255);
                    Game::Settings.cosmetic.navi_prop_inner_blue = (int)(navi_prop_i_col[2] * 255);
                    CVar_SetS32(const_cast<char*>("gNavi_Prop_Inner_Red"), Game::Settings.cosmetic.navi_prop_inner_red);
                    CVar_SetS32(const_cast<char*>("gNavi_Prop_Inner_Green"), Game::Settings.cosmetic.navi_prop_inner_green);
                    CVar_SetS32(const_cast<char*>("gNavi_Prop_Inner_Blue"), Game::Settings.cosmetic.navi_prop_inner_blue);
                    needs_save = true;
                }

                if (ImGui::ColorEdit3("Navi Prop Outer", navi_prop_o_col)) {
                    Game::Settings.cosmetic.navi_prop_outer_red = (int)(navi_prop_o_col[0] * 255);
                    Game::Settings.cosmetic.navi_prop_outer_green = (int)(navi_prop_o_col[1] * 255);
                    Game::Settings.cosmetic.navi_prop_outer_blue = (int)(navi_prop_o_col[2] * 255);
                    CVar_SetS32(const_cast<char*>("gNavi_Prop_Outer_Red"), Game::Settings.cosmetic.navi_prop_outer_red);
                    CVar_SetS32(const_cast<char*>("gNavi_Prop_Outer_Green"), Game::Settings.cosmetic.navi_prop_outer_green);
                    CVar_SetS32(const_cast<char*>("gNavi_Prop_Outer_Blue"), Game::Settings.cosmetic.navi_prop_outer_blue);
                    needs_save = true;
                }

                ImGui::EndMenu();
            }


            if (ImGui::BeginMenu("Developer Tools")) {
                HOOK(ImGui::MenuItem("Stats", nullptr, &Game::Settings.debug.soh));
                HOOK(ImGui::MenuItem("Console", nullptr, &console->opened));
                if (ImGui::Checkbox("Easy ISG", &Game::Settings.cheats.ez_isg)) {
                    CVar_SetS32("gEzISG", Game::Settings.cheats.ez_isg);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Unrestricted Items", &Game::Settings.cheats.no_restrict_item)) {
                    CVar_SetS32("gNoRestrictItems", Game::Settings.cheats.no_restrict_item);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Freeze Time", &Game::Settings.cheats.freeze_time)) {
                    CVar_SetS32("gFreezeTime", Game::Settings.cheats.freeze_time);
                    needs_save = true;
                }

                ImGui::EndMenu();
            }

            for (const auto& category : windowCategories) {
                if (ImGui::BeginMenu(category.first.c_str())) {
                    for (const std::string& name : category.second) {
                        HOOK(ImGui::MenuItem(name.c_str(), nullptr, &customWindows[name].enabled));
                    }
                    ImGui::EndMenu();
                }
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();

        if (Game::Settings.debug.soh) {
            const float framerate = ImGui::GetIO().Framerate;
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Debug Stats", nullptr, ImGuiWindowFlags_None);

            ImGui::Text("Platform: Windows");
            ImGui::Text("Status: %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
            ImGui::End();
            ImGui::PopStyleColor();
        }

        if (Game::Settings.graphics.show) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Anti-aliasing settings", nullptr, ImGuiWindowFlags_None);
            ImGui::Text("Internal Resolution:");
            ImGui::SliderInt("Mul", reinterpret_cast<int*>(&gfx_current_dimensions.internal_mul), 1, 8);
            ImGui::Text("MSAA:");
            ImGui::SliderInt("MSAA", reinterpret_cast<int*>(&gfx_msaa_level), 1, 8);
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
        gfx_current_dimensions.width = size.x * gfx_current_dimensions.internal_mul;
        gfx_current_dimensions.height = size.y * gfx_current_dimensions.internal_mul;
        gfx_current_game_window_viewport.x = main_pos.x;
        gfx_current_game_window_viewport.y = main_pos.y;
        gfx_current_game_window_viewport.width = size.x;
        gfx_current_game_window_viewport.height = size.y;
        if (Game::Settings.debug.n64mode) {
            gfx_current_dimensions.width = 320;
            gfx_current_dimensions.height = 240;
            const int sw = size.y * 320 / 240;
            gfx_current_game_window_viewport.x += (size.x - sw) / 2;
            gfx_current_game_window_viewport.width = sw;
            pos = ImVec2(size.x / 2 - sw / 2, 0);
            size = ImVec2(sw, size.y);
        }
    }

    void DrawFramebufferAndGameInput() {
        ImVec2 main_pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 pos = ImVec2(0, 0);
        if (Game::Settings.debug.n64mode) {
            const int sw = size.y * 320 / 240;
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

        const float scale = Game::Settings.controller.input_scale;
        ImVec2 BtnPos = ImVec2(160 * scale, 85 * scale);

        if (Game::Settings.controller.input_enabled) {
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
}
