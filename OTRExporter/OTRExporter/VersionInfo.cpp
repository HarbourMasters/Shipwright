#include "VersionInfo.h"
#include <Resource.h>

std::map<Ship::ResourceType, uint32_t> resourceVersions;

void InitVersionInfo()
{
	resourceVersions = std::map<Ship::ResourceType, uint32_t> {
	{ Ship::ResourceType::Animation, 0 },
	{ Ship::ResourceType::Model, 0 },
	{ Ship::ResourceType::Texture, 0 },
	{ Ship::ResourceType::Material, 0 },
	{ Ship::ResourceType::PlayerAnimation, 0 },
	{ Ship::ResourceType::DisplayList, 0 },
	{ Ship::ResourceType::Room, 0 },
	{ Ship::ResourceType::CollisionHeader, 0 },
	{ Ship::ResourceType::Skeleton, 0 },
	{ Ship::ResourceType::SkeletonLimb, 0 },
	{ Ship::ResourceType::Matrix, 0 },
	{ Ship::ResourceType::Path, 0 },
	{ Ship::ResourceType::Vertex, 0 },
	{ Ship::ResourceType::Cutscene, 0 },
	{ Ship::ResourceType::Array, 0 },
	{ Ship::ResourceType::Text, 0 },
	{ Ship::ResourceType::Blob, 0 },
	};
}
