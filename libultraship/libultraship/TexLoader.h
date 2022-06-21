#pragma once
#include <PR/ultra64/gbi.h>
#include "Lib/Fast3D/gfx_pc.h"

namespace Ship {
	class TexLoader {
	public:
		static void Init();
		static bool LoadReplacement(int tile, const char* path, GfxRenderingAPI* api, TextureCacheNode** node, uint32_t fmt, uint32_t siz, uint32_t palette, const uint8_t* orig_addr);
	};
}
