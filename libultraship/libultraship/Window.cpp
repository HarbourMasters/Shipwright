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
#include <SDL2/SDL.h>
#include <map>
#include <string>
#include <chrono>
#include "SohHooks.h"
#include "SohConsole.h"

#include <iostream>

extern "C" {
    struct OSMesgQueue;

    uint8_t __osMaxControllers = MAXCONTROLLERS;
    uint8_t __enableGameInput = 1;

    int32_t osContInit(OSMesgQueue* mq, uint8_t* controllerBits, OSContStatus* status) {
        std::shared_ptr<Ship::ConfigFile> pConf = Ship::GlobalCtx2::GetInstance()->GetConfig();
        Ship::ConfigFile& Conf = *pConf.get();

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

        // TODO: This for loop is debug. Burn it with fire.
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            if (SDL_IsGameController(i)) {
                // Get the GUID from SDL
                char buf[33];
                SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(i), buf, sizeof(buf));
                auto guid = std::string(buf);
                auto name = std::string(SDL_GameControllerNameForIndex(i));

                SPDLOG_INFO("Found Controller \"{}\" with ID \"{}\"", name, guid);
            }
        }

        for (int32_t i = 0; i < __osMaxControllers; i++) {
            std::string ControllerType = Conf["CONTROLLERS"]["CONTROLLER " + std::to_string(i+1)];
            mINI::INIStringUtil::toLower(ControllerType);

            if (ControllerType == "auto") {
                Ship::Window::Controllers[i].push_back(std::make_shared<Ship::KeyboardController>(i));
                Ship::Window::Controllers[i].push_back(std::make_shared<Ship::SDLController>(i));
            } else if (ControllerType == "keyboard") {
                Ship::Window::Controllers[i].push_back(std::make_shared<Ship::KeyboardController>(i));
            } else if (ControllerType == "usb") {
                Ship::Window::Controllers[i].push_back(std::make_shared<Ship::SDLController>(i));
            } else if (ControllerType == "unplugged") {
                // Do nothing for unplugged controllers
            } else {
                SPDLOG_ERROR("Invalid Controller Type: {}", ControllerType);
            }
        }

        *controllerBits = 0;
        for (size_t i = 0; i < __osMaxControllers; i++) {
            if (Ship::Window::Controllers[i].size() > 0) {
                *controllerBits |= 1 << i;
            }
        }

        return 0;
    }

    int32_t osContStartReadData(OSMesgQueue* mesg) {
        return 0;
    }

    void osContGetReadData(OSContPad* pad) {
        pad->button = 0;
        pad->stick_x = 0;
        pad->stick_y = 0;
        pad->err_no = 0;
        pad->gyro_x = 0;
        pad->gyro_y = 0;

        if (__enableGameInput)
        {
            for (size_t i = 0; i < __osMaxControllers; i++) {
                for (size_t j = 0; j < Ship::Window::Controllers[i].size(); j++) {
                    Ship::Window::Controllers[i][j]->Read(&pad[i]);
                }
            }
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

            //if (res != nullptr)
                return (Vtx*)res->vertices.data();
            //else
                //return (Vtx*)Ship::GlobalCtx2::GetInstance()->GetResourceManager()->LoadFile(hashStr)->buffer.get();
        }
        else {
            return nullptr;
        }
    }

    int32_t* ResourceMgr_LoadMtxByCRC(uint64_t crc) {
        const std::string* hashStr = Ship::GlobalCtx2::GetInstance()->GetResourceManager()->HashToString(crc);

        if (hashStr != nullptr) {
            auto res = std::static_pointer_cast<Ship::Matrix>(Ship::GlobalCtx2::GetInstance()->GetResourceManager()->LoadResource(hashStr->c_str()));
            return (int32_t*)res->mtx.data();
        } else {
            return nullptr;
        }
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
    std::map<size_t, std::vector<std::shared_ptr<Controller>>> Window::Controllers;
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

    void Window::Init() {
        std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
        ConfigFile& Conf = *pConf.get();

        SetAudioPlayer();
        bIsFullscreen = Ship::stob(Conf["WINDOW"]["FULLSCREEN"]);
        dwWidth = Ship::stoi(Conf["WINDOW"]["WINDOW WIDTH"], 320);
        dwHeight = Ship::stoi(Conf["WINDOW"]["WINDOW HEIGHT"], 240);
        dwWidth = Ship::stoi(Conf["WINDOW"]["FULLSCREEN WIDTH"], 1920);
        dwHeight = Ship::stoi(Conf["WINDOW"]["FULLSCREEN HEIGHT"], 1080);
        dwMenubar = Ship::stoi(Conf["WINDOW"]["menubar"], 0);
        const std::string& gfx_backend = Conf["WINDOW"]["GFX BACKEND"];
        SetWindowManager(&WmApi, &RenderingApi, gfx_backend);

        gfx_init(WmApi, RenderingApi, GetContext()->GetName().c_str(), bIsFullscreen);
        WmApi->set_fullscreen_changed_callback(Window::OnFullscreenChanged);
        WmApi->set_keyboard_callbacks(Window::KeyDown, Window::KeyUp, Window::AllKeysUp);
    }

    void Window::StartFrame() {
        gfx_start_frame();
    }

    void Window::RunCommands(Gfx* Commands, const std::vector<std::unordered_map<Mtx*, MtxF>>& mtx_replacements) {
        for (const auto& m : mtx_replacements) {
            gfx_run(Commands, m);
            gfx_end_frame();
        }
        gfx_run(Commands, {});
        gfx_end_frame();
    }

    void Window::SetFrameDivisor(int divisor) {
        gfx_set_framedivisor(divisor);
        //gfx_set_framedivisor(0);
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
        std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
        ConfigFile& Conf = *pConf.get();

        if (dwScancode == Ship::stoi(Conf["KEYBOARD SHORTCUTS"]["KEY_FULLSCREEN"])) {
            GlobalCtx2::GetInstance()->GetWindow()->ToggleFullscreen();
        }

        

        // OTRTODO: Rig with Kirito's console?
        //if (dwScancode == Ship::stoi(Conf["KEYBOARD SHORTCUTS"]["KEY_CONSOLE"])) {
        //    ToggleConsole();
        //}

        bool bIsProcessed = false;
        for (size_t i = 0; i < __osMaxControllers; i++) {
            for (size_t j = 0; j < Controllers[i].size(); j++) {
                KeyboardController* pad = dynamic_cast<KeyboardController*>(Ship::Window::Controllers[i][j].get());
                if (pad != nullptr) {
                    if (pad->ReleaseButton(dwScancode)) {
                        bIsProcessed = true;
                    }
                }
            }
        }

        return bIsProcessed;
    }

    bool Window::KeyDown(int32_t dwScancode) {
        bool bIsProcessed = false;
        for (size_t i = 0; i < __osMaxControllers; i++) {
            for (size_t j = 0; j < Controllers[i].size(); j++) {
                KeyboardController* pad = dynamic_cast<KeyboardController*>(Ship::Window::Controllers[i][j].get());
                if (pad != nullptr) {
                    if (pad->PressButton(dwScancode)) {
                        bIsProcessed = true;
                    }
                }
            }
        }

        lastScancode = dwScancode;

        return bIsProcessed;
    }


    void Window::AllKeysUp(void) {
        for (size_t i = 0; i < __osMaxControllers; i++) {
            for (size_t j = 0; j < Controllers[i].size(); j++) {
                KeyboardController* pad = dynamic_cast<KeyboardController*>(Ship::Window::Controllers[i][j].get());
                if (pad != nullptr) {
                    pad->ReleaseAllButtons();
                }
            }
        }
    }

    void Window::OnFullscreenChanged(bool bIsFullscreen) {
        std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
        ConfigFile& Conf = *pConf.get();
        GlobalCtx2::GetInstance()->GetWindow()->bIsFullscreen = bIsFullscreen;
        Conf["WINDOW"]["FULLSCREEN"] = std::to_string(bIsFullscreen);
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
