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
    std::vector<const char*> CustomTexts;
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

    const char* filters[3] = {
        "Three-Point",
        "Linear",
        "None"
    };

    std::map<std::string, std::vector<std::string>> windowCategories;
    std::map<std::string, CustomWindow> customWindows;

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

        if (UseViewports()) {
            io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        }
        console->Init();
        overlay->Init();
        ImGuiWMInit();
        ImGuiBackendInit();

        ModInternal::registerHookListener({ GFX_INIT, [](const HookEvent ev) {

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
        if (ImGui::RadioButton(text.c_str(), id == val)) {
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

    void EnhancementButton(std::string text, std::string cvarName)
    {
        bool val = (bool)CVar_GetS32(cvarName.c_str(), 0);
        if (ImGui::Button(text.c_str())) {
            CVar_SetS32(cvarName.c_str(), !val);
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

    void Tooltip(std::string text) {
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", text.c_str());
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

        if (ImGui::IsKeyPressed(TOGGLE_BTN)) {
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
                Tooltip("Shows currently pressed inputs on the bottom right of the screen");
                EnhancementCheckbox("Rumble Enabled", "gRumbleEnabled");

                EnhancementSliderFloat("Input Scale: %.1f", "##Input", "gInputScale", 1.0f, 3.0f, "", 1.0f, false);
                Tooltip("Sets the on screen size of the displayed inputs from Show Inputs");

                ImGui::Separator();

                EnhancementCheckbox("D-pad Support on Pause and File Select", "gDpadPauseName");
                EnhancementCheckbox("D-pad Support in Ocarina and Text Choice", "gDpadOcarinaText");
                EnhancementCheckbox("D-pad Support for Browsing Shop Items", "gDpadShop");

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

                EXPERIMENTAL();
                ImGui::Text("Texture Filter (Needs reload)");
                GfxRenderingAPI* gapi = gfx_get_current_rendering_api();
                if (ImGui::BeginCombo("##filters", filters[gapi->get_texture_filter()])) {
                    for (int fId = 0; fId <= FilteringMode::NONE; fId++) {
                        if (ImGui::Selectable(filters[fId], fId == gapi->get_texture_filter())) {
                            INFO("New Filter: %s", filters[fId]);
                            gapi->set_texture_filter((FilteringMode)fId);

                            CVar_SetS32("gTextureFilter", (int)fId);
                            needs_save = true;
                        }

                    }
                    ImGui::EndCombo();
                }
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
                    Tooltip("Allows equiping the tunic and boots to c-buttons");
                    EnhancementCheckbox("MM Bunny Hood", "gMMBunnyHood");
                    Tooltip("Wearing the Bunny Hood grants a speed increase like in Majora's Mask");

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Graphics"))
                {
                    EnhancementCheckbox("N64 Mode", "gN64Mode");
                    Tooltip("Sets aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution");
                    EnhancementCheckbox("Animated Link in Pause Menu", "gPauseLiveLink");
                    EnhancementCheckbox("Enable 3D Dropped items", "gNewDrops");
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
                    EnhancementCheckbox("Fix Dungeon entrances", "gFixDungeonMinimapIcon");
                    Tooltip("Show dungeon entrances icon only when it should be");
                    EnhancementCheckbox("Fix Two Handed idle animations", "gTwoHandedIdle");
                    Tooltip("Makes two handed idle animation play, a seemingly finished animation that was disabled on accident in the original game");

                    ImGui::EndMenu();
                }

                EXPERIMENTAL();

                EnhancementCheckbox("60FPS Interpolation", "g60FPS");
                EnhancementCheckbox("Disable LOD", "gDisableLOD");
                Tooltip("Turns off the level of detail setting, making models always use their higher poly variants");

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

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Developer Tools"))
            {
                EnhancementCheckbox("OoT Debug Mode", "gDebugEnabled");
                Tooltip("Enables Debug Mode, allowing you to select maps with L + R + Z, noclip with L + D-pad Right,\nand open the debug menu with L on the pause screen");
                ImGui::Separator();
                EnhancementCheckbox("Stats", "gStatsEnabled");
                Tooltip("Shows the stats window, with your FPS and frametimes, and the OS you're playing on");
                EnhancementCheckbox("Console", "gConsoleEnabled");
                Tooltip("Enables the console window, allowing you to input commands, type help for some examples");
                console->opened = CVar_GetS32("gConsoleEnabled", 0);

                ImGui::EndMenu();
            }

            for (const auto& category : windowCategories) {
                if (ImGui::BeginMenu(category.first.c_str())) {
                    for (const std::string& name : category.second) {
                        std::string varName(name);
                        varName.erase(std::ranges::remove_if(varName, isspace).begin(), varName.end());
                        std::string toggleName = "g" + varName + "Enabled";

                        EnhancementCheckbox(name, toggleName);
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

        overlay->Draw();
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
