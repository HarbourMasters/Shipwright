#pragma once

#include <ship/resource/Resource.h>
#include "libultraship/libultra.h"
#include "z64math.h"

namespace SOH {

/**
 * Holds a compact version of a vertex used in the Skin system
 * It is used to initialise the Vtx used by an animated limb
 */
typedef struct {
    /* 0x00 */ u16 index;
    /* 0x02 */ s16 s; // s and t are texture coordinates (also known as u and v)
    /* 0x04 */ s16 t;
    /* 0x06 */ s8 normX;
    /* 0x07 */ s8 normY;
    /* 0x08 */ s8 normZ;
    /* 0x09 */ u8 alpha;
} SkinVertex; // size = 0xA

class SkinVert : public Ship::Resource<SkinVertex> {
  public:
    using Resource::Resource;

    SkinVert() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {
    }

    SkinVertex* GetPointer() override;
    size_t GetPointerSize() override;

    std::vector<SkinVertex> skinVertexList;
};

/**
 * Describes a position displacement and a scale to be applied to a limb at index `limbIndex`
 */
typedef struct {
    /* 0x00 */ u8 limbIndex;
    /* 0x02 */ s16 x;
    /* 0x04 */ s16 y;
    /* 0x06 */ s16 z;
    /* 0x08 */ u8 scale;
} SkinTransformation; // size = 0xA

class SkinTransform : public Ship::Resource<SkinTransformation> {
  public:
    using Resource::Resource;

    SkinTransform() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {
    }

    SkinTransformation* GetPointer() override;
    size_t GetPointerSize() override;

    std::vector<SkinTransformation> skinTransformationList;
};

typedef struct {
    /* 0x00 */ u16 vtxCount; // number of vertices in this modif entry
    /* 0x02 */ u16 transformCount;
    /* 0x04 */ u16 unk_4; // index of limbTransformations?
    /* 0x08 */ SkinVertex* skinVertices;
    /* 0x0C */ SkinTransformation* limbTransformations;
} SkinLimbModif; // size = 0x10

class SkinModif : public Ship::Resource<SkinLimbModif> {
  public:
    using Resource::Resource;

    SkinModif() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {
    }

    SkinLimbModif* GetPointer() override;
    size_t GetPointerSize() override;

    std::vector<SkinLimbModif> skinLimbModifArray;
    std::vector<std::vector<SkinVertex>> skinVerticesArrays;
    std::vector<std::vector<SkinTransformation>> skinTransformationsArrays;
};

typedef struct {
    /* 0x00 */ u16 totalVtxCount; // total vertex count for all modif entries
    /* 0x02 */ u16 limbModifCount;
    /* 0x04 */ SkinLimbModif* limbModifications;
    /* 0x08 */ Gfx* dlist;
} SkinAnimatedLimbData; // size = 0xC

class SkinAnimData : public Ship::Resource<SkinAnimatedLimbData> {
  public:
    using Resource::Resource;

    SkinAnimData() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {
    }

    SkinAnimatedLimbData* GetPointer() override;
    size_t GetPointerSize() override;

    SkinAnimatedLimbData data;

    uint16_t totalVtxCount;
    uint32_t limbModifCount;
    std::string dListPtr;
    std::vector<SkinLimbModif> skinLimbModifArray;
};
} // namespace SOH