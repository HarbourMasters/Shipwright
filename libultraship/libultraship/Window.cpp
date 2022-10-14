#include <string>
#include <chrono>
#include <fstream>
#include <iostream>
#include "Window.h"
#include "ResourceMgr.h"
#include "KeyboardController.h"
#include "UltraController.h"
#include "DisplayList.h"
#include "Console.h"
#include "Array.h"
#include "Texture.h"
#include "Blob.h"
#include "Matrix.h"
#include "Hooks.h"
#include <string>
#include "Lib/Fast3D/gfx_pc.h"
#include "Lib/Fast3D/gfx_sdl.h"
#include "Lib/Fast3D/gfx_dxgi.h"
#include "Lib/Fast3D/gfx_glx.h"
#include "Lib/Fast3D/gfx_opengl.h"
#include "Lib/Fast3D/gfx_direct3d11.h"
#include "Lib/Fast3D/gfx_direct3d12.h"
#include "Lib/Fast3D/gfx_wiiu.h"
#include "Lib/Fast3D/gfx_gx2.h"
#include "Lib/Fast3D/gfx_rendering_api.h"
#include "Lib/Fast3D/gfx_window_manager_api.h"
#include <SDL2/SDL.h>
#include "ImGuiImpl.h"
#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/sohconsole_sink.h"
#include "PR/ultra64/gbi.h"
#ifdef __APPLE__
#include "OSXFolderManager.h"
#elif defined(__SWITCH__)
#include "SwitchImpl.h"
#elif defined(__WIIU__)
#include "WiiUImpl.h"
#endif


#define LOAD_TEX(texPath) static_cast<Ship::Texture*>(Ship::Window::GetInstance()->GetResourceManager()->LoadResource(texPath).get());

