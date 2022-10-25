#include "Model.h"

namespace Ship
{
    ModelVertex::ModelVertex()
    {
        pos = Vec3f(0, 0, 0);
        normal = Vec3f(0, 0, 0);
        color = Color3b(0, 0, 0);
        uv = Vec2f(0, 0);
    }

    ModelVertex::ModelVertex(BinaryReader* reader)
    {
        pos = reader->ReadVec3f();
        normal = reader->ReadVec3f();
        color = reader->ReadColor3b();
        uv = reader->ReadVec2f();
    }

    void ModelV0::ParseFileBinary(BinaryReader* reader, Resource* res)
    {
        Model* mdl = (Model*)res;

        uint32_t headerStart = reader->GetBaseAddress();

        modelType = (ModelType)reader->ReadInt8();

        numVerts = reader->ReadUInt32();
        numPolys = reader->ReadUInt32();

        vertices = reader->ReadUInt32();
        normals = reader->ReadUInt32();
        faces = reader->ReadUInt32();
        vertexColors = reader->ReadUInt32();
        uvCoords = reader->ReadUInt32();
        boneWeights = reader->ReadUInt32();

        ModelVertex* vtxData = new ModelVertex[numVerts];
        uint32_t* indicesData = new uint32_t[numPolys];

        if (vertices != 0)
        {
            reader->Seek(headerStart + vertices, SeekOffsetType::Start);

            for (uint32_t i = 0; i < numVerts; i++)
                vtxData[i].pos = reader->ReadVec3f();
        }

        if (normals != 0)
        {
            reader->Seek(headerStart + normals, SeekOffsetType::Start);

            for (uint32_t i = 0; i < numVerts; i++)
                vtxData[i].normal = reader->ReadVec3f();
        }

        if (vertexColors != 0)
        {
            reader->Seek(headerStart + vertexColors, SeekOffsetType::Start);

            for (uint32_t i = 0; i < numVerts; i++)
                vtxData[i].color = reader->ReadColor3b();
        }

        if (uvCoords != 0)
        {
            reader->Seek(headerStart + uvCoords, SeekOffsetType::Start);

            for (uint32_t i = 0; i < numVerts; i++)
                vtxData[i].uv = reader->ReadVec2f();
        }

        if (boneWeights != 0)
        {
            reader->Seek(headerStart + boneWeights, SeekOffsetType::Start);

            mdl->boneWeights = new Vec2f[numVerts];

            for (uint32_t i = 0; i < numVerts; i++)
                mdl->boneWeights[i] = reader->ReadVec2f();
        }

        if (faces != 0)
        {
            reader->Seek(headerStart + faces, SeekOffsetType::Start);
            reader->Read((char*)indicesData, numPolys * sizeof(uint32_t));
        }

        mdl->vertices = vtxData;
        mdl->indices = indicesData;
    }
}