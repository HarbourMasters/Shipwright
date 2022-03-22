/**
 * File: ZTextureAnimation.cpp
 * ZResources defined: ZTextureAnimation, ZTextureAnimationParams (XML declaration not supported for
 * the latter)
 * Purpose: extracting texture animating structures from asset files
 * Note: data type is exclusive to Majora's Mask
 *
 * Structure of data:
 * A texture animation consists of a main array of data of the form
 *     `SS 00 00 0T PPPPPPPP`,
 * where `S` is the segment that the functionality is sent to (and read by DLists), `T` is the
 * "type" (see below), and `P` is a pointer to the type's subsidiary information, which is
 * invariably just above this main array. Details of this subsidiary data are given below.
 *
 * Segment
 * ===
 * The segment starts at 1 and is incremented in each entry; the final one is negated, which is used
 * as the indication to stop reading. The actual segment the game will use to draw is S + 7.
 *
 * Types
 * ===
 * There are 7 values that `T` can take:
 *     `0`: A single texture scroll (Implemented by Gfx_TexScroll): subsidiary params are given as
 *         an array with one entry, a struct `XX YY WW HH` containing xStep, yStep, width, height
 *         (all u8).
 *
 *     `1`: A dual texture scroll (Implementated by Gfx_TwoTexScroll): same as type `0`, but with
 *         two entries in the params array.
 *
 *     `2`: Color changing: Changes the primColor (with a LOD factor) and envColor
 *         `KKKK LLLL PPPPPPPP QQQQQQQQ RRRRRRRR`
 *         - `K` (u16) is the total length of the animation (and in this case, the total number of
 *             colors)
 *         - `L` (u16) is seemingly always 0 for this type, and not used in the code
 *         - `P` segmented pointer to array of augmented primColors
 *         - `Q` segmented pointer to array of envColors, and can be NULL
 *         - `R` segmented pointer to array of frameData (u8), which is also seemingly always NULL
 *             for this type.
 *
 *         envColors take the form `RR GG BB AA` as usual, while primColors have an extra LODFrac
 *         element: RR GG BB AA LL
 *
 *     `3`: Color changing (LERP): similar to type `2`, but uses linear interpolation. The structure
 *         is similar to `2`, but
 *         - `K` is now just the animation length, while
 *         - `L` is the total number of colors, and
 *         - the frameData is used to determine which colors to interpolate.
 *
 *     `4`: Color changing (Lagrange interpolation): For extraction purposes identical to type 3.
 *         Uses a nonlinear interpolation formula to change the colours more smoothly.
 *
 *     `5`: Texture cycle: functions like a gif. Subsidiary params:
 *         `FFFF 0000 PPPPPPPP QQQQQQQQ`
 *         where
 *         - `F` (u16) time between changes in frames
 *         - `P` pointer to array of segmented pointers to textures to cycle through
 *         - `Q` array of indices, indicating which texture to use next when a change frame is
 *             reached. The maximum indicates the number of textures in the texture array.
 *
 *     `6`: This is used to indicate an empty/unimplemented params set. It is ignored by the game's
 *         function provided that the segment is 0. A generic empty one takes the form
 *         `00 00 00 06 00000000`,
 *         and has no extra data.
 *
 * Implementation
 * ===
 * - ZTextureAnimation requires a declaration in the XML to extract anything. It handles the main
 * array. It uses a POD struct for each entry, and declares references to the params structs.
 *
 * - ZTextureAnimationParams is not (currently) declarable in an XML. It is a parent class for the
 * three classes that handle the various cases:
 *     - TextureScrollingParams for types `0` and `1`
 *     - TextureColorChangingParams for types `2`,`3`,`4`
 *     - TextureCyclingParams for type `5`
 * Each of these will declare all its subsidiary arrays, using POD structs.
 */
#include "ZTextureAnimation.h"

#include <cassert>
#include <memory>
#include <vector>

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "WarningHandler.h"
#include "ZFile.h"
#include "ZResource.h"
#include "tinyxml2.h"

REGISTER_ZFILENODE(TextureAnimation, ZTextureAnimation);

/* Constructors */
ZTextureAnimationParams::ZTextureAnimationParams(ZFile* parent) : ZResource::ZResource(parent)
{
}
TextureScrollingParams::TextureScrollingParams(ZFile* parent)
	: ZTextureAnimationParams::ZTextureAnimationParams(parent)
{
}
TextureColorChangingParams::TextureColorChangingParams(ZFile* parent)
	: ZTextureAnimationParams::ZTextureAnimationParams(parent)
{
}
TextureCyclingParams::TextureCyclingParams(ZFile* parent)
	: ZTextureAnimationParams::ZTextureAnimationParams(parent)
{
}

