#include "ZTexture.h"

#include <cassert>

#include "CRC32.h"
#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/Directory.h"
#include "Utils/File.h"
#include "Utils/Path.h"
#include "WarningHandler.h"

REGISTER_ZFILENODE(Texture, ZTexture);

ZTexture::ZTexture(ZFile* nParent) : ZResource(nParent)
{
	width = 0;
	height = 0;
	dWordAligned = true;
	genOTRDef = true;

	RegisterRequiredAttribute("Width");
	RegisterRequiredAttribute("Height");
	RegisterRequiredAttribute("Format");
	RegisterOptionalAttribute("TlutOffset");
}

void ZTexture::ExtractFromBinary(uint32_t nRawDataIndex, int32_t nWidth, int32_t nHeight,
                                 TextureType nType, bool nIsPalette)
{
	width = nWidth;
	height = nHeight;
	format = nType;
	rawDataIndex = nRawDataIndex;
	isPalette = nIsPalette;
	name = GetDefaultName(parent->GetName());
	outName = name;

	// Don't parse raw data of external files
	if (parent->GetMode() == ZFileMode::ExternalFile)
		return;

	ParseRawData();
	CalcHash();
}

void ZTexture::FromPNG(const fs::path& pngFilePath, TextureType texType)
{
	format = texType;
	name = StringHelper::Split(Path::GetFileNameWithoutExtension(pngFilePath.string()), ".")[0];
	PrepareRawDataFromFile(pngFilePath);
}

void ZTexture::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	std::string widthXml = registeredAttributes.at("Width").value;
	std::string heightXml = registeredAttributes.at("Height").value;

	if (!StringHelper::HasOnlyDigits(widthXml))
	{
		std::string errorHeader = StringHelper::Sprintf(
			"value of 'Width' attribute has non-decimal digits: '%s'", widthXml.c_str());
		HANDLE_ERROR_RESOURCE(WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
		                      errorHeader, "");
	}
	if (!StringHelper::HasOnlyDigits(heightXml))
	{
		std::string errorHeader = StringHelper::Sprintf(
			"value of 'Height' attribute has non-decimal digits: '%s'", heightXml.c_str());
		HANDLE_ERROR_RESOURCE(WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
		                      errorHeader, "");
	}

	width = StringHelper::StrToL(widthXml);
	height = StringHelper::StrToL(heightXml);

	std::string formatStr = registeredAttributes.at("Format").value;
	format = GetTextureTypeFromString(formatStr);

	if (format == TextureType::Error)
	{
		HANDLE_ERROR_RESOURCE(WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
		                      "invalid value found for 'Format' attribute", "");
	}

	const auto& tlutOffsetAttr = registeredAttributes.at("TlutOffset");
	if (tlutOffsetAttr.wasSet)
	{
		switch (format)
		{
		case TextureType::Palette4bpp:
		case TextureType::Palette8bpp:
			tlutOffset = StringHelper::StrToL(tlutOffsetAttr.value, 16);
			break;

		default:
			HANDLE_ERROR_RESOURCE(WarningType::InvalidXML, parent, this, rawDataIndex,
			                      "'TlutOffset' declared in non color-indexed (ci4 or ci8) texture",
			                      "");
			break;
		}
	}
}

void ZTexture::ParseRawData()
{
	if (rawDataIndex % 8 != 0)
		dWordAligned = false;

	switch (format)
	{
	case TextureType::RGBA16bpp:
		PrepareBitmapRGBA16();
		break;
	case TextureType::RGBA32bpp:
		PrepareBitmapRGBA32();
		break;
	case TextureType::Grayscale4bpp:
		PrepareBitmapGrayscale4();
		break;
	case TextureType::Grayscale8bpp:
		PrepareBitmapGrayscale8();
		break;
	case TextureType::GrayscaleAlpha4bpp:
		PrepareBitmapGrayscaleAlpha4();
		break;
	case TextureType::GrayscaleAlpha8bpp:
		PrepareBitmapGrayscaleAlpha8();
		break;
	case TextureType::GrayscaleAlpha16bpp:
		PrepareBitmapGrayscaleAlpha16();
		break;
	case TextureType::Palette4bpp:
		PrepareBitmapPalette4();
		break;
	case TextureType::Palette8bpp:
		PrepareBitmapPalette8();
		break;
	case TextureType::Error:
		HANDLE_ERROR_RESOURCE(WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
		                      StringHelper::Sprintf("Invalid texture format", format), "");
		assert(!"TODO");
		break;
	}
}

