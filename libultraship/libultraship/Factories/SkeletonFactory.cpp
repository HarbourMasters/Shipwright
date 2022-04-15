#include "SkeletonFactory.h"

namespace Ship
{
    Skeleton* SkeletonFactory::ReadSkeleton(BinaryReader* reader, bool readFullHeader)
    {
        Skeleton* skel = new Skeleton();

        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            SkeletonV0 Skel = SkeletonV0();
            Skel.ParseFileBinary(reader, skel, readFullHeader);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return skel;
    }
}