#include "ImageBackend.h"

#include <cassert>
#include <cstdlib>
#include <png.h>
#include <stdexcept>

#include "Utils/StringHelper.h"
#include "WarningHandler.h"

/* ImageBackend */

ImageBackend::~ImageBackend()
{
	FreeImageData();
}

void ImageBackend::ReadPng(const char* filename)
{
	FreeImageData();

	FILE* fp = fopen(filename, "rb");
	if (fp == nullptr)
	{
		std::string errorHeader = StringHelper::Sprintf("could not open file '%s'", filename);
		HANDLE_ERROR(WarningType::InvalidPNG, errorHeader, "");
	}

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (png == nullptr)
	{
		HANDLE_ERROR(WarningType::InvalidPNG, "could not create png struct", "");
	}

	png_infop info = png_create_info_struct(png);
	if (info == nullptr)
	{
		HANDLE_ERROR(WarningType::InvalidPNG, "could not create png info", "");
	}

	if (setjmp(png_jmpbuf(png)))
	{
		// TODO: better warning explanation
		HANDLE_ERROR(WarningType::InvalidPNG, "setjmp(png_jmpbuf(png))", "");
	}

	png_init_io(png, fp);

	png_read_info(png, info);

	width = png_get_image_width(png, info);
	height = png_get_image_height(png, info);
	colorType = png_get_color_type(png, info);
	bitDepth = png_get_bit_depth(png, info);

#ifdef TEXTURE_DEBUG
	printf("Width: %u\n", width);
	printf("Height: %u\n", height);
	printf("ColorType: ");
	switch (colorType)
	{
	case PNG_COLOR_TYPE_RGBA:
		printf("PNG_COLOR_TYPE_RGBA\n");
		break;

	case PNG_COLOR_TYPE_RGB:
		printf("PNG_COLOR_TYPE_RGB\n");
		break;

	case PNG_COLOR_TYPE_PALETTE:
		printf("PNG_COLOR_TYPE_PALETTE\n");
		break;

	default:
		printf("%u\n", colorType);
		break;
	}
	printf("BitDepth: %u\n", bitDepth);
	printf("\n");
#endif

	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if (bitDepth == 16)
		png_set_strip_16(png);

	if (colorType == PNG_COLOR_TYPE_PALETTE)
	{
		// png_set_palette_to_rgb(png);
		isColorIndexed = true;
	}

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	/*if (png_get_valid(png, info, PNG_INFO_tRNS))
	    png_set_tRNS_to_alpha(png);*/

	// These color_type don't have an alpha channel then fill it with 0xff.
	/*if(*color_type == PNG_COLOR_TYPE_RGB ||
	    *color_type == PNG_COLOR_TYPE_GRAY ||
	    *color_type == PNG_COLOR_TYPE_PALETTE)
	    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);*/

	if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	size_t rowBytes = png_get_rowbytes(png, info);
	pixelMatrix = (uint8_t**)malloc(sizeof(uint8_t*) * height);
	for (size_t y = 0; y < height; y++)
	{
		pixelMatrix[y] = (uint8_t*)malloc(rowBytes);
	}

	png_read_image(png, pixelMatrix);

#ifdef TEXTURE_DEBUG
	printf("rowBytes: %zu\n", rowBytes);

	size_t bytePerPixel = GetBytesPerPixel();
	printf("imgData\n");
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			for (size_t z = 0; z < bytePerPixel; z++)
			{
				printf("%02X ", pixelMatrix[y][x * bytePerPixel + z]);
			}
			printf(" ");
		}
		printf("\n");
	}
	printf("\n");
#endif

	fclose(fp);

	png_destroy_read_struct(&png, &info, nullptr);

	hasImageData = true;
}

void ImageBackend::ReadPng(const fs::path& filename)
{
	ReadPng(filename.string().c_str());
}

void ImageBackend::WritePng(const char* filename)
{
	assert(hasImageData);

	FILE* fp = fopen(filename, "wb");
	if (fp == nullptr)
	{
		std::string errorHeader =
			StringHelper::Sprintf("could not open file '%s' in write mode", filename);
		HANDLE_ERROR(WarningType::InvalidPNG, errorHeader, "");
	}

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (png == nullptr)
	{
		HANDLE_ERROR(WarningType::InvalidPNG, "could not create png struct", "");
	}

	png_infop info = png_create_info_struct(png);
	if (info == nullptr)
	{
		HANDLE_ERROR(WarningType::InvalidPNG, "could not create png info", "");
	}

	if (setjmp(png_jmpbuf(png)))
	{
		// TODO: better warning description
		HANDLE_ERROR(WarningType::InvalidPNG, "setjmp(png_jmpbuf(png))", "");
	}

	png_init_io(png, fp);

	png_set_IHDR(png, info, width, height,
	             bitDepth,   // 8,
	             colorType,  // PNG_COLOR_TYPE_RGBA,
	             PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	if (isColorIndexed)
	{
		png_set_PLTE(png, info, static_cast<png_color*>(colorPalette), paletteSize);

#ifdef TEXTURE_DEBUG
		printf("palette\n");
		png_color* aux = (png_color*)colorPalette;
		for (size_t y = 0; y < paletteSize; y++)
		{
			printf("#%02X%02X%02X ", aux[y].red, aux[y].green, aux[y].blue);
			if ((y + 1) % 8 == 0)
				printf("\n");
		}
		printf("\n");
#endif

		png_set_tRNS(png, info, alphaPalette, paletteSize, nullptr);
	}

	png_write_info(png, info);

	// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
	// Use png_set_filler().
	// png_set_filler(png, 0, PNG_FILLER_AFTER);

#ifdef TEXTURE_DEBUG
	size_t bytePerPixel = GetBytesPerPixel();
	printf("imgData\n");
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width * bytePerPixel; x++)
		{
			printf("%02X ", pixelMatrix[y][x]);
		}
		printf("\n");
	}
	printf("\n");
