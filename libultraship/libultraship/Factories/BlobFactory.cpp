#include "BlobFactory.h"

namespace Ship
{
    Blob* BlobFactory::ReadBlob(BinaryReader* reader)
    {
        Blob* blob = new Blob();

        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            BlobV0 blobFac = BlobV0();
            blobFac.ParseFileBinary(reader, blob);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return blob;
    }
}