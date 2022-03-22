#include "CollisionHeaderFactory.h"

namespace Ship
{
    CollisionHeader* CollisionHeaderFactory::ReadCollisionHeader(BinaryReader* reader)
    {
        CollisionHeader* colHeader = new CollisionHeader();

        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            CollisionHeaderV0 col = CollisionHeaderV0();
            col.ParseFileBinary(reader, colHeader);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return colHeader;
    }
};