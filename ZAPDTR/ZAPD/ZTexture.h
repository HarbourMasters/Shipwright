#pragma once

#include "ImageBackend.h"
#include "ZResource.h"
#include "tinyxml2.h"

enum class TextureType
{
	Error,
	RGBA32bpp,
	RGBA16bpp,
	Palette4bpp,
	Palette8bpp,
	Grayscale4bpp,
	Grayscale8bpp,
	GrayscaleAlpha4bpp,
	GrayscaleAlpha8bpp,
	GrayscaleAlpha16bpp,
};

class ZTexture : public ZResource
{
protected:
	TextureType format = TextureType::Error;
	uint32_t width, height;

	ImageBackend textureData;
	std::vector<uint8_t> textureDataRaw;  // When reading from a PNG file.
	uint32_t tlutOffset = static_cast<uint32_t>(-1);
	ZTexture* tlut = nullptr;

	void PrepareBitmapRGBA16();
	void PrepareBitmapRGBA32();
	void PrepareBitmapGrayscale8();
	void PrepareBitmapGrayscaleAlpha8();
	void PrepareBitmapGrayscale4();
	void PrepareBitmapGrayscaleAlpha4();
	void PrepareBitmapGrayscaleAlpha16();
	void PrepareBitmapPalette4();
	void PrepareBitmapPalette8();

	void PrepareRawDataFromFile(const fs::path& inFolder);
	void PrepareRawDataRGBA16(const fs::path& rgbaPath);
	void PrepareRawDataRGBA32(const fs::path& rgbaPath);
	void PrepareRawDataGrayscale4(const fs::path& grayPath);
	void PrepareRawDataGrayscale8(const fs::path& grayPath);
	void PrepareRawDataGrayscaleAlpha4(const fs::path& grayAlphaPath);
	void PrepareRawDataGrayscaleAlpha8(const fs::path& grayAlphaPath);
	void PrepareRawDataGrayscaleAlpha16(const fs::path& grayAlphaPath);
	void PrepareRawDataPalette4(const fs::path& palPath);
	void PrepareRawDataPalette8(const fs::path& palPath);

public:
	ZTexture(ZFile* nParent);

	bool isPalette = false;
	bool dWordAligned = true;

	void ExtractFromBinary(uint32_t nRawDataIndex, int32_t nWidth, int32_t nHeight,
	                       TextureType nType, bool nIsPalette);
	void FromPNG(const fs::path& pngFilePath, TextureType texType);
	static TextureType GetTextureTypeFromString(const std::string& str);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;
	std::string GetBodySourceCode() const override;
	void CalcHash() override;
	void Save(const fs::path& outFolder) override;

	bool IsExternalResource() const override;
	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;
	std::string GetExternalExtension() const override;

	size_t GetRawDataSize() const override;
	std::string GetIMFmtFromType();
	std::string GetIMSizFromType();
	std::string GetDefaultName(const std::string& prefix) const override;
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	void SetDimensions(uint32_t nWidth, uint32_t nHeight);
	float GetPixelMultiplyer() const;
	TextureType GetTextureType() const;
	fs::path GetPoolOutPath(const fs::path& defaultValue);
	bool IsColorIndexed() const;
	void SetTlut(ZTexture* nTlut);
	bool HasTlut() const;
};