void ZTexture::PrepareBitmapRGBA16()
{
	textureData.InitEmptyRGBImage(width, height, true);
	auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			int32_t pos = rawDataIndex + ((y * width) + x) * 2;
			uint16_t data = parentRawData.at(pos + 1) | (parentRawData.at(pos) << 8);
			uint8_t r = (data & 0xF800) >> 11;
			uint8_t g = (data & 0x07C0) >> 6;
			uint8_t b = (data & 0x003E) >> 1;
			uint8_t alpha = data & 0x01;

			textureData.SetRGBPixel(y, x, r * 8, g * 8, b * 8, alpha * 255);
		}
	}
}

void ZTexture::PrepareBitmapRGBA32()
{
	textureData.InitEmptyRGBImage(width, height, true);
	auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t pos = rawDataIndex + ((y * width) + x) * 4;
			uint8_t r = parentRawData.at(pos + 0);
			uint8_t g = parentRawData.at(pos + 1);
			uint8_t b = parentRawData.at(pos + 2);
			uint8_t alpha = parentRawData.at(pos + 3);

			textureData.SetRGBPixel(y, x, r, g, b, alpha);
		}
	}
}

void ZTexture::PrepareBitmapGrayscale4()
{
	textureData.InitEmptyRGBImage(width, height, false);
	auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x += 2)
		{
			for (uint8_t i = 0; i < 2; i++)
			{
				size_t pos = rawDataIndex + ((y * width) + x) / 2;
				uint8_t grayscale = 0;

				if (i == 0)
					grayscale = parentRawData.at(pos) & 0xF0;
				else
					grayscale = (parentRawData.at(pos) & 0x0F) << 4;

				textureData.SetGrayscalePixel(y, x + i, grayscale);
			}
		}
	}
}

void ZTexture::PrepareBitmapGrayscale8()
{
	textureData.InitEmptyRGBImage(width, height, false);
	auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t pos = rawDataIndex + ((y * width) + x) * 1;

			textureData.SetGrayscalePixel(y, x, parentRawData.at(pos));
		}
	}
}

void ZTexture::PrepareBitmapGrayscaleAlpha4()
{
	textureData.InitEmptyRGBImage(width, height, true);
	auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x += 2)
		{
			for (uint16_t i = 0; i < 2; i++)
			{
				size_t pos = rawDataIndex + ((y * width) + x) / 2;
				uint8_t data = 0;

				if (i == 0)
					data = (parentRawData.at(pos) & 0xF0) >> 4;
				else
					data = parentRawData.at(pos) & 0x0F;

				uint8_t grayscale = ((data & 0x0E) >> 1) * 32;
				uint8_t alpha = (data & 0x01) * 255;

				textureData.SetGrayscalePixel(y, x + i, grayscale, alpha);
			}
		}
	}
}

void ZTexture::PrepareBitmapGrayscaleAlpha8()
{
	textureData.InitEmptyRGBImage(width, height, true);
	auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t pos = rawDataIndex + ((y * width) + x) * 1;
			uint8_t grayscale = parentRawData.at(pos) & 0xF0;
			uint8_t alpha = (parentRawData.at(pos) & 0x0F) << 4;

			textureData.SetGrayscalePixel(y, x, grayscale, alpha);
		}
	}
}

void ZTexture::PrepareBitmapGrayscaleAlpha16()
{
	textureData.InitEmptyRGBImage(width, height, true);
	auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t pos = rawDataIndex + ((y * width) + x) * 2;
			uint8_t grayscale = parentRawData.at(pos + 0);
			uint8_t alpha = parentRawData.at(pos + 1);

			textureData.SetGrayscalePixel(y, x, grayscale, alpha);
		}
	}
}

void ZTexture::PrepareBitmapPalette4()
{
	textureData.InitEmptyPaletteImage(width, height);
	auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x += 2)
		{
			for (uint16_t i = 0; i < 2; i++)
			{
				size_t pos = rawDataIndex + ((y * width) + x) / 2;
				uint8_t paletteIndex = 0;

				if (i == 0)
					paletteIndex = (parentRawData.at(pos) & 0xF0) >> 4;
				else
					paletteIndex = (parentRawData.at(pos) & 0x0F);

				textureData.SetIndexedPixel(y, x + i, paletteIndex, paletteIndex * 16);
			}
		}
	}
}

