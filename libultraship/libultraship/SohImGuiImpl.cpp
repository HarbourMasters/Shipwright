#include "SohImGuiImpl.h"

#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <algorithm>
#include <vector>

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
    int SelectedLanguage = CVar_GetS32("gLanguages", 0); //Default Language to 0=English 1=German 2=French
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
        
        // OTRTODO: This gameplay specific stuff should not be in libultraship. This needs to be moved to soh and use sTunicColors
        kokiri_col[0] = 30 / 255.0f;
        kokiri_col[1] = 105 / 255.0f;
        kokiri_col[2] = 27 / 255.0f;

        goron_col[0] = 100 / 255.0f;
        goron_col[1] = 20 / 255.0f;
        goron_col[2] = 0;
        
        zora_col[0] = 0;
        zora_col[1] = 60 / 255.0f;
        zora_col[2] = 100 / 255.0f;

        navi_idle_i_col[0] = 0;
        navi_idle_i_col[1] = 0;
        navi_idle_i_col[2] = 0;

        navi_idle_o_col[0] = 0;
        navi_idle_o_col[1] = 0;
        navi_idle_o_col[2] = 0;

        navi_npc_i_col[0] = 0;
        navi_npc_i_col[1] = 0;
        navi_npc_i_col[2] = 0;

        navi_npc_o_col[0] = 0;
        navi_npc_o_col[1] = 0;
        navi_npc_o_col[2] = 0;

        navi_enemy_i_col[0] = 0;
        navi_enemy_i_col[1] = 0;
        navi_enemy_i_col[2] = 0;

        navi_enemy_o_col[0] = 0;
        navi_enemy_o_col[1] = 0;
        navi_enemy_o_col[2] = 0;

        navi_prop_i_col[0] = 0;
        navi_prop_i_col[1] = 0;
        navi_prop_i_col[2] = 0;

        navi_prop_o_col[0] = 0;
        navi_prop_o_col[1] = 0;
        navi_prop_o_col[2] = 0;
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

        for (const auto& [i, controllers] : Ship::Window::Controllers)
        {
            CVar_SetFloat(StringHelper::Sprintf("gCont%i_GyroDriftX", i).c_str(), 0);
            CVar_SetFloat(StringHelper::Sprintf("gCont%i_GyroDriftY", i).c_str(), 0);
            needs_save = true;
        }

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

    void EnhancementRadioButton(std::string text, std::string cvarName, int id) {
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
        int val = CVar_GetS32(cvarName.c_str(), 0);
        if (ImGui::RadioButton(text.c_str(), id==val)) {
            CVar_SetS32(cvarName.c_str(), (int)id);
            needs_save = true;
        }
    }

    void EnhancementCheckbox(std::string text, std::string cvarName)
    {
        bool val = (bool)CVar_GetS32(cvarName.c_str(), 0);
        if (ImGui::Checkbox(text.c_str(), &val)) {
            CVar_SetS32(cvarName.c_str(), val);
            needs_save = true;
        }
    }

    void EnhancementSliderInt(std::string text, std::string id, std::string cvarName, int min, int max, std::string format)
    {
        int val = CVar_GetS32(cvarName.c_str(), 0);

        ImGui::Text(text.c_str(), val);

        if (ImGui::SliderInt(id.c_str(), &val, min, max, format.c_str())) 
        {
            CVar_SetS32(cvarName.c_str(), val);
            needs_save = true;
        }

        if (val < min)
        {
            val = min;
            CVar_SetS32(cvarName.c_str(), val);
            needs_save = true;
        }

        if (val > max)
        {
            val = max;
            CVar_SetS32(cvarName.c_str(), val);
            needs_save = true;
        }
    }

    void EnhancementSliderFloat(std::string text, std::string id, std::string cvarName, float min, float max, std::string format, float defaultValue, bool isPercentage)
    {
        float val = CVar_GetFloat(cvarName.c_str(), defaultValue);

        if (!isPercentage)
            ImGui::Text(text.c_str(), val);
        else
            ImGui::Text(text.c_str(), static_cast<int>(100 * val));

        if (ImGui::SliderFloat(id.c_str(), &val, min, max, format.c_str()))
        {
            CVar_SetFloat(cvarName.c_str(), val);
            needs_save = true;
        }

        if (val < min)
        {
            val = min;
            CVar_SetFloat(cvarName.c_str(), val);
            needs_save = true;
        }

        if (val > max)
        {
            val = max;
            CVar_SetFloat(cvarName.c_str(), val);
            needs_save = true;
        }
    }

    void EnhancementColor3(std::string text, std::string cvarName, float defaultColors[3])
    {
        int r = CVar_GetS32((cvarName + "_Red").c_str(), (defaultColors[0] * 255.0f));
        int g = CVar_GetS32((cvarName + "_Green").c_str(), (defaultColors[1] * 255.0f));
        int b = CVar_GetS32((cvarName + "_Blue").c_str(), (defaultColors[2] * 255.0f));

        float colors[3];
        colors[0] = r / 255.0f;
        colors[1] = g / 255.0f;
        colors[2] = b / 255.0f;

        {
            if (ImGui::ColorEdit3(text.c_str(), colors)) 
            {
                CVar_SetS32((cvarName + "_Red").c_str(), (int)(colors[0] * 255));
                CVar_SetS32((cvarName + "_Green").c_str(), (int)(colors[1] * 255));
                CVar_SetS32((cvarName + "_Blue").c_str(), (int)(colors[2] * 255));
                needs_save = true;
            }
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
                EnhancementSliderFloat("Master Volume: %d %%", "##Master_Vol", "gGameMasterVolume", 0.0f, 1.0f, "", 1.0f, true);

                BindAudioSlider("Main Music Volume: %d %%", "gMainMusicVolume", 1.0f, SEQ_BGM_MAIN);
                BindAudioSlider("Sub Music Volume: %d %%", "gSubMusicVolume", 1.0f, SEQ_BGM_SUB);
                BindAudioSlider("Sound Effects Volume: %d %%", "gSFXMusicVolume", 1.0f, SEQ_SFX);
                BindAudioSlider("Fanfare Volume: %d %%", "gFanfareVolume", 1.0f, SEQ_FANFARE);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Controller"))
            {
                for (const auto& [i, controllers] : Ship::Window::Controllers)
                {
                    bool hasPad = std::find_if(controllers.begin(), controllers.end(), [](const auto& c) {
                        return c->HasPadConf() && c->Connected();
                        }) != controllers.end();

                        if (!hasPad) continue;

                        auto menuLabel = "Controller " + std::to_string(i + 1);
                        if (ImGui::BeginMenu(menuLabel.c_str()))
                        {
                            EnhancementSliderFloat("Gyro Sensitivity: %d %%", "##GYROSCOPE", StringHelper::Sprintf("gCont%i_GyroSensitivity", i), 0.0f, 1.0f, "", 1.0f, true);

                            if (ImGui::Button("Recalibrate Gyro"))
                            {
                                CVar_SetFloat(StringHelper::Sprintf("gCont%i_GyroDriftX", i).c_str(), 0);
                                CVar_SetFloat(StringHelper::Sprintf("gCont%i_GyroDriftY", i).c_str(), 0);
                                needs_save = true;
                            }

                            ImGui::Separator();

                            EnhancementSliderFloat("Rumble Strength: %d %%", "##RUMBLE", StringHelper::Sprintf("gCont%i_RumbleStrength", i), 0.0f, 1.0f, "", 1.0f, true);

                            ImGui::EndMenu();
                        }
                        ImGui::Separator();
                }

                EnhancementCheckbox("Show Inputs", "gInputEnabled");
                EnhancementCheckbox("Rumble Enabled", "gRumbleEnabled");

                EnhancementSliderFloat("Input Scale: %.1f", "##Input", "gInputScale", 1.0f, 3.0f, "", 1.0f, false);

                ImGui::Separator();

                EnhancementCheckbox("Dpad Support on Pause and File Select", "gDpadPauseName");
                EnhancementCheckbox("DPad Support in Ocarina and Text Choice", "gDpadOcarinaText");
                EnhancementCheckbox("DPad Support for Browsing Shop Items", "gDpadShop");

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Enhancements"))
            {

                ImGui::Text("Gameplay");
                ImGui::Separator();

                EnhancementSliderInt("Text Speed: %dx", "##TEXTSPEED", "gTextSpeed", 1, 5, "");

                EnhancementCheckbox("Skip Text", "gSkipText");
                EnhancementCheckbox("Minimal UI", "gMinimalUI");
                EnhancementCheckbox("MM Bunny Hood", "gMMBunnyHood");
                EnhancementCheckbox("Visual Stone of Agony", "gVisualAgony");

                ImGui::Text("Graphics");
                ImGui::Separator();

                EnhancementCheckbox("N64 Mode", "gN64Mode");

                EnhancementCheckbox("Animated Link in Pause Menu", "gPauseLiveLink");
                EnhancementCheckbox("Disable LOD", "gDisableLOD");
                EnhancementCheckbox("Enable 3D Dropped items", "gNewDrops");
                EnhancementCheckbox("Dynamic Wallet Icon", "gDynamicWalletIcon");

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Developer Tools"))
            {
                HOOK(ImGui::MenuItem("Stats", nullptr, &Game::Settings.debug.soh));
                HOOK(ImGui::MenuItem("Console", nullptr, &console->opened));

                ImGui::Text("Debug");
                ImGui::Separator();

                EnhancementCheckbox("Debug Mode", "gDebugEnabled");

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Graphics"))
            {
                HOOK(ImGui::MenuItem("Anti-aliasing", nullptr, &Game::Settings.graphics.show));
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
                EnhancementCheckbox("Climb Everything", "gClimbEverything");
                EnhancementCheckbox("Moon Jump on L", "gMoonJumpOnL");
                EnhancementCheckbox("Super Tunic", "gSuperTunic");
                EnhancementCheckbox("Easy ISG", "gEzISG");
                EnhancementCheckbox("Unrestricted Items", "gNoRestrictItems");
                EnhancementCheckbox("Freeze Time", "gFreezeTime");


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

                if (ImGui::Checkbox("Easy ISG", &Game::Settings.cheats.ez_isg)) {
                    CVar_SetS32("gEzISG", Game::Settings.cheats.ez_isg);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Unrestricted Items", &Game::Settings.cheats.no_restrict_item)) {
                    CVar_SetS32("gNoRestrictItem", Game::Settings.cheats.no_restrict_item);
                    needs_save = true;
                }

                if (ImGui::Checkbox("Freeze Time", &Game::Settings.cheats.freeze_time)) {
                    CVar_SetS32("gFreezeTime", Game::Settings.cheats.freeze_time);
                    needs_save = true;
                }
                

                ImGui::EndMenu();

            }

            if (ImGui::BeginMenu("Cosmetics"))
            {
                ImGui::Text("Tunics");
                ImGui::Separator();

                EnhancementColor3("Kokiri Tunic", "gTunic_Kokiri", kokiri_col);
                EnhancementColor3("Goron Tunic", "gTunic_Goron", goron_col);
                EnhancementColor3("Zora Tunic", "gTunic_Zora", zora_col);

                ImGui::Text("Navi");
                ImGui::Separator();

                EnhancementColor3("Navi Idle Inner", "gNavi_Idle_Inner", navi_idle_i_col);
                EnhancementColor3("Navi Idle Outer", "gNavi_Idle_Outer", navi_idle_o_col);
                EnhancementColor3("Navi NPC Inner", "gNavi_NPC_Inner", navi_npc_i_col);
                EnhancementColor3("Navi NPC Outer", "gNavi_NPC_Outer", navi_npc_o_col);
                EnhancementColor3("Navi Enemy Inner", "gNavi_Enemy_Inner", navi_enemy_i_col);
                EnhancementColor3("Navi Enemy Outer", "gNavi_Enemy_Outer", navi_enemy_o_col);
                EnhancementColor3("Navi Prop Inner", "gNavi_Prop_Inner", navi_prop_i_col);
                EnhancementColor3("Navi Prop Outer", "gNavi_Prop_Outer", navi_prop_o_col);

                ImGui::EndMenu();
            }

            if (CVar_GetS32("gLanguages", 0) == 0) {
                SelectedLanguage = 0;
            } else if (CVar_GetS32("gLanguages", 0) == 1) {
                SelectedLanguage = 1;
            } else if (CVar_GetS32("gLanguages", 0) == 2) {
                SelectedLanguage = 2;
            }
            if (ImGui::BeginMenu("Languages")) {
                EnhancementRadioButton("English", "gLanguages", 0);
                EnhancementRadioButton("German", "gLanguages", 1);
                EnhancementRadioButton("French", "gLanguages", 2);
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

        if (CVar_GetS32("gN64Mode", 0))
        {
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
        if (CVar_GetS32("gN64Mode", 0)) {
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