/* TextureAnimationParams */
/* This class only implements the functions common to all or most its inheritors */

void ZTextureAnimationParams::ExtractFromBinary(uint32_t nRawDataIndex)
{
	rawDataIndex = nRawDataIndex;

	ParseRawData();
}

// Implemented by TextureScrollingParams only
void ZTextureAnimationParams::ExtractFromBinary([[maybe_unused]] uint32_t nRawDataIndex,
                                                [[maybe_unused]] int count)
{
}

std::string
ZTextureAnimationParams::GetDefaultName([[maybe_unused]] const std::string& prefix) const
{
	return "ShouldNotBeVIsible";
}

ZResourceType ZTextureAnimationParams::GetResourceType() const
{
	return ZResourceType::TextureAnimationParams;
}

/* TextureScrollingParams */

void TextureScrollingParams::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	for (int i = 0; i < count; i++)
	{
		rows[i].xStep = BitConverter::ToUInt8BE(rawData, rawDataIndex + 4 * i);
		rows[i].yStep = BitConverter::ToUInt8BE(rawData, rawDataIndex + 4 * i + 1);
		rows[i].width = BitConverter::ToUInt8BE(rawData, rawDataIndex + 4 * i + 2);
		rows[i].height = BitConverter::ToUInt8BE(rawData, rawDataIndex + 4 * i + 3);
	}
}

void TextureScrollingParams::ExtractFromBinary(uint32_t nRawDataIndex, int nCount)
{
	rawDataIndex = nRawDataIndex;
	count = nCount;

	ParseRawData();
}

std::string TextureScrollingParams::GetSourceTypeName() const
{
	return "AnimatedMatTexScrollParams";  // TODO: Better name
}

std::string TextureScrollingParams::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sTexScrollParams_%06X", prefix.c_str(), rawDataIndex);
}

size_t TextureScrollingParams::GetRawDataSize() const
{
	return 4 * count;
}

/**
 * Overrides the parent version to declare an array of the params rather than just one entry.
 */
Declaration* TextureScrollingParams::DeclareVar(const std::string& prefix,
                                                const std::string& bodyStr)
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	return parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                                   GetSourceTypeName(), auxName, count, bodyStr);
}

std::string TextureScrollingParams::GetBodySourceCode() const
{
	std::string bodyStr;

	for (int i = 0; i < count; i++)
	{
		bodyStr += StringHelper::Sprintf("\t{ %d, %d, 0x%02X, 0x%02X },\n", rows[i].xStep,
		                                 rows[i].yStep, rows[i].width, rows[i].height);
	}

	bodyStr.pop_back();

	return bodyStr;
}

/* TextureColorChangingParams */

/**
 * Also parses the color and frameData arrays
 */
void TextureColorChangingParams::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	animLength = BitConverter::ToUInt16BE(rawData, rawDataIndex);
	colorListCount = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);

	// Handle type 2 separately
	uint16_t listLength =
		((type == TextureAnimationParamsType::ColorChange) ? animLength : colorListCount);

	if (listLength == 0)
		HANDLE_ERROR_RESOURCE(WarningType::Always, parent, this, rawDataIndex,
		                      "color list length cannot be 0", "");

	primColorListAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);
	envColorListAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
	frameDataListAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0xC);

	uint32_t primColorListOffset = Seg2Filespace(primColorListAddress, parent->baseAddress);
	uint32_t envColorListOffset = Seg2Filespace(envColorListAddress, parent->baseAddress);
	uint32_t frameDataListOffset = Seg2Filespace(frameDataListAddress, parent->baseAddress);

	uint32_t currentPtr;

	F3DPrimColor currentPrimColor;

	for (currentPtr = primColorListOffset; currentPtr < primColorListOffset + 5 * listLength;
	     currentPtr += 5)
	{
		currentPrimColor = {BitConverter::ToUInt8BE(rawData, currentPtr),
		                    BitConverter::ToUInt8BE(rawData, currentPtr + 1),
		                    BitConverter::ToUInt8BE(rawData, currentPtr + 2),
		                    BitConverter::ToUInt8BE(rawData, currentPtr + 3),
		                    BitConverter::ToUInt8BE(rawData, currentPtr + 4)};
		primColorList.push_back(currentPrimColor);
	}

	F3DEnvColor currentEnvColor;

	for (currentPtr = envColorListOffset; currentPtr < envColorListOffset + 4 * listLength;
	     currentPtr += 4)
	{
		currentEnvColor = {BitConverter::ToUInt8BE(rawData, currentPtr),
		                   BitConverter::ToUInt8BE(rawData, currentPtr + 1),
		                   BitConverter::ToUInt8BE(rawData, currentPtr + 2),
		                   BitConverter::ToUInt8BE(rawData, currentPtr + 3)};
		envColorList.push_back(currentEnvColor);
	}

	uint16_t currentFrameData;

	for (currentPtr = frameDataListOffset; currentPtr < frameDataListOffset + 2 * listLength;
	     currentPtr += 2)
	{
		currentFrameData = BitConverter::ToUInt16BE(rawData, currentPtr);
		frameDataList.push_back(currentFrameData);
	}
}

