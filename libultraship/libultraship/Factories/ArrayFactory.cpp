#include "ArrayFactory.h"

namespace Ship
{
    Array* ArrayFactory::ReadArray(BinaryReader* reader)
    {
        Array* arr = new Array();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            ArrayV0 arrayObj = ArrayV0();
            arrayObj.ParseFileBinary(reader, arr);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return arr;
    }
};