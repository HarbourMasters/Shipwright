#include "CollisionHeaderFactory.h"

namespace Ship
{
    CollisionHeader* CollisionHeaderFactory::ReadCollisionHeader(BinaryReader* reader, bool readFullHeader)
    {
        CollisionHeader* colHeader = new CollisionHeader();

        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            CollisionHeaderV0 col = CollisionHeaderV0();
            col.ParseFileBinary(reader, colHeader, readFullHeader);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return colHeader;
    }
};