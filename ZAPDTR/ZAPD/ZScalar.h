#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ZResource.h"
#include "tinyxml2.h"

enum class ZScalarType
{
	ZSCALAR_NONE,
	ZSCALAR_S8,
	ZSCALAR_U8,
	ZSCALAR_X8,
	ZSCALAR_S16,
	ZSCALAR_U16,
	ZSCALAR_X16,
	ZSCALAR_S32,
	ZSCALAR_U32,
	ZSCALAR_X32,
	ZSCALAR_S64,
	ZSCALAR_U64,
	ZSCALAR_X64,
	ZSCALAR_F32,
	ZSCALAR_F64
};

typedef union ZScalarData
{
	uint8_t u8;
	int8_t s8;
	uint16_t u16;
	int16_t s16;
	uint32_t u32;
	int32_t s32;
	uint64_t u64;
	int64_t s64;
	float f32;
	double f64;
} ZScalarData;

class ZScalar : public ZResource
{
	friend class ZVector;

public:
	ZScalarData scalarData;
	ZScalarType scalarType;

	ZScalar(ZFile* nParent);

	void ExtractFromBinary(uint32_t nRawDataIndex, ZScalarType nScalarType);

	void ParseRawData() override;
	void ParseXML(tinyxml2::XMLElement* reader) override;
	std::string GetBodySourceCode() const override;

	bool DoesSupportArray() const override;
	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
	DeclarationAlignment GetDeclarationAlignment() const override;

	static size_t MapTypeToSize(const ZScalarType scalarType);
	static ZScalarType MapOutputTypeToScalarType(const std::string& type);
	static std::string MapScalarTypeToOutputType(const ZScalarType scalarType);
};
