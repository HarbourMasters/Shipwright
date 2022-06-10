#include "TexLoader.h"
#include "Lib/Fast3D/gfx_rendering_api.h"
#include "TexIOLoader.h"

#include "Archive.h"
#include "File.h"
#include "Utils.h"
#include "GlobalCtx2.h"
#include "ResourceMgr.h"
#include "DDSTexLoader.h"
#include "DefaultTexLoader.h"
#include <filesystem>
#include <ranges>

namespace fs = std::filesystem;

std::unordered_map<std::string, int> FileCache;

std::unordered_map<std::string, std::vector<TextureCacheNode*>> TextureCache;
std::unordered_map<std::string, Ship::TexIOLoader*> IOLoaders = {
	{ "dds", new Ship::DDSTexLoader },
	{ "stb", new Ship::DefaultTexLoader },
};

bool TexLoader::LoadReplacement(int tile, const char* path, GfxRenderingAPI* api, TextureCacheNode** node, uint32_t fmt, uint32_t siz, uint32_t palette, const uint8_t* orig_addr) {
	if (path == nullptr) return false;
	if (FileCache.contains(path)) return false;

	FileCache[path] = 0;

	if (TextureCache.contains(path) && TextureCache[path][tile] != nullptr) {
		*node = TextureCache[path][tile];
		api->select_texture(tile, (*node)->second.texture_id);
		return true;
	}

	Ship::TexIOLoader* loader = IOLoaders["stb"];

	/*  // OTRTEXTODO: Select loader based on file extension
		for (auto& file : Ship::GlobalCtx2::GetInstance()->GetResourceManager()->GetArchive()->ListFiles(path)) {
			fs::path fPath = file.szPlainName;
			std::string ext = fPath.extension().string();

			if (IOLoaders.contains(ext)) {
				texFile = file.cFileName;
				loader =
				break;
			}
		}
	*/

	std::string texPath = path;
	texPath.append(".png");

	uint32_t textureId = loader->UploadTexture(tile, texPath, api);

	if (textureId == -1) return false;

	if (!TextureCache.contains(path)) TextureCache[path].resize(10);

	TextureCacheKey key = { orig_addr, { }, static_cast<uint8_t>(fmt), static_cast<uint8_t>(siz), static_cast<uint8_t>(palette) };
	TextureCacheValue value = { textureId, 0, 0, false };
	const auto entry = new TextureCacheNode(key, value);
	*node = entry;
	TextureCache[path][tile] = entry;

	return true;
}
