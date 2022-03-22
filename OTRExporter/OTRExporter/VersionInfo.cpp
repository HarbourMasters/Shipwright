#include "VersionInfo.h"
#include <Resource.h>

std::map<Ship::ResourceType, uint32_t> resourceVersions;

void InitVersionInfo()
{
	resourceVersions[Ship::ResourceType::Animation] = 0;
	resourceVersions[Ship::ResourceType::Model] = 0;
	resourceVersions[Ship::ResourceType::Texture] = 0;
	resourceVersions[Ship::ResourceType::Material] = 0;
	resourceVersions[Ship::ResourceType::PlayerAnimation] = 0;
	resourceVersions[Ship::ResourceType::DisplayList] = 0;
	resourceVersions[Ship::ResourceType::Room] = 0;
	resourceVersions[Ship::ResourceType::CollisionHeader] = 0;
	resourceVersions[Ship::ResourceType::Skeleton] = 0;
	resourceVersions[Ship::ResourceType::SkeletonLimb] = 0;
	resourceVersions[Ship::ResourceType::Matrix] = 0;
	resourceVersions[Ship::ResourceType::Path] = 0;
	resourceVersions[Ship::ResourceType::Vertex] = 0;
	resourceVersions[Ship::ResourceType::Cutscene] = 0;
	resourceVersions[Ship::ResourceType::Array] = 0;
	resourceVersions[Ship::ResourceType::Text] = 0;
	resourceVersions[Ship::ResourceType::Blob] = 0;
}