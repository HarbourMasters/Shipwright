#include "VersionInfo.h"
#include <libultraship/bridge.h>

std::map<Ship::ResourceType, uint32_t> resourceVersions;

void InitVersionInfo()
{
	resourceVersions = std::map<Ship::ResourceType, uint32_t> {
	{ Ship::ResourceType::SOH_Animation, 0 },
	{ Ship::ResourceType::Texture, 0 },
	{ Ship::ResourceType::SOH_PlayerAnimation, 0 },
	{ Ship::ResourceType::DisplayList, 0 },
	{ Ship::ResourceType::SOH_Room, 0 },
	{ Ship::ResourceType::SOH_CollisionHeader, 0 },
	{ Ship::ResourceType::SOH_Skeleton, 0 },
	{ Ship::ResourceType::SOH_SkeletonLimb, 0 },
	{ Ship::ResourceType::Matrix, 0 },
	{ Ship::ResourceType::SOH_Path, 0 },
	{ Ship::ResourceType::Vertex, 0 },
	{ Ship::ResourceType::SOH_Cutscene, 0 },
	{ Ship::ResourceType::Array, 0 },
	{ Ship::ResourceType::SOH_Text, 0 },
	{ Ship::ResourceType::Blob, 0 },
	};
}
