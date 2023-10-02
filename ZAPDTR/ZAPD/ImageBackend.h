#pragma once

#include <cstdint>
#include <vector>

#include "Utils/Directory.h"

class RGBAPixel
{
public:
	RGBAPixel() = default;

	void SetRGBA(uint8_t nR, uint8_t nG, uint8_t nB, uint8_t nA);
	void SetGrayscale(uint8_t grayscale, uint8_t alpha = 0);

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 0;
};

class ImageBackend
{
public:
	ImageBackend() = default;
	~ImageBackend();

	void ReadPng(const char* filename);
	void ReadPng(const fs::path& filename);
	void WritePng(const char* filename);
	void WritePng(const fs::path& filename);

	void SetTextureData(const std::vector<std::vector<RGBAPixel>>& texData, uint32_t nWidth,
	                    uint32_t nHeight, uint8_t nColorType, uint8_t nBitDepth);
	void InitEmptyRGBImage(uint32_t nWidth, uint32_t nHeight, bool alpha);
	void InitEmptyPaletteImage(uint32_t nWidth, uint32_t nHeight);

	RGBAPixel GetPixel(size_t y, size_t x) const;
	uint8_t GetIndexedPixel(size_t y, size_t x) const;

	void SetRGBPixel(size_t y, size_t x, uint8_t nR, uint8_t nG, uint8_t nB, uint8_t nA = 0);
	void SetGrayscalePixel(size_t y, size_t x, uint8_t grayscale, uint8_t alpha = 0);

	void SetIndexedPixel(size_t y, size_t x, uint8_t index, uint8_t grayscale);
	void SetIndexedPixel(size_t y, size_t x, uint8_t index, RGBAPixel pixel);
	void SetPaletteIndex(size_t index, uint8_t nR, uint8_t nG, uint8_t nB, uint8_t nA);
	void SetPalette(const ImageBackend& pal, uint32_t offset = 0);

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	uint8_t GetColorType() const;
	uint8_t GetBitDepth() const;

protected:
	uint8_t** pixelMatrix = nullptr;  // height * [width * bytePerPixel]

	void* colorPalette = nullptr;
	uint8_t* alphaPalette = nullptr;
	size_t paletteSize = 16 * 16;

	uint32_t width = 0;
	uint32_t height = 0;
	uint8_t colorType = 0;
	uint8_t bitDepth = 0;

	bool hasImageData = false;
	bool isColorIndexed = false;

	double GetBytesPerPixel() const;

	void FreeImageData();
};
