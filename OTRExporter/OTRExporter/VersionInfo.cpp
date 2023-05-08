#include "VersionInfo.h"
#include <libultraship/bridge.h>

std::map<LUS::ResourceType, uint32_t> resourceVersions;

void InitVersionInfo()
{
	resourceVersions = std::map<LUS::ResourceType, uint32_t> {
	{ LUS::ResourceType::SOH_Animation, 0 },
	{ LUS::ResourceType::Texture, 0 },
	{ LUS::ResourceType::SOH_PlayerAnimation, 0 },
	{ LUS::ResourceType::DisplayList, 0 },
	{ LUS::ResourceType::SOH_Room, 0 },
	{ LUS::ResourceType::SOH_CollisionHeader, 0 },
	{ LUS::ResourceType::SOH_Skeleton, 0 },
	{ LUS::ResourceType::SOH_SkeletonLimb, 0 },
	{ LUS::ResourceType::Matrix, 0 },
	{ LUS::ResourceType::SOH_Path, 0 },
	{ LUS::ResourceType::Vertex, 0 },
	{ LUS::ResourceType::SOH_Cutscene, 0 },
	{ LUS::ResourceType::Array, 0 },
	{ LUS::ResourceType::SOH_Text, 0 },
	{ LUS::ResourceType::Blob, 0 },
	};
}
