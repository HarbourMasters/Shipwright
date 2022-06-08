#pragma once
#include <memory>
#include "PR/ultra64/gbi.h"
#include "Lib/Fast3D/gfx_pc.h"
#include "UltraController.h"
#include "Controller.h"
#include "GlobalCtx2.h"

namespace Ship {
	class AudioPlayer;

	class Window {
		public:
			static std::map<size_t, std::vector<std::shared_ptr<Controller>>> Controllers;
			static int32_t lastScancode;

			Window(std::shared_ptr<GlobalCtx2> Context);
			~Window();
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
			uint32_t dwMenubar;
			std::shared_ptr<GlobalCtx2> GetContext() { return Context.lock(); }
			std::shared_ptr<AudioPlayer> GetAudioPlayer() { return APlayer; }

		protected:
		private:
			static bool KeyDown(int32_t dwScancode);
			static bool KeyUp(int32_t dwScancode);
			static void AllKeysUp(void);
			static void OnFullscreenChanged(bool bIsNowFullscreen);
			void SetAudioPlayer();

			std::weak_ptr<GlobalCtx2> Context;
			std::shared_ptr<AudioPlayer> APlayer;

			GfxWindowManagerAPI* WmApi;
			GfxRenderingAPI* RenderingApi;
			bool bIsFullscreen;
			uint32_t dwWidth;
			uint32_t dwHeight;
	};
}