void ZTexture::PrepareBitmapPalette8()
{
	textureData.InitEmptyPaletteImage(width, height);
	auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t pos = rawDataIndex + ((y * width) + x) * 1;
			uint8_t grayscale = parentRawData.at(pos);

			textureData.SetIndexedPixel(y, x, grayscale, grayscale);
		}
	}
}

void ZTexture::DeclareReferences([[maybe_unused]] const std::string& prefix)
{
	if (tlutOffset != static_cast<uint32_t>(-1))
	{
		tlut = parent->GetTextureResource(tlutOffset);
		if (tlut == nullptr)
		{
			int32_t tlutDim = 16;
			if (format == TextureType::Palette4bpp)
				tlutDim = 4;

			tlut = new ZTexture(parent);
			tlut->ExtractFromBinary(tlutOffset, tlutDim, tlutDim, TextureType::RGBA16bpp, true);
			parent->AddTextureResource(tlutOffset, tlut);
			tlut->DeclareVar(prefix, "");
		}
		else
		{
			tlut->isPalette = true;
		}
		SetTlut(tlut);
	}
}

void ZTexture::PrepareRawDataFromFile(const fs::path& pngFilePath)
{
	switch (format)
	{
	case TextureType::RGBA16bpp:
		PrepareRawDataRGBA16(pngFilePath);
		break;
	case TextureType::RGBA32bpp:
		PrepareRawDataRGBA32(pngFilePath);
		break;
	case TextureType::Grayscale4bpp:
		PrepareRawDataGrayscale4(pngFilePath);
		break;
	case TextureType::Grayscale8bpp:
		PrepareRawDataGrayscale8(pngFilePath);
		break;
	case TextureType::GrayscaleAlpha4bpp:
		PrepareRawDataGrayscaleAlpha4(pngFilePath);
		break;
	case TextureType::GrayscaleAlpha8bpp:
		PrepareRawDataGrayscaleAlpha8(pngFilePath);
		break;
	case TextureType::GrayscaleAlpha16bpp:
		PrepareRawDataGrayscaleAlpha16(pngFilePath);
		break;
	case TextureType::Palette4bpp:
		PrepareRawDataPalette4(pngFilePath);
		break;
	case TextureType::Palette8bpp:
		PrepareRawDataPalette8(pngFilePath);
		break;
	case TextureType::Error:
		HANDLE_ERROR_PROCESS(WarningType::InvalidPNG, "Input PNG file has invalid format type", "");
		break;
	}
}

void ZTexture::PrepareRawDataRGBA16(const fs::path& rgbaPath)
{
	textureData.ReadPng(rgbaPath);

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x++)
		{
			size_t pos = ((y * width) + x) * 2;
			RGBAPixel pixel = textureData.GetPixel(y, x);

			uint8_t r = pixel.r / 8;
			uint8_t g = pixel.g / 8;
			uint8_t b = pixel.b / 8;

			uint8_t alphaBit = pixel.a != 0;

			uint16_t data = (r << 11) + (g << 6) + (b << 1) + alphaBit;

			textureDataRaw[pos + 0] = (data & 0xFF00) >> 8;
			textureDataRaw[pos + 1] = (data & 0x00FF);
		}
	}
}

void ZTexture::PrepareRawDataRGBA32(const fs::path& rgbaPath)
{
	textureData.ReadPng(rgbaPath);

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x++)
		{
			size_t pos = ((y * width) + x) * 4;
			RGBAPixel pixel = textureData.GetPixel(y, x);

			textureDataRaw[pos + 0] = pixel.r;
			textureDataRaw[pos + 1] = pixel.g;
			textureDataRaw[pos + 2] = pixel.b;
			textureDataRaw[pos + 3] = pixel.a;
		}
	}
}

void ZTexture::PrepareRawDataGrayscale4(const fs::path& grayPath)
{
	textureData.ReadPng(grayPath);

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x += 2)
		{
			size_t pos = ((y * width) + x) / 2;
			uint8_t r1 = textureData.GetPixel(y, x).r;
			uint8_t r2 = textureData.GetPixel(y, x + 1).r;

			textureDataRaw[pos] = (uint8_t)(((r1 / 16) << 4) + (r2 / 16));
		}
	}
}

void ZTexture::PrepareRawDataGrayscale8(const fs::path& grayPath)
{
	textureData.ReadPng(grayPath);

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x++)
		{
			size_t pos = (y * width) + x;
			RGBAPixel pixel = textureData.GetPixel(y, x);
			textureDataRaw[pos] = pixel.r;
		}
	}
}

