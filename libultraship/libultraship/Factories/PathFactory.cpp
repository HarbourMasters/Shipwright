#include "PathFactory.h"

namespace Ship
{
    Path* PathFactory::ReadPath(BinaryReader* reader, bool readFullHeader)
    {
        Path* path = new Path();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            PathV0 Path;
            Path.ParseFileBinary(reader, path, readFullHeader);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return path;
    }
};