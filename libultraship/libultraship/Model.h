#pragma once

#include <vector>
#include <string>
#include "Resource.h"
#include "Vec2f.h"
#include "Vec3f.h"
#include "Vec3s.h"
#include "Color3b.h"

namespace Ship
{
    enum class ModelType
    {
        Normal = 0,
        Flex = 1,
        Skinned = 2,
    };

    class VertexData
    {
        DataType dataType;
        uint8_t dimensionCnt;
        uint32_t entryCnt;
        uint32_t offsetToData;
    };

    class ModelV0 : public ResourceFile
    {
    public:
        // HEADER
        ModelType modelType;

        uint32_t numVerts;
        uint32_t numPolys;

        uint32_t vertices;
        uint32_t normals;
        uint32_t faces;
        uint32_t vertexColors;
        uint32_t uvCoords;
        uint32_t boneWeights;

        void ParseFileBinary(BinaryReader* reader, Resource* res) override;
    };

    struct ModelVertex
    {
        Vec3f pos;
        Vec3f normal;
        Color3b color;
        Vec2f uv;

        ModelVertex();
        ModelVertex(BinaryReader* reader);
    };

    class Model : public Resource
    {
    public:
        ModelType modelType;

        uint32_t numVerts;
        uint32_t numPolys;

        ModelVertex* vertices;
        Vec2f* boneWeights;
        uint32_t* indices;
    };
}