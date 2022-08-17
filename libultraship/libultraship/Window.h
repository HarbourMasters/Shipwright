#pragma once
#include <memory>
#include "PR/ultra64/gbi.h"
#include "Lib/Fast3D/gfx_pc.h"
#include "Controller.h"
#include "GlobalCtx2.h"
#include "ControlDeck.h"

#include "Lib/Fast3D/gfx_window_manager_api.h"

namespace Ship {
	class AudioPlayer;

	class Window {
		public:
			Window(std::shared_ptr<GlobalCtx2> Context);
			~Window();
			void CreateDefaults();
			void MainLoop(void (*MainFunction)(void));
			void Init();
			void StartFrame();
			void RunCommands(Gfx* Commands, const std::vector<std::unordered_map<Mtx*, MtxF>>& mtx_replacements);
			void SetTargetFps(int fps);
			void SetMaximumFrameLatency(int latency);
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
			std::shared_ptr<ControlDeck> GetControlDeck() { return ControllerApi; };
			std::shared_ptr<GlobalCtx2> GetContext() { return Context.lock(); }
			std::shared_ptr<AudioPlayer> GetAudioPlayer() { return APlayer; }
			const char* GetKeyName(int scancode) { return WmApi->get_key_name(scancode); }
			int32_t GetLastScancode() { return lastScancode;  };
			void SetLastScancode(int32_t scanCode) { lastScancode = scanCode; };

		protected:
		private:
			static bool KeyDown(int32_t dwScancode);
			static bool KeyUp(int32_t dwScancode);
			static void AllKeysUp(void);
			static void OnFullscreenChanged(bool bIsNowFullscreen);

			void InitializeControlDeck();
			void InitializeAudioPlayer();
			void InitializeWindowManager();

			std::weak_ptr<GlobalCtx2> Context;
			std::shared_ptr<AudioPlayer> APlayer;
			std::shared_ptr<ControlDeck> ControllerApi;
			std::string gfxBackend;
            std::string gfxApi;

			GfxRenderingAPI* RenderingApi;
			GfxWindowManagerAPI* WmApi;
			bool bIsFullscreen;
			uint32_t dwWidth;
			uint32_t dwHeight;
			uint32_t dwMenubar;
			int32_t lastScancode;
	};
}
