#pragma once

#include <cstdint>
#include <vector>
#include <ship/resource/Resource.h>
#include <libultraship/libultra/types.h>
#include "z64math.h"

namespace SOH {
typedef struct {
    /* 0x0 */ u8 count; // Number of points in the path
    /* 0x1 */ u8 additionalPathIndex;
    /* 0x2 */ s16 customValue; // Path specific to help distinguish different paths
    /* 0x4 */ Vec3s* points;   // Segment Address to the array of points
} PathDataMM;                  // size = 0x8

class PathMM : public Ship::Resource<PathDataMM> {
  public:
    using Resource::Resource;

    PathMM() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {
    }

    PathDataMM* GetPointer();
    size_t GetPointerSize();

    uint32_t numPaths;
    std::vector<PathDataMM> pathData;
    std::vector<std::vector<Vec3s>> paths;
};

}; // namespace SOH
