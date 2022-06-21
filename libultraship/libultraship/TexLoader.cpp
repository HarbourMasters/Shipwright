#include "TexLoader.h"
#include "Lib/Fast3D/gfx_rendering_api.h"
#include "TexIOLoader.h"

#include <Utils/StringHelper.h>
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

namespace Ship {
	struct TexEntry {
		Ship::TexIOLoader* loader;
		std::string path;
	};

	std::unordered_map<std::string, TexEntry> QueryCache;
	std::unordered_map<std::string, int> FileCache;
	std::unordered_map<std::string, Ship::TexIOLoader*> IOLoaders = {
		{ "dds", new Ship::DDSTexLoader },
		{ "stb", new Ship::DefaultTexLoader },
	};

	void TexLoader::Init() {
		for (auto &path : Ship::GlobalCtx2::GetInstance()->GetResourceManager()->GetArchive()->ListFiles("*.*")) {
			std::string rPath = path.cFileName;
			std::vector<std::string> raw = StringHelper::Split(rPath, ".");
			std::string ext = raw[raw.size() - 1];
			std::string nPath = rPath.substr(0, rPath.size() - (ext.size() + 1));

			// OTRTODO:Remove this when the new audio system get merged!
			replace(nPath.begin(), nPath.end(), '\\', '/');
			QueryCache[nPath] = { IOLoaders[ext == "dds" ? "dds" : "stb"], rPath };
		}
	}

	bool TexLoader::LoadReplacement(int tile, const char* path, GfxRenderingAPI* api, TextureCacheNode** node, uint32_t fmt, uint32_t siz, uint32_t palette, const uint8_t* orig_addr) {

		if (path == nullptr || FileCache.contains(path)) return false;

		FileCache[path] = 0;

		if (!QueryCache.contains(path)) return false;

		TexEntry tEntry = QueryCache[path];
		uint32_t textureId = tEntry.loader->UploadTexture(tile, tEntry.path.c_str(), api);

		if (textureId == (uint32_t) -1) return false;

		TextureCacheKey key = { orig_addr, { }, static_cast<uint8_t>(fmt), static_cast<uint8_t>(siz), static_cast<uint8_t>(palette) };
		TextureCacheValue value = { textureId, 0, 0, false };
		const auto entry = new TextureCacheNode(key, value);
		*node = entry;

		return true;
	}
}
