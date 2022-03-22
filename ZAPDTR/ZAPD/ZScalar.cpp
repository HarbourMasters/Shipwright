#include "ZScalar.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/File.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Scalar, ZScalar);

ZScalar::ZScalar(ZFile* nParent) : ZResource(nParent)
{
	memset(&scalarData, 0, sizeof(ZScalarData));
	scalarType = ZScalarType::ZSCALAR_NONE;
	RegisterRequiredAttribute("Type");
}

void ZScalar::ExtractFromBinary(uint32_t nRawDataIndex, ZScalarType nScalarType)
{
	rawDataIndex = nRawDataIndex;
	scalarType = nScalarType;

	// Don't parse raw data of external files
	if (parent->GetMode() == ZFileMode::ExternalFile)
		return;

	ParseRawData();
}

void ZScalar::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	scalarType = ZScalar::MapOutputTypeToScalarType(registeredAttributes.at("Type").value);
}

ZScalarType ZScalar::MapOutputTypeToScalarType(const std::string& type)
{
	if (type == "s8")
	{
		return ZScalarType::ZSCALAR_S8;
	}
	else if (type == "u8")
	{
		return ZScalarType::ZSCALAR_U8;
	}
	else if (type == "x8")
	{
		return ZScalarType::ZSCALAR_X8;
	}
	else if (type == "s16")
	{
		return ZScalarType::ZSCALAR_S16;
	}
	else if (type == "u16")
	{
		return ZScalarType::ZSCALAR_U16;
	}
	else if (type == "x16")
	{
		return ZScalarType::ZSCALAR_X16;
	}
	else if (type == "s32")
	{
		return ZScalarType::ZSCALAR_S32;
	}
	else if (type == "u32")
	{
		return ZScalarType::ZSCALAR_U32;
	}
	else if (type == "x32")
	{
		return ZScalarType::ZSCALAR_X32;
	}
	else if (type == "s64")
	{
		return ZScalarType::ZSCALAR_S64;
	}
	else if (type == "u64")
	{
		return ZScalarType::ZSCALAR_U64;
	}
	else if (type == "x64")
	{
		return ZScalarType::ZSCALAR_X64;
	}
	else if (type == "f32")
	{
		return ZScalarType::ZSCALAR_F32;
	}
	else if (type == "f64")
	{
		return ZScalarType::ZSCALAR_F64;
	}

	return ZScalarType::ZSCALAR_NONE;
}

std::string ZScalar::MapScalarTypeToOutputType(const ZScalarType scalarType)
{
	switch (scalarType)
	{
	case ZScalarType::ZSCALAR_S8:
		return "s8";
	case ZScalarType::ZSCALAR_U8:
	case ZScalarType::ZSCALAR_X8:
		return "u8";
	case ZScalarType::ZSCALAR_S16:
		return "s16";
	case ZScalarType::ZSCALAR_U16:
	case ZScalarType::ZSCALAR_X16:
		return "u16";
	case ZScalarType::ZSCALAR_S32:
		return "s32";
	case ZScalarType::ZSCALAR_U32:
	case ZScalarType::ZSCALAR_X32:
		return "u32";
	case ZScalarType::ZSCALAR_S64:
		return "s64";
	case ZScalarType::ZSCALAR_U64:
	case ZScalarType::ZSCALAR_X64:
		return "u64";
	case ZScalarType::ZSCALAR_F32:
		return "f32";
	case ZScalarType::ZSCALAR_F64:
		return "f64";
	default:
		return "";
	}
}

size_t ZScalar::MapTypeToSize(const ZScalarType scalarType)
{
	switch (scalarType)
	{
	case ZScalarType::ZSCALAR_S8:
		return sizeof(scalarData.s8);
	case ZScalarType::ZSCALAR_U8:
	case ZScalarType::ZSCALAR_X8:
		return sizeof(scalarData.u8);
	case ZScalarType::ZSCALAR_S16:
		return sizeof(scalarData.s16);
	case ZScalarType::ZSCALAR_U16:
	case ZScalarType::ZSCALAR_X16:
		return sizeof(scalarData.u16);
	case ZScalarType::ZSCALAR_S32:
		return sizeof(scalarData.s32);
	case ZScalarType::ZSCALAR_U32:
	case ZScalarType::ZSCALAR_X32:
		return sizeof(scalarData.u32);
	case ZScalarType::ZSCALAR_S64:
		return sizeof(scalarData.s64);
	case ZScalarType::ZSCALAR_U64:
	case ZScalarType::ZSCALAR_X64:
		return sizeof(scalarData.u64);
	case ZScalarType::ZSCALAR_F32:
		return sizeof(scalarData.f32);
	case ZScalarType::ZSCALAR_F64:
		return sizeof(scalarData.f64);
	default:
		return 0;
	}
}