void ZTexture::PrepareRawDataGrayscaleAlpha4(const fs::path& grayAlphaPath)
{
	textureData.ReadPng(grayAlphaPath);

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x += 2)
		{
			size_t pos = ((y * width) + x) / 2;
			uint8_t data = 0;

			for (uint16_t i = 0; i < 2; i++)
			{
				RGBAPixel pixel = textureData.GetPixel(y, x + i);
				uint8_t cR = pixel.r;
				uint8_t alphaBit = pixel.a != 0;

				if (i == 0)
					data |= (((cR / 32) << 1) + alphaBit) << 4;
				else
					data |= ((cR / 32) << 1) + alphaBit;
			}

			textureDataRaw[pos] = data;
		}
	}
}

void ZTexture::PrepareRawDataGrayscaleAlpha8(const fs::path& grayAlphaPath)
{
	textureData.ReadPng(grayAlphaPath);

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x++)
		{
			size_t pos = ((y * width) + x) * 1;
			RGBAPixel pixel = textureData.GetPixel(y, x);

			uint8_t r = pixel.r;
			uint8_t a = pixel.a;

			textureDataRaw[pos] = ((r / 16) << 4) + (a / 16);
		}
	}
}

void ZTexture::PrepareRawDataGrayscaleAlpha16(const fs::path& grayAlphaPath)
{
	textureData.ReadPng(grayAlphaPath);

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x++)
		{
			size_t pos = ((y * width) + x) * 2;
			RGBAPixel pixel = textureData.GetPixel(y, x);

			uint8_t cR = pixel.r;
			uint8_t aR = pixel.a;

			textureDataRaw[pos + 0] = cR;
			textureDataRaw[pos + 1] = aR;
		}
	}
}

void ZTexture::PrepareRawDataPalette4(const fs::path& palPath)
{
	textureData.ReadPng(palPath);

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x += 2)
		{
			size_t pos = ((y * width) + x) / 2;

			uint8_t cR1 = textureData.GetIndexedPixel(y, x);
			uint8_t cR2 = textureData.GetIndexedPixel(y, x + 1);

			textureDataRaw[pos] = (cR1 << 4) | (cR2);
		}
	}
}

void ZTexture::PrepareRawDataPalette8(const fs::path& palPath)
{
	textureData.ReadPng(palPath);

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x++)
		{
			size_t pos = ((y * width) + x);
			uint8_t cR = textureData.GetIndexedPixel(y, x);

			textureDataRaw[pos] = cR;
		}
	}
}

float ZTexture::GetPixelMultiplyer() const
{
	switch (format)
	{
	case TextureType::Grayscale4bpp:
	case TextureType::GrayscaleAlpha4bpp:
	case TextureType::Palette4bpp:
		return 0.5f;
	case TextureType::Grayscale8bpp:
	case TextureType::GrayscaleAlpha8bpp:
	case TextureType::Palette8bpp:
		return 1;
	case TextureType::GrayscaleAlpha16bpp:
	case TextureType::RGBA16bpp:
		return 2;
	case TextureType::RGBA32bpp:
		return 4;
	default:
		return -1;
	}
}

size_t ZTexture::GetRawDataSize() const
{
	return (width * height * GetPixelMultiplyer());
}

std::string ZTexture::GetIMFmtFromType()
{
	switch (format)
	{
	case TextureType::RGBA32bpp:
	case TextureType::RGBA16bpp:
		return "G_IM_FMT_RGBA";
	case TextureType::Grayscale4bpp:
	case TextureType::Grayscale8bpp:
		return "G_IM_FMT_I";
	case TextureType::Palette4bpp:
	case TextureType::Palette8bpp:
		return "G_IM_FMT_CI";
	case TextureType::GrayscaleAlpha4bpp:
	case TextureType::GrayscaleAlpha8bpp:
	case TextureType::GrayscaleAlpha16bpp:
		return "G_IM_FMT_IA";
	default:
		return "ERROR";
	}
}

std::string ZTexture::GetIMSizFromType()
{
	switch (format)
	{
	case TextureType::Grayscale4bpp:
	case TextureType::Palette4bpp:
	case TextureType::GrayscaleAlpha4bpp:
		return "G_IM_SIZ_4b";
	case TextureType::Palette8bpp:
	case TextureType::Grayscale8bpp:
		return "G_IM_SIZ_8b";
	case TextureType::GrayscaleAlpha16bpp:
	case TextureType::RGBA16bpp:
		return "G_IM_SIZ_16b";
	case TextureType::RGBA32bpp:
		return "G_IM_SIZ_32b";
	default:
		return "ERROR";
	}
}

