#include "Window.h"
#include "spdlog/spdlog.h"
#include "KeyboardController.h"
#include "SDLController.h"
#include "GlobalCtx2.h"
#include "DisplayList.h"
#include "Vertex.h"
#include "Array.h"
#include "ResourceMgr.h"
#include "Texture.h"
#include "Blob.h"
#include "Matrix.h"
#include "AudioPlayer.h"
#include "WasapiAudioPlayer.h"
#include "PulseAudioPlayer.h"
#include "SDLAudioPlayer.h"
#include "Lib/Fast3D/gfx_pc.h"
#include "Lib/Fast3D/gfx_sdl.h"
#include "Lib/Fast3D/gfx_opengl.h"
#include "stox.h"
#if __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <map>
#include <string>
#include <chrono>
#include "Hooks.h"
#include "Console.h"

#include <iostream>

extern "C" {
    struct OSMesgQueue;

    uint8_t __osMaxControllers = MAXCONTROLLERS;
    uint8_t __enableGameInput = 1;

    int32_t osContInit(OSMesgQueue* mq, uint8_t* controllerBits, OSContStatus* status) {
        *controllerBits = 0;

        if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
            SPDLOG_ERROR("Failed to initialize SDL game controllers ({})", SDL_GetError());
            exit(EXIT_FAILURE);
        }

        const char* controllerDb = "gamecontrollerdb.txt";
        int mappingsAdded = SDL_GameControllerAddMappingsFromFile(controllerDb);
        if (mappingsAdded >= 0) {
            SPDLOG_INFO("Added SDL game controllers from \"{}\" ({})", controllerDb, mappingsAdded);
        } else {
            SPDLOG_ERROR("Failed add SDL game controller mappings from \"{}\" ({})", controllerDb, SDL_GetError());
        }

        Ship::Window::ControllerApi->Init(controllerBits);

        return 0;
    }

    int32_t osContStartReadData(OSMesgQueue* mesg) {
        return 0;
    }

    void osContGetReadData(OSContPad* pad) {
        pad->button = 0;
        pad->stick_x = 0;
        pad->stick_y = 0;
        pad->cam_x = 0;
        pad->cam_y = 0;
        pad->err_no = 0;
        pad->gyro_x = 0;
        pad->gyro_y = 0;

        if (__enableGameInput) {
            Ship::Window::ControllerApi->WriteToPad(pad);
        }

        ModInternal::ExecuteHooks<ModInternal::ControllerRead>(pad);
    }

    const char* ResourceMgr_GetNameByCRC(uint64_t crc) {
        const std::string* hashStr = Ship::GlobalCtx2::GetInstance()->GetResourceManager()->HashToString(crc);
        return hashStr != nullptr ? hashStr->c_str() : nullptr;
    }

    Vtx* ResourceMgr_LoadVtxByCRC(uint64_t crc) {
        const std::string* hashStr = Ship::GlobalCtx2::GetInstance()->GetResourceManager()->HashToString(crc);

        if (hashStr != nullptr) {
            auto res = std::static_pointer_cast<Ship::Array>(Ship::GlobalCtx2::GetInstance()->GetResourceManager()->LoadResource(hashStr->c_str()));
            return (Vtx*)res->vertices.data();
        }

        return nullptr;
    }

    int32_t* ResourceMgr_LoadMtxByCRC(uint64_t crc) {
        const std::string* hashStr = Ship::GlobalCtx2::GetInstance()->GetResourceManager()->HashToString(crc);

        if (hashStr != nullptr) {
            auto res = std::static_pointer_cast<Ship::Matrix>(Ship::GlobalCtx2::GetInstance()->GetResourceManager()->LoadResource(hashStr->c_str()));
            return (int32_t*)res->mtx.data();
        }

        return nullptr;
    }

    Gfx* ResourceMgr_LoadGfxByCRC(uint64_t crc) {
        const std::string* hashStr = Ship::GlobalCtx2::GetInstance()->GetResourceManager()->HashToString(crc);

        if (hashStr != nullptr) {
            auto res = std::static_pointer_cast<Ship::DisplayList>(Ship::GlobalCtx2::GetInstance()->GetResourceManager()->LoadResource(hashStr->c_str()));
            return (Gfx*)&res->instructions[0];
        } else {
            return nullptr;
        }
    }

    char* ResourceMgr_LoadTexByCRC(uint64_t crc)  {
        const std::string* hashStr = Ship::GlobalCtx2::GetInstance()->GetResourceManager()->HashToString(crc);

        if (hashStr != nullptr)  {
            const auto res = static_cast<Ship::Texture*>(Ship::GlobalCtx2::GetInstance()->GetResourceManager()->LoadResource(hashStr->c_str()).get());

            ModInternal::ExecuteHooks<ModInternal::LoadTexture>(hashStr->c_str(), &res->imageData);

            return reinterpret_cast<char*>(res->imageData);
        } else {
            return nullptr;
        }
    }

    void ResourceMgr_RegisterResourcePatch(uint64_t hash, uint32_t instrIndex, uintptr_t origData)
    {
        const std::string* hashStr = Ship::GlobalCtx2::GetInstance()->GetResourceManager()->HashToString(hash);

        if (hashStr != nullptr)
        {
            auto res = (Ship::Texture*)Ship::GlobalCtx2::GetInstance()->GetResourceManager()->LoadResource(hashStr->c_str()).get();

            Ship::Patch patch;
            patch.crc = hash;
            patch.index = instrIndex;
            patch.origData = origData;

            res->patches.push_back(patch);
        }
    }

    char* ResourceMgr_LoadTexByName(char* texPath) {
        const auto res = static_cast<Ship::Texture*>(Ship::GlobalCtx2::GetInstance()->GetResourceManager()->LoadResource(texPath).get());
        ModInternal::ExecuteHooks<ModInternal::LoadTexture>(texPath, &res->imageData);
        return (char*)res->imageData;
    }

    void ResourceMgr_WriteTexS16ByName(char* texPath, size_t index, s16 value) {
        const auto res = static_cast<Ship::Texture*>(Ship::GlobalCtx2::GetInstance()->GetResourceManager()->LoadResource(texPath).get());

        if (res != nullptr)
        {
            if (index < res->imageDataSize)
                res->imageData[index] = value;
            else
            {
                // Dangit Morita
                int bp = 0;
            }
        }
    }

    char* ResourceMgr_LoadBlobByName(char* blobPath) {
        auto res = (Ship::Blob*)Ship::GlobalCtx2::GetInstance()->GetResourceManager()->LoadResource(blobPath).get();
        return (char*)res->data.data();
    }

    /* Should these go in their own file?*/
    uint64_t osGetTime(void) {
        return std::chrono::steady_clock::now().time_since_epoch().count();
    }

    uint32_t osGetCount(void) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }
}

