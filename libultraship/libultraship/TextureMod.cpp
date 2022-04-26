#include "TextureMod.h"

#include <unordered_map>
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
		BIND_HOOK(GRAYOUT_TEXTURE, Hook_GrayScaleFilter);
		//BIND_HOOK(LOAD_TEXTURE, Hook_LoadTexture);
		BIND_HOOK(INVALIDATE_TEXTURE, Hook_InvalidateTexture);

		SohImGui::BindCmd("reload", { .handler = [&](const std::vector<std::string>&) {
			INFO("Reloading all textures!");
			gfx_texture_cache_clear();
			this->TexturePool.clear();
			this->TextureCache.clear();
			return true;
		} });
	}

	void TextureModule::Open(std::shared_ptr<Archive> archive) {
		this->LoadedOTRS.push_back(archive);
	}

	void TextureModule::Close(Archive otr) {
		// Remove all loaded textures
		// this->LoadedOTRS.erase(std::find(this->LoadedOTRS.begin(), this->LoadedOTRS.end(), otr));
	}

	bool TextureModule::LookupTexture(int tile, char* path, GfxRenderingAPI* api, TextureCacheNode** node, uint32_t fmt, uint32_t siz, uint32_t palette, const uint8_t* orig_addr) {

		if (path == nullptr || (path != nullptr && (strlen(path) > 128 || path[0] == '\0')))
			return false;

		if (this->TextureCache.contains(path) && this->TextureCache[path][tile] != nullptr) {
			*node = this->TextureCache[path][tile];
			api->select_texture(tile, (*node)->second.texture_id);
			return true;
		}

		// OTRTODO: Implement loading order
		TextureData* tex_data = nullptr;
		if (!this->TexturePool.contains(path)) {
			for(auto &otr : LoadedOTRS) {
				const auto fix_path = normalize(path) + ".png";
				std::shared_ptr<File> raw_data = std::make_shared<File>();
				if (!otr->HasFile(fix_path)) continue;

				otr->LoadFile(fix_path, false, raw_data);

				if (raw_data->bIsLoaded) {
					auto tdata = new char[raw_data->dwBufferSize];
					memcpy(tdata, raw_data->buffer.get(), raw_data->dwBufferSize);
					tex_data = new TextureData({ .data = tdata, .size = raw_data->dwBufferSize });
					INFO("Loaded %s", path);
					this->TexturePool[path] = tex_data;
				}
			}
		}

		if (tex_data == nullptr)
			return false;

		if (!this->TextureCache.contains(path)) this->TextureCache[path].resize(10);

		TextureCacheKey key = { orig_addr, { }, static_cast<uint8_t>(fmt), static_cast<uint8_t>(siz), static_cast<uint8_t>(palette) };
		TextureCacheValue value = { api->new_texture(), 0, 0, false };
		const auto entry = new TextureCacheNode(key, value);
		api->select_texture(tile, entry->second.texture_id);
		api->set_sampler_parameters(tile, false, 0, 0);
		*node = entry;

		uint8_t* img_data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(tex_data->data), tex_data->size, &tex_data->width, &tex_data->height, nullptr, 4);

		if (!img_data)
			return false;

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
		return true;
	}

	void TextureModule::Hook_GrayScaleFilter(HookEvent event) {
		// this->Exit();
	}

	void TextureModule::Hook_LoadTexture(HookEvent event) {
		char* path = (char*)event->baseArgs["path"] + 7; // __OTR__
		void** outPtr = (void**)event->baseArgs["texture"];

		TextureData* tex_data = nullptr;
		if (!this->TexturePool.contains(path)) {
			for(auto &otr : LoadedOTRS) {
				const auto fix_path = normalize(path) + ".png";
				std::shared_ptr<File> raw_data = std::make_shared<File>();
				if (!otr->HasFile(fix_path)) continue;

				otr->LoadFile(fix_path, false, raw_data);

				if (raw_data->bIsLoaded) {
					auto tdata = new char[raw_data->dwBufferSize];
					memcpy(tdata, raw_data->buffer.get(), raw_data->dwBufferSize);
					tex_data = new TextureData({ .data = tdata, .size = raw_data->dwBufferSize });
					INFO("Loaded %s", path);
				}
			}
		}

		if (tex_data == nullptr)
			return;

		uint8_t* img_data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(tex_data->data), tex_data->size, &tex_data->width, &tex_data->height, nullptr, 4);

		if (!img_data)
		{
			delete[] tex_data->data;
			delete tex_data;
			return;
		}

		switch (tex_data->color_modifier) {
		case GRAYSCALE: {
			GrayOutTexture(img_data, tex_data->width, tex_data->height);
		}
		default:;
		}

		*outPtr = malloc(tex_data->size);
		memcpy(*outPtr, img_data, tex_data->size);

		//*outPtr = img_data;
		stbi_image_free(img_data);
		delete[] tex_data->data;
		delete tex_data;
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