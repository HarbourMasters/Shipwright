#include "GlobalCtx2.h"
#include <iostream>
#include <vector>
#include "ResourceMgr.h"
#include "Window.h"
#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/sohconsole_sink.h"
#include "ModManager.h"
#ifdef __APPLE__
#include "OSXFolderManager.h"
#endif

namespace Ship {
    std::weak_ptr<GlobalCtx2> GlobalCtx2::Context;
    ModManager* INSTANCE;
    std::shared_ptr<GlobalCtx2> GlobalCtx2::GetInstance() {
        return Context.lock();
    }

    std::shared_ptr<GlobalCtx2> GlobalCtx2::CreateInstance(const std::string& Name) {
        if (Context.expired()) {
            auto Shared = std::make_shared<GlobalCtx2>(Name);
            Context = Shared;
            Shared->InitWindow();
            return Shared;
        } else {
            SPDLOG_DEBUG("Trying to create a context when it already exists.");
        }

        return GetInstance();
    }

    std::string GlobalCtx2::GetAppDirectoryPath() {
        #ifdef __APPLE__
            FolderManager folderManager;
            std::string fpath = std::string(folderManager.pathForDirectory(NSApplicationSupportDirectory, NSUserDomainMask));
            fpath.append("/com.shipofharkinian.soh");
            return fpath;
        #endif

        return ".";

    }

    std::string GlobalCtx2::GetPathRelativeToAppDirectory(const char* path) {
        return GlobalCtx2::GetAppDirectoryPath() + "/" + path;
    }

    GlobalCtx2::GlobalCtx2(std::string Name) : Name(std::move(Name)) {

    }

    GlobalCtx2::~GlobalCtx2() {
        SPDLOG_INFO("destruct GlobalCtx2");
        INSTANCE->Exit();
    }

    void GlobalCtx2::InitWindow() {
        InitLogging();
        Config = std::make_shared<Mercury>(GetPathRelativeToAppDirectory("shipofharkinian.json"));
        Config->reload();

        MainPath = Config->getString("Game.Main Archive", GetPathRelativeToAppDirectory("oot.otr"));
        PatchesPath = Config->getString("Game.Patches Archive", GetAppDirectoryPath() + "/mods");

        ResMan = std::make_shared<ResourceMgr>(GetInstance(), MainPath, PatchesPath);
        Win = std::make_shared<Window>(GetInstance());

        if (!ResMan->DidLoadSuccessfully())
        {
#ifdef _WIN32
            MessageBox(nullptr, L"Main OTR file not found!", L"Uh oh", MB_OK);
#else
            SPDLOG_ERROR("Main OTR file not found!");
#endif
            exit(1);
        }
        INSTANCE = new ModManager(ResMan);
        INSTANCE->Init();
    }

    void GlobalCtx2::InitLogging() {
        try {
            auto logPath = GetPathRelativeToAppDirectory(("logs/" + GetName() + ".log").c_str());

            // Setup Logging
            spdlog::init_thread_pool(8192, 1);
            auto SohConsoleSink = std::make_shared<spdlog::sinks::soh_sink_mt>();
            auto ConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto FileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logPath, 1024 * 1024 * 10, 10);
            SohConsoleSink->set_level(spdlog::level::trace);
            ConsoleSink->set_level(spdlog::level::trace);
            FileSink->set_level(spdlog::level::trace);
            std::vector<spdlog::sink_ptr> Sinks{ ConsoleSink, FileSink, SohConsoleSink };
            Logger = std::make_shared<spdlog::async_logger>(GetName(), Sinks.begin(), Sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
            GetLogger()->set_level(spdlog::level::trace);
            GetLogger()->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%@] [%l] %v");
            spdlog::register_logger(GetLogger());
            spdlog::set_default_logger(GetLogger());
        }
        catch (const spdlog::spdlog_ex& ex) {
            std::cout << "Log initialization failed: " << ex.what() << std::endl;
        }
    }

    void GlobalCtx2::WriteSaveFile(const std::filesystem::path& savePath, const uintptr_t addr, void* dramAddr, const size_t size) {
        std::ofstream saveFile = std::ofstream(savePath, std::fstream::in | std::fstream::out | std::fstream::binary);
        saveFile.seekp(addr);
        saveFile.write((char*)dramAddr, size);
        saveFile.close();
    }

    void GlobalCtx2::ReadSaveFile(std::filesystem::path savePath, uintptr_t addr, void* dramAddr, size_t size) {
        std::ifstream saveFile = std::ifstream(savePath, std::fstream::in | std::fstream::out | std::fstream::binary);

        // If the file doesn't exist, initialize DRAM
        if (saveFile.good()) {
            saveFile.seekg(addr);
            saveFile.read((char*)dramAddr, size);
        } else {
            memset(dramAddr, 0, size);
        }

        saveFile.close();
    }
}
