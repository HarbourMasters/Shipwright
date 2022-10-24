#pragma once

#include <memory>
#include <filesystem>
#include <unordered_set>
#include "spdlog/spdlog.h"
#include "ControlDeck.h"
#include "AudioPlayer.h"
#include "Lib/Fast3D/gfx_window_manager_api.h"
#include "Lib/Mercury/Mercury.h"

struct GfxRenderingAPI;
struct GfxWindowManagerAPI;

namespace Ship {
	class ResourceMgr;

	class Window {
		public:
			static std::shared_ptr<Window> GetInstance();
			static std::shared_ptr<Window> CreateInstance(const std::string Name, const std::vector<std::string>& OTRFiles = {}, const std::unordered_set<uint32_t>& ValidHashes = {});
			static std::string GetAppDirectoryPath();
			static std::string GetPathRelativeToAppDirectory(const char* path);

			Window(std::string Name);
			~Window();
			void WriteSaveFile(const std::filesystem::path& savePath, uintptr_t addr, void* dramAddr, size_t size);
			void ReadSaveFile(std::filesystem::path savePath, uintptr_t addr, void* dramAddr, size_t size);
			void CreateDefaults();
			void MainLoop(void (*MainFunction)(void));
			void Initialize(const std::vector<std::string>& OTRFiles = {}, const std::unordered_set<uint32_t>& ValidHashes = {});
			void StartFrame();
			void SetTargetFps(int32_t fps);
			void SetMaximumFrameLatency(int32_t latency);
			void GetPixelDepthPrepare(float x, float y);
			uint16_t GetPixelDepth(float x, float y);
			void ToggleFullscreen();
			void SetFullscreen(bool bIsFullscreen);
			void ShowCursor(bool hide);
			uint32_t GetCurrentWidth();
			uint32_t GetCurrentHeight();
			bool IsFullscreen();
			uint32_t GetMenuBar();
			void SetMenuBar(uint32_t dwMenuBar);
			std::string GetName();
			std::shared_ptr<ControlDeck> GetControlDeck();
			std::shared_ptr<AudioPlayer> GetAudioPlayer();
			std::shared_ptr<ResourceMgr> GetResourceManager();
			std::shared_ptr<Mercury> GetConfig();
			std::shared_ptr<spdlog::logger> GetLogger();
			const char* GetKeyName(int32_t scancode);
			int32_t GetLastScancode();
			void SetLastScancode(int32_t scanCode);

		protected:
			Window() = default;
		private:
			static bool KeyDown(int32_t dwScancode);
			static bool KeyUp(int32_t dwScancode);
			static void AllKeysUp(void);
			static void OnFullscreenChanged(bool bIsNowFullscreen);
			static std::weak_ptr<Window> Context;

			void InitializeConfiguration();
			void InitializeControlDeck();
			void InitializeAudioPlayer();
			void InitializeLogging();
			void InitializeResourceManager(const std::vector<std::string>& OTRFiles = {}, const std::unordered_set<uint32_t>& ValidHashes = {});
			void InitializeWindowManager();

			std::shared_ptr<spdlog::logger> Logger;
			std::shared_ptr<Mercury> Config; // Config needs to be after the Window because we call the Window during it's destructor.
			std::shared_ptr<ResourceMgr> ResMan;
			std::shared_ptr<AudioPlayer> APlayer;
			std::shared_ptr<ControlDeck> ControllerApi;

			std::string gfxBackend;
			std::string audioBackend;
			GfxRenderingAPI* RenderingApi;
			GfxWindowManagerAPI* WmApi;
			bool bIsFullscreen;
			uint32_t dwWidth;
			uint32_t dwHeight;
			uint32_t dwMenubar;
			int32_t lastScancode;
			std::string Name;
			std::string MainPath;
            std::string BasePath;
			std::string PatchesPath;
	};
}
