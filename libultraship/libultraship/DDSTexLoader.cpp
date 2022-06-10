#include "DDSTexLoader.h"
#include "Lib/stb/stb_image.h"
#include "Archive.h"
#include "File.h"
#include "GlobalCtx2.h"
#include "ResourceMgr.h"

#define DXT1_EXT 0x83F1;
#define DXT3_EXT 0x83F2;
#define DXT5_EXT 0x83F3;

uint32_t Ship::DDSTexLoader::UploadTexture(uint32_t tile, const std::string& path, GfxRenderingAPI* rapi) {
	unsigned char* header;

	unsigned int width;
	unsigned int height;
	unsigned int mipMapCount;

	unsigned int blockSize;
	unsigned int format;

	unsigned int w;
	unsigned int h;

	unsigned char* buffer = 0;
	
	std::shared_ptr<File> file =
		GlobalCtx2::GetInstance()->GetResourceManager()->LoadFile(path);

	if (file->bIsLoaded) {
		header = new unsigned char[128];
		memcpy(header, file->buffer.get(), 128);

		if (memcmp(header, "DDS ", 4) != 0)
			return false;
		
		height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
		width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
		mipMapCount = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

		if (header[84] == 'D') {
			switch (header[87]) {
			case '1': // DXT1
				format = DXT1_EXT;
				blockSize = 8;
				break;
			case '3': // DXT3
				format = DXT3_EXT;
				blockSize = 16;
				break;
			case '5': // DXT5
				format = DXT5_EXT;
				blockSize = 16;
				break;
			case '0': // DX10
				// unsupported, else will error
				// as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
				// so, buffer = malloc((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
			default:
				return false;
			}
		}
		else
			return false;
		
		size_t bufferSize = file->dwBufferSize - 128;
		buffer = new unsigned char[bufferSize];
		memcpy(buffer, file->buffer.get() + 128, bufferSize);

		uint32_t tid = rapi->new_texture();

		rapi->select_compressed_texture(tile, mipMapCount, tid);
		rapi->set_sampler_parameters(tile, false, 0, 0);

		unsigned int offset = 0;
		unsigned int size = 0;
		w = width;
		h = height;

		// loop through sending block at a time with the magic formula
		// upload to opengl properly, note the offset transverses the pointer
		// assumes each mipmap is 1/2 the size of the previous mipmap
		for (unsigned int i = 0; i < mipMapCount; i++) {
			if (w == 0 || h == 0) { // discard any odd mipmaps 0x1 0x2 resolutions
				mipMapCount--;
				continue;
			}
			size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
			rapi->upload_compressed_texture(i, format, buffer + offset, w, h, size);
			offset += size;
			w /= 2;
			h /= 2;
		}

		rapi->finish_compressed_texture(mipMapCount);
		
		return tid;
	}

	return -1;
}
