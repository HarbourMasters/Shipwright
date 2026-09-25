#pragma once

#include "SkinAnimatedLimbData.h"
#include <ship/resource/Resource.h>
#include "z64math.h"

namespace SOH {
enum class LimbType {
    Invalid,
    Standard,
    LOD,
    Skin,
    Curve,
    Legacy,
};

enum class ZLimbSkinType {
    SkinType_0,          // Segment = 0
    SkinType_4 = 4,      // Segment = segmented address // Struct_800A5E28
    SkinType_5 = 5,      // Segment = 0
    SkinType_DList = 11, // Segment = DList address
};

typedef struct {
    /* 0x00 */ Vec3s jointPos; // Root is position in model space, children are relative to parent
    /* 0x06 */ u8 child;
    /* 0x07 */ u8 sibling;
    /* 0x08 */ Gfx* dLists[2]; // Near and far
} LodLimb;                     // size = 0x10

typedef struct {
    /* 0x00 */ Vec3s jointPos; // Root is position in model space, children are relative to parent
    /* 0x06 */ u8 child;
    /* 0x07 */ u8 sibling;
    /* 0x08 */ Gfx* dList;
} StandardLimb; // size = 0xC

typedef struct {
    /* 0x0000 */ u8 firstChildIdx;
    /* 0x0001 */ u8 nextLimbIdx;
    /* 0x0004 */ Gfx* dList[2];
} SkelCurveLimb; // size = 0xC

typedef struct {
    /* 0x00 */ Vec3s jointPos; // Root is position in model space, children are relative to parent
    /* 0x06 */ u8 child;
    /* 0x07 */ u8 sibling;
    /* 0x08 */ s32 segmentType; // Type of data contained in segment
    /* 0x0C */ void* segment;   // Gfx* if segmentType is SKIN_LIMB_TYPE_NORMAL, SkinAnimatedLimbData* if segmentType is
                                // SKIN_LIMB_TYPE_ANIMATED, NULL otherwise
} SkinLimb;                     // size = 0x10

union SkeletonLimbData {
    LodLimb lodLimb;
    StandardLimb standardLimb;
    SkelCurveLimb skelCurveLimb;
    SkinLimb skinLimb;
};

class SkeletonLimb : public Ship::Resource<SkeletonLimbData> {
  public:
    using Resource::Resource;

    SkeletonLimb() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {
    }

    SkeletonLimbData* GetPointer();
    size_t GetPointerSize();

    LimbType limbType;
    SkeletonLimbData limbData;

    ZLimbSkinType skinSegmentType;
    uint16_t skinVtxCnt;
    SkinAnimatedLimbData skinAnimLimbData;

    std::string skinDataDList;
    std::string skinDList;
    std::string skinDList2;

    float legTransX, legTransY, legTransZ; // Vec3f
    uint16_t rotX, rotY, rotZ;             // Vec3s

    std::string childPtr, siblingPtr, dListPtr, dList2Ptr;

    int16_t transX, transY, transZ;
    uint8_t childIndex, siblingIndex;

    uint32_t skinLimbModifCount;
    std::vector<SkinLimbModif> skinLimbModifArray;

    std::vector<std::vector<SkinVertex>> skinLimbModifVertexArrays;
    std::vector<std::vector<SkinTransformation>> skinLimbModifTransformationArrays;
};
} // namespace SOH