#pragma once

#include <cstdint>
#include <vector>
#include "Resource.h"
#include <libultraship/libultra/types.h>
#include "z64math.h"

namespace SOH {

typedef struct {
    /* 0x00 */ u8 count; // number of points in the path
    /* 0x04 */ Vec3s* points; // Segment Address to the array of points
} PathData; // size = 0x8

class Path : public Ship::Resource<PathData> {
public:
  using Resource::Resource;

    Path() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {}

    PathData* GetPointer();
    size_t GetPointerSize();

    uint32_t numPaths;
    std::vector<PathData> pathData;
    std::vector<std::vector<Vec3s>> paths;
};
}; // namespace LUS
