#pragma once

#include <memory>
#include <filesystem>
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
			static std::shared_ptr<Window> CreateInstance(const std::string Name);
			static std::string GetAppDirectoryPath();
			static std::string GetPathRelativeToAppDirectory(const char* path);

			Window(std::string Name);
			~Window();
			void WriteSaveFile(const std::filesystem::path& savePath, uintptr_t addr, void* dramAddr, size_t size);
			void ReadSaveFile(std::filesystem::path savePath, uintptr_t addr, void* dramAddr, size_t size);
			void CreateDefaults();
			void MainLoop(void (*MainFunction)(void));
			void Initialize();
			void StartFrame();
			void SetTargetFps(int32_t fps);
			void SetMaximumFrameLatency(int32_t latency);
			void GetPixelDepthPrepare(float x, float y);
			uint16_t GetPixelDepth(float x, float y);
			void ToggleFullscreen();
			void SetFullscreen(bool bIsFullscreen);
			void ShowCursor(bool hide);
			bool IsFullscreen() { return bIsFullscreen; }
			uint32_t GetCurrentWidth();
			uint32_t GetCurrentHeight();
			uint32_t GetMenuBar() { return dwMenubar; }
			void SetMenuBar(uint32_t dwMenuBar) { this->dwMenubar = dwMenuBar; }
			std::string GetName() { return Name; }
			std::shared_ptr<ControlDeck> GetControlDeck() { return ControllerApi; };
			std::shared_ptr<AudioPlayer> GetAudioPlayer() { return APlayer; }
			std::shared_ptr<ResourceMgr> GetResourceManager() { return ResMan; }
			std::shared_ptr<Mercury> GetConfig() { return Config; }
			std::shared_ptr<spdlog::logger> GetLogger() { return Logger; }
			const char* GetKeyName(int32_t scancode) { return WmApi->get_key_name(scancode); }
			int32_t GetLastScancode() { return lastScancode; }
			void SetLastScancode(int32_t scanCode) { lastScancode = scanCode; }

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
			void InitializeResourceManager();
			void InitializeWindowManager();

			std::shared_ptr<spdlog::logger> Logger;
			std::shared_ptr<Mercury> Config; // Config needs to be after the Window because we call the Window during it's destructor.
			std::shared_ptr<ResourceMgr> ResMan;
			std::shared_ptr<AudioPlayer> APlayer;
			std::shared_ptr<ControlDeck> ControllerApi;

			std::string gfxBackend;
			GfxRenderingAPI* RenderingApi;
			GfxWindowManagerAPI* WmApi;
			bool bIsFullscreen;
			uint32_t dwWidth;
			uint32_t dwHeight;
			uint32_t dwMenubar;
			int32_t lastScancode;
			std::string Name;
			std::string MainPath;
			std::string PatchesPath;
	};
}
