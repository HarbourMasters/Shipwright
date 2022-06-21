#pragma once
#include <cstdint>
#include <string>

#include "Lib/Fast3D/gfx_rendering_api.h"

namespace Ship {
	class TexIOLoader {
	public:
		virtual ~TexIOLoader() = default;
		virtual uint32_t UploadTexture(uint32_t tile, const std::string& path, GfxRenderingAPI* rapi) = 0;
	};
};