std::string TextureColorChangingParams::GetSourceTypeName() const
{
	return "AnimatedMatColorParams";  // TODO: Better name
}

std::string TextureColorChangingParams::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sColorParams_%06X", prefix.c_str(), rawDataIndex);
}

size_t TextureColorChangingParams::GetRawDataSize() const
{
	return 0x10;
}

void TextureColorChangingParams::DeclareReferences([[maybe_unused]] const std::string& prefix)
{
	if (primColorListAddress != 0)  // NULL
	{
		std::string primColorBodyStr;

		for (const auto& color : primColorList)
		{
			primColorBodyStr += StringHelper::Sprintf("\t{ %d, %d, %d, %d, %d },\n", color.r,
			                                          color.g, color.b, color.a, color.lodFrac);
		}

		primColorBodyStr.pop_back();

		parent->AddDeclarationArray(
			Seg2Filespace(primColorListAddress, parent->baseAddress), DeclarationAlignment::Align4,
			primColorList.size() * 5, "F3DPrimColor",
			StringHelper::Sprintf("%sTexColorChangingPrimColors_%06X", parent->GetName().c_str(),
		                          Seg2Filespace(primColorListAddress, parent->baseAddress)),
			primColorList.size(), primColorBodyStr);
	}

	if (envColorListAddress != 0)  // NULL
	{
		std::string envColorBodyStr;

		for (const auto& color : envColorList)
		{
			envColorBodyStr += StringHelper::Sprintf("\t{ %d, %d, %d, %d },\n", color.r, color.g,
			                                         color.b, color.a);
		}

		envColorBodyStr.pop_back();

		parent->AddDeclarationArray(
			Seg2Filespace(envColorListAddress, parent->baseAddress), DeclarationAlignment::Align4,
			envColorList.size() * 4, "F3DEnvColor",
			StringHelper::Sprintf("%sTexColorChangingEnvColors_%06X", parent->GetName().c_str(),
		                          Seg2Filespace(envColorListAddress, parent->baseAddress)),
			envColorList.size(), envColorBodyStr);
	}

	if (frameDataListAddress != 0)  // NULL
	{
		std::string frameDataBodyStr = "\t";

		for (const auto& frame : frameDataList)
		{
			frameDataBodyStr += StringHelper::Sprintf("%d, ", frame);
		}

		frameDataBodyStr.pop_back();

		parent->AddDeclarationArray(
			Seg2Filespace(frameDataListAddress, parent->baseAddress), DeclarationAlignment::Align4,
			frameDataList.size() * 2, "u16",
			StringHelper::Sprintf("%sTexColorChangingFrameData_%06X", parent->GetName().c_str(),
		                          Seg2Filespace(frameDataListAddress, parent->baseAddress)),
			frameDataList.size(), frameDataBodyStr);
	}
}

std::string TextureColorChangingParams::GetBodySourceCode() const
{
	std::string primColorListName;
	std::string envColorListName;
	std::string frameDataListName;

	Globals::Instance->GetSegmentedPtrName(primColorListAddress, parent, "", primColorListName);
	Globals::Instance->GetSegmentedPtrName(envColorListAddress, parent, "", envColorListName);
	Globals::Instance->GetSegmentedPtrName(frameDataListAddress, parent, "", frameDataListName);

	std::string bodyStr = StringHelper::Sprintf(
		"\n    %d, %d, %s, %s, %s,\n", animLength, colorListCount, primColorListName.c_str(),
		envColorListName.c_str(), frameDataListName.c_str());

	return bodyStr;
}

/* TextureCyclingParams */

