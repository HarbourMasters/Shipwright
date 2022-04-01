#include "TextureMod.h"

#include <map>
#include <string>
#include <iostream>
#include <filesystem>
#include <functional>
#include "Lib/stb/stb_image.h"
#include "Lib/Fast3D/gfx_pc.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Lib/stb/stb_image_write.h"
#include "Lib/Fast3D/gfx_rendering_api.h"
#include "SohImGuiImpl.h"

namespace fs = std::filesystem;

namespace Ship {
	void TextureModule::Init() {
		BIND_HOOK(LOOKUP_TEXTURE, Hook_LookupTexture);
		BIND_HOOK(GRAYOUT_TEXTURE, Hook_GrayScaleFilter);
		BIND_HOOK(INVALIDATE_TEXTURE, Hook_InvalidateTexture);

		SohImGui::BindCmd("reload", { .handler = [&](const std::vector<std::string>&) {
			INFO("Reloading all textures!");
			gfx_texture_cache_clear();
			this->TexturePool.clear();
			this->TextureCache.clear();
			return true;
		} });
	}

	void TextureModule::Open(std::shared_ptr<Ship::Archive> archive) {
		this->LoadedOTRS.push_back(archive);
	}

	void TextureModule::Close(Ship::Archive otr) {
		// Remove all loaded textures
		// this->LoadedOTRS.erase(std::find(this->LoadedOTRS.begin(), this->LoadedOTRS.end(), otr));
	}

	void TextureModule::Hook_LookupTexture(HookEvent call) {
		const auto raw_path = BIND_PTR("path", char*);
		if (raw_path == nullptr) return;

		const auto api = BIND_PTR("gfx_api", GfxRenderingAPI*);
		const auto path = normalize(raw_path) + ".png";
		const auto node = BIND_PTR("node", TextureCacheNode**);
		const auto fmt = BIND_VAR("fmt", uint32_t*);
		const auto siz = BIND_VAR("siz", uint32_t*);
		const auto tile = BIND_VAR("tile", int*);
		const auto palette = BIND_VAR("palette", uint32_t*);
		const auto orig_addr = BIND_VAR("addr", const uint8_t**);

		// INFO("The game is trying to load %s", path.c_str());

		if (this->TextureCache.contains(path) && this->TextureCache[path][tile] != nullptr) {
			*node = this->TextureCache[path][tile];
			api->select_texture(tile, (*node)->second.texture_id);
			call->cancelled = true;
			return;
		}

		// OTRTODO: Implement loading order
		TextureData* tex_data = nullptr;
		if (!this->TexturePool.contains(path)) {
			std::shared_ptr<Ship::File> raw_data = std::make_shared<Ship::File>();
			this->Manager->ResManager->GetArchive()->LoadPatchFile(path, false, raw_data);

			if (raw_data->bIsLoaded) {
				char* tdata = new char[raw_data->dwBufferSize];
				memcpy(tdata, raw_data->buffer.get(), raw_data->dwBufferSize);
				tex_data = new TextureData({ .data = tdata, .size = raw_data->dwBufferSize });
				INFO("Loaded %s", path.c_str());
				this->TexturePool[path] = tex_data;
			}
		}

		if (tex_data == nullptr)
			return;

		if (!this->TextureCache.contains(path)) this->TextureCache[path].resize(10);

		TextureCacheKey key = { orig_addr, { }, static_cast<uint8_t>(fmt), static_cast<uint8_t>(siz), static_cast<uint8_t>(palette) };
		TextureCacheValue value = { api->new_texture(), 0, 0, false };
		const auto entry = new TextureCacheNode(key, value);
		api->select_texture(tile, entry->second.texture_id);
		api->set_sampler_parameters(tile, false, 0, 0);
		*node = entry;

		uint8_t* img_data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(tex_data->data), tex_data->size, &tex_data->width, &tex_data->height, nullptr, 4);

		if (!img_data)
			return;

		switch (tex_data->color_modifier) {
		case GRAYSCALE: {
			GrayOutTexture(img_data, tex_data->width, tex_data->height);
		}
		default:;
		}

		std::cout << "Uploading to the GPU" << std::endl;
		api->upload_texture(img_data, tex_data->width, tex_data->height);
		this->TextureCache[path][tile] = entry;

		stbi_image_free(img_data);
		call->cancelled = true;
	}


	void TextureModule::Hook_GrayScaleFilter(HookEvent event) {
		// this->Exit();
	}

	void TextureModule::Hook_InvalidateTexture(HookEvent event) {
		// this->Exit();
	}

	void TextureModule::Exit() {
		TextureCache.clear();
		TexturePool.clear();
		LoadedOTRS.clear();
	}
}