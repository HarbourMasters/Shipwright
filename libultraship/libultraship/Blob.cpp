#include "Blob.h"

namespace Ship
{
    void BlobV0::ParseFileBinary(BinaryReader* reader, Resource* res)
    {
        Blob* blob = (Blob*)res;

        ResourceFile::ParseFileBinary(reader, blob);

        uint32_t dataSize = reader->ReadUInt32();

        blob->data.reserve(dataSize);

        for (uint32_t i = 0; i < dataSize; i++)
            blob->data.push_back(reader->ReadUByte());
    }
}