#endif

	png_write_image(png, pixelMatrix);
	png_write_end(png, nullptr);

	fclose(fp);

	png_destroy_write_struct(&png, &info);
}

void ImageBackend::WritePng(const fs::path& filename)
{
	// Note: The .string() is necessary for MSVC, due to the implementation of std::filesystem
	// differing from GCC. Do not remove!
	WritePng(filename.string().c_str());
}

void ImageBackend::SetTextureData(const std::vector<std::vector<RGBAPixel>>& texData,
                                  uint32_t nWidth, uint32_t nHeight, uint8_t nColorType,
                                  uint8_t nBitDepth)
{
	FreeImageData();

	width = nWidth;
	height = nHeight;
	colorType = nColorType;
	bitDepth = nBitDepth;

	size_t bytePerPixel = GetBytesPerPixel();

	pixelMatrix = static_cast<uint8_t**>(malloc(sizeof(uint8_t*) * height));
	for (size_t y = 0; y < height; y++)
	{
		pixelMatrix[y] = static_cast<uint8_t*>(malloc(sizeof(uint8_t*) * width * bytePerPixel));
		for (size_t x = 0; x < width; x++)
		{
			pixelMatrix[y][x * bytePerPixel + 0] = texData.at(y).at(x).r;
			pixelMatrix[y][x * bytePerPixel + 1] = texData.at(y).at(x).g;
			pixelMatrix[y][x * bytePerPixel + 2] = texData.at(y).at(x).b;

			if (colorType == PNG_COLOR_TYPE_RGBA)
				pixelMatrix[y][x * bytePerPixel + 3] = texData.at(y).at(x).a;
		}
	}
	hasImageData = true;
}

void ImageBackend::InitEmptyRGBImage(uint32_t nWidth, uint32_t nHeight, bool alpha)
{
	FreeImageData();

	width = nWidth;
	height = nHeight;
	colorType = PNG_COLOR_TYPE_RGB;
	if (alpha)
		colorType = PNG_COLOR_TYPE_RGBA;
	bitDepth = 8;  // nBitDepth;

	size_t bytePerPixel = GetBytesPerPixel();

	pixelMatrix = static_cast<uint8_t**>(malloc(sizeof(uint8_t*) * height));
	for (size_t y = 0; y < height; y++)
	{
		pixelMatrix[y] = static_cast<uint8_t*>(calloc(width * bytePerPixel, sizeof(uint8_t*)));
	}

	hasImageData = true;
}

void ImageBackend::InitEmptyPaletteImage(uint32_t nWidth, uint32_t nHeight)
{
	FreeImageData();

	width = nWidth;
	height = nHeight;
	colorType = PNG_COLOR_TYPE_PALETTE;
	bitDepth = 8;

	size_t bytePerPixel = GetBytesPerPixel();

	pixelMatrix = (uint8_t**)malloc(sizeof(uint8_t*) * height);
	for (size_t y = 0; y < height; y++)
	{
		pixelMatrix[y] = static_cast<uint8_t*>(calloc(width * bytePerPixel, sizeof(uint8_t*)));
	}
	colorPalette = calloc(paletteSize, sizeof(png_color));
	alphaPalette = static_cast<uint8_t*>(calloc(paletteSize, sizeof(uint8_t)));

	hasImageData = true;
	isColorIndexed = true;
}

RGBAPixel ImageBackend::GetPixel(size_t y, size_t x) const
{
	assert(y < height);
	assert(x < width);
	assert(!isColorIndexed);

	RGBAPixel pixel;
	size_t bytePerPixel = GetBytesPerPixel();
	pixel.r = pixelMatrix[y][x * bytePerPixel + 0];
	pixel.g = pixelMatrix[y][x * bytePerPixel + 1];
	pixel.b = pixelMatrix[y][x * bytePerPixel + 2];
	if (colorType == PNG_COLOR_TYPE_RGBA)
		pixel.a = pixelMatrix[y][x * bytePerPixel + 3];
	return pixel;
}

uint8_t ImageBackend::GetIndexedPixel(size_t y, size_t x) const
{
	assert(y < height);
	assert(x < width);
	assert(isColorIndexed);

	return pixelMatrix[y][x];
}

