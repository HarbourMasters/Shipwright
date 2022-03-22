#pragma once

#include <vector>
#include <string>
#include "Resource.h"
#include "Vec2f.h"
#include "Vec3f.h"
#include "Color3b.h"

namespace Ship
{
	class PlayerAnimationV0 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
	};

    class PlayerAnimation : public Resource
    {
    public:
		std::vector<int16_t> limbRotData;
    };
}