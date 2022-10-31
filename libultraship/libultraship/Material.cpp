#include "Material.h"

namespace Ship
{
    void MaterialV0::ParseFileBinary(BinaryReader* reader, Resource* res)
    {
        Material* mat = (Material*)res;

        ResourceFile::ParseFileBinary(reader, res);

        cmtH = (MaterialCmt)reader->ReadUByte();
        cmtV = (MaterialCmt)reader->ReadUByte();

        clrR = reader->ReadUByte();
        clrG = reader->ReadUByte();
        clrB = reader->ReadUByte();
        clrA = reader->ReadUByte();
        clrM = reader->ReadUByte();
        clrL = reader->ReadUByte();

        shaderID = reader->ReadUInt32();
        shaderParamsCnt = reader->ReadUInt32();
        offsetToShaderEntries = reader->ReadUInt32();

        mat->shaderID = shaderID;
        mat->shaderParams.reserve(shaderParamsCnt);

        for (uint32_t i = 0; i < shaderParamsCnt; i++)
            mat->shaderParams.push_back(new ShaderParam(reader));

        mat->cmtH = cmtH;
        mat->cmtV = cmtV;

        mat->clrR = clrR;
        mat->clrG = clrG;
        mat->clrB = clrB;
        mat->clrA = clrA;
        mat->clrM = clrM;
        mat->clrL = clrL;
    }

    ShaderParam::ShaderParam(BinaryReader* reader)
    {
        name = reader->ReadUInt32();
        dataType = (DataType)reader->ReadInt8();

        switch (dataType)
        {
        case DataType::U8:
            value = reader->ReadUByte();
            break;
        case DataType::S8:
            value = reader->ReadInt8();
            break;
        case DataType::U16:
            value = reader->ReadUInt16();
            break;
        case DataType::S16:
            value = reader->ReadInt16();
            break;
        case DataType::U32:
            value = reader->ReadUInt32();
            break;
        case DataType::S32:
            value = reader->ReadInt32();
            break;
        default:
            // TODO: ERROR GOES HERE
            break;
        }
    }
}