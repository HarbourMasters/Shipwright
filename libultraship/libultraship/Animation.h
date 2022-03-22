#pragma once

#include "Resource.h"

namespace Ship
{
	enum class AnimationType
	{
		Normal = 0,
		Link = 1,
		Curve = 2,
		Legacy = 3,
	};

	struct RotationIndex
	{
		uint16_t x, y, z;

		RotationIndex(uint16_t nX, uint16_t nY, uint16_t nZ) : x(nX), y(nY), z(nZ) {}
	};

	class TransformData
	{
	public:
		///* 0x0000 */ u16 unk_00; // appears to be flags
		uint16_t unk_00;
		///* 0x0002 */ s16 unk_02;
		int16_t unk_02;
		///* 0x0004 */ s16 unk_04;
		int16_t unk_04;
		///* 0x0006 */ s16 unk_06;
		int16_t unk_06;
		///* 0x0008 */ f32 unk_08;
		float unk_08;
	};

	class AnimationV0 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
	};

	class Animation : public Resource
	{
	public:
		AnimationType type;
		int16_t frameCount;
		
		// NORMAL
		std::vector<uint16_t> rotationValues;
		std::vector<RotationIndex> rotationIndices;
		int16_t limit = 0;

		// CURVE
		std::vector<uint8_t> refIndexArr;
		std::vector<TransformData> transformDataArr;
		std::vector<int16_t> copyValuesArr;

		// LINK
		uint32_t segPtr; // This is temp

	};
}