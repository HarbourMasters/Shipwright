#include "Texture.h"

namespace Ship
{
    void TextureV0::ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader)
    {
        Texture* tex = (Texture*)res;

        ResourceFile::ParseFileBinary(reader, tex, readFullHeader);

        tex->texType = (TextureType)reader->ReadUInt32();
        tex->width = reader->ReadUInt32();
        tex->height = reader->ReadUInt32();

        uint32_t dataSize = reader->ReadUInt32();

        tex->imageDataSize = dataSize;
        tex->imageData = new uint8_t[dataSize];

        for (uint32_t i = 0; i < dataSize; i++)
            tex->imageData[i] = reader->ReadUByte();
    }
}