size_t ZScalar::GetRawDataSize() const
{
	return ZScalar::MapTypeToSize(scalarType);
}

void ZScalar::ParseRawData()
{
	const auto& rawData = parent->GetRawData();
	switch (scalarType)
	{
	case ZScalarType::ZSCALAR_S8:
		scalarData.s8 = BitConverter::ToInt8BE(rawData, rawDataIndex);
		break;
	case ZScalarType::ZSCALAR_U8:
	case ZScalarType::ZSCALAR_X8:
		scalarData.u8 = BitConverter::ToUInt8BE(rawData, rawDataIndex);
		break;
	case ZScalarType::ZSCALAR_S16:
		scalarData.s16 = BitConverter::ToInt16BE(rawData, rawDataIndex);
		break;
	case ZScalarType::ZSCALAR_U16:
	case ZScalarType::ZSCALAR_X16:
		scalarData.u16 = BitConverter::ToUInt16BE(rawData, rawDataIndex);
		break;
	case ZScalarType::ZSCALAR_S32:
		scalarData.s32 = BitConverter::ToInt32BE(rawData, rawDataIndex);
		break;
	case ZScalarType::ZSCALAR_U32:
	case ZScalarType::ZSCALAR_X32:
		scalarData.u32 = BitConverter::ToUInt32BE(rawData, rawDataIndex);
		break;
	case ZScalarType::ZSCALAR_S64:
		scalarData.s64 = BitConverter::ToInt64BE(rawData, rawDataIndex);
		break;
	case ZScalarType::ZSCALAR_U64:
	case ZScalarType::ZSCALAR_X64:
		scalarData.u64 = BitConverter::ToUInt64BE(rawData, rawDataIndex);
		break;
	case ZScalarType::ZSCALAR_F32:
		scalarData.f32 = BitConverter::ToFloatBE(rawData, rawDataIndex);
		break;
	case ZScalarType::ZSCALAR_F64:
		scalarData.f64 = BitConverter::ToDoubleBE(rawData, rawDataIndex);
		break;
	case ZScalarType::ZSCALAR_NONE:
		HANDLE_ERROR_RESOURCE(WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
		                      "invalid value found for 'Type' attribute", "Defaulting to ''");
		break;
	}
}

std::string ZScalar::GetSourceTypeName() const
{
	return ZScalar::MapScalarTypeToOutputType(scalarType);
}

std::string ZScalar::GetBodySourceCode() const
{
	switch (scalarType)
	{
	case ZScalarType::ZSCALAR_S8:
		return StringHelper::Sprintf("%hhd", scalarData.s8);
	case ZScalarType::ZSCALAR_U8:
		return StringHelper::Sprintf("%hhu", scalarData.u8);
	case ZScalarType::ZSCALAR_X8:
		return StringHelper::Sprintf("0x%02X", scalarData.u8);
	case ZScalarType::ZSCALAR_S16:
		return StringHelper::Sprintf("%hd", scalarData.s16);
	case ZScalarType::ZSCALAR_U16:
		return StringHelper::Sprintf("%hu", scalarData.u16);
	case ZScalarType::ZSCALAR_X16:
		return StringHelper::Sprintf("0x%04X", scalarData.u16);
	case ZScalarType::ZSCALAR_S32:
		return StringHelper::Sprintf("%d", scalarData.s32);
	case ZScalarType::ZSCALAR_U32:
		return StringHelper::Sprintf("%u", scalarData.u32);
	case ZScalarType::ZSCALAR_X32:
		return StringHelper::Sprintf("0x%08X", scalarData.u32);
	case ZScalarType::ZSCALAR_S64:
		return StringHelper::Sprintf("%lld", scalarData.s64);
	case ZScalarType::ZSCALAR_U64:
		return StringHelper::Sprintf("%llu", scalarData.u64);
	case ZScalarType::ZSCALAR_X64:
		return StringHelper::Sprintf("0x%016X", scalarData.u64);
	case ZScalarType::ZSCALAR_F32:
		return StringHelper::Sprintf("%f", scalarData.f32);
	case ZScalarType::ZSCALAR_F64:
		return StringHelper::Sprintf("%lf", scalarData.f64);
	default:
		return "SCALAR_ERROR";
	}
}

ZResourceType ZScalar::GetResourceType() const
{
	return ZResourceType::Scalar;
}

bool ZScalar::DoesSupportArray() const
{
	return true;
}

DeclarationAlignment ZScalar::GetDeclarationAlignment() const
{
	switch (scalarType)
	{
	case ZScalarType::ZSCALAR_S64:
	case ZScalarType::ZSCALAR_U64:
	case ZScalarType::ZSCALAR_F64:
		return DeclarationAlignment::Align8;
	default:
		return DeclarationAlignment::Align4;
	}
}
