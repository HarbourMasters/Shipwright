#pragma once

#include "TexIOLoader.h"

namespace Ship {
	class DDSTexLoader : public TexIOLoader {
		uint32_t UploadTexture(uint32_t tile, const std::string& path, GfxRenderingAPI* rapi) override;
	};
}

