#include "ArrayFactory.h"

namespace Ship
{
    Array* ArrayFactory::ReadArray(BinaryReader* reader, bool readFullHeader)
    {
        Array* arr = new Array();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Roy:
        {
            ArrayV1 arrayObj = ArrayV1();
            arrayObj.ParseFileBinary(reader, arr, readFullHeader);
        }
        break;
        case Version::Deckard:
        {
            ArrayV0 arrayObj = ArrayV0();
            arrayObj.ParseFileBinary(reader, arr, readFullHeader);
        }
        break;
       
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return arr;
    }
};