extern GfxWindowManagerAPI gfx_sdl;
void SetWindowManager(GfxWindowManagerAPI** WmApi, GfxRenderingAPI** RenderingApi, const std::string& gfx_backend);

namespace Ship {

    int32_t Window::lastScancode;

    Window::Window(std::shared_ptr<GlobalCtx2> Context) : Context(Context), APlayer(nullptr) {
        WmApi = nullptr;
        RenderingApi = nullptr;
        bIsFullscreen = false;
        dwWidth = 320;
        dwHeight = 240;
    }

    Window::~Window() {
        SPDLOG_INFO("destruct window");
    }

    void Window::CreateDefaults() {
	    const std::shared_ptr<Mercury> pConf = GlobalCtx2::GetInstance()->GetConfig();
        if (pConf->isNewInstance) {
            pConf->setInt("Window.Width", 640);
            pConf->setInt("Window.Height", 480);
            pConf->setBool("Window.Options", false);
            pConf->setString("Window.GfxBackend", "");

            pConf->setBool("Window.Fullscreen.Enabled", false);
            pConf->setInt("Window.Fullscreen.Width", 640);
            pConf->setInt("Window.Fullscreen.Height", 480);

            pConf->setString("Game.SaveName", "");
            pConf->setString("Game.Main Archive", "");
            pConf->setString("Game.Patches Archive", "");

            pConf->setInt("Shortcuts.Fullscreen", 0x044);
            pConf->setInt("Shortcuts.Console", 0x029);
            pConf->save();
        }
    }

    void Window::Init() {
        std::shared_ptr<Mercury> pConf = GlobalCtx2::GetInstance()->GetConfig();

        CreateDefaults();

        SetAudioPlayer();
        bIsFullscreen = pConf->getBool("Window.Fullscreen.Enabled", false);

        dwWidth = pConf->getInt("Window.Fullscreen.Width", bIsFullscreen ? 1920 : 640);
        dwHeight = pConf->getInt("Window.Fullscreen.Height", bIsFullscreen ? 1080 : 480);
        dwMenubar = pConf->getBool("Window.Options", false);
        const std::string& gfx_backend = pConf->getString("Window.GfxBackend");
        SetWindowManager(&WmApi, &RenderingApi, gfx_backend);

        gfx_init(WmApi, RenderingApi, GetContext()->GetName().c_str(), bIsFullscreen, dwWidth, dwHeight);
        WmApi->set_fullscreen_changed_callback(OnFullscreenChanged);
        WmApi->set_keyboard_callbacks(KeyDown, KeyUp, AllKeysUp);

        ModInternal::RegisterHook<ModInternal::ExitGame>([]() {
            ControllerApi->SaveControllerSettings();
        });
    }