extern "C" {
    struct OSMesgQueue;

    uint8_t __osMaxControllers = MAXCONTROLLERS;

    int32_t osContInit(OSMesgQueue* mq, uint8_t* controllerBits, OSContStatus* status) {
        *controllerBits = 0;

#ifndef __WIIU__
        if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
            SPDLOG_ERROR("Failed to initialize SDL game controllers ({})", SDL_GetError());
            exit(EXIT_FAILURE);
        }

    #ifndef __SWITCH__
        const char* controllerDb = "gamecontrollerdb.txt";
        int mappingsAdded = SDL_GameControllerAddMappingsFromFile(controllerDb);
        if (mappingsAdded >= 0) {
            SPDLOG_INFO("Added SDL game controllers from \"{}\" ({})", controllerDb, mappingsAdded);
        } else {
            SPDLOG_ERROR("Failed add SDL game controller mappings from \"{}\" ({})", controllerDb, SDL_GetError());
        }
    #endif
#endif

        Ship::Window::GetInstance()->GetControlDeck()->Init(controllerBits);

        return 0;
    }

    int32_t osContStartReadData(OSMesgQueue* mesg) {
        return 0;
    }

    void osContGetReadData(OSContPad* pad) {
        pad->button = 0;
        pad->stick_x = 0;
        pad->stick_y = 0;
        pad->right_stick_x = 0;
        pad->right_stick_y = 0;
        pad->err_no = 0;
        pad->gyro_x = 0;
        pad->gyro_y = 0;
        
        if (SohImGui::GetInputEditor()->IsOpened()) return;

        Ship::Window::GetInstance()->GetControlDeck()->WriteToPad(pad);
        Ship::ExecuteHooks<Ship::ControllerRead>(pad);
    }

    const char* ResourceMgr_GetNameByCRC(uint64_t crc) {
        const std::string* hashStr = Ship::Window::GetInstance()->GetResourceManager()->HashToString(crc);
        return hashStr != nullptr ? hashStr->c_str() : nullptr;
    }

    Vtx* ResourceMgr_LoadVtxByCRC(uint64_t crc) {
        const std::string* hashStr = Ship::Window::GetInstance()->GetResourceManager()->HashToString(crc);

        if (hashStr != nullptr) {
            auto res = std::static_pointer_cast<Ship::Array>(Ship::Window::GetInstance()->GetResourceManager()->LoadResource(hashStr->c_str()));
            return (Vtx*)res->vertices.data();
        }

        return nullptr;
    }

    int32_t* ResourceMgr_LoadMtxByCRC(uint64_t crc) {
        const std::string* hashStr = Ship::Window::GetInstance()->GetResourceManager()->HashToString(crc);

        if (hashStr != nullptr) {
            auto res = std::static_pointer_cast<Ship::Matrix>(Ship::Window::GetInstance()->GetResourceManager()->LoadResource(hashStr->c_str()));
            return (int32_t*)res->mtx.data();
        }

        return nullptr;
    }

    Gfx* ResourceMgr_LoadGfxByCRC(uint64_t crc) {
        const std::string* hashStr = Ship::Window::GetInstance()->GetResourceManager()->HashToString(crc);

        if (hashStr != nullptr) {
            auto res = std::static_pointer_cast<Ship::DisplayList>(Ship::Window::GetInstance()->GetResourceManager()->LoadResource(hashStr->c_str()));
            return (Gfx*)&res->instructions[0];
        } else {
            return nullptr;
        }
    }

    char* ResourceMgr_LoadTexByCRC(uint64_t crc)  {
        const std::string* hashStr = Ship::Window::GetInstance()->GetResourceManager()->HashToString(crc);

        if (hashStr != nullptr)  {
            const auto res = LOAD_TEX(hashStr->c_str());
            Ship::ExecuteHooks<Ship::LoadTexture>(hashStr->c_str(), &res->imageData);

            return reinterpret_cast<char*>(res->imageData);
        } else {
            return nullptr;
        }
    }

    void ResourceMgr_RegisterResourcePatch(uint64_t hash, uint32_t instrIndex, uintptr_t origData)
    {
        const std::string* hashStr = Ship::Window::GetInstance()->GetResourceManager()->HashToString(hash);

        if (hashStr != nullptr)
        {
            auto res = (Ship::Texture*)Ship::Window::GetInstance()->GetResourceManager()->LoadResource(hashStr->c_str()).get();

            Ship::Patch patch;
            patch.crc = hash;
            patch.index = instrIndex;
            patch.origData = origData;

            res->patches.push_back(patch);
        }
    }

    char* ResourceMgr_LoadTexByName(char* texPath) {
        const auto res = LOAD_TEX(texPath);
        Ship::ExecuteHooks<Ship::LoadTexture>(texPath, &res->imageData);
        return (char*)res->imageData;
    }

    uint16_t ResourceMgr_LoadTexWidthByName(char* texPath) {
        const auto res = LOAD_TEX(texPath);
        if (res != nullptr)
            return res->width;

        SPDLOG_ERROR("Given texture path is a non-existent resource");
        return -1;
    }

    uint16_t ResourceMgr_LoadTexHeightByName(char* texPath) {
        const auto res = LOAD_TEX(texPath);
        if (res != nullptr)
            return res->height;

        SPDLOG_ERROR("Given texture path is a non-existent resource");
        return -1;
    }

    uint32_t ResourceMgr_LoadTexSizeByName(char* texPath) {
        const auto res = LOAD_TEX(texPath);
        if (res != nullptr)
            return res->imageDataSize;

        SPDLOG_ERROR("Given texture path is a non-existent resource");
        return -1;
    }

    void ResourceMgr_WriteTexS16ByName(char* texPath, size_t index, s16 value) {
        const auto res = LOAD_TEX(texPath);

        if (res != nullptr)
        {
            if ((index * 2) < res->imageDataSize)
                ((s16*)res->imageData)[index] = value;
            else
            {
                // Dangit Morita
                int bp = 0;
            }
        }
    }

    char* ResourceMgr_LoadBlobByName(char* blobPath) {
        auto res = (Ship::Blob*)Ship::Window::GetInstance()->GetResourceManager()->LoadResource(blobPath).get();
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

namespace Ship {

    std::weak_ptr<Window> Window::Context;

    std::shared_ptr<Window> Window::GetInstance() {
        return Context.lock();
    }

    std::shared_ptr<Window> Window::CreateInstance(const std::string Name, const std::vector<std::string>& OTRFiles, const std::unordered_set<uint32_t>& ValidHashes) {
        if (Context.expired()) {
            auto Shared = std::make_shared<Window>(Name);
            Context = Shared;
            Shared->Initialize(OTRFiles, ValidHashes);
            return Shared;
        }

        SPDLOG_DEBUG("Trying to create a context when it already exists. Returning existing.");

        return GetInstance();
    }

    Window::Window(std::string Name) : Name(std::move(Name)), APlayer(nullptr), ControllerApi(nullptr), ResMan(nullptr), Logger(nullptr), Config(nullptr) {
        WmApi = nullptr;
        RenderingApi = nullptr;
        bIsFullscreen = false;
        dwWidth = 320;
        dwHeight = 240;
    }

    Window::~Window() {
        SPDLOG_DEBUG("destruct window");
    }

    void Window::CreateDefaults() {
        if (GetConfig()->isNewInstance) {
            GetConfig()->setInt("Window.Width", 640);
            GetConfig()->setInt("Window.Height", 480);
            GetConfig()->setBool("Window.Options", false);
            GetConfig()->setString("Window.GfxBackend", "");
            GetConfig()->setString("Window.AudioBackend", "");

            GetConfig()->setBool("Window.Fullscreen.Enabled", false);
            GetConfig()->setInt("Window.Fullscreen.Width", 1920);
            GetConfig()->setInt("Window.Fullscreen.Height", 1080);

            GetConfig()->setString("Game.SaveName", "");
            GetConfig()->setString("Game.Main Archive", "");
            GetConfig()->setString("Game.Patches Archive", "");

            GetConfig()->setInt("Shortcuts.Fullscreen", 0x044);
            GetConfig()->setInt("Shortcuts.Console", 0x029);
            GetConfig()->save();
        }
    }

    void Window::Initialize(const std::vector<std::string>& OTRFiles, const std::unordered_set<uint32_t>& ValidHashes) {
        InitializeLogging();
        InitializeConfiguration();
        InitializeResourceManager(OTRFiles, ValidHashes);
        CreateDefaults();
        InitializeControlDeck();

        bIsFullscreen = GetConfig()->getBool("Window.Fullscreen.Enabled", false);

        if (bIsFullscreen) {
            dwWidth = GetConfig()->getInt("Window.Fullscreen.Width", 1920);
            dwHeight = GetConfig()->getInt("Window.Fullscreen.Height", 1080);
        } else {
            dwWidth = GetConfig()->getInt("Window.Width", 640);
            dwHeight = GetConfig()->getInt("Window.Height", 480);
        }

        dwMenubar = GetConfig()->getBool("Window.Options", false);

        gfxBackend = GetConfig()->getString("Window.GfxBackend");
        InitializeWindowManager();

        audioBackend = GetConfig()->getString("Window.AudioBackend");
        InitializeAudioPlayer();

        gfx_init(WmApi, RenderingApi, GetName().c_str(), bIsFullscreen, dwWidth, dwHeight);
        WmApi->set_fullscreen_changed_callback(OnFullscreenChanged);
        WmApi->set_keyboard_callbacks(KeyDown, KeyUp, AllKeysUp);

        Ship::RegisterHook<ExitGame>([this]() {
            ControllerApi->SaveControllerSettings();
        });
    }

    std::string Window::GetAppDirectoryPath() {
#ifdef __APPLE__
        FolderManager folderManager;
        std::string fpath = std::string(folderManager.pathForDirectory(NSApplicationSupportDirectory, NSUserDomainMask));
        fpath.append("/com.shipofharkinian.soh");
        return fpath;
#endif

#ifdef __linux__
        char* fpath = std::getenv("SHIP_HOME");
        if (fpath != NULL)
            return std::string(fpath);
#endif

        return ".";
    }

    std::string Window::GetPathRelativeToAppDirectory(const char* path) {
        return GetAppDirectoryPath() + "/" + path;
    }

    void Window::StartFrame() {
        gfx_start_frame();
    }

    void Window::SetTargetFps(int32_t fps) {
        gfx_set_target_fps(fps);
    }

    void Window::SetMaximumFrameLatency(int32_t latency) {
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
        if (dwScancode == GetInstance()->GetConfig()->getInt("Shortcuts.Fullscreen", 0x044)) {
            GetInstance()->ToggleFullscreen();
        }

        GetInstance()->SetLastScancode(-1);


        bool bIsProcessed = false;
        auto controlDeck = GetInstance()->GetControlDeck();
        const auto pad = dynamic_cast<KeyboardController*>(controlDeck->GetPhysicalDevice(controlDeck->GetNumPhysicalDevices() - 2).get());
        if (pad != nullptr) {
            if (pad->ReleaseButton(dwScancode)) {
                bIsProcessed = true;
            }
        }

        return bIsProcessed;
    }

    bool Window::KeyDown(int32_t dwScancode) {
        bool bIsProcessed = false;
        auto controlDeck = GetInstance()->GetControlDeck();
        const auto pad = dynamic_cast<KeyboardController*>(controlDeck->GetPhysicalDevice(controlDeck->GetNumPhysicalDevices() - 2).get());
        if (pad != nullptr) {
            if (pad->PressButton(dwScancode)) {
                bIsProcessed = true;
            }
        }

        GetInstance()->SetLastScancode(dwScancode);

        return bIsProcessed;
    }


    void Window::AllKeysUp(void) {
        auto controlDeck = Window::GetInstance()->GetControlDeck();
        const auto pad = dynamic_cast<KeyboardController*>(controlDeck->GetPhysicalDevice(controlDeck->GetNumPhysicalDevices() - 2).get());
        if (pad != nullptr) {
            pad->ReleaseAllButtons();
        }
    }

    void Window::OnFullscreenChanged(bool bIsFullscreen) {
        std::shared_ptr<Mercury> pConf = Window::GetInstance()->GetConfig();

        Window::GetInstance()->bIsFullscreen = bIsFullscreen;
        pConf->setBool("Window.Fullscreen.Enabled", bIsFullscreen);
        Window::GetInstance()->ShowCursor(!bIsFullscreen);
    }

    uint32_t Window::GetCurrentWidth() {
        WmApi->get_dimensions(&dwWidth, &dwHeight);
        return dwWidth;
    }

    uint32_t Window::GetCurrentHeight() {
        WmApi->get_dimensions(&dwWidth, &dwHeight);
        return dwHeight;
    }

    void Window::InitializeAudioPlayer() {
#ifdef _WIN32
        APlayer = std::make_shared<WasapiAudioPlayer>();
#elif defined(__linux)
        APlayer = std::make_shared<PulseAudioPlayer>();
#else
        APlayer = std::make_shared<SDLAudioPlayer>();
#endif

        // Config can override
#ifdef _WIN32
        if (audioBackend == "wasapi") {
            APlayer = std::make_shared<WasapiAudioPlayer>();
        }
#endif
#if defined(__linux)
        if (audioBackend == "pulse") {
            APlayer = std::make_shared<PulseAudioPlayer>();
        }
#endif
        if (audioBackend == "sdl") {
            APlayer = std::make_shared<SDLAudioPlayer>();
        }
    }

    void Window::InitializeWindowManager() {
        // First set default
#ifdef ENABLE_OPENGL
        RenderingApi = &gfx_opengl_api;
#if defined(__linux__) && defined(X11_SUPPORTED)
        // LINUX_TODO:
        // *WmApi = &gfx_glx;
        WmApi = &gfx_sdl;
#else
        WmApi = &gfx_sdl;
#endif
#endif
#ifdef ENABLE_DX12
    	RenderingApi = &gfx_direct3d12_api;
        WmApi = &gfx_dxgi_api;
#endif
#ifdef ENABLE_DX11
        RenderingApi = &gfx_direct3d11_api;
        WmApi = &gfx_dxgi_api;
#endif
#ifdef __WIIU__
        RenderingApi = &gfx_gx2_api;
        WmApi = &gfx_wiiu;
#endif

        // Config can override
#ifdef ENABLE_DX11
        if (gfxBackend == "dx11") {
            RenderingApi = &gfx_direct3d11_api;
            WmApi = &gfx_dxgi_api;
        }
#endif
#ifdef ENABLE_OPENGL
        if (gfxBackend == "sdl") {
            RenderingApi = &gfx_opengl_api;
            WmApi = &gfx_sdl;
        }
#if defined(__linux__) && defined(X11_SUPPORTED)
        if (gfxBackend == "glx") {
            RenderingApi = &gfx_opengl_api;
            WmApi = &gfx_glx;
        }
#endif
#endif
    }

    void Window::InitializeControlDeck() {
        ControllerApi = std::make_shared<ControlDeck>();
    }

    void Window::InitializeLogging() {
        try {
            // Setup Logging
            spdlog::init_thread_pool(8192, 1);
            std::vector<spdlog::sink_ptr> Sinks;

            auto SohConsoleSink = std::make_shared<spdlog::sinks::soh_sink_mt>();
            //SohConsoleSink->set_level(spdlog::level::trace);
            Sinks.push_back(SohConsoleSink);

#if (!defined(_WIN32) && !defined(__WIIU__)) || defined(_DEBUG)
#if defined(_DEBUG) && defined(_WIN32)
            if (AllocConsole() == 0) {
                throw std::system_error(GetLastError(), std::generic_category(), "Failed to create debug console");
            }

            SetConsoleOutputCP(CP_UTF8);

            FILE* fDummy;
            freopen_s(&fDummy, "CONOUT$", "w", stdout);
            freopen_s(&fDummy, "CONOUT$", "w", stderr);
            freopen_s(&fDummy, "CONIN$", "r", stdin);
            std::cout.clear();
            std::clog.clear();
            std::cerr.clear();
            std::cin.clear();

            HANDLE hConOut = CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            HANDLE hConIn = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
            SetStdHandle(STD_ERROR_HANDLE, hConOut);
            SetStdHandle(STD_INPUT_HANDLE, hConIn);
            std::wcout.clear();
            std::wclog.clear();
            std::wcerr.clear();
            std::wcin.clear();
#endif
            auto ConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            //ConsoleSink->set_level(spdlog::level::trace);
            Sinks.push_back(ConsoleSink);
#endif

#ifndef __WIIU__
            auto logPath = GetPathRelativeToAppDirectory(("logs/" + GetName() + ".log").c_str());
            auto FileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logPath, 1024 * 1024 * 10, 10);
            FileSink->set_level(spdlog::level::trace);
            Sinks.push_back(FileSink);
#endif

            Logger = std::make_shared<spdlog::async_logger>(GetName(), Sinks.begin(), Sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
            GetLogger()->set_level(spdlog::level::trace);

#ifndef __WIIU__
            GetLogger()->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%@] [%l] %v");
#else
            GetLogger()->set_pattern("[%s:%#] [%l] %v");
#endif

            spdlog::register_logger(GetLogger());
            spdlog::set_default_logger(GetLogger());
        }
        catch (const spdlog::spdlog_ex& ex) {
            std::cout << "Log initialization failed: " << ex.what() << std::endl;
        }
    }

    void Window::InitializeResourceManager(const std::vector<std::string>& OTRFiles, const std::unordered_set<uint32_t>& ValidHashes) {
        MainPath = Config->getString("Game.Main Archive", GetAppDirectoryPath());
        PatchesPath = Config->getString("Game.Patches Archive", GetAppDirectoryPath() + "/mods");
        if (OTRFiles.empty()) {
            ResMan = std::make_shared<ResourceMgr>(GetInstance(), MainPath, PatchesPath, ValidHashes);
        } else {
            ResMan = std::make_shared<ResourceMgr>(GetInstance(), OTRFiles, ValidHashes);
        }

        if (!ResMan->DidLoadSuccessfully())
        {
#if defined(__SWITCH__)
            printf("Main OTR file not found!\n");
#elif defined(__WIIU__)
            Ship::WiiU::ThrowMissingOTR(MainPath.c_str());
#else
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "OTR file not found", "Main OTR file not found. Please generate one", nullptr);
            SPDLOG_ERROR("Main OTR file not found!");
#endif
            exit(1);
    }
#ifdef __SWITCH__
        Ship::Switch::Init(PostInitPhase);
#endif
    }

    void Window::InitializeConfiguration() {
        Config = std::make_shared<Mercury>(GetPathRelativeToAppDirectory("shipofharkinian.json"));
    }

    void Window::WriteSaveFile(const std::filesystem::path& savePath, const uintptr_t addr, void* dramAddr, const size_t size) {
        std::ofstream saveFile = std::ofstream(savePath, std::fstream::in | std::fstream::out | std::fstream::binary);
        saveFile.seekp(addr);
        saveFile.write((char*)dramAddr, size);
        saveFile.close();
    }

    void Window::ReadSaveFile(std::filesystem::path savePath, uintptr_t addr, void* dramAddr, size_t size) {
        std::ifstream saveFile = std::ifstream(savePath, std::fstream::in | std::fstream::out | std::fstream::binary);

        // If the file doesn't exist, initialize DRAM
        if (saveFile.good()) {
            saveFile.seekg(addr);
            saveFile.read((char*)dramAddr, size);
        }
        else {
            memset(dramAddr, 0, size);
        }

        saveFile.close();
    }

    bool Window::IsFullscreen() {
	    return bIsFullscreen;
    }

    uint32_t Window::GetMenuBar() {
	    return dwMenubar;
    }

    void Window::SetMenuBar(uint32_t dwMenuBar) {
	    this->dwMenubar = dwMenuBar;
    }

    std::string Window::GetName() {
	    return Name;
    }

    std::shared_ptr<ControlDeck> Window::GetControlDeck() {
	    return ControllerApi;
    }

    std::shared_ptr<AudioPlayer> Window::GetAudioPlayer() {
	    return APlayer;
    }

    std::shared_ptr<ResourceMgr> Window::GetResourceManager() {
	    return ResMan;
    }

    std::shared_ptr<Mercury> Window::GetConfig() {
	    return Config;
    }

    std::shared_ptr<spdlog::logger> Window::GetLogger() {
	    return Logger;
    }

    const char* Window::GetKeyName(int32_t scancode) {
	    return WmApi->get_key_name(scancode);
    }

    int32_t Window::GetLastScancode() {
	    return lastScancode;
    }

    void Window::SetLastScancode(int32_t scanCode) {
	    lastScancode = scanCode;
    }
}
