#pragma once

#include "Resource.h"
#include "SkeletonLimb.h"

namespace Ship
{
	enum class SkeletonType
	{
		Normal,
		Flex,
		Curve,
	};

	class SkeletonV0 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
	};

	class Skeleton : public Resource
	{
	public:
		SkeletonType type;
		LimbType limbType;
		int limbCount;
		int dListCount;
		LimbType limbTableType;
		std::vector<std::string> limbTable;
	};
}