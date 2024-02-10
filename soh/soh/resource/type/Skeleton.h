#pragma once

#include <cstdint>
#include "Resource.h"
#include "SkeletonLimb.h"
#include <z64animation.h>

namespace LUS {

enum class SkeletonType {
    Normal,
    Flex,
    Curve,
};

// typedef struct {
//     /* 0x00 */ Vec3s jointPos; // Root is position in model space, children are relative to parent
//     /* 0x06 */ u8 child;
//     /* 0x07 */ u8 sibling;
//     /* 0x08 */ Gfx* dList;
// } StandardLimb; // size = 0xC

// Model has limbs with only rigid meshes
typedef struct {
    /* 0x00 */ void** segment;
    /* 0x04 */ uint8_t limbCount;
               uint8_t skeletonType;
} SkeletonHeader; // size = 0x8

// Model has limbs with flexible meshes
typedef struct {
    /* 0x00 */ SkeletonHeader sh;
    /* 0x08 */ uint8_t dListCount;
} FlexSkeletonHeader; // size = 0xC

// typedef struct {
//     /* 0x0000 */ u8 firstChildIdx;
//     /* 0x0001 */ u8 nextLimbIdx;
//     /* 0x0004 */ Gfx* dList[2];
// } SkelCurveLimb; // size = 0xC

typedef struct {
    /* 0x0000 */ SkelCurveLimb** limbs;
    /* 0x0004 */ uint8_t limbCount;
} SkelCurveLimbList; // size = 0x8

union SkeletonData {
    SkeletonHeader skeletonHeader;
    FlexSkeletonHeader flexSkeletonHeader;
    SkelCurveLimbList skelCurveLimbList;
};

class Skeleton : public Resource<SkeletonData> {
  public:
    using Resource::Resource;

    Skeleton() : Resource(std::shared_ptr<ResourceInitData>()) {}

    SkeletonData* GetPointer();
    size_t GetPointerSize();

    SkeletonType type;
    SkeletonData skeletonData;
    
    LimbType limbType;
    int limbCount;
    int dListCount;
    LimbType limbTableType;
    int limbTableCount;
    std::vector<StandardLimb> standardLimbArray;
    std::vector<SkelCurveLimb> curveLimbArray;
    std::vector<std::string> limbTable;
    std::vector<void*> skeletonHeaderSegments;
};

// TODO: CLEAN THIS UP LATER
struct SkeletonPatchInfo 
{
    SkelAnime* skelAnime;
    std::string vanillaSkeletonPath;
};

class SkeletonPatcher {
  public:
    static void RegisterSkeleton(std::string& path, SkelAnime* skelAnime);
    static void UnregisterSkeleton(SkelAnime* skelAnime);
    static void ClearSkeletons();
    static void UpdateSkeletons();

    static std::vector<SkeletonPatchInfo> skeletons;
};


} // namespace LUS