std::string ZTexture::GetDefaultName(const std::string& prefix) const
{
	const char* suffix = "Tex";
	if (isPalette)
		suffix = "TLUT";
	return StringHelper::Sprintf("%s%s_%06X", prefix.c_str(), suffix, rawDataIndex);
}

uint32_t ZTexture::GetWidth() const
{
	return width;
}

uint32_t ZTexture::GetHeight() const
{
	return height;
}

void ZTexture::SetDimensions(uint32_t nWidth, uint32_t nHeight)
{
	width = nWidth;
	height = nHeight;
	ParseRawData();
}

TextureType ZTexture::GetTextureType() const
{
	return format;
}


void ZTexture::Save(const fs::path& outFolder)
{
	// Optionally generate text file containing CRC information. This is going to be a one time
	// process for generating the Texture Pool XML.
	if (Globals::Instance->outputCrc)
	{
		File::WriteAllText((Globals::Instance->outputPath / (outName + ".txt")).string(),
		                   StringHelper::Sprintf("%08lX", hash));
	}

	// Do not save png files if we're making an OTR file. They're not needed...
	if (Globals::Instance->otrMode)
		return;

	auto outPath = GetPoolOutPath(outFolder);

	if (!Directory::Exists(outPath.string()))
		Directory::CreateDirectory(outPath.string());

//#ifdef _MSC_VER
	fs::path outFileName;
//#else
	//std::filesystem::__cxx11::path outFileName;
//#endif

	if (!dWordAligned)
		outFileName = outPath / (outName + ".u32" + "." + GetExternalExtension() + ".png");
	else
		outFileName = outPath / (outName + +"." + GetExternalExtension() + ".png");

#ifdef TEXTURE_DEBUG
	printf("Saving PNG: %s\n", outFileName.c_str());
	printf("\t Var name: %s\n", name.c_str());
	if (tlut != nullptr)
		printf("\t TLUT name: %s\n", tlut->name.c_str());
#endif

	textureData.WritePng(outFileName);

#ifdef TEXTURE_DEBUG
	printf("\n");
#endif
}

Declaration* ZTexture::DeclareVar(const std::string& prefix,
                                  [[maybe_unused]] const std::string& bodyStr)
{
	std::string auxName = name;
	std::string auxOutName = outName;
	std::string incStr;

	//if (Globals::Instance->otrMode)
		//return nullptr;

	if (auxName == "")
		auxName = GetDefaultName(prefix);

	if (auxOutName == "")
		auxOutName = GetDefaultName(prefix);

	auto filepath = Globals::Instance->outputPath / fs::path(auxOutName).stem();

	if (dWordAligned)
		incStr =
			StringHelper::Sprintf("%s.%s.inc.c", filepath.c_str(), GetExternalExtension().c_str());
	else
		incStr = StringHelper::Sprintf("%s.u32.%s.inc.c", filepath.c_str(),
		                               GetExternalExtension().c_str());

	if (!Globals::Instance->cfg.texturePool.empty())
	{
		CalcHash();

		// TEXTURE POOL CHECK
		const auto& poolEntry = Globals::Instance->cfg.texturePool.find(hash);
		if (poolEntry != Globals::Instance->cfg.texturePool.end())
		{
			if (dWordAligned)
				incStr = StringHelper::Sprintf("%s.%s.inc.c", poolEntry->second.path.c_str(),
				                               GetExternalExtension().c_str());
			else
				incStr =
					StringHelper::Sprintf("%s.u32.%s.inc.c", poolEntry->second.path.c_str(),
				                          GetExternalExtension().c_str());
		}
	}
	size_t texSizeDivisor = (dWordAligned) ? 8 : 4;

	Declaration* decl = parent->AddDeclarationIncludeArray(rawDataIndex, incStr, GetRawDataSize(),
	                                                       GetSourceTypeName(), auxName,
	                                                       GetRawDataSize() / texSizeDivisor);
	decl->staticConf = staticConf;
	return decl;
}

