#pragma once
#include "ModModule.h"
#include <PR/ultra64/gbi.h>
#include "Lib/Fast3D/gfx_pc.h"

namespace Ship {
	enum TextureMod {
		GRAYSCALE,
		NONE
	};

	struct TextureData {
		char* data;
		uint32_t size;
		int width;
		int height;
		char* loaded_data;
		std::shared_ptr<Ship::Archive> parent;
		TextureMod color_modifier = NONE;
	};

	class TextureModule : public ModModule {
	public:
		explicit TextureModule(ModManager* Manager) : ModModule(Manager) {}
	private:
		std::vector<std::shared_ptr<Ship::Archive>> LoadedOTRS;
		std::map<std::string, TextureData*> TexturePool;
		std::map<std::string, std::vector<TextureCacheNode*>> TextureCache;
		void Init() override;
		void Open(std::shared_ptr<Ship::Archive> archive) override;
		void Close(Ship::Archive mod) override;
		void Exit() override;
	protected:
		void Hook_LookupTexture(HookEvent event);
		void Hook_GrayScaleFilter(HookEvent event);
		void Hook_InvalidateTexture(HookEvent event);
	};

	inline void GrayOutTexture(uint8_t* data, int width, int height) {
		for (int x = 0; x < width * height * 4; x += 4) {

			int red = data[x];
			int green = data[x + 1];
			int blue = data[x + 2];
			int gray = (red + green + blue) / 3;
			data[x] = gray;
			data[x + 1] = gray;
			data[x + 2] = gray;
		}
	}
}