void ImageBackend::SetRGBPixel(size_t y, size_t x, uint8_t nR, uint8_t nG, uint8_t nB, uint8_t nA)
{
	assert(hasImageData);
	assert(y < height);
	assert(x < width);

	size_t bytePerPixel = GetBytesPerPixel();
	pixelMatrix[y][x * bytePerPixel + 0] = nR;
	pixelMatrix[y][x * bytePerPixel + 1] = nG;
	pixelMatrix[y][x * bytePerPixel + 2] = nB;
	if (colorType == PNG_COLOR_TYPE_RGBA)
		pixelMatrix[y][x * bytePerPixel + 3] = nA;
}

void ImageBackend::SetGrayscalePixel(size_t y, size_t x, uint8_t grayscale, uint8_t alpha)
{
	assert(hasImageData);
	assert(y < height);
	assert(x < width);

	size_t bytePerPixel = GetBytesPerPixel();
	pixelMatrix[y][x * bytePerPixel + 0] = grayscale;
	pixelMatrix[y][x * bytePerPixel + 1] = grayscale;
	pixelMatrix[y][x * bytePerPixel + 2] = grayscale;
	if (colorType == PNG_COLOR_TYPE_RGBA)
		pixelMatrix[y][x * bytePerPixel + 3] = alpha;
}

void ImageBackend::SetIndexedPixel(size_t y, size_t x, uint8_t index, uint8_t grayscale)
{
	assert(hasImageData);
	assert(y < height);
	assert(x < width);

	size_t bytePerPixel = GetBytesPerPixel();
	pixelMatrix[y][x * bytePerPixel + 0] = index;

	assert(index < paletteSize);
	png_color* pal = static_cast<png_color*>(colorPalette);
	pal[index].red = grayscale;
	pal[index].green = grayscale;
	pal[index].blue = grayscale;
	alphaPalette[index] = 255;
}

void ImageBackend::SetPaletteIndex(size_t index, uint8_t nR, uint8_t nG, uint8_t nB, uint8_t nA)
{
	assert(isColorIndexed);
	assert(index < paletteSize);

	png_color* pal = static_cast<png_color*>(colorPalette);
	pal[index].red = nR;
	pal[index].green = nG;
	pal[index].blue = nB;
	alphaPalette[index] = nA;
}

void ImageBackend::SetPalette(const ImageBackend& pal)
{
	assert(isColorIndexed);
	size_t bytePerPixel = pal.GetBytesPerPixel();

	for (size_t y = 0; y < pal.height; y++)
	{
		for (size_t x = 0; x < pal.width; x++)
		{
			size_t index = y * pal.width + x;
			if (index >= paletteSize)
			{
				/*
				 * Some TLUTs are bigger than 256 colors.
				 * For those cases, we will only take the first 256
				 * to colorize this CI texture.
				 */
				return;
			}

			uint8_t r = pal.pixelMatrix[y][x * bytePerPixel + 0];
			uint8_t g = pal.pixelMatrix[y][x * bytePerPixel + 1];
			uint8_t b = pal.pixelMatrix[y][x * bytePerPixel + 2];
			uint8_t a = pal.pixelMatrix[y][x * bytePerPixel + 3];
			SetPaletteIndex(index, r, g, b, a);
		}
	}
}

uint32_t ImageBackend::GetWidth() const
{
	return width;
}

uint32_t ImageBackend::GetHeight() const
{
	return height;
}

uint8_t ImageBackend::GetColorType() const
{
	return colorType;
}

uint8_t ImageBackend::GetBitDepth() const
{
	return bitDepth;
}

double ImageBackend::GetBytesPerPixel() const
{
	switch (colorType)
	{
	case PNG_COLOR_TYPE_RGBA:
		return 4 * bitDepth / 8;

	case PNG_COLOR_TYPE_RGB:
		return 3 * bitDepth / 8;

	case PNG_COLOR_TYPE_PALETTE:
		return 1 * bitDepth / 8;

	default:
		HANDLE_ERROR(WarningType::InvalidPNG, "invalid color type", "");
	}
}

void ImageBackend::FreeImageData()
{
	if (hasImageData)
	{
		for (size_t y = 0; y < height; y++)
			free(pixelMatrix[y]);
		free(pixelMatrix);
		pixelMatrix = nullptr;
	}

	if (isColorIndexed)
	{
		free(colorPalette);
		free(alphaPalette);
		colorPalette = nullptr;
		alphaPalette = nullptr;
		isColorIndexed = false;
	}

	hasImageData = false;
}

/* RGBAPixel */

void RGBAPixel::SetRGBA(uint8_t nR, uint8_t nG, uint8_t nB, uint8_t nA)
{
	r = nR;
	g = nG;
	b = nB;
	a = nA;
}

void RGBAPixel::SetGrayscale(uint8_t grayscale, uint8_t alpha)
{
	r = grayscale;
	g = grayscale;
	b = grayscale;
	a = alpha;
}