std::string ZTexture::GetBodySourceCode() const
{
	std::string sourceOutput;

	if (!Globals::Instance->otrMode)
	{
		size_t texSizeInc = (dWordAligned) ? 8 : 4;
		for (size_t i = 0; i < textureDataRaw.size(); i += texSizeInc)
		{
			if (i % 32 == 0)
				sourceOutput += "    ";
			if (dWordAligned)
				sourceOutput += StringHelper::Sprintf("0x%016llX, ",
				                                      BitConverter::ToUInt64BE(textureDataRaw, i));
			else
				sourceOutput += StringHelper::Sprintf("0x%08llX, ",
				                                      BitConverter::ToUInt32BE(textureDataRaw, i));
			if (i % 32 == 24)
				sourceOutput +=
					StringHelper::Sprintf(" // 0x%06X \n", rawDataIndex + ((i / 32) * 32));
		}

		// Ensure there's always a trailing line feed to prevent dumb warnings.
		// Please don't remove this line, unless you somehow made a way to prevent
		// that warning when building the OoT repo.
		sourceOutput += "\n";
	} else if (Globals::Instance->buildRawTexture) {
		sourceOutput += std::string(textureDataRaw.begin(), textureDataRaw.end());
	}

	return sourceOutput;
}

bool ZTexture::IsExternalResource() const
{
	return true;
}

ZResourceType ZTexture::GetResourceType() const
{
	return ZResourceType::Texture;
}

std::string ZTexture::GetSourceTypeName() const
{
	return dWordAligned ? "u64" : "u32";
}

void ZTexture::CalcHash()
{
	//if (hash == 0)
	{
		const auto& parentRawData = parent->GetRawData();
		hash = CRC32B(parentRawData.data() + rawDataIndex, GetRawDataSize());
	}
}

std::string ZTexture::GetExternalExtension() const
{
	switch (format)
	{
	case TextureType::RGBA32bpp:
		return "rgba32";
	case TextureType::RGBA16bpp:
		return "rgba16";
	case TextureType::Grayscale4bpp:
		return "i4";
	case TextureType::Grayscale8bpp:
		return "i8";
	case TextureType::GrayscaleAlpha4bpp:
		return "ia4";
	case TextureType::GrayscaleAlpha8bpp:
		return "ia8";
	case TextureType::GrayscaleAlpha16bpp:
		return "ia16";
	case TextureType::Palette4bpp:
		return "ci4";
	case TextureType::Palette8bpp:
		return "ci8";
	default:
		return "ERROR";
	}
}

fs::path ZTexture::GetPoolOutPath(const fs::path& defaultValue)
{
	if (Globals::Instance->cfg.texturePool.find(hash) != Globals::Instance->cfg.texturePool.end())
		return Path::GetDirectoryName(Globals::Instance->cfg.texturePool[hash].path.string());

	return defaultValue;
}

TextureType ZTexture::GetTextureTypeFromString(const std::string& str)
{
	TextureType texType = TextureType::Error;

	if (str == "rgba32")
		texType = TextureType::RGBA32bpp;
	else if (str == "rgba16")
		texType = TextureType::RGBA16bpp;
	else if (str == "rgb5a1")
	{
		texType = TextureType::RGBA16bpp;
		HANDLE_WARNING(WarningType::Deprecated,
		               "the texture format 'rgb5a1' is currently deprecated",
		               "It will be removed in a future version. Use the format 'rgba16' instead.");
	}
	else if (str == "i4")
		texType = TextureType::Grayscale4bpp;
	else if (str == "i8")
		texType = TextureType::Grayscale8bpp;
	else if (str == "ia4")
		texType = TextureType::GrayscaleAlpha4bpp;
	else if (str == "ia8")
		texType = TextureType::GrayscaleAlpha8bpp;
	else if (str == "ia16")
		texType = TextureType::GrayscaleAlpha16bpp;
	else if (str == "ci4")
		texType = TextureType::Palette4bpp;
	else if (str == "ci8")
		texType = TextureType::Palette8bpp;
	else
		// TODO: handle this case in a more coherent way
		HANDLE_WARNING(WarningType::InvalidAttributeValue,
		               "invalid value found for 'Type' attribute", "Defaulting to ''.");
	return texType;
}

bool ZTexture::IsColorIndexed() const
{
	switch (format)
	{
	case TextureType::Palette4bpp:
	case TextureType::Palette8bpp:
		return true;

	default:
		return false;
	}
}

void ZTexture::SetTlut(ZTexture* nTlut)
{
	assert(IsColorIndexed());
	assert(nTlut->isPalette);
	tlut = nTlut;

	textureData.SetPalette(tlut->textureData);
}

bool ZTexture::HasTlut() const
{
	return tlut != nullptr;
}
