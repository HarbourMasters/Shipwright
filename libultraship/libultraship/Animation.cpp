#include "Animation.h"

void Ship::AnimationV0::ParseFileBinary(BinaryReader* reader, Resource* res)
{
	Animation* anim = (Animation*)res;

	ResourceFile::ParseFileBinary(reader, res);

	AnimationType animType = (AnimationType)reader->ReadUInt32();
	anim->type = animType;

	if (animType == AnimationType::Normal)
	{
		anim->frameCount = reader->ReadInt16();

		uint32_t rotValuesCnt = reader->ReadUInt32();
		anim->rotationValues.reserve(rotValuesCnt);
		for (uint32_t i = 0; i < rotValuesCnt; i++)
			anim->rotationValues.push_back(reader->ReadUInt16());


		uint32_t rotIndCnt = reader->ReadUInt32();
		anim->rotationIndices.reserve(rotIndCnt);
		for (size_t i = 0; i < rotIndCnt; i++)
		{
			uint16_t x = reader->ReadUInt16();
			uint16_t y = reader->ReadUInt16();
			uint16_t z = reader->ReadUInt16();
			anim->rotationIndices.push_back(RotationIndex(x, y, z));
		}
		anim->limit = reader->ReadInt16();
	}
	else if (animType == AnimationType::Curve)
	{
		anim->frameCount = reader->ReadInt16();

		uint32_t refArrCnt = reader->ReadUInt32();
		anim->refIndexArr.reserve(refArrCnt);
		for (uint32_t i = 0; i < refArrCnt; i++)
			anim->refIndexArr.push_back(reader->ReadUByte());

		uint32_t transformDataCnt = reader->ReadUInt32();
		anim->transformDataArr.reserve(transformDataCnt);
		for (uint32_t i = 0; i < transformDataCnt; i++)
		{
			TransformData data;
			data.unk_00 = reader->ReadUInt16();
			data.unk_02 = reader->ReadInt16();
			data.unk_04 = reader->ReadInt16();
			data.unk_06 = reader->ReadInt16();
			data.unk_08 = reader->ReadFloat();

			anim->transformDataArr.push_back(data);
		}

		uint32_t copyValuesCnt = reader->ReadUInt32();
		anim->copyValuesArr.reserve(copyValuesCnt);
		for (uint32_t i = 0; i < copyValuesCnt; i++)
		{
			anim->copyValuesArr.push_back(reader->ReadInt16());
		}
	}
	else if (animType == AnimationType::Link)
	{
		anim->frameCount = reader->ReadInt16();
		anim->segPtr = reader->ReadUInt32();
	}
	else if (animType == AnimationType::Legacy)
	{
		SPDLOG_DEBUG("BEYTAH ANIMATION?!");
	}
}
