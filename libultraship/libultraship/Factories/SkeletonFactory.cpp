#include "SkeletonFactory.h"

namespace Ship
{
    Skeleton* SkeletonFactory::ReadSkeleton(BinaryReader* reader)
    {
        Skeleton* skel = new Skeleton();

        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            SkeletonV0 Skel = SkeletonV0();
            Skel.ParseFileBinary(reader, skel);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return skel;
    }
}