void TextureCyclingParams::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	cycleLength = BitConverter::ToUInt16BE(rawData, rawDataIndex);
	if (cycleLength == 0)
		HANDLE_ERROR_RESOURCE(WarningType::Always, parent, this, rawDataIndex,
		                      "cycle length cannot be 0", "");

	textureListAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);
	textureIndexListAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);

	uint32_t textureListOffset = Seg2Filespace(textureListAddress, parent->baseAddress);
	uint32_t textureIndexListOffset = Seg2Filespace(textureIndexListAddress, parent->baseAddress);

	uint32_t currentPtr;

	uint8_t currentIndex;
	uint8_t maxIndex = 0;  // To find the length of the texture list

	for (currentPtr = textureIndexListOffset; currentPtr < textureIndexListOffset + cycleLength;
	     currentPtr++)
	{
		currentIndex = BitConverter::ToUInt8BE(rawData, currentPtr);
		textureIndexList.push_back(currentIndex);
		if (currentIndex > maxIndex)
			maxIndex = currentIndex;
	}

	for (currentPtr = textureListOffset; currentPtr <= textureListOffset + 4 * maxIndex;
	     currentPtr += 4)
	{
		textureList.push_back(BitConverter::ToUInt32BE(rawData, currentPtr));
	}
}

std::string TextureCyclingParams::GetSourceTypeName() const
{
	return "AnimatedMatTexCycleParams";  // TODO: Better name
}

std::string TextureCyclingParams::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sTexCycleParams_%06X", prefix.c_str(), rawDataIndex);
}

size_t TextureCyclingParams::GetRawDataSize() const
{
	return 0xC;
}

void TextureCyclingParams::DeclareReferences([[maybe_unused]] const std::string& prefix)
{
	if (textureListAddress != 0)  // NULL
	{
		std::string texturesBodyStr;
		std::string texName;
		std::string comment;

		for (const auto& tex : textureList)
		{
			bool texFound = Globals::Instance->GetSegmentedPtrName(tex, parent, "", texName);

			// texName is a raw segmented pointer. This occurs if the texture is not declared
			// separately since we cannot read the format. In theory we could scan DLists for the
			// format on the appropriate segments.
			if (!texFound)
			{
				comment = " // Raw pointer, declare texture in XML to use proper symbol";

				auto msgHeader = StringHelper::Sprintf(
					"TexCycle texture array declared here points to unknown texture at address %s",
					texName.c_str());
				HANDLE_WARNING_RESOURCE(
					WarningType::HardcodedPointer, parent, this, rawDataIndex, msgHeader,
					"Please declare the texture in the XML to use the proper symbol.");
			}
			texturesBodyStr += StringHelper::Sprintf("\t%s,%s\n", texName.c_str(), comment.c_str());
		}

		texturesBodyStr.pop_back();

		parent->AddDeclarationArray(
			Seg2Filespace(textureListAddress, parent->baseAddress), DeclarationAlignment::Align4,
			textureList.size() * 4, "TexturePtr",
			StringHelper::Sprintf("%sTexCycleTexPtrs_%06X", parent->GetName().c_str(),
		                          Seg2Filespace(textureListAddress, parent->baseAddress)),
			textureList.size(), texturesBodyStr);
	}

	if (textureIndexListAddress != 0)  // NULL
	{
		std::string indicesBodyStr = "\t";

		for (uint8_t index : textureIndexList)
		{
			indicesBodyStr += StringHelper::Sprintf("%d, ", index);
		}

		indicesBodyStr.pop_back();

		parent->AddDeclarationArray(
			Seg2Filespace(textureIndexListAddress, parent->baseAddress),
			DeclarationAlignment::Align4, textureIndexList.size(), "u8",
			StringHelper::Sprintf("%sTexCycleTexIndices_%06X", parent->GetName().c_str(),
		                          Seg2Filespace(textureIndexListAddress, parent->baseAddress)),
			textureIndexList.size(), indicesBodyStr);
	}
}

std::string TextureCyclingParams::GetBodySourceCode() const
{
	std::string textureListName;
	std::string textureIndexListName;

	Globals::Instance->GetSegmentedPtrName(textureListAddress, parent, "", textureListName);
	Globals::Instance->GetSegmentedPtrName(textureIndexListAddress, parent, "",
	                                       textureIndexListName);

	std::string bodyStr = StringHelper::Sprintf(
		"\n    %d, %s, %s,\n", cycleLength, textureListName.c_str(), textureIndexListName.c_str());

	return bodyStr;
}

/* ZTextureAnimation */

ZTextureAnimation::ZTextureAnimation(ZFile* nParent) : ZResource(nParent)
{
}

/**
 * Builds the array of params
 */
