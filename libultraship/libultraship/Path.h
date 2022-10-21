#pragma once

#include <vector>
#include <string>
#include "Resource.h"
#include "Vec2f.h"
#include "Vec3f.h"
#include "Vec3s.h"
#include "Color3b.h"

namespace Ship
{
	class PathV0 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
	};

	class Path : public Resource
	{
	public:
		std::vector<std::vector<Vec3s>> paths;
	};
}