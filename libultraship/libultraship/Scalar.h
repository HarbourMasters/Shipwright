#pragma once

#include "Resource.h"
#include "Vertex.h"

namespace Ship
{
	typedef union ScalarData
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
	} ScalarData;

	enum class ScalarType
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

	class ScalarV2 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader) override;
	};

	class Scalar : public Resource
	{
	public:
		ScalarData scalar;
	};
}