void ZTextureAnimation::ParseRawData()
{
	ZResource::ParseRawData();

	TextureAnimationEntry currentEntry;
	auto rawData = parent->GetRawData();
	int16_t type;

	for (uint32_t currentPtr = rawDataIndex;; currentPtr += 8)
	{
		type = BitConverter::ToInt16BE(rawData, currentPtr + 2);

		currentEntry.segment = BitConverter::ToInt8BE(rawData, currentPtr);
		currentEntry.type = (TextureAnimationParamsType)type;
		currentEntry.paramsPtr = BitConverter::ToUInt32BE(rawData, currentPtr + 4);
		entries.push_back(currentEntry);

		if ((type < 0) || (type > 6))
		{
			HANDLE_ERROR_RESOURCE(
				WarningType::Always, parent, this, rawDataIndex,
				StringHelper::Sprintf(
					"unknown TextureAnimationParams type 0x%02X in TextureAnimation", type),
				StringHelper::Sprintf(
					"Entry reads  { 0x%02X, 0x%02X, 0x%08X } , but type should be "
					"between 0x00 and 0x06 inclusive.",
					currentEntry.segment, type, currentEntry.paramsPtr));
		}

		if (currentEntry.segment <= 0)
			break;
	}
}

#include <optional>
/**
 * For each params entry,
 */
void ZTextureAnimation::DeclareReferences(const std::string& prefix)
{
	std::string varPrefix = name;
	if (varPrefix == "")
		varPrefix = prefix;

	ZResource::DeclareReferences(varPrefix);

	for (const auto& entry : entries)
	{
		if (entry.paramsPtr != 0 && GETSEGNUM(entry.paramsPtr) == parent->segment)
		{
			uint32_t paramsOffset = Seg2Filespace(entry.paramsPtr, parent->baseAddress);
			if (!parent->HasDeclaration(paramsOffset))
			{
				ZTextureAnimationParams* params;
				int count;
				switch (entry.type)
				{
				case TextureAnimationParamsType::SingleScroll:
					if (true)
					{
						count = 1;
						// The else now allows SingleScroll to fall through to params = ... without
						// touching the code in the else block
					}
					else
					{
						// The contents of this block can only be run by jumping into it with the
						// case label
						[[fallthrough]];
					case TextureAnimationParamsType::DualScroll:
						count = 2;
					}
					params = new TextureScrollingParams(parent);
					params->ExtractFromBinary(paramsOffset, count);
					break;

				case TextureAnimationParamsType::ColorChange:
				case TextureAnimationParamsType::ColorChangeLERP:
				case TextureAnimationParamsType::ColorChangeLagrange:
					params = new TextureColorChangingParams(parent);
					params->type = entry.type;
					params->ExtractFromBinary(paramsOffset);
					break;

				case TextureAnimationParamsType::TextureCycle:
					params = new TextureCyclingParams(parent);
					params->ExtractFromBinary(paramsOffset);
					break;

				case TextureAnimationParamsType::Empty:
					HANDLE_WARNING_RESOURCE(
						WarningType::InvalidExtractedData, parent, this, rawDataIndex,
						"TextureAnimationParams entry has empty type (6), but params pointer is "
						"not NULL",
						StringHelper::Sprintf("Params read { 0x%02X, 0x%02X, 0x%08X } .",
					                          entry.segment, (int)entry.type, entry.paramsPtr));
					return;
				default:
					// Because GCC is worried this could happen
					assert(entry.type < TextureAnimationParamsType::SingleScroll ||
					       entry.type > TextureAnimationParamsType::Empty);
					return;
				}

				params->SetName(params->GetDefaultName(varPrefix));
				params->DeclareVar(varPrefix, "");
				parent->AddResource(params);
			}
		}
	}
}

std::string ZTextureAnimation::GetSourceTypeName() const
{
	return "AnimatedMaterial";  // TODO: Better name
}

ZResourceType ZTextureAnimation::GetResourceType() const
{
	return ZResourceType::TextureAnimation;
}

/**
 * The size of the main array
 */
size_t ZTextureAnimation::GetRawDataSize() const
{
	return entries.size() * 8;
}

std::string ZTextureAnimation::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sTexAnim_%06X", prefix.c_str(), rawDataIndex);
}

Declaration* ZTextureAnimation::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	Declaration* decl =
		parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                                GetSourceTypeName(), auxName, entries.size(), bodyStr);
	decl->staticConf = staticConf;
	return decl;
}

std::string ZTextureAnimation::GetBodySourceCode() const
{
	std::string bodyStr;

	for (const auto& entry : entries)
	{
		std::string paramName;
		Globals::Instance->GetSegmentedPtrName(entry.paramsPtr, parent, "", paramName);

		bodyStr += StringHelper::Sprintf("\t{ %d, %d, %s },\n", entry.segment, entry.type,
		                                 paramName.c_str());
	}

	bodyStr.pop_back();

	return bodyStr;
}
