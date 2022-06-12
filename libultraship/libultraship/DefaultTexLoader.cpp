#include "DefaultTexLoader.h"
#include "Lib/stb/stb_image.h"
#include "Archive.h"
#include "File.h"
#include "GlobalCtx2.h"
#include "ResourceMgr.h"

std::string normalize(std::string path) {
	std::replace(path.begin(), path.end(), '/', '\\');
	return path;
}

uint32_t Ship::DefaultTexLoader::UploadTexture(uint32_t tile, const std::string& p, GfxRenderingAPI* rapi) {
	int width;
	int height;
	std::shared_ptr<ResourceMgr> mgr = GlobalCtx2::GetInstance()->GetResourceManager();
	std::string path = normalize(p);
	std::shared_ptr<File> file = mgr->LoadFile(path);

	if (file->bIsLoaded) {
		uint8_t* img_data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(file->buffer.get()), file->dwBufferSize, &width, &height, nullptr, 4);

		uint32_t tid = rapi->new_texture();
		rapi->upload_texture(img_data, width, height);
		stbi_image_free(img_data);

		return tid;
	}

	return -1;
}