    void Window::StartFrame() {
        gfx_start_frame();
    }

    void Window::RunCommands(Gfx* Commands, const std::vector<std::unordered_map<Mtx*, MtxF>>& mtx_replacements) {
        for (const auto& m : mtx_replacements) {
            gfx_run(Commands, m);
            gfx_end_frame();
        }
    }

    void Window::SetTargetFps(int fps) {
        gfx_set_target_fps(fps);
    }

    void Window::SetMaximumFrameLatency(int latency) {
        gfx_set_maximum_frame_latency(latency);
    }

    void Window::GetPixelDepthPrepare(float x, float y) {
        gfx_get_pixel_depth_prepare(x, y);
    }

    uint16_t Window::GetPixelDepth(float x, float y) {
        return gfx_get_pixel_depth(x, y);
    }

    void Window::ToggleFullscreen() {
        SetFullscreen(!bIsFullscreen);
    }

    void Window::SetFullscreen(bool bIsFullscreen) {
        this->bIsFullscreen = bIsFullscreen;
        WmApi->set_fullscreen(bIsFullscreen);
    }

    void Window::ShowCursor(bool hide) {
        if (!this->bIsFullscreen || this->dwMenubar) {
            WmApi->show_cursor(true);
        }
        else {
            WmApi->show_cursor(hide);
        }
    }

    void Window::MainLoop(void (*MainFunction)(void)) {
        WmApi->main_loop(MainFunction);
    }
	
    bool Window::KeyUp(int32_t dwScancode) {
        std::shared_ptr<Mercury> pConf = GlobalCtx2::GetInstance()->GetConfig();

        if (dwScancode == pConf->getInt("Shortcuts.Fullscreen", 0x044)) {
            GlobalCtx2::GetInstance()->GetWindow()->ToggleFullscreen();
        }

        // OTRTODO: Rig with Kirito's console?
        //if (dwScancode == Ship::stoi(Conf["KEYBOARD SHORTCUTS"]["KEY_CONSOLE"])) {
        //    ToggleConsole();
        //}
		
        lastScancode = -1;

        bool bIsProcessed = false;
        const auto pad = dynamic_cast<KeyboardController*>(ControllerApi->physicalDevices[ControllerApi->physicalDevices.size() - 2].get());
        if (pad != nullptr) {
            if (pad->ReleaseButton(dwScancode)) {
                bIsProcessed = true;
            }
        }

        return bIsProcessed;
    }

    bool Window::KeyDown(int32_t dwScancode) {
        bool bIsProcessed = false;

        const auto pad = dynamic_cast<KeyboardController*>(ControllerApi->physicalDevices[ControllerApi->physicalDevices.size() - 2].get());
        if (pad != nullptr) {
            if (pad->PressButton(dwScancode)) {
                bIsProcessed = true;
            }
        }

        lastScancode = dwScancode;

        return bIsProcessed;
    }


    void Window::AllKeysUp(void) {
        const auto pad = dynamic_cast<KeyboardController*>(ControllerApi->physicalDevices[ControllerApi->physicalDevices.size() - 2].get());
        if (pad != nullptr) {
            pad->ReleaseAllButtons();
        }
    }

    void Window::OnFullscreenChanged(bool bIsFullscreen) {
        std::shared_ptr<Mercury> pConf = GlobalCtx2::GetInstance()->GetConfig();

        GlobalCtx2::GetInstance()->GetWindow()->bIsFullscreen = bIsFullscreen;
        pConf->setBool("Window.Fullscreen.Enabled", bIsFullscreen);
        GlobalCtx2::GetInstance()->GetWindow()->ShowCursor(!bIsFullscreen);
    }



    uint32_t Window::GetCurrentWidth() {
        WmApi->get_dimensions(&dwWidth, &dwHeight);
        return dwWidth;
    }

    uint32_t Window::GetCurrentHeight() {
        WmApi->get_dimensions(&dwWidth, &dwHeight);
        return dwHeight;
    }

    void Window::SetAudioPlayer() {
#ifdef _WIN32
        APlayer = std::make_shared<WasapiAudioPlayer>();
#elif defined(__linux)
        APlayer = std::make_shared<PulseAudioPlayer>();
#else
        APlayer = std::make_shared<SDLAudioPlayer>();
#endif
    }
}
